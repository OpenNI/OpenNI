using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class CroppingCapability : Capability
	{
		internal CroppingCapability(ProductionNode node) :
			base(node)
		{
			this.croppingChanged = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToCroppingChange,
				SafeNativeMethods.xnUnregisterFromCroppingChange);
		}

		public Cropping Cropping
		{
			get
			{
				Cropping cropping = new Cropping();
				int status = SafeNativeMethods.xnGetCropping(this.InternalObject, ref cropping);
				WrapperUtils.ThrowOnError(status);
				return cropping;
			}
			set
			{
				int status = SafeNativeMethods.xnSetCropping(this.InternalObject, ref value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		public event EventHandler CroppingChanged
		{
			add { this.croppingChanged.Event += value; }
			remove { this.croppingChanged.Event -= value; }
		}

		private StateChangedEvent croppingChanged;
	}
}