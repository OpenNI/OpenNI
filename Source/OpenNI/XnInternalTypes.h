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

    /// @brief Internal class used to translate pose detection callbacks to an in frame status.
    class PosePrivateData : public NodePrivateData
    {
    public:
        PosePrivateData(); ///< constructor
        virtual ~PosePrivateData(); ///< destructor
        virtual void BeforeNodeDestroy(); ///< cleans the internals. 
        /// @brief Initializes the class.
        /// 
        /// @param hUserGenerator The user generator node which holds the pose detection capability.
        /// @return The success status.
        XnStatus Init(XnNodeHandle hUserGenerator); 
        /// @brief Gets the current pose status
        /// 
        /// @param userID The user whose pose status we are interested in.
        /// @param poseName The pose we want to get a status on.
        /// @param poseTime The time stamp in which the user entered into the pose (0 if not in pose). 
        /// @param eStatus The status of the user's pose, i.e. the progress error for getting into 
        ///                pose (XnPoseDetectionStatus, the same as received from the in progress
        ///                callback. See @ref xnRegisterToPoseDetectionInProgress).
        /// @param eState  The state of the user pose (i.e. in pose, out of pose).
        /// @return The success status. The data is invalid if failed.
        XnStatus GetPoseStatus(XnUserID userID, const XnChar* poseName, XnUInt64& poseTime, XnPoseDetectionStatus& eStatus, XnPoseDetectionState& eState);
    private:
        /// @brief Internal structure to hold the info for a single pose of a single user
        struct PoseData
        {
            XnUInt64 m_lastTimeInPose; ///< @brief The time stamp when we last entered pose (0 if not in pose)
            XnPoseDetectionStatus m_lastStatus; ///< @brief The last status message from in progress.
            XnPoseDetectionState m_lastState;  ///< @brief The last state (in/out of pose). Determined by pose detected and out of pose callbacks!
            PoseData() { Reset(); };
            /// @brief Resets the struct to the default state which is being out of pose, with a 
            /// general error and time 0.
            void Reset() 
            { 
                m_lastState=XN_POSE_DETECTION_STATE_OUT_OF_POSE; 
                m_lastStatus=XN_POSE_DETECTION_STATUS_ERROR;
                m_lastTimeInPose=0;
            }
        };
        /// @brief Hash to hold the pose data for users for a single pose
        XN_DECLARE_DEFAULT_HASH(XnUserID,PoseData,UsersPoseDataHash);
        /// @brief Structure to hold the data of a single pose
        struct UsersPoseData
        {
            UsersPoseDataHash m_usersHash; ///< @brief The users data
            XnChar *m_poseName;             ///< @brief The pose name.
        };

        UsersPoseData *m_usersPoseDataPerPose; ///< @brief An array which holds the status for each pose
        XnUInt32 m_numPoses;        ///< @brief The size of the m_usersPoseDataPerPose array.

        XnCallbackHandle m_userCallbacksHandle; ///< @brief Callback handle to unregister from user callbacks
        XnCallbackHandle m_poseDetectHandle; ///< @brief Callback handle to unregister from user detection callbacks
        XnCallbackHandle m_outOfPoseHandle; ///< @brief Callback handle to unregister from out of pose callbacks
        XnCallbackHandle m_inProgressHandle; ///< @brief Callback handle to unregister from pose in progress callbacks
        XnNodeHandle m_hUserGenerator; ///< @brief The user generator everything relates to.

        /// @brief callback function
        static void XN_CALLBACK_TYPE XnNewUserCallback(XnNodeHandle hNode, XnUserID nUserId, void* pCookie);
        /// @brief callback function
        static void XN_CALLBACK_TYPE XnLostUserCallback(XnNodeHandle hNode, XnUserID nUserId, void* pCookie);
        /// @brief callback function
        static void XN_CALLBACK_TYPE XnPoseInProgressCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID nUserId, XnPoseDetectionStatus ePoseError, void* pCookie);
        /// @brief callback function
        static void XN_CALLBACK_TYPE XnPoseDetectedCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID nUserId, void* pCookie);
        /// @brief callback function
        static void XN_CALLBACK_TYPE XnOutOfPoseDetectedCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID nUserId, void* pCookie);
    };
}

struct XnModuleStateCookie; // Forward Declaration
XN_DECLARE_DEFAULT_HASH(XnModuleStateCookie*, XnModuleStateCookie*, XnModuleStateCookieHash);

struct XnInternalNodeData
{
	XnBitSet* pTypeHierarchy;
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
	XnDumpFile* pDumpRefCount;
	XnDumpFile* pDumpDataFlow;
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
