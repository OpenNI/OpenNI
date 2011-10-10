#############################################################################
# OpenNI makefile.
# 
# default configuration is Release. for a debug version use:
# 	make CFG=Debug
#
# default compiler is g++. for another one use:
#   make CXX=<comp>
#
#############################################################################

# default config is Release
ifndef CFG
	CFG = Release
endif

# default staging dir
ifndef STAGING_DIR
	STAGING_DIR = ../../../../LinuxARM_Staging
endif

# tools
CP = cp
RM = rm

# Bin directory
BIN_DIR = ../Bin
# output directory
OUT_DIR = $(BIN_DIR)/$(CFG)

# list all modules
ALL_MODULES = \
	Modules/nimCodecs \
	Modules/nimMockNodes \
	Modules/nimRecorder \

# list all utils
ALL_UTILS = \
	Utils/niReg \
	Utils/niLicense

# list all core projects
ALL_CORE_PROJS = \
	OpenNI \
	$(ALL_MODULES) \
	$(ALL_UTILS)

# list all samples
ALL_SAMPLES = \
	Samples/NiSimpleRead \
	Samples/NiBackRecorder \
	Samples/NiConvertXToONI \
	Samples/NiSampleModule \
	Samples/NiSimpleCreate \
	Samples/NiCRead \
	Samples/NiRecordSynthetic \


# list all projects
ALL_PROJS = \
	$(ALL_CORE_PROJS) \
	$(ALL_SAMPLES)

ALL_PROJS_CLEAN = $(foreach proj,$(ALL_PROJS),$(proj)-clean)

# define a function which creates a target for each proj
define CREATE_PROJ_TARGET
$1: 
	$(MAKE) -C $1 CFG=$(CFG)

$1-clean: 
	$(MAKE) -C $1 CFG=$(CFG) clean
endef

################ TARGETS ##################

.PHONY: all $(ALL_PROJS) $(ALL_PROJS_CLEAN) install uninstall clean 

# make all makefiles
all: $(ALL_PROJS)

# create projects targets
$(foreach proj,$(ALL_PROJS),$(eval $(call CREATE_PROJ_TARGET,$(proj))))

# additional dependencies
Modules/nimCodecs: OpenNI
Modules/nimMockNodes: OpenNI
Modules/nimRecorder: OpenNI
Utils/niReg: OpenNI
Utils/niLicense: OpenNI
Samples/NiSimpleRead: OpenNI
Samples/NiBackRecorder: OpenNI
Samples/NiConvertXToONI: OpenNI
Samples/NiSampleModule: OpenNI
Samples/NiSimpleCreate: OpenNI
Samples/NiCRead: OpenNI

# clean is cleaning all projects
clean: $(ALL_PROJS_CLEAN)

# redist target
redist: all
	cd ../CreateRedist; ./RedistMaker; cd -

# install target
install: redist
	cd ../Redist; ./install.sh -i $(STAGING_DIR); cd -

# uninstall target
uninstall:
	cd ../Redist; ./install.sh -u $(STAGING_DIR); cd -

