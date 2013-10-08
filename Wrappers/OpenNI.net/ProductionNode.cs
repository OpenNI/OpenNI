/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class ProductionNode : NodeWrapper
	{
		internal ProductionNode(Context context, IntPtr nodeHandle, bool addRef)
			: base(nodeHandle, addRef)
		{
			this.context = context;
		}

		public static ProductionNode FromNative(IntPtr nodeHandle)
		{
			return Context.CreateProductionNodeFromNative(nodeHandle);
		}

		public NodeInfo Info
		{
			get
			{
				return new NodeInfo(SafeNativeMethods.xnGetNodeInfo(this.InternalObject));
			}
		}

		public void AddNeededNode(ProductionNode needed)
		{
			int status = SafeNativeMethods.xnAddNeededNode(this.InternalObject, needed.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void RemoveNeededNode(ProductionNode needed)
		{
			int status = SafeNativeMethods.xnRemoveNeededNode(this.InternalObject, needed.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public Context Context
		{
			get
			{
				return this.context;
			}
		}

		public bool IsCapabilitySupported(string capabilityName)
		{
			return SafeNativeMethods.xnIsCapabilitySupported(this.InternalObject, capabilityName);
		}

		public void SetIntProperty(string propName, Int64 value)
		{
			int status = SafeNativeMethods.xnSetIntProperty(this.InternalObject, propName, (UInt64)value);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetRealProperty(string propName, double value)
		{
			int status = SafeNativeMethods.xnSetRealProperty(this.InternalObject, propName, value);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetStringProperty(string propName, string value)
		{
			int status = SafeNativeMethods.xnSetStringProperty(this.InternalObject, propName, value);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetGeneralProperty(string propName, int size, IntPtr buff)
		{
			int status = SafeNativeMethods.xnSetGeneralProperty(this.InternalObject, propName, (UInt32)size, buff);
			WrapperUtils.ThrowOnError(status);
		}

		public Int64 GetIntProperty(string propName)
		{
			UInt64 value;
			int status = SafeNativeMethods.xnGetIntProperty(this.InternalObject, propName, out value);
			WrapperUtils.ThrowOnError(status);
			return (Int64)value;
		}

		public double GetRealProperty(string propName)
		{
			double value;
			int status = SafeNativeMethods.xnGetRealProperty(this.InternalObject, propName, out value);
			WrapperUtils.ThrowOnError(status);
			return value;
		}

		public string GetStringProperty(string propName)
		{
			const int size = 2048;
			StringBuilder sb = new StringBuilder(size);
			int status = SafeNativeMethods.xnGetStringProperty(this.InternalObject, propName, sb, size);
			WrapperUtils.ThrowOnError(status);
			return sb.ToString();
		}

		public void GetGeneralProperty(string propName, int size, IntPtr buff)
		{
			int status = SafeNativeMethods.xnGetGeneralProperty(this.InternalObject, propName, (UInt32)size, buff);
			WrapperUtils.ThrowOnError(status);
		}

		public LockHandle LockForChanges()
		{
			int handle;
			int status = SafeNativeMethods.xnLockNodeForChanges(this.InternalObject, out handle);
			WrapperUtils.ThrowOnError(status);
			return new LockHandle(handle);
		}

		public void UnlockForChanges(LockHandle lockHandle)
		{
			int status = SafeNativeMethods.xnUnlockNodeForChanges(this.InternalObject, lockHandle.InternalHandle);
			WrapperUtils.ThrowOnError(status);
		}

		public void LockedNodeStartChanges(LockHandle lockHandle)
		{
			int status = SafeNativeMethods.xnLockedNodeStartChanges(this.InternalObject, lockHandle.InternalHandle);
			WrapperUtils.ThrowOnError(status);
		}

		public void LockedNodeEndChanges(LockHandle lockHandle)
		{
			int status = SafeNativeMethods.xnLockedNodeEndChanges(this.InternalObject, lockHandle.InternalHandle);
			WrapperUtils.ThrowOnError(status);
		}

		public GeneralIntCapability GetGeneralIntCapability(string capabilityName)
		{
			return new GeneralIntCapability(this, capabilityName);
		}

		private Context context;
	}
}