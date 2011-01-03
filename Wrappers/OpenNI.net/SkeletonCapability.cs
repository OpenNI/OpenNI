using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserId = System.UInt32;

namespace OpenNI
{
    public class SkeletonCapability : Capability
    {
        public SkeletonCapability(ProductionNode node)
            : base(node)
        {
            this.jointConfigurationChangedEvent = new StateChangedEvent(node,
                SafeNativeMethods.xnRegisterToJointConfigurationChange,
                SafeNativeMethods.xnUnregisterFromJointConfigurationChange);

            this.internalCalibrationStart = new SafeNativeMethods.XnCalibrationStart(this.InternalCalibrationStart);
            this.internalCalibrationEnd = new SafeNativeMethods.XnCalibrationEnd(this.InternalCalibrationEnd);
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

        public event EventHandler<StateChangedArgs> JointConfigurationChangedEvent
        {
            add { this.jointConfigurationChangedEvent.Event += value; }
            remove { this.jointConfigurationChangedEvent.Event -= value; }
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

        #region Calibration Start
        private event EventHandler<CalibrationStartedArgs> calibrationStartedEvent;
        public event EventHandler<CalibrationStartedArgs> CalibrationStarted
        {
            add
            {
                if (this.calibrationStartedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterCalibrationCallbacks(this.InternalObject, internalCalibrationStart, null, IntPtr.Zero, out calibrationStartHandle));
                    
                }
                this.calibrationStartedEvent += value;
            }
            remove
            {
                this.calibrationStartedEvent -= value;

                if (this.calibrationStartedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationStartHandle);
                }
            }
        }
        private void InternalCalibrationStart(IntPtr hNode, UserId id, IntPtr cookie)
        {
            var handler = this.calibrationStartedEvent;
            if (handler != null)
                handler(this, new CalibrationStartedArgs(id, cookie));
        }
        private SafeNativeMethods.XnCalibrationStart internalCalibrationStart;
        private IntPtr calibrationStartHandle;
        #endregion

        #region Calibration End
        private event EventHandler<CalibrationEndedArgs> calibrationEndedEvent;
        public event EventHandler<CalibrationEndedArgs> CalibrationEnded
        {
            add
            {
                if (this.calibrationEndedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterCalibrationCallbacks(this.InternalObject, null, internalCalibrationEnd, IntPtr.Zero, out calibrationEndHandle));
                    
                }
                this.calibrationEndedEvent += value;
            }
            remove
            {
                this.calibrationEndedEvent -= value;

                if (this.calibrationEndedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationEndHandle);
                }
            }
        }
        private void InternalCalibrationEnd(IntPtr hNode, UserId id, bool success, IntPtr cookie)
        {
            var handler = this.calibrationEndedEvent;
            if (handler != null)
                handler(this, new CalibrationEndedArgs(id, success, cookie));
        }
        private SafeNativeMethods.XnCalibrationEnd internalCalibrationEnd;
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
