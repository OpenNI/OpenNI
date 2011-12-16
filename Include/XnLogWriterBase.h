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
#ifndef __XN_LOG_WRITER_BASE_H__
#define __XN_LOG_WRITER_BASE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnLogTypes.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnLogWriterBase
{
public:
	XnLogWriterBase() : m_bRegistered(FALSE)
	{
		m_cObject.pCookie = this;
		m_cObject.WriteEntry = WriteEntryCallback;
		m_cObject.WriteUnformatted = WriteUnformattedCallback;
		m_cObject.OnConfigurationChanged = OnConfigurationChangedCallback;
		m_cObject.OnClosing = OnClosingCallback;
	}

	virtual ~XnLogWriterBase()
	{
		Unregister();
	}

	XnStatus Register()
	{
		XnStatus nRetVal = XN_STATUS_OK;
		
		if (!m_bRegistered)
		{
			OnRegister();

			nRetVal = xnLogRegisterLogWriter(&m_cObject);
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
			xnLogUnregisterLogWriter(&m_cObject);
			m_bRegistered = FALSE;

			OnUnregister();
		}
	}

	inline XnBool IsRegistered() { return m_bRegistered; }

	virtual void WriteEntry(const XnLogEntry* pEntry) = 0;
	virtual void WriteUnformatted(const XnChar* strMessage) = 0;
	virtual void OnConfigurationChanged() {};
	virtual void OnClosing() 
	{
		Unregister();
	};

	operator const XnLogWriter*() const
	{
		return &m_cObject;
	}

protected:
	virtual void OnRegister() {}
	virtual void OnUnregister() {}

private:
	static void XN_CALLBACK_TYPE WriteEntryCallback(const XnLogEntry* pEntry, void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->WriteEntry(pEntry);
	}
	static void XN_CALLBACK_TYPE WriteUnformattedCallback(const XnChar* strMessage, void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->WriteUnformatted(strMessage);
	}
	static void XN_CALLBACK_TYPE OnConfigurationChangedCallback(void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->OnConfigurationChanged();
	}
	static void XN_CALLBACK_TYPE OnClosingCallback(void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->OnClosing();
	}

	XnLogWriter m_cObject;
	XnBool m_bRegistered;
};

#endif // __XN_LOG_WRITER_BASE_H__
