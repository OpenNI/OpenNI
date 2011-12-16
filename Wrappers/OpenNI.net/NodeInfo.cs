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
				return new NodeInfoList(SafeNativeMethods.xnNodeInfoGetNeededNodes(this.InternalObject));
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

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
			// no need to free anything
		}
	}
}