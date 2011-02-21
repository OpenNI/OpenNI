using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace OpenNI
{
	internal class StateChangedEvent
	{
        public delegate Status RegisterFunc(NodeSafeHandle hInstance, SafeNativeMethods.XnStateChangedHandler handler, IntPtr cookie, out IntPtr phCallback);
        public delegate void UnregisterFunc(NodeSafeHandle hInstance, IntPtr hCallback);

        public StateChangedEvent(ProductionNode node, RegisterFunc reg, UnregisterFunc unreg)
		{
			this.node = node;
            this.stateChanged = new CallbackSubject<StateChangedArgs>(
                    () => Status.ThrowOnFail(reg(this.node.InternalObject, this.StateChangedHandler, IntPtr.Zero, out this.registerHandle)),
                    () => unreg(this.node.InternalObject, this.registerHandle)
                );
        }

        public IObservable<StateChangedArgs> StateChanged
        {
            get
            {
                return this.stateChanged;
            }
        }

        private void StateChangedHandler(IntPtr nodeHandle, IntPtr cookie)
		{
            this.stateChanged.OnNext(new StateChangedArgs(cookie));
        }

		private readonly ProductionNode node;
        private IntPtr registerHandle;

        private readonly CallbackSubject<StateChangedArgs> stateChanged;
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