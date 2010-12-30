using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using OpenNI;

namespace SimpleRead.net
{
	class Program
	{
		static void Main(string[] args)
		{
			string SAMPLE_XML_FILE = @"../../../../Data/SamplesConfig.xml";

			Context context = Context.InitFromXml(SAMPLE_XML_FILE);

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

				Console.WriteLine("Frame {0} Middle point is: {1}.", depthMD.FrameId, depthMD[(int)mapMode.XRes/2, (int)mapMode.YRes/2]);
			}
		}
	}
}
