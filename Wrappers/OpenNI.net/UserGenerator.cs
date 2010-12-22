using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace xn
{
	public class UserGenerator : Generator
    {
        public UserGenerator(IntPtr nodeHandle, bool addRef) : 
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

        private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
        {
            IntPtr handle;
            UInt32 status =
                OpenNIImporter.xnCreateUserGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? IntPtr.Zero : query.InternalObject,
                                                        errors == null ? IntPtr.Zero : errors.InternalObject);
            WrapperUtils.CheckStatus(status);
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
            UInt32 status = OpenNIImporter.xnGetUsers(this.InternalObject, users, ref count);
            WrapperUtils.CheckStatus(status);
            return users;
        }
        
        public Point3D GetCoM(UserID id)
        {
            Point3D com = new Point3D();
            UInt32 status = OpenNIImporter.xnGetUserCoM(this.InternalObject, id, out com);
            WrapperUtils.CheckStatus(status);
            return com;
        }

        public SceneMetaData GetUserPixels(UserID id)
        {
            SceneMetaData smd = new SceneMetaData();
			using (IMarshaler marsh = smd.GetMarshaler(true))
			{
				UInt32 status = OpenNIImporter.xnGetUserPixels(this.InternalObject, id, marsh.Native);
				WrapperUtils.CheckStatus(status);
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

        #region New User
        public delegate void NewUserHandler(ProductionNode node, UserID id);
        private event NewUserHandler newUserEvent;
        public event NewUserHandler NewUser
        {
            add
            {
                if (this.newUserEvent == null)
                {
                    UInt32 status = OpenNIImporter.xnRegisterUserCallbacks(this.InternalObject, this.internalNewUser, null, IntPtr.Zero, out newUserHandle);
                    WrapperUtils.CheckStatus(status);
                }
                this.newUserEvent += value;
            }
            remove
            {
                this.newUserEvent -= value;

                if (this.newUserEvent == null)
                {
                    OpenNIImporter.xnUnregisterUserCallbacks(this.InternalObject, this.newUserHandle);
                }
            }
        }
        private void InternalNewUser(IntPtr hNode, UserID id, IntPtr pCookie)
        {
            if (this.newUserEvent != null)
                this.newUserEvent(this, id);
        }
        private OpenNIImporter.XnUserHandler internalNewUser;
        private IntPtr newUserHandle;
        #endregion

        #region Lost User
        public delegate void LostUserHandler(ProductionNode node, UserID id);
        private event LostUserHandler lostUserEvent;
        public event LostUserHandler LostUser
        {
            add
            {
                if (this.lostUserEvent == null)
                {
                    UInt32 status = OpenNIImporter.xnRegisterUserCallbacks(this.InternalObject, null, this.internalLostUser, IntPtr.Zero, out lostUserHandle);
                    WrapperUtils.CheckStatus(status);
                }
                this.lostUserEvent += value;
            }
            remove
            {
                this.lostUserEvent -= value;

                if (this.lostUserEvent == null)
                {
                    OpenNIImporter.xnUnregisterUserCallbacks(this.InternalObject, this.lostUserHandle);
                }
            }
        }
        private void InternalLostUser(IntPtr hNode, UserID id, IntPtr pCookie)
        {
            if (this.lostUserEvent != null)
                this.lostUserEvent(this, id);
        }
        private OpenNIImporter.XnUserHandler internalLostUser;
        private IntPtr lostUserHandle;
        #endregion
    }
}
