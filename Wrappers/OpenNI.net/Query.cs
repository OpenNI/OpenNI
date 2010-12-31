using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class Query : ObjectWrapper
	{
		internal Query(QuerySafeHandle pQuery) :
			base(pQuery)
		{
		}

		/// <summary>
		/// Creates a managed Query object to wrap a native one.
		/// </summary>
		/// <param name="pQuery">The native query pointer</param>
		/// <returns>A managed Query object</returns>
        static internal Query FromNative(QuerySafeHandle pQuery)
		{
			return new Query(pQuery);
		}

		/// <summary>
		/// Creates a managed Query object.
		/// </summary>
		/// <returns>A managed Query object</returns>
		static public Query Create()
		{
            QuerySafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQueryAllocate(out handle));
            return new Query(handle);
		}

        internal new QuerySafeHandle InternalObject
        {
            get { return (QuerySafeHandle)base.InternalObject; }
        }

        public void SetVendor(string vendorName)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnNodeQuerySetVendor(this.InternalObject, vendorName));
		}

		public void SetName(string name)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQuerySetName(this.InternalObject, name));
		}

		public void SetMinVersion(Version min)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQuerySetMinVersion(this.InternalObject, ref min));
		}

		public void SetMaxVersion(Version max)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQuerySetMaxVersion(this.InternalObject, ref max));
		}

		public void AddSupportedCapability(string capName)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQueryAddSupportedCapability(this.InternalObject, capName));
		}

		public void AddSupportedMapOutputMode(MapOutputMode mode)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQueryAddSupportedMapOutputMode(this.InternalObject, ref mode));
		}

		public void SetSupportedMinUserPositions(uint count)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQuerySetSupportedMinUserPositions(this.InternalObject, count));
		}

		public void SetExistingNodeOnly(bool existing)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQuerySetExistingNodeOnly(this.InternalObject, existing));
		}

		public void AddNeededNode(string nodeName)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQueryAddNeededNode(this.InternalObject, nodeName));
		}

		public void SetCreationInfo(string creationInfo)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQuerySetCreationInfo(this.InternalObject, creationInfo));
		}

		public void FilterList(Context context, NodeInfoCollection list)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnNodeQueryFilterList(context.InternalObject, this.InternalObject, list.InternalObject));
		}
	}
}