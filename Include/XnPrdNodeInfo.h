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
#ifndef __XN_PRD_NODE_INFO_H__
#define __XN_PRD_NODE_INFO_H__

/**
 * @ingroup cref
 * @defgroup prdnodeinfo Production Node Info
 * This page describes the functionality of a production node info object. Each info object
 * can represent a possible production node, or an existing one.
 * @{
 */

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnTypes.h>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------



/**
 * Allocates a node info object, and sets its details.
 *
 * @param	pDescription	[in]	The description for the new node info object.
 * @param	strCreationInfo	[in]	The creation info for the new node info object.
 * @param	pNeededNodes	[in]	A list of node info's that are needed by the new node info.
 * @param	ppNodeInfo		[out]	A pointer to pointer to the new node info object.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoAllocate(const XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnNodeInfoList* pNeededNodes, XnNodeInfo** ppNodeInfo);

/**
 * Frees a node info object
 *
 * @param	pNodeInfo	[in]	The node info object to free.
 */
XN_C_API void XN_C_DECL xnNodeInfoFree(XnNodeInfo* pNodeInfo);

/**
 * Sets the instance name of the node info object. The name can only be set BEFORE the instance is
 * created.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 * @param	strInstanceName	[in]	The name to give to this instance.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoSetInstanceName(XnNodeInfo* pNodeInfo, const XnChar* strInstanceName);

/**
 * Gets the description of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const XnProductionNodeDescription* XN_C_DECL xnNodeInfoGetDescription(XnNodeInfo* pNodeInfo);

/**
 * Creates a printable representation of the production tree.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 * @param	csResult		[in]	A buffer to be filled with the printable representation.
 * @param	nSize			[in]	Size of the buffer.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoGetTreeStringRepresentation(XnNodeInfo* pNodeInfo, XnChar* csResult, XnUInt32 nSize);

/**
 * Gets the instance name of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const XnChar* XN_C_DECL xnNodeInfoGetInstanceName(XnNodeInfo* pNodeInfo);

/**
 * Gets the creation info of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const XnChar* XN_C_DECL xnNodeInfoGetCreationInfo(XnNodeInfo* pNodeInfo);

/**
 * Gets the list of needed nodes of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API XnNodeInfoList* XN_C_DECL xnNodeInfoGetNeededNodes(XnNodeInfo* pNodeInfo);

/**
 * Gets the handle of the production node. For nodes that weren't created yet, NULL is returned.
 * If a handle is returned, it should be released when no longer used by calling @ref xnProductionNodeRelease().
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API XnNodeHandle XN_C_DECL xnNodeInfoGetRefHandle(XnNodeInfo* pNodeInfo);

XN_C_API XnNodeHandle XN_API_DEPRECATED("Please use xnNodeInfoGetRefHandle() instead") XN_C_DECL xnNodeInfoGetHandle(XnNodeInfo* pNodeInfo);

/**
 * Gets the additional data object attached to the node info object. 
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const void* XN_C_DECL xnNodeInfoGetAdditionalData(XnNodeInfo* pNodeInfo);

/** @} */

#endif // __XN_PRD_NODE_INFO_H__
