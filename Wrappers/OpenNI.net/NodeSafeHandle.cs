using System;
using System.Runtime.ConstrainedExecution;

namespace OpenNI
{
    internal class NodeSafeHandle
        : Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
    {
        public static readonly NodeSafeHandle Zero = new NodeSafeHandle();

        public NodeSafeHandle()
            : base(true)
        {

        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            SafeNativeMethods.xnProductionNodeRelease(this.handle);

            return true;
        }
    }
}
