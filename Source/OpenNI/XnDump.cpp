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
#include <XnDump.h>
#include <XnDumpWriters.h>
#include <XnStringsHash.h>
#include <XnLogTypes.h>
#include <XnLog.h>
#include <XnList.h>
#include <XnArray.h>
#include "XnDumpFileWriter.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DUMP_MAX_FORMATTED_STRING_LENGTH	(8*1024)

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
XN_DECLARE_LIST(XnDumpWriter*, XnDumpWriters);
XN_DECLARE_STRINGS_HASH(XnBool, XnDumpsHash);

class DumpData
{
public:
	DumpData() : bDefaultState(FALSE) {}

	~DumpData() {}

	XnDumpWriters writers;
	XnDumpsHash dumpsState;
	XnBool bDefaultState;

	XnDumpFileWriter fileWriter;
};

typedef struct XnDumpWriterFile
{
	XnDumpWriter* pWriter;
	XnDumpWriterFileHandle hFile;
} XnDumpWriterFile;

typedef struct XnDumpFile
{
	XnArray<XnDumpWriterFile> m_writersFiles;
} XnDumpFile;

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
static DumpData* g_pDumpData = new DumpData();
static XnStatus _g_registerStatus = g_pDumpData->fileWriter.Register();

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnDumpSetMaskState(const XnChar* csMask, XnBool bEnabled)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strcmp(csMask, XN_LOG_MASK_ALL) == 0)
	{
		// change default (for future dumps)
		g_pDumpData->bDefaultState = bEnabled;

		// and set all existing ones
		for (XnDumpsHash::Iterator it = g_pDumpData->dumpsState.begin(); it != g_pDumpData->dumpsState.end(); ++it)
		{
			it.Value() = bEnabled;
		}
	}
	else
	{
		nRetVal = g_pDumpData->dumpsState.Set(csMask, bEnabled);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnBool XN_C_DECL xnLogIsDumpMaskEnabled(const XnChar* strDumpMask)
{
	XN_VALIDATE_INPUT_PTR(strDumpMask);

	XnBool bEnabled = FALSE;
	g_pDumpData->dumpsState.Get(strDumpMask, bEnabled);
	return bEnabled;
}

XN_C_API XnStatus XN_C_DECL xnDumpRegisterWriter(XnDumpWriter* pWriter)
{
	return g_pDumpData->writers.AddLast(pWriter);
}

XN_C_API void XN_C_DECL xnDumpUnregisterWriter(XnDumpWriter* pWriter)
{
	g_pDumpData->writers.Remove(pWriter);
}

XN_C_API XnStatus XN_C_DECL xnDumpSetFilesOutput(XnBool bOn)
{
	if (bOn)
	{
		return g_pDumpData->fileWriter.Register();
	}
	else
	{
		g_pDumpData->fileWriter.Unregister();
	}

	return XN_STATUS_OK;
}

XnDumpFile* xnDumpFileOpenImpl(const XnChar* strDumpName, XnBool bForce, XnBool bSessionDump, const XnChar* strNameFormat, va_list args)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if there are writers
	if (g_pDumpData->writers.IsEmpty())
	{
		return NULL;
	}

	if (!bForce)
	{
		if (!xnLogIsDumpMaskEnabled(strDumpName))
		{
			return NULL;
		}
	}

	// format file name
	XnChar strFileName[XN_FILE_MAX_PATH];
	XnUInt32 nChars;
	nRetVal = xnOSStrFormatV(strFileName, XN_FILE_MAX_PATH, &nChars, strNameFormat, args);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_ASSERT(FALSE);
		return NULL;
	}

	// create a handle that will hold all handles to all writers
	XnDumpFile* pFile = XN_NEW(XnDumpFile);

	// try to add writers
	for (XnDumpWriters::Iterator it = g_pDumpData->writers.begin(); it != g_pDumpData->writers.end(); ++it)
	{
		XnDumpWriterFile writerFile;
		writerFile.pWriter = *it;
		writerFile.hFile = writerFile.pWriter->OpenFile(writerFile.pWriter->pCookie, strDumpName, bSessionDump, strFileName);
		XN_ASSERT(writerFile.hFile.pInternal != NULL);
		if (writerFile.hFile.pInternal != NULL)
		{
			nRetVal = pFile->m_writersFiles.AddLast(writerFile);
			XN_ASSERT(nRetVal == XN_STATUS_OK);
		}
	}

	// check if any writer succeeded
	if (pFile->m_writersFiles.IsEmpty())
	{
		// no file. Release memory
		XN_DELETE(pFile);
		return NULL;
	}

	// return the file pointer
	return pFile;
}

XN_C_API XnDumpFile* XN_C_DECL xnDumpFileOpen(const XnChar* strDumpName, const XnChar* strNameFormat, ...)
{
	if (strDumpName == NULL || strNameFormat == NULL)
	{
		XN_ASSERT(FALSE);
		return NULL;
	}

	va_list args;
	va_start(args, strNameFormat);
	XnDumpFile* pFile = xnDumpFileOpenImpl(strDumpName, FALSE, TRUE, strNameFormat, args);
	va_end(args);

	return pFile;
}

XN_C_API XnDumpFile* XN_C_DECL xnDumpFileOpenEx(const XnChar* strDumpName, XnBool bForce, XnBool bSessionDump, const XnChar* strNameFormat, ...)
{
	if (strNameFormat == NULL)
	{
		XN_ASSERT(FALSE);
		return NULL;
	}

	va_list args;
	va_start(args, strNameFormat);
	XnDumpFile* pFile = xnDumpFileOpenImpl(strDumpName, bForce, bSessionDump, strNameFormat, args);
	va_end(args);

	return pFile;
}

XN_C_API void XN_C_DECL _xnDumpFileWriteBuffer(XnDumpFile* pFile, const void* pBuffer, XnUInt32 nBufferSize)
{
	if (pFile == NULL)
	{
		return;
	}

	// write to each writer
	for (XnUInt32 i = 0; i < pFile->m_writersFiles.GetSize(); ++i)
	{
		XnDumpWriter* pWriter = pFile->m_writersFiles[i].pWriter;
		XnDumpWriterFileHandle hWriterFile = pFile->m_writersFiles[i].hFile;

		pWriter->Write(pWriter->pCookie, hWriterFile, pBuffer, nBufferSize);
	}
}

XN_C_API void XN_C_DECL _xnDumpFileWriteString(XnDumpFile* pFile, const XnChar* strFormat, ...)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (pFile == NULL)
	{
		return;
	}

	// format string
	XnChar strBuffer[XN_DUMP_MAX_FORMATTED_STRING_LENGTH];
	XnUInt32 nCharsWritten;
	va_list args;
	va_start(args, strFormat);
	nRetVal = xnOSStrFormatV(strBuffer, sizeof(strBuffer), &nCharsWritten, strFormat, args);
	XN_ASSERT(nRetVal == XN_STATUS_OK);
	va_end(args);

	_xnDumpFileWriteBuffer(pFile, (const XnUInt8*)strBuffer, nCharsWritten);
}

XN_C_API void XN_C_DECL _xnDumpFileClose(XnDumpFile* pFile)
{
	if (pFile == NULL)
	{
		return;
	}

	// Notify each writer
	for (XnUInt32 i = 0; i < pFile->m_writersFiles.GetSize(); ++i)
	{
		XnDumpWriter* pWriter = pFile->m_writersFiles[i].pWriter;
		XnDumpWriterFileHandle hWriterFile = pFile->m_writersFiles[i].hFile;

		pWriter->CloseFile(pWriter->pCookie, hWriterFile);
	}
}

//---------------------------------------------------------------------------
// Backwards Compatibility Stuff
//---------------------------------------------------------------------------
#ifndef __XN_NO_BC__

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
		xnOSWriteFile(pDump->hFile, csHeader, (XnUInt32)strlen(csHeader));
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

#endif // #ifndef __XN_NO_BC__