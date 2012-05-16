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
	public class NodeInfoList : ObjectWrapper, IEnumerable<NodeInfo>
	{
		internal NodeInfoList(IntPtr pList, bool bOwn)
			: base(pList)
		{
            this.owns = bOwn;
		}

        internal NodeInfoList(IntPtr pList)
            : this(pList, true)
        {
        }

        /// <summary>
		/// Creates a managed NodeInfoList object to wrap a native one.
		/// </summary>
		/// <param name="pList">The native node info list pointer</param>
        /// <param name="bOwn">Should the native object be freed when this object is destroyed</param>
		/// <returns>A managed NodeInfoList object</returns>
		static public NodeInfoList FromNative(IntPtr pList, bool bOwn)
		{
			return new NodeInfoList(pList, bOwn);
		}

        /// <summary>
        /// Creates a managed NodeInfoList object to wrap a native one.
        /// </summary>
        /// <param name="pList">The native node info list pointer</param>
        /// <returns>A managed NodeInfoList object</returns>
        static public NodeInfoList FromNative(IntPtr pList)
        {
            return FromNative(pList, true);
        }

		#region NodeInfoListEnumerator Class

		private class NodeInfoListEnumerator : IEnumerator<NodeInfo>
		{
			public NodeInfoListEnumerator(NodeInfoList list)
			{
				this.list = list;
			}

			#region IEnumerator<NodeInfo> Members

			public NodeInfo Current
			{
				get
				{
					return new NodeInfo(SafeNativeMethods.xnNodeInfoListGetCurrent(it));
				}
			}

			#endregion

			#region IDisposable Members

			public void Dispose()
			{
			}

			#endregion

			#region IEnumerator Members

			object System.Collections.IEnumerator.Current
			{
				get { return ((IEnumerator<NodeInfo>)this).Current; }
			}

			public bool MoveNext()
			{
				if (this.reset)
				{
					it = SafeNativeMethods.xnNodeInfoListGetFirst(list.InternalObject);
					this.reset = false;
				}
				else
				{
					it = SafeNativeMethods.xnNodeInfoListGetNext(it);
				}

				return SafeNativeMethods.xnNodeInfoListIteratorIsValid(it);
			}

			public void Reset()
			{
				this.reset = true;
			}

			#endregion

			internal IntPtr InternalObject
			{
				get { return this.it; }
			}

			private NodeInfoList list;
			private IntPtr it;
			private bool reset = true;
		}

		#endregion

		#region IEnumerable<NodeInfo> Members

		public IEnumerator<NodeInfo> GetEnumerator()
		{
			return new NodeInfoListEnumerator(this);
		}

		#endregion

		#region IEnumerable Members

		System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
		{
			return new NodeInfoListEnumerator(this);
		}

		#endregion

		public void Add(ProductionNodeDescription description, string creationInfo, NodeInfoList neededNodes)
		{
			int status = SafeNativeMethods.xnNodeInfoListAdd(this.InternalObject, description, creationInfo,
				neededNodes == null ? IntPtr.Zero : neededNodes.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddNode(NodeInfo nodeInfo)
		{
			int status = SafeNativeMethods.xnNodeInfoListAddNode(this.InternalObject, nodeInfo.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddNodeFromList(IEnumerator<NodeInfo> current)
		{
			NodeInfoListEnumerator enumer = (NodeInfoListEnumerator)current;
			int status = SafeNativeMethods.xnNodeInfoListAddNodeFromList(this.InternalObject, enumer.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void Remove(IEnumerator<NodeInfo> location)
		{
			NodeInfoListEnumerator enumer = (NodeInfoListEnumerator)location;
			int status = SafeNativeMethods.xnNodeInfoListRemove(this.InternalObject, enumer.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void Clear()
		{
			int status = SafeNativeMethods.xnNodeInfoListClear(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void Append(NodeInfoList other)
		{
			int status = SafeNativeMethods.xnNodeInfoListAppend(this.InternalObject, other.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public bool IsEmpty()
		{
			return SafeNativeMethods.xnNodeInfoListIsEmpty(this.InternalObject);
		}

		public void Filter(Context context, Query query)
		{
			query.FilterList(context, this);
		}

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
            if (this.owns)
			    SafeNativeMethods.xnNodeInfoListFree(ptr);
		}

        private bool owns;
	}
}