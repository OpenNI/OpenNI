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