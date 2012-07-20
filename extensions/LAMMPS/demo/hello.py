# typical command to run
# python helloLAMMPS.py
# Make sure to place _tps.so and tps.py 
# in a global path or in the working directory

from tps import *
print "Testing the LAMMPS plugin..."
lmp = TpsSimulationPluginLAMMPS(argc, argv)
lmp.readInputScript("test_input.lammps.txt");
print "Success!"