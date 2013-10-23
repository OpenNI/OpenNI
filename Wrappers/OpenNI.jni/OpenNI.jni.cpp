/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#include <jni.h>
#include "org_openni_NativeMethods.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#define DEBUG 1

#if DEBUG && defined(ANDROID)
#include <android/log.h>
#  define  LOGD(x...)  __android_log_print(ANDROID_LOG_INFO,"OpenNIJNI",x)
#  define  LOGE(x...)  __android_log_print(ANDROID_LOG_ERROR,"OpenNIJNI",x)
#else
#  define  LOGD(...)
#  define  LOGE(...)
#endif

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

extern JavaVM* g_pVM;

static const char *classPathName = "org/openni/NativeMethods";

#include "methods.inl"

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0]))) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

JNIEXPORT
jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    LOGD("enter JNI_OnLoad()");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        LOGE("ERROR: GetEnv failed");
        return -1;
    }
    
    if (!registerNatives(uenv.env))
    {
        LOGE("ERROR: registerNatives failed");
        return -1;
    }

	g_pVM = vm;

    LOGD("JNI_OnLoad() complete!");
    return JNI_VERSION_1_4;
}

JNIEXPORT
void JNICALL JNI_OnUnload(JavaVM * /*vm*/, void * /*reserved*/)
{
	g_pVM = NULL;
}

