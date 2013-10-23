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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "NiSimpleViewer.h"
#include <XnOS.h>
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <cassert>


using namespace xn;


//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define GL_WIN_SIZE_X	1280
#define GL_WIN_SIZE_Y	1024
#define TEXTURE_SIZE	512

#define DEFAULT_DISPLAY_MODE	DISPLAY_MODE_DEPTH

#define MIN_NUM_CHUNKS(data_size, chunk_size)	((((data_size)-1) / (chunk_size) + 1))
#define MIN_CHUNKS_SIZE(data_size, chunk_size)	(MIN_NUM_CHUNKS(data_size, chunk_size) * (chunk_size))


//---------------------------------------------------------------------------
// Statics
//---------------------------------------------------------------------------
SimpleViewer* SimpleViewer::sm_pInstance = NULL;


//---------------------------------------------------------------------------
// GLUT Hooks
//---------------------------------------------------------------------------
void SimpleViewer::glutIdle (void)
{
	// Display the frame
	glutPostRedisplay();
}

void SimpleViewer::glutDisplay (void)
{
	Instance().Display();
}

void SimpleViewer::glutKeyboard (unsigned char key, int x, int y)
{
	Instance().OnKey(key, x, y);
}


//---------------------------------------------------------------------------
// Method Definitions
//---------------------------------------------------------------------------
SimpleViewer::SimpleViewer(xn::Context& context)
	:m_pTexMap(NULL),
	m_nTexMapX(0),
	m_nTexMapY(0),
	m_eViewState(DEFAULT_DISPLAY_MODE),
	m_rContext(context)
{}

SimpleViewer::~SimpleViewer()
{
	delete[] m_pTexMap;
	delete[] m_pDepthHist;
}

SimpleViewer& SimpleViewer::CreateInstance( xn::Context& context )
{
	assert(!sm_pInstance);
	return *(sm_pInstance = new SimpleViewer(context));
}

void SimpleViewer::DestroyInstance(SimpleViewer& instance)
{
	assert(sm_pInstance);
	assert(sm_pInstance == &instance);
	XN_REFERENCE_VARIABLE(instance);
	delete sm_pInstance;
	sm_pInstance = NULL;
}

SimpleViewer& SimpleViewer::Instance()
{
	assert(sm_pInstance);
	return *sm_pInstance;
}

XnStatus SimpleViewer::Init(int argc, char **argv)
{
	XnStatus	rc;

	rc = m_rContext.FindExistingNode(XN_NODE_TYPE_DEPTH, m_depth);
	if (rc != XN_STATUS_OK)
	{
		printf("No depth node exists! Check your XML.");
		return rc;
	}

	rc = m_rContext.FindExistingNode(XN_NODE_TYPE_IMAGE, m_image);
	if (rc != XN_STATUS_OK)
	{
		printf("No image node exists! Check your XML.");
		return rc;
	}

	m_depth.GetMetaData(m_depthMD);
	m_image.GetMetaData(m_imageMD);

	// Hybrid mode isn't supported in this sample
	if (m_imageMD.FullXRes() != m_depthMD.FullXRes() || m_imageMD.FullYRes() != m_depthMD.FullYRes())
	{
		printf ("The device depth and image resolution must be equal!\n");
		return 1;
	}

	// RGB is the only image format supported.
	if (m_imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
	{
		printf("The device image format must be RGB24\n");
		return 1;
	}

	// Texture map init
	m_nTexMapX = MIN_CHUNKS_SIZE(m_depthMD.FullXRes(), TEXTURE_SIZE);
	m_nTexMapY = MIN_CHUNKS_SIZE(m_depthMD.FullYRes(), TEXTURE_SIZE);
	m_pTexMap = new XnRGB24Pixel[m_nTexMapX * m_nTexMapY];

	m_pDepthHist = new float[m_depth.GetDeviceMaxDepth() + 1];

	return InitOpenGL(argc, argv);
}

XnStatus SimpleViewer::Run()
{
	// Per frame code is in Display
	glutMainLoop();	// Does not return!

	return XN_STATUS_OK;
}

XnStatus SimpleViewer::InitOpenGL(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("OpenNI Hand Tracker");
// 	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	InitOpenGLHooks();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	return XN_STATUS_OK;
}

void SimpleViewer::InitOpenGLHooks()
{
	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
}

void SimpleViewer::Display()
{
	XnStatus		rc = XN_STATUS_OK;

	// Read a new frame
	rc = m_rContext.WaitAnyUpdateAll();
	if (rc != XN_STATUS_OK)
	{
		printf("Read failed: %s\n", xnGetStatusString(rc));
		return;
	}

	m_depth.GetMetaData(m_depthMD);
	m_image.GetMetaData(m_imageMD);

	const XnDepthPixel* pDepth = m_depthMD.Data();

	// Copied from SimpleViewer
	// Clear the OpenGL buffers
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, GL_WIN_SIZE_X, GL_WIN_SIZE_Y, 0, -1.0, 1.0);

	// Calculate the accumulative histogram (the yellow display...)
	xnOSMemSet(m_pDepthHist, 0, m_depthMD.ZRes()*sizeof(float));

	unsigned int nNumberOfPoints = 0;
	for (XnUInt y = 0; y < m_depthMD.YRes(); ++y)
	{
		for (XnUInt x = 0; x < m_depthMD.XRes(); ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				m_pDepthHist[*pDepth]++;
				nNumberOfPoints++;
			}
		}
	}
	for (int nIndex=1; nIndex<m_depthMD.ZRes(); nIndex++)
	{
		m_pDepthHist[nIndex] += m_pDepthHist[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<m_depthMD.ZRes(); nIndex++)
		{
			m_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (m_pDepthHist[nIndex] / nNumberOfPoints)));
		}
	}

	xnOSMemSet(m_pTexMap, 0, m_nTexMapX*m_nTexMapY*sizeof(XnRGB24Pixel));

	// check if we need to draw image frame to texture
	if (m_eViewState == DISPLAY_MODE_OVERLAY ||
		m_eViewState == DISPLAY_MODE_IMAGE)
	{
		const XnRGB24Pixel* pImageRow = m_imageMD.RGB24Data();
		XnRGB24Pixel* pTexRow = m_pTexMap + m_imageMD.YOffset() * m_nTexMapX;

		for (XnUInt y = 0; y < m_imageMD.YRes(); ++y)
		{
			const XnRGB24Pixel* pImage = pImageRow;
			XnRGB24Pixel* pTex = pTexRow + m_imageMD.XOffset();

			for (XnUInt x = 0; x < m_imageMD.XRes(); ++x, ++pImage, ++pTex)
			{
				*pTex = *pImage;
			}

			pImageRow += m_imageMD.XRes();
			pTexRow += m_nTexMapX;
		}
	}

	// check if we need to draw depth frame to texture
	if (m_eViewState == DISPLAY_MODE_OVERLAY ||
		m_eViewState == DISPLAY_MODE_DEPTH)
	{
		const XnDepthPixel* pDepthRow = m_depthMD.Data();
		XnRGB24Pixel* pTexRow = m_pTexMap + m_depthMD.YOffset() * m_nTexMapX;

		for (XnUInt y = 0; y < m_depthMD.YRes(); ++y)
		{
			const XnDepthPixel* pDepth = pDepthRow;
			XnRGB24Pixel* pTex = pTexRow + m_depthMD.XOffset();

			for (XnUInt x = 0; x < m_depthMD.XRes(); ++x, ++pDepth, ++pTex)
			{
				if (*pDepth != 0)
				{
					int nHistValue = m_pDepthHist[*pDepth];
					pTex->nRed = nHistValue;
					pTex->nGreen = nHistValue;
					pTex->nBlue = nHistValue;
				}
			}

			pDepthRow += m_depthMD.XRes();
			pTexRow += m_nTexMapX;
		}
	}

	// Create the OpenGL texture map
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTexMapX, m_nTexMapY, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pTexMap);


	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	int nXRes = m_depthMD.FullXRes();
	int nYRes = m_depthMD.FullYRes();

	
	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	// upper right
	glTexCoord2f((float)nXRes/(float)m_nTexMapX, 0);
	glVertex2f(GL_WIN_SIZE_X, 0);
	// bottom right
	glTexCoord2f((float)nXRes/(float)m_nTexMapX, (float)nYRes/(float)m_nTexMapY);
	glVertex2f(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	// bottom left
	glTexCoord2f(0, (float)nYRes/(float)m_nTexMapY);
	glVertex2f(0, GL_WIN_SIZE_Y);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// Subclass draw hook
	DisplayPostDraw();

	// Swap the OpenGL display buffers
	glutSwapBuffers();
}

void SimpleViewer::OnKey(unsigned char key, int /*x*/, int /*y*/)
{
	switch (key)
	{
	case 27:
		exit (1);
	case '1':
		m_eViewState = DISPLAY_MODE_OVERLAY;
		m_depth.GetAlternativeViewPointCap().SetViewPoint(m_image);
		break;
	case '2':
		m_eViewState = DISPLAY_MODE_DEPTH;
		m_depth.GetAlternativeViewPointCap().ResetViewPoint();
		break;
	case '3':
		m_eViewState = DISPLAY_MODE_IMAGE;
		m_depth.GetAlternativeViewPointCap().ResetViewPoint();
		break;
	case 'm':
		m_rContext.SetGlobalMirror(!m_rContext.GetGlobalMirror());
		break;
	}
}

void SimpleViewer::ScalePoint(XnPoint3D& point)
{
	point.X *= GL_WIN_SIZE_X;
	point.X /= m_depthMD.XRes();

	point.Y *= GL_WIN_SIZE_Y;
	point.Y /= m_depthMD.YRes();
}
