using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
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

        public Cropping Cropping
		{
            get
            {
			    Cropping cropping = new Cropping();
			    UInt32 status = OpenNIImporter.xnGetCropping(this.InternalObject, ref cropping);
			    WrapperUtils.CheckStatus(status);
			    return cropping;
            }
            set
            {
                UInt32 status = OpenNIImporter.xnSetCropping(this.InternalObject, ref value);
                WrapperUtils.CheckStatus(status);
            }
		}

		public event StateChangedHandler CroppingChanged
		{
			add { this.croppingChanged.Event += value; }
			remove { this.croppingChanged.Event -= value; }
		}

		private StateChangedEvent croppingChanged;
	}
}