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
