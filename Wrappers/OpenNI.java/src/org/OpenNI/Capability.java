package org.OpenNI;

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
