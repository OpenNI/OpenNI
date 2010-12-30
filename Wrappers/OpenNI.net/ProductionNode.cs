using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public abstract class ProductionNode : NodeWrapper
	{
		internal ProductionNode(NodeSafeHandle nodeHandle, bool addRef)
			: base(nodeHandle, addRef)
		{
		}

        static internal ProductionNode FromNative(NodeSafeHandle nodeHandle)
		{
			return Context.CreateProductionNodeFromNative(nodeHandle);
		}

		public NodeInfo GetInfo()
		{
			return new NodeInfo(OpenNIImporter.xnGetNodeInfo(this.InternalObject));
		}

		public void AddNeededNode(ProductionNode needed)
		{
			Status.ThrowOnFail(OpenNIImporter.xnAddNeededNode(this.InternalObject, needed.InternalObject));
		}

		public void RemoveNeededNode(ProductionNode needed)
		{
			Status.ThrowOnFail(OpenNIImporter.xnRemoveNeededNode(this.InternalObject, needed.InternalObject));
		}

		public Context GetContext()
		{
			return new Context(OpenNIImporter.xnGetContextFromNodeHandle(this.InternalObject));
		}

		public bool IsCapabilitySupported(string strCapabilityName)
		{
			return OpenNIImporter.xnIsCapabilitySupported(this.InternalObject, strCapabilityName);
		}

		public void SetIntProperty(string propName, UInt64 value)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetIntProperty(this.InternalObject, propName, value));
		}

		public void SetRealProperty(string propName, double value)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetRealProperty(this.InternalObject, propName, value));
		}

		public void SetStringProperty(string propName, string value)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetStringProperty(this.InternalObject, propName, value));
		}

		public void SetGeneralProperty(string propName, uint size, IntPtr buff)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetGeneralProperty(this.InternalObject, propName, size, buff));
		}

		public UInt64 GetIntProperty(string propName)
		{
			UInt64 value;
			Status.ThrowOnFail(OpenNIImporter.xnGetIntProperty(this.InternalObject, propName, out value));
			return value;
		}

		public double GetRealProperty(string propName)
		{
			double value;
			Status.ThrowOnFail(OpenNIImporter.xnGetRealProperty(this.InternalObject, propName, out value));
			return value;
		}

		public string GetStringProperty(string propName)
		{
			const int size = 2048;
			StringBuilder sb = new StringBuilder(size);
			Status.ThrowOnFail(OpenNIImporter.xnGetStringProperty(this.InternalObject, propName, sb, size));
			return sb.ToString();
		}

		public void GetGeneralProperty(string propName, uint size, IntPtr buff)
		{
			Status.ThrowOnFail(OpenNIImporter.xnGetGeneralProperty(this.InternalObject, propName, size, buff));
		}

		public LockHandle LockForChanges()
		{
			uint handle;
			Status.ThrowOnFail(OpenNIImporter.xnLockNodeForChanges(this.InternalObject, out handle));
			return new LockHandle(handle);
		}

		public void UnlockForChanges(LockHandle lockHandle)
		{
			Status.ThrowOnFail(OpenNIImporter.xnUnlockNodeForChanges(this.InternalObject, lockHandle.InternalHandle));
		}

		public void LockedNodeStartChanges(LockHandle lockHandle)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLockedNodeStartChanges(this.InternalObject, lockHandle.InternalHandle));
		}

		public void LockedNodeEndChanges(LockHandle lockHandle)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLockedNodeEndChanges(this.InternalObject, lockHandle.InternalHandle));
		}
	}
}