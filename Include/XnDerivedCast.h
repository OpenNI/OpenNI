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
****************************************************************************/\

#ifndef __XN_DERIVED_CAST_H__
#define __XN_DERIVED_CAST_H__

// derived_cast Acts as dynamic_cast without the need for RTTI. used in platforms that not implement RTTI well like Linux-Arm.
template<class T>
static T derived_cast(ModuleProductionNode* pPrdNode);

#define DERIVED_CAST_IMPL(T, enm)							\
	template<>									\
	T* derived_cast<T*>(ModuleProductionNode* pPrdNode)				\
	{										\
		return (T*)pPrdNode->m_aInterfaces[enm];				\
	}

DERIVED_CAST_IMPL(ModuleDevice, XN_NODE_TYPE_DEVICE)
DERIVED_CAST_IMPL(ModuleRecorder, XN_NODE_TYPE_RECORDER)
DERIVED_CAST_IMPL(ModulePlayer, XN_NODE_TYPE_PLAYER)
DERIVED_CAST_IMPL(ModuleCodec, XN_NODE_TYPE_CODEC)
DERIVED_CAST_IMPL(ModuleScriptNode, XN_NODE_TYPE_SCRIPT)
DERIVED_CAST_IMPL(ModuleGenerator, XN_NODE_TYPE_GENERATOR)
DERIVED_CAST_IMPL(ModuleUserGenerator, XN_NODE_TYPE_USER)
DERIVED_CAST_IMPL(ModuleHandsGenerator, XN_NODE_TYPE_HANDS)
DERIVED_CAST_IMPL(ModuleGestureGenerator, XN_NODE_TYPE_GESTURE)
DERIVED_CAST_IMPL(ModuleAudioGenerator, XN_NODE_TYPE_AUDIO)
DERIVED_CAST_IMPL(ModuleMapGenerator, XN_NODE_TYPE_MAP_GENERATOR)
DERIVED_CAST_IMPL(ModuleDepthGenerator, XN_NODE_TYPE_DEPTH)
DERIVED_CAST_IMPL(ModuleImageGenerator, XN_NODE_TYPE_IMAGE)
DERIVED_CAST_IMPL(ModuleIRGenerator, XN_NODE_TYPE_IR)
DERIVED_CAST_IMPL(ModuleSceneAnalyzer, XN_NODE_TYPE_SCENE)

#ifdef dynamic_cast
#undef dynamic_cast
#endif

#define dynamic_cast derived_cast

#endif
