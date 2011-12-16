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
#ifndef __XNARRAY_H__
#define __XNARRAY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
template <typename T>
class XnArray
{
public:
	enum {BASE_SIZE = 8};

	/** An iterator object that allows you to iterate over members in the array. **/
	typedef T* Iterator;

	/** A constant iterator object that allows you to iterate over members in the array as read-only values. **/
	typedef const T* ConstIterator;

	/** Default constructor. Initializes the array to BASE_SIZE. **/
	XnArray(XnUInt32 nBaseSize = BASE_SIZE)
	{
		Init(nBaseSize);
	}

	/** Copy constructor. Initializes this array from another array of the same type. **/
	XnArray(const XnArray& other) : m_pData(NULL), m_nSize(0), m_nAllocatedSize(0)
	{
		*this = other;
	}

	/** Destructor. Deallocates data in this array. **/
	virtual ~XnArray()
	{
		XN_DELETE_ARR(m_pData);
	}

	/** Assignment operator. Copies data from another array of the same type. **/
	XnArray& operator=(const XnArray& other)
	{
		CopyFrom(other);
		return *this;
	}

	/** Copies data from another array of the same type. **/
	XnStatus CopyFrom(const XnArray& other)
	{
		if (this != &other)
		{
			XnStatus nRetVal = SetData(other.m_pData, other.m_nSize);
			XN_IS_STATUS_OK(nRetVal);
		}
		return XN_STATUS_OK;
	}

	/** Copies data from a raw buffer of the same type pointed to by pData, and sets size to nSize. **/
	XnStatus SetData(const T* pData, XnUInt32 nSize)
	{
		Clear();
		XnStatus nRetVal = SetSize(nSize);
		XN_IS_STATUS_OK(nRetVal);
		for (XnUInt32 i = 0; i < nSize; i++)
		{
			m_pData[i] = pData[i];
		}	
		return XN_STATUS_OK;
	}

	/** @returns the raw data of this array as a read-only buffer. **/
	const T* GetData() const
	{
		return m_pData;
	}

	/** @returns the raw data of this array as a modifiable buffer. **/
	T* GetData()
	{
		return m_pData;
	}

	/** Reserves space in this array for the specified number of elements. 
	    This saves you re-allocations and data copies if you know the size in advance. **/
	XnStatus Reserve(XnUInt32 nReservedSize)
	{
		if (nReservedSize > m_nAllocatedSize)
		{
			//Calculate next power of 2 after nReservedSize
			nReservedSize--;
			nReservedSize = (nReservedSize >> 1) | nReservedSize;
			nReservedSize = (nReservedSize >> 2) | nReservedSize;
			nReservedSize = (nReservedSize >> 4) | nReservedSize;
			nReservedSize = (nReservedSize >> 8) | nReservedSize;
			nReservedSize = (nReservedSize >> 16) | nReservedSize;
			nReservedSize++; // nReservedSize is now the next power of 2.

			//Allocate new data
			T* pNewData = XN_NEW_ARR(T, nReservedSize);
			XN_VALIDATE_ALLOC_PTR(pNewData);
			
			//Copy old data into new data
			for (XnUInt32 i = 0; i < m_nSize; i++)
			{
				pNewData[i] = m_pData[i];
			}

			//Delete old data
			XN_DELETE_ARR(m_pData);
			
			//Point to new data
			m_pData = pNewData;
			m_nAllocatedSize = nReservedSize;
		}
		return XN_STATUS_OK;
	}

	/** @returns TRUE if this array is empty, FALSE otherwise. **/
	XnBool IsEmpty() const
	{
		return (m_nSize == 0);
	}

	/** @returns The number of elements in this array. **/
	XnUInt32 GetSize() const
	{
		return m_nSize;
	}

	/** Sets the size of this array to the specified number of elements. 
	    Note that for primitive types, the data is uninitialized. **/
	XnStatus SetSize(XnUInt32 nSize)
	{
		//TODO: Shrink allocated array if new size is smaller
		XnStatus nRetVal = SetMinSize(nSize);
		XN_IS_STATUS_OK(nRetVal);
		m_nSize = nSize;
		return XN_STATUS_OK;
	}

	/** Sets the size of this array to the specified number of elements, initializing all elements with the 
	    value specified by fillVal. **/
	XnStatus SetSize(XnUInt32 nSize, const T& fillVal)
	{
		//TODO: Shrink allocated array if new size is smaller
		XnStatus nRetVal = SetMinSize(nSize, fillVal);
		XN_IS_STATUS_OK(nRetVal);
		m_nSize = nSize;
		return XN_STATUS_OK;
	}

	/** Makes sure the array has at least nSize elements. 
	    If nSize is less than the current size, there is no effect. 
		Note that for primitive types, any added elements are uninitialized. **/
	XnStatus SetMinSize(XnUInt32 nSize)
	{
		if (nSize > m_nSize)
		{
			XnStatus nRetVal = Reserve(nSize);
			XN_IS_STATUS_OK(nRetVal);
			m_nSize = nSize;
		}
		return XN_STATUS_OK;
	}

	/** Makes sure the array has at least nSize elements. 
	    If nSize is less than the current size, there is no effect. 
		Any added elements are initialized by the value specified by fillVal. **/
	XnStatus SetMinSize(XnUInt32 nSize, const T& fillVal)
	{
		if (nSize > m_nSize)
		{
			XnStatus nRetVal = Reserve(nSize);
			XN_IS_STATUS_OK(nRetVal);
			for (XnUInt32 i = m_nSize; i < nSize; i++)
			{
				m_pData[i] = fillVal;
			}
			m_nSize = nSize;
		}

		return XN_STATUS_OK;
	}

	/** @returns The allocated size of this array. This is the maximum number of elements that the array can hold
	    without re-allocating and copying data. **/
	XnUInt32 GetAllocatedSize() const
	{
		return m_nAllocatedSize;
	}

	/** Sets value at specified index, grows array if needed. 
	    Note that when dealing with primitive types and adding beyond the current size of the array, 
		any added values that fill the gap are uninitialized**/
	XnStatus Set(XnUInt32 nIndex, const T& val)
	{
		XnStatus nRetVal = SetMinSize(nIndex+1);
		XN_IS_STATUS_OK(nRetVal);
		m_pData[nIndex] = val;
		return XN_STATUS_OK;
	}
	
	/** Sets value at specified index and grows array if needed, filling any resulting blank elements with fillVal. **/
	XnStatus Set(XnUInt32 nIndex, const T& val, const T& fillVal)
	{
		XnStatus nRetVal = SetMinSize(nIndex+1, fillVal);
		XN_IS_STATUS_OK(nRetVal);
		m_pData[nIndex] = val;
		return XN_STATUS_OK;
	}

	/** Adds one element to the end of this array. **/
	XnStatus AddLast(const T& val)
	{
		return Set(m_nSize, val);
	}

	/** Adds a range of values to the end of this array. **/
	XnStatus AddLast(const T* aValues, XnUInt32 nCount)
	{
		XN_VALIDATE_INPUT_PTR(aValues);
		XnUInt32 nOffset = GetSize();
		XnStatus nRetVal = SetMinSize(GetSize() + nCount);
		XN_IS_STATUS_OK(nRetVal);
		for (XnUInt32 i = 0; i < nCount; ++i)
		{
			m_pData[nOffset + i] = aValues[i];
		}
		return XN_STATUS_OK;
	}

	/** Resets the data of this array and sets its size to 0. **/
	void Clear()
	{
		XN_DELETE_ARR(m_pData);
		Init();
	}

	/** Element access operator. Returns a modifiable reference to the element at specified index. **/
	T& operator[](XnUInt32 nIndex)
	{
		XN_ASSERT(nIndex < m_nSize);
		return m_pData[nIndex];
	}

	/** Element access operator. Returns a const reference to the element at specified index. **/
	const T& operator[](XnUInt32 nIndex) const
	{
		XN_ASSERT(nIndex < m_nSize);
		return m_pData[nIndex];
	}

	/** @returns a modifiable iterator pointing to the beginning of this array. **/
	Iterator begin()
	{
		return &m_pData[0];
	}

	/** @returns a const iterator pointing to the beginning of this array. **/
	ConstIterator begin() const
	{
		return &m_pData[0];
	}

	/** @returns a modifiable itertor pointing to the end of this array. **/
	Iterator end()
	{
		return m_pData + m_nSize;
	}

	/** @returns a const itertor pointing to the end of this array. **/
	ConstIterator end() const
	{
		return m_pData + m_nSize;
	}

private:
	void Init(XnUInt32 nBaseSize = BASE_SIZE)
	{
		m_pData = XN_NEW_ARR(T, nBaseSize);
		m_nAllocatedSize = nBaseSize;
		m_nSize = 0;
	}

	T* m_pData;
	XnUInt32 m_nSize;
	XnUInt32 m_nAllocatedSize;
};

#endif // __XNARRAY_H__