# OpenNI Android makefile.
# libusb.so
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
                  libusb/core.c \
                  libusb/descriptor.c \
                  libusb/io.c \
                  libusb/sync.c \
                  libusb/os/linux_usbfs.c


LOCAL_C_INCLUDES += \
                    $(LOCAL_PATH)/libusb/ \
                    $(LOCAL_PATH)/libusb/os/ \

LOCAL_PREBUILT_LIBS :=libc

LOCAL_CFLAGS:= -fPIC

LOCAL_CFLAGS += -D__arm__ -Dandroid

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp -mfpu=neon -DANDROID -flax-vector-conversions
endif

#LOCAL_CPPFLAGS += -frtti

LOCAL_MODULE:= libusb

include $(BUILD_SHARED_LIBRARY)
