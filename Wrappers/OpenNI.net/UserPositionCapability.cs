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
				OpenNIImporter.xnRegisterToUserPositionChange,
				OpenNIImporter.xnUnregisterFromUserPositionChange);
		}

		public uint GetSupportedPositionsCount()
		{
			return OpenNIImporter.xnGetSupportedUserPositionsCount(this.InternalObject);
		}

		public void SetPosition(uint index, BoundingBox3D pos)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetUserPosition(this.InternalObject, index, ref pos));
		}

		public BoundingBox3D GetPosition(uint index)
		{
			BoundingBox3D pos = new BoundingBox3D();
			Status.ThrowOnFail(OpenNIImporter.xnGetUserPosition(this.InternalObject, index, ref pos));
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