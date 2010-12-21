using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class UserPositionCapability : Capability
	{
		public UserPositionCapability(ProductionNode node) :
			base(node)
		{
			this.userPositionChanged = new StateChangedEvent(node,
				OpenNIImporter.xnRegisterToUserPositionChange,
				OpenNIImporter.xnUnregisterFromUserPositionChange);
		}

		public uint GetSupportedPositionsCount()
		{
			return OpenNIImporter.xnGetSupportedUserPositionsCount(this.InternalObject);
		}

		public void SetPosition(uint index, BoundingBox3D pos)
		{
			UInt32 status = OpenNIImporter.xnSetUserPosition(this.InternalObject, index, ref pos);
			WrapperUtils.CheckStatus(status);
		}

		public BoundingBox3D GetPosition(uint index)
		{
			BoundingBox3D pos = new BoundingBox3D();
			UInt32 status = OpenNIImporter.xnGetUserPosition(this.InternalObject, index, ref pos);
			WrapperUtils.CheckStatus(status);
			return pos;
		}

		public event StateChangedHandler UserPositionChanged
		{
			add { this.userPositionChanged.Event += value; }
			remove { this.userPositionChanged.Event -= value; }
		}

		private StateChangedEvent userPositionChanged;
	}
}