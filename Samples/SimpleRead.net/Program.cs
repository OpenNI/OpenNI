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
﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using OpenNI;

namespace SimpleRead.net
{
	class Program
	{
		static void Run()
		{
			string SAMPLE_XML_FILE = @"../../../../Data/SamplesConfig.xml";

			ScriptNode scriptNode;
			Context context = Context.CreateFromXmlFile(SAMPLE_XML_FILE, out scriptNode);

			DepthGenerator depth = context.FindExistingNode(NodeType.Depth) as DepthGenerator;
			if (depth == null)
			{
				Console.WriteLine("Sample must have a depth generator!");
				return;
			}

			MapOutputMode mapMode = depth.MapOutputMode;

			DepthMetaData depthMD = new DepthMetaData();

			Console.WriteLine("Press any key to stop...");

			while (!Console.KeyAvailable)
			{
				context.WaitOneUpdateAll(depth);

				depth.GetMetaData(depthMD);

				Console.WriteLine("Frame {0} Middle point is: {1}.", depthMD.FrameID, depthMD[(int)mapMode.XRes/2, (int)mapMode.YRes/2]);
			}
		}

		static void Main(string[] args)
		{
			try
			{
				Run();
			}
			catch (System.Exception ex)
			{
				Console.WriteLine("Error: {0}", ex.Message);
			}
		}
	}
}
