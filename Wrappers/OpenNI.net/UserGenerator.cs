using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;
using UserId = System.UInt32;

namespace OpenNI
{
	public class UserGenerator : Generator
    {
        internal UserGenerator(NodeSafeHandle nodeHandle, bool addRef) : 
			base(nodeHandle, addRef)
        {
            this.internalNewUser = new SafeNativeMethods.XnUserHandler(this.InternalNewUser);
            this.internalLostUser = new SafeNativeMethods.XnUserHandler(this.InternalLostUser);
        }

        public UserGenerator(Context context, Query query, ErrorCollection errors) :
            this(Create(context, query, errors), false)
        {
        }
        public UserGenerator(Context context, Query query)
            : this(context, query, null)
        {
        }
        public UserGenerator(Context context)
            : this(context, null, null)
        {
        }

        private static NodeSafeHandle Create(Context context, Query query, ErrorCollection errors)
        {
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateUserGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? QuerySafeHandle.Zero : query.InternalObject,
                                                        errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
            
            return handle;
        }

        public UInt16 UserCount
        {
            get
            {
                return SafeNativeMethods.xnGetNumberOfUsers(this.InternalObject);
            }
        }
        
        public UserId[] GetUsers()
        {
            ushort count = this.UserCount;
            UserId[] users = new UserId[count];
            Status.ThrowOnFail(SafeNativeMethods.xnGetUsers(this.InternalObject, users, ref count));
            return users;
        }
        
        public Point3D GetCenterOfMass(UserId id)
        {
            Point3D com = new Point3D();
            Status.ThrowOnFail(SafeNativeMethods.xnGetUserCoM(this.InternalObject, id, out com));
            return com;
        }

        public SceneMetadata GetUserPixels(UserId id)
        {
            SceneMetadata smd = new SceneMetadata();
			using (IMarshaler marsh = smd.GetMarshaler(true))
			{
                Status.ThrowOnFail(SafeNativeMethods.xnGetUserPixels(this.InternalObject, id, marsh.Native));
				
			}

            return smd;
        }

        public SkeletonCapability GetSkeletonCap()
        {
            Contract.Ensures(Contract.Result<SkeletonCapability>() != null);

            return new SkeletonCapability(this);
        }

        public PoseDetectionCapability GetPoseDetectionCap()
        {
            Contract.Ensures(Contract.Result<PoseDetectionCapability>() != null);

            return new PoseDetectionCapability(this);
        }

        #region User Found
        private event EventHandler<UserFoundArgs> userFoundEvent;
        public event EventHandler<UserFoundArgs> UserFound
        {
            add
            {
                if (this.userFoundEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterUserCallbacks(this.InternalObject, this.internalNewUser, null, IntPtr.Zero, out newUserHandle));
                    
                }
                this.userFoundEvent += value;
            }
            remove
            {
                this.userFoundEvent -= value;

                if (this.userFoundEvent == null)
                {
                    SafeNativeMethods.xnUnregisterUserCallbacks(this.InternalObject, this.newUserHandle);
                }
            }
        }
        private void InternalNewUser(NodeSafeHandle hNode, UserId id, IntPtr pCookie)
        {
            var handler = this.userFoundEvent;
            if (handler != null)
                handler(this, new UserFoundArgs(id, pCookie));
        }
        private SafeNativeMethods.XnUserHandler internalNewUser;
        private IntPtr newUserHandle;
        #endregion

        #region User Lost
        private event EventHandler<UserLostArgs> userLostEvent;
        public event EventHandler<UserLostArgs> UserLost
        {
            add
            {
                if (this.userLostEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterUserCallbacks(this.InternalObject, null, this.internalLostUser, IntPtr.Zero, out lostUserHandle));
                    
                }
                this.userLostEvent += value;
            }
            remove
            {
                this.userLostEvent -= value;

                if (this.userLostEvent == null)
                {
                    SafeNativeMethods.xnUnregisterUserCallbacks(this.InternalObject, this.lostUserHandle);
                }
            }
        }
        private void InternalLostUser(NodeSafeHandle hNode, UserId id, IntPtr pCookie)
        {
            var handler = this.userLostEvent;
            if (handler != null)
                handler(this, new UserLostArgs(id, pCookie));
        }
        private SafeNativeMethods.XnUserHandler internalLostUser;
        private IntPtr lostUserHandle;
        #endregion
    }

    /// <summary>
    /// Provides data for user found event.
    /// </summary>
    public class UserFoundArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the UserFoundArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public UserFoundArgs(UserId userId, IntPtr cookie)
        {
            this.UserId = userId;
           this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the user found.
        /// </summary>
        public UserId UserId { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

    /// <summary>
    /// Provides data for user lost event.
    /// </summary>
    public class UserLostArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the UserLostArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public UserLostArgs(UserId userId, IntPtr cookie)
        {
            this.UserId = userId;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the user lost.
        /// </summary>
        public UserId UserId { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }


}
