using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace OpenNI
{
    public class SkeletonCapability : Capability
    {
        public SkeletonCapability(ProductionNode node)
            : base(node)
        {
            this.jointConfigurationChangedEvent = new StateChangedEvent(node,
                OpenNIImporter.xnRegisterToJointConfigurationChange,
                OpenNIImporter.xnUnregisterFromJointConfigurationChange);

            this.internalCalibrationStart = new OpenNIImporter.XnCalibrationStart(this.InternalCalibrationStart);
            this.internalCalibrationEnd = new OpenNIImporter.XnCalibrationEnd(this.InternalCalibrationEnd);
        }

        public bool IsJointAvailable(SkeletonJoint joint)
        {
            return OpenNIImporter.xnIsJointAvailable(this.InternalObject, joint);
        }
        public bool IsProfileAvailable(SkeletonProfile profile)
        {
            return OpenNIImporter.xnIsProfileAvailable(this.InternalObject, profile);
        }

        public void SetSkeletonProfile(SkeletonProfile profile)
        {
            Status.ThrowOnFail(OpenNIImporter.xnSetSkeletonProfile(this.InternalObject, profile));
            
        }
        public void SetJointActive(SkeletonJoint joint, bool state)
        {
            Status.ThrowOnFail(OpenNIImporter.xnSetJointActive(this.InternalObject, joint, state));
            
        }
        public bool IsJointActive(SkeletonJoint joint)
        {
            return OpenNIImporter.xnIsJointActive(this.InternalObject, joint);
        }

        public event EventHandler<StateChangedArgs> JointConfigurationChangedEvent
        {
            add { this.jointConfigurationChangedEvent.Event += value; }
            remove { this.jointConfigurationChangedEvent.Event -= value; }
        }

        private StateChangedEvent jointConfigurationChangedEvent;

        // EnuemrateActiveJoints

        public void GetSkeletonJoint(UserID user, SkeletonJoint eJoint, SkeletonJointTransformation joint)
        {
            Status.ThrowOnFail(OpenNIImporter.xnGetSkeletonJoint(this.InternalObject, user, eJoint, joint));
            
        }

        public void GetSkeletonJointPosition(UserID user, SkeletonJoint eJoint, ref SkeletonJointPosition joint)
        {
            Status.ThrowOnFail(OpenNIImporter.xnGetSkeletonJointPosition(this.InternalObject, user, eJoint, ref joint));
            
        }

        public void GetSkeletonJointOrientation(UserID user, SkeletonJoint eJoint, SkeletonJointOrientation joint)
        {
            Status.ThrowOnFail(OpenNIImporter.xnGetSkeletonJointOrientation(this.InternalObject, user, eJoint, joint));
            
        }

        public bool IsTracking(UserID user)
        {
            return OpenNIImporter.xnIsSkeletonTracking(this.InternalObject, user);
        }

        public bool IsCalibrated(UserID user)
        {
            return OpenNIImporter.xnIsSkeletonCalibrated(this.InternalObject, user);
        }

        public bool IsCalibrating(UserID user)
        {
            return OpenNIImporter.xnIsSkeletonCalibrating(this.InternalObject, user);
        }

        public void RequestCalibration(UserID user, bool force)
        {
            Status.ThrowOnFail(OpenNIImporter.xnRequestSkeletonCalibration(this.InternalObject, user, force));
            
        }

        public void AbortCalibration(UserID user)
        {
            Status.ThrowOnFail(OpenNIImporter.xnAbortSkeletonCalibration(this.InternalObject, user));
            
        }

        public void SaveCalibrationData(UserID user, UInt32 slot)
        {
            Status.ThrowOnFail(OpenNIImporter.xnSaveSkeletonCalibrationData(this.InternalObject, user, slot));
            
        }

        public void LoadCalibrationData(UserID user, UInt32 slot)
        {
            Status.ThrowOnFail(OpenNIImporter.xnLoadSkeletonCalibrationData(this.InternalObject, user, slot));
            
        }
        public void ClearCalibrationData(UInt32 slot)
        {
            Status.ThrowOnFail(OpenNIImporter.xnClearSkeletonCalibrationData(this.InternalObject, slot));
            
        }
        public bool IsCalibrationData(UInt32 slot)
        {
            return OpenNIImporter.xnIsSkeletonCalibrationData(this.InternalObject, slot);
        }

        public void StartTracking(UserID user)
        {
            Status.ThrowOnFail(OpenNIImporter.xnStartSkeletonTracking(this.InternalObject, user));
            
        }
        public void StopTracking(UserID user)
        {
            Status.ThrowOnFail(OpenNIImporter.xnStopSkeletonTracking(this.InternalObject, user));
            
        }
        public void Reset(UserID user)
        {
            Status.ThrowOnFail(OpenNIImporter.xnResetSkeleton(this.InternalObject, user));
            
        }

        public bool NeedPoseForCalibration()
        {
            return OpenNIImporter.xnNeedPoseForSkeletonCalibration(this.InternalObject);
        }
        public string GetCalibrationPose()
        {
            StringBuilder sb = new StringBuilder(1024);
            Status.ThrowOnFail(OpenNIImporter.xnGetSkeletonCalibrationPose(this.InternalObject, sb));
            
            return sb.ToString();
        }
        public void SetSmoothing(float factor)
        {
            Status.ThrowOnFail(OpenNIImporter.xnSetSkeletonSmoothing(this.InternalObject, factor));
            
        }

        #region Calibration Start
        private event EventHandler<CalibrationStartedArgs> calibrationStartedEvent;
        public event EventHandler<CalibrationStartedArgs> CalibrationStarted
        {
            add
            {
                if (this.calibrationStartedEvent == null)
                {
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterCalibrationCallbacks(this.InternalObject, internalCalibrationStart, null, IntPtr.Zero, out calibrationStartHandle));
                    
                }
                this.calibrationStartedEvent += value;
            }
            remove
            {
                this.calibrationStartedEvent -= value;

                if (this.calibrationStartedEvent == null)
                {
                    OpenNIImporter.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationStartHandle);
                }
            }
        }
        private void InternalCalibrationStart(NodeSafeHandle hNode, UserID id, IntPtr cookie)
        {
            var handler = this.calibrationStartedEvent;
            if (handler != null)
                handler(this, new CalibrationStartedArgs(id, cookie));
        }
        private OpenNIImporter.XnCalibrationStart internalCalibrationStart;
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
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterCalibrationCallbacks(this.InternalObject, null, internalCalibrationEnd, IntPtr.Zero, out calibrationEndHandle));
                    
                }
                this.calibrationEndedEvent += value;
            }
            remove
            {
                this.calibrationEndedEvent -= value;

                if (this.calibrationEndedEvent == null)
                {
                    OpenNIImporter.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationEndHandle);
                }
            }
        }
        private void InternalCalibrationEnd(NodeSafeHandle hNode, UserID id, bool success, IntPtr cookie)
        {
            var handler = this.calibrationEndedEvent;
            if (handler != null)
                handler(this, new CalibrationEndedArgs(id, success, cookie));
        }
        private OpenNIImporter.XnCalibrationEnd internalCalibrationEnd;
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
        public CalibrationStartedArgs(UserID user, IntPtr cookie)
        {
            this.Cookie = cookie;
            this.UserID = UserID;
        }

        /// <summary>
        /// Gets the id of the user that's being calibrated.
        /// </summary>
        public UserID UserID { get; private set; }

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
        public CalibrationEndedArgs(UserID user, bool success, IntPtr cookie)
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
