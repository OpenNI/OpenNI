using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using UserId = System.UInt32;

namespace OpenNI
{
    public class SkeletonCapability : Capability
    {
        public SkeletonCapability(ProductionNode node)
            : base(node)
        {
            // initialize the observables

            this.jointConfigurationChangedEvent = new StateChangedEvent(node,
                SafeNativeMethods.xnRegisterToJointConfigurationChange,
                SafeNativeMethods.xnUnregisterFromJointConfigurationChange);

            this.calibrationStarted = new CallbackSubject<CalibrationStartedArgs>(
                () => Status.ThrowOnFail(SafeNativeMethods.xnRegisterCalibrationCallbacks(this.InternalObject, this.OnCalibrationStarted, null, IntPtr.Zero, out calibrationStartedHandle)),
                () => SafeNativeMethods.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationStartedHandle)
            );

            this.calibrationEnded = new CallbackSubject<CalibrationEndedArgs>(
                () => Status.ThrowOnFail(SafeNativeMethods.xnRegisterCalibrationCallbacks(this.InternalObject, null, this.OnCalibrationEnded, IntPtr.Zero, out this.calibrationEndHandle)),
                () => SafeNativeMethods.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationEndHandle)
            );

        }

        public bool IsJointAvailable(SkeletonJoint joint)
        {
            return SafeNativeMethods.xnIsJointAvailable(this.InternalObject, joint);
        }
        public bool IsProfileAvailable(SkeletonProfile profile)
        {
            return SafeNativeMethods.xnIsProfileAvailable(this.InternalObject, profile);
        }

        public void SetSkeletonProfile(SkeletonProfile profile)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnSetSkeletonProfile(this.InternalObject, profile));
            
        }
        public void SetJointActive(SkeletonJoint joint, bool state)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnSetJointActive(this.InternalObject, joint, state));
            
        }
        public bool IsJointActive(SkeletonJoint joint)
        {
            return SafeNativeMethods.xnIsJointActive(this.InternalObject, joint);
        }

        public IObservable<StateChangedArgs> JointConfigurationChangedEvent
        {
            get { return this.jointConfigurationChangedEvent.StateChanged; }
        }

        private StateChangedEvent jointConfigurationChangedEvent;

        // EnumerateActiveJoints

        public SkeletonJointTransformation GetSkeletonJoint(UserId user, SkeletonJoint eJoint)
        {
            SkeletonJointTransformation joint = new SkeletonJointTransformation();
            Status.ThrowOnFail(SafeNativeMethods.xnGetSkeletonJoint(this.InternalObject, user, eJoint, ref joint));
            return joint;
        }

        public SkeletonJointPosition GetSkeletonJointPosition(UserId user, SkeletonJoint eJoint)
        {
            SkeletonJointPosition joint = new SkeletonJointPosition();
            Status.ThrowOnFail(SafeNativeMethods.xnGetSkeletonJointPosition(this.InternalObject, user, eJoint, ref joint));
            return joint;
        }

        public SkeletonJointOrientation GetSkeletonJointOrientation(UserId user, SkeletonJoint eJoint)
        {
            SkeletonJointOrientation joint = new SkeletonJointOrientation();
            Status.ThrowOnFail(SafeNativeMethods.xnGetSkeletonJointOrientation(this.InternalObject, user, eJoint, ref joint));
            return joint;
        }

        public bool IsTracking(UserId user)
        {
            return SafeNativeMethods.xnIsSkeletonTracking(this.InternalObject, user);
        }

        public bool IsCalibrated(UserId user)
        {
            return SafeNativeMethods.xnIsSkeletonCalibrated(this.InternalObject, user);
        }

        public bool IsCalibrating(UserId user)
        {
            return SafeNativeMethods.xnIsSkeletonCalibrating(this.InternalObject, user);
        }

        public void RequestCalibration(UserId user, bool force)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnRequestSkeletonCalibration(this.InternalObject, user, force));
            
        }

        public void AbortCalibration(UserId user)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnAbortSkeletonCalibration(this.InternalObject, user));
            
        }

        public void SaveCalibrationData(UserId user, UInt32 slot)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnSaveSkeletonCalibrationData(this.InternalObject, user, slot));
            
        }

        public void LoadCalibrationData(UserId user, UInt32 slot)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnLoadSkeletonCalibrationData(this.InternalObject, user, slot));
            
        }
        public void ClearCalibrationData(UInt32 slot)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnClearSkeletonCalibrationData(this.InternalObject, slot));
            
        }
        public bool IsCalibrationData(UInt32 slot)
        {
            return SafeNativeMethods.xnIsSkeletonCalibrationData(this.InternalObject, slot);
        }

        public void StartTracking(UserId user)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnStartSkeletonTracking(this.InternalObject, user));
            
        }
        public void StopTracking(UserId user)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnStopSkeletonTracking(this.InternalObject, user));
            
        }
        public void Reset(UserId user)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnResetSkeleton(this.InternalObject, user));
            
        }

        public bool NeedPoseForCalibration
        {
            get
            {
                return SafeNativeMethods.xnNeedPoseForSkeletonCalibration(this.InternalObject);
            }
        }

        public string CalibrationPose
        {
            get
            {
                StringBuilder sb = new StringBuilder(1024);
                Status.ThrowOnFail(SafeNativeMethods.xnGetSkeletonCalibrationPose(this.InternalObject, sb));
                return sb.ToString();
            }
        }

        public void SetSmoothing(float factor)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnSetSkeletonSmoothing(this.InternalObject, factor));
            
        }

        #region Calibration Started

        public IObservable<CalibrationStartedArgs> CalibrationStarted
        {
            get
            {
                return this.calibrationStarted;
            }
        }

        private void OnCalibrationStarted(IntPtr nodeHandle, UserId userId, IntPtr cookie)
        {
            this.calibrationStarted.OnNext(new CalibrationStartedArgs(userId, cookie));
        }

        private readonly CallbackSubject<CalibrationStartedArgs> calibrationStarted;
        private IntPtr calibrationStartedHandle;

        #endregion

        #region Calibration End

        public IObservable<CalibrationEndedArgs> CalibrationEnded
        {
            get
            {
                return this.calibrationEnded;
            }
        }

        private void OnCalibrationEnded(IntPtr nodeHandle, UserId userId, bool success, IntPtr cookie)
        {
            this.calibrationEnded.OnNext(new CalibrationEndedArgs(userId, success, cookie));
        }

        private readonly CallbackSubject<CalibrationEndedArgs> calibrationEnded;
        private IntPtr calibrationEndHandle;

        #endregion
    }

    /// <summary>
    /// Provides data for calibration started event.
    /// </summary>
    public class CalibrationStartedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the CalibrationStartedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public CalibrationStartedArgs(UserId user, IntPtr cookie)
        {
            this.UserId = user;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the id of the user that's being calibrated.
        /// </summary>
        public UserId UserId { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

    /// <summary>
    /// Provides data for calibration ended event.
    /// </summary>
    public class CalibrationEndedArgs
        : CalibrationStartedArgs
    {
        /// <summary>
        /// Initializes a new instance of the CalibrationEndedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public CalibrationEndedArgs(UserId user, bool success, IntPtr cookie)
            : base(user, cookie)
        {
            this.Success = success;
        }

        /// <summary>
        /// Gets an indication of whether or not the calibration attempt succeeded.
        /// </summary>
        public bool Success { get; private set; }
    }
}
