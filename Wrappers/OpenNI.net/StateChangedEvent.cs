using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public delegate void StateChangedHandler(ProductionNode node);

	internal class StateChangedEvent
	{
        public delegate UInt32 RegisterFunc(NodeSafeHandle hInstance, OpenNIImporter.XnStateChangedHandler handler, IntPtr pCookie, out IntPtr phCallback);
        public delegate void UnregisterFunc(NodeSafeHandle hInstance, IntPtr hCallback);

		public StateChangedEvent(ProductionNode node, RegisterFunc reg, UnregisterFunc unreg)
		{
			this.node = node;
			this.reg = reg;
			this.unreg = unreg;
			this.internalHandler = new OpenNIImporter.XnStateChangedHandler(InternalHandler);
		}

		public event StateChangedHandler Event
		{
			add
			{
				if (this.internalEvent == null)
				{
					UInt32 status = this.reg(this.node.InternalObject, this.internalHandler, IntPtr.Zero, out this.registerHandle);
					WrapperUtils.CheckStatus(status);
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

        private void InternalHandler(NodeSafeHandle hNode, IntPtr pCookie)
		{
			if (this.internalEvent != null)
				this.internalEvent(this.node);
		}

		private ProductionNode node;
		private RegisterFunc reg;
		private UnregisterFunc unreg;
		private IntPtr registerHandle;
		private event StateChangedHandler internalEvent;
		// must keep a reference to the delegate
		private OpenNIImporter.XnStateChangedHandler internalHandler;
	}
}