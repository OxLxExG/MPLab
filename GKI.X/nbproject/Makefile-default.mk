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
FINAL_IMAGE=${DISTDIR}/GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=../_Lib/crc.S ../_Lib/protected_io.S ../_Lib/usarts.cpp main.cpp

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/761250667/crc.o ${OBJECTDIR}/_ext/761250667/protected_io.o ${OBJECTDIR}/_ext/761250667/usarts.o ${OBJECTDIR}/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/761250667/crc.o.d ${OBJECTDIR}/_ext/761250667/protected_io.o.d ${OBJECTDIR}/_ext/761250667/usarts.o.d ${OBJECTDIR}/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/761250667/crc.o ${OBJECTDIR}/_ext/761250667/protected_io.o ${OBJECTDIR}/_ext/761250667/usarts.o ${OBJECTDIR}/main.o

# Source Files
SOURCEFILES=../_Lib/crc.S ../_Lib/protected_io.S ../_Lib/usarts.cpp main.cpp

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

# The following macros may be used in the pre and post step lines
_/_=\\
ShExtension=.bat
Device=AVR128DB48
ProjectDir="D:\Projects\Mplab\GKI.X"
ProjectName=GKI
ConfName=default
ImagePath="dist\default\${IMAGE_TYPE}\GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\default\${IMAGE_TYPE}"
ImageName="GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif
MDFUHostPath="D:\IDEs\MPLab\mplab_platform\mplab-pymdfu\bin\windows\pymdfu-bin.exe"
PYFWImageBuilderPath="D:\IDEs\MPLab\mplab_platform\mplab-pymdfu\bin\windows\pyfwimagebuilder-bin.exe"

.build-conf:  .pre ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: [..\e.bat ${ProjectName} ${ProjectDir} ${ImagePath}]"
	@..\e.bat ${ProjectName} ${ProjectDir} ${ImagePath}
	@echo "--------------------------------------"

MP_PROCESSOR_OPTION=AVR128DB48
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/default/c97451e03eedcfd2b837e650272ac4119c82916 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.ok ${OBJECTDIR}/_ext/761250667/crc.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -DDEBUG  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT ${OBJECTDIR}/_ext/761250667/crc.o -o ${OBJECTDIR}/_ext/761250667/crc.o ../_Lib/crc.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/crc.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1
	
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/default/becb7199b10fa0c1d353e137042926e8ed238611 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.ok ${OBJECTDIR}/_ext/761250667/protected_io.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -DDEBUG  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT ${OBJECTDIR}/_ext/761250667/protected_io.o -o ${OBJECTDIR}/_ext/761250667/protected_io.o ../_Lib/protected_io.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/protected_io.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1
	
else
${OBJECTDIR}/_ext/761250667/crc.o: ../_Lib/crc.S  .generated_files/flags/default/bbdd150e82e78bfce703b436c171ef5f5030b330 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/crc.o.ok ${OBJECTDIR}/_ext/761250667/crc.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT "${OBJECTDIR}/_ext/761250667/crc.o.d" -MT ${OBJECTDIR}/_ext/761250667/crc.o -o ${OBJECTDIR}/_ext/761250667/crc.o ../_Lib/crc.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/crc.o.asm.d"
	
${OBJECTDIR}/_ext/761250667/protected_io.o: ../_Lib/protected_io.S  .generated_files/flags/default/990d2db513a01b48033c93bb46d085adda765fdd .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o 
	@${RM} ${OBJECTDIR}/_ext/761250667/protected_io.o.ok ${OBJECTDIR}/_ext/761250667/protected_io.o.err 
	 ${MP_CPPC} $(MP_EXTRA_AS_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x assembler-with-cpp -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -MD -MP -MF "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT "${OBJECTDIR}/_ext/761250667/protected_io.o.d" -MT ${OBJECTDIR}/_ext/761250667/protected_io.o -o ${OBJECTDIR}/_ext/761250667/protected_io.o ../_Lib/protected_io.S  -DXPRJ_default=$(CND_CONF)  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/761250667/protected_io.o.asm.d"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/761250667/usarts.o: ../_Lib/usarts.cpp  .generated_files/flags/default/a41945a826bb1778d7a47538745594ea5a6c4e33 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DADDRESS_PROCESSOR=3 -DCLOCK=CLKHT,TA0,CLKOUT -DDIOD=PF,2,INV -DGK=CMP0,AINP1,TA1 -DUSEUSART2=DEF0,255,TB0 -DUSEUSART3=DEF0,200,SPI_MODE -DMX25L256=SP1,ALT1,PF,4,UART2 -DADS131=UART3,PB,4,PB,3,PB,5 -DADXL356=3,-6,4,0 -DHMC=PA,4,1,5,-2 -DPWR=PD,4 -DBATON=PD,7 -DLTC2942=I2C0,ALT2 -DDEBUG -DTURBO=UART2,INDIC -Wall -MD -MP -MF "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT ${OBJECTDIR}/_ext/761250667/usarts.o  -o ${OBJECTDIR}/_ext/761250667/usarts.o ../_Lib/usarts.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/default/58f4f077ea4cc5e4dacf17a331b8dbe00fe4e1b3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DADDRESS_PROCESSOR=3 -DCLOCK=CLKHT,TA0,CLKOUT -DDIOD=PF,2,INV -DGK=CMP0,AINP1,TA1 -DUSEUSART2=DEF0,255,TB0 -DUSEUSART3=DEF0,200,SPI_MODE -DMX25L256=SP1,ALT1,PF,4,UART2 -DADS131=UART3,PB,4,PB,3,PB,5 -DADXL356=3,-6,4,0 -DHMC=PA,4,1,5,-2 -DPWR=PD,4 -DBATON=PD,7 -DLTC2942=I2C0,ALT2 -DDEBUG -DTURBO=UART2,INDIC -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
else
${OBJECTDIR}/_ext/761250667/usarts.o: ../_Lib/usarts.cpp  .generated_files/flags/default/b41c0ff081c39b0629e1bf741367658c98c76485 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/761250667" 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o.d 
	@${RM} ${OBJECTDIR}/_ext/761250667/usarts.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DADDRESS_PROCESSOR=3 -DCLOCK=CLKHT,TA0,CLKOUT -DDIOD=PF,2,INV -DGK=CMP0,AINP1,TA1 -DUSEUSART2=DEF0,255,TB0 -DUSEUSART3=DEF0,200,SPI_MODE -DMX25L256=SP1,ALT1,PF,4,UART2 -DADS131=UART3,PB,4,PB,3,PB,5 -DADXL356=3,-6,4,0 -DHMC=PA,4,1,5,-2 -DPWR=PD,4 -DBATON=PD,7 -DLTC2942=I2C0,ALT2 -DDEBUG -DTURBO=UART2,INDIC -Wall -MD -MP -MF "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT "${OBJECTDIR}/_ext/761250667/usarts.o.d" -MT ${OBJECTDIR}/_ext/761250667/usarts.o  -o ${OBJECTDIR}/_ext/761250667/usarts.o ../_Lib/usarts.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
${OBJECTDIR}/main.o: main.cpp  .generated_files/flags/default/5ed336ba1b933482392586bbbc2167d43797adad .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=avr128db48 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -I"_I" -I"../_Inc" -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DF_CPU=8000000UL -DADDRESS_PROCESSOR=3 -DCLOCK=CLKHT,TA0,CLKOUT -DDIOD=PF,2,INV -DGK=CMP0,AINP1,TA1 -DUSEUSART2=DEF0,255,TB0 -DUSEUSART3=DEF0,200,SPI_MODE -DMX25L256=SP1,ALT1,PF,4,UART2 -DADS131=UART3,PB,4,PB,3,PB,5 -DADXL356=3,-6,4,0 -DHMC=PA,4,1,5,-2 -DPWR=PD,4 -DBATON=PD,7 -DLTC2942=I2C0,ALT2 -DDEBUG -DTURBO=UART2,INDIC -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -fno-strict-aliasing
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=avr128db48 ${PACK_COMMON_OPTIONS}   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\GKI.X.${IMAGE_TYPE}.map"   -Xlinker -T ../avrxmega4.x  -Wl,-section-start=.text=0x2000  -o ${DISTDIR}/GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	
	${MP_CC_DIR}\\avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/GKI.X.${IMAGE_TYPE}.eep" || exit 0
	${MP_CC_DIR}\\avr-objdump -h -S "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "${DISTDIR}/GKI.X.${IMAGE_TYPE}.lss"
	${MP_CC_DIR}\\avr-objcopy -O srec -R .eeprom -R .fuse -R .lock -R .signature "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/GKI.X.${IMAGE_TYPE}.srec"
	${MP_CC_DIR}\\avr-objcopy -j .user_signatures --set-section-flags=.user_signatures=alloc,load --change-section-lma .user_signatures=0 --no-change-warnings -O ihex "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/GKI.X.${IMAGE_TYPE}.usersignatures" || exit 0
	
else
${DISTDIR}/GKI.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=avr128db48 ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\GKI.X.${IMAGE_TYPE}.map"   -Xlinker -T ../avrxmega4.x  -Wl,-section-start=.text=0x2000  -o ${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	${MP_CC_DIR}\\avr-objcopy -O ihex "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/GKI.X.${IMAGE_TYPE}.hex"
	${MP_CC_DIR}\\avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/GKI.X.${IMAGE_TYPE}.eep" || exit 0
	${MP_CC_DIR}\\avr-objdump -h -S "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "${DISTDIR}/GKI.X.${IMAGE_TYPE}.lss"
	${MP_CC_DIR}\\avr-objcopy -O srec -R .eeprom -R .fuse -R .lock -R .signature "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/GKI.X.${IMAGE_TYPE}.srec"
	${MP_CC_DIR}\\avr-objcopy -j .user_signatures --set-section-flags=.user_signatures=alloc,load --change-section-lma .user_signatures=0 --no-change-warnings -O ihex "${DISTDIR}/GKI.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/GKI.X.${IMAGE_TYPE}.usersignatures" || exit 0
	
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
