# The name of our project is "TPS".  CMakeLists files in this project can
# refer to the root source directory of the project as ${TPS_SOURCE_DIR} and
# to the root binary directory of the project as ${TPS_BINARY_DIR}.

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(TPS)

# Compiler and path settings:
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS}" CACHE INTERNAL "Compiler optimization flags.")
SET(EXECUTABLE_OUTPUT_PATH ${TPS_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for executables.")
SET(LIBRARY_OUTPUT_PATH ${TPS_BINARY_DIR}/lib CACHE INTERNAL "Single output directory for libraries.")
SET(OPTIMIZATION_FLAGS "-O3 -funroll-loops -Wall" CACHE STRING "Compiler optimization flags.")
SET(SWIG_DIR /usr/local CACHE STRING "directory containing SWIG.")

# Wrappers:
OPTION(WRAP_PYTHON "use tps from python" 0)
#OPTION(WRAP_JAVA "use tps from java" 0)

FILE(WRITE ${TPS_SOURCE_DIR}/src/config.h "//configure file for tps\n")

#################################
find_package(Doxygen)
IF(DOXYGEN_FOUND)
	OPTION(ENABLE_DOXYGEN "Enables building of documentation with doxygen" 1)
	IF(ENABLE_DOXYGEN)
		ADD_SUBDIRECTORY(dox)
		#ADD_CUSTOM_TARGET(doc ALL ${DOXYGEN_EXECUTABLE} ${TPS_SOURCE_DIR}/Doxyfile)
	ENDIF(ENABLE_DOXYGEN)
ENDIF(DOXYGEN_FOUND)
#################################

#################################
#make tps.h header file:

FILE(GLOB HEADER_FILES "${TPS_SOURCE_DIR}/src/*.h")
STRING(REPLACE "${TPS_SOURCE_DIR}/src/tps.h;" "" HEADER_FILES "${HEADER_FILES}")
STRING(REPLACE "${TPS_SOURCE_DIR}/src/" "" HEADER_FILES "${HEADER_FILES}")
#MESSAGE(STATUS ${HEADER_FILES})

FILE(WRITE ${TPS_SOURCE_DIR}/src/tps.h "#ifndef _TPS_H\n#define _TPS_H\n\n")
#FILE(APPEND ${TPS_SOURCE_DIR}/src/tps.h "#include \"config.h"\n) 

FOREACH(_FILENAME ${HEADER_FILES})
	FILE(APPEND ${TPS_SOURCE_DIR}/src/tps.h "#include \"${_FILENAME}\"\n")
ENDFOREACH(_FILENAME ${HEADER_FILES})
FILE(APPEND ${TPS_SOURCE_DIR}/src/tps.h "\n#endif\n\n") 
################################


#################################
#source files

INCLUDE_DIRECTORIES(src)
AUX_SOURCE_DIRECTORY(src TPS_SRC)

# Create a library called "tps" 
# note: some of these variables will be empty if not defined

ADD_LIBRARY(tps 
	${TPS_SRC}
)

TARGET_LINK_LIBRARIES(tps ${LIBRARIES_USED})

FILE(GLOB files "${TPS_SOURCE_DIR}/src/*.h")
INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/tps)
INSTALL(TARGETS tps
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
      		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

IF(WRAP_PYTHON)
	ADD_SUBDIRECTORY(wrap/python)
ENDIF(WRAP_PYTHON)

