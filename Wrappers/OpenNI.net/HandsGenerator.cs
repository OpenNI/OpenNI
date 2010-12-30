using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace xn
{
    public class HandsGenerator : Generator
    {
        internal HandsGenerator(NodeSafeHandle nodeHandle, bool addRef)
			: base(nodeHandle, addRef)
        {
            this.internalHandCreate = new OpenNIImporter.XnHandCreate(this.InternalHandCreate);
            this.internalHandUpdate = new OpenNIImporter.XnHandUpdate(this.InternalHandUpdate);
            this.internalHandDestroy = new OpenNIImporter.XnHandDestroy(this.InternalHandDestroy);
        }

        public HandsGenerator(Context context, Query query, EnumerationErrors errors) :
            this(Create(context, query, errors), false)
        {
        }
        public HandsGenerator(Context context, Query query)
            : this(context, query, null)
        {
        }
        public HandsGenerator(Context context)
            : this(context, null, null)
        {
        }

        private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
        {
            NodeSafeHandle handle;
            UInt32 status =
                OpenNIImporter.xnCreateHandsGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? QuerySafeHandle.Zero : query.InternalObject,
                                                        errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject);
            WrapperUtils.CheckStatus(status);
            return handle;
        }

        public void StopTracking(UserID id)
        {
            UInt32 status = OpenNIImporter.xnStopTracking(this.InternalObject, id);
            WrapperUtils.CheckStatus(status);
        }
        public void StopTrackingAll()
        {
            UInt32 status = OpenNIImporter.xnStopTrackingAll(this.InternalObject);
            WrapperUtils.CheckStatus(status);
        }
        public void StartTracking(ref Point3D position)
        {
            UInt32 status = OpenNIImporter.xnStartTracking(this.InternalObject, ref position);
            WrapperUtils.CheckStatus(status);
        }
        public void SetSmoothing(float factor)
        {
            UInt32 status = OpenNIImporter.xnSetTrackingSmoothing(this.InternalObject, factor);
            WrapperUtils.CheckStatus(status);
        }

        #region Hand Create
        public delegate void HandCreateHandler(ProductionNode node, UserID id, ref Point3D position, float fTime);
        private event HandCreateHandler handCreateEvent;
        public event HandCreateHandler HandCreate
        {
            add
            {
                if (this.handCreateEvent == null)
                {
                    UInt32 status = OpenNIImporter.xnRegisterHandCallbacks(this.InternalObject, this.internalHandCreate, null, null, IntPtr.Zero, out handCreateHandle);
                    WrapperUtils.CheckStatus(status);
                }
                this.handCreateEvent += value;
            }
            remove
            {
                this.handCreateEvent -= value;

                if (this.handCreateEvent == null)
                {
                    OpenNIImporter.xnUnregisterHandCallbacks(this.InternalObject, this.handCreateHandle);
                }
            }
        }
        private void InternalHandCreate(NodeSafeHandle hNode, UserID id, ref Point3D position, float fTime, IntPtr pCookie)
        {
            if (this.handCreateEvent != null)
                this.handCreateEvent(this, id, ref position, fTime);
        }
        private OpenNIImporter.XnHandCreate internalHandCreate;
        private IntPtr handCreateHandle;
        #endregion

        #region Hand Update
        public delegate void HandUpdateHandler(ProductionNode node, UserID id, ref Point3D position, float fTime);
        private event HandUpdateHandler handUpdateEvent;
        public event HandUpdateHandler HandUpdate
        {
            add
            {
                if (this.handUpdateEvent == null)
                {
                    UInt32 status = OpenNIImporter.xnRegisterHandCallbacks(this.InternalObject, null, this.internalHandUpdate, null, IntPtr.Zero, out handUpdateHandle);
                    WrapperUtils.CheckStatus(status);
                }
                this.handUpdateEvent += value;
            }
            remove
            {
                this.handUpdateEvent -= value;

                if (this.handUpdateEvent == null)
                {
                    OpenNIImporter.xnUnregisterHandCallbacks(this.InternalObject, this.handUpdateHandle);
                }
            } 
        }
        private void InternalHandUpdate(NodeSafeHandle hNode, UserID id, ref Point3D position, float fTime, IntPtr pCookie)
        {
            if (this.handUpdateEvent != null)
                this.handUpdateEvent(this, id, ref position, fTime);
        }
        private OpenNIImporter.XnHandUpdate internalHandUpdate;
        private IntPtr handUpdateHandle;
        #endregion

        #region Hand Destroy
        public delegate void HandDestroyHandler(ProductionNode node, UserID id, float fTime);
        private event HandDestroyHandler handDestroyEvent;
        public event HandDestroyHandler HandDestroy
        {
            add
            {
                if (this.handDestroyEvent == null)
                {
                    UInt32 status = OpenNIImporter.xnRegisterHandCallbacks(this.InternalObject, null, null, this.internalHandDestroy, IntPtr.Zero, out handDestroyHandle);
                    WrapperUtils.CheckStatus(status);
                }
                this.handDestroyEvent += value;
            }
            remove
            {
                this.handDestroyEvent -= value;

                if (this.handDestroyEvent == null)
                {
                    OpenNIImporter.xnUnregisterHandCallbacks(this.InternalObject, this.handDestroyHandle);
                }
            }
        }
        private void InternalHandDestroy(NodeSafeHandle hNode, UserID id, float fTime, IntPtr pCookie)
        {
            if (this.handDestroyEvent != null)
                this.handDestroyEvent(this, id, fTime);
        }
        private OpenNIImporter.XnHandDestroy internalHandDestroy;
        private IntPtr handDestroyHandle;
        #endregion

    }
}
