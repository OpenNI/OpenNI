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
#ifndef __XN_QUERIES_H__
#define __XN_QUERIES_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnTypes.h"

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------
/**
 * @ingroup cref
 * @defgroup queries Queries
 * This page details functions for building queries. Queries can be used during enumeration, and provide
 * an easy way for filtering results before they return to user.
 * @{
 */

/**
 * Allocates a new query object. This object must be freed using @ref xnNodeQueryFree().
 *
 * @param	ppQuery		[out]	Created query object.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQueryAllocate(XnNodeQuery** ppQuery);

/**
 * Frees a query object previously created using @ref xnNodeQueryAllocate().
 *
 * @param	pQuery		[in]	Query object.
 */
XN_C_API void XN_C_DECL xnNodeQueryFree(XnNodeQuery* pQuery);

/**
 * Sets the requested vendor.
 *
 * @param	pQuery		[in]	Query object.
 * @param	strVendor	[in]	Vendor Name.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetVendor(XnNodeQuery* pQuery, const XnChar* strVendor);

/**
 * Sets the requested name.
 *
 * @param	pQuery		[in]	Query object.
 * @param	strName		[in]	Product Name.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetName(XnNodeQuery* pQuery, const XnChar* strName);

/**
 * Sets the minimum required version.
 *
 * @param	pQuery		[in]	Query object.
 * @param	pMinVersion	[in]	Min Version. 
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetMinVersion(XnNodeQuery* pQuery, const XnVersion* pMinVersion);

/**
 * Sets the maximum required version. 
 *
 * @param	pQuery		[in]	Query object.
 * @param	pMaxVersion	[in]	Max Version. 
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetMaxVersion(XnNodeQuery* pQuery, const XnVersion* pMaxVersion);

/**
 * Adds a capability that the node must support. Note that for checking if a capability is supported, 
 * the node must be created (if it's not an existing one). This operation might take a bit longer...
 *
 * @param	pQuery				[in]	Query object.
 * @param	strNeededCapability	[in]	A needed capability name.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQueryAddSupportedCapability(XnNodeQuery* pQuery, const XnChar* strNeededCapability);

/**
 * Adds a MapOutputMode that the node must support. Note that for checking if a capability is supported, 
 * the node must be created (if it's not an existing one). This operation might take a bit longer...
 *
 * @param	pQuery			[in]	Query object.
 * @param	pMapOutputMode	[in]	A map output mode. Each field of the mode can be set to (-1), meaning 
 * results will not be filtered by this field. 
 */
XN_C_API XnStatus XN_C_DECL xnNodeQueryAddSupportedMapOutputMode(XnNodeQuery* pQuery, const XnMapOutputMode* pMapOutputMode);

/**
 * Sets the minimum required user positions this node supports. Note that for checking if a capability is supported, 
 * the node must be created (if it's not an existing one). This operation might take a bit longer...
 *
 * @param	pQuery		[in]	Query object.
 * @param	nCount		[in]	The number of required positions.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetSupportedMinUserPositions(XnNodeQuery* pQuery, const XnUInt32 nCount);

/**
 * Filter results so that only existing nodes will be returned.
 *
 * @param	pQuery			[in]	Query object.
 * @param	bExistingNode	[in]	TRUE to return only existing ones, FALSE otherwise.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetExistingNodeOnly(XnNodeQuery* pQuery, XnBool bExistingNode);

/**
 * Filter results so that only non-existing nodes will be returned.
 *
 * @param	pQuery			 [in]	Query object.
 * @param	bNonExistingNode [in]	TRUE to return only non-existing ones, FALSE otherwise.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetNonExistingNodeOnly(XnNodeQuery* pQuery, XnBool bNonExistingNode);

/**
 * Adds a needed node.
 *
 * @param	pQuery			[in]	Query object.
 * @param	strInstanceName	[in]	The name of the needed instance.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQueryAddNeededNode(XnNodeQuery* pQuery, const XnChar* strInstanceName);

/**
 * Sets the creation info field of a query object.
 *
 * @param	pQuery			[in]	Query object.
 * @param	strCreationInfo	[in]	The creation info to set in the query.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQuerySetCreationInfo(XnNodeQuery* pQuery, const XnChar* strCreationInfo);

/**
 * Filters a list of nodes according to query. The list will contain only nodes that match all query terms.
 * Note that some query terms might require creation of a node, and thus may take longer.
 *
 * @param	pContext	[in]		OpenNI Context object.
 * @param	pQuery		[in]		Query object.
 * @param	pList		[in]		The list to be filtered.
 */
XN_C_API XnStatus XN_C_DECL xnNodeQueryFilterList(XnContext* pContext, const XnNodeQuery* pQuery, XnNodeInfoList* pList);

/** @} */

#endif // __XN_QUERIES_H__
