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
#ifndef NI_SIMPLE_VIEWER_H__
#define NI_SIMPLE_VIEWER_H__

#include <XnCppWrapper.h>


enum DisplayModes_e
{
	DISPLAY_MODE_OVERLAY,
	DISPLAY_MODE_DEPTH,
	DISPLAY_MODE_IMAGE
};


class SimpleViewer
{
public:
	// Singleton
	static SimpleViewer& CreateInstance(xn::Context& context);
	static void DestroyInstance(SimpleViewer& instance);

	virtual XnStatus Init(int argc, char **argv);
	virtual XnStatus Run();	//Does not return

protected:
	SimpleViewer(xn::Context& context);
	virtual ~SimpleViewer();

	virtual void Display();
	virtual void DisplayPostDraw(){};	// Overload to draw over the screen image

	virtual void OnKey(unsigned char key, int x, int y);

	virtual XnStatus InitOpenGL(int argc, char **argv);
	void InitOpenGLHooks();

	static SimpleViewer& Instance();

	xn::Context&		m_rContext;
	xn::DepthGenerator	m_depth;
	xn::ImageGenerator	m_image;

	static SimpleViewer*	sm_pInstance;

	void ScalePoint(XnPoint3D& point);
private:
	// GLUT callbacks
	static void glutIdle();
	static void glutDisplay();
	static void glutKeyboard(unsigned char key, int x, int y);

	float*				m_pDepthHist;
	XnRGB24Pixel*		m_pTexMap;
	unsigned int		m_nTexMapX;
	unsigned int		m_nTexMapY;
	DisplayModes_e		m_eViewState;
	xn::DepthMetaData	m_depthMD;
	xn::ImageMetaData	m_imageMD;
};

#endif //NI_SIMPLE_VIEWER_H__