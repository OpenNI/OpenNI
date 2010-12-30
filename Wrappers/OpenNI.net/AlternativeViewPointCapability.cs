using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class AlternativeViewPointCapability : Capability
	{
		public AlternativeViewPointCapability(ProductionNode node) :
			base(node)
		{
			this.viewPointChangedEvent = new StateChangedEvent(node,
				OpenNIImporter.xnRegisterToViewPointChange,
				OpenNIImporter.xnUnregisterFromViewPointChange);
		}

		public bool IsViewPointSupported(ProductionNode other)
		{
			return OpenNIImporter.xnIsViewPointSupported(this.InternalObject, other.InternalObject);
		}

		public void SetViewPoint(ProductionNode other)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetViewPoint(this.InternalObject, other.InternalObject));
		}

		public void ResetViewPoint()
		{
			Status.ThrowOnFail(OpenNIImporter.xnResetViewPoint(this.InternalObject));
		}

		public bool IsViewPointAs(ProductionNode other)
		{
			return OpenNIImporter.xnIsViewPointAs(this.InternalObject, other.InternalObject);
		}

		public event EventHandler<StateChangedArgs> ViewPointChanged
		{
			add { this.viewPointChangedEvent.Event += value; }
			remove { this.viewPointChangedEvent.Event -= value; }
		}

		private StateChangedEvent viewPointChangedEvent;
	}
}