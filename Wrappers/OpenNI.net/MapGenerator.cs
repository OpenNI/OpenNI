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
	public class MapGenerator : Generator
	{
		internal MapGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
			this.mapOutputModeChanged = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToMapOutputModeChange,
				SafeNativeMethods.xnUnregisterFromMapOutputModeChange);
            if(IsCapabilitySupported(Capabilities.AntiFlicker))
                m_antiFlickerCapability = new AntiFlickerCapability(this);
            else
                m_antiFlickerCapability = null;
            if (IsCapabilitySupported(Capabilities.Cropping))
                m_croppingCapablity = new CroppingCapability(this);
            else
                m_croppingCapablity = null;
            if (IsCapabilitySupported(Capabilities.Brightness))
                m_brightnessCapability = new GeneralIntCapability(this, Capabilities.Brightness);
            else
                m_brightnessCapability = null;
            if (IsCapabilitySupported(Capabilities.Contrast))
                m_contractCapability = new GeneralIntCapability(this, Capabilities.Contrast);
            else
                m_contractCapability = null;
            if (IsCapabilitySupported(Capabilities.Saturation))
                m_saturationCapability = new GeneralIntCapability(this, Capabilities.Saturation);
            else
                m_saturationCapability = null;
            if (IsCapabilitySupported(Capabilities.Hue))
                m_heuCapability = new GeneralIntCapability(this, Capabilities.Hue);
            else
                m_heuCapability = null;
            if (IsCapabilitySupported(Capabilities.Sharpness))
                m_sharpnessCapability = new GeneralIntCapability(this, Capabilities.Sharpness);
            else
                m_sharpnessCapability = null;
            if (IsCapabilitySupported(Capabilities.Gamma))
                m_gammaCapability = new GeneralIntCapability(this, Capabilities.Gamma);
            else
                m_gammaCapability = null;
            if (IsCapabilitySupported(Capabilities.WhiteBalance))
                m_whiteBalanceCapability = new GeneralIntCapability(this, Capabilities.WhiteBalance);
            else
                m_whiteBalanceCapability = null;
            if (IsCapabilitySupported(Capabilities.BacklightCompensation))
                m_backlightCompensationCapability = new GeneralIntCapability(this, Capabilities.BacklightCompensation);
            else
                m_backlightCompensationCapability = null;
            if (IsCapabilitySupported(Capabilities.Gain))
                m_gainCapability = new GeneralIntCapability(this, Capabilities.Gain);
            else
                m_gainCapability = null;
            if (IsCapabilitySupported(Capabilities.Pan))
                m_panCapability = new GeneralIntCapability(this, Capabilities.Pan);
            else
                m_panCapability = null;
            if (IsCapabilitySupported(Capabilities.Tilt))
                m_tiltCapability = new GeneralIntCapability(this, Capabilities.Tilt);
            else
                m_tiltCapability = null;
            if (IsCapabilitySupported(Capabilities.Roll))
                m_rollCapability = new GeneralIntCapability(this, Capabilities.Roll);
            else
                m_rollCapability = null;
            if (IsCapabilitySupported(Capabilities.Zoom))
                m_zoomCapability = new GeneralIntCapability(this, Capabilities.Zoom);
            else
                m_zoomCapability = null;
            if (IsCapabilitySupported(Capabilities.Exposure))
                m_exposureCapability = new GeneralIntCapability(this, Capabilities.Exposure);
            else
                m_exposureCapability = null;
            if (IsCapabilitySupported(Capabilities.Iris))
                m_irisCapability = new GeneralIntCapability(this, Capabilities.Iris);
            else
                m_irisCapability = null;
            if (IsCapabilitySupported(Capabilities.Focus))
                m_focusCapability = new GeneralIntCapability(this, Capabilities.Focus);
            else
                m_focusCapability = null;
            if (IsCapabilitySupported(Capabilities.LowLightCompensation))
                m_lowLightCompensationCapability = new GeneralIntCapability(this, Capabilities.LowLightCompensation);
            else
                m_lowLightCompensationCapability = null;
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

        public int BytesPerPixel
        {
            get
            {
                return (int)SafeNativeMethods.xnGetBytesPerPixel(this.InternalObject);
            }
        }

		public CroppingCapability CroppingCapability
		{
			get { return m_croppingCapablity; }
		}

        
		public GeneralIntCapability BrightnessCapability
		{
			get	{ return m_brightnessCapability;}
		}

		public GeneralIntCapability ContrastCapability
		{
			get { return m_contractCapability; }
		}

		public GeneralIntCapability SaturationCapability
		{
			get	{ return m_saturationCapability;}
		}

		public GeneralIntCapability HueCapability
		{
			get	{ return m_heuCapability; }
		}


		public GeneralIntCapability SharpnessCapability
		{
			get	{ return m_sharpnessCapability;	}
		}

		public GeneralIntCapability GammaCapability
		{
			get	{ return m_gammaCapability;	}
		}

		public GeneralIntCapability WhiteBalanceCapability
		{
			get	{ return m_whiteBalanceCapability; }
		}

		public GeneralIntCapability BacklightCompensationCapability
		{
			get	{ return m_backlightCompensationCapability; }
		}

		public GeneralIntCapability GainCapability
		{
			get	{ return m_gainCapability; }
		}

		public GeneralIntCapability PanCapability
		{
			get { return m_panCapability; }
		}

		public GeneralIntCapability TiltCapability
		{
			get	{ return m_tiltCapability; }
		}

		public GeneralIntCapability RollCapability
		{
			get { return m_rollCapability; }
		}

		public GeneralIntCapability ZoomCapability
		{
			get	{ return m_zoomCapability; }
		}


        public GeneralIntCapability ExposureCapability
		{
			get	{ return m_exposureCapability;	}
		}

		public GeneralIntCapability IrisCapability
		{
			get { return m_irisCapability; }
		}

		public GeneralIntCapability FocusCapability
		{
			get	{ return m_focusCapability;	}
		}

		public GeneralIntCapability LowLightCompensationCapability
		{
			get	{ return m_lowLightCompensationCapability;	}
		}

		public AntiFlickerCapability AntiFlickerCapability
		{
			get { return m_antiFlickerCapability; }
		}

		protected MapData<T> GetMapData<T>(IntPtr dataBuff)
		{
			MapOutputMode mode = this.MapOutputMode;
			return new MapData<T>(mode.XRes, mode.YRes, dataBuff);
		}

        ///  @todo this is a temporary solution for capability not being disposed by anyone external
        public override void Dispose()
        {
            if (m_antiFlickerCapability != null)
            {
                m_antiFlickerCapability.InternalDispose();
                m_antiFlickerCapability = null;
            }
            if (m_croppingCapablity != null)
            {
                m_croppingCapablity.InternalDispose();
                m_croppingCapablity = null;
            }
            if (m_brightnessCapability!=null)
            {
                m_brightnessCapability.InternalDispose();
                m_brightnessCapability=null;
            }
            if (m_contractCapability != null)
            {
                m_contractCapability.InternalDispose();
                m_contractCapability = null;
            }
            if (m_saturationCapability != null)
            {
                m_saturationCapability.InternalDispose();
                m_saturationCapability = null;
            }
            if (m_heuCapability != null)
            {
                m_heuCapability.InternalDispose();
                m_heuCapability = null;
            }
            if (m_sharpnessCapability != null)
            {
                m_sharpnessCapability.InternalDispose();
                m_sharpnessCapability = null;
            }
            if (m_gammaCapability != null)
            {
                m_gammaCapability.InternalDispose();
                m_gammaCapability = null;
            }
            if (m_whiteBalanceCapability != null)
            {
                m_whiteBalanceCapability.InternalDispose();
                m_whiteBalanceCapability = null;
            }
            if (m_backlightCompensationCapability != null)
            {
                m_backlightCompensationCapability.InternalDispose();
                m_backlightCompensationCapability = null;
            }
            if (m_gainCapability != null)
            {
                m_gainCapability.InternalDispose();
                m_gainCapability = null;
            }
            if (m_panCapability != null)
            {
                m_panCapability.InternalDispose();
                m_panCapability = null;
            }
            if (m_tiltCapability != null)
            {
                m_tiltCapability.InternalDispose();
                m_tiltCapability = null;
            }
            if (m_rollCapability != null)
            {
                m_rollCapability.InternalDispose();
                m_rollCapability = null;
            }
            if (m_zoomCapability != null)
            {
                m_zoomCapability.InternalDispose();
                m_zoomCapability = null;
            }
            if (m_exposureCapability != null)
            {
                m_exposureCapability.InternalDispose();
                m_exposureCapability = null;
            }
            if (m_irisCapability != null)
            {
                m_irisCapability.InternalDispose();
                m_irisCapability = null;
            }
            if (m_focusCapability != null)
            {
                m_focusCapability.InternalDispose();
                m_focusCapability = null;
            }
            if (m_lowLightCompensationCapability != null)
            {
                m_lowLightCompensationCapability.InternalDispose();
                m_lowLightCompensationCapability = null;
            }
            base.Dispose();
        }

		private StateChangedEvent mapOutputModeChanged;
        protected AntiFlickerCapability m_antiFlickerCapability;
        protected CroppingCapability m_croppingCapablity;

        protected GeneralIntCapability m_brightnessCapability;
        protected GeneralIntCapability m_contractCapability;
        protected GeneralIntCapability m_saturationCapability; 
        protected GeneralIntCapability m_heuCapability;
        protected GeneralIntCapability m_sharpnessCapability;
        protected GeneralIntCapability m_gammaCapability;
        protected GeneralIntCapability m_whiteBalanceCapability;
        protected GeneralIntCapability m_backlightCompensationCapability;
        protected GeneralIntCapability m_gainCapability;
        protected GeneralIntCapability m_panCapability;
        protected GeneralIntCapability m_tiltCapability;
        protected GeneralIntCapability m_rollCapability;
        protected GeneralIntCapability m_zoomCapability;
        protected GeneralIntCapability m_exposureCapability;
        protected GeneralIntCapability m_irisCapability;
        protected GeneralIntCapability m_focusCapability;
        protected GeneralIntCapability m_lowLightCompensationCapability;
	}
}