using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Linq;
using System.Runtime.InteropServices;
using UserId = System.UInt32;

namespace OpenNI
{
	public class UserGenerator : Generator
    {
        internal UserGenerator(NodeSafeHandle nodeHandle, bool addRef) : 
			base(nodeHandle, addRef)
        {
            this.internalNewUser = new SafeNativeMethods.XnUserHandler(this.OnNewUser);
            this.internalLostUser = new SafeNativeMethods.XnUserHandler(this.OnLostUser);

            // initialize the observables

            this.userFound = Observable.Create<UserFoundArgs>(observer =>
                {
                    // register the callback if required
                    if (this.userFoundObservers.Count == 0)
                    {
                        Status.ThrowOnFail(SafeNativeMethods.xnRegisterUserCallbacks(this.InternalObject, this.internalNewUser, null, IntPtr.Zero, out this.newUserHandle));
                    }
                    // add to the observers list
                    this.userFoundObservers.Add(observer);

                    // return the unregister method
                    return () =>
                        {
                            // remove form the observers list
                            this.userFoundObservers.Remove(observer);
                            // unregister the callback if possible
                            if (this.userFoundObservers.Count == 0)
                            {
                                SafeNativeMethods.xnUnregisterUserCallbacks(this.InternalObject, this.newUserHandle);
                            }
                        };
                });

            this.userLost = Observable.Create<UserLostArgs>(observer =>
            {
                // register the callback if required
                if (this.userLostObservers.Count == 0)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterUserCallbacks(this.InternalObject, null, this.internalLostUser, IntPtr.Zero, out this.lostUserHandle));
                }
                // add to the observers list
                this.userLostObservers.Add(observer);

                // return the unregister method
                return () =>
                {
                    // remove form the observers list
                    this.userLostObservers.Remove(observer);
                    // unregister the callback if possible
                    if (this.userLostObservers.Count == 0)
                    {
                        SafeNativeMethods.xnUnregisterUserCallbacks(this.InternalObject, this.lostUserHandle);
                    }
                };
            });
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

        public IObservable<UserFoundArgs> UserFound
        {
            get
            {
                return this.userFound;
            }
        }

        private void OnNewUser(IntPtr nodeHandle, UserId userId, IntPtr cookie)
        {
            this.userFoundObservers.ForEach(observer => observer.OnNext(new UserFoundArgs(userId, cookie)));
        }

        private readonly List<IObserver<UserFoundArgs>> userFoundObservers = new List<IObserver<UserFoundArgs>>();
        private readonly IObservable<UserFoundArgs> userFound;
        private SafeNativeMethods.XnUserHandler internalNewUser;
        private IntPtr newUserHandle;

        #endregion

        #region User Lost

        public IObservable<UserLostArgs> UserLost
        {
            get
            {
                return this.userLost;
            }
        }

        private void OnLostUser(IntPtr nodeHandle, UserId userId, IntPtr cookie)
        {
            this.userLostObservers.ForEach(observer => observer.OnNext(new UserLostArgs(userId, cookie)));
        }

        private readonly List<IObserver<UserLostArgs>> userLostObservers = new List<IObserver<UserLostArgs>>();
        private readonly IObservable<UserLostArgs> userLost;
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
