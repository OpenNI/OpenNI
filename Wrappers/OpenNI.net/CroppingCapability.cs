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