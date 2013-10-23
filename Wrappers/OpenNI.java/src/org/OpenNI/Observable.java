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
package org.openni;

import java.util.ArrayList;

/**
 * Base class for events thrown by generators. <BR><BR>
 * 
 * This class, along with StateChangedObservable, serves as the type for
 * events.  A list of observers is maintained, and notified of new events
 * via the notify function.<BR><BR>
 *
 * @param <Args> This will be the appropriate Args type for the event in question
 */
public abstract class Observable<Args> implements IObservable<Args> 
{
	/**
	 * Default constructor, creates a new Observable with an empty observer list
	 */
	public Observable()
	{
		this.observers = new ArrayList<IObserver<Args>>();
	}
	
	/**
	 * Adds a new observer to the list of observers -- every object added 
	 * by this function will be notified of new events
	 * @param observer Class to be notified of new events
	 * @throws StatusException Underlying native code may cause an exception
	 */
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

	/**
	 * Removes an object from the observer list
	 * @param observer Object to be removed from the observer list
	 */
	@Override
	public void deleteObserver(IObserver<Args> observer) 
	{
		this.observers.remove(observer);

		if (this.observers.size() == 0)
		{
			unregisterNative(this.hCallback);
		}
	}
	
	/**
	 * Notifies all objects on the observer list that an event has occurred.  Each
	 * of these objects will be passed the appropriate data for this event type
	 * @param args The argument list, appropriate to this event type
	 */
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
