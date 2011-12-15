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
package org.OpenNI.Samples.SimpleRead;

import org.OpenNI.*;

public class SimpleRead implements IObserver<ErrorStateEventArgs>
{
	public static final String SAMPLES_XML = "../../../../Data/SamplesConfig.xml";
	
	public static void main(String[] args) 
	{
		try 
		{
			OutArg<ScriptNode> scriptNodeArg = new OutArg<ScriptNode>();
			Context context = Context.createFromXmlFile(SAMPLES_XML, scriptNodeArg);
			
			SimpleRead pThis = new SimpleRead();
			
			context.getErrorStateChangedEvent().addObserver(pThis);
			
			DepthGenerator depth = (DepthGenerator)context.findExistingNode(NodeType.DEPTH);
			
			DepthMetaData depthMD = new DepthMetaData();
			
			while (true)
			{
				context.waitAnyUpdateAll();
				
				depth.getMetaData(depthMD);
				
				System.out.printf("Frame %d Middle point is: %d.\n", depthMD.getFrameID(), depthMD.getData().readPixel(depthMD.getXRes() / 2, depthMD.getYRes() / 2));
			}
		} 
		catch (Throwable e) 
		{
			e.printStackTrace();
		}
	}
	
	@Override
	public void update(IObservable<ErrorStateEventArgs> arg0, ErrorStateEventArgs arg1)
	{
		System.out.printf("Global error state has changed: %s", arg1.getCurrentError());
		System.exit(1);
	}
}
