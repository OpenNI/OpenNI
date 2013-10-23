/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class SceneMetaData : UInt16MapMetaData
	{
		public SceneMetaData()
		{
		}

		public int this[int index]
		{
			get { return GetAt(this.scene.pData, index); }
			set { SetAt(this.scene.pData, index, value); }
		}

		public int this[int x, int y]
		{
			get { return GetAt(this.scene.pData, x, y); }
			set { SetAt(this.scene.pData, x, y, value); }
		}

		public UInt16MapData GetLabelMap()
		{
			return new UInt16MapData(XRes, YRes, this.scene.pData);
		}

		public IntPtr LabelMapPtr
		{
			get { return this.scene.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new SceneMetaDataMarshaler(this, passOut);
		}

		internal SafeNativeMethods.XnSceneMetaData scene = new SafeNativeMethods.XnSceneMetaData();

		private class SceneMetaDataMarshaler : Marshaler<SafeNativeMethods.XnSceneMetaData>
		{
			public SceneMetaDataMarshaler(SceneMetaData obj, bool marshalOut) :
				base(obj.scene, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(SceneMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetaData)obj).GetMarshaler(marshalOut);
				obj.scene.pMap = inner.Native;
				return inner;
			}
		}
	}
}
