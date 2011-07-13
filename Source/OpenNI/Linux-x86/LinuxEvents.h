/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#ifndef __LINUX_EVENTS_H__
#define __LINUX_EVENTS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct _XnEvent {};

class XnLinuxEvent : public _XnEvent
{
public:
	XnLinuxEvent(XnBool bManualReset) : m_bSignaled(FALSE), m_bManualReset(bManualReset) {}
	virtual XnStatus Init() = 0;
	virtual XnStatus Destroy() = 0;
	virtual XnStatus Set() = 0;
	virtual XnStatus Reset() = 0;
	virtual XnStatus Wait(XnUInt32 nMilliseconds) = 0;

protected:
	XnBool m_bSignaled;
	XnBool m_bManualReset;
};

class XnLinuxNamedEvent : public XnLinuxEvent
{
public:
	XnLinuxNamedEvent(XnBool bManualReset, const XnChar* strName, XnBool bCreate) : XnLinuxEvent(bManualReset), m_strName(strName), m_bCreate(bCreate) {}

	virtual XnStatus Init()
	{
		if (m_bCreate)
		{
			return CreateNamed(m_strName);
		}
		else
		{
			return OpenNamed(m_strName);
		}
	}

protected:
	virtual XnStatus CreateNamed(const XnChar* strName) = 0;
	virtual XnStatus OpenNamed(const XnChar* strName) = 0;

private:
	XnBool m_bCreate;
	const XnChar* m_strName; // Valid only on creation
};

#endif // __LINUX_EVENTS_H__
