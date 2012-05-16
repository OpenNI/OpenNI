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
XN_C_API XnNodeInfo* XN_C_DECL xnGetNodeInfo(XnNodeHandle hNode);

/**
 * @brief Gets the instance name of a node by its handle.
 *
 * @param	hNode	[in]	A handle to the node.
 */
XN_C_API const XnChar* XN_C_DECL xnGetNodeName(XnNodeHandle hNode);

/**
 * @brief Gets the context a node belongs to. The context ref count is increased. The user
 * is responsible for releasing the context when it's no longer used.
 *
 * @param	hNode	[in]	A handle to the node.
 */
XN_C_API XnContext* XN_C_DECL xnGetRefContextFromNodeHandle(XnNodeHandle hNode);

typedef XnContext* XnContextPtr;
XN_C_API XnContextPtr XN_API_DEPRECATED("Please use xnGetRefContextFromNodeHandle() instead") XN_C_DECL xnGetContextFromNodeHandle(XnNodeHandle hNode);

/**
 * @brief Checks if a production node supports specific capability.
 * 
 * @param	hInstance			[in]	A handle to the instance to be queried.
 * @param	strCapabilityName	[in]	The name of the capability to check.
 */
XN_C_API XnBool XN_C_DECL xnIsCapabilitySupported(XnNodeHandle hInstance, const XnChar* strCapabilityName);

/**
 * @brief Sets an integer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	nValue		[in]	New value
 */
XN_C_API XnStatus XN_C_DECL xnSetIntProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt64 nValue);
/**
 * @brief Sets a real property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	dValue		[in]	New value
 */
XN_C_API XnStatus XN_C_DECL xnSetRealProperty(XnNodeHandle hInstance, const XnChar* strName, XnDouble dValue);
/**
 * @brief Sets a string property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	strValue	[in]	New value
 */
XN_C_API XnStatus XN_C_DECL xnSetStringProperty(XnNodeHandle hInstance, const XnChar* strName, const XnChar* strValue);
/**
 * @brief Sets a buffer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	nBufferSize	[in]	The size of the buffer passed, in bytes.
 * @param	pBuffer		[in]	The data buffer.
 */
XN_C_API XnStatus XN_C_DECL xnSetGeneralProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

/**
 * @brief Gets an integer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	pnValue		[out]	Current value
 */
XN_C_API XnStatus XN_C_DECL xnGetIntProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt64* pnValue);
/**
 * @brief Gets a real property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	pdValue		[out]	Current value
 */
XN_C_API XnStatus XN_C_DECL xnGetRealProperty(XnNodeHandle hInstance, const XnChar* strName, XnDouble* pdValue);
/**
 * @brief Gets a string property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	csValue		[out]	Current value 
 * @param	nBufSize	[in]	The size of the csValue buffer. 
 */
XN_C_API XnStatus XN_C_DECL xnGetStringProperty(XnNodeHandle hInstance, const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize);
/**
 * @brief Gets a buffer property.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	strName		[in]	Property name
 * @param	nBufferSize	[in]	The size of the buffer passed, in bytes.
 * @param	pBuffer		[in]	The data buffer to be filled.
 */
XN_C_API XnStatus XN_C_DECL xnGetGeneralProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer);

/**
 * @brief Locks a node, not allowing any changes (any "set" function).
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	phLock		[out]	A handle of the lock, that can be used to manage the lock.
 */
XN_C_API XnStatus XN_C_DECL xnLockNodeForChanges(XnNodeHandle hInstance, XnLockHandle* phLock);

/**
 * @brief Unlocks a previously locked node.
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hLock		[in]	The handle of the lock, returned from @ref xnLockNodeForChanges().
 */
XN_C_API XnStatus XN_C_DECL xnUnlockNodeForChanges(XnNodeHandle hInstance, XnLockHandle hLock);

/**
 * @brief Start changes request on a locked node, without releasing that lock. 
 *
 * It allows "set" operations from the same context (same thread for that matter) until a call is made to 
 * @ref xnLockedNodeEndChanges().
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hLock		[in]	The handle of the lock, returned from @ref xnLockNodeForChanges().
 */
XN_C_API XnStatus XN_C_DECL xnLockedNodeStartChanges(XnNodeHandle hInstance, XnLockHandle hLock);

/**
 * @brief Ends changes request on a locked node. 
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hLock		[in]	The handle of the lock, returned from @ref xnLockNodeForChanges().
 */
XN_C_API XnStatus XN_C_DECL xnLockedNodeEndChanges(XnNodeHandle hInstance, XnLockHandle hLock);

/**
 * @brief Adds another node to the list of needed nodes for this node.
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hNeededNode	[in]	The node that is needed by hInstance.
 */
XN_C_API XnStatus XN_C_DECL xnAddNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);

/**
 * @brief Removes a needed node from the list of needed nodes. 
 *
 * If this node is no longer needed by any other node, and @ref xnRefProductionNode() was not called on it, 
 * it will be destroyed.
 *
 * @param	hInstance	[in]	A handle to the node.
 * @param	hNeededNode	[in]	The node to be removed from the list.
 */
XN_C_API XnStatus XN_C_DECL xnRemoveNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);

/** @} */

//---------------------------------------------------------------------------
// Device
//---------------------------------------------------------------------------

/**
 * @ingroup prd_node
 * @defgroup device Device
 *
 * A device node represents a physical device. The following functions can be 
 * executed for device nodes in an OpenNI context.
 * @{
 */

/**
 * @brief Creates a device node.
 *
 * @param	pContext			[in]	The context in which to create the device
 * @param	phDevice			[out]	A handle to the created device
 * @param	pQuery				[in]	Optional. Can be used to select which device to create. If not specified, this function may create any device node that is available.
 * @param	pErrors				[in]	Optional. If provided, will be filled with information about device nodes that could not be created.
 */
XN_C_API XnStatus XN_C_DECL xnCreateDevice(XnContext* pContext, XnNodeHandle* phDevice, XnNodeQuery* pQuery, XnEnumerationErrors* pErrors);

/** @} */

//---------------------------------------------------------------------------
// Device Identification Capability
//---------------------------------------------------------------------------

/**
 * @ingroup device
 * @defgroup device_id Device Identification Capability
 *
 * The Device Identification Capability (@ref XN_CAPABILITY_DEVICE_IDENTIFICATION) allows to identify
 * the device according to its name, to a vendor-specific string and its serial number.
 * @{
 */

/**
 * Gets the device name.
 *
 * @param	hInstance		[in]		A handle to the instance.
 * @param	strBuffer		[in]		A buffer to accept the device name.
 * @param	pnBufferSize	[in/out]	Size of the buffer.
 *
 * @returns XN_STATUS_OK if succeeded, or XN_STATUS_OUTPUT_BUFFER_OVERFLOW if buffer is not sufficient.
 * in such a case, the device name should be truncated to fit in the buffer, and pnBufferSize should be
 * updated to the required size.
 */
XN_C_API XnStatus XN_C_DECL xnGetDeviceName(XnNodeHandle hInstance, XnChar* strBuffer, XnUInt32* pnBufferSize);

/**
 * Gets a vendor-specific string.
 *
 * @param	hInstance		[in]		A handle to the instance.
 * @param	strBuffer		[in]		A buffer to accept the string.
 * @param	pnBufferSize	[in/out]	Size of the buffer.
 *
 * @returns XN_STATUS_OK if succeeded, or XN_STATUS_OUTPUT_BUFFER_OVERFLOW if buffer is not sufficient.
 * in such a case, the string should be truncated to fit in the buffer, and pnBufferSize should be
 * updated to the required size.
 */
XN_C_API XnStatus XN_C_DECL xnGetVendorSpecificData(XnNodeHandle hInstance, XnChar* strBuffer, XnUInt32* pnBufferSize);

/**
 * Gets the serial number of the device.
 *
 * @param	hInstance		[in]		A handle to the instance.
 * @param	strBuffer		[in]		A buffer to accept the string.
 * @param	pnBufferSize	[in/out]	Size of the buffer.
 *
 * @returns XN_STATUS_OK if succeeded, or XN_STATUS_OUTPUT_BUFFER_OVERFLOW if buffer is not sufficient.
 * in such a case, the string should be truncated to fit in the buffer, and pnBufferSize should be
 * updated to the required size.
 */
XN_C_API XnStatus XN_C_DECL xnGetSerialNumber(XnNodeHandle hInstance, XnChar* strBuffer, XnUInt32* pnBufferSize);

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
XN_C_API XnStatus XN_C_DECL xnGetNodeErrorState(XnNodeHandle hInstance);

/**
 * @brief Registers a callback function to error state changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when error state changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromNodeErrorStateChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToNodeErrorStateChange
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToNodeErrorStateChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToNodeErrorStateChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromNodeErrorStateChange
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// GeneralIntCapability
//---------------------------------------------------------------------------

/** 
 * @ingroup node
 * @defgroup general_int General Int Capability
 *
 * The General Int Capability is a set of method that is used by several capabilities. It allows
 * a general range value to be changed. If supported by the node, the value can also be set to 
 * @ref XN_AUTO_CONTROL for automatic adjustment.
 *
 * The capabilities supporting this interface are:
 * - Brightness (@ref XN_CAPABILITY_BRIGHTNESS)
 * - Contrast (@ref XN_CAPABILITY_CONTRAST)
 * - Hue (@ref XN_CAPABILITY_HUE)
 * - Saturation (@ref XN_CAPABILITY_SATURATION)
 * - Sharpness (@ref XN_CAPABILITY_SHARPNESS)
 * - Gamma (@ref XN_CAPABILITY_GAMMA)
 * - WhiteBalance (@ref XN_CAPABILITY_COLOR_TEMPERATURE)
 * - BacklightCompensation (@ref XN_CAPABILITY_BACKLIGHT_COMPENSATION)
 * - Gain (@ref XN_CAPABILITY_GAIN)
 * - Pan (@ref XN_CAPABILITY_PAN)
 * - Tilt (@ref XN_CAPABILITY_TILT)
 * - Roll (@ref XN_CAPABILITY_ROLL)
 * - Zoom (@ref XN_CAPABILITY_ZOOM)
 * - Exposure (@ref XN_CAPABILITY_EXPOSURE)
 * - Iris (@ref XN_CAPABILITY_IRIS)
 * - Focus (@ref XN_CAPABILITY_FOCUS)
 * - Low Light Compensation (@ref XN_CAPABILITY_LOW_LIGHT_COMPENSATION)
 * @{
 */

/**
 * @brief Gets the range of this capability values
 *
 * @param	hNode				[in]	A handle to the instance
 * @param	strCap				[in]	Name of the capability
 * @param	pnMin				[out]	Minimum value
 * @param	pnMax				[out]	Maximum value	
 * @param	pnStep				[out]	Step size
 * @param	pnDefault			[out]	Default value
 * @param	pbIsAutoSupported	[out]	TRUE if auto adjustment is supported, FALSE otherwise
 */
XN_C_API XnStatus XN_C_DECL xnGetGeneralIntRange(XnNodeHandle hNode, const XnChar* strCap, XnInt32* pnMin, XnInt32* pnMax, XnInt32* pnStep, XnInt32* pnDefault, XnBool* pbIsAutoSupported);

/**
 * @brief Gets the current value of this capability
 *
 * @param	hNode				[in]	A handle to the instance
 * @param	strCap				[in]	Name of the capability
 * @param	pnValue				[out]	Current value
 */
XN_C_API XnStatus XN_C_DECL xnGetGeneralIntValue(XnNodeHandle hNode, const XnChar* strCap, XnInt32* pnValue);

/**
 * @brief Sets the current value of this capability
 *
 * @param	hNode				[in]	A handle to the instance
 * @param	strCap				[in]	Name of the capability
 * @param	nValue				[in]	Value to set
 */
XN_C_API XnStatus XN_C_DECL xnSetGeneralIntValue(XnNodeHandle hNode, const XnChar* strCap, XnInt32 nValue);

/**
 * @brief Registers a callback function to values changes.
 *
 * @param	hNode		[in]	A handle to the instance.
 * @param	strCap		[in]	Name of the capability
 * @param	handler		[in]	A pointer to a function that will be called when value changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromGeneralIntValueChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToGeneralIntValueChange
	(XnNodeHandle hNode, const XnChar* strCap, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToGeneralIntValueChange().
 *
 * @param	hNode		[in]	A handle to the instance.
 * @param	strCap		[in]	Name of the capability
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToGeneralIntValueChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromGeneralIntValueChange
	(XnNodeHandle hNode, const XnChar* strCap, XnCallbackHandle hCallback);

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
XN_C_API XnStatus XN_C_DECL xnStartGenerating(XnNodeHandle hInstance);

/** 
 * @brief Checks if this node is currently generating.
 * 
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns FALSE if this production node is not a generator.
 */
XN_C_API XnBool XN_C_DECL xnIsGenerating(XnNodeHandle hInstance);

/** 
 * @brief Stops generation of the output.
 * 
 * @param	hInstance	[in]	A handle to the instance to stop generating.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a generator.
 */
XN_C_API XnStatus XN_C_DECL xnStopGenerating(XnNodeHandle hInstance);

/**
 * @brief Registers a callback function to be called when generation starts or stops.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when generation starts/stops.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromGenerationRunningChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToGenerationRunningChange
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToGenerationRunningChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToGenerationRunningChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromGenerationRunningChange
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
XN_C_API XnStatus XN_C_DECL xnRegisterToNewDataAvailable
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToNewDataAvailable().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToNewDataAvailable().
 */
XN_C_API void XN_C_DECL xnUnregisterFromNewDataAvailable
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Checks whether this node has new data (and so a call to @ref xnWaitAndUpdateData() will not block).
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pnTimestamp	[out]	[Optional] If not NULL, will be filled with that data timestamp.
 */
XN_C_API XnBool XN_C_DECL xnIsNewDataAvailable(XnNodeHandle hInstance, XnUInt64* pnTimestamp);

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
XN_C_API XnStatus XN_C_DECL xnWaitAndUpdateData(XnNodeHandle hInstance);

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
XN_C_API XnBool XN_C_DECL xnIsDataNew(XnNodeHandle hInstance);

/**
 * @brief Gets the current data.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	NULL if this production node is not a generator.
 */
XN_C_API const void* XN_C_DECL xnGetData(XnNodeHandle hInstance);

/**
 * @brief Gets the size of current data, in bytes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	(XnUInt32)-1 if this production node is not a generator.
 */
XN_C_API XnUInt32 XN_C_DECL xnGetDataSize(XnNodeHandle hInstance);

/**
 * @brief Gets the timestamp of current data, in microseconds.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	(XnUInt64)-1 if this production node is not a generator.
 */
XN_C_API XnUInt64 XN_C_DECL xnGetTimestamp(XnNodeHandle hInstance);

/**
 * @brief Gets the frame ID of current data.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	(XnUInt32)-1 if this production node is not a generator.
 */
XN_C_API XnUInt32 XN_C_DECL xnGetFrameID(XnNodeHandle hInstance);

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
XN_C_API XnStatus XN_C_DECL xnSetMirror(XnNodeHandle hInstance, XnBool bMirror);

/**
 * @brief Gets current mirroring configuration.
 *
 * @param	hInstance	[in]	A handle to the instance.
 */
XN_C_API XnBool XN_C_DECL xnIsMirrored(XnNodeHandle hInstance);

/**
 * @brief Registers a callback function to mirror changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when mirror changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromMirrorChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToMirrorChange
	(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToMirrorChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToMirrorChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromMirrorChange
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
XN_C_API XnBool XN_C_DECL xnIsViewPointSupported(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Sets the view point of this generator to look like as if placed at another generator location.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The view point to be set.
 */
XN_C_API XnStatus XN_C_DECL xnSetViewPoint(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Sets the view point of this generator to its normal one.
 *
 * @param	hInstance	[in]	A handle to the instance.
 */
XN_C_API XnStatus XN_C_DECL xnResetViewPoint(XnNodeHandle hInstance);

/**
 * @brief Checks if current view point is as if coming from the other node view point.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The view point to be checked.
 */
XN_C_API XnBool XN_C_DECL xnIsViewPointAs(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Registers a callback function to view point changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when view point changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromViewPointChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToViewPointChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToViewPointChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToViewPointChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromViewPointChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

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
XN_C_API XnBool XN_C_DECL xnCanFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Activates frame sync with the other node.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The node to sync to.
 */
XN_C_API XnStatus XN_C_DECL xnFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Stops frame sync with the other node.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The node to sync to.
 */
XN_C_API XnStatus XN_C_DECL xnStopFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Checks if current view point is as if coming from the other node view point.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hOther		[in]	The node to be checked.
 */
XN_C_API XnBool XN_C_DECL xnIsFrameSyncedWith(XnNodeHandle hInstance, XnNodeHandle hOther);

/**
 * @brief Registers a callback function to view point changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when frame sync changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromFrameSyncChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToFrameSyncChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToFrameSyncChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToFrameSyncChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromFrameSyncChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

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
XN_C_API XnUInt32 XN_C_DECL xnGetSupportedMapOutputModesCount(XnNodeHandle hInstance);

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
XN_C_API XnStatus XN_C_DECL xnGetSupportedMapOutputModes(XnNodeHandle hInstance, XnMapOutputMode* aModes, XnUInt32* pnCount);

/** 
 * @brief Sets the output mode. 
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pOutputMode	[in]	The output mode to be set.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a map generator.
 */
XN_C_API XnStatus XN_C_DECL xnSetMapOutputMode(XnNodeHandle hInstance, const XnMapOutputMode* pOutputMode);

/** 
 * @brief Gets the current output mode. 
 * 
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pOutputMode	[out]	Current output mode.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a map generator.
 */
XN_C_API XnStatus XN_C_DECL xnGetMapOutputMode(XnNodeHandle hInstance, XnMapOutputMode* pOutputMode);

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
XN_C_API XnStatus XN_C_DECL xnRegisterToMapOutputModeChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToMapOutputModeChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToMapOutputModeChange.
 */
XN_C_API void XN_C_DECL xnUnregisterFromMapOutputModeChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Gets the number of bytes per pixel for this map generator.
 *
 * @param	hInstance	[in]	A handle to the instance.
 *
 * @returns	(XnUInt32)-1 if this production node is not a map generator.
 */
XN_C_API XnUInt32 XN_C_DECL xnGetBytesPerPixel(XnNodeHandle hInstance);

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
XN_C_API XnStatus XN_C_DECL xnSetCropping(XnNodeHandle hInstance, const XnCropping* pCropping);

/** 
 * @brief Gets current cropping configuration.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pCropping	[out]	Current cropping configuration.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the Cropping capability.
 */
XN_C_API XnStatus XN_C_DECL xnGetCropping(XnNodeHandle hInstance, XnCropping* pCropping);

/**
 * @brief Registers a callback function to cropping changes.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when cropping changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromCroppingChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToCroppingChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToCroppingChange().
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from xnRegisterToCroppingChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromCroppingChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/** @} */

//---------------------------------------------------------------------------
// Anti Flicker Capability
//---------------------------------------------------------------------------

/**
 * @ingroup mapgen
 * @defgroup anti_flicker Anti Flicker Capability
 * The Anti Flicker capability (@ref XN_CAPABILITY_ANTI_FLICKER) allows a @ref mapgen to time its frames
 * in such a way that no flickering will take place due to electric light frequencies.
 * @{
 */

/** 
 * @brief Sets the power line frequency: 50 Hz, 60 Hz, or 0 to turn off anti-flicker.
 *
 * @param	hGenerator	[in]	A handle to the instance.
 * @param	nFrequency	[in]	The frequency to be used.
 */
XN_C_API XnStatus XN_C_DECL xnSetPowerLineFrequency(XnNodeHandle hGenerator, XnPowerLineFrequency nFrequency);

/** 
 * @brief Gets the power line frequency.
 *
 * @param	hGenerator	[in]	A handle to the instance.
 * @returns (XnUInt32)-1 if node does not support this capability
 */
XN_C_API XnPowerLineFrequency XN_C_DECL xnGetPowerLineFrequency(XnNodeHandle hGenerator);

/**
 * @brief Registers a callback function to power line frequency changes.
 *
 * @param	hGenerator	[in]	A handle to the instance.
 * @param	handler		[in]	A pointer to a function that will be called when power line frequency changes.
 * @param	pCookie		[in]	A user cookie that will be passed to the callback function.
 * @param	phCallback	[out]	Optional. Will be filled with a handle to be passed to @ref xnUnregisterFromPowerLineFrequencyChange().
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToPowerLineFrequencyChange
	(XnNodeHandle hGenerator, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToPowerLineFrequencyChange().
 *
 * @param	hGenerator	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToPowerLineFrequencyChange().
 */
XN_C_API void XN_C_DECL xnUnregisterFromPowerLineFrequencyChange
	(XnNodeHandle hGenerator, XnCallbackHandle hCallback);

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
XN_C_API XnStatus XN_C_DECL xnCreateDepthGenerator(
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
XN_C_API XnDepthPixel XN_C_DECL xnGetDeviceMaxDepth(XnNodeHandle hInstance);

/** 
 * @brief Gets the Field-Of-View of the depth generator, in radians.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	pFOV		[in]	A struct to be filled with field of view.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not a depth generator.
 */
XN_C_API XnStatus XN_C_DECL xnGetDepthFieldOfView(XnNodeHandle hInstance, XnFieldOfView* pFOV);

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
XN_C_API XnStatus XN_C_DECL xnRegisterToDepthFieldOfViewChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToDepthFieldOfViewChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToDepthFieldOfViewChange.
 */
XN_C_API void XN_C_DECL xnUnregisterFromDepthFieldOfViewChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

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
XN_C_API XnStatus XN_C_DECL xnConvertProjectiveToRealWorld(
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
XN_C_API XnStatus XN_C_DECL xnConvertRealWorldToProjective(
	XnNodeHandle hInstance, XnUInt32 nCount, const XnPoint3D* aRealWorld, XnPoint3D* aProjective);

/** 
 * @brief Gets the current depth-map. This map is updated after a call to @ref xnWaitAndUpdateData(). 
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not a depth generator.
 */
XN_C_API XnDepthPixel* XN_C_DECL xnGetDepthMap(XnNodeHandle hInstance);

/**
 * @brief Gets the current depth-map meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void XN_C_DECL xnGetDepthMetaData(XnNodeHandle hInstance, XnDepthMetaData* pMetaData);

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
XN_C_API XnUInt32 XN_C_DECL xnGetSupportedUserPositionsCount(XnNodeHandle hInstance);

/** 
 * @brief Sets the current user position. 
 * 
 * @param	hInstance	[in]		A handle to the instance.
 * @param	nIndex		[in]		The user position to set.
 * @param	pPosition	[in]		The user position in the frame.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the User Position capability.
 */
XN_C_API XnStatus XN_C_DECL xnSetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, const XnBoundingBox3D* pPosition);

/** 
 * @brief Gets the current user position. 
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	nIndex		[in]	The user position to get.
 * @param	pPosition	[out]	The user position.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node does not support the User Position capability.
 */
XN_C_API XnStatus XN_C_DECL xnGetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, XnBoundingBox3D* pPosition);

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
XN_C_API XnStatus XN_C_DECL xnRegisterToUserPositionChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToUserPositionChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToUserPositionChange.
 */
XN_C_API void XN_C_DECL xnUnregisterFromUserPositionChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

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
XN_C_API XnStatus XN_C_DECL xnCreateImageGenerator(
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
XN_C_API XnRGB24Pixel* XN_C_DECL xnGetRGB24ImageMap(XnNodeHandle hInstance);

/** 
 * @brief Gets the current YUV422 image-map. 
 * This map is updated after a call to @ref xnWaitAndUpdateData(). It is assumed that the node is currently in YUV422 pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnYUV422DoublePixel* XN_C_DECL xnGetYUV422ImageMap(XnNodeHandle hInstance);

/** 
 * @brief Gets the current Grayscale8 image-map. 
 * This map is updated after a call to @ref xnWaitAndUpdateData(). It is assumed that the node is currently in Grayscale8 pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnGrayscale8Pixel* XN_C_DECL xnGetGrayscale8ImageMap(XnNodeHandle hInstance);

/** 
 * @brief Gets the current Grayscale16 image-map. 
 * This map is updated after a call to @ref xnWaitAndUpdateData(). It is assumed that the node is currently in Grayscale16 pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	NULL if this production node is not an image generator.
 */
XN_C_API XnGrayscale16Pixel* XN_C_DECL xnGetGrayscale16ImageMap(XnNodeHandle hInstance);

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
XN_C_API XnUInt8* XN_C_DECL xnGetImageMap(XnNodeHandle hInstance);

/**
 * @brief Checks if a specific pixel format is supported.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	Format		[in]		The format to check for.
 */
XN_C_API XnBool XN_C_DECL xnIsPixelFormatSupported(XnNodeHandle hInstance, XnPixelFormat Format);

/**
 * @brief Sets the pixel format of the image map.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	Format		[in]		The format to set.
 *
 * @returns	XN_STATUS_INVALID_OPERATION if this production node is not an image generator.
 */
XN_C_API XnStatus XN_C_DECL xnSetPixelFormat(XnNodeHandle hInstance, XnPixelFormat Format);

/**
 * @brief Gets current pixel format.
 *
 * @param	hInstance	[in]		A handle to the instance.
 *
 * @returns	-1 if this production node is not an image generator.
 */
XN_C_API XnPixelFormat XN_C_DECL xnGetPixelFormat(XnNodeHandle hInstance);

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
XN_C_API XnStatus XN_C_DECL xnRegisterToPixelFormatChange(XnNodeHandle hInstance, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToPixelFormatChange.
 *
 * @param	hInstance	[in]	A handle to the instance.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToPixelFormatChange.
 */
XN_C_API void XN_C_DECL xnUnregisterFromPixelFormatChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Gets the current image-map meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void XN_C_DECL xnGetImageMetaData(XnNodeHandle hInstance, XnImageMetaData* pMetaData);

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
XN_C_API XnStatus XN_C_DECL xnCreateIRGenerator(
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
XN_C_API XnIRPixel* XN_C_DECL xnGetIRMap(XnNodeHandle hInstance);

/**
 * @brief Gets the current IR-map meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void XN_C_DECL xnGetIRMetaData(XnNodeHandle hInstance, XnIRMetaData* pMetaData);

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
XN_C_API XnStatus XN_C_DECL xnCreateGestureGenerator(
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
XN_C_API XnStatus XN_C_DECL xnAddGesture(XnNodeHandle hInstance, const XnChar* strGesture, XnBoundingBox3D* pArea);
/**
 * @brief Turn off gesture.  The generator will no longer look for this gesture
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	strGesture	[in]		The name of the gesture to not look for anymore
 */
XN_C_API XnStatus XN_C_DECL xnRemoveGesture(XnNodeHandle hInstance, const XnChar* strGesture);
/**
 * @brief Get the names of the gestures that are currently active
 * 
 * @param	hInstance		[in]		A handle to the instance
 * @param	pstrGestures	[out]		Preallocated memory, for the gesture names
 * @param	nGestures		[in,out]	The size of the preallocated memory. Changed to number of gestures
 */
XN_C_API XnStatus XN_C_DECL xnGetActiveGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt16* nGestures);
/**
 * @brief Get the names of the gestures that are currently active
 * 
 * @param	hInstance		[in]		A handle to the instance
 * @param	pstrGestures	[out]		Preallocated memory, for the gesture names
 * @param	nNameLength		[in]		Memory size for each gesture name
 * @param	nGestures		[in,out]	The size of the preallocated memory. Changed to number of gestures
 */
XN_C_API XnStatus XN_C_DECL xnGetAllActiveGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* nGestures);
/**
 * @brief Get the names of all gestures available
 * 
 * @param	hInstance		[in]		A handle to the instance
 * @param	pstrGestures	[out]		Preallocated memory, for the gesture names
 * @param	nGestures		[in,out]	The size of the preallocated memory. Changed to number of gestures
 */
XN_C_API XnStatus XN_C_DECL xnEnumerateGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt16* nGestures);
/**
 * @brief Get the names of all gestures available
 * 
 * @param	hInstance		[in]		A handle to the instance
 * @param	pstrGestures	[out]		Preallocated memory, for the gesture names
 * @param	nNameLength		[in]		Memory size for each gesture name
 * @param	nGestures		[in,out]	The size of the preallocated memory. Changed to number of gestures
 */
XN_C_API XnStatus XN_C_DECL xnEnumerateAllGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* nGestures);

/**
 * @brief Get the number of all gestures available
 * 
 * @param	hInstance		[in]		A handle to the instance
 * @return the number of gestures available
 */
XN_C_API XnUInt16 XN_C_DECL xnGetNumberOfAvailableGestures(XnNodeHandle hInstance);


/**
 * @brief Check if a specific gesture is available in this generator
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	strGesture	[in]		Name of the gesture to check
 */
XN_C_API XnBool XN_C_DECL xnIsGestureAvailable(XnNodeHandle hInstance, const XnChar* strGesture);
/**
 * @brief Check if the specific gesture supports 'in progress' callbacks
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	strGesture	[in]		Name of the gesture to check
 */
XN_C_API XnBool XN_C_DECL xnIsGestureProgressSupported(XnNodeHandle hInstance, const XnChar* strGesture);
/**
 * @brief Register to all gesture callbacks.
 * 
 * @param	hInstance		[in]	A handle to the instance
 * @param	RecognizedCB	[in]	A callback to be called when a gesture is recognized
 * @param	ProgressCB		[in]	A callback to be called when a gesture is on its way to be recognized
 * @param	pCookie			[in]	User's cookie, to be delivered to the callbacks
 * @param	phCallback		[out]	The handle to these callbacks, to allow unregistration
 */
XN_C_API XnStatus XN_C_DECL xnRegisterGestureCallbacks(XnNodeHandle hInstance, XnGestureRecognized RecognizedCB, XnGestureProgress ProgressCB, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from gesture callbacks
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void XN_C_DECL xnUnregisterGestureCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
 * @brief Register to when gestures are added or removed
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	handler		[in]		The callback to be called when gesture configuration changes.
 * @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
 * @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToGestureChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from when gestures are added or removed
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void XN_C_DECL xnUnregisterFromGestureChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Register to when a gesture is in progress
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	handler		[in]		The callback to be called when the gesture is in progress
 * @param	pCookie		[in]		User's cookie, to be delivered to the callback
 * @param	phCallback	[out]		The handle to the callback, to allow unregistration
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToGestureIntermediateStageCompleted(XnNodeHandle hInstance, XnGestureIntermediateStageCompleted handler, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from when a gesture is in progress
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void XN_C_DECL xnUnregisterFromGestureIntermediateStageCompleted(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
 * @brief Register to when a gesture is ready for its next stage (specific to the gesture)
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	handler		[in]		The callback to be called when the gesture is ready for its next stage
 * @param	pCookie		[in]		User's cookie, to be delivered to the callback
 * @param	phCallback	[out]		The handle to this callback, to allow unregistration
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToGestureReadyForNextIntermediateStage(XnNodeHandle hInstance, XnGestureReadyForNextIntermediateStage handler, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from when a gesture is ready for its next stage
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void XN_C_DECL xnUnregisterFromGestureReadyForNextIntermediateStage(XnNodeHandle hInstance, XnCallbackHandle hCallback);
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
XN_C_API XnStatus XN_C_DECL xnCreateSceneAnalyzer(
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
XN_C_API const XnLabel* XN_C_DECL xnGetLabelMap(XnNodeHandle hInstance);
/**
 * @brief Gets a description of the floor, if it was found.
 * 
 * @param	hInstance	[in]		A handle to the instance
 * @param	pPlane		[out]		A description of the floor
 */
XN_C_API XnStatus XN_C_DECL xnGetFloor(XnNodeHandle hInstance, XnPlane3D* pPlane);

/**
 * @brief Gets the current scene meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void XN_C_DECL xnGetSceneMetaData(XnNodeHandle hInstance, XnSceneMetaData* pMetaData);

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
XN_C_API XnStatus XN_C_DECL xnCreateUserGenerator(
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
XN_C_API XnUInt16 XN_C_DECL xnGetNumberOfUsers(XnNodeHandle hInstance);
/**
 * @brief Get the current users
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	pUsers		[out]		Preallocated memory for the users
 * @param	pnUsers		[in,out]	The size of the preallocated memory. Changes to the number of users
 */
XN_C_API XnStatus XN_C_DECL xnGetUsers(XnNodeHandle hInstance, XnUserID* pUsers, XnUInt16* pnUsers);
/**
 * @brief Get the center of mass of a user
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	user		[in]		The user for which to get the center of mass
 * @param	pCoM		[out]		The center of mass
 */
XN_C_API XnStatus XN_C_DECL xnGetUserCoM(XnNodeHandle hInstance, XnUserID user, XnPoint3D* pCoM);
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
XN_C_API XnStatus XN_C_DECL xnGetUserPixels(XnNodeHandle hInstance, XnUserID user, XnSceneMetaData* pScene);
/**
 * @brief Register to user callbacks
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	NewUserCB	[in]		Callback to be called when a new user is identified
 * @param	LostUserCB	[in]		Callback to be called when an existing user is no longer identified
 * @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
 * @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
 */
XN_C_API XnStatus XN_C_DECL xnRegisterUserCallbacks(XnNodeHandle hInstance, XnUserHandler NewUserCB, XnUserHandler LostUserCB, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from user callbacks
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void XN_C_DECL xnUnregisterUserCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Register to when the user exits the scene (but not lost yet)
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	handler		[in]		Callback to be called when a user leaves the scene
 * @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
 * @param	phCallback	[out]		The handle to this callback, to allow unregistration
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToUserExit(XnNodeHandle hInstance, XnUserHandler handler, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from when a user exits the scene
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void XN_C_DECL xnUnregisterFromUserExit(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
 * @brief Register to when a user re-enters the scene after exiting
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	handler		[in]		Callback to be called when a user leaves the scene
 * @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
 * @param	phCallback	[out]		The handle to this callback, to allow unregistration
 */
XN_C_API XnStatus XN_C_DECL xnRegisterToUserReEnter(XnNodeHandle hInstance, XnUserHandler handler, void* pCookie, XnCallbackHandle* phCallback);
/**
 * @brief Unregister from when a user re-enters the scene
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	hCallback	[in]		The handle received from registration
 */
XN_C_API void XN_C_DECL xnUnregisterFromUserReEnter(XnNodeHandle hInstance, XnCallbackHandle hCallback);
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
XN_C_API XnBool XN_C_DECL xnIsJointAvailable(XnNodeHandle hInstance, XnSkeletonJoint eJoint);
/**
* @brief Check if generator supports a specific profile.
*
* @param	hInstance	[in]		A handle to the instance
* @param	eProfile	[in]		The profile to check
*/
XN_C_API XnBool XN_C_DECL xnIsProfileAvailable(XnNodeHandle hInstance, XnSkeletonProfile eProfile);
/**
* @brief Set the profile. this will set some joints to be active, and others to be inactive.
*
* @param	hInstance	[in]		A handle to the instance
* @param	eProfile	[in]		The profile to set
*/
XN_C_API XnStatus XN_C_DECL xnSetSkeletonProfile(XnNodeHandle hInstance, XnSkeletonProfile eProfile);
/**
* @brief Change the state of a specific joint, to be active or inactive
*
* @param	hInstance	[in]		A handle to the instance
* @param	eJoint		[in]		The joint
* @param	bState		[in]		The new state
*/
XN_C_API XnStatus XN_C_DECL xnSetJointActive(XnNodeHandle hInstance, XnSkeletonJoint eJoint, XnBool bState);
/**
* @brief Check if joint is currently active
*
* @param	hInstance	[in]		A handle to the instance
* @param	eJoint		[in]		The joint to check
*/
XN_C_API XnBool XN_C_DECL xnIsJointActive(XnNodeHandle hInstance, XnSkeletonJoint eJoint);
/**
* @brief Register to joint configuration changes - when joints are activated and deactivated
*
* @param	hInstance	[in]		A handle to the instance
* @param	handler		[in]		The callback to be called when a joint is activated or deactivated
* @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus XN_C_DECL xnRegisterToJointConfigurationChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from joint configuration changes
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_C_DECL xnUnregisterFromJointConfigurationChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
* @brief Get all active joints
*
* @param	hInstance	[in]		A handle to the instance
* @param	pJoints		[out]		Preallocated memory for the joints
* @param	pnJoints	[in,out]	The size of the preallocated memory. Changed to the number of the active joints.
*/
XN_C_API XnStatus XN_C_DECL xnEnumerateActiveJoints(XnNodeHandle hInstance, XnSkeletonJoint* pJoints, XnUInt16* pnJoints);
/**
* @brief Get a specific joint's full information
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The ID of the user to which the skeleton belongs
* @param	eJoint		[in]		The interesting joint
* @param	pJoint		[out]		The joint's full information
*/
XN_C_API XnStatus XN_C_DECL xnGetSkeletonJoint(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation* pJoint);
/**
* @brief Get a specific joint's position
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The ID of the user to which the skeleton belongs
* @param	eJoint		[in]		The interesting joint
* @param	pJoint		[out]		The joint's current position
*/
XN_C_API XnStatus XN_C_DECL xnGetSkeletonJointPosition(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition* pJoint);
/**
* @brief Get a specific joint's orientation
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The ID of the user to which the skeleton belongs
* @param	eJoint		[in]		The interesting joint
* @param	pJoint		[out]		The joint's current orientation
*/
XN_C_API XnStatus XN_C_DECL xnGetSkeletonJointOrientation(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation* pJoint);
/**
 * @brief Check if skeleton is being tracked
 *
 * @param	hInstance	[in]		A handle to the instance
 * @param	user		[in]		Id of the user to check
 */
XN_C_API XnBool XN_C_DECL xnIsSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Check if skeleton is being calibrated
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnBool XN_C_DECL xnIsSkeletonCalibrated(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Check if skeleton is being calibrated
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnBool XN_C_DECL xnIsSkeletonCalibrating(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Request calibration when possible
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
* @param	bForce		[in]		Disregard previous calibration
*/
XN_C_API XnStatus XN_C_DECL xnRequestSkeletonCalibration(XnNodeHandle hInstance, XnUserID user, XnBool bForce);
/**
* @brief stop calibration
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus XN_C_DECL xnAbortSkeletonCalibration(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Save the calibration data to file
*
* @param	hInstance	[in]	A handle to the instance
* @param	user		[in]	the user for which the calibration is saved
* @param	strFileName	[in]	The file to which to save the calibration data
*/
XN_C_API XnStatus XN_C_DECL xnSaveSkeletonCalibrationDataToFile(XnNodeHandle hInstance, XnUserID user, const XnChar* strFileName);
/**
* @brief Load previously saved calibration data from file
*
* @param	hInstance	[in]	A handle to the instance
* @param	user		[in]	the user to be assigned the calibration data
* @param	strFileName	[in]	The file from which to load the calibration data.
*/
XN_C_API XnStatus XN_C_DECL xnLoadSkeletonCalibrationDataFromFile(XnNodeHandle hInstance, XnUserID user, const XnChar* strFileName);
/**
 * @brief Save the calibration data
 *
 * @param	hInstance	[in]	A handle to the instance
 * @param	user		[in]	the user for which the calibration is saved
 * @param	nSlot		[in]	The slot to use for saving the calibration data
 */
XN_C_API XnStatus XN_C_DECL xnSaveSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
/**
* @brief Load previously saved calibration data.
*
* @param	hInstance	[in]	A handle to the instance
* @param	user		[in]	the user to be assigned the calibration data
* @param	nSlot		[in]	The slot to use for saving the calibration data.
*/
XN_C_API XnStatus XN_C_DECL xnLoadSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
/**
* @brief Clear the requested slot from any saved calibration data.
*
* @param	hInstance	[in]	A handle to the instance
* @param	nSlot		[in]	The slot to clear
*/
XN_C_API XnStatus XN_C_DECL xnClearSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
/**
* @brief Check if a specific slot already holds calibration data
*
* @param	hInstance	[in]	A handle to the instance
* @param	nSlot		[in]	The slot to check
*/
XN_C_API XnBool XN_C_DECL xnIsSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
/**
* @brief Start tracking a skeleton
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus XN_C_DECL xnStartSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Stop tracking a skeleton
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus XN_C_DECL xnStopSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Reset the skeleton - discard calibration
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of the user to check
*/
XN_C_API XnStatus XN_C_DECL xnResetSkeleton(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Check if a specific pose is required for calibration
*
* @param	hInstance	[in]		A handle to the instance
*/
XN_C_API XnBool XN_C_DECL xnNeedPoseForSkeletonCalibration(XnNodeHandle hInstance);
/**
* @brief Get the pose that is required for calibration
*
* @param	hInstance	[in]		A handle to the instance
* @param	strPose		[in]		Name of the required pose
*/
XN_C_API XnStatus XN_C_DECL xnGetSkeletonCalibrationPose(XnNodeHandle hInstance, XnChar* strPose);
/**
* @brief Set the skeleton's smoothing factor
*
* @param	hInstance	[in]		A handle to the instance
* @param	fFactor		[in]		The smoothing factor
*/
XN_C_API XnStatus XN_C_DECL xnSetSkeletonSmoothing(XnNodeHandle hInstance, XnFloat fFactor);
/**
* @brief Register to calibration callbacks.
*
* @param	hInstance			[in]		A handle to the instance
* @param	CalibrationStartCB	[in]		Callback to be called when calibration is starting
* @param	CalibrationEndCB	[in]		Callback to be called when calibration is complete
* @param	pCookie				[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback			[out]		The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus XN_API_DEPRECATED("Please use RegisterToCalibrationStart/Complete") XN_C_DECL xnRegisterCalibrationCallbacks(XnNodeHandle hInstance, XnCalibrationStart CalibrationStartCB, XnCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from calibration callbacks
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_API_DEPRECATED("Please use UnregisterFromCalibrationStart/Complete") XN_C_DECL xnUnregisterCalibrationCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
* @brief Register to calibration start callback
*
* @param	hInstance	[in]	A handle to the instance
* @param	handler		[in]	Callback to be called when calibration starts
* @param	pCookie		[in]	User's cookie, to be delivered to the callback
* @param	phCallback	[out]	the handle to this callback, to allow unregistration
*/
XN_C_API XnStatus XN_C_DECL xnRegisterToCalibrationStart(XnNodeHandle hInstance, XnCalibrationStart handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from calibration start callback
*
* @param	[in]	hInstance	A handle to the instance
* @param	[in]	hCallback	The handle received from registration
*/
XN_C_API void XN_C_DECL xnUnregisterFromCalibrationStart(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
* @brief Register to calibration status while calibration is in progress
*
* @param	hInstance			[in]		A handle to the instance
* @param	handler				[in]		Callback to be called when calibration is in progress
* @param	pCookie				[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback			[out]		The handle to this callback, to allow unregistration
*/
XN_C_API XnStatus XN_C_DECL xnRegisterToCalibrationInProgress(XnNodeHandle hInstance, XnCalibrationInProgress handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from calibration status while in progress
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_C_DECL xnUnregisterFromCalibrationInProgress(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
* @brief Register to when calibration is complete, with status
*
* @param	hInstance			[in]		A handle to the instance
* @param	handler				[in]		Callback to be called when calibration is complete
* @param	pCookie				[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback			[out]		The handle to this callback, to allow unregistration
*/
XN_C_API XnStatus XN_C_DECL xnRegisterToCalibrationComplete(XnNodeHandle hInstance, XnCalibrationComplete handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from calibration complete with status
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_C_DECL xnUnregisterFromCalibrationComplete(XnNodeHandle hInstance, XnCallbackHandle hCallback);
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
XN_C_API XnUInt32 XN_C_DECL xnGetNumberOfPoses(XnNodeHandle hInstance);
/**
* @brief Get the names of the supported poses
*
* @param	hInstance	[in]		A handle to the instance
* @param	pstrPoses	[out]		Pre-allocated memory for the names of the supported poses
* @param	pnPoses		[in,out]	In input - size of the preallocated memory, in output - the number of pose names
*/
XN_C_API XnStatus XN_C_DECL xnGetAvailablePoses(XnNodeHandle hInstance, XnChar** pstrPoses, XnUInt32* pnPoses);
/**
* @brief Get the names of the supported poses
*
* @param	hInstance	[in]		A handle to the instance
* @param	pstrPoses	[out]		Pre-allocated memory for the names of the supported poses
* @param	nNameLength	[in]		Memory size for each pose name
* @param	pnPoses		[in,out]	In input - size of the preallocated memory, in output - the number of pose names
*/
XN_C_API XnStatus XN_C_DECL xnGetAllAvailablePoses(XnNodeHandle hInstance, XnChar** pstrPoses, XnUInt32 nNameLength, XnUInt32* pnPoses);

/**
* @brief Tests if a pose is supported
*
* @param	hInstance	[in]		A handle to the instance
* @param	strPose 	[in]		The pose string to test
* @return   True if the pose is supported and False otherwise.
*/
XN_C_API XnBool XN_C_DECL xnIsPoseSupported(XnNodeHandle hInstance, const XnChar* strPose);

/** 
* @brief Gets the current pose status
*
* @param	hInstance	[in]	A handle to the instance
* @param    userID      [in]    The user whose pose status we are interested in.
* @param    poseName    [in]    The pose we want to get a status on.
* @param    poseTime    [out]   The time stamp in which the user entered into the pose (0 if not in pose). 
* @param    eStatus     [out]   The status of the user's pose, i.e. the progress error for getting into 
*                               pose (XnPoseDetectionStatus, the same as received from the in progress
*                               callback. See @ref xnRegisterToPoseDetectionInProgress).
* @param    eState      [out]   The state of the user pose (i.e. in pose, out of pose).
* @return   The success status. The data is invalid if failed.
*/
XN_C_API XnStatus xnGetPoseStatus(XnNodeHandle hInstance, XnUserID userID, const XnChar* poseName, XnUInt64* poseTime, XnPoseDetectionStatus* eStatus, XnPoseDetectionState* eState);
/**
* @brief Start detection of a specific pose for a specific user
*
* @param	hInstance	[in]		A handle to the instance
* @param	strPose		[in]		Name of requested pose
* @param	user		[in]		Id of user
*/
XN_C_API XnStatus XN_C_DECL xnStartPoseDetection(XnNodeHandle hInstance,const  XnChar* strPose, XnUserID user);
/**
* @brief Stop detection of poses for a specific user
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of user
*/
XN_C_API XnStatus XN_C_DECL xnStopPoseDetection(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Stop detection of a specific pose for a specific user
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		Id of user
* @param	strPose		[in]		Name of the pose
*/
XN_C_API XnStatus XN_C_DECL xnStopSinglePoseDetection(XnNodeHandle hInstance, XnUserID user, const XnChar* strPose);
/**
* @brief Register to callbacks for pose events
*
* @param	hInstance	[in]		A handle to the instance
* @param	PoseDetectionStartCB	[in]	Function to be called when the pose is detected
* @param	PoseDetectionEndCB		[in]	Function to be called when the user is out of pose
* @param	pCookie					[in]	User's cookie, to be delivered to the callbacks
* @param	phCallback				[out]	The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus XN_API_DEPRECATED("Please use PoseDetected and/or OutOfPose callbacks") XN_C_DECL xnRegisterToPoseCallbacks(XnNodeHandle hInstance, XnPoseDetectionCallback PoseDetectionStartCB, XnPoseDetectionCallback PoseDetectionEndCB, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from pose events
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_API_DEPRECATED("Please use PoseDetected and/or OutOfPose callbacks") XN_C_DECL xnUnregisterFromPoseCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
* @brief Register to callback when a user is in pose
*
* @param	hInstance	[in]	A handle to the instance
* @param	handler		[in]	Callback to be called when the pose is detected
* @param	pCookie		[in]	User's cookie, to be delivered to the callback
* @param	phCallback	[out]	the handle to this callback, to allow unregistration
*/
XN_C_API XnStatus xnRegisterToPoseDetected(XnNodeHandle hInstance, XnPoseDetectionCallback handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from pose detected callback
*
* @param	[in]	hInstance	A handle to the instance
* @param	[in]	hCallback	The handle received from registration
*/
XN_C_API void xnUnregisterFromPoseDetected(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
* @brief Register to callback when a use is no longer in pose
*
* @param	hInstance	[in]	A handle to the instance
* @param	handler		[in]	Callback to be called when user isn't in pose anymore
* @param	pCookie		[in]	User's cookie, to be delivered to the callback
* @param	phCallback	[out]	the handle to this callback, to allow unregistration
*/
XN_C_API XnStatus xnRegisterToOutOfPose(XnNodeHandle hInstance, XnPoseDetectionCallback handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from out of pose callback
*
* @param	[in]	hInstance	A handle to the instance
* @param	[in]	hCallback	The handle received from registration
*/
XN_C_API void xnUnregisterFromOutOfPose(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
* @brief Register to callback for status when pose is detected
*
* @param	hInstance				[in]	A handle to the instance
* @param	handler					[in]	Function to be called when pose detection is in progress
* @param	pCookie					[in]	User's cookie, to be delivered to the callbacks
* @param	phCallback				[out]	The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus XN_C_DECL xnRegisterToPoseDetectionInProgress(XnNodeHandle hInstance, XnPoseDetectionInProgress handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from pose status callback
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_C_DECL xnUnregisterFromPoseDetectionInProgress(XnNodeHandle hInstance, XnCallbackHandle hCallback);

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
XN_C_API XnStatus XN_C_DECL xnCreateHandsGenerator(
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
XN_C_API XnStatus XN_C_DECL xnRegisterHandCallbacks(XnNodeHandle hInstance, XnHandCreate CreateCB, XnHandUpdate UpdateCB, XnHandDestroy DestroyCB, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from hands callbacks
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_C_DECL xnUnregisterHandCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
/**
* @brief Stop tracking a specific hand
*
* @param	hInstance	[in]		A handle to the instance
* @param	user		[in]		The hand to stop tracking
*/
XN_C_API XnStatus XN_C_DECL xnStopTracking(XnNodeHandle hInstance, XnUserID user);
/**
* @brief Stop tracking all hands
*
* @param	hInstance	[in]		A handle to the instance
*/
XN_C_API XnStatus XN_C_DECL xnStopTrackingAll(XnNodeHandle hInstance);
/**
* @brief Start tracking at a specific position
*
* @param	hInstance	[in]		A handle to the instance
* @param	pPosition	[in]		The position at which there is a new hand
*/
XN_C_API XnStatus XN_C_DECL xnStartTracking(XnNodeHandle hInstance, const XnPoint3D* pPosition);
/**
* @brief Change smoothing factor
*
* @param	hInstance	[in]		A handle to the instance
* @param	fFactor		[in]		Smoothing factor, in the range 0..1. 0 Means no smoothing, 1 means infinite smoothing. Inside the range is generator dependent.
*/
XN_C_API XnStatus XN_C_DECL xnSetTrackingSmoothing(XnNodeHandle hInstance, XnFloat fFactor);
/** @} */

//---------------------------------------------------------------------------
// Hand Touching FOV Edge Capability
//---------------------------------------------------------------------------

/** 
* @ingroup user
* @defgroup touchingEdge Hand Touching FOV Edge Capability
* The Touching FOV Edge capability (@ref XN_CAPABILITY_HAND_TOUCHING_FOV_EDGE) allows callbacks when a hand (from hands generator) is touching the edge of the FOV
* @{
*/

/**
* @brief Register to when a hand is approaching the edge of the FOV
*
* @param	hInstance	[in]		A handle to the instance
* @param	handler		[in]		Callback to be called when a hand is approaching the FOV
* @param	pCookie		[in]		User's cookie, to be delivered to the callbacks
* @param	phCallback	[out]		The handle to these callbacks, to allow unregistration
*/
XN_C_API XnStatus XN_C_DECL xnRegisterToHandTouchingFOVEdge(XnNodeHandle hInstance, XnHandTouchingFOVEdge handler, void* pCookie, XnCallbackHandle* phCallback);
/**
* @brief Unregister from hand approaching the edge of the FOV
*
* @param	hInstance	[in]		A handle to the instance
* @param	hCallback	[in]		The handle received from registration
*/
XN_C_API void XN_C_DECL xnUnregisterFromHandTouchingFOVEdge(XnNodeHandle hInstance, XnCallbackHandle hCallback);
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
XN_C_API XnStatus XN_C_DECL xnCreateAudioGenerator(
	XnContext* pContext,
	XnNodeHandle* phAudioGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	);

XN_C_API XnUChar* XN_C_DECL xnGetAudioBuffer(XnNodeHandle hInstance);
XN_C_API XnUInt32 XN_C_DECL xnGetSupportedWaveOutputModesCount(XnNodeHandle hInstance);
XN_C_API XnStatus XN_C_DECL xnGetSupportedWaveOutputModes(XnNodeHandle hInstance, XnWaveOutputMode* aSupportedModes, XnUInt32* pnCount);
XN_C_API XnStatus XN_C_DECL xnSetWaveOutputMode(XnNodeHandle hInstance, const XnWaveOutputMode* OutputMode);
XN_C_API XnStatus XN_C_DECL xnGetWaveOutputMode(XnNodeHandle hInstance, XnWaveOutputMode* OutputMode);
XN_C_API XnStatus XN_C_DECL xnRegisterToWaveOutputModeChanges(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
XN_C_API void XN_C_DECL xnUnregisterFromWaveOutputModeChanges(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Gets the current audio meta data.
 *
 * @param	hInstance	[in]		A handle to the instance.
 * @param	pMetaData	[in]		The struct to be filled.
 */
XN_C_API void XN_C_DECL xnGetAudioMetaData(XnNodeHandle hInstance, XnAudioMetaData* pMetaData);

/** @} */

//---------------------------------------------------------------------------
// Mocks
//---------------------------------------------------------------------------

/** 
 * @ingroup prd_node
 * @defgroup mocks Mock Nodes
 * @{
 */

XN_C_API XnStatus XN_C_DECL xnMockDepthSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnDepthPixel* pData);
XN_C_API XnStatus XN_C_DECL xnMockImageSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pData);
XN_C_API XnStatus XN_C_DECL xnMockIRSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnIRPixel* pData);
XN_C_API XnStatus XN_C_DECL xnMockAudioSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pData);
XN_C_API XnStatus XN_C_DECL xnMockRawSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const void* pData);

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

XN_C_API XnStatus XN_C_DECL xnCreateCodec(XnContext* pContext, XnCodecID codecID, XnNodeHandle hInitializerNode, XnNodeHandle* phCodec);
XN_C_API XnCodecID XN_C_DECL xnGetCodecID(XnNodeHandle hCodec);
XN_C_API XnStatus XN_C_DECL xnEncodeData(XnNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, 
							   void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten);
XN_C_API XnStatus XN_C_DECL xnDecodeData(XnNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, 
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
XN_C_API XnStatus XN_C_DECL xnCreateRecorder(XnContext* pContext, const XnChar* strFormatName, XnNodeHandle* phRecorder);

/**
 * @brief Tells the recorder where to record.
 *
 * @param	hRecorder	[in]	A handle to the recorder
 * @param	destType	[in]	The type of medium to record to. Currently only XN_RECORD_MEDIUM_FILE is supported.
 * @param	strDest		[in]	Recording destination. If destType is XN_RECORD_MEDIUM_FILE, this specifies a file name.
 */
XN_C_API XnStatus XN_C_DECL xnSetRecorderDestination(XnNodeHandle hRecorder, XnRecordMedium destType, const XnChar* strDest);

/**
 * @brief Returns the recoder's destination
 *
 * @param	hRecorder	[in]	A handle to the recorder
 * @param	pDestType	[out]	The type of medium the recorder is set to record.
 * @param	strDest		[out]	Recording destination. If destType is XN_RECORD_MEDIUM_FILE, this specifies a file name.
 * @param	nBufSize	[in]	The size of the strDest buffer.
 */
XN_C_API XnStatus XN_C_DECL xnGetRecorderDestination(XnNodeHandle hRecorder, XnRecordMedium* pDestType, XnChar* strDest, XnUInt32 nBufSize);

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
XN_C_API XnStatus XN_C_DECL xnAddNodeToRecording(XnNodeHandle hRecorder, XnNodeHandle hNode, XnCodecID compression);

/**
 * @brief Removes node from recording and stop recording it. This function can be called on a node that was added to 
 * recording with @ref xnAddNodeToRecording.
 *
 * @param	hRecorder	[in]	A handle to the recorder.
 * @param	hNode		[in]	The node to remove from recording.
 */
XN_C_API XnStatus XN_C_DECL xnRemoveNodeFromRecording(XnNodeHandle hRecorder, XnNodeHandle hNode);

/**
 * @brief Records one frame of data from each node that was added to the recorder with @ref xnAddNodeToRecording.
 *
 * @param	hRecorder	[in]	A handle to the recorder.
 */
XN_C_API XnStatus XN_C_DECL xnRecord(XnNodeHandle hRecorder);

/**
 * @brief Gets a string representation of the recording format.
 *
 * @param	hRecorder	[in]	A handle to the recorder.
 */
XN_C_API const XnChar* XN_C_DECL xnGetRecorderFormat(XnNodeHandle hRecorder);

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
XN_C_API XnStatus XN_C_DECL xnCreatePlayer(XnContext* pContext, const XnChar* strFormatName, XnNodeHandle* phPlayer);

/**
 * @brief Determines whether the player will automatically rewind to the beginning of the recording when reaching the end.
 *
 * @param	hPlayer	[in]	A handle to the player.
 * @param	bRepeat	[in]	Determines whether the player will repeat or not.
 * 
 * @remark If bRepeat is set to TRUE, the player will automatically rewind when reaching the end.
 * @remark If bRepeat is set to FALSE, the player will stop playing when reaching the end, and will raise the event that was registered with @ref xnRegisterToEndOfFileReached(). In this state, @ref xnIsPlayerAtEOF() returns TRUE.
 */
XN_C_API XnStatus XN_C_DECL xnSetPlayerRepeat(XnNodeHandle hPlayer, XnBool bRepeat);

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
XN_C_API XnStatus XN_C_DECL xnSetPlayerSource(XnNodeHandle hPlayer, XnRecordMedium sourceType, const XnChar* strSource);

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
XN_C_API XnStatus XN_C_DECL xnGetPlayerSource(XnNodeHandle hPlayer, XnRecordMedium* pSourceType, XnChar* strSource, XnUInt32 nBufSize);

/**
 * @brief Reads the next data element from the player. 
 *
 * Can cause data updates and/or property updates of the played nodes.
 *
 * @param	hPlayer	[in]	A handle to the player.
 */
XN_C_API XnStatus XN_C_DECL xnPlayerReadNext(XnNodeHandle hPlayer);

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
XN_C_API XnStatus XN_C_DECL xnSeekPlayerToTimeStamp(XnNodeHandle hPlayer, XnInt64 nTimeOffset, XnPlayerSeekOrigin origin);

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
XN_C_API XnStatus XN_C_DECL xnSeekPlayerToFrame(XnNodeHandle hPlayer, const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin);

/**
 * @brief Reports the current timestamp of a player, i.e. the amount of time passed since the beginning of the recording.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	pnTimestamp	[out]	A pointer to the retrieved timestamp in microseconds.
 */
XN_C_API XnStatus XN_C_DECL xnTellPlayerTimestamp(XnNodeHandle hPlayer, XnUInt64* pnTimestamp);

/**
 * @brief Reports the current frame number of a specific node played by a player.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	strNodeName	[in]	The name of the node for which to report the frame number.
 * @param	pnFrame		[out]	A pointer to the reported frame number.
 *
 * @remark Different nodes that belong to the player can have different frame numbers, because the nodes are not necessarily in sync.
 */
XN_C_API XnStatus XN_C_DECL xnTellPlayerFrame(XnNodeHandle hPlayer, const XnChar* strNodeName, XnUInt32* pnFrame);

/**
 * @brief Retrieves the number of frames of a specific node played by a player.
 *
 * @param	hPlayer		[in]	A handle to the player.
 * @param	strNodeName	[in]	The name of the node for which to retrieve the number of frames.
 * @param	pnFrames	[out]	A pointer to the retrieved number of frames.
 */
XN_C_API XnStatus XN_C_DECL xnGetPlayerNumFrames(XnNodeHandle hPlayer, const XnChar* strNodeName, XnUInt32* pnFrames);

/**
 * @brief Gets the name of the format supported by a player.
 *
 * @param	hPlayer	[in]	A handle to the player.
 * @returns	The name of the format supported by the player, or NULL if hPlayer is NULL.
 */
XN_C_API const XnChar* XN_C_DECL xnGetPlayerSupportedFormat(XnNodeHandle hPlayer);

/**
 * @brief Retrieves a list of the nodes played by a player.
 *
 * @param	hPlayer	[in]	A handle to the player.
 * @param	ppList	[out]	A pointer to the retrieved list of nodes.
 *
 * @remark This function fills the pointer that is pointed to by ppList with a newly allocated node info list
 * of the played nodes. This list must later be freed by the caller with @ref xnNodeInfoListFree().
 */
XN_C_API XnStatus XN_C_DECL xnEnumeratePlayerNodes(XnNodeHandle hPlayer, XnNodeInfoList** ppList);

/**
 * @brief Checks whether the player is at the end-of-file marker.
 * @note In the built-in ONI player, this function will never return TRUE for a player that is in repeat mode, although the event registered with @ref xnRegisterToEndOfFileReached() will be fired.
 *
 * @param	hPlayer	[in]	A handle to the player.
 *
 * @sa xnSetPlayerRepeat()
 */
XN_C_API XnBool XN_C_DECL xnIsPlayerAtEOF(XnNodeHandle hPlayer);

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
XN_C_API XnStatus XN_C_DECL xnRegisterToEndOfFileReached
	(XnNodeHandle hPlayer, XnStateChangedHandler handler,
	void* pCookie, XnCallbackHandle* phCallback);

/**
 * @brief Unregisters a callback function which was registered using @ref xnRegisterToEndOfFileReached().
 *
 * @param	hInstance	[in]	A handle to the player.
 * @param	hCallback	[in]	The handle to the callback returned from @ref xnRegisterToEndOfFileReached().
 */
XN_C_API void XN_C_DECL xnUnregisterFromEndOfFileReached
	(XnNodeHandle hInstance, XnCallbackHandle hCallback);

/**
 * @brief Sets the playback speed, as a ratio of the time passed in the recording.
 * A value of 1.0 means the player will try to output frames in the rate they were recorded (according to
 * their timestamps).
 * A value bigger than 1.0 means fast-forward, and a value between 0.0 and 1.0 means slow-motion.
 * The special value of XN_PLAYBACK_SPEED_FASTEST (0.0) means there will be no delay, and that frames will
 * be returned as soon as asked for.
 *
 * @param	hInstance	[in]	A handle to the player.
 * @param	dSpeed		[in]	The speed ratio (see function description).
 */
XN_C_API XnStatus XN_C_DECL xnSetPlaybackSpeed(XnNodeHandle hInstance, XnDouble dSpeed);

/**
 * @brief Gets the playback speed. see @ref xnSetPlaybackSpeed() for more details.
 *
 * @param	hInstance	[in]	A handle to the player.
 * @returns speed ratio, or -1.0 if this node is not a player.
 */
XN_C_API XnDouble XN_C_DECL xnGetPlaybackSpeed(XnNodeHandle hInstance);

/** @} */

//---------------------------------------------------------------------------
// ScriptNode
//---------------------------------------------------------------------------

/**
 * @ingroup prd_node
 * @defgroup script_node Script Node
 * A Script node is a @ref node that allows running scripts, and acts as a lifetime manager for nodes created
 * by that script.
 * @{
 */

XN_C_API XnStatus XN_C_DECL xnCreateScriptNode(XnContext* pContext, const XnChar* strFormat, XnNodeHandle* phScript);
XN_C_API const XnChar* XN_C_DECL xnScriptNodeGetSupportedFormat(XnNodeHandle hScript);
XN_C_API XnStatus XN_C_DECL xnLoadScriptFromFile(XnNodeHandle hScript, const XnChar* strFileName);
XN_C_API XnStatus XN_C_DECL xnLoadScriptFromString(XnNodeHandle hScript, const XnChar* strScript);
XN_C_API XnStatus XN_C_DECL xnScriptNodeRun(XnNodeHandle hScript, XnEnumerationErrors* pErrors);

/** @} */

#endif // __XN_PRD_NODE_H__
