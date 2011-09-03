/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#ifndef __XN_INTERNAL_TYPES_H__
#define __XN_INTERNAL_TYPES_H__

#include <XnPlatform.h>
#include <XnHash.h>
#include <XnEvent.h>
#include <XnTypes.h>
#include <XnStringsHash.h>
#include <XnFPSCalculator.h>
#include <XnBitSet.h>
#include <XnDump.h>

#define XN_OPEN_NI_XML_ROOT_NAME	"OpenNI"

struct XnModuleInstance;
typedef struct XnModuleInstance ModuleInstance;
struct XnNodeInfo;
typedef struct XnNodeInfo XnNodeInfo;

typedef struct XnNeededNodeData
{
	XnUInt64 nLastReadTimestamp;
} XnNeededNodeData;

XN_DECLARE_DEFAULT_HASH(XnNodeHandle, XnNeededNodeData*, XnNeededNodesDataHash)

typedef struct XnLockData
{
	XnUInt32 nCurrentLock;
	XN_THREAD_ID nCurrentThread;
	XnCallbackHandle hLockChangedCallback;
} XnLockData;

namespace xn
{
	class NodePrivateData
	{
	public:
		virtual ~NodePrivateData() {}
		virtual void BeforeNodeDestroy() = 0;
	};

	class DepthPrivateData : public NodePrivateData
	{
	public:
		DepthPrivateData();
		virtual ~DepthPrivateData();
		virtual void BeforeNodeDestroy();

		XnStatus Init(XnNodeHandle hDepth);

		inline XnDouble GetRealWorldXtoZ() { return m_fRealWorldXtoZ; }
		inline XnDouble GetRealWorldYtoZ() { return m_fRealWorldYtoZ; }

	private:
		void OnFieldOfViewChanged();
		static void XN_CALLBACK_TYPE FieldOfViewChangedCallback(XnNodeHandle hNode, void* pCookie);

		XnDouble m_fRealWorldXtoZ;
		XnDouble m_fRealWorldYtoZ;
		XnNodeHandle m_hNode;
		XnCallbackHandle m_hFOVCallbackHandle;
	};
}

struct XnModuleStateCookie; // Forward Declaration
XN_DECLARE_DEFAULT_HASH(XnModuleStateCookie*, XnModuleStateCookie*, XnModuleStateCookieHash);

struct XnInternalNodeData
{
	XnBitSet typeHierarchy;
	XnModuleInstance* pModuleInstance;
	XnNodeInfo* pNodeInfo;
	XnUInt32 nRefCount;
	XnContext* pContext;
	XnBool bIsNewData;
	XnCallbackHandle hNewDataCallback;
	XnNeededNodesDataHash* pNeededNodesDataHash; // holds data specific for this edge.
	XnModuleStateCookieHash* pRegistrationCookiesHash;
	XnLockData LockData;
	XnCallbackHandle hErrorStateCallback;
	XnNodeHandle hFrameSyncedWith;
	XnCallbackHandle hFrameSyncCallback;
	XnFPSData genFPS;
	XnFPSData readFPS;
	union
	{
		XnDepthMetaData* Depth;
		XnImageMetaData* Image;
		XnIRMetaData* IR;
		XnAudioMetaData* Audio;
		XnSceneMetaData* Scene;
		void* General;
	} pMetaData;
	XnBool* pbMetaDataIsNewFlag;
	xn::NodePrivateData* pPrivateData;
	XnBool bWasDataRead; // Changes to TRUE on the first UpdateData() called.
	XN_CRITICAL_SECTION_HANDLE hLock;
	XnBool bIsOwnedByContext;
};

struct XnGestureRecognizedParams
{
	XnGestureRecognizedParams(const XnChar *strGesture, const XnPoint3D &idPosition, const XnPoint3D &endPosition) :
		m_idPosition(idPosition),
		m_endPosition(endPosition)
	{
		xnOSStrNCopy(m_strGesture, strGesture, sizeof(m_strGesture)-1, sizeof(m_strGesture));
	}
	XnChar m_strGesture[XN_MAX_NAME_LENGTH];
	XnPoint3D m_idPosition;
	XnPoint3D m_endPosition;
};

struct XnGestureProgressParams
{
	XnGestureProgressParams(const XnChar *strGesture, const XnPoint3D &position, XnFloat fProgress) :
		m_position(position), m_fProgress(fProgress) 
	{
		xnOSStrNCopy(m_strGesture, strGesture, sizeof(m_strGesture)-1, sizeof(m_strGesture));
	}
	XnChar m_strGesture[XN_MAX_NAME_LENGTH]; 
	XnPoint3D m_position;
	XnFloat m_fProgress;
};

//////////////////////////////////////////////////////////////////////////////////////////
/** Declared licenses list. */
XN_DECLARE_LIST(XnLicense, XnLicenseList)

XN_DECLARE_STRINGS_HASH(XnInternalNodeData*, XnNodesMap)

XN_DECLARE_DEFAULT_HASH(const XnNodeInfo*, XnValue, XnProductionNodesSet)

XN_DECLARE_EVENT_1ARG(XnErrorStateChangedEvent, IXnErrorStateChangedEvent, XnStatus, errorState)
XN_DECLARE_EVENT_1ARG(XnContextShuttingDownEvent, IXnContextShuttingDownEvent, XnContext*, pContext);

class XnModuleLoader;

/** NI Context. */
struct XnContext
{
	XnLicenseList* pLicenses;
	XnModuleLoader* pModuleLoader;
	XnNodesMap* pNodesMap;
	XnBool bGlobalMirrorSet;
	XnBool bGlobalMirror;
	XnStatus globalErrorState;
	XnErrorStateChangedEvent* pGlobalErrorChangeEvent;
	XN_EVENT_HANDLE hNewDataEvent;
	XnUInt32 nLastLockID;
	XnFPSData readFPS;
	XnUInt32 nRefCount;
	XN_CRITICAL_SECTION_HANDLE hLock;
	XnNodeInfoList* pOwnedNodes;
	XnDump dumpRefCount;
	XnContextShuttingDownEvent* pShutdownEvent;
};

struct XnNodeInfo
{
	/** Description of this generator. */
	XnProductionNodeDescription Description;

	/** The name of the instance. */
	XnChar strInstanceName[XN_MAX_NAME_LENGTH];

	/** Additional creation info (this can contain S/N, etc.) */
	XnChar strCreationInfo[XN_MAX_CREATION_INFO_LENGTH];

	/** 
	 * A handle that can be used for production nodes functions. A value of NULL signifies an
	 * option that currently does not exist in the context.
	 */
	XnNodeHandle hNode;

	/** A list of needed nodes for this instance to work. */
	XnNodeInfoList* pNeededTrees;

	/** Ref count */
	XnUInt32 nRefCount;

	/** additional data */
	const void* pAdditionalData;

	/** Callback function to be called before freeing the object. */
	XnFreeHandler pAdditionalDataFreeFunc;
};

struct XnNodeInfoListNode
{
	/** Current node information. */
	XnNodeInfo* pCurrent;

	/** A pointer to the previous node in the list. */
	XnNodeInfoListNode* pPrev;

	/** A pointer to the next node in the list. */
	XnNodeInfoListNode* pNext;
};

struct XnNodeInfoList
{
	XnNodeInfoListNode* pFirst;
	XnNodeInfoListNode* pLast;
};

#endif //__XN_INTERNAL_TYPES_H__
