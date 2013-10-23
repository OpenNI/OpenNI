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
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class AudioMetaData : OutputMetaData
	{
		public AudioMetaData()
		{
		}

		public byte NumberOfChannels
		{
			get { return this.audio.Wave.Channels; }
			set { this.audio.Wave.Channels = value; }
		}

		public int SampleRate
		{
			get { return this.audio.Wave.SampleRate; }
			set { this.audio.Wave.SampleRate = value; }
		}

		public int BitsPerSample
		{
			get { return this.audio.Wave.BitsPerSample; }
			set { this.audio.Wave.BitsPerSample = (ushort)value; }
		}

		public IntPtr AudioBufferPtr
		{
			get { return this.audio.pData; }
		}

		public new IMarshaler GetMarshaler(bool passOut)
		{
			return new AudioMetaDataMarshaler(this, passOut);
		}

		private SafeNativeMethods.XnAudioMetaData audio = new SafeNativeMethods.XnAudioMetaData();

		private class AudioMetaDataMarshaler : Marshaler<SafeNativeMethods.XnAudioMetaData>
		{
			public AudioMetaDataMarshaler(AudioMetaData obj, bool marshalOut) :
				base(obj.audio, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(AudioMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((OutputMetaData)obj).GetMarshaler(marshalOut);
				obj.audio.pOutput = inner.Native;
				return inner;
			}
		}
	}
}
