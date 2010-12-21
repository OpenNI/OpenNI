using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class Device : ProductionNode
	{
		internal Device(IntPtr nodeHandle) :
			base(nodeHandle)
		{
		}

		public Device(Context context, Query query) :
			this(Create(context, query))
		{
		}

		public Device(Context context) :
			this(context, null)
		{
		}

		private static IntPtr Create(Context context, Query query)
		{
			return context.CreateAnyProductionTreeImpl(NodeType.Device, query);
		}
	}
}