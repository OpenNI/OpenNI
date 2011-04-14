using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class FrameSyncCapability : Capability
	{
		internal FrameSyncCapability(Generator node) :
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
			int status = SafeNativeMethods.xnFrameSyncWith(this.InternalObject, other.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void StopFrameSyncWith(Generator other)
		{
			int status = SafeNativeMethods.xnStopFrameSyncWith(this.InternalObject, other.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public bool IsFrameSyncedWith(Generator other)
		{
			return SafeNativeMethods.xnIsFrameSyncedWith(this.InternalObject, other.InternalObject);
		}

		public event EventHandler FrameSyncChanged
		{
			add { this.frameSyncChangedEvent.Event += value; }
			remove { this.frameSyncChangedEvent.Event -= value; }
		}

		private StateChangedEvent frameSyncChangedEvent;
	}
}