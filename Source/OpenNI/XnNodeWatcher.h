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
#ifndef __XN_NODE_WATCHER_H__
#define __XN_NODE_WATCHER_H__

#include <XnCppWrapper.h>
#include <XnTypes.h>

namespace xn
{

class NodeWatcher;

XnStatus CreateNodeWatcher(ProductionNode &node, 
						   XnProductionNodeType type, 
						   void* pCookie, 
						   XnNodeNotifications &notifications, 
						   NodeWatcher*& pNodeWatcher);

class NodeWatcher
{
public:
	NodeWatcher(const ProductionNode& node,
				XnNodeNotifications& notifications, 
				void* pCookie);
	virtual ~NodeWatcher() {}
	virtual XnStatus Register() { return XN_STATUS_OK; }
	virtual void Unregister() {}
	virtual XnStatus NotifyState();
	virtual XnStatus Watch() { return XN_STATUS_OK; }
	virtual XnUInt64 GetTimestamp() { return 0; }

protected:
	virtual XnStatus NotifyStateImpl();
	XnStatus NotifyIntPropChanged(const XnChar* strPropName, XnUInt64 nValue);
	XnStatus NotifyRealPropChanged(const XnChar* strPropName, XnDouble dValue);
	XnStatus NotifyStringPropChanged(const XnChar* strPropName, const XnChar* strValue);
	XnStatus NotifyGeneralPropChanged(const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer);

	XnNodeNotifications& m_notifications;
	void* m_pCookie;
	ProductionNode m_node;

private:
	XN_DISABLE_COPY_AND_ASSIGN(NodeWatcher);
};

class DeviceWatcher : public NodeWatcher
{
public:
	DeviceWatcher(const Device& device,
	              XnNodeNotifications& notifications, 
				  void* pCookie);
};

class GeneratorWatcher : public NodeWatcher
{
public:
	GeneratorWatcher(const Generator &generator,
		             XnNodeNotifications& notifications, 
		             void* pCookie);
	virtual ~GeneratorWatcher();
	virtual XnStatus Register();
	virtual void Unregister();
	virtual XnStatus Watch();
	virtual XnUInt64 GetTimestamp();

protected:
	virtual XnStatus NotifyStateImpl();
	virtual const void* GetCurrentData();

private:
	Generator m_generator;

	static void XN_CALLBACK_TYPE HandleGenerationRunningChange(ProductionNode& node, void* pCookie);
	static void XN_CALLBACK_TYPE HandleMirrorChange(ProductionNode& node, void* pCookie);
	static void XN_CALLBACK_TYPE HandleFrameSyncChange(ProductionNode& node, void* pCookie);

	XnStatus UpdateFrameSync();

	XnCallbackHandle m_hGenerationRunningChangeCB;
	XnCallbackHandle m_hMirrorChangeCB;
	XnCallbackHandle m_hFrameSyncChangeCB;

	XnUInt64 m_nLastDataTimeStamp;
	XnUInt32 m_nLastDataFrameID;
};

class MapWatcher : public GeneratorWatcher
{
public:
	MapWatcher(const MapGenerator &mapGenerator, 
		       XnNodeNotifications& notifications, 
		       void* pCookie);
	virtual ~MapWatcher();
	virtual XnStatus Register();
	virtual void Unregister();
	virtual XnStatus NotifyStateImpl();

private:
	virtual Generator& GetGenerator() { return m_mapGenerator; }
	XnStatus NotifySupportedOutputModes();
	XnStatus NotifyOutputMode();
	XnStatus NotifyCropping();

	static void XN_CALLBACK_TYPE HandleMapOutputModeChange(ProductionNode& node, void* pCookie);
	static void XN_CALLBACK_TYPE HandleCroppingChange(ProductionNode& node, void* pCookie);

	XnCallbackHandle m_hMapOutputModeChangeCB;
	XnCallbackHandle m_hCroppingChangeCB;

	MapGenerator m_mapGenerator;
};

class ImageWatcher : public MapWatcher
{
public:
	ImageWatcher(const ImageGenerator &imageGenerator, 
		XnNodeNotifications& notifications, 
		void* pCookie);
	virtual ~ImageWatcher();
	virtual XnStatus Register();
	virtual void Unregister();
	virtual XnStatus NotifyStateImpl();

private:
	XnStatus NotifySupportedPixelFormats();

	static void XN_CALLBACK_TYPE HandlePixelFormatChange(ProductionNode& node, void* pCookie);
	XnCallbackHandle m_hPixelFormatChangeCB;
	ImageGenerator m_imageGenerator;
};

class IRWatcher : public MapWatcher
{
public:
	IRWatcher(const IRGenerator &irGenerator, 
		XnNodeNotifications& notifications, 
		void* pCookie);

private:
	IRGenerator m_irGenerator;
};

class DepthWatcher : public MapWatcher
{
public:
	DepthWatcher(const DepthGenerator &depthGenerator, 
		XnNodeNotifications& notifications, 
		void* pCookie);
	virtual ~DepthWatcher();
	virtual XnStatus Register();
	virtual void Unregister();
	virtual XnStatus NotifyStateImpl();

private:
	XnStatus NotifyFieldOfView();
	XnStatus NotifyUserPositions();
	static void XN_CALLBACK_TYPE HandleUserPositionChange(ProductionNode& node, void* pCookie);
	static void XN_CALLBACK_TYPE HandleFieldOfViewChange(ProductionNode& node, void* pCookie);

	XnCallbackHandle m_hFieldOfViewChangeCB;
	XnCallbackHandle m_hUserPositionChangeCB;
	DepthGenerator m_depthGenerator;
};

class AudioWatcher : public GeneratorWatcher
{
public:
	AudioWatcher(const AudioGenerator &audioGenerator, 
		XnNodeNotifications& notifications, 
		void* pCookie);
	virtual ~AudioWatcher();
	virtual XnStatus Register();
	virtual void Unregister();
	virtual XnStatus NotifyStateImpl();

private:
	XnStatus NotifySupportedOutputModes();
	XnStatus NotifyOutputMode();

	static void XN_CALLBACK_TYPE HandleWaveOutputModeChange(ProductionNode& node, void* pCookie);

	XnCallbackHandle m_hOutputModeChangeCB;
	AudioGenerator m_audioGenerator;
};

class GestureWatcher : public GeneratorWatcher
{
public:
	GestureWatcher(const GestureGenerator &gestureGenerator, 
		XnNodeNotifications& notifications, 
		void* pCookie);
	virtual ~GestureWatcher();
	virtual XnStatus Register();
	virtual void Unregister();
	virtual XnStatus NotifyStateImpl();

private:
	static void XN_CALLBACK_TYPE HandleGestureRecognized(
		GestureGenerator& generator, 
		const XnChar* strGesture, 
		const XnPoint3D* pIDPosition, 
		const XnPoint3D* pEndPosition, 
		void* pCookie);
	
	static void XN_CALLBACK_TYPE HandleGestureProgress(
		GestureGenerator& generator, 
		const XnChar* strGesture, 
		const XnPoint3D* pPosition, 
		XnFloat fProgress, 
		void* pCookie);
	
	XnCallbackHandle m_hGestureCB;
	GestureGenerator m_gestureGenerator;
};

}

#endif //__XN_NODE_WATCHER_H__
