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
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenNI
{
	public class NodeInfo : ObjectWrapper
	{
		internal NodeInfo(IntPtr pNodeInfo)
			: base(pNodeInfo)
		{
		}

		/// <summary>
		/// Creates a managed NodeInfo object to wrap a native one.
		/// </summary>
		/// <param name="pNodeInfo">The native node info pointer</param>
		/// <returns>A managed NodeInfo object</returns>
		static public NodeInfo FromNative(IntPtr pNodeInfo)
		{
			return new NodeInfo(pNodeInfo);
		}

		public ProductionNodeDescription Description
		{
			get
			{
                IntPtr pDescription = SafeNativeMethods.xnNodeInfoGetDescription(this.InternalObject);
                ProductionNodeDescription description = ProductionNodeDescription.Empty;
                return (ProductionNodeDescription)Marshal.PtrToStructure(pDescription, typeof(ProductionNodeDescription));
			}
		}

		public string InstanceName
		{
			get
			{
				return Marshal.PtrToStringAnsi(SafeNativeMethods.xnNodeInfoGetInstanceName(this.InternalObject));
			}
			set
			{
				int status = SafeNativeMethods.xnNodeInfoSetInstanceName(this.InternalObject, value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		public string CreationInfo
		{
			get
			{
				return Marshal.PtrToStringAnsi(SafeNativeMethods.xnNodeInfoGetCreationInfo(this.InternalObject));
			}
		}

		public NodeInfoList NeededNodes
		{
			get
			{
				return new NodeInfoList(SafeNativeMethods.xnNodeInfoGetNeededNodes(this.InternalObject), false);
			}
		}

		public ProductionNode Instance
		{
			get
			{
				IntPtr handle = SafeNativeMethods.xnNodeInfoGetRefHandle(this.InternalObject);
				if (handle == IntPtr.Zero)
					return null;
				else
				{
					ProductionNode result = ProductionNode.FromNative(handle);
					SafeNativeMethods.xnProductionNodeRelease(handle);
					return result;
				}
			}
		}

        public override string ToString()
        {
            const int size = 4096;
            StringBuilder sb = new StringBuilder(size);
            int status = SafeNativeMethods.xnNodeInfoGetTreeStringRepresentation(this.InternalObject, sb, size);
            WrapperUtils.ThrowOnError(status);
            return sb.ToString();
        }

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
			// no need to free anything
		}
	}
}