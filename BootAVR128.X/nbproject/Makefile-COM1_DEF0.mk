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
ifeq "$(wildcard nbproject/Makefile-local-COM1_DEF0.mk)" "nbproject/Makefile-local-COM1_DEF0.mk"
include nbproject/Makefile-local-COM1_DEF0.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=COM1_DEF0
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=main.cpp ../_Lib/crc.S ../_Lib/usarts.cpp ../_Lib/protected_io.S

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/_ext/761250667/crc.o ${OBJECTDIR}/_ext/761250667/usarts.o ${OBJECTDIR}/_ext/761250667/protected_io.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/_ext/761250667/crc.o.d ${OBJECTDIR}/_ext/761250667/usarts.o.d ${OBJECTDIR}/_ext/761250667/protected_io.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/_ext/761250667/crc.o ${OBJECTDIR}/_ext/761250667/usarts.o ${OBJECTDIR}/_ext/761250667/protected_io.o

# Source Files
SOURCEFILES=main.cpp ../_Lib/crc.S ../_Lib/usarts.cpp ../_Lib/protected_io.S

# Pack Options 
PACK_COMPILER_OPTIONS=-I "${DFP_DIR}/include"
PACK_COMMON_OPTIONS=-B "${DFP_DIR}/gcc/dev/avr128db48"



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

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-COM1_DEF0.mk ${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=AVR128DB48
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/COM1_DEF0/1e8b73bef40c319f525ccf274dcb05c59e9a5d88 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/COM1_DEF0/fa56e37f7e2688940f6704d9d2c690cd660c72af .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.ok ${OBJECTDIR}/_ext/761250667/crc.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -DDEBUG  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT ${OBJECTDIR}/_ext/761250667/crc.o -o ${OBJECTDIR}/_ext/761250667/crc.o ../_Lib/crc.S  -DXPRJ_COM1_DEF0=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/crc.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1
	
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/COM1_DEF0/a95b690f36715015dbda6b46fa3ab9044ffae120 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/COM1_DEF0/23843006232b64b98744f14b2696a672727cabc0 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.ok ${OBJECTDIR}/_ext/761250667/protected_io.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -DDEBUG  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT ${OBJECTDIR}/_ext/761250667/protected_io.o -o ${OBJECTDIR}/_ext/761250667/protected_io.o ../_Lib/protected_io.S  -DXPRJ_COM1_DEF0=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/protected_io.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1
	
else
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/COM1_DEF0/b92cdb1c1aabe25d7a5bdf3e95cee55dc4e7d3af .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/COM1_DEF0/392dd667f951f441082e625adc590b611eff20ee .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.ok ${OBJECTDIR}/_ext/761250667/crc.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT ${OBJECTDIR}/_ext/761250667/crc.o -o ${OBJECTDIR}/_ext/761250667/crc.o ../_Lib/crc.S  -DXPRJ_COM1_DEF0=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/crc.o.asm.d"
	
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/COM1_DEF0/adf6750d37cbdd7fbdbfd15a45adadacbc052c75 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/COM1_DEF0/4ede408d22e86b854bb825a7e67c66bfa8d91138 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.ok ${OBJECTDIR}/_ext/761250667/protected_io.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT ${OBJECTDIR}/_ext/761250667/protected_io.o -o ${OBJECTDIR}/_ext/761250667/protected_io.o ../_Lib/protected_io.S  -DXPRJ_COM1_DEF0=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/protected_io.o.asm.d"
	
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
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/COM1_DEF0/4ecb7dbd1f7d6478e4bd0edc495ba0c7cbc9912d .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/COM1_DEF0/858ddd8b70b8dfccb4a3fdcdaacd7f476d068bf4 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DUSEUSART1=DEF0,255,TB0 -DNOINT_UART -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.cpp  -DXPRJ_COM1_DEF0=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/usarts.o: ../_Lib/usarts.cpp  .generated_files/flags/COM1_DEF0/f74b2aac7ddccdc864ae313e2cce354a2a296554 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/usarts.o: ../_Lib/usarts.cpp  .generated_files/flags/COM1_DEF0/cc81e6d86a0fe2ce08697f2398cdb4dbcf95da6c .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DUSEUSART1=DEF0,255,TB0 -DNOINT_UART -Wall -MD -MP -MF "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT ${OBJECTDIR}/_ext/761250667/usarts.o  -o ${OBJECTDIR}/_ext/761250667/usarts.o ../_Lib/usarts.cpp  -DXPRJ_COM1_DEF0=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
else
<<<<<<< HEAD
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/COM1_DEF0/e027881478b24da2e55c2f7bb0db3b47b6ed1ba2 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/COM1_DEF0/23e74e18c4785cefcfccafb9d8162cd78688e1f1 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DUSEUSART1=DEF0,255,TB0 -DNOINT_UART -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.cpp  -DXPRJ_COM1_DEF0=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
<<<<<<< HEAD
${OBJECTDIR}/_ext/761250667/usarts.o: ../_Lib/usarts.cpp  .generated_files/flags/COM1_DEF0/c9fb9769126ff6013fed2ebe838bc9eaa140deb7 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
=======
${OBJECTDIR}/_ext/761250667/usarts.o: ../_Lib/usarts.cpp  .generated_files/flags/COM1_DEF0/8fb1243156145617232cac4e868be7fa86a17110 .generated_files/flags/COM1_DEF0/da39a3ee5e6b4b0d3255bfef95601890afd80709
>>>>>>> 1e2ebb7c39ea71bec11d11114db458bc0a636ebf
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DUSEUSART1=DEF0,255,TB0 -DNOINT_UART -Wall -MD -MP -MF "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT ${OBJECTDIR}/_ext/761250667/usarts.o  -o ${OBJECTDIR}/_ext/761250667/usarts.o ../_Lib/usarts.cpp  -DXPRJ_COM1_DEF0=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=avr128db48 ${PACK_COMMON_OPTIONS}   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -nostartfiles -Wl,-Map="${DISTDIR}\BootAVR128.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_COM1_DEF0=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	
	
	${MP_CC_DIR}\\avr-objdump -h -S "${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.lss"
	
	
	
else
${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=avr128db48 ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -nostartfiles -Wl,-Map="${DISTDIR}\BootAVR128.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_COM1_DEF0=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	${MP_CC_DIR}\\avr-objcopy -O ihex "${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.hex"
	
	${MP_CC_DIR}\\avr-objdump -h -S "${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "${DISTDIR}/BootAVR128.X.${IMAGE_TYPE}.lss"
	
	
	
endif


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
