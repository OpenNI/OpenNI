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
 * Generates PCM audio data from a sensor's hardware microphones. <BR><BR>
 * 
 * This generator is used to read the audio data from a sensor's microphones.  Number
 * of available channels is determined by number of available microphones.  A variety of 
 * data formats are available, but the exact mix of supported formats will depend on the
 * sensor being used.
 * 
 * Defines the following events:
 * waveOutputModeChanged -- Triggered when the audio format changes
 * 
 */
public class AudioGenerator extends Generator
{
	/**
	 * Creates a new audio generator for the given OpenNI context
	 * @param context OpenNI context for this generator
	 * @param nodeHandle Native pointer to this object
	 * @param addRef Whether to register the native pointer to this object
	 * @throws GeneralException If underlying native code returns an error code, General Exception is thrown by this function
	 */
	public AudioGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException
	{
		super(context, nodeHandle, addRef);
		
		this.waveOutputModeChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToWaveOutputModeChanges(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromWaveOutputModeChanges(toNative(), hCallback);
			}
		}; 
	}

	/**
	 * Creates a new audio generator
	 * @param context OpenNI context for this generator
	 * @param query Place to store query information 
	 * @param errors Place to store error information
	 * @return A new audio generator, created according to the given query
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static AudioGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateAudioGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		AudioGenerator result = (AudioGenerator)context.createProductionNodeObject(handle.value, NodeType.AUDIO);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	/**
	 * Creates a new audio generator, but does not store error values
	 * @param context OpenNI context for this generator
	 * @param query Place to store query information
	 * @return A new audio generator, created according to the given query
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static AudioGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	/**
	 * Creates a new audio generator, with a query value of 0, and no place to store any error values
	 * @param context OpenNI context for this generator
	 * @return A new audio generator
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static AudioGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	/**
	 * Generates a list of available output modes 
	 * @return A list of WaveOutputModes, one for each available output mode
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public WaveOutputMode[] getSupportedMapOutputModes() throws StatusException
	{
		int count = NativeMethods.xnGetSupportedWaveOutputModesCount(this.toNative());
		WaveOutputMode[] supportedModes = new WaveOutputMode[count];
		int status = NativeMethods.xnGetSupportedWaveOutputModes(this.toNative(), supportedModes);
		WrapperUtils.throwOnError(status);
		return supportedModes;
	}

	/**
	 * Returns the current wave output mode of this audio generator
	 * @return Current wave output mode, represented as a WaveOutputMode object
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public WaveOutputMode getWaveOutputMode() throws StatusException
	{
		OutArg<Integer> sampleRate = new OutArg<Integer>();
		OutArg<Short> bitsPerSample = new OutArg<Short>();
		OutArg<Byte> numberOfChannels = new OutArg<Byte>();
		int status = NativeMethods.xnGetWaveOutputMode(this.toNative(), sampleRate, bitsPerSample, numberOfChannels);
		WrapperUtils.throwOnError(status);
		return new WaveOutputMode(sampleRate.value, bitsPerSample.value, numberOfChannels.value);
	}
	
	/**
	 * Sets the current wave output mode of this node
	 * @param mode Desired new wave output mode of this node
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void setWaveOutputMode(WaveOutputMode mode) throws StatusException
	{
		int status = NativeMethods.xnSetWaveOutputMode(this.toNative(), mode.getSampleRate(), mode.getBitsPerSample(), mode.getNumberOfChannels());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Provides access to the Output Mode Changed event type
	 * @return
	 */
	public IStateChangedObservable getMapOutputModeChangedEvent() { return this.waveOutputModeChanged; }

	/**
	 * Gets the current data from this node
	 * @param audioMD AudioMetaData object to store the data in
	 */
	public void getMetaData(AudioMetaData audioMD)
	{
		NativeMethods.xnGetAudioMetaData(this.toNative(), audioMD);
	}

	/**
	 * Gets the current data from this node
	 * @return AudioMeta data with the data stored in it
	 */
	public AudioMetaData getMetaData()
	{
		AudioMetaData audioMD = new AudioMetaData();
		getMetaData(audioMD);
		return audioMD;
	}

	private StateChangedObservable waveOutputModeChanged;
}
