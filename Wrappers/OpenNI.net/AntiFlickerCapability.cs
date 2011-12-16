/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
﻿using System;
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
