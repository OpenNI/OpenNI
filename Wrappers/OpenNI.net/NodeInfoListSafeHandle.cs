﻿using System;
using System.Runtime.ConstrainedExecution;

namespace OpenNI
{
    internal class NodeInfoListSafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly NodeInfoListSafeHandle Zero = new NodeInfoListSafeHandle();

        public NodeInfoListSafeHandle()
            : base(true)
        {

        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            SafeNativeMethods.xnNodeInfoListFree(this);

            return true;
        }
    }
}
