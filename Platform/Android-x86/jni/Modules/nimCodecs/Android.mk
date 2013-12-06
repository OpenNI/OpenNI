# OpenNI Android makefile.
# libnimCodecs.so
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS) 

# set path to source
MY_PREFIX := $(LOCAL_PATH)/../../../../../Source/

# list all source files
MY_SRC_FILES := \
	$(MY_PREFIX)Modules/nimCodecs/*.cpp \
	$(MY_PREFIX)../Externals/LibJPEG/*.c

# expand the wildcards
MY_SRC_FILE_EXPANDED := $(wildcard $(MY_SRC_FILES))

# make those paths relative to here
LOCAL_SRC_FILES := $(MY_SRC_FILE_EXPANDED:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/../../../../../Externals/LibJPEG

LOCAL_CFLAGS := -fvisibility=hidden

LOCAL_SHARED_LIBRARIES := OpenNI 

LOCAL_MODULE := nimCodecs

include $(BUILD_SHARED_LIBRARY)

