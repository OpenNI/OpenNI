APP_STL := gnustl_static

# Android >= v2.3
APP_PLATFORM := android-15

# Build ARMv7-A (armeabi-v7a) machine code as default. But I had to modify it to compile for a x86 android target
APP_ABI := x86
#APP_CFLAGS := -O3 -ftree-vectorize -ffast-math -funroll-loops

APP_CFLAGS += -fPIC

ifeq ($(APP_ABI),armeabi-v7a)
	APP_CFLAGS += -march=armv7-a -mfloat-abi=softfp -mtune=cortex-a9 -mfp=vfpv3-d16 -mfpu=vfp

# optionally add NEON to compilation flags.
# to activate, run: "ndk-build USE_NEON=1"
ifdef USE_NEON
$(call __ndk_info,Building everything with NEON support!)
	APP_CFLAGS += -mfpu=neon -DHAVE_NEON=1 -flax-vector-conversions
endif
endif

APP_CPPFLAGS += -frtti

#$(call __ndk_info,APP_CFLAGS=$(APP_CFLAGS))
#$(call __ndk_info,APP_CPPFLAGS=$(APP_CPPFLAGS))

#-fsingle-precision-constant
