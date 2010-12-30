using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class FrameSyncCapability : Capability
	{
		public FrameSyncCapability(Generator node) :
			base(node)
		{
			this.frameSyncChangedEvent = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToFrameSyncChange,
				SafeNativeMethods.xnUnregisterFromFrameSyncChange);
		}

		public bool CanFrameSyncWith(Generator other)
		{
			return SafeNativeMethods.xnCanFrameSyncWith(this.InternalObject, other.InternalObject);
		}

		public void FrameSyncWith(Generator other)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnFrameSyncWith(this.InternalObject, other.InternalObject));
		}

		public void StopFrameSyncWith(Generator other)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnStopFrameSyncWith(this.InternalObject, other.InternalObject));
		}

		public bool IsFrameSyncedWith(Generator other)
		{
			return SafeNativeMethods.xnIsFrameSyncedWith(this.InternalObject, other.InternalObject);
		}

		public event EventHandler<StateChangedArgs> FrameSyncChanged
		{
			add { this.frameSyncChangedEvent.Event += value; }
			remove { this.frameSyncChangedEvent.Event -= value; }
		}

		private StateChangedEvent frameSyncChangedEvent;
	}
}