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
package org.openni.Samples.SimpleRead;

import org.openni.*;

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
