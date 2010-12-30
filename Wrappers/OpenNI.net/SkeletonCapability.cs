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

        public event StateChangedHandler JointConfigurationChangedEvent
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
        public delegate void CalibrationStartHandler(ProductionNode node, UserID id);
        private event CalibrationStartHandler calibrationStartEvent;
        public event CalibrationStartHandler CalibrationStart
        {
            add
            {
                if (this.calibrationStartEvent == null)
                {
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterCalibrationCallbacks(this.InternalObject, internalCalibrationStart, null, IntPtr.Zero, out calibrationStartHandle));
                    
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
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterCalibrationCallbacks(this.InternalObject, null, internalCalibrationEnd, IntPtr.Zero, out calibrationEndHandle));
                    
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
