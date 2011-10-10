###################
#      NOTE       #
###################
# if you change the flags here, you might want/need to change them in the libOpenNI.so as well.


# Build ARMv7-A machine code.
APP_ABI := armeabi-v7a
APP_CFLAGS := -O3 -ftree-vectorize -ffast-math 

APP_CFLAGS += -fPIC
APP_CFLAGS += -D__arm__ -Dandroid

ifeq ($(APP_ABI),armeabi-v7a)
	APP_CFLAGS += -march=armv7-a -mfloat-abi=softfp -DANDROID

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
