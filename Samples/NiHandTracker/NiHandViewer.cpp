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
#include "NiHandViewer.h"
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define LENGTHOF(arr)			(sizeof(arr)/sizeof(arr[0]))


//---------------------------------------------------------------------------
// Method Definitions
//---------------------------------------------------------------------------
SimpleViewer& HandViewer::CreateInstance( xn::Context& context )
{
	assert(!sm_pInstance);
	return *(sm_pInstance = new HandViewer(context));
}

HandViewer::HandViewer(xn::Context& context)
:SimpleViewer(context),
m_HandTracker(context)
{}

XnStatus HandViewer::Init(int argc, char **argv)
{
	XnStatus rc;
	rc = SimpleViewer::Init(argc, argv);
	if(rc != XN_STATUS_OK)
	{
		return rc;
	}

	return m_HandTracker.Init();
}

XnStatus HandViewer::Run()
{
	XnStatus rc = m_HandTracker.Run();

	if(rc != XN_STATUS_OK)
	{
		return rc;
	}

	return SimpleViewer::Run(); // Does not return, enters OpenGL main loop instead
}

void HandViewer::DisplayPostDraw()
{
	typedef TrailHistory			History;
	typedef History::ConstIterator	HistoryIterator;
	typedef Trail::ConstIterator	TrailIterator;

	static const float colours[][3] =
	{
		{ 0.5f, 0.5f, 0.5f},
		{ 0.0f, 1.0f, 0.0f},
		{ 0.0f, 0.5f, 1.0f},
		{ 1.0f, 1.0f, 0.0f},
		{ 1.0f, 0.5f, 0.0f},
		{ 1.0f, 0.0f, 1.0f}
	};
	const TrailHistory&	history = m_HandTracker.GetHistory();

	// History points coordinates buffer
	XnFloat	coordinates[3 * MAX_HAND_TRAIL_LENGTH];

	const HistoryIterator	hend = history.End();
	for(HistoryIterator		hit = history.Begin(); hit != hend; ++hit)
	{

		// Dump the history to local buffer
		int				numpoints = 0;
		const Trail&	trail = hit->Value();

		const TrailIterator	tend = trail.End();
		for(TrailIterator	tit = trail.Begin(); tit != tend; ++tit)
		{
			XnPoint3D	point = *tit;
			m_depth.ConvertRealWorldToProjective(1, &point, &point);
			ScalePoint(point);
			coordinates[numpoints * 3] = point.X;
			coordinates[numpoints * 3 + 1] = point.Y;
			coordinates[numpoints * 3 + 2] = 0;

			++numpoints;
		}
		assert(numpoints <= MAX_HAND_TRAIL_LENGTH);

		// Draw the hand trail history
		XnUInt32 nColor = hit->Key() % LENGTHOF(colours);
		glColor4f(colours[nColor][0],
			colours[nColor][1],
			colours[nColor][2],
			1.0f);
		glPointSize(2);
		glVertexPointer(3, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_LINE_STRIP, 0, numpoints);
		// Current point as a larger dot
		glPointSize(8);
		glDrawArrays(GL_POINTS, 0, 1);
		glFlush();
	}
}


XnStatus HandViewer::InitOpenGL( int argc, char **argv )
{
	XnStatus rc = SimpleViewer::InitOpenGL(argc, argv); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	return rc;
}
