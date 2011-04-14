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
 * Sets the instance name of the node info object. The name can only be set BEFORE the instance is
 * created.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 * @param	strInstanceName	[in]	The name to give to this instance.
 */
XN_C_API XnStatus xnNodeInfoSetInstanceName(XnNodeInfo* pNodeInfo, const XnChar* strInstanceName);

/**
 * Gets the description of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const XnProductionNodeDescription* xnNodeInfoGetDescription(XnNodeInfo* pNodeInfo);

/**
 * Creates a printable representation of the production tree.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 * @param	csResult		[in]	A buffer to be filled with the printable representation.
 * @param	nSize			[in]	Size of the buffer.
 */
XN_C_API XnStatus xnNodeInfoGetTreeStringRepresentation(XnNodeInfo* pNodeInfo, XnChar* csResult, XnUInt32 nSize);

/**
 * Gets the instance name of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const XnChar* xnNodeInfoGetInstanceName(XnNodeInfo* pNodeInfo);

/**
 * Gets the creation info of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const XnChar* xnNodeInfoGetCreationInfo(XnNodeInfo* pNodeInfo);

/**
 * Gets the list of needed nodes of the production node.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API XnNodeInfoList* xnNodeInfoGetNeededNodes(XnNodeInfo* pNodeInfo);

/**
 * Gets the handle of the production node. For nodes that weren't created yet, NULL is returned.
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API XnNodeHandle xnNodeInfoGetHandle(XnNodeInfo* pNodeInfo);

/**
 * Gets the additional data object attached to the node info object. 
 *
 * @param	pNodeInfo		[in]	A production node info object.
 */
XN_C_API const void* xnNodeInfoGetAdditionalData(XnNodeInfo* pNodeInfo);

/** @} */

#endif // __XN_PRD_NODE_INFO_H__
