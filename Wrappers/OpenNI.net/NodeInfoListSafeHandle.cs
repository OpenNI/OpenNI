using System;
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

        public NodeInfoListSafeHandle(IntPtr handle)
            : this()
        {
            this.SetHandle(handle);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected override bool ReleaseHandle()
        {
            OpenNIImporter.xnNodeInfoListFree(this);

            return true;
        }
    }
}
