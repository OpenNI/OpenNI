using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class CroppingCapability : Capability
	{
		public CroppingCapability(ProductionNode node) :
			base(node)
		{
			this.croppingChanged = new StateChangedEvent(node,
				OpenNIImporter.xnRegisterToCroppingChange,
				OpenNIImporter.xnUnregisterFromCroppingChange);
		}

		public void SetCropping(ref Cropping cropping)
		{
			UInt32 status = OpenNIImporter.xnSetCropping(this.InternalObject, ref cropping);
			WrapperUtils.CheckStatus(status);
		}

		public Cropping GetCropping()
		{
			Cropping cropping = new Cropping();
			UInt32 status = OpenNIImporter.xnGetCropping(this.InternalObject, ref cropping);
			WrapperUtils.CheckStatus(status);
			return cropping;
		}

		public event StateChangedHandler CroppingChanged
		{
			add { this.croppingChanged.Event += value; }
			remove { this.croppingChanged.Event -= value; }
		}

		private StateChangedEvent croppingChanged;
	}
}