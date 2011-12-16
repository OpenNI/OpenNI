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
﻿using System;
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
