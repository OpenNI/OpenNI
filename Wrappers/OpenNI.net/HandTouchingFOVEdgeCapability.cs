using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.Int32;

namespace OpenNI
{
    public class HandTouchingFOVEdgeEventArgs : EventArgs
    {
        public HandTouchingFOVEdgeEventArgs(UserID id, Point3D position, float time, Direction dir)
        {
            this.id = id;
            this.position = position;
            this.time = time;
            this.dir = dir;
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
        public Direction Direction
        {
            get { return dir; }
            set { dir = value; }
        }

        private UserID id;
        private Point3D position;
        private float time;
        private Direction dir;
    }

    public class HandTouchingFOVEdgeCapability : Capability
    {
        internal HandTouchingFOVEdgeCapability(ProductionNode node)
            : base(node)
        {
            this.internalHandTouchingFOVEdge = new SafeNativeMethods.XnHandTouchingFOVEdge(this.InternalHandTouchingFOVEdge);
        }
        #region Hand Touching FOV
        private event EventHandler<HandTouchingFOVEdgeEventArgs> handTouchingFOVEdgeEvent;
        public event EventHandler<HandTouchingFOVEdgeEventArgs> HandTouchingFOVEdge
        {
            add
            {
                if (this.handTouchingFOVEdgeEvent == null)
                {
                    int status = SafeNativeMethods.xnRegisterToHandTouchingFOVEdge(this.InternalObject, this.internalHandTouchingFOVEdge, IntPtr.Zero, out handTouchingFOVEdgeHandle);
                    WrapperUtils.ThrowOnError(status);
                }
                this.handTouchingFOVEdgeEvent += value;
            }
            remove
            {
                this.handTouchingFOVEdgeEvent -= value;

                if (this.handTouchingFOVEdgeEvent == null)
                {
                    SafeNativeMethods.xnUnregisterFromHandTouchingFOVEdge(this.InternalObject, this.handTouchingFOVEdgeHandle);
                }
            }
        }
        private void InternalHandTouchingFOVEdge(IntPtr hNode, UserID id, ref Point3D position, float fTime, Direction dir, IntPtr pCookie)
        {
            EventHandler<HandTouchingFOVEdgeEventArgs> handlers = this.handTouchingFOVEdgeEvent;
            if (handlers != null)
                handlers(this, new HandTouchingFOVEdgeEventArgs(id, position, fTime, dir));
        }
        private SafeNativeMethods.XnHandTouchingFOVEdge internalHandTouchingFOVEdge;
        private IntPtr handTouchingFOVEdgeHandle;
        #endregion
    }
}
