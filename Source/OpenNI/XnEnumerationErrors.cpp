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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnEnumerationErrors.h"
#include "XnInternalTypes.h"
#include <XnUtils.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnModuleError
{
	XnProductionNodeDescription description;
	XnStatus nError;
	XnModuleError* pNext;
} XnModuleError;

struct XnEnumerationErrors
{
	XnModuleError* pFirst;
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_C_API XnStatus xnEnumerationErrorsAllocate(XnEnumerationErrors** ppError)
{
	XN_VALIDATE_OUTPUT_PTR(ppError);

	XN_VALIDATE_CALLOC(*ppError, XnEnumerationErrors, 1);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnEnumerationErrorsClear(XnEnumerationErrors* pError)
{
	XN_VALIDATE_INPUT_PTR(pError);
	XnModuleError* pModuleError = pError->pFirst;
	while (pModuleError != NULL)
	{
		XnModuleError* pNext = pModuleError->pNext;
		xnOSFree(pModuleError);
		pModuleError = pNext;
	}

	pError->pFirst = NULL;
	return (XN_STATUS_OK);
}

XN_C_API void xnEnumerationErrorsFree(const XnEnumerationErrors* pError)
{
	if (pError != NULL)
	{
		// object will be freed in a second, it's OK to const_cast it.
		xnEnumerationErrorsClear(const_cast<XnEnumerationErrors*>(pError));

		xnOSFree(pError);
	}
}

XN_C_API XnStatus xnEnumerationErrorsAdd(XnEnumerationErrors* pError, const XnProductionNodeDescription* pDesc, XnStatus nError)
{
	XN_VALIDATE_INPUT_PTR(pError);
	XN_VALIDATE_INPUT_PTR(pDesc);

	// find where to add it
	XnModuleError** ppWhere = &pError->pFirst;
	while ((*ppWhere) != NULL)
	{
		ppWhere = &(*ppWhere)->pNext;
	}

	// allocate
	XnModuleError* pModuleError = NULL;
	XN_VALIDATE_CALLOC(pModuleError, XnModuleError, 1);

	pModuleError->description = *pDesc;
	pModuleError->nError = nError;

	*ppWhere = pModuleError;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnEnumerationErrorsToString(const XnEnumerationErrors* pErrors, XnChar* csBuffer, XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt32 nWritten = 0;
	csBuffer[0] = '\0';

	nRetVal = xnOSStrAppend(csBuffer, "One or more of the following nodes could not be enumerated:\n\n", nSize);
	XN_IS_STATUS_OK(nRetVal);

	nWritten = xnOSStrLen(csBuffer);

	for (XnEnumerationErrorsIterator it = xnEnumerationErrorsGetFirst(pErrors);
		xnEnumerationErrorsIteratorIsValid(it);
		it = xnEnumerationErrorsGetNext(it))
	{
		nRetVal = xnProductionNodeDescriptionToString(xnEnumerationErrorsGetCurrentDescription(it), csBuffer + nWritten, nSize - nWritten);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnOSStrAppend(csBuffer, ": ", nSize);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnOSStrAppend(csBuffer, xnGetStatusString(xnEnumerationErrorsGetCurrentError(it)), nSize);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnOSStrAppend(csBuffer, "\n", nSize);
		XN_IS_STATUS_OK(nRetVal);

		nWritten = xnOSStrLen(csBuffer);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnEnumerationErrorsIterator xnEnumerationErrorsGetFirst(const XnEnumerationErrors* pError)
{
	return (pError->pFirst);
}

XN_C_API XnBool xnEnumerationErrorsIteratorIsValid(XnEnumerationErrorsIterator it)
{
	return (it != NULL);
}

XN_C_API XnEnumerationErrorsIterator xnEnumerationErrorsGetNext(XnEnumerationErrorsIterator it)
{
	return (it->pNext);
}

XN_C_API const XnProductionNodeDescription* xnEnumerationErrorsGetCurrentDescription(XnEnumerationErrorsIterator it)
{
	return (&it->description);
}

XN_C_API XnStatus xnEnumerationErrorsGetCurrentError(XnEnumerationErrorsIterator it)
{
	return (it->nError);
}

