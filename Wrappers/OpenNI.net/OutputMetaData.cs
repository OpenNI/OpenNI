using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class OutputMetaData
	{
		public UInt64 Timestamp
		{
			get { return this.data.nTimestamp; }
			set { this.data.nTimestamp = value; }
		}

		public UInt32 FrameID
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
			return new OutputMetaDataMarshaler(this, passOut);
		}

		internal class OutputMetaDataMarshaler : Marshaler<OpenNIImporter.XnOutputMetaData>
		{
			public OutputMetaDataMarshaler(OutputMetaData output, bool marshalOut) :
				base(output.data, marshalOut)
			{
			}
		}

		internal OpenNIImporter.XnOutputMetaData data = new OpenNIImporter.XnOutputMetaData();
	}
}
