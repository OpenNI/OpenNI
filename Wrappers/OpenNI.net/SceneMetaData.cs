using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class SceneMetaData : MapMetaData<UInt16>
	{
		public SceneMetaData()
		{
		}

		public UInt16 this[int index]
		{
			get { return base[index, this.scene.pData]; }
			set { base[index, this.scene.pData] = value; }
		}

		public UInt16 this[int x, int y]
		{
			get { return base[x, y, this.scene.pData]; }
			set { base[x, y, this.scene.pData] = value; }
		}

		public MapData<UInt16> GetSceneMap()
		{
			return new MapData<UInt16>(XRes, YRes, this.scene.pData);
		}

		public IntPtr SceneMapPtr
		{
			get { return this.scene.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new SceneMetaDataMarshaler(this, passOut);
		}

		internal OpenNIImporter.XnSceneMetaData scene = new OpenNIImporter.XnSceneMetaData();

		private class SceneMetaDataMarshaler : Marshaler<OpenNIImporter.XnSceneMetaData>
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
