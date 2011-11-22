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
#ifndef NI_HAND_VIEWER_H__
#define NI_HAND_VIEWER_H__

#include "NiSimpleViewer.h"
#include "NiHandTracker.h"

class HandViewer: public SimpleViewer
{
public:
	// Singleton
	static SimpleViewer& CreateInstance(xn::Context& context);

	virtual XnStatus Init(int argc, char **argv);
	virtual XnStatus Run();	//Does not return if successful

protected:
	HandViewer(xn::Context& context);

	virtual void DisplayPostDraw();

	virtual XnStatus InitOpenGL(int argc, char **argv);

private:
	HandTracker	m_HandTracker;
};

#endif //NI_HAND_VIEWER_H__