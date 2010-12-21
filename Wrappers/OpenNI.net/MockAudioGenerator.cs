using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class MockAudioGenerator : AudioGenerator
	{
		internal MockAudioGenerator(IntPtr nodeHandle) :
			base(nodeHandle)
		{
		}

		public MockAudioGenerator(Context context, string name) :
			this(Create(context, name))
		{
		}

		public MockAudioGenerator(Context context) :
			this(context, null)
		{
		}

		public MockAudioGenerator(AudioGenerator basedOn, string name) :
			this(CreateBasedOn(basedOn, name))
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

		private static IntPtr Create(Context context, string name)
		{
			IntPtr handle;
			UInt32 status = OpenNIImporter.xnCreateMockNode(context.InternalObject, NodeType.Audio, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}

		private static IntPtr CreateBasedOn(AudioGenerator basedOn, string name)
		{
			IntPtr handle;
			UInt32 status = OpenNIImporter.xnCreateMockNodeBasedOn(basedOn.GetContext().InternalObject, 
				basedOn.InternalObject, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}
	}
}