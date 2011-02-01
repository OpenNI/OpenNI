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
			this.internalHandler = new SafeNativeMethods.XnStateChangedHandler(StateChangedHandler);

            this.stateChanged = Observable.Create<StateChangedArgs>(observer =>
            {
                // register the callback if required
                if (this.observers.Count == 0)
                {
                    Status.ThrowOnFail(reg(this.node.InternalObject, this.internalHandler, IntPtr.Zero, out this.registerHandle));
                }
                // add to the observers list
                this.observers.Add(observer);

                // return the unregister method
                return () =>
                {
                    // remove form the observers list
                    this.observers.Remove(observer);
                    // unregister the callback if possible
                    if (this.observers.Count == 0)
                    {
                        unreg(this.node.InternalObject, this.registerHandle);
                    }
                };
            });
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
            this.observers.ForEach(observer => observer.OnNext(new StateChangedArgs(cookie)));
        }

		private readonly ProductionNode node;
        private IntPtr registerHandle;
		// must keep a reference to the delegate
        private readonly SafeNativeMethods.XnStateChangedHandler internalHandler;

        private readonly List<IObserver<StateChangedArgs>> observers = new List<IObserver<StateChangedArgs>>();
        private readonly IObservable<StateChangedArgs> stateChanged;
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