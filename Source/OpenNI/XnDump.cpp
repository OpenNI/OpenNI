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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDump.h>
#include <XnDumpWriters.h>
#include <XnStringsHashT.h>
#include <XnLogTypes.h>
#include <XnLog.h>
#include <XnListT.h>
#include <XnArray.h>
#include "XnDumpFileWriter.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DUMP_MAX_FORMATTED_STRING_LENGTH	(8*1024)

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef XnListT<XnDumpWriter*> XnDumpWriters;
typedef XnStringsHashT<XnBool> XnDumpsHash;

class DumpData
{
public:
	static DumpData& GetInstance()
	{
		// NOTE: this instance will never be destroyed (because some static object destructor might write/close dumps, and 
		// destruction order is not-deterministic).
		static DumpData* pSingleton = XN_NEW(DumpData);
		return *pSingleton;
	}

	void SetStateGlobally(XnBool bState)
	{
		// change default (for future dumps)
		this->bDefaultState = bState;
		// and set all existing ones
		for (XnDumpsHash::Iterator it = this->dumpsState.Begin(); it != this->dumpsState.End(); ++it)
		{
			it->Value() = bState;
		}
	}

	XnDumpWriters writers;
	XnDumpsHash dumpsState;
	XnBool bDefaultState;

private:
	DumpData() : bDefaultState(FALSE) {}
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
XnDumpFileWriter g_fileWriter;
XnStatus _register_status = g_fileWriter.Register();

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnDumpSetMaskState(const XnChar* csMask, XnBool bEnabled)
{
	XnStatus nRetVal = XN_STATUS_OK;

	DumpData& dumpData = DumpData::GetInstance();

	if (strcmp(csMask, XN_LOG_MASK_ALL) == 0)
	{
		dumpData.SetStateGlobally(bEnabled);
	}
	else
	{
		nRetVal = dumpData.dumpsState.Set(csMask, bEnabled);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnBool XN_C_DECL xnLogIsDumpMaskEnabled(const XnChar* strDumpMask)
{
	XN_VALIDATE_INPUT_PTR(strDumpMask);

	XnBool bEnabled = FALSE;
	DumpData::GetInstance().dumpsState.Get(strDumpMask, bEnabled);
	return bEnabled;
}

XN_C_API XnStatus XN_C_DECL xnDumpRegisterWriter(XnDumpWriter* pWriter)
{
	return DumpData::GetInstance().writers.AddLast(pWriter);
}

XN_C_API void XN_C_DECL xnDumpUnregisterWriter(XnDumpWriter* pWriter)
{
	DumpData::GetInstance().writers.Remove(pWriter);
}

XN_C_API XnStatus XN_C_DECL xnDumpSetFilesOutput(XnBool bOn)
{
	if (bOn)
	{
		return g_fileWriter.Register();
	}
	else
	{
		g_fileWriter.Unregister();
	}

	return XN_STATUS_OK;
}

XnDumpFile* xnDumpFileOpenImpl(const XnChar* strDumpName, XnBool bForce, XnBool bSessionDump, const XnChar* strNameFormat, va_list args)
{
	XnStatus nRetVal = XN_STATUS_OK;

	DumpData& dumpData = DumpData::GetInstance();

	// check if there are writers
	if (dumpData.writers.IsEmpty())
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
	for (XnDumpWriters::Iterator it = dumpData.writers.Begin(); it != dumpData.writers.End(); ++it)
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
	XN_REFERENCE_VARIABLE(nRetVal);
	
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

    XN_DELETE(pFile);
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

	XnChar strFullPath[XN_FILE_MAX_PATH];
	nRetVal = xnLogCreateNewFile(strFileName, TRUE, strFullPath, XN_FILE_MAX_PATH, &pDump->hFile);
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