using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace OpenNI
{
	public class UserGenerator : Generator
    {
        internal UserGenerator(NodeSafeHandle nodeHandle, bool addRef) : 
			base(nodeHandle, addRef)
        {
            this.internalNewUser = new OpenNIImporter.XnUserHandler(this.InternalNewUser);
            this.internalLostUser = new OpenNIImporter.XnUserHandler(this.InternalLostUser);
        }

        public UserGenerator(Context context, Query query, EnumerationErrors errors) :
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

        private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
        {
            NodeSafeHandle handle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateUserGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? QuerySafeHandle.Zero : query.InternalObject,
                                                        errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
            
            return handle;
        }

        public UInt16 GetNumberOfUsers()
        {
            return OpenNIImporter.xnGetNumberOfUsers(this.InternalObject);
        }
        
        public UserID[] GetUsers()
        {
            ushort count = GetNumberOfUsers();
            UserID[] users = new UserID[count];
            Status.ThrowOnFail(OpenNIImporter.xnGetUsers(this.InternalObject, users, ref count));
            
            return users;
        }
        
        public Point3D GetCoM(UserID id)
        {
            Point3D com = new Point3D();
            Status.ThrowOnFail(OpenNIImporter.xnGetUserCoM(this.InternalObject, id, out com));
            
            return com;
        }

        public SceneMetaData GetUserPixels(UserID id)
        {
            SceneMetaData smd = new SceneMetaData();
			using (IMarshaler marsh = smd.GetMarshaler(true))
			{
                Status.ThrowOnFail(OpenNIImporter.xnGetUserPixels(this.InternalObject, id, marsh.Native));
				
			}

            return smd;
        }

        public SkeletonCapability GetSkeletonCap()
        {
            return new SkeletonCapability(this);
        }
        public PoseDetectionCapability GetPoseDetectionCap()
        {
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
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterUserCallbacks(this.InternalObject, this.internalNewUser, null, IntPtr.Zero, out newUserHandle));
                    
                }
                this.userFoundEvent += value;
            }
            remove
            {
                this.userFoundEvent -= value;

                if (this.userFoundEvent == null)
                {
                    OpenNIImporter.xnUnregisterUserCallbacks(this.InternalObject, this.newUserHandle);
                }
            }
        }
        private void InternalNewUser(NodeSafeHandle hNode, UserID id, IntPtr pCookie)
        {
            var handler = this.userFoundEvent;
            if (handler != null)
                handler(this, new UserFoundArgs(id, pCookie));
        }
        private OpenNIImporter.XnUserHandler internalNewUser;
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
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterUserCallbacks(this.InternalObject, null, this.internalLostUser, IntPtr.Zero, out lostUserHandle));
                    
                }
                this.userLostEvent += value;
            }
            remove
            {
                this.userLostEvent -= value;

                if (this.userLostEvent == null)
                {
                    OpenNIImporter.xnUnregisterUserCallbacks(this.InternalObject, this.lostUserHandle);
                }
            }
        }
        private void InternalLostUser(NodeSafeHandle hNode, UserID id, IntPtr pCookie)
        {
            var handler = this.userLostEvent;
            if (handler != null)
                handler(this, new UserLostArgs(id, pCookie));
        }
        private OpenNIImporter.XnUserHandler internalLostUser;
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
        public UserFoundArgs(UserID userId, IntPtr cookie)
        {
            this.UserID = userId;
           this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the user found.
        /// </summary>
        public UserID UserID { get; private set; }

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
        public UserLostArgs(UserID userId, IntPtr cookie)
        {
            this.UserID = userId;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the user lost.
        /// </summary>
        public UserID UserID { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }


}
