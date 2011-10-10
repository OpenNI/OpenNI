/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class Query : ObjectWrapper
	{
		public Query() : 
			this(Create())
		{
		}

		internal Query(IntPtr pQuery) :
			base(pQuery)
		{
		}

		/// <summary>
		/// Creates a managed Query object to wrap a native one.
		/// </summary>
		/// <param name="pQuery">The native query pointer</param>
		/// <returns>A managed Query object</returns>
		static public Query FromNative(IntPtr pQuery)
		{
			return new Query(pQuery);
		}

		public void SetVendor(string vendorName)
		{
			int status = SafeNativeMethods.xnNodeQuerySetVendor(this.InternalObject, vendorName);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetName(string name)
		{
			int status = SafeNativeMethods.xnNodeQuerySetName(this.InternalObject, name);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetMinVersion(Version min)
		{
			int status = SafeNativeMethods.xnNodeQuerySetMinVersion(this.InternalObject, ref min);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetMaxVersion(Version max)
		{
			int status = SafeNativeMethods.xnNodeQuerySetMaxVersion(this.InternalObject, ref max);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddSupportedCapability(string capName)
		{
			int status = SafeNativeMethods.xnNodeQueryAddSupportedCapability(this.InternalObject, capName);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddSupportedMapOutputMode(MapOutputMode mode)
		{
			int status = SafeNativeMethods.xnNodeQueryAddSupportedMapOutputMode(this.InternalObject, ref mode);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetSupportedMinUserPositions(Int32 count)
		{
			int status = SafeNativeMethods.xnNodeQuerySetSupportedMinUserPositions(this.InternalObject, (UInt32)count);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetExistingNodeOnly(bool existing)
		{
			int status = SafeNativeMethods.xnNodeQuerySetExistingNodeOnly(this.InternalObject, existing);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddNeededNode(string nodeName)
		{
			int status = SafeNativeMethods.xnNodeQueryAddNeededNode(this.InternalObject, nodeName);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetCreationInfo(string creationInfo)
		{
			int status = SafeNativeMethods.xnNodeQuerySetCreationInfo(this.InternalObject, creationInfo);
			WrapperUtils.ThrowOnError(status);
		}

		public void FilterList(Context context, NodeInfoList list)
		{
			int status = SafeNativeMethods.xnNodeQueryFilterList(context.InternalObject, this.InternalObject, list.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
			SafeNativeMethods.xnNodeQueryFree(ptr);
		}

		static private IntPtr Create()
		{
			IntPtr pQuery;
			int status = SafeNativeMethods.xnNodeQueryAllocate(out pQuery);
			WrapperUtils.ThrowOnError(status);
			return pQuery;
		}
	}
}