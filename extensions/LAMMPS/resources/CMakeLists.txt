# The name of our project is "LAMMPS".  CMakeLists files in this project can
# refer to the root source directory of the project as ${LAMMPS_SOURCE_DIR} and
# to the root binary directory of the project as ${LAMMPS_SOURCE_DIR}.

cmake_minimum_required(VERSION 2.4)

PROJECT(LAMMPS)

#############################################
# Compiler and path settings:
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS}" CACHE INTERNAL "Compiler optimization flags.")
SET(EXECUTABLE_OUTPUT_PATH ${LAMMPS_SOURCE_DIR}/bin CACHE INTERNAL "Single output directory for executables.")
SET(LIBRARY_OUTPUT_PATH ${LAMMPS_SOURCE_DIR}/lib CACHE INTERNAL "Single output directory for libraries.")
SET(COMPILER_FLAGS "-O3 -funroll-loops" CACHE STRING "Compiler optimization flags.")

############################################

#Set up definitions and libraries:

#############################################
#enable / disable MPI:
OPTION(ENABLE_MPI "use MPI for parallel code" 1)
IF(ENABLE_MPI)
	EXECUTE_PROCESS(WORKING_DIRECTORY /src COMMAND make no-STUBS ERROR_QUIET OUTPUT_QUIET)
	SET(CMAKE_CXX_COMPILER mpic++)
	SET(CMAKE_C_COMPILER mpicc)	
ELSE(ENABLE_MPI)
	EXECUTE_PROCESS(WORKING_DIRECTORY /src COMMAND make yes-STUBS ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_MPI)
############################################

 
#############################################
#enable / disable the ASPHERE package:
OPTION(ENABLE_ASPHERE "use the ASPHERE package" 0)
IF(ENABLE_ASPHERE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-ASPHERE ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_ASPHERE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-ASPHERE ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_ASPHERE)
############################################

 
#############################################
#enable / disable the CLASS2 package:
OPTION(ENABLE_CLASS2 "use the CLASS2 package" 0)
IF(ENABLE_CLASS2)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-CLASS2 ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_CLASS2)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-CLASS2 ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_CLASS2)
############################################

 
#############################################
#enable / disable the COLLOID package:
OPTION(ENABLE_COLLOID "use the COLLOID package" 0)
IF(ENABLE_COLLOID)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-COLLOID ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_COLLOID)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-COLLOID ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_COLLOID)
############################################

 
#############################################
#enable / disable the DIPOLE package:
OPTION(ENABLE_DIPOLE "use the DIPOLE package" 0)
IF(ENABLE_DIPOLE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-DIPOLE ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_DIPOLE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-DIPOLE ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_DIPOLE)
############################################

 
#############################################
#enable / disable the DPD package:
OPTION(ENABLE_DPD "use the DPD package" 0)
IF(ENABLE_DPD)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-DPD ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_DPD)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-DPD ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_DPD)
############################################

 
#############################################
#enable / disable the GRANULAR package:
OPTION(ENABLE_GRANULAR "use the GRANULAR package" 0)
IF(ENABLE_GRANULAR)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-GRANULAR ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_GRANULAR)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-GRANULAR ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_GRANULAR)
############################################

 
#############################################
#enable / disable the KSPACE package:
OPTION(ENABLE_KSPACE "use the KSPACE package" 0)
IF(ENABLE_KSPACE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-KSPACE ERROR_QUIET OUTPUT_QUIET)
	
	OPTION(USE_FFTW "use FFTW for FFT" 1)
	IF(USE_FFTW)
		SET(FFTW_LIB_ROOT /usr/local CACHE STRING "location of FFTW library")
		SET(FFTW_LIB_NAME fftw CACHE STRING "name of the FFTW library")
		SET(LIBRARIES_USED ${LIBRARIES_USED};${FFTW_LIB_NAME})
		LINK_DIRECTORIES(${FFTW_LIB_ROOT}/lib)
		ADD_DEFINITIONS(-DFFT_FFTW)
		INCLUDE_DIRECTORIES(${FFTW_LIB_ROOT}/include)
	ELSE(USE_FFTW)	
		SET(FFT_FLAG FFT_NONE CACHE STRING "FFT compiler flag. See src/fft3d.h for details/options")
	ENDIF(USE_FFTW)
ELSE(ENABLE_KSPACE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-KSPACE ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_KSPACE)
############################################

 
#############################################
#enable / disable the MANYBODY package:
OPTION(ENABLE_MANYBODY "use the MANYBODY package" 0)
IF(ENABLE_MANYBODY)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-MANYBODY ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_MANYBODY)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-MANYBODY ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_MANYBODY)
############################################

 
#############################################
#enable / disable the MEAM package:
OPTION(ENABLE_MEAM "use the MEAM package" 0)
IF(ENABLE_MEAM)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-MEAM ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_MEAM)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-MEAM ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_MEAM)
############################################

 
#############################################
#enable / disable the MOLECULE package:
OPTION(ENABLE_MOLECULE "use the MOLECULE package" 0)
IF(ENABLE_MOLECULE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-MOLECULE ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_MOLECULE)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-MOLECULE ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_MOLECULE)
############################################

 
#############################################
#enable / disable the OPT package:
OPTION(ENABLE_OPT "use the OPT package" 0)
IF(ENABLE_OPT)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-OPT ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_OPT)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-OPT ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_OPT)
############################################

 
#############################################
#enable / disable the PERI package:
OPTION(ENABLE_PERI "use the PERI package" 0)
IF(ENABLE_PERI)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-PERI ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_PERI)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-PERI ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_PERI)
############################################

 
#############################################
#enable / disable the POEMS package:
OPTION(ENABLE_POEMS "use the POEMS package" 0)
IF(ENABLE_POEMS)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-POEMS ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_POEMS)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-POEMS ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_POEMS)
############################################

 
#############################################
#enable / disable the REAX package:
OPTION(ENABLE_REAX "use the REAX package" 0)
IF(ENABLE_REAX)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-REAX ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_REAX)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-REAX ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_REAX)
############################################

 
#############################################
#enable / disable the STUBS package:
OPTION(ENABLE_STUBS "use the STUBS package" 0)
IF(ENABLE_STUBS)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-STUBS ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_STUBS)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-STUBS ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_STUBS)
############################################

 
#############################################
#enable / disable the USER-ACKLAND package:
OPTION(ENABLE_USER-ACKLAND "use the USER-ACKLAND package" 0)
IF(ENABLE_USER-ACKLAND)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-USER-ACKLAND ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_USER-ACKLAND)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-USER-ACKLAND ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_USER-ACKLAND)
############################################

 
#############################################
#enable / disable the USER-CG-CMM package:
OPTION(ENABLE_USER-CG-CMM "use the USER-CG-CMM package" 0)
IF(ENABLE_USER-CG-CMM)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-USER-CG-CMM ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_USER-CG-CMM)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-USER-CG-CMM ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_USER-CG-CMM)
############################################

 
#############################################
#enable / disable the USER-EWALDN package:
OPTION(ENABLE_USER-EWALDN "use the USER-EWALDN package" 0)
IF(ENABLE_USER-EWALDN)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-USER-EWALDN ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_USER-EWALDN)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-USER-EWALDN ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_USER-EWALDN)
############################################

 
#############################################
#enable / disable the USER-SMD package:
OPTION(ENABLE_USER-SMD "use the USER-SMD package" 0)
IF(ENABLE_USER-SMD)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-USER-SMD ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_USER-SMD)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-USER-SMD ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_USER-SMD)
############################################

 
#############################################
#enable / disable the XTC package:
OPTION(ENABLE_XTC "use the XTC package" 0)
IF(ENABLE_XTC)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make yes-XTC ERROR_QUIET OUTPUT_QUIET)
ELSE(ENABLE_XTC)
	EXECUTE_PROCESS(WORKING_DIRECTORY ${LAMMPS_SOURCE_DIR}/src COMMAND make no-XTC ERROR_QUIET OUTPUT_QUIET)
ENDIF(ENABLE_XTC)
############################################




############################################
#Set up source files:
INCLUDE_DIRECTORIES(src)
AUX_SOURCE_DIRECTORY(src LAMMPS_SRC)
############################################

############################################
#build a static library:
OPTION(BUILD_LIBRARY "build lammps as a static library" 0)
IF(BUILD_LIBRARY) 
	SET(LAMMPS_LIBRARY_NAME "lammps" CACHE STRING "Name of LAMMPS library.  Note: automatically prepends lib and appends .a")
	STRING(REPLACE "src/main.cpp;" "" LAMMPS_LIB_SRC "${LAMMPS_SRC}")
	ADD_LIBRARY(${LAMMPS_LIBRARY_NAME} ${LAMMPS_LIB_SRC})
	TARGET_LINK_LIBRARIES(${LAMMPS_LIBRARY_NAME} ${LIBRARIES_USED})
	#INSTALL_FILES(${CMAKE_INSTALL_PREFIX}/include/lammps .h ${LAMMPS_LIB_SRC})
	FILE(GLOB files "${LAMMPS_SOURCE_DIR}/src/*.h")
	INSTALL(FILES ${files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lammps)
	
	INSTALL(TARGETS ${LAMMPS_LIBRARY_NAME}
			RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
			LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
            		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF(BUILD_LIBRARY) 
############################################

############################################
#build the program:
OPTION(BUILD_EXECUTABLE "build the LAMMPS program" 1)
IF(BUILD_EXECUTABLE) 
	IF(APPLE) 
		SET(LAMMPS_EXECUTABLE_NAME "lmp_mac" CACHE STRING "Name of LAMMPS executable")
	ENDIF(APPLE) 
	IF(WIN32) 
		SET(LAMMPS_EXECUTABLE_NAME "lmp_win" CACHE STRING "Name of LAMMPS executable")
	ENDIF(WIN32) 
	IF(UNIX) 
		SET(LAMMPS_EXECUTABLE_NAME "lmp_unix" CACHE STRING "Name of LAMMPS executable")
	ELSE(UNIX)
		SET(LAMMPS_EXECUTABLE_NAME "lmp_linux" CACHE STRING "Name of LAMMPS executable")
	ENDIF(UNIX) 

	SET(LIBRARIES_USED ${LIBRARIES_USED};${FFTW_LIB})
	ADD_EXECUTABLE(${LAMMPS_EXECUTABLE_NAME} ${LAMMPS_SRC})
	TARGET_LINK_LIBRARIES(${LAMMPS_EXECUTABLE_NAME} "${LIBRARIES_USED}")

	INSTALL(TARGETS ${LAMMPS_EXECUTABLE_NAME}
			RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
			LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
            		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF(BUILD_EXECUTABLE) 
############################################


