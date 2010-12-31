using System;
using System.Runtime.ConstrainedExecution;

namespace OpenNI
{
    internal class ContextSafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly ContextSafeHandle Zero = new ContextSafeHandle();

        public ContextSafeHandle()
            : base(true)
        {

        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            SafeNativeMethods.xnShutdown(this);

            return true;
        }
    }
}
