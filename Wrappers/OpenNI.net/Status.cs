using System;
using System.Diagnostics;
using System.Diagnostics.Contracts;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Security;

namespace OpenNI
{
    [Serializable]
    [DebuggerDisplay("Group = {Group}, Code = {Code}")]
    [StructLayout(LayoutKind.Sequential)]
    public struct Status
        : IEquatable<Status>
    {
        // predefined error groups
        // not an enum as new values can be defined by third-parties
        public const ushort NI = 1;
        public const ushort OS = 2;
        public const ushort PrimeSense = 3;

        private uint status;

        /** Definition of the OK error code. */
        public static readonly Status Ok = new Status(0);

        private Status(uint value)
        {
            this.status = value;
        }

        public Status(ushort group, ushort code)
        {
            this.status = ((uint)group << 16) | code;
        }

        [Pure]
        public ushort Group
        {
            get
            {
                return (ushort)(this.status >> 16);
            }
        }

        [Pure]
        public ushort Code
        {
            get
            {
                return (ushort)(this.status & 0x0000FFFF);
            }
        }

        [Pure]
        public bool IsOk
        {
            get
            {
                return (this.status == 0);
            }
        }

        internal static void ThrowOnFail(Status status)
        {
            if (!status.IsOk)
                throw new OpenNiException(status);
        }

        internal static void ThrowOnFail(Status status, ErrorCollection errors)
        {
            if (!status.IsOk)
            {
                if (errors != null && !errors.IsEmpty)
                {
                    throw new OpenNiException(errors.ToString()); // TODO: better handling
                }
                else
                {
                    throw new OpenNiException(status);
                }
            }
        }

        /// <summary>
        /// Gets a meaningful error string.
        /// </summary>
        /// <returns>A meaningful error string.</returns>
        public static string GetString(Status status)
        {
            return SafeNativeMethods.xnGetStatusString(status);
        }

        /// <summary>
        /// Get the name of the status.
        /// </summary>
        /// <returns>The name of the status.</returns>
        public static string GetName(Status status)
        {
            return SafeNativeMethods.xnGetStatusName(status);
        }

        public bool Equals(Status other)
        {
            return this.status.Equals(other.status);
        }

        public static bool operator ==(Status left, Status right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(Status left, Status right)
        {
            return !left.Equals(right);
        }

        #region object overrides

        public override bool Equals(object obj)
        {
            if (obj == null)
                return false; // a status type (struct) cannot be null
            if (!(obj is Status))
                return false;
            return Equals((Status)obj);
        }

        public override int GetHashCode()
        {
            return this.status.GetHashCode();
        }

        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "{0}: {1}", Status.GetName(this), Status.GetString(this));
        }

        #endregion

    }
}
