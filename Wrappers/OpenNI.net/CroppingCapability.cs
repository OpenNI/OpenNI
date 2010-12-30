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
			    Status.ThrowOnFail(OpenNIImporter.xnGetCropping(this.InternalObject, ref cropping));
			    return cropping;
            }
            set
            {
                Status.ThrowOnFail(OpenNIImporter.xnSetCropping(this.InternalObject, ref value));
            }
		}

		public event EventHandler<StateChangedArgs> CroppingChanged
		{
			add { this.croppingChanged.Event += value; }
			remove { this.croppingChanged.Event -= value; }
		}

		private StateChangedEvent croppingChanged;
	}
}