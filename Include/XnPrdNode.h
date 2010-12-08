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




#ifndef __XN_PRD_NODE_H__
#define __XN_PRD_NODE_H__

/**
 * @ingroup cref
 * @defgroup prd_node Production Nodes Functionality
 * This page details functions that can be run on production nodes.
 */

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnTypes.h>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @ingroup prd_node
 * @defgroup node Production Node
 *
 * A production node is the most basic unit of processing in OpenNI. The following functions can be 
 * executed for every node in an OpenNI context.
 * @{
 */

/**
 * @brief Gets information about a specific node, like its description, and dependent nodes.
 *
 * @param	hNode	[in]	A handle to the node.
 */
XN_C_API XnNodeInfo* xnGetNodeInfo(XnNodeHandle hNode);

/**
 * @brief Gets the instance name of a node by its handle.
 *
 * @param	hNode	[in]	A handle to the node.
 */
XN_C_API const XnChar* xnGetNodeName(XnNodeHandle hNode);

/**
 * @brief Gets the context a node belongs to.
 *
 * @param	hNode	[in]	A handle to the node.
 */
XN_C_API XnContext* xnGetContextFromNodeHandle(XnNodeHandle hNode);

/**
 * @brief Checks if a production node supports specific capability.
 * 
 * @param	hInstance			[in]	A handle to the instance to be queried.
 * @param	strCapabilityName	[in]	The name of the capability to check.
 */
XN_C_API XnBool xnIsCapabilitySupported(XnNodeHandle hInstance, const XnChar* strCapabilityName);

/**
 * @brief Sets an integer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	nValue		[in]	New value
 */
XN_C_API XnStatus xnSetIntProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt64 nValue);
/**
 * @brief Sets a real property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	dValue		[in]	New value
 */
XN_C_API XnStatus xnSetRealProperty(XnNodeHandle hInstance, const XnChar* strName, XnDouble dValue);
/**
 * @brief Sets a string property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	strValue	[in]	New value
 */
XN_C_API XnStatus xnSetStringProperty(XnNodeHandle hInstance, const XnChar* strName, const XnChar* strValue);
/**
 * @brief Sets a buffer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	nBufferSize	[in]	The size of the buffer passed, in bytes.
 * @param	pBuffer		[in]	The data buffer.
 */
XN_C_API XnStatus xnSetGeneralProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

/**
 * @brief Gets an integer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	pnValue		[out]	Current value
 */
XN_C_API XnStatus xnGetIntProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt64* pnValue);
/**
 * @brief Gets a real property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	pdValue		[out]	Current value
 */
XN_C_API XnStatus xnGetRealProperty(XnNodeHandle hInstance, const XnChar* strName, XnDouble* pdValue);
/**
 * @brief Gets a string property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	csValue		[out]	Current value 
 * @param	nBufSize	[in]	The size of the csValue buffer. 
 */
XN_C_API XnStatus xnGetStringProperty(XnNodeHandle hInstance, const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize);
/**
 * @brief Gets a buffer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	nBufferSize	[in]	The size of the buffer passed, in bytes.
 * @param	pBuffer		[in]	The data buffer to be filled.
 */
XN_C_API XnStatus xnGetGeneralProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer);

/**
 * @brief Locks a node, not allowing any changes (any "set" function).
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	phLock		[out]	A handle of the lock, that can be used to manage the lock.
 */
XN_C_API XnStatus xnLockNodeForChanges(XnNodeHandle hInstance, XnLockHandle* phLock);

/**
 * @brief Unlocks a previously locked node.
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hLock		[in]	The handle of the lock, returned from @ref xnLockNodeForChanges().
 */
XN_C_API XnStatus xnUnlockNodeForChanges(XnNodeHandle hInstance, XnLockHandle hLock);

/**
 * @brief Start changes request on a locked node, without releasing that lock. 
 *
 * It allows "set" operations from the same context (same thread for that matter) until a call is made to 
 * @ref xnLockedNodeEndChanges().
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hLock		[in]	The handle of the lock, returned from @ref xnLockNodeForChanges().
 */
XN_C_API XnStatus xnLockedNodeStartChanges(XnNodeHandle hInstance, XnLockHandle hLock);

/**
 * @brief Ends changes request on a locked node. 
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hLock		[in]	The handle of the lock, returned from @ref xnLockNodeForChanges().
 */
XN_C_API XnStatus xnLockedNodeEndChanges(XnNodeHandle hInstance, XnLockHandle hLock);

/**
 * @brief Adds another node to the list of needed nodes for this node.
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hNeededNode	[in]	The node that is needed by hInstance.
 */
XN_C_API XnStatus xnAddNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);

/**
 * @brief Removes a needed node from the list of needed nodes. 
 *
 * If this node is no longer needed by any other node, and @ref xnRefProductionNode() was not called on it, 
 * it will be destroyed.
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hNeededNode	[in]	The node to be removed from the list.
 */
XN_C_API XnStatus xnRemoveNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);

/** @} */

//---------------------------------------------------------------------------
// ErrorState Capability
//---------------------------------------------------------------------------

/** 
 * @ingroup node
 * @defgroup err_state Error State Capability
 *
 * The Error State capability (@ref XN_CAPABILITY_ERROR_STATE) allows a node to report it is now in
 * an error state, and so, might not function correctly. An application may, at all times, check the 
 * error state of a node, and it may also register a callback function to be called whenever that state
 * changes.
 *
 * The error state of a node is represented using @ref XnStatus. A value of XN_STATUS_OK means the node
 * is OK. Any other value represents some sort of error situation in the node.
 * @{
 */

/**
 * @brief Gets current error state of this node.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns The error state of this node.
 */
XN_C_API XnStatus xnGetNodeErrorState(XnNodeHandle hInstance);

/**
 * @brief Registers a callback function to error state changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when error state changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromNodeErrorStateChange().
 */
XN_C_API XnStatus xnRegisterToNodeErrorStateChange
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToNodeErrorStateChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToNodeErrorStateChange().
 */
XN_C_API void xnUnregisterFromNodeErrorStateChange
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Generators
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup generator Generator
 * 
 * A Generator node is a production node that also generates continuous data. It supports all @ref node functions,
 * and adds additional functions.
 * @{
 */

/**
 * @name Controlling Generation
 * The following functions allows controlling if a node is currently generating data or not.
 * @{
 */

/** 
 * @brief Starts generation of the output. This will also cause all dependencies to start generating.
 * 
 * @param	hInstance	[in]	A handle to the instance to start generating.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a generator.
 */
XN_C_API XnStatus xnStartGenerating(XnNodeHandle hInstance);

/** 
 * @brief Checks if this node is currently generating.
 * 
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns FALSE if this production node is not a generator.
 */
XN_C_API XnBool xnIsGenerating(XnNodeHandle hInstance);

/** 
 * @brief Stops generation of the output.
 * 
 * @param	hInstance	[in]	A handle to the instance to stop generating.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a generator.
 */
XN_C_API XnStatus xnStopGenerating(XnNodeHandle hInstance);

/**
 * @brief Registers a callback function to be called when generation starts or stops.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when generation starts/stops.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromGenerationRunningChange().
 */
XN_C_API XnStatus xnRegisterToGenerationRunningChange
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToGenerationRunningChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToGenerationRunningChange().
 */
XN_C_API void xnUnregisterFromGenerationRunningChange
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/// @}

/**
 * @name Checking if new data is available
 * The following functions allows an application to know if and when a node has new data available.
 * When new data is available, the node data is still not replaced, until explicitly done so using one
 * of the update data functions.
 * @{
 */

/**
 * @brief Registers a callback function to be called when new data is available.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when new data is available.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromNewDataAvailable().
 */
XN_C_API XnStatus xnRegisterToNewDataAvailable
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToNewDataAvailable().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToNewDataAvailable().
 */
XN_C_API void xnUnregisterFromNewDataAvailable
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Checks whether this node has new data (and so a call to @ref xnWaitAndUpdateData() will not block).
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pnTimestamp	[out]	[Optional] If not NULL, will be filled with that data timestamp.
 */
XN_C_API XnBool xnIsNewDataAvailable(XnNodeHandle hInstance, XnUInt64* pnTimestamp);

/// @}

/**
 * @name Getting new data
 * @{
 */

/** 
 * @brief Updates the data to the latest available one. If needed, the call will block until new data is available.
 *
 * @param	hInstance	[in]	A handle to the instance to be updated.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a generator.
 */
XN_C_API XnStatus xnWaitAndUpdateData(XnNodeHandle hInstance);

/// @}

/**
 * @name Data properties
 * @{
 */

/**
 * @brief Checks whether current data is new. Meaning, did the data change on the last call to @ref xnWaitAndUpdateAll(),
 * @ref xnWaitOneUpdateAll() or @ref xnWaitAndUpdateData().
 *
 * @param	hInstance	[in]	A handle to the instance.
 */
XN_C_API XnBool xnIsDataNew(XnNodeHandle hInstance);

/**
 * @brief Gets the size of current data, in bytes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	(XnUInt32)-1 if this production node is not a generator.
 */
XN_C_API XnUInt32 xnGetDataSize(XnNodeHandle hInstance);

/**
 * @brief Gets the timestamp of current data, in microseconds.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	(XnUInt64)-1 if this production node is not a generator.
 */
XN_C_API XnUInt64 xnGetTimestamp(XnNodeHandle hInstance);

/**
 * @brief Gets the frame ID of current data.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	(XnUInt32)-1 if this production node is not a generator.
 */
XN_C_API XnUInt32 xnGetFrameID(XnNodeHandle hInstance);

/** @} */

/** @} */

//---------------------------------------------------------------------------
// Mirror Capability
//---------------------------------------------------------------------------

/** 
 * @ingroup generator
 * @defgroup mirror Mirror Capability
 * The mirror capability (@ref XN_CAPABILITY_MIRROR) allows mirroring of the data produced by a @ref generator.
 * Mirroring is useful if the sensor is placed in front of the user (so that it sees the user right hand as the
 * left one), but applications want user to get a more natural feeling of control.
 * @{
 */

/**
 * @brief Sets current mirror configuration.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	bMirror		[in]	TRUE for mirroring output, FALSE otherwise.
 */
XN_C_API XnStatus xnSetMirror(XnNodeHandle hInstance, XnBool bMirror);

/**
 * @brief Gets current mirroring configuration.
 *
 * @param	hInstance	[in]	A handle to the instance.
 */
XN_C_API XnBool xnIsMirrored(XnNodeHandle hInstance);

/**
 * @brief Registers a callback function to mirror changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when mirror changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromMirrorChange().
 */
XN_C_API XnStatus xnRegisterToMirrorChange
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToMirrorChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToMirrorChange().
 */
XN_C_API void xnUnregisterFromMirrorChange
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Alternative View Point Capability
//---------------------------------------------------------------------------

/** 
 * @ingroup generator
 * @defgroup altviewpoint Alternative View Point Capability
 * The Alternative View Point capability (@ref XN_CAPABILITY_ALTERNATIVE_VIEW_POINT) allows a @ref generator
 * to transform its data to appear as if the sensor is placed in another location. For example, when more
 * than one sensor is active (or one sensor that produces several outputs coming from close locations, yet not
 * the same one), one sensor can change its data to appear as if coming from the location of the other sensor,
 * so that both data buffers represent the same (for example depth and image maps, where the same coordinates
 * in the map represent the same location is the scene).
 * @{
 */

/**
 * @brief Checks if this generator can change its output to look like it was taken from a different location, represented by another generator.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The view point to be checked.
 *
 * @returns TRUE if view point is supported, FALSE otherwise.
 */
XN_C_API XnBool xnIsViewPointSupported(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Sets the view point of this generator to look like as if placed at another generator location.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The view point to be set.
 */
XN_C_API XnStatus xnSetViewPoint(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Sets the view point of this generator to its normal one.
 *
 * @param	hInstance	[in]	A handle to the instance.
 */
XN_C_API XnStatus xnResetViewPoint(XnNodeHandle hInstance);

/**
 * @brief Checks if current view point is as if coming from the other node view point.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The view point to be checked.
 */
XN_C_API XnBool xnIsViewPointAs(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Registers a callback function to view point changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when view point changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromViewPointChange().
 */
XN_C_API XnStatus xnRegisterToViewPointChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToViewPointChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToViewPointChange().
 */
XN_C_API void xnUnregisterFromViewPointChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// FrameSync Capability
//---------------------------------------------------------------------------

/** 
 * @ingroup generator
 * @defgroup framesync Frame-Sync Capability
 * The Frame Sync capability (@ref XN_CAPABILITY_FRAME_SYNC) allows two sensors producing frame data to synchronize
 * their frames so that they arrive at the same time. This allows smaller latency between two maps of the same scene
 * (for example depth and image).
 * @{
 */

/**
 * @brief Checks if this generator can frame sync to another node.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The node to be checked.
 *
 * @returns TRUE if frame sync to node is supported, FALSE otherwise.
 */
XN_C_API XnBool xnCanFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Activates frame sync with the other node.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The node to sync to.
 */
XN_C_API XnStatus xnFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Stops frame sync with the other node.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The node to sync to.
 */
XN_C_API XnStatus xnStopFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Checks if current view point is as if coming from the other node view point.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The node to be checked.
 */
XN_C_API XnBool xnIsFrameSyncedWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Registers a callback function to view point changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when frame sync changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromFrameSyncChange().
 */
XN_C_API XnStatus xnRegisterToFrameSyncChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToFrameSyncChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToFrameSyncChange().
 */
XN_C_API void xnUnregisterFromFrameSyncChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Map Generators
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup mapgen Map Generator
 * A Map Generator node is a @ref generator that has output data in the form of a pixel map. It supports all @ref generator functions,
 * and adds additional functions.
 * @{
 */

/** 
 * @brief Gets the number of supported modes. This is useful for allocating an array that will be passed to @ref xnGetSupportedMapOutputModes().
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	0 if this production node is not a map generator.
 */
XN_C_API XnUInt32 xnGetSupportedMapOutputModesCount(XnNodeHandle hInstance);

/** 
 * @brief Gets a list of all supported modes. The size of the array that should be passed can be obtained by calling @ref xnGetSupportedMapOutputModesCount().
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	aModes		[in/out]	An array to be filled with supported modes. 
 * @param	pnCount		[in/out]	In: number of elements allocated in the array. Out: number of elements
 *									actually written to the array.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a map generator.
 */
XN_C_API XnStatus xnGetSupportedMapOutputModes(XnNodeHandle hInstance, XnMapOutputMode* aModes, XnUInt32* pnCount);

/** 
 * @brief Sets the output mode. 
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pOutputMode	[in]	The output mode to be set.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a map generator.
 */
XN_C_API XnStatus xnSetMapOutputMode(XnNodeHandle hInstance, const XnMapOutputMode* pOutputMode);

/** 
 * @brief Gets the current output mode. 
 * 
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pOutputMode	[out]	Current output mode.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a map generator.
 */
XN_C_API XnStatus xnGetMapOutputMode(XnNodeHandle hInstance, XnMapOutputMode* pOutputMode);

/**
 * @brief Registers a callback function to mode changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when mode changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromMapOutputModeChange.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a map generator.
 */
XN_C_API XnStatus xnRegisterToMapOutputModeChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToMapOutputModeChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToMapOutputModeChange.
 */
XN_C_API void xnUnregisterFromMapOutputModeChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Cropping Capability
//---------------------------------------------------------------------------


/**
 * @ingroup mapgen
 * @defgroup cropping Cropping Capability
 * The Cropping capability (@ref XN_CAPABILITY_CROPPING) allows a @ref mapgen to output a selected area
 * of the frame instead of the entire frame. When cropping is turned on, the data buffer looks just like
 * resolution is smaller, meaning data is packed. For example if the MapGenerator is working in VGA resolution
 * (640x480) and the application chose a cropping of 300x200, then after first 300 pixels the next pixel row
 * begins. Pixels outside the cropping area are not returned in the buffer (and not just being blacked out).
 * This is very useful for performance boost.
 * @{
 */

/** 
 * @brief Sets the cropping.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pCropping	[in]	The cropping configuration to be set.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the Cropping capability.
 */
XN_C_API XnStatus xnSetCropping(XnNodeHandle hInstance, const XnCropping* pCropping);

/** 
 * @brief Gets current cropping configuration.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pCropping	[out]	Current cropping configuration.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the Cropping capability.
 */
XN_C_API XnStatus xnGetCropping(XnNodeHandle hInstance, XnCropping* pCropping);

/**
 * @brief Registers a callback function to cropping changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when cropping changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromCroppingChange().
 */
XN_C_API XnStatus xnRegisterToCroppingChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToCroppingChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from xnRegisterToCroppingChange().
 */
XN_C_API void xnUnregisterFromCroppingChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Depth Generators
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup depthgen Depth Generator
 * A Depth Generator node is a @ref mapgen that outputs depth maps. It supports all @ref mapgen functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates a depth generator.
 *
 * @param	pContext			[in]	The context in which to create the depth generator
 * @param	phDepthGenerator	[out]	A handle to the created depth generator
 * @param	pQuery				[in]	Optional. Can be used to select which depth generator to create. If not specified, this function may create any depth generator that is available.
 * @param	pErrors				[in]	Optional. If provided, will be filled with information about depth generators that could not be created.
 */
XN_C_API XnStatus xnCreateDepthGenerator(
	XnContext* pContext,
	XnNodeHandle* phDepthGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors);

/** 
 * @brief Gets the maximum depth the device can produce. 
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	-1 if this production node is not a depth generator.
 */
XN_C_API XnDepthPixel xnGetDeviceMaxDepth(XnNodeHandle hInstance);

/** 
 * @brief Gets the Field-Of-View of the depth generator, in radians.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pFOV		[in]	A struct to be filled with field of view.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a depth generator.
 */
XN_C_API XnStatus xnGetDepthFieldOfView(XnNodeHandle hInstance, XnFieldOfView* pFOV);

/**
 * @brief Registers a callback function to field of view changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when field of view changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromDepthFieldOfViewChange.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the User Position capability.
 */
XN_C_API XnStatus xnRegisterToDepthFieldOfViewChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToDepthFieldOfViewChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToDepthFieldOfViewChange.
 */
XN_C_API void xnUnregisterFromDepthFieldOfViewChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Converts a list of points from projective coordinates to real world coordinates.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	nCount		[in]		The number of points to translate.
 * @param	aProjective	[in]		An array of projective coordinates points.
 * @param	aRealWorld	[in/out]	An array to be filled with real world coordinates points.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a depth generator.
 */
XN_C_API XnStatus xnConvertProjectiveToRealWorld(
	XnNodeHandle hInstance, XnUInt32 nCount, const XnPoint3D* aProjective, XnPoint3D* aRealWorld);

/**
 * @brief Converts a list of points from projective coordinates to real world coordinates.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	nCount		[in]		The number of points to translate.
 * @param	aRealWorld	[in]		An array of real world coordinates points.
 * @param	aProjective	[in/out]	An array to be filled with projective coordinates points.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a depth generator.
 */
XN_C_API XnStatus xnConvertRealWorldToProjective(
	XnNodeHandle hInstance, XnUInt32 nCount, const XnPoint3D* aRealWorld, XnPoint3D* aProjective);

/** 
 * @brief Gets the current depth-map. This map is updated after a call to @ref xnWaitAndUpdateData(). 
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not a depth generator.
 */
XN_C_API XnDepthPixel* xnGetDepthMap(XnNodeHandle hInstance);

/**
 * @brief Gets the current depth-map meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void xnGetDepthMetaData(XnNodeHandle hInstance, XnDepthMetaData* pMetaData);

/** @} */

//---------------------------------------------------------------------------
// User Position Capability
//---------------------------------------------------------------------------

/** 
 * @ingroup depthgen
 * @defgroup userpos User Position Capability
 * The User Position capability (@ref XN_CAPABILITY_USER_POSITION) allows a @ref depthgen to change its
 * output, so that it would be optimized in specific locations in the scene.
 * @{
 */

/**
 * @brief Gets the number of user positions supported by this generator.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	0 if this production node does not support the User Position capability.
 */
XN_C_API XnUInt32 xnGetSupportedUserPositionsCount(XnNodeHandle hInstance);

/** 
 * @brief Sets the current user position. 
 * 
 * @param	hInstance	[in]		A handle to the instance.
 * @param	nIndex		[in]		The user position to set.
 * @param	pPosition	[in]		The user position in the frame.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the User Position capability.
 */
XN_C_API XnStatus xnSetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, const XnBoundingBox3D* pPosition);

/** 
 * @brief Gets the current user position. 
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	nIndex		[in]	The user position to get.
 * @param	pPosition	[out]	The user position.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the User Position capability.
 */
XN_C_API XnStatus xnGetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, XnBoundingBox3D* pPosition);

/**
 * @brief Registers a callback function to user position changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when user position changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromUserPositionChange.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the User Position capability.
 */
XN_C_API XnStatus xnRegisterToUserPositionChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToUserPositionChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToUserPositionChange.
 */
XN_C_API void xnUnregisterFromUserPositionChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Image Generators
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup imagegen Image Generator
 * An Image Generator node is a @ref mapgen that outputs image maps. It supports all @ref mapgen functions,
 * and adds additional functions.
 * Image maps may arrive in different pixel formats.
 * @{
 */

/**
 * @brief Creates an image generator.
 *
 * @param	pContext			[in]	The context in which to create the image generator.
 * @param	phImageGenerator	[out]	A handle to the created image generator.
 * @param	pQuery				[in]	Optional. Can be used to select which image generator to create. If not specified, this function may create any image generator that is available.
 * @param	pErrors				[in]	Optional. If provided, will be filled with information about image generators that could not be created.
 */
XN_C_API XnStatus xnCreateImageGenerator(
	XnContext* pContext,
	XnNodeHandle* phImageGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

/** 
 * @brief Gets the current RGB24 image-map. 
 * This map is updated after a call to @ref xnWaitAndUpdateData(). It is assumed that the node is currently in RGB24 pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnRGB24Pixel* xnGetRGB24ImageMap(XnNodeHandle hInstance);

/** 
 * @brief Gets the current YUV422 image-map. 
 * This map is updated after a call to @ref xnWaitAndUpdateData(). It is assumed that the node is currently in YUV422 pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnYUV422DoublePixel* xnGetYUV422ImageMap(XnNodeHandle hInstance);

/** 
 * @brief Gets the current Grayscale8 image-map. 
 * This map is updated after a call to @ref xnWaitAndUpdateData(). It is assumed that the node is currently in Grayscale8 pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnGrayscale8Pixel* xnGetGrayscale8ImageMap(XnNodeHandle hInstance);

/** 
 * @brief Gets the current Grayscale16 image-map. 
 * This map is updated after a call to @ref xnWaitAndUpdateData(). It is assumed that the node is currently in Grayscale16 pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnGrayscale16Pixel* xnGetGrayscale16ImageMap(XnNodeHandle hInstance);

/** 
 * @brief Gets the current image-map as a byte buffer. 
 *
 * This map is updated after a call to @ref xnWaitAndUpdateData(). 
 * It is highly advised not to use this function. Instead, if you know the format of the image, use
 * @ref xnGetRGB24ImageMap(), @ref xnGetYUV422ImageMap(), @ref xnGetGrayscale8ImageMap() or 
 * @ref xnGetGrayscale16ImageMap().
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnUInt8* xnGetImageMap(XnNodeHandle hInstance);

/**
 * @brief Checks if a specific pixel format is supported.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	Format		[in]		The format to check for.
 */
XN_C_API XnBool xnIsPixelFormatSupported(XnNodeHandle hInstance, XnPixelFormat Format);

/**
 * @brief Sets the pixel format of the image map.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	Format		[in]		The format to set.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not an image generator.
 */
XN_C_API XnStatus xnSetPixelFormat(XnNodeHandle hInstance, XnPixelFormat Format);

/**
 * @brief Gets current pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	-1 if this production node is not an image generator.
 */
XN_C_API XnPixelFormat xnGetPixelFormat(XnNodeHandle hInstance);

/**
 * @brief Registers a callback function to pixel format changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when pixel format changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromPixelFormatChange.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the User Position capability.
 */
XN_C_API XnStatus xnRegisterToPixelFormatChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToPixelFormatChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToPixelFormatChange.
 */
XN_C_API void xnUnregisterFromPixelFormatChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Gets the current image-map meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void xnGetImageMetaData(XnNodeHandle hInstance, XnImageMetaData* pMetaData);

/** @} */

//---------------------------------------------------------------------------
// IR Generators
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup irgen IR Generator
 * An IR Generator node is a @ref mapgen that outputs infra-red maps. It supports all @ref mapgen functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates an IR generator.
 *
 * @param	pContext			[in]	The context in which to create the IR generator.
 * @param	phIRGenerator		[out]	A handle to the created IR generator.
 * @param	pQuery				[in]	Optional. Can be used to select which IR generator to create. If not specified, this function may create any IR generator that is available.
 * @param	pErrors				[in]	Optional. If provided, will be filled with information about IR generators that could not be created.
 */
XN_C_API XnStatus xnCreateIRGenerator(
	XnContext* pContext,
	XnNodeHandle* phIRGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

/** 
 * @brief Gets the current IR-map. This map is updated after a call to @ref xnWaitAndUpdateData(). 
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an IR generator.
 */
XN_C_API XnIRPixel* xnGetIRMap(XnNodeHandle hInstance);

/**
 * @brief Gets the current IR-map meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void xnGetIRMetaData(XnNodeHandle hInstance, XnIRMetaData* pMetaData);

/** @} */

//---------------------------------------------------------------------------
// Gesture Generators
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup gestures Gesture Generator
 * A Gestures Generator node is a @ref generator that recognizes certain gestures. It supports all @ref generator functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates a Gesture Generator.
 *
 * @param	pContext				[in]	The context in which to create the gesture generator.
 * @param	phGestureGenerator		[out]	A handle to the created gesture generator.
 * @param	pQuery					[in]	Optional. Can be used to select which Gesture node to create. If not specified, this function may create any gesture node that is available.
 * @param	pErrors					[in]	Optional. If provided, will be filled with information about gesture nodes that could not be created.
 */
XN_C_API XnStatus xnCreateGestureGenerator(
	XnContext* pContext,
	XnNodeHandle* phGestureGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

/**
 * @brief Turn on gesture. The generator will now look for this gesture
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	strGesture	[in]		The name of the gesture to look for
 * @param	pArea		[in]		The area in which to look for the gesture (in Real-World coordinates)
 */
XN_C_API XnStatus xnAddGesture(XnNodeHandle hInstance, const XnChar* strGesture, XnBoundingBox3D* pArea);
/**
 * @brief Turn off gesture.  The generator will no longer look for this gesture
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	strGesture	[in]		The name of the gesture to not look for anymore
 */
XN_C_API XnStatus xnRemoveGesture(XnNodeHandle hInstance, const XnChar* strGesture);
/**
 * @brief Get the names of the gestures that are currently active
 * 
 * @param	hInstance		[in]		A handle to the instance
 * @param	pstrGestures	[out]		Preallocated memory, for the gesture names
 * @param	nGestures		[in,out]	The size of the preallocated memory. Changed to number of gestures
 */
XN_C_API XnStatus xnGetActiveGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt16* nGestures);
/**
 * @brief Get the names of all gestures available
 * 
 * @param	hInstance		[in]		A handle to the instance
 * @param	pstrGestures	[out]		Preallocated memory, for the gesture names
 * @param	nGestures		[in,out]	The size of the preallocated memory. Changed to number of gestures
 */
XN_C_API XnStatus xnEnumerateGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt16* nGestures);
/**
 * @brief Check if a specific gesture is available in this generator
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	strGesture	[in]		Name of the gesture to check
 */
XN_C_API XnBool xnIsGestureAvailable(XnNodeHandle hInstance, const XnChar* strGesture);
/**
 * @brief Check if the specific gesture supports 'in progress' callbacks
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	strGesture	[in]		Name of the gesture to check
 */
XN_C_API XnBool xnIsGestureProgressSupported(XnNodeHandle hInstance, const XnChar* strGesture);
/**
 * @brief Register to all gesture callbacks.
 * 
 * @param	hInstance		[in]	A handle to the instance
 * @param	RecognizedCB	[in]	A callback to be called when a gesture is recognized
 * @param	ProgressCB		[in]	A callback to be called when a gesture is on its way to be recognized
 * @param	pCookie			[in]	User's cookie, to be delivered to the callbacks
 * @param	phCallback		[out]	The handle to these callbacks, to allow unregistration
 */
XN_C_API XnStatus xnRegisterGestureCallbacks(XnNodeHandle hInstance, XnGestureRecognized RecognizedCB, XnGestureProgress ProgressCB, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from gesture callbacks
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void xnUnregisterGestureCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
 * @brief Register to when gestures are added or removed
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	handler		[in]		The callback to be called when gesture configuration changes.
 * @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
 * @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
 */
XN_C_API XnStatus xnRegisterToGestureChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from when gestures are added or removed
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void xnUnregisterFromGestureChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Scene Analysis
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup scene Scene Analyzer
 * A Scene Analyzer node is a @ref mapgen that performs scene analysis. It supports all @ref mapgen functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates an scene analyzer.
 *
 * @param	pContext			[in]	The context in which to create the scene analyzer.
 * @param	phSceneAnalyzer		[out]	A handle to the created scene analyzer.
 * @param	pQuery				[in]	Optional. Can be used to select which scene analyzer to create. If not specified, this function may create any scene analyzer that is available.
 * @param	pErrors				[in]	Optional. If provided, will be filled with information about scene analyzers that could not be created.
 */
XN_C_API XnStatus xnCreateSceneAnalyzer(
	XnContext* pContext,
	XnNodeHandle* phSceneAnalyzer,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

/**
 * @brief Gets the label map, describing the current segmentation of the scene.
 * 
 * @param	hInstance	[in]		A handle to the instance
 */
XN_C_API const XnLabel* xnGetLabelMap(XnNodeHandle hInstance);
/**
 * @brief Gets a description of the floor, if it was found.
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	pPlane		[out]		A description of the floor
 */
XN_C_API XnStatus xnGetFloor(XnNodeHandle hInstance, XnPlane3D* pPlane);

/**
 * @brief Gets the current scene meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void xnGetSceneMetaData(XnNodeHandle hInstance, XnSceneMetaData* pMetaData);

/** @} */

//---------------------------------------------------------------------------
// User Generator
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup user User Generator
 * A User Generator node is a @ref generator that identifies a user in the scene. It supports all @ref generator functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates a user generator.
 *
 * @param	pContext			[in]	The context in which to create the user generator.
 * @param	phUserGenerator		[out]	A handle to the created user generator.
 * @param	pQuery				[in]	Optional. Can be used to select which user generator to create. If not specified, this function may create any user generator that is available.
 * @param	pErrors				[in]	Optional. If provided, will be filled with information about user generators that could not be created.
 */
XN_C_API XnStatus xnCreateUserGenerator(
	XnContext* pContext,
	XnNodeHandle* phUserGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

/**
 * @brief Get current number of users
 *
 * @param	hInstance	[in]		A handle to the instance
 */
XN_C_API XnUInt16 xnGetNumberOfUsers(XnNodeHandle hInstance);
/**
 * @brief Get the current users
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	pUsers		[out]		Preallocated memory for the users
 * @param	pnUsers		[in,out]	The size of the preallocated memory. Changes to the number of users
 */
XN_C_API XnStatus xnGetUsers(XnNodeHandle hInstance, XnUserID* pUsers, XnUInt16* pnUsers);
/**
 * @brief Get the center of mass of a user
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	user		[in]		The user for which to get the center of mass
 * @param	pCoM		[out]		The center of mass
 */
XN_C_API XnStatus xnGetUserCoM(XnNodeHandle hInstance, XnUserID user, XnPoint3D* pCoM);
/**
 * @brief Get the pixels that belong to a user.
 *
 * The output is in a XnSceneMetaData object, holding a map of the pixels of the scene,
 * with the user's label as the value in the places corresponding with the user's pixels.
 * Pixels outside the user will not contain the user's ID.
 * ID 0 means a request for the pixels of all users. In this case, the background will get the label 0.
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	user		[in]		The user for which to get the pixels
 * @param	pScene		[out]		XnSceneMetaData object, that holds the pixels of the user
 */
XN_C_API XnStatus xnGetUserPixels(XnNodeHandle hInstance, XnUserID user, XnSceneMetaData* pScene);
/**
 * @brief Register to user callbacks
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	NewUserCB	[in]		Callback to be called when a new user is identified
 * @param	LostUserCB	[in]		Callback to be called when an existing user is no longer identified
 * @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
 * @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
 */
XN_C_API XnStatus xnRegisterUserCallbacks(XnNodeHandle hInstance, XnUserHandler NewUserCB, XnUserHandler LostUserCB, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from user callbacks
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void xnUnregisterUserCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Skeleton Capability
//---------------------------------------------------------------------------

/** 
 * @ingroup user
 * @defgroup skeleton Skeleton Capability
 * The Skeleton capability (@ref XN_CAPABILITY_SKELETON) allows a @ref user to also output the skeleton
 * data of the user, meaning, where every joint is located.
 * @{
 */

/**
 * @brief Check if generator supports a specific joint.
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	eJoint		[in]		The joint to check
 */
XN_C_API XnBool xnIsJointAvailable(XnNodeHandle hInstance, XnSkeletonJoint eJoint);
/**
* @brief Check if generator supports a specific profile.
*
* @param	hInstance	[in]		A handle to the instance
* @param	eProfile	[in]		The profile to check
*/
XN_C_API XnBool xnIsProfileAvailable(XnNodeHandle hInstance, XnSkeletonProfile eProfile);
/**
* @brief Set the profile. this will set some joints to be active, and others to be inactive.
*
* @param	hInstance	[in]		A handle to the instance
* @param	eProfile	[in]		The profile to set
*/
XN_C_API XnStatus xnSetSkeletonProfile(XnNodeHandle hInstance, XnSkeletonProfile eProfile);
/**
* @brief Change the state of a specific joint, to be active or inactive
*
* @param	hInstance	[in]		A handle to the instance
* @param	eJoint		[in]		The joint
* @param	bState		[in]		The new state
*/
XN_C_API XnStatus xnSetJointActive(XnNodeHandle hInstance, XnSkeletonJoint eJoint, XnBool bState);
/**
* @brief Check if joint is currently active
*
* @param	hInstance	[in]		A handle to the instance
* @param	eJoint		[in]		The joint to check
*/
XN_C_API XnBool xnIsJointActive(XnNodeHandle hInstance, XnSkeletonJoint eJoint);
/**
* @brief Register to joint configuration changes - when joints are activated and deactivated
*
* @param	hInstance	[in]		A handle to the instance
* @param	handler		[in]		The callback to be called when a joint is activated or deactivated
* @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus xnRegisterToJointConfigurationChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from joint configuration changes
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void xnUnregisterFromJointConfigurationChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
* @brief Get all active joints
*
* @param	hInstance	[in]		A handle to the instance
* @param	pJoints		[out]		Preallocated memory for the joints
* @param	pnJoints	[in,out]	The size of the preallocated memory. Changed to the number of the active joints.
*/
XN_C_API XnStatus xnEnumerateActiveJoints(XnNodeHandle hInstance, XnSkeletonJoint* pJoints, XnUInt16* pnJoints);
/**
* @brief Get a specific joint's full information
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The ID of the user to which the skeleton belongs
* @param	eJoint		[in]		The interesting joint
* @param	pJoint		[out]		The joint's full information
*/
XN_C_API XnStatus xnGetSkeletonJoint(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation* pJoint);
/**
* @brief Get a specific joint's position
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The ID of the user to which the skeleton belongs
* @param	eJoint		[in]		The interesting joint
* @param	pJoint		[out]		The joint's current position
*/
XN_C_API XnStatus xnGetSkeletonJointPosition(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition* pJoint);
/**
* @brief Get a specific joint's orientation
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The ID of the user to which the skeleton belongs
* @param	eJoint		[in]		The interesting joint
* @param	pJoint		[out]		The joint's current orientation
*/
XN_C_API XnStatus xnGetSkeletonJointOrientation(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation* pJoint);
/**
 * @brief Check if skeleton is being tracked
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	user		[in]		Id of the user to check
 */
XN_C_API XnBool xnIsSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Check if skeleton is being calibrated
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnBool xnIsSkeletonCalibrated(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Check if skeleton is being calibrated
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnBool xnIsSkeletonCalibrating(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Request calibration when possible
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
* @param	bForce		[in]		Disregard previous calibration
*/
XN_C_API XnStatus xnRequestSkeletonCalibration(XnNodeHandle hInstance, XnUserID user, XnBool bForce);
/**
* @brief stop calibration
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus xnAbortSkeletonCalibration(XnNodeHandle hInstance, XnUserID user);
/**
 * @brief Save the calibration data
 *
 * @param	hInstance	[in]	A handle to the instance
 * @param	user		[in]	the user for which the calibration is saved
 * @param	nSlot		[in]	The slot to use for saving the calibration data
 */
XN_C_API XnStatus xnSaveSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
/**
* @brief Load previously saved calibration data.
*
* @param	hInstance	[in]	A handle to the instance
* @param	user		[in]	the user to be assigned the calibration data
* @param	nSlot		[in]	The slot to use for saving the calibration data.
*/
XN_C_API XnStatus xnLoadSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
/**
* @brief Clear the requested slot from any saved calibration data.
*
* @param	hInstance	[in]	A handle to the instance
* @param	nSlot		[in]	The slot to clear
*/
XN_C_API XnStatus xnClearSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
/**
* @brief Check if a specific slot already holds calibration data
*
* @param	hInstance	[in]	A handle to the instance
* @param	nSlot		[in]	The slot to check
*/
XN_C_API XnBool xnIsSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
/**
* @brief Start tracking a skeleton
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus xnStartSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Stop tracking a skeleton
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus xnStopSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Reset the skeleton - discard calibration
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus xnResetSkeleton(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Check if a specific pose is required for calibration
*
* @param	hInstance	[in]		A handle to the instance
*/
XN_C_API XnBool xnNeedPoseForSkeletonCalibration(XnNodeHandle hInstance);
/**
* @brief Get the pose that is required for calibration
*
* @param	hInstance	[in]		A handle to the instance
* @param	strPose		[in]		Name of the required pose
*/
XN_C_API XnStatus xnGetSkeletonCalibrationPose(XnNodeHandle hInstance, XnChar* strPose);
/**
* @brief Set the skeleton's smoothing factor
*
* @param	hInstance	[in]		A handle to the instance
* @param	fFactor		[in]		The smoothing factor
*/
XN_C_API XnStatus xnSetSkeletonSmoothing(XnNodeHandle hInstance, XnFloat fFactor);
/**
* @brief Register to calibration callbacks.
*
* @param	hInstance			[in]		A handle to the instance
* @param	CalibrationStartCB	[in]		Callback to be called when calibration is starting
* @param	CalibrationEndCB	[in]		Callback to be called when calibration is complete
* @param	pCookie				[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback			[out]		The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus xnRegisterCalibrationCallbacks(XnNodeHandle hInstance, XnCalibrationStart CalibrationStartCB, XnCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from calibration callbacks
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void xnUnregisterCalibrationCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Pose Detection Capability
//---------------------------------------------------------------------------

/** 
 * @ingroup user
 * @defgroup pose Pose Detection Capability
 * The Pose capability (@ref XN_CAPABILITY_POSE_DETECTION) allows a @ref user to recognize when the user
 * is in a specific pose.
 * @{
 */

/**
* @brief Get the number of the supported poses
*
* @param	hInstance	[in]		A handle to the instance
*/
XN_C_API XnUInt32 xnGetNumberOfPoses(XnNodeHandle hInstance);
/**
* @brief Get the names of the supported poses
*
* @param	hInstance	[in]		A handle to the instance
* @param	pstrPoses	[out]		Pre-allocated memory for the names of the supported poses
* @param	pnPoses		[in,out]	In input - size of the preallocated memory, in output - the number of pose names
*/
XN_C_API XnStatus xnGetAvailablePoses(XnNodeHandle hInstance, XnChar** pstrPoses, XnUInt32* pnPoses);
/**
* @brief Start detection of a specific pose for a specific user
*
* @param	hInstance	[in]		A handle to the instance
* @param	strPose		[in]		Name of requested pose
* @param	user		[in]		Id of user
*/
XN_C_API XnStatus xnStartPoseDetection(XnNodeHandle hInstance,const  XnChar* strPose, XnUserID user);
/**
* @brief Stop detection of poses for a specific user
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of user
*/
XN_C_API XnStatus xnStopPoseDetection(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Register to callbacks for pose events
*
* @param	hInstance	[in]		A handle to the instance
* @param	PoseDetectionStartCB	[in]	Function to be called when the pose is detected
* @param	PoseDetectionEndCB		[in]	Function to be called when the user is out of pose
* @param	pCookie					[in]	User's cookie, to be delivered to the callbacks
* @param	phCallback				[out]	The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus xnRegisterToPoseCallbacks(XnNodeHandle hInstance, XnPoseDetectionCallback PoseDetectionStartCB, XnPoseDetectionCallback PoseDetectionEndCB, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from pose events
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void xnUnregisterFromPoseCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Hands Generator
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup hands Hands Generator
 * A Hands Generator node is a @ref generator that tracks hand points. It supports all @ref generator functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates an hands generator.
 *
 * @param	pContext				[in]	The context in which to create the hands generator.
 * @param	phHandsGenerator		[out]	A handle to the created hands generator.
 * @param	pQuery					[in]	Optional. Can be used to select which hands generator to create. If not specified, this function may create any hands generator that is available.
 * @param	pErrors					[in]	Optional. If provided, will be filled with information about hands generators that could not be created.
 */
XN_C_API XnStatus xnCreateHandsGenerator(
	XnContext* pContext,
	XnNodeHandle* phHandsGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

/**
* @brief Register to hands callbacks
*
* @param	hInstance	[in]		A handle to the instance
* @param	CreateCB	[in]		Callback to be called when a new hand is created
* @param	UpdateCB	[in]		Callback to be called when an existing hand has a new position
* @param	DestroyCB	[in]		Callback to be called when an existing hand has vanished
* @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus xnRegisterHandCallbacks(XnNodeHandle hInstance, XnHandCreate CreateCB, XnHandUpdate UpdateCB, XnHandDestroy DestroyCB, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from hands callbacks
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void xnUnregisterHandCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
* @brief Stop tracking a specific hand
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The hand to stop tracking
*/
XN_C_API XnStatus xnStopTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Stop tracking all hands
*
* @param	hInstance	[in]		A handle to the instance
*/
XN_C_API XnStatus xnStopTrackingAll(XnNodeHandle hInstance);
/**
* @brief Start tracking at a specific position
*
* @param	hInstance	[in]		A handle to the instance
* @param	pPosition	[in]		The position at which there is a new hand
*/
XN_C_API XnStatus xnStartTracking(XnNodeHandle hInstance, const XnPoint3D* pPosition);
/**
* @brief Change smoothing factor
*
* @param	hInstance	[in]		A handle to the instance
* @param	fFactor		[in]		Smoothing factor, in the range 0..1. 0 Means no smoothing, 1 means infinite smoothing. Inside the range is generator dependent.
*/
XN_C_API XnStatus xnSetTrackingSmoothing(XnNodeHandle hInstance, XnFloat fFactor);

/** @} */

//---------------------------------------------------------------------------
// Audio Generator
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup audio Audio Generator
 * An Audio Generator node is a @ref generator that outputs audio data. It supports all @ref generator functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates an audio generator.
 *
 * @param	pContext				[in]	The context in which to create the audio generator.
 * @param	phAudioGenerator		[out]	A handle to the created audio generator.
 * @param	pQuery					[in]	Optional. Can be used to select which audio generator to create. If not specified, this function may create any audio generator that is available.
 * @param	pErrors					[in]	Optional. If provided, will be filled with information about audio generators that could not be created.
 */
XN_C_API XnStatus xnCreateAudioGenerator(
	XnContext* pContext,
	XnNodeHandle* phAudioGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

XN_C_API XnUChar* xnGetAudioBuffer(XnNodeHandle hInstance);
XN_C_API XnUInt32 xnGetSupportedWaveOutputModesCount(XnNodeHandle hInstance);
XN_C_API XnStatus xnGetSupportedWaveOutputModes(XnNodeHandle hInstance, XnWaveOutputMode* aSupportedModes, XnUInt32* pnCount);
XN_C_API XnStatus xnSetWaveOutputMode(XnNodeHandle hInstance, const XnWaveOutputMode* OutputMode);
XN_C_API XnStatus xnGetWaveOutputMode(XnNodeHandle hInstance, XnWaveOutputMode* OutputMode);
XN_C_API XnStatus xnRegisterToWaveOutputModeChanges(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
XN_C_API void xnUnregisterFromWaveOutputModeChanges(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Gets the current audio meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void xnGetAudioMetaData(XnNodeHandle hInstance, XnAudioMetaData* pMetaData);

/** @} */

//---------------------------------------------------------------------------
// Mocks
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup mocks Mock Nodes
 * @{
 */

XN_C_API XnStatus xnMockDepthSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnDepthPixel* pData);
XN_C_API XnStatus xnMockImageSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pData);
XN_C_API XnStatus xnMockIRSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnIRPixel* pData);
XN_C_API XnStatus xnMockAudioSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pData);

/** @} */

//---------------------------------------------------------------------------
// Codecs
//---------------------------------------------------------------------------

/**
 * @ingroup prd_node
 * @defgroup codec Codec
 * A Codec node is a @ref node that allows compression and decompression of data. It supports all @ref node functions,
 * and adds additional functions.
 * @{
 */

XN_C_API XnStatus xnCreateCodec(XnContext* pContext, XnCodecID codecID, XnNodeHandle hInitializerNode, XnNodeHandle* phCodec);
XN_C_API XnCodecID xnGetCodecID(XnNodeHandle hCodec);
XN_C_API XnStatus xnEncodeData(XnNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, 
							   void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten);
XN_C_API XnStatus xnDecodeData(XnNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, 
							   void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten);

/** @} */

//---------------------------------------------------------------------------
// Recorder
//---------------------------------------------------------------------------
/** 
 * @ingroup prd_node
 * @defgroup recorder Recorder
 * A Recorder node is a @ref node that allows recording of data. It supports all @ref node functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates a recorder.
 *
 * @param	pContext		[in]	The context in which to create the recorder.
 * @param	strFormatName	[in]	Optional. The name of the format the recorded file will be in. If NULL, file will be in ONI format.
 * @param	phRecorder		[out]	A pointer to the created recorder handle.
 */
XN_C_API XnStatus xnCreateRecorder(XnContext* pContext, const XnChar* strFormatName, XnNodeHandle* phRecorder);

/**
 * @brief Tells the recorder where to record.
 *
 * @param	hRecorder	[in]	A handle to the recorder
 * @param	destType	[in]	The type of medium to record to. Currently only XN_RECORD_MEDIUM_FILE is supported.
 * @param	strDest		[in]	Recording destination. If destType is XN_RECORD_MEDIUM_FILE, this specifies a file name.
 */
XN_C_API XnStatus xnSetRecorderDestination(XnNodeHandle hRecorder, XnRecordMedium destType, const XnChar* strDest);

/**
 * @brief Adds a node to recording and start recording it. This function must be called on each node that is to be 
 * recorded with this recorder.
 *
 * @param	hRecorder	[in]	A handle to the recorder.
 * @param	hNode		[in]	The node to add to the recording.
 * @param	compression	[in]	The type of compression that will be used to encode the node's data 
 *								(See @ref XnCodecIDs.h). If XN_CODEC_NULL is specified, a default compression 
 *								will be chosen according to the node type.
 */
XN_C_API XnStatus xnAddNodeToRecording(XnNodeHandle hRecorder, XnNodeHandle hNode, XnCodecID compression);

/**
 * @brief Removes node from recording and stop recording it. This function can be called on a node that was added to 
 * recording with @ref xnAddNodeToRecording.
 *
 * @param	hRecorder	[in]	A handle to the recorder.
 * @param	hNode		[in]	The node to remove from recording.
 */
XN_C_API XnStatus xnRemoveNodeFromRecording(XnNodeHandle hRecorder, XnNodeHandle hNode);

/**
 * @brief Records one frame of data from each node that was added to the recorder with @ref xnAddNodeToRecording.
 *
 * @param	hRecorder	[in]	A handle to the recorder.
 */
XN_C_API XnStatus xnRecord(XnNodeHandle hRecorder);

/**
 * @brief Gets a string representation of the recording format.
 *
 * @param	hRecorder	[in]	A handle to the recorder.
 */
XN_C_API const XnChar* xnGetRecorderFormat(XnNodeHandle hRecorder);

/** @} */

//---------------------------------------------------------------------------
// Player
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup player Player
 * A Player node is a @ref node that allows playing of a recording. It supports all @ref node functions,
 * and adds additional functions.
 * @{
 */

/**
 * @brief Creates a player.
 *
 * @param	pContext		[in]	The context in which to create the player.
 * @param	strFormatName	[in]	The name of the recording format that the created player should play.
 * @param	phPlayer		[out]	Pointer to the created player handle.
 */
XN_C_API XnStatus xnCreatePlayer(XnContext* pContext, const XnChar* strFormatName, XnNodeHandle* phPlayer);

/**
 * @brief Determines whether the player will automatically rewind to the beginning of the recording when reaching the end.
 *
 * @param	hPlayer	[in]	A handle to the player.
 * @param	bRepeat	[in]	Determines whether the player will repeat or not.
 * 
 * @remark If bRepeat is set to TRUE, the player will automatically rewind when reaching the end.
 * @remark If bRepeat is set to FALSE, the player will stop playing when reaching the end, and will raise the event that was registered with @ref xnRegisterToEndOfFileReached(). In this state, @ref xnIsPlayerAtEOF() returns TRUE.
 */
XN_C_API XnStatus xnSetPlayerRepeat(XnNodeHandle hPlayer, XnBool bRepeat);

/**
 * @brief Sets the source for the player, i.e. where the played events will come from. 
 
 * The only supported source type at this point is a file.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	sourceType	[in]	The type of source to set. Currently only XN_RECORD_MEDIUM_FILE is supported.
 * @param	strSource	[in]	The source from which to play. If sourceType is XN_RECORD_MEDIUM_FILE, strSource specifies a file name.
 *
 * @sa xnGetPlayerSource()
 */
XN_C_API XnStatus xnSetPlayerSource(XnNodeHandle hPlayer, XnRecordMedium sourceType, const XnChar* strSource);

/**
 * @brief Gets the player's source, i.e where the played events come from.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	pSourceType	[out]	The player's source type.
 * @param	strSource	[in]	The player's source.
 * @param	nBufSize	[in]	The size of the buffer specified by strSource.
 *
 * @sa xnSetPlayerSource()
 */
XN_C_API XnStatus xnGetPlayerSource(XnNodeHandle hPlayer, XnRecordMedium* pSourceType, XnChar* strSource, XnUInt32 nBufSize);

/**
 * @brief Reads the next data element from the player. 
 *
 * Can cause data updates and/or property updates of the played nodes.
 *
 * @param	hPlayer	[in]	A handle to the player.
 */
XN_C_API XnStatus xnPlayerReadNext(XnNodeHandle hPlayer);

/**
 * @brief Seeks the player to a specific timestamp, so that playing will continue from that point onwards.
 * @note For the built-in ONI player, this function is not currently supported and always returns XN_STATUS_NOT_IMPLEMENTED.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	nTimeOffset	[in]	The offset, in microseconds, to move, relative to the specified origin. See the remark below.
 * @param	origin		[in]	The origin to seek from. See the remark below.
 *
 * @remark The meaning of the nTimeOffset parameter changes according to the origin parameter:
 *
 *	<table>
 *		<tr><td><b>origin</b></td><td><b>Meaning of the nTimeOffset parameter</b></td>
 *		<tr><td>XN_PLAYER_SEEK_SET</td><td>nTimeOffset specifies the total time passed since the beginning of the recording. Note that nTimeOffset will be case to XnUInt64 in this case.</td>
 *		<tr><td>XN_PLAYER_SEEK_CUR</td><td>nTimeOffset specifies a period of time to move, relative to the current player position. A positive value means move forward, and a negative value means move backwards.</td>
 *		<tr><td>XN_PLAYER_SEEK_END</td><td>nTimeOffset specifies a period of time to move, relative to the end of the recording. This must be a negative value.</td></tr>
 *	</table>
 */
XN_C_API XnStatus xnSeekPlayerToTimeStamp(XnNodeHandle hPlayer, XnInt64 nTimeOffset, XnPlayerSeekOrigin origin);

/**
 * @brief Seeks the player to a specific frame of a specific played node, so that playing will continue from that frame onwards.
 *
 * @param	hPlayer			[in]	A handle to the player.
 * @param	strNodeName		[in]	The name of the node whose frame is to be sought.
 * @param	nFrameOffset	[in]	The number of frames to move, relative to the specified origin. See the remark below.
 * @param	origin			[in]	The origin to seek from. See the remark below.
 *
 * @remark The meaning of the nTimeOffset parameter changes according to the origin parameter:
 *
 *	<table>
 *		<tr><td><b>origin</b></td><td><b>Meaning of the nFrameOffset parameter</b></td>
 *		<tr><td>XN_PLAYER_SEEK_SET</td><td>nFrameOffset specifies the total number of frames since the beginning of the node's recording. Note that nFrameOffset will be case to XnUInt64 in this case.</td>
 *		<tr><td>XN_PLAYER_SEEK_CUR</td><td>nFrameOffset specifies the number of frames to move, relative to the current frame of the specifies node. A positive value means move forward, and a negative value means move backwards.</td>
 *		<tr><td>XN_PLAYER_SEEK_END</td><td>nFrameOffset specifies the number of frames to move, relative to the end of the node's recording. This must be a negative value.</td></tr>
 *	</table>
 *
 * @note You can get different results using this function for different values of strNodeName, because the frame numbers of different nodes are not necessarily in sync.
 */
XN_C_API XnStatus xnSeekPlayerToFrame(XnNodeHandle hPlayer, const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin);

/**
 * @brief Reports the current timestamp of a player, i.e. the amount of time passed since the beginning of the recording.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	pnTimestamp	[out]	A pointer to the retrieved timestamp in microseconds.
 */
XN_C_API XnStatus xnTellPlayerTimestamp(XnNodeHandle hPlayer, XnUInt64* pnTimestamp);

/**
 * @brief Reports the current frame number of a specific node played by a player.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	strNodeName	[in]	The name of the node for which to report the frame number.
 * @param	pnFrame		[out]	A pointer to the reported frame number.
 *
 * @remark Different nodes that belong to the player can have different frame numbers, because the nodes are not necessarily in sync.
 */
XN_C_API XnStatus xnTellPlayerFrame(XnNodeHandle hPlayer, const XnChar* strNodeName, XnUInt32* pnFrame);

/**
 * @brief Retrieves the number of frames of a specific node played by a player.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	strNodeName	[in]	The name of the node for which to retrieve the number of frames.
 * @param	pnFrames	[out]	A pointer to the retrieved number of frames.
 */
XN_C_API XnStatus xnGetPlayerNumFrames(XnNodeHandle hPlayer, const XnChar* strNodeName, XnUInt32* pnFrames);

/**
 * @brief Gets the name of the format supported by a player.
 *
 * @param	hPlayer	[in]	A handle to the player.
 * @returns	The name of the format supported by the player, or NULL if hPlayer is NULL.
 */
XN_C_API const XnChar* xnGetPlayerSupportedFormat(XnNodeHandle hPlayer);

/**
 * @brief Retrieves a list of the nodes played by a player.
 *
 * @param	hPlayer	[in]	A handle to the player.
 * @param	ppList	[out]	A pointer to the retrieved list of nodes.
 *
 * @remark This function fills the pointer that is pointed to by ppList with a newly allocated node info list
 * of the played nodes. This list must later be freed by the caller with @ref xnNodeInfoListFree().
 */
XN_C_API XnStatus xnEnumeratePlayerNodes(XnNodeHandle hPlayer, XnNodeInfoList** ppList);

/**
 * @brief Checks whether the player is at the end-of-file marker.
 * @note In the built-in ONI player, this function will never return TRUE for a player that is in repeat mode, although the event registered with @ref xnRegisterToEndOfFileReached() will be fired.
 *
 * @param	hPlayer	[in]	A handle to the player.
 *
 * @sa xnSetPlayerRepeat()
 */
XN_C_API XnBool xnIsPlayerAtEOF(XnNodeHandle hPlayer);

/**
 * @brief Registers a callback function to be called when end-of-file is reached.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	handler		[in]	A pointer to a function that will be called.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Will be filled with a handle to be passed to @ref xnUnregisterFromEndOfFileReached().
 *
 * @sa xnSetPlayerRepeat()
 */
XN_C_API XnStatus xnRegisterToEndOfFileReached
	(XnNodeHandle hPlayer, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToEndOfFileReached().
 *
 * @param	hInstance	[in]	A handle to the player.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToEndOfFileReached().
 */
XN_C_API void xnUnregisterFromEndOfFileReached
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

#endif // __XN_PRD_NODE_H__
