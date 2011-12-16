/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
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