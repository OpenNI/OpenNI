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

        public EnumerationErrorsSafeHandle(IntPtr handle)
            : this()
        {
            this.SetHandle(handle);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            OpenNIImporter.xnEnumerationErrorsFree(this);

            return true;
        }
    }
}
