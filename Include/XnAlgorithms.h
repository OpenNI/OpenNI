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
#ifndef __XN_ALGORITHMS_H__
#define __XN_ALGORITHMS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
template<class T>
XnBool DefaultComparer(const T& arg1, const T& arg2)
{
	return arg1 < arg2;
}

class XnAlgorithms
{
public:
	template<class T, class Comparer>
	static void BubbleSort(T elements[], XnUInt32 nCount, Comparer comp)
	{
		XnUInt32 n = nCount;
		XnBool bSwapped;
		T temp;

		if (nCount == 0)
			return;

		do
		{
			bSwapped = FALSE;
			for (XnUInt32 i = 0; i < n - 1; ++i)
			{
				if (!comp(elements[i], elements[i+1]))
				{
					// swap
					temp = elements[i];
					elements[i] = elements[i+1];
					elements[i+1] = temp;

					bSwapped = TRUE;
				}
			}

			n -= 1;

		} while (bSwapped);
	}

	template<class T>
	static void BubbleSort(T elements[], XnUInt32 nCount)
	{
		BubbleSort(elements, nCount, DefaultComparer);
	}

};

#endif // __XN_ALGORITHMS_H__