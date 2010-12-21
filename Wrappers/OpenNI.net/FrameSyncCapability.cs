using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class FrameSyncCapability : Capability
	{
		public FrameSyncCapability(Generator node) :
			base(node)
		{
			this.frameSyncChangedEvent = new StateChangedEvent(node,
				OpenNIImporter.xnRegisterToFrameSyncChange,
				OpenNIImporter.xnUnregisterFromFrameSyncChange);
		}

		public bool CanFrameSyncWith(Generator other)
		{
			return OpenNIImporter.xnCanFrameSyncWith(this.InternalObject, other.InternalObject);
		}

		public void FrameSyncWith(Generator other)
		{
			UInt32 status = OpenNIImporter.xnFrameSyncWith(this.InternalObject, other.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void StopFrameSyncWith(Generator other)
		{
			UInt32 status = OpenNIImporter.xnStopFrameSyncWith(this.InternalObject, other.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public bool IsFrameSyncedWith(Generator other)
		{
			return OpenNIImporter.xnIsFrameSyncedWith(this.InternalObject, other.InternalObject);
		}

		public event StateChangedHandler FrameSyncChanged
		{
			add { this.frameSyncChangedEvent.Event += value; }
			remove { this.frameSyncChangedEvent.Event -= value; }
		}

		private StateChangedEvent frameSyncChangedEvent;
	}
}