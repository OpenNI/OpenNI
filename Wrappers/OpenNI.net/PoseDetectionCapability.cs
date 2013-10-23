/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
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

    /// Utility structure to provide information about the state and status of a pose
    public struct PoseDetectionStateStatus
    {
        public Int64 m_poseTime; ///< @brief The time stamp in which the user entered into the pose (0 if not in pose). 

        /// @brief The status of the user's pose.
        /// 
        /// The progress error for getting into pose (PoseDetectionStatus, the same as received from
        /// the in progress callback. See @ref xnRegisterToPoseDetectionInProgress).
        public PoseDetectionStatus m_eStatus;
        public PoseDetectionState m_eState; ///< @brief The state of the user pose (i.e. in pose, out of pose).
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

        public PoseDetectionStateStatus GetPoseStatus(UserID userID, string poseName)
        {
            UInt64 outPoseTime;
            PoseDetectionStateStatus poseStatus = new PoseDetectionStateStatus();
            int status = SafeNativeMethods.xnGetPoseStatus(this.InternalObject,userID, poseName, out outPoseTime,out poseStatus.m_eStatus,out poseStatus.m_eState);
            poseStatus.m_poseTime = (Int64)outPoseTime;
            WrapperUtils.ThrowOnError(status);
            return poseStatus;
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
        public void StopSinglePoseDetection(UserID user, string pose)
        {
            int status = SafeNativeMethods.xnStopSinglePoseDetection(this.InternalObject, user, pose);
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
