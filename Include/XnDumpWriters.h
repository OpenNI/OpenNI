#ifndef __XN_DUMP_WRITERS_H__
#define __XN_DUMP_WRITERS_H__ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDump.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnDumpWriterFileHandle
{
	void* pInternal;
} XnDumpWriterFileHandle;

typedef struct XnDumpWriter
{
	void* pCookie;
	XnDumpWriterFileHandle (XN_CALLBACK_TYPE* OpenFile)(void* pCookie, const XnChar* strDumpName, XnBool bSessionDump, const XnChar* strFileName);
	void (XN_CALLBACK_TYPE* Write)(void* pCookie, XnDumpWriterFileHandle hFile, const void* pBuffer, XnUInt32 nBufferSize);
	void (XN_CALLBACK_TYPE* CloseFile)(void* pCookie, XnDumpWriterFileHandle hFile);
} XnDumpWriter;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnDumpRegisterWriter(XnDumpWriter* pWriter);

XN_C_API void XN_C_DECL xnDumpUnregisterWriter(XnDumpWriter* pWriter);

XN_C_API XnStatus XN_C_DECL xnDumpSetFilesOutput(XnBool bOn);

//---------------------------------------------------------------------------
// Helpers
//---------------------------------------------------------------------------
#ifdef __cplusplus

class XnDumpWriterBase
{
public:
	XnDumpWriterBase() : m_bRegistered(FALSE)
	{
		m_cObject.pCookie = this;
		m_cObject.OpenFile = OpenFileCallback;
		m_cObject.Write = WriteCallback;
		m_cObject.CloseFile = CloseFileCallback;
	}

	virtual ~XnDumpWriterBase()
	{
		Unregister();
	}

	XnStatus Register()
	{
		XnStatus nRetVal = XN_STATUS_OK;

		if (!m_bRegistered)
		{
			OnRegister();

			nRetVal = xnDumpRegisterWriter(&m_cObject);
			if (nRetVal != XN_STATUS_OK)
			{
				OnUnregister();
				return (nRetVal);
			}

			m_bRegistered = TRUE;
		}

		return (XN_STATUS_OK);
	}

	void Unregister()
	{
		if (m_bRegistered)
		{
			xnDumpUnregisterWriter(&m_cObject);
			m_bRegistered = FALSE;

			OnUnregister();
		}
	}

	inline XnBool IsRegistered() { return m_bRegistered; }

	virtual XnDumpWriterFileHandle OpenFile(const XnChar* strDumpName, XnBool bSessionDump, const XnChar* strFileName) = 0;
	virtual void Write(XnDumpWriterFileHandle hFile, const void* pBuffer, XnUInt32 nBufferSize) = 0;
	virtual void CloseFile(XnDumpWriterFileHandle hFile) = 0;

	operator const XnDumpWriter*() const
	{
		return &m_cObject;
	}

protected:
	virtual void OnRegister() {}
	virtual void OnUnregister() {}

private:
	static XnDumpWriterFileHandle XN_CALLBACK_TYPE OpenFileCallback(void* pCookie, const XnChar* strDumpName, XnBool bSessionDump, const XnChar* strFileName)
	{
		XnDumpWriterBase* pThis = (XnDumpWriterBase*)pCookie;
		return pThis->OpenFile(strDumpName, bSessionDump, strFileName);
	}

	static void XN_CALLBACK_TYPE WriteCallback(void* pCookie, XnDumpWriterFileHandle hFile, const void* pBuffer, XnUInt32 nBufferSize)
	{
		XnDumpWriterBase* pThis = (XnDumpWriterBase*)pCookie;
		return pThis->Write(hFile, pBuffer, nBufferSize);
	}

	static void XN_CALLBACK_TYPE CloseFileCallback(void* pCookie, XnDumpWriterFileHandle hFile)
	{
		XnDumpWriterBase* pThis = (XnDumpWriterBase*)pCookie;
		return pThis->CloseFile(hFile);
	}

	XnDumpWriter m_cObject;
	XnBool m_bRegistered;
};

#endif

#endif // __XN_DUMP_WRITERS_H__