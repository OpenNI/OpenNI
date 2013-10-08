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
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.Int32;

namespace OpenNI
{
	public class CalibrationStartEventArgs : UserEventArgs
	{
		public CalibrationStartEventArgs(UserID id)
			: base(id)
		{
		}
	}

	public class CalibrationEndEventArgs : UserEventArgs
	{
		public CalibrationEndEventArgs(UserID id, bool success)
			: base(id)
		{
			this.success = success;
		}

		public bool Success
		{
			get { return success; }
			set { success = value; }
		}

		private bool success;
	}

    public class CalibrationProgressEventArgs : UserEventArgs
    {

        public CalibrationProgressEventArgs(UserID id, CalibrationStatus status)
            : base(id)
        {
            this.status = status;
        }
        public CalibrationStatus Status
        {
            get { return status; }
            set { status = value; }
        }
        private CalibrationStatus status;
    }

    public class SkeletonCapability : Capability
    {
		internal SkeletonCapability(ProductionNode node)
            : base(node)
        {
            this.jointConfigurationChangedEvent = new StateChangedEvent(node,
                SafeNativeMethods.xnRegisterToJointConfigurationChange,
                SafeNativeMethods.xnUnregisterFromJointConfigurationChange);

            this.internalCalibrationStart = new SafeNativeMethods.XnCalibrationStart(this.InternalCalibrationStart);
            this.internalCalibrationEnd = new SafeNativeMethods.XnCalibrationEnd(this.InternalCalibrationEnd);
            this.internalCalibrationInProgress = new SafeNativeMethods.XnCalibrationInProgress(this.InternalCalibrationInProgress);
            this.internalCalibrationComplete = new SafeNativeMethods.XnCalibrationComplete(this.InternalCalibrationComplete);
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
            int status = SafeNativeMethods.xnSetSkeletonProfile(this.InternalObject, profile);
            WrapperUtils.ThrowOnError(status);
        }
        public void SetJointActive(SkeletonJoint joint, bool state)
        {
            int status = SafeNativeMethods.xnSetJointActive(this.InternalObject, joint, state);
            WrapperUtils.ThrowOnError(status);
        }
        public bool IsJointActive(SkeletonJoint joint)
        {
            return SafeNativeMethods.xnIsJointActive(this.InternalObject, joint);
        }

        public event EventHandler JointConfigurationChangedEvent
        {
            add { this.jointConfigurationChangedEvent.Event += value; }
            remove { this.jointConfigurationChangedEvent.Event -= value; }
        }

        private StateChangedEvent jointConfigurationChangedEvent;

        // EnuemrateActiveJoints

		public SkeletonJointTransformation GetSkeletonJoint(UserID user, SkeletonJoint joint)
        {
			SkeletonJointTransformation transformation = new SkeletonJointTransformation();
			int status = SafeNativeMethods.xnGetSkeletonJoint(this.InternalObject, user, joint, ref transformation);
            WrapperUtils.ThrowOnError(status);
			return transformation;
        }

		public SkeletonJointPosition GetSkeletonJointPosition(UserID user, SkeletonJoint joint)
        {
			SkeletonJointPosition position = new SkeletonJointPosition();
			int status = SafeNativeMethods.xnGetSkeletonJointPosition(this.InternalObject, user, joint, ref position);
            WrapperUtils.ThrowOnError(status);
			return position;
        }

		public SkeletonJointOrientation GetSkeletonJointOrientation(UserID user, SkeletonJoint joint)
        {
			SkeletonJointOrientation orientation = new SkeletonJointOrientation();
			int status = SafeNativeMethods.xnGetSkeletonJointOrientation(this.InternalObject, user, joint, ref orientation);
            WrapperUtils.ThrowOnError(status);
			return orientation;
        }

        public bool IsTracking(UserID user)
        {
            return SafeNativeMethods.xnIsSkeletonTracking(this.InternalObject, user);
        }

        public bool IsCalibrated(UserID user)
        {
            return SafeNativeMethods.xnIsSkeletonCalibrated(this.InternalObject, user);
        }

        public bool IsCalibrating(UserID user)
        {
            return SafeNativeMethods.xnIsSkeletonCalibrating(this.InternalObject, user);
        }

        public void RequestCalibration(UserID user, bool force)
        {
            int status = SafeNativeMethods.xnRequestSkeletonCalibration(this.InternalObject, user, force);
            WrapperUtils.ThrowOnError(status);
        }

        public void AbortCalibration(UserID user)
        {
            int status = SafeNativeMethods.xnAbortSkeletonCalibration(this.InternalObject, user);
            WrapperUtils.ThrowOnError(status);
        }

		public void SaveCalibrationDataToFile(UserID user, string fileName)
        {
			int status = SafeNativeMethods.xnSaveSkeletonCalibrationDataToFile(this.InternalObject, user, fileName);
            WrapperUtils.ThrowOnError(status);
        }

        public void LoadCalibrationDataFromFile(UserID user, string fileName)
        {
			int status = SafeNativeMethods.xnLoadSkeletonCalibrationDataFromFile(this.InternalObject, user, fileName);
            WrapperUtils.ThrowOnError(status);
        }
		public void SaveCalibrationData(UserID user, Int32 slot)
        {
            int status = SafeNativeMethods.xnSaveSkeletonCalibrationData(this.InternalObject, user, (UInt32)slot);
            WrapperUtils.ThrowOnError(status);
        }

		public void LoadCalibrationData(UserID user, Int32 slot)
        {
			int status = SafeNativeMethods.xnLoadSkeletonCalibrationData(this.InternalObject, user, (UInt32)slot);
            WrapperUtils.ThrowOnError(status);
        }
        public void ClearCalibrationData(Int32 slot)
        {
			int status = SafeNativeMethods.xnClearSkeletonCalibrationData(this.InternalObject, (UInt32)slot);
            WrapperUtils.ThrowOnError(status);
        }
        public bool IsCalibrationData(Int32 slot)
        {
			return SafeNativeMethods.xnIsSkeletonCalibrationData(this.InternalObject, (UInt32)slot);
        }

        public void StartTracking(UserID user)
        {
            int status = SafeNativeMethods.xnStartSkeletonTracking(this.InternalObject, user);
            WrapperUtils.ThrowOnError(status);
        }
        public void StopTracking(UserID user)
        {
            int status = SafeNativeMethods.xnStopSkeletonTracking(this.InternalObject, user);
            WrapperUtils.ThrowOnError(status);
        }
        public void Reset(UserID user)
        {
            int status = SafeNativeMethods.xnResetSkeleton(this.InternalObject, user);
            WrapperUtils.ThrowOnError(status);
        }

        public bool DoesNeedPoseForCalibration
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
				int status = SafeNativeMethods.xnGetSkeletonCalibrationPose(this.InternalObject, sb);
				WrapperUtils.ThrowOnError(status);
				return sb.ToString();
			}
        }

        public void SetSmoothing(float factor)
        {
            int status = SafeNativeMethods.xnSetSkeletonSmoothing(this.InternalObject, factor);
            WrapperUtils.ThrowOnError(status);
        }

        #region Calibration Start
        private event EventHandler<CalibrationStartEventArgs> calibrationStartEvent;
		public event EventHandler<CalibrationStartEventArgs> CalibrationStart
        {
            add
            {
                if (this.calibrationStartEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToCalibrationStart(this.InternalObject, internalCalibrationStart, IntPtr.Zero, out calibrationStartHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.calibrationStartEvent += value;
            }
            remove
            {
                this.calibrationStartEvent -= value;

                if (this.calibrationStartEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromCalibrationStart(this.InternalObject, this.calibrationStartHandle);
                }
            }
        }
        private void InternalCalibrationStart(IntPtr hNode, UserID id, IntPtr pCookie)
        {
			EventHandler<CalibrationStartEventArgs> handlers = this.calibrationStartEvent;
			if (handlers != null)
				handlers(this.node, new CalibrationStartEventArgs(id));
        }
        private SafeNativeMethods.XnCalibrationStart internalCalibrationStart;
        private IntPtr calibrationStartHandle;
        #endregion

        #region Calibration End
		private event EventHandler<CalibrationEndEventArgs> calibrationEndEvent;
        [Obsolete("use SkeletonCapability.CalibrationComplete instead")]
        public event EventHandler<CalibrationEndEventArgs> CalibrationEnd
        {
            add
            {
                if (this.calibrationEndEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterCalibrationCallbacks(this.InternalObject, null, internalCalibrationEnd, IntPtr.Zero, out calibrationEndHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.calibrationEndEvent += value;
            }
            remove
            {
                this.calibrationEndEvent -= value;

                if (this.calibrationEndEvent == null)
                {
                    SafeNativeMethods.xnUnregisterCalibrationCallbacks(this.InternalObject, this.calibrationEndHandle);
                }
            }
        }
        private void InternalCalibrationEnd(IntPtr hNode, UserID id, bool success, IntPtr pCookie)
        {
			EventHandler<CalibrationEndEventArgs> handlers = this.calibrationEndEvent;
			if (handlers != null)
				handlers(this.node, new CalibrationEndEventArgs(id, success));
        }
        private SafeNativeMethods.XnCalibrationEnd internalCalibrationEnd;
        private IntPtr calibrationEndHandle;
        #endregion

        #region Calibration In Progress
        private event EventHandler<CalibrationProgressEventArgs> calibrationInProgressEvent;
        public event EventHandler<CalibrationProgressEventArgs> CalibrationInProgress
        {
            add
            {
                if (this.calibrationInProgressEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToCalibrationInProgress(this.InternalObject, internalCalibrationInProgress, IntPtr.Zero, out calibrationInProgressHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.calibrationInProgressEvent += value;
            }
            remove
            {
                this.calibrationInProgressEvent -= value;

                if (this.calibrationInProgressEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromCalibrationInProgress(this.InternalObject, this.calibrationInProgressHandle);
                }
            }
        }
        private void InternalCalibrationInProgress(IntPtr hNode, UserID id, CalibrationStatus status, IntPtr pCookie)
        {
            EventHandler<CalibrationProgressEventArgs> handlers = this.calibrationInProgressEvent;
            if (handlers != null)
                handlers(this.node, new CalibrationProgressEventArgs(id, status));
        }
        private SafeNativeMethods.XnCalibrationInProgress internalCalibrationInProgress;
        private IntPtr calibrationInProgressHandle;
        #endregion

        #region Calibration Complete
        private event EventHandler<CalibrationProgressEventArgs> calibrationCompleteEvent;
        public event EventHandler<CalibrationProgressEventArgs> CalibrationComplete
        {
            add
            {
                if (this.calibrationCompleteEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToCalibrationComplete(this.InternalObject, internalCalibrationComplete, IntPtr.Zero, out calibrationCompleteHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.calibrationCompleteEvent += value;
            }
            remove
            {
                this.calibrationCompleteEvent -= value;

                if (this.calibrationCompleteEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromCalibrationComplete(this.InternalObject, this.calibrationCompleteHandle);
                }
            }
        }
        private void InternalCalibrationComplete(IntPtr hNode, UserID id, CalibrationStatus status, IntPtr pCookie)
        {
            EventHandler<CalibrationProgressEventArgs> handlers = this.calibrationCompleteEvent;
            if (handlers != null)
                handlers(this.node, new CalibrationProgressEventArgs(id, status));
        }
        private SafeNativeMethods.XnCalibrationComplete internalCalibrationComplete;
        private IntPtr calibrationCompleteHandle;
        #endregion
    }
}
