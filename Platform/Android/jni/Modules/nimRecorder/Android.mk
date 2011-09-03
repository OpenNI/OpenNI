# OpenNI Android makefile.
# libnimRecorder.so
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS) 

# set path to source
MY_PREFIX := $(LOCAL_PATH)/../../../../../Source/Modules/

# list all source files
MY_SRC_FILES := \
	$(MY_PREFIX)/nimRecorder/*.cpp \
	$(MY_PREFIX)/Common/*.cpp

# expand the wildcards
MY_SRC_FILE_EXPANDED := $(wildcard $(MY_SRC_FILES))

# make those paths relative to here
LOCAL_SRC_FILES := $(MY_SRC_FILE_EXPANDED:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../../Include \
	$(LOCAL_PATH)/../../../../../Source/Modules/Common

LOCAL_CFLAGS := -fPIC -fvisibility=hidden

LOCAL_CFLAGS += -D__arm__ -Dandroid

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp -mfpu=neon -DANDROID -flax-vector-conversions
endif

LOCAL_CPPFLAGS += -frtti

LOCAL_SHARED_LIBRARIES := libdl libc libusb

LOCAL_SHARED_LIBRARIES := libOpenNI 

LOCAL_MODULE:= nimRecorder

include $(BUILD_SHARED_LIBRARY)

