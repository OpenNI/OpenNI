using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
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
                Status.ThrowOnFail(OpenNIImporter.xnGetSupportedWaveOutputModes(this.InternalObject, modes, ref count));
                
                return modes;
            }
		}

		public WaveOutputMode WaveOutputMode
		{
            get
            {
                WaveOutputMode mode = new WaveOutputMode();
                Status.ThrowOnFail(OpenNIImporter.xnGetWaveOutputMode(this.InternalObject, ref mode));
                
                return mode;
            }
            set
            {
                Status.ThrowOnFail(OpenNIImporter.xnSetWaveOutputMode(this.InternalObject, ref value));
			    
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
			Status.ThrowOnFail(OpenNIImporter.xnCreateAudioGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
                errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
			
			return handle;
		}

		private StateChangedEvent outputModeChanged;
	}
}