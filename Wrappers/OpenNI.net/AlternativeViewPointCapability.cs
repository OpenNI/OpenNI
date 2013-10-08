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

        public void GetPixelCoordinatesInViewpoint(ProductionNode viewpoint, int x, int y, out int altX, out int altY)
        {
            uint uAltX;
            uint uAltY;
            int status = SafeNativeMethods.xnGetPixelCoordinatesInViewPoint(this.InternalObject, viewpoint.InternalObject, (uint)x, (uint)y, out uAltX, out uAltY);
            WrapperUtils.ThrowOnError(status);
            altX = (int)uAltX;
            altY = (int)uAltY;
        }

		public event EventHandler ViewpointChanged
		{
			add { this.viewpointChangedEvent.Event += value; }
			remove { this.viewpointChangedEvent.Event -= value; }
		}

		private StateChangedEvent viewpointChangedEvent;
	}
}