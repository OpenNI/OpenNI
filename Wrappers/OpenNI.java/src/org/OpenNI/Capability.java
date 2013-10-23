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
package org.openni;

/**
 * Provides string names for all available capabilities
 *
 */
public enum Capability 
{
	ExtendedSerialization ("ExtendedSerialization"),
	Mirror ("Mirror"),
	AlternativeViewPoint ("AlternativeViewPoint"),
	Cropping ("Cropping"),
	UserPosition ("UserPosition"),
	Skeleton ("User::Skeleton"),
	PoseDetection ("User::PoseDetection"),
	LockAware ("LockAware"),
	ErrorState ("ErrorState"),
	FrameSync ("FrameSync"),
	DeviceIdentification ("DeviceIdentification"),
	Brightness ("Brightness"),
	Contrast ("Contrast"),
	Hue ("Hue"),
	Saturation ("Saturation"),
	Sharpness ("Sharpness"),
	Gamma ("Gamma"),
	WhiteBalance ("ColorTemperature"),
	BacklightCompensation ("BacklightCompensation"),
	Gain ("Gain"),
	Pan ("Pan"),
	Tilt ("Tilt"),
	Roll ("Roll"),
	Zoom ("Zoom"),
	Exposure ("Exposure"),
	Iris ("Iris"),
	Focus ("Focus"),
	LowLightCompensation ("LowLightCompensation"),
	AntiFlicker ("AntiFlicker");
	
	Capability(String name)
	{
		this.name = name;
	}
	
	public String getName() { return this.name; }
	
	private String name;
}
