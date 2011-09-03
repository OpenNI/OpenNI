package org.OpenNI;

public class HandTouchingFOVEdgeCapability extends CapabilityBase
{
	public HandTouchingFOVEdgeCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		// Events
		handTouchingFOVEdgeEvent = new Observable<ActiveHandDirectionEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToHandTouchingFOVEdge(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromHandTouchingFOVEdge(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int id, Point3D position, float time, int direction)
			{
				notify(new ActiveHandDirectionEventArgs(id, position, time, Direction.fromNative(direction)));
			}
		};
	}

	// Events
	public IObservable<ActiveHandDirectionEventArgs> getHandTouchingFOVEdgeEvent()
	{
		return handTouchingFOVEdgeEvent;
	}
	
	private Observable<ActiveHandDirectionEventArgs> handTouchingFOVEdgeEvent;
}
