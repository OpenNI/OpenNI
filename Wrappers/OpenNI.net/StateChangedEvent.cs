using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	internal class StateChangedEvent
	{
        public delegate Status RegisterFunc(NodeSafeHandle hInstance, OpenNIImporter.XnStateChangedHandler handler, IntPtr pCookie, out IntPtr phCallback);
        public delegate void UnregisterFunc(NodeSafeHandle hInstance, IntPtr hCallback);

		public StateChangedEvent(ProductionNode node, RegisterFunc reg, UnregisterFunc unreg)
		{
			this.node = node;
			this.reg = reg;
			this.unreg = unreg;
			this.internalHandler = new OpenNIImporter.XnStateChangedHandler(InternalHandler);
		}

        public event EventHandler<StateChangedArgs> Event
		{
			add
			{
				if (this.internalEvent == null)
				{
					Status.ThrowOnFail(this.reg(this.node.InternalObject, this.internalHandler, IntPtr.Zero, out this.registerHandle));
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
            var handler = this.internalEvent;
			if (handler != null)
                handler(this, new StateChangedArgs(pCookie)); // TODO: add node
		}

		private readonly ProductionNode node;
        private readonly RegisterFunc reg;
        private readonly UnregisterFunc unreg;
        private IntPtr registerHandle;
        private event EventHandler<StateChangedArgs> internalEvent;
		// must keep a reference to the delegate
        private readonly OpenNIImporter.XnStateChangedHandler internalHandler;
	}

    /// <summary>
    /// Provides data for state change event.
    /// </summary>
    public class StateChangedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the StateChangedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public StateChangedArgs(IntPtr cookie)
        {
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

}