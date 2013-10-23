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
#ifndef __XN_LICENSING_H__
#define __XN_LICENSING_H__

/**
 * @ingroup cref
 * @defgroup lic Licenses
 * This page details functions for license management.
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
 * Adds a license to the context. 
 *
 * @param	pContext	[in]	OpenNI context.
 * @param	pLicense	[in]	A license key.
 */
XN_C_API XnStatus XN_C_DECL xnAddLicense(XnContext* pContext, const XnLicense* pLicense);

/** 
 * Enumerates all licenses currently registered. The list of licenses should later be freed
 * by calling @ref xnFreeLicensesList().
 *
 * @param	pContext		[in]	OpenNI context.
 * @param	paLicenses		[out]	An array of license pointers.
 * @param	pnCount			[out]	The number of licenses in the array.
 */
XN_C_API XnStatus XN_C_DECL xnEnumerateLicenses(
	 XnContext* pContext,
	 XnLicense** paLicenses,
	 XnUInt32* pnCount
	 );

/**
 * Frees a list of licenses.
 *
 * @param	aLicenses	[in]	The list to free.
 */
XN_C_API void XN_C_DECL xnFreeLicensesList(XnLicense* aLicenses);

/** @} */

#endif // __XN_LICENSING_H__
