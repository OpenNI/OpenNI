using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class UserPositionCapability : Capability
	{
		public UserPositionCapability(ProductionNode node) :
			base(node)
		{
			this.userPositionChanged = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToUserPositionChange,
				SafeNativeMethods.xnUnregisterFromUserPositionChange);
		}

		public uint GetSupportedPositionsCount()
		{
			return SafeNativeMethods.xnGetSupportedUserPositionsCount(this.InternalObject);
		}

		public void SetPosition(uint index, BoundingBox3D pos)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnSetUserPosition(this.InternalObject, index, ref pos));
		}

		public BoundingBox3D GetPosition(uint index)
		{
			BoundingBox3D pos = new BoundingBox3D();
			Status.ThrowOnFail(SafeNativeMethods.xnGetUserPosition(this.InternalObject, index, ref pos));
			return pos;
		}

		public event EventHandler<StateChangedArgs> UserPositionChanged
		{
			add { this.userPositionChanged.Event += value; }
			remove { this.userPositionChanged.Event -= value; }
		}

		private StateChangedEvent userPositionChanged;
	}
}