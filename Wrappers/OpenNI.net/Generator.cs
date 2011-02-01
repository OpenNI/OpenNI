using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public abstract class Generator : ProductionNode
	{
		internal Generator(NodeSafeHandle pNode, bool addRef)
			: base(pNode, addRef)
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
			Status.ThrowOnFail(SafeNativeMethods.xnStartGenerating(this.InternalObject));
		}

		public bool IsGenerating()
		{
			return SafeNativeMethods.xnIsGenerating(this.InternalObject);
		}

		public void StopGenerating()
		{
			Status.ThrowOnFail(SafeNativeMethods.xnStopGenerating(this.InternalObject));
		}

		public IObservable<StateChangedArgs> GenerationRunningChanged
		{
            get { return this.generationRunningChanged.StateChanged; }
		}

		public bool IsNewDataAvailable(out UInt64 timestamp)
		{
			return SafeNativeMethods.xnIsNewDataAvailable(this.InternalObject, out timestamp);
		}

		public bool IsNewDataAvailable()
		{
			UInt64 timestamp;
			return SafeNativeMethods.xnIsNewDataAvailable(this.InternalObject, out timestamp);
		}

		public IObservable<StateChangedArgs> NewDataAvailable
		{
            get { return this.newDataAvailable.StateChanged; }
		}

		public void WaitAndUpdateData()
		{
			Status.ThrowOnFail(SafeNativeMethods.xnWaitAndUpdateData(this.InternalObject));
		}

		public bool IsDataNew
		{
            get
            {
                return SafeNativeMethods.xnIsDataNew(this.InternalObject);
            }
		}

		public UInt32 DataSize
		{
            get
            {
                return SafeNativeMethods.xnGetDataSize(this.InternalObject);
            }
		}

		public UInt64 Timestamp
		{
            get
            {
                return SafeNativeMethods.xnGetTimestamp(this.InternalObject);
            }
		}

		public UInt32 FrameId
		{
            get
            {
                return SafeNativeMethods.xnGetFrameID(this.InternalObject);
            }
		}

		public MirrorCapability GetMirrorCap()
		{
            Contract.Ensures(Contract.Result<MirrorCapability>() != null);

			return new MirrorCapability(this);
		}

		public AlternativeViewpointCapability GetAlternativeViewpointCap()
		{
            Contract.Ensures(Contract.Result<AlternativeViewpointCapability>() != null);

			return new AlternativeViewpointCapability(this);
		}

		public FrameSyncCapability GetFrameSyncCap()
		{
            Contract.Ensures(Contract.Result<FrameSyncCapability>() != null);

			return new FrameSyncCapability(this);
		}

		private StateChangedEvent generationRunningChanged;
		private StateChangedEvent newDataAvailable;
	}

}