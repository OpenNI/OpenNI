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