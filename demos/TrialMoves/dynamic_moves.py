from tps import *
import tpsgraphics
from math import sqrt
from math import fabs
import random
import os
import telnetlib

pid = os.fork()
if pid == 0:
	#run vmd on this thread:
	print "I'm the child, and my pid is %d!" % (os.getpid())
	os.system("vmd -e remote_ctl.tcl")
	os._exit(2)
else:
	print "I'm the parent, and I just forked pid %d!" % (pid)
	print "waiting to connect with vmd..."
	os.system("sleep 5")
	tn = telnetlib.Telnet("localhost",5555)
	rng = TpsRNG48()
	writer = TpsAnalysisWriteTrajectory("trajectory%.xyz", 5)
	
	class CustomSimulation(TpsTestRandomWalk):
		"a specialized order parameter"
		def __init__(self, rng):
			TpsTestRandomWalk.__init__(self, rng)
			self.setParticlePosition(0.0, 0.0)
			self.npath = 0
		
		def setPathEnsemble(self, tpe):
			self.tpe = tpe
		
		def callbackOnRunCommand(self):
			if not self.show:
				return

			self.writeRestart("temp.txt")
			
			trj = self.tpe.getTrajectory(-1)
			nt = trj.getNumberOfTimeslices()
			
			tn.write("graphics 1 delete all\n")
			tn.write("graphics 1 color white\n")
			self.reset_path_line = 0
			
			for i in range (0, nt-1):
				self.readRestart(trj.getTimeslice(i).getFilename())
				ts = self.getCurrentTimeslice()
				x = ts.getPositionX()
				y = ts.getPositionY()

				if i == 0:
					self.xo = x
					self.yo = y

				tn.write("graphics 1 line {" + str(self.xo) + " " + str(self.yo) + " "  + str(0) + "} " + \
						"{" + str(x) + " " + str(y) + " "  + str(0) + "} width 2 style dashed\n")

				self.xo = x
				self.yo = y
			
			if nt > 0:
				self.readRestart(trj.getTimeslice(0).getFilename())
				ts = self.getCurrentTimeslice()
				x = ts.getPositionX()
				y = ts.getPositionY()
			
				tn.write("graphics 0 replace 0\n")
				tn.write("graphics 0 sphere {" + str(x) + " " + str(y) + " "  + str(0) + "} radius 0.2\n")

				self.readRestart(trj.getTimeslice(nt-1).getFilename())
				ts = self.getCurrentTimeslice()
				x = ts.getPositionX()
				y = ts.getPositionY()

				tn.write("graphics 0 replace 1\n")
				tn.write("graphics 0 color 1\n")
				tn.write("graphics 0 replace 2\n")
				tn.write("graphics 0 sphere {" + str(x) + " " + str(y) + " "  + str(0) + "} radius 0.2\n")
	
			self.readRestart("temp.txt")
			os.system("sleep 0.01")
		
		def writeData(self, filename, position):
			#print filename, position
			
			if position == -1:
				#we are starting the trajectory
				tn.write("graphics 2 delete all\n")
				tn.write("graphics 2 color red\n")
				self.reset = 1
				
			elif position == 0:
				#we are continuing the trajectory			
				ts = self.getCurrentTimeslice()
				x = ts.getPositionX()
				y = ts.getPositionY()
				
				if self.reset == 1:
					self.xo = x
					self.yo = y
					self.reset = 0
					
				tn.write("graphics 2 line {" + str(self.xo) + " " + str(self.yo) + " "  + str(0) + "} " + \
							"{" + str(x) + " " + str(y) + " "  + str(0) + "}\n")

				self.xo = x
				self.yo = y
			
			elif position == 1:
				pass
				#we are ending the trajectory

			
	class CustomPathSampling(TpsAlgorithmTPS):
		"a specialized TPS algorithm where callbacks have been added"
		def __init__(self):
			#do everything in the constructor.
			#this way, when we instantiate the object below, the whole
			#simulation will automatically run
			
			print "Initializing simulation"
			self.sim = CustomSimulation(rng)
			tn.write("mol new\n")			
			tn.write("axes location off\n")
			tn.write("display projection orthographic\n")
			tn.write("color scale method BGR\n")
			tn.write("graphics 0 sphere {0 0 -5} radius 0.2\n")
			tn.write("graphics 0 color 2\n")
			tn.write("graphics 0 sphere {0 0 -5} radius 0.2\n")
			tn.write("mol new\n")
			tn.write("mol new\n")
						
			print "Done Initializing simulation"
			
			q = TpsTestRandomWalkOrderParam();
			q.setDistanceCutoff(3.0)
			self.sim.setStepSize(0.05)
			
			init = TpsInitializerBruteForce()
			trj = TpsTrajectoryRandomStep(self.sim, rng, 500, 5)
			self.tpe = TpsTrajectoryEnsemble(trj)
			self.sim.setPathEnsemble(self.tpe)
			
			print "Initializing"
			self.sim.show = 0
			TpsAlgorithmTPS.__init__(self, self.tpe, rng, q, init)

			#dv = TpsTimesliceModifierPerturbVelocities(0.0001, 1.0, 1, 1)
			shift = TpsTrialMoveShift(5, 200)
			bshot = TpsTrialMoveShotBackward()
			fshot = TpsTrialMoveShotForward()
			shot = TpsTrialMoveShot()
			vshot = TpsTrialMoveShotFlexible(500)
			vshot2 = TpsTrialMoveShotFlexibleV2(500)
			ashot = TpsTrialMoveShotAimless(5)

			self.addTrialMove(vshot2)			
			#self.addTrialMove(fshot)
			#self.addTrialMove(bshot)
			#self.addTrialMove(shift)
			
			self.sim.show = 0
			self.nstep = 0
			print "starting path sampling:"
			for i in range(1, 1000):
				print i
				self.doStep()

			var = raw_input("Return/Enter to EXIT ")
			tn.write("exit\n")
			exit(1)
		
		def callbackOnTrialMove(self):
			if self.nstep > 5:
				tn.write("display resetview\n")
				tn.write("scale by 0.7\n")
				self.sim.show = 1
				self.sim.reset_path_line = 1
				var = raw_input("Return/Enter to do a trial move: ")
				if self.nstep == 6:
					self.callbackOnTrialMoveAccepted()
			self.nstep = self.nstep + 1
			
		def callbackOnTrialMoveAccepted(self):
			print "accepted\n"
			print "new path length", self.tpe.getLastTrajectory().getNumberOfTimeslices()
			if self.nstep > 5:
				self.sim.show = 0					
				writer.analyze(self.tpe.getLastTrajectory())
			
			
			
	CustomPathSampling()
		
		