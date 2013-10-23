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