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
#include "XnNodeWatcher.h"
#include "XnPropNames.h"
#include "XnInternalTypes.h"
#include "xnInternalFuncs.h"
#include "XnLog.h"
#include "XnTypeManager.h"

namespace xn
{

XnStatus CreateNodeWatcher(ProductionNode &node, 
						   XnProductionNodeType type, 
						   void* pCookie, 
						   XnNodeNotifications &notifications, 
						   NodeWatcher*& pNodeWatcher)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	pNodeWatcher = NULL;

	const XnBitSet* pHierarchy;
	nRetVal = TypeManager::GetInstance().GetTypeHierarchy(type, pHierarchy);
	XN_IS_STATUS_OK(nRetVal);

	// start with concrete types
	if (pHierarchy->IsSet(XN_NODE_TYPE_DEVICE))
	{
		XN_VALIDATE_NEW(pNodeWatcher, DeviceWatcher, (Device&)node, notifications, pCookie);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_DEPTH))
	{
		XN_VALIDATE_NEW(pNodeWatcher, DepthWatcher, (DepthGenerator&)node, notifications, pCookie);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_IMAGE))
	{
		XN_VALIDATE_NEW(pNodeWatcher, ImageWatcher, (ImageGenerator&)node, notifications, pCookie);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_IR))
	{
		XN_VALIDATE_NEW(pNodeWatcher, IRWatcher, (IRGenerator&)node, notifications, pCookie);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_GESTURE))
	{
		//TODO: Finish GestureWatcher implementation
		return XN_STATUS_NOT_IMPLEMENTED;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_USER))
	{
		//TODO: implement UserWatcher
		return XN_STATUS_NOT_IMPLEMENTED;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_HANDS))
	{
		//TODO: Implement hands watcher
		return XN_STATUS_NOT_IMPLEMENTED;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_SCENE))
	{
		//TODO: Implement scene watcher
		return XN_STATUS_NOT_IMPLEMENTED;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_AUDIO))
	{
		XN_VALIDATE_NEW(pNodeWatcher, AudioWatcher, (AudioGenerator&)node, notifications, pCookie);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_RECORDER))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Cannot Record a Recorder");
		XN_ASSERT(FALSE);
		return XN_STATUS_INVALID_OPERATION;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_PLAYER))
	{
		//TODO: implement PlayerWatcher
		return XN_STATUS_NOT_IMPLEMENTED;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_CODEC))
	{
		//TODO: implement CodecWatcher
		return XN_STATUS_NOT_IMPLEMENTED;
	}

	// and now, some abstract types
	else if (pHierarchy->IsSet(XN_NODE_TYPE_MAP_GENERATOR))
	{
		XN_VALIDATE_NEW(pNodeWatcher, MapWatcher, (MapGenerator&)node, notifications, pCookie);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_GENERATOR))
	{
		XN_VALIDATE_NEW(pNodeWatcher, GeneratorWatcher, (Generator&)node, notifications, pCookie);
	}
	else
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Got unknown type: %d", type);
		XN_ASSERT(FALSE);
		return XN_STATUS_UNKNOWN_GENERATOR_TYPE;
	}

	return XN_STATUS_OK;
}

/********************/
/* NodeWatcher		*/
/********************/
NodeWatcher::NodeWatcher(const ProductionNode& node, 
						 XnNodeNotifications& notifications, 
						 void* pCookie) :
	m_node(node),
	m_notifications(notifications),
	m_pCookie(pCookie)
{}

XnStatus NodeWatcher::NotifyState()
{
	XnStatus nRetVal = NotifyStateImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_notifications.OnNodeStateReady(m_pCookie, m_node.GetName());
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeWatcher::NotifyStateImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnBool bExSerCap = m_node.IsCapabilitySupported(XN_CAPABILITY_EXTENDED_SERIALIZATION);

	nRetVal = NotifyIntPropChanged(XN_CAPABILITY_EXTENDED_SERIALIZATION, bExSerCap);
	XN_IS_STATUS_OK(nRetVal);

	if (bExSerCap)
	{
		nRetVal = xnNotifyExState(m_node.GetHandle(), &m_notifications, m_pCookie);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

XnStatus NodeWatcher::NotifyIntPropChanged(const XnChar* strPropName, XnUInt64 nValue)
{
	return m_notifications.OnNodeIntPropChanged(m_pCookie, m_node.GetName(), strPropName, nValue);
}

XnStatus NodeWatcher::NotifyRealPropChanged(const XnChar* strPropName, XnDouble dValue)
{
	return m_notifications.OnNodeRealPropChanged(m_pCookie, m_node.GetName(), strPropName, dValue);
}

XnStatus NodeWatcher::NotifyStringPropChanged(const XnChar* strPropName, const XnChar* strValue)
{
	return m_notifications.OnNodeStringPropChanged(m_pCookie, m_node.GetName(), strPropName, strValue);
}

XnStatus NodeWatcher::NotifyGeneralPropChanged(const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	return m_notifications.OnNodeGeneralPropChanged(m_pCookie, m_node.GetName(), strPropName, nBufferSize, pBuffer);
}

/********************/
/* NodeWatcher		*/
/********************/
DeviceWatcher::DeviceWatcher(const Device& device, 
						 XnNodeNotifications& notifications, 
						 void* pCookie) :
	NodeWatcher(device, notifications, pCookie)
{}

/********************/
/* GeneratorWatcher */
/********************/
GeneratorWatcher::GeneratorWatcher(const Generator &generator,
								   XnNodeNotifications& notifications, 
								   void* pCookie) : 
	NodeWatcher(generator, notifications, pCookie),
	m_generator(generator),
	m_hGenerationRunningChangeCB(NULL),
	m_hMirrorChangeCB(NULL),
	m_hFrameSyncChangeCB(NULL),
	m_nLastDataTimeStamp(0),
	m_nLastDataFrameID(0)
{
}

GeneratorWatcher::~GeneratorWatcher()
{
	Unregister();
}
	
XnStatus GeneratorWatcher::Register()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = NodeWatcher::Register();
	XN_IS_STATUS_OK(nRetVal);
	
	nRetVal = m_generator.RegisterToGenerationRunningChange(&HandleGenerationRunningChange, this, 
		m_hGenerationRunningChangeCB);
	XN_IS_STATUS_OK(nRetVal);

	if (m_generator.IsCapabilitySupported(XN_CAPABILITY_MIRROR))
	{
		nRetVal = m_generator.GetMirrorCap().RegisterToMirrorChange(&HandleMirrorChange, this, m_hMirrorChangeCB);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (m_generator.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC))
	{
		nRetVal = m_generator.GetFrameSyncCap().RegisterToFrameSyncChange(&HandleFrameSyncChange, this, m_hFrameSyncChangeCB);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (m_generator.IsCapabilitySupported(XN_CAPABILITY_EXTENDED_SERIALIZATION))
	{
		//TODO: Add a call to m_generator.GetExtendedSerializationCap().RegisterExNotifications()
	}

	return XN_STATUS_OK;
}

void GeneratorWatcher::Unregister()
{
	if (m_hGenerationRunningChangeCB != NULL)
	{
		m_generator.UnregisterFromGenerationRunningChange(m_hGenerationRunningChangeCB);
		m_hGenerationRunningChangeCB = NULL;
	}

	if (m_generator.IsCapabilitySupported(XN_CAPABILITY_MIRROR) && (m_hMirrorChangeCB != NULL))
	{
		m_generator.GetMirrorCap().UnregisterFromMirrorChange(m_hMirrorChangeCB);
		m_hMirrorChangeCB = NULL;
	}

	if (m_generator.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC) && (m_hFrameSyncChangeCB != NULL))
	{
		m_generator.GetFrameSyncCap().UnregisterFromFrameSyncChange(m_hFrameSyncChangeCB);
		m_hFrameSyncChangeCB = NULL;
	}

	if (m_generator.IsCapabilitySupported(XN_CAPABILITY_EXTENDED_SERIALIZATION))
	{
		xnUnregisterExNotifications(m_generator.GetHandle());
	}

	NodeWatcher::Unregister();
}

XnStatus GeneratorWatcher::NotifyStateImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = NodeWatcher::NotifyStateImpl();
	XN_IS_STATUS_OK(nRetVal);

	XnBool bMirrorCap = m_generator.IsCapabilitySupported(XN_CAPABILITY_MIRROR);
	XnBool bAltViewCap = m_generator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT);
	XnBool bFrameSyncCap = m_generator.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC);

	nRetVal = NotifyIntPropChanged(XN_CAPABILITY_MIRROR, bMirrorCap);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyIntPropChanged(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT, bAltViewCap);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyIntPropChanged(XN_CAPABILITY_FRAME_SYNC, bFrameSyncCap);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyIntPropChanged(XN_PROP_IS_GENERATING, m_generator.IsGenerating());
	XN_IS_STATUS_OK(nRetVal);

	if (bMirrorCap)
	{
		nRetVal = NotifyIntPropChanged(XN_PROP_MIRROR, m_generator.GetMirrorCap().IsMirrored());
		XN_IS_STATUS_OK(nRetVal);
	}

	if (bFrameSyncCap)
	{
		nRetVal = UpdateFrameSync();
		XN_IS_STATUS_OK(nRetVal);
	}

	//TODO: Notify alt view cap props
	
	return XN_STATUS_OK;
}

const void* GeneratorWatcher::GetCurrentData()
{
	return m_generator.GetData();
}

XnStatus GeneratorWatcher::Watch()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = NodeWatcher::Watch();
	XN_IS_STATUS_OK(nRetVal);

	XnUInt64 nCurrentTimeStamp = m_generator.GetTimestamp();
	XnUInt32 nCurrentFrameID = m_generator.GetFrameID();

	// check if timestamp has changed since last time.
	// Note that the first frame might have a timestamp of zero, so also make sure frame ID changes
	if ((nCurrentTimeStamp > m_nLastDataTimeStamp) || 
		(nCurrentFrameID > m_nLastDataFrameID))
	{
		m_nLastDataTimeStamp = nCurrentTimeStamp;
		m_nLastDataFrameID = nCurrentFrameID;

		const void* pData = GetCurrentData();
		if (pData != NULL)
		{
			nRetVal = m_notifications.OnNodeNewData(m_pCookie, 
				m_generator.GetName(), 
				nCurrentTimeStamp, 
				m_generator.GetFrameID(),
				pData,
				m_generator.GetDataSize());
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	return XN_STATUS_OK;
}

XnUInt64 GeneratorWatcher::GetTimestamp()
{
	return m_generator.GetTimestamp();
}

void XN_CALLBACK_TYPE GeneratorWatcher::HandleGenerationRunningChange(ProductionNode& /*node*/, void* pCookie)
{
	GeneratorWatcher *pThis = (GeneratorWatcher*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}

	pThis->NotifyIntPropChanged(XN_PROP_IS_GENERATING, pThis->m_generator.IsGenerating());
}

void XN_CALLBACK_TYPE GeneratorWatcher::HandleMirrorChange(ProductionNode& /*node*/, void* pCookie)
{
	GeneratorWatcher *pThis = (GeneratorWatcher*)pCookie;
	if ((pThis == NULL) || !pThis->m_generator.IsCapabilitySupported(XN_CAPABILITY_MIRROR))
	{
		XN_ASSERT(FALSE);
		return;
	}

	pThis->NotifyIntPropChanged(XN_PROP_MIRROR, pThis->m_generator.GetMirrorCap().IsMirrored());
}

void XN_CALLBACK_TYPE GeneratorWatcher::HandleFrameSyncChange(ProductionNode& /*node*/, void* pCookie)
{
	GeneratorWatcher *pThis = (GeneratorWatcher*)pCookie;
	if ((pThis == NULL) || !pThis->m_generator.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC))
	{
		XN_ASSERT(FALSE);
		return;
	}

	pThis->UpdateFrameSync();
}

XnStatus GeneratorWatcher::UpdateFrameSync()
{
	// go over all nodes, and find the frame synced one
	Context context;
	m_generator.GetContext(context);
	
	NodeInfoList nodes;
	XnStatus nRetVal = context.EnumerateExistingNodes(nodes);
	XN_IS_STATUS_OK(nRetVal);

	for (NodeInfoList::Iterator it = nodes.Begin(); it != nodes.End(); ++it)
	{
		NodeInfo info = *it;
		// make sure this is a generator
		if (xnIsTypeDerivedFrom(info.GetDescription().Type, XN_NODE_TYPE_GENERATOR))
		{
			Generator otherGen;
			nRetVal = info.GetInstance(otherGen);
			XN_IS_STATUS_OK(nRetVal);

			if (m_generator.GetFrameSyncCap().IsFrameSyncedWith(otherGen))
			{
				nRetVal = NotifyStringPropChanged(XN_PROP_FRAME_SYNCED_WITH, otherGen.GetName());
				XN_IS_STATUS_OK(nRetVal);

				return XN_STATUS_OK;
			}
		}
	}

	// if we got here, we're not frame synced
	nRetVal = NotifyStringPropChanged(XN_PROP_FRAME_SYNCED_WITH, "");
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}


/**************/
/* MapWatcher */
/**************/
MapWatcher::MapWatcher(const MapGenerator &mapGenerator, 
					   XnNodeNotifications& notifications, 
					   void* pCookie) : 
	m_mapGenerator(mapGenerator),
	GeneratorWatcher(mapGenerator, notifications, pCookie),
	m_hMapOutputModeChangeCB(NULL),
	m_hCroppingChangeCB(NULL)
{
}

MapWatcher::~MapWatcher()
{
	Unregister();
}

XnStatus MapWatcher::Register()
{
	XnStatus nRetVal = GeneratorWatcher::Register();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_mapGenerator.RegisterToMapOutputModeChange(HandleMapOutputModeChange, this, m_hMapOutputModeChangeCB);
	XN_IS_STATUS_OK(nRetVal);
	if (m_mapGenerator.IsCapabilitySupported(XN_CAPABILITY_CROPPING))
	{
		nRetVal = m_mapGenerator.GetCroppingCap().RegisterToCroppingChange(HandleCroppingChange, this, m_hCroppingChangeCB);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

void MapWatcher::Unregister()
{
	XnBool bCroppingCap = m_mapGenerator.IsCapabilitySupported(XN_CAPABILITY_CROPPING);

	if (m_hMapOutputModeChangeCB != NULL)
	{
		m_mapGenerator.UnregisterFromMapOutputModeChange(m_hMapOutputModeChangeCB);
		m_hMapOutputModeChangeCB = NULL;
	}

	if (bCroppingCap && (m_hCroppingChangeCB != NULL))
	{
		m_mapGenerator.GetCroppingCap().UnregisterFromCroppingChange(m_hCroppingChangeCB);
		m_hCroppingChangeCB = NULL;
	}
	GeneratorWatcher::Unregister();
}

XnStatus MapWatcher::NotifyStateImpl()
{
	XnStatus nRetVal = GeneratorWatcher::NotifyStateImpl();
	XN_IS_STATUS_OK(nRetVal);
	XnBool bCapCropping = m_mapGenerator.IsCapabilitySupported(XN_CAPABILITY_CROPPING);
	nRetVal = NotifyIntPropChanged(XN_CAPABILITY_CROPPING, bCapCropping);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyIntPropChanged(XN_PROP_BYTES_PER_PIXEL, m_mapGenerator.GetBytesPerPixel());
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifySupportedOutputModes();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyOutputMode();
	XN_IS_STATUS_OK(nRetVal);

	if (bCapCropping)
	{
		nRetVal = NotifyCropping();
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return XN_STATUS_OK;
}

XnStatus MapWatcher::NotifySupportedOutputModes()
{
	XnUInt32 nOutputModes = m_mapGenerator.GetSupportedMapOutputModesCount();
	if (nOutputModes == 0)
	{
		return XN_STATUS_ERROR;
	}

	XnStatus nRetVal = NotifyIntPropChanged(XN_PROP_SUPPORTED_MAP_OUTPUT_MODES_COUNT, nOutputModes);
	XN_IS_STATUS_OK(nRetVal);
	XnMapOutputMode *pOutputModes = XN_NEW_ARR(XnMapOutputMode, nOutputModes);
	XN_VALIDATE_ALLOC_PTR(pOutputModes);
	nRetVal = m_mapGenerator.GetSupportedMapOutputModes(pOutputModes, nOutputModes);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE_ARR(pOutputModes);
		return nRetVal;
	}
	nRetVal = NotifyGeneralPropChanged(XN_PROP_SUPPORTED_MAP_OUTPUT_MODES, sizeof(pOutputModes[0]) * nOutputModes, pOutputModes);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE_ARR(pOutputModes);
		return nRetVal;
	}

	XN_DELETE_ARR(pOutputModes);
	return XN_STATUS_OK;	
}

XnStatus MapWatcher::NotifyOutputMode()
{
	XnMapOutputMode outputMode;
	XnStatus nRetVal = m_mapGenerator.GetMapOutputMode(outputMode);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_LOG_ERROR_RETURN(nRetVal, XN_MASK_OPEN_NI, "Failed to get map output mode: %s", xnGetStatusString(nRetVal));
	}

	nRetVal = NotifyGeneralPropChanged(XN_PROP_MAP_OUTPUT_MODE, sizeof(outputMode), &outputMode);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus MapWatcher::NotifyCropping()
{
	XnCropping cropping;
	XnStatus nRetVal = m_mapGenerator.GetCroppingCap().GetCropping(cropping);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyGeneralPropChanged(XN_PROP_CROPPING, sizeof(cropping), &cropping);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE MapWatcher::HandleMapOutputModeChange(ProductionNode& /*node*/, void* pCookie)
{
	MapWatcher *pThis = (MapWatcher*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	XnStatus nRetVal = pThis->NotifyOutputMode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to notify output mode: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}
}

void XN_CALLBACK_TYPE MapWatcher::HandleCroppingChange(ProductionNode& /*node*/, void* pCookie)
{
	MapWatcher *pThis = (MapWatcher*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	XnStatus nRetVal = pThis->NotifyCropping();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to notify output mode: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}
}

/****************/
/* ImageWatcher */
/****************/
ImageWatcher::ImageWatcher(const ImageGenerator &imageGenerator, 
						   XnNodeNotifications& notifications, 
						   void* pCookie) : 
	m_imageGenerator(imageGenerator),
	MapWatcher(imageGenerator, notifications, pCookie),
	m_hPixelFormatChangeCB(NULL)
{
}

ImageWatcher::~ImageWatcher()
{
	Unregister();
}

XnStatus ImageWatcher::Register()
{
	XnStatus nRetVal = MapWatcher::Register();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_imageGenerator.RegisterToPixelFormatChange(HandlePixelFormatChange, this, m_hPixelFormatChangeCB);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

void ImageWatcher::Unregister()
{
	if (m_hPixelFormatChangeCB != NULL)
	{
		m_imageGenerator.UnregisterFromPixelFormatChange(m_hPixelFormatChangeCB);
		m_hPixelFormatChangeCB = NULL;
	}
	MapWatcher::Unregister();
}

XnStatus ImageWatcher::NotifyStateImpl()
{
	XnStatus nRetVal = MapWatcher::NotifyStateImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifySupportedPixelFormats();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyIntPropChanged(XN_PROP_PIXEL_FORMAT, m_imageGenerator.GetPixelFormat());
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus ImageWatcher::NotifySupportedPixelFormats()
{
	XnSupportedPixelFormats supportedPixelFormats;
	xnOSMemSet(&supportedPixelFormats, 0, sizeof(supportedPixelFormats));
	supportedPixelFormats.m_bRGB24 = m_imageGenerator.IsPixelFormatSupported(XN_PIXEL_FORMAT_RGB24);
	supportedPixelFormats.m_bYUV422 = m_imageGenerator.IsPixelFormatSupported(XN_PIXEL_FORMAT_YUV422);
	supportedPixelFormats.m_bGrayscale8Bit = m_imageGenerator.IsPixelFormatSupported(XN_PIXEL_FORMAT_GRAYSCALE_8_BIT);
	supportedPixelFormats.m_bGrayscale16Bit = m_imageGenerator.IsPixelFormatSupported(XN_PIXEL_FORMAT_GRAYSCALE_16_BIT);
	supportedPixelFormats.m_bMJPEG = m_imageGenerator.IsPixelFormatSupported(XN_PIXEL_FORMAT_MJPEG);

	XnStatus nRetVal = NotifyGeneralPropChanged(XN_PROP_SUPPORTED_PIXEL_FORMATS, sizeof(supportedPixelFormats), &supportedPixelFormats);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE ImageWatcher::HandlePixelFormatChange(ProductionNode& /*node*/, void* pCookie)
{
	ImageWatcher *pThis = (ImageWatcher*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	pThis->NotifyIntPropChanged(XN_PROP_PIXEL_FORMAT, pThis->m_imageGenerator.GetPixelFormat());
}

/*************/
/* IRWatcher */
/*************/
IRWatcher::IRWatcher(const IRGenerator &irGenerator, 
					 XnNodeNotifications& notifications, 
					 void* pCookie) :
	m_irGenerator(irGenerator),
	MapWatcher(irGenerator, notifications, pCookie)
{
}

/****************/
/* DepthWatcher */
/****************/
DepthWatcher::DepthWatcher(const DepthGenerator &depthGenerator, 
						   XnNodeNotifications& notifications, 
						   void* pCookie) : 
	m_depthGenerator(depthGenerator),
	MapWatcher(depthGenerator, notifications, pCookie),
	m_hFieldOfViewChangeCB(NULL),
	m_hUserPositionChangeCB(NULL)
{
}

DepthWatcher::~DepthWatcher()
{
	Unregister();
}

XnStatus DepthWatcher::Register()
{
	XnStatus nRetVal = MapWatcher::Register();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_depthGenerator.RegisterToFieldOfViewChange(HandleFieldOfViewChange, this, m_hFieldOfViewChangeCB);
	XN_IS_STATUS_OK(nRetVal);

	if (m_depthGenerator.IsCapabilitySupported(XN_CAPABILITY_USER_POSITION))
	{
		//nRetVal = pUserPosCap->RegisterToUserPositionChange(HandleUserPositionChange, this, m_hUserPositionChangeCB);
		//XN_IS_STATUS_OK(nRetVal);
		//TODO: Uncomment when this gets fixed
	}
	return XN_STATUS_OK;
}

void DepthWatcher::Unregister()
{
	if (m_hFieldOfViewChangeCB != NULL)
	{
		m_depthGenerator.UnregisterFromFieldOfViewChange(m_hFieldOfViewChangeCB);
		m_hFieldOfViewChangeCB = NULL;
	}

	if (m_depthGenerator.IsCapabilitySupported(XN_CAPABILITY_USER_POSITION) && (m_hUserPositionChangeCB != NULL))
	{
		m_depthGenerator.GetUserPositionCap().UnregisterFromUserPositionChange(m_hUserPositionChangeCB);
		m_hUserPositionChangeCB = NULL;
	}
	
	MapWatcher::Unregister();	
}

XnStatus DepthWatcher::NotifyStateImpl()
{
	XnStatus nRetVal = MapWatcher::NotifyStateImpl();
	XN_IS_STATUS_OK(nRetVal);

	// Max depth
	nRetVal = NotifyIntPropChanged(XN_PROP_DEVICE_MAX_DEPTH, m_depthGenerator.GetDeviceMaxDepth());
	XN_IS_STATUS_OK(nRetVal);

	// FOV
	nRetVal = NotifyFieldOfView();
	XN_IS_STATUS_OK(nRetVal);

	// User position
	XnBool bUserPositionsCap = m_depthGenerator.IsCapabilitySupported(XN_CAPABILITY_USER_POSITION);
	nRetVal = NotifyIntPropChanged(XN_CAPABILITY_USER_POSITION, bUserPositionsCap);
	XN_IS_STATUS_OK(nRetVal);

	if (bUserPositionsCap)
	{
		nRetVal = NotifyUserPositions();
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

XnStatus DepthWatcher::NotifyFieldOfView()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnFieldOfView FOV;
	nRetVal = m_depthGenerator.GetFieldOfView(FOV);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = NotifyGeneralPropChanged(XN_PROP_FIELD_OF_VIEW, sizeof(FOV), &FOV);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus DepthWatcher::NotifyUserPositions()
{
	XnUInt32 nUserPositions = m_depthGenerator.GetUserPositionCap().GetSupportedUserPositionsCount();
	if (nUserPositions == 0)
	{
		return XN_STATUS_NOT_IMPLEMENTED;
	}

	XnStatus nRetVal = NotifyIntPropChanged(XN_PROP_SUPPORTED_USER_POSITIONS_COUNT, nUserPositions);
	XN_IS_STATUS_OK(nRetVal);
	XnBoundingBox3D *pPositions = XN_NEW_ARR(XnBoundingBox3D, nUserPositions);
	XN_VALIDATE_ALLOC_PTR(pPositions);

	XnUInt32 i;
	for (i = 0; i < nUserPositions; i++)
	{
		nRetVal = m_depthGenerator.GetUserPositionCap().GetUserPosition(i, pPositions[i]);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE_ARR(pPositions);
			return nRetVal;
		}
	}

	nRetVal = NotifyGeneralPropChanged(XN_PROP_USER_POSITIONS, sizeof(pPositions[0]) * nUserPositions, pPositions);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE_ARR(pPositions);
		return nRetVal;
	}
	
	XN_DELETE_ARR(pPositions);
	return XN_STATUS_OK;	
}

void XN_CALLBACK_TYPE DepthWatcher::HandleFieldOfViewChange(ProductionNode& /*node*/, void* pCookie)
{
	DepthWatcher *pThis = (DepthWatcher*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}

	XnStatus nRetVal = pThis->NotifyFieldOfView();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to notify field of view: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}
}

void XN_CALLBACK_TYPE DepthWatcher::HandleUserPositionChange(ProductionNode& /*node*/, void* pCookie)
{
	DepthWatcher *pThis = (DepthWatcher*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}

	XnStatus nRetVal = pThis->NotifyUserPositions();
	if ((nRetVal != XN_STATUS_OK) && (nRetVal != XN_STATUS_NOT_IMPLEMENTED))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to notify user positions: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}
}

/****************/
/* AudioWatcher */
/****************/
AudioWatcher::AudioWatcher(const AudioGenerator &audioGenerator, 
						   XnNodeNotifications& notifications, 
						   void* pCookie) : 
	m_audioGenerator(audioGenerator),
	GeneratorWatcher(audioGenerator, notifications, pCookie),
	m_hOutputModeChangeCB(NULL)
{	
}

AudioWatcher::~AudioWatcher()
{
	Unregister();
}

XnStatus AudioWatcher::Register()
{
	XnStatus nRetVal = GeneratorWatcher::Register();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_audioGenerator.RegisterToWaveOutputModeChanges(HandleWaveOutputModeChange, this, m_hOutputModeChangeCB);
	XN_IS_STATUS_OK(nRetVal);
	
	return XN_STATUS_OK;
}

void AudioWatcher::Unregister()
{
	m_audioGenerator.UnregisterFromWaveOutputModeChanges(m_hOutputModeChangeCB);
	m_hOutputModeChangeCB = NULL;
	GeneratorWatcher::Unregister();
}

XnStatus AudioWatcher::NotifyStateImpl()
{
	XnStatus nRetVal = GeneratorWatcher::NotifyStateImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifySupportedOutputModes();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyOutputMode();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus AudioWatcher::NotifyOutputMode()
{
	XnWaveOutputMode outputMode;
	XnStatus nRetVal = m_audioGenerator.GetWaveOutputMode(outputMode);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NotifyGeneralPropChanged(XN_PROP_WAVE_OUTPUT_MODE, sizeof(outputMode), &outputMode);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus AudioWatcher::NotifySupportedOutputModes()
{
	XnUInt32 nModes = m_audioGenerator.GetSupportedWaveOutputModesCount();
	if (nModes == 0)
	{
		return XN_STATUS_ERROR;
	}

	XnStatus nRetVal = NotifyIntPropChanged(XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES_COUNT, nModes);
	XN_IS_STATUS_OK(nRetVal);
	XnWaveOutputMode *pSupportedModes = XN_NEW_ARR(XnWaveOutputMode, nModes);
	XN_VALIDATE_ALLOC_PTR(pSupportedModes);
	nRetVal = m_audioGenerator.GetSupportedWaveOutputModes(pSupportedModes, nModes);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE_ARR(pSupportedModes);
		return nRetVal;
	}

	nRetVal = NotifyGeneralPropChanged(XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES, nModes * sizeof(XnWaveOutputMode), pSupportedModes);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE_ARR(pSupportedModes);
		return nRetVal;
	}
	XN_DELETE_ARR(pSupportedModes);
	return XN_STATUS_OK;	
}

void XN_CALLBACK_TYPE AudioWatcher::HandleWaveOutputModeChange(ProductionNode& /*node*/, void* pCookie)
{
	AudioWatcher *pThis = (AudioWatcher*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	pThis->NotifyOutputMode();
}

/******************/
/* GestureWatcher */
/******************/
GestureWatcher::GestureWatcher(const GestureGenerator &gestureGenerator, 
							   XnNodeNotifications& notifications, 
							   void* pCookie) : 
	m_gestureGenerator(gestureGenerator),
	GeneratorWatcher(gestureGenerator, notifications, pCookie),
	m_hGestureCB(NULL)
{
}

GestureWatcher::~GestureWatcher()
{
	Unregister();
}

XnStatus GestureWatcher::Register()
{
	XnStatus nRetVal = GeneratorWatcher::Register();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_gestureGenerator.RegisterGestureCallbacks(HandleGestureRecognized, HandleGestureProgress, this, m_hGestureCB);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

void GestureWatcher::Unregister()
{
	m_gestureGenerator.UnregisterGestureCallbacks(m_hGestureCB);
	m_hGestureCB = NULL;
	GeneratorWatcher::Unregister();
}

XnStatus GestureWatcher::NotifyStateImpl()
{
	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE GestureWatcher::HandleGestureRecognized(GestureGenerator& /*generator*/, 
															  const XnChar* strGesture, 
														  	  const XnPoint3D* pIDPosition, 
															  const XnPoint3D* pEndPosition, 
															  void* pCookie)
{
	GestureWatcher *pThis = (GestureWatcher*)pCookie;
	if ((pThis == NULL) || (strGesture == NULL) || (pIDPosition == NULL) || (pEndPosition == NULL))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Got NULL parameter");
		XN_ASSERT(FALSE);
		return;
	}

	XnGestureRecognizedParams gestureRecognizedParams(strGesture, *pIDPosition, *pEndPosition);
	pThis->NotifyGeneralPropChanged(XN_PROP_GESTURE_RECOGNIZED, 
		sizeof(gestureRecognizedParams), &gestureRecognizedParams);
}

void XN_CALLBACK_TYPE GestureWatcher::HandleGestureProgress(GestureGenerator& /*generator*/, 
														    const XnChar* strGesture, 
														    const XnPoint3D* pPosition, 
														    XnFloat fProgress, 
														    void* pCookie)
{
	GestureWatcher *pThis = (GestureWatcher*)pCookie;

	if ((pThis == NULL) || (strGesture == NULL) || (pPosition == NULL))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Got NULL parameter");
		XN_ASSERT(FALSE);
		return;
	}

	XnGestureProgressParams gestureProgressParams(strGesture, *pPosition, fProgress);
	pThis->NotifyGeneralPropChanged(XN_PROP_GESTURE_PROGRESS, 
		sizeof(gestureProgressParams), &gestureProgressParams);
}

}
