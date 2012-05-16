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
