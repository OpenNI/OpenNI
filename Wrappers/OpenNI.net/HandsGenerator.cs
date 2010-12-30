using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserId = System.UInt32;

namespace OpenNI
{
    public class HandsGenerator : Generator
    {
        internal HandsGenerator(NodeSafeHandle nodeHandle, bool addRef)
			: base(nodeHandle, addRef)
        {
            this.internalHandCreate = new SafeNativeMethods.XnHandCreate(this.InternalHandCreate);
            this.internalHandUpdate = new SafeNativeMethods.XnHandUpdate(this.InternalHandUpdate);
            this.internalHandDestroy = new SafeNativeMethods.XnHandDestroy(this.InternalHandDestroy);
        }

        public HandsGenerator(Context context, Query query, EnumerationErrors errors) :
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

        private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
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
        public void StartTracking(ref Point3D position)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnStartTracking(this.InternalObject, ref position));
            
        }
        public void SetSmoothing(float factor)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnSetTrackingSmoothing(this.InternalObject, factor));
            
        }

        #region Hand Created
        private event EventHandler<HandCreatedArgs> handCreatedEvent;
        public event EventHandler<HandCreatedArgs> HandCreated
        {
            add
            {
                if (this.handCreatedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, this.internalHandCreate, null, null, IntPtr.Zero, out handCreateHandle));
                    
                }
                this.handCreatedEvent += value;
            }
            remove
            {
                this.handCreatedEvent -= value;

                if (this.handCreatedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handCreateHandle);
                }
            }
        }
        private void InternalHandCreate(NodeSafeHandle hNode, UserId id, ref Point3D position, float fTime, IntPtr pCookie)
        {
            var handler = this.handCreatedEvent;
            if (handler != null)
                handler(this, new HandCreatedArgs(id, position, fTime, pCookie));
        }
        private SafeNativeMethods.XnHandCreate internalHandCreate;
        private IntPtr handCreateHandle;
        #endregion

        #region Hand Updated
        private event EventHandler<HandUpdatedArgs> handUpdatedEvent;
        public event EventHandler<HandUpdatedArgs> HandUpdated
        {
            add
            {
                if (this.handUpdatedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, null, this.internalHandUpdate, null, IntPtr.Zero, out handUpdateHandle));
                    
                }
                this.handUpdatedEvent += value;
            }
            remove
            {
                this.handUpdatedEvent -= value;

                if (this.handUpdatedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handUpdateHandle);
                }
            } 
        }
        private void InternalHandUpdate(NodeSafeHandle hNode, UserId id, ref Point3D position, float fTime, IntPtr pCookie)
        {
            var handler = this.handUpdatedEvent;
            if (handler != null)
                handler(this, new HandUpdatedArgs(id, position, fTime, pCookie));
        }
        private SafeNativeMethods.XnHandUpdate internalHandUpdate;
        private IntPtr handUpdateHandle;
        #endregion

        #region Hand Destroy
        private event EventHandler<HandDestroyedArgs> handDestroyedEvent;
        public event EventHandler<HandDestroyedArgs> HandDestroyed
        {
            add
            {
                if (this.handDestroyedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, null, null, this.internalHandDestroy, IntPtr.Zero, out handDestroyHandle));
                    
                }
                this.handDestroyedEvent += value;
            }
            remove
            {
                this.handDestroyedEvent -= value;

                if (this.handDestroyedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handDestroyHandle);
                }
            }
        }
        private void InternalHandDestroy(NodeSafeHandle hNode, UserId id, float fTime, IntPtr pCookie)
        {
            var handler = this.handDestroyedEvent;
            if (handler != null)
                handler(this, new HandDestroyedArgs(id, fTime, pCookie));
        }
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
