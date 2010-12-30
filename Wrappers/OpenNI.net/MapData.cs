using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace xn
{
	public class MapData<T> where T: struct
	{
		public MapData(int xRes, int yRes, IntPtr dataBuff)
		{
            Contract.Requires(xRes > 0);
            Contract.Requires(yRes > 0);
            Contract.Requires(dataBuff != IntPtr.Zero);

			this.xRes = xRes;
			this.yRes = yRes;
			this.dataBuff = dataBuff;
            this.sizeOfType = Marshal.SizeOf(typeof(T));
		}

        [Pure]
		public int XRes
		{
			get { return this.xRes; }
		}

        [Pure]
        public int YRes
		{
			get { return this.yRes; }
		}

		public T this[int index]
		{
			get
			{
                Contract.Requires(index >= 0 && index < this.XRes * this.YRes);

                IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + index * sizeOfType);
				return (T)Marshal.PtrToStructure(ptr, typeof(T));
			}
			set
			{
                Contract.Requires(index >= 0 && index < this.XRes * this.YRes);

                IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + index * sizeOfType);
				Marshal.StructureToPtr(value, ptr, false);
			}
		}

		public T this[int x, int y]
		{
			get
			{
                Contract.Requires(x >= 0 && x < this.XRes);
                Contract.Requires(y >= 0 && y < this.YRes);

                IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + (y * XRes + x) * sizeOfType);
				return (T)Marshal.PtrToStructure(ptr, typeof(T));
			}
			set
			{
                Contract.Requires(x >= 0 && x < this.XRes);
                Contract.Requires(y >= 0 && y < this.YRes);

                IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + (y * XRes + x) * sizeOfType);
				Marshal.StructureToPtr(value, ptr, false);
			}
		}

		private readonly int xRes;
		private readonly int yRes;
		private readonly IntPtr dataBuff;
        private readonly int sizeOfType;
	}
}
