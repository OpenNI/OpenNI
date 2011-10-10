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
