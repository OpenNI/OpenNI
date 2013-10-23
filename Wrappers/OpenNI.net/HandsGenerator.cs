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
using UserID = System.Int32;

namespace OpenNI
{
	public class HandCreateEventArgs : EventArgs
	{
		public HandCreateEventArgs(UserID id, Point3D position, float time)
		{
			this.id = id;
			this.position = position;
			this.time = time;
		}

		public UserID UserID
		{
			get { return id; }
			set { id = value; }
		}

		public OpenNI.Point3D Position
		{
			get { return position; }
			set { position = value; }
		}

		public float Time
		{
			get { return time; }
			set { time = value; }
		}

		private UserID id;
		private Point3D position;
		private float time;
	}

	public class HandUpdateEventArgs : EventArgs
	{
		public HandUpdateEventArgs(UserID id, Point3D position, float time)
		{
			this.id = id;
			this.position = position;
			this.time = time;
		}

		public UserID UserID
		{
			get { return id; }
			set { id = value; }
		}

		public OpenNI.Point3D Position
		{
			get { return position; }
			set { position = value; }
		}

		public float Time
		{
			get { return time; }
			set { time = value; }
		}

		private UserID id;
		private Point3D position;
		private float time;
	}

	public class HandDestroyEventArgs : EventArgs
	{
		public HandDestroyEventArgs(UserID id, float time)
		{
			this.id = id;
			this.time = time;
		}

		public UserID UserID
		{
			get { return id; }
			set { id = value; }
		}

		public float Time
		{
			get { return time; }
			set { time = value; }
		}

		private UserID id;
		private float time;
	}

    public class HandsGenerator : Generator
    {
		internal HandsGenerator(Context context, IntPtr nodeHandle, bool addRef)
			: base(context, nodeHandle, addRef)
        {
            this.internalHandCreate = new SafeNativeMethods.XnHandCreate(this.InternalHandCreate);
            this.internalHandUpdate = new SafeNativeMethods.XnHandUpdate(this.InternalHandUpdate);
            this.internalHandDestroy = new SafeNativeMethods.XnHandDestroy(this.InternalHandDestroy);
            if (IsCapabilitySupported(Capabilities.HandTouchingFOVEdge))
                m_HandTouchingFOVEdgeCapability = new HandTouchingFOVEdgeCapability(this);
            else
                m_HandTouchingFOVEdgeCapability = null;
        }

        public HandsGenerator(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
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

        private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
        {
            IntPtr handle;
            int status =
                SafeNativeMethods.xnCreateHandsGenerator(context.InternalObject,
                                                        out handle,
                                                        query == null ? IntPtr.Zero : query.InternalObject,
                                                        errors == null ? IntPtr.Zero : errors.InternalObject);
            WrapperUtils.ThrowOnError(status);
            return handle;
        }

        public void StopTracking(UserID id)
        {
            int status = SafeNativeMethods.xnStopTracking(this.InternalObject, id);
            WrapperUtils.ThrowOnError(status);
        }

        public void StopTrackingAll()
        {
            int status = SafeNativeMethods.xnStopTrackingAll(this.InternalObject);
            WrapperUtils.ThrowOnError(status);
        }

        public void StartTracking(Point3D position)
        {
            int status = SafeNativeMethods.xnStartTracking(this.InternalObject, ref position);
            WrapperUtils.ThrowOnError(status);
        }

        public void SetSmoothing(float factor)
        {
            int status = SafeNativeMethods.xnSetTrackingSmoothing(this.InternalObject, factor);
            WrapperUtils.ThrowOnError(status);
        }

        public HandTouchingFOVEdgeCapability HandTouchingFOVEdgeCapability
        {
            get { return m_HandTouchingFOVEdgeCapability; }
        }

        public override void Dispose()
        {
            if (m_HandTouchingFOVEdgeCapability != null)
            {
                m_HandTouchingFOVEdgeCapability.InternalDispose();
                m_HandTouchingFOVEdgeCapability = null;
            }
            base.Dispose();
        }
        protected HandTouchingFOVEdgeCapability m_HandTouchingFOVEdgeCapability;

        #region Hand Create
        private event EventHandler<HandCreateEventArgs> handCreateEvent;
		public event EventHandler<HandCreateEventArgs> HandCreate
        {
            add
            {
                if (this.handCreateEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, this.internalHandCreate, null, null, IntPtr.Zero, out handCreateHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.handCreateEvent += value;
            }
            remove
            {
                this.handCreateEvent -= value;

                if (this.handCreateEvent == null)
                {
                    SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handCreateHandle);
                }
            }
        }
        private void InternalHandCreate(IntPtr hNode, UserID id, ref Point3D position, float fTime, IntPtr pCookie)
        {
			EventHandler<HandCreateEventArgs> handlers = this.handCreateEvent;
			if (handlers != null)
				handlers(this, new HandCreateEventArgs(id, position, fTime));
        }
        private SafeNativeMethods.XnHandCreate internalHandCreate;
        private IntPtr handCreateHandle;
        #endregion

        #region Hand Update
		private event EventHandler<HandUpdateEventArgs> handUpdateEvent;
		public event EventHandler<HandUpdateEventArgs> HandUpdate
        {
            add
            {
                if (this.handUpdateEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, null, this.internalHandUpdate, null, IntPtr.Zero, out handUpdateHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.handUpdateEvent += value;
            }
            remove
            {
                this.handUpdateEvent -= value;

                if (this.handUpdateEvent == null)
                {
                    SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handUpdateHandle);
                }
            }
        }
        private void InternalHandUpdate(IntPtr hNode, UserID id, ref Point3D position, float fTime, IntPtr pCookie)
        {
			EventHandler<HandUpdateEventArgs> handlers = this.handUpdateEvent;
			if (handlers != null)
				handlers(this, new HandUpdateEventArgs(id, position, fTime));
        }
        private SafeNativeMethods.XnHandUpdate internalHandUpdate;
        private IntPtr handUpdateHandle;
        #endregion

        #region Hand Destroy
		private event EventHandler<HandDestroyEventArgs> handDestroyEvent;
		public event EventHandler<HandDestroyEventArgs> HandDestroy
        {
            add
            {
                if (this.handDestroyEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterHandCallbacks(this.InternalObject, null, null, this.internalHandDestroy, IntPtr.Zero, out handDestroyHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.handDestroyEvent += value;
            }
            remove
            {
                this.handDestroyEvent -= value;

                if (this.handDestroyEvent == null)
                {
                    SafeNativeMethods.xnUnregisterHandCallbacks(this.InternalObject, this.handDestroyHandle);
                }
            }
        }
        private void InternalHandDestroy(IntPtr hNode, UserID id, float fTime, IntPtr pCookie)
        {
			EventHandler<HandDestroyEventArgs> handlers = this.handDestroyEvent;
			if (handlers != null)
				handlers(this, new HandDestroyEventArgs(id, fTime));
        }
        private SafeNativeMethods.XnHandDestroy internalHandDestroy;
        private IntPtr handDestroyHandle;
        #endregion
    }
}
