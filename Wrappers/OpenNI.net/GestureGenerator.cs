using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
    public class GestureGenerator : Generator
    {
        internal GestureGenerator(NodeSafeHandle nodeHandle, bool addRef)
            : base(nodeHandle, addRef)
        {
            this.gestureChanged = new StateChangedEvent(this,
                SafeNativeMethods.xnRegisterToGestureChange,
                SafeNativeMethods.xnUnregisterFromGestureChange);

            this.internalGestureRecognized = new SafeNativeMethods.XnGestureRecognized(this.InternalGestureRecognized);
            this.internalGestureProgress = new SafeNativeMethods.XnGestureProgress(this.InternalGestureProgress);
        }
        public GestureGenerator(Context context, Query query, ErrorCollection errors) :
            this(Create(context, query, errors), false)
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

        private static NodeSafeHandle Create(Context context, Query query, ErrorCollection errors)
        {
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateGestureGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? QuerySafeHandle.Zero : query.InternalObject,
                                                        errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
            
            return handle;
        }

        public void AddGesture(string gestureName, BoundingBox3D area)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnAddGesture(InternalObject, gestureName, ref area));
            
        }

		public void AddGesture(string gestureName)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnAddGesture(InternalObject, gestureName, IntPtr.Zero));
			
		}

		public void RemoveGesture(string gestureName)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnRemoveGesture(InternalObject, gestureName));
            
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

                Status.ThrowOnFail(SafeNativeMethods.xnGetAllActiveGestures(this.InternalObject, arr, nameSize, ref count));
                

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
        public string[] GetAllGestures()
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

                Status.ThrowOnFail(SafeNativeMethods.xnEnumerateAllGestures(this.InternalObject, arr, nameSize, ref count));
                

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

        public bool IsGestureAvailable(string gestureName)
        {
            return SafeNativeMethods.xnIsGestureAvailable(InternalObject, gestureName);
        }
        public bool IsGestureProgressSupported(string gestureName)
        {
            return SafeNativeMethods.xnIsGestureProgressSupported(InternalObject, gestureName);
        }
        public event EventHandler<StateChangedArgs> GestureChanged
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

        #region GestureName Recognized
        private event EventHandler<GestureRecognizedArgs> gestureRecognizedEvent;
        public event EventHandler<GestureRecognizedArgs> GestureRecognized
        {
            add
            {
                if (this.gestureRecognizedEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterGestureCallbacks(this.InternalObject, this.internalGestureRecognized, null, IntPtr.Zero, out gestureRecognizedHandle));
                    
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
        private void InternalGestureRecognized(IntPtr hNode, string gestureName, ref Point3D idPosition, ref Point3D endPosition, IntPtr pCookie)
        {
            var handler = this.gestureRecognizedEvent;
            if (handler != null)
                handler(this, new GestureRecognizedArgs(gestureName, idPosition, endPosition, pCookie));
        }
        private SafeNativeMethods.XnGestureRecognized internalGestureRecognized;
        private IntPtr gestureRecognizedHandle;
        #endregion

        #region GestureName Progress
        private event EventHandler<GestureProgressArgs> gestureProgressEvent;
        public event EventHandler<GestureProgressArgs> GestureProgress
        {
            add
            {
                if (this.gestureProgressEvent == null)
                {
                    Status.ThrowOnFail(SafeNativeMethods.xnRegisterGestureCallbacks(this.InternalObject, null, this.internalGestureProgress, IntPtr.Zero, out gestureProgressHandle));
                    
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
        private void InternalGestureProgress(IntPtr hNode, string gestureName, ref Point3D position, float progress, IntPtr pCookie)
        {
            var handler = this.gestureProgressEvent;
            if (handler != null)
                handler(this, new GestureProgressArgs(gestureName, position, progress, pCookie));
        }
        private SafeNativeMethods.XnGestureProgress internalGestureProgress;
        private IntPtr gestureProgressHandle;
        #endregion
    }

    /// <summary>
    /// Provides data for gestureName recognized event.
    /// </summary>
    public class GestureRecognizedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the GestureRecognizedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public GestureRecognizedArgs(string gestureName, Point3D identifiedPosition, Point3D endPosition, IntPtr cookie)
        {
            this.GestureName = gestureName;
            this.IdentifiedPosition = identifiedPosition;
            this.EndPosition = endPosition;
            this.Cookie = cookie;
        }

        /// <summary>
        /// The gestureName that was recognized.
        /// </summary>
        public string GestureName { get; private set; }

        /// <summary>
        /// The position in which the gestureName was identified.
        /// </summary>
        public Point3D IdentifiedPosition { get; private set; }

        /// <summary>
        /// The position of the hand that performed the gestureName at the end of the gestureName.
        /// </summary>
        public Point3D EndPosition { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

    /// <summary>
    /// Provides data for gestureName progressed event.
    /// </summary>
    public class GestureProgressArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the GestureProgressArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public GestureProgressArgs(string gestureName, Point3D position, float progress, IntPtr cookie)
        {
            this.GestureName = gestureName;
            this.Position = position;
            this.Progress = progress;
            this.Cookie = cookie;
        }

        /// <summary>
        /// The gestureName that is on its way to being recognized. 
        /// </summary>
        public string GestureName { get; private set; }

        /// <summary>
        /// The current position of the hand that is performing the gestureName. 
        /// </summary>
        public Point3D Position { get; private set; }

        /// <summary>
        /// The percentage of the gestureName that was already performed.
        /// </summary>
        public float Progress { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }


}
