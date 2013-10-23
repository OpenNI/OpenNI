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
	internal class StateChangedEvent
	{
		public delegate int RegisterFunc(IntPtr hInstance, SafeNativeMethods.XnStateChangedHandler handler, IntPtr pCookie, out IntPtr phCallback);
		public delegate void UnregisterFunc(IntPtr hInstance, IntPtr hCallback);

		public StateChangedEvent(ProductionNode node, RegisterFunc reg, UnregisterFunc unreg)
		{
			this.node = node;
			this.reg = reg;
			this.unreg = unreg;
			this.internalHandler = new SafeNativeMethods.XnStateChangedHandler(InternalHandler);
		}

		public event EventHandler Event
		{
			add
			{
				if (this.internalEvent == null)
				{
					int status = this.reg(this.node.InternalObject, this.internalHandler, IntPtr.Zero, out this.registerHandle);
					WrapperUtils.ThrowOnError(status);
				}

				this.internalEvent += value;
			}
			remove
			{
				this.internalEvent -= value;

				if (this.internalEvent == null)
				{
					this.unreg(this.node.InternalObject, this.registerHandle);
				}
			}
		}

		private void InternalHandler(IntPtr hNode, IntPtr pCookie)
		{
			EventHandler handlers = this.internalEvent;
			if (handlers != null)
				handlers(this.node, EventArgs.Empty);
		}

		private ProductionNode node;
		private RegisterFunc reg;
		private UnregisterFunc unreg;
		private IntPtr registerHandle;
		private event EventHandler internalEvent;
		// must keep a reference to the delegate (unmanaged does not keep reference to it)
		private SafeNativeMethods.XnStateChangedHandler internalHandler;
	}
}