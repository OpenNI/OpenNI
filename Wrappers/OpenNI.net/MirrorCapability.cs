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
				OpenNIImporter.xnRegisterToMirrorChange,
				OpenNIImporter.xnUnregisterFromMirrorChange);
		}

		public bool IsMirrored
		{
            get
            {
                return OpenNIImporter.xnIsMirrored(this.InternalObject);
            }
            set
            {
			    UInt32 status = OpenNIImporter.xnSetMirror(this.InternalObject, value);
			    WrapperUtils.CheckStatus(status);
            }
		}

		public event StateChangedHandler MirrorChangedEvent
		{
			add { this.mirrorChangedEvent.Event += value; }
			remove { this.mirrorChangedEvent.Event -= value; }
		}

		private StateChangedEvent mirrorChangedEvent;
	}
}