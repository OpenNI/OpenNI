using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class Device : ProductionNode
	{
		internal Device(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public Device(Context context, Query query) :
			this(Create(context, query), false)
		{
		}

		public Device(Context context) :
			this(context, null)
		{
		}

        private static NodeSafeHandle Create(Context context, Query query)
		{
			return context.CreateAnyProductionTreeImpl(NodeType.Device, query);
		}
	}
}