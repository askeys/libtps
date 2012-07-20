The TPS library uses any version of LAMMPS, but requires
some minor patches in order to work correctly with ensembles
other than NVE (NVT, NPT, etc.).  

Steps to use TPS library with LAMMPS:

1) Download LAMMPS from http://lammps.sandia.gov/download.html
2) Copy the CMakeLists.txt and patch.py files from this directory
   to the lammps root directory.
3) Run patch.py in the LAMMPS main directory:
4) Run CMake and install LAMMPS as a library


#Example:

#assume tps library source lives in ~/Codes/tps
#assume lammps source lives in ~/Codes/lammps-6Apr08

  cp ~/Codes/tps/3rdparty/LAMMPS/* ~/Codes/lammps-6Apr08
  cd ~/Codes/lammps-6Apr08
  python patch.py 
  cd ~/Builds
  mkdir lammps-build
  ccmake ~/Codes/lammps-6Apr08


If you ever need to undo the patch, run "python unpatch.py" in 
the LAMMPS root directory
