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
#ifndef __XNMOCKNOTIFIER_H__
#define __XNMOCKNOTIFIER_H__

#include <XnTypes.h>

namespace xn
{

class MockNotifier : public XnNodeNotifications
{
public:
	static MockNotifier& GetInstance();

private:
	MockNotifier();
	static XnStatus XN_CALLBACK_TYPE OnNodeAddedImpl(void* pCookie, const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression);
	static XnStatus XN_CALLBACK_TYPE OnNodeRemovedImpl(void* pCookie, const XnChar* strNodeName);
	static XnStatus XN_CALLBACK_TYPE OnNodeIntPropChangedImpl(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeRealPropChangedImpl(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, XnDouble dValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeStringPropChangedImpl(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeGeneralPropChangedImpl(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer);
	static XnStatus XN_CALLBACK_TYPE OnNodeStateReadyImpl(void* pCookie, const XnChar* strNodeName);
	static XnStatus XN_CALLBACK_TYPE OnNodeNewDataImpl(void* pCookie, const XnChar* strNodeName, 
		XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize);
};

}


#endif // __XNMOCKNOTIFIER_H__