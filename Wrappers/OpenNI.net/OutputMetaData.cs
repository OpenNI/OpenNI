using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class OutputMetaData
	{
		public Int64 Timestamp
		{
			get { return (Int64)this.data.nTimestamp; }
			set { this.data.nTimestamp = (UInt64)value; }
		}

		public int FrameID
		{
			get { return (int)this.data.nFrameID; }
			set { this.data.nFrameID = (UInt32)value; }
		}

		public int DataSize
		{
			get { return (int)this.data.nDataSize; }
			set { this.data.nDataSize = (UInt32)value; }
		}

		public bool IsDataNew
		{
			get { return this.data.bIsNew; }
			set { this.data.bIsNew = value; }
		}

		internal IMarshaler GetMarshaler(bool passOut)
		{
			return new OutputMetaDataMarshaler(this, passOut);
		}

		internal class OutputMetaDataMarshaler : Marshaler<SafeNativeMethods.XnOutputMetaData>
		{
			public OutputMetaDataMarshaler(OutputMetaData output, bool marshalOut) :
				base(output.data, marshalOut)
			{
			}
		}

		internal SafeNativeMethods.XnOutputMetaData data = new SafeNativeMethods.XnOutputMetaData();
	}
}
