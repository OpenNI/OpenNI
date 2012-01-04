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
#ifndef __XN_MODULE_CPP_INTERFACE_H__
#define __XN_MODULE_CPP_INTERFACE_H__

/**
 * NOTE: this file should never be compiled into OpenNI. It must always remain
 * in this h-file only (to make sure nothing breaks when installing new OpenNI DLL).
 * It is only provided as a syntactic sugar for exporting interfaces.
 */

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleInterface.h"
#include "XnList.h"
#include "XnCppWrapper.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
namespace xn
{
	XN_DECLARE_LIST(XnModuleGetExportedInterfacePtr, ExportedNodesList);

	class ModuleProductionNode;

	class Module
	{
	public:
		Module() {}
		virtual ~Module() {}

		virtual XnStatus Load() { return XN_STATUS_OK; }
		virtual void Unload() {}

		inline XnStatus AddExportedNode(XnModuleGetExportedInterfacePtr pEntryPoint) { return m_ExportedNodes.AddLast(pEntryPoint); }

		XnUInt32 GetExportedNodesCount() { return m_ExportedNodes.Size(); }

		XnStatus GetExportedNodes(XnModuleGetExportedInterfacePtr* aEntryPoints, XnUInt32 nCount)
		{
			if (nCount < m_ExportedNodes.Size())										
			{																
				return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;					
			}																

			XnUInt32 i = 0;												
			for (ExportedNodesList::ConstIterator it = m_ExportedNodes.begin();			
				it != m_ExportedNodes.end();											
				++it, ++i)													
			{																
				aEntryPoints[i] = *it;										
			}																

			return (XN_STATUS_OK);											
		}

	private:
		ExportedNodesList m_ExportedNodes;
	};

	class ExtensionModule : public Module
	{
	public:
		virtual XnStatus Load()
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			nRetVal = Module::Load();
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = RegisterNewTypes();
			XN_IS_STATUS_OK(nRetVal);
			
			return (XN_STATUS_OK);
		}

	protected:
		virtual XnStatus RegisterNewTypes() = 0;
	};

	class ModuleExportedProductionNode
	{
	public:
		ModuleExportedProductionNode() {}
		virtual ~ModuleExportedProductionNode() {}

		virtual void GetDescription(XnProductionNodeDescription* pDescription) = 0;
		virtual XnStatus EnumerateProductionTrees(Context& context, NodeInfoList& TreesList, EnumerationErrors* pErrors) = 0;
		virtual XnStatus Create(Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, ModuleProductionNode** ppInstance) = 0;
		virtual void Destroy(ModuleProductionNode* pInstance) = 0;
	};

	class ModuleExtendedSerializationInterface
	{
	public:
		virtual ~ModuleExtendedSerializationInterface() {}
		virtual XnStatus NotifyExState(XnNodeNotifications* pNotifications, void* pCookie) = 0;
		virtual void UnregisterExNotifications() = 0;
	};

	class ModuleLockAwareInterface
	{
	public:
		virtual ~ModuleLockAwareInterface() {}
		virtual XnStatus SetLockState(XnBool bLocked) = 0;
		virtual XnBool GetLockState() = 0;
		virtual XnStatus RegisterToLockChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromLockChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleErrorStateInterface
	{
	public:
		virtual ~ModuleErrorStateInterface() {}
		virtual XnStatus GetErrorState() = 0;
		virtual XnStatus RegisterToErrorStateChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromErrorStateChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleGeneralIntInterface
	{
	public:
		virtual XnStatus GetRange(const XnChar* strCap, XnInt32& nMin, XnInt32& nMax, XnInt32& nStep, XnInt32& nDefault, XnBool& bIsAutoSupported) = 0;
		virtual XnStatus Get(const XnChar* strCap, XnInt32& nValue) = 0;
		virtual XnInt32 Set(const XnChar* strCap, XnInt32 nValue) = 0;
		virtual XnStatus RegisterToValueChange(const XnChar* strCap, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromValueChange(const XnChar* strCap, XnCallbackHandle hCallback) = 0;
	};

	class ModuleProductionNode
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleProductionNode()
		{
			xnOSMemSet(m_aInterfaces, 0, sizeof(m_aInterfaces));
			m_aInterfaces[XN_NODE_TYPE_PRODUCTION_NODE] = this;
		}
#endif
		virtual ~ModuleProductionNode() {}
		virtual XnBool IsCapabilitySupported(const XnChar* /*strCapabilityName*/) { return FALSE; }
		virtual XnStatus SetIntProperty(const XnChar* /*strName*/, XnUInt64 /*nValue*/) { return XN_STATUS_ERROR; }
		virtual XnStatus SetRealProperty(const XnChar* /*strName*/, XnDouble /*dValue*/) { return XN_STATUS_ERROR; }
		virtual XnStatus SetStringProperty(const XnChar* /*strName*/, const XnChar* /*strValue*/) { return XN_STATUS_ERROR; }
		virtual XnStatus SetGeneralProperty(const XnChar* /*strName*/, XnUInt32 /*nBufferSize*/, const void* /*pBuffer*/) { return XN_STATUS_ERROR; }
		virtual XnStatus GetIntProperty(const XnChar* /*strName*/, XnUInt64& /*nValue*/) const { return XN_STATUS_ERROR; }
		virtual XnStatus GetRealProperty(const XnChar* /*strName*/, XnDouble& /*dValue*/) const { return XN_STATUS_ERROR; }
		virtual XnStatus GetStringProperty(const XnChar* /*strName*/, XnChar* /*csValue*/, XnUInt32 /*nBufSize*/) const { return XN_STATUS_ERROR; }
		virtual XnStatus GetGeneralProperty(const XnChar* /*strName*/, XnUInt32 /*nBufferSize*/, void* /*pBuffer*/) const { return XN_STATUS_ERROR; }
		virtual ModuleExtendedSerializationInterface* GetExtendedSerializationInterface() { return NULL; }
		virtual ModuleLockAwareInterface* GetLockAwareInterface() { return NULL; }
		virtual ModuleErrorStateInterface* GetErrorStateInterface() { return NULL; }
		virtual ModuleGeneralIntInterface* GetGeneralIntInterface(const XnChar* /*strCap*/) { return NULL; }

	public:
#ifdef XN_NO_DYNAMIC_CAST
		void* m_aInterfaces[XN_NODE_TYPE_FIRST_EXTENSION];
#endif
	};

	class ModuleDeviceIdentificationInterface
	{
	public:
		virtual ~ModuleDeviceIdentificationInterface() {}
		virtual XnStatus GetDeviceName(XnChar* strBuffer, XnUInt32& nBufferSize) = 0;
		virtual XnStatus GetVendorSpecificData(XnChar* strBuffer, XnUInt32& nBufferSize) = 0;
		virtual XnStatus GetSerialNumber(XnChar* strBuffer, XnUInt32& nBufferSize) = 0;
	};

	class ModuleDevice : virtual public ModuleProductionNode
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleDevice() { m_aInterfaces[XN_NODE_TYPE_DEVICE] = this; }
#endif
		virtual ~ModuleDevice() {}
		virtual ModuleDeviceIdentificationInterface* GetIdentificationInterface() { return NULL; }
	};

	class ModuleMirrorInterface
	{
	public:
		virtual ~ModuleMirrorInterface() {}
		virtual XnStatus SetMirror(XnBool bMirror) = 0;
		virtual XnBool IsMirrored() = 0;
		virtual XnStatus RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromMirrorChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleAlternativeViewPointInterface
	{
	public:
		virtual ~ModuleAlternativeViewPointInterface() {}
		virtual XnBool IsViewPointSupported(ProductionNode& other) = 0;
		virtual XnStatus SetViewPoint(ProductionNode& other) = 0;
		virtual XnBool IsViewPointAs(ProductionNode& other) = 0;
		virtual XnStatus ResetViewPoint() = 0;
		virtual XnStatus RegisterToViewPointChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromViewPointChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleFrameSyncInterface
	{
	public:
		virtual XnBool CanFrameSyncWith(ProductionNode& other) = 0;
		virtual XnStatus FrameSyncWith(ProductionNode& other) = 0;
		virtual XnStatus StopFrameSyncWith(ProductionNode& other) = 0;
		virtual XnBool IsFrameSyncedWith(ProductionNode& other) = 0;
		virtual XnStatus RegisterToFrameSyncChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromFrameSyncChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleGenerator : virtual public ModuleProductionNode
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleGenerator() { m_aInterfaces[XN_NODE_TYPE_GENERATOR] = this;}
#endif
		virtual ~ModuleGenerator() {}
		virtual XnStatus StartGenerating() = 0;
		virtual XnBool IsGenerating() = 0;
		virtual void StopGenerating() = 0;
		virtual XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback) = 0;
		virtual XnStatus RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback) = 0;
		virtual XnBool IsNewDataAvailable(XnUInt64& nTimestamp) = 0;
		virtual XnStatus UpdateData() = 0;
		virtual const void* GetData() = 0;
		virtual XnUInt32 GetDataSize() = 0;
		virtual XnUInt64 GetTimestamp() = 0;
		virtual XnUInt32 GetFrameID() = 0;
		virtual ModuleMirrorInterface* GetMirrorInterface() { return NULL; }
		virtual ModuleAlternativeViewPointInterface* GetAlternativeViewPointInterface() { return NULL; }
		virtual ModuleFrameSyncInterface* GetFrameSyncInterface() { return NULL; }
	};

	class ModuleNodeNotifications
	{
	public:
		virtual ~ModuleNodeNotifications() {}
		virtual XnStatus OnNodeAdded(const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression) = 0;
		virtual XnStatus OnNodeRemoved(const XnChar* strNodeName) = 0;
		virtual XnStatus OnNodeIntPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nValue) = 0;
		virtual XnStatus OnNodeRealPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dValue) = 0;
		virtual XnStatus OnNodeStringPropChanged(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue) = 0;
		virtual XnStatus OnNodeStateReady(const XnChar* strNodeName) = 0;
		virtual XnStatus OnNodeGeneralPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer) = 0;
		virtual XnStatus OnNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize) = 0;
	};

	class ModuleRecorder : 
		virtual public ModuleProductionNode, 
		virtual public ModuleNodeNotifications
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleRecorder() { m_aInterfaces[XN_NODE_TYPE_RECORDER] = this; }
#endif
		virtual ~ModuleRecorder() {}
		virtual XnStatus SetOutputStream(void* pCookie, XnRecorderOutputStreamInterface* pStream) = 0;
	};

	class ModulePlayer : virtual public ModuleProductionNode
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModulePlayer() { m_aInterfaces[XN_NODE_TYPE_PLAYER] = this; }
#endif
		virtual ~ModulePlayer() {}
		virtual XnStatus SetInputStream(void* pStreamCookie, XnPlayerInputStreamInterface* pStream) = 0;
		virtual XnStatus ReadNext() = 0;
		virtual XnStatus SetNodeNotifications(void* pNodeNotificationsCookie, XnNodeNotifications* pNodeNotifications) = 0;

		virtual XnStatus SetRepeat(XnBool bRepeat) = 0;
		virtual XnStatus SeekToTimeStamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin) = 0;
		virtual XnStatus SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin) = 0;
		virtual XnStatus TellTimestamp(XnUInt64& nTimestamp) = 0;
		virtual XnStatus TellFrame(const XnChar* strNodeName, XnUInt32& nFrame) = 0;
		virtual XnUInt32 GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames) = 0;
		virtual const XnChar* GetSupportedFormat() = 0;
		virtual XnBool IsEOF() = 0;
		virtual XnStatus RegisterToEndOfFileReached(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromEndOfFileReached(XnCallbackHandle hCallback) = 0;
	};

	class ModuleCroppingInterface
	{
	public:
		virtual ~ModuleCroppingInterface() {}
		virtual XnStatus SetCropping(const XnCropping &Cropping) = 0;
		virtual XnStatus GetCropping(XnCropping &Cropping) = 0;
		virtual XnStatus RegisterToCroppingChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromCroppingChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleAntiFlickerInterface
	{
	public:
		virtual ~ModuleAntiFlickerInterface() {}
		virtual XnStatus SetPowerLineFrequency(XnPowerLineFrequency nFrequency) = 0;
		virtual XnPowerLineFrequency GetPowerLineFrequency() = 0;
		virtual XnStatus RegisterToPowerLineFrequencyChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromPowerLineFrequencyChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleMapGenerator : virtual public ModuleGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleMapGenerator() { m_aInterfaces[XN_NODE_TYPE_MAP_GENERATOR] = this; }
#endif
		virtual ~ModuleMapGenerator() {}
		virtual XnUInt32 GetSupportedMapOutputModesCount() = 0;
		virtual XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount) = 0;
		virtual XnStatus SetMapOutputMode(const XnMapOutputMode& Mode) = 0;
		virtual XnStatus GetMapOutputMode(XnMapOutputMode& Mode) = 0;
		virtual XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback) = 0;
		virtual XnUInt32 GetBytesPerPixel() = 0;
		virtual ModuleCroppingInterface* GetCroppingInterface() { return NULL; }
		virtual ModuleAntiFlickerInterface* GetAntiFlickerInterface() { return NULL; }
	};

	class ModuleUserPositionInterface
	{
	public:
		virtual ~ModuleUserPositionInterface() {}
		virtual XnUInt32 GetSupportedUserPositionsCount() = 0;
		virtual XnStatus SetUserPosition(XnUInt32 nIndex, const XnBoundingBox3D& Position) = 0;
		virtual XnStatus GetUserPosition(XnUInt32 nIndex, XnBoundingBox3D& Position) = 0;
		virtual XnStatus RegisterToUserPositionChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromUserPositionChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleDepthGenerator : virtual public ModuleMapGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleDepthGenerator() { m_aInterfaces[XN_NODE_TYPE_DEPTH] = this; }
#endif
		virtual ~ModuleDepthGenerator() {}
		virtual const void* GetData() { return GetDepthMap(); }
		virtual XnUInt32 GetBytesPerPixel() { return sizeof(XnDepthPixel); }
		virtual XnDepthPixel* GetDepthMap() = 0;
		virtual XnDepthPixel GetDeviceMaxDepth() = 0;
		virtual void GetFieldOfView(XnFieldOfView& FOV) = 0;
		virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback) = 0;
		virtual ModuleUserPositionInterface* GetUserPositionInterface() { return NULL; }
	};

	class ModuleImageGenerator : virtual public ModuleMapGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleImageGenerator() { m_aInterfaces[XN_NODE_TYPE_IMAGE] = this; }
#endif
		virtual ~ModuleImageGenerator() {}
		virtual const void* GetData() { return GetImageMap(); }
		virtual XnUInt32 GetBytesPerPixel() { return xnGetBytesPerPixelForPixelFormat(GetPixelFormat()); }
		virtual XnUInt8* GetImageMap() = 0;
		virtual XnBool IsPixelFormatSupported(XnPixelFormat Format) = 0;
		virtual XnStatus SetPixelFormat(XnPixelFormat Format) = 0;
		virtual XnPixelFormat GetPixelFormat() = 0;
		virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback) = 0;
	};

	class ModuleIRGenerator : virtual public ModuleMapGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleIRGenerator() { m_aInterfaces[XN_NODE_TYPE_IR] = this; }
#endif
		virtual ~ModuleIRGenerator() {}
		virtual const void* GetData() { return GetIRMap(); }
		virtual XnUInt32 GetBytesPerPixel() { return sizeof(XnIRPixel); }
		virtual XnIRPixel* GetIRMap() = 0;
	};

	class ModuleGestureGenerator : virtual public ModuleGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleGestureGenerator() { m_aInterfaces[XN_NODE_TYPE_GESTURE] = this; }
#endif
		virtual ~ModuleGestureGenerator() {}
		virtual const void* GetData() { return NULL; }
		virtual XnStatus AddGesture(const XnChar* strGesture, XnBoundingBox3D* pArea) = 0;
		virtual XnStatus RemoveGesture(const XnChar* strGesture) = 0;
		virtual XnStatus GetActiveGestures(XnChar** pstrGestures, XnUInt16& nGestures) = 0;
		virtual XnStatus GetAllActiveGestures(XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16& nGestures) = 0;
		virtual XnStatus EnumerateGestures(XnChar** pstrGestures, XnUInt16& nGestures) = 0;
		virtual XnStatus EnumerateAllGestures(XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16& nGestures) = 0;
		virtual XnBool IsGestureAvailable(const XnChar* strGesture) = 0;
		virtual XnBool IsGestureProgressSupported(const XnChar* strGesture) = 0;
		virtual XnStatus RegisterGestureCallbacks(XnModuleGestureRecognized RecognizedCB, XnModuleGestureProgress ProgressCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterGestureCallbacks(XnCallbackHandle hCallback) = 0;
		virtual XnStatus RegisterToGestureChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromGestureChange(XnCallbackHandle hCallback) = 0;

		virtual XnStatus RegisterToGestureIntermediateStageCompleted(XnModuleGestureIntermediateStageCompleted GestureIntermediateStageCompletedCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromGestureIntermediateStageCompleted(XnCallbackHandle hCallback) = 0;
		virtual XnStatus RegisterToGestureReadyForNextIntermediateStage(XnModuleGestureReadyForNextIntermediateStage ReadyForNextIntermediateStageCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromGestureReadyForNextIntermediateStage(XnCallbackHandle hCallback) = 0;
	};

	class ModuleSceneAnalyzer : virtual public ModuleMapGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleSceneAnalyzer() { m_aInterfaces[XN_NODE_TYPE_SCENE] = this; }
#endif
		virtual ~ModuleSceneAnalyzer() {}
		virtual const void* GetData() { return GetLabelMap(); }
		virtual XnUInt32 GetBytesPerPixel() { return sizeof(XnLabel); }
		virtual const XnLabel* GetLabelMap() = 0;
		virtual XnStatus GetFloor(XnPlane3D& pPlane) = 0;
	};

	class ModuleHandTouchingFOVEdgeInterface
	{
	public:
		virtual ~ModuleHandTouchingFOVEdgeInterface() {}
		virtual XnStatus RegisterToHandTouchingFOVEdge(XnModuleHandTouchingFOVEdge TouchingFOVEdgeCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromHandTouchingFOVEdge(XnCallbackHandle hCallback) = 0;
	};

	class ModuleHandsGenerator : virtual public ModuleGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleHandsGenerator() { m_aInterfaces[XN_NODE_TYPE_HANDS] = this; }
#endif
		virtual ~ModuleHandsGenerator() {}
		virtual const void* GetData() { return NULL; }
		virtual XnStatus RegisterHandCallbacks(XnModuleHandCreate CreateCB, XnModuleHandUpdate UpdateCB, XnModuleHandDestroy DestroyCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterHandCallbacks(XnCallbackHandle hCallback) = 0;
		virtual XnStatus StopTracking(XnUserID user) = 0;
		virtual XnStatus StopTrackingAll() = 0;
		virtual XnStatus StartTracking(const XnPoint3D& ptPosition) = 0;
		virtual XnStatus SetSmoothing(XnFloat fSmoothingFactor) = 0;

		virtual ModuleHandTouchingFOVEdgeInterface* GetHandTouchingFOVEdgeInterface() { return NULL; }
	};

	class ModuleSkeletonInterface
	{
	public:
		virtual ~ModuleSkeletonInterface() {}
		virtual XnBool IsJointAvailable(XnSkeletonJoint eJoint) = 0;
		virtual XnBool IsProfileAvailable(XnSkeletonProfile eProfile) = 0;
		virtual XnStatus SetSkeletonProfile(XnSkeletonProfile eProfile) = 0;
		virtual XnStatus SetJointActive(XnSkeletonJoint eJoint, XnBool bState) = 0;
		virtual XnBool IsJointActive(XnSkeletonJoint eJoint) = 0;
		virtual XnStatus RegisterToJointConfigurationChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromJointConfigurationChange(XnCallbackHandle hCallback) = 0;
		virtual XnStatus EnumerateActiveJoints(XnSkeletonJoint* pJoints, XnUInt16& nJoints) = 0;
		virtual XnStatus GetSkeletonJoint(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation& jointTransformation) = 0;
		virtual XnStatus GetSkeletonJointPosition(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition& pJointPosition) = 0;
		virtual XnStatus GetSkeletonJointOrientation(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation& pJointOrientation) = 0;
		virtual XnBool IsTracking(XnUserID user) = 0;
		virtual XnBool IsCalibrated(XnUserID user) = 0;
		virtual XnBool IsCalibrating(XnUserID user) = 0;
		virtual XnStatus RequestCalibration(XnUserID user, XnBool bForce) = 0;
		virtual XnStatus AbortCalibration(XnUserID user) = 0;
		virtual XnStatus SaveCalibrationDataToFile(XnUserID user, const XnChar* strFileName) = 0;
		virtual XnStatus LoadCalibrationDataFromFile(XnUserID user, const XnChar* strFileName) = 0;
		virtual XnStatus SaveCalibrationData(XnUserID user, XnUInt32 nSlot) = 0;
		virtual XnStatus LoadCalibrationData(XnUserID user, XnUInt32 nSlot) = 0;
		virtual XnStatus ClearCalibrationData(XnUInt32 nSlot) = 0;
		virtual XnBool IsCalibrationData(XnUInt32 nSlot) = 0;
		virtual XnStatus StartTracking(XnUserID user) = 0;
		virtual XnStatus StopTracking(XnUserID user) = 0;
		virtual XnStatus Reset(XnUserID user) = 0;
		virtual XnBool NeedPoseForCalibration() = 0;
		virtual XnStatus GetCalibrationPose(XnChar* strPose) = 0;
		virtual XnStatus SetSmoothing(XnFloat fSmoothingFactor) = 0;
		virtual XnStatus RegisterCalibrationCallbacks(XnModuleCalibrationStart CalibrationStartCB, XnModuleCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterCalibrationCallbacks(XnCallbackHandle hCallback) = 0;

		virtual XnStatus RegisterToCalibrationInProgress(XnModuleCalibrationInProgress CalibrationInProgressCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromCalibrationInProgress(XnCallbackHandle hCallback) = 0;
		virtual XnStatus RegisterToCalibrationComplete(XnModuleCalibrationComplete CalibrationCompleteCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromCalibrationComplete(XnCallbackHandle hCallback) = 0;

		virtual XnStatus RegisterToCalibrationStart(XnModuleCalibrationStart handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromCalibrationStart(XnCallbackHandle hCallback) = 0;
	};

	class ModulePoseDetectionInteface
	{
	public:
		virtual ~ModulePoseDetectionInteface() {}

		virtual XnUInt32 GetNumberOfPoses() = 0;
		virtual XnStatus GetAvailablePoses(XnChar** pstrPoses, XnUInt32& nPoses) = 0;
		virtual XnStatus GetAllAvailablePoses(XnChar** pstrPoses, XnUInt32 nNameLength, XnUInt32& nPoses) = 0;

		virtual XnStatus StartPoseDetection(const XnChar* strPose, XnUserID user) = 0;
		virtual XnStatus StopPoseDetection(XnUserID user) = 0;
		virtual XnStatus StopSinglePoseDetection(XnUserID user, const XnChar* strPose) = 0;

		virtual XnStatus RegisterToPoseDetectionCallbacks(XnModulePoseDetectionCallback StartPoseCB, XnModulePoseDetectionCallback EndPoseCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromPoseDetectionCallbacks(XnCallbackHandle hCallback) = 0;

		virtual XnStatus RegisterToPoseDetectionInProgress(XnModulePoseDetectionInProgressCallback InProgressCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromPoseDetectionInProgress(XnCallbackHandle hCallback) = 0;

		virtual XnStatus RegisterToPoseDetected(XnModulePoseDetectionCallback handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual XnStatus RegisterToOutOfPose(XnModulePoseDetectionCallback handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromPoseDetected(XnCallbackHandle hCallback) = 0;
		virtual void UnregisterFromOutOfPose(XnCallbackHandle hCallback) = 0;
	};

	class ModuleUserGenerator : virtual public ModuleGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleUserGenerator() { m_aInterfaces[XN_NODE_TYPE_USER] = this;}
#endif
		virtual ~ModuleUserGenerator() {}
		virtual const void* GetData() { return NULL; }
		virtual XnUInt16 GetNumberOfUsers() = 0;
		virtual XnStatus GetUsers(XnUserID* pUsers, XnUInt16& nUsers) = 0;
		virtual XnStatus GetCoM(XnUserID user, XnPoint3D& com) = 0;
		virtual XnStatus GetUserPixels(XnUserID user, XnSceneMetaData *pScene) = 0;
		virtual XnStatus RegisterUserCallbacks(XnModuleUserHandler NewUserCB, XnModuleUserHandler LostUserCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterUserCallbacks(XnCallbackHandle hCallback) = 0;
		virtual ModuleSkeletonInterface* GetSkeletonInterface() { return NULL; }
		virtual ModulePoseDetectionInteface* GetPoseDetectionInteface() {return NULL;}

		virtual XnStatus RegisterToUserExit(XnModuleUserHandler UserExitCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromUserExit(XnCallbackHandle hCallback) = 0;
		virtual XnStatus RegisterToUserReEnter(XnModuleUserHandler UserReEnterCB, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromUserReEnter(XnCallbackHandle hCallback) = 0;
	};

	class ModuleAudioGenerator : virtual public ModuleGenerator
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleAudioGenerator() { m_aInterfaces[XN_NODE_TYPE_AUDIO] = this; }
#endif
		virtual ~ModuleAudioGenerator() {}
		virtual const void* GetData() { return GetAudioBuffer(); }
		virtual XnUChar* GetAudioBuffer() = 0;
		virtual XnUInt32 GetSupportedWaveOutputModesCount() = 0;
		virtual XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount) = 0;
		virtual XnStatus SetWaveOutputMode(const XnWaveOutputMode& OutputMode) = 0;
		virtual XnStatus GetWaveOutputMode(XnWaveOutputMode& OutputMode) = 0;
		virtual XnStatus RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) = 0;
		virtual void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback) = 0;
	};

	class ModuleCodec : virtual public ModuleProductionNode
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleCodec() { m_aInterfaces[XN_NODE_TYPE_CODEC] = this; }
#endif
		virtual ~ModuleCodec() {}
		virtual XnCodecID GetCodecID() const = 0;
		virtual XnStatus Init(const ProductionNode& node) = 0;
		virtual XnStatus CompressData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const = 0;
		virtual XnStatus DecompressData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const = 0;
	};

	class ModuleScriptNode : virtual public ModuleProductionNode
	{
	public:
#ifdef XN_NO_DYNAMIC_CAST
		ModuleScriptNode() { m_aInterfaces[XN_NODE_TYPE_SCRIPT] = this; }
#endif
		virtual ~ModuleScriptNode() {}
		virtual const XnChar* GetSupportedFormat() = 0;
		virtual XnStatus LoadScriptFromFile(const XnChar* strFileName) = 0;
		virtual XnStatus LoadScriptFromString(const XnChar* strScript) = 0;
		virtual XnStatus Run(NodeInfoList& createdNodes, EnumerationErrors& errors) = 0;
	};
}

#endif // __XN_MODULE_CPP_INTERFACE_H__
