using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class AlternativeViewpointCapability : Capability
	{
		public AlternativeViewpointCapability(ProductionNode node) :
			base(node)
		{
			this.viewpointChangedEvent = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToViewPointChange,
				SafeNativeMethods.xnUnregisterFromViewPointChange);
		}

		public bool IsViewpointSupported(ProductionNode other)
		{
			return SafeNativeMethods.xnIsViewPointSupported(this.InternalObject, other.InternalObject);
		}

		public void SetViewpoint(ProductionNode other)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnSetViewPoint(this.InternalObject, other.InternalObject));
		}

		public void ResetViewpoint()
		{
			Status.ThrowOnFail(SafeNativeMethods.xnResetViewPoint(this.InternalObject));
		}

		public bool IsViewpointAs(ProductionNode other)
		{
			return SafeNativeMethods.xnIsViewPointAs(this.InternalObject, other.InternalObject);
		}

		public event EventHandler<StateChangedArgs> ViewpointChanged
		{
			add { this.viewpointChangedEvent.Event += value; }
			remove { this.viewpointChangedEvent.Event -= value; }
		}

		private StateChangedEvent viewpointChangedEvent;
	}
}