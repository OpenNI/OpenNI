using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;
using System.Text;

namespace OpenNI
{
    public class PoseDetectionCapability : Capability
    {
        public PoseDetectionCapability(ProductionNode node)
            : base(node)
        {
            this.internalPoseDetected = new OpenNIImporter.XnPoseDetectionCallback(this.InternalPoseDetected);
            this.internalPoseEnded = new OpenNIImporter.XnPoseDetectionCallback(this.InternalPoseEnded);
        }

        public UInt32 GetNumberOfPoses()
        {
            return OpenNIImporter.xnGetNumberOfPoses(this.InternalObject);
        }

		public string[] GetAllAvailablePoses()
		{
			uint count = OpenNIImporter.xnGetNumberOfPoses(this.InternalObject);
			IntPtr[] arr = new IntPtr[count];
			const int nameSize = 80;
			string[] poses;

			try
			{
				for (int i = 0; i < count; ++i)
				{
					arr[i] = Marshal.AllocHGlobal(nameSize);
				}

                Status.ThrowOnFail(OpenNIImporter.xnGetAllAvailablePoses(this.InternalObject, arr, nameSize, ref count));
				

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
            Status.ThrowOnFail(OpenNIImporter.xnStartPoseDetection(this.InternalObject, pose, user));
            
        }
        public void StopPoseDetection(UserID user)
        {
            OpenNIImporter.xnStopPoseDetection(this.InternalObject, user);
        }

        #region Pose Detected
        private event EventHandler<PoseDetectionArgs> poseDetectedEvent;
        public event EventHandler<PoseDetectionArgs> PoseDetected
        {
            add
            {
                if (this.poseDetectedEvent == null)
                {
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterToPoseCallbacks(this.InternalObject, internalPoseDetected, null, IntPtr.Zero, out poseDetectedHandle));
                    
                }
                this.poseDetectedEvent += value;
            }
            remove
            {
                this.poseDetectedEvent -= value;

                if (this.poseDetectedEvent == null)
                {
                    OpenNIImporter.xnUnregisterFromPoseCallbacks(this.InternalObject, this.poseDetectedHandle);
                }
            }
        }
        private void InternalPoseDetected(NodeSafeHandle hNode, string pose, UserID id, IntPtr pCookie)
        {
            var handler = this.poseDetectedEvent;
            if (handler != null)
                handler(this, new PoseDetectionArgs(pose, id, pCookie));
        }
        private OpenNIImporter.XnPoseDetectionCallback internalPoseDetected;
        private IntPtr poseDetectedHandle;
        #endregion

        #region Pose Ended
        private event EventHandler<PoseDetectionArgs> poseEndedEvent;
        public event EventHandler<PoseDetectionArgs> PoseEnded
        {
            add
            {
                if (this.poseEndedEvent == null)
                {
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterToPoseCallbacks(this.InternalObject, null, InternalPoseEnded, IntPtr.Zero, out poseEndedHandle));
                    
                }
                this.poseEndedEvent += value;
            }
            remove
            {
                this.poseEndedEvent -= value;

                if (this.poseEndedEvent == null)
                {
                    OpenNIImporter.xnUnregisterFromPoseCallbacks(this.InternalObject, this.poseEndedHandle);
                }
            }
        }
        private void InternalPoseEnded(NodeSafeHandle hNode, string pose, UserID id, IntPtr pCookie)
        {
            var handler = this.poseEndedEvent;
            if (handler != null)
                handler(this, new PoseDetectionArgs(pose, id, pCookie));
        }
        private OpenNIImporter.XnPoseDetectionCallback internalPoseEnded;
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
        public PoseDetectionArgs(string pose, UserID userId, IntPtr cookie)
        {
            this.Pose = pose;
            this.UserID = userId;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the pose detected.
        /// </summary>
        public string Pose { get; private set; }

        /// <summary>
        /// Gets the id of the user that entered the pose or left it. 
        /// </summary>
        public UserID UserID { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

}
