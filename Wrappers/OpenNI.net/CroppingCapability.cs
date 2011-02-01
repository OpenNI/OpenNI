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
				SafeNativeMethods.xnRegisterToCroppingChange,
				SafeNativeMethods.xnUnregisterFromCroppingChange);
		}

        public Cropping Cropping
		{
            get
            {
			    Cropping cropping = new Cropping();
			    Status.ThrowOnFail(SafeNativeMethods.xnGetCropping(this.InternalObject, ref cropping));
			    return cropping;
            }
            set
            {
                Status.ThrowOnFail(SafeNativeMethods.xnSetCropping(this.InternalObject, ref value));
            }
		}

		public IObservable<StateChangedArgs> CroppingChanged
		{
            get { return this.croppingChanged.StateChanged; }
		}

		private StateChangedEvent croppingChanged;
	}
}