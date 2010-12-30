﻿using System;
using System.Runtime.ConstrainedExecution;

namespace OpenNI
{
    internal class NodeInfoSafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly NodeInfoSafeHandle Zero = new NodeInfoSafeHandle();

        public NodeInfoSafeHandle()
            : base(true)
        {

        }

        public NodeInfoSafeHandle(IntPtr handle)
            : this()
        {
            this.SetHandle(handle);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            // no need to free anything

            return true;
        }
    }
}
