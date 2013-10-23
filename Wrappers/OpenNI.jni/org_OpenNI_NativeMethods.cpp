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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "org_openni_NativeMethods.h"
#include <XnOpenNI.h>
#include <XnCppWrapper.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
JavaVM* g_pVM = NULL;

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class JavaString
{
public:
	JavaString(JNIEnv *env, jstring str) : m_env(env), m_str(str), m_utfString(env->GetStringUTFChars(str, NULL)) 
	{
	}

	~JavaString()
	{
		m_env->ReleaseStringUTFChars(m_str, m_utfString);
	}

	operator const char*()
	{
		return m_utfString;
	}

private:
	JNIEnv* m_env;
	jstring m_str;
	const char* m_utfString;
};

class JNIEnvSupplier
{
public:
	JNIEnvSupplier() : m_pEnv(NULL), m_bShouldDetach(FALSE)
	{
		if (JNI_EDETACHED == g_pVM->GetEnv((void**)&m_pEnv, JNI_VERSION_1_2))
		{
			g_pVM->AttachCurrentThread((void**)&m_pEnv, NULL);
			m_bShouldDetach = TRUE;
		}
	}

	~JNIEnvSupplier()
	{
		if (m_bShouldDetach)
		{
			g_pVM->DetachCurrentThread();
		}
	}

	JNIEnv* GetEnv() { return m_pEnv; }

private:
	JNIEnv* m_pEnv;
	XnBool m_bShouldDetach;
};

class CallbackCookie
{
public:
	CallbackCookie(JNIEnv *env, jobject obj, jstring cb, const char* sig)
	{
		this->cls = env->GetObjectClass(obj);
		this->mid = env->GetMethodID(cls, JavaString(env, cb), sig);
		this->env = env;
		this->obj = env->NewGlobalRef(obj);
		this->mid = mid;
	}

	~CallbackCookie()
	{
		env->DeleteGlobalRef(obj);
	}

	JNIEnv* env;
	jclass cls; // need to store class so that method ID is still valid
	jobject obj;
	jmethodID mid;
	XnCallbackHandle hCallback;
};

class StateChangedCallbackCookie : public CallbackCookie
{
public:
	StateChangedCallbackCookie(JNIEnv *env, jobject obj, jstring cb) : CallbackCookie(env, obj, cb, "()V") {}
};

//---------------------------------------------------------------------------
// Marshaling
//---------------------------------------------------------------------------
JNIEXPORT jbyte JNICALL 
Java_org_openni_NativeMethods_readByte(JNIEnv *, jclass, jlong ptr)
{
	return *(jbyte*)ptr;
}

JNIEXPORT jshort JNICALL 
Java_org_openni_NativeMethods_readShort(JNIEnv *, jclass, jlong ptr)
{
	return *(jshort*)ptr;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_readInt(JNIEnv *, jclass, jlong ptr)
{
	return *(jint*)ptr;
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_readLong(JNIEnv *, jclass, jlong ptr)
{
	return *(jlong*)ptr;
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_copyToBuffer(JNIEnv *env, jclass, jobject buffer, jlong ptr, jint size)
{
	void* pDest = env->GetDirectBufferAddress(buffer);
	memcpy(pDest, (const void*)ptr, size);
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_createProductionNodeDescription(JNIEnv *env, jclass /*cls*/, jint type, jstring vendor, jstring name, jbyte major, jbyte minor, jshort maintenance, jint build)
{
	XnProductionNodeDescription* pDesc = (XnProductionNodeDescription*)xnOSMalloc(sizeof(XnProductionNodeDescription));
	if (pDesc == NULL)
		return 0;

	pDesc->Type = type;
	strcpy(pDesc->strVendor, JavaString(env, vendor));
	strcpy(pDesc->strName, JavaString(env, name));
	pDesc->Version.nMajor = major;
	pDesc->Version.nMinor = minor;
	pDesc->Version.nMaintenance = maintenance;
	pDesc->Version.nBuild = build;

	return (jlong)pDesc;
}

JNIEXPORT void JNICALL 
Java_org_openni_NativeMethods_freeProductionNodeDescription(JNIEnv* /*env*/, jclass /*cls*/, jlong pNative)
{
	xnOSFree((void*)pNative);
}

void CopyOutputMetaData(JNIEnv* env, jobject metaData, const XnOutputMetaData* pNative)
{
	jclass cls = env->FindClass("org/openni/OutputMetaData");
	jmethodID midTimestamp = env->GetMethodID(cls, "setTimestamp", "(J)V");
	jmethodID midFrameID = env->GetMethodID(cls, "setFrameID", "(I)V");
	jmethodID midDataSize = env->GetMethodID(cls, "setDataSize", "(I)V");
	jmethodID midIsNew = env->GetMethodID(cls, "setIsNew", "(Z)V");

	env->CallVoidMethod(metaData, midTimestamp, pNative->nTimestamp);
	env->CallVoidMethod(metaData, midFrameID, pNative->nFrameID);
	env->CallVoidMethod(metaData, midDataSize, pNative->nDataSize);
	env->CallVoidMethod(metaData, midIsNew, (jboolean)pNative->bIsNew);
}

void CopyMapMetaData(JNIEnv* env, jobject metaData, const XnMapMetaData* pNative)
{
	CopyOutputMetaData(env, metaData, pNative->pOutput);

	jclass cls = env->FindClass("org/openni/MapMetaData");
	jmethodID midXRes = env->GetMethodID(cls, "setXRes", "(I)V");
	jmethodID midYRes = env->GetMethodID(cls, "setYRes", "(I)V");
	jmethodID midXOffset = env->GetMethodID(cls, "setXOffset", "(I)V");
	jmethodID midYOffset = env->GetMethodID(cls, "setYOffset", "(I)V");
	jmethodID midFullXRes = env->GetMethodID(cls, "setFullXRes", "(I)V");
	jmethodID midFullYRes = env->GetMethodID(cls, "setFullYRes", "(I)V");
	jmethodID midFPS = env->GetMethodID(cls, "setFPS", "(I)V");

	env->CallVoidMethod(metaData, midXRes, pNative->Res.X);
	env->CallVoidMethod(metaData, midYRes, pNative->Res.Y);
	env->CallVoidMethod(metaData, midXOffset, pNative->Offset.X);
	env->CallVoidMethod(metaData, midYOffset, pNative->Offset.Y);
	env->CallVoidMethod(metaData, midFullXRes, pNative->FullRes.X);
	env->CallVoidMethod(metaData, midFullYRes, pNative->FullRes.Y);
	env->CallVoidMethod(metaData, midFPS, pNative->nFPS);
}

void CopyDepthMetaData(JNIEnv* env, jobject metaData, const XnDepthMetaData* pNative)
{
	CopyMapMetaData(env, metaData, pNative->pMap);

	jclass cls = env->FindClass("org/openni/DepthMetaData");
	jmethodID midZRes = env->GetMethodID(cls, "setZRes", "(I)V");
	jmethodID midDataPtr = env->GetMethodID(cls, "setDataPtr", "(J)V");

	env->CallVoidMethod(metaData, midZRes, pNative->nZRes);
	env->CallVoidMethod(metaData, midDataPtr, (jlong)pNative->pData);
}

void CopySceneMetaData(JNIEnv* env, jobject metaData, const XnSceneMetaData* pNative)
{
	CopyMapMetaData(env, metaData, pNative->pMap);

	jclass cls = env->FindClass("org/openni/SceneMetaData");
	jmethodID midDataPtr = env->GetMethodID(cls, "setDataPtr", "(J)V");

	env->CallVoidMethod(metaData, midDataPtr, (jlong)pNative->pData);
}

void CopyIRMetaData(JNIEnv* env, jobject metaData, const XnIRMetaData* pNative)
{
	CopyMapMetaData(env, metaData, pNative->pMap);

	jclass cls = env->FindClass("org/openni/IRMetaData");
	jmethodID midDataPtr = env->GetMethodID(cls, "setDataPtr", "(J)V");

	env->CallVoidMethod(metaData, midDataPtr, (jlong)pNative->pData);
}

void CopyImageMetaData(JNIEnv* env, jobject metaData, const XnImageMetaData* pNative)
{
	CopyMapMetaData(env, metaData, pNative->pMap);

	jclass cls = env->FindClass("org/openni/ImageMetaData");
	jmethodID midDataPtr = env->GetMethodID(cls, "setDataPtr", "(J)V");

	env->CallVoidMethod(metaData, midDataPtr, (jlong)pNative->pData);
}

void CopyAudioMetaData(JNIEnv* env, jobject metaData, const XnAudioMetaData* pNative)
{
	CopyOutputMetaData(env, metaData, pNative->pOutput);

	jclass cls = env->FindClass("org/openni/AudioMetaData");
	jmethodID midSampleRate = env->GetMethodID(cls, "setSampleRate", "(I)V");
	jmethodID midBitsPerSample = env->GetMethodID(cls, "setBitsPerSample", "(S)V");
	jmethodID midNumberOfChannels = env->GetMethodID(cls, "setNumberOfChannels", "(B)V");
	jmethodID midDataPtr = env->GetMethodID(cls, "setDataPtr", "(J)V");

	env->CallVoidMethod(metaData, midSampleRate, pNative->Wave.nSampleRate);
	env->CallVoidMethod(metaData, midBitsPerSample, pNative->Wave.nBitsPerSample);
	env->CallVoidMethod(metaData, midNumberOfChannels, pNative->Wave.nChannels);
	env->CallVoidMethod(metaData, midDataPtr, (jlong)pNative->pData);
}

//---------------------------------------------------------------------------
// Utilities
//---------------------------------------------------------------------------
jobject CreatePoint3D(JNIEnv* pEnv, const XnPoint3D* pPoint)
{
	jclass PointCls = pEnv->FindClass("org/openni/Point3D");
	jmethodID PointCtor = pEnv->GetMethodID(PointCls, "<init>", "(FFF)V");

	return pEnv->NewObject(PointCls, PointCtor, pPoint->X, pPoint->Y, pPoint->Z);
}
void Point3DToNative(JNIEnv* env, jobject point, XnPoint3D* pPoint)
{
	jclass cls = env->FindClass("org/openni/Point3D");
	jmethodID midGetX = env->GetMethodID(cls, "getX", "()F");
	jmethodID midGetY = env->GetMethodID(cls, "getY", "()F");
	jmethodID midGetZ = env->GetMethodID(cls, "getZ", "()F");

	pPoint->X = env->CallFloatMethod(point, midGetX);
	pPoint->Y = env->CallFloatMethod(point, midGetY);
	pPoint->Z = env->CallFloatMethod(point, midGetZ);
}

void JavaPointArrayToNative(JNIEnv* env, jobjectArray javaArray, XnPoint3D* nativeArray)
{
	int count = env->GetArrayLength(javaArray);
	for (int i = 0; i < count; ++i)
	{
		jobject curr = env->GetObjectArrayElement(javaArray, i);
		Point3DToNative(env, curr, &nativeArray[i]);
	}
}

void NativePointArrayToJava(JNIEnv* env, const XnPoint3D* nativeArray, jobjectArray javaArray)
{
	int count = env->GetArrayLength(javaArray);
	for (int i = 0; i < count; ++i)
	{
		jobject curr = CreatePoint3D(env, &nativeArray[i]);
		env->SetObjectArrayElement(javaArray, i, curr);
	}
}

jobject CreateJointPosition(JNIEnv* pEnv, const XnSkeletonJointPosition* pPosition)
{
	jclass cls = pEnv->FindClass("org/openni/SkeletonJointPosition");
	jmethodID ctor = pEnv->GetMethodID(cls, "<init>", "(Lorg/openni/Point3D;F)V");

	return pEnv->NewObject(cls, ctor, CreatePoint3D(pEnv, &pPosition->position), pPosition->fConfidence);
}
jobject CreateJointOrientation(JNIEnv* pEnv, const XnSkeletonJointOrientation* pOrientation)
{
	jclass cls = pEnv->FindClass("org/openni/SkeletonJointOrientation");
	jmethodID ctor = pEnv->GetMethodID(cls, "<init>", "(FFFFFFFFFF)V");

	return pEnv->NewObject(cls, ctor, pOrientation->orientation.elements[0], pOrientation->orientation.elements[1], pOrientation->orientation.elements[2], pOrientation->orientation.elements[3], pOrientation->orientation.elements[4], pOrientation->orientation.elements[5], pOrientation->orientation.elements[6], pOrientation->orientation.elements[7], pOrientation->orientation.elements[8], pOrientation->fConfidence);
}
jobject CreateJointTransformation(JNIEnv* pEnv, const XnSkeletonJointTransformation* pTransformation)
{
	jclass cls = pEnv->FindClass("org/openni/SkeletonJointTransformation");
	jmethodID ctor = pEnv->GetMethodID(cls, "<init>", "(Lorg/openni/SkeletonJointPosition;Lorg/openni/SkeletonJointOrientation;)V");

	return pEnv->NewObject(cls, ctor, CreateJointPosition(pEnv, &pTransformation->position), CreateJointOrientation(pEnv, &pTransformation->orientation));
}

jobject CreateBoundingBox3D(JNIEnv* env, const XnBoundingBox3D* pBox)
{
	jclass cls = env->FindClass("org/openni/BoundingBox3D");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(Lorg/openni/Point3D;Lorg/openni/Point3D;)V");

	jobject leftBottomNear = CreatePoint3D(env, &pBox->LeftBottomNear);
	jobject rightTopFar = CreatePoint3D(env, &pBox->RightTopFar);
	return env->NewObject(cls, ctor, leftBottomNear, rightTopFar);
}

void SetOutArgObjectValue(JNIEnv* env, jobject p, jobject value)
{
	jclass cls = env->GetObjectClass(p);
	jfieldID fieldID = env->GetFieldID(cls, "value", "Ljava/lang/Object;");
	env->SetObjectField(p, fieldID, value);
}

void SetOutArgDoubleValue(JNIEnv* env, jobject p, double value)
{
	jclass cls = env->FindClass("java/lang/Double");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(D)V");
	SetOutArgObjectValue(env, p, env->NewObject(cls, ctor, value));
}

void SetOutArgLongValue(JNIEnv* env, jobject p, XnUInt64 value)
{
	jclass cls = env->FindClass("java/lang/Long");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(J)V");
	SetOutArgObjectValue(env, p, env->NewObject(cls, ctor, value));
}

void SetOutArgPointerValue(JNIEnv* env, jobject p, const void* value)
{
	SetOutArgLongValue(env, p, (jlong)value);
}

void SetOutArgPoint3DValue(JNIEnv* env, jobject p, const XnPoint3D* pPoint)
{
	SetOutArgObjectValue(env, p, CreatePoint3D(env, pPoint));
}

void SetOutArgBoundingBox3DValue(JNIEnv* env, jobject p, const XnBoundingBox3D* pBox)
{
	SetOutArgObjectValue(env, p, CreateBoundingBox3D(env, pBox));
}

void SetOutArgIntValue(JNIEnv* env, jobject p, int value)
{
	jclass cls = env->FindClass("java/lang/Integer");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(I)V");
	SetOutArgObjectValue(env, p, env->NewObject(cls, ctor, value));
}

void SetOutArgShortValue(JNIEnv* env, jobject p, short value)
{
	jclass cls = env->FindClass("java/lang/Short");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(S)V");
	SetOutArgObjectValue(env, p, env->NewObject(cls, ctor, value));
}

void SetOutArgByteValue(JNIEnv* env, jobject p, XnUInt8 value)
{
	jclass cls = env->FindClass("java/lang/Byte");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(B)V");
	SetOutArgObjectValue(env, p, env->NewObject(cls, ctor, value));
}

void SetOutArgBoolValue(JNIEnv* env, jobject p, jboolean value)
{
	jclass cls = env->FindClass("java/lang/Boolean");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(Z)V");
	SetOutArgObjectValue(env, p, env->NewObject(cls, ctor, value));
}

void SetOutArgStringValue(JNIEnv* env, jobject p, const XnChar* value)
{
	SetOutArgObjectValue(env, p, env->NewStringUTF(value));
}

typedef XnStatus XN_C_DECL StateChangedRegisterFunc(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
typedef void XN_C_DECL StateChangedUnregisterFunc(XnNodeHandle hInstance, XnCallbackHandle hCallback);

static void XN_CALLBACK_TYPE StateChangedHandler(XnNodeHandle /*hNode*/, void* pCookie)
{
	StateChangedCallbackCookie* pCallback = (StateChangedCallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid);
}

XnStatus RegisterToStateChangeEvent(StateChangedRegisterFunc func, JNIEnv *env, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	StateChangedCallbackCookie* pCookie = new StateChangedCallbackCookie(env, obj, cb);
	XnStatus rc = func((XnNodeHandle)hNode, StateChangedHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}

void UnregisterFromStateChangeEvent(StateChangedUnregisterFunc func, jlong hNode, jlong hCallback)
{
	StateChangedCallbackCookie* pCallback = (StateChangedCallbackCookie*)hCallback;
	func((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}

//---------------------------------------------------------------------------
// General
//---------------------------------------------------------------------------
JNIEXPORT jstring JNICALL 
Java_org_openni_NativeMethods_xnGetStatusString(JNIEnv *env, jclass /*cls*/, jint status)
{
	const XnChar* errorMessage = xnGetStatusString(status);

	return(env->NewStringUTF(errorMessage));
}

//---------------------------------------------------------------------------
// Context
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnInit(JNIEnv *env, jclass /*cls*/, jobject cxt)
{
	XnContext* pContext = NULL;
	XnStatus rc = xnInit(&pContext);
	SetOutArgPointerValue(env, cxt, pContext);
	return rc;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnContextRunXmlScriptFromFileEx(JNIEnv *env, jclass, jlong pContext, jstring fileName, jlong pErrors, jobject phScriptNode)
{
	XnNodeHandle hScript;
	XnStatus nRetVal = xnContextRunXmlScriptFromFileEx((XnContext*)pContext, JavaString(env, fileName), (XnEnumerationErrors*)pErrors, &hScript);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phScriptNode, hScript);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnContextRunXmlScriptEx(JNIEnv *env, jclass, jlong pContext, jstring script, jlong pErrors, jobject phScriptNode)
{
	XnNodeHandle hScript;
	XnStatus nRetVal = xnContextRunXmlScriptEx((XnContext*)pContext, JavaString(env, script), (XnEnumerationErrors*)pErrors, &hScript);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phScriptNode, hScript);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnInitFromXmlFileEx(JNIEnv *env, jclass /*cls*/, jstring xmlFile, jobject cxt, jlong /*errors*/, jobject scriptNode)
{
	XnContext* pContext = NULL;
	XnInternalNodeData* pScriptNode = NULL;
	XnStatus rc = xnInitFromXmlFileEx(JavaString(env, xmlFile), &pContext, NULL, &pScriptNode);

	SetOutArgPointerValue(env, cxt, pContext);
	SetOutArgPointerValue(env, scriptNode, pScriptNode);

	return rc;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnContextOpenFileRecordingEx(JNIEnv *env, jclass, jlong pContext, jstring fileName, jobject phPlayer)
{
	XnNodeHandle hPlayer;
	XnStatus nRetVal = xnContextOpenFileRecordingEx((XnContext*)pContext, JavaString(env, fileName), &hPlayer);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phPlayer, hPlayer);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnContextAddRef(JNIEnv* /*env*/, jclass /*cls*/, jlong pContext)
{
	return xnContextAddRef((XnContext*)pContext);
}

JNIEXPORT void JNICALL 
Java_org_openni_NativeMethods_xnContextRelease(JNIEnv* /*env*/, jclass /*cls*/, jlong pContext)
{
	xnContextRelease((XnContext*)pContext);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerateProductionTrees(JNIEnv *env, jclass, jlong pContext, jint type, jlong pQuery, jobject ppList, jlong pErrors)
{
	XnNodeInfoList* pList = NULL;
	XnStatus nRetVal = xnEnumerateProductionTrees((XnContext*)pContext, type, (XnNodeQuery*)pQuery, &pList, (XnEnumerationErrors*)pErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, ppList, pList);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateProductionTree(JNIEnv *env, jclass, jlong pContext, jlong pNodeInfo, jobject phNode)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateProductionTree((XnContext*)pContext, (XnNodeInfo*)pNodeInfo, &hNode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateAnyProductionTree(JNIEnv *env, jclass, jlong pContext, jint type, jlong pQuery, jobject phNode, jlong pErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateAnyProductionTree((XnContext*)pContext, type, (XnNodeQuery*)pQuery, &hNode, (XnEnumerationErrors*)pErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

static void XN_CALLBACK_TYPE ErrorStateChangedHandler(XnStatus errorState, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, errorState);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnRegisterToGlobalErrorStateChange(JNIEnv *env, jclass /*cls*/, jlong pContext, jobject obj, jstring cb, jobject hCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(I)V");
	XnStatus rc = xnRegisterToGlobalErrorStateChange((XnContext*)pContext, ErrorStateChangedHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, hCallback, pCookie);
	return rc;
}

JNIEXPORT void JNICALL 
Java_org_openni_NativeMethods_xnUnregisterFromGlobalErrorStateChange(JNIEnv* /*env*/, jclass /*cls*/, jlong pContext, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromGlobalErrorStateChange((XnContext*)pContext, pCallback->hCallback);
	delete pCallback;
}

static void XN_CALLBACK_TYPE NodeCreatedHandler(XnContext* /*pContext*/, XnNodeHandle hCreatedNode, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, hCreatedNode);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnRegisterToNodeCreation(JNIEnv *env, jclass /*cls*/, jlong pContext, jobject obj, jstring cb, jobject hCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(J)V");
	XnStatus rc = xnRegisterToNodeCreation((XnContext*)pContext, NodeCreatedHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, hCallback, pCookie);
	return rc;
}

JNIEXPORT void JNICALL 
Java_org_openni_NativeMethods_xnUnregisterFromNodeCreation(JNIEnv* /*env*/, jclass /*cls*/, jlong pContext, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromNodeCreation((XnContext*)pContext, pCallback->hCallback);
	delete pCallback;
}

static void XN_CALLBACK_TYPE NodeDestroyedHandler(XnContext* /*pContext*/, const XnChar* strDestroyedNodeName, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier supplier;
	supplier.GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, supplier.GetEnv()->NewStringUTF(strDestroyedNodeName));
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnRegisterToNodeDestruction(JNIEnv *env, jclass /*cls*/, jlong pContext, jobject obj, jstring cb, jobject hCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;)V");
	XnStatus rc = xnRegisterToNodeDestruction((XnContext*)pContext, NodeDestroyedHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, hCallback, pCookie);
	return rc;
}

JNIEXPORT void JNICALL 
Java_org_openni_NativeMethods_xnUnregisterFromNodeDestruction(JNIEnv* /*env*/, jclass /*cls*/, jlong pContext, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromNodeDestruction((XnContext*)pContext, pCallback->hCallback);
	delete pCallback;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnAddLicense(JNIEnv *env, jclass, jlong pContext, jstring vendor, jstring key)
{
	XnLicense license;
	strcpy(license.strVendor, JavaString(env, vendor));
	strcpy(license.strKey, JavaString(env, key));
	return xnAddLicense((XnContext*)pContext, &license);
}

jobject CreateLicense(JNIEnv* env, const XnLicense* pLicense)
{
	jclass cls = env->FindClass("org/openni/License");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
	
	jstring vendor = env->NewStringUTF(pLicense->strVendor);
	jstring key = env->NewStringUTF(pLicense->strKey);
	return env->NewObject(cls, ctor, vendor, key);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerateLicenses(JNIEnv *env, jclass, jlong pContext, jobject pLicenses)
{
	XnLicense* aLicenses;
	XnUInt32 nCount;
	XnStatus nRetVal = xnEnumerateLicenses((XnContext*)pContext, &aLicenses, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// create array
	jclass cls = env->FindClass("org/openni/License");
	jobjectArray licenses = env->NewObjectArray(nCount, cls, NULL);

	// fill it up
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		jobject license = CreateLicense(env, &aLicenses[i]);
		env->SetObjectArrayElement(licenses, i, license);
	}

	// now return the array
	SetOutArgObjectValue(env, pLicenses, licenses);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnStartGeneratingAll(JNIEnv *, jclass, jlong hContext)
{
	return xnStartGeneratingAll((XnContext*)hContext);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnStopGeneratingAll(JNIEnv *, jclass, jlong hContext)
{
	return xnStopGeneratingAll((XnContext*)hContext);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetGlobalMirror(JNIEnv *, jclass, jlong pContext, jboolean bMirror)
{
	return xnSetGlobalMirror((XnContext*)pContext, bMirror);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnGetGlobalMirror(JNIEnv *, jclass, jlong pContext)
{
	return (jboolean)xnGetGlobalMirror((XnContext*)pContext);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetGlobalErrorState(JNIEnv *, jclass, jlong pContext)
{
	return xnGetGlobalErrorState((XnContext*)pContext);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerateExistingNodes(JNIEnv *env, jclass, jlong pContext, jobject ppList)
{
	XnNodeInfoList* pList = NULL;
	XnStatus nRetVal = xnEnumerateExistingNodes((XnContext*)pContext, &pList);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, ppList, pList);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerateExistingNodesByType(JNIEnv *env, jclass, jlong pContext, jint type, jobject ppList)
{
	XnNodeInfoList* pList = NULL;
	XnStatus nRetVal = xnEnumerateExistingNodesByType((XnContext*)pContext, type, &pList);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, ppList, pList);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnFindExistingRefNodeByType(JNIEnv *env, jclass, jlong pContext, jint type, jobject phNode)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnFindExistingRefNodeByType((XnContext*)pContext, (XnProductionNodeType)type, &hNode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetRefNodeHandleByName(JNIEnv *env, jclass, jlong pContext, jstring name, jobject phNode)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnGetRefNodeHandleByName((XnContext*)pContext, JavaString(env, name), &hNode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnWaitAndUpdateAll(JNIEnv* /*env*/, jclass /*cls*/, jlong hContext)
{
	return xnWaitAndUpdateAll((XnContext*)hContext);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnWaitOneUpdateAll(JNIEnv* /*env*/, jclass, jlong pContext, jlong hNode)
{
	return xnWaitOneUpdateAll((XnContext*)pContext, (XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnWaitAnyUpdateAll(JNIEnv* /*env*/, jclass /*cls*/, jlong hContext)
{
	return xnWaitAnyUpdateAll((XnContext*)hContext);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnWaitNoneUpdateAll(JNIEnv* /*env*/, jclass /*cls*/, jlong hContext)
{
	return xnWaitNoneUpdateAll((XnContext*)hContext);
}

//---------------------------------------------------------------------------
// Version
//---------------------------------------------------------------------------
jobject CreateVersion(JNIEnv *env, const XnVersion* pVersion)
{
	jclass cls = env->FindClass("org/openni/Version");
	jmethodID clsCtor = env->GetMethodID(cls, "<init>", "(BBSI)V");
	return env->NewObject(cls, clsCtor, pVersion->nMajor, pVersion->nMinor, pVersion->nMaintenance, pVersion->nBuild);
}

void SetOutArgVersionValue(JNIEnv* env, jobject pOut, XnVersion* pVersion)
{
	return SetOutArgObjectValue(env, pOut, CreateVersion(env, pVersion));
}

//---------------------------------------------------------------------------
// ProductionNodeDescription
//---------------------------------------------------------------------------
jobject CreateProductionNodeDescription(JNIEnv *env, const XnProductionNodeDescription* pDesc)
{
	jclass cls = env->FindClass("org/openni/ProductionNodeDescription");
	jmethodID clsCtor = env->GetMethodID(cls, "<init>", "(ILjava/lang/String;Ljava/lang/String;Lorg/openni/Version;)V");

	jobject version = CreateVersion(env, &pDesc->Version);
	return env->NewObject(cls, clsCtor, pDesc->Type, pDesc->strVendor, pDesc->strName, version);
}

//---------------------------------------------------------------------------
// EnumerationErrors
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerationErrorsAllocate(JNIEnv *env, jclass, jobject ppErrors)
{
	XnEnumerationErrors* pErrors = NULL;
	XnStatus nRetVal = xnEnumerationErrorsAllocate(&pErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, ppErrors, pErrors);
	return XN_STATUS_OK;
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnEnumerationErrorsFree(JNIEnv *, jclass, jlong pErrors)
{
	xnEnumerationErrorsFree((XnEnumerationErrors*)pErrors);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerationErrorsToString(JNIEnv *env, jclass, jlong pErrors, jobject result)
{
	XnChar strResult[2048];
	XnStatus nRetVal = xnEnumerationErrorsToString((XnEnumerationErrors*)pErrors, strResult, 2048);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgStringValue(env, result, strResult);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerationErrorsClear(JNIEnv *, jclass, jlong pErrors)
{
	return xnEnumerationErrorsClear((XnEnumerationErrors*)pErrors);
}

JNIEXPORT jlong JNICALL Java_org_openni_NativeMethods_xnEnumerationErrorsGetFirst(JNIEnv *, jclass, jlong pErrors)
{
	return (jlong)xnEnumerationErrorsGetFirst((XnEnumerationErrors*)pErrors);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnEnumerationErrorsIteratorIsValid(JNIEnv *, jclass, jlong it)
{
	return (jboolean)xnEnumerationErrorsIteratorIsValid((XnEnumerationErrorsIterator)it);
}

//---------------------------------------------------------------------------
// NodeInfo
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoSetInstanceName(JNIEnv *env, jclass, jlong pNodeInfo, jstring name)
{
	return xnNodeInfoSetInstanceName((XnNodeInfo*)pNodeInfo, JavaString(env, name));
}

JNIEXPORT jobject JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoGetDescription(JNIEnv *env, jclass /*cls*/, jlong pNodeInfo)
{
	const XnProductionNodeDescription* pDesc = xnNodeInfoGetDescription((XnNodeInfo*)pNodeInfo);
	return CreateProductionNodeDescription(env, pDesc);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoGetTreeStringRepresentation(JNIEnv *env, jclass , jlong pNodeInfo, jobject result)
{
	XnChar strResult[4096];
	XnStatus nRetVal = xnNodeInfoGetTreeStringRepresentation((XnNodeInfo*)pNodeInfo, strResult, 4096);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgStringValue(env, result, strResult);
	return XN_STATUS_OK;
}

JNIEXPORT jstring JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoGetInstanceName(JNIEnv *env, jclass /*cls*/, jlong pNodeInfo)
{
	const XnChar* strName = xnNodeInfoGetInstanceName((XnNodeInfo*)pNodeInfo);
	return env->NewStringUTF(strName);
}

JNIEXPORT jstring JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoGetCreationInfo(JNIEnv *env, jclass /*cls*/, jlong pNodeInfo)
{
	const XnChar* strCreationInfo = xnNodeInfoGetCreationInfo((XnNodeInfo*)pNodeInfo);
	return env->NewStringUTF(strCreationInfo);
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoGetNeededNodes(JNIEnv* /*env*/, jclass /*cls*/, jlong pNodeInfo)
{
	return (jlong)xnNodeInfoGetNeededNodes((XnNodeInfo*)pNodeInfo);
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoGetRefHandle(JNIEnv* /*env*/, jclass /*cls*/, jlong pNodeInfo)
{
	return (jlong)xnNodeInfoGetRefHandle((XnNodeInfo*)pNodeInfo);
}

//---------------------------------------------------------------------------
// NodeInfoList
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListAllocate(JNIEnv *env, jclass /*cls*/, jobject ppList)
{
	XnNodeInfoList* pList = NULL;
	int rc = xnNodeInfoListAllocate(&pList);
	SetOutArgPointerValue(env, ppList, pList);
	return rc;
}

JNIEXPORT void JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListFree(JNIEnv* /*env*/, jclass /*cls*/, jlong pList)
{
	xnNodeInfoListFree((XnNodeInfoList*)pList);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListAdd(JNIEnv *env, jclass /*cls*/, jlong pList, jlong desc, jstring creationInfo, jlong pNeeded)
{
	return xnNodeInfoListAdd((XnNodeInfoList*)pList, (XnProductionNodeDescription*)desc, JavaString(env, creationInfo), (XnNodeInfoList*)pNeeded);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListAddNode(JNIEnv *, jclass, jlong pList, jlong pNode)
{
	return xnNodeInfoListAddNode((XnNodeInfoList*)pList, (XnNodeInfo*)pNode);
}

inline XnNodeInfoListIterator GetInfoListIterator(jlong it)
{
	XnNodeInfoListIterator result;
	result.pCurrent = (XnNodeInfoListNode*)it;
	return result;
}

inline jlong GetInfoListIteratorAsLong(XnNodeInfoListIterator it)
{
	return (jlong)it.pCurrent;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListAddNodeFromList(JNIEnv *, jclass, jlong pList, jlong it)
{
	return xnNodeInfoListAddNodeFromList((XnNodeInfoList*)pList, GetInfoListIterator(it));
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListRemove(JNIEnv *, jclass, jlong pList, jlong it)
{
	return xnNodeInfoListRemove((XnNodeInfoList*)pList, GetInfoListIterator(it));
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListClear(JNIEnv *, jclass, jlong pList)
{
	return xnNodeInfoListClear((XnNodeInfoList*)pList);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListAppend(JNIEnv *, jclass, jlong pList, jlong pOther)
{
	return xnNodeInfoListAppend((XnNodeInfoList*)pList, (XnNodeInfoList*)pOther);
}

JNIEXPORT jboolean JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListIsEmpty(JNIEnv *, jclass, jlong pList)
{
	return (jboolean)xnNodeInfoListIsEmpty((XnNodeInfoList*)pList);
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListGetFirst(JNIEnv *, jclass, jlong pList)
{
	return GetInfoListIteratorAsLong(xnNodeInfoListGetFirst((XnNodeInfoList*)pList));
}

JNIEXPORT jboolean JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListIteratorIsValid(JNIEnv *, jclass, jlong it)
{
	return (jboolean)xnNodeInfoListIteratorIsValid(GetInfoListIterator(it));
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListGetCurrent(JNIEnv *, jclass, jlong it)
{
	return (jlong)xnNodeInfoListGetCurrent(GetInfoListIterator(it));
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_xnNodeInfoListGetNext(JNIEnv *, jclass, jlong it)
{
	return GetInfoListIteratorAsLong(xnNodeInfoListGetNext(GetInfoListIterator(it)));
}

//---------------------------------------------------------------------------
// Query
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQueryAllocate(JNIEnv *env, jclass, jobject ppQuery)
{
	XnNodeQuery* pQuery;
	XnStatus nRetVal = xnNodeQueryAllocate(&pQuery);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, ppQuery, pQuery);
	return XN_STATUS_OK;
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnNodeQueryFree(JNIEnv *, jclass, jlong pQuery)
{
	xnNodeQueryFree((XnNodeQuery*)pQuery);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetVendor(JNIEnv *env, jclass, jlong pQuery, jstring vendor)
{
	return xnNodeQuerySetVendor((XnNodeQuery*)pQuery, JavaString(env, vendor));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetName(JNIEnv *env, jclass, jlong pQuery, jstring name)
{
	return xnNodeQuerySetName((XnNodeQuery*)pQuery, JavaString(env, name));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetMinVersion(JNIEnv *, jclass, jlong pQuery, jbyte major, jbyte minor, jshort maintenance, jint build)
{
	XnVersion version = { major, minor, maintenance, build };
	return xnNodeQuerySetMinVersion((XnNodeQuery*)pQuery, &version);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetMaxVersion(JNIEnv *, jclass, jlong pQuery, jbyte major, jbyte minor, jshort maintenance, jint build)
{
	XnVersion version = { major, minor, maintenance, build };
	return xnNodeQuerySetMaxVersion((XnNodeQuery*)pQuery, &version);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQueryAddSupportedCapability(JNIEnv *env, jclass, jlong pQuery, jstring cap)
{
	return xnNodeQueryAddSupportedCapability((XnNodeQuery*)pQuery, JavaString(env, cap));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQueryAddSupportedMapOutputMode(JNIEnv* /*env*/, jclass, jlong pQuery, jint xRes, jint yRes, jint FPS)
{
	XnMapOutputMode mode = { xRes, yRes, FPS };
	return xnNodeQueryAddSupportedMapOutputMode((XnNodeQuery*)pQuery, &mode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetSupportedMinUserPositions(JNIEnv *, jclass, jlong pQuery, jint nCount)
{
	return xnNodeQuerySetSupportedMinUserPositions((XnNodeQuery*)pQuery, nCount);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetExistingNodeOnly(JNIEnv *, jclass, jlong pQuery, jboolean bExisting)
{
	return xnNodeQuerySetExistingNodeOnly((XnNodeQuery*)pQuery, bExisting);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetNonExistingNodeOnly(JNIEnv *, jclass, jlong pQuery, jboolean bNonExisting)
{
	return xnNodeQuerySetNonExistingNodeOnly((XnNodeQuery*)pQuery, bNonExisting);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQueryAddNeededNode(JNIEnv *env, jclass, jlong pQuery, jstring nodeName)
{
	return xnNodeQueryAddNeededNode((XnNodeQuery*)pQuery, JavaString(env, nodeName));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQuerySetCreationInfo(JNIEnv *env, jclass, jlong pQuery, jstring creationInfo)
{
	return xnNodeQuerySetCreationInfo((XnNodeQuery*)pQuery, JavaString(env, creationInfo));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnNodeQueryFilterList(JNIEnv *, jclass, jlong pContext, jlong pQuery, jlong pList)
{
	return xnNodeQueryFilterList((XnContext*)pContext, (XnNodeQuery*)pQuery, (XnNodeInfoList*)pList);
}

//---------------------------------------------------------------------------
// ProductionNode
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnProductionNodeAddRef(JNIEnv *, jclass /*cls*/, jlong hNode)
{
	return xnProductionNodeAddRef((XnNodeHandle)hNode);
}

JNIEXPORT void JNICALL 
Java_org_openni_NativeMethods_xnProductionNodeRelease(JNIEnv *, jclass /*cls*/, jlong hNode)
{
	xnProductionNodeRelease((XnNodeHandle)hNode);
}

JNIEXPORT jstring JNICALL 
Java_org_openni_NativeMethods_xnGetNodeName(JNIEnv *env, jclass, jlong hNode)
{
	return env->NewStringUTF(xnGetNodeName((XnNodeHandle)hNode));
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_xnGetNodeInfo(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetNodeInfo((XnNodeHandle)hNode);
}

JNIEXPORT jlong JNICALL 
Java_org_openni_NativeMethods_xnGetRefContextFromNodeHandle(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetRefContextFromNodeHandle((XnNodeHandle)hNode);
}

JNIEXPORT jboolean JNICALL 
Java_org_openni_NativeMethods_xnIsCapabilitySupported(JNIEnv *env, jclass, jlong hNode, jstring strName)
{
	return (jboolean)xnIsCapabilitySupported((XnNodeHandle)hNode, JavaString(env, strName));
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnSetIntProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jlong value)
{
	return xnSetIntProperty((XnNodeHandle)hNode, JavaString(env, strName), value);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnSetRealProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jdouble value)
{
	return xnSetRealProperty((XnNodeHandle)hNode, JavaString(env, strName), value);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnSetStringProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jstring value)
{
	return xnSetStringProperty((XnNodeHandle)hNode, JavaString(env, strName), JavaString(env, value));
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnSetGeneralProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jint size, jlong buffer)
{
	return xnSetGeneralProperty((XnNodeHandle)hNode, JavaString(env, strName), size, (const void*)buffer);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnSetGeneralPropertyArray(JNIEnv *env, jclass, jlong hNode, jstring strName, jbyteArray buffer)
{
	jboolean isCopy;
	jbyte* cBuffer = env->GetByteArrayElements(buffer, &isCopy);
	jsize length = env->GetArrayLength(buffer);

	XnStatus rc = xnSetGeneralProperty((XnNodeHandle)hNode, JavaString(env, strName), length, cBuffer);

	env->ReleaseByteArrayElements(buffer, cBuffer, 0);

	return rc;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnGetIntProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jobject value)
{
	XnUInt64 nValue = 0;
	XnStatus rc = xnGetIntProperty((XnNodeHandle)hNode, JavaString(env, strName), &nValue);
	SetOutArgLongValue(env, value, nValue);
	return rc;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnGetRealProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jobject value)
{
	XnDouble dValue = 0;
	XnStatus rc = xnGetRealProperty((XnNodeHandle)hNode, JavaString(env, strName), &dValue);
	SetOutArgDoubleValue(env, value, dValue);
	return rc;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnGetStringProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jobject value)
{
	XnChar strValue[2048];
	XnStatus rc = xnGetStringProperty((XnNodeHandle)hNode, JavaString(env, strName), strValue, 2048);
	SetOutArgStringValue(env, value, strValue);
	return rc;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnGetGeneralProperty(JNIEnv *env, jclass, jlong hNode, jstring strName, jint size, jlong buffer)
{
	return xnGetGeneralProperty((XnNodeHandle)hNode, JavaString(env, strName), size, (void*)buffer);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnGetGeneralPropertyArray(JNIEnv *env, jclass, jlong hNode, jstring strName, jbyteArray buffer)
{
	jboolean isCopy;
	jbyte* cBuffer = env->GetByteArrayElements(buffer, &isCopy);
	jsize length = env->GetArrayLength(buffer);

	XnStatus rc = xnGetGeneralProperty((XnNodeHandle)hNode, JavaString(env, strName), length, cBuffer);

	env->ReleaseByteArrayElements(buffer, cBuffer, 0);

	return rc;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnLockNodeForChanges(JNIEnv *env, jclass, jlong hNode, jobject phLock)
{
	XnLockHandle hLock = 0;
	XnStatus rc = xnLockNodeForChanges((XnNodeHandle)hNode, &hLock);
	SetOutArgIntValue(env, phLock, hLock);
	return rc;
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnUnlockNodeForChanges(JNIEnv *, jclass, jlong hNode, jint hLock)
{
	return xnUnlockNodeForChanges((XnNodeHandle)hNode, (XnLockHandle)hLock);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnLockedNodeStartChanges(JNIEnv *, jclass, jlong hNode, jint hLock)
{
	return xnLockedNodeStartChanges((XnNodeHandle)hNode, (XnLockHandle)hLock);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnLockedNodeEndChanges(JNIEnv *, jclass, jlong hNode, jint hLock)
{
	return xnLockedNodeEndChanges((XnNodeHandle)hNode, (XnLockHandle)hLock);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnAddNeededNode(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return xnAddNeededNode((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jint JNICALL 
Java_org_openni_NativeMethods_xnRemoveNeededNode(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return xnRemoveNeededNode((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

//---------------------------------------------------------------------------
// Device
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateDevice(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateDevice((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

//---------------------------------------------------------------------------
// Device Identification Capability
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetDeviceName(JNIEnv *env, jclass, jlong hNode, jobject pResult)
{
	XnChar strResult[1024];
	XnUInt32 nLength = 1024;
	XnStatus nRetVal = xnGetDeviceName((XnNodeHandle)hNode, strResult, &nLength);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgStringValue(env, pResult, strResult);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetVendorSpecificData(JNIEnv *env, jclass, jlong hNode, jobject pResult)
{
	XnChar strResult[1024];
	XnUInt32 nLength = 1024;
	XnStatus nRetVal = xnGetVendorSpecificData((XnNodeHandle)hNode, strResult, &nLength);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgStringValue(env, pResult, strResult);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSerialNumber(JNIEnv *env, jclass, jlong hNode, jobject pResult)
{
	XnChar strResult[1024];
	XnUInt32 nLength = 1024;
	XnStatus nRetVal = xnGetSerialNumber((XnNodeHandle)hNode, strResult, &nLength);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgStringValue(env, pResult, strResult);
	return XN_STATUS_OK;
}

//---------------------------------------------------------------------------
// ErrorState Capability
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetNodeErrorState(JNIEnv *, jclass, jlong hNode)
{
	return xnGetNodeErrorState((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToNodeErrorStateChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToNodeErrorStateChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromNodeErrorStateChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	return UnregisterFromStateChangeEvent(xnUnregisterFromNodeErrorStateChange, hNode, hCallback);
}

//---------------------------------------------------------------------------
// GeneralInt Capability
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetGeneralIntRange(JNIEnv *env, jclass, jlong hNode, jstring capName, jobject pMin, jobject pMax, jobject pStep, jobject pDefault, jobject pAuto)
{
	XnInt32 nMin, nMax, nStep, nDefault;
	XnBool bAuto;
	XnStatus nRetVal = xnGetGeneralIntRange((XnNodeHandle)hNode, JavaString(env, capName), &nMin, &nMax, &nStep, &nDefault, &bAuto);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pMin, nMin);
	SetOutArgIntValue(env, pMax, nMin);
	SetOutArgIntValue(env, pStep, nStep);
	SetOutArgIntValue(env, pDefault, nDefault);
	SetOutArgIntValue(env, pAuto, bAuto);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetGeneralIntValue(JNIEnv *env, jclass, jlong hNode, jstring strCap, jobject pVal)
{
	XnInt32 nVal;
	XnStatus nRetVal = xnGetGeneralIntValue((XnNodeHandle)hNode, JavaString(env, strCap), &nVal);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pVal, nVal);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetGeneralIntValue(JNIEnv *env, jclass, jlong hNode, jstring strCap, jint val)
{
	return xnSetGeneralIntValue((XnNodeHandle)hNode, JavaString(env, strCap), val);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToGeneralIntValueChange(JNIEnv *env, jclass, jlong hNode, jstring strCap, jobject obj, jstring cb, jobject phCallback)
{
	StateChangedCallbackCookie* pCookie = new StateChangedCallbackCookie(env, obj, cb);
	XnStatus rc = xnRegisterToGeneralIntValueChange((XnNodeHandle)hNode, JavaString(env, strCap), StateChangedHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromGeneralIntValueChange(JNIEnv *env, jclass, jlong hNode, jstring strCap, jlong hCallback)
{
	StateChangedCallbackCookie* pCallback = (StateChangedCallbackCookie*)hCallback;
	xnUnregisterFromGeneralIntValueChange((XnNodeHandle)hNode, JavaString(env, strCap), pCallback->hCallback);
	delete pCallback;
}

//---------------------------------------------------------------------------
// Generator
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnStartGenerating(JNIEnv *, jclass, jlong hNode)
{
	return xnStartGenerating((XnNodeHandle)hNode);
}

JNIEXPORT jboolean JNICALL
Java_org_openni_NativeMethods_xnIsGenerating(JNIEnv *, jclass, jlong hNode)
{
	return (jboolean)xnIsGenerating((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnStopGenerating(JNIEnv *, jclass, jlong hNode)
{
	return xnStopGenerating((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnRegisterToGenerationRunningChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToGenerationRunningChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL
Java_org_openni_NativeMethods_xnUnregisterFromGenerationRunningChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromGenerationRunningChange, hNode, hCallback);
}

JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnRegisterToNewDataAvailable(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToNewDataAvailable, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL
Java_org_openni_NativeMethods_xnUnregisterFromNewDataAvailable(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromNewDataAvailable, hNode, hCallback);
}

JNIEXPORT jboolean JNICALL
Java_org_openni_NativeMethods_xnIsNewDataAvailable(JNIEnv *env, jclass, jlong hNode, jobject pnTimestamp)
{
	XnUInt64 nTimestamp = 0;
	XnBool rc = xnIsNewDataAvailable((XnNodeHandle)hNode, &nTimestamp);
	SetOutArgLongValue(env, pnTimestamp, nTimestamp);
	return (jboolean)rc;
}

JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnWaitAndUpdateData(JNIEnv *, jclass, jlong hNode)
{
	return xnWaitAndUpdateData((XnNodeHandle)hNode);
}

JNIEXPORT jboolean JNICALL
Java_org_openni_NativeMethods_xnIsDataNew(JNIEnv *, jclass, jlong hNode)
{
	return (jboolean)xnIsDataNew((XnNodeHandle)hNode);
}

JNIEXPORT jlong JNICALL
Java_org_openni_NativeMethods_xnGetData(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetData((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnGetDataSize(JNIEnv *, jclass, jlong hNode)
{
	return xnGetDataSize((XnNodeHandle)hNode);
}

JNIEXPORT jlong JNICALL
Java_org_openni_NativeMethods_xnGetTimestamp(JNIEnv *, jclass, jlong hNode)
{
	return xnGetTimestamp((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL
Java_org_openni_NativeMethods_xnGetFrameID(JNIEnv *, jclass, jlong hNode)
{
	return xnGetFrameID((XnNodeHandle)hNode);
}

//---------------------------------------------------------------------------
// Mirror
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetMirror(JNIEnv *, jclass, jlong hNode, jboolean isMirror)
{
	return xnSetMirror((XnNodeHandle)hNode, isMirror);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsMirrored(JNIEnv *, jclass, jlong hNode)
{
	return (jboolean)xnIsMirrored((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToMirrorChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToMirrorChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromMirrorChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromMirrorChange, hNode, hCallback);
}

//---------------------------------------------------------------------------
// ViewPoint
//---------------------------------------------------------------------------
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsViewPointSupported(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return (jboolean)xnIsViewPointSupported((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetViewPoint(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return xnSetViewPoint((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnResetViewPoint(JNIEnv *, jclass, jlong hNode)
{
	return xnResetViewPoint((XnNodeHandle)hNode);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsViewPointAs(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return (jboolean)xnIsViewPointAs((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToViewPointChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToViewPointChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromViewPointChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromViewPointChange, hNode, hCallback);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetPixelCoordinatesInViewPoint(JNIEnv* env, jclass, jlong hNode, jlong hViewpoint, jint x, jint y, jobject pAltX, jobject pAltY)
{
	XnUInt32 nAltX;
	XnUInt32 nAltY;
	XnStatus nRetVal = xnGetPixelCoordinatesInViewPoint((XnNodeHandle)hNode, (XnNodeHandle)hViewpoint, x, y, &nAltX, &nAltY);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pAltX, nAltX);
	SetOutArgIntValue(env, pAltY, nAltY);
	return XN_STATUS_OK;
}

//---------------------------------------------------------------------------
// FrameSync
//---------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnCanFrameSyncWith(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return (jboolean)xnCanFrameSyncWith((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnFrameSyncWith(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return xnFrameSyncWith((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}
  
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStopFrameSyncWith(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return xnStopFrameSyncWith((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsFrameSyncedWith(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return (jboolean)xnIsFrameSyncedWith((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToFrameSyncChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToFrameSyncChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromFrameSyncChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromFrameSyncChange, hNode, hCallback);
}

//---------------------------------------------------------------------------
// Map Generator
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSupportedMapOutputModesCount(JNIEnv *, jclass, jlong hNode)
{
	return xnGetSupportedMapOutputModesCount((XnNodeHandle)hNode);
}

jobject CreateMapOutputMode(JNIEnv *env, const XnMapOutputMode* pMode)
{
	jclass cls = env->FindClass("org/openni/MapOutputMode");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(III)V");
	return env->NewObject(cls, ctor, pMode->nXRes, pMode->nYRes, pMode->nFPS);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSupportedMapOutputModes(JNIEnv *env, jclass, jlong hNode, jobjectArray outputModes)
{
	XnUInt32 size = env->GetArrayLength(outputModes);
	XnMapOutputMode* modes = new XnMapOutputMode[size];
	XN_VALIDATE_ALLOC_PTR(modes);

	XnUInt32 nCount = size;
	XnStatus nRetVal = xnGetSupportedMapOutputModes((XnNodeHandle)hNode, modes, &nCount);
	if (nRetVal != XN_STATUS_OK)
	{
		delete[] modes;
		return nRetVal;
	}

	XN_ASSERT(nCount == size);

	// now copy to managed array
	for (XnUInt32 i = 0; i < size; ++i)
	{
		env->SetObjectArrayElement(outputModes, i, CreateMapOutputMode(env, &modes[i]));
	}

	delete[] modes;
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetMapOutputMode(JNIEnv *, jclass, jlong hNode, jint xRes, jint yRes, jint fps)
{
	XnMapOutputMode mode = { xRes, yRes, fps };
	return xnSetMapOutputMode((XnNodeHandle)hNode, &mode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetMapOutputMode(JNIEnv *env, jclass, jlong hNode, jobject xRes, jobject yRes, jobject fps)
{
	XnMapOutputMode mode;
	XnStatus nRetVal = xnGetMapOutputMode((XnNodeHandle)hNode, &mode);
	XN_IS_STATUS_OK(nRetVal);

	SetOutArgIntValue(env, xRes, mode.nXRes);
	SetOutArgIntValue(env, yRes, mode.nYRes);
	SetOutArgIntValue(env, fps, mode.nFPS);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToMapOutputModeChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToMapOutputModeChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromMapOutputModeChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromMapOutputModeChange, hNode, hCallback);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetBytesPerPixel(JNIEnv *, jclass, jlong hNode)
{
	return xnGetBytesPerPixel((XnNodeHandle)hNode);
}

//---------------------------------------------------------------------------
// Cropping Capability
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetCropping(JNIEnv *, jclass, jlong hNode, jint xOffset, jint yOffset, jint xSize, jint ySize, jboolean isEnabled)
{
	XnCropping cropping = { isEnabled, (XnUInt16)xOffset, (XnUInt16)yOffset, (XnUInt16)xSize, (XnUInt16)ySize };
	return xnSetCropping((XnNodeHandle)hNode, &cropping);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetCropping(JNIEnv *env, jclass, jlong hNode, jobject pxOffset, jobject pyOffset, jobject pxSize, jobject pySize, jobject pEnabled)
{
	XnCropping cropping = {0};
	XnStatus nRetVal = xnGetCropping((XnNodeHandle)hNode, &cropping);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pxOffset, cropping.nXOffset);
	SetOutArgIntValue(env, pyOffset, cropping.nYOffset);
	SetOutArgIntValue(env, pxSize, cropping.nXSize);
	SetOutArgIntValue(env, pySize, cropping.nYSize);
	SetOutArgBoolValue(env, pEnabled, (jboolean)cropping.bEnabled);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToCroppingChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToCroppingChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromCroppingChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromCroppingChange, hNode, hCallback);
}

//---------------------------------------------------------------------------
// AntiFlicker
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetPowerLineFrequency(JNIEnv *, jclass, jlong hNode, jint frequency)
{
	return xnSetPowerLineFrequency((XnNodeHandle)hNode, (XnPowerLineFrequency)frequency);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetPowerLineFrequency(JNIEnv *, jclass, jlong hNode)
{
	return xnGetPowerLineFrequency((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToPowerLineFrequencyChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToPowerLineFrequencyChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromPowerLineFrequencyChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromPowerLineFrequencyChange, hNode, hCallback);
}

//---------------------------------------------------------------------------
// Depth
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateDepthGenerator(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pEnumerationErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateDepthGenerator((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pEnumerationErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jshort JNICALL Java_org_openni_NativeMethods_xnGetDeviceMaxDepth(JNIEnv *, jclass, jlong hNode)
{
	return xnGetDeviceMaxDepth((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetDepthFieldOfView(JNIEnv *env, jclass, jlong hNode, jobject phFOV, jobject pvFOV)
{
	XnFieldOfView FOV;
	XnStatus nRetVal = xnGetDepthFieldOfView((XnNodeHandle)hNode, &FOV);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgDoubleValue(env, phFOV, FOV.fHFOV);
	SetOutArgDoubleValue(env, pvFOV, FOV.fVFOV);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToDepthFieldOfViewChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToDepthFieldOfViewChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromDepthFieldOfViewChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromDepthFieldOfViewChange, hNode, hCallback);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnConvertProjectiveToRealWorld(JNIEnv *env, jclass, jlong hNode, jobjectArray projective, jobjectArray realWorld)
{
	int count = env->GetArrayLength(projective);
	XnPoint3D* aProjective = new XnPoint3D[count];
	XnPoint3D* aRealWorld = new XnPoint3D[count];

	JavaPointArrayToNative(env, projective, aProjective);

	XnStatus nRetVal = xnConvertProjectiveToRealWorld((XnNodeHandle)hNode, count, aProjective, aRealWorld);
	if (nRetVal != XN_STATUS_OK)
	{
		delete[] aProjective;
		delete[] aRealWorld;
		return (nRetVal);
	}

	NativePointArrayToJava(env, aRealWorld, realWorld);
	delete[] aProjective;
	delete[] aRealWorld;
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnConvertRealWorldToProjective(JNIEnv *env, jclass, jlong hNode, jobjectArray realWorld, jobjectArray projective)
{
	int count = env->GetArrayLength(realWorld);
	XnPoint3D* aRealWorld = new XnPoint3D[count];
	XnPoint3D* aProjective = new XnPoint3D[count];

	JavaPointArrayToNative(env, realWorld, aRealWorld);

	XnStatus nRetVal = xnConvertRealWorldToProjective((XnNodeHandle)hNode, count, aRealWorld, aProjective);
	if (nRetVal != XN_STATUS_OK)
	{
		delete[] aRealWorld;
		delete[] aProjective;
		return (nRetVal);
	}

	NativePointArrayToJava(env, aProjective, projective);
	delete[] aProjective;
	delete[] aRealWorld;
	return XN_STATUS_OK;
}

JNIEXPORT jlong JNICALL Java_org_openni_NativeMethods_xnGetDepthMap(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetDepthMap((XnNodeHandle)hNode);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnGetDepthMetaData(JNIEnv *env, jclass, jlong hNode, jobject metaData)
{
	static XnDepthMetaData* pNativeMD = xnAllocateDepthMetaData();
	xnGetDepthMetaData((XnNodeHandle)hNode, pNativeMD);
	CopyDepthMetaData(env, metaData, pNativeMD);
}

//---------------------------------------------------------------------------
// UserPosition
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSupportedUserPositionsCount(JNIEnv *, jclass, jlong hNode)
{
	return xnGetSupportedUserPositionsCount((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetUserPosition(JNIEnv *, jclass, jlong hNode, jint index, jfloat xMin, jfloat yMin, jfloat zMin, jfloat xMax, jfloat yMax, jfloat zMax)
{
	XnBoundingBox3D box = {
		{ xMin, yMin, zMin },
		{ xMax, yMax, zMax }
	};
	return xnSetUserPosition((XnNodeHandle)hNode, index, &box);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetUserPosition(JNIEnv *env, jclass, jlong hNode, jint index, jobject pBox)
{
	XnBoundingBox3D box;
	XnStatus nRetVal = xnGetUserPosition((XnNodeHandle)hNode, index, &box);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgBoundingBox3DValue(env, pBox, &box);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToUserPositionChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToUserPositionChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromUserPositionChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromUserPositionChange, hNode, hCallback);
}

//---------------------------------------------------------------------------
// Image
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateImageGenerator(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pEnumerationErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateImageGenerator((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pEnumerationErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jlong JNICALL Java_org_openni_NativeMethods_xnGetImageMap(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetImageMap((XnNodeHandle)hNode);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsPixelFormatSupported(JNIEnv *, jclass, jlong hNode, jint format)
{
	return (jboolean)xnIsPixelFormatSupported((XnNodeHandle)hNode, (XnPixelFormat)format);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetPixelFormat(JNIEnv *, jclass, jlong hNode, jint format)
{
	return xnSetPixelFormat((XnNodeHandle)hNode, (XnPixelFormat)format);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetPixelFormat(JNIEnv *, jclass, jlong hNode)
{
	return xnGetPixelFormat((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToPixelFormatChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToPixelFormatChange, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromPixelFormatChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromPixelFormatChange, hNode, hCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnGetImageMetaData(JNIEnv *env, jclass, jlong hNode, jobject metaData)
{
	static XnImageMetaData* pNativeMD = xnAllocateImageMetaData();
	xnGetImageMetaData((XnNodeHandle)hNode, pNativeMD);
	CopyImageMetaData(env, metaData, pNativeMD);
}

//---------------------------------------------------------------------------
// IR
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateIRGenerator(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pEnumerationErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateIRGenerator((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pEnumerationErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jlong JNICALL Java_org_openni_NativeMethods_xnGetIRMap(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetIRMap((XnNodeHandle)hNode);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnGetIRMetaData(JNIEnv *env, jclass, jlong hNode, jobject metaData)
{
	static XnIRMetaData* pNativeMD = xnAllocateIRMetaData();
	xnGetIRMetaData((XnNodeHandle)hNode, pNativeMD);
	CopyIRMetaData(env, metaData, pNativeMD);
}

//---------------------------------------------------------------------------
// GestureGenerator
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateGestureGenerator(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pEnumerationErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateGestureGenerator((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pEnumerationErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnAddGesture__JLjava_lang_String_2(JNIEnv *env, jclass, jlong hNode, jstring strGesture)
{
	JavaString jName(env, strGesture);
	return xnAddGesture((XnNodeHandle)hNode, jName, NULL);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnAddGesture__JLjava_lang_String_2FFFFFF(JNIEnv *env, jclass, jlong hNode, jstring strGesture, jfloat minx, jfloat miny, jfloat minz, jfloat maxx, jfloat maxy, jfloat maxz)
{
	XnBoundingBox3D area;
	area.LeftBottomNear = xnCreatePoint3D(minx, miny, minz);
	area.RightTopFar = xnCreatePoint3D(maxx, maxy, maxz);
	JavaString jName(env, strGesture);
	return xnAddGesture((XnNodeHandle)hNode, jName, &area);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRemoveGesture(JNIEnv *env, jclass, jlong hNode, jstring strGesture)
{
	JavaString jName(env, strGesture);
	return xnRemoveGesture((XnNodeHandle)hNode, jName);
}
typedef XnChar* XnCharPtr;
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetAllActiveGestures(JNIEnv *env, jclass, jlong hNode, jobject pGestures)
{
	XnChar** nativeGestures = NULL;
	nativeGestures = new XnCharPtr[20];
	for (int i = 0; i < 20; ++i)
	{
		nativeGestures[i] = new XnChar[80];
		xnOSMemSet(nativeGestures[i], 0, 80);
	}
	XnUInt16 nCount = 20;

	XnStatus nRetVal = xnGetAllActiveGestures((XnNodeHandle)hNode, nativeGestures, 80, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// Create array
	jclass cls = env->FindClass("java/lang/String");
	jobjectArray gestures = env->NewObjectArray(nCount, cls, NULL);

	jstring* jGestures = new jstring[nCount];

	// Fill it up
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		jGestures[i] = env->NewStringUTF(nativeGestures[i]);
		env->SetObjectArrayElement(gestures, i, jGestures[i]);
	}

	// now return the array
	SetOutArgObjectValue(env, pGestures, gestures);

	for (int i = 0; i < nCount; ++i)
	{
		delete []nativeGestures[i];
	}
	delete []jGestures;
	delete []nativeGestures;
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerateAllGestures(JNIEnv *env, jclass, jlong hNode, jobject pGestures)
{
	XnChar** nativeGestures = NULL;
	nativeGestures = new XnCharPtr[20];
	for (int i = 0; i < 20; ++i)
	{
		nativeGestures[i] = new XnChar[80];
		xnOSMemSet(nativeGestures[i], 0, 80);
	}
	XnUInt16 nCount = 20;

	XnStatus nRetVal = xnEnumerateAllGestures((XnNodeHandle)hNode, nativeGestures, 80, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// Create array
	jclass cls = env->FindClass("java/lang/String");
	jobjectArray gestures = env->NewObjectArray(nCount, cls, NULL);

	jstring* jGestures = new jstring[nCount];

	// Fill it up
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		jGestures[i] = env->NewStringUTF(nativeGestures[i]);
		env->SetObjectArrayElement(gestures, i, jGestures[i]);
	}

	// now return the array
	SetOutArgObjectValue(env, pGestures, gestures);

	delete [] jGestures;
	for (int i = 0; i < 20; ++i)
	{
		delete[] nativeGestures[i];
	}
	delete[] nativeGestures;
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetNumberOfAvailableGestures(JNIEnv *, jclass, jlong hNode)
{
    return xnGetNumberOfAvailableGestures((XnNodeHandle)hNode);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsGestureAvailable(JNIEnv *env, jclass, jlong hNode, jstring strGesture)
{
	JavaString jName(env, strGesture);
	return (jboolean)xnIsGestureAvailable((XnNodeHandle)hNode, jName);
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsGestureProgressSupported(JNIEnv *env, jclass, jlong hNode, jstring strGesture)
{
	JavaString jName(env, strGesture);
	return (jboolean)xnIsGestureProgressSupported((XnNodeHandle)hNode, jName);
}

void XN_CALLBACK_TYPE GestureRecognizedHandler(XnNodeHandle , const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier supplier;
	jstring gestureName = supplier.GetEnv()->NewStringUTF(strGesture);
	supplier.GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, gestureName, CreatePoint3D(supplier.GetEnv(), pIDPosition), CreatePoint3D(supplier.GetEnv(), pEndPosition));
}
void XN_CALLBACK_TYPE GestureProgressHandler(XnNodeHandle , const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier supplier;
	jstring gestureName = supplier.GetEnv()->NewStringUTF(strGesture);
	supplier.GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, gestureName, CreatePoint3D(supplier.GetEnv(), pPosition), fProgress);
}
jint RegisterGestureRecognized(JNIEnv* env, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;Lorg/openni/Point3D;Lorg/openni/Point3D;)V");
	XnStatus rc = xnRegisterGestureCallbacks((XnNodeHandle)hNode, GestureRecognizedHandler, NULL, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
jint RegisterGestureProgress(JNIEnv* env, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;Lorg/openni/Point3D;F)V");
	XnStatus rc = xnRegisterGestureCallbacks((XnNodeHandle)hNode, NULL, GestureProgressHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterGestureCallbacks(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring recognizedCB, jstring progressCB, jobject phCallback)
{
	if (recognizedCB != NULL)
	{
		return RegisterGestureRecognized(env, hNode, obj, recognizedCB, phCallback);
	}
	else if (progressCB != NULL)
	{
		return RegisterGestureProgress(env, hNode, obj, progressCB, phCallback);
	}
	return -1;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterGestureCallbacks(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterGestureCallbacks((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToGestureChange(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToGestureChange, env, hNode, obj, cb, phCallback);
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromGestureChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	return UnregisterFromStateChangeEvent(xnUnregisterFromGestureChange, hNode, hCallback);
}
void XN_CALLBACK_TYPE GesturePositionRecognizedHandler(XnNodeHandle , const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier supplier;
	jstring gestureName = supplier.GetEnv()->NewStringUTF(strGesture);
	supplier.GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, gestureName, CreatePoint3D(supplier.GetEnv(), pPosition));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToGestureIntermediateStageCompleted(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;Lorg/openni/Point3D;)V");
	XnStatus rc = xnRegisterToGestureIntermediateStageCompleted((XnNodeHandle)hNode, GesturePositionRecognizedHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromGestureIntermediateStageCompleted(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromGestureIntermediateStageCompleted((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToGestureReadyForNextIntermediateStage(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;Lorg/openni/Point3D;)V");
	XnStatus rc = xnRegisterToGestureReadyForNextIntermediateStage((XnNodeHandle)hNode, GesturePositionRecognizedHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromGestureReadyForNextIntermediateStage(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromGestureReadyForNextIntermediateStage((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}

//---------------------------------------------------------------------------
// Scene Analyzer
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateSceneAnalyzer(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pEnumerationErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateSceneAnalyzer((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pEnumerationErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}
JNIEXPORT jlong JNICALL Java_org_openni_NativeMethods_xnGetLabelMap(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetLabelMap((XnNodeHandle)hNode);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetFloor(JNIEnv *env, jclass, jlong hNode, jobject pPlaneNormal, jobject pPlanePoint)
{
	XnPlane3D plane;
	XnStatus nRetVal = xnGetFloor((XnNodeHandle)hNode, &plane);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPoint3DValue(env, pPlaneNormal, &plane.vNormal);
	SetOutArgPoint3DValue(env, pPlanePoint, &plane.ptPoint);
	return XN_STATUS_OK;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnGetSceneMetaData(JNIEnv *env, jclass, jlong hNode, jobject metaData)
{
	static XnSceneMetaData* pNativeMD = xnAllocateSceneMetaData();
	xnGetSceneMetaData((XnNodeHandle)hNode, pNativeMD);
	CopySceneMetaData(env, metaData, pNativeMD);
}

//---------------------------------------------------------------------------
// User Generator
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateUserGenerator(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pEnumerationErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateUserGenerator((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pEnumerationErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetNumberOfUsers(JNIEnv *, jclass, jlong hNode)
{
	return xnGetNumberOfUsers((XnNodeHandle)hNode);
}

jobject CreateObjectFromInteger(JNIEnv* env, XnUserID id)
{
	jclass cls = env->FindClass("java/lang/Integer");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(I)V");
	return env->NewObject(cls, ctor, id);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetUsers(JNIEnv *env, jclass, jlong hNode, jobject pUsers)
{
	XnUInt16 nCount = xnGetNumberOfUsers((XnNodeHandle)hNode);

	XnUserID* aUsers = new XnUserID[nCount];

	XnStatus nRetVal = xnGetUsers((XnNodeHandle)hNode, aUsers, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// Create array
	jclass cls = env->FindClass("java/lang/Integer");
	jobjectArray users = env->NewObjectArray(nCount, cls, NULL);

	// Fill it up
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		env->SetObjectArrayElement(users, i, CreateObjectFromInteger(env, aUsers[i]));
	}

	// now return the array
	SetOutArgObjectValue(env, pUsers, users);

	delete []aUsers;
	return XN_STATUS_OK;	
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetUserCoM(JNIEnv *env, jclass, jlong hNode, jint nId, jobject pCoM)
{
	XnPoint3D com;
	XnStatus nRetVal = xnGetUserCoM((XnNodeHandle)hNode, nId, &com);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPoint3DValue(env, pCoM, &com);
	return XN_STATUS_OK;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetUserPixels(JNIEnv *env, jclass, jlong hNode, jint id, jobject metaData)
{
	static XnSceneMetaData* pNativeMD = xnAllocateSceneMetaData();
	XnStatus nRetVal = xnGetUserPixels((XnNodeHandle)hNode, id, pNativeMD);
	XN_IS_STATUS_OK(nRetVal);
	xnGetSceneMetaData((XnNodeHandle)hNode, pNativeMD);
	CopySceneMetaData(env, metaData, pNativeMD);
	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE UserEventHandler(XnNodeHandle , XnUserID id, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, id);
}
jint RegisterNewUserCallback(JNIEnv* env, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(I)V");
	XnStatus rc = xnRegisterUserCallbacks((XnNodeHandle)hNode, UserEventHandler, NULL, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
jint RegisterLostUserCallback(JNIEnv* env, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(I)V");
	XnStatus rc = xnRegisterUserCallbacks((XnNodeHandle)hNode, NULL, UserEventHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterUserCallbacks(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring newUserCB, jstring lostUserCB, jobject phCallback)
{
	if (newUserCB != NULL)
	{
		return RegisterNewUserCallback(env, hNode, obj, newUserCB, phCallback);
	}
	else if (lostUserCB != NULL)
	{
		return RegisterLostUserCallback(env, hNode, obj, lostUserCB, phCallback);
	}
	return -1;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterUserCallbacks(JNIEnv * , jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterUserCallbacks((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToUserExit(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(I)V");
	XnStatus rc = xnRegisterToUserExit((XnNodeHandle)hNode, UserEventHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromUserExit(JNIEnv * , jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromUserExit((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToUserReEnter(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(I)V");
	XnStatus rc = xnRegisterToUserReEnter((XnNodeHandle)hNode, UserEventHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromUserReEnter(JNIEnv * , jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromUserReEnter((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}

//---------------------------------------------------------------------------
// Skeleton Capability
//---------------------------------------------------------------------------
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsJointAvailable(JNIEnv *, jclass, jlong hNode, jint eJoint)
{
	return (jboolean)xnIsJointAvailable((XnNodeHandle)hNode, XnSkeletonJoint(eJoint));
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsProfileAvailable(JNIEnv *, jclass, jlong hNode, jint eProfile)
{
	return (jboolean)xnIsProfileAvailable((XnNodeHandle)hNode, XnSkeletonProfile(eProfile));
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetSkeletonProfile(JNIEnv *, jclass, jlong hNode, jint eProfile)
{
	return xnSetSkeletonProfile((XnNodeHandle)hNode, XnSkeletonProfile(eProfile));
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetJointActive(JNIEnv *, jclass, jlong hNode, jint eJoint, jboolean bActive)
{
	return xnSetJointActive((XnNodeHandle)hNode, XnSkeletonJoint(eJoint), bActive);
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsJointActive(JNIEnv *, jclass, jlong hNode, jint eJoint)
{
	return (jboolean)xnIsJointActive((XnNodeHandle)hNode, XnSkeletonJoint(eJoint));
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToJointConfigurationChange(JNIEnv* env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToJointConfigurationChange, env, hNode, obj, cb, phCallback);
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromJointConfigurationChange(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromJointConfigurationChange, hNode, hCallback);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumerateActiveJoints(JNIEnv* env, jclass, jlong hNode, jobject pJoints)
{
	XnUInt16 nCount = 40;

	XnSkeletonJoint* aJoints = new XnSkeletonJoint[nCount];

	XnStatus nRetVal = xnEnumerateActiveJoints((XnNodeHandle)hNode, aJoints, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// Create array
	jclass cls = env->FindClass("java/lang/Integer");
	jobjectArray joints = env->NewObjectArray(nCount, cls, NULL);

	// Fill it up
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		env->SetObjectArrayElement(joints, i, CreateObjectFromInteger(env, aJoints[i]));
	}

	// now return the array
	SetOutArgObjectValue(env, pJoints, joints);

	delete []aJoints;
	return XN_STATUS_OK;	

}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSkeletonJoint(JNIEnv* env, jclass, jlong hNode, jint user, jint eJoint, jobject pJoint)
{
	XnSkeletonJointTransformation transformation;
	XnStatus nRetVal = xnGetSkeletonJoint((XnNodeHandle)hNode, user, (XnSkeletonJoint)eJoint, &transformation);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgObjectValue(env, pJoint, CreateJointTransformation(env, &transformation));
	return XN_STATUS_OK;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSkeletonJointPosition(JNIEnv* env, jclass, jlong hNode, jint user, jint eJoint, jobject pPosition)
{
	XnSkeletonJointPosition position;
	XnStatus nRetVal = xnGetSkeletonJointPosition((XnNodeHandle)hNode, user, (XnSkeletonJoint)eJoint, &position);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgObjectValue(env, pPosition, CreateJointPosition(env, &position));
	return XN_STATUS_OK;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSkeletonJointOrientation(JNIEnv* env, jclass, jlong hNode, jint user, jint eJoint, jobject pOrientation)
{
	XnSkeletonJointOrientation orientation;
	XnStatus nRetVal = xnGetSkeletonJointOrientation((XnNodeHandle)hNode, user, (XnSkeletonJoint)eJoint, &orientation);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgObjectValue(env, pOrientation, CreateJointOrientation(env, &orientation));
	return XN_STATUS_OK;
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsSkeletonTracking(JNIEnv *, jclass, jlong hNode, jint user)
{
	return (jboolean)xnIsSkeletonTracking((XnNodeHandle)hNode, user);
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsSkeletonCalibrated(JNIEnv *, jclass, jlong hNode, jint user)
{
	return (jboolean)xnIsSkeletonCalibrated((XnNodeHandle)hNode, user);
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsSkeletonCalibrating(JNIEnv *, jclass, jlong hNode, jint user)
{
	return (jboolean)xnIsSkeletonCalibrating((XnNodeHandle)hNode, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRequestSkeletonCalibration(JNIEnv *, jclass, jlong hNode, jint user, jboolean bForce)
{
	return xnRequestSkeletonCalibration((XnNodeHandle)hNode, user, bForce);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnAbortSkeletonCalibration(JNIEnv *, jclass, jlong hNode, jint user)
{
	return xnAbortSkeletonCalibration((XnNodeHandle)hNode, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSaveSkeletonCalibrationDataToFile(JNIEnv* env, jclass, jlong hNode, jint user, jstring fileName)
{
	JavaString jName(env, fileName);
	return xnSaveSkeletonCalibrationDataToFile((XnNodeHandle)hNode, user, jName);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnLoadSkeletonCalibrationDataFromFile(JNIEnv* env, jclass, jlong hNode, jint user, jstring fileName)
{
	JavaString jName(env, fileName);
	return xnLoadSkeletonCalibrationDataFromFile((XnNodeHandle)hNode, user, jName);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSaveSkeletonCalibrationData(JNIEnv *, jclass, jlong hNode, jint user, jint nSlot)
{
	return xnSaveSkeletonCalibrationData((XnNodeHandle)hNode, user, nSlot);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnLoadSkeletonCalibrationData(JNIEnv *, jclass, jlong hNode, jint user, jint nSlot)
{
	return xnLoadSkeletonCalibrationData((XnNodeHandle)hNode, user, nSlot);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnClearSkeletonCalibrationData(JNIEnv *, jclass, jlong hNode, jint nSlot)
{
	return xnClearSkeletonCalibrationData((XnNodeHandle)hNode, nSlot);
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsSkeletonCalibrationData(JNIEnv *, jclass, jlong hNode, jint nSlot)
{
	return (jboolean)xnIsSkeletonCalibrationData((XnNodeHandle)hNode, nSlot);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStartSkeletonTracking(JNIEnv *, jclass, jlong hNode, jint user)
{
	return xnStartSkeletonTracking((XnNodeHandle)hNode, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStopSkeletonTracking(JNIEnv *, jclass, jlong hNode, jint user)
{
	return xnStopSkeletonTracking((XnNodeHandle)hNode, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnResetSkeleton(JNIEnv *, jclass, jlong hNode, jint user)
{
	return xnResetSkeleton((XnNodeHandle)hNode, user);
}
JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnNeedPoseForSkeletonCalibration(JNIEnv *, jclass, jlong hNode)
{
	return (jboolean)xnNeedPoseForSkeletonCalibration((XnNodeHandle)hNode);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSkeletonCalibrationPose(JNIEnv* env, jclass, jlong hNode, jobject pPose)
{
	XnChar* poseName = new XnChar[80];
	XnStatus rc = xnGetSkeletonCalibrationPose((XnNodeHandle)hNode, poseName);
	if (rc == XN_STATUS_OK)
	{
		SetOutArgStringValue(env, pPose, poseName);
	}
	delete []poseName;
	return rc;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetSkeletonSmoothing(JNIEnv *, jclass, jlong hNode, jfloat factor)
{
	return xnSetSkeletonSmoothing((XnNodeHandle)hNode, factor);
}
void XN_CALLBACK_TYPE CalibrationStartHandler(XnNodeHandle , XnUserID user, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToCalibrationStart(JNIEnv* env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(I)V");
	XnStatus rc = xnRegisterToCalibrationStart((XnNodeHandle)hNode, CalibrationStartHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromCalibrationStart(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromCalibrationStart((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
void XN_CALLBACK_TYPE CalibrationProgressHandler(XnNodeHandle , XnUserID user, XnCalibrationStatus state, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, user, state);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToCalibrationInProgress(JNIEnv* env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(II)V");
	XnStatus rc = xnRegisterToCalibrationInProgress((XnNodeHandle)hNode, CalibrationProgressHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromCalibrationInProgress(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromCalibrationInProgress((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToCalibrationComplete(JNIEnv* env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(II)V");
	XnStatus rc = xnRegisterToCalibrationComplete((XnNodeHandle)hNode, CalibrationProgressHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromCalibrationComplete(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromCalibrationComplete((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
//---------------------------------------------------------------------------
// Pose Detection Capability
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetNumberOfPoses(JNIEnv *, jclass, jlong hNode)
{
	return xnGetNumberOfPoses((XnNodeHandle)hNode);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetAllAvailablePoses(JNIEnv *env, jclass, jlong hNode, jobject pPoses)
{
	XnChar** nativePoses = NULL;
	XnUInt32 nCount = xnGetNumberOfPoses((XnNodeHandle)hNode);

	nativePoses = new XnCharPtr[nCount];
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		nativePoses[i] = new XnChar[80];
		xnOSMemSet(nativePoses[i], 0, 80);
	}

	XnStatus nRetVal = xnGetAllAvailablePoses((XnNodeHandle)hNode, nativePoses, 80, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// Create array
	jclass cls = env->FindClass("java/lang/String");
	jobjectArray poses = env->NewObjectArray(nCount, cls, NULL);

	jstring* jPoses = new jstring[nCount];

	// Fill it up
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		jPoses[i] = env->NewStringUTF(nativePoses[i]);
		env->SetObjectArrayElement(poses, i, jPoses[i]);
	}

	// now return the array
	SetOutArgObjectValue(env, pPoses, poses);

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		delete []nativePoses[i];
	}
	delete []jPoses;
	delete []nativePoses;
	return XN_STATUS_OK;
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsPoseSupported(JNIEnv* env, jclass, jlong hNode, jstring strPose)
{
    JavaString jPose(env, strPose);
    return (jboolean)xnIsPoseSupported((XnNodeHandle)hNode, jPose);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetPoseStatus(JNIEnv* env, jclass, jlong hNode, jint user, jstring strPose, jobject pTime, jobject eStatus, jobject eState)
{
    
    JavaString jPose(env, strPose);
    XnUInt64 nTimestamp;
    XnPoseDetectionStatus tmpStatus;
    XnPoseDetectionState tmpState;
    XnStatus nRetVal = xnGetPoseStatus((XnNodeHandle)hNode,user,jPose,&nTimestamp,&tmpStatus,&tmpState);
    XN_IS_STATUS_OK(nRetVal);
    SetOutArgLongValue(env, pTime, nTimestamp);
    SetOutArgIntValue(env, eState, tmpState);
    SetOutArgIntValue(env, eStatus, tmpStatus);
    return XN_STATUS_OK;

}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStartPoseDetection(JNIEnv* env, jclass, jlong hNode, jstring strPose, jint user)
{
	JavaString jPose(env, strPose);
	return xnStartPoseDetection((XnNodeHandle)hNode, jPose, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStopPoseDetection(JNIEnv *, jclass, jlong hNode, jint user)
{
	return xnStopPoseDetection((XnNodeHandle)hNode, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStopSinglePoseDetection(JNIEnv * env, jclass, jlong hNode, jint user, jstring strPose)
{
	JavaString jPose(env, strPose);
	return xnStopSinglePoseDetection((XnNodeHandle)hNode, user, jPose);
}
void XN_CALLBACK_TYPE PoseDetectionHandler(XnNodeHandle , const XnChar* strPose, XnUserID user, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier supplier;
	jstring jPose = supplier.GetEnv()->NewStringUTF(strPose);
	supplier.GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, jPose, user);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToPoseDetected(JNIEnv* env, jclass, jlong hNode, jobject obj, jstring cb,jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;I)V");
	XnStatus rc = xnRegisterToPoseDetected((XnNodeHandle)hNode, PoseDetectionHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromPoseDetected(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromPoseDetected((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToOutOfPose(JNIEnv* env, jclass, jlong hNode, jobject obj, jstring cb,jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;I)V");
	XnStatus rc = xnRegisterToOutOfPose((XnNodeHandle)hNode, PoseDetectionHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromOutOfPose(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromOutOfPose((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}

void XN_CALLBACK_TYPE PoseDetectionInProgressHandler(XnNodeHandle , const XnChar* strPose, XnUserID user, XnPoseDetectionStatus eStatus, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier supplier;
	jstring jPose = supplier.GetEnv()->NewStringUTF(strPose);
	supplier.GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, jPose, user, eStatus);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToPoseDetectionInProgress(JNIEnv* env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(Ljava/lang/String;II)V");
	XnStatus rc = xnRegisterToPoseDetectionInProgress((XnNodeHandle)hNode, PoseDetectionInProgressHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, phCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromPoseDetectionInProgress(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromPoseDetectionInProgress((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}

//---------------------------------------------------------------------------
// Hands Generator
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateHandsGenerator(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pEnumerationErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateHandsGenerator((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pEnumerationErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE ActiveHandHandler(XnNodeHandle , XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, nId, CreatePoint3D(JNIEnvSupplier().GetEnv(), pPosition), fTime);
}
void XN_CALLBACK_TYPE InactiveHandHandler(XnNodeHandle , XnUserID nId, XnFloat fTime, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, nId, fTime);
}
jint RegisterHandCreateCallback(JNIEnv* env, XnNodeHandle hNode, jobject obj, jstring cb, jobject hCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(ILorg/openni/Point3D;F)V");
	XnStatus rc = xnRegisterHandCallbacks((XnNodeHandle)hNode, ActiveHandHandler, NULL, NULL, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, hCallback, pCookie);
	return rc;
}
jint RegisterHandUpdateCallback(JNIEnv* env, XnNodeHandle hNode, jobject obj, jstring cb, jobject hCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(ILorg/openni/Point3D;F)V");
	XnStatus rc = xnRegisterHandCallbacks((XnNodeHandle)hNode, NULL, ActiveHandHandler, NULL, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, hCallback, pCookie);
	return rc;
}
jint RegisterHandDestroyCallback(JNIEnv* env, XnNodeHandle hNode, jobject obj, jstring cb, jobject hCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(IF)V");
	XnStatus rc = xnRegisterHandCallbacks((XnNodeHandle)hNode, NULL, NULL, InactiveHandHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, hCallback, pCookie);
	return rc;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterHandCallbacks(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring handCreateCB, jstring handUpdateCB, jstring handDestroyCB, jobject hCallback)
{
	if (handCreateCB != NULL)
	{
		return RegisterHandCreateCallback(env, (XnNodeHandle)hNode, obj, handCreateCB, hCallback);
	}
	else if (handUpdateCB != NULL)
	{
		return RegisterHandUpdateCallback(env, (XnNodeHandle)hNode, obj, handUpdateCB, hCallback);
	}
	else if (handDestroyCB != NULL)
	{
		return RegisterHandDestroyCallback(env, (XnNodeHandle)hNode, obj, handDestroyCB, hCallback);
	}
	return -1;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterHandCallbacks(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterHandCallbacks((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStopTracking(JNIEnv *, jclass, jlong hNode, jint nId)
{
	return xnStopTracking((XnNodeHandle)hNode, nId);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStopTrackingAll(JNIEnv *, jclass, jlong hNode)
{
	return xnStopTrackingAll((XnNodeHandle)hNode);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnStartTracking(JNIEnv *, jclass, jlong hNode, jfloat x, jfloat y, jfloat z)
{
	XnPoint3D point = xnCreatePoint3D(x, y, z);
	return xnStartTracking((XnNodeHandle)hNode, &point);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetTrackingSmoothing(JNIEnv *, jclass, jlong hNode, jfloat fFactor)
{
	return xnSetTrackingSmoothing((XnNodeHandle)hNode, fFactor);
}
void XN_CALLBACK_TYPE HandTouchingFOVEdgeHandler(XnNodeHandle , XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDirection, void* pCookie)
{
	CallbackCookie* pCallback = (CallbackCookie*)pCookie;
	JNIEnvSupplier().GetEnv()->CallVoidMethod(pCallback->obj, pCallback->mid, nId, pPosition, fTime, eDirection);
}
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToHandTouchingFOVEdge(JNIEnv *env, jclass /*cls*/, jlong hNode, jobject obj, jstring cb, jobject hCallback)
{
	CallbackCookie* pCookie = new CallbackCookie(env, obj, cb, "(ILorg/openni/Point3D;FI)V");
	XnStatus rc = xnRegisterToHandTouchingFOVEdge((XnNodeHandle)hNode, HandTouchingFOVEdgeHandler, pCookie, &pCookie->hCallback);
	SetOutArgPointerValue(env, hCallback, pCookie);
	return rc;
}
JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromHandTouchingFOVEdge(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	CallbackCookie* pCallback = (CallbackCookie*)hCallback;
	xnUnregisterFromHandTouchingFOVEdge((XnNodeHandle)hNode, pCallback->hCallback);
	delete pCallback;
}
//---------------------------------------------------------------------------
// Audio
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateAudioGenerator(JNIEnv *env, jclass, jlong pContext, jobject phNode, jlong pQuery, jlong pErrors)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateAudioGenerator((XnContext*)pContext, &hNode, (XnNodeQuery*)pQuery, (XnEnumerationErrors*)pErrors);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jlong JNICALL Java_org_openni_NativeMethods_xnGetAudioBuffer(JNIEnv *, jclass, jlong hNode)
{
	return (jlong)xnGetAudioBuffer((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSupportedWaveOutputModesCount(JNIEnv *, jclass, jlong hNode)
{
	return xnGetSupportedWaveOutputModesCount((XnNodeHandle)hNode);
}

jobject CreateWaveOutputMode(JNIEnv *env, const XnWaveOutputMode* pMode)
{
	jclass cls = env->FindClass("org/openni/WaveOutputMode");
	jmethodID ctor = env->GetMethodID(cls, "<init>", "(ISB)V");
	return env->NewObject(cls, ctor, pMode->nSampleRate, pMode->nBitsPerSample, pMode->nChannels);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetSupportedWaveOutputModes(JNIEnv *env, jclass, jlong hNode, jobjectArray outputModes)
{
	XnUInt32 size = env->GetArrayLength(outputModes);
	XnWaveOutputMode* modes = new XnWaveOutputMode[size];
	XN_VALIDATE_ALLOC_PTR(modes);

	XnUInt32 nCount = size;
	XnStatus nRetVal = xnGetSupportedWaveOutputModes((XnNodeHandle)hNode, modes, &nCount);
	if (nRetVal != XN_STATUS_OK)
	{
		delete[] modes;
		return nRetVal;
	}

	XN_ASSERT(nCount == size);

	// now copy to managed array
	for (XnUInt32 i = 0; i < size; ++i)
	{
		env->SetObjectArrayElement(outputModes, i, CreateWaveOutputMode(env, &modes[i]));
	}

	delete[] modes;
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetWaveOutputMode(JNIEnv *, jclass, jlong hNode, jint nSampleRate, jshort nBitsPerSample, jbyte nChannels)
{
	XnWaveOutputMode mode = { nSampleRate, nBitsPerSample, nChannels };
	return xnSetWaveOutputMode((XnNodeHandle)hNode, &mode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetWaveOutputMode(JNIEnv *env, jclass, jlong hNode, jobject pnSampleRate, jobject pnBitsPerSample, jobject pnChannels)
{
	XnWaveOutputMode mode;
	XnStatus nRetVal = xnGetWaveOutputMode((XnNodeHandle)hNode, &mode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pnSampleRate, mode.nSampleRate);
	SetOutArgShortValue(env, pnBitsPerSample, mode.nBitsPerSample);
	SetOutArgByteValue(env, pnChannels, mode.nChannels);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToWaveOutputModeChanges(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToWaveOutputModeChanges, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromWaveOutputModeChanges(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromWaveOutputModeChanges, hNode, hCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnGetAudioMetaData(JNIEnv *env, jclass, jlong hNode, jobject metaData)
{
	static XnAudioMetaData* pNativeMD = xnAllocateAudioMetaData();
	xnGetAudioMetaData((XnNodeHandle)hNode, pNativeMD);
	CopyAudioMetaData(env, metaData, pNativeMD);
}

//---------------------------------------------------------------------------
// Codec
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateCodec(JNIEnv *env, jclass, jlong pContext, jint codecID, jlong hInitializer, jobject phNode)
{
	XnNodeHandle hNode = NULL;
	XnStatus nRetVal = xnCreateCodec((XnContext*)pContext, codecID, (XnNodeHandle)hInitializer, &hNode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetCodecID(JNIEnv *, jclass, jlong hNode)
{
	return xnGetCodecID((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEncodeData(JNIEnv *env, jclass, jlong hNode, jlong pSrc, jint nSrcSize, jlong pDst, jint nDstSize, jobject pnWritten)
{
	XnUInt32 nWritten = 0;
	XnStatus nRetVal = xnEncodeData((XnNodeHandle)hNode, (const void*)pSrc, nSrcSize, (void*)pDst, nDstSize, &nWritten);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pnWritten, nWritten);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnDecodeData(JNIEnv *env, jclass, jlong hNode, jlong pSrc, jint nSrcSize, jlong pDst, jint nDstSize, jobject pnWritten)
{
	XnUInt32 nWritten = 0;
	XnStatus nRetVal = xnDecodeData((XnNodeHandle)hNode, (const void*)pSrc, nSrcSize, (void*)pDst, nDstSize, &nWritten);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pnWritten, nWritten);
	return XN_STATUS_OK;
}

//---------------------------------------------------------------------------
// Recorder
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateRecorder(JNIEnv *env, jclass, jlong pContext, jstring strFormat, jobject phNode)
{
	XnNodeHandle hNode;
	XnStatus nRetVal = xnCreateRecorder((XnContext*)pContext, JavaString(env, strFormat), &hNode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetRecorderDestination(JNIEnv *env, jclass, jlong hNode, jint medium, jstring name)
{
	return xnSetRecorderDestination((XnNodeHandle)hNode, (XnRecordMedium)medium, JavaString(env, name));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetRecorderDestination(JNIEnv *env, jclass, jlong hNode, jobject pMedium, jobject pName)
{
	XnRecordMedium medium;
	XnChar strName[1024];
	XnStatus nRetVal = xnGetRecorderDestination((XnNodeHandle)hNode, &medium, strName, 1024);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pMedium, medium);
	SetOutArgStringValue(env, pName, strName);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnAddNodeToRecording(JNIEnv *, jclass, jlong hNode, jlong hOther, jint codecID)
{
	return xnAddNodeToRecording((XnNodeHandle)hNode, (XnNodeHandle)hOther, (XnCodecID)codecID);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRemoveNodeFromRecording(JNIEnv *, jclass, jlong hNode, jlong hOther)
{
	return xnRemoveNodeFromRecording((XnNodeHandle)hNode, (XnNodeHandle)hOther);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRecord(JNIEnv *, jclass, jlong hNode)
{
	return xnRecord((XnNodeHandle)hNode);
}

JNIEXPORT jstring JNICALL Java_org_openni_NativeMethods_xnGetRecorderFormat(JNIEnv *env, jclass, jlong hNode)
{
	return env->NewStringUTF(xnGetRecorderFormat((XnNodeHandle)hNode));
}

//---------------------------------------------------------------------------
// Player
//---------------------------------------------------------------------------

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreatePlayer(JNIEnv *env, jclass, jlong pContext, jstring strFormat, jobject phNode)
{
	XnNodeHandle hNode;
	XnStatus nRetVal = xnCreatePlayer((XnContext*)pContext, JavaString(env, strFormat), &hNode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetPlayerRepeat(JNIEnv *, jclass, jlong hNode, jboolean bRepeat)
{
	return xnSetPlayerRepeat((XnNodeHandle)hNode, bRepeat);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetPlayerSource(JNIEnv *env, jclass, jlong hNode, jint medium, jstring name)
{
	return xnSetPlayerSource((XnNodeHandle)hNode, (XnRecordMedium)medium, JavaString(env, name));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetPlayerSource(JNIEnv *env, jclass, jlong hNode, jobject pMedium, jobject pName)
{
	XnRecordMedium medium;
	XnChar strName[1024];
	XnStatus nRetVal = xnGetPlayerSource((XnNodeHandle)hNode, &medium, strName, 1024);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pMedium, medium);
	SetOutArgStringValue(env, pName, strName);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnPlayerReadNext(JNIEnv *, jclass, jlong hNode)
{
	return xnPlayerReadNext((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSeekPlayerToTimeStamp(JNIEnv *, jclass, jlong hNode, jlong offset, jint origin)
{
	return xnSeekPlayerToTimeStamp((XnNodeHandle)hNode, offset, (XnPlayerSeekOrigin)origin);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSeekPlayerToFrame(JNIEnv *env, jclass, jlong hNode, jstring strNode, jint offset, jint origin)
{
	return xnSeekPlayerToFrame((XnNodeHandle)hNode, JavaString(env, strNode), offset, (XnPlayerSeekOrigin)origin);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnTellPlayerTimestamp(JNIEnv *env, jclass, jlong hNode, jobject pnTimestamp)
{
	XnUInt64 nTimestamp;
	XnStatus nRetVal = xnTellPlayerTimestamp((XnNodeHandle)hNode, &nTimestamp);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgLongValue(env, pnTimestamp, nTimestamp);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnTellPlayerFrame(JNIEnv *env, jclass, jlong hNode, jstring strName, jobject pnFrameID)
{
	XnUInt32 nFrameID;
	XnStatus nRetVal = xnTellPlayerFrame((XnNodeHandle)hNode, JavaString(env, strName), &nFrameID);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pnFrameID, nFrameID);
	return XN_STATUS_OK;
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetPlayerNumFrames(JNIEnv *env, jclass, jlong hNode, jstring strName, jobject pnFrameID)
{
	XnUInt32 nFrameID;
	XnStatus nRetVal = xnGetPlayerNumFrames((XnNodeHandle)hNode, JavaString(env, strName), &nFrameID);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgIntValue(env, pnFrameID, nFrameID);
	return XN_STATUS_OK;
}

JNIEXPORT jstring JNICALL Java_org_openni_NativeMethods_xnGetPlayerSupportedFormat(JNIEnv *env, jclass, jlong hNode)
{
	return env->NewStringUTF(xnGetPlayerSupportedFormat((XnNodeHandle)hNode));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnEnumeratePlayerNodes(JNIEnv *env, jclass, jlong hNode, jobject ppList)
{
	XnNodeInfoList* pList;
	XnStatus nRetVal = xnEnumeratePlayerNodes((XnNodeHandle)hNode, &pList);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, ppList, pList);
	return XN_STATUS_OK;
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsPlayerAtEOF(JNIEnv *, jclass, jlong hNode)
{
	return (jboolean)xnIsPlayerAtEOF((XnNodeHandle)hNode);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnRegisterToEndOfFileReached(JNIEnv *env, jclass, jlong hNode, jobject obj, jstring cb, jobject phCallback)
{
	return RegisterToStateChangeEvent(xnRegisterToEndOfFileReached, env, hNode, obj, cb, phCallback);
}

JNIEXPORT void JNICALL Java_org_openni_NativeMethods_xnUnregisterFromEndOfFileReached(JNIEnv *, jclass, jlong hNode, jlong hCallback)
{
	UnregisterFromStateChangeEvent(xnUnregisterFromEndOfFileReached, hNode, hCallback);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnSetPlaybackSpeed(JNIEnv *, jclass, jlong hNode, jdouble dSpeed)
{
	return xnSetPlaybackSpeed((XnNodeHandle)hNode, dSpeed);
}

JNIEXPORT jdouble JNICALL Java_org_openni_NativeMethods_xnGetPlaybackSpeed(JNIEnv *, jclass, jlong hNode)
{
	return xnGetPlaybackSpeed((XnNodeHandle)hNode);
}


//---------------------------------------------------------------------------
// ScriptNode
//---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnCreateScriptNode(JNIEnv *env, jclass, jlong pContext, jstring strFormat, jobject phNode)
{
	XnNodeHandle hNode;
	XnStatus nRetVal = xnCreateScriptNode((XnContext*)pContext, JavaString(env, strFormat), &hNode);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgPointerValue(env, phNode, hNode);
	return XN_STATUS_OK;
}

JNIEXPORT jstring JNICALL Java_org_openni_NativeMethods_xnScriptNodeGetSupportedFormat(JNIEnv *env, jclass, jlong hNode)
{
	return env->NewStringUTF(xnScriptNodeGetSupportedFormat((XnNodeHandle)hNode));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnLoadScriptFromFile(JNIEnv *env, jclass, jlong hNode, jstring strFile)
{
	return xnLoadScriptFromFile((XnNodeHandle)hNode, JavaString(env, strFile));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnLoadScriptFromString(JNIEnv *env, jclass, jlong hNode, jstring strScript)
{
	return xnLoadScriptFromString((XnNodeHandle)hNode, JavaString(env, strScript));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnScriptNodeRun(JNIEnv *, jclass, jlong hNode, jlong pErrors)
{
	return xnScriptNodeRun((XnNodeHandle)hNode, (XnEnumerationErrors*)pErrors);
}

//---------------------------------------------------------------------------
// Utils
//---------------------------------------------------------------------------
JNIEXPORT jstring JNICALL Java_org_openni_NativeMethods_xnProductionNodeTypeToString(JNIEnv *env, jclass, jint type)
{
	return env->NewStringUTF(xnProductionNodeTypeToString(type));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnResolutionGetXRes(JNIEnv *, jclass, jint res)
{
	return xnResolutionGetXRes((XnResolution)res);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnResolutionGetYRes(JNIEnv *, jclass, jint res)
{
	return xnResolutionGetYRes((XnResolution)res);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnResolutionGetFromXYRes(JNIEnv *, jclass, jint xRes, jint yRes)
{
	return xnResolutionGetFromXYRes(xRes, yRes);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnResolutionGetFromName(JNIEnv *env, jclass, jstring strName)
{
	return xnResolutionGetFromName(JavaString(env, strName));
}

JNIEXPORT jstring JNICALL Java_org_openni_NativeMethods_xnResolutionGetName(JNIEnv *env, jclass, jint res)
{
	return env->NewStringUTF(xnResolutionGetName((XnResolution)res));
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetVersion(JNIEnv *env, jclass, jobject pVersion)
{
	XnVersion version;
	XnStatus nRetVal = xnGetVersion(&version);
	XN_IS_STATUS_OK(nRetVal);
	SetOutArgVersionValue(env, pVersion, &version);
	return XN_STATUS_OK;
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsTypeGenerator(JNIEnv *, jclass, jint type)
{
	return (jboolean)xnIsTypeGenerator(type);
}

JNIEXPORT jboolean JNICALL Java_org_openni_NativeMethods_xnIsTypeDerivedFrom(JNIEnv *, jclass, jint type, jint base)
{
	return (jboolean)xnIsTypeDerivedFrom(type, base);
}

JNIEXPORT jint JNICALL Java_org_openni_NativeMethods_xnGetBytesPerPixelForPixelFormat(JNIEnv *, jclass, jint format)
{
	return xnGetBytesPerPixelForPixelFormat((XnPixelFormat)format);
}
