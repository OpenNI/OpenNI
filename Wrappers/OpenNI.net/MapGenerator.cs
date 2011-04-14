using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MapGenerator : Generator
	{
		internal MapGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
			this.mapOutputModeChanged = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToMapOutputModeChange,
				SafeNativeMethods.xnUnregisterFromMapOutputModeChange);
		}

		public MapOutputMode[] GetSupportedMapOutputModes()
		{
			uint count = SafeNativeMethods.xnGetSupportedMapOutputModesCount(this.InternalObject);
			MapOutputMode[] supportedModes = new MapOutputMode[count];
			int status = SafeNativeMethods.xnGetSupportedMapOutputModes(this.InternalObject, supportedModes, ref count);
			WrapperUtils.ThrowOnError(status);
			return supportedModes;
		}

		public MapOutputMode MapOutputMode
		{
			get
			{
				MapOutputMode mode = new MapOutputMode();
				int status = SafeNativeMethods.xnGetMapOutputMode(this.InternalObject, ref mode);
				WrapperUtils.ThrowOnError(status);
				return mode;
			}
			set
			{
				int status = SafeNativeMethods.xnSetMapOutputMode(this.InternalObject, ref value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		public event EventHandler MapOutputModeChanged
		{
			add { this.mapOutputModeChanged.Event += value; }
			remove { this.mapOutputModeChanged.Event -= value; }
		}

		public CroppingCapability CroppingCapability
		{
			get
			{
				return new CroppingCapability(this);
			}
		}

		public GeneralIntCapability BrightnessCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Brightness);
			}
		}

		public GeneralIntCapability ContrastCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Contrast);
			}
		}

		public GeneralIntCapability SaturationCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Saturation);
			}
		}

		public GeneralIntCapability HueCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Hue);
			}
		}

		public GeneralIntCapability SharpnessCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Sharpness);
			}
		}

		public GeneralIntCapability GammaCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Gamma);
			}
		}

		public GeneralIntCapability WhiteBalanceCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.WhiteBalance);
			}
		}

		public GeneralIntCapability BacklightCompensationCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.BacklightCompensation);
			}
		}

		public GeneralIntCapability GainCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Gain);
			}
		}

		public GeneralIntCapability PanCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Pan);
			}
		}

		public GeneralIntCapability TiltCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Tilt);
			}
		}

		public GeneralIntCapability RollCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Roll);
			}
		}

		public GeneralIntCapability ZoomCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Zoom);
			}
		}

		public GeneralIntCapability ExposureCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Exposure);
			}
		}

		public GeneralIntCapability IrisCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Iris);
			}
		}

		public GeneralIntCapability FocusCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.Focus);
			}
		}

		public GeneralIntCapability LowLightCompensationCapability
		{
			get
			{
				return new GeneralIntCapability(this, Capabilities.LowLightCompensation);
			}
		}

		public AntiFlickerCapability AntiFlickerCapability
		{
			get
			{
				return new AntiFlickerCapability(this);
			}
		}

		protected MapData<T> GetMapData<T>(IntPtr dataBuff)
		{
			MapOutputMode mode = this.MapOutputMode;
			return new MapData<T>(mode.XRes, mode.YRes, dataBuff);
		}

		private StateChangedEvent mapOutputModeChanged;
	}
}