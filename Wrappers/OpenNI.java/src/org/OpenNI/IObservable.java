package org.OpenNI;

public interface IObservable<Args> 
{
	void addObserver(IObserver<Args> observer) throws StatusException;
	void deleteObserver(IObserver<Args> observer);
}
