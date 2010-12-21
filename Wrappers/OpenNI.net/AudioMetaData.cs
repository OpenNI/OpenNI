using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class AudioMetaData : OutputMetaData
	{
		public AudioMetaData()
		{
		}

		public byte NumberOfChannels
		{
			get { return this.audio.Wave.nChannels; }
			set { this.audio.Wave.nChannels = value; }
		}

		public UInt32 SampleRate
		{
			get { return this.audio.Wave.nSampleRate; }
			set { this.audio.Wave.nSampleRate = value; }
		}

		public UInt16 BitsPerSample
		{
			get { return this.audio.Wave.nBitsPerSample; }
			set { this.audio.Wave.nBitsPerSample = value; }
		}

		public IntPtr AudioBufferPtr
		{
			get { return this.audio.pData; }
		}

		public new IMarshaler GetMarshaler(bool passOut)
		{
			return new AudioMetaDataMarshaler(this, passOut);
		}

		private OpenNIImporter.XnAudioMetaData audio = new OpenNIImporter.XnAudioMetaData();

		private class AudioMetaDataMarshaler : Marshaler<OpenNIImporter.XnAudioMetaData>
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
