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

		public IRGenerator(Context context, Query query, ErrorCollection errors) :
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
			return SafeNativeMethods.xnGetIRMap(this.InternalObject);
		}

		public MapData<UInt16> GetIRMap()
		{
			return GetMapData<UInt16>(GetIRMapPtr());
		}

		public void GetMetadata(IRMetadata irMetadata)
		{
			using (IMarshaler marsh = irMetadata.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetIRMetadata(this.InternalObject, marsh.Native);
			}
		}

		public IRMetadata Metadata
		{
            get
            {
                IRMetadata irMetadata = new IRMetadata();
                GetMetadata(irMetadata);
                return irMetadata;
            }
		}

        private static NodeSafeHandle Create(Context context, Query query, ErrorCollection errors)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateIRGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
                errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
			
			return handle;
		}
	}
}