#
# Generated Makefile - do not edit!
#
#
# This file contains information about the location of compilers and other tools.
# If you commmit this file into your revision control server, you will be able to 
# to checkout the project and build it from the command line with make. However,
# if more than one person works on the same project, then this file might show
# conflicts since different users are bound to have compilers in different places.
# In that case you might choose to not commit this file and let MPLAB X recreate this file
# for each user. The disadvantage of not commiting this file is that you must run MPLAB X at
# least once so the file gets created and the project can be built. Finally, you can also
# avoid using this file at all if you are only building from the command line with make.
# You can invoke make with the values of the macros:
# $ makeMP_CC="/opt/microchip/mplabc30/v3.30c/bin/pic30-gcc" ...  
#
SHELL=cmd.exe
PATH_TO_IDE_BIN=D:/IDEs/MPLab/mplab_platform/platform/../mplab_ide/modules/../../bin/
# Adding MPLAB X bin directory to path.
PATH:=D:/IDEs/MPLab/mplab_platform/platform/../mplab_ide/modules/../../bin/:$(PATH)
# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH="D:\IDEs\MPLab\sys\java\zulu8.80.0.17-ca-fx-jre8.0.422-win_x64/bin/"
OS_CURRENT="$(shell uname -s)"
MP_CC="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe"
MP_CPPC="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe"
# MP_BC is not defined
MP_AS="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-as.exe"
MP_LD="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-ld.exe"
MP_AR="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc-ar.exe"
DEP_GEN=${MP_JAVA_PATH}java -jar "D:/IDEs/MPLab/mplab_platform/platform/../mplab_ide/modules/../../bin/extractobjectdependencies.jar"
MP_CC_DIR="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin"
MP_CPPC_DIR="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin"
# MP_BC_DIR is not defined
MP_AS_DIR="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin"
MP_LD_DIR="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin"
MP_AR_DIR="D:\IDEs\ArmelStudio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin"
DFP_DIR=D:/IDEs/MPLab/packs/Microchip/AVR-Dx_DFP/2.7.321
