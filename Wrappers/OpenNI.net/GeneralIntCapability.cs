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
using System.Text;

namespace OpenNI
{
	public class GeneralIntCapability : Capability
	{
		internal GeneralIntCapability(ProductionNode node, string capabilityName) :
			base(node)
		{
			this.capabilityName = capabilityName;
			this.internalHandler = this.InternalHandler;

			int status = SafeNativeMethods.xnGetGeneralIntRange(this.InternalObject, this.capabilityName, out this.min, out this.max, out this.step, out this.defaultVal, out this.isAutoSupported);
			WrapperUtils.ThrowOnError(status);
		}

		public static Int32 AutoControl
		{
			get { return Int32.MinValue; }
		}

		public Int32 Min
		{
			get { return this.min; }
		}

		public Int32 Max
		{
			get { return this.max; }
		}

		public Int32 Step
		{
			get { return this.step; }
		}

		public Int32 Default
		{
			get { return this.defaultVal; }
		}

		public bool IsAutoModeSupported
		{
			get { return this.isAutoSupported; }
		}

		public Int32 Value
		{
			get
			{
				Int32 value;
				int status = SafeNativeMethods.xnGetGeneralIntValue(this.InternalObject, this.capabilityName, out value);
				WrapperUtils.ThrowOnError(status);
				return value;
			}
			set
			{
				int status = SafeNativeMethods.xnSetGeneralIntValue(this.InternalObject, this.capabilityName, value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		public event EventHandler ValueChanged
		{
			add 
			{
				if (this.valueChangedEvent == null)
				{
					int status = SafeNativeMethods.xnRegisterToGeneralIntValueChange(this.InternalObject, this.capabilityName, this.internalHandler, IntPtr.Zero, out this.registerHandle);
					WrapperUtils.ThrowOnError(status);
				}

				this.valueChangedEvent += value; 
			}
			remove 
			{ 
				this.valueChangedEvent -= value;

				if (this.valueChangedEvent == null)
				{
					SafeNativeMethods.xnUnregisterFromGeneralIntValueChange(this.InternalObject, this.capabilityName, this.registerHandle);
				}
			}
		}

		private void InternalHandler(IntPtr hNode, IntPtr pCookie)
		{
			EventHandler handlers = this.valueChangedEvent;
			if (handlers != null)
				handlers(this.node, EventArgs.Empty);
		}

		private string capabilityName;
		private EventHandler valueChangedEvent;
		private IntPtr registerHandle;
		private Int32 min;
		private Int32 max;
		private Int32 step;
		private Int32 defaultVal;
		private bool isAutoSupported;
		private SafeNativeMethods.XnStateChangedHandler internalHandler;
	}
}
