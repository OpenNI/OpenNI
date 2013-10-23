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

/**
 * Interface that enables a class to maintain a list of observers. <BR><BR>
 * 
 * This interface is primarily used to allow the implementing class to generate events.
 * The list of observers is notified when an event is generated.  The template allows 
 * the Observer list to understand the number and type of data values created
 * along with the event.  
 *
 * @param <Args> The Args type (eg EventArgs) that the registered observers of this class should
 * be prepared to receive when an event is generated
 */
public interface IObservable<Args> 
{
	/**
	 * Adds a new observer to this object's list of observers
	 * @param observer The object to be added to this class's list of observers
	 * @throws StatusException
	 */
	void addObserver(IObserver<Args> observer) throws StatusException;
	/**
	 * Removes an observer from this object's list of observers
	 * @param observer The object to be removed from this class's list of observers
	 */
	void deleteObserver(IObserver<Args> observer);
}
