#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/d76af62e/main.o \
	${OBJECTDIR}/_ext/9f9f4a83/functions.o \
	${OBJECTDIR}/_ext/faac28bc/image.o \
	${OBJECTDIR}/_ext/faac28bc/message.o \
	${OBJECTDIR}/_ext/faac28bc/monitor.o \
	${OBJECTDIR}/_ext/faac28bc/robot.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/raspberryrobot

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/raspberryrobot: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/raspberryrobot ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/d76af62e/main.o: ../../superviseur_robot-master/destijl_init/main.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d76af62e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d76af62e/main.o ../../superviseur_robot-master/destijl_init/main.cpp

${OBJECTDIR}/_ext/9f9f4a83/functions.o: ../../superviseur_robot-master/destijl_init/src/functions.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9f9f4a83
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9f9f4a83/functions.o ../../superviseur_robot-master/destijl_init/src/functions.cpp

${OBJECTDIR}/_ext/faac28bc/image.o: ../../superviseur_robot-master/src/image.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/faac28bc
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/faac28bc/image.o ../../superviseur_robot-master/src/image.cpp

${OBJECTDIR}/_ext/faac28bc/message.o: ../../superviseur_robot-master/src/message.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/faac28bc
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/faac28bc/message.o ../../superviseur_robot-master/src/message.cpp

${OBJECTDIR}/_ext/faac28bc/monitor.o: ../../superviseur_robot-master/src/monitor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/faac28bc
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/faac28bc/monitor.o ../../superviseur_robot-master/src/monitor.cpp

${OBJECTDIR}/_ext/faac28bc/robot.o: ../../superviseur_robot-master/src/robot.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/faac28bc
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/faac28bc/robot.o ../../superviseur_robot-master/src/robot.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
