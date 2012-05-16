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