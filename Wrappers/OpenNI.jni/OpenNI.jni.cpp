/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
#include <jni.h>
#include "org_OpenNI_NativeMethods.h"

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

static const char *classPathName = "org/OpenNI/NativeMethods";

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

