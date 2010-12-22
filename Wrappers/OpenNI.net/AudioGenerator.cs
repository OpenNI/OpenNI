using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class AudioGenerator : Generator
	{
		internal AudioGenerator(IntPtr nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
			this.outputModeChanged = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToWaveOutputModeChanges,
				OpenNIImporter.xnUnregisterFromWaveOutputModeChanges);
		}

		public AudioGenerator(Context context, Query query, EnumerationErrors errors) :
			this(Create(context, query, errors), false)
		{
		}

		public AudioGenerator(Context context, Query query) :
			this(context, query, null)
		{
		}

		public AudioGenerator(Context context) :
			this(context, null, null)
		{
		}

		public WaveOutputMode[] GetSupportedWaveOutputModes()
		{
			uint count = OpenNIImporter.xnGetSupportedWaveOutputModesCount(this.InternalObject);
			WaveOutputMode[] modes = new WaveOutputMode[count];
			UInt32 status = OpenNIImporter.xnGetSupportedWaveOutputModes(this.InternalObject, modes, ref count);
			WrapperUtils.CheckStatus(status);
			return modes;
		}

		public void SetWaveOutputMode(WaveOutputMode mode)
		{
			UInt32 status = OpenNIImporter.xnSetWaveOutputMode(this.InternalObject, ref mode);
			WrapperUtils.CheckStatus(status);
		}

		public WaveOutputMode GetWaveOutputMode()
		{
			WaveOutputMode mode = new WaveOutputMode();
			UInt32 status = OpenNIImporter.xnGetWaveOutputMode(this.InternalObject, ref mode);
			WrapperUtils.CheckStatus(status);
			return mode;
		}

		public IntPtr GetAudioBufferPtr()
		{
			return OpenNIImporter.xnGetAudioBuffer(this.InternalObject);
		}

		public void GetMetaData(AudioMetaData audioMD)
		{
			using (IMarshaler marsh = audioMD.GetMarshaler(true))
			{
				OpenNIImporter.xnGetAudioMetaData(this.InternalObject, marsh.Native);
			}
		}

		public AudioMetaData GetMetaData()
		{
			AudioMetaData audioMD = new AudioMetaData();
			GetMetaData(audioMD);
			return audioMD;
		}

		public event StateChangedHandler WaveOutputModeChanged
		{
			add { this.outputModeChanged.Event += value; }
			remove { this.outputModeChanged.Event -= value; }
		}

		private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
		{
			IntPtr handle;
			UInt32 status = OpenNIImporter.xnCreateAudioGenerator(context.InternalObject, out handle,
				query == null ? IntPtr.Zero : query.InternalObject,
				errors == null ? IntPtr.Zero : errors.InternalObject);
			WrapperUtils.CheckStatus(status);
			return handle;
		}

		private StateChangedEvent outputModeChanged;
	}
}