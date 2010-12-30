using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
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
            UInt32 status = OpenNIImporter.xnNodeQueryAllocate(out handle);
			WrapperUtils.CheckStatus(status);
            return new Query(handle);
		}

        internal new QuerySafeHandle InternalObject
        {
            get { return (QuerySafeHandle)base.InternalObject; }
        }

        public void SetVendor(string vendorName)
		{
			UInt32 status = OpenNIImporter.xnNodeQuerySetVendor(this.InternalObject, vendorName);
			WrapperUtils.CheckStatus(status);
		}

		public void SetName(string name)
		{
			UInt32 status = OpenNIImporter.xnNodeQuerySetName(this.InternalObject, name);
			WrapperUtils.CheckStatus(status);
		}

		public void SetMinVersion(Version min)
		{
			UInt32 status = OpenNIImporter.xnNodeQuerySetMinVersion(this.InternalObject, ref min);
			WrapperUtils.CheckStatus(status);
		}

		public void SetMaxVersion(Version max)
		{
			UInt32 status = OpenNIImporter.xnNodeQuerySetMaxVersion(this.InternalObject, ref max);
			WrapperUtils.CheckStatus(status);
		}

		public void AddSupportedCapability(string capName)
		{
			UInt32 status = OpenNIImporter.xnNodeQueryAddSupportedCapability(this.InternalObject, capName);
			WrapperUtils.CheckStatus(status);
		}

		public void AddSupportedMapOutputMode(MapOutputMode mode)
		{
			UInt32 status = OpenNIImporter.xnNodeQueryAddSupportedMapOutputMode(this.InternalObject, ref mode);
			WrapperUtils.CheckStatus(status);
		}

		public void SetSupportedMinUserPositions(uint count)
		{
			UInt32 status = OpenNIImporter.xnNodeQuerySetSupportedMinUserPositions(this.InternalObject, count);
			WrapperUtils.CheckStatus(status);
		}

		public void SetExistingNodeOnly(bool existing)
		{
			UInt32 status = OpenNIImporter.xnNodeQuerySetExistingNodeOnly(this.InternalObject, existing);
			WrapperUtils.CheckStatus(status);
		}

		public void AddNeededNode(string nodeName)
		{
			UInt32 status = OpenNIImporter.xnNodeQueryAddNeededNode(this.InternalObject, nodeName);
			WrapperUtils.CheckStatus(status);
		}

		public void SetCreationInfo(string creationInfo)
		{
			UInt32 status = OpenNIImporter.xnNodeQuerySetCreationInfo(this.InternalObject, creationInfo);
			WrapperUtils.CheckStatus(status);
		}

		public void FilterList(Context context, NodeInfoList list)
		{
			UInt32 status = OpenNIImporter.xnNodeQueryFilterList(context.InternalObject, this.InternalObject, list.InternalObject);
			WrapperUtils.CheckStatus(status);
		}
	}
}