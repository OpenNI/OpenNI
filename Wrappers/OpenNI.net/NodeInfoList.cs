using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class NodeInfoList : ObjectWrapper, IEnumerable<NodeInfo>
	{
		internal NodeInfoList(IntPtr pList)
			: base(pList)
		{
		}

		/// <summary>
		/// Creates a managed NodeInfoList object to wrap a native one.
		/// </summary>
		/// <param name="pList">The native node info list pointer</param>
		/// <returns>A managed NodeInfoList object</returns>
		static public NodeInfoList FromNative(IntPtr pList)
		{
			return new NodeInfoList(pList);
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
			UInt32 status = OpenNIImporter.xnNodeInfoListAdd(this.InternalObject, description, creationInfo,
				neededNodes == null ? IntPtr.Zero : neededNodes.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void AddNode(NodeInfo nodeInfo)
		{
			UInt32 status = OpenNIImporter.xnNodeInfoListAddNode(this.InternalObject, nodeInfo.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void AddNodeFromList(IEnumerator<NodeInfo> curr)
		{
			NodeInfoListEnumerator enumer = (NodeInfoListEnumerator)curr;
			UInt32 status = OpenNIImporter.xnNodeInfoListAddNodeFromList(this.InternalObject, enumer.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void Remove(IEnumerator<NodeInfo> location)
		{
			NodeInfoListEnumerator enumer = (NodeInfoListEnumerator)location;
			UInt32 status = OpenNIImporter.xnNodeInfoListRemove(this.InternalObject, enumer.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void Clear()
		{
			UInt32 status = OpenNIImporter.xnNodeInfoListClear(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void Append(NodeInfoList other)
		{
			UInt32 status = OpenNIImporter.xnNodeInfoListAppend(this.InternalObject, other.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public bool IsEmpty()
		{
			return OpenNIImporter.xnNodeInfoListIsEmpty(this.InternalObject);
		}

		public void Filter(Context context, Query query)
		{
			query.FilterList(context, this);
		}

		protected override void FreeObject(IntPtr ptr)
		{
			OpenNIImporter.xnNodeInfoListFree(ptr);
		}
	}
}