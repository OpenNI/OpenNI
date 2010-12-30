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
                OpenNIImporter.xnRegisterToGestureChange,
                OpenNIImporter.xnUnregisterFromGestureChange);

            this.internalGestureRecognized = new OpenNIImporter.XnGestureRecognized(this.InternalGestureRecognized);
            this.internalGestureProgress = new OpenNIImporter.XnGestureProgress(this.InternalGestureProgress);
        }
        public GestureGenerator(Context context, Query query, EnumerationErrors errors) :
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

        private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
        {
            NodeSafeHandle handle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateGestureGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? QuerySafeHandle.Zero : query.InternalObject,
                                                        errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
            
            return handle;
        }

        public void AddGesture(string strGesture, BoundingBox3D area)
        {
            Status.ThrowOnFail(OpenNIImporter.xnAddGesture(InternalObject, strGesture, ref area));
            
        }

		public void AddGesture(string strGesture)
		{
            Status.ThrowOnFail(OpenNIImporter.xnAddGesture(InternalObject, strGesture, IntPtr.Zero));
			
		}

		public void RemoveGesture(string strGesture)
        {
            Status.ThrowOnFail(OpenNIImporter.xnRemoveGesture(InternalObject, strGesture));
            
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

                Status.ThrowOnFail(OpenNIImporter.xnGetAllActiveGestures(this.InternalObject, arr, nameSize, ref count));
                

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

                Status.ThrowOnFail(OpenNIImporter.xnEnumerateAllGestures(this.InternalObject, arr, nameSize, ref count));
                

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

        public bool IsGestureAvailable(string strGesture)
        {
            return OpenNIImporter.xnIsGestureAvailable(InternalObject, strGesture);
        }
        public bool IsGestureProgressSupported(string strGesture)
        {
            return OpenNIImporter.xnIsGestureProgressSupported(InternalObject, strGesture);
        }
        public event StateChangedHandler GestureChanged
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
        public delegate void GestureRecognizedHandler(ProductionNode node, string strGesture, ref Point3D idPosition, ref Point3D endPosition);
        private event GestureRecognizedHandler gestureRecognizedEvent;
        public event GestureRecognizedHandler GestureRecognized
        {
            add
            {
                if (this.gestureRecognizedEvent == null)
                {
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterGestureCallbacks(this.InternalObject, this.internalGestureRecognized, null, IntPtr.Zero, out gestureRecognizedHandle));
                    
                }
                this.gestureRecognizedEvent += value;
            }
            remove
            {
                this.gestureRecognizedEvent -= value;

                if (this.gestureRecognizedEvent == null)
                {
                    OpenNIImporter.xnUnregisterGestureCallbacks(this.InternalObject, this.gestureRecognizedHandle);
                }
            }
        }
        private void InternalGestureRecognized(NodeSafeHandle hNode, string strGesture, ref Point3D idPosition, ref Point3D endPosition, IntPtr pCookie)
        {
            if (this.gestureRecognizedEvent != null)
                this.gestureRecognizedEvent(this, strGesture, ref idPosition, ref endPosition);
        }
        private OpenNIImporter.XnGestureRecognized internalGestureRecognized;
        private IntPtr gestureRecognizedHandle;
        #endregion

        #region Gesture Progress
        public delegate void GestureProgressHandler(ProductionNode node, string strGesture, ref Point3D position, float progress);
        private event GestureProgressHandler gestureProgressEvent;
        public event GestureProgressHandler GestureProgress
        {
            add
            {
                if (this.gestureProgressEvent == null)
                {
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterGestureCallbacks(this.InternalObject, null, this.internalGestureProgress, IntPtr.Zero, out gestureProgressHandle));
                    
                }
                this.gestureProgressEvent += value;
            }
            remove
            {
                this.gestureProgressEvent -= value;

                if (this.gestureProgressEvent == null)
                {
                    OpenNIImporter.xnUnregisterGestureCallbacks(this.InternalObject, this.gestureProgressHandle);
                }
            }
        }
        private void InternalGestureProgress(NodeSafeHandle hNode, string strGesture, ref Point3D position, float progress, IntPtr pCookie)
        {
            if (this.gestureProgressEvent != null)
                this.gestureProgressEvent(this, strGesture, ref position, progress);
        }
        private OpenNIImporter.XnGestureProgress internalGestureProgress;
        private IntPtr gestureProgressHandle;
        #endregion
    }
}
