//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDumpFileWriter.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDumpWriterFileHandle XnDumpFileWriter::OpenFile(const XnChar* /*strDumpName*/, XnBool bSessionDump, const XnChar* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDumpWriterFileHandle result = { NULL };

	XN_FILE_HANDLE* phFile = (XN_FILE_HANDLE*)xnOSMalloc(sizeof(XN_FILE_HANDLE));
	if (phFile == NULL)
	{
		return result;
	}
	
	XnChar strFullPath[XN_FILE_MAX_PATH];
	nRetVal = xnLogCreateNewFile(strFileName, bSessionDump, strFullPath, XN_FILE_MAX_PATH, phFile);
	if (nRetVal != XN_STATUS_OK)
	{
		// we don't have much to do if files can't be open. Dump will not be written
		xnLogWarning(XN_MASK_LOG, "Couldn't create dump file %s! Dump will not be written", strFileName);
	}
	else
	{
		result.pInternal = phFile;
	}

	return result;
}

void XnDumpFileWriter::Write(XnDumpWriterFileHandle hFile, const void* pBuffer, XnUInt32 nBufferSize)
{
	XN_FILE_HANDLE* phFileOS = (XN_FILE_HANDLE*)hFile.pInternal;
	xnOSWriteFile(*phFileOS, pBuffer, nBufferSize);
}

void XnDumpFileWriter::CloseFile(XnDumpWriterFileHandle hFile)
{
	XN_FILE_HANDLE* phFileOS = (XN_FILE_HANDLE*)hFile.pInternal;
	xnOSCloseFile(phFileOS);
	xnOSFree(phFileOS);
}
