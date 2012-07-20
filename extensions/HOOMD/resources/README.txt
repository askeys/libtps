The TPS library works with together with HOOMD to run GP/GPU simulations.  To use HOOMD with libtps first download the hoomd source code from CodeBlue:

mkdir hoomd
cd hoomd
svn co https://codeblue.umich.edu/hoomd-blue/svn/trunk .

See http://codeblue.umich.edu/hoomd-blue/download.html for more details.

Then configure hoomd using CMake. See http://codeblue.umich.edu/hoomd-blue/doc/page_compile_guide.html for more details.  *** Make sure to set the "ENABLE_PLUGINAPI" option to "YES." ***

After installing you should be able to compile HOOMD-enabled codes by using something like:

c++ TpsSimulationPluginHOOMD.cpp my_hoomd_enabled_tps_code.cpp -ltps $(hoomd-config.sh --cflags --libs)

The hoomd-config.sh script is defined by the HOOMD plugin API and generates the appropriate C-flags and link libraries.