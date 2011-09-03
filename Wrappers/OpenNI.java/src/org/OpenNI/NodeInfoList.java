package org.OpenNI;

import java.util.Iterator;
import java.util.NoSuchElementException;

public class NodeInfoList extends ObjectWrapper implements Iterable<NodeInfo>
{
	NodeInfoList(long ptr)
	{
		super(ptr);
	}
	
	public void add(ProductionNodeDescription description, String creationInfo, NodeInfoList neededNodes) throws StatusException
	{
		long pNativeDesc = description.createNative();
		int status = NativeMethods.xnNodeInfoListAdd(toNative(), pNativeDesc, creationInfo,
				neededNodes == null ? 0 : neededNodes.toNative());
		ProductionNodeDescription.freeNative(pNativeDesc);
		WrapperUtils.throwOnError(status);
	}

	public void addNode(NodeInfo nodeInfo) throws StatusException
	{
		int status = NativeMethods.xnNodeInfoListAddNode(toNative(), nodeInfo.toNative());
		WrapperUtils.throwOnError(status);
	}

	public void addNodeFromList(Iterator<NodeInfo> current) throws StatusException
	{
		NodeInfoListIterator iter = (NodeInfoListIterator)current;
		int status = NativeMethods.xnNodeInfoListAddNodeFromList(toNative(), iter.it);
		WrapperUtils.throwOnError(status);
	}

	public void remove(Iterator<NodeInfo> location) throws StatusException
	{
		NodeInfoListIterator iter = (NodeInfoListIterator)location;
		int status = NativeMethods.xnNodeInfoListRemove(toNative(), iter.it);
		WrapperUtils.throwOnError(status);
	}

	public void clear() throws StatusException
	{
		int status = NativeMethods.xnNodeInfoListClear(toNative());
		WrapperUtils.throwOnError(status);
	}

	public void append(NodeInfoList other) throws StatusException
	{
		int status = NativeMethods.xnNodeInfoListAppend(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}

	public boolean isEmpty()
	{
		return NativeMethods.xnNodeInfoListIsEmpty(toNative());
	}
	
	public void filter(Query query, Context context) throws StatusException
	{
		int status = NativeMethods.xnNodeQueryFilterList(context.toNative(), query.toNative(), toNative());
		WrapperUtils.throwOnError(status);
	}

	@Override
	public Iterator<NodeInfo> iterator() {
		return new NodeInfoListIterator(toNative());
	}

	@Override
	protected void freeObject(long ptr) 
	{
		NativeMethods.xnNodeInfoListFree(ptr);
	}
	
	private class NodeInfoListIterator implements Iterator<NodeInfo>
	{
		NodeInfoListIterator(long pNodeInfoList)
		{
			this.it = NativeMethods.xnNodeInfoListGetFirst(pNodeInfoList);
		}
		
		@Override
		public boolean hasNext() 
		{
			return NativeMethods.xnNodeInfoListIteratorIsValid(this.it);
		}

		@Override
		public NodeInfo next() 
		{
			if (!hasNext())
				throw new NoSuchElementException();
			
			NodeInfo next = null;
			
			try 
			{
				next = new NodeInfo(NativeMethods.xnNodeInfoListGetCurrent(this.it));
			} 
			catch (GeneralException e) 
			{
				// can't happen
			}
			
			this.it = NativeMethods.xnNodeInfoListGetNext(this.it);
			return next;
		}

		@Override
		public void remove() 
		{
			throw new UnsupportedOperationException();
		}

		protected long it;
	}
}
