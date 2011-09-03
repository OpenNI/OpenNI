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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDump.h>
#include <XnStringsHash.h>
#include <XnLogTypes.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
XnLogFilteringType g_DumpFilteringType;
XnStringsHash g_DumpMasks;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnDumpSetMaskState(const XnChar* csMask, XnBool bEnabled)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strcmp(csMask, XN_LOG_MASK_ALL) == 0)
	{
		g_DumpFilteringType = bEnabled ? XN_LOG_WRITE_ALL : XN_LOG_WRITE_NONE;

		if (!bEnabled)
		{
			g_DumpMasks.Clear();
		}
	}
	else
	{
		g_DumpFilteringType = XN_LOG_WRITE_MASKS;

		if (bEnabled)
		{
			nRetVal = g_DumpMasks.Set(csMask, NULL);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			XnValue val = NULL;
			nRetVal = g_DumpMasks.Remove(csMask, val);
			if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_NO_MATCH)
			{
				return (XN_STATUS_OK);
			}
		}
	}

	return (XN_STATUS_OK);
}

XnBool xnLogIsDumpMaskEnabled(const XnChar* csDumpMask)
{
	XN_VALIDATE_INPUT_PTR(csDumpMask);

	switch (g_DumpFilteringType)
	{
	case XN_LOG_WRITE_ALL:
		return TRUE;
	case XN_LOG_WRITE_NONE:
		return FALSE;
	case XN_LOG_WRITE_MASKS:
		{
			XnStringsHash::Iterator it = g_DumpMasks.end();
			return (XN_STATUS_OK == g_DumpMasks.Find(csDumpMask, it));
		}
	default:
		printf("Log: Unknown filter type: %d", g_DumpFilteringType);
		return FALSE;
	}
}

XnStatus xnDumpCreate(XnDump* pDump, const XnChar* csHeader, const XnChar* csFileNameFormat, va_list args)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnChar strFileName[XN_FILE_MAX_PATH];
	XnUInt32 nChars;
	nRetVal = xnOSStrFormatV(strFileName, XN_FILE_MAX_PATH, &nChars, csFileNameFormat, args);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLogCreateFile(strFileName, &pDump->hFile);
	if (nRetVal != XN_STATUS_OK)
	{
		// we don't have much to do if files can't be open. Dump will not be written
		xnLogWarning(XN_MASK_LOG, "Couldn't create dump file %s! Dump will not be written", strFileName);
		pDump->hFile = XN_INVALID_FILE_HANDLE;
		return nRetVal;
	}

	if (csHeader != NULL)
	{
		xnDumpWriteStringImpl(*pDump, csHeader);
	}

	return XN_STATUS_OK;
}

XN_C_API void xnDumpInit(XnDump* pDump, const XnChar* csDumpMask, const XnChar* csHeader, const XnChar* csFileNameFormat, ...)
{
	if (pDump->hFile == XN_INVALID_FILE_HANDLE && xnLogIsDumpMaskEnabled(csDumpMask))
	{
		va_list args;
		va_start(args, csFileNameFormat);
		xnDumpCreate(pDump, csHeader, csFileNameFormat, args);
		va_end(args);
	}
}

XN_C_API void xnDumpForceInit(XnDump* pDump, const XnChar* csHeader, const XnChar* csFileNameFormat, ...)
{
	if (pDump->hFile == XN_INVALID_FILE_HANDLE)
	{
		va_list args;
		va_start(args, csFileNameFormat);
		xnDumpCreate(pDump, csHeader, csFileNameFormat, args);
		va_end(args);
	}
}

XN_C_API void xnDumpClose(XnDump* pDump)
{
	if (pDump->hFile != XN_INVALID_FILE_HANDLE)
	{
		xnOSCloseFile(&pDump->hFile);
		*pDump = XN_DUMP_CLOSED;
	}
}

XN_C_API void xnDumpWriteBufferImpl(XnDump dump, const void* pBuffer, XnUInt32 nBufferSize)
{
	if (dump.hFile != XN_INVALID_FILE_HANDLE) 
	{
		xnOSWriteFile(dump.hFile, pBuffer, nBufferSize);
	}
}

XN_C_API void xnDumpWriteStringImpl(XnDump dump, const XnChar* csFormat, ...)
{
	if (dump.hFile != XN_INVALID_FILE_HANDLE)
	{
		const XnUInt32 nStringLength = 1024;
		XnChar csString[1024];

		va_list args;
		va_start(args, csFormat);

		// format message
		XnUInt32 nChars;
		xnOSStrFormatV(csString, nStringLength, &nChars, csFormat, args);

		// and write it to file
		xnOSWriteFile(dump.hFile, csString, nChars);

		va_end(args);
	}
}

XN_C_API void xnDumpFlush(XnDump dump)
{
	if (dump.hFile != XN_INVALID_FILE_HANDLE)
	{
		xnOSFlushFile(dump.hFile);
	}
}
