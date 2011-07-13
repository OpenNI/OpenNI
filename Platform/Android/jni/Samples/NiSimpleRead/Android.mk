# OpenNI Samples Android makefile.
# Sample-SimpleRead
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= ../../../../../Samples/NiSimpleRead/NiSimpleRead.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../Include/ 

LOCAL_CFLAGS:= -fPIC -fvisibility=hidden -DXN_EXPORTS

LOCAL_CFLAGS += -D__arm__ -Dandroid

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp -mfpu=neon -DANDROID -flax-vector-conversions
endif

#LOCAL_CPPFLAGS += -frtti

LOCAL_SHARED_LIBRARIES := libc libusb libOpenNI

LOCAL_MODULE := Sample-SimpleRead

include $(BUILD_EXECUTABLE)
