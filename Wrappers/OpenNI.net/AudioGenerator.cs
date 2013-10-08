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
	public class AudioGenerator : Generator
	{
		internal AudioGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
			this.outputModeChanged = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToWaveOutputModeChanges,
				SafeNativeMethods.xnUnregisterFromWaveOutputModeChanges);
		}

		public AudioGenerator(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
		{
		}

		public AudioGenerator(Context context, Query query) :
			this(context, query, null)
		{
		}

		public AudioGenerator(Context context) :
			this(context, null, null)
		{
		}

		public WaveOutputMode[] GetSupportedWaveOutputModes()
		{
			uint count = SafeNativeMethods.xnGetSupportedWaveOutputModesCount(this.InternalObject);
			WaveOutputMode[] modes = new WaveOutputMode[count];
			int status = SafeNativeMethods.xnGetSupportedWaveOutputModes(this.InternalObject, modes, ref count);
			WrapperUtils.ThrowOnError(status);
			return modes;
		}

		public WaveOutputMode WaveOutputMode
		{
			get
			{
				WaveOutputMode mode = new WaveOutputMode();
				int status = SafeNativeMethods.xnGetWaveOutputMode(this.InternalObject, ref mode);
				WrapperUtils.ThrowOnError(status);
				return mode;
			}
			set
			{
				int status = SafeNativeMethods.xnSetWaveOutputMode(this.InternalObject, ref value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		public IntPtr AudioBufferPtr
		{
			get
			{
				return SafeNativeMethods.xnGetAudioBuffer(this.InternalObject);
			}
		}

		public void GetMetaData(AudioMetaData audioMD)
		{
			using (IMarshaler marsh = audioMD.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetAudioMetaData(this.InternalObject, marsh.Native);
			}
		}

		public AudioMetaData GetMetaData()
		{
			AudioMetaData audioMD = new AudioMetaData();
			GetMetaData(audioMD);
			return audioMD;
		}

		public event EventHandler WaveOutputModeChanged
		{
			add { this.outputModeChanged.Event += value; }
			remove { this.outputModeChanged.Event -= value; }
		}

		private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateAudioGenerator(context.InternalObject, out handle,
				query == null ? IntPtr.Zero : query.InternalObject,
				errors == null ? IntPtr.Zero : errors.InternalObject);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}

		private StateChangedEvent outputModeChanged;
	}
}