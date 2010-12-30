using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class NodeInfoList : ObjectWrapper, IEnumerable<NodeInfo>
	{
        internal NodeInfoList(NodeInfoListSafeHandle pList)
			: base(pList)
		{
		}

		/// <summary>
		/// Creates a managed NodeInfoList object to wrap a native one.
		/// </summary>
		/// <param name="pList">The native node info list pointer</param>
		/// <returns>A managed NodeInfoList object</returns>
        static internal NodeInfoList FromNative(NodeInfoListSafeHandle pList)
		{
			return new NodeInfoList(pList);
		}

        internal new NodeInfoListSafeHandle InternalObject
        {
            get { return (NodeInfoListSafeHandle)base.InternalObject; }
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
					return new NodeInfo(OpenNIImporter.xnNodeInfoListGetCurrent(it));
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
					it = OpenNIImporter.xnNodeInfoListGetFirst(list.InternalObject);
					this.reset = false;
				}
				else
				{
					it = OpenNIImporter.xnNodeInfoListGetNext(it);
				}

				return OpenNIImporter.xnNodeInfoListIteratorIsValid(it);
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
			Status.ThrowOnFail(OpenNIImporter.xnNodeInfoListAdd(this.InternalObject, description, creationInfo,
                neededNodes == null ? NodeInfoListSafeHandle.Zero : neededNodes.InternalObject));
			
		}

		public void AddNode(NodeInfo nodeInfo)
		{
            Status.ThrowOnFail(OpenNIImporter.xnNodeInfoListAddNode(this.InternalObject, nodeInfo.InternalObject));
			
		}

		public void AddNodeFromList(IEnumerator<NodeInfo> curr)
		{
			NodeInfoListEnumerator enumer = (NodeInfoListEnumerator)curr;
            Status.ThrowOnFail(OpenNIImporter.xnNodeInfoListAddNodeFromList(this.InternalObject, enumer.InternalObject));
			
		}

		public void Remove(IEnumerator<NodeInfo> location)
		{
			NodeInfoListEnumerator enumer = (NodeInfoListEnumerator)location;
            Status.ThrowOnFail(OpenNIImporter.xnNodeInfoListRemove(this.InternalObject, enumer.InternalObject));
			
		}

		public void Clear()
		{
            Status.ThrowOnFail(OpenNIImporter.xnNodeInfoListClear(this.InternalObject));
			
		}

		public void Append(NodeInfoList other)
		{
            Status.ThrowOnFail(OpenNIImporter.xnNodeInfoListAppend(this.InternalObject, other.InternalObject));
			
		}

		public bool IsEmpty()
		{
			return OpenNIImporter.xnNodeInfoListIsEmpty(this.InternalObject);
		}

		public void Filter(Context context, Query query)
		{
			query.FilterList(context, this);
		}
	}
}