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
			UInt32 status = OpenNIImporter.xnMockAudioSetData(this.InternalObject, frameID, timestamp, dataSize, buffer);
			WrapperUtils.CheckStatus(status);
		}

		public void SetData(AudioMetaData audioMD, UInt32 frameID, UInt64 timestamp)
		{
			SetData(frameID, timestamp, audioMD.DataSize, audioMD.AudioBufferPtr);
		}

		public void SetData(AudioMetaData audioMD)
		{
			SetData(audioMD, audioMD.FrameID, audioMD.Timestamp);
		}

		private static NodeSafeHandle Create(Context context, string name)
		{
            NodeSafeHandle handle;
            UInt32 status = OpenNIImporter.xnCreateMockNode(context.InternalObject, NodeType.Audio, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}

        private static NodeSafeHandle CreateBasedOn(AudioGenerator basedOn, string name)
		{
            NodeSafeHandle handle;
			UInt32 status = OpenNIImporter.xnCreateMockNodeBasedOn(basedOn.GetContext().InternalObject,
                basedOn.InternalObject, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}
	}
}