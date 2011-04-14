using System;
using System.Collections.Generic;
using System.Text;

namespace OpenNI
{
	public class AntiFlickerCapability : Capability
	{
		internal AntiFlickerCapability(MapGenerator node) :
			base(node)
		{
			this.frequencyChangedEvent = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToPowerLineFrequencyChange,
				SafeNativeMethods.xnUnregisterFromPowerLineFrequencyChange);
		}

		public PowerLineFrequency PowerLineFrequency
		{
			get
			{
				return SafeNativeMethods.xnGetPowerLineFrequency(this.InternalObject);
			}
			set
			{
				SafeNativeMethods.xnSetPowerLineFrequency(this.InternalObject, value);
			}
		}

		public event EventHandler PowerLineFrequencyChanged
		{
			add { this.frequencyChangedEvent.Event += value; }
			remove { this.frequencyChangedEvent.Event -= value; }
		}

		private StateChangedEvent frequencyChangedEvent;
	}
}
