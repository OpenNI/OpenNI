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
#ifndef __XNBITSET_H__
#define __XNBITSET_H__

#include <XnArray.h>

class XnBitSet
{
public:
	XnBitSet() : m_nSize(0) {}

	/** Reserves space in this bitset for the specified number of bits. 
	    This saves you re-allocations and data copies if you know the size in advance. **/
	XnStatus Reserve(XnUInt32 nBits)
	{
		return m_array.Reserve((nBits >> 5) + 1);
	}

	/** Sets the size of the bitset to the specified number of bits and sets them all to 0. **/
	XnStatus SetSize(XnUInt32 nBits)
	{
		return m_array.SetSize((nBits >> 5) + 1, 0);
	}

	/** Sets the bit at nIndex to bValue. **/
	XnStatus Set(XnUInt32 nIndex, XnBool bValue)
	{
		XnUInt32 nArrayIndex = (nIndex >> 5);
		XnUInt32 nMask = (1 << ((~nIndex) & 0x1F));
		XnUInt32 nOldVal = nArrayIndex < m_array.GetSize() ? m_array[nArrayIndex] : 0;
		XnUInt32 nNewVal = bValue ? (nOldVal | nMask) : (nOldVal & (~nMask));
		XnStatus nRetVal = m_array.Set(nArrayIndex, nNewVal, 0);
		XN_IS_STATUS_OK(nRetVal);
		m_nSize = XN_MAX(m_nSize, nIndex + 1);
		return XN_STATUS_OK;
	}
	
	/** @returns the value of the bit specified by nIndex. **/
	XnBool IsSet(XnUInt32 nIndex) const
	{
		XnUInt32 nArrayIndex = (nIndex >> 5);
		if (nArrayIndex >= m_array.GetSize())
		{
			return FALSE;
		}
		return (m_array[nArrayIndex] & (1 << ((~nIndex) & 0x1F))) ? TRUE : FALSE;
	}

	/** Copies raw data from a buffer of dwords to this bitset. **/
	XnStatus SetData(const XnUInt32* pData, XnUInt32 nSizeInDwords)
	{
		XnStatus nRetVal = m_array.SetData(pData, nSizeInDwords);
		XN_IS_STATUS_OK(nRetVal);
		m_nSize = (nSizeInDwords << 5);
		return XN_STATUS_OK;
	}

	/** Copies raw data from a buffer of bytes to this bitset. **/
	XnStatus SetDataBytes(const XnUInt8* pData, XnUInt32 nSizeInBytes)
	{
		//XnStatus nRetVal = m_array.SetData(reinterpret_cast<const XnUInt32*>(pData), XN_MAX(1, nSizeInBytes >> 2));
		XnUInt32 nSizeInDwords = XN_MAX(1, nSizeInBytes >> 2);
		XnStatus nRetVal = m_array.SetSize(nSizeInDwords);
		XN_IS_STATUS_OK(nRetVal);
		for (XnUInt32 nDwordIdx = 0, nByteIdx = 0; nDwordIdx < nSizeInDwords; nDwordIdx++, nByteIdx += 4)
		{
			m_array[nDwordIdx] = ((pData[nByteIdx] << 24) | (pData[nByteIdx + 1] << 16) | (pData[nByteIdx + 2] << 8) | pData[nByteIdx + 3] );
		}
		m_nSize = (nSizeInBytes << 3);
		return XN_STATUS_OK;
	}

	/** @returns The raw data of this bitset as a buffer of dwords. **/
	const XnUInt32* GetData() const
	{
		return m_array.GetData();
	}

	/** @returns The raw data of this bitset as a buffer of dwords. Allows modification of underlying data. **/
	XnUInt32* GetData()
	{
		return m_array.GetData();
	}

	/** @returns size in bytes of this bitset. **/
	XnUInt32 GetDataSize() const
	{
		return m_array.GetSize();
	}

	/** @returns size in bits of this bitset. **/
	XnUInt32 GetSize() const
	{
		return m_nSize;
	}

	/** Clears data in this bitset and sets size to 0. **/
	void Clear()
	{
		m_array.Clear();
		m_nSize = 0;
	}

	/** @returns TRUE if this bitset is empty, FALSE otherwise. **/
	XnBool IsEmpty() const
	{
		return m_array.IsEmpty();
	}
	
private:
	XnArray<XnUInt32> m_array;
	XnUInt32 m_nSize;
};

#endif // __XNBITSET_H__