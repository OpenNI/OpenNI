/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
package org.OpenNI;

public class ErrorStateCapability extends CapabilityBase
{
	public ErrorStateCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.errorStateChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToNodeErrorStateChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNodeErrorStateChange(toNative(), hCallback);
			}
		};
	}
	
	public String getErrorState()
	{
		int status = NativeMethods.xnGetNodeErrorState(toNative());
		if (status == 0)
		{
			return null;
		}
		else
		{
			return NativeMethods.xnGetStatusString(status);
		}
	}
	
	public IStateChangedObservable getErrorStateChangedEvent() { return this.errorStateChanged; }

	private StateChangedObservable errorStateChanged;
}
