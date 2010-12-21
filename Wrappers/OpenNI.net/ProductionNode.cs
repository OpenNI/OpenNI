using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class ProductionNode : NodeWrapper
	{
		internal ProductionNode(IntPtr nodeHandle)
			: base(nodeHandle)
		{
		}

		public static ProductionNode FromNative(IntPtr nodeHandle)
		{
			return Context.CreateProductionNodeFromNative(nodeHandle);
		}

		public NodeInfo GetInfo()
		{
			return new NodeInfo(OpenNIImporter.xnGetNodeInfo(this.InternalObject));
		}

		public void AddNeededNode(ProductionNode needed)
		{
			UInt32 status = OpenNIImporter.xnAddNeededNode(this.InternalObject, needed.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void RemoveNeededNode(ProductionNode needed)
		{
			UInt32 status = OpenNIImporter.xnRemoveNeededNode(this.InternalObject, needed.InternalObject);
			WrapperUtils.CheckStatus(status);
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
			UInt32 status = OpenNIImporter.xnSetIntProperty(this.InternalObject, propName, value);
			WrapperUtils.CheckStatus(status);
		}

		public void SetRealProperty(string propName, double value)
		{
			UInt32 status = OpenNIImporter.xnSetRealProperty(this.InternalObject, propName, value);
			WrapperUtils.CheckStatus(status);
		}

		public void SetStringProperty(string propName, string value)
		{
			UInt32 status = OpenNIImporter.xnSetStringProperty(this.InternalObject, propName, value);
			WrapperUtils.CheckStatus(status);
		}

		public void SetGeneralProperty(string propName, uint size, IntPtr buff)
		{
			UInt32 status = OpenNIImporter.xnSetGeneralProperty(this.InternalObject, propName, size, buff);
			WrapperUtils.CheckStatus(status);
		}

		public UInt64 GetIntProperty(string propName)
		{
			UInt64 value;
			UInt32 status = OpenNIImporter.xnGetIntProperty(this.InternalObject, propName, out value);
			WrapperUtils.CheckStatus(status);
			return value;
		}

		public double GetRealProperty(string propName)
		{
			double value;
			UInt32 status = OpenNIImporter.xnGetRealProperty(this.InternalObject, propName, out value);
			WrapperUtils.CheckStatus(status);
			return value;
		}

		public string GetStringProperty(string propName)
		{
			const int size = 2048;
			StringBuilder sb = new StringBuilder(size);
			UInt32 status = OpenNIImporter.xnGetStringProperty(this.InternalObject, propName, sb, size);
			WrapperUtils.CheckStatus(status);
			return sb.ToString();
		}

		public void GetGeneralProperty(string propName, uint size, IntPtr buff)
		{
			UInt32 status = OpenNIImporter.xnGetGeneralProperty(this.InternalObject, propName, size, buff);
			WrapperUtils.CheckStatus(status);
		}

		public LockHandle LockForChanges()
		{
			uint handle;
			UInt32 status = OpenNIImporter.xnLockNodeForChanges(this.InternalObject, out handle);
			WrapperUtils.CheckStatus(status);
			return new LockHandle(handle);
		}

		public void UnlockForChanges(LockHandle lockHandle)
		{
			UInt32 status = OpenNIImporter.xnUnlockNodeForChanges(this.InternalObject, lockHandle.InternalHandle);
			WrapperUtils.CheckStatus(status);
		}

		public void LockedNodeStartChanges(LockHandle lockHandle)
		{
			UInt32 status = OpenNIImporter.xnLockedNodeStartChanges(this.InternalObject, lockHandle.InternalHandle);
			WrapperUtils.CheckStatus(status);
		}

		public void LockedNodeEndChanges(LockHandle lockHandle)
		{
			UInt32 status = OpenNIImporter.xnLockedNodeEndChanges(this.InternalObject, lockHandle.InternalHandle);
			WrapperUtils.CheckStatus(status);
		}
	}
}