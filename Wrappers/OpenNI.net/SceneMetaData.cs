using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class SceneMetadata : MapMetadata<UInt16>
	{
		public SceneMetadata()
		{
		}

		public UInt16 this[int index]
		{
			get 
            {
                Contract.Requires(index >= 0 && index < this.FullXRes * this.FullYRes);

                return base[index, this.scene.pData]; 
            }
			set 
            {
                Contract.Requires(index >= 0 && index < this.FullXRes * this.FullYRes);

                base[index, this.scene.pData] = value; 
            }
		}

		public UInt16 this[int x, int y]
		{
			get 
            {
                Contract.Requires(x >= 0 && x < this.FullXRes);
                Contract.Requires(y >= 0 && y < this.FullYRes);

                return base[x, y, this.scene.pData]; 
            }
			set 
            {
                Contract.Requires(x >= 0 && x < this.FullXRes);
                Contract.Requires(y >= 0 && y < this.FullYRes);

                base[x, y, this.scene.pData] = value; 
            }
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
			return new SceneMetadataMarshaler(this, passOut);
		}

		internal SafeNativeMethods.XnSceneMetadata scene = new SafeNativeMethods.XnSceneMetadata();

		private class SceneMetadataMarshaler : Marshaler<SafeNativeMethods.XnSceneMetadata>
		{
			public SceneMetadataMarshaler(SceneMetadata obj, bool marshalOut) :
				base(obj.scene, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(SceneMetadata obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetadata)obj).GetMarshaler(marshalOut);
				obj.scene.pMap = inner.Native;
				return inner;
			}
		}
	}
}
