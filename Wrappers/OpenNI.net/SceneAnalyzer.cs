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
﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace OpenNI
{
    public class SceneAnalyzer : MapGenerator
    {
		internal SceneAnalyzer(Context context, IntPtr nodeHandle, bool addRef)
			: base(context, nodeHandle, addRef)
        {

        }

        public SceneAnalyzer(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
        {
        }
        public SceneAnalyzer(Context context, Query query)
            : this(context, query, null)
        {
        }
        public SceneAnalyzer(Context context)
            : this(context, null, null)
        {
        }

        private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
        {
            IntPtr handle;
            int status =
                SafeNativeMethods.xnCreateSceneAnalyzer(context.InternalObject,
                                                        out handle,
                                                        query == null ? IntPtr.Zero : query.InternalObject,
                                                        errors == null ? IntPtr.Zero : errors.InternalObject);
            WrapperUtils.ThrowOnError(status);
            return handle;
        }

		public Plane3D Floor
        {
			get
			{
				Plane3D plane = new Plane3D();
				int status = SafeNativeMethods.xnGetFloor(this.InternalObject, ref plane);
				WrapperUtils.ThrowOnError(status);
				return plane;
			}
        }

        public IntPtr LabelMapPtr
        {
			get
			{
				return SafeNativeMethods.xnGetLabelMap(this.InternalObject);
			}
        }

        public UInt16MapData GetLabelMap()
        {
			MapOutputMode mode = this.MapOutputMode;
			return new UInt16MapData(mode.XRes, mode.YRes, LabelMapPtr);
        }

        // GetMetaData


    }
}
