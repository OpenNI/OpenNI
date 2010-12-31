using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Diagnostics.Contracts;

namespace OpenNI
{
	public abstract class MapGenerator : Generator
	{
		internal MapGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
			this.mapOutputModeChanged = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToMapOutputModeChange,
				SafeNativeMethods.xnUnregisterFromMapOutputModeChange);
		}

		public MapOutputMode[] GetSupportedMapOutputModes()
		{
            uint count = SafeNativeMethods.xnGetSupportedMapOutputModesCount(this.InternalObject);
            MapOutputMode[] supportedModes = new MapOutputMode[count];
            Status.ThrowOnFail(SafeNativeMethods.xnGetSupportedMapOutputModes(this.InternalObject, supportedModes, ref count));
            return supportedModes;
		}

		public MapOutputMode MapOutputMode
		{
            get
            {
                MapOutputMode mode = new MapOutputMode();
                Status.ThrowOnFail(SafeNativeMethods.xnGetMapOutputMode(this.InternalObject, ref mode));
                return mode;
            }
            set
            {
			    Status.ThrowOnFail(SafeNativeMethods.xnSetMapOutputMode(this.InternalObject, ref value));
            }
		}

		public event EventHandler<StateChangedArgs> MapOutputModeChanged
		{
			add { this.mapOutputModeChanged.Event += value; }
			remove { this.mapOutputModeChanged.Event -= value; }
		}

		public CroppingCapability GetCroppingCap()
		{
            Contract.Ensures(Contract.Result<CroppingCapability>() != null);

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