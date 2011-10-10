/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class IRGenerator : MapGenerator
	{
		internal IRGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public IRGenerator(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
		{
		}

		public IRGenerator(Context context, Query query) :
			this(context, query, null)
		{
		}

		public IRGenerator(Context context) :
			this(context, null, null)
		{
		}

		public IntPtr IRMapPtr
		{
			get
			{
				return SafeNativeMethods.xnGetIRMap(this.InternalObject);
			}
		}

		public UInt16MapData GetIRMap()
		{
			MapOutputMode mode = this.MapOutputMode;
			return new UInt16MapData(mode.XRes, mode.YRes, IRMapPtr);
		}

		public void GetMetaData(IRMetaData irMD)
		{
			using (IMarshaler marsh = irMD.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetIRMetaData(this.InternalObject, marsh.Native);
			}
		}

		public IRMetaData GetMetaData()
		{
			IRMetaData irMD = new IRMetaData();
			GetMetaData(irMD);
			return irMD;
		}

		private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateIRGenerator(context.InternalObject, out handle,
				query == null ? IntPtr.Zero : query.InternalObject,
				errors == null ? IntPtr.Zero : errors.InternalObject);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}
	}
}