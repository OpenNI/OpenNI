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
			get
			{
				return new MirrorCapability(this);
			}
		}

		public AlternativeViewpointCapability AlternativeViewpointCapability
		{
			get
			{
				return new AlternativeViewpointCapability(this);
			}
		}

		public FrameSyncCapability FrameSyncCapability
		{
			get
			{
				return new FrameSyncCapability(this);
			}
		}

		private StateChangedEvent generationRunningChanged;
		private StateChangedEvent newDataAvailable;
	}

}