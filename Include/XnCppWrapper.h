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
	// Some Utilities
	//---------------------------------------------------------------------------
	class Version
	{
	public:
		Version(const XnVersion& version) : m_version(version) {}
		Version(XnUInt8 nMajor, XnUInt8 nMinor, XnUInt16 nMaintenance, XnUInt32 nBuild)
		{
			m_version.nMajor = nMajor;
			m_version.nMinor = nMinor;
			m_version.nMaintenance = nMaintenance;
			m_version.nBuild = nBuild;
		}

		bool operator==(const Version& other) const
		{
			return (xnVersionCompare(&m_version, &other.m_version) == 0);
		}
		bool operator!=(const Version& other) const
		{
			return (xnVersionCompare(&m_version, &other.m_version) != 0);
		}
		bool operator<(const Version& other) const 
		{
			return (xnVersionCompare(&m_version, &other.m_version) < 0);
		}
		bool operator<=(const Version& other) const
		{
			return (xnVersionCompare(&m_version, &other.m_version) <= 0);
		}
		bool operator>(const Version& other) const
		{
			return (xnVersionCompare(&m_version, &other.m_version) > 0);
		}
		bool operator>=(const Version& other) const
		{
			return (xnVersionCompare(&m_version, &other.m_version) >= 0);
		}
	private:
		XnVersion m_version;
	};

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
				case XN_PIXEL_FORMAT_MJPEG:
					return 2;
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
		inline NodeWrapper(XnNodeHandle hNode) : m_hNode(NULL), m_hShuttingDownCallback(NULL)
		{
			NodeWrapper::SetHandle(hNode);
		}

		inline NodeWrapper(const NodeWrapper& other) : m_hNode(NULL), m_hShuttingDownCallback(NULL)
		{
			NodeWrapper::SetHandle(other.GetHandle());
		}

		inline NodeWrapper& operator=(const NodeWrapper& other)
		{
			NodeWrapper::SetHandle(other.GetHandle());
			return *this;
		}

		inline ~NodeWrapper()
		{
			NodeWrapper::SetHandle(NULL);
		}

		inline operator XnNodeHandle() const { return GetHandle(); }

		/// Gets the underlying C handle.
		inline XnNodeHandle GetHandle() const { return m_hNode; }

		/**
		 * Checks if two node wrappers point to the same object.
		 *
		 * @param	other		[in]	Another object
		 */
		inline XnBool operator==(const NodeWrapper& other)
		{
			return (GetHandle() == other.GetHandle());
		}

		/**
		 * Checks if two node wrappers point to different objects.
		 *
		 * @param	other		[in]	Another object
		 */
		inline XnBool operator!=(const NodeWrapper& other)
		{
			return (GetHandle() != other.GetHandle());
		}

		/// TRUE if the object points to an actual node, FALSE otherwise.
		inline XnBool IsValid() const { return (GetHandle() != NULL); }
		
		/** @copybrief xnGetNodeName()
		 * For full details and usage, see @ref xnGetNodeName()
		 */
		const XnChar* GetName() const {return xnGetNodeName(GetHandle()); }

		/** @copybrief xnProductionNodeAddRef
		 * For full details and usage, see @ref xnProductionNodeAddRef
		 */
		inline XnStatus AddRef() { return xnProductionNodeAddRef(GetHandle()); }

		/** @copybrief xnProductionNodeRelease
		 * For full details and usage, see @ref xnProductionNodeRelease
		 */
		inline void Release() 
		{
			NodeWrapper::SetHandle(NULL);
		}

		inline XnStatus XN_API_DEPRECATED("Please use AddRef() instead.") Ref() { return AddRef(); }
		inline void XN_API_DEPRECATED("Please use Release() instead.") Unref() { Release(); }

		/// Replaces the object being pointed.
		inline void SetHandle(XnNodeHandle hNode) 
		{ 
			if (m_hNode == hNode)
			{
				// Optimization: do nothing
				return;
			}

			// check currently held node. If we're holding a node, release it
			if (m_hNode != NULL)
			{
				XnContext* pContext = xnGetRefContextFromNodeHandle(m_hNode);
				xnContextUnregisterFromShutdown(pContext, m_hShuttingDownCallback);
				xnContextRelease(pContext);
				xnProductionNodeRelease(m_hNode);
			}

			// check new node handle, if it points to a node, add ref to it
			if (hNode != NULL)
			{
				XnStatus nRetVal = xnProductionNodeAddRef(hNode);
				XN_ASSERT(nRetVal == XN_STATUS_OK);

				XnContext* pContext = xnGetRefContextFromNodeHandle(hNode);

				nRetVal = xnContextRegisterForShutdown(pContext, ContextShuttingDownCallback, this, &m_hShuttingDownCallback);
				XN_ASSERT(nRetVal == XN_STATUS_OK);

				xnContextRelease(pContext);
			}

			m_hNode = hNode; 
		}

		inline void TakeOwnership(XnNodeHandle hNode)
		{
			SetHandle(hNode);

			if (hNode != NULL)
			{
				xnProductionNodeRelease(hNode);
			}
		}

	private:
		XnNodeHandle m_hNode;
		XnCallbackHandle m_hShuttingDownCallback;

		static void XN_CALLBACK_TYPE ContextShuttingDownCallback(XnContext* /*pContext*/, void* pCookie)
		{
			NodeWrapper* pThis = (NodeWrapper*)pCookie;
			pThis->m_hNode = NULL;
		}
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
		NodeInfo(XnNodeInfo* pInfo) : m_pNeededNodes(NULL), m_bOwnerOfNode(FALSE)
		{
			SetUnderlyingObject(pInfo);
		}

		/**
		 * Copy Ctor
		 *
		 * @param	other		[in]	An object to copy from.
		 */
		NodeInfo(const NodeInfo& other) : m_pNeededNodes(NULL), m_bOwnerOfNode(FALSE)
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
		 */		
		inline const XnChar* GetCreationInfo() const
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

		/** @copybrief xnNodeInfoGetAdditionalData
		 * For full details and usage, see @ref xnNodeInfoGetAdditionalData
		 */
		inline const void* GetAdditionalData() const
		{
			return xnNodeInfoGetAdditionalData(m_pInfo);
		}

		/** @copybrief xnNodeInfoGetTreeStringRepresentation
		 * For full details and usage, see @ref xnNodeInfoGetTreeStringRepresentation
		 */
		inline XnStatus GetTreeStringRepresentation(XnChar* csResultBuffer, XnUInt32 nBufferSize) const
		{
			return xnNodeInfoGetTreeStringRepresentation(m_pInfo, csResultBuffer, nBufferSize);
		}

	private:
		inline void SetUnderlyingObject(XnNodeInfo* pInfo);

		XnNodeInfo* m_pInfo;
		mutable NodeInfoList* m_pNeededNodes;
		XnBool m_bOwnerOfNode; // backwards compatibility
		friend class Context;
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
		inline Query() : m_bAllocated(TRUE)
		{
			xnNodeQueryAllocate(&m_pQuery);
		}

		inline Query(XnNodeQuery* pNodeQuery) : m_bAllocated(FALSE), m_pQuery(pNodeQuery)
		{
		}

		/// Dtor
		~Query()
		{
			if (m_bAllocated)
			{
				xnNodeQueryFree(m_pQuery);
			}
		}

		/// Gets the underlying C object
		inline const XnNodeQuery* GetUnderlyingObject() const { return m_pQuery; }
		inline XnNodeQuery* GetUnderlyingObject() { return m_pQuery; }

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
		XnBool m_bAllocated;
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

		/** @copybrief xnNodeInfoListAddEx
		 * For full details and usage, see @ref xnNodeInfoListAddEx
		 */
		inline XnStatus AddEx(XnProductionNodeDescription& description, const XnChar* strCreationInfo, NodeInfoList* pNeededNodes, const void* pAdditionalData, XnFreeHandler pFreeHandler)
		{
			XnNodeInfoList* pList = (pNeededNodes == NULL) ? NULL : pNeededNodes->GetUnderlyingObject();
			return xnNodeInfoListAddEx(m_pList, &description, strCreationInfo, pList, pAdditionalData, pFreeHandler);
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
	class Capability : public NodeWrapper
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	The node handle
		 */
		Capability(XnNodeHandle hNode) : NodeWrapper(hNode) {}
		Capability(const NodeWrapper& node) : NodeWrapper(node) {}
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
		ErrorStateCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnGetNodeErrorState
		 * For full details and usage, see @ref xnGetNodeErrorState
		 */
		inline XnStatus GetErrorState() const
		{
			return xnGetNodeErrorState(GetHandle());
		}

		/** @copybrief xnRegisterToNodeErrorStateChange
		 * For full details and usage, see @ref xnRegisterToNodeErrorStateChange
		 */
		inline XnStatus RegisterToErrorStateChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToNodeErrorStateChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromNodeErrorStateChange
		 * For full details and usage, see @ref xnUnregisterFromNodeErrorStateChange
		 */
		inline void UnregisterFromErrorStateChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromNodeErrorStateChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to general integer capabilities functions. see @ref general_int for additional details.
	 */
	class GeneralIntCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 * @param	strCap		[in]	Name of the capability
		 */
		GeneralIntCapability(XnNodeHandle hNode, const XnChar* strCap) : Capability(hNode), m_strCap(strCap) {}
		GeneralIntCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnGetGeneralIntRange
		 * For full details and usage, see @ref xnGetGeneralIntRange
		 */
		inline void GetRange(XnInt32& nMin, XnInt32& nMax, XnInt32& nStep, XnInt32& nDefault, XnBool& bIsAutoSupported) const
		{
			xnGetGeneralIntRange(GetHandle(), m_strCap, &nMin, &nMax, &nStep, &nDefault, &bIsAutoSupported);
		}

		/** @copybrief xnGetGeneralIntValue
		 * For full details and usage, see @ref xnGetGeneralIntValue
		 */
		inline XnInt32 Get()
		{
			XnInt32 nValue;
			xnGetGeneralIntValue(GetHandle(), m_strCap, &nValue);
			return nValue;
		}

		/** @copybrief xnSetGeneralIntValue
		 * For full details and usage, see @ref xnSetGeneralIntValue
		 */
		inline XnStatus Set(XnInt32 nValue)
		{
			return xnSetGeneralIntValue(GetHandle(), m_strCap, nValue);
		}

		/** @copybrief xnRegisterToGeneralIntValueChange
		 * For full details and usage, see @ref xnRegisterToGeneralIntValueChange
		 */
		XnStatus RegisterToValueChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);

		/** @copybrief xnUnregisterFromGeneralIntValueChange
		 * For full details and usage, see @ref xnUnregisterFromGeneralIntValueChange
		 */
		void UnregisterFromValueChange(XnCallbackHandle hCallback);

	private:
		const XnChar* m_strCap;
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
		inline ProductionNode(const NodeWrapper& other) : NodeWrapper(other) {}

		/** @copybrief xnGetNodeInfo
		 * For full details and usage, see @ref xnGetNodeInfo
		 */
		inline NodeInfo GetInfo() const { return NodeInfo(xnGetNodeInfo(GetHandle())); }

		/** @copybrief xnAddNeededNode
		 * For full details and usage, see @ref xnAddNeededNode
		 */
		inline XnStatus AddNeededNode(ProductionNode& needed)
		{
			return xnAddNeededNode(GetHandle(), needed.GetHandle());
		}

		/** @copybrief xnRemoveNeededNode
		 * For full details and usage, see @ref xnRemoveNeededNode
		 */
		inline XnStatus RemoveNeededNode(ProductionNode& needed)
		{
			return xnRemoveNeededNode(GetHandle(), needed.GetHandle());
		}

		/** @copybrief xnGetContextFromNodeHandle
		 * For full details and usage, see @ref xnGetContextFromNodeHandle
		 */
		inline void GetContext(Context& context) const;

		/** @copybrief xnIsCapabilitySupported
		 * For full details and usage, see @ref xnIsCapabilitySupported
		 */
		inline XnBool IsCapabilitySupported(const XnChar* strCapabilityName) const
		{
			return xnIsCapabilitySupported(GetHandle(), strCapabilityName);
		}

		/** @copybrief xnSetIntProperty
		 * For full details and usage, see @ref xnSetIntProperty
		 */
		inline XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
		{
			return xnSetIntProperty(GetHandle(), strName, nValue);
		}

		/** @copybrief xnSetRealProperty
		 * For full details and usage, see @ref xnSetRealProperty
		 */
		inline XnStatus SetRealProperty(const XnChar* strName, XnDouble dValue)
		{
			return xnSetRealProperty(GetHandle(), strName, dValue);
		}

		/** @copybrief xnSetStringProperty
		 * For full details and usage, see @ref xnSetStringProperty
		 */
		inline XnStatus SetStringProperty(const XnChar* strName, const XnChar* strValue)
		{
			return xnSetStringProperty(GetHandle(), strName, strValue);
		}

		/** @copybrief xnSetGeneralProperty
		 * For full details and usage, see @ref xnSetGeneralProperty
		 */
		inline XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
		{
			return xnSetGeneralProperty(GetHandle(), strName, nBufferSize, pBuffer);
		}

		/** @copybrief xnGetIntProperty
		 * For full details and usage, see @ref xnGetIntProperty
		 */
		inline XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
		{
			return xnGetIntProperty(GetHandle(), strName, &nValue);
		}

		/** @copybrief xnGetRealProperty
		 * For full details and usage, see @ref xnGetRealProperty
		 */
		inline XnStatus GetRealProperty(const XnChar* strName, XnDouble &dValue) const
		{
			return xnGetRealProperty(GetHandle(), strName, &dValue);
		}

		/** @copybrief xnGetStringProperty
		 * For full details and usage, see @ref xnGetStringProperty
		 */
		inline XnStatus GetStringProperty(const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize) const
		{
			return xnGetStringProperty(GetHandle(), strName, csValue, nBufSize);
		}

		/** @copybrief xnGetGeneralProperty
		 * For full details and usage, see @ref xnGetGeneralProperty
		 */
		inline XnStatus GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
		{
			return xnGetGeneralProperty(GetHandle(), strName, nBufferSize, pBuffer);
		}

		/** @copybrief xnLockNodeForChanges
		 * For full details and usage, see @ref xnLockNodeForChanges
		 */
		inline XnStatus LockForChanges(XnLockHandle* phLock)
		{
			return xnLockNodeForChanges(GetHandle(), phLock);
		}

		/** @copybrief xnUnlockNodeForChanges
		 * For full details and usage, see @ref xnUnlockNodeForChanges
		 */
		inline void UnlockForChanges(XnLockHandle hLock)
		{
			xnUnlockNodeForChanges(GetHandle(), hLock);
		}

		/** @copybrief xnLockedNodeStartChanges
		 * For full details and usage, see @ref xnLockedNodeStartChanges
		 */
		inline XnStatus LockedNodeStartChanges(XnLockHandle hLock)
		{
			return xnLockedNodeStartChanges(GetHandle(), hLock);
		}

		/** @copybrief xnLockedNodeEndChanges
		 * For full details and usage, see @ref xnLockedNodeEndChanges
		 */
		inline void LockedNodeEndChanges(XnLockHandle hLock)
		{
			xnLockedNodeEndChanges(GetHandle(), hLock);
		}

		/**
		 * Gets an @ref ErrorStateCapability object for accessing Error State functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ERROR_STATE is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const ErrorStateCapability GetErrorStateCap() const
		{
			return ErrorStateCapability(GetHandle());
		}

		/**
		 * Gets an @ref ErrorStateCapability object for accessing Error State functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ERROR_STATE is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline ErrorStateCapability GetErrorStateCap()
		{
			return ErrorStateCapability(GetHandle());
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing the capability functionality.
		 * It is the application responsibility to check first if the capability is supported
		 * by calling @ref IsCapabilitySupported().
		 *
		 * @param	strCapability	[in]	Name of the capability to get
		 */
		inline GeneralIntCapability GetGeneralIntCap(const XnChar* strCapability)
		{
			return GeneralIntCapability(GetHandle(), strCapability);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Device Identification</b> capability functions. see @ref device_id for additional details.
	 */
	class DeviceIdentificationCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		DeviceIdentificationCapability(XnNodeHandle hNode) : Capability(hNode) {}
		DeviceIdentificationCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnGetDeviceName
		 * For full details and usage, see @ref xnGetDeviceName
		 */
		inline XnStatus GetDeviceName(XnChar* strBuffer, XnUInt32 nBufferSize)
		{
			return xnGetDeviceName(GetHandle(), strBuffer, &nBufferSize);
		}

		/** @copybrief xnGetVendorSpecificData
		 * For full details and usage, see @ref xnGetVendorSpecificData
		 */
		inline XnStatus GetVendorSpecificData(XnChar* strBuffer, XnUInt32 nBufferSize)
		{
			return xnGetVendorSpecificData(GetHandle(), strBuffer, &nBufferSize);
		}

		/** @copybrief xnGetSerialNumber
		 * For full details and usage, see @ref xnGetSerialNumber
		 */
		inline XnStatus GetSerialNumber(XnChar* strBuffer, XnUInt32 nBufferSize)
		{
			return xnGetSerialNumber(GetHandle(), strBuffer, &nBufferSize);
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
		inline Device(const NodeWrapper& other) : ProductionNode(other) {}

		/** @copybrief xnCreateDevice
		 * For full details and usage, see @ref xnCreateDevice
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * Gets a @ref DeviceIdentificationCapability object for accessing device identification functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_DEVICE_IDENTIFICATION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline DeviceIdentificationCapability GetIdentificationCap()
		{
			return DeviceIdentificationCapability(GetHandle());
		}
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
		MirrorCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnSetMirror
		 * For full details and usage, see @ref xnSetMirror
		 */
		inline XnStatus SetMirror(XnBool bMirror)
		{
			return xnSetMirror(GetHandle(), bMirror);
		}

		/** @copybrief xnIsMirrored
		 * For full details and usage, see @ref xnIsMirrored
		 */
		inline XnBool IsMirrored() const
		{
			return xnIsMirrored(GetHandle());
		}

		/** @copybrief xnRegisterToMirrorChange
		 * For full details and usage, see @ref xnRegisterToMirrorChange
		 */
		inline XnStatus RegisterToMirrorChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToMirrorChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromMirrorChange
		 * For full details and usage, see @ref xnUnregisterFromMirrorChange
		 */
		inline void UnregisterFromMirrorChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromMirrorChange, GetHandle(), hCallback);
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
		AlternativeViewPointCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnIsViewPointSupported
		 * For full details and usage, see @ref xnIsViewPointSupported
		 */
		inline XnBool IsViewPointSupported(ProductionNode& otherNode) const
		{
			return xnIsViewPointSupported(GetHandle(), otherNode.GetHandle());
		}

		/** @copybrief xnSetViewPoint
		 * For full details and usage, see @ref xnSetViewPoint
		 */
		inline XnStatus SetViewPoint(ProductionNode& otherNode)
		{
			return xnSetViewPoint(GetHandle(), otherNode.GetHandle());
		}

		/** @copybrief xnResetViewPoint
		 * For full details and usage, see @ref xnResetViewPoint
		 */
		inline XnStatus ResetViewPoint()
		{
			return xnResetViewPoint(GetHandle());
		}

		/** @copybrief xnIsViewPointAs
		 * For full details and usage, see @ref xnIsViewPointAs
		 */
		inline XnBool IsViewPointAs(ProductionNode& otherNode) const
		{
			return xnIsViewPointAs(GetHandle(), otherNode.GetHandle());
		}

		/** @copybrief xnRegisterToViewPointChange
		 * For full details and usage, see @ref xnRegisterToViewPointChange
		 */
		inline XnStatus RegisterToViewPointChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToViewPointChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromViewPointChange
		 * For full details and usage, see @ref xnUnregisterFromViewPointChange
		 */
		inline void UnregisterFromViewPointChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromViewPointChange, GetHandle(), hCallback);
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
		FrameSyncCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnCanFrameSyncWith
		 * For full details and usage, see @ref xnCanFrameSyncWith
		 */
		inline XnBool CanFrameSyncWith(Generator& other) const;

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
		inline XnBool IsFrameSyncedWith(Generator& other) const;

		/** @copybrief xnRegisterToFrameSyncChange
		 * For full details and usage, see @ref xnRegisterToFrameSyncChange
		 */
		inline XnStatus RegisterToFrameSyncChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToFrameSyncChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromFrameSyncChange
		 * For full details and usage, see @ref xnUnregisterFromFrameSyncChange
		 */
		inline void UnregisterFromFrameSyncChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromFrameSyncChange, GetHandle(), hCallback);
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
		inline Generator(const NodeWrapper& other) : ProductionNode(other) {}

		/** @copybrief xnStartGenerating
		 * For full details and usage, see @ref xnStartGenerating
		 */
		inline XnStatus StartGenerating()
		{
			return xnStartGenerating(GetHandle());
		}

		/** @copybrief xnIsGenerating
		 * For full details and usage, see @ref xnIsGenerating
		 */
		inline XnBool IsGenerating() const
		{
			return xnIsGenerating(GetHandle());
		}

		/** @copybrief xnStopGenerating
		 * For full details and usage, see @ref xnStopGenerating
		 */
		inline XnStatus StopGenerating()
		{
			return xnStopGenerating(GetHandle());
		}

		/** @copybrief xnRegisterToGenerationRunningChange
		 * For full details and usage, see @ref xnRegisterToGenerationRunningChange
		 */
		inline XnStatus RegisterToGenerationRunningChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle &hCallback)
		{
			return _RegisterToStateChange(xnRegisterToGenerationRunningChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromGenerationRunningChange
		 * For full details and usage, see @ref xnUnregisterFromGenerationRunningChange
		 */
		inline void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromGenerationRunningChange, GetHandle(), hCallback);
		}

		/** @copybrief xnRegisterToNewDataAvailable
		 * For full details and usage, see @ref xnRegisterToNewDataAvailable
		 */
		inline XnStatus RegisterToNewDataAvailable(StateChangedHandler handler, void* pCookie, XnCallbackHandle &hCallback)
		{
			return _RegisterToStateChange(xnRegisterToNewDataAvailable, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromNewDataAvailable
		 * For full details and usage, see @ref xnUnregisterFromNewDataAvailable
		 */
		inline void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromNewDataAvailable, GetHandle(), hCallback);
		}

		/** @copybrief xnIsNewDataAvailable
		 * For full details and usage, see @ref xnIsNewDataAvailable
		 */
		inline XnBool IsNewDataAvailable(XnUInt64* pnTimestamp = NULL) const
		{
			return xnIsNewDataAvailable(GetHandle(), pnTimestamp);
		}

		/** @copybrief xnWaitAndUpdateData
		 * For full details and usage, see @ref xnWaitAndUpdateData
		 */
		inline XnStatus WaitAndUpdateData()
		{
			return xnWaitAndUpdateData(GetHandle());
		}

		/** @copybrief xnIsDataNew
		 * For full details and usage, see @ref xnIsDataNew
		 */
		inline XnBool IsDataNew() const
		{
			return xnIsDataNew(GetHandle());
		}

		/** @copybrief xnGetData
		 * For full details and usage, see @ref xnGetData
		 */
		inline const void* GetData()
		{
			return xnGetData(GetHandle());
		}
		
		/** @copybrief xnGetDataSize
		 * For full details and usage, see @ref xnGetDataSize
		 */
		inline XnUInt32 GetDataSize() const
		{
			return xnGetDataSize(GetHandle());
		}

		/** @copybrief xnGetTimestamp
		 * For full details and usage, see @ref xnGetTimestamp
		 */
		inline XnUInt64 GetTimestamp() const
		{
			return xnGetTimestamp(GetHandle());
		}

		/** @copybrief xnGetFrameID
		 * For full details and usage, see @ref xnGetFrameID
		 */
		inline XnUInt32 GetFrameID() const
		{
			return xnGetFrameID(GetHandle());
		}

		/**
		 * Gets an @ref MirrorCapability object for accessing Mirror functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_MIRROR is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const MirrorCapability GetMirrorCap() const
		{ 
			return MirrorCapability(GetHandle()); 
		}

		/**
		 * Gets an @ref MirrorCapability object for accessing Mirror functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_MIRROR is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline MirrorCapability GetMirrorCap()
		{ 
			return MirrorCapability(GetHandle()); 
		}

		/**
		 * Gets an @ref AlternativeViewPointCapability object for accessing Alternative View Point functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ALTERNATIVE_VIEW_POINT is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const AlternativeViewPointCapability GetAlternativeViewPointCap() const
		{ 
			return AlternativeViewPointCapability(GetHandle()); 
		}

		/**
		 * Gets an @ref AlternativeViewPointCapability object for accessing Alternative View Point functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ALTERNATIVE_VIEW_POINT is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline AlternativeViewPointCapability GetAlternativeViewPointCap()
		{ 
			return AlternativeViewPointCapability(GetHandle()); 
		}

		/**
		 * Gets an @ref FrameSyncCapability object for accessing Frame Sync functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_FRAME_SYNC is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const FrameSyncCapability GetFrameSyncCap() const
		{
			return FrameSyncCapability(GetHandle());
		}

		/**
		 * Gets an @ref FrameSyncCapability object for accessing Frame Sync functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_FRAME_SYNC is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline FrameSyncCapability GetFrameSyncCap()
		{
			return FrameSyncCapability(GetHandle());
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
		inline Recorder(const NodeWrapper& other) : ProductionNode(other) {}

		/** @copybrief xnCreateRecorder
		 * For full details and usage, see @ref xnCreateRecorder
		 */
		inline XnStatus Create(Context& context, const XnChar* strFormatName = NULL);

		/** @copybrief xnSetRecorderDestination
		 * For full details and usage, see @ref xnSetRecorderDestination
		 */
		inline XnStatus SetDestination(XnRecordMedium destType, const XnChar* strDest)
		{
			return xnSetRecorderDestination(GetHandle(), destType, strDest);
		}

		inline XnStatus GetDestination(XnRecordMedium& destType, XnChar* strDest, XnUInt32 nBufSize)
		{
			return xnGetRecorderDestination(GetHandle(), &destType, strDest, nBufSize);
		}

		/** @copybrief xnAddNodeToRecording
		 * For full details and usage, see @ref xnAddNodeToRecording
		 */
		inline XnStatus AddNodeToRecording(ProductionNode& Node, XnCodecID compression = XN_CODEC_NULL)
		{
			return xnAddNodeToRecording(GetHandle(), Node.GetHandle(), compression);
		}

		/** @copybrief xnRemoveNodeFromRecording
		 * For full details and usage, see @ref xnRemoveNodeFromRecording
		 */
		inline XnStatus RemoveNodeFromRecording(ProductionNode& Node)
		{
			return xnRemoveNodeFromRecording(GetHandle(), Node.GetHandle());
		}

		/** @copybrief xnRecord
		 * For full details and usage, see @ref xnRecord
		 */
		inline XnStatus Record()
		{
			return xnRecord(GetHandle());
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
		inline Player(const NodeWrapper& other) : ProductionNode(other) {}

		/** @copybrief xnCreatePlayer
		 * For full details and usage, see @ref xnCreatePlayer
		 */
		inline XnStatus Create(Context& context, const XnChar* strFormatName);

		/** @copybrief xnSetPlayerRepeat
		 * For full details and usage, see @ref xnSetPlayerRepeat
		 */
		inline XnStatus SetRepeat(XnBool bRepeat)
		{
			return xnSetPlayerRepeat(GetHandle(), bRepeat);
		}

		/** @copybrief xnSetPlayerSource
		 * For full details and usage, see @ref xnSetPlayerSource
		 */
		inline XnStatus SetSource(XnRecordMedium sourceType, const XnChar* strSource)
		{
			return xnSetPlayerSource(GetHandle(), sourceType, strSource);
		}

		/** @copybrief xnGetPlayerSource
		 * For full details and usage, see @ref xnGetPlayerSource
		 */
		inline XnStatus GetSource(XnRecordMedium &sourceType, XnChar* strSource, XnUInt32 nBufSize) const
		{
			return xnGetPlayerSource(GetHandle(), &sourceType, strSource, nBufSize);
		}

		/** @copybrief xnPlayerReadNext
		 * For full details and usage, see @ref xnPlayerReadNext
		 */
		inline XnStatus ReadNext()
		{
			return xnPlayerReadNext(GetHandle());
		}

		/** @copybrief xnSeekPlayerToTimeStamp
		 * For full details and usage, see @ref xnSeekPlayerToTimeStamp
		 */
		inline XnStatus SeekToTimeStamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin)
		{
			return xnSeekPlayerToTimeStamp(GetHandle(), nTimeOffset, origin);
		}

		/** @copybrief xnSeekPlayerToFrame
		 * For full details and usage, see @ref xnSeekPlayerToFrame
		 */
		inline XnStatus SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin)
		{
			return xnSeekPlayerToFrame(GetHandle(), strNodeName, nFrameOffset, origin);
		}

		/** @copybrief xnTellPlayerTimestamp
		 * For full details and usage, see @ref xnTellPlayerTimestamp
		 */
		inline XnStatus TellTimestamp(XnUInt64& nTimestamp) const
		{
			return xnTellPlayerTimestamp(GetHandle(), &nTimestamp);
		}

		/** @copybrief xnTellPlayerFrame
		 * For full details and usage, see @ref xnTellPlayerFrame
		 */
		inline XnStatus TellFrame(const XnChar* strNodeName, XnUInt32& nFrame) const
		{
			return xnTellPlayerFrame(GetHandle(), strNodeName, &nFrame);
		}

		/** @copybrief xnGetPlayerNumFrames
		 * For full details and usage, see @ref xnGetPlayerNumFrames
		 */
		inline XnStatus GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames) const
		{
			return xnGetPlayerNumFrames(GetHandle(), strNodeName, &nFrames);
		}

		/** @copybrief xnGetPlayerSupportedFormat
		 * For full details and usage, see @ref xnGetPlayerSupportedFormat
		 */
		inline const XnChar* GetSupportedFormat() const
		{
			return xnGetPlayerSupportedFormat(GetHandle());
		}

		/** @copybrief xnEnumeratePlayerNodes
		 * For full details and usage, see @ref xnEnumeratePlayerNodes
		 */
		inline XnStatus EnumerateNodes(NodeInfoList& list) const
		{
			XnNodeInfoList* pList;
			XnStatus nRetVal = xnEnumeratePlayerNodes(GetHandle(), &pList);
			XN_IS_STATUS_OK(nRetVal);

			list.ReplaceUnderlyingObject(pList);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnIsPlayerAtEOF
		 * For full details and usage, see @ref xnIsPlayerAtEOF
		 */
		inline XnBool IsEOF() const
		{
			return xnIsPlayerAtEOF(GetHandle());
		}

		/** @copybrief xnRegisterToEndOfFileReached
		 * For full details and usage, see @ref xnRegisterToEndOfFileReached
		 */
		inline XnStatus RegisterToEndOfFileReached(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToEndOfFileReached, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromEndOfFileReached
		 * For full details and usage, see @ref xnUnregisterFromEndOfFileReached
		 */
		inline void UnregisterFromEndOfFileReached(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromEndOfFileReached, GetHandle(), hCallback);
		}

		/** @copybrief xnSetPlaybackSpeed
		 * For full details and usage, see @ref xnSetPlaybackSpeed
		 */
		inline XnStatus SetPlaybackSpeed(XnDouble dSpeed)
		{
			return xnSetPlaybackSpeed(GetHandle(), dSpeed);
		}

		/** @copybrief xnGetPlaybackSpeed
		 * For full details and usage, see @ref xnGetPlaybackSpeed
		 */
		inline XnDouble GetPlaybackSpeed() const
		{
			return xnGetPlaybackSpeed(GetHandle());
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
		CroppingCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnSetCropping
		 * For full details and usage, see @ref xnSetCropping
		 */
		inline XnStatus SetCropping(const XnCropping& Cropping)
		{
			return xnSetCropping(GetHandle(), &Cropping);
		}

		/** @copybrief xnGetCropping
		 * For full details and usage, see @ref xnGetCropping
		 */
		inline XnStatus GetCropping(XnCropping& Cropping) const
		{
			return xnGetCropping(GetHandle(), &Cropping);
		}

		/** @copybrief xnRegisterToCroppingChange
		 * For full details and usage, see @ref xnRegisterToCroppingChange
		 */
		inline XnStatus RegisterToCroppingChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToCroppingChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromCroppingChange
		 * For full details and usage, see @ref xnUnregisterFromCroppingChange
		 */
		inline void UnregisterFromCroppingChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromCroppingChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref
	 * Allows access to <b>Anti Flicker</b> capability functions. see @ref anti_flicker for additional details.
	 */
	class AntiFlickerCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline AntiFlickerCapability(XnNodeHandle hNode) : Capability(hNode) {}
		AntiFlickerCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnSetPowerLineFrequency
		 * For full details and usage, see @ref xnSetPowerLineFrequency
		 */
		inline XnStatus SetPowerLineFrequency(XnPowerLineFrequency nFrequency)
		{
			return xnSetPowerLineFrequency(GetHandle(), nFrequency);
		}

		/** @copybrief xnGetPowerLineFrequency
		 * For full details and usage, see @ref xnGetPowerLineFrequency
		 */
		inline XnPowerLineFrequency GetPowerLineFrequency()
		{
			return xnGetPowerLineFrequency(GetHandle());
		}

		/** @copybrief xnRegisterToPowerLineFrequencyChange
		 * For full details and usage, see @ref xnRegisterToPowerLineFrequencyChange
		 */
		inline XnStatus RegisterToPowerLineFrequencyChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToPowerLineFrequencyChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromPowerLineFrequencyChange
		 * For full details and usage, see @ref xnUnregisterFromPowerLineFrequencyChange
		 */
		inline void UnregisterFromPowerLineFrequencyChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromPowerLineFrequencyChange, GetHandle(), hCallback);
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
		inline MapGenerator(const NodeWrapper& other) : Generator(other) {}

		/** @copybrief xnGetSupportedMapOutputModesCount
		 * For full details and usage, see @ref xnGetSupportedMapOutputModesCount
		 */
		inline XnUInt32 GetSupportedMapOutputModesCount() const
		{
			return xnGetSupportedMapOutputModesCount(GetHandle());
		}

		/** @copybrief xnGetSupportedMapOutputModes
		 * For full details and usage, see @ref xnGetSupportedMapOutputModes
		 */
		inline XnStatus GetSupportedMapOutputModes(XnMapOutputMode* aModes, XnUInt32& nCount) const
		{
			return xnGetSupportedMapOutputModes(GetHandle(), aModes, &nCount);
		}

		/** @copybrief xnSetMapOutputMode
		 * For full details and usage, see @ref xnSetMapOutputMode
		 */
		inline XnStatus SetMapOutputMode(const XnMapOutputMode& OutputMode)
		{
			return xnSetMapOutputMode(GetHandle(), &OutputMode);
		}

		/** @copybrief xnGetMapOutputMode
		 * For full details and usage, see @ref xnGetMapOutputMode
		 */
		inline XnStatus GetMapOutputMode(XnMapOutputMode &OutputMode) const
		{
			return xnGetMapOutputMode(GetHandle(), &OutputMode);
		}

		/** @copybrief xnGetBytesPerPixel
		 * For full details and usage, see @ref xnGetBytesPerPixel
		 */
		inline XnUInt32 GetBytesPerPixel() const
		{
			return xnGetBytesPerPixel(GetHandle());
		}

		/** @copybrief xnRegisterToMapOutputModeChange
		 * For full details and usage, see @ref xnRegisterToMapOutputModeChange
		 */
		inline XnStatus RegisterToMapOutputModeChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToMapOutputModeChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromMapOutputModeChange
		 * For full details and usage, see @ref xnUnregisterFromMapOutputModeChange
		 */
		inline void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromMapOutputModeChange, GetHandle(), hCallback);
		}

		/**
		 * Gets an @ref CroppingCapability object for accessing Cropping functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_CROPPING is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const CroppingCapability GetCroppingCap() const
		{
			return CroppingCapability(GetHandle());
		}

		/**
		 * Gets an @ref CroppingCapability object for accessing Cropping functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_CROPPING is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline CroppingCapability GetCroppingCap()
		{
			return CroppingCapability(GetHandle());
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Brightness functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_BRIGHTNESS is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetBrightnessCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_BRIGHTNESS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Contrast functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_CONTRAST is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetContrastCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_CONTRAST);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Hue functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_HUE is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetHueCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_HUE);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Saturation functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_SATURATION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetSaturationCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_SATURATION);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Sharpness functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_SHARPNESS is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetSharpnessCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_SHARPNESS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Gamma functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_GAMMA is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetGammaCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_GAMMA);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing WhiteBalance functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_COLOR_TEMPERATURE is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetWhiteBalanceCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_COLOR_TEMPERATURE);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing BacklightCompensation functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_BACKLIGHT_COMPENSATION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetBacklightCompensationCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_BACKLIGHT_COMPENSATION);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Gain functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_GAIN is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetGainCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_GAIN);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Pan functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_PAN is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetPanCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_PAN);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Tilt functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_TILT is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetTiltCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_TILT);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Roll functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ROLL is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetRollCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_ROLL);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Zoom functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ZOOM is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetZoomCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_ZOOM);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Exposure functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_EXPOSURE is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetExposureCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_EXPOSURE);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Iris functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_IRIS is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetIrisCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_IRIS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Focus functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_FOCUS is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetFocusCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_FOCUS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Low Light Compensation functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_LOW_LIGHT_COMPENSATION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetLowLightCompensationCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_LOW_LIGHT_COMPENSATION);
		}

		/**
		 * Gets an @ref AntiFlickerCapability object for accessing Anti Flicker functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ANTI_FLICKER is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline AntiFlickerCapability GetAntiFlickerCap()
		{
			return AntiFlickerCapability(GetHandle());
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
		UserPositionCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnGetSupportedUserPositionsCount
		 * For full details and usage, see @ref xnGetSupportedUserPositionsCount
		 */
		inline XnUInt32 GetSupportedUserPositionsCount() const
		{
			return xnGetSupportedUserPositionsCount(GetHandle());
		}

		/** @copybrief xnSetUserPosition
		 * For full details and usage, see @ref xnSetUserPosition
		 */
		inline XnStatus SetUserPosition(XnUInt32 nIndex, const XnBoundingBox3D& Position)
		{
			return xnSetUserPosition(GetHandle(), nIndex, &Position);
		}

		/** @copybrief xnGetUserPosition
		 * For full details and usage, see @ref xnGetUserPosition
		 */
		inline XnStatus GetUserPosition(XnUInt32 nIndex, XnBoundingBox3D& Position) const
		{
			return xnGetUserPosition(GetHandle(), nIndex, &Position);
		}

		/** @copybrief xnRegisterToUserPositionChange
		 * For full details and usage, see @ref xnRegisterToUserPositionChange
		 */
		inline XnStatus RegisterToUserPositionChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToUserPositionChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromUserPositionChange
		 * For full details and usage, see @ref xnUnregisterFromUserPositionChange
		 */
		inline void UnregisterFromUserPositionChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromUserPositionChange, GetHandle(), hCallback);
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
		inline DepthGenerator(const NodeWrapper& other) : MapGenerator(other) {}

		/** @copybrief xnCreateDepthGenerator
		 * For full details and usage, see @ref xnCreateDepthGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetDepthMetaData
		 * For full details and usage, see @ref xnGetDepthMetaData
		 */
		inline void GetMetaData(DepthMetaData& metaData) const 
		{
			xnGetDepthMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/** @copybrief xnGetDepthMap
		 * For full details and usage, see @ref xnGetDepthMap
		 */
		inline const XnDepthPixel* GetDepthMap() const
		{
			return xnGetDepthMap(GetHandle());
		}

		/** @copybrief xnGetDeviceMaxDepth
		 * For full details and usage, see @ref xnGetDeviceMaxDepth
		 */
		inline XnDepthPixel GetDeviceMaxDepth() const
		{
			return xnGetDeviceMaxDepth(GetHandle());
		}

		/** @copybrief xnGetDepthFieldOfView
		 * For full details and usage, see @ref xnGetDepthFieldOfView
		 */
		inline XnStatus GetFieldOfView(XnFieldOfView& FOV) const
		{
			return xnGetDepthFieldOfView(GetHandle(), &FOV);
		}

		/** @copybrief xnRegisterToDepthFieldOfViewChange
		 * For full details and usage, see @ref xnRegisterToDepthFieldOfViewChange
		 */
		inline XnStatus RegisterToFieldOfViewChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToDepthFieldOfViewChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromDepthFieldOfViewChange
		 * For full details and usage, see @ref xnUnregisterFromDepthFieldOfViewChange
		 */
		inline void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromDepthFieldOfViewChange, GetHandle(), hCallback);
		}

		/** @copybrief xnConvertProjectiveToRealWorld
		 * For full details and usage, see @ref xnConvertProjectiveToRealWorld
		 */
		inline XnStatus ConvertProjectiveToRealWorld(XnUInt32 nCount, const XnPoint3D aProjective[], XnPoint3D aRealWorld[]) const
		{
			return xnConvertProjectiveToRealWorld(GetHandle(), nCount, aProjective, aRealWorld);
		}

		/** @copybrief xnConvertRealWorldToProjective
		 * For full details and usage, see @ref xnConvertRealWorldToProjective
		 */
		inline XnStatus ConvertRealWorldToProjective(XnUInt32 nCount, const XnPoint3D aRealWorld[], XnPoint3D aProjective[]) const
		{
			return xnConvertRealWorldToProjective(GetHandle(), nCount, aRealWorld, aProjective);
		}

		/**
		 * Gets an @ref UserPositionCapability object for accessing User Position functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_USER_POSITION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const UserPositionCapability GetUserPositionCap() const
		{
			return UserPositionCapability(GetHandle());
		}

		/**
		 * Gets an @ref UserPositionCapability object for accessing User Position functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_USER_POSITION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline UserPositionCapability GetUserPositionCap()
		{
			return UserPositionCapability(GetHandle());
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
		inline MockDepthGenerator(const NodeWrapper& other) : DepthGenerator(other) {}

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
			return xnMockDepthSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pDepthMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	depthMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(const DepthMetaData& depthMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, depthMD.DataSize(), depthMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param	depthMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(const DepthMetaData& depthMD)
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
		inline ImageGenerator(const NodeWrapper& other) : MapGenerator(other) {}

		/** @copybrief xnCreateImageGenerator
		 * For full details and usage, see @ref xnCreateImageGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetImageMetaData
		 * For full details and usage, see @ref xnGetImageMetaData
		 */
		inline void GetMetaData(ImageMetaData& metaData) const 
		{
			xnGetImageMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/** @copybrief xnGetRGB24ImageMap
		 * For full details and usage, see @ref xnGetRGB24ImageMap
		 */
		inline const XnRGB24Pixel* GetRGB24ImageMap() const
		{
			return xnGetRGB24ImageMap(GetHandle());
		}

		/** @copybrief xnGetYUV422ImageMap
		 * For full details and usage, see @ref xnGetYUV422ImageMap
		 */
		inline const XnYUV422DoublePixel* GetYUV422ImageMap() const
		{
			return xnGetYUV422ImageMap(GetHandle());
		}

		/** @copybrief xnGetGrayscale8ImageMap
		 * For full details and usage, see @ref xnGetGrayscale8ImageMap
		 */
		inline const XnGrayscale8Pixel* GetGrayscale8ImageMap() const
		{
			return xnGetGrayscale8ImageMap(GetHandle());
		}

		/** @copybrief xnGetGrayscale16ImageMap
		 * For full details and usage, see @ref xnGetGrayscale16ImageMap
		 */
		inline const XnGrayscale16Pixel* GetGrayscale16ImageMap() const
		{
			return xnGetGrayscale16ImageMap(GetHandle());
		}

		/** @copybrief xnGetImageMap() 
		 * For full details, see @ref xnGetImageMap().
		 */
		inline const XnUInt8* GetImageMap() const
		{
			return xnGetImageMap(GetHandle());
		}

		/** @copybrief xnIsPixelFormatSupported
		 * For full details and usage, see @ref xnIsPixelFormatSupported
		 */
		inline XnBool IsPixelFormatSupported(XnPixelFormat Format) const
		{
			return xnIsPixelFormatSupported(GetHandle(), Format);
		}

		/** @copybrief xnSetPixelFormat
		 * For full details and usage, see @ref xnSetPixelFormat
		 */
		inline XnStatus SetPixelFormat(XnPixelFormat Format)
		{
			return xnSetPixelFormat(GetHandle(), Format);
		}

		/** @copybrief xnGetPixelFormat
		 * For full details and usage, see @ref xnGetPixelFormat
		 */
		inline XnPixelFormat GetPixelFormat() const
		{
			return xnGetPixelFormat(GetHandle());
		}

		/** @copybrief xnRegisterToPixelFormatChange
		 * For full details and usage, see @ref xnRegisterToPixelFormatChange
		 */
		inline XnStatus RegisterToPixelFormatChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToPixelFormatChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromPixelFormatChange
		 * For full details and usage, see @ref xnUnregisterFromPixelFormatChange
		 */
		inline void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromPixelFormatChange, GetHandle(), hCallback);
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
		inline MockImageGenerator(const NodeWrapper& other) : ImageGenerator(other) {}

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
			return xnMockImageSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pImageMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	imageMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(const ImageMetaData& imageMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, imageMD.DataSize(), imageMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param	imageMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(const ImageMetaData& imageMD)
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
		inline IRGenerator(const NodeWrapper& other) : MapGenerator(other) {}

		/** @copybrief xnCreateIRGenerator
		 * For full details and usage, see @ref xnCreateIRGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetIRMetaData
		 * For full details and usage, see @ref xnGetIRMetaData
		 */
		inline void GetMetaData(IRMetaData& metaData) const 
		{ 
			xnGetIRMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/** @copybrief xnGetIRMap
		 * For full details and usage, see @ref xnGetIRMap
		 */
		inline const XnIRPixel* GetIRMap() const
		{
			return xnGetIRMap(GetHandle());
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
		inline MockIRGenerator(const NodeWrapper& other) : IRGenerator(other) {}

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
			return xnMockIRSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pIRMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	irMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(const IRMetaData& irMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, irMD.DataSize(), irMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param	irMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(const IRMetaData& irMD)
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
		inline GestureGenerator(const NodeWrapper& other) : Generator(other) {}

		/** @copybrief xnCreateGestureGenerator
		 * For full details and usage, see @ref xnCreateGestureGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnAddGesture
		 * For full details and usage, see @ref xnAddGesture
		 */
		inline XnStatus AddGesture(const XnChar* strGesture, XnBoundingBox3D* pArea)
		{
			return xnAddGesture(GetHandle(), strGesture, pArea);
		}

		/** @copybrief xnRemoveGesture
		 * For full details and usage, see @ref xnRemoveGesture
		 */
		inline XnStatus RemoveGesture(const XnChar* strGesture)
		{
			return xnRemoveGesture(GetHandle(), strGesture);
		}

		/** @copybrief xnGetActiveGestures
		 * For full details and usage, see @ref xnGetActiveGestures
		 */
		inline XnStatus GetActiveGestures(XnChar*& astrGestures, XnUInt16& nGestures) const
		{
			return xnGetActiveGestures(GetHandle(), &astrGestures, &nGestures);
		}

		/** @copybrief xnGetActiveGestures
		 * For full details and usage, see @ref xnGetActiveGestures
		 */
		inline XnStatus GetAllActiveGestures(XnChar** astrGestures, XnUInt32 nNameLength, XnUInt16& nGestures) const
		{
			return xnGetAllActiveGestures(GetHandle(), astrGestures, nNameLength, &nGestures);
		}

		/** @copybrief xnEnumerateGestures
		 * For full details and usage, see @ref xnEnumerateGestures
		 */
		inline XnStatus EnumerateGestures(XnChar*& astrGestures, XnUInt16& nGestures) const
		{
			return xnEnumerateGestures(GetHandle(), &astrGestures, &nGestures);
		}

        /** @copybrief xnGetNumberOfAvailableGestures
         * For full details and usage, see @ref xnGetNumberOfAvailableGestures
         */
        inline XnUInt16 GetNumberOfAvailableGestures() const
        {
            return xnGetNumberOfAvailableGestures(GetHandle());
        }

		/** @copybrief xnEnumerateAllGestures
		 * For full details and usage, see @ref xnEnumerateAllGestures
		 */
		inline XnStatus EnumerateAllGestures(XnChar** astrGestures, XnUInt32 nNameLength, XnUInt16& nGestures) const
		{
			return xnEnumerateAllGestures(GetHandle(), astrGestures, nNameLength, &nGestures);
		}

		/** @copybrief xnIsGestureAvailable
		 * For full details and usage, see @ref xnIsGestureAvailable
		 */
		inline XnBool IsGestureAvailable(const XnChar* strGesture) const
		{
			return xnIsGestureAvailable(GetHandle(), strGesture);
		}

		/** @copybrief xnIsGestureProgressSupported
		 * For full details and usage, see @ref xnIsGestureProgressSupported
		 */
		inline XnBool IsGestureProgressSupported(const XnChar* strGesture) const
		{
			return xnIsGestureProgressSupported(GetHandle(), strGesture);
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

			nRetVal = xnRegisterGestureCallbacks(GetHandle(), GestureRecognizedCallback, GestureProgressCallback, pGestureCookie, &pGestureCookie->hCallback);
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
			xnUnregisterGestureCallbacks(GetHandle(), pGestureCookie->hCallback);
			xnOSFree(pGestureCookie);
		}

		/** @copybrief xnRegisterToGestureChange
		 * For full details and usage, see @ref xnRegisterToGestureChange
		 */
		inline XnStatus RegisterToGestureChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToGestureChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromGestureChange
		 * For full details and usage, see @ref xnUnregisterFromGestureChange
		 */
		inline void UnregisterFromGestureChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromGestureChange, GetHandle(), hCallback);
		}

		/**
		 * Callback for indication that a certain gesture is in progress
		 *
		 * @param	generator	[in]	The node that raised the event.
		 * @param	strGesture	[in]	The gesture that is on its way to being recognized.
		 * @param	pPosition	[in]	The current position of the hand that is performing the gesture.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureIntermediateStageCompleted)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie);
		/** @copybrief xnRegisterToGestureIntermediateStageCompleted
		 * For full details and usage, see @ref xnRegisterToGestureIntermediateStageCompleted
		 */
		XnStatus RegisterToGestureIntermediateStageCompleted(GestureIntermediateStageCompleted handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			GestureIntermediateStageCompletedCookie* pGestureCookie;
			XN_VALIDATE_ALLOC(pGestureCookie, GestureIntermediateStageCompletedCookie);
			pGestureCookie->handler = handler;
			pGestureCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterToGestureIntermediateStageCompleted(GetHandle(), GestureIntermediateStageCompletedCallback, pGestureCookie, &pGestureCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pGestureCookie);
				return (nRetVal);
			}

			hCallback = pGestureCookie;

			return (XN_STATUS_OK);
		}
		/** @copybrief xnUnregisterFromGestureIntermediateStageCompleted
		 * For full details and usage, see @ref xnUnregisterFromGestureIntermediateStageCompleted
		 */
		inline void UnregisterFromGestureIntermediateStageCompleted(XnCallbackHandle hCallback)
		{
			GestureIntermediateStageCompletedCookie* pGestureCookie = (GestureIntermediateStageCompletedCookie*)hCallback;
			xnUnregisterFromGestureIntermediateStageCompleted(GetHandle(), pGestureCookie->hCallback);
			xnOSFree(pGestureCookie);
		}

		/**
		 * Callback for indication that a certain gesture is ready for its next stage (whatever that is)
		 *
		 * @param	generator	[in]	The node that raised the event.
		 * @param	strGesture	[in]	The gesture that is on its way to being recognized.
		 * @param	pPosition	[in]	The current position of the hand that is performing the gesture.
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureReadyForNextIntermediateStage)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie);
		/** @copybrief xnRegisterToGestureReadyForNextIntermediateStage
		 * For full details and usage, see @ref xnRegisterToGestureReadyForNextIntermediateStage
		 */
		XnStatus RegisterToGestureReadyForNextIntermediateStage(GestureReadyForNextIntermediateStage handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			GestureReadyForNextIntermediateStageCookie* pGestureCookie;
			XN_VALIDATE_ALLOC(pGestureCookie, GestureReadyForNextIntermediateStageCookie);
			pGestureCookie->handler = handler;
			pGestureCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterToGestureReadyForNextIntermediateStage(GetHandle(), GestureReadyForNextIntermediateStageCallback, pGestureCookie, &pGestureCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pGestureCookie);
				return (nRetVal);
			}

			hCallback = pGestureCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterFromGestureReadyForNextIntermediateStage
		 * For full details and usage, see @ref xnUnregisterFromGestureReadyForNextIntermediateStage
		 */
		inline void UnregisterFromGestureReadyForNextIntermediateStageCallbacks(XnCallbackHandle hCallback)
		{
			GestureReadyForNextIntermediateStageCookie* pGestureCookie = (GestureReadyForNextIntermediateStageCookie*)hCallback;
			xnUnregisterFromGestureReadyForNextIntermediateStage(GetHandle(), pGestureCookie->hCallback);
			xnOSFree(pGestureCookie);
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
			if (pGestureCookie->recognizedHandler != NULL)
			{
				pGestureCookie->recognizedHandler(gen, strGesture, pIDPosition, pEndPosition, pGestureCookie->pUserCookie);
			}
		}

		static void XN_CALLBACK_TYPE GestureProgressCallback(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie)
		{
			GestureCookie* pGestureCookie = (GestureCookie*)pCookie;
			GestureGenerator gen(hNode);
			if (pGestureCookie->progressHandler != NULL)
			{
				pGestureCookie->progressHandler(gen, strGesture, pPosition, fProgress, pGestureCookie->pUserCookie);
			}
		}

		typedef struct GestureIntermediateStageCompletedCookie
		{
			GestureIntermediateStageCompleted handler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} GestureIntermediateStageCompletedCookie;

		static void XN_CALLBACK_TYPE GestureIntermediateStageCompletedCallback(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie)
		{
			GestureIntermediateStageCompletedCookie* pGestureCookie = (GestureIntermediateStageCompletedCookie*)pCookie;
			GestureGenerator gen(hNode);
			if (pGestureCookie->handler != NULL)
			{
				pGestureCookie->handler(gen, strGesture, pPosition, pGestureCookie->pUserCookie);
			}
		}

		typedef struct GestureReadyForNextIntermediateStageCookie
		{
			GestureReadyForNextIntermediateStage handler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} GestureReadyForNextIntermediateStageCookie;

		static void XN_CALLBACK_TYPE GestureReadyForNextIntermediateStageCallback(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie)
		{
			GestureReadyForNextIntermediateStageCookie* pGestureCookie = (GestureReadyForNextIntermediateStageCookie*)pCookie;
			GestureGenerator gen(hNode);
			if (pGestureCookie->handler != NULL)
			{
				pGestureCookie->handler(gen, strGesture, pPosition, pGestureCookie->pUserCookie);
			}
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
		inline SceneAnalyzer(const NodeWrapper& other) : MapGenerator(other) {}

		/** @copybrief xnCreateSceneAnalyzer
		 * For full details and usage, see @ref xnCreateSceneAnalyzer
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetSceneMetaData
		 * For full details and usage, see @ref xnGetSceneMetaData
		 */
		inline void GetMetaData(SceneMetaData& metaData) const
		{
			xnGetSceneMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/** @copybrief xnGetLabelMap
		 * For full details and usage, see @ref xnGetLabelMap
		 */
		inline const XnLabel* GetLabelMap() const
		{
			return xnGetLabelMap(GetHandle());
		}

		/** @copybrief xnGetFloor
		 * For full details and usage, see @ref xnGetFloor
		 */
		inline XnStatus GetFloor(XnPlane3D& Plane) const
		{
			return xnGetFloor(GetHandle(), &Plane);
		}
	};

	/**
	* @ingroup cppref
	* Allows access to <b>Hand touching FOV edge</b> capability functions. see @ref HandTouchingFOVEdgeCapability for additional details.
	*/
	class HandTouchingFOVEdgeCapability : public Capability
	{
	public:
		/**
		* Ctor
		*
		* @param	hNode		[in]	Node handle
		*/
		inline HandTouchingFOVEdgeCapability(XnNodeHandle hNode) : Capability(hNode) {}
		HandTouchingFOVEdgeCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		* Callback for the hand getting close to the edge of the FOV
		*
		* @param	touchingfov	[in]	the hand touching fov edge capability that raised this event.
		* @param	user		[in]	The id of the hand that disappeared.
		* @param	pPosition	[in]	The position of the hand.
		* @param	fTime		[in]	Timestamp, in seconds.
		* @param	eDir		[in]	Direction of the FOV touched
		* @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		*/
		typedef void (XN_CALLBACK_TYPE* HandTouchingFOVEdge)(HandTouchingFOVEdgeCapability& touchingfov, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDir, void* pCookie);
		/** @copybrief xnRegisterToHandTouchingFOVEdge
		* For full details and usage, see @ref xnRegisterToHandTouchingFOVEdge
		*/
		inline XnStatus RegisterToHandTouchingFOVEdge(HandTouchingFOVEdge handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			HandTouchingFOVEdgeCookie* pHandCookie;
			XN_VALIDATE_ALLOC(pHandCookie, HandTouchingFOVEdgeCookie);
			pHandCookie->handler = handler;
			pHandCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterToHandTouchingFOVEdge(GetHandle(), HandTouchingFOVEdgeCB, pHandCookie, &pHandCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pHandCookie);
				return (nRetVal);
			}

			hCallback = pHandCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterFromHandTouchingFOVEdge
		* For full details and usage, see @ref xnUnregisterFromHandTouchingFOVEdge
		*/
		inline void UnregisterFromHandTouchingFOVEdge(XnCallbackHandle hCallback)
		{
			HandTouchingFOVEdgeCookie* pHandCookie = (HandTouchingFOVEdgeCookie*)hCallback;
			xnUnregisterFromHandTouchingFOVEdge(GetHandle(), pHandCookie->hCallback);
			xnOSFree(pHandCookie);
		}
	private:
		typedef struct HandTouchingFOVEdgeCookie
		{
			HandTouchingFOVEdge handler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} HandTouchingFOVEdgeCookie;

		static void XN_CALLBACK_TYPE HandTouchingFOVEdgeCB(XnNodeHandle hNode, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDir, void* pCookie)
		{
			HandTouchingFOVEdgeCookie* pHandCookie = (HandTouchingFOVEdgeCookie*)pCookie;
			HandTouchingFOVEdgeCapability cap(hNode);
			if (pHandCookie->handler != NULL)
			{
				pHandCookie->handler(cap, user, pPosition, fTime, eDir, pHandCookie->pUserCookie);
			}
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
		inline HandsGenerator(const NodeWrapper& other) : Generator(other) {}

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

			nRetVal = xnRegisterHandCallbacks(GetHandle(), HandCreateCB, HandUpdateCB, HandDestroyCB, pHandCookie, &pHandCookie->hCallback);
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
			xnUnregisterHandCallbacks(GetHandle(), pHandCookie->hCallback);
			xnOSFree(pHandCookie);
		}

		/** @copybrief xnStopTracking
		 * For full details and usage, see @ref xnStopTracking
		 */
		inline XnStatus StopTracking(XnUserID user)
		{
			return xnStopTracking(GetHandle(), user);
		}

		/** @copybrief xnStopTrackingAll
		 * For full details and usage, see @ref xnStopTrackingAll
		 */
		inline XnStatus StopTrackingAll()
		{
			return xnStopTrackingAll(GetHandle());
		}

		/** @copybrief xnStartTracking
		 * For full details and usage, see @ref xnStartTracking
		 */
		inline XnStatus StartTracking(const XnPoint3D& ptPosition)
		{
			return xnStartTracking(GetHandle(), &ptPosition);
		}

		/** @copybrief xnSetTrackingSmoothing
		 * For full details and usage, see @ref xnSetTrackingSmoothing
		 */
		inline XnStatus SetSmoothing(XnFloat fSmoothingFactor)
		{
			return xnSetTrackingSmoothing(GetHandle(), fSmoothingFactor);
		}

		/**
		* Gets an @ref HandTouchingFOVEdgeCapability object for accessing hand touching FOV edge callbacks functionality.
		* It is the application responsibility to check first if @ref XN_CAPABILITY_HAND_TOUCHING_FOV_EDGE is supported
		* by calling @ref IsCapabilitySupported().
		*/
		inline const HandTouchingFOVEdgeCapability GetHandTouchingFOVEdgeCap() const
		{
			return HandTouchingFOVEdgeCapability(GetHandle());
		}

		/**
		* Gets an @ref HandTouchingFOVEdgeCapability object for accessing hand touching FOV edge callbacks functionality.
		* It is the application responsibility to check first if @ref XN_CAPABILITY_HAND_TOUCHING_FOV_EDGE is supported
		* by calling @ref IsCapabilitySupported().
		*/
		inline HandTouchingFOVEdgeCapability GetHandTouchingFOVEdgeCap()
		{
			return HandTouchingFOVEdgeCapability(GetHandle());
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
			if (pHandCookie->createHandler != NULL)
			{
				pHandCookie->createHandler(gen, user, pPosition, fTime, pHandCookie->pUserCookie);
			}
		}
		static void XN_CALLBACK_TYPE HandUpdateCB(XnNodeHandle hNode, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
		{
			HandCookie* pHandCookie = (HandCookie*)pCookie;
			HandsGenerator gen(hNode);
			if (pHandCookie->updateHandler != NULL)
			{
				pHandCookie->updateHandler(gen, user, pPosition, fTime, pHandCookie->pUserCookie);
			}
		}
		static void XN_CALLBACK_TYPE HandDestroyCB(XnNodeHandle hNode, XnUserID user, XnFloat fTime, void* pCookie)
		{
			HandCookie* pHandCookie = (HandCookie*)pCookie;
			HandsGenerator gen(hNode);
			if (pHandCookie->destroyHandler != NULL)
			{
				pHandCookie->destroyHandler(gen, user, fTime, pHandCookie->pUserCookie);
			}
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
		SkeletonCapability(const NodeWrapper& node) : Capability(node) {}

		/** @copybrief xnIsJointAvailable
		 * For full details and usage, see @ref xnIsJointAvailable
		 */
		inline XnBool IsJointAvailable(XnSkeletonJoint eJoint) const
		{
			return xnIsJointAvailable(GetHandle(), eJoint);
		}

		/** @copybrief xnIsProfileAvailable
		 * For full details and usage, see @ref xnIsProfileAvailable
		 */
		inline XnBool IsProfileAvailable(XnSkeletonProfile eProfile) const
		{
			return xnIsProfileAvailable(GetHandle(), eProfile);
		}

		/** @copybrief xnSetSkeletonProfile
		 * For full details and usage, see @ref xnSetSkeletonProfile
		 */
		inline XnStatus SetSkeletonProfile(XnSkeletonProfile eProfile)
		{
			return xnSetSkeletonProfile(GetHandle(), eProfile);
		}

		/** @copybrief xnSetJointActive
		 * For full details and usage, see @ref xnSetJointActive
		 */
		inline XnStatus SetJointActive(XnSkeletonJoint eJoint, XnBool bState)
		{
			return xnSetJointActive(GetHandle(), eJoint, bState);
		}

		/** @copybrief xnIsJointActive
		 * For full details and usage, see @ref xnIsJointActive
		 */
		XN_API_DEPRECATED("Use the version with one argument") 
		inline XnBool IsJointActive(XnSkeletonJoint eJoint, XnBool /*bState*/) const
		{
			return xnIsJointActive(GetHandle(), eJoint);
		}

		/** @copybrief xnIsJointActive
		 * For full details and usage, see @ref xnIsJointActive
		 */
		inline XnBool IsJointActive(XnSkeletonJoint eJoint) const
		{
			return xnIsJointActive(GetHandle(), eJoint);
		}

		/** @copybrief xnRegisterToJointConfigurationChange
		 * For full details and usage, see @ref xnRegisterToJointConfigurationChange
		 */
		inline XnStatus RegisterToJointConfigurationChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToJointConfigurationChange, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromJointConfigurationChange
		 * For full details and usage, see @ref xnUnregisterFromJointConfigurationChange
		 */
		inline void UnregisterFromJointConfigurationChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromJointConfigurationChange, GetHandle(), hCallback);
		}

		/** @copybrief xnEnumerateActiveJoints
		 * For full details and usage, see @ref xnEnumerateActiveJoints
		 */
		inline XnStatus EnumerateActiveJoints(XnSkeletonJoint* pJoints, XnUInt16& nJoints) const
		{
			return xnEnumerateActiveJoints(GetHandle(), pJoints, &nJoints);
		}

		/** @copybrief xnGetSkeletonJoint
		 * For full details and usage, see @ref xnGetSkeletonJoint
		 */
		inline XnStatus GetSkeletonJoint(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation& Joint) const
		{
			return xnGetSkeletonJoint(GetHandle(), user, eJoint, &Joint);
		}

		/** @copybrief xnGetSkeletonJointPosition
		 * For full details and usage, see @ref xnGetSkeletonJointPosition
		 */
		inline XnStatus GetSkeletonJointPosition(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition& Joint) const
		{
			return xnGetSkeletonJointPosition(GetHandle(), user, eJoint, &Joint);
		}

		/** @copybrief xnGetSkeletonJointOrientation
		 * For full details and usage, see @ref xnGetSkeletonJointOrientation
		 */
		inline XnStatus GetSkeletonJointOrientation(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation& Joint) const
		{
			return xnGetSkeletonJointOrientation(GetHandle(), user, eJoint, &Joint);
		}

		/** @copybrief xnIsSkeletonTracking
		 * For full details and usage, see @ref xnIsSkeletonTracking
		 */
		inline XnBool IsTracking(XnUserID user) const
		{
			return xnIsSkeletonTracking(GetHandle(), user);
		}

		/** @copybrief xnIsSkeletonCalibrated
		 * For full details and usage, see @ref xnIsSkeletonCalibrated
		 */
		inline XnBool IsCalibrated(XnUserID user) const
		{
			return xnIsSkeletonCalibrated(GetHandle(), user);
		}

		/** @copybrief xnIsSkeletonCalibrating
		 * For full details and usage, see @ref xnIsSkeletonCalibrating
		 */
		inline XnBool IsCalibrating(XnUserID user) const
		{
			return xnIsSkeletonCalibrating(GetHandle(), user);
		}

		/** @copybrief xnRequestSkeletonCalibration
		 * For full details and usage, see @ref xnRequestSkeletonCalibration
		 */
		inline XnStatus RequestCalibration(XnUserID user, XnBool bForce)
		{
			return xnRequestSkeletonCalibration(GetHandle(), user, bForce);
		}

		/** @copybrief xnAbortSkeletonCalibration
		 * For full details and usage, see @ref xnAbortSkeletonCalibration
		 */
		inline XnStatus AbortCalibration(XnUserID user)
		{
			return xnAbortSkeletonCalibration(GetHandle(), user);
		}

		/** @copybrief xnSaveSkeletonCalibrationDataToFile
		* For full details and usage, see @ref xnSaveSkeletonCalibrationDataToFile
		*/
		inline XnStatus SaveCalibrationDataToFile(XnUserID user, const XnChar* strFileName)
		{
			return xnSaveSkeletonCalibrationDataToFile(GetHandle(), user, strFileName);
		}

		/** @copybrief xnLoadSkeletonCalibrationDataFromFile
		* For full details and usage, see @ref xnLoadSkeletonCalibrationDataFromFile
		*/
		inline XnStatus LoadCalibrationDataFromFile(XnUserID user, const XnChar* strFileName)
		{
			return xnLoadSkeletonCalibrationDataFromFile(GetHandle(), user, strFileName);
		}

		/** @copybrief xnSaveSkeletonCalibrationData
		 * For full details and usage, see @ref xnSaveSkeletonCalibrationData
		 */
		inline XnStatus SaveCalibrationData(XnUserID user, XnUInt32 nSlot)
		{
			return xnSaveSkeletonCalibrationData(GetHandle(), user, nSlot);
		}

		/** @copybrief xnLoadSkeletonCalibrationData
		 * For full details and usage, see @ref xnLoadSkeletonCalibrationData
		 */
		inline XnStatus LoadCalibrationData(XnUserID user, XnUInt32 nSlot)
		{
			return xnLoadSkeletonCalibrationData(GetHandle(), user, nSlot);
		}

		/** @copybrief xnClearSkeletonCalibrationData
		 * For full details and usage, see @ref xnClearSkeletonCalibrationData
		 */
		inline XnStatus ClearCalibrationData(XnUInt32 nSlot)
		{
			return xnClearSkeletonCalibrationData(GetHandle(), nSlot);
		}

		/** @copybrief xnIsSkeletonCalibrationData
		 * For full details and usage, see @ref xnIsSkeletonCalibrationData
		 */
		inline XnBool IsCalibrationData(XnUInt32 nSlot) const
		{
			return xnIsSkeletonCalibrationData(GetHandle(), nSlot);
		}

		/** @copybrief xnStartSkeletonTracking
		 * For full details and usage, see @ref xnStartSkeletonTracking
		 */
		inline XnStatus StartTracking(XnUserID user)
		{
			return xnStartSkeletonTracking(GetHandle(), user);
		}

		/** @copybrief xnStopSkeletonTracking
		 * For full details and usage, see @ref xnStopSkeletonTracking
		 */
		inline XnStatus StopTracking(XnUserID user)
		{
			return xnStopSkeletonTracking(GetHandle(), user);
		}

		/** @copybrief xnResetSkeleton
		 * For full details and usage, see @ref xnResetSkeleton
		 */
		inline XnStatus Reset(XnUserID user)
		{
			return xnResetSkeleton(GetHandle(), user);
		}

		/** @copybrief xnNeedPoseForSkeletonCalibration
		 * For full details and usage, see @ref xnNeedPoseForSkeletonCalibration
		 */
		inline XnBool NeedPoseForCalibration() const
		{
			return xnNeedPoseForSkeletonCalibration(GetHandle());
		}

		/** @copybrief xnGetSkeletonCalibrationPose
		 * For full details and usage, see @ref xnGetSkeletonCalibrationPose
		 */
		inline XnStatus GetCalibrationPose(XnChar* strPose) const
		{
			return xnGetSkeletonCalibrationPose(GetHandle(), strPose);
		}

		/** @copybrief xnSetSkeletonSmoothing
		 * For full details and usage, see @ref xnSetSkeletonSmoothing
		 */
		inline XnStatus SetSmoothing(XnFloat fSmoothingFactor)
		{
			return xnSetSkeletonSmoothing(GetHandle(), fSmoothingFactor);
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
		inline XnStatus XN_API_DEPRECATED("Please use RegisterToCalibrationStart/Complete") RegisterCalibrationCallbacks(CalibrationStart CalibrationStartCB, CalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			SkeletonCookie* pSkeletonCookie;
			XN_VALIDATE_ALLOC(pSkeletonCookie, SkeletonCookie);
			pSkeletonCookie->startHandler = CalibrationStartCB;
			pSkeletonCookie->endHandler = CalibrationEndCB;
			pSkeletonCookie->pUserCookie = pCookie;

#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			nRetVal = xnRegisterCalibrationCallbacks(GetHandle(), CalibrationStartBundleCallback, CalibrationEndBundleCallback, pSkeletonCookie, &pSkeletonCookie->hCallback);
#pragma warning (pop)
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
		inline void XN_API_DEPRECATED("Please use UnregisterFromCalibrationStart/Complete") UnregisterCalibrationCallbacks(XnCallbackHandle hCallback)
		{
			SkeletonCookie* pSkeletonCookie = (SkeletonCookie*)hCallback;
#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			xnUnregisterCalibrationCallbacks(GetHandle(), pSkeletonCookie->hCallback);
#pragma warning (pop)
			xnOSFree(pSkeletonCookie);
		}

		/** @copybrief xnRegisterToCalibrationStart
		 * For full details and usage, see @ref xnRegisterToCalibrationStart
		 */
		inline XnStatus RegisterToCalibrationStart(CalibrationStart handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			CalibrationStartCookie* pCalibrationCookie;
			XN_VALIDATE_ALLOC(pCalibrationCookie, CalibrationStartCookie);
			pCalibrationCookie->handler = handler;
			pCalibrationCookie->pUserCookie = pCookie;
			nRetVal = xnRegisterToCalibrationStart(GetHandle(), CalibrationStartCallback, pCalibrationCookie, &pCalibrationCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pCalibrationCookie);
				return nRetVal;
			}
			hCallback = pCalibrationCookie;
			return XN_STATUS_OK;
		}

		/** @copybrief xnUnregisterFromCalibrationStart
		 * For full details and usage, see @ref xnUnregisterFromCalibrationStart.
		 *
		 * Note: due to history constraints, this function has an XnStatus return value. In practice, it
		 * will always succeed. The user can safely ignore the return value.
		 */
		inline XnStatus UnregisterFromCalibrationStart(XnCallbackHandle hCallback)
		{
			CalibrationStartCookie* pCalibrationCookie = (CalibrationStartCookie*)hCallback;
			xnUnregisterFromCalibrationStart(GetHandle(), pCalibrationCookie->hCallback);
			xnOSFree(pCalibrationCookie);
			return XN_STATUS_OK;
		}

		/**
		 * Callback for indication that a specific user's skeleton is calibrating, and its current status
		 * 
		 * @param	hNode				[in]	The node that raised the event.
		 * @param	user				[in]	The id of the user for which calibration was attempted.
		 * @param	calibrationError	[in]	the current status of the calibration
		 * @param	pCookie				[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationInProgress)(SkeletonCapability& skeleton, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie);

		/** @copybrief xnRegisterToCalibrationInProgress
		 * For full details and usage, see @ref xnRegisterToCalibrationInProgress
		 */
		inline XnStatus RegisterToCalibrationInProgress(CalibrationInProgress handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			CalibrationInProgressCookie* pSkeletonCookie;
			XN_VALIDATE_ALLOC(pSkeletonCookie, CalibrationInProgressCookie);
			pSkeletonCookie->handler = handler;
			pSkeletonCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterToCalibrationInProgress(GetHandle(), CalibrationInProgressCallback, pSkeletonCookie, &pSkeletonCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pSkeletonCookie);
				return (nRetVal);
			}

			hCallback = pSkeletonCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterFromCalibrationInProgress
		 * For full details and usage, see @ref xnUnregisterFromCalibrationInProgress
		 */
		inline void UnregisterFromCalibrationInProgress(XnCallbackHandle hCallback)
		{
			CalibrationInProgressCookie* pSkeletonCookie = (CalibrationInProgressCookie*)hCallback;
			xnUnregisterFromCalibrationInProgress(GetHandle(), pSkeletonCookie->hCallback);
			xnOSFree(pSkeletonCookie);
		}

		/**
		 * Callback for indication that a specific user's skeleton has now completed the calibration process, with a status
		 * 
		 * @param	hNode				[in]	The node that raised the event.
		 * @param	user				[in]	The id of the user for which calibration was attempted.
		 * @param	calibrationError	[in]	The status in which the calibration has ended.
		 * @param	pCookie				[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationComplete)(SkeletonCapability& skeleton, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie);
		/** @copybrief xnRegisterToCalibrationComplete
		 * For full details and usage, see @ref xnRegisterToCalibrationComplete
		 */
		inline XnStatus RegisterToCalibrationComplete(CalibrationComplete handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			CalibrationCompleteCookie* pSkeletonCookie;
			XN_VALIDATE_ALLOC(pSkeletonCookie, CalibrationCompleteCookie);
			pSkeletonCookie->handler = handler;
			pSkeletonCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterToCalibrationComplete(GetHandle(), CalibrationCompleteCallback, pSkeletonCookie, &pSkeletonCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pSkeletonCookie);
				return (nRetVal);
			}

			hCallback = pSkeletonCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterFromCalibrationComplete
		 * For full details and usage, see @ref xnUnregisterFromCalibrationComplete
		 */
		inline void UnregisterFromCalibrationComplete(XnCallbackHandle hCallback)
		{
			CalibrationCompleteCookie* pSkeletonCookie = (CalibrationCompleteCookie*)hCallback;
			xnUnregisterFromCalibrationComplete(GetHandle(), pSkeletonCookie->hCallback);
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

		static void XN_CALLBACK_TYPE CalibrationStartBundleCallback(XnNodeHandle hNode, XnUserID user, void* pCookie)
		{
			SkeletonCookie* pSkeletonCookie = (SkeletonCookie*)pCookie;
			SkeletonCapability cap(hNode);
			if (pSkeletonCookie->startHandler != NULL)
			{
				pSkeletonCookie->startHandler(cap, user, pSkeletonCookie->pUserCookie);
			}
		}

		static void XN_CALLBACK_TYPE CalibrationEndBundleCallback(XnNodeHandle hNode, XnUserID user, XnBool bSuccess, void* pCookie)
		{
			SkeletonCookie* pSkeletonCookie = (SkeletonCookie*)pCookie;
			SkeletonCapability cap(hNode);
			if (pSkeletonCookie->endHandler != NULL)
			{
				pSkeletonCookie->endHandler(cap, user, bSuccess, pSkeletonCookie->pUserCookie);
			}
		}
		typedef struct CalibrationStartCookie
		{
			CalibrationStart handler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} CalibrationStartCookie;

		static void XN_CALLBACK_TYPE CalibrationStartCallback(XnNodeHandle hNode, XnUserID user, void* pCookie)
		{
			CalibrationStartCookie* pCalibrationCookie = (CalibrationStartCookie*)pCookie;
			SkeletonCapability cap(hNode);
			if (pCalibrationCookie->handler != NULL)
			{
				pCalibrationCookie->handler(cap, user, pCalibrationCookie->pUserCookie);
			}
		}
		typedef struct CalibrationInProgressCookie
		{
			CalibrationInProgress handler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} CalibrationInProgressCookie;

		static void XN_CALLBACK_TYPE CalibrationInProgressCallback(XnNodeHandle hNode, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie)
		{
			CalibrationInProgressCookie* pSkeletonCookie = (CalibrationInProgressCookie*)pCookie;
			SkeletonCapability cap(hNode);
			if (pSkeletonCookie->handler != NULL)
			{
				pSkeletonCookie->handler(cap, user, calibrationError, pSkeletonCookie->pUserCookie);
			}
		}

		typedef struct CalibrationCompleteCookie
		{
			CalibrationComplete handler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} CalibrationCompleteCookie;

		static void XN_CALLBACK_TYPE CalibrationCompleteCallback(XnNodeHandle hNode, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie)
		{
			CalibrationCompleteCookie* pSkeletonCookie = (CalibrationCompleteCookie*)pCookie;
			SkeletonCapability cap(hNode);
			if (pSkeletonCookie->handler != NULL)
			{
				pSkeletonCookie->handler(cap, user, calibrationError, pSkeletonCookie->pUserCookie);
			}
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
		PoseDetectionCapability(const NodeWrapper& node) : Capability(node) {}

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
		inline XnUInt32 GetNumberOfPoses() const
		{
			return xnGetNumberOfPoses(GetHandle());
		}

		/** @copybrief xnGetAvailablePoses
		 * For full details and usage, see @ref xnGetAvailablePoses
		 */
		inline XnStatus GetAvailablePoses(XnChar** pstrPoses, XnUInt32& nPoses) const
		{
			return xnGetAvailablePoses(GetHandle(), pstrPoses, &nPoses);
		}
		/** @copybrief xnGetAvailablePoses
		* For full details and usage, see @ref xnGetAvailablePoses
		*/
		inline XnStatus GetAllAvailablePoses(XnChar** pstrPoses, XnUInt32 nNameLength, XnUInt32& nPoses) const
		{
			return xnGetAllAvailablePoses(GetHandle(), pstrPoses, nNameLength, &nPoses);
		}

        inline XnBool IsPoseSupported(const XnChar* strPose)
        {
            return xnIsPoseSupported(GetHandle(), strPose);
        }

        inline XnStatus GetPoseStatus(XnUserID userID, const XnChar* poseName, XnUInt64& poseTime, XnPoseDetectionStatus& eStatus, XnPoseDetectionState& eState)
        {
            return xnGetPoseStatus(GetHandle(), userID, poseName, &poseTime, &eStatus, &eState);
        }

		/** @copybrief xnStartPoseDetection
		 * For full details and usage, see @ref xnStartPoseDetection
		 */
		inline XnStatus StartPoseDetection(const XnChar* strPose, XnUserID user)
		{
			return xnStartPoseDetection(GetHandle(), strPose, user);
		}

		/** @copybrief xnStopPoseDetection
		 * For full details and usage, see @ref xnStopPoseDetection
		 */
		inline XnStatus StopPoseDetection(XnUserID user)
		{
			return xnStopPoseDetection(GetHandle(), user);
		}

		/** @copybrief xnStopSinglePoseDetection
		 * For full details and usage, see @ref xnStopSinglePoseDetection
		 */
		inline XnStatus StopSinglePoseDetection(XnUserID user, const XnChar* strPose)
		{
			return xnStopSinglePoseDetection(GetHandle(), user, strPose);
		}

		/** @copybrief xnRegisterToPoseCallbacks
		 * For full details and usage, see @ref xnRegisterToPoseCallbacks
		 */
		inline XnStatus XN_API_DEPRECATED("Please use RegisterToPoseDetected/RegisterToOutOfPose instead") RegisterToPoseCallbacks(PoseDetection PoseStartCB, PoseDetection PoseEndCB, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			PoseCookie* pPoseCookie;
			XN_VALIDATE_ALLOC(pPoseCookie, PoseCookie);
			pPoseCookie->startHandler = PoseStartCB;
			pPoseCookie->endHandler = PoseEndCB;
			pPoseCookie->pPoseCookie = pCookie;

#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			nRetVal = xnRegisterToPoseCallbacks(GetHandle(), PoseDetectionStartBundleCallback, PoseDetectionStartEndBundleCallback, pPoseCookie, &pPoseCookie->hCallback);
#pragma warning (pop)
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
		inline void XN_API_DEPRECATED("Please use UnregisterFromPoseDetected/UnregisterFromOutOfPose instead") UnregisterFromPoseCallbacks(XnCallbackHandle hCallback)
		{
			PoseCookie* pPoseCookie = (PoseCookie*)hCallback;
#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			xnUnregisterFromPoseCallbacks(GetHandle(), pPoseCookie->hCallback);
#pragma warning (pop)
			xnOSFree(pPoseCookie);
		}

		/** @copybrief xnRegisterToPoseDetected
		 * For full details and usage, see @ref xnRegisterToPoseDetected
		 */
		inline XnStatus RegisterToPoseDetected(PoseDetection handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			PoseDetectionCookie* pPoseCookie;
			XN_VALIDATE_ALLOC(pPoseCookie, PoseDetectionCookie);
			pPoseCookie->handler = handler;
			pPoseCookie->pPoseCookie = pCookie;

			nRetVal = xnRegisterToPoseDetected(GetHandle(), PoseDetectionCallback, pPoseCookie, &pPoseCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pPoseCookie);
				return nRetVal;
			}
			hCallback = pPoseCookie;
			return XN_STATUS_OK;
		}
		/** @copybrief xnRegisterToOutOfPose
		 * For full details and usage, see @ref xnRegisterToOutOfPose
		 */
		inline XnStatus RegisterToOutOfPose(PoseDetection handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			PoseDetectionCookie* pPoseCookie;
			XN_VALIDATE_ALLOC(pPoseCookie, PoseDetectionCookie);
			pPoseCookie->handler = handler;
			pPoseCookie->pPoseCookie = pCookie;

			nRetVal = xnRegisterToOutOfPose(GetHandle(), PoseDetectionCallback, pPoseCookie, &pPoseCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pPoseCookie);
				return nRetVal;
			}
			hCallback = pPoseCookie;
			return XN_STATUS_OK;
		}
		/** @copybrief xnUnregisterFromPoseDetected
		 * For full details and usage, see @ref xnUnregisterFromPoseDetected
		 */
		inline void UnregisterFromPoseDetected(XnCallbackHandle hCallback)
		{
			PoseDetectionCookie* pPoseCookie = (PoseDetectionCookie*)hCallback;
			xnUnregisterFromPoseDetected(GetHandle(), pPoseCookie->hCallback);
			xnOSFree(pPoseCookie);
		}
		/** @copybrief xnUnregisterFromOutOfPose
		 * For full details and usage, see @ref xnUnregisterFromOutOfPose
		 */
		inline void UnregisterFromOutOfPose(XnCallbackHandle hCallback)
		{
			PoseDetectionCookie* pPoseCookie = (PoseDetectionCookie*)hCallback;
			xnUnregisterFromOutOfPose(GetHandle(), pPoseCookie->hCallback);
			xnOSFree(pPoseCookie);
		}

		/**
		 * Callback for indication of the current status of pose detection for a user
		 *
		 * @param	pose		[in]	The node that raised the event.
		 * @param	strPose		[in]	The pose that is relevant to the callback.
		 * @param	user		[in]	The id of the user that entered the pose or left it.
		 * @param	poseError	[in]	The current status of the pose detection
		 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* PoseInProgress)(PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, XnPoseDetectionStatus poseError, void* pCookie);
		/** @copybrief xnRegisterToPoseDetectionInProgress
		 * For full details and usage, see @ref xnRegisterToPoseDetectionInProgress
		 */
		inline XnStatus RegisterToPoseInProgress(PoseInProgress handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			PoseInProgressCookie* pPoseCookie;
			XN_VALIDATE_ALLOC(pPoseCookie, PoseInProgressCookie);
			pPoseCookie->handler = handler;
			pPoseCookie->pPoseCookie = pCookie;

			nRetVal = xnRegisterToPoseDetectionInProgress(GetHandle(), PoseDetectionInProgressCallback, pPoseCookie, &pPoseCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pPoseCookie);
				return (nRetVal);
			}

			hCallback = pPoseCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterFromPoseDetectionInProgress
		 * For full details and usage, see @ref xnUnregisterFromPoseDetectionInProgress
		 */
		inline void UnregisterFromPoseInProgress(XnCallbackHandle hCallback)
		{
			PoseInProgressCookie* pPoseCookie = (PoseInProgressCookie*)hCallback;
			xnUnregisterFromPoseDetectionInProgress(GetHandle(), pPoseCookie->hCallback);
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

		static void XN_CALLBACK_TYPE PoseDetectionStartBundleCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, void* pCookie)
		{
			PoseCookie* pPoseCookie = (PoseCookie*)pCookie;
			PoseDetectionCapability cap(hNode);
			if (pPoseCookie->startHandler != NULL)
			{
				pPoseCookie->startHandler(cap, strPose, user, pPoseCookie->pPoseCookie);
			}
		}

		static void XN_CALLBACK_TYPE PoseDetectionStartEndBundleCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, void* pCookie)
		{
			PoseCookie* pPoseCookie = (PoseCookie*)pCookie;
			PoseDetectionCapability cap(hNode);
			if (pPoseCookie->endHandler != NULL)
			{
				pPoseCookie->endHandler(cap, strPose, user, pPoseCookie->pPoseCookie);
			}
		}
		typedef struct PoseDetectionCookie
		{
			PoseDetection handler;
			void* pPoseCookie;
			XnCallbackHandle hCallback;
		} PoseDetectionCookie;
		static void XN_CALLBACK_TYPE PoseDetectionCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, void* pCookie)
		{
			PoseDetectionCookie* pPoseDetectionCookie = (PoseDetectionCookie*)pCookie;
			PoseDetectionCapability cap(hNode);
			if (pPoseDetectionCookie->handler != NULL)
			{
				pPoseDetectionCookie->handler(cap, strPose, user, pPoseDetectionCookie->pPoseCookie);
			}
		}

		typedef struct PoseInProgressCookie
		{
			PoseInProgress handler;
			void* pPoseCookie;
			XnCallbackHandle hCallback;
		} PoseInProgressCookie;

		static void XN_CALLBACK_TYPE PoseDetectionInProgressCallback(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, XnPoseDetectionStatus poseErrors, void* pCookie)
		{
			PoseInProgressCookie* pPoseCookie = (PoseInProgressCookie*)pCookie;
			PoseDetectionCapability cap(hNode);
			if (pPoseCookie->handler != NULL)
			{
				pPoseCookie->handler(cap, strPose, user, poseErrors, pPoseCookie->pPoseCookie);
			}
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
		inline UserGenerator(const NodeWrapper& other) : Generator(other) {}

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
			return xnGetNumberOfUsers(GetHandle());
		}

		/** @copybrief xnGetUsers
		 * For full details and usage, see @ref xnGetUsers
		 */
		inline XnStatus GetUsers(XnUserID aUsers[], XnUInt16& nUsers) const
		{
			return xnGetUsers(GetHandle(), aUsers, &nUsers);
		}

		/** @copybrief xnGetUserCoM
		 * For full details and usage, see @ref xnGetUserCoM
		 */
		inline XnStatus GetCoM(XnUserID user, XnPoint3D& com) const
		{
			return xnGetUserCoM(GetHandle(), user, &com);
		}

		/** @copybrief xnGetUserPixels
		 * For full details and usage, see @ref xnGetUserPixels
		 */
		inline XnStatus GetUserPixels(XnUserID user, SceneMetaData& smd) const
		{
			return xnGetUserPixels(GetHandle(), user, smd.GetUnderlying());
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

			nRetVal = xnRegisterUserCallbacks(GetHandle(), NewUserCallback, LostUserCallback, pUserCookie, &pUserCookie->hCallback);
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
			xnUnregisterUserCallbacks(GetHandle(), pUserCookie->hCallback);
			xnOSFree(pUserCookie);
		}

		/**
		 * Gets an @ref SkeletonCapability object for accessing Skeleton functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_SKELETON is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const SkeletonCapability GetSkeletonCap() const
		{
			return SkeletonCapability(GetHandle());
		}

		/**
		 * Gets an @ref SkeletonCapability object for accessing Skeleton functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_SKELETON is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline SkeletonCapability GetSkeletonCap()
		{
			return SkeletonCapability(GetHandle());
		}

		/**
		 * Gets an @ref PoseDetectionCapability object for accessing Pose-Detection functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_POSE_DETECTION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const PoseDetectionCapability GetPoseDetectionCap() const
		{
			return PoseDetectionCapability(GetHandle());
		}

		/**
		 * Gets an @ref PoseDetectionCapability object for accessing Pose-Detection functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_POSE_DETECTION is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline PoseDetectionCapability GetPoseDetectionCap()
		{
			return PoseDetectionCapability(GetHandle());
		}

		/** @copybrief xnRegisterToUserExit
		 * For full details and usage, see @ref xnRegisterToUserExit
		 */
		inline XnStatus RegisterToUserExit(UserHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			UserSingleCookie* pUserCookie;
			XN_VALIDATE_ALLOC(pUserCookie, UserSingleCookie);
			pUserCookie->handler = handler;
			pUserCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterToUserExit(GetHandle(), UserSingleCallback, pUserCookie, &pUserCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pUserCookie);
				return (nRetVal);
			}

			hCallback = pUserCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterFromUserExit
		 * For full details and usage, see @ref xnUnregisterFromUserExit
		 */
		inline void UnregisterFromUserExit(XnCallbackHandle hCallback)
		{
			UserSingleCookie* pUserCookie = (UserSingleCookie*)hCallback;
			xnUnregisterFromUserExit(GetHandle(), pUserCookie->hCallback);
			xnOSFree(pUserCookie);
		}

		/** @copybrief xnRegisterToUserReEnter
		 * For full details and usage, see @ref xnRegisterToUserReEnter
		 */
		inline XnStatus RegisterToUserReEnter(UserHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			UserSingleCookie* pUserCookie;
			XN_VALIDATE_ALLOC(pUserCookie, UserSingleCookie);
			pUserCookie->handler = handler;
			pUserCookie->pUserCookie = pCookie;

			nRetVal = xnRegisterToUserReEnter(GetHandle(), UserSingleCallback, pUserCookie, &pUserCookie->hCallback);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pUserCookie);
				return (nRetVal);
			}

			hCallback = pUserCookie;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnUnregisterFromUserReEnter
		 * For full details and usage, see @ref xnUnregisterFromUserReEnter
		 */
		inline void UnregisterFromUserReEnter(XnCallbackHandle hCallback)
		{
			UserSingleCookie* pUserCookie = (UserSingleCookie*)hCallback;
			xnUnregisterFromUserReEnter(GetHandle(), pUserCookie->hCallback);
			xnOSFree(pUserCookie);
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
			if (pUserCookie->newHandler != NULL)
			{
				pUserCookie->newHandler(gen, user, pUserCookie->pUserCookie);
			}
		}

		static void XN_CALLBACK_TYPE LostUserCallback(XnNodeHandle hNode, XnUserID user, void* pCookie)
		{
			UserCookie* pUserCookie = (UserCookie*)pCookie;
			UserGenerator gen(hNode);
			if (pUserCookie->lostHandler != NULL)
			{
				pUserCookie->lostHandler(gen, user, pUserCookie->pUserCookie);
			}
		}

		typedef struct UserSingleCookie
		{
			UserHandler handler;
			void* pUserCookie;
			XnCallbackHandle hCallback;
		} UserSingleCookie;

		static void XN_CALLBACK_TYPE UserSingleCallback(XnNodeHandle hNode, XnUserID user, void* pCookie)
		{
			UserSingleCookie* pUserCookie = (UserSingleCookie*)pCookie;
			UserGenerator gen(hNode);
			if (pUserCookie->handler != NULL)
			{
				pUserCookie->handler(gen, user, pUserCookie->pUserCookie);
			}
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
		inline AudioGenerator(const NodeWrapper& other) : Generator(other) {}

		/** @copybrief xnCreateAudioGenerator
		 * For full details and usage, see @ref xnCreateAudioGenerator
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/** @copybrief xnGetAudioMetaData
		 * For full details and usage, see @ref xnGetAudioMetaData
		 */
		inline void GetMetaData(AudioMetaData& metaData) const
		{
			xnGetAudioMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/** @copybrief xnGetAudioBuffer
		 * For full details and usage, see @ref xnGetAudioBuffer
		 */
		inline const XnUChar* GetAudioBuffer() const
		{
			return xnGetAudioBuffer(GetHandle());
		}

		/** @copybrief xnGetSupportedWaveOutputModesCount
		 * For full details and usage, see @ref xnGetSupportedWaveOutputModesCount
		 */
		inline XnUInt32 GetSupportedWaveOutputModesCount() const
		{
			return xnGetSupportedWaveOutputModesCount(GetHandle());
		}

		/** @copybrief xnGetSupportedWaveOutputModes
		 * For full details and usage, see @ref xnGetSupportedWaveOutputModes
		 */
		inline XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode* aSupportedModes, XnUInt32& nCount) const
		{
			return xnGetSupportedWaveOutputModes(GetHandle(), aSupportedModes, &nCount);
		}

		/** @copybrief xnSetWaveOutputMode
		 * For full details and usage, see @ref xnSetWaveOutputMode
		 */
		inline XnStatus SetWaveOutputMode(const XnWaveOutputMode& OutputMode)
		{
			return xnSetWaveOutputMode(GetHandle(), &OutputMode);
		}

		/** @copybrief xnGetWaveOutputMode
		 * For full details and usage, see @ref xnGetWaveOutputMode
		 */
		inline XnStatus GetWaveOutputMode(XnWaveOutputMode& OutputMode) const
		{
			return xnGetWaveOutputMode(GetHandle(), &OutputMode);
		}

		/** @copybrief xnRegisterToWaveOutputModeChanges
		 * For full details and usage, see @ref xnRegisterToWaveOutputModeChanges
		 */
		inline XnStatus RegisterToWaveOutputModeChanges(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToWaveOutputModeChanges, GetHandle(), handler, pCookie, hCallback);
		}

		/** @copybrief xnUnregisterFromWaveOutputModeChanges
		 * For full details and usage, see @ref xnUnregisterFromWaveOutputModeChanges
		 */
		inline void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromWaveOutputModeChanges, GetHandle(), hCallback);
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
		inline MockAudioGenerator(const NodeWrapper& other) : AudioGenerator(other) {}

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
			return xnMockAudioSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pAudioBuffer);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and timestamp
		 *
		 * @param	audioMD			[in]	Object to take data from
		 * @param	nFrameID		[in]	Frame ID
		 * @param	nTimestamp		[in]	Timestamp
		 */
		inline XnStatus SetData(const AudioMetaData& audioMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, audioMD.DataSize(), audioMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object
		 *
		 * @param	audioMD			[in]	Object to take data from
		 */
		inline XnStatus SetData(const AudioMetaData& audioMD)
		{
			return SetData(audioMD, audioMD.FrameID(), audioMD.Timestamp());
		}
	};

	class MockRawGenerator : public Generator
	{
	public:
		MockRawGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}
		MockRawGenerator(const NodeWrapper& other) : Generator(other) {}

		inline XnStatus Create(Context& context, const XnChar* strName = NULL);

		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const void* pData)
		{
			return xnMockRawSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pData);
		}

	};

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
		inline Codec(const NodeWrapper& other) : ProductionNode(other) {}

		/** @copybrief xnCreateCodec
		 * For full details and usage, see @ref xnCreateCodec
		 */
		inline XnStatus Create(Context& context, XnCodecID codecID, ProductionNode& initializerNode);

		/** @copybrief xnGetCodecID
		 * For full details and usage, see @ref xnGetCodecID
		 */
		inline XnCodecID GetCodecID() const
		{
			return xnGetCodecID(GetHandle());
		}

		/** @copybrief xnEncodeData
		 * For full details and usage, see @ref xnEncodeData
		 */
		inline XnStatus EncodeData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
		{
			return xnEncodeData(GetHandle(), pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
		}

		/** @copybrief xnDecodeData
		 * For full details and usage, see @ref xnDecodeData
		 */
		inline XnStatus DecodeData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
		{
			return xnDecodeData(GetHandle(), pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
		}
	};

	/**
	 * @ingroup cppref
	 * Represents a Script node.
	 */
	class ScriptNode : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param	hNode		[in]	Node handle
		 */
		inline ScriptNode(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
		inline ScriptNode(const NodeWrapper& other) : ProductionNode(other) {}

		inline XnStatus Create(Context& context, const XnChar* strFormat);

		inline const XnChar* GetSupportedFormat()
		{
			return xnScriptNodeGetSupportedFormat(GetHandle());
		}

		inline XnStatus LoadScriptFromFile(const XnChar* strFileName)
		{
			return xnLoadScriptFromFile(GetHandle(), strFileName);
		}

		inline XnStatus LoadScriptFromString(const XnChar* strScript)
		{
			return xnLoadScriptFromString(GetHandle(), strScript);
		}

		inline XnStatus Run(EnumerationErrors* pErrors);
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
		inline Context() : m_pContext(NULL), m_bUsingDeprecatedAPI(FALSE), m_bAllocated(FALSE), m_hShuttingDownCallback(NULL) {}

		/**
		 * Ctor
		 *
		 * @param	pContext		[in]	Underlying C object
		 */
		inline Context(XnContext* pContext) : m_pContext(NULL), m_bUsingDeprecatedAPI(FALSE), m_bAllocated(FALSE), m_hShuttingDownCallback(NULL)
		{
			SetHandle(pContext);
		}

		/**
		 * Copy Ctor
		 *
		 * @param	other		[in]	Another context. Note that the context will only be destroyed when
		 *								The original object is destroyed.
		 */
		inline Context(const Context& other) : m_pContext(NULL), m_bUsingDeprecatedAPI(FALSE), m_bAllocated(FALSE), m_hShuttingDownCallback(NULL)
		{
			SetHandle(other.m_pContext);
		}

		/// Dtor
		~Context() 
		{ 
			SetHandle(NULL);
		}

		inline Context& operator=(const Context& other)
		{
			SetHandle(other.m_pContext);
			return *this;
		}

		/// Gets the underlying C object
		inline XnContext* GetUnderlyingObject() const { return m_pContext; }

		/** @copybrief xnInit
		 * For full details and usage, see @ref xnInit
		 */
		inline XnStatus Init()
		{
			XnContext* pContext = NULL;
			XnStatus nRetVal = xnInit(&pContext);
			XN_IS_STATUS_OK(nRetVal);

			TakeOwnership(pContext);
			m_bAllocated = TRUE;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnContextRunXmlScript
		 * For full details and usage, see @ref xnContextRunXmlScript
		 */
		inline XnStatus XN_API_DEPRECATED("Use other overload!") RunXmlScript(const XnChar* strScript, EnumerationErrors* pErrors = NULL)
		{
			m_bUsingDeprecatedAPI = TRUE;
			#pragma warning (push)
			#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			return xnContextRunXmlScript(m_pContext, strScript, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			#pragma warning (pop)
		}

		/** @copybrief xnContextRunXmlScript
		 * For full details and usage, see @ref xnContextRunXmlScript
		 */
		inline XnStatus RunXmlScript(const XnChar* strScript, ScriptNode& scriptNode, EnumerationErrors* pErrors = NULL)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			XnNodeHandle hScriptNode;
			nRetVal = xnContextRunXmlScriptEx(m_pContext, strScript, pErrors == NULL ? NULL : pErrors->GetUnderlying(), &hScriptNode);
			XN_IS_STATUS_OK(nRetVal);

			scriptNode.TakeOwnership(hScriptNode);
			
			return (XN_STATUS_OK);
		}

		/** @copybrief xnContextRunXmlScriptFromFile
		 * For full details and usage, see @ref xnContextRunXmlScriptFromFile
		 */
		inline XnStatus XN_API_DEPRECATED("Use other overload!") RunXmlScriptFromFile(const XnChar* strFileName, EnumerationErrors* pErrors = NULL)
		{
			m_bUsingDeprecatedAPI = TRUE;
			#pragma warning (push)
			#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			return xnContextRunXmlScriptFromFile(m_pContext, strFileName, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			#pragma warning (pop)
		}

		/** @copybrief xnContextRunXmlScriptFromFileEx
		 * For full details and usage, see @ref xnContextRunXmlScriptFromFileEx
		 */
		inline XnStatus RunXmlScriptFromFile(const XnChar* strFileName, ScriptNode& scriptNode, EnumerationErrors* pErrors = NULL)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hScriptNode;
			nRetVal = xnContextRunXmlScriptFromFileEx(m_pContext, strFileName, pErrors == NULL ? NULL : pErrors->GetUnderlying(), &hScriptNode);
			XN_IS_STATUS_OK(nRetVal);

			scriptNode.TakeOwnership(hScriptNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnInitFromXmlFile
		 * For full details and usage, see @ref xnInitFromXmlFile
		 */
		inline XnStatus XN_API_DEPRECATED("Use other overload!") InitFromXmlFile(const XnChar* strFileName, EnumerationErrors* pErrors = NULL)
		{
			XnContext* pContext = NULL;
			m_bUsingDeprecatedAPI = TRUE;

			#pragma warning (push)
			#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			XnStatus nRetVal = xnInitFromXmlFile(strFileName, &pContext, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			#pragma warning (pop)
			XN_IS_STATUS_OK(nRetVal);

			TakeOwnership(pContext);
			m_bAllocated = TRUE;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnInitFromXmlFile
		 * For full details and usage, see @ref xnInitFromXmlFile
		 */
		inline XnStatus InitFromXmlFile(const XnChar* strFileName, ScriptNode& scriptNode, EnumerationErrors* pErrors = NULL)
		{
			XnContext* pContext = NULL;

			XnNodeHandle hScriptNode;
			XnStatus nRetVal = xnInitFromXmlFileEx(strFileName, &pContext, pErrors == NULL ? NULL : pErrors->GetUnderlying(), &hScriptNode);
			XN_IS_STATUS_OK(nRetVal);

			scriptNode.TakeOwnership(hScriptNode);
			TakeOwnership(pContext);
			m_bAllocated = TRUE;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnContextOpenFileRecording
		 * For full details and usage, see @ref xnContextOpenFileRecording
		 */
		inline XnStatus XN_API_DEPRECATED("Use other overload!") OpenFileRecording(const XnChar* strFileName)
		{
			m_bUsingDeprecatedAPI = TRUE;
			#pragma warning (push)
			#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			return xnContextOpenFileRecording(m_pContext, strFileName);
			#pragma warning (pop)
		}

		/** @copybrief xnContextOpenFileRecording
		 * For full details and usage, see @ref xnContextOpenFileRecording
		 */
		inline XnStatus OpenFileRecording(const XnChar* strFileName, ProductionNode& playerNode)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			XnNodeHandle hPlayer;
			nRetVal = xnContextOpenFileRecordingEx(m_pContext, strFileName, &hPlayer);
			XN_IS_STATUS_OK(nRetVal);

			playerNode.TakeOwnership(hPlayer);
			
			return (XN_STATUS_OK);
		}

		/** @copybrief xnCreateMockNode
		 * For full details and usage, see @ref xnCreateMockNode
		 */
		inline XnStatus CreateMockNode(XnProductionNodeType type, const XnChar* strName, ProductionNode& mockNode)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			XnNodeHandle hMockNode;
			nRetVal = xnCreateMockNode(m_pContext, type, strName, &hMockNode);
			XN_IS_STATUS_OK(nRetVal);

			mockNode.TakeOwnership(hMockNode);
			
			return (XN_STATUS_OK);
		}

		/** @copybrief xnCreateMockNodeBasedOn
		 * For full details and usage, see @ref xnCreateMockNodeBasedOn
		 */
		inline XnStatus CreateMockNodeBasedOn(ProductionNode& originalNode, const XnChar* strName, ProductionNode& mockNode)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hMockNode;
			nRetVal = xnCreateMockNodeBasedOn(m_pContext, originalNode, strName, &hMockNode);
			XN_IS_STATUS_OK(nRetVal);

			mockNode.TakeOwnership(hMockNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnCreateCodec
		 * For full details and usage, see @ref xnCreateCodec
		 */
		inline XnStatus CreateCodec(XnCodecID codecID, ProductionNode& initializerNode, Codec& codec)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hCodec;
			nRetVal =  xnCreateCodec(m_pContext, codecID, initializerNode.GetHandle(), &hCodec);
			XN_IS_STATUS_OK(nRetVal);

			codec.TakeOwnership(hCodec);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnContextAddRef
		 * For full details and usage, see @ref xnContextAddRef
		 */
		inline XnStatus AddRef()
		{
			return xnContextAddRef(m_pContext);
		}

		/** @copybrief xnContextRelease
		 * For full details and usage, see @ref xnContextRelease
		 */
		inline void Release()
		{
			SetHandle(NULL);
		}

		/** @copybrief xnShutdown
		 * For full details and usage, see @ref xnShutdown
		 */
		inline void XN_API_DEPRECATED("You may use Release() instead, or count on dtor") Shutdown()
		{
			if (m_pContext != NULL)
			{
				#pragma warning (push)
				#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
				xnShutdown(m_pContext);
				#pragma warning (pop)
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
			xnFreeLicensesList(aLicenses);
		}

		/** @copybrief xnEnumerateProductionTrees
		 * For full details and usage, see @ref xnEnumerateProductionTrees
		 */
		XnStatus EnumerateProductionTrees(XnProductionNodeType Type, const Query* pQuery, NodeInfoList& TreesList, EnumerationErrors* pErrors = NULL) const
		{
			XnStatus nRetVal = XN_STATUS_OK;

			const XnNodeQuery* pInternalQuery = (pQuery != NULL) ? pQuery->GetUnderlyingObject() : NULL;

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

			node.TakeOwnership(hNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnCreateProductionTree
		 * For full details and usage, see @ref xnCreateProductionTree
		 */
		XnStatus XN_API_DEPRECATED("Please use other overload") CreateProductionTree(NodeInfo& Tree)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnCreateProductionTree(m_pContext, Tree, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			Tree.m_bOwnerOfNode = TRUE;

			return (XN_STATUS_OK);
		}

		/** @copybrief xnCreateProductionTree
		 * For full details and usage, see @ref xnCreateProductionTree
		 */
		XnStatus CreateProductionTree(NodeInfo& Tree, ProductionNode& node)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnCreateProductionTree(m_pContext, Tree, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			node.TakeOwnership(hNode);

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
			nRetVal = xnFindExistingRefNodeByType(m_pContext, type, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			node.TakeOwnership(hNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnGetNodeHandleByName
		 * For full details and usage, see @ref xnGetNodeHandleByName
		 */
		XnStatus GetProductionNodeByName(const XnChar* strInstanceName, ProductionNode& node) const
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnGetRefNodeHandleByName(m_pContext, strInstanceName, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			node.TakeOwnership(hNode);

			return (XN_STATUS_OK);
		}

		/** @copybrief xnGetNodeHandleByName
		 * For full details and usage, see @ref xnGetNodeHandleByName
		 */
		XnStatus GetProductionNodeInfoByName(const XnChar* strInstanceName, NodeInfo& nodeInfo) const
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnGetRefNodeHandleByName(m_pContext, strInstanceName, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			xnProductionNodeRelease(hNode);

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
			xnUnregisterFromGlobalErrorStateChange(m_pContext, hCallback);
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
			return xnWaitOneUpdateAll(m_pContext, node.GetHandle());
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
			if (m_pContext == pContext)
			{
				return;
			}

			if (m_pContext != NULL)
			{
				if (m_bUsingDeprecatedAPI && m_bAllocated)
				{
					// Backwards compatibility: call shutdown instead of release, to make old programs get the 
					// exact same behavior they used to have.
					xnForceShutdown(m_pContext);
				}
				else
				{
					xnContextUnregisterFromShutdown(m_pContext, m_hShuttingDownCallback);
					xnContextRelease(m_pContext);
				}
			}

			if (pContext != NULL)
			{
				XnStatus nRetVal = xnContextAddRef(pContext);
				XN_ASSERT(nRetVal == XN_STATUS_OK);

				nRetVal = xnContextRegisterForShutdown(pContext, ContextShuttingDownCallback, this, &m_hShuttingDownCallback);
				XN_ASSERT(nRetVal == XN_STATUS_OK);
			}

			m_pContext = pContext;
		}

		inline void TakeOwnership(XnContext* pContext)
		{
			SetHandle(pContext);

			if (pContext != NULL)
			{
				xnContextRelease(pContext);
			}
		}

	private:
		static void XN_CALLBACK_TYPE ContextShuttingDownCallback(XnContext* /*pContext*/, void* pCookie)
		{
			Context* pThis = (Context*)pCookie;
			pThis->m_pContext = NULL;
		}

		XnContext* m_pContext;
		XnBool m_bUsingDeprecatedAPI;
		XnBool m_bAllocated;
		XnCallbackHandle m_hShuttingDownCallback;
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

	inline void ProductionNode::GetContext(Context& context) const
	{
		context.TakeOwnership(xnGetRefContextFromNodeHandle(GetHandle()));
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

		m_bOwnerOfNode = FALSE;
		m_pInfo = pInfo;
		m_pNeededNodes = NULL;
	}

	inline XnBool FrameSyncCapability::CanFrameSyncWith(Generator& other) const
	{
		return xnCanFrameSyncWith(GetHandle(), other.GetHandle());
	}

	inline XnStatus FrameSyncCapability::FrameSyncWith(Generator& other)
	{
		return xnFrameSyncWith(GetHandle(), other.GetHandle());
	}

	inline XnStatus FrameSyncCapability::StopFrameSyncWith(Generator& other)
	{
		return xnStopFrameSyncWith(GetHandle(), other.GetHandle());
	}

	inline XnBool FrameSyncCapability::IsFrameSyncedWith(Generator& other) const
	{
		return xnIsFrameSyncedWith(GetHandle(), other.GetHandle());
	}

	inline XnStatus NodeInfo::GetInstance(ProductionNode& node) const
	{
		if (m_pInfo == NULL)
		{
			return XN_STATUS_INVALID_OPERATION;
		}

		XnNodeHandle hNode = xnNodeInfoGetRefHandle(m_pInfo);
		node.TakeOwnership(hNode);

		if (m_bOwnerOfNode)
		{
			xnProductionNodeRelease(hNode);
		}

		return (XN_STATUS_OK);
	}

	//---------------------------------------------------------------------------
	// Node creation functions
	//---------------------------------------------------------------------------

	inline XnStatus Device::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateDevice(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus Recorder::Create(Context& context, const XnChar* strFormatName /*= NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateRecorder(context.GetUnderlyingObject(), strFormatName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus Player::Create(Context& context, const XnChar* strFormatName)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreatePlayer(context.GetUnderlyingObject(), strFormatName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus DepthGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateDepthGenerator(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockDepthGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_DEPTH, strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockDepthGenerator::CreateBasedOn(DepthGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other.GetHandle(), strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus ImageGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateImageGenerator(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockImageGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_IMAGE, strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockImageGenerator::CreateBasedOn(ImageGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other.GetHandle(), strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus IRGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateIRGenerator(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}
	
	inline XnStatus MockIRGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_IR, strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockIRGenerator::CreateBasedOn(IRGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other.GetHandle(), strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus GestureGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateGestureGenerator(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus SceneAnalyzer::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		//You're creating a scene!
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateSceneAnalyzer(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus HandsGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateHandsGenerator(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus UserGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateUserGenerator(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus AudioGenerator::Create(Context& context, Query* pQuery/*=NULL*/, EnumerationErrors* pErrors/*=NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateAudioGenerator(context.GetUnderlyingObject(), &hNode, pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockAudioGenerator::Create(Context& context, const XnChar* strName /* = NULL */)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_AUDIO, strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockAudioGenerator::CreateBasedOn(AudioGenerator& other, const XnChar* strName /* = NULL */)
	{
		Context context;
		other.GetContext(context);
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNodeBasedOn(context.GetUnderlyingObject(), other.GetHandle(), strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus MockRawGenerator::Create(Context& context, const XnChar* strName /*= NULL*/)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateMockNode(context.GetUnderlyingObject(), XN_NODE_TYPE_GENERATOR, strName, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus Codec::Create(Context& context, XnCodecID codecID, ProductionNode& initializerNode)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateCodec(context.GetUnderlyingObject(), codecID, initializerNode.GetHandle(), &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
	}

	inline XnStatus ScriptNode::Run(EnumerationErrors* pErrors)
	{
		return xnScriptNodeRun(GetHandle(), pErrors == NULL ? NULL : pErrors->GetUnderlying());
	}

	inline XnStatus ScriptNode::Create(Context& context, const XnChar* strFormat)
	{
		XnNodeHandle hNode;
		XnStatus nRetVal = xnCreateScriptNode(context.GetUnderlyingObject(), strFormat, &hNode);
		XN_IS_STATUS_OK(nRetVal);
		TakeOwnership(hNode);
		return (XN_STATUS_OK);
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
		friend class GeneralIntCapability;

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

	inline XnStatus GeneralIntCapability::RegisterToValueChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		StateChangedCallbackTranslator* pTrans;
		XN_VALIDATE_NEW(pTrans, StateChangedCallbackTranslator, handler, pCookie);

		nRetVal = xnRegisterToGeneralIntValueChange(GetHandle(), m_strCap, pTrans->StateChangedCallback, pTrans, &pTrans->m_hCallback);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(pTrans);
			return (nRetVal);
		}

		hCallback = pTrans;

		return (XN_STATUS_OK);
	}

	inline void GeneralIntCapability::UnregisterFromValueChange(XnCallbackHandle hCallback)
	{
		StateChangedCallbackTranslator* pTrans = (StateChangedCallbackTranslator*)hCallback;
		xnUnregisterFromGeneralIntValueChange(GetHandle(), m_strCap, pTrans->m_hCallback);
		XN_DELETE(pTrans);
	}

	/// @}
};

#endif // __XN_CPP_WRAPPER_H__
