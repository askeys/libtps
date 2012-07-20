HOOMD_BUILD_DIR=$1
CONFIG_FILE="hoomd-config.sh"

echo "#HOOMD setup file" > $CONFIG_FILE
str=$(cat ${HOOMD_BUILD_DIR}/unit_tests/CMakeFiles/system_test.dir/flags.make | grep CXX_FLAGS | sed s/"CXX_FLAGS = "/HOOMD_CXX_FLAGS=\"/)
echo $str\">> $CONFIG_FILE
str=$(cat ${HOOMD_BUILD_DIR}/unit_tests/CMakeFiles/system_test.dir/flags.make | grep CXX_DEFINES | sed s/"CXX_DEFINES = "/HOOMD_CXX_DEFINES=\"/)
echo $str\">> $CONFIG_FILE
str=$(cat ${HOOMD_BUILD_DIR}/unit_tests/CMakeFiles/system_test.dir/link.txt | sed s/"\/usr\/bin\/c++"/HOOMD_LINK_FLAGS=\"/ | sed s%CMakeFiles/system_test.dir/system_test.cc.o%""% | sed s%CMakeFiles%${HOOMD_BUILD_DIR}/unit_tests/CMakeFiles% | sed s%"\.\.\/"%${HOOMD_BUILD_DIR}/%g | sed s/"-o system_test"/""/)
echo $str\">> $CONFIG_FILE
echo "HOOMD_FLAGS=\"\${HOOMD_CXX_FLAGS} \${HOOMD_CXX_DEFINES} \${HOOMD_LINK_FLAGS}\"" >> $CONFIG_FILE


