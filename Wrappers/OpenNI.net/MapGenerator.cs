using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public abstract class MapGenerator : Generator
	{
		internal MapGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
			this.mapOutputModeChanged = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToMapOutputModeChange,
				OpenNIImporter.xnUnregisterFromMapOutputModeChange);
		}

		public MapOutputMode[] SupportedMapOutputModes
		{
            get
            {
                uint count = OpenNIImporter.xnGetSupportedMapOutputModesCount(this.InternalObject);
                MapOutputMode[] supportedModes = new MapOutputMode[count];
                Status.ThrowOnFail(OpenNIImporter.xnGetSupportedMapOutputModes(this.InternalObject, supportedModes, ref count));
                return supportedModes;
            }
		}

		public MapOutputMode MapOutputMode
		{
            get
            {
                MapOutputMode mode = new MapOutputMode();
                Status.ThrowOnFail(OpenNIImporter.xnGetMapOutputMode(this.InternalObject, ref mode));
                return mode;
            }
            set
            {
			    Status.ThrowOnFail(OpenNIImporter.xnSetMapOutputMode(this.InternalObject, ref value));
            }
		}

		public event EventHandler<StateChangedArgs> MapOutputModeChanged
		{
			add { this.mapOutputModeChanged.Event += value; }
			remove { this.mapOutputModeChanged.Event -= value; }
		}

		public CroppingCapability GetCroppingCap()
		{
			return new CroppingCapability(this);
		}

		protected MapData<T> GetMapData<T>(IntPtr pData) where T: struct
		{
			MapOutputMode mode = this.MapOutputMode;
			return new MapData<T>((int)mode.XRes, (int)mode.YRes, pData);
		}

		private StateChangedEvent mapOutputModeChanged;
	}
}