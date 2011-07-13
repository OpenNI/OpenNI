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
#ifndef __LINUX_POSIX_NAMED_EVENTS_H__
#define __LINUX_POSIX_NAMED_EVENTS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "LinuxEvents.h"

#ifdef XN_PLATFORM_LINUX_NO_SYSV
#include <semaphore.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnLinuxPosixNamedEvent : public XnLinuxNamedEvent
{
public:
	XnLinuxPosixNamedEvent(XnBool bManualReset, const XnChar* strName, XnBool bCreate);

	virtual XnStatus CreateNamed(const XnChar* strName);
	virtual XnStatus OpenNamed(const XnChar* strName);
	virtual XnStatus Destroy();
	virtual XnStatus Set();
	virtual XnStatus Reset();
	virtual XnStatus Wait(XnUInt32 nTimeout);

private:
	sem_t* m_pSem;
};

#endif // if no SysV

#endif // __LINUX_POSIX_NAMED_EVENTS_H__
