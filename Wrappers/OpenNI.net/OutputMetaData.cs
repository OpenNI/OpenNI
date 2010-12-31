using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public abstract class OutputMetadata
	{
		public UInt64 Timestamp
		{
			get { return this.data.nTimestamp; }
			set { this.data.nTimestamp = value; }
		}

		public UInt32 FrameId
		{
			get { return this.data.nFrameID; }
			set { this.data.nFrameID = value; }
		}

		public UInt32 DataSize
		{
			get { return this.data.nDataSize; }
			set { this.data.nDataSize = value; }
		}

		public bool IsDataNew
		{
			get { return this.data.bIsNew; }
			set { this.data.bIsNew = value; }
		}

		internal IMarshaler GetMarshaler(bool passOut)
		{
			return new OutputMetadataMarshaler(this, passOut);
		}

		internal class OutputMetadataMarshaler : Marshaler<SafeNativeMethods.XnOutputMetadata>
		{
			public OutputMetadataMarshaler(OutputMetadata output, bool marshalOut) :
				base(output.data, marshalOut)
			{
			}
		}

		internal SafeNativeMethods.XnOutputMetadata data = new SafeNativeMethods.XnOutputMetadata();
	}
}
