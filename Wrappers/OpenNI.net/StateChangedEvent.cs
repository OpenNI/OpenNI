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