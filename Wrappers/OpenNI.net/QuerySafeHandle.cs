﻿using System;
using System.Runtime.ConstrainedExecution;

namespace OpenNI
{
    internal class QuerySafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly QuerySafeHandle Zero = new QuerySafeHandle();

        public QuerySafeHandle()
            : base(true)
        {

        }

        public QuerySafeHandle(IntPtr handle)
            : this()
        {
            this.SetHandle(handle);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            OpenNIImporter.xnNodeQueryFree(this);

            return true;
        }
    }
}
