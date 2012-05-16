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
#ifndef __XN_PRD_NODE_INFO_LIST_H__
#define __XN_PRD_NODE_INFO_LIST_H__

/**
 * @ingroup cref
 * @defgroup infolist Production Node Info List
 * This page details functions for handling production nodes lists. The lists are implemented as
 * doubly-linked lists, but it's not recommended to access the list members directly.
 * instead, use the provided functions for handling the list.
 * 
 * @section append Adding and Removing Node Info Objects
 *
 * Nodes can be added to the list using @ref xnNodeInfoListAdd(). Note that once an info object
 * is part of the list, it will be freed if that list is freed. A node can be removed by calling @ref 
 * xnNodeInfoListRemove(), which also frees that element.
 *
 * A short example:
 * @code
// Start with an empty list
XnNodeInfoList* pList;
nRetVal = xnNodeInfoListAllocate(&pList);
// TODO: check error code

// Add first
pList = xnNodeInfoListAdd(pList, pDescription1, NULL, NULL);

// Add second
pList = xnNodeInfoListAdd(pList, pDescription2, NULL, NULL);
 * @endcode
 *
 * @section enum Enumeration
 *
 * Forward Iteration:
 * @code
for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pList); 
	xnNodeInfoListIteratorIsValid(it); 
	it = xnNodeInfoListGetNext(it))
{
	XnNodeInfo* pCurrent = xnNodeInfoListGetCurrent(it);
	...
}
 * @endcode
 *
 * Backwards Iteration:
 * @code
for (XnNodeInfoListIterator it = xnNodeInfoListGetLast(pList); 
	xnNodeInfoListIteratorIsValid(it); 
	it = xnNodeInfoListGetPrevious(it))
{
	XnNodeInfo* pCurrent = xnNodeInfoListGetCurrent(it);
	...
}
 * @endcode
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
 * Allocates a @ref XnNodeInfoList object. This object should be freed using @ref xnNodeInfoListFree().
 *
 * @param	ppList				[out]		The list.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListAllocate(XnNodeInfoList** ppList);

/**
 * Frees a @ref XnNodeInfoList object previously allocated with @ref xnNodeInfoListAllocate().
 *
 * @param	pList				[out]		The list.
 */
XN_C_API void XN_C_DECL xnNodeInfoListFree(XnNodeInfoList* pList);

/**
 * Creates and adds a single @ref XnNodeInfo object to the list. See also @ref xnNodeInfoListAddEx().
 *
 * @param	pList				[in]		The list.
 * @param	pDescription		[in]		The description of this production node.
 * @param	strCreationInfo		[in]		Optional. Additional needed information for instantiation.
 * @param	pNeededNodes		[in]		Optional. A list of needed nodes.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListAdd(XnNodeInfoList* pList, const XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnNodeInfoList* pNeededNodes);

/**
 * Creates and adds a single @ref XnNodeInfo object to the list, with additional data. This data can be later
 * extracted using the @ref xnNodeInfoGetAdditionalData(). Before the node info object is freed, the pFreeHandler
 * callback will be called, so it could free the additional data object.
 *
 * @param	pList				[in]		The list.
 * @param	pDescription		[in]		The description of this production node.
 * @param	strCreationInfo		[in]		Optional. Additional needed information for instantiation.
 * @param	pNeededNodes		[in]		Optional. A list of needed nodes.
 * @param	pAdditionalData		[in]		Additional data, which is specific to this node.
 * @param	pFreeHandler		[in]		Optional. A callback function for freeing the additional data.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListAddEx(XnNodeInfoList* pList, const XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnNodeInfoList* pNeededNodes, const void* pAdditionalData, XnFreeHandler pFreeHandler);

/**
 * Adds a single @ref XnNodeInfo object to the list.
 *
 * @param	pList				[in]		The list.
 * @param	pNode				[in]		The node to add.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListAddNode(XnNodeInfoList* pList, XnNodeInfo* pNode);

/**
 * Adds a node from another list to this list (the node is not removed from the other list).
 *
 * @param	pList				[in]		The list.
 * @param	otherListIt			[in]		An iterator obtained from another list.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListAddNodeFromList(XnNodeInfoList* pList, XnNodeInfoListIterator otherListIt);

/**
 * Removes an element from the list, and frees it.
 *
 * @param	pList	[in]		The list.
 * @param	it		[in]		Iterator to the element that should be removed.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListRemove(XnNodeInfoList* pList, XnNodeInfoListIterator it);

/**
 * Clears a node info list, freeing all the elements in it.
 *
 * @param	pList	[in]	The list to be freed.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListClear(XnNodeInfoList* pList);

/**
 * Appends another list at the end of this list. Note that the other list becomes empty,
 * but still needs to be freed.
 *
 * @param	pList		[in]		A list.
 * @param	pOther		[in]		The list to be appended.
 */
XN_C_API XnStatus XN_C_DECL xnNodeInfoListAppend(XnNodeInfoList* pList, XnNodeInfoList* pOther);

/**
* Checks if the given list is empty
*
* @param	pList		[in]		A list.
*/
XN_C_API XnBool XN_C_DECL xnNodeInfoListIsEmpty(XnNodeInfoList* pList);

/**
 * Gets the first element of the list.
 *
 * @param	pList		[in]	[Optional] A list.
 *
 * @returns an iterator to the first element of the list, or NULL if the list is empty.
 */
XN_C_API XnNodeInfoListIterator XN_C_DECL xnNodeInfoListGetFirst(XnNodeInfoList* pList);

/**
 * Gets the last element of the list.
 *
 * @param	pList		[in]	[Optional] A list.
 *
 * @returns an iterator to the last element of the list, or NULL if the list is empty.
 */
XN_C_API XnNodeInfoListIterator XN_C_DECL xnNodeInfoListGetLast(XnNodeInfoList* pList);

/**
 * Checks if the current iterator points to a valid location.
 *
 * @param	it		[in]	An iterator.
 */
XN_C_API XnBool XN_C_DECL xnNodeInfoListIteratorIsValid(XnNodeInfoListIterator it);

/**
 * Gets current element from an iterator. 
 *
 * @param	it		[in]	An iterator.
 * 
 * @returns an @ref XnNodeInfo pointer.
 */
XN_C_API XnNodeInfo* XN_C_DECL xnNodeInfoListGetCurrent(XnNodeInfoListIterator it);

/**
 * Gets an iterator to the next element from a current iterator.
 *
 * @param	it		[in]	An iterator.
 * 
 * @returns an iterator to the next element.
 */
XN_C_API XnNodeInfoListIterator XN_C_DECL xnNodeInfoListGetNext(XnNodeInfoListIterator it);

/**
 * Gets an iterator to the previous element from a current iterator.
 *
 * @param	it		[in]	An iterator.
 * 
 * @returns an iterator to the previous element.
 */
XN_C_API XnNodeInfoListIterator XN_C_DECL xnNodeInfoListGetPrevious(XnNodeInfoListIterator it);

/** @} */

#endif // __XN_PRD_NODE_INFO_LIST_H__
