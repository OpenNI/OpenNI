/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




#ifndef __XN_MODULE_INTERFACE_H__
#define __XN_MODULE_INTERFACE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnTypes.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MODULE_LOAD								xnModuleLoad
#define XN_MODULE_UNLOAD							xnModuleUnload
#define XN_MODULE_GET_EXPORTED_NODES_COUNT			xnModuleGetExportedNodesCount
#define XN_MODULE_GET_EXPORTED_NODES_ENTRY_POINTS	xnModuleGetExportedNodesEntryPoints
#define XN_MODULE_GET_OPEN_NI_VERSION				xnModuleGetOpenNIVersion

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
struct XnModuleExportedProductionNodeInterface;
struct XnModuleProductionNodeInterface;
struct XnModuleDeviceInterface;
struct XnModuleDepthGeneratorInterface;
struct XnModuleImageGeneratorInterface;
struct XnModuleIRGeneratorInterface;
struct XnModuleGestureGeneratorInterface;
struct XnModuleUserGeneratorInterface;
struct XnModuleHandsGeneratorInterface;
struct XnModuleSceneAnalyzerInterface;
struct XnModuleAudioGeneratorInterface;
struct XnModuleRecorderInterface;
struct XnModulePlayerInterface;
struct XnModuleGeneratorInterface;
struct XnModuleCodecInterface;

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

typedef void (XN_CALLBACK_TYPE* XnModuleGetExportedInterfacePtr)(XnModuleExportedProductionNodeInterface* pInterface);
typedef XnStatus (XN_C_DECL* XnModuleLoadPtr)();
typedef XnStatus (XN_C_DECL* XnModuleUnloadPtr)();
typedef XnUInt32 (XN_C_DECL* XnModuleGetExportedNodesCountPtr)();
typedef XnStatus (XN_C_DECL* XnModuleGetExportedNodesEntryPointsPtr)(XnModuleGetExportedInterfacePtr* aEntryPoints, XnUInt32 nCount);
typedef void (XN_C_DECL* XnModuleGetOpenNIVersionPtr)(XnVersion* pVersion);

/** Prototype for state change callback function. **/
typedef void (XN_CALLBACK_TYPE* XnModuleStateChangedHandler)(void* pCookie);

// User
typedef void (XN_CALLBACK_TYPE* XnModuleUserHandler)(XnUserID user, void* pCookie);

// UI
typedef void (XN_CALLBACK_TYPE* XnModuleHandCreate)(XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnModuleHandUpdate)(XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnModuleHandDestroy)(XnUserID user, XnFloat fTime, void* pCookie);

// Gesture Module
typedef void (XN_CALLBACK_TYPE* XnModuleGestureRecognized)(const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnModuleGestureProgress)(const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie);

// Skeleton
typedef void (XN_CALLBACK_TYPE* XnModuleCalibrationStart)(XnUserID user, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnModuleCalibrationEnd)(XnUserID user, XnBool bSuccess, void* pCookie);

// Pose Detection
typedef void (XN_CALLBACK_TYPE* XnModulePoseDetectionCallback)(const XnChar* strPose, XnUserID user, void* pCookie);

typedef struct XnModuleExportedProductionNodeInterface
{
	/** 
	* Gets a description of this generator. 
	* 
	* @param	pDescription	[in/out]	A struct to be filled with the description.
	*/
	void (XN_CALLBACK_TYPE* GetDescription)
		(XnProductionNodeDescription* pDescription);

	/** 
	* Find all tree possibilities for this production node.
	*
	* @param	pContext		[in]	Current OpenNI context
	* @param	pNodesList		[in]	A list to be filled with production trees.
	* @param	pErrors			[in]	Optional. An enumeration errors object to be passed to enumeration functions.
	*/
	XnStatus (XN_CALLBACK_TYPE* EnumerateProductionTrees)
		(XnContext* pContext,
		XnNodeInfoList* pNodesList,
		XnEnumerationErrors* pErrors);

	/** 
	* Creates an instance of this generator. 
	*
	* @param	pContext			[in]	Current OpenNI context
	* @param	strInstanceName		[in]	The name of this new instance.
	* @param	strCreationInfo		[in]	Optional creation info (returned from enumeration).
	* @param	pNeededNodes		[in]	A list of inputs for this generator.
	* @param	strConfigurationDir	[in]	The module configuration dir, or NULL if it doesn't have one.
	* @param	phInstance			[out]	A handle to the created instance.
	*/
	XnStatus (XN_CALLBACK_TYPE* Create)
		(XnContext* pContext,
		const XnChar* strInstanceName, 
		const XnChar* strCreationInfo,
		XnNodeInfoList* pNeededNodes, 
		const XnChar* strConfigurationDir,
		XnModuleNodeHandle* phInstance);

	/** 
	* Destroys an instance previously created using Create(). 
	* 
	* @param	hGenerator	[in]	A handle to the instance to be destroyed.
	*/
	void (XN_CALLBACK_TYPE* Destroy)
		(XnModuleNodeHandle hInstance);

	/**
	 * Fills a struct with the entire interface exported. The type of the struct passed
	 * to this function is determined according to the type this production node declares itself.
	 * For example, a depth generator will be passed a pointer to a XnModuleDepthGeneratorInterface struct.
	 *
	 * @param	pInterface		[in/out]	A pointer to a struct to be filled.
	 */
	union
	{
		void (XN_CALLBACK_TYPE* Device)(XnModuleDeviceInterface* pInterface);
		void (XN_CALLBACK_TYPE* Depth)(XnModuleDepthGeneratorInterface* pInterface);
		void (XN_CALLBACK_TYPE* Image)(XnModuleImageGeneratorInterface* pInterface);
		void (XN_CALLBACK_TYPE* IR)(XnModuleIRGeneratorInterface* pInterface);
		void (XN_CALLBACK_TYPE* User)(XnModuleUserGeneratorInterface* pInterface);
		void (XN_CALLBACK_TYPE* Hands)(XnModuleHandsGeneratorInterface* pInterace);
		void (XN_CALLBACK_TYPE* Gesture)(XnModuleGestureGeneratorInterface* pInterface);
		void (XN_CALLBACK_TYPE* Scene)(XnModuleSceneAnalyzerInterface* pInterface);
		void (XN_CALLBACK_TYPE* Audio)(XnModuleAudioGeneratorInterface* pInterface);
		void (XN_CALLBACK_TYPE* Recorder)(XnModuleRecorderInterface* pInterface);
		void (XN_CALLBACK_TYPE* Player)(XnModulePlayerInterface* pInterface);
		void (XN_CALLBACK_TYPE* Codec)(XnModuleCodecInterface* pInterface);

		void (XN_CALLBACK_TYPE* General)(void*);
	} GetInterface;

} XnModuleExportedProductionNodeInterface;

typedef struct XnModuleExtendedSerializationInterface
{
	XnStatus (XN_CALLBACK_TYPE* InitNotifications)(XnModuleNodeHandle hInstance, XnNodeNotifications* pNotifications, void* pCookie);
	void (XN_CALLBACK_TYPE* StopNotifications)(XnModuleNodeHandle hInstance);

} XnModuleExtendedSerializationInterface;

typedef struct XnModuleLockAwareInterface
{
	/**
	 * Notifies the node its lock state was changed.
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 * @param	bLocked		[in]	Current lock state.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetLockState)(XnModuleNodeHandle hInstance, XnBool bLocked);

	/**
	 * Gets current lock state.
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 */
	XnBool (XN_CALLBACK_TYPE* GetLockState)(XnModuleNodeHandle hInstance);

	/**
	 * Registers a callback function to lock changes.
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when lock changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromLockChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToLockChange)
		(XnModuleNodeHandle hInstance, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToLockChange().
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToLockChange().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromLockChange)
		(XnModuleNodeHandle hInstance, XnCallbackHandle hCallback);

} XnModuleLockAwareInterface;

typedef struct XnModuleErrorStateInterface
{
	/**
	 * Gets current error state of this node.
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 */
	XnStatus (XN_CALLBACK_TYPE* GetErrorState)(XnModuleNodeHandle hInstance);

	/**
	 * Registers a callback function to error state changes.
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when lock changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromErrorStateChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToErrorStateChange)
		(XnModuleNodeHandle hInstance, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToErrorStateChange().
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToErrorStateChange().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromErrorStateChange)
		(XnModuleNodeHandle hInstance, XnCallbackHandle hCallback);

} XnModuleErrorStateInterface;

typedef struct XnModuleProductionNodeInterface
{
	/** 
	* Checks if the specified capability is supported. 
	* 
	* @param	hInstance			[in]	A handle to the instance to be queried.
	* @param	strCapabilityName	[in]	The name of the capability to check.
	*/
	XnBool (XN_CALLBACK_TYPE* IsCapabilitySupported)(
		XnModuleNodeHandle hInstance, 
		const XnChar* strCapabilityName
		);

	/**
	 * [Optional] Sets a property.
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 * @param	strName		[in]	Property name
	 * @param	nValue		[in]	New value
	 */
	XnStatus (XN_CALLBACK_TYPE* SetIntProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt64 nValue);
	XnStatus (XN_CALLBACK_TYPE* SetRealProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, XnDouble dValue);
	XnStatus (XN_CALLBACK_TYPE* SetStringProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, const XnChar* strValue);
	XnStatus (XN_CALLBACK_TYPE* SetGeneralProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

	/**
	 * [Optional] Gets a property.
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 * @param	strName		[in]	Property name
	 * @param	pnValue		[out]	Current value
	 */
	XnStatus (XN_CALLBACK_TYPE* GetIntProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt64* pnValue);
	XnStatus (XN_CALLBACK_TYPE* GetRealProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, XnDouble* pdValue);
	XnStatus (XN_CALLBACK_TYPE* GetStringProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize);
	XnStatus (XN_CALLBACK_TYPE* GetGeneralProperty)(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer);

	XnModuleExtendedSerializationInterface* pExtendedSerializationInterface;
	XnModuleLockAwareInterface* pLockAwareInterface;
	XnModuleErrorStateInterface* pErrorStateInterface;

} XnModuleProductionNodeInterface;

typedef struct XnModuleDeviceInterface
{
	XnModuleProductionNodeInterface* pProductionNode;
} XnModuleDeviceInterface;

typedef struct XnModuleMirrorInterface
{
	/**
	 * Sets current mirror configuration.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	bMirror		[in]	TRUE for mirroring output, FALSE otherwise.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetMirror)(XnModuleNodeHandle hInstance, XnBool bMirror);

	/**
	 * Gets current mirroring configuration.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 */
	XnBool (XN_CALLBACK_TYPE* IsMirrored)(XnModuleNodeHandle hInstance);

	/**
	 * Registers a callback function to mirror changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when mirror changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromMirrorChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToMirrorChange)
		(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToMirrorChange().
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToMirrorChange().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromMirrorChange)
		(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleMirrorInterface;

typedef struct XnModuleAlternativeViewPointInterface
{
	/**
	 * Checks if this generator can change its output to look like as if it was taken from 
	 * a different location, represented by another generator.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hOther		[in]	The view point to be checked.
	 *
	 * @return TRUE if view point is supported, FALSE otherwise.
	 */
	XnBool (XN_CALLBACK_TYPE* IsViewPointSupported)(XnModuleNodeHandle hGenerator, XnNodeHandle hOther);

	/**
	 * Sets the view point of this generator to look like as if placed at another generator location.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hOther		[in]	The view point to be set.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetViewPoint)(XnModuleNodeHandle hGenerator, XnNodeHandle hOther);

	/**
	 * Checks if current view point is as if coming from the other node view point.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hOther		[in]	The view point to be checked.
	 */
	XnBool (XN_CALLBACK_TYPE* IsViewPointAs)(XnModuleNodeHandle hGenerator, XnNodeHandle hOther);

	/**
	 * Sets the view point of this generator to its normal one.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 */
	XnStatus (XN_CALLBACK_TYPE* ResetViewPoint)(XnModuleNodeHandle hGenerator);

	/**
	 * Registers a callback function to view point changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when view point changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromViewPointChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToViewPointChange)(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToViewPointChange().
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToViewPointChange().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromViewPointChange)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleAlternativeViewPointInterface;

typedef struct XnModuleFrameSyncInterface
{
	/**
	 * Checks if this generator can frame sync with another generator.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hOther		[in]	The node to be checked.
	 *
	 * @returns TRUE if frame sync is supported, FALSE otherwise.
	 */
	XnBool (XN_CALLBACK_TYPE* CanFrameSyncWith)(XnModuleNodeHandle hGenerator, XnNodeHandle hOther);

	/**
	 * Activates frame sync with another node
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hOther		[in]	The node to sync with.
	 */
	XnStatus (XN_CALLBACK_TYPE* FrameSyncWith)(XnModuleNodeHandle hGenerator, XnNodeHandle hOther);

	/**
	 * Stops frame sync with another node.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hOther		[in]	The node to stop sync with.
	 */
	XnStatus (XN_CALLBACK_TYPE* StopFrameSyncWith)(XnModuleNodeHandle hGenerator, XnNodeHandle hOther);

	/**
	 * Checks if frame synced with other node.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hOther		[in]	The node to be checked.
	 */
	XnBool (XN_CALLBACK_TYPE* IsFrameSyncedWith)(XnModuleNodeHandle hGenerator, XnNodeHandle hOther);

	/**
	 * Registers a callback function to frame sync changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when frame sync changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromFrameSyncChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToFrameSyncChange)(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToFrameSyncChange().
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToFrameSyncChange().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromFrameSyncChange)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleFrameSyncInterface;

typedef struct XnModuleSeekingInterface
{
	XnUInt64 (XN_CALLBACK_TYPE* GetMinTimestamp)(XnModuleNodeHandle hGenerator);
	XnUInt64 (XN_CALLBACK_TYPE* GetMaxTimestamp)(XnModuleNodeHandle hGenerator);
	XnStatus (XN_CALLBACK_TYPE* SeekToTimestamp)(XnModuleNodeHandle hGenerator, XnUInt64 nTimestamp);

} XnModuleSeekingInterface;

/** The interface of a generator. */
typedef struct XnModuleGeneratorInterface
{
	/**
	 * Contains production node interface.
	 */
	XnModuleProductionNodeInterface* pProductionNodeInterface;

	/** 
	 * Starts generation of the output. 
	 * 
	 * @param	hGenerator	[in]	A handle to the instance to start generating.
	 */
	XnStatus (XN_CALLBACK_TYPE* StartGenerating)
		(XnModuleNodeHandle hGenerator);

	/** 
	 * Checks if the generator is currently generating.
	 * 
	 * @param	hGenerator	[in]	A handle to the instance.
	 */
	XnBool (XN_CALLBACK_TYPE* IsGenerating)
		(XnModuleNodeHandle hGenerator);

	/** 
	 * Stops generation of the output.
	 * 
	 * @param	hGenerator	[in]	A handle to the instance to stop generating.
	 */
	void (XN_CALLBACK_TYPE* StopGenerating)
		(XnModuleNodeHandle hGenerator);

	/**
	 * Registers a callback function to be called when generation starts or stops.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when generation starts/stops.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromGenerationRunningChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToGenerationRunningChange)
		(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToGenerationRunningChange().
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToGenerationRunningChange().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromGenerationRunningChange)
		(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	/**
	 * Registers a callback function to be called when new data is available.
	 *
	 * @param	handler		[in]	A pointer to a function that will be called when new data is available.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromNewDataAvailable().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToNewDataAvailable)
		(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToNewDataAvailable().
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToNewDataAvailable().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromNewDataAvailable)
		(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	/** 
	* Checks whether this node has new data (and so a call to @ref xn::Generator::WaitAndUpdateData() will not block).
	*
	* @param	hGenerator	[in]	A handle to the instance to be updated.
	* @param	pnTimestamp	[out]	If new data is available, the timestamp of that data.
	*/
	XnBool (XN_CALLBACK_TYPE* IsNewDataAvailable)
		(XnModuleNodeHandle hGenerator, XnUInt64* pnTimestamp);

	/** 
	* Updates the data to the latest available one. This function will only be called AFTER the node
	* notified OpenNI it has new data available.
	*
	* @param	hGenerator	[in]	A handle to the instance to be updated.
	*/
	XnStatus (XN_CALLBACK_TYPE* UpdateData)
		(XnModuleNodeHandle hGenerator);

	/**
	 * Gets the size of current data, in bytes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 */
	XnUInt32 (XN_CALLBACK_TYPE* GetDataSize)(XnModuleNodeHandle hGenerator);

	/**
	 * Gets the timestamp of current data, in microseconds.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 */
	XnUInt64 (XN_CALLBACK_TYPE* GetTimestamp)(XnModuleNodeHandle hGenerator);

	/**
	 * Gets the frame ID of current data.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 */
	XnUInt32 (XN_CALLBACK_TYPE* GetFrameID)(XnModuleNodeHandle hGenerator);

	XnModuleMirrorInterface* pMirrorInterface;
	XnModuleAlternativeViewPointInterface* pAlternativeViewPointInterface;
	XnModuleSeekingInterface* pSeekingInterface;
	XnModuleFrameSyncInterface* pFrameSyncInterface;

} XnModuleGeneratorInterface;

typedef struct XnModuleRecorderInterface
{
	/*
	 * Sets the output stream for the recorder module.
	 *
	 * @param	hInstance		[in]	A handle to the instance.
	 * @param	pStreamToken	[in]	A token that the recorder module must save for passing to later stream calls.
	 * @param	pStream			[in]	The stream interface the recorder module must save for later stream calls.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetOutputStream)
		(XnModuleNodeHandle hInstance, void *pStreamToken, XnRecorderOutputStreamInterface *pStream);

	XnModuleProductionNodeInterface* pProductionNode;
	XnNodeNotifications* pNodeNotifications;
} XnModuleRecorderInterface;

typedef struct XnModulePlayerInterface
{
	/*
	 * Sets the input stream for the player module
	 *
	 * @param	hInstance		[in]	A handle to the instance.
	 * @param	pStreamCookie	[in]	A cookie that the player module must save for passing to later stream calls.
	 * @param	pStream			[in]	The stream interface the player module must save for later stream calls.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetInputStream)
		(XnModuleNodeHandle hInstance, void *pStreamCookie, XnPlayerInputStreamInterface *pStream);

	/**
	 * Reads next data from the input stream.
	 *
	 * @param	hInstance		[in]	A handle to the instance.
	 */
	XnStatus (XN_CALLBACK_TYPE* ReadNext)(XnModuleNodeHandle hInstance);

	/*
	 * Sets the node notifications object to be used by the player. The player will use this object to
	 * notify about events it encounters in the stream that was set with @ref SetInputStream().
	 *
	 * @param	hInstance					[in]	A handle to the instance.
	 * @param	pNodeNotificationsCookie	[in]	A cookie that the player module must save for passing to node notification calls.
	 * @param	pNodeNotifications			[in]	The node notifications interface that the player module will use to raise node notifications it finds in the stream.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetNodeNotifications)
		(XnModuleNodeHandle hInstance, void *pNodeNotificationsCookie, XnNodeNotifications *pNodeNotifications);

	XnStatus (XN_CALLBACK_TYPE* SetRawNodeNotifications)
		(XnModuleNodeHandle hInstance, void *pRawNodeNotificationsCookie, XnNodeNotifications *pRawNodeNotifications);


	/*
	 * Determines whether the player repeats the played stream or not.
	 *
	 * @param	bRepeat		[in]	If TRUE, repeat is set to be ON. If FALSE, repeat is set to be OFF.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetRepeat)
		(XnModuleNodeHandle hInstance, XnBool bRepeat);

	XnStatus (XN_CALLBACK_TYPE* SeekToTimeStamp)
		(XnModuleNodeHandle hInstance, XnInt64 nTimeOffset, XnPlayerSeekOrigin origin);

	XnStatus (XN_CALLBACK_TYPE* SeekToFrame)
		(XnModuleNodeHandle hInstance, const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin);

	XnStatus (XN_CALLBACK_TYPE* TellTimestamp)
		(XnModuleNodeHandle hInstance, XnUInt64* pnTimestamp);

	XnStatus (XN_CALLBACK_TYPE* TellFrame)
		(XnModuleNodeHandle hInstance, const XnChar* strNodeName, XnUInt32* pnFrame);

	XnStatus (XN_CALLBACK_TYPE* GetNumFrames)
		(XnModuleNodeHandle hInstance, const XnChar* strNodeName, XnUInt32* pnFrames);

	const XnChar* (XN_CALLBACK_TYPE* GetSupportedFormat)
		(XnModuleNodeHandle hInstance);

	XnBool (XN_CALLBACK_TYPE* IsEOF)
		(XnModuleNodeHandle hInstance);

	/**
	 * Registers a callback function to be called when end-of-file was reached.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function to be called.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to UnregisterFromEndOfFileReached().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToEndOfFileReached)
		(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using RegisterToEndOfFileReached().
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from RegisterToEndOfFileReached().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromEndOfFileReached)
		(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	XnModuleProductionNodeInterface* pProductionNode;
	XnModuleSeekingInterface* pSeekingInterface;
} XnModulePlayerInterface;

typedef struct XnModuleCroppingInterface
{
	/** 
	 * Sets the cropping.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	pCropping	[in]	The cropping configuration to be set.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetCropping)(XnModuleNodeHandle hGenerator, const XnCropping* pCropping);

	/** 
	 * Gets current cropping configuration.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	pCropping	[out]	Current cropping configuration.
	 */
	XnStatus (XN_CALLBACK_TYPE* GetCropping)(XnModuleNodeHandle hGenerator, XnCropping* pCropping);

	/**
	 * Registers a callback function to cropping changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when cropping changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to UnregisterFromCroppingChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToCroppingChange)
		(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using RegisterToCroppingChange().
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from RegisterToCroppingChange().
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromCroppingChange)
		(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleCroppingInterface;

typedef struct XnModuleMapGeneratorInterface
{
	/**
	 * Contains the generator interface.
	 */
	XnModuleGeneratorInterface* pGeneratorInterface;

	/** 
	 * Gets the number of supported modes. This is useful for allocating an array that will be passed to
	 * @ref GetSupportedMapOutputModes().
	 *
	 * @param	hInstance	[in]	A handle to the instance.
	 */
	XnUInt32 (XN_CALLBACK_TYPE* GetSupportedMapOutputModesCount)
		(XnModuleNodeHandle hGenerator);

	/** 
	 * Gets a list of all supported modes. The size of the array that should be passed can be obtained by calling
	 * @ref GetSupportedMapOutputModesCount().
	 *
	 * @param	hInstance	[in]		A handle to the instance.
	 * @param	aModes		[in/out]	An array to be filled with supported modes. 
	 * @param	pnCount		[in/out]	In: number of elements allocated in the array. Out: number of elements
	 *									actually written to the array.
	 */
	XnStatus (XN_CALLBACK_TYPE* GetSupportedMapOutputModes)
		(XnModuleNodeHandle hGenerator, 
		XnMapOutputMode* aModes, XnUInt32* pnCount);

	/** 
	 * Sets the output mode. 
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	pOutputMode	[in]	The output mode to be set.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetMapOutputMode)
		(XnModuleNodeHandle hGenerator, const XnMapOutputMode* pOutputMode);

	/** 
	 * Gets the current output mode. 
	 * 
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	pOutputMode	[out]	Current output mode.
	 */
	XnStatus (XN_CALLBACK_TYPE* GetMapOutputMode)
		(XnModuleNodeHandle hGenerator, XnMapOutputMode* pOutputMode);

	/**
	 * Registers a callback function to mode changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when mode changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to UnregisterFromMapOutputModeChange.
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToMapOutputModeChange)
		(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using RegisterToMapOutputModeChange.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from RegisterToMapOutputModeChange.
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromMapOutputModeChange)
		(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	XnModuleCroppingInterface* pCroppingInterface;

} XnModuleMapGeneratorInterface;

/**
 * A set of functions supported by depth generators who supports the User Position capability.
 */
typedef struct XnModuleUserPositionCapabilityInterface
{
	/**
	 * Gets the number of user positions supported by this generator.
	 *
	 * @param	hGenerator	[in]		A handle to the instance.
	 */
	XnUInt32 (XN_CALLBACK_TYPE* GetSupportedUserPositionsCount)
		(XnModuleNodeHandle hGenerator);

	/** 
	 * Sets the current user position. 
	 * 
	 * @param	hGenerator	[in]		A handle to the instance.
	 * @param	nIndex		[in]		The user position to set.
	 * @param	pPosition	[in]		The user position in the frame.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetUserPosition)(
		XnModuleNodeHandle hGenerator, 
		XnUInt32 nIndex, 
		const XnBoundingBox3D* pPosition);

	/** 
	 * Gets the current user position. 
	 *
	 * @param	hGenerator	[in]		A handle to the instance.
	 * @param	nIndex		[in]		The user position to get.
	 * @param	pPosition	[out]		Current user position.
	 */
	XnStatus (XN_CALLBACK_TYPE* GetUserPosition)
		(XnModuleNodeHandle hGenerator, XnUInt32 nIndex, XnBoundingBox3D* pPosition);

	/**
	 * Registers a callback function to user position changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when user position changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to UnregisterFromUserPositionChange.
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToUserPositionChange)
		(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using RegisterToUserPositionChange.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from RegisterToUserPositionChange.
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromUserPositionChange)
		(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleUserPositionCapabilityInterface;

/** Depth generator additional methods. */
typedef struct XnModuleDepthGeneratorInterface
{
	/**
	 * Contains map generator interface.
	 */
	XnModuleMapGeneratorInterface* pMapInterface;

	/** 
	 * Gets the current depth-map. This map is updated after a call to WaitAndUpdateData(). 
	 *
	 * @param	hGenerator	[in]		A handle to the instance.
	 */
	XnDepthPixel* (XN_CALLBACK_TYPE* GetDepthMap)
		(XnModuleNodeHandle hGenerator);

	/** 
	 * Gets the maximum depth the device can produce. 
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 */
	XnDepthPixel (XN_CALLBACK_TYPE* GetDeviceMaxDepth)
		(XnModuleNodeHandle hGenerator);

	/**
	 * Gets the field of view of the sensor, in radians.
	 * This value will be used for translation between projective and real-world coordinates. 
	 * It is assumed that the sensor is a pinhole camera.
	 *
	 * @param	hGenerator			[in]		A handle to the instance.
	 * @param	pFOV				[in]		A struct to be filled with data.
	 */
	void (XN_CALLBACK_TYPE* GetFieldOfView)(XnModuleNodeHandle hGenerator, XnFieldOfView* pFOV);

	/**
	 * Registers a callback function to field of view changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when field of view changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref UnregisterFromFieldOfViewChange().
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToFieldOfViewChange)(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref RegisterToFieldOfViewChange.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref RegisterToFieldOfViewChange.
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromFieldOfViewChange)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	/**
	 * Contains User Position Capability interface.
	 */
	XnModuleUserPositionCapabilityInterface* pUserPositionInterface;

} XnModuleDepthGeneratorInterface;

/** Image generator Interface. */
typedef struct XnModuleImageGeneratorInterface
{
	/**
	 * Contains map generator interface.
	 */
	XnModuleMapGeneratorInterface* pMapInterface;

	/** Gets the current image-map. This map is updated after a call to WaitAndUpdateData(). */
	XnUInt8* (XN_CALLBACK_TYPE* GetImageMap)(
		XnModuleNodeHandle hGenerator
	);

	/**
	 * Checks if a specific pixel format is supported.
	 *
	 * @param	hGenerator	[in]		A handle to the instance.
	 * @param	Format		[in]		The format to check for.
	 */
	XnBool (XN_CALLBACK_TYPE* IsPixelFormatSupported)(XnModuleNodeHandle hGenerator, XnPixelFormat Format);

	/**
	 * Sets the pixel format of the image map.
	 *
	 * @param	hGenerator	[in]		A handle to the instance.
	 * @param	Format		[in]		The format to set.
	 */
	XnStatus (XN_CALLBACK_TYPE* SetPixelFormat)(XnModuleNodeHandle hGenerator, XnPixelFormat Format);

	/**
	 * Gets current pixel format.
	 *
	 * @param	hGenerator	[in]		A handle to the instance.
	 */
	XnPixelFormat (XN_CALLBACK_TYPE* GetPixelFormat)(XnModuleNodeHandle hGenerator);

	/**
	 * Registers a callback function to pixel format changes.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	handler		[in]	A pointer to a function that will be called when pixel format changes.
	 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
	 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromPixelFormatChange.
	 */
	XnStatus (XN_CALLBACK_TYPE* RegisterToPixelFormatChange)(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);

	/**
	 * Unregisters a callback function which was registered using @ref xnRegisterToPixelFormatChange.
	 *
	 * @param	hGenerator	[in]	A handle to the instance.
	 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToPixelFormatChange.
	 */
	void (XN_CALLBACK_TYPE* UnregisterFromPixelFormatChange)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleImageGeneratorInterface;

/** IR generator Interface. */
typedef struct XnModuleIRGeneratorInterface
{
	/**
	 * Contains map generator interface.
	 */
	XnModuleMapGeneratorInterface* pMapInterface;

	/** Gets the current IR-map. This map is updated after a call to WaitAndUpdateData(). */
	XnIRPixel* (XN_CALLBACK_TYPE* GetIRMap)(
		XnModuleNodeHandle hGenerator
	);

} XnModuleIRGeneratorInterface;

/** Gesture generator Interface. */
typedef struct XnModuleGestureGeneratorInterface
{
	XnModuleGeneratorInterface* pGeneratorInterface;

	XnStatus (XN_CALLBACK_TYPE* AddGesture)(XnModuleNodeHandle hGenerator, const XnChar* strGesture, XnBoundingBox3D* pArea);
	XnStatus (XN_CALLBACK_TYPE* RemoveGesture)(XnModuleNodeHandle hGenerator, const XnChar* strGesture);
	XnStatus (XN_CALLBACK_TYPE* GetActiveGestures)(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt16* nGestures);
	XnStatus (XN_CALLBACK_TYPE* EnumerateGestures)(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt16* nGestures);
	XnBool (XN_CALLBACK_TYPE* IsGestureAvailable)(XnModuleNodeHandle hInstance, const XnChar* strGesture);
	XnBool (XN_CALLBACK_TYPE* IsGestureProgressSupported)(XnModuleNodeHandle hGenerator, const XnChar* strGesture);
	XnStatus (XN_CALLBACK_TYPE* RegisterGestureCallbacks)(XnModuleNodeHandle hGenerator, XnModuleGestureRecognized RecognizedCB, XnModuleGestureProgress ProgressCB, void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterGestureCallbacks)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);
	XnStatus (XN_CALLBACK_TYPE* RegisterToGestureChange)(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterFromGestureChange)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	XnStatus (XN_CALLBACK_TYPE* GetAllActiveGestures)(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* nGestures);
	XnStatus (XN_CALLBACK_TYPE* EnumerateAllGestures)(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* nGestures);
} XnModuleGestureGeneratorInterface;

/** Scene Analyzer Interface. */
typedef struct XnModuleSceneAnalyzerInterface
{
	XnModuleMapGeneratorInterface* pMapInterface;

	const XnLabel* (XN_CALLBACK_TYPE* GetLabelMap)(XnModuleNodeHandle hGenerator);
	XnStatus (XN_CALLBACK_TYPE* GetFloor)(XnModuleNodeHandle hGenerator, XnPlane3D* pPlane);
} XnModuleSceneAnalyzerInterface;

/**
* A set of functions supported by user generators who supports the UI capability.
*/
typedef struct XnModuleHandsGeneratorInterface
{
	XnModuleGeneratorInterface* pGeneratorInterface;

	XnStatus (XN_CALLBACK_TYPE* RegisterHandCallbacks)(XnModuleNodeHandle hGenerator, XnModuleHandCreate CreateCB, XnModuleHandUpdate UpdateCB, XnModuleHandDestroy DestroyCB, void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterHandCallbacks)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);
	XnStatus (XN_CALLBACK_TYPE* StopTracking)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnStatus (XN_CALLBACK_TYPE* StopTrackingAll)(XnModuleNodeHandle hGenerator);
	XnStatus (XN_CALLBACK_TYPE* StartTracking)(XnModuleNodeHandle hGenerator, const XnPoint3D* pPosition);
	XnStatus (XN_CALLBACK_TYPE* SetSmoothing)(XnModuleNodeHandle hGenerator, XnFloat fSmoothingFactor);
} XnModuleHandsGeneratorInterface;

/**
* A set of functions supported by user generators who supports the Skeleton capability.
*/
typedef struct XnModuleSkeletonCapabilityInterface
{
	XnBool (XN_CALLBACK_TYPE* IsJointAvailable)(XnModuleNodeHandle hGenerator, XnSkeletonJoint eJoint);
	XnBool (XN_CALLBACK_TYPE* IsProfileAvailable)(XnModuleNodeHandle hGenerator, XnSkeletonProfile eProfile);
	XnStatus (XN_CALLBACK_TYPE* SetSkeletonProfile)(XnModuleNodeHandle hGenerator, XnSkeletonProfile eProfile);
	XnStatus (XN_CALLBACK_TYPE* SetJointActive)(XnModuleNodeHandle hGenerator, XnSkeletonJoint eJoint, XnBool bState);
	XnBool (XN_CALLBACK_TYPE* IsJointActive)(XnModuleNodeHandle hGenerator, XnSkeletonJoint eJoint);
	XnStatus (XN_CALLBACK_TYPE* RegisterToJointConfigurationChange)(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterFromJointConfigurationChange)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);
	XnStatus (XN_CALLBACK_TYPE* EnumerateActiveJoints)(XnModuleNodeHandle hGenerator, XnSkeletonJoint* pJoints, XnUInt16* pnJoints);
	XnStatus (XN_CALLBACK_TYPE* GetSkeletonJoint)(XnModuleNodeHandle hGenerator, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation* pJoint);
	XnStatus (XN_CALLBACK_TYPE* GetSkeletonJointPosition)(XnModuleNodeHandle hGenerator, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition* pJoint);
	XnStatus (XN_CALLBACK_TYPE* GetSkeletonJointOrientation)(XnModuleNodeHandle hGenerator, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation* pJoint);
	XnBool (XN_CALLBACK_TYPE* IsTracking)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnBool (XN_CALLBACK_TYPE* IsCalibrated)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnBool (XN_CALLBACK_TYPE* IsCalibrating)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnStatus (XN_CALLBACK_TYPE* RequestCalibration)(XnModuleNodeHandle hGenerator, XnUserID user, XnBool bForce);
	XnStatus (XN_CALLBACK_TYPE* AbortCalibration)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnStatus (XN_CALLBACK_TYPE* SaveCalibrationData)(XnModuleNodeHandle hGenerator, XnUserID user, XnUInt32 nSlot);
	XnStatus (XN_CALLBACK_TYPE* LoadCalibrationData)(XnModuleNodeHandle hGenerator, XnUserID user, XnUInt32 nSlot);
	XnStatus (XN_CALLBACK_TYPE* ClearCalibrationData)(XnModuleNodeHandle hGenerator, XnUInt32 nSlot);
	XnBool (XN_CALLBACK_TYPE* IsCalibrationData)(XnModuleNodeHandle hGenerator, XnUInt32 nSlot);
	XnStatus (XN_CALLBACK_TYPE* StartTracking)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnStatus (XN_CALLBACK_TYPE* StopTracking)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnStatus (XN_CALLBACK_TYPE* Reset)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnBool (XN_CALLBACK_TYPE* NeedPoseForCalibration)(XnModuleNodeHandle hGenerator);
	XnStatus (XN_CALLBACK_TYPE* GetCalibrationPose)(XnModuleNodeHandle hGenerator, XnChar* strPose);
	XnStatus (XN_CALLBACK_TYPE* SetSmoothing)(XnModuleNodeHandle hGenerator, XnFloat fSmoothingFactor);
	XnStatus (XN_CALLBACK_TYPE* RegisterCalibrationCallbacks)(XnModuleNodeHandle hGenerator, XnModuleCalibrationStart CalibrationStartCB, XnModuleCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterCalibrationCallbacks)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleSkeletonCapabilityInterface;

typedef struct XnModulePoseDetectionCapabilityInterface
{
	XnUInt32 (XN_CALLBACK_TYPE* GetNumberOfPoses)(XnModuleNodeHandle hGenerator);
	XnStatus (XN_CALLBACK_TYPE* GetAvailablePoses)(XnModuleNodeHandle hGenerator, XnChar** pstrPoses, XnUInt32* pnPoses);
	XnStatus (XN_CALLBACK_TYPE* StartPoseDetection)(XnModuleNodeHandle hGenerator, const XnChar* strPose, XnUserID user);
	XnStatus (XN_CALLBACK_TYPE* StopPoseDetection)(XnModuleNodeHandle hGenerator, XnUserID user);
	XnStatus (XN_CALLBACK_TYPE* RegisterToPoseCallbacks)(XnModuleNodeHandle hGenerator, XnModulePoseDetectionCallback StartPoseCB, XnModulePoseDetectionCallback EndCB, void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterFromPoseCallbacks)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	XnStatus (XN_CALLBACK_TYPE* GetAllAvailablePoses)(XnModuleNodeHandle hGenerator, XnChar** pstrPoses, XnUInt32 nNameLength, XnUInt32* pnPoses);
} XnModulePoseDetectionCapabilityInterface;

/** User generator Interface. */
typedef struct XnModuleUserGeneratorInterface
{
	XnModuleGeneratorInterface* pGeneratorInterface;

	XnUInt16 (XN_CALLBACK_TYPE* GetNumberOfUsers)(XnModuleNodeHandle hGenerator);
	XnStatus (XN_CALLBACK_TYPE* GetUsers)(XnModuleNodeHandle hGenerator, XnUserID* pUsers, XnUInt16* pnUsers);
	XnStatus (XN_CALLBACK_TYPE* GetCoM)(XnModuleNodeHandle hGenerator, XnUserID user, XnPoint3D* pCoM);
	XnStatus (XN_CALLBACK_TYPE* GetUserPixels)(XnModuleNodeHandle hGenerator, XnUserID user, XnSceneMetaData* pScene);
	XnStatus (XN_CALLBACK_TYPE* RegisterUserCallbacks)(XnModuleNodeHandle hGenerator, XnModuleUserHandler NewUserCB, XnModuleUserHandler LostUserCB, void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterUserCallbacks)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

	/**
	* Contains Skeleton Capability interface.
	*/
	XnModuleSkeletonCapabilityInterface* pSkeletonInterface;
	/**
	* Contains Pose Detection Capability interface
	*/
	XnModulePoseDetectionCapabilityInterface* pPoseDetectionInteface;

} XnModuleUserGeneratorInterface;

/** Audio Generator Interface. */
typedef struct XnModuleAudioGeneratorInterface
{
	XnModuleGeneratorInterface* pGeneratorInterface;

	XnUChar* (XN_CALLBACK_TYPE* GetAudioBuffer)(XnModuleNodeHandle hGenerator);
	XnUInt32 (XN_CALLBACK_TYPE* GetSupportedWaveOutputModesCount)(XnModuleNodeHandle hGenerator);
	XnStatus (XN_CALLBACK_TYPE* GetSupportedWaveOutputModes)(XnModuleNodeHandle hGenerator, XnWaveOutputMode* aSupportedModes, XnUInt32* pnCount);
	XnStatus (XN_CALLBACK_TYPE* SetWaveOutputMode)(XnModuleNodeHandle hGenerator, const XnWaveOutputMode* OutputMode);
	XnStatus (XN_CALLBACK_TYPE* GetWaveOutputMode)(XnModuleNodeHandle hGenerator, XnWaveOutputMode* OutputMode);
	XnStatus (XN_CALLBACK_TYPE* RegisterToWaveOutputModeChanges)(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler,
		void* pCookie, XnCallbackHandle* phCallback);
	void (XN_CALLBACK_TYPE* UnregisterFromWaveOutputModeChanges)(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback);

} XnModuleAudioGeneratorInterface;

typedef struct XnModuleCodecInterface
{
	XnModuleProductionNodeInterface* pProductionNode;

	XnCodecID (XN_CALLBACK_TYPE* GetCodecID)(XnModuleNodeHandle hCodec);
	XnStatus (XN_CALLBACK_TYPE* Init)(XnModuleNodeHandle hCodec, XnNodeHandle hNode);
	XnStatus (XN_CALLBACK_TYPE* CompressData)(XnModuleNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten);
	XnStatus (XN_CALLBACK_TYPE* DecompressData)(XnModuleNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten);
} XnModuleCodecInterface;

#endif // __XN_MODULE_INTERFACE_H__
