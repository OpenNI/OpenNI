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

    public class PoseDetectionCapability : Capability
    {
		internal PoseDetectionCapability(ProductionNode node)
            : base(node)
        {
            this.internalPoseDetected = new SafeNativeMethods.XnPoseDetectionCallback(this.InternalPoseDetected);
            this.internalPoseEnded = new SafeNativeMethods.XnPoseDetectionCallback(this.InternalPoseEnded);
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
                    int status = SafeNativeMethods.xnRegisterToPoseCallbacks(this.InternalObject, internalPoseDetected, null, IntPtr.Zero, out poseDetectedHandle);
                    WrapperUtils.ThrowOnError(status);
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

    }
}
