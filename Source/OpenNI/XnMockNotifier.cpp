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
