using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UserId = System.UInt32;

namespace OpenNI
{
    public class HandsGenerator : Generator
    {
        internal HandsGenerator(NodeSafeHandle nodeHandle, bool addRef)
			: base(nodeHandle, addRef)
        {
            this.internalHandCreated = new SafeNativeMethods.XnHandCreate(this.OnHandCreated);
            this.internalHandUpdate = new SafeNativeMethods.XnHandUpdate(this.OnHandUpdated);
            this.internalHandDestroy = new SafeNativeMethods.XnHandDestroy(this.OnHandDestroyed);

            // initialize the observables

            this.handCreated = Observable.Create<HandCreatedArgs>(observer =>
            {
                // register the callback if required
                if (this.handCreatedObservers.Count == 0)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, this.internalHandCreated, null, null, IntPtr.Zero, out this.handCreatedHandle));
                }
                // add to the observers list
                this.handCreatedObservers.Add(observer);

                // return the unregister method
                return () =>
                {
                    // remove form the observers list
                    this.handCreatedObservers.Remove(observer);
                    // unregister the callback if possible
                    if (this.handCreatedObservers.Count == 0)
                    {
                        SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handCreatedHandle);
                    }
                };
            });

            this.handUpdated = Observable.Create<HandUpdatedArgs>(observer =>
            {
                // register the callback if required
                if (this.handUpdatedObservers.Count == 0)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, null, this.internalHandUpdate, null, IntPtr.Zero, out this.handUpdateHandle));
                }
                // add to the observers list
                this.handUpdatedObservers.Add(observer);

                // return the unregister method
                return () =>
                {
                    // remove form the observers list
                    this.handUpdatedObservers.Remove(observer);
                    // unregister the callback if possible
                    if (this.handUpdatedObservers.Count == 0)
                    {
                        SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handUpdateHandle);
                    }
                };
            });

            this.handDestroyed = Observable.Create<HandDestroyedArgs>(observer =>
            {
                // register the callback if required
                if (this.handDestroyedObservers.Count == 0)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, null, null, this.internalHandDestroy, IntPtr.Zero, out this.handDestroyHandle));
                }
                // add to the observers list
                this.handDestroyedObservers.Add(observer);

                // return the unregister method
                return () =>
                {
                    // remove form the observers list
                    this.handDestroyedObservers.Remove(observer);
                    // unregister the callback if possible
                    if (this.handDestroyedObservers.Count == 0)
                    {
                        SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handDestroyHandle);
                    }
                };
            });

        }

        public HandsGenerator(Context context, Query query, ErrorCollection errors) :
            this(Create(context, query, errors), false)
        {
        }
        public HandsGenerator(Context context, Query query)
            : this(context, query, null)
        {
        }
        public HandsGenerator(Context context)
            : this(context, null, null)
        {
        }

        private static NodeSafeHandle Create(Context context, Query query, ErrorCollection errors)
        {
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateHandsGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? QuerySafeHandle.Zero : query.InternalObject,
                                                        errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
            
            return handle;
        }

        public void StopTracking(UserId id)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnStopTracking(this.InternalObject, id));        
        }
        public void StopTrackingAll()
        {
            Status.ThrowOnFail(SafeNativeMethods.xnStopTrackingAll(this.InternalObject));          
        }
        public void StartTracking(Point3D position)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnStartTracking(this.InternalObject, ref position));           
        }
        public void SetSmoothing(float factor)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnSetTrackingSmoothing(this.InternalObject, factor));         
        }

        #region Hand Created

        public IObservable<HandCreatedArgs> HandCreated
        {
            get
            {
                return this.handCreated;
            }
        }

        private void OnHandCreated(IntPtr nodeHandle, UserId userId, ref Point3D position, float time, IntPtr cookie)
        {
            foreach (var observer in this.handCreatedObservers)
            {
                observer.OnNext(new HandCreatedArgs(userId, position, time, cookie));
            }
        }

        private readonly List<IObserver<HandCreatedArgs>> handCreatedObservers = new List<IObserver<HandCreatedArgs>>();
        private readonly IObservable<HandCreatedArgs> handCreated;
        private SafeNativeMethods.XnHandCreate internalHandCreated;
        private IntPtr handCreatedHandle;

        #endregion

        #region Hand Updated

        public IObservable<HandUpdatedArgs> HandUpdated
        {
            get
            {
                return this.handUpdated;
            }
        }

        private void OnHandUpdated(IntPtr nodeHandle, UserId userId, ref Point3D position, float time, IntPtr cookie)
        {
            foreach (var observer in this.handUpdatedObservers)
            {
                observer.OnNext(new HandUpdatedArgs(userId, position, time, cookie));
            }
        }

        private readonly List<IObserver<HandUpdatedArgs>> handUpdatedObservers = new List<IObserver<HandUpdatedArgs>>();
        private readonly IObservable<HandUpdatedArgs> handUpdated;
        private SafeNativeMethods.XnHandUpdate internalHandUpdate;
        private IntPtr handUpdateHandle;

        #endregion

        #region Hand Destroyed

        public IObservable<HandDestroyedArgs> HandDestroyed
        {
            get
            {
                return this.handDestroyed;
            }
        }

        private void OnHandDestroyed(IntPtr nodeHandle, UserId userId, float time, IntPtr cookie)
        {
            foreach (var observer in this.handDestroyedObservers)
            {
                observer.OnNext(new HandDestroyedArgs(userId, time, cookie));
            }
        }

        private readonly List<IObserver<HandDestroyedArgs>> handDestroyedObservers = new List<IObserver<HandDestroyedArgs>>();
        private readonly IObservable<HandDestroyedArgs> handDestroyed;
        private SafeNativeMethods.XnHandDestroy internalHandDestroy;
        private IntPtr handDestroyHandle;

        #endregion

    }

    /// <summary>
    /// Provides data for hand created event.
    /// </summary>
    public class HandCreatedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the HandCreatedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public HandCreatedArgs(UserId userId, Point3D position, float timestamp, IntPtr cookie)
        {
            this.UserId = userId;
            this.Position = position;
            this.Timestamp = timestamp;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the new hand.
        /// </summary>
        public UserId UserId { get; private set; }

        /// <summary>
        /// Gets the position in which the hand was created. 
        /// </summary>
        public Point3D Position { get; private set; }

        /// <summary>
        /// Timestamp, in seconds.
        /// </summary>
        public float Timestamp { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

    /// <summary>
    /// Provides data for hand updated event.
    /// </summary>
    public class HandUpdatedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the HandUpdatedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public HandUpdatedArgs(UserId userId, Point3D position, float timestamp, IntPtr cookie)
        {
            this.UserId = userId;
            this.Position = position;
            this.Timestamp = timestamp;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the hand that moved.
        /// </summary>
        public UserId UserId { get; private set; }

        /// <summary>
        /// Gets the new position of the hand. 
        /// </summary>
        public Point3D Position { get; private set; }

        /// <summary>
        /// Timestamp, in seconds.
        /// </summary>
        public float Timestamp { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

    /// <summary>
    /// Provides data for hand destroyed event.
    /// </summary>
    public class HandDestroyedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the HandDestroyedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public HandDestroyedArgs(UserId userId, float timestamp, IntPtr cookie)
        {
            this.UserId = userId;
            this.Timestamp = timestamp;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the hand that disappeared.
        /// </summary>
        public UserId UserId { get; private set; }

        /// <summary>
        /// Timestamp, in seconds.
        /// </summary>
        public float Timestamp { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

}
