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
#ifndef __XN_ENUMERATION_ERRORS_H__
#define __XN_ENUMERATION_ERRORS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnTypes.h>

/**
 * @ingroup cref
 * @defgroup enum_errors Enumeration Errors
 * This page details functions for managing an Enumeration Errors object.
 *
 * @section enum Enumeration
 *
 * The following code iterates over an enumeration errors object:
 * @code
for (XnEnumerationErrorsIterator it = xnEnumerationErrorsGetFirst(pList); 
	xnEnumerationErrorsIteratorIsValid(it); 
	it = xnEnumerationErrorsGetNext(it))
{
	const XnProductionNodeDescription* pCurrDesc = xnEnumerationErrorsGetCurrentDescription(it);
	XnStatus nCurrError = xnEnumerationErrorsGetCurrentError(it);
	...
}
 * @endcode
 * @{
 */

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
 * Represents an iterator for an @ref XnEnumerationErrors object.
 */
typedef struct XnModuleError* XnEnumerationErrorsIterator;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------


/**
 * Allocates an enumeration errors object.
 *
 * @param	ppErrors		[out]	The allocated object
 */
XN_C_API XnStatus XN_C_DECL xnEnumerationErrorsAllocate(XnEnumerationErrors** ppErrors);

/**
 * Frees an enumeration errors object
 *
 * @param	pErrors		[in]	Parameter description
 */
XN_C_API void XN_C_DECL xnEnumerationErrorsFree(const XnEnumerationErrors* pErrors);

/**
 * Adds an error to the object.
 *
 * @param	pErrors		[in]	Enumeration Errors object.
 * @param	pDesc		[in]	Description of the failing module.
 * @param	nError		[in]	The enumeration error that occurred.
 */
XN_C_API XnStatus XN_C_DECL xnEnumerationErrorsAdd(XnEnumerationErrors* pErrors, const XnProductionNodeDescription* pDesc, XnStatus nError);

/**
 * Returns a string representation of the Enumeration Errors object.
 *
 * @param	pErrors		[in]	Enumeration Errors object.
 * @param	csBuffer	[in]	A char buffer to be filled.
 * @param	nSize		[in]	Size of the char buffer.
 */
XN_C_API XnStatus XN_C_DECL xnEnumerationErrorsToString(const XnEnumerationErrors* pErrors, XnChar* csBuffer, XnUInt32 nSize);

/**
 * Clears an Enumeration Errors object from all errors in it.
 *
 * @param	pErrors		[in]	Enumeration Errors object.
 */
XN_C_API XnStatus XN_C_DECL xnEnumerationErrorsClear(XnEnumerationErrors* pErrors);

/**
 * Gets an iterator to the first error in the object.
 *
 * @param	pErrors		[in]	Enumeration Errors object.
 */
XN_C_API XnEnumerationErrorsIterator XN_C_DECL xnEnumerationErrorsGetFirst(const XnEnumerationErrors* pErrors);

/**
 * Gets an iterator to the next error in the object.
 *
 * @param	it		[in]	An iterator to an error in the object.
 */
XN_C_API XnEnumerationErrorsIterator XN_C_DECL xnEnumerationErrorsGetNext(XnEnumerationErrorsIterator it);

/**
 * Checks if an iterator is valid (i.e. points to a real error)
 *
 * @param	it		[in]	The iterator to check.
 */
XN_C_API XnBool XN_C_DECL xnEnumerationErrorsIteratorIsValid(XnEnumerationErrorsIterator it);

/**
 * Gets the description of the node from current error.
 *
 * @param	it		[in]	The iterator to the error.
 */
XN_C_API const XnProductionNodeDescription* XN_C_DECL xnEnumerationErrorsGetCurrentDescription(XnEnumerationErrorsIterator it);

/**
 * Gets the enumeration error caused when enumerating the node.
 *
 * @param	it		[in]	The iterator to the error.
 */
XN_C_API XnStatus XN_C_DECL xnEnumerationErrorsGetCurrentError(XnEnumerationErrorsIterator it);

/**
 * @}
 */

#endif // __XN_ENUMERATION_ERRORS_H__
