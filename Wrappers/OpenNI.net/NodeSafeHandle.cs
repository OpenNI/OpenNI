﻿using System;
using System.Runtime.ConstrainedExecution;

namespace xn
{
    internal class NodeSafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly NodeSafeHandle Zero = new NodeSafeHandle();

        public NodeSafeHandle()
            : base(true)
        {

        }

        public NodeSafeHandle(IntPtr handle)
            : this()
        {
            this.SetHandle(handle);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            OpenNIImporter.xnProductionNodeRelease(this);

            return true;
        }
    }
}
