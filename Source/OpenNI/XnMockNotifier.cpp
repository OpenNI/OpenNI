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
#include "XnMockNotifier.h"
#include <XnPrdNode.h>
#include <XnPropNames.h>

namespace xn 
{

MockNotifier::MockNotifier()
{
	OnNodeAdded = &OnNodeAddedImpl;
	OnNodeRemoved = &OnNodeRemovedImpl;
	OnNodeIntPropChanged = &OnNodeIntPropChangedImpl;
	OnNodeRealPropChanged = &OnNodeRealPropChangedImpl;
	OnNodeStringPropChanged = &OnNodeStringPropChangedImpl;
	OnNodeGeneralPropChanged = &OnNodeGeneralPropChangedImpl;
	OnNodeStateReady = &OnNodeStateReadyImpl;
	OnNodeNewData = &OnNodeNewDataImpl;
}

MockNotifier& MockNotifier::GetInstance()
{
	static MockNotifier _instance;
	return _instance;
}

XnStatus MockNotifier::OnNodeAddedImpl(void* /*pCookie*/, const XnChar* /*strNodeName*/, XnProductionNodeType /*type*/, XnCodecID /*compression*/)
{
	XN_ASSERT(FALSE);
	return XN_STATUS_NOT_IMPLEMENTED;
}

XnStatus MockNotifier::OnNodeRemovedImpl(void* /*pCookie*/, const XnChar* /*strNodeName*/)
{
	XN_ASSERT(FALSE);
	return XN_STATUS_NOT_IMPLEMENTED;
}

XnStatus MockNotifier::OnNodeIntPropChangedImpl(void* pCookie, const XnChar* /*strNodeName*/, const XnChar* strPropName, XnUInt64 nValue)
{
	return xnSetIntProperty((XnNodeHandle)pCookie, strPropName, nValue);
}

XnStatus MockNotifier::OnNodeRealPropChangedImpl(void* pCookie, const XnChar* /*strNodeName*/, const XnChar* strPropName, XnDouble dValue)
{
	return xnSetRealProperty((XnNodeHandle)pCookie, strPropName, dValue);
}

XnStatus MockNotifier::OnNodeStringPropChangedImpl(void* pCookie, const XnChar* /*strNodeName*/, const XnChar* strPropName, const XnChar* strValue)
{
	return xnSetStringProperty((XnNodeHandle)pCookie, strPropName, strValue);
}

XnStatus MockNotifier::OnNodeGeneralPropChangedImpl(void* pCookie, const XnChar* /*strNodeName*/, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	return xnSetGeneralProperty((XnNodeHandle)pCookie, strPropName, nBufferSize, pBuffer);
}

XnStatus MockNotifier::OnNodeStateReadyImpl(void* pCookie, const XnChar* /*strNodeName*/)
{
	return xnSetIntProperty((XnNodeHandle)pCookie, XN_PROP_STATE_READY, 1);
}

XnStatus MockNotifier::OnNodeNewDataImpl(void* /*pCookie*/, const XnChar* /*strNodeName*/, XnUInt64 /*nTimeStamp*/, XnUInt32 /*nFrame*/, const void* /*pData*/, XnUInt32 /*nSize*/)
{
	XN_ASSERT(FALSE);
	return XN_STATUS_INVALID_OPERATION;
}

} //namespace xn
