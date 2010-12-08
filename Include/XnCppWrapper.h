/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




#ifndef __XN_CPP_WRAPPER_H__
#define __XN_CPP_WRAPPER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
namespace xn
{
	//---------------------------------------------------------------------------
	// Forward Declarations
	//---------------------------------------------------------------------------
	class ProductionNode;
	class EnumerationErrors;
	class NodeInfo;
	class NodeInfoList;
	class Context;
	class Query;
	class Generator;

	/**
	 * @ingroup cppref
	 * @{
	 */

	//---------------------------------------------------------------------------
	// Types
	//---------------------------------------------------------------------------

	/**
	 * A handler for configuration changes event.
	 *
	 * @param	node	[in]	The node issuing the event.
	 * @param	pCookie	[in]	The user cookie passed on registration.
	 */
	typedef void (XN_CALLBACK_TYPE* StateChangedHandler)(ProductionNode& node, void* pCookie);

	//---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------
	typedef XnStatus (*_XnRegisterStateChangeFuncPtr)(XnNodeHandle hNode, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
	typedef void (*_XnUnregisterStateChangeFuncPtr)(XnNodeHandle hNode, XnCallbackHandle hCallback);

	static XnStatus _RegisterToStateChange(_XnRegisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode, StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	static void _UnregisterFromStateChange(_XnUnregisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode, XnCallbackHandle hCallback);

	//---------------------------------------------------------------------------
	// Meta Data
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref
	 * Represents the basic MetaData object.
	 */
	class OutputMetaData
	{
	public:
		/**
		 * Ctor. Meant to be used by other MetaData objects.
		 *
		 * @param	ppData		[in]	A pointer to the data member of the meta data object.
		 */
		inline OutputMetaData(const XnUInt8** ppData) : m_ppData(ppData), m_nAllocatedSize(0), m_pAllocatedData(NULL)
		{
			xnOSMemSet(&m_output, 0, sizeof(XnOutputMetaData));
		}

		/**
		 * Dtor. Frees any allocated data.
		 */
		virtual ~OutputMetaData() { Free(); }

		/** Gets the timestamp */
		inline XnUInt64 Timestamp() const { return m_output.nTimestamp; }
		/** Gets the timestamp */
		inline XnUInt64& Timestamp() { return m_output.nTimestamp; }

		/** Gets the frame ID */
		inline XnUInt32 FrameID() const { return m_output.nFrameID; }
		/** Gets the frame ID */
		inline XnUInt32& FrameID() { return m_output.nFrameID; }

		/** Gets the data size, in bytes */
		inline XnUInt32 DataSize() const { return m_output.nDataSize; }
		/** Gets the data size, in bytes */
		inline XnUInt32& DataSize() { return m_output.nDataSize; }

		/** TRUE if data was updated since last call to any <i>Update</i> function. */
		inline XnBool IsDataNew() const { return m_output.bIsNew; }
		/** TRUE if data was updated since last call to any <i>Update</i> function. */
		inline XnBool& IsDataNew() { return m_output.bIsNew; }

		/** Gets the C object that is wrapped by this object. */
		inline const XnOutputMetaData* GetUnderlying() const { return &m_output; }
		/** Gets the C object that is wrapped by this object. */
		inline XnOutputMetaData* GetUnderlying() { return &m_output; }

		/** Gets a pointer to the data. */
		inline const XnUInt8* Data() const { return *m_ppData; }
		/** Gets a pointer to the data. */
		inline const XnUInt8*& Data() { return *m_ppData; }
		/** Gets a pointer to the writable buffer (see @ref conc_meta_data). */
		inline XnUInt8* WritableData()
		{
			MakeDataWritable();
			return m_pAllocatedData;
		}

		/**
		 * @brief Allocated a writable buffer. If a previous buffer was allocated it will be freed (or reused if possible).
		 * Also see @ref conc_meta_data.
		 *
		 * @param	nBytes		[in]	The requested size for the buffer, in bytes.
		 */
		XnStatus AllocateData(XnUInt32 nBytes)
		{
			if (nBytes > m_nAllocatedSize)
			{
				// reallocate
				XnUInt8* pData = (XnUInt8*)xnOSMallocAligned(nBytes, XN_DEFAULT_MEM_ALIGN);
				XN_VALIDATE_ALLOC_PTR(pData);

				// allocation succeeded, replace
				Free();
				m_pAllocatedData = pData;
				m_nAllocatedSize = nBytes;
			}

			DataSize() = nBytes;
			*m_ppData = m_pAllocatedData;

			return XN_STATUS_OK;
		}

		/**
		 * Frees the writable buffer, if one was allocated.
		 */
		void Free()
		{
			if (m_nAllocatedSize != 0)
			{
				xnOSFreeAligned(m_pAllocatedData);
				m_pAllocatedData = NULL;
				m_nAllocatedSize = 0;
			}
		}

		/**
		 * Allocates a buffer and copies current data to it, so that data can be edited.
		 * Also see @ref conc_meta_data.
		 */
		XnStatus MakeDataWritable()
		{
			XnStatus nRetVal = XN_STATUS_OK;

			// check data isn't already writable
			if (Data() != m_pAllocatedData || DataSize() > m_nAllocatedSize)
			{
				const XnUInt8* pOrigData = *m_ppData;

				nRetVal = AllocateData(DataSize());
				XN_IS_STATUS_OK(nRetVal);

				if (pOrigData != NULL)
				{
					xnOSMemCopy(m_pAllocatedData, pOrigData, DataSize());
				}
				else
				{
					xnOSMemSet(m_pAllocatedData, 0, DataSize());
				}
			}

			return (XN_STATUS_OK);
		}

	protected:
		XnUInt8* m_pAllocatedData;

	private:
		XnOutputMetaData m_output;

		const XnUInt8** m_ppData;
		XnUInt32 m_nAllocatedSize;
	};

	/**
	 * @ingroup cppref
	 * Represents a MetaData object for generators producing pixel-map-based outputs
	 */
	class MapMetaData : public OutputMetaData
	{
	public:
		/**
		 * Ctor. Meant to be used by inheriting classes.
		 *
		 * @param	format		[in]	Pixel Format
		 * @param	ppData		[in]	A pointer to the data member of the meta data object
		 */
		inline MapMetaData(XnPixelFormat format, const XnUInt8** ppData) : OutputMetaData(ppData)
		{
			xnOSMemSet(&m_map, 0, sizeof(XnMapMetaData));
			m_map.pOutput = OutputMetaData::GetUnderlying();
			m_map.PixelFormat = format;
		}

		/** Gets the actual number of columns in the frame (after cropping) */
		inline XnUInt32 XRes() const { return m_map.Res.X; }
		/** Gets the actual number of columns in the frame (after cropping) */
		inline XnUInt32& XRes() { return m_map.Res.X; }

		/** Gets the actual number of rows in the frame (after cropping) */
		inline XnUInt32 YRes() const { return m_map.Res.Y; }
		/** Gets the actual number of rows in the frame (after cropping) */
		inline XnUInt32& YRes() { return m_map.Res.Y; }

		/** Gets the offset, in columns, of the buffer within the field of view (0 if cropping is off). */
		inline XnUInt32 XOffset() const { return m_map.Offset.X; }
		/** Gets the offset, in columns, of the buffer within the field of view (0 if cropping is off). */
		inline XnUInt32& XOffset() { return m_map.Offset.X; }

		/** Gets the offset, in rows, of the buffer within the field of view (0 if cropping is off). */
		inline XnUInt32 YOffset() const { return m_map.Offset.Y; }
		/** Gets the offset, in rows, of the buffer within the field of view (0 if cropping is off). */
		inline XnUInt32& YOffset() { return m_map.Offset.Y; }

		/** Gets the number of columns in the full frame (entire field-of-view, ignoring cropping). */
		inline XnUInt32 FullXRes() const { return m_map.FullRes.X; }
		/** Gets the number of columns in the full frame (entire field-of-view, ignoring cropping). */
		inline XnUInt32& FullXRes() { return m_map.FullRes.X; }

		/** Gets the number of rows in the full frame (entire field-of-view, ignoring cropping). */
		inline XnUInt32 FullYRes() const { return m_map.FullRes.Y; }
		/** Gets the number of rows in the full frame (entire field-of-view, ignoring cropping). */
		inline XnUInt32& FullYRes() { return m_map.FullRes.Y; }

		/** Gets the FPS in which frame was generated. */
		inline XnUInt32 FPS() const { return m_map.nFPS; }
		/** Gets the FPS in which frame was generated. */
		inline XnUInt32& FPS() { return m_map.nFPS; }

		/** Gets the pixel format of the pixel-map. */
		inline XnPixelFormat PixelFormat() const { return m_map.PixelFormat; }

		/** Gets the C object that is wrapped by this object. */
		inline const XnMapMetaData* GetUnderlying() const { return &m_map; }
		/** Gets the C object that is wrapped by this object. */
		inline XnMapMetaData* GetUnderlying() { return &m_map; }

		/** Gets the number of bytes each pixel occupies. */
		inline XnUInt32 BytesPerPixel() const
		{
			switch (PixelFormat())
			{
				case XN_PIXEL_FORMAT_RGB24:
					return sizeof(XnRGB24Pixel);
				case XN_PIXEL_FORMAT_YUV422:
					return sizeof(XnYUV422DoublePixel)/2;
				case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
					return sizeof(XnGrayscale8Pixel);
				case XN_PIXEL_FORMAT_GRAYSCALE_16_BIT:
					return sizeof(XnGrayscale16Pixel);
				default:
					XN_ASSERT(FALSE);
					return 0;
			}
		}

		/**
		 * @copybrief xn::OutputMetaData::AllocateData
		 *
		 * @param	nXRes		[in]	The number of required columns in the map.
		 * @param	nYRes		[in]	The number of required rows in the map.
		 */
		XnStatus AllocateData(XnUInt32 nXRes, XnUInt32 nYRes)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			XnUInt32 nSize = nXRes * nYRes * BytesPerPixel();
			nRetVal = OutputMetaData::AllocateData(nSize);
			XN_IS_STATUS_OK(nRetVal);

			FullXRes() = XRes() = nXRes;
			FullYRes() = YRes() = nYRes;
			XOffset() = YOffset() = 0;
			
			return (XN_STATUS_OK);
		}

		/**
		 * Changes dimensions of the pixel-map.
		 *
		 * @param	nXRes			[in]	Number of columns in the map
		 * @param	nYRes			[in]	Number of rows in the map
		 * @param	pExternalBuffer	[in]	Optional. An external buffer to be used. If NULL is passed,
		 *									a buffer will be allocated.
		 */
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, const XnUInt8* pExternalBuffer)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			if (pExternalBuffer == NULL)
			{
				nRetVal = AllocateData(nXRes, nYRes);
				XN_IS_STATUS_OK(nRetVal);
			}
			else
			{
				FullXRes() = XRes() = nXRes;
				FullYRes() = YRes() = nYRes;
				XOffset() = YOffset() = 0;
				Data() = pExternalBuffer;
				DataSize() = nXRes * nYRes * BytesPerPixel();
			}

			return (XN_STATUS_OK);
		}

	protected:
		XnPixelFormat& PixelFormatImpl() { return m_map.PixelFormat; }

	private:
		// block copy ctor and assignment operator
		MapMetaData& operator=(const MapMetaData&);
		inline MapMetaData(const MapMetaData& other);

		// Members
		XnMapMetaData m_map;
	};

/* Declares a map data accessor class */
#define _XN_DECLARE_MAP_DATA_CLASS(_name, _pixelType)							\
	class _name																	\
	{																			\
	public:																		\
		inline _name(_pixelType*& pData, XnUInt32& nXRes, XnUInt32 &nYRes) :	\
		m_pData(pData), m_nXRes(nXRes), m_nYRes(nYRes) {}						\
																				\
		inline XnUInt32 XRes() const { return m_nXRes; }						\
		inline XnUInt32 YRes() const { return m_nYRes; }						\
																				\
		inline const _pixelType& operator[](XnUInt32 nIndex) const				\
		{																		\
			XN_ASSERT(nIndex < (m_nXRes * m_nYRes));							\
			return m_pData[nIndex];												\
		}																		\
		inline _pixelType& operator[](XnUInt32 nIndex)							\
		{																		\
			XN_ASSERT(nIndex < (m_nXRes *m_nYRes));								\
			return m_pData[nIndex];												\
		}																		\
																				\
		inline const _pixelType& operator()(XnUInt32 x, XnUInt32 y) const		\
		{																		\
			XN_ASSERT(x < m_nXRes && y < m_nYRes);								\
			return m_pData[y*m_nXRes + x];										\
		}																		\
		inline _pixelType& operator()(XnUInt32 x, XnUInt32 y)					\
		{																		\
			XN_ASSERT(x < m_nXRes && y < m_nYRes);								\
			return m_pData[y*m_nXRes + x];										\
		}																		\
																				\
	private:																	\
		/* block copy ctor and assignment operator */							\
		_name(const _name& other);												\
		_name& operator=(const _name&);											\
																				\
		_pixelType*& m_pData;													\
		XnUInt32& m_nXRes;														\
		XnUInt32& m_nYRes;														\
	};																		

	_XN_DECLARE_MAP_DATA_CLASS(DepthMap, XnDepthPixel);
	_XN_DECLARE_MAP_DATA_CLASS(ImageMap, XnUInt8);
	_XN_DECLARE_MAP_DATA_CLASS(RGB24Map, XnRGB24Pixel);
	_XN_DECLARE_MAP_DATA_CLASS(Grayscale16Map, XnGrayscale16Pixel);
	_XN_DECLARE_MAP_DATA_CLASS(Grayscale8Map, XnGrayscale8Pixel);
	_XN_DECLARE_MAP_DATA_CLASS(IRMap, XnIRPixel);
	_XN_DECLARE_MAP_DATA_CLASS(LabelMap, XnLabel);

	/**
	 * @ingroup cppref
	 * Represents a MetaData object for generators producing depth-maps.
	 */
	class DepthMetaData : public MapMetaData
	{
	public:
		/**
		 * Ctor.
		 */
		inline DepthMetaData() : 
			MapMetaData(XN_PIXEL_FORMAT_GRAYSCALE_16_BIT, (const XnUInt8**)&m_depth.pData),
			m_depthMap(const_cast<XnDepthPixel*&>(m_depth.pData), MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableDepthMap((XnDepthPixel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y)
		{
			xnOSMemSet(&m_depth, 0, sizeof(XnDepthMetaData));
			m_depth.pMap = MapMetaData::GetUnderlying();
		}

		/**
		 * @copybrief xnCopyDepthMetaData
		 *
		 * @param	other		[in]	source object.
		 */
		inline void InitFrom(const DepthMetaData& other)
		{
			xnCopyDepthMetaData(&m_depth, &other.m_depth);
		}

		/**
		 * Shallow copies another Depth Meta Data object, and then @ref ReAdjust it.
		 *
		 * @param	other			[in]	source object
		 * @param	nXRes			[in]	requested number of columns in the map
		 * @param	nYRes			[in]	requested number of rows in the map
		 * @param	pExternalBuffer	[in]	Optional. An external buffer matching requested resolution. If NULL,
		 *									A buffer will be allocated.
		 */
		inline XnStatus InitFrom(const DepthMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnDepthPixel* pExternalBuffer)
		{
			InitFrom(other);
			return ReAdjust(nXRes, nYRes, pExternalBuffer);
		}

		/**
		 * Performs a deep-copy of another Meta Data object (including a duplication of the data buffer)
		 *
		 * @param	other		[in]	source object
		 */
		XnStatus CopyFrom(const DepthMetaData& other)
		{
			// copy props
			InitFrom(other);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/** @copydoc xn::MapMetaData::ReAdjust */
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, const XnDepthPixel* pExternalBuffer = NULL)
		{
			return MapMetaData::ReAdjust(nXRes, nYRes, (const XnUInt8*)pExternalBuffer);
		}

		/** Gets the resolution of the depth axis (i.e. @ref DepthGenerator::GetDeviceMaxDepth() + 1) */
		inline XnDepthPixel ZRes() const { return m_depth.nZRes; }
		/** Gets the resolution of the depth axis (i.e. @ref DepthGenerator::GetDeviceMaxDepth() + 1) */
		inline XnDepthPixel& ZRes() { return m_depth.nZRes; }

		/// @copydoc xn::OutputMetaData::Data
		inline const XnDepthPixel* Data() const { return (const XnDepthPixel*)MapMetaData::Data(); }
		/// @copydoc xn::OutputMetaData::Data
		inline const XnDepthPixel*& Data() { return (const XnDepthPixel*&)MapMetaData::Data(); }
		/// @copydoc xn::OutputMetaData::WritableData
		inline XnDepthPixel* WritableData() { return (XnDepthPixel*)MapMetaData::WritableData(); }

		/// Gets a light object wrapping the depth-map
		inline const xn::DepthMap& DepthMap() const { return m_depthMap; }
		/// Gets a light object wrapping the writable depth-map
		inline xn::DepthMap& WritableDepthMap() 
		{ 
			MakeDataWritable();
			return m_writableDepthMap; 
		}

		/**
		 * Gets the value of the pixel
		 *
		 * @param	nIndex		[in]	The index of the pixel in the buffer.
		 */
		inline const XnDepthPixel& operator[](XnUInt32 nIndex) const 
		{ 
			XN_ASSERT(nIndex < (XRes()*YRes()));
			return Data()[nIndex]; 
		}

		/**
		 * Gets the value of the pixel
		 *
		 * @param	x		[in]	X-coordinate of the pixel in the map
		 * @param	y		[in]	Y-coordinate of the pixel in the map
		 */
		inline const XnDepthPixel& operator()(XnUInt32 x, XnUInt32 y) const 
		{
			XN_ASSERT(x < XRes() && y < YRes());
			return Data()[y*XRes() + x]; 
		}

		/// @copydoc xn::OutputMetaData::GetUnderlying
		inline const XnDepthMetaData* GetUnderlying() const { return &m_depth; }
		/// @copydoc xn::OutputMetaData::GetUnderlying
		inline XnDepthMetaData* GetUnderlying() { return &m_depth; }

	private:
		// block copy ctor and assignment operator (because we can't return errors in those)
		DepthMetaData(const DepthMetaData& other);
		DepthMetaData& operator=(const DepthMetaData&);

		XnDepthMetaData m_depth;
		const xn::DepthMap m_depthMap;
		xn::DepthMap m_writableDepthMap;
	};

	/**
	 * @ingroup cppref
	 * Represents a MetaData object for generators producing image-maps.
	 */
	class ImageMetaData : public MapMetaData
	{
	public:
		/// Ctor
		inline ImageMetaData() : 
			MapMetaData(XN_PIXEL_FORMAT_RGB24, &m_image.pData),
			m_imageMap(const_cast<XnUInt8*&>(m_image.pData), MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableImageMap((XnUInt8*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_rgb24Map((XnRGB24Pixel*&)m_image.pData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableRgb24Map((XnRGB24Pixel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_gray16Map((XnGrayscale16Pixel*&)m_image.pData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableGray16Map((XnGrayscale16Pixel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_gray8Map((XnGrayscale8Pixel*&)m_image.pData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableGray8Map((XnGrayscale8Pixel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y)
		{
			xnOSMemSet(&m_image, 0, sizeof(XnImageMetaData));
			m_image.pMap = MapMetaData::GetUnderlying();
		}

		/**
		 * @copybrief xnCopyImageMetaData
		 *
		 * @param	other		[in]	source object.
		 */
		inline void InitFrom(const ImageMetaData& other)
		{
			xnCopyImageMetaData(&m_image, &other.m_image);
		}

		/**
		 * Shallow copies another Image Meta Data object, and then @ref ReAdjust it.
		 *
		 * @param	other			[in]	source object
		 * @param	nXRes			[in]	requested number of columns in the map
		 * @param	nYRes			[in]	requested number of rows in the map
		 * @param	format			[in]	requested pixel format
		 * @param	pExternalBuffer	[in]	Optional. An external buffer matching requested resolution. If NULL,
		 *									A buffer will be allocated.
		 */
		inline XnStatus InitFrom(const ImageMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, XnPixelFormat format, const XnUInt8* pExternalBuffer)
		{
			InitFrom(other);
			XnStatus nRetVal = ReAdjust(nXRes, nYRes, format, pExternalBuffer);
			XN_IS_STATUS_OK(nRetVal);
			PixelFormat() = format;
			return XN_STATUS_OK;
		}

		/**
		* @copybrief xn::OutputMetaData::AllocateData
		 *
		 * @param	nXRes		[in]	Requested number of columns in the map
		 * @param	nYRes		[in]	Requested number of rows in the map
		 * @param	format		[in]	Requested pixel format
		 */
		inline XnStatus AllocateData(XnUInt32 nXRes, XnUInt32 nYRes, XnPixelFormat format)
		{
			XnPixelFormat origFormat = PixelFormat();
			PixelFormat() = format;
			XnStatus nRetVal = MapMetaData::AllocateData(nXRes, nYRes);
			if (nRetVal != XN_STATUS_OK)
			{
				PixelFormat() = origFormat;
				return (nRetVal);
			}

			return XN_STATUS_OK;
		}

		/// @copydoc xn::DepthMetaData::CopyFrom
		inline XnStatus CopyFrom(const ImageMetaData& other)
		{
			// copy props
			xnCopyImageMetaData(&m_image, &other.m_image);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/**
		 * @copybrief xn::DepthMetaData::ReAdjust
		 *
		 * @param	nXRes			[in]	Requested number of columns in the map
		 * @param	nYRes			[in]	Requested number of rows in the map
		 * @param	format			[in]	Requested pixel format
		 * @param	pExternalBuffer	[in]	Optional. An external buffer to be used. If NULL is passed,
		 *									a buffer will be allocated.
		 */
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, XnPixelFormat format, const XnUInt8* pExternalBuffer = NULL)
		{
			XnPixelFormat origFormat = PixelFormat();
			PixelFormat() = format;
			XnStatus nRetVal = MapMetaData::ReAdjust(nXRes, nYRes, pExternalBuffer);
			if (nRetVal != XN_STATUS_OK)
			{
				PixelFormat() = origFormat;
				return (nRetVal);
			}

			return XN_STATUS_OK;
		}

		/// Gets the pixel format
		inline XnPixelFormat PixelFormat() const { return MapMetaData::PixelFormat(); }
		/// Gets the pixel format
		inline XnPixelFormat& PixelFormat() { return MapMetaData::PixelFormatImpl(); }

		/// @copydoc MapMetaData::WritableData
		inline XnUInt8* WritableData() { return MapMetaData::WritableData(); }

		/// @copydoc OutputMetaData::Data
		inline const XnRGB24Pixel* RGB24Data() const { return (const XnRGB24Pixel*)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::Data
		inline const XnRGB24Pixel*& RGB24Data() { return (const XnRGB24Pixel*&)MapMetaData::Data(); }
		/// @copydoc MapMetaData::WritableData
		inline XnRGB24Pixel* WritableRGB24Data() { return (XnRGB24Pixel*)MapMetaData::WritableData(); }

		/// @copydoc OutputMetaData::Data
		inline const XnYUV422DoublePixel* YUV422Data() const { return (const XnYUV422DoublePixel*)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::Data
		inline const XnYUV422DoublePixel*& YUV422Data() { return (const XnYUV422DoublePixel*&)MapMetaData::Data(); }
		/// @copydoc MapMetaData::WritableData
		inline XnYUV422DoublePixel* WritableYUV422Data() { return (XnYUV422DoublePixel*)MapMetaData::WritableData(); }

		/// @copydoc OutputMetaData::Data
		inline const XnGrayscale8Pixel* Grayscale8Data() const { return (const XnGrayscale8Pixel*)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::Data
		inline const XnGrayscale8Pixel*& Grayscale8Data() { return (const XnGrayscale8Pixel*&)MapMetaData::Data(); }
		/// @copydoc MapMetaData::WritableData
		inline XnGrayscale8Pixel* WritableGrayscale8Data() { return (XnGrayscale8Pixel*)MapMetaData::WritableData(); }

		/// @copydoc OutputMetaData::Data
		inline const XnGrayscale16Pixel* Grayscale16Data() const { return (const XnGrayscale16Pixel*)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::Data
		inline const XnGrayscale16Pixel*& Grayscale16Data() { return (const XnGrayscale16Pixel*&)MapMetaData::Data(); }
		/// @copydoc MapMetaData::WritableData
		inline XnGrayscale16Pixel* WritableGrayscale16Data() { return (XnGrayscale16Pixel*)MapMetaData::WritableData(); }

		/// Gets a light object wrapping the image-map
		inline const xn::ImageMap& ImageMap() const { return m_imageMap; }
		/// Gets a light object wrapping the writable image-map
		inline xn::ImageMap& WritableImageMap() { MakeDataWritable(); return m_writableImageMap; }

		/// @copydoc xn::ImageMetaData::ImageMap
		inline const xn::RGB24Map& RGB24Map() const { return m_rgb24Map; }
		/// @copydoc xn::ImageMetaData::WritableImageMap
		inline xn::RGB24Map& WritableRGB24Map() { MakeDataWritable(); return m_writableRgb24Map; }

		/// @copydoc xn::ImageMetaData::ImageMap
		inline const xn::Grayscale8Map& Grayscale8Map() const { return m_gray8Map; }
		/// @copydoc xn::ImageMetaData::WritableImageMap
		inline xn::Grayscale8Map& WritableGrayscale8Map() { MakeDataWritable(); return m_writableGray8Map; }

		/// @copydoc xn::ImageMetaData::ImageMap
		inline const xn::Grayscale16Map& Grayscale16Map() const { return m_gray16Map; }
		/// @copydoc xn::ImageMetaData::WritableImageMap
		inline xn::Grayscale16Map& WritableGrayscale16Map() { MakeDataWritable(); return m_writableGray16Map; }

		/// @copydoc xn::OutputMetaData::GetUnderlying
		inline const XnImageMetaData* GetUnderlying() const { return &m_image; }
		/// @copydoc xn::OutputMetaData::GetUnderlying
		inline XnImageMetaData* GetUnderlying() { return &m_image; }

	private:
		// block copy ctor and assignment operator
		ImageMetaData(const ImageMetaData& other);
		ImageMetaData& operator=(const ImageMetaData&);

		XnImageMetaData m_image;
		const xn::ImageMap m_imageMap;
		xn::ImageMap m_writableImageMap;
		const xn::RGB24Map m_rgb24Map;
		xn::RGB24Map m_writableRgb24Map;
		const xn::Grayscale16Map m_gray16Map;
		xn::Grayscale16Map m_writableGray16Map;
		const xn::Grayscale8Map m_gray8Map;
		xn::Grayscale8Map m_writableGray8Map;
	};

	/**
	 * @ingroup cppref
	 * Represents a MetaData object for generators producing infra-red-maps.
	 */
	class IRMetaData : public MapMetaData
	{
	public:
		/// Ctor
		inline IRMetaData() : 
			MapMetaData(XN_PIXEL_FORMAT_GRAYSCALE_16_BIT, (const XnUInt8**)&m_ir.pData),
			m_irMap(const_cast<XnIRPixel*&>(m_ir.pData), MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableIRMap((XnIRPixel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y)
		{
			xnOSMemSet(&m_ir, 0, sizeof(XnIRMetaData));
			m_ir.pMap = MapMetaData::GetUnderlying();
		}

		/**
		 * @copybrief xnCopyIRMetaData
		 *
		 * @param	other		[in]	source object.
		 */
		inline void InitFrom(const IRMetaData& other)
		{
			xnCopyIRMetaData(&m_ir, &other.m_ir);
		}

		/// @copydoc DepthMetaData::InitFrom(const DepthMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnDepthPixel* pExternalBuffer)
		inline XnStatus InitFrom(const IRMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnIRPixel* pExternalBuffer)
		{
			InitFrom(other);
			return ReAdjust(nXRes, nYRes, pExternalBuffer);
		}

		/// @copydoc DepthMetaData::CopyFrom
		XnStatus CopyFrom(const IRMetaData& other)
		{
			// copy props
			xnCopyIRMetaData(&m_ir, &other.m_ir);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/// @copydoc DepthMetaData::ReAdjust
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, const XnIRPixel* pExternalBuffer = NULL)
		{
			return MapMetaData::ReAdjust(nXRes, nYRes, (const XnUInt8*)pExternalBuffer);
		}

		/// @copydoc OutputMetaData::Data
		inline const XnIRPixel* Data() const { return (const XnIRPixel*)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::Data
		inline const XnIRPixel*& Data() { return (const XnIRPixel*&)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::WritableData
		inline XnIRPixel* WritableData() { return (XnIRPixel*)MapMetaData::WritableData(); }

		/// Gets a light object wrapping the IR map
		inline const xn::IRMap& IRMap() const { return m_irMap; }
		/// Gets a light object wrapping the writable IR map
		inline xn::IRMap& WritableIRMap() { MakeDataWritable(); return m_writableIRMap; }

		/// @copydoc OutputMetaData::GetUnderlying
		inline const XnIRMetaData* GetUnderlying() const { return &m_ir; }
		/// @copydoc OutputMetaData::GetUnderlying
		inline XnIRMetaData* GetUnderlying() { return &m_ir; }

	private:
		// block copy ctor and assignment operator
		IRMetaData(const IRMetaData& other);
		IRMetaData& operator=(const IRMetaData&);

		XnIRMetaData m_ir;
		const xn::IRMap m_irMap;
		xn::IRMap m_writableIRMap;
	};

	/**
	 * @ingroup cppref
	 * Represents a MetaData object for generators producing audio data.
	 */
	class AudioMetaData : public OutputMetaData
	{
	public:
		/// Ctor
		inline AudioMetaData() : OutputMetaData(&m_audio.pData)
		{
			xnOSMemSet(&m_audio, 0, sizeof(XnAudioMetaData));
			m_audio.pOutput = OutputMetaData::GetUnderlying();
		}

		/**
		 * @copybrief xnCopyAudioMetaData
		 *
		 * @param	other		[in]	source object.
		 */
		inline void InitFrom(const AudioMetaData& other)
		{
			xnCopyAudioMetaData(&m_audio, &other.m_audio);
		}

		/// Gets the number of channels in every sample
		inline XnUInt8 NumberOfChannels() const { return m_audio.Wave.nChannels; }
		/// Gets the number of channels in every sample
		inline XnUInt8& NumberOfChannels() { return m_audio.Wave.nChannels; }

		/// Gets the rate in which audio is sampled
		inline XnUInt32 SampleRate() const { return m_audio.Wave.nSampleRate; }
		/// Gets the rate in which audio is sampled
		inline XnUInt32& SampleRate() { return m_audio.Wave.nSampleRate; }

		/// Gets the number of bits per channel in a sample
		inline XnUInt16 BitsPerSample() const { return m_audio.Wave.nBitsPerSample; }
		/// Gets the number of bits per channel in a sample
		inline XnUInt16& BitsPerSample() { return m_audio.Wave.nBitsPerSample; }

		/// @copydoc OutputMetaData::GetUnderlying
		inline const XnAudioMetaData* GetUnderlying() const { return &m_audio; }
		/// @copydoc OutputMetaData::GetUnderlying
		inline XnAudioMetaData* GetUnderlying() { return &m_audio; }

	private:
		// block copy ctor and assignment operator
		AudioMetaData(const AudioMetaData& other);
		AudioMetaData& operator=(const AudioMetaData&);

		XnAudioMetaData m_audio;
		XnBool m_bAllocated;
	};

	/**
	 * @ingroup cppref
	 * Represents a MetaData object for generators producing scene analysis maps
	 */
	class SceneMetaData : public MapMetaData
	{
	public:
		/// Ctor
		inline SceneMetaData() : 
			MapMetaData(XN_PIXEL_FORMAT_GRAYSCALE_16_BIT, (const XnUInt8**)&m_scene.pData),
			m_labelMap(const_cast<XnLabel*&>(m_scene.pData), MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableLabelMap((XnLabel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y)
		{
			xnOSMemSet(&m_scene, 0, sizeof(XnSceneMetaData));
			m_scene.pMap = MapMetaData::GetUnderlying();
		}

		/**
		 * @copybrief xnCopySceneMetaData
		 *
		 * @param	other		[in]	source object.
		 */
		inline void InitFrom(const SceneMetaData& other)
		{
			xnCopySceneMetaData(&m_scene, &other.m_scene);
		}

		/// @copydoc DepthMetaData::InitFrom(const DepthMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnDepthPixel* pExternalBuffer)
		inline XnStatus InitFrom(const SceneMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnLabel* pExternalBuffer)
		{
			InitFrom(other);
			return ReAdjust(nXRes, nYRes, pExternalBuffer);
		}

		/// @copydoc DepthMetaData::CopyFrom
		XnStatus CopyFrom(const SceneMetaData& other)
		{
			// copy props
			xnCopySceneMetaData(&m_scene, &other.m_scene);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/// @copydoc DepthMetaData::ReAdjust
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, const XnLabel* pExternalBuffer = NULL)
		{
			return MapMetaData::ReAdjust(nXRes, nYRes, (const XnUInt8*)pExternalBuffer);
		}

		/// @copydoc OutputMetaData::Data
		inline const XnLabel* Data() const { return (const XnLabel*)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::Data
		inline const XnLabel*& Data() { return (const XnLabel*&)MapMetaData::Data(); }
		/// @copydoc OutputMetaData::WritableData
		inline XnLabel* WritableData() { return (XnLabel*)MapMetaData::WritableData(); }

		/// Gets a light object wrapping the label map
		inline const xn::LabelMap& LabelMap() const { return m_labelMap; }
		/// Gets a light object wrapping the writable label map
		inline xn::LabelMap& WritableLabelMap() { MakeDataWritable(); return m_writableLabelMap; }

		/**
		 * Gets the label of the pixel
		 *
		 * @param	nIndex		[in]	The index of the pixel in the buffer.
		 */
		inline const XnLabel& operator[](XnUInt32 nIndex) const
		{
			XN_ASSERT(nIndex < (XRes()*YRes()));
			return Data()[nIndex];
		}

		/**
		 * Gets the label of the pixel
		 *
		 * @param	x		[in]	X-coordinate of the pixel in the map
		 * @param	y		[in]	Y-coordinate of the pixel in the map
		 */
		inline const XnLabel& operator()(XnUInt32 x, XnUInt32 y) const
		{
			XN_ASSERT(x < XRes() && y < YRes());
			return (*this)[y*XRes() + x];
		}

		/// @copydoc OutputMetaData::GetUnderlying
		inline const XnSceneMetaData* GetUnderlying() const { return &m_scene; }
		/// @copydoc OutputMetaData::GetUnderlying
		inline XnSceneMetaData* GetUnderlying() { return &m_scene; }

	private:
		// block copy ctor and assignment operator
		SceneMetaData(const SceneMetaData& other);
		SceneMetaData& operator=(const SceneMetaData&);

		XnSceneMetaData m_scene;
		const xn::LabelMap m_labelMap;
		xn::LabelMap m_writableLabelMap;
	};

	//---------------------------------------------------------------------------
	// NodeWrapper
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref
	 * Base class for all node wrappers
	 */
	class NodeWrapper
	{
	public:
		friend class Context;

		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	A node handle.
		 */
		inline NodeWrapper(XnNodeHandle hNode)
		{
			NodeWrapper::SetHandle(hNode);
		}

		/// Gets the underlying C handle.
		inline operator XnNodeHandle() const { return m_hNode; }

		/**
		 * Checks if two node wrappers point to the same object.
		 *
		 * @param	other		[in]	Another object
		 */
		inline XnBool operator==(const NodeWrapper& other)
		{
			return (m_hNode == other.m_hNode);
		}

		/**
		 * Checks if two node wrappers point to different objects.
		 *
		 * @param	other		[in]	Another object
		 */
		inline XnBool operator!=(const NodeWrapper& other)
		{
			return (m_hNode != other.m_hNode);
		}

		/// TRUE if the object points to an actual node, FALSE otherwise.
		inline XnBool IsValid() const { return (m_hNode != NULL); }
		
		/** @copybrief xnGetNodeName()
		 * For full details and usage, see @ref xnGetNodeName()
		 */
		const XnChar* GetName() const {return xnGetNodeName(m_hNode); }

		/// Replaces the object being pointed.
		virtual void SetHandle(XnNodeHandle hNode) { m_hNode = hNode; }

	protected:
		XnNodeHandle m_hNode;
	};

	//---------------------------------------------------------------------------
	// Node Info
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref
	 * Represents information about an existing node, or a possibility of a node.
	 */
	class NodeInfo
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	pInfo		[in]	The C object to wrap
		 */
		NodeInfo(XnNodeInfo* pInfo) : m_pNeededNodes(NULL)
		{
			SetUnderlyingObject(pInfo);
		}

		/**
		 * Copy Ctor
		 *
		 * @param	other		[in]	An object to copy from.
		 */
		NodeInfo(const NodeInfo& other) : m_pNeededNodes(NULL)
		{
			SetUnderlyingObject(other.m_pInfo);
		}

		/// Dtor
		~NodeInfo()
		{
			SetUnderlyingObject(NULL);
		}

		/**
		 * Assignment operator
		 *
		 * @param	other		[in]	another object
		 */
		inline NodeInfo& operator=(const NodeInfo& other)
		{
			SetUnderlyingObject(other.m_pInfo);
			return *this;
		}

		/// Gets the underlying C object
		inline operator XnNodeInfo*()
		{
			return m_pInfo;
		}

		/** @copybrief xnNodeInfoSetInstanceName
		 * For full details and usage, see @ref xnNodeInfoSetInstanceName
		 */
		inline XnStatus SetInstanceName(const XnChar* strName)
		{
			return xnNodeInfoSetInstanceName(m_pInfo, strName);
		}

		/** @copybrief xnNodeInfoGetDescription
		 * For full details and usage, see @ref xnNodeInfoGetDescription()
		 */
		inline const XnProductionNodeDescription& GetDescription() const
		{
			return *xnNodeInfoGetDescription(m_pInfo);
		}

		/** @copybrief xnNodeInfoGetInstanceName
		 * For full details and usage, see @ref xnNodeInfoGetInstanceName()
		 */
		inline const XnChar* GetInstanceName() const
		{
			return xnNodeInfoGetInstanceName(m_pInfo);
		}

		/** @copybrief xnNodeInfoGetCreationInfo
		 * For full details and usage, see @ref xnNodeInfoGetCreationInfo
		 */		inline const XnChar* GetCreationInfo() const
		{
			return xnNodeInfoGetCreationInfo(m_pInfo);
		}

		/** @copybrief xnNodeInfoGetNeededNodes
		 * For full details and usage, see @ref xnNodeInfoGetNeededNodes
		 */
		inline NodeInfoList& GetNeededNodes() const;

		/**
		 * Gets the node instance represented by this info object.
		 *
		 * @param	node		[in]	A production node to be pointing to the node.
		 * 
		 * @returns an error if this node info object does not point to an actual node.
		 */
		inline XnStatus GetInstance(ProductionNode& node) const;

	private:
		inline void SetUnderlyingObject(XnNodeInfo* pInfo);

		XnNodeInfo* m_pInfo;
		mutable NodeInfoList* m_pNeededNodes;
	};

	//---------------------------------------------------------------------------
	// Query
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref
	 * Represents a query, containing filters regarding node info objects.
	 * Note that the relation between the filters is an AND relation.
	 */
	class Query
	{
	public:
		/// Ctor
		inline Query()
		{
			xnNodeQueryAllocate(&m_pQuery);
		}

		/// Dtor
		~Query()
		{
			xnNodeQueryFree(m_pQuery);
			m_pQuery = NULL;
		}

		/// Gets the underlying C object
		inline XnNodeQuery* GetUnderlyingObject() const { return m_pQuery; }

		/** @copybrief xnNodeQuerySetVendor()
		 * For full details and usage, see @ref xnNodeQuerySetVendor()
		 */
		inline XnStatus SetVendor(const XnChar* strVendor)
		{
			return xnNodeQuerySetVendor(m_pQuery, strVendor);
		}

		/** @copybrief xnNodeQuerySetName()
		 * For full details and usage, see @ref xnNodeQuerySetName()
		 */		
		inline XnStatus SetName(const XnChar* strName)
		{
			return xnNodeQuerySetName(m_pQuery, strName);
		}

		/** @copybrief xnNodeQuerySetMinVersion
		 * For full details and usage, see @ref xnNodeQuerySetMinVersion
		 */		
		inline XnStatus SetMinVersion(const XnVersion& minVersion)
		{
			return xnNodeQuerySetMinVersion(m_pQuery, &minVersion);
		}

		/** @copybrief xnNodeQuerySetMaxVersion
		 * For full details and usage, see @ref xnNodeQuerySetMaxVersion
		 */
		inline XnStatus SetMaxVersion(const XnVersion& maxVersion)
		{
			return xnNodeQuerySetMaxVersion(m_pQuery, &maxVersion);
		}

		/** @copybrief xnNodeQueryAddSupportedCapability
		 * For full details and usage, see @ref xnNodeQueryAddSupportedCapability
		 */
		inline XnStatus AddSupportedCapability(const XnChar* strNeededCapability)
		{
			return xnNodeQueryAddSupportedCapability(m_pQuery, strNeededCapability);
		}

		/** @copybrief xnNodeQueryAddSupportedMapOutputMode
		 * For full details and usage, see @ref xnNodeQueryAddSupportedMapOutputMode
		 */
		inline XnStatus AddSupportedMapOutputMode(const XnMapOutputMode& MapOutputMode)
		{
			return xnNodeQueryAddSupportedMapOutputMode(m_pQuery, &MapOutputMode);
		}

		/** @copybrief xnNodeQuerySetSupportedMinUserPositions
		 * For full details and usage, see @ref xnNodeQuerySetSupportedMinUserPositions
		 */
		inline XnStatus SetSupportedMinUserPositions(const XnUInt32 nCount)
		{
			return xnNodeQuerySetSupportedMinUserPositions(m_pQuery, nCount);
		}

		/** @copybrief xnNodeQuerySetExistingNodeOnly
		 * For full details and usage, see @ref xnNodeQuerySetExistingNodeOnly
		 */
		inline XnStatus SetExistingNodeOnly(XnBool bExistingNode)
		{
			return xnNodeQuerySetExistingNodeOnly(m_pQuery, bExistingNode);
		}

		/** @copybrief xnNodeQueryAddNeededNode
		 * For full details and usage, see @ref xnNodeQueryAddNeededNode
		 */
		inline XnStatus AddNeededNode(const XnChar* strInstanceName)
		{
			return xnNodeQueryAddNeededNode(m_pQuery, strInstanceName);
		}

		/** @copybrief xnNodeQuerySetCreationInfo
		 * For full details and usage, see @ref xnNodeQuerySetCreationInfo
		 */		
		inline XnStatus SetCreationInfo(const XnChar* strCreationInfo)
		{
			return xnNodeQuerySetCreationInfo(m_pQuery, strCreationInfo);
		}

	private:
		XnNodeQuery* m_pQuery;
	};

	//---------------------------------------------------------------------------
	// Node Info List
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref
	 * Represents a list of @ref NodeInfo objects.
	 */
	class NodeInfoList
	{
	public:
		/// Represents an iterator over a @ref NodeInfoList list.
		class Iterator
		{
		public:
			friend class NodeInfoList;

			/**
			 * Checks if another iterator points at the same location.
			 *
			 * @param	other		[in]	Another iterator
			 */
			XnBool operator==(const Iterator& other) const
			{
				return m_it.pCurrent == other.m_it.pCurrent;
			}

			/**
			 * Checks if another iterator points at a different location.
			 *
			 * @param	other		[in]	Another iterator
			 */
			XnBool operator!=(const Iterator& other) const
			{
				return m_it.pCurrent != other.m_it.pCurrent;
			}

			/**
			 * Moves the iterator to the next object. If the end of the list was reached, the iterator
			 * will now be equal to @ref NodeInfoList::End().
			 */
			inline Iterator& operator++()
			{
				UpdateInternalObject(xnNodeInfoListGetNext(m_it));
				return *this;
			}

			/**
			 * Returns an iterator pointing at the next object. If the end of the list was reached, that iterator
			 * will now be equal to @ref NodeInfoList::End().
			 */
			inline Iterator operator++(int)
			{
				XnNodeInfoListIterator curr = m_it;
				UpdateInternalObject(xnNodeInfoListGetNext(m_it));
				return Iterator(curr);
			}

			/**
			 * Moves the iterator to the previous object.
			 */
			inline Iterator& operator--()
			{
				UpdateInternalObject(xnNodeInfoListGetPrevious(m_it));
				return *this;
			}

			/**
			 * Returns an iterator pointing at the previous object.
			 */
			inline Iterator operator--(int)
			{
				XnNodeInfoListIterator curr = m_it;
				UpdateInternalObject(xnNodeInfoListGetPrevious(m_it));
				return Iterator(curr);
			}

			/// Gets the @ref NodeInfo object pointed by the iterator.
			inline NodeInfo operator*()
			{
				return m_Info;
			}

		private:
			inline Iterator(XnNodeInfoListIterator it) : m_Info(NULL)
			{
				UpdateInternalObject(it);
			}

			inline void UpdateInternalObject(XnNodeInfoListIterator it)
			{
				m_it = it;
				if (xnNodeInfoListIteratorIsValid(it))
				{
					XnNodeInfo* pInfo = xnNodeInfoListGetCurrent(it);
					m_Info = NodeInfo(pInfo);
				}
				else
				{
					m_Info = NodeInfo(NULL);
				}
			}

			NodeInfo m_Info;
			XnNodeInfoListIterator m_it;
		};

		/**
		 * Creates a new @ref NodeInfoList object.
		 */
		inline NodeInfoList() 
		{
			xnNodeInfoListAllocate(&m_pList);
			m_bAllocated = TRUE;
		}

		/**
		 * Creates a new @ref NodeInfoList object by wrapping a C @ref XnNodeInfoList object.
		 * Note that this list will not be freed upon destruction.
		 *
		 * @param	pList		[in]	The list to wrap.
		 */
		inline NodeInfoList(XnNodeInfoList* pList) : m_pList(pList), m_bAllocated(FALSE) {}

		/// Dtor
		inline ~NodeInfoList()
		{
			FreeImpl();
		}

		/// Gets the underlying C object.
		inline XnNodeInfoList* GetUnderlyingObject() const { return m_pList; }

		/**
		 * Replaces the underlying object, taking ownership of the object (meaning, it
		 * will be freed upon destruction).
		 *
		 * @param	pList		[in]	The list to wrap.
		 */
		inline void ReplaceUnderlyingObject(XnNodeInfoList* pList) 
		{
			FreeImpl();
			m_pList = pList;
			m_bAllocated = TRUE;
		}

		/** @copybrief xnNodeInfoListAdd
		 * For full details and usage, see @ref xnNodeInfoListAdd
		 */

		inline XnStatus Add(XnProductionNodeDescription& description, const XnChar* strCreationInfo, NodeInfoList* pNeededNodes)
		{
			XnNodeInfoList* pList = (pNeededNodes == NULL) ? NULL : pNeededNodes->GetUnderlyingObject();
			return xnNodeInfoListAdd(m_pList, &description, strCreationInfo, pList);
		}

		/** @copybrief xnNodeInfoListAddNode
		 * For full details and usage, see @ref xnNodeInfoListAddNode
		 */
		inline XnStatus AddNode(NodeInfo& info)
		{
			return xnNodeInfoListAddNode(m_pList, info);
		}

		/** @copybrief xnNodeInfoListAddNodeFromList
		 * For full details and usage, see @ref xnNodeInfoListAddNodeFromList
		 */
		inline XnStatus AddNodeFromAnotherList(Iterator& it)
		{
			return xnNodeInfoListAddNodeFromList(m_pList, it.m_it);
		}

		/// Gets an iterator to the first item in the list.
		inline Iterator Begin() const
		{
			return Iterator(xnNodeInfoListGetFirst(m_pList));
		}

		/// Gets an iterator marking the end of the list. This iterator does not point to a valid item.
		inline Iterator End() const
		{
			XnNodeInfoListIterator it = { NULL };
			return Iterator(it);
		}

		/// Gets an iterator to the last item in the list
		inline Iterator RBegin() const
		{
			return Iterator(xnNodeInfoListGetLast(m_pList));
		}

		/// Gets an iterator marking the start of the list. This iterator does not point to a valid item.
		inline Iterator REnd() const
		{
			XnNodeInfoListIterator it = { NULL };
			return Iterator(it);
		}

		/** @copybrief xnNodeInfoListRemove
		 * For full details and usage, see @ref xnNodeInfoListRemove
		 */
		inline XnStatus Remove(Iterator& it)
		{
			return xnNodeInfoListRemove(m_pList, it.m_it);
		}

		/** @copybrief xnNodeInfoListClear
		 * For full details and usage, see @ref xnNodeInfoListClear
		 */
		inline XnStatus Clear()
		{
			return xnNodeInfoListClear(m_pList);
		}

		/** @copybrief xnNodeInfoListAppend
		 * For full details and usage, see @ref xnNodeInfoListAppend
		 */
		inline XnStatus Append(NodeInfoList& other)
		{
			return xnNodeInfoListAppend(m_pList, other.GetUnderlyingObject());
		}

		/** @copybrief xnNodeInfoListIsEmpty
		 * For full details and usage, see @ref xnNodeInfoListIsEmpty
		 */
		inline XnBool IsEmpty()
		{
			return xnNodeInfoListIsEmpty(m_pList);
		}

		/** @copybrief xnNodeQueryFilterList
		 * For full details and usage, see @ref xnNodeQueryFilterList
		 */
		inline XnStatus FilterList(Context& context, Query& query);

	private:
		inline void FreeImpl()
		{
			if (m_bAllocated)
			{
				xnNodeInfoListFree(m_pList);
				m_bAllocated = FALSE;
				m_pList = NULL;
			}
		}

		XnNodeInfoList* m_pList;
		XnBool m_bAllocated;
	};

	//---------------------------------------------------------------------------
	// Production Nodes Functionality
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref
	 * Base class for all capabilities.
	 */
	class Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	The node handle
		 */
		Capability(XnNodeHandle hNode) : m_hNode(hNode) {}

		/**
		 * Replaces the node handle
		 *
		 * @param	hNode		[in]	Underlying node handle
		 */
		inline void SetUnderlyingHandle(XnNodeHandle hNode)
		{
			m_hNode = hNode;
		}

	protected:
		XnNodeHandle m_hNode;
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Error State</b> capability functions. see @ref err_state for additional details.
	 */
	class ErrorStateCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		ErrorStateCapability(XnNodeHandle hNode) : Capability(hNode) {}

		/** @copybrief xnGetNodeErrorState
		 * For full details and usage, see @ref xnGetNodeErrorState
		 */
		inline XnStatus GetErrorState()
		{
			return xnGetNodeErrorState(m_hNode);
		}

		/** @copybrief xnRegisterToNodeErrorStateChange
		 * For full details and usage, see @ref xnRegisterToNodeErrorStateChange
		 */
		inline XnStatus RegisterToErrorStateChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToNodeErrorStateChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromNodeErrorStateChange
		 * For full details and usage, see @ref xnUnregisterFromNodeErrorStateChange
		 */
		inline void UnregisterFromErrorStateChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromNodeErrorStateChange, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Base class for all production nodes.
	 */
	class ProductionNode : public NodeWrapper
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline ProductionNode(XnNodeHandle hNode = NULL) : NodeWrapper(hNode) {}

		/** @copybrief xnGetNodeInfo
		 * For full details and usage, see @ref xnGetNodeInfo
		 */
		inline NodeInfo GetInfo() const { return NodeInfo(xnGetNodeInfo(m_hNode)); }

		/** @copybrief xnRefProductionNode
		 * For full details and usage, see @ref xnRefProductionNode
		 */
		inline XnStatus Ref() { return xnRefProductionNode(m_hNode); }

		/** @copybrief xnUnrefProductionNode
		 * For full details and usage, see @ref xnUnrefProductionNode
		 */
		inline void Unref() 
		{ 
			xnUnrefProductionNode(m_hNode); 
			SetHandle(NULL);
		}

		/** @copybrief xnAddNeededNode
		 * For full details and usage, see @ref xnAddNeededNode
		 */
		inline XnStatus AddNeededNode(ProductionNode& needed)
		{
			return xnAddNeededNode(m_hNode, needed.m_hNode);
		}

		/** @copybrief xnRemoveNeededNode
		 * For full details and usage, see @ref xnRemoveNeededNode
		 */
		inline XnStatus RemoveNeededNode(ProductionNode& needed)
		{
			return xnRemoveNeededNode(m_hNode, needed.m_hNode);
		}

		/** @copybrief xnGetContextFromNodeHandle
		 * For full details and usage, see @ref xnGetContextFromNodeHandle
		 */
		inline void GetContext(Context& context);

		/** @copybrief xnIsCapabilitySupported
		 * For full details and usage, see @ref xnIsCapabilitySupported
		 */
		inline XnBool IsCapabilitySupported(const XnChar* strCapabilityName) const
		{
			return xnIsCapabilitySupported(m_hNode, strCapabilityName);
		}

		/** @copybrief xnSetIntProperty
		 * For full details and usage, see @ref xnSetIntProperty
		 */
		inline XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
		{
			return xnSetIntProperty(m_hNode, strName, nValue);
		}

		/** @copybrief xnSetRealProperty
		 * For full details and usage, see @ref xnSetRealProperty
		 */
		inline XnStatus SetRealProperty(const XnChar* strName, XnDouble dValue)
		{
			return xnSetRealProperty(m_hNode, strName, dValue);
		}

		/** @copybrief xnSetStringProperty
		 * For full details and usage, see @ref xnSetStringProperty
		 */
		inline XnStatus SetStringProperty(const XnChar* strName, const XnChar* strValue)
		{
			return xnSetStringProperty(m_hNode, strName, strValue);
		}

		/** @copybrief xnSetGeneralProperty
		 * For full details and usage, see @ref xnSetGeneralProperty
		 */
		inline XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
		{
			return xnSetGeneralProperty(m_hNode, strName, nBufferSize, pBuffer);
		}

		/** @copybrief xnGetIntProperty
		 * For full details and usage, see @ref xnGetIntProperty
		 */
		inline XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
		{
			return xnGetIntProperty(m_hNode, strName, &nValue);
		}

		/** @copybrief xnGetRealProperty
		 * For full details and usage, see @ref xnGetRealProperty
		 */
		inline XnStatus GetRealProperty(const XnChar* strName, XnDouble &dValue) const
		{
			return xnGetRealProperty(m_hNode, strName, &dValue);
		}

		/** @copybrief xnGetStringProperty
		 * For full details and usage, see @ref xnGetStringProperty
		 */
		inline XnStatus GetStringProperty(const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize) const
		{
			return xnGetStringProperty(m_hNode, strName, csValue, nBufSize);
		}

		/** @copybrief xnGetGeneralProperty
		 * For full details and usage, see @ref xnGetGeneralProperty
		 */
		inline XnStatus GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
		{
			return xnGetGeneralProperty(m_hNode, strName, nBufferSize, pBuffer);
		}

		/** @copybrief xnLockNodeForChanges
		 * For full details and usage, see @ref xnLockNodeForChanges
		 */
		inline XnStatus LockForChanges(XnLockHandle* phLock)
		{
			return xnLockNodeForChanges(m_hNode, phLock);
		}

		/** @copybrief xnUnlockNodeForChanges
		 * For full details and usage, see @ref xnUnlockNodeForChanges
		 */
		inline void UnlockForChanges(XnLockHandle hLock)
		{
			xnUnlockNodeForChanges(m_hNode, hLock);
		}

		/** @copybrief xnLockedNodeStartChanges
		 * For full details and usage, see @ref xnLockedNodeStartChanges
		 */
		inline XnStatus LockedNodeStartChanges(XnLockHandle hLock)
		{
			return xnLockedNodeStartChanges(m_hNode, hLock);
		}

		/** @copybrief xnLockedNodeEndChanges
		 * For full details and usage, see @ref xnLockedNodeEndChanges
		 */
		inline void LockedNodeEndChanges(XnLockHandle hLock)
		{
			xnLockedNodeEndChanges(m_hNode, hLock);
		}

		/**
		 * Gets an @ref ErrorStateCapability object for accessing Error State functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ERROR_STATE is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline ErrorStateCapability GetErrorStateCap()
		{
			return ErrorStateCapability(m_hNode);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a hardware device node
	 */
	class Device : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline Device(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Mirror</b> capability functions. see @ref mirror for additional details.
	 */
	class MirrorCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline MirrorCapability(XnNodeHandle hNode) : Capability(hNode) {}

		/** @copybrief xnSetMirror
		 * For full details and usage, see @ref xnSetMirror
		 */
		inline XnStatus SetMirror(XnBool bMirror)
		{
			return xnSetMirror(m_hNode, bMirror);
		}

		/** @copybrief xnIsMirrored
		 * For full details and usage, see @ref xnIsMirrored
		 */
		inline XnBool IsMirrored() const
		{
			return xnIsMirrored(m_hNode);
		}

		/** @copybrief xnRegisterToMirrorChange
		 * For full details and usage, see @ref xnRegisterToMirrorChange
		 */
		inline XnStatus RegisterToMirrorChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToMirrorChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromMirrorChange
		 * For full details and usage, see @ref xnUnregisterFromMirrorChange
		 */
		inline void UnregisterFromMirrorChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromMirrorChange, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Alternative View Point</b> capability functions. see @ref altviewpoint for additional details.
	 */
	class AlternativeViewPointCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline AlternativeViewPointCapability(XnNodeHandle hNode) : Capability(hNode) {}

		/** @copybrief xnIsViewPointSupported
		 * For full details and usage, see @ref xnIsViewPointSupported
		 */
		inline XnBool IsViewPointSupported(ProductionNode& otherNode) const
		{
			return xnIsViewPointSupported(m_hNode, otherNode);
		}

		/** @copybrief xnSetViewPoint
		 * For full details and usage, see @ref xnSetViewPoint
		 */
		inline XnStatus SetViewPoint(ProductionNode& otherNode)
		{
			return xnSetViewPoint(m_hNode, otherNode);
		}

		/** @copybrief xnResetViewPoint
		 * For full details and usage, see @ref xnResetViewPoint
		 */
		inline XnStatus ResetViewPoint()
		{
			return xnResetViewPoint(m_hNode);
		}

		/** @copybrief xnIsViewPointAs
		 * For full details and usage, see @ref xnIsViewPointAs
		 */
		inline XnBool IsViewPointAs(ProductionNode& otherNode) const
		{
			return xnIsViewPointAs(m_hNode, otherNode);
		}

		/** @copybrief xnRegisterToViewPointChange
		 * For full details and usage, see @ref xnRegisterToViewPointChange
		 */
		inline XnStatus RegisterToViewPointChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToViewPointChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromViewPointChange
		 * For full details and usage, see @ref xnUnregisterFromViewPointChange
		 */
		inline void UnregisterFromViewPointChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromViewPointChange, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Frame Sync</b> capability functions. see @ref framesync for additional details.
	 */
	class FrameSyncCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline FrameSyncCapability(XnNodeHandle hNode) : Capability(hNode) {}

		/** @copybrief xnCanFrameSyncWith
		 * For full details and usage, see @ref xnCanFrameSyncWith
		 */
		inline XnBool CanFrameSyncWith(Generator& other);

		/** @copybrief xnFrameSyncWith
		 * For full details and usage, see @ref xnFrameSyncWith
		 */
		inline XnStatus FrameSyncWith(Generator& other);

		/** @copybrief xnStopFrameSyncWith
		 * For full details and usage, see @ref xnStopFrameSyncWith
		 */
		inline XnStatus StopFrameSyncWith(Generator& other);

		/** @copybrief xnIsFrameSyncedWith
		 * For full details and usage, see @ref xnIsFrameSyncedWith
		 */
		inline XnBool IsFrameSyncedWith(Generator& other);

		/** @copybrief xnRegisterToFrameSyncChange
		 * For full details and usage, see @ref xnRegisterToFrameSyncChange
		 */
		inline XnStatus RegisterToFrameSyncChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToFrameSyncChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromFrameSyncChange
		 * For full details and usage, see @ref xnUnregisterFromFrameSyncChange
		 */
		inline void UnregisterFromFrameSyncChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromFrameSyncChange, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Base class for all generator nodes
	 */
	class Generator : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline Generator(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}

		/** @copybrief xnStartGenerating
		 * For full details and usage, see @ref xnStartGenerating
		 */
		inline XnStatus StartGenerating()
		{
			return xnStartGenerating(m_hNode);
		}

		/** @copybrief xnIsGenerating
		 * For full details and usage, see @ref xnIsGenerating
		 */
		inline XnBool IsGenerating() const
		{
			return xnIsGenerating(m_hNode);
		}

		/** @copybrief xnStopGenerating
		 * For full details and usage, see @ref xnStopGenerating
		 */
		inline XnStatus StopGenerating()
		{
			return xnStopGenerating(m_hNode);
		}

		/** @copybrief xnRegisterToGenerationRunningChange
		 * For full details and usage, see @ref xnRegisterToGenerationRunningChange
		 */
		inline XnStatus RegisterToGenerationRunningChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle &hCallback)
		{
			return _RegisterToStateChange(xnRegisterToGenerationRunningChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromGenerationRunningChange
		 * For full details and usage, see @ref xnUnregisterFromGenerationRunningChange
		 */
		inline void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromGenerationRunningChange, m_hNode, hCallback);
		}

		/** @copybrief xnRegisterToNewDataAvailable
		 * For full details and usage, see @ref xnRegisterToNewDataAvailable
		 */
		inline XnStatus RegisterToNewDataAvailable(StateChangedHandler handler, void* pCookie, XnCallbackHandle &hCallback)
		{
			return _RegisterToStateChange(xnRegisterToNewDataAvailable, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromNewDataAvailable
		 * For full details and usage, see @ref xnUnregisterFromNewDataAvailable
		 */
		inline void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromNewDataAvailable, m_hNode, hCallback);
		}

		/** @copybrief xnIsNewDataAvailable
		 * For full details and usage, see @ref xnIsNewDataAvailable
		 */
		inline XnBool IsNewDataAvailable(XnUInt64* pnTimestamp = NULL)
		{
			return xnIsNewDataAvailable(m_hNode, pnTimestamp);
		}

		/** @copybrief xnWaitAndUpdateData
		 * For full details and usage, see @ref xnWaitAndUpdateData
		 */
		inline XnStatus WaitAndUpdateData()
		{
			return xnWaitAndUpdateData(m_hNode);
		}

		/** @copybrief xnIsDataNew
		 * For full details and usage, see @ref xnIsDataNew
		 */
		inline XnBool IsDataNew() const
		{
			return xnIsDataNew(m_hNode);
		}

		/** @copybrief xnGetDataSize
		 * For full details and usage, see @ref xnGetDataSize
		 */
		inline XnUInt32 GetDataSize() const
		{
			return xnGetDataSize(m_hNode);
		}

		/** @copybrief xnGetTimestamp
		 * For full details and usage, see @ref xnGetTimestamp
		 */
		inline XnUInt64 GetTimestamp() const
		{
			return xnGetTimestamp(m_hNode);
		}

		/** @copybrief xnGetFrameID
		 * For full details and usage, see @ref xnGetFrameID
		 */
		inline XnUInt32 GetFrameID() const
		{
			return xnGetFrameID(m_hNode);
		}

		/**
		 * Gets an @ref MirrorCapability object for accessing Mirror functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_MIRROR is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline MirrorCapability GetMirrorCap()
		{ 
			return MirrorCapability(m_hNode); 
		}

		/**
		 * Gets an @ref AlternativeViewPointCapability object for accessing Alternative View Point functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ALTERNATIVE_VIEW_POINT is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline AlternativeViewPointCapability GetAlternativeViewPointCap() 
		{ 
			return AlternativeViewPointCapability(m_hNode); 
		}

		/**
		 * Gets an @ref FrameSyncCapability object for accessing Frame Sync functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_FRAME_SYNC is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline FrameSyncCapability GetFrameSyncCap()
		{
			return FrameSyncCapability(m_hNode);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a Recorder node
	 */
	class Recorder : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	A node handle
		 */
		inline Recorder(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}

		/** @copybrief xnCreateRecorder
		 * For full details and usage, see @ref xnCreateRecorder
		 */
		inline XnStatus Create(Context& context, const XnChar* strFormatName);

		/** @copybrief xnSetRecorderDestination
		 * For full details and usage, see @ref xnSetRecorderDestination
		 */
		inline XnStatus SetDestination(XnRecordMedium destType, const XnChar* strDest)
		{
			return xnSetRecorderDestination(m_hNode, destType, strDest);
		}

		/** @copybrief xnAddNodeToRecording
		 * For full details and usage, see @ref xnAddNodeToRecording
		 */
		inline XnStatus AddNodeToRecording(ProductionNode& Node, XnCodecID compression = XN_CODEC_NULL)
		{
			return xnAddNodeToRecording(m_hNode, Node, compression);
		}

		/** @copybrief xnRemoveNodeFromRecording
		 * For full details and usage, see @ref xnRemoveNodeFromRecording
		 */
		inline XnStatus RemoveNodeFromRecording(ProductionNode& Node)
		{
			return xnRemoveNodeFromRecording(m_hNode, Node);
		}

		/** @copybrief xnRecord
		 * For full details and usage, see @ref xnRecord
		 */
		inline XnStatus Record()
		{
			return xnRecord(m_hNode);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a Player node
	 */
	class Player : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Player node handle
		 */
		inline Player(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}

		/** @copybrief xnCreatePlayer
		 * For full details and usage, see @ref xnCreatePlayer
		 */
		inline XnStatus Create(Context& context, const XnChar* strFormatName);

		/** @copybrief xnSetPlayerRepeat
		 * For full details and usage, see @ref xnSetPlayerRepeat
		 */
		inline XnStatus SetRepeat(XnBool bRepeat)
		{
			return xnSetPlayerRepeat(m_hNode, bRepeat);
		}

		/** @copybrief xnSetPlayerSource
		 * For full details and usage, see @ref xnSetPlayerSource
		 */
		inline XnStatus SetSource(XnRecordMedium sourceType, const XnChar* strSource)
		{
			return xnSetPlayerSource(m_hNode, sourceType, strSource);
		}

		/** @copybrief xnGetPlayerSource
		 * For full details and usage, see @ref xnGetPlayerSource
		 */
		inline XnStatus GetSource(XnRecordMedium &sourceType, XnChar* strSource, XnUInt32 nBufSize)
		{
			return xnGetPlayerSource(m_hNode, &sourceType, strSource, nBufSize);
		}

		/** @copybrief xnPlayerReadNext
		 * For full details and usage, see @ref xnPlayerReadNext
		 */
		inline XnStatus ReadNext()
		{
			return xnPlayerReadNext(m_hNode);
		}

		/** @copybrief xnSeekPlayerToTimeStamp
		 * For full details and usage, see @ref xnSeekPlayerToTimeStamp
		 */
		inline XnStatus SeekToTimeStamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin)
		{
			return xnSeekPlayerToTimeStamp(m_hNode, nTimeOffset, origin);
		}

		/** @copybrief xnSeekPlayerToFrame
		 * For full details and usage, see @ref xnSeekPlayerToFrame
		 */
		inline XnStatus SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin)
		{
			return xnSeekPlayerToFrame(m_hNode, strNodeName, nFrameOffset, origin);
		}

		/** @copybrief xnTellPlayerTimestamp
		 * For full details and usage, see @ref xnTellPlayerTimestamp
		 */
		inline XnStatus TellTimestamp(XnUInt64& nTimestamp)
		{
			return xnTellPlayerTimestamp(m_hNode, &nTimestamp);
		}

		/** @copybrief xnTellPlayerFrame
		 * For full details and usage, see @ref xnTellPlayerFrame
		 */
		inline XnStatus TellFrame(const XnChar* strNodeName, XnUInt32& nFrame)
		{
			return xnTellPlayerFrame(m_hNode, strNodeName, &nFrame);
		}

		/** @copybrief xnGetPlayerNumFrames
		 * For full details and usage, see @ref xnGetPlayerNumFrames
		 */
		inline XnStatus GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames)
		{
			return xnGetPlayerNumFrames(m_hNode, strNodeName, &nFrames);
		}

		/** @copybrief xnGetPlayerSupportedFormat
		 * For full details and usage, see @ref xnGetPlayerSupportedFormat
		 */
		inline const XnChar* GetSupportedFormat()
		{
			return xnGetPlayerSupportedFormat(m_hNode);
		}

		/** @copybrief xnEnumeratePlayerNodes
		 * For full details and usage, see @ref xnEnumeratePlayerNodes
		 */
		inline XnStatus EnumerateNodes(NodeInfoList& list)
		{
			XnNodeInfoList* pList;
			XnStatus nRetVal = xnEnumeratePlayerNodes(m_hNode, &pList);
			XN_IS_STATUS_OK(nRetVal);

			list.ReplaceUnderlyingObject(pList);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnIsPlayerAtEOF
		 * For full details and usage, see @ref xnIsPlayerAtEOF
		 */
		inline XnBool IsEOF()
		{
			return xnIsPlayerAtEOF(m_hNode);
		}

		/** @copybrief xnRegisterToEndOfFileReached
		 * For full details and usage, see @ref xnRegisterToEndOfFileReached
		 */
		inline XnStatus RegisterToEndOfFileReached(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToEndOfFileReached, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromEndOfFileReached
		 * For full details and usage, see @ref xnUnregisterFromEndOfFileReached
		 */
		inline void UnregisterFromEndOfFileReached(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromEndOfFileReached, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Cropping</b> capability functions. see @ref cropping for additional details.
	 */
	class CroppingCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline CroppingCapability(XnNodeHandle hNode) : Capability(hNode) {}

		/** @copybrief xnSetCropping
		 * For full details and usage, see @ref xnSetCropping
		 */
		inline XnStatus SetCropping(const XnCropping& Cropping)
		{
			return xnSetCropping(m_hNode, &Cropping);
		}

		/** @copybrief xnGetCropping
		 * For full details and usage, see @ref xnGetCropping
		 */
		inline XnStatus GetCropping(XnCropping& Cropping) const
		{
			return xnGetCropping(m_hNode, &Cropping);
		}

		/** @copybrief xnRegisterToCroppingChange
		 * For full details and usage, see @ref xnRegisterToCroppingChange
		 */
		inline XnStatus RegisterToCroppingChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToCroppingChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromCroppingChange
		 * For full details and usage, see @ref xnUnregisterFromCroppingChange
		 */
		inline void UnregisterFromCroppingChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromCroppingChange, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Base class for Map Generators
	 */
	class MapGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline MapGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}

		/** @copybrief xnGetSupportedMapOutputModesCount
		 * For full details and usage, see @ref xnGetSupportedMapOutputModesCount
		 */
		inline XnUInt32 GetSupportedMapOutputModesCount() const
		{
			return xnGetSupportedMapOutputModesCount(m_hNode);
		}

		/** @copybrief xnGetSupportedMapOutputModes
		 * For full details and usage, see @ref xnGetSupportedMapOutputModes
		 */
		inline XnStatus GetSupportedMapOutputModes(XnMapOutputMode* aModes, XnUInt32& nCount) const
		{
			return xnGetSupportedMapOutputModes(m_hNode, aModes, &nCount);
		}

		/** @copybrief xnSetMapOutputMode
		 * For full details and usage, see @ref xnSetMapOutputMode
		 */
		inline XnStatus SetMapOutputMode(const XnMapOutputMode& OutputMode)
		{
			return xnSetMapOutputMode(m_hNode, &OutputMode);
		}

		/** @copybrief xnGetMapOutputMode
		 * For full details and usage, see @ref xnGetMapOutputMode
		 */
		inline XnStatus GetMapOutputMode(XnMapOutputMode &OutputMode) const
		{
			return xnGetMapOutputMode(m_hNode, &OutputMode);
		}

		/** @copybrief xnRegisterToMapOutputModeChange
		 * For full details and usage, see @ref xnRegisterToMapOutputModeChange
		 */
		inline XnStatus RegisterToMapOutputModeChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToMapOutputModeChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromMapOutputModeChange
		 * For full details and usage, see @ref xnUnregisterFromMapOutputModeChange
		 */
		inline void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromMapOutputModeChange, m_hNode, hCallback);
		}

		/**
		 * Gets an @ref CroppingCapability object for accessing Cropping functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_CROPPING is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline CroppingCapability GetCroppingCap()
		{
			return CroppingCapability(m_hNode);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>User Position</b> capability functions. see @ref userpos for additional details.
	 */
	class UserPositionCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline UserPositionCapability(XnNodeHandle hNode = NULL) : Capability(hNode) {}

		/** @copybrief xnGetSupportedUserPositionsCount
		 * For full details and usage, see @ref xnGetSupportedUserPositionsCount
		 */
		inline XnUInt32 GetSupportedUserPositionsCount() const
		{
			return xnGetSupportedUserPositionsCount(m_hNode);
		}

		/** @copybrief xnSetUserPosition
		 * For full details and usage, see @ref xnSetUserPosition
		 */
		inline XnStatus SetUserPosition(XnUInt32 nIndex, const XnBoundingBox3D& Position)
		{
			return xnSetUserPosition(m_hNode, nIndex, &Position);
		}

		/** @copybrief xnGetUserPosition
		 * For full details and usage, see @ref xnGetUserPosition
		 */
		inline XnStatus GetUserPosition(XnUInt32 nIndex, XnBoundingBox3D& Position) const
		{
			return xnGetUserPosition(m_hNode, nIndex, &Position);
		}

		/** @copybrief xnRegisterToUserPositionChange
		 * For full details and usage, see @ref xnRegisterToUserPositionChange
		 */
		inline XnStatus RegisterToUserPositionChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToUserPositionChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromUserPositionChange
		 * For full details and usage, see @ref xnUnregisterFromUserPositionChange
		 */
		inline void UnregisterFromUserPositionChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromUserPositionChange, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a depth generator.
	 */
	class DepthGenerator : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline DepthGenerator(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}

		/** @copybrief xnCreateDepthGenerator
		 * For full details and usage, see @ref xnCreateDepthGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetDepthMetaData
		 * For full details and usage, see @ref xnGetDepthMetaData
		 */
		inline void GetMetaData(DepthMetaData& metaData) const 
		{
			return xnGetDepthMetaData(m_hNode, metaData.GetUnderlying());
		}

		/** @copybrief xnGetDepthMap
		 * For full details and usage, see @ref xnGetDepthMap
		 */
		inline const XnDepthPixel* GetDepthMap() const
		{
			return xnGetDepthMap(m_hNode);
		}

		/** @copybrief xnGetDeviceMaxDepth
		 * For full details and usage, see @ref xnGetDeviceMaxDepth
		 */
		inline XnDepthPixel GetDeviceMaxDepth() const
		{
			return xnGetDeviceMaxDepth(m_hNode);
		}

		/** @copybrief xnGetDepthFieldOfView
		 * For full details and usage, see @ref xnGetDepthFieldOfView
		 */
		inline XnStatus GetFieldOfView(XnFieldOfView& FOV) const
		{
			return xnGetDepthFieldOfView(m_hNode, &FOV);
		}

		/** @copybrief xnRegisterToDepthFieldOfViewChange
		 * For full details and usage, see @ref xnRegisterToDepthFieldOfViewChange
		 */
		inline XnStatus RegisterToFieldOfViewChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToDepthFieldOfViewChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromDepthFieldOfViewChange
		 * For full details and usage, see @ref xnUnregisterFromDepthFieldOfViewChange
		 */
		inline void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromDepthFieldOfViewChange, m_hNode, hCallback);
		}

		/** @copybrief xnConvertProjectiveToRealWorld
		 * For full details and usage, see @ref xnConvertProjectiveToRealWorld
		 */
		inline XnStatus ConvertProjectiveToRealWorld(XnUInt32 nCount, const XnPoint3D aProjective[], XnPoint3D aRealWorld[]) const
		{
			return xnConvertProjectiveToRealWorld(m_hNode, nCount, aProjective, aRealWorld);
		}

		/** @copybrief xnConvertRealWorldToProjective
		 * For full details and usage, see @ref xnConvertRealWorldToProjective
		 */
		inline XnStatus ConvertRealWorldToProjective(XnUInt32 nCount, const XnPoint3D aRealWorld[], XnPoint3D aProjective[]) const
		{
			return xnConvertRealWorldToProjective(m_hNode, nCount, aRealWorld, aProjective);
		}

		/**
		 * Gets an @ref UserPositionCapability object for accessing User Position functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_USER_POSITION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline UserPositionCapability GetUserPositionCap() 
		{
			return UserPositionCapability(m_hNode);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a mock depth generator
	 */
	class MockDepthGenerator : public DepthGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline MockDepthGenerator(XnNodeHandle hNode = NULL) : DepthGenerator(hNode) {}

		/**
		 * Creates a mock depth generator
		 *
		 * @param	context		[in]	The context in which the node should be generated
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);

		/**
		 * Creates a mock depth generator, copying the initial state from another depth generator.
		 *
		 * @param	other		[in]	The depth generator to copy state from
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(DepthGenerator& other, const XnChar* strName = NULL);

		/** @copybrief xnMockDepthSetData
		 * For full details and usage, see @ref xnMockDepthSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnDepthPixel* pDepthMap)
		{
			return xnMockDepthSetData(m_hNode, nFrameID, nTimestamp, nDataSize, pDepthMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	depthMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(DepthMetaData& depthMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, depthMD.DataSize(), depthMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param	depthMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(DepthMetaData& depthMD)
		{
			return SetData(depthMD, depthMD.FrameID(), depthMD.Timestamp());
		}
	};

	/**
	 * @ingroup cppref
	 * Represents an image generator
	 */
	class ImageGenerator : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline ImageGenerator(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}

		/** @copybrief xnCreateImageGenerator
		 * For full details and usage, see @ref xnCreateImageGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetImageMetaData
		 * For full details and usage, see @ref xnGetImageMetaData
		 */
		inline void GetMetaData(ImageMetaData& metaData) const 
		{
			return xnGetImageMetaData(m_hNode, metaData.GetUnderlying());
		}

		/** @copybrief xnGetRGB24ImageMap
		 * For full details and usage, see @ref xnGetRGB24ImageMap
		 */
		inline const XnRGB24Pixel* GetRGB24ImageMap() const
		{
			return xnGetRGB24ImageMap(m_hNode);
		}

		/** @copybrief xnGetYUV422ImageMap
		 * For full details and usage, see @ref xnGetYUV422ImageMap
		 */
		inline const XnYUV422DoublePixel* GetYUV422ImageMap() const
		{
			return xnGetYUV422ImageMap(m_hNode);
		}

		/** @copybrief xnGetGrayscale8ImageMap
		 * For full details and usage, see @ref xnGetGrayscale8ImageMap
		 */
		inline const XnGrayscale8Pixel* GetGrayscale8ImageMap() const
		{
			return xnGetGrayscale8ImageMap(m_hNode);
		}

		/** @copybrief xnGetGrayscale16ImageMap
		 * For full details and usage, see @ref xnGetGrayscale16ImageMap
		 */
		inline const XnGrayscale16Pixel* GetGrayscale16ImageMap() const
		{
			return xnGetGrayscale16ImageMap(m_hNode);
		}

		/** @copybrief xnGetImageMap() 
		 * For full details, see @ref xnGetImageMap().
		 */
		inline const XnUInt8* GetImageMap() const
		{
			return xnGetImageMap(m_hNode);
		}

		/** @copybrief xnIsPixelFormatSupported
		 * For full details and usage, see @ref xnIsPixelFormatSupported
		 */
		inline XnBool IsPixelFormatSupported(XnPixelFormat Format) const
		{
			return xnIsPixelFormatSupported(m_hNode, Format);
		}

		/** @copybrief xnSetPixelFormat
		 * For full details and usage, see @ref xnSetPixelFormat
		 */
		inline XnStatus SetPixelFormat(XnPixelFormat Format)
		{
			return xnSetPixelFormat(m_hNode, Format);
		}

		/** @copybrief xnGetPixelFormat
		 * For full details and usage, see @ref xnGetPixelFormat
		 */
		inline XnPixelFormat GetPixelFormat() const
		{
			return xnGetPixelFormat(m_hNode);
		}

		/** @copybrief xnRegisterToPixelFormatChange
		 * For full details and usage, see @ref xnRegisterToPixelFormatChange
		 */
		inline XnStatus RegisterToPixelFormatChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToPixelFormatChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromPixelFormatChange
		 * For full details and usage, see @ref xnUnregisterFromPixelFormatChange
		 */
		inline void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromPixelFormatChange, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a mock image generator
	 */
	class MockImageGenerator : public ImageGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline MockImageGenerator(XnNodeHandle hNode = NULL) : ImageGenerator(hNode) {}

		/**
		 * Creates a mock image generator
		 *
		 * @param	context		[in]	The context in which the node should be generated
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);

		/**
		 * Creates a mock image generator, copying the initial state from another image generator.
		 *
		 * @param	other		[in]	The image generator to copy state from
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(ImageGenerator& other, const XnChar* strName = NULL);

		/** @copybrief xnMockImageSetData
		 * For full details and usage, see @ref xnMockImageSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pImageMap)
		{
			return xnMockImageSetData(m_hNode, nFrameID, nTimestamp, nDataSize, pImageMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	imageMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(ImageMetaData& imageMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, imageMD.DataSize(), imageMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param	imageMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(ImageMetaData& imageMD)
		{
			return SetData(imageMD, imageMD.FrameID(), imageMD.Timestamp());
		}
	};

	/**
	 * @ingroup cppref
	 * Represents an IR generator
	 */
	class IRGenerator : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline IRGenerator(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}

		/** @copybrief xnCreateIRGenerator
		 * For full details and usage, see @ref xnCreateIRGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetIRMetaData
		 * For full details and usage, see @ref xnGetIRMetaData
		 */
		inline void GetMetaData(IRMetaData& metaData) const 
		{ 
			xnGetIRMetaData(m_hNode, metaData.GetUnderlying());
		}

		/** @copybrief xnGetIRMap
		 * For full details and usage, see @ref xnGetIRMap
		 */
		inline const XnIRPixel* GetIRMap() const
		{
			return xnGetIRMap(m_hNode);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a mock IR generator
	 */
	class MockIRGenerator : public IRGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline MockIRGenerator(XnNodeHandle hNode = NULL) : IRGenerator(hNode) {}

		/**
		 * Creates a mock IR generator
		 *
		 * @param	context		[in]	The context in which the node should be generated
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);
		/**
		 * Creates a mock IR generator, copying the initial state from another IR generator.
		 *
		 * @param	other		[in]	The IR generator to copy state from
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(IRGenerator& other, const XnChar* strName = NULL);

		/** @copybrief xnMockIRSetData
		 * For full details and usage, see @ref xnMockIRSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnIRPixel* pIRMap)
		{
			return xnMockIRSetData(m_hNode, nFrameID, nTimestamp, nDataSize, pIRMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	irMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(IRMetaData& irMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, irMD.DataSize(), irMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param	irMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(IRMetaData& irMD)
		{
			return SetData(irMD, irMD.FrameID(), irMD.Timestamp());
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a Gestures Generator
	 */
	class GestureGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline GestureGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {} 

		/** @copybrief xnCreateGestureGenerator
		 * For full details and usage, see @ref xnCreateGestureGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnAddGesture
		 * For full details and usage, see @ref xnAddGesture
		 */
		inline XnStatus AddGesture(const XnChar* strGesture, XnBoundingBox3D* pArea)
		{
			return xnAddGesture(m_hNode, strGesture, pArea);
		}

		/** @copybrief xnRemoveGesture
		 * For full details and usage, see @ref xnRemoveGesture
		 */
		inline XnStatus RemoveGesture(const XnChar* strGesture)
		{
			return xnRemoveGesture(m_hNode, strGesture);
		}

		/** @copybrief xnGetActiveGestures
		 * For full details and usage, see @ref xnGetActiveGestures
		 */
		inline XnStatus GetActiveGestures(XnChar*& astrGestures, XnUInt16& nGestures) const
		{
			return xnGetActiveGestures(m_hNode, &astrGestures, &nGestures);
		}

		/** @copybrief xnEnumerateGestures
		 * For full details and usage, see @ref xnEnumerateGestures
		 */
		inline XnStatus EnumerateGestures(XnChar*& astrGestures, XnUInt16& nGestures)
		{
			return xnEnumerateGestures(m_hNode, &astrGestures, &nGestures);
		}

		/** @copybrief xnIsGestureAvailable
		 * For full details and usage, see @ref xnIsGestureAvailable
		 */
		inline XnBool IsGestureAvailable(const XnChar* strGesture)
		{
			return xnIsGestureAvailable(m_hNode, strGesture);
		}

		/** @copybrief xnIsGestureProgressSupported
		 * For full details and usage, see @ref xnIsGestureProgressSupported
		 */
		inline XnBool IsGestureProgressSupported(const XnChar* strGesture)
		{
			return xnIsGestureProgressSupported(m_hNode, strGesture);
		}

		/**
		 * Callback for the recognition of a gesture
		 * 
		 * @param	generator		[in]	The node that raised the event.
		 * @param	strGesture		[in]	The gesture that was recognized.
		 * @param	pIDPosition		[in]	The position in which the gesture was identified.
		 * @param	pEndPosition	[in]	The position of the hand that performed the gesture at the end of the gesture.
		 * @param	pCookie			[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureRecognized)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie);
		/**
		 * Callback for indication that a certain gesture is in progress
		 *
		 * @param	generator	[in]	The node that raised the event.
		 * @param	strGesture	[in]	The gesture that is on its way to being recognized.
		 * @param	pPosition	[in]	The current position of the hand that is performing the gesture.
		 * @param	fProgress	[in]	The percentage of the gesture that was already performed.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureProgress)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie);

		/** @copybrief RegisterGestureCallbacks
		 * For full details and usage, see @ref RegisterGestureCallbacks
		 */
		XnStatus RegisterGestureCallbacks(GestureRecognized RecognizedCB, GestureProgress ProgressCB, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			GestureCookie* pGestureCookie;
			XN_VALIDATE_ALLOC(pGestureCookie, GestureCookie);
			pGestureCookie->recognizedHandler = RecognizedCB;
			pGestureCookie->progressHandler = ProgressCB;
			pGestureCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterGestureCallbacks(m_hNode, GestureRecognizedCallback, GestureProgressCallback, pGestureCookie, &pGestureCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pGestureCookie);
				return (nRetVal);
			}

			hCallback = pGestureCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterGestureCallbacks
		 * For full details and usage, see @ref xnUnregisterGestureCallbacks
		 */
		inline void UnregisterGestureCallbacks(XnCallbackHandle hCallback)
		{
			GestureCookie* pGestureCookie = (GestureCookie*)hCallback;
			xnUnregisterGestureCallbacks(m_hNode, pGestureCookie->hCallback);
			xnOSFree(pGestureCookie);
		}

		/** @copybrief xnRegisterToPixelFormatChange
		 * For full details and usage, see @ref xnRegisterToPixelFormatChange
		 */
		inline XnStatus RegisterToGestureChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToPixelFormatChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromGestureChange
		 * For full details and usage, see @ref xnUnregisterFromGestureChange
		 */
		inline void UnregisterFromGestureChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromGestureChange, m_hNode, hCallback);
		}

	private:
		typedef struct GestureCookie
		{
			GestureRecognized recognizedHandler;
			GestureProgress progressHandler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} GestureCookie;

		static void XN_CALLBACK_TYPE GestureRecognizedCallback(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie)
		{
			GestureCookie* pGestureCookie = (GestureCookie*)pCookie;
			GestureGenerator gen(hNode);
			pGestureCookie->recognizedHandler(gen, strGesture, pIDPosition, pEndPosition, pGestureCookie->pUserCookie);
		}

		static void XN_CALLBACK_TYPE GestureProgressCallback(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie)
		{
			GestureCookie* pGestureCookie = (GestureCookie*)pCookie;
			GestureGenerator gen(hNode);
			pGestureCookie->progressHandler(gen, strGesture, pPosition, fProgress, pGestureCookie->pUserCookie);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a Scene Analyzer node
	 */
	class SceneAnalyzer : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline SceneAnalyzer(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}

		/** @copybrief xnCreateSceneAnalyzer
		 * For full details and usage, see @ref xnCreateSceneAnalyzer
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetSceneMetaData
		 * For full details and usage, see @ref xnGetSceneMetaData
		 */
		inline void GetMetaData(SceneMetaData& metaData) const
		{
			xnGetSceneMetaData(m_hNode, metaData.GetUnderlying());
		}

		/** @copybrief xnGetLabelMap
		 * For full details and usage, see @ref xnGetLabelMap
		 */
		inline const XnLabel* GetLabelMap() const
		{
			return xnGetLabelMap(m_hNode);
		}

		/** @copybrief xnGetFloor
		 * For full details and usage, see @ref xnGetFloor
		 */
		inline XnStatus GetFloor(XnPlane3D& Plane) const
		{
			return xnGetFloor(m_hNode, &Plane);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a Hands Generator node
	 */
	class HandsGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline HandsGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}

		/** @copybrief xnCreateHandsGenerator
		 * For full details and usage, see @ref xnCreateHandsGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * Callback for the creation of a new hand.
		 *
		 * @param	generator	[in]	the generator that raised this event.
		 * @param	user		[in]	The id of the new hand.
		 * @param	pPosition	[in]	The position in which the hand was created.
		 * @param	fTime		[in]	Timestamp, in seconds.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* HandCreate)(HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);
		/**
		 * Callback for a new position of an already-existing hand.
		 *
		 * @param	generator	[in]	the generator that raised this event.
		 * @param	user		[in]	The id of the hand that moved.
		 * @param	pPosition	[in]	The new position of the relevant hand.
		 * @param	fTime		[in]	Timestamp, in seconds.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* HandUpdate)(HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);
		/**
		 * Callback for the disappearance of a hand.
		 *
		 * @param	generator	[in]	the generator that raised this event.
		 * @param	user		[in]	The id of the hand that disappeared.
		 * @param	fTime		[in]	Timestamp, in seconds.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* HandDestroy)(HandsGenerator& generator, XnUserID user, XnFloat fTime, void* pCookie);

		/** @copybrief xnRegisterHandCallbacks
		 * For full details and usage, see @ref xnRegisterHandCallbacks
		 */
		inline XnStatus RegisterHandCallbacks(HandCreate CreateCB, HandUpdate UpdateCB, HandDestroy DestroyCB, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			HandCookie* pHandCookie;
			XN_VALIDATE_ALLOC(pHandCookie, HandCookie);
			pHandCookie->createHandler = CreateCB;
			pHandCookie->updateHandler = UpdateCB;
			pHandCookie->destroyHandler = DestroyCB;
			pHandCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterHandCallbacks(m_hNode, HandCreateCB, HandUpdateCB, HandDestroyCB, pHandCookie, &pHandCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pHandCookie);
				return (nRetVal);
			}

			hCallback = pHandCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterHandCallbacks
		 * For full details and usage, see @ref xnUnregisterHandCallbacks
		 */
		inline void UnregisterHandCallbacks(XnCallbackHandle hCallback)
		{
			HandCookie* pHandCookie = (HandCookie*)hCallback;
			xnUnregisterHandCallbacks(m_hNode, pHandCookie->hCallback);
			xnOSFree(pHandCookie);
		}

		/** @copybrief xnStopTracking
		 * For full details and usage, see @ref xnStopTracking
		 */
		inline XnStatus StopTracking(XnUserID user)
		{
			return xnStopTracking(m_hNode, user);
		}

		/** @copybrief xnStopTrackingAll
		 * For full details and usage, see @ref xnStopTrackingAll
		 */
		inline XnStatus StopTrackingAll()
		{
			return xnStopTrackingAll(m_hNode);
		}

		/** @copybrief xnStartTracking
		 * For full details and usage, see @ref xnStartTracking
		 */
		inline XnStatus StartTracking(const XnPoint3D& ptPosition)
		{
			return xnStartTracking(m_hNode, &ptPosition);
		}

		/** @copybrief xnSetTrackingSmoothing
		 * For full details and usage, see @ref xnSetTrackingSmoothing
		 */
		inline XnStatus SetSmoothing(XnFloat fSmoothingFactor)
		{
			return xnSetTrackingSmoothing(m_hNode, fSmoothingFactor);
		}

	private:
		typedef struct HandCookie
		{
			HandCreate createHandler;
			HandUpdate updateHandler;
			HandDestroy destroyHandler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} HandCookie;

		static void XN_CALLBACK_TYPE HandCreateCB(XnNodeHandle hNode, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
		{
			HandCookie* pHandCookie = (HandCookie*)pCookie;
			HandsGenerator gen(hNode);
			pHandCookie->createHandler(gen, user, pPosition, fTime, pHandCookie->pUserCookie);
		}
		static void XN_CALLBACK_TYPE HandUpdateCB(XnNodeHandle hNode, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
		{
			HandCookie* pHandCookie = (HandCookie*)pCookie;
			HandsGenerator gen(hNode);
			pHandCookie->updateHandler(gen, user, pPosition, fTime, pHandCookie->pUserCookie);
		}
		static void XN_CALLBACK_TYPE HandDestroyCB(XnNodeHandle hNode, XnUserID user, XnFloat fTime, void* pCookie)
		{
			HandCookie* pHandCookie = (HandCookie*)pCookie;
			HandsGenerator gen(hNode);
			pHandCookie->destroyHandler(gen, user, fTime, pHandCookie->pUserCookie);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Skeleton</b> capability functions. see @ref skeleton for additional details.
	 */
	class SkeletonCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline SkeletonCapability(XnNodeHandle hNode) : Capability(hNode) {}

		/** @copybrief xnIsJointAvailable
		 * For full details and usage, see @ref xnIsJointAvailable
		 */
		inline XnBool IsJointAvailable(XnSkeletonJoint eJoint) const
		{
			return xnIsJointAvailable(m_hNode, eJoint);
		}

		/** @copybrief xnIsProfileAvailable
		 * For full details and usage, see @ref xnIsProfileAvailable
		 */
		inline XnBool IsProfileAvailable(XnSkeletonProfile eProfile) const
		{
			return xnIsProfileAvailable(m_hNode, eProfile);
		}

		/** @copybrief xnSetSkeletonProfile
		 * For full details and usage, see @ref xnSetSkeletonProfile
		 */
		inline XnStatus SetSkeletonProfile(XnSkeletonProfile eProfile)
		{
			return xnSetSkeletonProfile(m_hNode, eProfile);
		}

		/** @copybrief xnSetJointActive
		 * For full details and usage, see @ref xnSetJointActive
		 */
		inline XnStatus SetJointActive(XnSkeletonJoint eJoint, XnBool bState)
		{
			return xnSetJointActive(m_hNode, eJoint, bState);
		}

		/** @copybrief xnIsJointActive
		 * For full details and usage, see @ref xnIsJointActive
		 */
		inline XnBool IsJointActive(XnSkeletonJoint eJoint, XnBool bState)
		{
			return xnIsJointActive(m_hNode, eJoint);
		}

		/** @copybrief xnRegisterToJointConfigurationChange
		 * For full details and usage, see @ref xnRegisterToJointConfigurationChange
		 */
		inline XnStatus RegisterToJointConfigurationChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToJointConfigurationChange, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromJointConfigurationChange
		 * For full details and usage, see @ref xnUnregisterFromJointConfigurationChange
		 */
		inline void UnregisterFromJointConfigurationChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromJointConfigurationChange, m_hNode, hCallback);
		}

		/** @copybrief xnEnumerateActiveJoints
		 * For full details and usage, see @ref xnEnumerateActiveJoints
		 */
		inline XnStatus EnumerateActiveJoints(XnSkeletonJoint* pJoints, XnUInt16& nJoints)
		{
			return xnEnumerateActiveJoints(m_hNode, pJoints, &nJoints);
		}

		/** @copybrief xnGetSkeletonJoint
		 * For full details and usage, see @ref xnGetSkeletonJoint
		 */
		inline XnStatus GetSkeletonJoint(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation& Joint) const
		{
			return xnGetSkeletonJoint(m_hNode, user, eJoint, &Joint);
		}

		/** @copybrief xnGetSkeletonJointPosition
		 * For full details and usage, see @ref xnGetSkeletonJointPosition
		 */
		inline XnStatus GetSkeletonJointPosition(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition& Joint) const
		{
			return xnGetSkeletonJointPosition(m_hNode, user, eJoint, &Joint);
		}

		/** @copybrief xnGetSkeletonJointOrientation
		 * For full details and usage, see @ref xnGetSkeletonJointOrientation
		 */
		inline XnStatus GetSkeletonJointOrientation(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation& Joint) const
		{
			return xnGetSkeletonJointOrientation(m_hNode, user, eJoint, &Joint);
		}

		/** @copybrief xnIsSkeletonTracking
		 * For full details and usage, see @ref xnIsSkeletonTracking
		 */
		inline XnBool IsTracking(XnUserID user)
		{
			return xnIsSkeletonTracking(m_hNode, user);
		}

		/** @copybrief xnIsSkeletonCalibrated
		 * For full details and usage, see @ref xnIsSkeletonCalibrated
		 */
		inline XnBool IsCalibrated(XnUserID user)
		{
			return xnIsSkeletonCalibrated(m_hNode, user);
		}

		/** @copybrief xnIsSkeletonCalibrating
		 * For full details and usage, see @ref xnIsSkeletonCalibrating
		 */
		inline XnBool IsCalibrating(XnUserID user)
		{
			return xnIsSkeletonCalibrating(m_hNode, user);
		}

		/** @copybrief xnRequestSkeletonCalibration
		 * For full details and usage, see @ref xnRequestSkeletonCalibration
		 */
		inline XnStatus RequestCalibration(XnUserID user, XnBool bForce)
		{
			return xnRequestSkeletonCalibration(m_hNode, user, bForce);
		}

		/** @copybrief xnAbortSkeletonCalibration
		 * For full details and usage, see @ref xnAbortSkeletonCalibration
		 */
		inline XnStatus AbortCalibration(XnUserID user)
		{
			return xnAbortSkeletonCalibration(m_hNode, user);
		}

		/** @copybrief xnSaveSkeletonCalibrationData
		 * For full details and usage, see @ref xnSaveSkeletonCalibrationData
		 */
		inline XnStatus SaveCalibrationData(XnUserID user, XnUInt32 nSlot)
		{
			return xnSaveSkeletonCalibrationData(m_hNode, user, nSlot);
		}

		/** @copybrief xnLoadSkeletonCalibrationData
		 * For full details and usage, see @ref xnLoadSkeletonCalibrationData
		 */
		inline XnStatus LoadCalibrationData(XnUserID user, XnUInt32 nSlot)
		{
			return xnLoadSkeletonCalibrationData(m_hNode, user, nSlot);
		}

		/** @copybrief xnClearSkeletonCalibrationData
		 * For full details and usage, see @ref xnClearSkeletonCalibrationData
		 */
		inline XnStatus ClearCalibrationData(XnUInt32 nSlot)
		{
			return xnClearSkeletonCalibrationData(m_hNode, nSlot);
		}

		/** @copybrief xnIsSkeletonCalibrationData
		 * For full details and usage, see @ref xnIsSkeletonCalibrationData
		 */
		inline XnBool IsCalibrationData(XnUInt32 nSlot)
		{
			return xnIsSkeletonCalibrationData(m_hNode, nSlot);
		}

		/** @copybrief xnStartSkeletonTracking
		 * For full details and usage, see @ref xnStartSkeletonTracking
		 */
		inline XnStatus StartTracking(XnUserID user)
		{
			return xnStartSkeletonTracking(m_hNode, user);
		}

		/** @copybrief xnStopSkeletonTracking
		 * For full details and usage, see @ref xnStopSkeletonTracking
		 */
		inline XnStatus StopTracking(XnUserID user)
		{
			return xnStopSkeletonTracking(m_hNode, user);
		}

		/** @copybrief xnResetSkeleton
		 * For full details and usage, see @ref xnResetSkeleton
		 */
		inline XnStatus Reset(XnUserID user)
		{
			return xnResetSkeleton(m_hNode, user);
		}

		/** @copybrief xnNeedPoseForSkeletonCalibration
		 * For full details and usage, see @ref xnNeedPoseForSkeletonCalibration
		 */
		inline XnBool NeedPoseForCalibration()
		{
			return xnNeedPoseForSkeletonCalibration(m_hNode);
		}

		/** @copybrief xnGetSkeletonCalibrationPose
		 * For full details and usage, see @ref xnGetSkeletonCalibrationPose
		 */
		inline XnStatus GetCalibrationPose(XnChar* strPose)
		{
			return xnGetSkeletonCalibrationPose(m_hNode, strPose);
		}

		/** @copybrief xnSetSkeletonSmoothing
		 * For full details and usage, see @ref xnSetSkeletonSmoothing
		 */
		inline XnStatus SetSmoothing(XnFloat fSmoothingFactor)
		{
			return xnSetSkeletonSmoothing(m_hNode, fSmoothingFactor);
		}

		/**
		 * Callback for indication that a specific user's skeleton is now starting the calibration process
		 * 
		 * @param	skeleton	[in]	The node that raised the event.
		 * @param	user		[in]	The id of the user that's being calibrated.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationStart)(SkeletonCapability& skeleton, XnUserID user, void* pCookie);
		/**
		 * Callback for indication that a specific user's skeleton has now completed the calibration process
		 * 
		 * @param	hNode		[in]	The node that raised the event.
		 * @param	user		[in]	The id of the user for which calibration was attempted.
		 * @param	bSuccess	[in]	An indication of whether or not the calibration attempt succeeded.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationEnd)(SkeletonCapability& skeleton, XnUserID user, XnBool bSuccess, void* pCookie);

		/** @copybrief xnRegisterCalibrationCallbacks
		 * For full details and usage, see @ref xnRegisterCalibrationCallbacks
		 */
		inline XnStatus RegisterCalibrationCallbacks(CalibrationStart CalibrationStartCB, CalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			SkeletonCookie* pSkeletonCookie;
			XN_VALIDATE_ALLOC(pSkeletonCookie, SkeletonCookie);
			pSkeletonCookie->startHandler = CalibrationStartCB;
			pSkeletonCookie->endHandler = CalibrationEndCB;
			pSkeletonCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterCalibrationCallbacks(m_hNode, CalibrationStartCallback, CalibrationEndCallback, pSkeletonCookie, &pSkeletonCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pSkeletonCookie);
				return (nRetVal);
			}

			hCallback = pSkeletonCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterCalibrationCallbacks
		 * For full details and usage, see @ref xnUnregisterCalibrationCallbacks
		 */
		inline void UnregisterCalibrationCallbacks(XnCallbackHandle hCallback)
		{
			SkeletonCookie* pSkeletonCookie = (SkeletonCookie*)hCallback;
			xnUnregisterCalibrationCallbacks(m_hNode, pSkeletonCookie->hCallback);
			xnOSFree(pSkeletonCookie);
		}

	private:
		typedef struct SkeletonCookie
		{
			CalibrationStart startHandler;
			CalibrationEnd endHandler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} SkeletonCookie;

		static void XN_CALLBACK_TYPE CalibrationStartCallback(XnNodeHandle hNode, XnUserID user, void* pCookie)
		{
			SkeletonCookie* pGestureCookie = (SkeletonCookie*)pCookie;
			SkeletonCapability cap(hNode);
			pGestureCookie->startHandler(cap, user, pGestureCookie->pUserCookie);
		}

		static void XN_CALLBACK_TYPE CalibrationEndCallback(XnNodeHandle hNode, XnUserID user, XnBool bSuccess, void* pCookie)
		{
			SkeletonCookie* pGestureCookie = (SkeletonCookie*)pCookie;
			SkeletonCapability cap(hNode);
			pGestureCookie->endHandler(cap, user, bSuccess, pGestureCookie->pUserCookie);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Pose Detection</b> capability functions. see @ref pose for additional details.
	 */
	class PoseDetectionCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline PoseDetectionCapability(XnNodeHandle hNode) : Capability(hNode) {}

		/**
		 * Callback for indication that a specific user has entered a pose, or left it.
		 *
		 * @param	pose		[in]	The node that raised the event.
		 * @param	strPose		[in]	The pose that is relevant to the callback.
		 * @param	user		[in]	The id of the user that entered the pose or left it.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* PoseDetection)(PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, void* pCookie);

		/** @copybrief xnGetNumberOfPoses
		 * For full details and usage, see @ref xnGetNumberOfPoses
		 */
		inline XnUInt32 GetNumberOfPoses()
		{
			return xnGetNumberOfPoses(m_hNode);
		}

		/** @copybrief xnGetAvailablePoses
		 * For full details and usage, see @ref xnGetAvailablePoses
		 */
		inline XnStatus GetAvailablePoses(XnChar** pstrPoses, XnUInt32& nPoses)
		{
			return xnGetAvailablePoses(m_hNode, pstrPoses, &nPoses);
		}

		/** @copybrief xnStartPoseDetection
		 * For full details and usage, see @ref xnStartPoseDetection
		 */
		inline XnStatus StartPoseDetection(const XnChar* strPose, XnUserID user)
		{
			return xnStartPoseDetection(m_hNode, strPose, user);
		}

		/** @copybrief xnStopPoseDetection
		 * For full details and usage, see @ref xnStopPoseDetection
		 */
		inline XnStatus StopPoseDetection(XnUserID user)
		{
			return xnStopPoseDetection(m_hNode, user);
		}

		/** @copybrief xnRegisterToPoseCallbacks
		 * For full details and usage, see @ref xnRegisterToPoseCallbacks
		 */
		inline XnStatus RegisterToPoseCallbacks(PoseDetection PoseStartCB, PoseDetection PoseEndCB, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			PoseCookie* pPoseCookie;
			XN_VALIDATE_ALLOC(pPoseCookie, PoseCookie);
			pPoseCookie->startHandler = PoseStartCB;
			pPoseCookie->endHandler = PoseEndCB;
			pPoseCookie->pPoseCookie = pCookie;

			nRetVal = xnRegisterToPoseCallbacks(m_hNode, PoseDetectionStartCallback, PoseDetectionStartEndCallback, pPoseCookie, &pPoseCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pPoseCookie);
				return (nRetVal);
			}

			hCallback = pPoseCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterCalibrationCallbacks
		 * For full details and usage, see @ref xnUnregisterCalibrationCallbacks
		 */
		inline void UnregisterFromPoseCallbacks(XnCallbackHandle hCallback)
		{
			PoseCookie* pPoseCookie = (PoseCookie*)hCallback;
			xnUnregisterCalibrationCallbacks(m_hNode, pPoseCookie->hCallback);
			xnOSFree(pPoseCookie);
		}
	private:
		typedef struct PoseCookie
		{
			PoseDetection startHandler;
			PoseDetection endHandler;
			void* pPoseCookie;
			XnCallbackHandle hCallback;
		} PoseCookie;

		static void XN_CALLBACK_TYPE PoseDetectionStartCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, void* pCookie)
		{
			PoseCookie* pPoseCookie = (PoseCookie*)pCookie;
			PoseDetectionCapability cap(hNode);
			pPoseCookie->startHandler(cap, strPose, user, pPoseCookie->pPoseCookie);
		}

		static void XN_CALLBACK_TYPE PoseDetectionStartEndCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, void* pCookie)
		{
			PoseCookie* pPoseCookie = (PoseCookie*)pCookie;
			PoseDetectionCapability cap(hNode);
			pPoseCookie->endHandler(cap, strPose, user, pPoseCookie->pPoseCookie);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a User Generator node
	 */
	class UserGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline UserGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}

		/** @copybrief xnCreateUserGenerator
		 * For full details and usage, see @ref xnCreateUserGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		typedef void (XN_CALLBACK_TYPE* UserHandler)(UserGenerator& generator, XnUserID user, void* pCookie);

		/** @copybrief xnGetNumberOfUsers
		 * For full details and usage, see @ref xnGetNumberOfUsers
		 */
		inline XnUInt16 GetNumberOfUsers() const
		{
			return xnGetNumberOfUsers(m_hNode);
		}

		/** @copybrief xnGetUsers
		 * For full details and usage, see @ref xnGetUsers
		 */
		inline XnStatus GetUsers(XnUserID aUsers[], XnUInt16& nUsers) const
		{
			return xnGetUsers(m_hNode, aUsers, &nUsers);
		}

		/** @copybrief xnGetUserCoM
		 * For full details and usage, see @ref xnGetUserCoM
		 */
		inline XnStatus GetCoM(XnUserID user, XnPoint3D& com) const
		{
			return xnGetUserCoM(m_hNode, user, &com);
		}

		/** @copybrief xnGetUserPixels
		 * For full details and usage, see @ref xnGetUserPixels
		 */
		inline XnStatus GetUserPixels(XnUserID user, SceneMetaData& smd)
		{
			return xnGetUserPixels(m_hNode, user, smd.GetUnderlying());
		}
		
		/** @copybrief xnRegisterUserCallbacks
		 * For full details and usage, see @ref xnRegisterUserCallbacks
		 */
		inline XnStatus RegisterUserCallbacks(UserHandler NewUserCB, UserHandler LostUserCB, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			UserCookie* pUserCookie;
			XN_VALIDATE_ALLOC(pUserCookie, UserCookie);
			pUserCookie->newHandler = NewUserCB;
			pUserCookie->lostHandler = LostUserCB;
			pUserCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterUserCallbacks(m_hNode, NewUserCallback, LostUserCallback, pUserCookie, &pUserCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pUserCookie);
				return (nRetVal);
			}

			hCallback = pUserCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterUserCallbacks
		 * For full details and usage, see @ref xnUnregisterUserCallbacks
		 */
		inline void UnregisterUserCallbacks(XnCallbackHandle hCallback)
		{
			UserCookie* pUserCookie = (UserCookie*)hCallback;
			xnUnregisterUserCallbacks(m_hNode, pUserCookie->hCallback);
			xnOSFree(pUserCookie);
		}

		/**
		 * Gets an @ref SkeletonCapability object for accessing Skeleton functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_SKELETON is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline SkeletonCapability GetSkeletonCap()
		{
			return SkeletonCapability(m_hNode);
		}

		/**
		 * Gets an @ref PoseDetectionCapability object for accessing Pose-Detection functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_POSE_DETECTION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline PoseDetectionCapability GetPoseDetectionCap()
		{
			return PoseDetectionCapability(m_hNode);
		}

	private:
		typedef struct UserCookie
		{
			UserHandler newHandler;
			UserHandler lostHandler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} UserCookie;

		static void XN_CALLBACK_TYPE NewUserCallback(XnNodeHandle hNode, XnUserID user, void* pCookie)
		{
			UserCookie* pUserCookie = (UserCookie*)pCookie;
			UserGenerator gen(hNode);
			pUserCookie->newHandler(gen, user, pUserCookie->pUserCookie);
		}

		static void XN_CALLBACK_TYPE LostUserCallback(XnNodeHandle hNode, XnUserID user, void* pCookie)
		{
			UserCookie* pUserCookie = (UserCookie*)pCookie;
			UserGenerator gen(hNode);
			pUserCookie->lostHandler(gen, user, pUserCookie->pUserCookie);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents an Audio Generator node
	 */
	class AudioGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline AudioGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}

		/** @copybrief xnCreateAudioGenerator
		 * For full details and usage, see @ref xnCreateAudioGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetAudioMetaData
		 * For full details and usage, see @ref xnGetAudioMetaData
		 */
		inline void GetMetaData(AudioMetaData& metaData) const
		{
			xnGetAudioMetaData(m_hNode, metaData.GetUnderlying());
		}

		/** @copybrief xnGetAudioBuffer
		 * For full details and usage, see @ref xnGetAudioBuffer
		 */
		inline const XnUChar* GetAudioBuffer() const
		{
			return xnGetAudioBuffer(m_hNode);
		}

		/** @copybrief xnGetSupportedWaveOutputModesCount
		 * For full details and usage, see @ref xnGetSupportedWaveOutputModesCount
		 */
		inline XnUInt32 GetSupportedWaveOutputModesCount() const
		{
			return xnGetSupportedWaveOutputModesCount(m_hNode);
		}

		/** @copybrief xnGetSupportedWaveOutputModes
		 * For full details and usage, see @ref xnGetSupportedWaveOutputModes
		 */
		inline XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode* aSupportedModes, XnUInt32& nCount) const
		{
			return xnGetSupportedWaveOutputModes(m_hNode, aSupportedModes, &nCount);
		}

		/** @copybrief xnSetWaveOutputMode
		 * For full details and usage, see @ref xnSetWaveOutputMode
		 */
		inline XnStatus SetWaveOutputMode(const XnWaveOutputMode& OutputMode)
		{
			return xnSetWaveOutputMode(m_hNode, &OutputMode);
		}

		/** @copybrief xnGetWaveOutputMode
		 * For full details and usage, see @ref xnGetWaveOutputMode
		 */
		inline XnStatus GetWaveOutputMode(XnWaveOutputMode& OutputMode) const
		{
			return xnGetWaveOutputMode(m_hNode, &OutputMode);
		}

		/** @copybrief xnRegisterToWaveOutputModeChanges
		 * For full details and usage, see @ref xnRegisterToWaveOutputModeChanges
		 */
		inline XnStatus RegisterToWaveOutputModeChanges(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToWaveOutputModeChanges, m_hNode, handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromWaveOutputModeChanges
		 * For full details and usage, see @ref xnUnregisterFromWaveOutputModeChanges
		 */
		inline void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromWaveOutputModeChanges, m_hNode, hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a Mock Audio Generator node
	 */
	class MockAudioGenerator : public AudioGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline MockAudioGenerator(XnNodeHandle hNode = NULL) : AudioGenerator(hNode) {}

		/**
		 * Creates a mock audio generator
		 *
		 * @param	context		[in]	The context in which the node should be generated
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);

		/**
		 * Creates a mock audio generator, copying the initial state from another audio generator.
		 *
		 * @param	other		[in]	The audio generator to copy state from
		 * @param	strName		[in]	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(AudioGenerator& other, const XnChar* strName = NULL);

		/** @copybrief xnMockAudioSetData
		 * For full details and usage, see @ref xnMockAudioSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pAudioBuffer)
		{
			return xnMockAudioSetData(m_hNode, nFrameID, nTimestamp, nDataSize, pAudioBuffer);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	audioMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(AudioMetaData& audioMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, audioMD.DataSize(), audioMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object
		 *
		 * @param	audioMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(AudioMetaData& audioMD)
		{
			return SetData(audioMD, audioMD.FrameID(), audioMD.Timestamp());
		}
	};

	//---------------------------------------------------------------------------
	// Codec
	//---------------------------------------------------------------------------
	/**
	 * @ingroup cppref
	 * Represents a Codec node.
	 */
	class Codec : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline Codec(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}

		/** @copybrief xnCreateCodec
		 * For full details and usage, see @ref xnCreateCodec
		 */
		inline XnStatus Create(Context& context, XnCodecID codecID, ProductionNode& initializerNode);

		/** @copybrief xnGetCodecID
		 * For full details and usage, see @ref xnGetCodecID
		 */
		inline XnCodecID GetCodecID() const
		{
			return xnGetCodecID(m_hNode);
		}

		/** @copybrief xnEncodeData
		 * For full details and usage, see @ref xnEncodeData
		 */
		inline XnStatus EncodeData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
		{
			return xnEncodeData(m_hNode, pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
		}

		/** @copybrief xnDecodeData
		 * For full details and usage, see @ref xnDecodeData
		 */
		inline XnStatus DecodeData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
		{
			return xnDecodeData(m_hNode, pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
		}
	};

	//---------------------------------------------------------------------------
	// EnumerationErrors
	//---------------------------------------------------------------------------
	/**
	 * @ingroup cppref
	 * Represents an enumeration errors object.
	 */
	class EnumerationErrors
	{
	public:
		/// Ctor
		inline EnumerationErrors() : m_bAllocated(TRUE), m_pErrors(NULL) { xnEnumerationErrorsAllocate(&m_pErrors); }

		/**
		 * Ctor
		 *
		 * @param	pErrors		[in]	underlying C object to wrap
		 * @param	bOwn		[in]	TRUE to own the object (i.e. free it upon destruction), FALSE otherwise.
		 */
		inline EnumerationErrors(XnEnumerationErrors* pErrors, XnBool bOwn = FALSE) : m_bAllocated(bOwn), m_pErrors(pErrors) {}

		/// Dtor
		~EnumerationErrors() { Free(); }

		/// An iterator over enumeration errors
		class Iterator
		{
		public:
			friend class EnumerationErrors;

			/**
			 * Checks if the other iterator points to the same location
			 *
			 * @param	other		[in]	another iterator
			 */
			XnBool operator==(const Iterator& other) const
			{
				return m_it == other.m_it;
			}

			/**
			 * Checks if the other iterator points to a different location
			 *
			 * @param	other		[in]	another iterator
			 */
			XnBool operator!=(const Iterator& other) const
			{
				return m_it != other.m_it;
			}

			/**
			 * Moves the iterator to the next location. If end of list was reached, iterator will be
			 * equal to @ref EnumerationErrors::End().
			 */
			inline Iterator& operator++()
			{
				m_it = xnEnumerationErrorsGetNext(m_it);
				return *this;
			}

			/**
			 * Returns an iterator to the next location. If end of list was reached, iterator will be
			 * equal to @ref EnumerationErrors::End().
			 */
			inline Iterator operator++(int)
			{
				return Iterator(xnEnumerationErrorsGetNext(m_it));
			}

			/// Gets the description of the failing node this iterator points to.
			inline const XnProductionNodeDescription& Description() { return *xnEnumerationErrorsGetCurrentDescription(m_it); }
			/// Gets the failure error code  of the failing node this iterator points to.
			inline XnStatus Error() { return xnEnumerationErrorsGetCurrentError(m_it); }

		private:
			inline Iterator(XnEnumerationErrorsIterator it) : m_it(it) {}

			XnEnumerationErrorsIterator m_it;
		};

		/// Gets an iterator to the first item in the list.
		inline Iterator Begin() const { return Iterator(xnEnumerationErrorsGetFirst(m_pErrors)); } 
		/// Gets an iterator representing the end of the list. This iterator does not point to an actual item.
		inline Iterator End() const { return Iterator(NULL); } 

		/** @copybrief xnEnumerationErrorsToString
		 * For full details and usage, see @ref xnEnumerationErrorsToString
		 */
		inline XnStatus ToString(XnChar* csBuffer, XnUInt32 nSize)
		{
			return xnEnumerationErrorsToString(m_pErrors, csBuffer, nSize);
		}

		/** @copybrief xnEnumerationErrorsFree
		 * For full details and usage, see @ref xnEnumerationErrorsFree
		 */
		inline void Free()
		{
			if (m_bAllocated)
			{
				xnEnumerationErrorsFree(m_pErrors);
				m_pErrors = NULL;
				m_bAllocated = FALSE;
			}
		}

		/// Gets the underlying C object
		inline XnEnumerationErrors* GetUnderlying() { return m_pErrors; }

	private:
		XnEnumerationErrors* m_pErrors;
		XnBool m_bAllocated;
	};

	//---------------------------------------------------------------------------
	// Context
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref
	 * Represents an OpenNI context object.
	 */
	class Context
	{
	public:
		/// Ctor
		inline Context() : m_pContext(NULL), m_bAllocated(FALSE) {}
		/**
		 * Ctor
		 *
		 * @param	pContext		[in]	Underlying C object
		 */
		inline Context(XnContext* pContext) : m_pContext(pContext), m_bAllocated(FALSE) {}
		/**
		 * Copy Ctor
		 *
		 * @param	other		[in]	Another context. Note that the context will only be destroyed when
		 *								The original object is destroyed.
		 */
		inline Context(const Context& other) : m_pContext(other.m_pContext), m_bAllocated(FALSE) {}

		/// Dtor
		~Context() 
		{ 
			FreeImpl();
		}

		/// Gets the underlying C object
		inline XnContext* GetUnderlyingObject() const { return m_pContext; }

		/** @copybrief xnInit
		 * For full details and usage, see @ref xnInit
		 */
		inline XnStatus Init()
		{
			XnStatus nRetVal = xnInit(&m_pContext);
			XN_IS_STATUS_OK(nRetVal);
			m_bAllocated = TRUE;
			return (XN_STATUS_OK);
		}

		/** @copybrief xnContextRunXmlScript
		 * For full details and usage, see @ref xnContextRunXmlScript
		 */
		inline XnStatus RunXmlScript(const XnChar* strScript, EnumerationErrors* pErrors = NULL)
		{
			return xnContextRunXmlScript(m_pContext, strScript, pErrors == NULL ? NULL : pErrors->GetUnderlying());
		}

		/** @copybrief xnContextRunXmlScriptFromFile
		 * For full details and usage, see @ref xnContextRunXmlScriptFromFile
		 */
		inline XnStatus RunXmlScriptFromFile(const XnChar* strFileName, EnumerationErrors* pErrors = NULL)
		{
			return xnContextRunXmlScriptFromFile(m_pContext, strFileName, pErrors == NULL ? NULL : pErrors->GetUnderlying());
		}

		/** @copybrief xnInitFromXmlFile
		 * For full details and usage, see @ref xnInitFromXmlFile
		 */
		inline XnStatus InitFromXmlFile(const XnChar* strFileName, EnumerationErrors* pErrors = NULL)
		{
			XnStatus nRetVal = xnInitFromXmlFile(strFileName, &m_pContext, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			XN_IS_STATUS_OK(nRetVal);
			m_bAllocated = TRUE;
			return (XN_STATUS_OK);
		}

		/** @copybrief xnContextOpenFileRecording
		 * For full details and usage, see @ref xnContextOpenFileRecording
		 */
		inline XnStatus OpenFileRecording(const XnChar* strFileName)
		{
			return xnContextOpenFileRecording(m_pContext, strFileName);
		}

		/** @copybrief xnCreateMockNode
		 * For full details and usage, see @ref xnCreateMockNode
		 */
		inline XnStatus CreateMockNode(XnProductionNodeType type, const XnChar* strName, ProductionNode& node)
		{
			return xnCreateMockNode(m_pContext, type, strName, &node.m_hNode);
		}

		/** @copybrief xnCreateMockNodeBasedOn
		 * For full details and usage, see @ref xnCreateMockNodeBasedOn
		 */
		inline XnStatus CreateMockNodeBasedOn(ProductionNode& originalNode, const XnChar* strName, ProductionNode& mockNode)
		{
			return xnCreateMockNodeBasedOn(m_pContext, originalNode.m_hNode, strName, &mockNode.m_hNode);
		}

		/** @copybrief xnCreateCodec
		 * For full details and usage, see @ref xnCreateCodec
		 */
		inline XnStatus CreateCodec(XnCodecID codecID, ProductionNode& initializerNode, Codec& codec)
		{
			return xnCreateCodec(m_pContext, codecID, initializerNode, &codec.m_hNode);
		}

		/** @copybrief xnShutdown
		 * For full details and usage, see @ref xnShutdown
		 */
		inline void Shutdown()
		{
			if (m_pContext != NULL)
			{
				xnShutdown(m_pContext);
				m_pContext = NULL;
			}
		}

		/** @copybrief xnAddLicense
		 * For full details and usage, see @ref xnAddLicense
		 */
		inline XnStatus AddLicense(const XnLicense& License)
		{
			return xnAddLicense(m_pContext, &License);
		}

		/** @copybrief xnEnumerateLicenses
		 * For full details and usage, see @ref xnEnumerateLicenses
		 */
		inline XnStatus EnumerateLicenses(XnLicense*& aLicenses, XnUInt32& nCount) const
		{
			return xnEnumerateLicenses(m_pContext, &aLicenses, &nCount);
		}

		/** @copybrief xnFreeLicensesList
		 * For full details and usage, see @ref xnFreeLicensesList
		 */
		inline static void FreeLicensesList(XnLicense aLicenses[])
		{
			return xnFreeLicensesList(aLicenses);
		}

		/** @copybrief xnEnumerateProductionTrees
		 * For full details and usage, see @ref xnEnumerateProductionTrees
		 */
		XnStatus EnumerateProductionTrees(XnProductionNodeType Type, Query* pQuery, NodeInfoList& TreesList, EnumerationErrors* pErrors = NULL) const
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeQuery* pInternalQuery = (pQuery != NULL) ? pQuery->GetUnderlyingObject() : NULL;

			XnNodeInfoList* pList = NULL;
			nRetVal = xnEnumerateProductionTrees(m_pContext, Type, pInternalQuery, &pList, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			XN_IS_STATUS_OK(nRetVal);

			TreesList.ReplaceUnderlyingObject(pList);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnCreateAnyProductionTree
		 * For full details and usage, see @ref xnCreateAnyProductionTree
		 */
		XnStatus CreateAnyProductionTree(XnProductionNodeType type, Query* pQuery, ProductionNode& node, EnumerationErrors* pErrors = NULL)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			XnNodeQuery* pInternalQuery = (pQuery != NULL) ? pQuery->GetUnderlyingObject() : NULL;

			XnNodeHandle hNode;
			nRetVal = xnCreateAnyProductionTree(m_pContext, type, pInternalQuery, &hNode, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			XN_IS_STATUS_OK(nRetVal);

			node.SetHandle(hNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnCreateProductionTree
		 * For full details and usage, see @ref xnCreateProductionTree
		 */
		XnStatus CreateProductionTree(NodeInfo& Tree)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnCreateProductionTree(m_pContext, Tree, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnEnumerateExistingNodes
		 * For full details and usage, see @ref xnEnumerateExistingNodes
		 */
		XnStatus EnumerateExistingNodes(NodeInfoList& list) const
		{
			XnNodeInfoList* pList;
			XnStatus nRetVal = xnEnumerateExistingNodes(m_pContext, &pList);
			XN_IS_STATUS_OK(nRetVal);

			list.ReplaceUnderlyingObject(pList);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnEnumerateExistingNodesByType
		 * For full details and usage, see @ref xnEnumerateExistingNodesByType
		 */
		XnStatus EnumerateExistingNodes(NodeInfoList& list, XnProductionNodeType type) const
		{
			XnNodeInfoList* pList;
			XnStatus nRetVal = xnEnumerateExistingNodesByType(m_pContext, type, &pList);
			XN_IS_STATUS_OK(nRetVal);

			list.ReplaceUnderlyingObject(pList);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnFindExistingNodeByType
		 * For full details and usage, see @ref xnFindExistingNodeByType
		 */
		XnStatus FindExistingNode(XnProductionNodeType type, ProductionNode& node) const
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnFindExistingNodeByType(m_pContext, type, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			node.SetHandle(hNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnGetNodeHandleByName
		 * For full details and usage, see @ref xnGetNodeHandleByName
		 */
		XnStatus GetProductionNodeByName(const XnChar* strInstanceName, ProductionNode& node) const
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnGetNodeHandleByName(m_pContext, strInstanceName, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			node.SetHandle(hNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnGetNodeHandleByName
		 * For full details and usage, see @ref xnGetNodeHandleByName
		 */
		XnStatus GetProductionNodeInfoByName(const XnChar* strInstanceName, NodeInfo& nodeInfo) const
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnGetNodeHandleByName(m_pContext, strInstanceName, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			nodeInfo = NodeInfo(xnGetNodeInfo(hNode));

			return (XN_STATUS_OK);
		}

		/** @copybrief xnStartGeneratingAll
		 * For full details and usage, see @ref xnStartGeneratingAll
		 */
		inline XnStatus StartGeneratingAll()
		{
			return xnStartGeneratingAll(m_pContext);
		}

		/** @copybrief xnStopGeneratingAll
		 * For full details and usage, see @ref xnStopGeneratingAll
		 */
		inline XnStatus StopGeneratingAll()
		{
			return xnStopGeneratingAll(m_pContext);
		}

		/** @copybrief xnSetGlobalMirror
		 * For full details and usage, see @ref xnSetGlobalMirror
		 */
		inline XnStatus SetGlobalMirror(XnBool bMirror)
		{
			return xnSetGlobalMirror(m_pContext, bMirror);
		}

		/** @copybrief xnGetGlobalMirror
		 * For full details and usage, see @ref xnGetGlobalMirror
		 */
		inline XnBool GetGlobalMirror()
		{
			return xnGetGlobalMirror(m_pContext);
		}

		/** @copybrief xnGetGlobalErrorState
		 * For full details and usage, see @ref xnGetGlobalErrorState
		 */
		inline XnStatus GetGlobalErrorState()
		{
			return xnGetGlobalErrorState(m_pContext);
		}

		/** @copybrief xnRegisterToGlobalErrorStateChange
		 * For full details and usage, see @ref xnRegisterToGlobalErrorStateChange
		 */
		inline XnStatus RegisterToErrorStateChange(XnErrorStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return xnRegisterToGlobalErrorStateChange(m_pContext, handler, pCookie, &hCallback);
		}

		/** @copybrief xnUnregisterFromGlobalErrorStateChange
		 * For full details and usage, see @ref xnUnregisterFromGlobalErrorStateChange
		 */
		inline void UnregisterFromErrorStateChange(XnCallbackHandle hCallback)
		{
			return xnUnregisterFromGlobalErrorStateChange(m_pContext, hCallback);
		}

		/** @copybrief xnWaitAndUpdateAll
		 * For full details and usage, see @ref xnWaitAndUpdateAll
		 */
		inline XnStatus WaitAndUpdateAll()
		{
			return xnWaitAndUpdateAll(m_pContext);
		}

		/** @copybrief xnWaitAnyUpdateAll
		 * For full details and usage, see @ref xnWaitAnyUpdateAll
		 */
		inline XnStatus WaitAnyUpdateAll()
		{
			return xnWaitAnyUpdateAll(m_pContext);
		}

		/** @copybrief xnWaitOneUpdateAll
		 * For full details and usage, see @ref xnWaitOneUpdateAll
		 */
		inline XnStatus WaitOneUpdateAll(ProductionNode& node)
		{
			return xnWaitOneUpdateAll(m_pContext, node);
		}

		/** @copybrief xnWaitNoneUpdateAll
		 * For full details and usage, see @ref xnWaitNoneUpdateAll
		 */
		inline XnStatus WaitNoneUpdateAll()
		{
			return xnWaitNoneUpdateAll(m_pContext);
		}

		/** @copybrief xnAutoEnumerateOverSingleInput
		 * For full details and usage, see @ref xnAutoEnumerateOverSingleInput
		 */
		inline XnStatus AutoEnumerateOverSingleInput(NodeInfoList& List, XnProductionNodeDescription& description, const XnChar* strCreationInfo, XnProductionNodeType InputType, EnumerationErrors* pErrors, Query* pQuery = NULL) const
		{
			return xnAutoEnumerateOverSingleInput(m_pContext, List.GetUnderlyingObject(), &description, strCreationInfo, InputType, pErrors == NULL ? NULL : pErrors->GetUnderlying(), pQuery == NULL ? NULL : pQuery->GetUnderlyingObject());
		}

		/// Replaces the underlying C object pointed to by this object.
		inline void SetHandle(XnContext* pContext)
		{
			if (m_pContext != pContext)
			{
				FreeImpl();
				m_pContext = pContext;
			}
		}

	private:
		void FreeImpl()
		{
			if (m_bAllocated)
			{
				Shutdown();
				m_pContext = NULL;
				m_bAllocated = FALSE;
			}
		}

		XnContext* m_pContext;
		XnBool m_bAllocated;
	};

	/**
	 * @ingroup cppref
	 * A utility class for resolution info
	 */
	class Resolution
	{
	public:
		/**
		 * Creates a @ref Resolution object.
		 *
		 * @param	res		[in]	Resolution
		 */
		inline Resolution(XnResolution res) : m_Res(res)
		{
			m_nXRes = xnResolutionGetXRes(res);
			m_nYRes = xnResolutionGetYRes(res);
			m_strName = xnResolutionGetName(res);
		}

		/**
		 * Creates a @ref Resolution object out of X and Y resolutions.
		 *
		 * @param	xRes		[in]	X resolution
		 * @param	yRes		[in]	Y resolution
		 */
		inline Resolution(XnUInt32 xRes, XnUInt32 yRes) : m_nXRes(xRes), m_nYRes(yRes)
		{
			m_Res = xnResolutionGetFromXYRes(xRes, yRes);
			m_strName = xnResolutionGetName(m_Res);
		}

		/**
		 * Creates a @ref Resolution object out of its name
		 *
		 * @param	strName		[in]	name of the resolution
		 */
		inline Resolution(const XnChar* strName)
		{
			m_Res = xnResolutionGetFromName(strName);
			m_nXRes = xnResolutionGetXRes(m_Res);
			m_nYRes = xnResolutionGetYRes(m_Res);
			m_strName = xnResolutionGetName(m_Res);
		}

		/// Gets the resolution
		inline XnResolution GetResolution() const { return m_Res; }
		/// Gets the X resolution
		inline XnUInt32 GetXResolution() const { return m_nXRes; }
		/// Gets the Y resolution
		inline XnUInt32 GetYResolution() const { return m_nYRes; }
		/// Gets the name of the resolution
		inline const XnChar* GetName() const { return m_strName; }

	private:
		XnResolution m_Res;
		XnUInt32 m_nXRes;
		XnUInt32 m_nYRes;
		const XnChar* m_strName;
	};

	//---------------------------------------------------------------------------
	// Functions Implementation
	//---------------------------------------------------------------------------
	inline XnStatus NodeInfoList::FilterList(Context& context, Query& query)
	{
		return xnNodeQueryFilterList(context.GetUnderlyingObject(), query.GetUnderlyingObject(), m_pList);
	}

	inline void ProductionNode::GetContext(Context& context)
	{
		context.SetHandle(xnGetContextFromNodeHandle(m_hNode));
	}

	inline NodeInfoList& NodeInfo::GetNeededNodes() const
	{
		if (m_pNeededNodes == NULL)
		{
			XnNodeInfoList* pList = xnNodeInfoGetNeededNodes(m_pInfo);
			m_pNeededNodes = XN_NEW(NodeInfoList, pList);
		}

		return *m_pNeededNodes;
	}

	inline void NodeInfo::SetUnderlyingObject(XnNodeInfo* pInfo)
	{
		if (m_pNeededNodes != NULL)
		{
			XN_DELETE(m_pNeededNodes);
		}

		m_pInfo = pInfo;
		m_pNeededNodes = NULL;
	}

	inline XnBool FrameSyncCapability::CanFrameSyncWith(Generator& other)
	{
		return xnCanFrameSyncWith(m_hNode, other);
	}

	inline XnStatus FrameSyncCapability::FrameSyncWith(Generator& other)
	{
		return xnFrameSyncWith(m_hNode, other);
	}

	inline XnStatus FrameSyncCapability::StopFrameSyncWith(Generator& other)
	{
		return xnStopFrameSyncWith(m_hNode, other);
	}

	inline XnBool FrameSyncCapability::IsFrameSyncedWith(Generator& other)
	{
		return xnIsFrameSyncedWith(m_hNode, other);
	}

	inline XnStatus NodeInfo::GetInstance(ProductionNode& node) const
	{
		XnStatus nRetVal = XN_STATUS_OK;

		if (m_pInfo == NULL)
		{
			return XN_STATUS_INVALID_OPERATION;
		}

		node.SetHandle(xnNodeInfoGetHandle(m_pInfo));

		return (XN_STATUS_OK);
	}



	//---------------------------------------------------------------------------
	// Node creation functions
	//---------------------------------------------------------------------------

	inline XnStatus Recorder::Create(Context& context, const XnChar* strFormatName = NULL)
	{
		return xnCreateRecorder(context.GetUnderlyingObject(), strFormatName, &m_hNode);
	}

	inline XnStatus Player::Create(Context& context, const XnChar* strFormatName)
	{
		return xnCreatePlayer(context.GetUnderlyingObject(), strFormatName, &m_hNode);
	}

	inline XnStatus DepthGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		return xnCreateDepthGenerator(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus MockDepthGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		return xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_DEPTH, strName, &m_hNode);
	}

	inline XnStatus MockDepthGenerator::CreateBasedOn(DepthGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		return xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other, strName, &m_hNode);
	}

	inline XnStatus ImageGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		return xnCreateImageGenerator(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus MockImageGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		return xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_IMAGE, strName, &m_hNode);
	}

	inline XnStatus MockImageGenerator::CreateBasedOn(ImageGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		return xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other, strName, &m_hNode);
	}

	inline XnStatus IRGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		return xnCreateIRGenerator(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}
	
	inline XnStatus MockIRGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		return xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_IR, strName, &m_hNode);
	}

	inline XnStatus MockIRGenerator::CreateBasedOn(IRGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		return xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other, strName, &m_hNode);
	}

	inline XnStatus GestureGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		return xnCreateGestureGenerator(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus SceneAnalyzer::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		//You're creating a scene!
		return xnCreateSceneAnalyzer(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus HandsGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		return xnCreateHandsGenerator(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus UserGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		return xnCreateUserGenerator(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus AudioGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		return xnCreateAudioGenerator(context.GetUnderlyingObject(), &m_hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus MockAudioGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		return xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_AUDIO, strName, &m_hNode);
	}

	inline XnStatus MockAudioGenerator::CreateBasedOn(AudioGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		return xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other, strName, &m_hNode);
	}

	inline XnStatus Codec::Create(Context& context, XnCodecID codecID, ProductionNode& initializerNode)
	{
		return xnCreateCodec(context.GetUnderlyingObject(), codecID, initializerNode, &m_hNode);
	}

	//---------------------------------------------------------------------------
	// Global Helper Functions
	//---------------------------------------------------------------------------

	/** @copybrief xnGetVersion
	 * For full details and usage, see @ref xnGetVersion
	 */
	inline void GetVersion(XnVersion& Version)
	{
		xnGetVersion(&Version);
	}

	//---------------------------------------------------------------------------
	// Internal Helper Classes and Functions
	//---------------------------------------------------------------------------

	class StateChangedCallbackTranslator
	{
	public:
		StateChangedCallbackTranslator(StateChangedHandler handler, void* pCookie) : m_UserHandler(handler), m_pUserCookie(pCookie), m_hCallback(NULL) {}

		XnStatus Register(_XnRegisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode)
		{
			return xnFunc(hNode, StateChangedCallback, this, &m_hCallback);
		}

		void Unregister(_XnUnregisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode)
		{
			xnFunc(hNode, m_hCallback);
		}

		static XnStatus RegisterToUnderlying(_XnRegisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode, StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			StateChangedCallbackTranslator* pTrans;
			XN_VALIDATE_NEW(pTrans, StateChangedCallbackTranslator, handler, pCookie);

			nRetVal = pTrans->Register(xnFunc, hNode);
			if (nRetVal != XN_STATUS_OK)
			{
				XN_DELETE(pTrans);
				return (nRetVal);
			}

			hCallback = pTrans;
			
			return (XN_STATUS_OK);
		}

		static XnStatus UnregisterFromUnderlying(_XnUnregisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode, XnCallbackHandle hCallback)
		{
			StateChangedCallbackTranslator* pTrans = (StateChangedCallbackTranslator*)hCallback;
			pTrans->Unregister(xnFunc, hNode);
			XN_DELETE(pTrans);
			return XN_STATUS_OK;
		}

	private:
		typedef struct StateChangeCookie
		{
			StateChangedHandler userHandler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} StateChangeCookie;

		static void XN_CALLBACK_TYPE StateChangedCallback(XnNodeHandle hNode, void* pCookie)
		{
			StateChangedCallbackTranslator* pTrans = (StateChangedCallbackTranslator*)pCookie;
			ProductionNode node(hNode);
			pTrans->m_UserHandler(node, pTrans->m_pUserCookie);
		}

		StateChangedHandler m_UserHandler;
		void* m_pUserCookie;
		XnCallbackHandle m_hCallback;
	};

	static XnStatus _RegisterToStateChange(_XnRegisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode, StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		return StateChangedCallbackTranslator::RegisterToUnderlying(xnFunc, hNode, handler, pCookie, hCallback);
	}

	static void _UnregisterFromStateChange(_XnUnregisterStateChangeFuncPtr xnFunc, XnNodeHandle hNode, XnCallbackHandle hCallback)
	{
		StateChangedCallbackTranslator::UnregisterFromUnderlying(xnFunc, hNode, hCallback);
	}

	/// @}
};

#endif // __XN_CPP_WRAPPER_H__
