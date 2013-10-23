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
#ifndef __XN_MODULE_INTERFACE_CONTAINERS_H__
#define __XN_MODULE_INTERFACE_CONTAINERS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleInterface.h>
#include <XnOS.h>
#include <XnBitSet.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SPECIAL_BC_BEHAVIOR ((void*)0xbcbcbcbc)

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

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

		xnOSMemSet(&GeneralInt, 0, sizeof(GeneralInt));
		ProductionNode.pGeneralIntInterface = &GeneralInt;

		HierarchyType.Set(XN_NODE_TYPE_PRODUCTION_NODE, TRUE);
	}
	XnModuleProductionNodeInterface ProductionNode;
	XnModuleExtendedSerializationInterface ExtendedSerialization;
	XnModuleLockAwareInterface LockAware;
	XnModuleErrorStateInterface ErrorState;
	XnModuleGeneralIntInterface GeneralInt;
	XnBitSet HierarchyType;
};

class XnDeviceInterfaceContainer : public XnProductionNodeInterfaceContainer
{
public:
	XnDeviceInterfaceContainer()
	{
		xnOSMemSet(&Device, 0, sizeof(Device));
		Device.pProductionNode = &ProductionNode;

		xnOSMemSet(&Identification, 0, sizeof(Identification));
		Device.pDeviceIdentificationInterface = &Identification;

		HierarchyType.Set(XN_NODE_TYPE_DEVICE, TRUE);
	}
	XnModuleDeviceInterface Device;
	XnModuleDeviceIdentificationInterface Identification;
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
		xnOSMemSet(&FrameSync, 0, sizeof(FrameSync));
		Generator.pFrameSyncInterface = &FrameSync;
		HierarchyType.Set(XN_NODE_TYPE_GENERATOR, TRUE);
	}
	XnModuleGeneratorInterface Generator;
	XnModuleMirrorInterface Mirror;
	XnModuleAlternativeViewPointInterface AlternativeViewPoint;
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
		HierarchyType.Set(XN_NODE_TYPE_RECORDER, TRUE);
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

		HierarchyType.Set(XN_NODE_TYPE_PLAYER, TRUE);
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

		xnOSMemSet(&AntiFlicker, 0, sizeof(AntiFlicker));
		Map.pAntiFlickerInterface = &AntiFlicker;

		HierarchyType.Set(XN_NODE_TYPE_MAP_GENERATOR, TRUE);
	}
	XnModuleMapGeneratorInterface Map;
	XnModuleCroppingInterface Cropping;
	XnModuleAntiFlickerInterface AntiFlicker;
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

		HierarchyType.Set(XN_NODE_TYPE_DEPTH, TRUE);
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
		HierarchyType.Set(XN_NODE_TYPE_IMAGE, TRUE);
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
		HierarchyType.Set(XN_NODE_TYPE_IR, TRUE);
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
		HierarchyType.Set(XN_NODE_TYPE_GESTURE, TRUE);
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
		HierarchyType.Set(XN_NODE_TYPE_SCENE, TRUE);
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
		User.pPoseDetectionInterface = &PoseDetection;
		HierarchyType.Set(XN_NODE_TYPE_USER, TRUE);
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
		xnOSMemSet(&HandTouchingFOVEdge, 0, sizeof(HandTouchingFOVEdge));
		Hands.pHandTouchingFOVEdgeInterface = &HandTouchingFOVEdge;
		HierarchyType.Set(XN_NODE_TYPE_HANDS, TRUE);
	}
	XnModuleHandsGeneratorInterface Hands;

	XnModuleHandTouchingFOVEdgeCapabilityInterface HandTouchingFOVEdge;
};

class XnAudioGeneratorInterfaceContainer : public XnGeneratorInterfaceContainer
{
public:
	XnAudioGeneratorInterfaceContainer()
	{
		xnOSMemSet(&Audio, 0, sizeof(Audio));
		Audio.pGeneratorInterface = &Generator;

		HierarchyType.Set(XN_NODE_TYPE_AUDIO, TRUE);
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

		HierarchyType.Set(XN_NODE_TYPE_CODEC, TRUE);
	}

	XnModuleCodecInterface Codec;
};

class XnScriptNodeInterfaceContainer : public XnProductionNodeInterfaceContainer
{
public:
	XnScriptNodeInterfaceContainer()
	{
		xnOSMemSet(&Script, 0, sizeof(Script));
		Script.pProductionNode = &ProductionNode;

		HierarchyType.Set(XN_NODE_TYPE_SCRIPT, TRUE);
	}

	XnModuleScriptNodeInterface Script;
};

#endif // __XN_MODULE_INTERFACE_CONTAINERS_H__