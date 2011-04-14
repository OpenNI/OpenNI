using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class UserPositionCapability : Capability
	{
		internal UserPositionCapability(ProductionNode node) :
			base(node)
		{
			this.userPositionChanged = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToUserPositionChange,
				SafeNativeMethods.xnUnregisterFromUserPositionChange);
		}

		public int SupportedPositionsCount
		{
			get
			{
				return (int)SafeNativeMethods.xnGetSupportedUserPositionsCount(this.InternalObject);
			}
		}

		public void SetPosition(int index, BoundingBox3D pos)
		{
			int status = SafeNativeMethods.xnSetUserPosition(this.InternalObject, (uint)index, ref pos);
			WrapperUtils.ThrowOnError(status);
		}

		public BoundingBox3D GetPosition(int index)
		{
			BoundingBox3D pos = new BoundingBox3D();
			int status = SafeNativeMethods.xnGetUserPosition(this.InternalObject, (uint)index, ref pos);
			WrapperUtils.ThrowOnError(status);
			return pos;
		}

		public event EventHandler UserPositionChanged
		{
			add { this.userPositionChanged.Event += value; }
			remove { this.userPositionChanged.Event -= value; }
		}

		private StateChangedEvent userPositionChanged;
	}
}