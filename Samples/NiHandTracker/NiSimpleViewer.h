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