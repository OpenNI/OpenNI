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
#ifndef __XN_UTILS_H__
#define __XN_UTILS_H__

/**
 * @ingroup cref
 * @defgroup utils Utilities
 * This page details some utility functions.
 * @{
 */

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnTypes.h>
#include <XnModuleInterface.h>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/** 
 * @name General
 * @{
 */

/**
 * Gets current OpenNI version
 *
 * @param	pVersion	[out]	Current DLL version
 */
XN_C_API XnStatus XN_C_DECL xnGetVersion(XnVersion* pVersion);

/// @}

/**
 * @name ProductionNodeType Utils
 * @{
 */

/**
 * Gets a string representation of the production node type.
 *
 * @param	Type	[in]	The type of the production node.
 */
XN_C_API const XnChar* XN_C_DECL xnProductionNodeTypeToString(XnProductionNodeType Type);

/**
 * Gets a type from its string representation.
 *
 * @param	strType	[in]	The string representation of the type.
 * @param	pType	[out]	The type of the production node.
 */
XN_C_API XnStatus XN_C_DECL xnProductionNodeTypeFromString(const XnChar* strType, XnProductionNodeType* pType);

/**
 * @brief Checks if a certain production node type is a generator or not
 *
 * @param	type	[in]	The type to check
 */
XN_C_API XnBool XN_C_DECL xnIsTypeGenerator(XnProductionNodeType type);

/**
 * @brief Checks if a certain production node type is a derived of another type
 *
 * @param	type	[in]	The type to check
 * @param	base	[in]	The base type to check against
 */
XN_C_API XnBool XN_C_DECL xnIsTypeDerivedFrom(XnProductionNodeType type, XnProductionNodeType base);

/// @}

/**
 * @name PixelFormat Utils
 * @{
 */

/**
 * Get the name of the pixel format.
 *
 * @param	format [in]	Pixel Format
 */
XN_C_API const XnChar* XN_C_DECL xnPixelFormatToString(XnPixelFormat format);

/**
 * Gets pixel format by its name.
 *
 * @param	strName	[in]	Name of the pixel format.
 * @param	pFormat	[out]	Pixel format.
 */
XN_C_API XnStatus XN_C_DECL xnPixelFormatFromString(const XnChar* strName, XnPixelFormat* pFormat);

/**
 * Gets the bytes per pixel of a pixel format.
 *
 * @param	format	[in]	The pixel format.
 */
XN_C_API XnUInt32 XN_C_DECL xnGetBytesPerPixelForPixelFormat(XnPixelFormat format);

/// @}

/**
 * @name Resolution Utilities
 * @{
 */

/**
 * Gets the number of pixels in a row for given resolution.
 *
 * @param	resolution	[in]	The resolution to translate.
 */
XN_C_API XnUInt32 XN_C_DECL xnResolutionGetXRes(XnResolution resolution);

/**
 * Gets the number of pixels in a column for given resolution.
 *
 * @param	resolution	[in]	The resolution to translate.
 */
XN_C_API XnUInt32 XN_C_DECL xnResolutionGetYRes(XnResolution resolution);

/**
 * Gets the resolution from the number of pixels in every row and column.
 *
 * @param	xRes	[in]	Number of pixels in a row.
 * @param	yRes	[in]	Number of pixels in a column.
 */
XN_C_API XnResolution XN_C_DECL xnResolutionGetFromXYRes(XnUInt32 xRes, XnUInt32 yRes);

/**
 * Gets resolution by its name. If resolution is unknown, XN_RESOLUTION_CUSTOM is returned.
 *
 * @param	strName	[in]	Name of the resolution.
 */
XN_C_API XnResolution XN_C_DECL xnResolutionGetFromName(const XnChar* strName);

/**
 * Get the name of the resolution.
 *
 * @param	resolution [in]	Resolution.
 */
XN_C_API const XnChar* XN_C_DECL xnResolutionGetName(XnResolution resolution);

/// @}

/**
 * @name XnVersion Utilities
 * @{
 */

/**
 * Compares two versions.
 *
 * @param	pVersion1	[in]	First version.
 * @param	pVersion2	[in]	Second version.
 *
 * @returns <0 if first is smaller than second, 0 if same, >0 if first is greater than second.
 */
XN_C_API XnInt32 XN_C_DECL xnVersionCompare(const XnVersion* pVersion1, const XnVersion* pVersion2);

XN_C_API XnStatus XN_C_DECL xnVersionToString(const XnVersion* pVersion, XnChar* csResult, XnUInt32 nSize);

/// @}

/**
 * @name XnProductionNodeDescription Utilities
 * @{
 */

XN_C_API XnStatus XN_C_DECL xnProductionNodeDescriptionToString(const XnProductionNodeDescription* pDescription, XnChar* csResult, XnUInt32 nSize);

/// @}

/**
 * @name C Style Constructors
 * @{
 */

XN_C_API XnPoint3D XN_C_DECL xnCreatePoint3D(XnFloat x, XnFloat y, XnFloat z);
XN_C_API XnPlane3D XN_C_DECL xnCreatePlane3D(XnPoint3D ptPoint, XnVector3D vNormal);

/// @}

/**
 * @name Modules Registry Management
 * @{
 */

XN_C_API XnStatus XN_C_DECL xnRegisterModule(const XnChar* strModule, const XnChar* strConfigDir);
XN_C_API XnStatus XN_C_DECL xnUnregisterModule(const XnChar* strModule);
XN_C_API XnStatus XN_C_DECL xnPrintRegisteredModules();

/// @}

/**
 * @name Global Licenses Registry Management
 * @{
 */

XN_C_API XnStatus XN_C_DECL xnRegisterGlobalLicense(XnLicense* pLicense);
XN_C_API XnStatus XN_C_DECL xnUnregisterGlobalLicense(XnLicense* pLicense);
XN_C_API XnStatus XN_C_DECL xnPrintRegisteredLicenses();

/// @}

/**
 * @name MetaData Objects Management
 * @{
 */

/**
 * Allocates and returns an @ref XnOutputMetaData object. When no longer needed, this object should be freed
 * by calling @ref xnFreeOutputMetaData().
 * @returns NULL if allocation failed.
 */
XN_C_API XnOutputMetaData* XN_C_DECL xnAllocateOutputMetaData();
/**
 * Frees a previously allocated @ref XnOutputMetaData object.
 *
 * @param	pMetaData		[in]	object to be freed.
 */
XN_C_API void XN_C_DECL xnFreeOutputMetaData(const XnOutputMetaData* pMetaData);
/**
 * @brief Shallow-Copies an Output Meta Data object.
 *
 * @param	pDestination	[in]	Destination object.
 * @param	pSource			[in]	Source object.
 */
XN_C_API XnStatus XN_C_DECL xnCopyOutputMetaData(XnOutputMetaData* pDestination, const XnOutputMetaData* pSource);

/**
 * Allocates and returns an @ref XnMapMetaData object. When no longer needed, this object should be freed
 * by calling @ref xnFreeMapMetaData().
 * @returns NULL if allocation failed.
 */
XN_C_API XnMapMetaData* XN_C_DECL xnAllocateMapMetaData();
/**
 * Frees a previously allocated @ref XnMapMetaData object.
 *
 * @param	pMetaData		[in]	object to be freed.
 */
XN_C_API void XN_C_DECL xnFreeMapMetaData(const XnMapMetaData* pMetaData);
/**
 * @brief Shallow-Copies a Map Meta Data object.
 *
 * @param	pDestination	[in]	Destination object.
 * @param	pSource			[in]	Source object.
 */
XN_C_API XnStatus XN_C_DECL xnCopyMapMetaData(XnMapMetaData* pDestination, const XnMapMetaData* pSource);

/**
 * Allocates and returns an @ref XnDepthMetaData object. When no longer needed, this object should be freed
 * by calling @ref xnFreeDepthMetaData().
 * @returns NULL if allocation failed.
 */
XN_C_API XnDepthMetaData* XN_C_DECL xnAllocateDepthMetaData();
/**
 * Frees a previously allocated @ref XnDepthMetaData object.
 *
 * @param	pMetaData		[in]	object to be freed.
 */
XN_C_API void XN_C_DECL xnFreeDepthMetaData(const XnDepthMetaData* pMetaData);
/**
 * @brief Shallow-Copies a Depth Meta Data object. Note that the data buffer is not copied, and that both
 * object will point to the same buffer.
 *
 * @param	pDestination	[in]	Destination object.
 * @param	pSource			[in]	Source object.
 */
XN_C_API XnStatus XN_C_DECL xnCopyDepthMetaData(XnDepthMetaData* pDestination, const XnDepthMetaData* pSource);

/**
 * Allocates and returns an @ref XnImageMetaData object. When no longer needed, this object should be freed
 * by calling @ref xnFreeImageMetaData().
 * @returns NULL if allocation failed.
 */
XN_C_API XnImageMetaData* XN_C_DECL xnAllocateImageMetaData();
/**
 * Frees a previously allocated @ref XnImageMetaData object.
 *
 * @param	pMetaData		[in]	object to be freed.
 */
XN_C_API void XN_C_DECL xnFreeImageMetaData(const XnImageMetaData* pMetaData);
/**
 * @brief Shallow-Copies an Image Meta Data object. Note that the data buffer is not copied, and that both
 * object will point to the same buffer.
 *
 * @param	pDestination	[in]	Destination object.
 * @param	pSource			[in]	Source object.
 */
XN_C_API XnStatus XN_C_DECL xnCopyImageMetaData(XnImageMetaData* pDestination, const XnImageMetaData* pSource);

/**
 * Allocates and returns an @ref XnIRMetaData object. When no longer needed, this object should be freed
 * by calling @ref xnFreeIRMetaData().
 * @returns NULL if allocation failed.
 */
XN_C_API XnIRMetaData* XN_C_DECL xnAllocateIRMetaData();
/**
 * Frees a previously allocated @ref XnIRMetaData object.
 *
 * @param	pMetaData		[in]	object to be freed.
 */
XN_C_API void XN_C_DECL xnFreeIRMetaData(const XnIRMetaData* pMetaData);
/**
 * @brief Shallow-Copies an IR Meta Data object. Note that the data buffer is not copied, and that both
 * object will point to the same buffer.
 *
 * @param	pDestination	[in]	Destination object.
 * @param	pSource			[in]	Source object.
 */
XN_C_API XnStatus XN_C_DECL xnCopyIRMetaData(XnIRMetaData* pDestination, const XnIRMetaData* pSource);

/**
 * Allocates and returns an @ref XnAudioMetaData object. When no longer needed, this object should be freed
 * by calling @ref xnFreeAudioMetaData().
 * @returns NULL if allocation failed.
 */
XN_C_API XnAudioMetaData* XN_C_DECL xnAllocateAudioMetaData();
/**
 * Frees a previously allocated @ref XnAudioMetaData object.
 *
 * @param	pMetaData		[in]	object to be freed.
 */
XN_C_API void XN_C_DECL xnFreeAudioMetaData(const XnAudioMetaData* pMetaData);
/**
 * @brief Shallow-Copies an Audio Meta Data object. Note that the data buffer is not copied, and that both
 * object will point to the same buffer.
 *
 * @param	pDestination	[in]	Destination object.
 * @param	pSource			[in]	Source object.
 */
XN_C_API XnStatus XN_C_DECL xnCopyAudioMetaData(XnAudioMetaData* pDestination, const XnAudioMetaData* pSource);

/**
 * Allocates and returns an @ref XnSceneMetaData object. When no longer needed, this object should be freed
 * by calling @ref xnFreeSceneMetaData().
 * @returns NULL if allocation failed.
 */
XN_C_API XnSceneMetaData* XN_C_DECL xnAllocateSceneMetaData();
/**
 * Frees a previously allocated @ref XnSceneMetaData object.
 *
 * @param	pMetaData		[in]	object to be freed.
 */
XN_C_API void XN_C_DECL xnFreeSceneMetaData(const XnSceneMetaData* pMetaData);
/**
 * @brief Shallow-Copies a Scene Meta Data object. Note that the data buffer is not copied, and that both
 * object will point to the same buffer.
 *
 * @param	pDestination	[in]	Destination object.
 * @param	pSource			[in]	Source object.
 */
XN_C_API XnStatus XN_C_DECL xnCopySceneMetaData(XnSceneMetaData* pDestination, const XnSceneMetaData* pSource);

/// @}

/**
 * @name Functions for Modules Implementors
 * @{
 */

/**
 * A helper function for production nodes implementers. Enumerates needed child nodes (assuming there
 * is only one needed input), and populates a list with production trees over that input.
 *
 * @param	pContext		[in]	OpenNI Context.
 * @param	pList			[in]	The list to be populated.
 * @param	pDescription	[in]	The description of the enumerated node.
 * @param	strCreationInfo	[in]	[Optional] Additional creation info.
 * @param	InputType		[in]	The type of the input required by this production node.
 * @param	pErrors			[in]	The errors object passed to Enumerate method.
 * @param	pQuery			[in]	[Optional] A filter to be used for finding the appropriate input.
 */
XN_C_API XnStatus XN_C_DECL xnAutoEnumerateOverSingleInput(XnContext* pContext, XnNodeInfoList* pList, XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnProductionNodeType InputType, XnEnumerationErrors* pErrors, XnNodeQuery* pQuery);

/**
 * Registers an extension node with OpenNI. An extension node is a node of type not defined by OpenNI.
 *
 * @param	strTypeName		[in]	Name of this new node type.
 * @param	baseType		[in]	Type of the direct base for this extension.
 * @param	pTypeID			[out]	The new ID registered with OpenNI.
 */
XN_C_API XnStatus XN_C_DECL xnRegisterExtensionNode(const XnChar* strTypeName, XnProductionNodeType baseType, XnProductionNodeType* pTypeID);

/**
 * Gets the module node handle of a node. This is useful for extension implementors.
 *
 * @param	hNode			[in]	The node to get its module node handle.
 */
XN_C_API XnModuleNodeHandle XN_C_DECL xnGetModuleNodeHandle(XnNodeHandle hNode);

#if !XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
/**
 * Registers a module to OpenNI in environments where dynamic library loading is not supported.
 *
 * @param	pInterface		[in]	The module interface.
 * @param	strConfigDir	[in]	The module configuration directory.
 * @param	strName			[in]	Name of this module.
 */
XN_C_API XnStatus XN_C_DECL xnRegisterModuleWithOpenNI(XnOpenNIModuleInterface* pInterface, const XnChar* strConfigDir, const XnChar* strName);
#endif

/// @}

/** @} */

#endif // __XN_UTILS_H__
