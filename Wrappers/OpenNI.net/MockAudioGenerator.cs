using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MockAudioGenerator : AudioGenerator
	{
		internal MockAudioGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public MockAudioGenerator(Context context, string name) :
			this(Create(context, name), false)
		{
		}

		public MockAudioGenerator(Context context) :
			this(context, null)
		{
		}

		public MockAudioGenerator(AudioGenerator basedOn, string name) :
			this(CreateBasedOn(basedOn, name), false)
		{
		}

		public MockAudioGenerator(AudioGenerator basedOn) :
			this(basedOn, null)
		{
		}

		public void SetData(UInt32 frameID, UInt64 timestamp, UInt32 dataSize, IntPtr buffer)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnMockAudioSetData(this.InternalObject, frameID, timestamp, dataSize, buffer));
			
		}

		public void SetData(AudioMetadata audioMetadata, UInt32 frameID, UInt64 timestamp)
		{
			SetData(frameID, timestamp, audioMetadata.DataSize, audioMetadata.AudioBufferPtr);
		}

		public void SetData(AudioMetadata audioMetadata)
		{
			SetData(audioMetadata, audioMetadata.FrameId, audioMetadata.Timestamp);
		}

		private static NodeSafeHandle Create(Context context, string name)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateMockNode(context.InternalObject, NodeType.Audio, name, out handle));
			
			return handle;
		}

        private static NodeSafeHandle CreateBasedOn(AudioGenerator basedOn, string name)
		{
            NodeSafeHandle handle;
			Status.ThrowOnFail(SafeNativeMethods.xnCreateMockNodeBasedOn(basedOn.Context.InternalObject,
                basedOn.InternalObject, name, out handle));
			
			return handle;
		}
	}
}