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
#ifndef NI_TRAIL_HISTORY_H__
#define NI_TRAIL_HISTORY_H__

#include <XnCppWrapper.h>
#include <XnList.h>
#include <XnHash.h>
#include <cassert>

// Substitute for map<XnUserID, queue<XnPoint3D>>
class TrailHistory
{
public:
	//TrailHistory service classes
	class Trail
	{
	public:
		class ConstIterator
		{
		public:
			ConstIterator(XnList::ConstIterator	base)						:m_Iterator(base){};

			ConstIterator&	operator++()									{++m_Iterator; return *this;}
			XnBool			operator==(const ConstIterator& other)	const	{return m_Iterator == other.m_Iterator;}
			XnBool			operator!=(const ConstIterator& other)	const	{return m_Iterator != other.m_Iterator;}

			const XnPoint3D&	operator*()	const							{return *static_cast<XnPoint3D*>(*m_Iterator);}

		private:
			XnList::ConstIterator	m_Iterator;
		};

	public:
		Trail(unsigned short maxLength)										:m_nLength(0),m_nMaxLength(maxLength){assert(maxLength > 0);}
		~Trail();

		void Push(const XnPoint3D&	position);

		ConstIterator		begin() const									{return ConstIterator(m_List.begin());}
		const ConstIterator	end()	const									{return ConstIterator(m_List.end());}

	private:
		// Not supported:
		Trail(const Trail&);
		Trail&	operator=(const Trail&);

		// Service
		XnPoint3D*	Pop();

		XnList			m_List;
		unsigned short	m_nLength;
		unsigned short	m_nMaxLength;
	};

	XN_DECLARE_DEFAULT_HASH(XnUserID, Trail*, XnUserTrailHash);

	class ConstIterator
	{
	public:
		ConstIterator(XnUserTrailHash::ConstIterator	base)						:m_Iterator(base){};

		ConstIterator&	operator++()									{++m_Iterator; return *this;}
		XnBool			operator==(const ConstIterator& other)	const	{return m_Iterator == other.m_Iterator;}
		XnBool			operator!=(const ConstIterator& other)	const	{return m_Iterator != other.m_Iterator;}

		XnUserID		GetKey()	const								{return m_Iterator.Key();}
		const Trail&	GetTrail()	const								{return *m_Iterator.Value();}

	private:
		XnUserTrailHash::ConstIterator	m_Iterator;
	};

	// TrailHistory methods
public:
	TrailHistory(unsigned short maxTrailLength)							:m_nMaxTrailLength(maxTrailLength){assert(maxTrailLength > 0);}
	~TrailHistory();

	Trail&	Add(XnUserID id);
	Trail*	Find(XnUserID id)	const;
	void	Remove(XnUserID id);

	ConstIterator		begin() const									{return ConstIterator(m_Hash.begin());}
	const ConstIterator	end()	const									{return ConstIterator(m_Hash.end());}

private:
	// Not supported:
	TrailHistory(const TrailHistory&);
	TrailHistory&	operator=(const TrailHistory&);

	XnUserTrailHash			m_Hash;
	unsigned short	m_nMaxTrailLength;
};

#endif //NI_TRAIL_HISTORY_H__