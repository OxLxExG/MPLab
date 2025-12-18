#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../_Lib/crc.S ../_Lib/protected_io.S main.cpp

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/761250667/crc.o ${OBJECTDIR}/_ext/761250667/protected_io.o ${OBJECTDIR}/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/761250667/crc.o.d ${OBJECTDIR}/_ext/761250667/protected_io.o.d ${OBJECTDIR}/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/761250667/crc.o ${OBJECTDIR}/_ext/761250667/protected_io.o ${OBJECTDIR}/main.o

# Source Files
SOURCEFILES=../_Lib/crc.S ../_Lib/protected_io.S main.cpp

# Pack Options 
PACK_COMPILER_OPTIONS=-I "${DFP_DIR}/include"
PACK_COMMON_OPTIONS=-B "${DFP_DIR}/gcc/dev/attiny1616"



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
_/_=\\
ShExtension=.bat
Device=ATtiny1616
ProjectDir="C:\Projects\MPLab\ggkp1616.X"
ProjectName=ggkp1616
ConfName=default
ImagePath="dist\default\${IMAGE_TYPE}\ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\default\${IMAGE_TYPE}"
ImageName="ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif
MDFUHostPath="C:\Program Files\Microchip\MPLABX\v6.25\mplab_platform\mplab-pymdfu\bin\windows\pymdfu-bin.exe"
PYFWImageBuilderPath="C:\Program Files\Microchip\MPLABX\v6.25\mplab_platform\mplab-pymdfu\bin\windows\pyfwimagebuilder-bin.exe"

.build-conf:  .pre ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: [..\e.bat ${ProjectName} ${ProjectDir} ${ImagePath}]"
	@..\e.bat ${ProjectName} ${ProjectDir} ${ImagePath}
	@echo "--------------------------------------"

MP_PROCESSOR_OPTION=ATtiny1616
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/default/283dfd6b8450fa55904cc1ca3f4b4ec8b9790595 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/default/1eccb7340ae11e9c4043bffc933e4ffc53f78928 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.ok ${OBJECTDIR}/_ext/761250667/crc.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=attiny1616 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -DDEBUG  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -I"_I" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT ${OBJECTDIR}/_ext/761250667/crc.o -o ${OBJECTDIR}/_ext/761250667/crc.o ../_Lib/crc.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/crc.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1
	
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/default/61e2e35fdd129605581e41103ca37440370eb8f6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/default/22bdd2bd3ac6a26f1412377d867ec92e14960d65 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.ok ${OBJECTDIR}/_ext/761250667/protected_io.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=attiny1616 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -DDEBUG  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -I"_I" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT ${OBJECTDIR}/_ext/761250667/protected_io.o -o ${OBJECTDIR}/_ext/761250667/protected_io.o ../_Lib/protected_io.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/protected_io.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1
	
else
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/default/a951ba4ae77194dbc86573569929bc528c64a86 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/default/7c40ddeb10bdad06c927161f8e02b6c28524e035 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.ok ${OBJECTDIR}/_ext/761250667/crc.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=attiny1616 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -I"_I" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT ${OBJECTDIR}/_ext/761250667/crc.o -o ${OBJECTDIR}/_ext/761250667/crc.o ../_Lib/crc.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/crc.o.asm.d"
	
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/default/15e29eafbbab9fa4aed0b0ae61e77f4116a2a719 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/default/f8a2b9e0941db801500e6083e0d34edf1a168243 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.ok ${OBJECTDIR}/_ext/761250667/protected_io.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=attiny1616 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -I"_I" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT ${OBJECTDIR}/_ext/761250667/protected_io.o -o ${OBJECTDIR}/_ext/761250667/protected_io.o ../_Lib/protected_io.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/protected_io.o.asm.d"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
<<<<<<< HEAD
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/default/abd5968fce265167c1bc76d917bcee245bb111b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/default/f9367f505312d877da9e1791650108273928373b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1616 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -I"_I" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=10000000UL -DADDRESS_PROCESSOR=4 -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
else
<<<<<<< HEAD
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/default/3d39c71a78e335b41fe55850ec21d776eba1bace .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/default/1742658996232dc6cba1d58f4940de2314123d1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1616 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -I"_I" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=10000000UL -DADDRESS_PROCESSOR=4 -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1616 ${PACK_COMMON_OPTIONS}   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\ggkp1616.X.${IMAGE_TYPE}.map"   -Xlinker -T ../avrxmega3.x  -Wl,-section-start=.text=0x400  -o ${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	
	
	${MP_CC_DIR}\\avr-objdump -h -S "${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.lss"
	
	
	
else
${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1616 ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\ggkp1616.X.${IMAGE_TYPE}.map"   -Xlinker -T ../avrxmega3.x  -Wl,-section-start=.text=0x400  -o ${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	${MP_CC_DIR}\\avr-objcopy -O ihex "${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.hex"
	
	${MP_CC_DIR}\\avr-objdump -h -S "${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "${DISTDIR}/ggkp1616.X.${IMAGE_TYPE}.lss"
	
	
	
endif

.pre:
	@echo "--------------------------------------"
	@echo "User defined pre-build step: [..\p.bat ${ProjectDir}]"
	@..\p.bat ${ProjectDir}
	@echo "--------------------------------------"

# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
