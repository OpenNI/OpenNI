//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDumpFileWriter.h"
#include <xnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDumpWriterFileHandle XnDumpFileWriter::OpenFile(const XnChar* strDumpName, XnBool bSessionDump, const XnChar* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDumpWriterFileHandle result = { NULL };

	XN_FILE_HANDLE hFile;
	nRetVal = xnLogCreateFileEx(strFileName, bSessionDump, &hFile);
	if (nRetVal != XN_STATUS_OK)
	{
		// we don't have much to do if files can't be open. Dump will not be written
		xnLogWarning(XN_MASK_LOG, "Couldn't create dump file %s! Dump will not be written", strFileName);
	}
	else
	{
		result.pInternal = hFile;
	}

	return result;
}

void XnDumpFileWriter::Write(XnDumpWriterFileHandle hFile, const XnUInt8* pBuffer, XnUInt32 nBufferSize)
{
	XN_FILE_HANDLE hFileOS = hFile.pInternal;
	xnOSWriteFile(hFileOS, pBuffer, nBufferSize);
}

void XnDumpFileWriter::CloseFile(XnDumpWriterFileHandle hFile)
{
	XN_FILE_HANDLE hFileOS = hFile.pInternal;
	xnOSCloseFile(&hFileOS);	
}
