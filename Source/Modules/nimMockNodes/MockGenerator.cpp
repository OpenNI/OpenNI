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
#include "MockGenerator.h"
#include <XnPropNames.h>
#include <XnLog.h>

#define XN_MOCK_LOG_MASK	"Mock"

MockGenerator::MockGenerator(xn::Context& context, const XnChar* strName, XnBool bAggregateData /* = FALSE */) :
	MockProductionNode(context, strName),
	m_bAggregateData(bAggregateData),
	m_bGenerating(FALSE),
	m_bMirror(FALSE),
	m_nCurrentDataIdx(0),
	m_nNextDataIdx(1),
	m_bNewDataAvailable(FALSE),
	m_bMirrorCap(FALSE),
	m_bFrameSyncCap(FALSE),
	m_bFrameSyncWithExists(FALSE),
	m_hNodeCreationCallback(NULL),
	m_hNodeDestructionCallback(NULL)
{
	memset(&m_data, 0, sizeof(m_data));
	m_strFrameSyncWith[0] = '\0';
}

MockGenerator::~MockGenerator()
{
	if (m_hNodeCreationCallback != NULL)
	{
		m_context.UnregisterFromNodeCreation(m_hNodeCreationCallback);
		m_hNodeCreationCallback = NULL;
	}

	if (m_hNodeDestructionCallback != NULL)
	{
		m_context.UnregisterFromNodeDestruction(m_hNodeDestructionCallback);
		m_hNodeDestructionCallback = NULL;
	}

	for (XnUInt32 i = 0; i < NUM_BUFFERS; i++)
	{
		xnOSFreeAligned(m_data[i].pData);
	}
}

XnBool MockGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	if (strcmp(strCapabilityName, XN_CAPABILITY_MIRROR) == 0)
	{
		return (!m_bStateReady || m_bMirrorCap);
	}
	else if (strcmp(strCapabilityName, XN_CAPABILITY_FRAME_SYNC) == 0)
	{
		return (!m_bStateReady || m_bFrameSyncCap);
	}
	//TODO: Support alt view cap
	else
	{
		return MockProductionNode::IsCapabilitySupported(strCapabilityName);
	}
}

XnStatus MockGenerator::SetIntProperty(const XnChar* strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strcmp(strName, XN_PROP_IS_GENERATING) == 0)
	{
		SetGenerating((XnBool)nValue);
	}
	else if (strcmp(strName, XN_PROP_TIMESTAMP) == 0)
	{
		m_data[m_nNextDataIdx].nTimeStamp = nValue;
	}
	else if (strcmp(strName, XN_PROP_FRAME_ID) == 0)
	{
		m_data[m_nNextDataIdx].nFrameID = (XnUInt32)nValue;
	}
	else if (strcmp(strName, XN_CAPABILITY_MIRROR) == 0)
	{
		m_bMirrorCap = (XnBool)nValue;
	}
	else if (strcmp(strName, XN_CAPABILITY_FRAME_SYNC) == 0)
	{
		m_bFrameSyncCap = (XnBool)nValue;
	}
	else if (strcmp(strName, XN_PROP_MIRROR) == 0)
	{
		nRetVal = SetMirror((XnBool)nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	//TODO: Check for alternative view point cap, framesync cap	
	else
	{
		return MockProductionNode::SetIntProperty(strName, nValue);
	}

	return XN_STATUS_OK;
}

XnStatus MockGenerator::SetStringProperty(const XnChar* strName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (strcmp(strName, XN_PROP_FRAME_SYNCED_WITH) == 0)
	{
		nRetVal = SetFrameSyncNode(strValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = MockProductionNode::SetStringProperty(strName, strValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus MockGenerator::SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (strcmp(strName, XN_PROP_NEWDATA) == 0)
	{
		nRetVal = SetNextData(pBuffer, nBufferSize);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = MockProductionNode::SetGeneralProperty(strName, nBufferSize, pBuffer);
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

XnStatus MockGenerator::SetNextData(const void *pData, XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	//Make sure the node is in Generating state so it would be recorded properly
	SetGenerating(TRUE);

	DataInfo& nextData = m_data[m_nNextDataIdx];

	if (!m_bAggregateData)
	{
		nextData.nDataSize = 0;
	}

	nRetVal = ResizeBuffer(m_nNextDataIdx, nextData.nDataSize + nSize);
	XN_IS_STATUS_OK(nRetVal);

	xnOSMemCopy((XnUChar*)nextData.pData + nextData.nDataSize, pData, nSize);
	nextData.nDataSize += nSize;

	nRetVal = SetNewDataAvailable();
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus MockGenerator::SetNewDataAvailable()
{
	m_bNewDataAvailable = TRUE;
	return m_newDataAvailableEvent.Raise();
}

XnStatus MockGenerator::StartGenerating()
{
	SetGenerating(TRUE);
	return XN_STATUS_OK;
}

XnBool MockGenerator::IsGenerating()
{
	return m_bGenerating;
}

void MockGenerator::StopGenerating()
{
	SetGenerating(FALSE);
}

XnStatus MockGenerator::RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_generatingChangedEvent.Register(handler, pCookie, hCallback);
}

void MockGenerator::UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
{
	m_generatingChangedEvent.Unregister(hCallback);
}

XnStatus MockGenerator::RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_newDataAvailableEvent.Register(handler, pCookie, hCallback);
}

void MockGenerator::UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
{
	m_newDataAvailableEvent.Unregister(hCallback);
}

XnBool MockGenerator::IsNewDataAvailable(XnUInt64& nTimestamp)
{
	if (m_bNewDataAvailable)
	{
		// the timestamp is used for frame sync algorithm. If frameID is zero, this means
		// there is no actual frame, so we don't want the timestamp to be checked for sync.
		// instead, return an "invalid" timestamp. This is allowed, as FrameID 0 is a non-valid
		// frame.
		if (m_data[m_nNextDataIdx].nFrameID == 0)
		{
			nTimestamp = XN_MAX_UINT64;
		}
		else
		{
			nTimestamp = m_data[m_nNextDataIdx].nTimeStamp;
		}
	}

	return m_bNewDataAvailable;
}

XnStatus MockGenerator::UpdateData()
{
	if (m_bNewDataAvailable)
	{
		//Rotate current data and next data
		m_nCurrentDataIdx = 1 - m_nCurrentDataIdx;
		m_nNextDataIdx = 1 - m_nNextDataIdx;
		m_data[m_nNextDataIdx].nDataSize = 0;
		m_bNewDataAvailable = FALSE;
	}
	return XN_STATUS_OK;
}

const void* MockGenerator::GetData()
{
	return m_data[m_nCurrentDataIdx].pData;
}

XnUInt32 MockGenerator::GetDataSize()
{
	return m_data[m_nCurrentDataIdx].nDataSize;
}

XnUInt64 MockGenerator::GetTimestamp()
{
	return m_data[m_nCurrentDataIdx].nTimeStamp;
}

XnUInt32 MockGenerator::GetFrameID()
{
	return m_data[m_nCurrentDataIdx].nFrameID;
}

xn::ModuleMirrorInterface* MockGenerator::GetMirrorInterface()
{
	// the interface always exists, even if capability is not supported (support may change later on)
	return this;
}

xn::ModuleAlternativeViewPointInterface* MockGenerator::GetAlternativeViewPointInterface()
{
	//TODO: Support AlternativeViewPointInterface
	return NULL;
}

xn::ModuleFrameSyncInterface* MockGenerator::GetFrameSyncInterface()
{
	// the interface always exists, even if capability is not supported (support may change later on)
	return this;
}

XnStatus MockGenerator::SetMirror(XnBool bMirror)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (!m_bMirrorCap)
	{
		return XN_STATUS_NOT_IMPLEMENTED;
	}

	if (bMirror != m_bMirror)
	{
		m_bMirror = bMirror;
		nRetVal = m_mirrorChangeEvent.Raise();
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

XnBool MockGenerator::IsMirrored()
{
	return m_bMirror;
}

XnStatus MockGenerator::RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	if (!m_bMirrorCap)
	{
		return XN_STATUS_NOT_IMPLEMENTED;
	}

	return m_mirrorChangeEvent.Register(handler, pCookie, hCallback);
}

void MockGenerator::UnregisterFromMirrorChange(XnCallbackHandle hCallback)
{
	if (m_bMirrorCap)
	{
		m_mirrorChangeEvent.Unregister(hCallback);
	}
}

void MockGenerator::SetGenerating(XnBool bGenerating)
{
	if (bGenerating != m_bGenerating)
	{
		m_bGenerating = bGenerating;
		m_generatingChangedEvent.Raise();
	}
}

XnStatus MockGenerator::OnStateReady()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = MockProductionNode::OnStateReady();
	XN_IS_STATUS_OK(nRetVal);
	
	// allocate current buffer (so the app won't get a NULL pointer)
	XnUInt32 nNeededSize = GetRequiredBufferSize();

	nRetVal = ResizeBuffer(m_nCurrentDataIdx, nNeededSize);
	XN_IS_STATUS_OK(nRetVal);

	xnOSMemSet(m_data[m_nCurrentDataIdx].pData, 0, nNeededSize);

	return (XN_STATUS_OK);
}

XnUInt32 MockGenerator::GetRequiredBufferSize()
{
	return 0;
}

XnStatus MockGenerator::ResizeBuffer(XnUInt32 nIndex, XnUInt32 nNeededSize)
{
	DataInfo& dataInfo = m_data[nIndex];
	if (nNeededSize > dataInfo.nAllocatedSize)
	{
		xnOSFreeAligned(dataInfo.pData);
		dataInfo.pData = xnOSMallocAligned(nNeededSize, XN_DEFAULT_MEM_ALIGN);
		XN_VALIDATE_ALLOC_PTR(dataInfo.pData);
		dataInfo.nAllocatedSize = nNeededSize;
	}
	
	return (XN_STATUS_OK);
}

XnBool MockGenerator::CanFrameSyncWith(xn::ProductionNode& other)
{
	if (!m_bFrameSyncCap)
	{
		return FALSE;
	}

	if (!other.IsValid())
	{
		return XN_STATUS_BAD_PARAM;
	}

	return (strcmp(other.GetName(), m_strFrameSyncWith) == 0);
}

XnStatus MockGenerator::FrameSyncWith(xn::ProductionNode& other)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!other.IsValid())
	{
		return XN_STATUS_BAD_PARAM;
	}

	nRetVal = SetFrameSyncNode(other.GetName());
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus MockGenerator::StopFrameSyncWith(xn::ProductionNode& other)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (!other.IsValid())
	{
		return XN_STATUS_BAD_PARAM;
	}

	if (strcmp(other.GetName(), m_strFrameSyncWith) != 0)
	{
		// not synced right now with this node
		return XN_STATUS_BAD_PARAM;
	}

	nRetVal = SetFrameSyncNode("");
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);	
}

XnBool MockGenerator::IsFrameSyncedWith(xn::ProductionNode& other)
{
	return other.IsValid() && (strcmp(other.GetName(), m_strFrameSyncWith) == 0);
}

XnStatus MockGenerator::RegisterToFrameSyncChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_frameSyncChangeEvent.Register(handler, pCookie, hCallback);
}

void MockGenerator::UnregisterFromFrameSyncChange(XnCallbackHandle hCallback)
{
	m_frameSyncChangeEvent.Unregister(hCallback);
}

XnStatus MockGenerator::SetFrameSyncNode(const XnChar* strOther)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_bFrameSyncCap)
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	// if we haven't registered yet to the events, do it now (we need to know the lifetime of this other node)
	if (m_hNodeCreationCallback == NULL)
	{
		nRetVal = m_context.RegisterToNodeCreation(OnNodeCreationCallback, this, m_hNodeCreationCallback);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (m_hNodeDestructionCallback == NULL)
	{
		nRetVal = m_context.RegisterToNodeDestruction(OnNodeDestructionCallback, this, m_hNodeDestructionCallback);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (strcmp(strOther, m_strFrameSyncWith) != 0)
	{
		nRetVal = xnOSStrCopy(m_strFrameSyncWith, strOther, sizeof(m_strFrameSyncWith));
		XN_IS_STATUS_OK(nRetVal);

		if (strOther[0] != '\0')
		{
			// check if node already exists
			xn::ProductionNode other;
			nRetVal = m_context.GetProductionNodeByName(strOther, other);
			m_bFrameSyncWithExists = (nRetVal == XN_STATUS_OK);
		}

		nRetVal = m_frameSyncChangeEvent.Raise();
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

void MockGenerator::OnNodeCreation(xn::ProductionNode& createdNode)
{
	if (strcmp(createdNode.GetName(), m_strFrameSyncWith) == 0)
	{
		m_bFrameSyncWithExists = TRUE;
		m_frameSyncChangeEvent.Raise();
	}
}

void MockGenerator::OnNodeDestruction(const XnChar* strDestroyedNodeName)
{
	if (strcmp(strDestroyedNodeName, m_strFrameSyncWith) == 0)
	{
		m_bFrameSyncWithExists = FALSE;
		m_frameSyncChangeEvent.Raise();
	}
}

void XN_CALLBACK_TYPE MockGenerator::OnNodeCreationCallback(xn::Context& /*context*/, xn::ProductionNode& createdNode, void* pCookie)
{
	MockGenerator* pThis = (MockGenerator*)pCookie;
	pThis->OnNodeCreation(createdNode);
}

void XN_CALLBACK_TYPE MockGenerator::OnNodeDestructionCallback(xn::Context& /*context*/, const XnChar* strDestroyedNodeName, void* pCookie)
{
	MockGenerator* pThis = (MockGenerator*)pCookie;
	pThis->OnNodeDestruction(strDestroyedNodeName);
}

