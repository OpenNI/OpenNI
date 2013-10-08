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
	public class Generator : ProductionNode
	{
		internal Generator(Context context, IntPtr pNode, bool addRef)
			: base(context, pNode, addRef)
		{
			this.generationRunningChanged = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToGenerationRunningChange,
				SafeNativeMethods.xnUnregisterFromGenerationRunningChange);

			this.newDataAvailable = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToNewDataAvailable,
				SafeNativeMethods.xnUnregisterFromNewDataAvailable);
            if(IsCapabilitySupported(Capabilities.AlternativeViewPoint))
                m_alternativeViewpointCapability = new AlternativeViewpointCapability(this);
            else
                m_alternativeViewpointCapability = null;
            if(IsCapabilitySupported(Capabilities.FrameSync))
                m_frameSyncCapability = new FrameSyncCapability(this);
            else
                m_frameSyncCapability = null;

            if (IsCapabilitySupported(Capabilities.Mirror))
                m_mirrorCapability = new MirrorCapability(this);
            else
                m_mirrorCapability = null;
		}

		public void StartGenerating()
		{
			int status = SafeNativeMethods.xnStartGenerating(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public bool IsGenerating
		{
			get
			{
				return SafeNativeMethods.xnIsGenerating(this.InternalObject);
			}
		}

		public void StopGenerating()
		{
			int status = SafeNativeMethods.xnStopGenerating(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public event EventHandler GenerationRunningChanged
		{
			add { generationRunningChanged.Event += value; }
			remove { generationRunningChanged.Event -= value; }
		}

		public bool IsNewDataAvailable
		{
			get
			{
				UInt64 timestamp;
				return SafeNativeMethods.xnIsNewDataAvailable(this.InternalObject, out timestamp);
			}
		}

		public Int64 AvailableTimestamp
		{
			get
			{
				UInt64 timestamp;
				SafeNativeMethods.xnIsNewDataAvailable(this.InternalObject, out timestamp);
				return (Int64)timestamp;
			}
		}

		public event EventHandler NewDataAvailable
		{
			add { newDataAvailable.Event += value; }
			remove { newDataAvailable.Event -= value; }
		}

		public void WaitAndUpdateData()
		{
			int status = SafeNativeMethods.xnWaitAndUpdateData(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public bool IsDataNew
		{
			get
			{
				return SafeNativeMethods.xnIsDataNew(this.InternalObject);
			}
		}

        public IntPtr DataPtr
        {
            get
            {
                return SafeNativeMethods.xnGetData(this.InternalObject);
            }
        }

		public Int32 DataSize
		{
			get
			{
				return (Int32)SafeNativeMethods.xnGetDataSize(this.InternalObject);
			}
		}

		public Int64 Timestamp
		{
			get
			{
				return (Int64)SafeNativeMethods.xnGetTimestamp(this.InternalObject);
			}
		}

		public Int32 FrameID
		{
			get
			{
				return (Int32)SafeNativeMethods.xnGetFrameID(this.InternalObject);
			}
		}

		public MirrorCapability MirrorCapability
		{
            get { return m_mirrorCapability; }
		}

		public AlternativeViewpointCapability AlternativeViewpointCapability
		{
			get
			{
				return m_alternativeViewpointCapability;
			}
		}

		public FrameSyncCapability FrameSyncCapability
		{
			get { return m_frameSyncCapability; }
		}


        ///  @todo this is a temporary solution for capability not being disposed by anyone external
        public override void Dispose()
        {
            if (m_alternativeViewpointCapability != null)
            {
                m_alternativeViewpointCapability.InternalDispose();
                m_alternativeViewpointCapability = null;
            }
            if (m_frameSyncCapability != null)
            {
                m_frameSyncCapability.InternalDispose();
                m_frameSyncCapability = null;
            }
            if (m_mirrorCapability != null)
            {
                m_mirrorCapability.InternalDispose();
                m_mirrorCapability = null;
            }
            base.Dispose();
        }



		private StateChangedEvent generationRunningChanged;
		private StateChangedEvent newDataAvailable;
        protected AlternativeViewpointCapability m_alternativeViewpointCapability;
        protected FrameSyncCapability m_frameSyncCapability;
        protected MirrorCapability m_mirrorCapability;
	}

}