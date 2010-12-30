using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class MapGenerator : Generator
	{
		internal MapGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
			this.mapOutputModeChanged = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToMapOutputModeChange,
				OpenNIImporter.xnUnregisterFromMapOutputModeChange);
		}

		public MapOutputMode[] GetSupportedMapOutputModes()
		{
			uint count = OpenNIImporter.xnGetSupportedMapOutputModesCount(this.InternalObject);
			MapOutputMode[] supportedModes = new MapOutputMode[count];
			UInt32 status = OpenNIImporter.xnGetSupportedMapOutputModes(this.InternalObject, supportedModes, ref count);
			WrapperUtils.CheckStatus(status);
			return supportedModes;
		}

		public void SetMapOutputMode(MapOutputMode mode)
		{
			UInt32 status = OpenNIImporter.xnSetMapOutputMode(this.InternalObject, ref mode);
			WrapperUtils.CheckStatus(status);
		}

		public MapOutputMode GetMapOutputMode()
		{
			MapOutputMode mode = new MapOutputMode();
			UInt32 status = OpenNIImporter.xnGetMapOutputMode(this.InternalObject, ref mode);
			WrapperUtils.CheckStatus(status);
			return mode;
		}

		public event StateChangedHandler MapOutputModeChanged
		{
			add { this.mapOutputModeChanged.Event += value; }
			remove { this.mapOutputModeChanged.Event -= value; }
		}

		public CroppingCapability GetCroppingCap()
		{
			return new CroppingCapability(this);
		}

		protected MapData<T> GetMapData<T>(IntPtr pData)
		{
			MapOutputMode mode = GetMapOutputMode();
			return new MapData<T>((int)mode.nXRes, (int)mode.nYRes, pData);
		}

		private StateChangedEvent mapOutputModeChanged;
	}
}