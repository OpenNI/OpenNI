using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class AlternativeViewpointCapability : Capability
	{
		internal AlternativeViewpointCapability(ProductionNode node) :
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
			int status = SafeNativeMethods.xnSetViewPoint(this.InternalObject, other.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void ResetViewpoint()
		{
			int status = SafeNativeMethods.xnResetViewPoint(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public bool IsViewpointAs(ProductionNode other)
		{
			return SafeNativeMethods.xnIsViewPointAs(this.InternalObject, other.InternalObject);
		}

		public event EventHandler ViewpointChanged
		{
			add { this.viewpointChangedEvent.Event += value; }
			remove { this.viewpointChangedEvent.Event -= value; }
		}

		private StateChangedEvent viewpointChangedEvent;
	}
}