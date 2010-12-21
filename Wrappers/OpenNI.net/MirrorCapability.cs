using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class MirrorCapability : Capability
	{
		public MirrorCapability(ProductionNode node) :
			base(node)
		{
			this.mirrorChangedEvent = new StateChangedEvent(node,
				OpenNIImporter.xnRegisterToMirrorChange,
				OpenNIImporter.xnUnregisterFromMirrorChange);
		}

		public void SetMirror(bool mirror)
		{
			UInt32 status = OpenNIImporter.xnSetMirror(this.InternalObject, mirror);
			WrapperUtils.CheckStatus(status);
		}

		public bool IsMirrored()
		{
			return OpenNIImporter.xnIsMirrored(this.InternalObject);
		}

		public event StateChangedHandler MirrorChangedEvent
		{
			add { this.mirrorChangedEvent.Event += value; }
			remove { this.mirrorChangedEvent.Event -= value; }
		}

		private StateChangedEvent mirrorChangedEvent;
	}
}