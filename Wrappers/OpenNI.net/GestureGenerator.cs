using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace OpenNI
{
    public class GestureGenerator : Generator
    {
        internal GestureGenerator(NodeSafeHandle nodeHandle, bool addRef)
            : base(nodeHandle, addRef)
        {
            // initialize the observables

            this.gestureChanged = new StateChangedEvent(this,
                SafeNativeMethods.xnRegisterToGestureChange,
                SafeNativeMethods.xnUnregisterFromGestureChange);

            this.gestureRecognized = new CallbackSubject<GestureRecognizedArgs>(
                    () => Status.ThrowOnFail(SafeNativeMethods.xnRegisterGestureCallbacks(this.InternalObject, this.OnGestureRecognized, null, IntPtr.Zero, out this.gestureRecognizedHandle)),
                    () => SafeNativeMethods.xnUnregisterGestureCallbacks(this.InternalObject, this.gestureRecognizedHandle)
                );

            this.gestureProgressed = new CallbackSubject<GestureProgressedArgs>(
                    () => Status.ThrowOnFail(SafeNativeMethods.xnRegisterGestureCallbacks(this.InternalObject, null, this.OnGestureProgressed, IntPtr.Zero, out this.gestureProgressedHandle)),
                    () => SafeNativeMethods.xnUnregisterGestureCallbacks(this.InternalObject, this.gestureProgressedHandle)
                );

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

        public void AddGesture(string gesture, BoundingBox3D area)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnAddGesture(InternalObject, gesture, ref area));
            
        }

		public void AddGesture(string gesture)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnAddGesture(InternalObject, gesture, IntPtr.Zero));
			
		}

		public void RemoveGesture(string gesture)
        {
            Status.ThrowOnFail(SafeNativeMethods.xnRemoveGesture(InternalObject, gesture));
            
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

        public bool IsGestureAvailable(string gesture)
        {
            return SafeNativeMethods.xnIsGestureAvailable(InternalObject, gesture);
        }

        public bool IsGestureProgressSupported(string gesture)
        {
            return SafeNativeMethods.xnIsGestureProgressSupported(InternalObject, gesture);
        }

        #region Gesture Changed

        public IObservable<StateChangedArgs> GestureChanged
        {
            get { return this.gestureChanged.StateChanged; }
        }

        private StateChangedEvent gestureChanged;

        #endregion

        #region Gesture Recognized

        public IObservable<GestureRecognizedArgs> GestureRecognized
        {
            get
            {
                return this.gestureRecognized;
            }
        }

        private void OnGestureRecognized(IntPtr nodeHandle, string gesture, ref Point3D identifiedPosition, ref Point3D endPosition, IntPtr cookie)
        {
            this.gestureRecognized.OnNext(new GestureRecognizedArgs(gesture, identifiedPosition, endPosition, cookie));
        }

        private readonly CallbackSubject<GestureRecognizedArgs> gestureRecognized;
        private IntPtr gestureRecognizedHandle;

        #endregion

        #region Gesture Progress

        public IObservable<GestureProgressedArgs> GestureProgressed
        {
            get
            {
                return this.gestureProgressed;
            }
        }

        private void OnGestureProgressed(IntPtr nodeHandle, string gesture, ref Point3D position, float progress, IntPtr cookie)
        {
            this.gestureProgressed.OnNext(new GestureProgressedArgs(gesture, position, progress, cookie));
        }

        private readonly CallbackSubject<GestureProgressedArgs> gestureProgressed;
        private IntPtr gestureProgressedHandle;

        #endregion
    }

    /// <summary>
    /// Provides data for gesture recognized event.
    /// </summary>
    public class GestureRecognizedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the GestureRecognizedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public GestureRecognizedArgs(string gesture, Point3D identifiedPosition, Point3D endPosition, IntPtr cookie)
        {
            this.GestureName = gesture;
            this.IdentifiedPosition = identifiedPosition;
            this.EndPosition = endPosition;
            this.Cookie = cookie;
        }

        /// <summary>
        /// The gesture that was recognized.
        /// </summary>
        public string GestureName { get; private set; }

        /// <summary>
        /// The position in which the gesture was identified.
        /// </summary>
        public Point3D IdentifiedPosition { get; private set; }

        /// <summary>
        /// The position of the hand that performed the gesture at the end of the gesture.
        /// </summary>
        public Point3D EndPosition { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

    /// <summary>
    /// Provides data for gesture progressed event.
    /// </summary>
    public class GestureProgressedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the GestureProgressedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public GestureProgressedArgs(string gesture, Point3D position, float progress, IntPtr cookie)
        {
            this.GestureName = gesture;
            this.Position = position;
            this.Progress = progress;
            this.Cookie = cookie;
        }

        /// <summary>
        /// The gesture that is on its way to being recognized. 
        /// </summary>
        public string GestureName { get; private set; }

        /// <summary>
        /// The current position of the hand that is performing the gesture. 
        /// </summary>
        public Point3D Position { get; private set; }

        /// <summary>
        /// The percentage of the gesture that was already performed.
        /// </summary>
        public float Progress { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }


}
