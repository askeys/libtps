# The name of our project is "TPS".  CMakeLists files in this project can
# refer to the root source directory of the project as ${TPS_SOURCE_DIR} and
# to the root binary directory of the project as ${TPS_BINARY_DIR}.

#build python wrappers:
#link them with the TPS shared library

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "-I../../../include")
SET(W_LIBRARIES_USED tps;m;${PYTHON_LIBRARIES})

IF(ENABLE_LAMMPS)
	SET(W_LIBRARIES_USED ${W_LIBRARIES_USED};lammps)
	SET(CMAKE_SWIG_FLAGS "${CMAKE_SWIG_FLAGS};-D_USE_LAMMPS")	
	IF(ENABLE_LAMMPS_REVERSIBLE_INTEGRATORS)
		SET(CMAKE_SWIG_FLAGS "${CMAKE_SWIG_FLAGS};-D_USE_LAMMPS_PATCH")	
	ENDIF(ENABLE_LAMMPS_REVERSIBLE_INTEGRATORS)
ENDIF(ENABLE_LAMMPS)

#MESSAGE(${CMAKE_SWIG_FLAGS})

SET_SOURCE_FILES_PROPERTIES(tps.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(tps.i PROPERTIES SWIG_FLAGS ${CMAKE_SWIG_FLAGS})
SWIG_ADD_MODULE(tps python
	tps.i)
SWIG_LINK_LIBRARIES(tps ${W_LIBRARIES_USED})	

#SET_SOURCE_FILES_PROPERTIES(tpstest.i PROPERTIES CPLUSPLUS ON)
#SET_SOURCE_FILES_PROPERTIES(tpstest.i PROPERTIES SWIG_FLAGS ${CMAKE_SWIG_FLAGS})
#SWIG_ADD_MODULE(tpstest python
#	tpstest.i)
#SWIG_LINK_LIBRARIES(tpstest tps;${W_LIBRARIES_USED})	

IF (APPLE)
	SET(PYTHON_INSTALL_DIR "/Library/Python/2.5/site-packages")
ELSE(APPLE)
	SET(PYTHON_INSTALL_DIR ${LIBRARY_OUTPUT_PATH})
ENDIF(APPLE)

FILE(GLOB tpspyfiles "${TPS_SOURCE_DIR}/wrap/python/tps*.py*")
INSTALL(FILES ${tpspyfiles} DESTINATION ${PYTHON_INSTALL_DIR})

FILE(WRITE ${EXECUTABLE_OUTPUT_PATH}/pythonpath.csh 
        "setenv PYTHONPATH ${PYTHON_INSTALL_DIR}\n"
        "\n"
        "if ( $?WXPATH ) then\n"
        "  setenv PYTHONPATH $WXPATH\\:$PYTHONPATH\n"
        "endif\n"
        "\n"
        "if ( $?WXLIB ) then\n"
        "  if ( $?LD_LIBRARY_PATH ) then\n"
        "    setenv LD_LIBRARY_PATH $WXLIB\\:$LD_LIBRARY_PATH\n"
        "  else\n"
        "    setenv LD_LIBRARY_PATH $WXLIB\n"
        "  endif\n"
        "endif\n"
        "\n"
        )
FILE(WRITE ${EXECUTABLE_OUTPUT_PATH}/pythonpath.sh 
        "export PYTHONPATH=${PYTHON_INSTALL_DIR}\n\n")
