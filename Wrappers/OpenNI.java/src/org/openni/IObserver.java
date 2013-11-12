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
 * This class enables the implementing object to be added to an observer list. <BR><BR>
 * 
 * The purpose of this interface is to enable the implementing object to be added to the 
 * Observer list of another object that implements the IObservable interface.  The most common
 * use for this is to enable events listeners to be created.  The class generating the event
 * implements IObservable, and the class wishing to listen for evens implements the IObserver
 * interface.
 *
 * @param <Args> The argument format of the event being observed
 */
public interface IObserver<Args> 
{
	/**
	 * Checks for new events
	 * @param observable Specifies the object to be observed
	 * @param args Specifies the argument format of the event to be observed
	 */
	void update(IObservable<Args> observable, Args args);
}
