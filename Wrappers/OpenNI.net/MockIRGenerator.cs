using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class MockIRGenerator : IRGenerator
	{
		internal MockIRGenerator(IntPtr nodeHandle) :
			base(nodeHandle)
		{
		}

		public MockIRGenerator(Context context, string name) :
			this(Create(context, name))
		{
		}

		public MockIRGenerator(Context context) :
			this(context, null)
		{
		}

		public MockIRGenerator(IRGenerator basedOn, string name) :
			this(CreateBasedOn(basedOn, name))
		{
		}

		public MockIRGenerator(IRGenerator basedOn) :
			this(basedOn, null)
		{
		}

		public void SetData(UInt32 frameID, UInt64 timestamp, UInt32 dataSize, IntPtr buffer)
		{
			UInt32 status = OpenNIImporter.xnMockIRSetData(this.InternalObject, frameID, timestamp, dataSize, buffer);
			WrapperUtils.CheckStatus(status);
		}

		public void SetData(IRMetaData irMD, UInt32 frameID, UInt64 timestamp)
		{
			SetData(frameID, timestamp, irMD.DataSize, irMD.IRMapPtr);
		}

		public void SetData(IRMetaData irMD)
		{
			SetData(irMD, irMD.FrameID, irMD.Timestamp);
		}

		private static IntPtr Create(Context context, string name)
		{
			IntPtr handle;
			UInt32 status = OpenNIImporter.xnCreateMockNode(context.InternalObject, NodeType.IR, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}

		private static IntPtr CreateBasedOn(IRGenerator basedOn, string name)
		{
			IntPtr handle;
			UInt32 status = OpenNIImporter.xnCreateMockNodeBasedOn(basedOn.GetContext().InternalObject, 
				basedOn.InternalObject, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}
	}
}