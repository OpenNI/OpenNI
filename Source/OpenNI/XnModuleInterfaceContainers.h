/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




#ifndef __XN_MODULE_INTERFACE_CONTAINERS_H__
#define __XN_MODULE_INTERFACE_CONTAINERS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleInterface.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef enum XnHierarchyType
{
	XN_HIERARCHY_TYPE_PRODUCTION_NODE = 1 << 0,
	XN_HIERARCHY_TYPE_DEVICE = 1 << 1,
	XN_HIERARCHY_TYPE_GENERATOR = 1 << 2,
	XN_HIERARCHY_TYPE_MAP_GENERATOR = 1 << 3,
	XN_HIERARCHY_TYPE_DEPTH_GENERATOR = 1 << 4,
	XN_HIERARCHY_TYPE_IMAGE_GENERATOR = 1 << 5,
	XN_HIERARCHY_TYPE_IR_GENERATOR = 1 << 6,
	XN_HIERARCHY_TYPE_USER_GENERATOR = 1 << 7,
	XN_HIERARCHY_TYPE_GESTURE_GENERATOR = 1 << 8,
	XN_HIERARCHY_TYPE_SCENE_ANALYZER = 1 << 9,
	XN_HIERARCHY_TYPE_AUDIO_GENERATOR = 1 << 10,
	XN_HIERARCHY_TYPE_RECORDER = 1 << 11,
	XN_HIERARCHY_TYPE_PLAYER = 1 << 12,
	XN_HIERARCHY_TYPE_HANDS_GENERATOR = 1 << 13,
	XN_HIERARCHY_TYPE_NODE_NOTIFICATIONS = 1 << 14,
	XN_HIERARCHY_TYPE_DEPRECATED_PLAYER = 1 << 15,
	XN_HIERARCHY_TYPE_CODEC = 1 << 16,
} XnHierarchyType;

class XnProductionNodeInterfaceContainer
{
public:
	XnProductionNodeInterfaceContainer()
	{
		xnOSMemSet(&ProductionNode, 0, sizeof(ProductionNode));

		xnOSMemSet(&ExtendedSerialization, 0, sizeof(ExtendedSerialization));
		ProductionNode.pExtendedSerializationInterface = &ExtendedSerialization;

		xnOSMemSet(&LockAware, 0, sizeof(LockAware));
		ProductionNode.pLockAwareInterface = &LockAware;

		xnOSMemSet(&ErrorState, 0, sizeof(ErrorState));
		ProductionNode.pErrorStateInterface = &ErrorState;

		HierarchyType = XN_HIERARCHY_TYPE_PRODUCTION_NODE;
	}
	XnModuleProductionNodeInterface ProductionNode;
	XnModuleExtendedSerializationInterface ExtendedSerialization;
	XnModuleLockAwareInterface LockAware;
	XnModuleErrorStateInterface ErrorState;
	XnUInt32 HierarchyType;
};

class XnDeviceInterfaceContainer : public XnProductionNodeInterfaceContainer
{
public:
	XnDeviceInterfaceContainer()
	{
		xnOSMemSet(&Device, 0, sizeof(Device));
		Device.pProductionNode = &ProductionNode;
		HierarchyType |= XN_HIERARCHY_TYPE_DEVICE;
	}
	XnModuleDeviceInterface Device;
};

class XnGeneratorInterfaceContainer : public XnProductionNodeInterfaceContainer
{
public:
	XnGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Generator, 0, sizeof(Generator));
		Generator.pProductionNodeInterface = &ProductionNode;
		xnOSMemSet(&Mirror, 0, sizeof(Mirror));
		Generator.pMirrorInterface = &Mirror;
		xnOSMemSet(&AlternativeViewPoint, 0, sizeof(AlternativeViewPoint));
		Generator.pAlternativeViewPointInterface = &AlternativeViewPoint;
		xnOSMemSet(&Seeking, 0, sizeof(Seeking));
		Generator.pSeekingInterface = &Seeking;
		xnOSMemSet(&FrameSync, 0, sizeof(FrameSync));
		Generator.pFrameSyncInterface = &FrameSync;
		HierarchyType |= XN_HIERARCHY_TYPE_GENERATOR;
	}
	XnModuleGeneratorInterface Generator;
	XnModuleMirrorInterface Mirror;
	XnModuleAlternativeViewPointInterface AlternativeViewPoint;
	XnModuleSeekingInterface Seeking;
	XnModuleFrameSyncInterface FrameSync;
};

class XnRecorderInterfaceContainer : public XnProductionNodeInterfaceContainer
{
public:
	XnRecorderInterfaceContainer()
	{
		xnOSMemSet(&recorder, 0, sizeof(recorder));
		recorder.pProductionNode = &ProductionNode;
		xnOSMemSet(&nodeNotifications, 0, sizeof(nodeNotifications));
		recorder.pNodeNotifications = &nodeNotifications;
		HierarchyType |= XN_HIERARCHY_TYPE_RECORDER;
	}
	XnModuleRecorderInterface recorder;
	XnNodeNotifications nodeNotifications;
};

class XnPlayerInterfaceContainer : public XnProductionNodeInterfaceContainer
{
public:
	XnPlayerInterfaceContainer()
	{
		xnOSMemSet(&Player, 0, sizeof(Player));
		Player.pProductionNode = &ProductionNode;

		HierarchyType |= XN_HIERARCHY_TYPE_PLAYER;
	}
	XnModulePlayerInterface Player;
};

class XnMapGeneratorInterfaceContainer : public XnGeneratorInterfaceContainer
{
public:
	XnMapGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Map, 0, sizeof(Map));
		Map.pGeneratorInterface = &Generator;
		xnOSMemSet(&Cropping, 0, sizeof(Cropping));
		Map.pCroppingInterface = &Cropping;
		HierarchyType |= XN_HIERARCHY_TYPE_MAP_GENERATOR;
	}
	XnModuleMapGeneratorInterface Map;
	XnModuleCroppingInterface Cropping;
};

class XnDepthGeneratorInterfaceContainer : public XnMapGeneratorInterfaceContainer
{
public:
	XnDepthGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Depth, 0, sizeof(Depth));
		Depth.pMapInterface = &Map;
		xnOSMemSet(&UserPosition, 0, sizeof(UserPosition));
		Depth.pUserPositionInterface = &UserPosition;
		HierarchyType |= XN_HIERARCHY_TYPE_DEPTH_GENERATOR;
	}
	XnModuleDepthGeneratorInterface Depth;
	XnModuleUserPositionCapabilityInterface UserPosition;
};

class XnImageGeneratorInterfaceContainer : public XnMapGeneratorInterfaceContainer
{
public:
	XnImageGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Image, 0, sizeof(Image));
		Image.pMapInterface = &Map;
		HierarchyType |= XN_HIERARCHY_TYPE_IMAGE_GENERATOR;
	}
	XnModuleImageGeneratorInterface Image;
};

class XnIRGeneratorInterfaceContainer : public XnMapGeneratorInterfaceContainer
{
public:
	XnIRGeneratorInterfaceContainer()
	{
		xnOSMemSet(&IR, 0, sizeof(IR));
		IR.pMapInterface = &Map;
		HierarchyType |= XN_HIERARCHY_TYPE_IR_GENERATOR;
	}
	XnModuleIRGeneratorInterface IR;
};

class XnGestureGeneratorInterfaceContainer : public XnGeneratorInterfaceContainer
{
public:
	XnGestureGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Gesture, 0, sizeof(Gesture));
		Gesture.pGeneratorInterface = &Generator;
		HierarchyType |= XN_HIERARCHY_TYPE_GESTURE_GENERATOR;
	}
	XnModuleGestureGeneratorInterface Gesture;
};

class XnSceneAnalyzerInterfaceContainer : public XnMapGeneratorInterfaceContainer
{
public:
	XnSceneAnalyzerInterfaceContainer()
	{
		xnOSMemSet(&Scene, 0, sizeof(Scene));
		Scene.pMapInterface = &Map;
		HierarchyType |= XN_HIERARCHY_TYPE_SCENE_ANALYZER;
	}
	XnModuleSceneAnalyzerInterface Scene;
};

class XnUserGeneratorInterfaceContainer : public XnGeneratorInterfaceContainer
{
public:
	XnUserGeneratorInterfaceContainer()
	{
		xnOSMemSet(&User, 0, sizeof(User));
		User.pGeneratorInterface = &Generator;
		xnOSMemSet(&Skeleton, 0, sizeof(Skeleton));
		User.pSkeletonInterface = &Skeleton;
		xnOSMemSet(&PoseDetection, 0, sizeof(PoseDetection));
		User.pPoseDetectionInteface = &PoseDetection;
		HierarchyType |= XN_HIERARCHY_TYPE_USER_GENERATOR;
	}
	XnModuleUserGeneratorInterface User;

	XnModuleSkeletonCapabilityInterface Skeleton;
	XnModulePoseDetectionCapabilityInterface PoseDetection;
};

class XnHandsGeneratorInterfaceContainer : public XnGeneratorInterfaceContainer
{
public:
	XnHandsGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Hands, 0, sizeof(Hands));
		Hands.pGeneratorInterface = &Generator;
		HierarchyType |= XN_HIERARCHY_TYPE_HANDS_GENERATOR;
	}
	XnModuleHandsGeneratorInterface Hands;
};

class XnAudioGeneratorInterfaceContainer : public XnGeneratorInterfaceContainer
{
public:
	XnAudioGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Audio, 0, sizeof(Audio));
		Audio.pGeneratorInterface = &Generator;

		HierarchyType |= XN_HIERARCHY_TYPE_AUDIO_GENERATOR;
	}

	XnModuleAudioGeneratorInterface Audio;
};

class XnCodecInterfaceContainer : public XnProductionNodeInterfaceContainer
{
public:
	XnCodecInterfaceContainer()
	{
		xnOSMemSet(&Codec, 0, sizeof(Codec));
		Codec.pProductionNode = &ProductionNode;

		HierarchyType |= XN_HIERARCHY_TYPE_CODEC;
	}

	XnModuleCodecInterface Codec;
};

#endif // __XN_MODULE_INTERFACE_CONTAINERS_H__