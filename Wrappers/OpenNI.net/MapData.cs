using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class MapData<T>
	{
		public MapData(int xRes, int yRes, IntPtr dataBuff)
		{
			this.xRes = xRes;
			this.yRes = yRes;
			this.dataBuff = dataBuff;
		}

		public int XRes
		{
			get { return this.xRes; }
		}

		public int YRes
		{
			get { return this.yRes; }
		}

		public T this[int index]
		{
			get
			{
				IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + index * Marshal.SizeOf(typeof(T)));
				return (T)Marshal.PtrToStructure(ptr, typeof(T));
			}
			set
			{
				IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + index * Marshal.SizeOf(typeof(T)));
				Marshal.StructureToPtr(value, ptr, false);
			}
		}

		public T this[int x, int y]
		{
			get
			{
				IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + (y * XRes + x) * Marshal.SizeOf(typeof(T)));
				return (T)Marshal.PtrToStructure(ptr, typeof(T));
			}
			set
			{
				IntPtr ptr = new IntPtr(this.dataBuff.ToInt64() + (y * XRes + x) * Marshal.SizeOf(typeof(T)));
				Marshal.StructureToPtr(value, ptr, false);
			}
		}

		private int xRes;
		private int yRes;
		private IntPtr dataBuff;
	}
}
