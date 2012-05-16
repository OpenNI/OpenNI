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