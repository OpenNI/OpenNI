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