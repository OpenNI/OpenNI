package org.OpenNI;


public abstract class StateChangedObservable extends Observable<EventArgs> implements IStateChangedObservable
{
	@Override
	protected int registerNative(OutArg<Long> phCallback) 
	{
		return registerNative("callback", phCallback);
	}

	protected abstract int registerNative(String cb, OutArg<Long> phCallback);

	@SuppressWarnings("unused")
	private void callback()
	{
		notify(null);
	}
}
