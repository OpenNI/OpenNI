using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public abstract class Generator : ProductionNode
	{
		internal Generator(NodeSafeHandle pNode, bool addRef)
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
			Status.ThrowOnFail(OpenNIImporter.xnStartGenerating(this.InternalObject));
		}

		public bool IsGenerating()
		{
			return OpenNIImporter.xnIsGenerating(this.InternalObject);
		}

		public void StopGenerating()
		{
			Status.ThrowOnFail(OpenNIImporter.xnStopGenerating(this.InternalObject));
		}

		public event EventHandler<StateChangedArgs> GenerationRunningChanged
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

		public event EventHandler<StateChangedArgs> NewDataAvailable
		{
			add { newDataAvailable.Event += value; }
			remove { newDataAvailable.Event -= value; }
		}

		public void WaitAndUpdateData()
		{
			Status.ThrowOnFail(OpenNIImporter.xnWaitAndUpdateData(this.InternalObject));
		}

		public bool IsDataNew
		{
            get
            {
                return OpenNIImporter.xnIsDataNew(this.InternalObject);
            }
		}

		public UInt32 DataSize
		{
            get
            {
                return OpenNIImporter.xnGetDataSize(this.InternalObject);
            }
		}

		public UInt64 Timestamp
		{
            get
            {
                return OpenNIImporter.xnGetTimestamp(this.InternalObject);
            }
		}

		public UInt32 FrameID
		{
            get
            {
                return OpenNIImporter.xnGetFrameID(this.InternalObject);
            }
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