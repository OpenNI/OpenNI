using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class IRGenerator : MapGenerator
	{
		internal IRGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public IRGenerator(Context context, Query query, EnumerationErrors errors) :
			this(Create(context, query, errors), false)
		{
		}

		public IRGenerator(Context context, Query query) :
			this(context, query, null)
		{
		}

		public IRGenerator(Context context) :
			this(context, null, null)
		{
		}

		public IntPtr GetIRMapPtr()
		{
			return OpenNIImporter.xnGetIRMap(this.InternalObject);
		}

		public MapData<UInt16> GetIRMap()
		{
			return GetMapData<UInt16>(GetIRMapPtr());
		}

		public void GetMetaData(IRMetaData irMD)
		{
			using (IMarshaler marsh = irMD.GetMarshaler(true))
			{
				OpenNIImporter.xnGetIRMetaData(this.InternalObject, marsh.Native);
			}
		}

		public IRMetaData MetaData
		{
            get
            {
                IRMetaData irMD = new IRMetaData();
                GetMetaData(irMD);
                return irMD;
            }
		}

        private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateIRGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
                errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
			
			return handle;
		}
	}
}