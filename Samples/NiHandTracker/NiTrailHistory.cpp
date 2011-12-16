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
#include "NiTrailHistory.h"


//---------------------------------------------------------------------------
// Trail Method Definitions
//---------------------------------------------------------------------------
TrailHistory::Trail::~Trail()
{
	// Pop and delete all data
	XnPoint3D* point;
	while((point = Pop()))
	{
		delete point;
	}
}

void TrailHistory::Trail::Push( const XnPoint3D& position )
{
	XnPoint3D*	point;

	if(m_nLength == m_nMaxLength)
	{
		// Reuse the already allocated memory, remove oldest history element
		point = Pop();
		assert(point);
	}
	else
	{
		// Should never overflow
		assert(m_nLength < m_nMaxLength);
		point = new XnPoint3D;
	}

	// Add the pushed value to the list
	*point = position;
	if(m_List.AddFirst(point) != XN_STATUS_OK)
	{
		printf("Unable to add to XnList");
		exit(1);
	}
	++m_nLength;
}

XnPoint3D* TrailHistory::Trail::Pop()
{
	void*	point = NULL;

	if(m_List.IsEmpty())
	{
		return NULL;
	}

	if(m_List.Remove(m_List.rbegin(), point) != XN_STATUS_OK)
	{
		printf("Unable to remove from XnList");
		exit(1);
	}
	assert(point);
	assert(m_nLength);
	--m_nLength;
	assert(m_nLength < m_nMaxLength);
	return static_cast<XnPoint3D*>(point);
}


//---------------------------------------------------------------------------
// TrailHistory Method Definitions
//---------------------------------------------------------------------------

TrailHistory::~TrailHistory()
{
	// Delete all data
	while(!m_Hash.IsEmpty())
	{
		Trail*	trail;

		if(m_Hash.Remove(m_Hash.begin().Key(), trail) != XN_STATUS_OK)
		{
			printf("Unable to remove from XnHash");
			exit(1);
		}
		assert(trail);

		delete static_cast<Trail*>(trail);
	}
}

TrailHistory::Trail& TrailHistory::Add( XnUserID id )
{
	Trail*	dummy = NULL;

	// An existing user should not be added again
	assert(m_Hash.Get(id, dummy) == XN_STATUS_NO_MATCH);

	Trail*	trail = new Trail(m_nMaxTrailLength);

	if(m_Hash.Set(id, trail) != XN_STATUS_OK)
	{
		printf("Unable to add to XnHash");
		exit(1);
	}

	return *trail;
}

TrailHistory::Trail* TrailHistory::Find( XnUserID id ) const
{
	Trail*	trail;

	switch(m_Hash.Get(id, trail))
	{
	case XN_STATUS_OK:
		return trail;

	case XN_STATUS_NO_MATCH:
		return NULL;

	default:
		printf("Unable to search XnHash");
		exit(1);
	}
}

void TrailHistory::Remove( XnUserID id )
{
	Trail*	trail = NULL;

	if(m_Hash.Remove(id, trail) != XN_STATUS_OK)
	{
		printf("Unable to remove from XnHash");
		exit(1);
	}
	assert(trail);

	delete trail;
}
