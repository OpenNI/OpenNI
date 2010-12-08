/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/


#include <cstdio>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include "XnRecorderImpl.h"
#include "XnNodeWatcher.h"
#include "XnModuleLoader.h"
#include <XnModuleInterface.h>
#include "XnInternalTypes.h"
#include "XnPropNames.h"
#include <XnCodecIDs.h>

using namespace std;

namespace xn 
{

XnRecorderOutputStreamInterface RecorderImpl::s_fileOutputStream = 
{
	&RecorderImpl::OpenFile,
	&RecorderImpl::WriteFile,
	&RecorderImpl::SeekFile,
	&RecorderImpl::TellFile,
	&RecorderImpl::CloseFile
};

RecorderImpl::RecorderImpl() : 
	m_hRecorder(NULL),
	m_pOutFile(NULL)
{
	xnOSMemSet(m_strFileName, 0, sizeof(m_strFileName));
}

RecorderImpl::~RecorderImpl()
{
	Destroy();
}

XnStatus RecorderImpl::Init(XnNodeHandle hRecorder)
{
	XN_VALIDATE_PTR(hRecorder, XN_STATUS_ERROR);
	XnModuleInstance* pModuleInstance = hRecorder->pModuleInstance;
	XN_VALIDATE_PTR(pModuleInstance, XN_STATUS_ERROR);
	XnModuleNodeHandle hModule = pModuleInstance->hNode;
	XN_VALIDATE_PTR(hModule, XN_STATUS_ERROR);
	XN_VALIDATE_PTR(pModuleInstance->pLoaded, XN_STATUS_ERROR);
	XN_VALIDATE_PTR(pModuleInstance->pLoaded->pInterface, XN_STATUS_ERROR);
	
	m_hRecorder = hRecorder;
	
	return XN_STATUS_OK;
}

void RecorderImpl::BeforeNodeDestroy()
{
	//Do nothing here - we only destroy in the destructor.
}

void RecorderImpl::Destroy()
{
	for (NodeWatchersMap::Iterator it = m_nodeWatchersMap.begin(); it != m_nodeWatchersMap.end(); it++)
	{
		XN_DELETE(it.Value());
	}
	m_nodeWatchersMap.Clear();
	CloseFileImpl();	
}

XnStatus RecorderImpl::AddNode(ProductionNode &node, XnCodecID compression)
{
	if (!node.IsValid())
	{
		return XN_STATUS_BAD_PARAM;		
	}

	if (compression == XN_CODEC_NULL)
	{
		compression = GetDefaultCodecID(node);
	}

	XnProductionNodeType type = node.GetInfo().GetDescription().Type;
	NodeWatcher* pNodeWatcher = NULL;
	XnStatus nRetVal = CreateNodeWatcher(node, type, ModuleHandle(), Notifications(), pNodeWatcher);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = pNodeWatcher->Register();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNodeWatcher);
		return nRetVal;
	}

	nRetVal = NotifyNodeAdded(node, type, compression);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = pNodeWatcher->NotifyState();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNodeWatcher);
		return nRetVal;
	}

	nRetVal = m_nodeWatchersMap.Set(node, pNodeWatcher);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNodeWatcher);
		return nRetVal;
	}
	
	return XN_STATUS_OK;
}

XnStatus RecorderImpl::RemoveNode(ProductionNode &node)
{
	if (!node.IsValid())
	{
		return XN_STATUS_BAD_PARAM;		
	}

	XnStatus nRetVal = NotifyNodeRemoved(node);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = RemoveNodeImpl(node);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}


XnStatus RecorderImpl::AddRawNode(const XnChar* strNodeName)
{
	if (IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Attempted to add a raw node by name of '%s' but there is already a raw node by that name", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_INVALID_OPERATION;
	}

	XnNodeHandle hNode = NULL;
	NodeWatcher* pNodeWatcher = NULL;
	if ((xnGetNodeHandleByName(m_hRecorder->pContext, strNodeName, &hNode) == XN_STATUS_OK) &&
		(m_nodeWatchersMap.Get(hNode, pNodeWatcher) == XN_STATUS_OK))
	{
		//There's a node by that name and we're already watching it
		xnLogWarning(XN_MASK_OPEN_NI, "Attempted to add a raw node by name of '%s' but there is already another node by that name that is being recorded", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_INVALID_OPERATION;
	}

	XnStatus nRetVal = Notifications().OnNodeAdded(ModuleHandle(), 
		strNodeName, (XnProductionNodeType)0, XN_CODEC_UNCOMPRESSED);
	XN_IS_STATUS_OK(nRetVal);

	RawNodeInfo rawNodeInfo;
	nRetVal = m_rawNodesMap.Set(strNodeName, rawNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}


XnStatus RecorderImpl::RemoveRawNode(const XnChar* strNodeName)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to remove non-existing raw node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeRemoved(ModuleHandle(), strNodeName);
}


XnStatus RecorderImpl::SetRawNodeIntProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nVal)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeIntPropChanged(ModuleHandle(), strNodeName, strPropName, nVal);
}

XnStatus RecorderImpl::SetRawNodeRealProp(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dVal)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeRealPropChanged(ModuleHandle(), strNodeName, strPropName, dVal);
}

XnStatus RecorderImpl::SetRawNodeStringProp(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strVal)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeStringPropChanged(ModuleHandle(), strNodeName, strPropName, strVal);
}

XnStatus RecorderImpl::SetRawNodeGeneralProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeGeneralPropChanged(ModuleHandle(), strNodeName, strPropName, nBufferSize, pBuffer);
}

XnStatus RecorderImpl::NotifyRawNodeStateReady(const XnChar* strNodeName)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "There is no node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}
	
	return Notifications().OnNodeStateReady(ModuleHandle(), strNodeName);
}

XnStatus RecorderImpl::SetRawNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "There is no node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeNewData(ModuleHandle(), strNodeName, nTimeStamp, nFrame, pData, nSize);
}

XnStatus RecorderImpl::RemoveNodeImpl(ProductionNode &node)
{
	NodeWatcher* pNodeWatcher = NULL;
	XnStatus nRetVal = m_nodeWatchersMap.Remove(node, pNodeWatcher);
	XN_DELETE(pNodeWatcher);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnNodeNotifications& RecorderImpl::Notifications()
{
	return ((XnRecorderInterfaceContainer*)
		(m_hRecorder->pModuleInstance->pLoaded->pInterface))->nodeNotifications;
}

XnModuleRecorderInterface& RecorderImpl::ModuleRecorder()
{
	return ((XnRecorderInterfaceContainer*)
		m_hRecorder->pModuleInstance->pLoaded->pInterface)->recorder;
}

XnModuleNodeHandle RecorderImpl::ModuleHandle()
{
	return m_hRecorder->pModuleInstance->hNode;
}

XnStatus RecorderImpl::NotifyNodeAdded(XnNodeHandle hNode, XnProductionNodeType type, XnCodecID compression)
{
	return Notifications().OnNodeAdded(ModuleHandle(), xnGetNodeName(hNode), type, compression);
}

XnStatus RecorderImpl::NotifyNodeRemoved(XnNodeHandle hNode)
{
	return Notifications().OnNodeRemoved(ModuleHandle(), xnGetNodeName(hNode));
}

XnStatus RecorderImpl::SetDestination(XnRecordMedium destType, const XnChar* strDest)
{
	XnStatus nRetVal = XN_STATUS_OK;
	switch (destType)
	{
		//Right now only file destination is supported
		case XN_RECORD_MEDIUM_FILE:
		{
			nRetVal = xnOSStrCopy(m_strFileName, strDest, sizeof(m_strFileName));
			XN_IS_STATUS_OK(nRetVal);
			nRetVal = ModuleRecorder().SetOutputStream(ModuleHandle(), this, &s_fileOutputStream);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
		default:
			XN_ASSERT(FALSE);
			return XN_STATUS_BAD_PARAM;
	}

	return XN_STATUS_OK;
}

XnStatus RecorderImpl::Record()
{
	XnStatus nRetVal = XN_STATUS_OK;
	for (NodeWatchersMap::Iterator it = m_nodeWatchersMap.begin(); it != m_nodeWatchersMap.end(); it++)
	{
		nRetVal = it.Value()->Watch();
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

XnStatus RecorderImpl::OpenFile(void* pCookie)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->OpenFileImpl();
}

XnStatus RecorderImpl::WriteFile(void* pCookie, const XnChar* strNodeName, const void* pData, XnUInt32 nSize)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->WriteFileImpl(strNodeName, pData, nSize);
}


XnStatus XN_CALLBACK_TYPE RecorderImpl::SeekFile(void* pCookie, XnOSSeekType seekType, const XnUInt32 nOffset)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->SeekFileImpl(seekType, nOffset);
}

XnUInt32 XN_CALLBACK_TYPE RecorderImpl::TellFile(void* pCookie)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->TellFileImpl();
}

void RecorderImpl::CloseFile(void* pCookie)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	pThis->CloseFileImpl();
}

XnStatus RecorderImpl::OpenFileImpl()
{
	if (m_pOutFile != NULL)
	{
		//Already open
		return XN_STATUS_OK;
	}

	m_pOutFile = fopen(m_strFileName, "wb");
	if (m_pOutFile  == NULL)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to open file '%s' for writing", m_strFileName);
		return XN_STATUS_OS_FILE_OPEN_FAILED;
	}

	return XN_STATUS_OK;	
}

XnStatus RecorderImpl::WriteFileImpl(const XnChar* strNodeName, 
									 const void* pData, 
									 XnUInt32 nSize)
{
	//strNodeName may be NULL
	XN_VALIDATE_PTR(m_pOutFile, XN_STATUS_ERROR);
	size_t nBytesWritten = fwrite(pData, 1, nSize, m_pOutFile);
	if (nBytesWritten < nSize)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Written only %u bytes out of %u to file", nBytesWritten, nSize);
		return XN_STATUS_OS_FILE_WRITE_FAILED;
	}
	return XN_STATUS_OK;
}


XnStatus RecorderImpl::SeekFileImpl(XnOSSeekType seekType, const XnUInt32 nOffset)
{
	XN_VALIDATE_PTR(m_pOutFile, XN_STATUS_ERROR);
	long nOrigin = 0;
	switch (seekType)
	{
	case XN_OS_SEEK_CUR:
		nOrigin = SEEK_CUR;
		break;
	case XN_OS_SEEK_END:
		nOrigin = SEEK_END;
		break;
	case SEEK_SET:
		nOrigin = SEEK_SET;
		break;
	default:
		XN_ASSERT(FALSE);
		return XN_STATUS_BAD_PARAM;
	}

	if (fseek(m_pOutFile, nOffset, nOrigin) != 0)
	{
		return XN_STATUS_ERROR;
	}

	return XN_STATUS_OK;
}


XnUInt32 RecorderImpl::TellFileImpl()
{
	return ftell(m_pOutFile);
}

void RecorderImpl::CloseFileImpl()
{
	if (m_pOutFile != NULL)
	{
		fclose(m_pOutFile);
		m_pOutFile = NULL;
	}
}

XnCodecID RecorderImpl::GetDefaultCodecID(ProductionNode& node)
{
	XN_ASSERT(node.IsValid());
	XnProductionNodeType type = node.GetInfo().GetDescription().Type;

	switch (type)
	{
		case XN_NODE_TYPE_DEPTH:
			return XN_CODEC_16Z_EMB_TABLES;
		case XN_NODE_TYPE_IMAGE:
		{
			ImageGenerator gen(node);
			XnPixelFormat format = gen.GetPixelFormat();
			switch (format)
			{
				case XN_PIXEL_FORMAT_RGB24:
					return XN_CODEC_JPEG;
				case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
					return XN_CODEC_8Z;
				default:
					return XN_CODEC_UNCOMPRESSED;
			}
		}
		default:
			return XN_CODEC_UNCOMPRESSED;
	}
}

XnBool RecorderImpl::IsRawNode(const XnChar* strNodeName)
{
	RawNodeInfo* pRawNodeInfo = NULL;
	return (m_rawNodesMap.Get(strNodeName, pRawNodeInfo) == XN_STATUS_OK);
}

} //namespace xn
