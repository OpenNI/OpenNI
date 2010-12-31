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
				SafeNativeMethods.xnRegisterToViewpointChange,
				SafeNativeMethods.xnUnregisterFromViewpointChange);
		}

		public bool IsViewpointSupported(ProductionNode other)
		{
			return SafeNativeMethods.xnIsViewpointSupported(this.InternalObject, other.InternalObject);
		}

		public void SetViewpoint(ProductionNode other)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnSetViewpoint(this.InternalObject, other.InternalObject));
		}

		public void ResetViewpoint()
		{
			Status.ThrowOnFail(SafeNativeMethods.xnResetViewpoint(this.InternalObject));
		}

		public bool IsViewpointAs(ProductionNode other)
		{
			return SafeNativeMethods.xnIsViewpointAs(this.InternalObject, other.InternalObject);
		}

		public event EventHandler<StateChangedArgs> ViewpointChanged
		{
			add { this.viewpointChangedEvent.Event += value; }
			remove { this.viewpointChangedEvent.Event -= value; }
		}

		private StateChangedEvent viewpointChangedEvent;
	}
}