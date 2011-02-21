using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using UserId = System.UInt32;

namespace OpenNI
{
    public class PoseDetectionCapability : Capability
    {
        public PoseDetectionCapability(ProductionNode node)
            : base(node)
        {
            // initialize the observables

            this.poseDetected = new CallbackSubject<PoseDetectionArgs>(
                () => Status.ThrowOnFail(SafeNativeMethods.xnRegisterToPoseCallbacks(this.InternalObject, this.OnPoseDetected, null, IntPtr.Zero, out this.poseDetectedHandle)),
                () => SafeNativeMethods.xnUnregisterFromPoseCallbacks(this.InternalObject, this.poseDetectedHandle)
            );

            this.poseEnded = new CallbackSubject<PoseDetectionArgs>(
                () => Status.ThrowOnFail(SafeNativeMethods.xnRegisterToPoseCallbacks(this.InternalObject, null, this.OnPoseEnded, IntPtr.Zero, out this.poseEndedHandle)),
                () => SafeNativeMethods.xnUnregisterFromPoseCallbacks(this.InternalObject, this.poseEndedHandle)
            );
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

        #region Pose Detected

        public IObservable<PoseDetectionArgs> PoseDetected
        {
            get
            {
                return this.poseDetected;
            }
        }

        private void OnPoseDetected(IntPtr nodeHandle, string poseName, UserId userId, IntPtr cookie)
        {
            this.poseDetected.OnNext(new PoseDetectionArgs(poseName, userId, cookie));
        }

        private readonly CallbackSubject<PoseDetectionArgs> poseDetected;
        private IntPtr poseDetectedHandle;

        #endregion

        #region Pose Ended

        public IObservable<PoseDetectionArgs> PoseEnded
        {
            get
            {
                return this.poseEnded;
            }
        }

        private void OnPoseEnded(IntPtr nodeHandle, string poseName, UserId userId, IntPtr cookie)
        {
            this.poseEnded.OnNext(new PoseDetectionArgs(poseName, userId, cookie));
        }

        private readonly CallbackSubject<PoseDetectionArgs> poseEnded;
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
