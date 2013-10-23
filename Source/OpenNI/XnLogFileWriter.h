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
#ifndef __XN_LOG_FILE_WRITER_H__
#define __XN_LOG_FILE_WRITER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnLogWriterBase.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnLogFileWriter : public XnLogWriterBase
{
public:
	XnLogFileWriter();
	~XnLogFileWriter();

	virtual void WriteEntry(const XnLogEntry* pEntry);
	virtual void WriteUnformatted(const XnChar* strMessage);

	XnStatus OpenFile();
	void CloseFile();

	void SetLineInfo(XnBool bLineInfo);

	const XnChar* GetFileName() { return m_strCurrFileName; }

protected:
	virtual void OnRegister();
	virtual void OnUnregister();

private:
	XN_FILE_HANDLE m_fLogFile;
	XnBool m_bWriteLineInfo;
	XnChar m_strCurrFileName[XN_FILE_MAX_PATH];
};

#endif // __XN_LOG_FILE_WRITER_H__