using System;
using System.Runtime.ConstrainedExecution;

namespace xn
{
    internal class ContextSafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly ContextSafeHandle Zero = new ContextSafeHandle();

        public ContextSafeHandle()
            : base(true)
        {

        }

        public ContextSafeHandle(IntPtr handle)
            : this()
        {
            this.SetHandle(handle);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            OpenNIImporter.xnShutdown(this);

            return true;
        }
    }
}
