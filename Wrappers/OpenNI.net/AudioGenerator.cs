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
				SafeNativeMethods.xnRegisterToWaveOutputModeChanges,
				SafeNativeMethods.xnUnregisterFromWaveOutputModeChanges);
		}

		public AudioGenerator(Context context, Query query, ErrorCollection errors) :
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
            uint count = SafeNativeMethods.xnGetSupportedWaveOutputModesCount(this.InternalObject);
            WaveOutputMode[] modes = new WaveOutputMode[count];
            Status.ThrowOnFail(SafeNativeMethods.xnGetSupportedWaveOutputModes(this.InternalObject, modes, ref count));                
            return modes;
		}

		public WaveOutputMode WaveOutputMode
		{
            get
            {
                WaveOutputMode mode = new WaveOutputMode();
                Status.ThrowOnFail(SafeNativeMethods.xnGetWaveOutputMode(this.InternalObject, ref mode));
                
                return mode;
            }
            set
            {
                Status.ThrowOnFail(SafeNativeMethods.xnSetWaveOutputMode(this.InternalObject, ref value));
			    
            }
		}

		public IntPtr AudioBufferPtr
		{
            get
            {
                return SafeNativeMethods.xnGetAudioBuffer(this.InternalObject);
            }
		}

		public void GetMetadata(AudioMetadata audioMetadata)
		{
			using (IMarshaler marsh = audioMetadata.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetAudioMetaData(this.InternalObject, marsh.Native);
			}
		}

		public AudioMetadata Metadata
		{
            get
            {
                AudioMetadata audioMetadata = new AudioMetadata();
                GetMetadata(audioMetadata);
                return audioMetadata;
            }
		}

		public event EventHandler<StateChangedArgs> WaveOutputModeChanged
		{
			add { this.outputModeChanged.Event += value; }
			remove { this.outputModeChanged.Event -= value; }
		}

		private static NodeSafeHandle Create(Context context, Query query, ErrorCollection errors)
		{
            NodeSafeHandle handle;
			Status.ThrowOnFail(SafeNativeMethods.xnCreateAudioGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
                errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
			
			return handle;
		}

		private StateChangedEvent outputModeChanged;
	}
}