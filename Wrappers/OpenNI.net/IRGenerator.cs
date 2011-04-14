using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class IRGenerator : MapGenerator
	{
		internal IRGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public IRGenerator(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
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

		public IntPtr IRMapPtr
		{
			get
			{
				return SafeNativeMethods.xnGetIRMap(this.InternalObject);
			}
		}

		public UInt16MapData GetIRMap()
		{
			MapOutputMode mode = this.MapOutputMode;
			return new UInt16MapData(mode.XRes, mode.YRes, IRMapPtr);
		}

		public void GetMetaData(IRMetaData irMD)
		{
			using (IMarshaler marsh = irMD.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetIRMetaData(this.InternalObject, marsh.Native);
			}
		}

		public IRMetaData GetMetaData()
		{
			IRMetaData irMD = new IRMetaData();
			GetMetaData(irMD);
			return irMD;
		}

		private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateIRGenerator(context.InternalObject, out handle,
				query == null ? IntPtr.Zero : query.InternalObject,
				errors == null ? IntPtr.Zero : errors.InternalObject);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}
	}
}