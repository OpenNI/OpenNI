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
