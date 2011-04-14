/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#include "MockGenerator.h"
#include <XnPropNames.h>

MockGenerator::MockGenerator(const XnChar* strName, XnBool bAggregateData /* = FALSE */) :
	MockProductionNode(strName),
	m_bAggregateData(bAggregateData),
	m_bGenerating(FALSE),
	m_bMirror(FALSE),
	m_nCurrentDataIdx(0),
	m_nNextDataIdx(1),
	m_bNewDataAvailable(FALSE),
	m_bMirrorCap(FALSE)
{
	memset(&m_data, 0, sizeof(m_data));
}

MockGenerator::~MockGenerator()
{
	for (XnUInt32 i = 0; i < NUM_BUFFERS; i++)
	{
		xnOSFreeAligned(m_data[i].pData);
	}
}

XnBool MockGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	if (strcmp(strCapabilityName, XN_CAPABILITY_MIRROR) == 0)
	{
		return m_bMirrorCap;
	}
	//TODO: Support alt view cap, seeking cap, framesync cap
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
	return m_generatingChangedEvent.Register(handler, pCookie, &hCallback);
}

void MockGenerator::UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
{
	m_generatingChangedEvent.Unregister(hCallback);
}

XnStatus MockGenerator::RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_newDataAvailableEvent.Register(handler, pCookie, &hCallback);
}

void MockGenerator::UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
{
	m_newDataAvailableEvent.Unregister(hCallback);
}

XnBool MockGenerator::IsNewDataAvailable(XnUInt64& nTimestamp)
{
	if (m_bNewDataAvailable)
	{
		nTimestamp = m_data[m_nNextDataIdx].nTimeStamp;
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
	return m_bMirrorCap ? this : NULL;
}

xn::ModuleAlternativeViewPointInterface* MockGenerator::GetAlternativeViewPointInterface()
{
	//TODO: Support AlternativeViewPointInterface
	return NULL;
}

xn::ModuleFrameSyncInterface* MockGenerator::GetFrameSyncInterface()
{
	//TODO: Support FrameSyncInterface
	return NULL;
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

	return m_mirrorChangeEvent.Register(handler, pCookie, &hCallback);
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
	XnStatus nRetVal = XN_STATUS_OK;
	
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

