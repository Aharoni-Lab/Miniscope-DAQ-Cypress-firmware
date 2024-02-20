## Copyright Cypress Semiconductor Corporation, 2016-2017,
## All Rights Reserved
## UNPUBLISHED, LICENSED SOFTWARE.
##
## CONFIDENTIAL AND PROPRIETARY INFORMATION
## WHICH IS THE PROPERTY OF CYPRESS.
##
## Use of this file is governed
## by the license agreement included in the file
##
##      <install>/license/license.txt
##
## where <install> is the Cypress software
## installation root directory path.
##

ifeq ($(FX3FWROOT),)
	FX3FWROOT = ../../..
endif

ifeq ($(CYCONFOPT),)
	CYCONFOPT=fx3_release
endif

all:compile

include $(FX3FWROOT)/fw_build/fx3_fw/fx3_build_config.mak

ifeq ($(MS_FW_VARIANT),128k)
# remember to also pass `-i2cconf 0x1E` to `elf2img`!
CFLAGS += -Wl,--entry,CyU3PFirmwareEntry -Xlinker --gc-sections
LDFLAGS += -d --gc-sections
endif

MODULE = cyfx_uvc_an75779

SOURCE= uvc.c 			\
	miniscope.c		\
	cyfxuvcdscr.c		\
	cyfxtx.c

ifeq ($(CYFXBUILD),arm)
SOURCE_ASM=cyfx_startup.S
else
SOURCE_ASM=cyfx_gcc_startup.S
endif

C_OBJECT=$(SOURCE:%.c=./%.o)
A_OBJECT=$(SOURCE_ASM:%.S=./%.o)

EXES = $(MODULE).$(EXEEXT)

$(MODULE).$(EXEEXT): $(A_OBJECT) $(C_OBJECT)
	$(LINK)

cyfxtx.c:
	cp $(FX3FWROOT)/fw_build/fx3_fw/cyfxtx.c .

cyfx_startup.S:
	cp $(FX3FWROOT)/fw_build/fx3_fw/cyfx_startup.S .

cyfx_gcc_startup.S:
	cp $(FX3FWROOT)/fw_build/fx3_fw/cyfx_gcc_startup.S .

$(C_OBJECT) : %.o : %.c
	$(COMPILE)

$(A_OBJECT) : %.o : %.S
	$(ASSEMBLE)

clean:
	rm -f ./$(MODULE).$(EXEEXT)
	rm -f ./$(MODULE).map
	rm -f ./*.o
	rm -f cyfxtx.c cyfx_startup.S cyfx_gcc_startup.S

compile: $(C_OBJECT) $(A_OBJECT) $(EXES)

#[]#
