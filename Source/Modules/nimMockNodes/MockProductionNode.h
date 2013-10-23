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
#ifndef __MOCKPRODUCTIONNODE_H__
#define __MOCKPRODUCTIONNODE_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include <XnStringsHashT.h>
#include <XnGeneralBuffer.h>
#include <XnEventT.h>

class MockProductionNode : 
	virtual public xn::ModuleProductionNode,
	virtual public xn::ModuleExtendedSerializationInterface
{
public:
	MockProductionNode(xn::Context& context, const XnChar* strName);
	virtual ~MockProductionNode();

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetRealProperty(const XnChar* strName, XnDouble dValue);
	virtual XnStatus SetStringProperty(const XnChar* strName, const XnChar* strValue);
	virtual XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);
	virtual XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const;
	virtual XnStatus GetRealProperty(const XnChar* strName, XnDouble& dValue) const;
	virtual XnStatus GetStringProperty(const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize) const;
	virtual XnStatus GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const;

	virtual xn::ModuleExtendedSerializationInterface* GetExtendedSerializationInterface() { return this; }

	virtual XnStatus NotifyExState(XnNodeNotifications* pNotifications, void* pCookie);
	virtual void UnregisterExNotifications();

protected:
	typedef XnEventNoArgs PropChangeEvent;

	virtual XnStatus OnStateReady();

	typedef XnStringsHashT<XnUInt64> IntProps;
	typedef XnStringsHashT<XnDouble> RealProps;
	typedef XnStringsHashT<const XnChar*> StringProps;
	typedef XnStringsHashT<XnGeneralBuffer> GeneralProps;

	xn::Context m_context;
	XnChar m_strName[XN_MAX_NAME_LENGTH];
	IntProps m_intProps;
	RealProps m_realProps;
	StringProps m_stringProps;
	GeneralProps m_generalProps;
	XnBool m_bExtendedSerializationCap;
	XnBool m_bStateReady;
	XnNodeNotifications* m_pNotifications;
	void* m_pNotificationsCookie;
};

#endif // __MOCKPRODUCTIONNODE_H__
