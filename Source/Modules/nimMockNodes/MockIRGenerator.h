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
#ifndef __MOCKIRGENERATOR_H__
#define __MOCKIRGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockMapGenerator.h"

XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_INHERITS_VIA_DOMINANCE_WARNING_ID)

class MockIRGenerator : 
	public MockMapGenerator,
	virtual public xn::ModuleIRGenerator
{
public:
	MockIRGenerator(xn::Context& context, const XnChar* strName);
	virtual ~MockIRGenerator();
	/*Generator*/
	virtual const void* GetData() { return MockMapGenerator::GetData(); }

	/*Map Generator*/
	virtual XnUInt32 GetBytesPerPixel() { return xn::ModuleIRGenerator::GetBytesPerPixel(); }

	/*IR Generator*/
	virtual XnIRPixel* GetIRMap();
};

XN_PRAGMA_STOP_DISABLED_WARNING_SECTION

#endif // __MOCKIRGENERATOR_H__