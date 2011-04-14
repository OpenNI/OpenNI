using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class Device : ProductionNode
	{
		internal Device(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public Device(Context context, Query query) :
			this(context, Create(context, query), false)
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