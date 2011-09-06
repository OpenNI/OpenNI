using System;
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
