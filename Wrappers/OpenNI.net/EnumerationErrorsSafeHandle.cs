using System;
using System.Runtime.ConstrainedExecution;

namespace OpenNI
{
    internal class EnumerationErrorsSafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly EnumerationErrorsSafeHandle Zero = new EnumerationErrorsSafeHandle();

        public EnumerationErrorsSafeHandle()
            : base(true)
        {

        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            SafeNativeMethods.xnEnumerationErrorsFree(this.handle);

            return true;
        }
    }
}
