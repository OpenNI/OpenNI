using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class AudioGenerator : Generator
	{
		internal AudioGenerator(NodeSafeHandle nodeHandle, bool addRef) :
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

		public WaveOutputMode[] SupportedWaveOutputModes
		{
            get
            {
                uint count = OpenNIImporter.xnGetSupportedWaveOutputModesCount(this.InternalObject);
                WaveOutputMode[] modes = new WaveOutputMode[count];
                UInt32 status = OpenNIImporter.xnGetSupportedWaveOutputModes(this.InternalObject, modes, ref count);
                WrapperUtils.CheckStatus(status);
                return modes;
            }
		}

		public WaveOutputMode WaveOutputMode
		{
            get
            {
                WaveOutputMode mode = new WaveOutputMode();
                UInt32 status = OpenNIImporter.xnGetWaveOutputMode(this.InternalObject, ref mode);
                WrapperUtils.CheckStatus(status);
                return mode;
            }
            set
            {
			    UInt32 status = OpenNIImporter.xnSetWaveOutputMode(this.InternalObject, ref value);
			    WrapperUtils.CheckStatus(status);
            }
		}

		public IntPtr AudioBufferPtr
		{
            get
            {
                return OpenNIImporter.xnGetAudioBuffer(this.InternalObject);
            }
		}

		public void GetMetaData(AudioMetaData audioMD)
		{
			using (IMarshaler marsh = audioMD.GetMarshaler(true))
			{
				OpenNIImporter.xnGetAudioMetaData(this.InternalObject, marsh.Native);
			}
		}

		public AudioMetaData MetaData
		{
            get
            {
                AudioMetaData audioMD = new AudioMetaData();
                GetMetaData(audioMD);
                return audioMD;
            }
		}

		public event StateChangedHandler WaveOutputModeChanged
		{
			add { this.outputModeChanged.Event += value; }
			remove { this.outputModeChanged.Event -= value; }
		}

		private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
		{
            NodeSafeHandle handle;
			UInt32 status = OpenNIImporter.xnCreateAudioGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
				errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject);
			WrapperUtils.CheckStatus(status);
			return handle;
		}

		private StateChangedEvent outputModeChanged;
	}
}