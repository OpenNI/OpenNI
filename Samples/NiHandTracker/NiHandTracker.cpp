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
#include "NiHandTracker.h"
#include <cassert>


using namespace xn;


//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define LENGTHOF(arr)			(sizeof(arr)/sizeof(arr[0]))
#define FOR_ALL(arr, action)	{for(int i = 0; i < LENGTHOF(arr); ++i){action(arr[i])}}

#define ADD_GESTURE(name)		{if(m_GestureGenerator.AddGesture(name, NULL) != XN_STATUS_OK){printf("Unable to add gesture"); exit(1);}}
#define REMOVE_GESTURE(name)	{if(m_GestureGenerator.RemoveGesture(name) != XN_STATUS_OK){printf("Unable to remove gesture"); exit(1);}}

#define ADD_ALL_GESTURES		FOR_ALL(cGestures, ADD_GESTURE)
#define REMOVE_ALL_GESTURES		FOR_ALL(cGestures, REMOVE_GESTURE)


//---------------------------------------------------------------------------
// Consts
//---------------------------------------------------------------------------
// Gestures to track
static const char			cClickStr[] = "Click";
static const char			cWaveStr[] = "Wave";
static const char* const	cGestures[] =
{
	cClickStr,
	cWaveStr
};

//---------------------------------------------------------------------------
// Statics
//---------------------------------------------------------------------------
XnListT<HandTracker*>	HandTracker::sm_Instances;


//---------------------------------------------------------------------------
// Hooks
//---------------------------------------------------------------------------
void XN_CALLBACK_TYPE HandTracker::Gesture_Recognized(	xn::GestureGenerator&	/*generator*/, 
														const XnChar*			strGesture, 
														const XnPoint3D*		/*pIDPosition*/, 
														const XnPoint3D*		pEndPosition, 
														void*					pCookie)
{
	printf("Gesture recognized: %s\n", strGesture);

	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(sm_Instances.Find(pThis) == sm_Instances.End())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	pThis->m_HandsGenerator.StartTracking(*pEndPosition);
}

void XN_CALLBACK_TYPE HandTracker::Hand_Create(	xn::HandsGenerator& /*generator*/, 
												XnUserID			nId, 
												const XnPoint3D*	pPosition, 
												XnFloat				/*fTime*/, 
												void*				pCookie)
{
	printf("New Hand: %d @ (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);

	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(sm_Instances.Find(pThis) == sm_Instances.End())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	pThis->m_History[nId].Push(*pPosition);
}

void XN_CALLBACK_TYPE HandTracker::Hand_Update(	xn::HandsGenerator& /*generator*/, 
												XnUserID			nId, 
												const XnPoint3D*	pPosition, 
												XnFloat				/*fTime*/, 
												void*				pCookie)
{
	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(sm_Instances.Find(pThis) == sm_Instances.End())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	// Add to this user's hands history
	TrailHistory::Iterator it = pThis->m_History.Find(nId);
	if (it == pThis->m_History.End())
	{
		printf("Dead hand update: skipped!\n");
		return;
	}

	it->Value().Push(*pPosition);
}

void XN_CALLBACK_TYPE HandTracker::Hand_Destroy(	xn::HandsGenerator& /*generator*/, 
													XnUserID			nId, 
													XnFloat				/*fTime*/, 
													void*				pCookie)
{
	printf("Lost Hand: %d\n", nId);

	HandTracker*	pThis = static_cast<HandTracker*>(pCookie);
	if(sm_Instances.Find(pThis) == sm_Instances.End())
	{
		printf("Dead HandTracker: skipped!\n");
		return;
	}

	// Remove this user from hands history
	pThis->m_History.Remove(nId);
}


//---------------------------------------------------------------------------
// Method Definitions
//---------------------------------------------------------------------------
HandTracker::HandTracker(xn::Context& context) : m_rContext(context)
{
	// Track all living instances (to protect against calling dead pointers in the Gesture/Hand Generator hooks)
	XnStatus rc = sm_Instances.AddLast(this);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to add NiHandTracker instance to the list.");
		exit(1);
	}
}

HandTracker::~HandTracker()
{
	// Remove the current instance from living instances list
	XnListT<HandTracker*>::ConstIterator it = sm_Instances.Find(this);
	assert(it != sm_Instances.End());
	sm_Instances.Remove(it);
}

XnStatus HandTracker::Init()
{            
	XnStatus			rc;
	XnCallbackHandle	chandle;

	// Create generators
	rc = m_GestureGenerator.Create(m_rContext);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to create GestureGenerator.");
		return rc;
	}

	rc = m_HandsGenerator.Create(m_rContext);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to create HandsGenerator.");
		return rc;
	}

	// Register callbacks
	// Using this as cookie
	rc = m_GestureGenerator.RegisterGestureCallbacks(Gesture_Recognized, Gesture_Process, this, chandle);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to register gesture callbacks.");
		return rc;
	}

	rc = m_HandsGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, this, chandle);
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to register hand callbacks.");
		return rc;
	}

	return XN_STATUS_OK;
}

XnStatus HandTracker::Run()
{
	//ADD_ALL_GESTURES;

	XnStatus	rc = m_rContext.StartGeneratingAll();
	if (rc != XN_STATUS_OK)
	{
		printf("Unable to start generating.");
		return rc;
	}

	ADD_ALL_GESTURES;

	return XN_STATUS_OK;
}
