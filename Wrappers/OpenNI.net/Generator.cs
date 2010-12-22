using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class Generator : ProductionNode
	{
		internal Generator(IntPtr pNode, bool addRef)
			: base(pNode, addRef)
		{
			this.generationRunningChanged = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToGenerationRunningChange,
				OpenNIImporter.xnUnregisterFromGenerationRunningChange);

			this.newDataAvailable = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToNewDataAvailable,
				OpenNIImporter.xnUnregisterFromNewDataAvailable);
		}

		public void StartGenerating()
		{
			UInt32 status = OpenNIImporter.xnStartGenerating(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public bool IsGenerating()
		{
			return OpenNIImporter.xnIsGenerating(this.InternalObject);
		}

		public void StopGenerating()
		{
			UInt32 status = OpenNIImporter.xnStopGenerating(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public event StateChangedHandler GenerationRunningChanged
		{
			add { generationRunningChanged.Event += value; }
			remove { generationRunningChanged.Event -= value; }
		}

		public bool IsNewDataAvailable(out UInt64 timestamp)
		{
			return OpenNIImporter.xnIsNewDataAvailable(this.InternalObject, out timestamp);
		}

		public bool IsNewDataAvailable()
		{
			UInt64 timestamp;
			return OpenNIImporter.xnIsNewDataAvailable(this.InternalObject, out timestamp);
		}

		public event StateChangedHandler NewDataAvailable
		{
			add { newDataAvailable.Event += value; }
			remove { newDataAvailable.Event -= value; }
		}

		public void WaitAndUpdateData()
		{
			UInt32 status = OpenNIImporter.xnWaitAndUpdateData(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public bool IsDataNew()
		{
			return OpenNIImporter.xnIsDataNew(this.InternalObject);
		}

		public UInt32 GetDataSize()
		{
			return OpenNIImporter.xnGetDataSize(this.InternalObject);
		}

		public UInt64 GetTimestamp()
		{
			return OpenNIImporter.xnGetTimestamp(this.InternalObject);
		}

		public UInt32 GetFrameID()
		{
			return OpenNIImporter.xnGetFrameID(this.InternalObject);
		}

		public MirrorCapability GetMirrorCap()
		{
			return new MirrorCapability(this);
		}

		public AlternativeViewPointCapability GetAlternativeViewPointCap()
		{
			return new AlternativeViewPointCapability(this);
		}

		public FrameSyncCapability GetFrameSyncCap()
		{
			return new FrameSyncCapability(this);
		}

		private StateChangedEvent generationRunningChanged;
		private StateChangedEvent newDataAvailable;
	}

}