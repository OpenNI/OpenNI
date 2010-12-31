﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserId = System.UInt32;
using System.Text;

namespace OpenNI
{
    public class PoseDetectionCapability : Capability
    {
        public PoseDetectionCapability(ProductionNode node)
            : base(node)
        {
            this.internalPoseDetected = new SafeNativeMethods.XnPoseDetectionCallback(this.InternalPoseDetected);
            this.internalPoseEnded = new SafeNativeMethods.XnPoseDetectionCallback(this.InternalPoseEnded);
        }

        public UInt32 PoseCount
        {
            get
            {
                return SafeNativeMethods.xnGetNumberOfPoses(this.InternalObject);
            }
        }

		public string[] GetAllAvailablePoses()
		{
			uint count = SafeNativeMethods.xnGetNumberOfPoses(this.InternalObject);
			IntPtr[] arr = new IntPtr[count];
			const int nameSize = 80;
			string[] poses;

			try
			{
				for (int i = 0; i < count; ++i)
				{
					arr[i] = Marshal.AllocHGlobal(nameSize);
				}

                Status.ThrowOnFail(SafeNativeMethods.xnGetAllAvailablePoses(this.InternalObject, arr, nameSize, ref count));
				

				poses = new string[count];
				for (int i = 0; i < count; ++i)
				{
					poses[i] = Marshal.PtrToStringAnsi(arr[i]);
				}
			}
			finally
			{
				for (int i = 0; i < count; ++i)
				{
					if (arr[i] != IntPtr.Zero)
						Marshal.FreeHGlobal(arr[i]);
				}
			}

			return poses;
		}

		public void StartPoseDetection(string poseName, UserId user)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnStartPoseDetection(this.InternalObject, poseName, user));
            
        }
        public void StopPoseDetection(UserId user)
        {
            SafeNativeMethods.xnStopPoseDetection(this.InternalObject, user);
        }

        #region PoseName Detected
        private event EventHandler<PoseDetectionArgs> poseDetectedEvent;
        public event EventHandler<PoseDetectionArgs> PoseDetected
        {
            add
            {
                if (this.poseDetectedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterToPoseCallbacks(this.InternalObject, internalPoseDetected, null, IntPtr.Zero, out poseDetectedHandle));
                    
                }
                this.poseDetectedEvent += value;
            }
            remove
            {
                this.poseDetectedEvent -= value;

                if (this.poseDetectedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromPoseCallbacks(this.InternalObject, this.poseDetectedHandle);
                }
            }
        }
        private void InternalPoseDetected(NodeSafeHandle hNode, string poseName, UserId id, IntPtr pCookie)
        {
            var handler = this.poseDetectedEvent;
            if (handler != null)
                handler(this, new PoseDetectionArgs(poseName, id, pCookie));
        }
        private SafeNativeMethods.XnPoseDetectionCallback internalPoseDetected;
        private IntPtr poseDetectedHandle;
        #endregion

        #region PoseName Ended
        private event EventHandler<PoseDetectionArgs> poseEndedEvent;
        public event EventHandler<PoseDetectionArgs> PoseEnded
        {
            add
            {
                if (this.poseEndedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterToPoseCallbacks(this.InternalObject, null, InternalPoseEnded, IntPtr.Zero, out poseEndedHandle));
                    
                }
                this.poseEndedEvent += value;
            }
            remove
            {
                this.poseEndedEvent -= value;

                if (this.poseEndedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromPoseCallbacks(this.InternalObject, this.poseEndedHandle);
                }
            }
        }
        private void InternalPoseEnded(NodeSafeHandle hNode, string poseName, UserId id, IntPtr pCookie)
        {
            var handler = this.poseEndedEvent;
            if (handler != null)
                handler(this, new PoseDetectionArgs(poseName, id, pCookie));
        }
        private SafeNativeMethods.XnPoseDetectionCallback internalPoseEnded;
        private IntPtr poseEndedHandle;
        #endregion

    }

    /// <summary>
    /// Provides data for pose detection events.
    /// </summary>
    public class PoseDetectionArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the PoseDetectedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public PoseDetectionArgs(string poseName, UserId userId, IntPtr cookie)
        {
            this.PoseName = poseName;
            this.UserId = userId;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the pose detected.
        /// </summary>
        public string PoseName { get; private set; }

        /// <summary>
        /// Gets the id of the user that entered the pose or left it. 
        /// </summary>
        public UserId UserId { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

}
