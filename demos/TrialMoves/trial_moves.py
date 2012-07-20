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
	tmp_sim = TpsTestPeSurf2dBD(rng)
	writer = TpsAnalysisWriteTrajectory("trajectory%.xyz", 5)
	working_traj = TpsTrajectoryUniformStep(tmp_sim, 1000, 5)
	zmax = 2
	
	class CustomSimulation(TpsTestPeSurf2dMC):
		"a specialized order parameter"
		def __init__(self, rng):
			TpsTestPeSurf2dMC.__init__(self, rng)
			pef = self.getPotentialField(-3., 3.0, -1.5, 3.5, 0.150)			
			potential = []
			for i in range(0, len(pef)):
				potential.append(pef[i][2])
			maxp = max(potential)
			self.minp = min(potential)
			self.rangep = maxp-self.minp			
			self.show = 1
			self.npath = 0
			self.reset_path_line = 0
		
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
				z = zmax*(self.computePotential(x, y) - self.minp)/self.rangep

				if i == 0:
					self.xo = x
					self.yo = y
					self.zo = z

				tn.write("graphics 1 line {" + str(self.xo) + " " + str(self.yo) + " "  + str(self.zo+0.10) + "} " + \
						"{" + str(x) + " " + str(y) + " "  + str(z+0.10) + "} width 2 style dashed\n")

				self.xo = x
				self.yo = y
				self.zo = z
			
			if nt > 0:
				self.readRestart(trj.getTimeslice(0).getFilename())
				ts = self.getCurrentTimeslice()
				x = ts.getPositionX()
				y = ts.getPositionY()
				z = zmax*(self.computePotential(x, y) - self.minp)/self.rangep
			
				tn.write("graphics 0 replace 0\n")
				tn.write("graphics 0 sphere {" + str(x) + " " + str(y) + " "  + str(z+0.150) + "} radius 0.05\n")

				self.readRestart(trj.getTimeslice(nt-1).getFilename())
				ts = self.getCurrentTimeslice()
				x = ts.getPositionX()
				y = ts.getPositionY()
				z = zmax*(self.computePotential(x, y) - self.minp)/self.rangep

				tn.write("graphics 0 replace 1\n")
				tn.write("graphics 0 color 1\n")
				tn.write("graphics 0 replace 2\n")
				tn.write("graphics 0 sphere {" + str(x) + " " + str(y) + " "  + str(z+0.150) + "} radius 0.05\n")
	
			self.readRestart("temp.txt")
			os.system("sleep 0.01")
		
		def writeData(self, filename, position):
			#print filename, position
			
			if position == -1:
				#we are starting the trajectory
				self.last_file = filename
				f=open("movie_" + filename, 'w')
				f.close()
				tn.write("mol new\n")
				tn.write("mol rename path_" + filename + "\n")
				tn.write("graphics top color " + str(random.randint(0, 32)) + "\n")
				self.xo = 0
				self.yo = 0
				self.zo = 0
				
			elif position == 0:
				#we are continuing the trajectory			
				ts = self.getCurrentTimeslice()
				x = ts.getPositionX()
				y = ts.getPositionY()
				z = zmax*(self.computePotential(x, y) - self.minp)/self.rangep
				
				self.last_file = filename
				f=open("movie_" + filename, 'a')
				print >> f,  "1\n"
				print >> f,  "C " + str(x) + " " + str(y) + " "  + str(z+0.150)
				f.close()

				tn.write("graphics top line {" + str(self.xo) + " " + str(self.yo) + " "  + str(self.zo+0.10) + "} " + \
							"{" + str(x) + " " + str(y) + " "  + str(z+0.10) + "}\n")

				self.xo = x
				self.yo = y
				self.zo = z
			
			elif position == 1:
				#we are ending the trajectory
				molnum = 2 + 2*self.npath
				self.npath = self.npath + 1
				print "MOLNUM", molnum
				if molnum > 2:
					tn.write("mol off " + str(molnum-2) + "\n")
					tn.write("mol off " + str(molnum-1) + "\n")
				
				tn.write("graphics 0 replace 0\n")
				tn.write("graphics 0 replace 0\n")
				tn.write("graphics 0 sphere {0 0 0} radius 0.0\n")
				tn.write("mol load xyz " + "movie_" + self.last_file + "\n")
				tn.write("mol modstyle 0 top VDW 0.05 8.000000\n")
				tn.write("scale by 0.65\n")
				tn.write("rotate x by -20\n")

			
	class CustomPathSampling(TpsAlgorithmTPS):
		"a specialized TPS algorithm where callbacks have been added"
		def __init__(self):
			#do everything in the constructor.
			#this way, when we instantiate the object below, the whole
			#simulation will automatically run
			
			print "Initializing simulation"
			self.sim = CustomSimulation(rng)
			self.sim.setTemperature(0.5)
			#self.sim.setDragCoefficient(2.5)
			#self.sim.setTimestep(0.05)
			tpsgraphics.pefield(self.sim, tn, -3., 3.0, -1.5, 3.5, 0, 2, 0.150)
			tn.write("scale by 0.5\n")
			tn.write("rotate x by -20\n")
			tn.write("mol new\n")
			
			self.sim.setParticlePosition(-1, 0)

			print "Done Initializing simulation"

			q1 = TpsTestPeSurf2dOrderParamX(3)
			q2 = TpsTestPeSurf2dOrderParamY(3)
			q = TpsOrderParameterAggregate()
			q.addOrderParameter(q1)
			q.addOrderParameter(q2)
			init = TpsInitializerBruteForce()

			trj = TpsTrajectoryUniformStep(self.sim, 500, 5)
			self.tpe = TpsTrajectoryEnsemble(trj)
			working_traj = self.tpe.getTrajectory(-1)
			self.sim.setPathEnsemble(self.tpe)
			
			print "Initializing"
			self.sim.show = 0
			TpsAlgorithmTPS.__init__(self, self.tpe, rng, q, init)
			writer.analyze(self.tpe.getLastTrajectory())

			#dv = TpsTimesliceModifierPerturbVelocities(0.0001, 1.0, 1, 1)
			self.shift = TpsTrialMoveShift(50, 400)
			self.fshot = TpsTrialMoveShotForward()
			self.bshot = TpsTrialMoveShotBackward()
			self.shot = TpsTrialMoveShot()
			self.flex = TpsTrialMoveShotFlexible(500)
			self.ashot = TpsTrialMoveShotAimless(5)
			
			self.addTrialMove(self.ashot)			
			#self.addTrialMove(fshot)
			#self.addTrialMove(bshot)
			#self.addTrialMove(shift)
			
			self.sim.setTemperature(0.15)
			self.sim.show = 0
			self.nstep = 0
			print "starting path sampling:"
			for i in range(1, 501):
				print i
				self.doStep()

			for i in range(1, 1000):
				self.doStepCustom()
				
			var = raw_input("Return/Enter to EXIT ")
			tn.write("exit\n")
			exit(1)
		
		def doStepCustom(self):
			print "What type of TPS step do you want to try?"
			print "(1) Shooting move"
			print "(2) Shifting move"
			print "(3) Aimless shooting move"
			print "(4) Path reversal move"
			print "(5) Forward shooting Move"
			print "(6) Backward shooting Move"
			print "(7) Forward shifting Move"
			print "(8) Backward shifting Move"
			print "(9) Flexible shooting Move"
			
			move = raw_input()
			
			if move == '1':
				self.shot.attempt(self)
			elif move == '2':
				self.shift.attempt(self)
			elif move == '3':
				self.ashot.attempt(self)
			elif move == '4':
				self.reversal.attempt(self)
			elif move == '5':
				self.fshot.attempt(self)
			elif move == '6':
				self.bshot.attempt(self)
			elif move == '7':
				self.fshift.attempt(self)
			elif move == '8':
				self.bshift.attempt(self)
			elif move == '9':
				self.flex.attempt(self)
			else:
				print "ERROR: did not understand input"
			
		def callbackOnTrialMove(self):
			print self.nstep
			if self.nstep >= 500:
				self.sim.show = 1
				self.sim.reset_path_line = 1
				#var = raw_input("Return/Enter to do a trial move: ")
			self.nstep = self.nstep + 1
			
		def callbackOnTrialMoveAccepted(self):
			print "accepted\n"
			print "new path length", self.tpe.getLastTrajectory().getNumberOfTimeslices()
			self.sim.show = 0					
			writer.analyze(self.tpe.getLastTrajectory())
			
			
			
	CustomPathSampling()
		
		