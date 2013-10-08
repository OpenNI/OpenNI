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