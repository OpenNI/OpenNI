/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
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