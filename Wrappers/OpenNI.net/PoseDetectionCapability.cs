﻿using System;
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
