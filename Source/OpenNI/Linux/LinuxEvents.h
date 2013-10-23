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
	virtual ~XnLinuxEvent() {}
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
