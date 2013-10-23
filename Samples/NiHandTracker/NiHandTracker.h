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
#ifndef NI_HAND_TRACKER_H__
#define NI_HAND_TRACKER_H__

#include <XnCppWrapper.h>
#include <XnCyclicStackT.h>
#include <XnHashT.h>

// Hand position history length (positions)
#define MAX_HAND_TRAIL_LENGTH	10

typedef XnCyclicStackT<XnPoint3D, MAX_HAND_TRAIL_LENGTH> Trail;
typedef XnHashT<XnUserID, Trail> TrailHistory;

class HandTracker
{
public:
	HandTracker(xn::Context& context);
	~HandTracker();

	XnStatus Init();
	XnStatus Run();

	const TrailHistory&	GetHistory()	const	{return m_History;}

private:
	// OpenNI Gesture and Hands Generator callbacks
	static void XN_CALLBACK_TYPE Gesture_Recognized(xn::GestureGenerator&	generator, 
													const XnChar*			strGesture, 
													const XnPoint3D*		pIDPosition, 
													const XnPoint3D*		pEndPosition, 
													void*					pCookie);
	static void XN_CALLBACK_TYPE Gesture_Process(	xn::GestureGenerator&	/*generator*/, 
													const XnChar*			/*strGesture*/, 
													const XnPoint3D*		/*pPosition*/, 
													XnFloat					/*fProgress*/, 
													void*					/*pCookie*/)	{}
	static void XN_CALLBACK_TYPE Hand_Create(	xn::HandsGenerator& generator, 
												XnUserID			nId, 
												const XnPoint3D*	pPosition, 
												XnFloat				fTime, 
												void*				pCookie);
	static void XN_CALLBACK_TYPE Hand_Update(	xn::HandsGenerator& generator, 
												XnUserID			nId, 
												const XnPoint3D*	pPosition, 
												XnFloat				fTime, 
												void*				pCookie);
	static void XN_CALLBACK_TYPE Hand_Destroy(	xn::HandsGenerator& generator, 
												XnUserID			nId, 
												XnFloat				fTime, 
												void*				pCookie);

	xn::Context&			m_rContext;
	TrailHistory			m_History;
	xn::GestureGenerator	m_GestureGenerator;
	xn::HandsGenerator		m_HandsGenerator;

	static XnListT<HandTracker*>	sm_Instances;	// Living instances of the class

private:
	XN_DISABLE_COPY_AND_ASSIGN(HandTracker);
};

#endif //NI_HAND_TRACKER_H__