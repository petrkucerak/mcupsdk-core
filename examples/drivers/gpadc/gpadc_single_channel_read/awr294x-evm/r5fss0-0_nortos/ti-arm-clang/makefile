
export MCU_PLUS_SDK_PATH?=$(abspath ../../../../../../..)
include $(MCU_PLUS_SDK_PATH)/imports.mak
include $(MCU_PLUS_SDK_PATH)/devconfig/devconfig.mak

CG_TOOL_ROOT=$(CGT_TI_ARM_CLANG_PATH)

CC=$(CG_TOOL_ROOT)/bin/tiarmclang
LNK=$(CG_TOOL_ROOT)/bin/tiarmclang
STRIP=$(CG_TOOL_ROOT)/bin/tiarmstrip
OBJCOPY=$(CG_TOOL_ROOT)/bin/tiarmobjcopy
COV=$(CG_TOOL_ROOT)/bin/tiarmcov
PROFDATA=$(CG_TOOL_ROOT)/bin/tiarmprofdata
COVERAGE_PATH=$(abspath .)
ifeq ($(OS), Windows_NT)
	PYTHON=python
else
	PYTHON=python3
endif

PROFILE?=release
ConfigName:=$(PROFILE)

OUTNAME:=gpadc_single_channel_read.$(PROFILE).out
COREOUTNAME:=gpadc_single_channel_read.$(PROFILE).optishare.out

BOOTIMAGE_PATH=$(abspath .)
BOOTIMAGE_NAME:=gpadc_single_channel_read.$(PROFILE).appimage
BOOTIMAGE_NAME_XIP:=gpadc_single_channel_read.$(PROFILE).appimage_xip
BOOTIMAGE_NAME_QSPI:=gpadc_single_channel_read.$(PROFILE).appimage_qspi
BOOTIMAGE_NAME_SIGNED:=gpadc_single_channel_read.$(PROFILE).appimage.signed
BOOTIMAGE_RPRC_NAME:=gpadc_single_channel_read.$(PROFILE).rprc
BOOTIMAGE_RPRC_NAME_XIP:=gpadc_single_channel_read.$(PROFILE).rprc_xip
BOOTIMAGE_RPRC_NAME_TMP:=gpadc_single_channel_read.$(PROFILE).rprc_tmp
BOOTIMAGE_NAME_HS:=gpadc_single_channel_read.$(PROFILE).appimage.hs
BOOTIMAGE_NAME_QSPI_HS:=gpadc_single_channel_read.$(PROFILE).appimage_qspi.hs
BOOTIMAGE_NAME_HS_FS:=gpadc_single_channel_read.$(PROFILE).appimage.hs_fs
TARGETS := $(BOOTIMAGE_NAME)

ifeq ($(DEVICE_TYPE), HS)
   TARGETS += $(BOOTIMAGE_NAME_HS)
   TARGETS += $(BOOTIMAGE_NAME_QSPI_HS)
endif

FILES_common := \
	gpadc_single_channel_read.c \
	main.c \
	ti_drivers_config.c \
	ti_drivers_open_close.c \
	ti_board_config.c \
	ti_board_open_close.c \
	ti_dpl_config.c \
	ti_pinmux_config.c \
	ti_power_clock_config.c \

FILES_PATH_common = \
	.. \
	../../.. \
	generated \

INCLUDES_common := \
	-I${CG_TOOL_ROOT}/include/c \
	-I${MCU_PLUS_SDK_PATH}/source \
	-Igenerated \

DEFINES_common := \
	-DSOC_AWR294X \

CFLAGS_common := \
	-mcpu=cortex-r5 \
	-mfloat-abi=hard \
	-mfpu=vfpv3-d16 \
	-mthumb \
	-Wall \
	-Werror \
	-g \
	-Wno-gnu-variable-sized-type-not-at-end \
	-Wno-unused-function \

CFLAGS_cpp_common := \
	-Wno-c99-designator \
	-Wno-extern-c-compat \
	-Wno-c++11-narrowing \
	-Wno-reorder-init-list \
	-Wno-register \
	-Wno-writable-strings \
	-Wno-enum-compare \
	-Wno-reserved-user-defined-literal \
	-Wno-unused-const-variable \
	-Wno-vla-cxx-extension \
	-x c++ \

CFLAGS_debug := \
	-D_DEBUG_=1 \

CFLAGS_release := \
	-Os \

LNK_FILES_common = \
	linker.cmd \

LIBS_PATH_common = \
	-Wl,-i${MCU_PLUS_SDK_PATH}/source/kernel/nortos/lib \
	-Wl,-i${MCU_PLUS_SDK_PATH}/source/drivers/lib \
	-Wl,-i${MCU_PLUS_SDK_PATH}/source/board/lib \
	-Wl,-i${CG_TOOL_ROOT}/lib \

LIBS_common = \
	-lnortos.awr294x.r5f.ti-arm-clang.${ConfigName}.lib \
	-ldrivers.awr294x.r5f.ti-arm-clang.${ConfigName}.lib \
	-lboard.awr294x.r5f.ti-arm-clang.${ConfigName}.lib \
	-llibc.a \
	-llibsysbm.a \

LFLAGS_common = \
	-Wl,--diag_suppress=10063 \
	-Wl,--ram_model \
	-Wl,--reread_libs \
	-Wl,--gen_xml_func_hash \


LIBS_NAME = \
	nortos.awr294x.r5f.ti-arm-clang.${ConfigName}.lib \
	drivers.awr294x.r5f.ti-arm-clang.${ConfigName}.lib \
	board.awr294x.r5f.ti-arm-clang.${ConfigName}.lib \
	libc.a \
	libsysbm.a \

LIBS_PATH_NAME = \
	${MCU_PLUS_SDK_PATH}/source/kernel/nortos/lib \
	${MCU_PLUS_SDK_PATH}/source/drivers/lib \
	${MCU_PLUS_SDK_PATH}/source/board/lib \
	${CG_TOOL_ROOT}/lib \

FILES := $(FILES_common) $(FILES_$(PROFILE))
ASMFILES := $(ASMFILES_common) $(ASMFILES_$(PROFILE))
FILES_PATH := $(FILES_PATH_common) $(FILES_PATH_$(PROFILE))
CFLAGS := $(CFLAGS_common) $(CFLAGS_$(PROFILE))
ifeq ($(INSTRUMENTATION_MODE), yes)
CFLAGS += -fprofile-instr-generate -fcoverage-mapping
endif
DEFINES := $(DEFINES_common) $(DEFINES_$(PROFILE))
INCLUDES := $(INCLUDES_common) $(INCLUDE_$(PROFILE))
LIBS := $(LIBS_common) $(LIBS_$(PROFILE))
LIBS_PATH := $(LIBS_PATH_common) $(LIBS_PATH_$(PROFILE))
LFLAGS := $(LFLAGS_common) $(LFLAGS_$(PROFILE))
LNKOPTFLAGS := $(LNKOPTFLAGS_common) $(LNKOPTFLAGS_$(PROFILE))
LNK_FILES := $(LNK_FILES_common) $(LNK_FILES_$(PROFILE))

OBJDIR := obj/$(PROFILE)/
OBJS := $(FILES:%.c=%.obj)
OBJS += $(ASMFILES:%.S=%.obj)
DEPS := $(FILES:%.c=%.d)

vpath %.obj $(OBJDIR)
vpath %.c $(FILES_PATH)
vpath %.S $(FILES_PATH)
vpath %.lib $(LIBS_PATH_NAME)
vpath %.a $(LIBS_PATH_NAME)

$(OBJDIR)/%.obj %.obj: %.c
	@echo  Compiling: awr294x:r5fss0-0:nortos:ti-arm-clang $(OUTNAME): $<
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) -MMD -o $(OBJDIR)/$@ $<

$(OBJDIR)/%.obj %.obj: %.S
	@echo  Compiling: awr294x:r5fss0-0:nortos:ti-arm-clang $(LIBNAME): $<
	$(CC) -c $(CFLAGS) -o $(OBJDIR)/$@ $<

all: $(TARGETS)

SYSCFG_GEN_FILES=generated/ti_drivers_config.c generated/ti_drivers_config.h
SYSCFG_GEN_FILES+=generated/ti_drivers_open_close.c generated/ti_drivers_open_close.h
SYSCFG_GEN_FILES+=generated/ti_dpl_config.c generated/ti_dpl_config.h
SYSCFG_GEN_FILES+=generated/ti_pinmux_config.c generated/ti_power_clock_config.c
SYSCFG_GEN_FILES+=generated/ti_board_config.c generated/ti_board_config.h
SYSCFG_GEN_FILES+=generated/ti_board_open_close.c generated/ti_board_open_close.h

SYSTEM_FLAG ?= false

ifeq ($(SYSTEM_FLAG), false)
	SYSTEM_COMMAND := syscfg $(SYSCFG_GEN_FILES) $(OBJS) $(LNK_FILES) $(LIBS_NAME)
else
	SYSTEM_COMMAND := $(OBJS) $(LNK_FILES) $(LIBS_NAME)
endif

$(OUTNAME):  $(SYSTEM_COMMAND)
	@echo  .
	@echo  Linking: awr294x:r5fss0-0:nortos:ti-arm-clang $@ ...
	$(LNK) $(LNKOPTFLAGS) $(LFLAGS) $(LIBS_PATH) -Wl,-m=$(basename $@).map -o $@ $(addprefix $(OBJDIR), $(OBJS)) $(LIBS) $(LNK_FILES) -Wl,--xml_link_info=$(basename $@).lnkxml
	@echo  Linking: awr294x:r5fss0-0:nortos:ti-arm-clang $@ Done !!!
	@echo  .

coreout: $(COREOUTNAME)
$(COREOUTNAME):  $(SYSTEM_COMMAND)
	@echo  .
	@echo  Relinking Linking: awr294x:r5fss0-0:nortos:ti-arm-clang $@ with OptiShare SSO..
	$(LNK) $(LNKOPTFLAGS) $(LFLAGS) $(LIBS_PATH) -Wl,-m=$(basename $@).map -o $@ $(addprefix $(OBJDIR), $(OBJS)) $(LIBS) $(LNK_FILES) -Wl,--xml_link_info=$(basename $@).lnkxml -Wl,--import_sso=$(SSO_PATH)
	@echo  Relinking with optishare: am263px:r5fss0-0:freertos:ti-arm-clang $@ Done !!!
	@echo  .

clean:
	@echo  Cleaning: awr294x:r5fss0-0:nortos:ti-arm-clang $(OUTNAME) ...
	$(RMDIR) $(OBJDIR)
	$(RM) $(OUTNAME)
	$(RM) $(BOOTIMAGE_NAME)
	$(RM) $(BOOTIMAGE_NAME_XIP)
	$(RM) $(BOOTIMAGE_NAME_QSPI)
	$(RM) $(BOOTIMAGE_NAME_QSPI_HS)
	$(RM) $(BOOTIMAGE_NAME_SIGNED)
	$(RM) $(BOOTIMAGE_NAME_HS)
	$(RM) $(BOOTIMAGE_NAME_HS_FS)
	$(RM) $(BOOTIMAGE_RPRC_NAME)
	$(RM) $(BOOTIMAGE_RPRC_NAME_XIP)
	$(RMDIR) generated/

scrub:
	@echo  Scrubing: awr294x:r5fss0-0:nortos:ti-arm-clang gpadc_single_channel_read ...
	$(RMDIR) obj
ifeq ($(OS),Windows_NT)
	$(RM) \*.out
	$(RM) \*.map
	$(RM) \*.appimage*
	$(RM) \*.rprc*
	$(RM) \*.tiimage*
	$(RM) \*.bin
	$(RM) \*.lnkxml
	$(RM) \*.ossr
else
	$(RM) *.out
	$(RM) *.map
	$(RM) *.appimage*
	$(RM) *.rprc*
	$(RM) *.tiimage*
	$(RM) *.bin
	$(RM) *.lnkxml
	$(RM) *.ossr
endif
	$(RMDIR) generated

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	$(MKDIR) $@


.NOTPARALLEL:

.INTERMEDIATE: syscfg
$(SYSCFG_GEN_FILES): syscfg

ifeq ($(SYSTEM_FLAG), false)
syscfg: ../example.syscfg
	@echo Generating SysConfig files ...
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(SYSCFG_SDKPRODUCT) --context r5fss0-0 --part Default --package ETS --output generated/ ../example.syscfg
endif

syscfg-gui:
	$(SYSCFG_NWJS) $(SYSCFG_PATH) --product $(SYSCFG_SDKPRODUCT) --device AWR294X --context r5fss0-0 --part Default --package ETS --output generated/  ../example.syscfg

#
# Generation of boot image which can be loaded by Secondary Boot Loader (SBL)
#
ifeq ($(OS),Windows_NT)
EXE_EXT=.exe
endif
ifeq ($(OS),Windows_NT)
  BOOTIMAGE_CERT_GEN_CMD=powershell -executionpolicy unrestricted -command $(MCU_PLUS_SDK_PATH)/source/security/security_common/tools/boot/signing/x509CertificateGen.ps1
else
  BOOTIMAGE_CERT_GEN_CMD=$(MCU_PLUS_SDK_PATH)/source/security/security_common/tools/boot/signing/x509CertificateGen.sh
endif
BOOTIMAGE_TEMP_OUT_FILE=temp_stdout_$(PROFILE).txt


BOOTIMAGE_CORE_ID_r5fss0-0 = 0
BOOTIMAGE_CORE_ID_r5fss0-1 = 1
BOOTIMAGE_CORE_ID_c66ss0   = 2
BOOTIMAGE_CORE_ID_r4       = 3
R4_RPRC_PATH = $(MCU_PLUS_SDK_PATH)/source/drivers/bootloader/soc/awr294x/xwr2xxx_radarss_metarprc.bin
SBL_RUN_ADDRESS=0x10200000
SBL_DEV_ID=55

MULTI_CORE_IMAGE_GEN = $(SYSCFG_NODE) $(MCU_PLUS_SDK_PATH)/tools/boot/multicoreImageGen/multicoreImageGen.js
OUTRPRC_CMD = $(SYSCFG_NODE) $(MCU_PLUS_SDK_PATH)/tools/boot/out2rprc/elf2rprc.js
APP_IMAGE_SIGN_CMD = $(MCU_PLUS_SDK_PATH)/source/security/security_common/tools/boot/signing/mcu_appimage_x509_cert_gen.py

ifeq ($(OS),Windows_NT)
  XIPGEN_CMD=$(MCU_PLUS_SDK_PATH)/tools/boot/xipGen/xipGen.exe
else
  UNAME_S = $(shell uname -s)
  ifeq ($(UNAME_S), Darwin)
    XIPGEN_CMD=$(MCU_PLUS_SDK_PATH)/tools/boot/xipGen/xipGen.out.mac
  else
    XIPGEN_CMD=$(MCU_PLUS_SDK_PATH)/tools/boot/xipGen/xipGen.out
endif
endif

MULTI_CORE_IMAGE_PARAMS = \
	$(BOOTIMAGE_RPRC_NAME)@$(BOOTIMAGE_CORE_ID_r5fss0-0) \

MULTI_CORE_IMAGE_PARAMS_XIP = \
	$(BOOTIMAGE_RPRC_NAME_XIP)@$(BOOTIMAGE_CORE_ID_r5fss0-0) \

MULTI_CORE_IMAGE_PARAMS_QSPI = $(MULTI_CORE_IMAGE_PARAMS) $(R4_RPRC_PATH)@$(BOOTIMAGE_CORE_ID_r4)

$(BOOTIMAGE_NAME): $(OUTNAME)
	@echo  Boot image: awr294x:r5fss0-0:nortos:ti-arm-clang $(BOOTIMAGE_PATH)/$@ ...
ifneq ($(OS),Windows_NT)
	$(CHMOD) a+x $(XIPGEN_CMD)
endif
	$(OUTRPRC_CMD) $(OUTNAME) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(COPY) $(BOOTIMAGE_RPRC_NAME) $(BOOTIMAGE_RPRC_NAME_TMP)
	$(RM) $(BOOTIMAGE_RPRC_NAME)
	$(XIPGEN_CMD) -i $(BOOTIMAGE_RPRC_NAME_TMP) -o $(BOOTIMAGE_RPRC_NAME) -x $(BOOTIMAGE_RPRC_NAME_XIP) --flash-start-addr 0xc6000000 -v > $(BOOTIMAGE_TEMP_OUT_FILE)
	$(MULTI_CORE_IMAGE_GEN) --devID $(SBL_DEV_ID) --out $(BOOTIMAGE_NAME) $(MULTI_CORE_IMAGE_PARAMS) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(MULTI_CORE_IMAGE_GEN) --devID $(SBL_DEV_ID) --out $(BOOTIMAGE_NAME_XIP) $(MULTI_CORE_IMAGE_PARAMS_XIP) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(MULTI_CORE_IMAGE_GEN) --devID $(SBL_DEV_ID) --out $(BOOTIMAGE_NAME_QSPI) $(MULTI_CORE_IMAGE_PARAMS_QSPI) >> $(BOOTIMAGE_TEMP_OUT_FILE)
	$(RM) $(BOOTIMAGE_RPRC_NAME_TMP)
	$(RM) $(BOOTIMAGE_TEMP_OUT_FILE)
	@echo  Boot image: awr294x:r5fss0-0:nortos:ti-arm-clang $(BOOTIMAGE_PATH)/$@ Done !!!
$(BOOTIMAGE_NAME_HS): $(BOOTIMAGE_NAME)
ifeq ($(DEVICE_TYPE), HS)
# Sign the appimage using appimage signing script
ifeq ($(ENC_ENABLED),no)
	@echo Boot image signing: Encryption is disabled.
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME) --key $(APP_SIGNING_KEY) --output $(BOOTIMAGE_NAME_HS)
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME_QSPI) --key $(APP_SIGNING_KEY) --output $(BOOTIMAGE_NAME_QSPI_HS)
else
	@echo Boot image signing: Encryption is enabled.
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME) --key $(APP_SIGNING_KEY) --enc y --enckey $(APP_ENCRYPTION_KEY) --kd-salt $(KD_SALT) --output $(BOOTIMAGE_NAME_HS)
	$(PYTHON) $(APP_IMAGE_SIGN_CMD) --bin $(BOOTIMAGE_NAME_QSPI) --key $(APP_SIGNING_KEY) --enc y --enckey $(APP_ENCRYPTION_KEY) --kd-salt $(KD_SALT) --output $(BOOTIMAGE_NAME_QSPI_HS)

	$(RM) $(BOOTIMAGE_NAME)-enc
	$(RM) $(BOOTIMAGE_NAME_QSPI)-enc
endif
	@echo  Boot image: awr294x:r5fss0-0:nortos:ti-arm-clang $(BOOTIMAGE_PATH)/$(BOOTIMAGE_NAME_HS) Done !!!
	@echo  Boot image: awr294x:r5fss0-0:nortos:ti-arm-clang $(BOOTIMAGE_PATH)/$(BOOTIMAGE_NAME_QSPI_HS) Done !!!
	@echo  .
endif



.PHONY: coverage
coverage:
	@echo Creating Coverage Report for gpadc_single_channel_read.$(PROFILE) ...
	$(MKDIR) coverage
	$(PROFDATA) merge -sparse -obj-file=$(OUTNAME) $(OUTNAME).cnt -o gpadc_single_channel_read.$(PROFILE).profdata
	$(COV) show --format=html --show-expansions --show-instantiations --show-branches=count --object=$(OUTNAME) -instr-profile=gpadc_single_channel_read.$(PROFILE).profdata --output-dir=$(COVERAGE_PATH)/coverage --ignore-filename-regex=build_jenkins
	$(COV) export --format=text --object=$(OUTNAME) --instr-profile=gpadc_single_channel_read.$(PROFILE).profdata > coverage/gpadc_single_channel_read.$(PROFILE).profdata.json
	node $(MCU_PLUS_SDK_PATH)/tools/smart_placement/clang_coverage_analyse.js --input=coverage/gpadc_single_channel_read.$(PROFILE).profdata.json --output-json=coverage/gpadc_single_channel_read.$(PROFILE).analysis.json --output=../gpadc_single_channel_read.annotations.$(PROFILE).S --top-function-count=500
	@echo Coverage Report Generated at $(COVERAGE_PATH)/coverage folder !!!

-include $(addprefix $(OBJDIR)/, $(DEPS))
