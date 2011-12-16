/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.Int32;
using System.Text;

namespace OpenNI
{
	public class PoseDetectedEventArgs : EventArgs
	{
		public PoseDetectedEventArgs(string pose, UserID id)
		{
			this.pose = pose;
			this.id = id;
		}

		public string Pose
		{
			get { return pose; }
			set { pose = value; }
		}

		public UserID ID
		{
			get { return id; }
			set { id = value; }
		}

		private string pose;
		private UserID id;
	}

	public class PoseEndedEventArgs : EventArgs
	{
		public PoseEndedEventArgs(string pose, UserID id)
		{
			this.pose = pose;
			this.id = id;
		}

		public string Pose
		{
			get { return pose; }
			set { pose = value; }
		}

		public UserID ID
		{
			get { return id; }
			set { id = value; }
		}

		private string pose;
		private UserID id;
	}
    public class OutOfPoseEventArgs : EventArgs
    {
        public OutOfPoseEventArgs(string pose, UserID id)
        {
            this.pose = pose;
            this.id = id;
        }

        public string Pose
        {
            get { return pose; }
            set { pose = value; }
        }

        public UserID ID
        {
            get { return id; }
            set { id = value; }
        }

        private string pose;
        private UserID id;
    }

    public class PoseInProgressEventArgs : EventArgs
    {
        public PoseInProgressEventArgs(string pose, UserID id, PoseDetectionStatus status)
        {
            this.pose = pose;
            this.id = id;
            this.status = status;
        }

        public string Pose
        {
            get { return pose; }
            set { pose = value; }
        }

        public UserID ID
        {
            get { return id; }
            set { id = value; }
        }
        public PoseDetectionStatus Status
        {
            get { return status; }
            set { status = value; }
        }

        private string pose;
        private UserID id;
        private PoseDetectionStatus status;
    }

    public class PoseDetectionCapability : Capability
    {
		internal PoseDetectionCapability(ProductionNode node)
            : base(node)
        {
            this.internalPoseDetected = new SafeNativeMethods.XnPoseDetectionCallback(this.InternalPoseDetected);
            this.internalPoseEnded = new SafeNativeMethods.XnPoseDetectionCallback(this.InternalPoseEnded);
            this.internalOutOfPose = new SafeNativeMethods.XnPoseDetectionCallback(this.InternalOutOfPose);
            this.internalPoseDetectionInProgress = new SafeNativeMethods.XnPoseDetectionInProgress(this.InternalPoseDetectionInProgress);
        }

        public int NumberOfPoses
        {
			get
			{
				return (Int32)SafeNativeMethods.xnGetNumberOfPoses(this.InternalObject);
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

				int status = SafeNativeMethods.xnGetAllAvailablePoses(this.InternalObject, arr, nameSize, ref count);
				WrapperUtils.ThrowOnError(status);

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

        public bool IsPoseSupported(string pose)
        {
            return SafeNativeMethods.xnIsPoseSupported(this.InternalObject,pose);
        }

        public void GetPoseStatus(UserID userID, string poseName, out UInt64 poseTime, out PoseDetectionStatus eStatus, out PoseDetectionState eState)
        {
            int status = SafeNativeMethods.xnGetPoseStatus(this.InternalObject,userID, poseName, out poseTime,out eStatus,out eState);
            WrapperUtils.ThrowOnError(status);
        }
		public void StartPoseDetection(string pose, UserID user)
        {
            int status = SafeNativeMethods.xnStartPoseDetection(this.InternalObject, pose, user);
            WrapperUtils.ThrowOnError(status);
        }
        public void StopPoseDetection(UserID user)
        {
			int status = SafeNativeMethods.xnStopPoseDetection(this.InternalObject, user); 
			WrapperUtils.ThrowOnError(status);
        }


        #region Pose Detected
        private event EventHandler<PoseDetectedEventArgs> poseDetectedEvent;
		public event EventHandler<PoseDetectedEventArgs> PoseDetected
        {
            add
            {
                if (this.poseDetectedEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToPoseDetected(this.InternalObject, internalPoseDetected, IntPtr.Zero, out poseDetectedHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.poseDetectedEvent += value;
            }
            remove
            {
                this.poseDetectedEvent -= value;

                if (this.poseDetectedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromPoseDetected(this.InternalObject, this.poseDetectedHandle);
                }
            }
        }
        private void InternalPoseDetected(IntPtr hNode, string pose, UserID id, IntPtr pCookie)
        {
			EventHandler<PoseDetectedEventArgs> handlers = this.poseDetectedEvent;
			if (handlers != null)
				handlers(this.node, new PoseDetectedEventArgs(pose, id));
        }
        private SafeNativeMethods.XnPoseDetectionCallback internalPoseDetected;
        private IntPtr poseDetectedHandle;
        #endregion

        #region Pose Ended
        private event EventHandler<PoseEndedEventArgs> poseEndedEvent;
        [Obsolete("use PoseDetectionCapability.OutOfPose instead")]
        public event EventHandler<PoseEndedEventArgs> PoseEnded
        {
            add
            {
                if (this.poseEndedEvent == null)
                {
					int status = SafeNativeMethods.xnRegisterToPoseCallbacks(this.InternalObject, null, this.internalPoseEnded, IntPtr.Zero, out poseEndedHandle);
                    WrapperUtils.ThrowOnError(status);
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
        private void InternalPoseEnded(IntPtr hNode, string pose, UserID id, IntPtr pCookie)
        {
			EventHandler<PoseEndedEventArgs> handlers = this.poseEndedEvent;
			if (handlers != null)
				handlers(this.node, new PoseEndedEventArgs(pose, id));
        }
        private SafeNativeMethods.XnPoseDetectionCallback internalPoseEnded;
        private IntPtr poseEndedHandle;
        #endregion

        #region Out of Pose
        private event EventHandler<OutOfPoseEventArgs> outOfPoseEvent;
        public event EventHandler<OutOfPoseEventArgs> OutOfPose
        {
            add
            {
                if (this.outOfPoseEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToOutOfPose(this.InternalObject, this.internalOutOfPose, IntPtr.Zero, out outOfPoseHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.outOfPoseEvent += value;
            }
            remove
            {
                this.outOfPoseEvent -= value;

                if (this.outOfPoseEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromOutOfPose(this.InternalObject, this.outOfPoseHandle);
                }
            }
        }
        private void InternalOutOfPose(IntPtr hNode, string pose, UserID id, IntPtr pCookie)
        {
            EventHandler<OutOfPoseEventArgs> handlers = this.outOfPoseEvent;
            if (handlers != null)
                handlers(this.node, new OutOfPoseEventArgs(pose, id));
        }
        private SafeNativeMethods.XnPoseDetectionCallback internalOutOfPose;
        private IntPtr outOfPoseHandle;
        #endregion

        #region Pose In Progress
        private event EventHandler<PoseInProgressEventArgs> poseDetectionInProgressEvent;
        public event EventHandler<PoseInProgressEventArgs> PoseDetectionInProgress
        {
            add
            {
                if (this.poseDetectionInProgressEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToPoseDetectionInProgress(this.InternalObject, this.internalPoseDetectionInProgress, IntPtr.Zero, out poseDetectionInProgressHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.poseDetectionInProgressEvent += value;
            }
            remove
            {
                this.poseDetectionInProgressEvent -= value;

                if (this.poseDetectionInProgressEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromPoseDetectionInProgress(this.InternalObject, this.poseDetectionInProgressHandle);
                }
            }
        }
        private void InternalPoseDetectionInProgress(IntPtr hNode, string pose, UserID id, PoseDetectionStatus status, IntPtr pCookie)
        {
            EventHandler<PoseInProgressEventArgs> handlers = this.poseDetectionInProgressEvent;
            if (handlers != null)
                handlers(this.node, new PoseInProgressEventArgs(pose, id, status));
        }
        private SafeNativeMethods.XnPoseDetectionInProgress internalPoseDetectionInProgress;
        private IntPtr poseDetectionInProgressHandle;
        #endregion

    }
}
