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

namespace OpenNI
{
	public class GestureRecognizedEventArgs : EventArgs
	{
		public GestureRecognizedEventArgs(string gesture, Point3D identifiedPosition, Point3D endPosition)
		{
			this.gesture = gesture;
			this.identifiedPosition = identifiedPosition;
			this.endPosition = endPosition;
		}

		public string Gesture
		{
			get { return gesture; }
			set { gesture = value; }
		}
		public Point3D IdentifiedPosition
		{
			get { return identifiedPosition; }
			set { identifiedPosition = value; }
		}
		public Point3D EndPosition
		{
			get { return endPosition; }
			set { endPosition = value; }
		}

		private string gesture;
		private Point3D identifiedPosition;
		private Point3D endPosition;
	}

	public class GestureProgressEventArgs : EventArgs
	{
		public GestureProgressEventArgs(string gesture, Point3D position, float progress)
		{
			this.gesture = gesture;
			this.position = position;
			this.progress = progress;
		}

		public string Gesture
		{
			get { return gesture; }
			set { gesture = value; }
		}
		public Point3D Position
		{
			get { return position; }
			set { position = value; }
		}
		public float Progress
		{
			get { return progress; }
			set { progress = value; }
		}

		private string gesture;
		private Point3D position;
		private float progress;
	}

    public class GestureIntermediateStageCompletedEventArgs : EventArgs
    {
        public GestureIntermediateStageCompletedEventArgs(string gesture, Point3D position)
        {
            this.gesture = gesture;
            this.position = position;
        }

        public string Gesture
        {
            get { return gesture; }
            set { gesture = value; }
        }
        public Point3D Position
        {
            get { return position; }
            set { position = value; }
        }

        private string gesture;
        private Point3D position;
    }

    public class GestureReadyForNextIntermediateStageEventArgs : EventArgs
    {
        public GestureReadyForNextIntermediateStageEventArgs(string gesture, Point3D position)
        {
            this.gesture = gesture;
            this.position = position;
        }

        public string Gesture
        {
            get { return gesture; }
            set { gesture = value; }
        }
        public Point3D Position
        {
            get { return position; }
            set { position = value; }
        }

        private string gesture;
        private Point3D position;
    }

    public class GestureGenerator : Generator
    {
		internal GestureGenerator(Context context, IntPtr nodeHandle, bool addRef)
			: base(context, nodeHandle, addRef)
        {
            this.gestureChanged = new StateChangedEvent(this,
                SafeNativeMethods.xnRegisterToGestureChange,
                SafeNativeMethods.xnUnregisterFromGestureChange);

            this.internalGestureRecognized = new SafeNativeMethods.XnGestureRecognized(this.InternalGestureRecognized);
            this.internalGestureProgress = new SafeNativeMethods.XnGestureProgress(this.InternalGestureProgress);
            this.internalGestureIntermediateStageCompleted = new SafeNativeMethods.XnGestureIntermediateStageCompleted(this.InternalGestureIntermediateStageCompleted);
            this.internalGestureReadyForNextIntermediateStage = new SafeNativeMethods.XnGestureReadyForNextIntermediateStage(this.InternalGestureReadyForNextIntermediateStage);
        }

        public GestureGenerator(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
        {
        }

        public GestureGenerator(Context context, Query query)
            : this(context, query, null)
        {
        }

        public GestureGenerator(Context context)
            : this(context, null, null)
        {
        }

        private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
        {
            IntPtr handle;
            int status =
                SafeNativeMethods.xnCreateGestureGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? IntPtr.Zero : query.InternalObject,
                                                        errors == null ? IntPtr.Zero : errors.InternalObject);
            WrapperUtils.ThrowOnError(status);
            return handle;
        }

        public void AddGesture(string gesture, BoundingBox3D area)
        {
			int status = SafeNativeMethods.xnAddGesture(InternalObject, gesture, ref area);
            WrapperUtils.ThrowOnError(status);
        }

		public void AddGesture(string gesture)
		{
			int status = SafeNativeMethods.xnAddGesture(InternalObject, gesture, IntPtr.Zero);
			WrapperUtils.ThrowOnError(status);
		}

		public void RemoveGesture(string gesture)
        {
			int status = SafeNativeMethods.xnRemoveGesture(InternalObject, gesture);
            WrapperUtils.ThrowOnError(status);
        }

        public string[] GetAllActiveGestures()
        {
            ushort count = 20;
            IntPtr[] arr = new IntPtr[count];
            const int nameSize = 80;
            string[] poses;

            try
            {
                for (int i = 0; i < count; ++i)
                {
                    arr[i] = Marshal.AllocHGlobal(nameSize);
                }

                int status = SafeNativeMethods.xnGetAllActiveGestures(this.InternalObject, arr, nameSize, ref count);
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

        public string[] EnumerateAllGestures()
        {
            ushort count = 20;
            IntPtr[] arr = new IntPtr[count];
            const int nameSize = 80;
            string[] poses;

            try
            {
                for (int i = 0; i < count; ++i)
                {
                    arr[i] = Marshal.AllocHGlobal(nameSize);
                }

                int status = SafeNativeMethods.xnEnumerateAllGestures(this.InternalObject, arr, nameSize, ref count);
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

        public int NumberOfEnumeratedGestures
        {
            get
            {
                return (Int32)SafeNativeMethods.xnGetNumberOfAvailableGestures(this.InternalObject);
            }
            
        }

		public bool IsGestureAvailable(string gesture)
        {
			return SafeNativeMethods.xnIsGestureAvailable(InternalObject, gesture);
        }

		public bool IsGestureProgressSupported(string gesture)
        {
			return SafeNativeMethods.xnIsGestureProgressSupported(InternalObject, gesture);
        }

		public event EventHandler GestureChanged
        {
            add
            {
                gestureChanged.Event += value;
            }
            remove
            {
                gestureChanged.Event -= value;
            }
        }

        private StateChangedEvent gestureChanged;

        #region Gesture Recognized
        private event EventHandler<GestureRecognizedEventArgs> gestureRecognizedEvent;
		public event EventHandler<GestureRecognizedEventArgs> GestureRecognized
        {
            add
            {
                if (this.gestureRecognizedEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterGestureCallbacks(this.InternalObject, this.internalGestureRecognized, null, IntPtr.Zero, out gestureRecognizedHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.gestureRecognizedEvent += value;
            }
            remove
            {
                this.gestureRecognizedEvent -= value;

                if (this.gestureRecognizedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterGestureCallbacks(this.InternalObject, this.gestureRecognizedHandle);
                }
            }
        }
        private void InternalGestureRecognized(IntPtr hNode, string strGesture, ref Point3D idPosition, ref Point3D endPosition, IntPtr pCookie)
        {
			EventHandler<GestureRecognizedEventArgs> handlers = this.gestureRecognizedEvent;
			if (handlers != null)
				handlers(this, new GestureRecognizedEventArgs(strGesture, idPosition, endPosition));
        }
        private SafeNativeMethods.XnGestureRecognized internalGestureRecognized;
        private IntPtr gestureRecognizedHandle;
        #endregion

        #region Gesture Progress
		private event EventHandler<GestureProgressEventArgs> gestureProgressEvent;
		public event EventHandler<GestureProgressEventArgs> GestureProgress
        {
            add
            {
                if (this.gestureProgressEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterGestureCallbacks(this.InternalObject, null, this.internalGestureProgress, IntPtr.Zero, out gestureProgressHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.gestureProgressEvent += value;
            }
            remove
            {
                this.gestureProgressEvent -= value;

                if (this.gestureProgressEvent == null)
                {
                    SafeNativeMethods.xnUnregisterGestureCallbacks(this.InternalObject, this.gestureProgressHandle);
                }
            }
        }
        private void InternalGestureProgress(IntPtr hNode, string strGesture, ref Point3D position, float progress, IntPtr pCookie)
        {
			EventHandler<GestureProgressEventArgs> handlers = this.gestureProgressEvent;
			if (handlers != null)
				handlers(this, new GestureProgressEventArgs(strGesture, position, progress));
        }
        private SafeNativeMethods.XnGestureProgress internalGestureProgress;
        private IntPtr gestureProgressHandle;
        #endregion

        #region Gesture intermediate stage completed
        private event EventHandler<GestureIntermediateStageCompletedEventArgs> gestureIntermediateStageCompletedEvent;
        public event EventHandler<GestureIntermediateStageCompletedEventArgs> GestureIntermediateStageCompleted
        {
            add
            {
                if (this.gestureIntermediateStageCompletedEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToGestureIntermediateStageCompleted(this.InternalObject, this.internalGestureIntermediateStageCompleted, IntPtr.Zero, out gestureIntermediateStageCompletedHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.gestureIntermediateStageCompletedEvent += value;
            }
            remove
            {
                this.gestureIntermediateStageCompletedEvent -= value;

                if (this.gestureIntermediateStageCompletedEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromGestureIntermediateStageCompleted(this.InternalObject, this.gestureIntermediateStageCompletedHandle);
                }
            }
        }
        private void InternalGestureIntermediateStageCompleted(IntPtr hNode, string strGesture, ref Point3D position, IntPtr pCookie)
        {
            EventHandler<GestureIntermediateStageCompletedEventArgs> handlers = this.gestureIntermediateStageCompletedEvent;
            if (handlers != null)
                handlers(this, new GestureIntermediateStageCompletedEventArgs(strGesture, position));
        }
        private SafeNativeMethods.XnGestureIntermediateStageCompleted internalGestureIntermediateStageCompleted;
        private IntPtr gestureIntermediateStageCompletedHandle;
        #endregion

        #region Gesture Ready for Next Intermediate Stage
        private event EventHandler<GestureReadyForNextIntermediateStageEventArgs> gestureReadyForNextIntermediateStageEvent;
        public event EventHandler<GestureReadyForNextIntermediateStageEventArgs> GestureReadyForNextIntermediateStage
        {
            add
            {
                if (this.gestureReadyForNextIntermediateStageEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToGestureReadyForNextIntermediateStage(this.InternalObject, this.internalGestureReadyForNextIntermediateStage, IntPtr.Zero, out gestureReadyForNextIntermediateStageHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.gestureReadyForNextIntermediateStageEvent += value;
            }
            remove
            {
                this.gestureReadyForNextIntermediateStageEvent -= value;

                if (this.gestureReadyForNextIntermediateStageEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromGestureReadyForNextIntermediateStage(this.InternalObject, this.gestureReadyForNextIntermediateStageHandle);
                }
            }
        }
        private void InternalGestureReadyForNextIntermediateStage(IntPtr hNode, string strGesture, ref Point3D position, IntPtr pCookie)
        {
            EventHandler<GestureReadyForNextIntermediateStageEventArgs> handlers = this.gestureReadyForNextIntermediateStageEvent;
            if (handlers != null)
                handlers(this, new GestureReadyForNextIntermediateStageEventArgs(strGesture, position));
        }
        private SafeNativeMethods.XnGestureReadyForNextIntermediateStage internalGestureReadyForNextIntermediateStage;
        private IntPtr gestureReadyForNextIntermediateStageHandle;
        #endregion
    }
}
