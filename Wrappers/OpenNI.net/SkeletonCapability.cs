using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace xn
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
            UInt32 status = OpenNIImporter.xnSetSkeletonProfile(this.InternalObject, profile);
            WrapperUtils.CheckStatus(status);
        }
        public void SetJointActive(SkeletonJoint joint, bool state)
        {
            UInt32 status = OpenNIImporter.xnSetJointActive(this.InternalObject, joint, state);
            WrapperUtils.CheckStatus(status);
        }
        public bool IsJointActive(SkeletonJoint joint)
        {
            return OpenNIImporter.xnIsJointActive(this.InternalObject, joint);
        }

        public event StateChangedHandler JointConfigurationChangedEvent
        {
            add { this.jointConfigurationChangedEvent.Event += value; }
            remove { this.jointConfigurationChangedEvent.Event -= value; }
        }

        private StateChangedEvent jointConfigurationChangedEvent;

        // EnuemrateActiveJoints

        public void GetSkeletonJoint(UserID user, SkeletonJoint eJoint, SkeletonJointTransformation joint)
        {
            UInt32 status = OpenNIImporter.xnGetSkeletonJoint(this.InternalObject, user, eJoint, joint);
            WrapperUtils.CheckStatus(status);
        }

        public void GetSkeletonJointPosition(UserID user, SkeletonJoint eJoint, ref SkeletonJointPosition joint)
        {
            UInt32 status = OpenNIImporter.xnGetSkeletonJointPosition(this.InternalObject, user, eJoint, ref joint);
            WrapperUtils.CheckStatus(status);
        }

        public void GetSkeletonJointOrientation(UserID user, SkeletonJoint eJoint, SkeletonJointOrientation joint)
        {
            UInt32 status = OpenNIImporter.xnGetSkeletonJointOrientation(this.InternalObject, user, eJoint, joint);
            WrapperUtils.CheckStatus(status);
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
            UInt32 status = OpenNIImporter.xnRequestSkeletonCalibration(this.InternalObject, user, force);
            WrapperUtils.CheckStatus(status);
        }

        public void AbortCalibration(UserID user)
        {
            UInt32 status = OpenNIImporter.xnAbortSkeletonCalibration(this.InternalObject, user);
            WrapperUtils.CheckStatus(status);
        }

        public void SaveCalibrationData(UserID user, UInt32 slot)
        {
            UInt32 status = OpenNIImporter.xnSaveSkeletonCalibrationData(this.InternalObject, user, slot);
            WrapperUtils.CheckStatus(status);
        }

        public void LoadCalibrationData(UserID user, UInt32 slot)
        {
            UInt32 status = OpenNIImporter.xnLoadSkeletonCalibrationData(this.InternalObject, user, slot);
            WrapperUtils.CheckStatus(status);
        }
        public void ClearCalibrationData(UInt32 slot)
        {
            UInt32 status = OpenNIImporter.xnClearSkeletonCalibrationData(this.InternalObject, slot);
            WrapperUtils.CheckStatus(status);
        }
        public bool IsCalibrationData(UInt32 slot)
        {
            return OpenNIImporter.xnIsSkeletonCalibrationData(this.InternalObject, slot);
        }

        public void StartTracking(UserID user)
        {
            UInt32 status = OpenNIImporter.xnStartSkeletonTracking(this.InternalObject, user);
            WrapperUtils.CheckStatus(status);
        }
        public void StopTracking(UserID user)
        {
            UInt32 status = OpenNIImporter.xnStopSkeletonTracking(this.InternalObject, user);
            WrapperUtils.CheckStatus(status);
        }
        public void Reset(UserID user)
        {
            UInt32 status = OpenNIImporter.xnResetSkeleton(this.InternalObject, user);
            WrapperUtils.CheckStatus(status);
        }

        public bool NeedPoseForCalibration()
        {
            return OpenNIImporter.xnNeedPoseForSkeletonCalibration(this.InternalObject);
        }
        public string GetCalibrationPose()
        {
            StringBuilder sb = new StringBuilder(1024);
            UInt32 status = OpenNIImporter.xnGetSkeletonCalibrationPose(this.InternalObject, sb);
            WrapperUtils.CheckStatus(status);
            return sb.ToString();
        }
        public void SetSmoothing(float factor)
        {
            UInt32 status = OpenNIImporter.xnSetSkeletonSmoothing(this.InternalObject, factor);
            WrapperUtils.CheckStatus(status);
        }

        #region Calibration Start
        public delegate void CalibrationStartHandler(ProductionNode node, UserID id);
        private event CalibrationStartHandler calibrationStartEvent;
        public event CalibrationStartHandler CalibrationStart
        {
            add
            {
                if (this.calibrationStartEvent == null)
                {
                    UInt32 status = OpenNIImporter.xnRegisterCalibrationCallbacks(this.InternalObject, internalCalibrationStart, null, IntPtr.Zero, out calibrationStartHandle);
                    WrapperUtils.CheckStatus(status);
                }
                this.calibrationStartEvent += value;
            }
            remove
            {
                this.calibrationStartEvent -= value;

                if (this.calibrationStartEvent == null)
                {
                    OpenNIImporter.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationStartHandle);
                }
            }
        }
        private void InternalCalibrationStart(NodeSafeHandle hNode, UserID id, IntPtr pCookie)
        {
            if (this.calibrationStartEvent != null)
                this.calibrationStartEvent(this.node, id);
        }
        private OpenNIImporter.XnCalibrationStart internalCalibrationStart;
        private IntPtr calibrationStartHandle;
        #endregion

        #region Calibration End
        public delegate void CalibrationEndHandler(ProductionNode node, UserID id, bool success);
        private event CalibrationEndHandler calibrationEndEvent;
        public event CalibrationEndHandler CalibrationEnd
        {
            add
            {
                if (this.calibrationEndEvent == null)
                {
                    UInt32 status = OpenNIImporter.xnRegisterCalibrationCallbacks(this.InternalObject, null, internalCalibrationEnd, IntPtr.Zero, out calibrationEndHandle);
                    WrapperUtils.CheckStatus(status);
                }
                this.calibrationEndEvent += value;
            }
            remove
            {
                this.calibrationEndEvent -= value;

                if (this.calibrationEndEvent == null)
                {
                    OpenNIImporter.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationEndHandle);
                }
            }
        }
        private void InternalCalibrationEnd(NodeSafeHandle hNode, UserID id, bool success, IntPtr pCookie)
        {
            if (this.calibrationEndEvent != null)
                this.calibrationEndEvent(this.node, id, success);
        }
        private OpenNIImporter.XnCalibrationEnd internalCalibrationEnd;
        private IntPtr calibrationEndHandle;
        #endregion
    }
}
