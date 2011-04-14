using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MirrorCapability : Capability
	{
		internal MirrorCapability(ProductionNode node) :
			base(node)
		{
			this.mirrorChangedEvent = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToMirrorChange,
				SafeNativeMethods.xnUnregisterFromMirrorChange);
		}

		public void SetMirror(bool mirror)
		{
			int status = SafeNativeMethods.xnSetMirror(this.InternalObject, mirror);
			WrapperUtils.ThrowOnError(status);
		}

		public bool IsMirrored()
		{
			return SafeNativeMethods.xnIsMirrored(this.InternalObject);
		}

		public event EventHandler MirrorChangedEvent
		{
			add { this.mirrorChangedEvent.Event += value; }
			remove { this.mirrorChangedEvent.Event -= value; }
		}

		private StateChangedEvent mirrorChangedEvent;
	}
}