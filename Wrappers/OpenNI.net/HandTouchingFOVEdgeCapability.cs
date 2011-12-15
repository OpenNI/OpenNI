/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
﻿using System;
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
