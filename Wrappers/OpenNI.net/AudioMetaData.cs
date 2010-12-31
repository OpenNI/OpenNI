using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class AudioMetadata : OutputMetadata
	{
		public AudioMetadata()
		{
		}

		public byte NumberOfChannels
		{
			get { return this.audio.Wave.Channels; }
			set { this.audio.Wave.Channels = value; }
		}

		public UInt32 SampleRate
		{
			get { return this.audio.Wave.SampleRate; }
			set { this.audio.Wave.SampleRate = value; }
		}

		public UInt16 BitsPerSample
		{
			get { return this.audio.Wave.BitsPerSample; }
			set { this.audio.Wave.BitsPerSample = value; }
		}

		public IntPtr AudioBufferPtr
		{
			get { return this.audio.pData; }
		}

		public new IMarshaler GetMarshaler(bool passOut)
		{
			return new AudioMetadataMarshaler(this, passOut);
		}

		private SafeNativeMethods.XnAudioMetadata audio = new SafeNativeMethods.XnAudioMetadata();

		private class AudioMetadataMarshaler : Marshaler<SafeNativeMethods.XnAudioMetadata>
		{
			public AudioMetadataMarshaler(AudioMetadata obj, bool marshalOut) :
				base(obj.audio, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(AudioMetadata obj, bool marshalOut)
			{
				IMarshaler inner = ((OutputMetadata)obj).GetMarshaler(marshalOut);
				obj.audio.pOutput = inner.Native;
				return inner;
			}
		}
	}
}
