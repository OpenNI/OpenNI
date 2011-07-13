# OpenNI Android makefile.
# niReg
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= ../../../../../Source/Utils/niReg/niReg.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../Include

LOCAL_CFLAGS:=-fPIC -fvisibility=hidden -DXN_FORMATS_EXPORTS 

LOCAL_CFLAGS += -D__arm__ -Dandroid

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp -mfpu=neon -DANDROID -flax-vector-conversions
endif

#LOCAL_CPPFLAGS += -frtti

LOCAL_MODULE := niReg

LOCAL_SHARED_LIBRARIES := libusb libOpenNI 

include $(BUILD_EXECUTABLE)
