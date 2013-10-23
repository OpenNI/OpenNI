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
#include "MockProductionNode.h"
#include "XnPropNames.h"
#include <XnLog.h>


MockProductionNode::MockProductionNode(xn::Context& context, const XnChar* strName) :
	m_bExtendedSerializationCap(FALSE),
	m_pNotifications(NULL),
	m_pNotificationsCookie(NULL),
	m_bStateReady(FALSE),
	m_context(context)
{
	xnOSStrNCopy(m_strName, strName, sizeof(m_strName)-1, sizeof(m_strName));
}

MockProductionNode::~MockProductionNode()
{
	for (StringProps::Iterator it = m_stringProps.Begin(); it != m_stringProps.End(); ++it)
	{
		xnOSFree(it->Value());
	}

	for (GeneralProps::Iterator it2 = m_generalProps.Begin(); it2 != m_generalProps.End(); ++it2)
	{
		XnGeneralBufferFree(&(it2->Value()));
	}
}

XnBool MockProductionNode::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	// during construction (until state is ready), mock node supports all capabilities it can. This is
	// done because OpenNI checks for some capabilities *before* state is ready, and then it gets the
	// wrong information
	if (strcmp(strCapabilityName, XN_CAPABILITY_EXTENDED_SERIALIZATION) == 0)
	{
		return (!m_bStateReady || m_bExtendedSerializationCap);
	}
	else
	{
		return FALSE;
	}
}

XnStatus MockProductionNode::SetIntProperty(const XnChar* strName, XnUInt64 nValue)
{
	if (strcmp(strName, XN_CAPABILITY_EXTENDED_SERIALIZATION) == 0)
	{
		m_bExtendedSerializationCap = (XnBool)nValue;
	}
	else if (strcmp(strName, XN_PROP_STATE_READY) == 0)
	{
		return OnStateReady();
	}
	else
	{
		XnStatus nRetVal = m_intProps.Set(strName, nValue);
		XN_IS_STATUS_OK(nRetVal);

		if (m_pNotifications != NULL)
		{
			nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, m_strName, strName, nValue);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	
	return XN_STATUS_OK;
}

XnStatus MockProductionNode::SetRealProperty(const XnChar* strName, XnDouble dValue)
{
	XnStatus nRetVal = m_realProps.Set(strName, dValue);
	XN_IS_STATUS_OK(nRetVal);

	if (m_pNotifications != NULL)
	{
		nRetVal = m_pNotifications->OnNodeRealPropChanged(m_pNotificationsCookie, m_strName, strName, dValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus MockProductionNode::SetStringProperty(const XnChar* strName, const XnChar* strValue)
{
	const XnChar* strOldVal = NULL;
	if (m_stringProps.Get(strName, strOldVal) == XN_STATUS_OK)
	{
		xnOSFree(strOldVal);
	}

	XnStatus nRetVal = m_stringProps.Set(strName, xnOSStrDup(strValue));
	XN_IS_STATUS_OK(nRetVal);

	if (m_pNotifications != NULL)
	{
		nRetVal = m_pNotifications->OnNodeStringPropChanged(m_pNotificationsCookie, m_strName, strName, strValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus MockProductionNode::SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnGeneralBuffer generalBuffer = {NULL, 0};

	m_generalProps.Get(strName, generalBuffer);
	//If m_generalProps.Get() failed, generalBuffer is still not allocated, and XnGeneralBufferAlloc will allocate it.
	if (nBufferSize != generalBuffer.nDataSize)
	{
		XnGeneralBufferFree(&generalBuffer);

		nRetVal = XnGeneralBufferAlloc(&generalBuffer, nBufferSize);
		XN_IS_STATUS_OK(nRetVal);
	}
	xnOSMemCopy(generalBuffer.pData, pBuffer, nBufferSize);

	nRetVal = m_generalProps.Set(strName, generalBuffer);
	if (nRetVal != XN_STATUS_OK)
	{
		XnGeneralBufferFree(&generalBuffer);
		return nRetVal;
	}

	if (m_pNotifications != NULL)
	{
		nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, m_strName, strName, nBufferSize, pBuffer);
		if (nRetVal != XN_STATUS_OK)
		{
			XnGeneralBufferFree(&generalBuffer);
			return nRetVal;
		}
	}

	return XN_STATUS_OK;
}

XnStatus MockProductionNode::GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
{
	return m_intProps.Get(strName, nValue);
}

XnStatus MockProductionNode::GetRealProperty(const XnChar* strName, XnDouble& dValue) const
{
	return m_realProps.Get(strName, dValue);
}

XnStatus MockProductionNode::GetStringProperty(const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar* val;

	nRetVal = m_stringProps.Get(strName, val);
	XN_IS_STATUS_OK(nRetVal);

	if (strlen(val) > nBufSize)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OUTPUT_BUFFER_OVERFLOW, XN_MASK_OPEN_NI, "Can't get string property '%s' - destination buffer too small", strName);
	}

	strcpy(csValue, val);

	return (XN_STATUS_OK);
}

XnStatus MockProductionNode::GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
{
	XnGeneralBuffer source;
	XnStatus nRetVal = m_generalProps.Get(strName, source);
	XN_IS_STATUS_OK(nRetVal);
	XnGeneralBuffer dest = XnGeneralBufferPack(pBuffer, nBufferSize);
	nRetVal = XnGeneralBufferCopy(&dest, &source);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus MockProductionNode::NotifyExState(XnNodeNotifications* pNotifications, void* pCookie)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// notify int props
	for (IntProps::ConstIterator it = m_intProps.Begin(); it != m_intProps.End(); ++it)
	{
		nRetVal = pNotifications->OnNodeIntPropChanged(pCookie, m_strName, it->Key(), it->Value());
		XN_IS_STATUS_OK(nRetVal);
	}

	// notify real props
	for (RealProps::ConstIterator it = m_realProps.Begin(); it != m_realProps.End(); ++it)
	{
		nRetVal = pNotifications->OnNodeRealPropChanged(pCookie, m_strName, it->Key(), it->Value());
		XN_IS_STATUS_OK(nRetVal);
	}

	// notify string props
	for (StringProps::ConstIterator it = m_stringProps.Begin(); it != m_stringProps.End(); ++it)
	{
		nRetVal = pNotifications->OnNodeStringPropChanged(pCookie, m_strName, it->Key(), it->Value());
		XN_IS_STATUS_OK(nRetVal);
	}

	// notify general props
	for (GeneralProps::ConstIterator it = m_generalProps.Begin(); it != m_generalProps.End(); ++it)
	{
		nRetVal = pNotifications->OnNodeGeneralPropChanged(pCookie, m_strName, it->Key(), it->Value().nDataSize, it->Value().pData);
		XN_IS_STATUS_OK(nRetVal);
	}

	m_pNotifications = pNotifications;
	m_pNotificationsCookie = pCookie;
	
	return (XN_STATUS_OK);
		
}

void MockProductionNode::UnregisterExNotifications()
{
	m_pNotifications = NULL;
	m_pNotificationsCookie = NULL;
}

XnStatus MockProductionNode::OnStateReady()
{
	m_bStateReady = TRUE;
	return (XN_STATUS_OK);
}
