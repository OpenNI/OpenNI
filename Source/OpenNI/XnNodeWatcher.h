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

	XnCallbackHandle m_hGenerationRunningChangeCB;
	XnCallbackHandle m_hMirrorChangeCB;

	XnUInt64 m_nLastDataTimeStamp;
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
