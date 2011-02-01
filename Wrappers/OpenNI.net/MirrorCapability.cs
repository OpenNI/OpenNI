using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MirrorCapability : Capability
	{
		public MirrorCapability(ProductionNode node) :
			base(node)
		{
			this.mirrorChangedEvent = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToMirrorChange,
				SafeNativeMethods.xnUnregisterFromMirrorChange);
		}

		public bool IsMirrored
		{
            get
            {
                return SafeNativeMethods.xnIsMirrored(this.InternalObject);
            }
            set
            {
			    Status.ThrowOnFail(SafeNativeMethods.xnSetMirror(this.InternalObject, value));
            }
		}

		public IObservable<StateChangedArgs> MirrorChanged
		{
            get { return this.mirrorChangedEvent.StateChanged; }
		}

		private StateChangedEvent mirrorChangedEvent;
	}
}