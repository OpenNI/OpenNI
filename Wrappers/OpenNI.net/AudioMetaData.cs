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
