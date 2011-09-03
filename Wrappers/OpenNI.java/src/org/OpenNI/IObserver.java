package org.OpenNI;

public interface IObserver<Args> 
{
	void update(IObservable<Args> observable, Args args);
}
