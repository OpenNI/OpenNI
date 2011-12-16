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
#ifndef __MOCKPRODUCTIONNODE_H__
#define __MOCKPRODUCTIONNODE_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include <XnStringsHash.h>
#include <XnGeneralBuffer.h>
#include <XnEvent.h>

class MockProductionNode : 
	virtual public xn::ModuleProductionNode,
	virtual public xn::ModuleExtendedSerializationInterface
{
public:
	MockProductionNode(const XnChar* strName);
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
	XN_DECLARE_EVENT_0ARG(PropChangeEvent, PropChangeEventInterface);

	virtual XnStatus OnStateReady();

	XN_DECLARE_STRINGS_HASH(XnUInt64, IntProps);
	XN_DECLARE_STRINGS_HASH(XnDouble, RealProps);
	XN_DECLARE_STRINGS_HASH(const XnChar*, StringProps);
	XN_DECLARE_STRINGS_HASH(XnGeneralBuffer, GeneralProps);

	XnChar m_strName[XN_MAX_NAME_LENGTH];
	IntProps m_intProps;
	RealProps m_realProps;
	StringProps m_stringProps;
	GeneralProps m_generalProps;
	XnBool m_bExtendedSerializationCap;
	XnNodeNotifications* m_pNotifications;
	void* m_pNotificationsCookie;
};

#endif // __MOCKPRODUCTIONNODE_H__
