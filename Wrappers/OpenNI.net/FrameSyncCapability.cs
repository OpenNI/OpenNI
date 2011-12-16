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
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class FrameSyncCapability : Capability
	{
		internal FrameSyncCapability(Generator node) :
			base(node)
		{
			this.frameSyncChangedEvent = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToFrameSyncChange,
				SafeNativeMethods.xnUnregisterFromFrameSyncChange);
		}

		public bool CanFrameSyncWith(Generator other)
		{
			return SafeNativeMethods.xnCanFrameSyncWith(this.InternalObject, other.InternalObject);
		}

		public void FrameSyncWith(Generator other)
		{
			int status = SafeNativeMethods.xnFrameSyncWith(this.InternalObject, other.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void StopFrameSyncWith(Generator other)
		{
			int status = SafeNativeMethods.xnStopFrameSyncWith(this.InternalObject, other.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public bool IsFrameSyncedWith(Generator other)
		{
			return SafeNativeMethods.xnIsFrameSyncedWith(this.InternalObject, other.InternalObject);
		}

		public event EventHandler FrameSyncChanged
		{
			add { this.frameSyncChangedEvent.Event += value; }
			remove { this.frameSyncChangedEvent.Event -= value; }
		}

		private StateChangedEvent frameSyncChangedEvent;
	}
}