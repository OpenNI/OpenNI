using System;

namespace OpenNI
{
	interface IMapData<T>
	{
		T this[int index] { get; set; }
		T this[int x, int y] { get; set; }
		int XRes { get; }
		int YRes { get; }
	}
}
