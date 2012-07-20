from tps import *
from math import sqrt
from math import fabs
import telnetlib

def pefield(sim, tn, xmin, xmax, ymin, ymax, zmin, zmax, step):
	print "Drawing potential field, could take a minute..."
	#tn.write("display projection orthographic\n")
	tn.write("axes location off\n")
	tn.write("color scale method BGR\n")
	tn.write("draw sphere {0 0 -5} radius 0.1\n")
	tn.write("graphics 0 color 2\n")
	tn.write("graphics 0 sphere {0 0 -5} radius 0.1\n")
	#tn.write("color Display Background white\n")
	pef = sim.getPotentialField(xmin, xmax, ymin, ymax, step)			
	potential = []
	for i in range(0, len(pef)):
		potential.append(pef[i][2])
	maxp = max(potential)
	minp = min(potential)
	rangep = maxp-minp
	cut = sqrt(2.0)*step*1.1			
	cutoffsq = cut*cut
	for i in range(0, len(pef)):
		xi = pef[i][0]
		yi = pef[i][1]
		zi = zmin + (pef[i][2] - minp) / rangep * (zmax-zmin)
		#tn.write("draw sphere {"+ str(xi) + " " +  str(yi) + " " + str(zi) + "} radius 0.05\n")
		
		for j in range(i+1, len(pef)):
			xj = pef[j][0]
			yj = pef[j][1]
			zj = zmin + (pef[j][2] - minp) / rangep * (zmax-zmin)
			rx1 = xi - pef[j][0]
			ry1 = yi - yj
			rz1 = zi - zj
			rij = rx1*rx1 + ry1*ry1
			
			if rij >= cutoffsq:
				continue

			for k in range(j+1, len(pef)):
				xk = pef[k][0]
				yk = pef[k][1]
				zk = zmin + (pef[k][2] - minp) / rangep * (zmax-zmin)
				rx2 = xi - xk
				ry2 = yi - yk
				rz2 = zi - zk
				rik = rx2*rx2 + ry2*ry2

				rx3 = xj - xk
				ry3 = yj - yk
				rz3 = zj - zk
				rjk = rx3*rx3 + ry3*ry3
				
				#print rij, rik, rjk
				#print i, j, k
				
				if(rik < cutoffsq and rjk < cutoffsq and rij < cutoffsq):
					zr = zmax-zmin
					h = (zi/zr + zj/zr + zk/zr) / 3.0;
					color = int(33 + h*(1057-33))
					colori = int(33 + zi*(1057-33))
					colorj = int(33 + zj*(1057-33))
					colork = int(33 + zk*(1057-33))

					tn.write("draw color " + str(color) + "\n")
					tn.write("draw triangle " + \
					"{" + str(xi) + " " + str(yi) + " " + str(zi) + "} " + \
					"{" + str(xj) + " " + str(yj) + " " + str(zj) + "} " + \
					"{" + str(xk) + " " + str(yk) + " " + str(zk) + "}\n")

					#tn.write("draw tricolor " + \
					#"{" + str(xi) + " " + str(yi) + " " + str(zi) + "} " + \
					#"{" + str(xj) + " " + str(yj) + " " + str(zj) + "} " + \
					#"{" + str(xk) + " " + str(yk) + " " + str(zk) + "} " + \
					#"{" + str(0) + " " + str(0) + " " + str(1) + "} " + \
					#"{" + str(0) + " " + str(0) + " " + str(1) + "} " + \
					#"{" + str(0) + " " + str(0) + " " + str(1) + "} " + \
					#str(colori) + " " + str(colorj) +  " " + str(colork) + "\n")
