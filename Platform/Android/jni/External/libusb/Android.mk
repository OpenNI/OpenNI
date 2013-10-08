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
                  libusb/os/linux_usbfs.c \
                  libusb/os/threads_posix.c

LOCAL_C_INCLUDES += \
                    $(LOCAL_PATH)/libusb/ \
                    $(LOCAL_PATH)/libusb/os/ \

LOCAL_PREBUILT_LIBS := libc

LOCAL_CFLAGS := -fvisibility=default # just a reminder. if it's "hidden" - it doesn't link!

LOCAL_LDLIBS := -llog

LOCAL_MODULE := libusb

include $(BUILD_SHARED_LIBRARY)
