package org.OpenNI;

import java.util.ArrayList;

public abstract class Observable<Args> implements IObservable<Args> 
{
	public Observable()
	{
		this.observers = new ArrayList<IObserver<Args>>();
	}
	
	@Override
	public void addObserver(IObserver<Args> observer) throws StatusException 
	{
		if (this.observers.size() == 0)
		{
			OutArg<Long> hCallback = new OutArg<Long>();
			int status = registerNative(hCallback);
			WrapperUtils.throwOnError(status);
			this.hCallback = hCallback.value;
		}
		
		this.observers.add(observer);
	}

	@Override
	public void deleteObserver(IObserver<Args> observer) 
	{
		this.observers.remove(observer);

		if (this.observers.size() == 0)
		{
			unregisterNative(this.hCallback);
		}
	}
	
	public void notify(Args args)
	{
		for (IObserver<Args> observer : this.observers)
		{
			observer.update(this, args);
		}
	}
	
	protected abstract int registerNative(OutArg<Long> phCallback) throws StatusException;
	protected abstract void unregisterNative(long hCallback);

	private ArrayList<IObserver<Args>> observers; 
	private long hCallback;
}
