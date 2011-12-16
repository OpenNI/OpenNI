#ifndef __XN_DUMP_FILE_WRITER_H__
#define __XN_DUMP_FILE_WRITER_H__ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDumpWriters.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
class XnDumpFileWriter : public XnDumpWriterBase
{
public:
	virtual XnDumpWriterFileHandle OpenFile(const XnChar* strDumpName, XnBool bSessionDump, const XnChar* strFileName);
	virtual void Write(XnDumpWriterFileHandle hFile, const void* pBuffer, XnUInt32 nBufferSize);
	virtual void CloseFile(XnDumpWriterFileHandle hFile);
};

#endif // __XN_DUMP_FILE_WRITER_H__