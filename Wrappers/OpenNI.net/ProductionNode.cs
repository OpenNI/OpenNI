using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;
using System.Text;

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

		public NodeInfo Info
		{
            get
            {
                return new NodeInfo(SafeNativeMethods.xnGetNodeInfo(this.InternalObject));
            }
		}

		public void AddNeededNode(ProductionNode needed)
		{
            Contract.Requires(needed != null);

			Status.ThrowOnFail(SafeNativeMethods.xnAddNeededNode(this.InternalObject, needed.InternalObject));
		}

		public void RemoveNeededNode(ProductionNode needed)
		{
            Contract.Requires(needed != null);

            Status.ThrowOnFail(SafeNativeMethods.xnRemoveNeededNode(this.InternalObject, needed.InternalObject));
		}

		public Context Context
		{
            get
            {
                return new Context(SafeNativeMethods.xnGetContextFromNodeHandle(this.InternalObject));
            }
		}

		public bool IsCapabilitySupported(string capabilityName)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(capabilityName));

            return SafeNativeMethods.xnIsCapabilitySupported(this.InternalObject, capabilityName);
		}

		public void SetIntProperty(string propertyName, UInt64 value)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            Status.ThrowOnFail(SafeNativeMethods.xnSetIntProperty(this.InternalObject, propertyName, value));
		}

		public void SetRealProperty(string propertyName, double value)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            Status.ThrowOnFail(SafeNativeMethods.xnSetRealProperty(this.InternalObject, propertyName, value));
		}

		public void SetStringProperty(string propertyName, string value)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            Status.ThrowOnFail(SafeNativeMethods.xnSetStringProperty(this.InternalObject, propertyName, value));
		}

		public void SetGeneralProperty(string propertyName, uint size, IntPtr buffer)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            Status.ThrowOnFail(SafeNativeMethods.xnSetGeneralProperty(this.InternalObject, propertyName, size, buffer));
		}

		public UInt64 GetIntProperty(string propertyName)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            UInt64 value;
			Status.ThrowOnFail(SafeNativeMethods.xnGetIntProperty(this.InternalObject, propertyName, out value));
			return value;
		}

		public double GetRealProperty(string propertyName)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            double value;
			Status.ThrowOnFail(SafeNativeMethods.xnGetRealProperty(this.InternalObject, propertyName, out value));
			return value;
		}

		public string GetStringProperty(string propertyName)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            const int size = 2048;
			StringBuilder sb = new StringBuilder(size);
			Status.ThrowOnFail(SafeNativeMethods.xnGetStringProperty(this.InternalObject, propertyName, sb, size));
			return sb.ToString();
		}

		public void GetGeneralProperty(string propertyName, uint size, IntPtr buff)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(propertyName));

            Status.ThrowOnFail(SafeNativeMethods.xnGetGeneralProperty(this.InternalObject, propertyName, size, buff));
		}

		public LockHandle LockForChanges()
		{
            Contract.Ensures(Contract.Result<LockHandle>() != null);

			uint handle;
			Status.ThrowOnFail(SafeNativeMethods.xnLockNodeForChanges(this.InternalObject, out handle));
			return new LockHandle(handle);
		}

		public void UnlockForChanges(LockHandle lockHandle)
		{
            Contract.Requires(lockHandle != null);

			Status.ThrowOnFail(SafeNativeMethods.xnUnlockNodeForChanges(this.InternalObject, lockHandle.InternalHandle));
		}

		public void LockedNodeStartChanges(LockHandle lockHandle)
		{
            Contract.Requires(lockHandle != null);

            Status.ThrowOnFail(SafeNativeMethods.xnLockedNodeStartChanges(this.InternalObject, lockHandle.InternalHandle));
		}

		public void LockedNodeEndChanges(LockHandle lockHandle)
		{
            Contract.Requires(lockHandle != null);

            Status.ThrowOnFail(SafeNativeMethods.xnLockedNodeEndChanges(this.InternalObject, lockHandle.InternalHandle));
		}
	}
}