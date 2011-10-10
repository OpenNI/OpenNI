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