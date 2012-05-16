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
	 * @defgroup cppref_glb_ev_hndlrs Global Event Handlers
	 */
	 
	/**
	 * @ingroup cppref
	 * @defgroup cppref_graph_mgmt Managing the Production Graph
	 */
	 
	/**
	 * @ingroup cppref
	 * @defgroup cppref_meta_data Meta-Data Objects (Frame Objects)
	 */
	 
	/**
	 * @ingroup cppref
	 * @defgroup cppref_prd_func Production Nodes Functionality
	 */
	 
	/**
	 * @ingroup cppref
	 * @defgroup cppref_misc Misc
	 */

	//---------------------------------------------------------------------------
	// Types
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref_glb_ev_hndlrs
	 * Type definition to define event handlers for the 'State Change' event.
	 *
	 * @param [in]	node		Node issuing the event.
	 * @param [in]	pCookie		User cookie passed on registration.
	 *
	 * <b>Remarks</b>
	 *
	 * The 'State Change' event can be invoked by a number of OpenNI objects.
	 * For example, this event is invoked by the @ref xn::ErrorStateCapability "ErrorStateCapability"
	 * object whenever a node's  error state changes. As another example, this event is
	 * also invoked by the @ref xn::MirrorCapability "MirrorCapability" object whenever a node's
	 * mirror configuration has been changed.
	 *
	 * <b>Example:</b>
	 *
	 * The following is an example of a callback method that follows that signature:
	 *
	@code
	void XN_CALLBACK_TYPE OnEvent(ProductionNode& node, void* pCookie)
	{
	}
	@endcode
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
	/**
	 * @ingroup cppref_misc
	 * <b>Purpose:</b>  The Version class provides functionality for accessing the OpenNI version information.
	 */
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

		static Version Current()
		{
			XnVersion version;
			xnGetVersion(&version);
			return Version(version);
		}

		XnUInt8 Major() const { return m_version.nMajor; }
		XnUInt8 Minor() const { return m_version.nMinor; }
		XnUInt16 Maintenance() const { return m_version.nMaintenance; }
		XnUInt32 Build() const { return m_version.nBuild; }

		XnUInt8& Major() { return m_version.nMajor; }
		XnUInt8& Minor() { return m_version.nMinor; }
		XnUInt16& Maintenance() { return m_version.nMaintenance; }
		XnUInt32& Build() { return m_version.nBuild; }

		const XnVersion* GetUnderlying() const { return &m_version; }
		XnVersion* GetUnderlying() { return &m_version; }

	private:
		XnVersion m_version;
	};

	//---------------------------------------------------------------------------
	// Meta Data
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref_meta_data
	 * <b>Purpose:</b> The OutputMetaData class provides functionality for supporting fast
	 * cached data access to many types of generator nodes.
	 *
	 * <b>Remarks:</b>
	 *
	 * The OutputMetaData class is the most basic OpenNI metadata class, whose main properties
	 * include  <code>Timestamp</code>, <code>FrameID</code>, and <code>DataSize</code>.
	 * This class is never instantiated; it is used a base class to help build the hierarchy.
	 *
	 * An instance of this class hold a @ref glos_frame_object "frame object".
	 * The frame object is a snapshot of the generated data at a certain point in time.
	 */
	class OutputMetaData
	{
	public:
		/**
		 * Ctor. Intended for use by other MetaData objects.
		 *
		 * @param [in]	ppData	A pointer to the data member of the meta data object.
		 */
		inline OutputMetaData(const XnUInt8** ppData) : m_ppData(ppData), m_nAllocatedSize(0), m_pAllocatedData(NULL)
		{
			xnOSMemSet(&m_output, 0, sizeof(XnOutputMetaData));
		}

		/**
		 * Dtor. Frees any allocated data.
		 */
		virtual ~OutputMetaData() { Free(); }

		/**
		 * Returns the timestamp of the @ref glos_frame_object "frame object" saved in this object.
		 */
		inline XnUInt64 Timestamp() const { return m_output.nTimestamp; }

		/**
		 * Returns the timestamp of the @ref glos_frame_object "frame object" saved in this object.
		 */
		inline XnUInt64& Timestamp() { return m_output.nTimestamp; }

		/**
		 * Returns the frame ID of the @ref glos_frame_object "frame object" saved in this object.
		 * Frame IDs are sequential unique numbers starting with 1.
		 */
		inline XnUInt32 FrameID() const { return m_output.nFrameID; }

		/**
		 * Returns the frame ID of the @ref glos_frame_object "frame object" saved in this object.
		 * Frame IDs are sequential unique numbers starting with 1.
		 */
		inline XnUInt32& FrameID() { return m_output.nFrameID; }

		/**
		 * @brief Returns the size, in bytes, of the data of the @ref glos_frame_object "frame object" saved in this object.
		 *
		 * <b>Remarks</b>
		 *
		 * For map metadata, this value equals to
		 * @ref xn::MapMetaData::XRes "XRes()" * @ref xn::MapMetaData::YRes "YRes()" * @ref xn::MapMetaData::BytesPerPixel "BytesPerPixel()".
		 */
		inline XnUInt32 DataSize() const { return m_output.nDataSize; }

		/**
		 * @brief Returns the size, in bytes, of the data of the @ref glos_frame_object "frame object" saved in this object.
		 *
		 * <b>Remarks</b>
		 *
		 * For map metadata, this value equals to
		 * @ref xn::MapMetaData::XRes "XRes()" * @ref xn::MapMetaData::YRes "YRes()" * @ref xn::MapMetaData::BytesPerPixel "BytesPerPixel()".
		 */
		inline XnUInt32& DataSize() { return m_output.nDataSize; }

		/**
		 * Returns whether this node's frame data was updated by the most recent call to
		 * any @ref conc_updating_data "'WaitXUpdateAll()'" function, e.g., @ref xn::Context::WaitAnyUpdateAll().
		 */
		inline XnBool IsDataNew() const { return m_output.bIsNew; }

		/**
		 * Returns whether this node's frame data was updated by the most recent call to
		 * any @ref conc_updating_data "'WaitXUpdateAll()'" function, e.g., @ref xn::Context::WaitAnyUpdateAll().
		 */
		inline XnBool& IsDataNew() { return m_output.bIsNew; }

		/**
		 * Gets the C object that is wrapped by this object.
		 */
		inline const XnOutputMetaData* GetUnderlying() const { return &m_output; }
		/**
		 * Gets the C object that is wrapped by this object.
		 */
		inline XnOutputMetaData* GetUnderlying() { return &m_output; }

		/**
		 * Gets a pointer to the data of the @ref glos_frame_object "frame object"
		 * saved in this object.
		 */
		inline const XnUInt8* Data() const { return *m_ppData; }
		/**
		 * Gets a pointer to the data of the @ref glos_frame_object "frame object"
		 * saved in this object.
		 */
		inline const XnUInt8*& Data() { return *m_ppData; }

		/**
		 * Gets a pointer to the writable buffer (see @ref conc_meta_data).
		 */
		inline XnUInt8* WritableData()
		{
			MakeDataWritable();
			return m_pAllocatedData;
		}

		/**
		 * @brief Allocates a writable buffer. If a previous buffer was allocated it will
		 * be freed (or reused if possible).
		 *
		 * Also see @ref conc_meta_data.
		 *
		 * @param [in]	nBytes	The requested size for the buffer, in bytes.
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
		 * Creates a writable copy of the data, and stores it instead of the original (read-only) data.
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
	 * @ingroup cppref_meta_data
	 * The MapMetaData class is a base class for providing the fast data access functionality
	 * for the @ref glos_frame_object "frame object" saved in metadata. This functionality
	 * is applicable to all generator nodes that have an associated metadata object.
	 *
	 * MapMetaData is the next metadata class in the metadata hierarchy after the OutputMetaData
	 * class.
	 *
	 * The MapMetaData class - like OutputMetaData - is never instantiated;
	 * it is  a base class of the OpenNI metadata class hierarchy.
	 */
	class MapMetaData : public OutputMetaData
	{
	public:
		/**
		 * Ctor. Intended for use by inheriting classes.
		 *
		 * @param [in]	format	Pixel Format
		 * @param [in]	ppData	A pointer to the data member of the meta data object
		 */
		inline MapMetaData(XnPixelFormat format, const XnUInt8** ppData) : OutputMetaData(ppData)
		{
			xnOSMemSet(&m_map, 0, sizeof(XnMapMetaData));
			m_map.pOutput = OutputMetaData::GetUnderlying();
			m_map.PixelFormat = format;
		}

		/**
		 * Gets the frame X resolution of the @ref glos_frame_object "frame object" saved in this object.
		 * The X resolution is the actual number of columns in the frame after any required cropping has
		 * been applied.
		 */
		inline XnUInt32 XRes() const { return m_map.Res.X; }
		/**
		 * Gets the frame X resolution of the @ref glos_frame_object "frame object" saved in this object.
		 * The X resolution is the actual number of columns in the frame after any required cropping has
		 * been applied.
		 */
		inline XnUInt32& XRes() { return m_map.Res.X; }

		/**
		 * Gets the frame Y resolution of the @ref glos_frame_object "frame object" saved in this object.
		 * The Y resolution is the actual number of rows in the frame after any required cropping has
		 * been applied.
		 */
		inline XnUInt32 YRes() const { return m_map.Res.Y; }
		/**
		 * Gets the frame Y resolution of the @ref glos_frame_object "frame object" saved in this object.
		 * The Y resolution is the actual number of rows in the frame after any required cropping has
		 * been applied.
		 */
		inline XnUInt32& YRes() { return m_map.Res.Y; }

		/**
		 * Gets the cropping area X offset of the @ref glos_frame_object "frame object" saved in this object.
		 * This is the horizontal offset, measured in units of pixel columns, of the buffer within the field of view.
		 *
		 * <b>Remarks</b>
		 *
		 * The X offset is 0 if cropping is disabled (see @ref XnCropping).
		 */
		inline XnUInt32 XOffset() const { return m_map.Offset.X; }
		/**
		 * Gets the cropping area X offset of the @ref glos_frame_object "frame object" saved in this object.
		 * This is the horizontal offset, measured in units of pixel columns, of the buffer within the field of view.
		 *
		 * <b>Remarks</b>
		 *
		 * The X offset is 0 if cropping is disabled (see @ref XnCropping).
		 */
		inline XnUInt32& XOffset() { return m_map.Offset.X; }

		/**
		 * Gets the cropping area Y offset of the @ref glos_frame_object "frame object" saved in this object.
		 * This is the vertical offset, measured in units of pixel rows, of the buffer within the field of view.
		 *
		 * <b>Remarks</b>
		 *
		 * The Y offset is 0 if cropping is disabled (see @ref XnCropping).
		 */
		inline XnUInt32 YOffset() const { return m_map.Offset.Y; }
		/**
		 * Gets the cropping area Y offset of the @ref glos_frame_object "frame object" saved in this object.
		 * This is the vertical offset, measured in units of pixel rows, of the buffer within the field of view.
		 *
		 * <b>Remarks</b>
		 *
		 * The Y offset is 0 if cropping is disabled (see @ref XnCropping).
		 */
		inline XnUInt32& YOffset() { return m_map.Offset.Y; }

		/**
		 * Gets the number of columns in the full frame (entire field-of-view, ignoring
		 * cropping).
		 */
		inline XnUInt32 FullXRes() const { return m_map.FullRes.X; }


		/**
		 * Gets the number of columns in the full frame (entire field-of-view, ignoring
		 * cropping).
		 */
		inline XnUInt32& FullXRes() { return m_map.FullRes.X; }

		/**
		 * Gets the number of rows in the full frame (entire field-of-view, ignoring cropping).
		 */
		inline XnUInt32 FullYRes() const { return m_map.FullRes.Y; }
		/**
		 * Gets the number of rows in the full frame (entire field-of-view, ignoring cropping).
		 */
		inline XnUInt32& FullYRes() { return m_map.FullRes.Y; }

		/**
		 * Gets the FPS in which frame was generated.
		 */
		inline XnUInt32 FPS() const { return m_map.nFPS; }
		/**
		 * Gets the FPS in which frame was generated.
		 */
		inline XnUInt32& FPS() { return m_map.nFPS; }

		/**
		 * Gets the pixel format of the pixel-map.
		 */
		inline XnPixelFormat PixelFormat() const { return m_map.PixelFormat; }

		/**
		 * Gets the C object that is wrapped by this object.
		 */
		inline const XnMapMetaData* GetUnderlying() const { return &m_map; }
		/**
		 * Gets the C object that is wrapped by this object.
		 */
		inline XnMapMetaData* GetUnderlying() { return &m_map; }

		/**
		 * Gets the number of bytes each pixel occupies.
		 */
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
		 * @param [in]	nXRes	The number of required columns in the map.
		 * @param [in]	nYRes	The number of required rows in the map.
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
		 * @param [in]	nXRes			Number of columns in the map
		 * @param [in]	nYRes			Number of rows in the map
		 * @param [in]	pExternalBuffer	Optional. An external buffer to be used. If NULL is passed,
		 *								a buffer will be allocated.
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

	/**
	 * @ingroup cppref_meta_data
	 * The Map Wrapper classes are light wrappers for @ref glos_frame_object maps (from metadata classes). These wrapper classes
	 * provide small data-focused objects for simplified data access to frame objects generated by the generator nodes.
	 *
	 * @tparam _pixelType	The data type of a single pixel in this map
	 */
	template<class _pixelType>
	class Map
	{
	public:
		inline Map(_pixelType*& pData, XnUInt32& nXRes, XnUInt32 &nYRes) :
			m_pData(pData), m_nXRes(nXRes), m_nYRes(nYRes)
		{}

		/**
		 * Gets the frame X resolution. The X resolution is the actual number of columns in the frame after any required cropping has been applied.
		 *
		 * <b>Remarks</b>
		 *
		 * This is the same as @ref xn::MapMetaData::XRes "MapMetaData::XRes()".
		 */
		inline XnUInt32 XRes() const { return m_nXRes; }
		/**
		 * Gets the frame Y resolution. The Y resolution is the actual number of rows in the frame after any required cropping has been applied.
		 *
		 * <b>Remarks</b>
		 *
		 * This is the same as @ref xn::MapMetaData::YRes "MapMetaData::YRes()".
		 */
		inline XnUInt32 YRes() const { return m_nYRes; }

		/**
		 * Gets a single pixel of the frame map by indexing a map as a one-dimensional array.
		 *
		 * @param [in]	nIndex	The position of the pixel in the buffer
		 *
		 * @returns A pixel of type <code>_pixelType</code>.
		 *
		 * <b>Remarks</b>
		 *
		 * Indexing a map in this way can give improved speed in some situations.
		 */
		inline const _pixelType& operator[](XnUInt32 nIndex) const
		{
			XN_ASSERT(nIndex < (m_nXRes * m_nYRes));
			return m_pData[nIndex];
		}
		/**
		 * Gets a single pixel of the frame map by indexing a map as a one-dimensional array.
		 *
		 * @param [in]	nIndex	The position of the pixel in the buffer
		 *
		 * @returns A pixel of type <code>_pixelType</code>.
		 *
		 * <b>Remarks</b>
		 *
		 * Indexing a map in this way can give improved speed in some situations.
		 */
		inline _pixelType& operator[](XnUInt32 nIndex)
		{
			XN_ASSERT(nIndex < (m_nXRes *m_nYRes));
			return m_pData[nIndex];
		}

		/**
		 * Gets a single pixel of the frame map by its X and Y coordinates.
		 *
		 * @param [in]	x	The horizontal position of the pixel (from left to right)
		 * @param [in]	y	The vertical position of the pixel in the buffer (from top to bottom)
		 *
		 * @returns A pixel of type <code>_pixelType</code>.
		 */
		inline const _pixelType& operator()(XnUInt32 x, XnUInt32 y) const
		{
			XN_ASSERT(x < m_nXRes && y < m_nYRes);
			return m_pData[y*m_nXRes + x];
		}
		/**
		 * Gets a single pixel of the frame map by its X and Y coordinates.
		 *
		 * @param [in]	x	The horizontal position of the pixel (from left to right)
		 * @param [in]	y	The vertical position of the pixel in the buffer (from top to bottom)
		 *
		 * @returns A pixel of type <code>_pixelType</code>.
		 */
		inline _pixelType& operator()(XnUInt32 x, XnUInt32 y)
		{
			XN_ASSERT(x < m_nXRes && y < m_nYRes);
			return m_pData[y*m_nXRes + x];
		}

	private:
		/* block copy ctor and assignment operator */
		Map(const Map& other);
		Map& operator=(const Map&);

		_pixelType*& m_pData;
		XnUInt32& m_nXRes;
		XnUInt32& m_nYRes;
	};

	/** @ingroup cppref_meta_data
	 *  @{
	 */
	/** Declares a @ref xn::Map "Map" class with pixel of type XnDepthPixel */
	typedef Map<XnDepthPixel> DepthMap;
	/** Declares a @ref xn::Map "Map" class with pixel of type XnUInt8 */
	typedef Map<XnUInt8> ImageMap;
	/** Declares a @ref xn::Map "Map" class with pixel of type XnRGB24Pixel */
	typedef Map<XnRGB24Pixel> RGB24Map;
	/** Declares a @ref xn::Map "Map" class with pixel of type XnGrayscale16Pixel */
	typedef Map<XnGrayscale16Pixel> Grayscale16Map;
	/** Declares a @ref xn::Map "Map" class with pixel of type XnGrayscale8Pixel */
	typedef Map<XnGrayscale8Pixel> Grayscale8Map;
	/** Declares a @ref xn::Map "Map" class with pixel of type XnIRPixel */
	typedef Map<XnIRPixel> IRMap;
	/** Declares a @ref xn::Map "Map" class with pixel of type XnLabel */
	typedef Map<XnLabel> LabelMap;
	/** @} */

	/**
	 * @ingroup cppref_meta_data
	 *
	 * For saving the @ref glos_frame_object "frame object" (data and configuration) from
	 * the @ref xn::DepthGenerator "DepthGenerator" node. This @ref glos_frame_object "frame
	 * object" is a snapshot of the DepthGenerator's generated depth map and its associated
	 * configuration information at a certain point in time. This saved @ref glos_frame_object
	 * "frame object" provides fast and easy access to the DepthGenerator node's data and
	 * configuration information.
	 *
	 * It is important to get a good understanding of the purpose and design of the OpenNI
	 * metadata objects. For a comprehensive overview of OpenNI metadata objects, see @ref
	 * conc_meta_data.
	 *
	 * The DepthGenerator node generates depth data of the FOV. This is the
	 * Z-coordinate of the X-Y-Z coordinate of each point. The Z-axis is the distance, in
	 * millimeters from the sensor plane.
	 * A smaller depth value indicates a scene point that is a <i>closer</i> to the sensor,
	 * and a larger depth value indicates a scene point that is <i>further away</i> from
	 * the sensor. A zero depth value indicates that the DepthGenerator node did not succeed
	 * in obtaining a valid depth reading.
	 *
	 * DepthMetaData's @ref xn::DepthMetaData::Data() method provides the actual depth
	 * data. This method returns the pointer to the first depth pixel in the map.
	 *
	 * The pixel format of depth map is XnDepthPixel, i.e. 16-bit values.
	 *
	 * DepthMetaData provides the <code>ZRes</code> property to contain the depth resolution.
	 * This is the maximum value of a pixel (plus one).
	 *
	 * DepthMetaData is similar to ImageMetaData; they are both metadata for a type of map.
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
		 * @brief Shallow-Copies a DepthMetaData object.
		 *
		 * @param [in]	other	source object.
		 *
		 * @remarks Note that the data buffer is not copied, and that both object will point to the same buffer.
		 */
		inline void InitFrom(const DepthMetaData& other)
		{
			xnCopyDepthMetaData(&m_depth, &other.m_depth);
		}

		/**
		 * Shallow copies another @ref DepthMetaData object, and then @ref ReAdjust "readjusts
		 * it".
		 *
		 * @param [in]	other			Source object
		 * @param [in]	nXRes			Requested number of columns in the map
		 * @param [in]	nYRes			Requested number of rows in the map
		 * @param [in]	pExternalBuffer	Optional. An external buffer matching requested resolution.
		 * 								If NULL, A buffer will be allocated.
		 */
		inline XnStatus InitFrom(const DepthMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnDepthPixel* pExternalBuffer)
		{
			InitFrom(other);
			return ReAdjust(nXRes, nYRes, pExternalBuffer);
		}

		/**
		 * Performs a deep-copy of another metadata object (including duplication of the
		 * data buffer).
		 *
		 * @param [in]	other	source object
		 */
		XnStatus CopyFrom(const DepthMetaData& other)
		{
			// copy props
			InitFrom(other);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/**
		 * @copydoc xn::MapMetaData::ReAdjust
		 */
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, const XnDepthPixel* pExternalBuffer = NULL)
		{
			return MapMetaData::ReAdjust(nXRes, nYRes, (const XnUInt8*)pExternalBuffer);
		}

		/**
		 * Gets the maximum depth (depth resolution) that the DepthGenerator node can produce.
		 * This is the same as the resolution of the depth axis
		 * (i.e., @ref DepthGenerator::GetDeviceMaxDepth() + 1).
		 */
		inline XnDepthPixel ZRes() const { return m_depth.nZRes; }
		/**
		 * Gets the maximum depth (depth resolution) that the DepthGenerator node can produce.
		 * This is the same as the resolution of the depth axis
		 * (i.e., @ref DepthGenerator::GetDeviceMaxDepth() + 1).
		 */
		inline XnDepthPixel& ZRes() { return m_depth.nZRes; }

		/**
		 * @brief Returns the depth map.
		 *
		 * <b>Remarks</b>
		 *
		 * This method returns a pointer to the first depth pixel in the map. The depth
		 * map is implemented as a row-major order compact layout of pixels in the map.
		 *
		 * The pixel format of the map is XnDepthPixel, i.e. 16-bit integer values.
		 *
		 * This method is reimplemented from the @ref xn::OutputMetaData class.
		 */
		inline const XnDepthPixel* Data() const { return (const XnDepthPixel*)MapMetaData::Data(); }
		/**
		 * @brief Returns the depth map.
		 *
		 * <b>Remarks</b>
		 *
		 * This method returns a pointer to the first depth pixel in the map. The depth
		 * map is implemented as a row-major order compact layout of pixels in the map.
		 *
		 * The pixel format of the map is XnDepthPixel, i.e. 16-bit integer values.
		 *
		 * This method is reimplemented from the @ref xn::OutputMetaData class.
		 */
		inline const XnDepthPixel*& Data() { return (const XnDepthPixel*&)MapMetaData::Data(); }


		/**
		 * @copydoc xn::OutputMetaData::WritableData
		 */
		inline XnDepthPixel* WritableData() { return (XnDepthPixel*)MapMetaData::WritableData(); }

		/**
		 * Gets a light object wrapping the depth map
		 */
		inline const xn::DepthMap& DepthMap() const { return m_depthMap; }
		/**
		 * Gets a light object wrapping the writable depth map
		 */
		inline xn::DepthMap& WritableDepthMap()
		{
			MakeDataWritable();
			return m_writableDepthMap;
		}

		/**
		 * Gets the value of a single pixel of the frame map by its index in the array.
		 *
		 * @param [in]	nIndex	The index of the pixel in the buffer.
		 */
		inline const XnDepthPixel& operator[](XnUInt32 nIndex) const
		{
			XN_ASSERT(nIndex < (XRes()*YRes()));
			return Data()[nIndex];
		}

		/**
		 * Gets the value of a single pixel of the frame map by its X and Y coordinates.
		 *
		 * @param [in]	x	X-coordinate of the pixel in the map
		 * @param [in]	y	Y-coordinate of the pixel in the map
		 */
		inline const XnDepthPixel& operator()(XnUInt32 x, XnUInt32 y) const
		{
			XN_ASSERT(x < XRes() && y < YRes());
			return Data()[y*XRes() + x];
		}

		/**
		 * @copydoc xn::OutputMetaData::GetUnderlying
		 */
		inline const XnDepthMetaData* GetUnderlying() const { return &m_depth; }
		/**
		 * @copydoc xn::OutputMetaData::GetUnderlying
		 */
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
	 * @ingroup cppref_meta_data
	 *
	 * For saving the @ref glos_frame_object "frame object" (data and configuration) from
	 * the @ref xn::DepthGenerator "DepthGenerator" node. This @ref glos_frame_object "frame
	 * object" is a snapshot of the DepthGenerator's generated depth map and its associated
	 * configuration information at a certain point in time. This saved @ref glos_frame_object
	 * "frame object" provides fast and easy access to the DepthGenerator node's data and
	 * configuration information.
	 *
	 * It is important to get a good understanding of the purpose and design of the OpenNI
	 * metadata objects. For a comprehensive overview of OpenNI metadata objects, see @ref
	 * conc_meta_data.
	 *
	 * The ImageMetaData object provides a number of different methods to get the image
	 * as a different format. You must use the 'Get Data' method most appropriate for the
	 * data format of the latest available frame. For example, if you requested the ImageGenerator
	 * node to generate RGB24 format you must use the @ref RGB24Data() method to get the
	 * data.
	 *
	 * Each of the 'Get Data' methods gets the image as a pointer to the first pixel
	 * in the image map. From the address of that first pixel you can access all the pixels.
	 *
	 * Depending on the format, the pixels are different in size. This means that image
	 * maps are a different size.
	 */
	class ImageMetaData : public MapMetaData
	{
	public:
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
		 * @brief Shallow-copies an ImageMetaData object.
		 *
		 * @param [in]	other	source object.
		 *
		 * @remarks Note that the data buffer is not copied, and that both object will point to the same buffer.
		 */
		inline void InitFrom(const ImageMetaData& other)
		{
			xnCopyImageMetaData(&m_image, &other.m_image);
		}

		/**
		 * Shallow copies another @ref ImageMetaData object, and then @ref ReAdjust "readjusts"
		 * it.
		 *
		 * @param [in]	other			Source object
		 * @param [in]	nXRes			Requested number of columns in the map
		 * @param [in]	nYRes			Requested number of rows in the map
		 * @param [in]	format			Requested pixel format
		 * @param [in]	pExternalBuffer	Optional. An external buffer matching requested resolution.
		 * 								If NULL, A buffer will be allocated.
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
		 * @param [in]	nXRes	Requested number of columns in the map
		 * @param [in]	nYRes	Requested number of rows in the map
		 * @param [in]	format	Requested pixel format
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

		/**
		 * Performs a deep-copy of another metadata object (including duplication of the data buffer)
		 *
		 * @param [in]	other	source object
		 */
		inline XnStatus CopyFrom(const ImageMetaData& other)
		{
			// copy props
			xnCopyImageMetaData(&m_image, &other.m_image);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/**
		 * @copybrief xn::MapMetaData::ReAdjust
		 *
		 * @param [in]	nXRes			Requested number of columns in the map
		 * @param [in]	nYRes			Requested number of rows in the map
		 * @param [in]	format			Requested pixel format
		 * @param [in]	pExternalBuffer	Optional. An external buffer to be used. If NULL
		 * 								is passed, a buffer will be allocated.
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

		/**
		 * @brief Gets the frame's pixel color format used in this image map. This is the
		 * format of the @ref glos_frame_object "frame object" saved in this object.
		 *
		 * <b>Remarks</b>
		 *
		 * Knowing the pixel format of the image map helps you to enumerate over the map
		 * and process its  data.
		 *
		 * Reimplemented from @ref xn::MapMetaData.
		 */
		inline XnPixelFormat PixelFormat() const { return MapMetaData::PixelFormat(); }
		/**
		 * @brief Gets the frame's pixel color format used in this image map. This is the
		 * format of the @ref glos_frame_object "frame object" saved in this object.
		 *
		 * <b>Remarks</b>
		 *
		 * Knowing the pixel format of the image map helps you to enumerate over the map
		 * and process its  data.
		 */
		inline XnPixelFormat& PixelFormat() { return MapMetaData::PixelFormatImpl(); }

		/**
		 * @copydoc MapMetaData::WritableData
		 */
		inline XnUInt8* WritableData() { return MapMetaData::WritableData(); }

		/**
		 * @brief Gets a pointer to the first pixel of the image in @ref xn::XnRGB24Pixel "RGB24" format.
		 * RGB24 format represents each pixel as one byte for red, one byte for green, and one byte for blue.
		 */
		inline const XnRGB24Pixel* RGB24Data() const { return (const XnRGB24Pixel*)MapMetaData::Data(); }
		/**
		 * @brief Gets a pointer to the first pixel of the image in @ref xn::XnRGB24Pixel "RGB24" format.
		 * RGB24 format represents each pixel as one byte for red, one byte for green, and one byte for blue.
		 */
		inline const XnRGB24Pixel*& RGB24Data() { return (const XnRGB24Pixel*&)MapMetaData::Data(); }
		/**
		 * @copydoc MapMetaData::WritableData
		 */
		inline XnRGB24Pixel* WritableRGB24Data() { return (XnRGB24Pixel*)MapMetaData::WritableData(); }

		/**
		 * @brief Gets a pointer to the first pixel of the image in @ref xn::XnYUV422DoublePixel "YUV422"  format.
		 * YUV422 is a type of compression with a single Y byte followed by a U byte, then another Y byte and then a V byte: YUY'V.
		 * These four values represent two pixels: YUV and Y'UV.
		 */
		inline const XnYUV422DoublePixel* YUV422Data() const { return (const XnYUV422DoublePixel*)MapMetaData::Data(); }
		/**
		 * @brief Gets a pointer to the first pixel of the image in @ref xn::XnYUV422DoublePixel "YUV422"  format.
		 * YUV422 is a type of compression with a single Y byte followed by a U byte, then another Y byte and then a V byte: YUY'V.
		 * These four values represent two pixels: YUV and Y'UV.
		 */
		inline const XnYUV422DoublePixel*& YUV422Data() { return (const XnYUV422DoublePixel*&)MapMetaData::Data(); }
		/**
		 * @copydoc MapMetaData::WritableData
		 */
		inline XnYUV422DoublePixel* WritableYUV422Data() { return (XnYUV422DoublePixel*)MapMetaData::WritableData(); }

		/**
		 * @brief Gets a pointer to the first pixel of the image in Grayscale8 format.
		 * Grayscale8 represents each pixel as an 8-bit (1 byte) gray scale.
		 */
		inline const XnGrayscale8Pixel* Grayscale8Data() const { return (const XnGrayscale8Pixel*)MapMetaData::Data(); }
		/**
		 * @brief Gets a pointer to the first pixel of the image in Grayscale8 format.
		 * Grayscale8 represents each pixel as an 8-bit (1-byte) gray scale.
		 */
		inline const XnGrayscale8Pixel*& Grayscale8Data() { return (const XnGrayscale8Pixel*&)MapMetaData::Data(); }
		/**
		 * @copydoc MapMetaData::WritableData
		 */
		inline XnGrayscale8Pixel* WritableGrayscale8Data() { return (XnGrayscale8Pixel*)MapMetaData::WritableData(); }

		/**
		 * @brief Gets a pointer to the first pixel of the image in Grayscale16 format.
		 * Grayscale16 represents each pixel as a 16-bit (2-byte) gray scale.
		 */
		inline const XnGrayscale16Pixel* Grayscale16Data() const { return (const XnGrayscale16Pixel*)MapMetaData::Data(); }
		/**
		 * @brief Gets a pointer to the first pixel of the image in Grayscale16 format.
		 * Grayscale16 represents each pixel as a 16-bit (2-byte) gray scale.
		 */
		inline const XnGrayscale16Pixel*& Grayscale16Data() { return (const XnGrayscale16Pixel*&)MapMetaData::Data(); }
		/**
		 * @copydoc MapMetaData::WritableData
		 */
		inline XnGrayscale16Pixel* WritableGrayscale16Data() { return (XnGrayscale16Pixel*)MapMetaData::WritableData(); }

		/**
		 * @brief Gets a light object wrapping the image map.
		 */
		inline const xn::ImageMap& ImageMap() const { return m_imageMap; }
		/**
		 * Gets a light object wrapping the writable image-map
		 */
		inline xn::ImageMap& WritableImageMap() { MakeDataWritable(); return m_writableImageMap; }

		/**
		 * @brief Gets a light wrapper object for wrapping the image map as RGB24 format.
		 * RGB24 format represents each pixel as one byte for red, one byte for green, and
		 * one byte for blue.
		 */
		inline const xn::RGB24Map& RGB24Map() const { return m_rgb24Map; }
		/**
		 * @copydoc xn::ImageMetaData::WritableImageMap
		 */
		inline xn::RGB24Map& WritableRGB24Map() { MakeDataWritable(); return m_writableRgb24Map; }

		/**
		 * @brief Gets a light wrapper object wrapping the image map as Grayscale8 format.
		 */
		inline const xn::Grayscale8Map& Grayscale8Map() const { return m_gray8Map; }
		/**
		 * @copydoc xn::ImageMetaData::WritableImageMap
		 */
		inline xn::Grayscale8Map& WritableGrayscale8Map() { MakeDataWritable(); return m_writableGray8Map; }

		/**
		 * @brief Gets a light wrapper object wrapping the frame's image map as Grayscale16
		 * format.
		 */
		inline const xn::Grayscale16Map& Grayscale16Map() const { return m_gray16Map; }
		/**
		 * @copydoc xn::ImageMetaData::WritableImageMap
		 */
		inline xn::Grayscale16Map& WritableGrayscale16Map() { MakeDataWritable(); return m_writableGray16Map; }

		/**
		 * @copydoc xn::OutputMetaData::GetUnderlying
		 */
		inline const XnImageMetaData* GetUnderlying() const { return &m_image; }
		/**
		 * @copydoc xn::OutputMetaData::GetUnderlying
		 */
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
	 * @ingroup cppref_meta_data
	 *
	 * <b>Purpose:</b> For saving the @ref glos_frame_object "frame object" (data and configuration)
	 * from an xn::IRGenerator node. This @ref glos_frame_object "frame object" is a snapshot
	 * of the generated infra-red map data and its associated configuration information
	 * at a certain point in time. This saved @ref glos_frame_object "frame object" provides
	 * fast and easy access to the IRGenerator node's data and configuration information.
	 */
	class IRMetaData : public MapMetaData
	{
	public:
		inline IRMetaData() :
			MapMetaData(XN_PIXEL_FORMAT_GRAYSCALE_16_BIT, (const XnUInt8**)&m_ir.pData),
			m_irMap(const_cast<XnIRPixel*&>(m_ir.pData), MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableIRMap((XnIRPixel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y)
		{
			xnOSMemSet(&m_ir, 0, sizeof(XnIRMetaData));
			m_ir.pMap = MapMetaData::GetUnderlying();
		}

		/**
		 * @brief Shallow-Copies an IRMetaData object.
		 *
		 * @param [in]	other	source object.
		 *
		 * @remarks Note that the data buffer is not copied, and that both object will point to the same buffer.
		 */
		inline void InitFrom(const IRMetaData& other)
		{
			xnCopyIRMetaData(&m_ir, &other.m_ir);
		}

		/**
		 * Shallow copies another IRMetaData object, and then @ref ReAdjust it.
		 *
		 * @param [in]	other			source object
		 * @param [in]	nXRes			requested number of columns in the map
		 * @param [in]	nYRes			requested number of rows in the map
		 * @param [in]	pExternalBuffer	Optional. An external buffer matching requested resolution. If NULL,
		 *								A buffer will be allocated.
		 */
		inline XnStatus InitFrom(const IRMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnIRPixel* pExternalBuffer)
		{
			InitFrom(other);
			return ReAdjust(nXRes, nYRes, pExternalBuffer);
		}

		/**
		 * Performs a deep-copy of another metadata object (including duplication of the data buffer)
		 *
		 * @param [in]	other	source object
		 */
		XnStatus CopyFrom(const IRMetaData& other)
		{
			// copy props
			xnCopyIRMetaData(&m_ir, &other.m_ir);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/**
		 * @copydoc MapMetaData::ReAdjust
		 */
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, const XnIRPixel* pExternalBuffer = NULL)
		{
			return MapMetaData::ReAdjust(nXRes, nYRes, (const XnUInt8*)pExternalBuffer);
		}

		/**
		 * @brief Returns the IR map.
		 *
		 * <b>Remarks</b>
		 *
		 * This method returns a pointer to the first IR pixel in the map. The IR
		 * map is implemented as a row-major order compact layout of pixels in the map.
		 *
		 * The pixel format of the map is XnIRPixel, i.e. 16-bit integer values.
		 *
		 * This method is reimplemented from the @ref xn::OutputMetaData class.
		 */
		inline const XnIRPixel* Data() const { return (const XnIRPixel*)MapMetaData::Data(); }
		/**
		 * @brief Returns the IR map.
		 *
		 * <b>Remarks</b>
		 *
		 * This method returns a pointer to the first IR pixel in the map. The IR
		 * map is implemented as a row-major order compact layout of pixels in the map.
		 *
		 * The pixel format of the map is XnIRPixel, i.e. 16-bit integer values.
		 *
		 * This method is reimplemented from the @ref xn::OutputMetaData class.
		 */
		inline const XnIRPixel*& Data() { return (const XnIRPixel*&)MapMetaData::Data(); }
		/**
		 * @copydoc OutputMetaData::WritableData
		 */
		inline XnIRPixel* WritableData() { return (XnIRPixel*)MapMetaData::WritableData(); }

		/**
		 * Gets the value of a single pixel of the frame map by its index in the array.
		 *
		 * @param [in]	nIndex	The index of the pixel in the buffer.
		 */
		inline const XnIRPixel& operator[](XnUInt32 nIndex) const
		{
			XN_ASSERT(nIndex < (XRes()*YRes()));
			return Data()[nIndex];
		}

		/**
		 * Gets the value of a single pixel of the frame map by its X and Y coordinates.
		 *
		 * @param [in]	x	X-coordinate of the pixel in the map
		 * @param [in]	y	Y-coordinate of the pixel in the map
		 */
		inline const XnIRPixel& operator()(XnUInt32 x, XnUInt32 y) const
		{
			XN_ASSERT(x < XRes() && y < YRes());
			return Data()[y*XRes() + x];
		}

		/**
		 * @brief Gets a fast, light object wrapping the IR map.
		 * <b>Remarks</b>
		 *
		 * The Map Wrapper classes are light wrappers for generated maps saved as @ref glos_frame_object
		 * "frame objects" in metadata. These wrapper classes provide small data-focused
		 * objects for simplified data access to frame objects.
		 */
		inline const xn::IRMap& IRMap() const { return m_irMap; }
		/**
		 * @brief Gets a fast, light object wrapping the writable IR map.
		 * <b>Remarks</b>
		 *
		 * The Map Wrapper classes are light wrappers for generated maps saved as @ref glos_frame_object
		 * "frame objects" in metadata. These wrapper classes provide small data-focused
		 * objects for simplified data access to frame objects.
		 */
		inline xn::IRMap& WritableIRMap() { MakeDataWritable(); return m_writableIRMap; }

		/**
		 * @copydoc OutputMetaData::GetUnderlying
		 */
		inline const XnIRMetaData* GetUnderlying() const { return &m_ir; }
		/**
		 * @copydoc OutputMetaData::GetUnderlying
		 */
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
	 * @ingroup cppref_meta_data
	 * For saving the audio data from the @ref xn::AudioGenerator node.
	 * This saved metadata object provides fast and easy access to the AudioGenerator node's
	 * data and configuration information.
	 */
	class AudioMetaData : public OutputMetaData
	{
	public:
		XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_UNALIGNED_ADDRESS_WARNING_ID);
		inline AudioMetaData() : OutputMetaData(&m_audio.pData)
		{
			xnOSMemSet(&m_audio, 0, sizeof(XnAudioMetaData));
			m_audio.pOutput = OutputMetaData::GetUnderlying();
		}

		XN_PRAGMA_STOP_DISABLED_WARNING_SECTION;

		/**
		 * @brief Shallow-Copies an AudioMetaData object.
		 *
		 * @param [in]	other	source object.
		 *
		 * @remarks Note that the data buffer is not copied, and that both object will point to the same buffer.
		 */
		inline void InitFrom(const AudioMetaData& other)
		{
			xnCopyAudioMetaData(&m_audio, &other.m_audio);
		}

		/**
		 * @brief Gets the number of channels in each sample. This is a packet configuration setting associated with the saved audio metadata object.
		 */
		inline XnUInt8 NumberOfChannels() const { return m_audio.Wave.nChannels; }
		/**
		 * @brief Gets the number of channels in each sample. This is a packet configuration setting associated with the saved audio metadata object.
		 */
		inline XnUInt8& NumberOfChannels() { return m_audio.Wave.nChannels; }

		/**
		 * @brief Gets the rate at which the audio interface was sampled. This is a packet configuration setting associated with the saved audio metadata object.
		 */
		inline XnUInt32 SampleRate() const { return m_audio.Wave.nSampleRate; }
		/**
		 * @brief Gets the rate at which the audio interface was sampled. This is a packet configuration setting associated with the saved audio metadata object.
		 */
		inline XnUInt32& SampleRate() { return m_audio.Wave.nSampleRate; }

		/**
		 * @brief Gets the number of bits per channel in a sample. This is a packet configuration setting associated with the saved audio metadata object.
		 */
		inline XnUInt16 BitsPerSample() const { return m_audio.Wave.nBitsPerSample; }
		/**
		 * @brief Gets the number of bits per channel in a sample. This is a packet configuration setting associated with the saved audio metadata object.
		 */
		inline XnUInt16& BitsPerSample() { return m_audio.Wave.nBitsPerSample; }

		/**
		 * @copydoc OutputMetaData::GetUnderlying
		 */
		inline const XnAudioMetaData* GetUnderlying() const { return &m_audio; }
		/**
		 * @copydoc OutputMetaData::GetUnderlying
		 */
		inline XnAudioMetaData* GetUnderlying() { return &m_audio; }

	private:
		// block copy ctor and assignment operator
		AudioMetaData(const AudioMetaData& other);
		AudioMetaData& operator=(const AudioMetaData&);

		XnAudioMetaData m_audio;
	};

	/**
	 * @ingroup cppref_meta_data
	 * Saves the @ref glos_frame_object "frame object" (data and configuration)
	 * from the @ref xn::SceneAnalyzer node. This @ref glos_frame_object "frame object"
	 * is a snapshot of the generated scene data and its associated configuration information
	 * at a certain point in time. This metadata provides fast and easy access to the SceneAnalyzer
	 * node's saved frame object.
	 */
	class SceneMetaData : public MapMetaData
	{
	public:
		inline SceneMetaData() :
			MapMetaData(XN_PIXEL_FORMAT_GRAYSCALE_16_BIT, (const XnUInt8**)&m_scene.pData),
			m_labelMap(const_cast<XnLabel*&>(m_scene.pData), MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y),
			m_writableLabelMap((XnLabel*&)m_pAllocatedData, MapMetaData::GetUnderlying()->Res.X, MapMetaData::GetUnderlying()->Res.Y)
		{
			xnOSMemSet(&m_scene, 0, sizeof(XnSceneMetaData));
			m_scene.pMap = MapMetaData::GetUnderlying();
		}

		/**
		 * @brief Shallow-Copies an SceneMetaData object.
		 *
		 * @param [in]	other	source object.
		 *
		 * @remarks Note that the data buffer is not copied, and that both object will point to the same buffer.
		 */
		inline void InitFrom(const SceneMetaData& other)
		{
			xnCopySceneMetaData(&m_scene, &other.m_scene);
		}

		/**
		 * Shallow copies another SceneMetaData object, and then @ref ReAdjust it.
		 *
		 * @param [in]	other			source object
		 * @param [in]	nXRes			requested number of columns in the map
		 * @param [in]	nYRes			requested number of rows in the map
		 * @param [in]	pExternalBuffer	Optional. An external buffer matching requested resolution. If NULL,
		 *								A buffer will be allocated.
		 */
		inline XnStatus InitFrom(const SceneMetaData& other, XnUInt32 nXRes, XnUInt32 nYRes, const XnLabel* pExternalBuffer)
		{
			InitFrom(other);
			return ReAdjust(nXRes, nYRes, pExternalBuffer);
		}

		/**
		 * Performs a deep-copy of another metadata object (including duplication of the data buffer)
		 *
		 * @param [in]	other	source object
		 */
		XnStatus CopyFrom(const SceneMetaData& other)
		{
			// copy props
			xnCopySceneMetaData(&m_scene, &other.m_scene);
			// and make a copy of the data (this will allocate and copy data)
			return MakeDataWritable();
		}

		/**
		 * @copydoc DepthMetaData::ReAdjust
		 */
		XnStatus ReAdjust(XnUInt32 nXRes, XnUInt32 nYRes, const XnLabel* pExternalBuffer = NULL)
		{
			return MapMetaData::ReAdjust(nXRes, nYRes, (const XnUInt8*)pExternalBuffer);
		}

		/**
		 * @brief Returns the scene map.
		 *
		 * <b>Remarks</b>
		 *
		 * This method returns a pointer to the first scene pixel in the map. The scene
		 * map is implemented as a row-major order compact layout of pixels in the map.
		 *
		 * The pixel format of the map is XnLabel, i.e. 16-bit integer values, where every
		 * user has a distinct label.
		 *
		 * This method is reimplemented from the @ref xn::OutputMetaData class.
		 */
		inline const XnLabel* Data() const { return (const XnLabel*)MapMetaData::Data(); }
		/**
		 * @brief Returns the scene map.
		 *
		 * <b>Remarks</b>
		 *
		 * This method returns a pointer to the first scene pixel in the map. The scene
		 * map is implemented as a row-major order compact layout of pixels in the map.
		 *
		 * The pixel format of the map is XnLabel, i.e. 16-bit integer values, where every
		 * user has a distinct label.
		 *
		 * This method is reimplemented from the @ref xn::OutputMetaData class.
		 */
		inline const XnLabel*& Data() { return (const XnLabel*&)MapMetaData::Data(); }

		/**
		 * @copydoc OutputMetaData::WritableData
		 */
		inline XnLabel* WritableData() { return (XnLabel*)MapMetaData::WritableData(); }

		/**
		 * Gets a light object wrapping the label map
		 */
		inline const xn::LabelMap& LabelMap() const { return m_labelMap; }
		/**
		 * Gets a light object wrapping the writable label map
		 */
		inline xn::LabelMap& WritableLabelMap() { MakeDataWritable(); return m_writableLabelMap; }

		/**
		 * Gets the label of a single pixel of the frame map by its index in the array.
		 *
		 * @param [in]	nIndex	The index of the pixel in the buffer.
		 *
		 * <b>Remarks</b>
		 *
		 * The values are of type @ref XnLabel. Each pixel's value labels it according to
		 * user.
		 */
		inline const XnLabel& operator[](XnUInt32 nIndex) const
		{
			XN_ASSERT(nIndex < (XRes()*YRes()));
			return Data()[nIndex];
		}

		/**
		 * Gets the label of a single pixel of the frame map by its X and Y positions.
		 *
		 * @param [in]	x	X-coordinate of the pixel in the map
		 * @param [in]	y	Y-coordinate of the pixel in the map
		 *
		 * <b>Remarks</b>
		 *
		 * The values are of type @ref XnLabel. Each pixel's value labels it according to
		 * user.
		 */
		inline const XnLabel& operator()(XnUInt32 x, XnUInt32 y) const
		{
			XN_ASSERT(x < XRes() && y < YRes());
			return (*this)[y*XRes() + x];
		}

		/**
		 * @copydoc OutputMetaData::GetUnderlying
		 */
		inline const XnSceneMetaData* GetUnderlying() const { return &m_scene; }
		/**
		 * @copydoc OutputMetaData::GetUnderlying
		 */
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
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> The NodeWrapper class is the base class for all OpenNI production
	 * node classes in C++, for example, the @ref xn::ProductionNode class and the @ref
	 * xn::Generator class.
	 *
	 * The NodeWrapper class is the C++ API wrapper around the OpenNI C XnNodeHandle API.
	 *
	 * <b>Remarks:</b>
	 *
	 * A fundamental OpenNI concept that is critical to understand about nodes and node
	 * creation is that all C++ objects are "smart pointers" to the actual nodes. The reason
	 * why OpenNI is designed this way is so that the same node can be used to produce data
	 * (or provide information) for more than just one (dependant) node or for more than
	 * just one software component.
	 *
	 * When instantiating an object of this class, it doesn't point to any actual node. In order
	 * to create an actual node, use one of the <code>Create()</code> methods (e.g. @ref DepthGenerator::Create()).
	 */
	class NodeWrapper
	{
	public:
		friend class Context;

		/**
		 * Ctor
		 *
		 * @param [in]	hNode	A node handle.
		 */
		inline NodeWrapper(XnNodeHandle hNode) : m_hNode(NULL), m_hShuttingDownCallback(NULL)
		{
			SetHandle(hNode);
		}

		inline NodeWrapper(const NodeWrapper& other) : m_hNode(NULL), m_hShuttingDownCallback(NULL)
		{
			SetHandle(other.GetHandle());
		}

		inline NodeWrapper& operator=(const NodeWrapper& other)
		{
			SetHandle(other.GetHandle());
			return *this;
		}

		inline ~NodeWrapper()
		{
			SetHandle(NULL);
		}

		inline operator XnNodeHandle() const { return GetHandle(); }

		/**
		 * Gets the underlying C handle.
		 */
		inline XnNodeHandle GetHandle() const { return m_hNode; }

		/**
		 * Checks if two node references point to the same actual node
		 *
		 * @param [in]	other	Another object
		 */
		inline XnBool operator==(const NodeWrapper& other)
		{
			return (GetHandle() == other.GetHandle());
		}

		/**
		 * Checks if two node references do <i>not</i> point to the same actual node.
		 *
		 * @param [in]	other	Another object
		 */
		inline XnBool operator!=(const NodeWrapper& other)
		{
			return (GetHandle() != other.GetHandle());
		}

		/** @brief
		 * This method checks that this object points to an <i>actual</i> node (that has
		 * been 'created') and does not point to NULL.
		 *
		 * Returns TRUE if the object points to a node, i.e., the node has been 'created';
		 * FALSE otherwise.
		 *
		 * <b>Remarks</b>
		 *
		 * This method is a very important for managing nodes in the production graph. This
		 * check is concerned with the stage before actually creating the node, i.e., before
		 * invoking the node's <code>Create()</code> method.
		 */
		inline XnBool IsValid() const { return (GetHandle() != NULL); }

		/*
		 * @brief Gets the instance name of a node. Unless the application made a specific
		 * request for a specific name, the name will be of the form: "Depth1", "Image2", etc.
		 */
		const XnChar* GetName() const {return xnGetNodeName(GetHandle()); }

		/**
		 * @copybrief xnProductionNodeAddRef
		 * For full details and usage, see @ref xnProductionNodeAddRef
		 */
		inline XnStatus AddRef() { return xnProductionNodeAddRef(GetHandle()); }

		/**
		 * Unreference a production node, decreasing its reference count by 1. If the reference count reaches
		 * zero, the node will be destroyed.
		 */
		inline void Release()
		{
			SetHandle(NULL);
		}


		/**
		 * Replaces the object being pointed.
		 */
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
				XN_REFERENCE_VARIABLE(nRetVal);

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

		/** @cond */
		inline XnStatus XN_API_DEPRECATED("Please use AddRef() instead.") Ref() { return AddRef(); }
		inline void XN_API_DEPRECATED("Please use Release() instead.") Unref() { Release(); }
		/** @endcond */

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
	 * @ingroup cppref_graph_mgmt
	 * <b>Purpose:</b> This class provides functionality for getting information about a
	 * @ref node_alternative "node alternative". A @ref node_alternative is either an existing
	 * node, or a not-yet instantiated alternative.
	 *
	 * <b>Remarks:</b>
	 *
	 * Each instantiated node has a NodeInfo object and it can be accessed through the @ref
	 * xn::ProductionNode::GetInfo() method.
	 */
	class NodeInfo
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	pInfo	The C object to wrap
		 */
		NodeInfo(XnNodeInfo* pInfo) : m_pNeededNodes(NULL), m_bOwnerOfNode(FALSE)
		{
			SetUnderlyingObject(pInfo);
		}

		/**
		 * Copy Ctor
		 *
		 * @param [in]	other	An object to copy from.
		 */
		NodeInfo(const NodeInfo& other) : m_pNeededNodes(NULL), m_bOwnerOfNode(FALSE)
		{
			SetUnderlyingObject(other.m_pInfo);
		}

		/**
		 * Dtor
		 */
		~NodeInfo()
		{
			SetUnderlyingObject(NULL);
		}

		/**
		 * Assignment operator
		 *
		 * @param [in]	other	another object
		 */
		inline NodeInfo& operator=(const NodeInfo& other)
		{
			SetUnderlyingObject(other.m_pInfo);
			return *this;
		}

		/**
		 * Gets the underlying C object
		 */
		inline operator XnNodeInfo*()
		{
			return m_pInfo;
		}

		/**
		 * @brief Sets the instance name of the NodeInfo object. The name can only be set  <i>before the instance is created</i>.
		 *
		 * @param [in]	strName Name to give to this instance.
		 */
		inline XnStatus SetInstanceName(const XnChar* strName)
		{
			return xnNodeInfoSetInstanceName(m_pInfo, strName);
		}

		/**
		 * @brief Gets the description of a node alternative.
		 *
		 * <b>Remarks</b>
		 *
		 * When you instantiate a production node object -- for example a DepthGenerator
		 * --  you cannot access it's NodeInfo object until after <code>Create()</code>
		 * has been called.
		 *
		 * However - you can also get NodeInfo objects by enumeration, for example, from
		 * the @ref xn::Context::EnumerateProductionTrees() "EnumerateProductionTrees()"
		 * method.
		 */
		inline const XnProductionNodeDescription& GetDescription() const
		{
			return *xnNodeInfoGetDescription(m_pInfo);
		}

		/**
		 * @brief Gets the instance name of a node alternative.
		 *
		 * <b>Remarks</b>
		 *
		 * When you instantiate a DepthGenerator object, for example, you cannot access
		 * it's NodeInfo object until after its <code>Create()</code> method has been called.
		 *
		 * However - you can also get NodeInfo objects by enumeration, for example, from
		 * the EnumerateProductionTrees() method.
		 */
		inline const XnChar* GetInstanceName() const
		{
			return xnNodeInfoGetInstanceName(m_pInfo);
		}

		/**
		 * @brief Gets the creation information of the node alternative.
		 *
		 * <b>Remarks</b>
		 *
		 * A node's creation information is a way for a node implementation to have two
		 * distinct instances of the same node class, that is two nodes.
		 * For example, a Device node implementation can use this field for the name of
		 * the device it represents, so that if two sensors are connected, each will have
		 * a different value here.
		 */
		inline const XnChar* GetCreationInfo() const
		{
			return xnNodeInfoGetCreationInfo(m_pInfo);
		}

		/*
		 * @brief Gets the list of dependant nodes for this node alternative.
		 * These are any other node alternatives that the node is dependant on.
		 */
		inline NodeInfoList& GetNeededNodes() const;

		/**
		 * Gets a reference to the node instance represented by this node alternative.
		 *
		 * @param	node	[in/out]	The production node to be pointing to the node.
		 *
		 * @returns an error if this node info object does not point to an actual node.
		 */
		inline XnStatus GetInstance(ProductionNode& node) const;

		/**
		 * @copybrief xnNodeInfoGetAdditionalData
		 * For full details and usage, see @ref xnNodeInfoGetAdditionalData
		 */
		inline const void* GetAdditionalData() const
		{
			return xnNodeInfoGetAdditionalData(m_pInfo);
		}

		/**
		 * @copybrief xnNodeInfoGetTreeStringRepresentation
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
	 * @ingroup cppref_graph_mgmt
	 *
	 * <b>Purpose:</b> Represents a query, containing filters regarding node info objects.
	 * Note that the relation between the filters is an AND relation.
	 *
	 * <b>Usage: </b> Instantiate a Query object and then use the provided methods to set
	 * up the filter conditions.
	 *
	 * <b>Remarks:</b>
	 * The Query class provides a mechanism for searching and returning NodeInfo objects
	 * from a list while filtering them according to a filter argument. The relation between
	 * the filters is an AND relationship.
	 *
	 * The mechanism works by instantiating a Query object and then using the provided methods
	 * to set up the filter conditions. The Query object is then used by a number of methods
	 * that search for nodes, e.g., the Create() method.
	 */
	class Query
	{
	public:
		inline Query() : m_bAllocated(TRUE)
		{
			xnNodeQueryAllocate(&m_pQuery);
		}

		inline Query(XnNodeQuery* pNodeQuery) : m_bAllocated(FALSE), m_pQuery(pNodeQuery)
		{
		}

		~Query()
		{
			if (m_bAllocated)
			{
				xnNodeQueryFree(m_pQuery);
			}
		}

		/**
		 * Gets the underlying C object
		 */
		inline const XnNodeQuery* GetUnderlyingObject() const { return m_pQuery; }
		inline XnNodeQuery* GetUnderlyingObject() { return m_pQuery; }

		/**
		 * @brief Sets in the Query object the vendor name to search for.
		 * @param [in]	strVendor Vendor Name.
		 */
		inline XnStatus SetVendor(const XnChar* strVendor)
		{
			return xnNodeQuerySetVendor(m_pQuery, strVendor);
		}

		/**
		 * @brief Sets in the Query object the implementation name to search for.
		 * The same vendor might have more than one implementation, for example,
		 * two @ref xn::DepthGenerator "DepthGenerators" "SensorV2" and "EEDepthGenerator".
		 * This is not the same as the instance name.
		 *
		 * @param [in]	strName Implementation name.
		 */
		inline XnStatus SetName(const XnChar* strName)
		{
			return xnNodeQuerySetName(m_pQuery, strName);
		}

		/**
		 * @brief Sets in the Query object the minimum required version allowed for the @ref node_alternative.
		 */
		inline XnStatus SetMinVersion(const XnVersion& minVersion)
		{
			return xnNodeQuerySetMinVersion(m_pQuery, &minVersion);
		}

		/**
		 * @brief Sets in the Query object the minimum required version allowed for the @ref node_alternative.
		 */
		inline XnStatus SetMinVersion(const Version& minVersion)
		{
			return xnNodeQuerySetMinVersion(m_pQuery, minVersion.GetUnderlying());
		}

		/**
		 * @brief Sets in the Query object the maximum required version allowed for the node alternative.
		 */
		inline XnStatus SetMaxVersion(const XnVersion& maxVersion)
		{
			return xnNodeQuerySetMaxVersion(m_pQuery, &maxVersion);
		}

		/**
		 * @brief Sets in the Query object the maximum required version allowed for the node alternative.
		 */
		inline XnStatus SetMaxVersion(const Version& maxVersion)
		{
			return xnNodeQuerySetMaxVersion(m_pQuery, maxVersion.GetUnderlying());
		}

		/**
		 * @brief Sets in the Query object a capability that the @ref node_alternative must support.
		 *
		 * @param [in]	strNeededCapability	Name of the capability that the @ref node_alternative
		 * 									must support.
		 *
		 * <b>Remarks</b>
		 *
		 * Note that for checking if a capability is supported, the node must be created
		 * if it's not an already existing node. This operation might take a bit longer
		 * than usual.
		 */
		inline XnStatus AddSupportedCapability(const XnChar* strNeededCapability)
		{
			return xnNodeQueryAddSupportedCapability(m_pQuery, strNeededCapability);
		}

		/**
		 * @brief Sets in the Query object a MapOutput mode that the @ref node_alternative must support.
		 *
		 * @param [in]	MapOutputMode	Map output mode. Each field of the mode can be set to
		 * 								(-1), meaning results will not be filtered by this field.
		 *
		 * @remark Note that for checking if a capability is supported, the node must be created
		 * if it's not an already existing node. This operation might take a bit longer
		 * than usual.
		 */
		inline XnStatus AddSupportedMapOutputMode(const XnMapOutputMode& MapOutputMode)
		{
			return xnNodeQueryAddSupportedMapOutputMode(m_pQuery, &MapOutputMode);
		}

		/**
		 * @copybrief xnNodeQuerySetSupportedMinUserPositions
		 * For full details and usage, see @ref xnNodeQuerySetSupportedMinUserPositions
		 */
		inline XnStatus SetSupportedMinUserPositions(const XnUInt32 nCount)
		{
			return xnNodeQuerySetSupportedMinUserPositions(m_pQuery, nCount);
		}

		/**
		 * @brief Sets in the Query object that the query will return only existing nodes,
		 * i.e., nodes that have been created, in other words their <code>Create()</code> method has been called.
		 *
		 * @param [in]	bExistingNode TRUE to return only existing nodes, FALSE otherwise.
		 *
		 * <b>Remarks</b>
		 *
		 * Note that there are many ways to create nodes, not just by their @ref create_method
		 * "Create() method".
		 */
		inline XnStatus SetExistingNodeOnly(XnBool bExistingNode)
		{
			return xnNodeQuerySetExistingNodeOnly(m_pQuery, bExistingNode);
		}

		/**
		 * @brief Sets in the Query object that a specified node must be a dependant of the named node type.
		 * @param [in]	strInstanceName Name of the needed node.
		 */
		inline XnStatus AddNeededNode(const XnChar* strInstanceName)
		{
			return xnNodeQueryAddNeededNode(m_pQuery, strInstanceName);
		}

		/**
		 * @brief Sets the creation information field of the Query object. This relates to the creation information of the @ref xn::NodeInfo class.
		 * @param [in]	strCreationInfo Creation information to set in the Query object.
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
	 * @ingroup cppref_graph_mgmt
	 * A NodeInfoList object contains a list of @ref NodeInfo objects.
	 */
	class NodeInfoList
	{
	public:
		/**
		 * Represents an iterator over a @ref NodeInfoList list.
		 */
		class Iterator
		{
		public:
			friend class NodeInfoList;

			/**
			 * Returns whether the iterator points to the same location as another iterator.
			 *
			 * @param [in]	other	Another iterator
			 *
			 * <b>Remarks</b>
			 *
			 * This method can be used for checking if the two iterators are pointing to
			 * the same NodeInfo object.
			 *
			 * Note that the two iterators can also both point to NULL.
			 */
			XnBool operator==(const Iterator& other) const
			{
				return m_it.pCurrent == other.m_it.pCurrent;
			}

			/**
			 * Returns whether the iterator points to a different location than another
			 * iterator.
			 *
			 * @param [in]	other	Another iterator
			 */
			XnBool operator!=(const Iterator& other) const
			{
				return m_it.pCurrent != other.m_it.pCurrent;
			}

			/**
			 * Moves the iterator to the next object. If the end of the list was reached,
			 * the iterator will now be equal to @ref NodeInfoList::End().
			 */
			inline Iterator& operator++()
			{
				UpdateInternalObject(xnNodeInfoListGetNext(m_it));
				return *this;
			}

			/**
			 * Returns an iterator pointing at the next object. If the end of the list was
			 * reached, that iterator will now be equal to @ref NodeInfoList::End().
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

			/**
			 * Gets the @ref NodeInfo object pointed by the iterator.
			 */
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
		 * @param [in]	pList	The list to wrap.
		 */
		inline NodeInfoList(XnNodeInfoList* pList) : m_pList(pList), m_bAllocated(FALSE) {}

		inline ~NodeInfoList()
		{
			FreeImpl();
		}

		/**
		 * Gets the underlying C object.
		 */
		inline XnNodeInfoList* GetUnderlyingObject() const { return m_pList; }

		/**
		 * Replaces the underlying object, taking ownership of the object (meaning, it
		 * will be freed upon destruction).
		 *
		 * @param [in]	pList	The list to wrap.
		 */
		inline void ReplaceUnderlyingObject(XnNodeInfoList* pList)
		{
			FreeImpl();
			m_pList = pList;
			m_bAllocated = TRUE;
		}

		/**
		 * @copybrief xnNodeInfoListAdd
		 * For full details and usage, see @ref xnNodeInfoListAdd
		 */
		inline XnStatus Add(XnProductionNodeDescription& description, const XnChar* strCreationInfo, NodeInfoList* pNeededNodes)
		{
			XnNodeInfoList* pList = (pNeededNodes == NULL) ? NULL : pNeededNodes->GetUnderlyingObject();
			return xnNodeInfoListAdd(m_pList, &description, strCreationInfo, pList);
		}

		/**
		 * @copybrief xnNodeInfoListAddEx
		 * For full details and usage, see @ref xnNodeInfoListAddEx
		 */
		inline XnStatus AddEx(XnProductionNodeDescription& description, const XnChar* strCreationInfo, NodeInfoList* pNeededNodes, const void* pAdditionalData, XnFreeHandler pFreeHandler)
		{
			XnNodeInfoList* pList = (pNeededNodes == NULL) ? NULL : pNeededNodes->GetUnderlyingObject();
			return xnNodeInfoListAddEx(m_pList, &description, strCreationInfo, pList, pAdditionalData, pFreeHandler);
		}

		/**
		 * @copybrief xnNodeInfoListAddNode
		 * For full details and usage, see @ref xnNodeInfoListAddNode
		 */
		inline XnStatus AddNode(NodeInfo& info)
		{
			return xnNodeInfoListAddNode(m_pList, info);
		}

		/**
		 * @copybrief xnNodeInfoListAddNodeFromList
		 * For full details and usage, see @ref xnNodeInfoListAddNodeFromList
		 */
		inline XnStatus AddNodeFromAnotherList(Iterator& it)
		{
			return xnNodeInfoListAddNodeFromList(m_pList, it.m_it);
		}

		/**
		 * Returns an iterator to the beginning of the list. If the list
		 * is empty, that iterator will be equal to @ref NodeInfoList::End().
		 */
		inline Iterator Begin() const
		{
			return Iterator(xnNodeInfoListGetFirst(m_pList));
		}

		/**
		 * Returns an iterator marking the end of the list.
		 * This iterator does not point to a valid item.
		 */
		inline Iterator End() const
		{
			XnNodeInfoListIterator it = { NULL };
			return Iterator(it);
		}

		/**
		 * Returns an iterator to the reverse beginning of the list. If the list
		 * is empty, that iterator will be equal to @ref NodeInfoList::REnd().
		 */
		inline Iterator RBegin() const
		{
			return Iterator(xnNodeInfoListGetLast(m_pList));
		}

		/**
		 * Returns an iterator marking the reverse end of the list.
		 * This iterator does not point to a valid item.
		 */
		inline Iterator REnd() const
		{
			XnNodeInfoListIterator it = { NULL };
			return Iterator(it);
		}

		/**
		 * @copybrief xnNodeInfoListRemove
		 * For full details and usage, see @ref xnNodeInfoListRemove
		 */
		inline XnStatus Remove(Iterator& it)
		{
			return xnNodeInfoListRemove(m_pList, it.m_it);
		}

		/**
		 * @copybrief xnNodeInfoListClear
		 * For full details and usage, see @ref xnNodeInfoListClear
		 */
		inline XnStatus Clear()
		{
			return xnNodeInfoListClear(m_pList);
		}

		/**
		 * @copybrief xnNodeInfoListAppend
		 * For full details and usage, see @ref xnNodeInfoListAppend
		 */
		inline XnStatus Append(NodeInfoList& other)
		{
			return xnNodeInfoListAppend(m_pList, other.GetUnderlyingObject());
		}

		/**
		 * @brief Checks if the list NodeInfoList object is empty.
		 */
		inline XnBool IsEmpty()
		{
			return xnNodeInfoListIsEmpty(m_pList);
		}

		/**
		 * @copybrief xnNodeQueryFilterList
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
	 * @ingroup cppref_prd_func
	 * Base class for all capabilities.
	 */
	class Capability : public NodeWrapper
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	The node handle
		 */
		Capability(XnNodeHandle hNode) : NodeWrapper(hNode) {}
		Capability(const NodeWrapper& node) : NodeWrapper(node) {}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> The Error State capability lets a node report that it has entered
	 * an error state, and so might not function correctly.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref ProductionNode::GetErrorStateCap()
	 * to obtain an instance.
	 *
	 * <b>Remarks:</b>
	 *
	 * An application can use the @ref GetErrorState() method to get the error state of
	 * a node at any time, and it can also register a callback function to be called whenever
	 * the error state changes.
	 *
	 * The error state of a node is represented using @ref XnStatus. A value of @ref XN_STATUS_OK
	 * means the node is OK. Any other value represents some sort of error situation in
	 * the node.
	 *
	 * <b>Event: 'Error State Change'</b>
	 *
	 * Signals that the @ref holder_node "holder node"'s error state has changed.
	 *
	 * This is the same value that can be obtained by the @ref GetErrorState() method.
	 *
	 * This event can be used by the application to get a signal as soon as any error occurs
	 * in the @ref holder_node "holder node". By handling all the error states of all nodes,
	 * the application can use this event to respond immediately to any error situation.
	 *
	 * Use @ref RegisterToErrorStateChange() and @ref UnregisterFromErrorStateChange() for using this event.
	 *
	 * Instead of registering to the error state callback of all the nodes in the context,
	 * an application may register to the 'Global Error State Change' event - see @ref context_global_error_event.
	 */
	class ErrorStateCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		ErrorStateCapability(XnNodeHandle hNode) : Capability(hNode) {}
		ErrorStateCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @brief Gets the current error state of the holding node.
		 */
		inline XnStatus GetErrorState() const
		{
			return xnGetNodeErrorState(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'Error State Change' event.
		 *
		 * @param [in]	handler		Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie		User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback	Handle to the callback. To be used for unregistering the callback.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToErrorStateChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToNodeErrorStateChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'Error State Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromErrorStateChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromNodeErrorStateChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Allows access to general integer capabilities functions - see @ref general_int for
	 * additional details.
	 */
	class GeneralIntCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 * @param [in]	strCap	Name of the capability
		 */
		GeneralIntCapability(XnNodeHandle hNode, const XnChar* strCap) : Capability(hNode), m_strCap(strCap) {}
		GeneralIntCapability(const NodeWrapper& node, const XnChar* strCap) : Capability(node), m_strCap(strCap) {}

		/**
		 * @copybrief xnGetGeneralIntRange
		 * For full details and usage, see @ref xnGetGeneralIntRange
		 */
		inline void GetRange(XnInt32& nMin, XnInt32& nMax, XnInt32& nStep, XnInt32& nDefault, XnBool& bIsAutoSupported) const
		{
			xnGetGeneralIntRange(GetHandle(), m_strCap, &nMin, &nMax, &nStep, &nDefault, &bIsAutoSupported);
		}

		/**
		 * @copybrief xnGetGeneralIntValue
		 * For full details and usage, see @ref xnGetGeneralIntValue
		 */
		inline XnInt32 Get()
		{
			XnInt32 nValue;
			xnGetGeneralIntValue(GetHandle(), m_strCap, &nValue);
			return nValue;
		}

		/**
		 * @copybrief xnSetGeneralIntValue
		 * For full details and usage, see @ref xnSetGeneralIntValue
		 */
		inline XnStatus Set(XnInt32 nValue)
		{
			return xnSetGeneralIntValue(GetHandle(), m_strCap, nValue);
		}

		/**
		 * @brief Registers an event handler for the 'Value Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		XnStatus RegisterToValueChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);

		/**
		 * @brief Unregisters the event handler for the 'Value Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		void UnregisterFromValueChange(XnCallbackHandle hCallback);

	private:
		const XnChar* m_strCap;
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> The ProductionNode class is a base class for all production nodes
	 * and generator nodes. As such, the ProductionNode class is the fundamental base class
	 * of the entire OpenNI interface for building and accessing the production graph.
	 *
	 * <b>Usage:</b> Base class for all production nodes; not usually instantiated.
	 *
	 * <b>Class Overview:</b>
	 *
	 * The OpenNI Production Graph is comprised entirely of production nodes of one type
	 * or another. The ProductionNode class itself exposes a very limited functionality
	 * set. This is the most basic and common functionality set of all the nodes in the
	 * graph.
	 *
	 * All types of core generator nodes are derived from this ProductionNode class, for
	 * example, the @ref xn::DepthGenerator node and the @ref xn::Device node. Each node
	 * type has its own set of methods that it supports.
	 *
	 * Two of the most important methods provided by the ProductionNode class are @ref IsCapabilitySupported()
	 * and @ref GetContext(), both described further below.
	 *
	 * <b>Capabilities Mechanism:</b>
	 *
	 * In addition to being the base class for all core generator nodes, the ProductionNode
	 * class also provides key support for the OpenNI capabilities mechanism. OpenNI defines
	 * core functionality which is always supported by all node implementations of a specific node
	 * type. Additional optional functionality is also defined by OpenNI and exposed
	 * as "capabilities".
	 * The ProductionNode class provides the @ref IsCapabilitySupported() method to check
	 * if this specific implementation supports a certain capability.
	 *
	 * For a comprehensive overview to the Capabilities mechanism see @ref conc_capabilities
	 * "Capabilities Mechanism".
	 */
	class ProductionNode : public NodeWrapper
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline ProductionNode(XnNodeHandle hNode = NULL) : NodeWrapper(hNode) {}
		inline ProductionNode(const NodeWrapper& other) : NodeWrapper(other) {}

		/**
		 * @brief Gets information about the node.
		 *
		 * <b>Remarks</b>
		 *
		 * This method returns a @ref xn::NodeInfo object containing information such as
		 * the node description and the identities of dependent nodes.
		 */
		inline NodeInfo GetInfo() const { return NodeInfo(xnGetNodeInfo(GetHandle())); }

		/**
		 * @copybrief xnAddNeededNode
		 * For full details and usage, see @ref xnAddNeededNode
		 */
		inline XnStatus AddNeededNode(ProductionNode& needed)
		{
			return xnAddNeededNode(GetHandle(), needed.GetHandle());
		}

		/**
		 * @copybrief xnRemoveNeededNode
		 * For full details and usage, see @ref xnRemoveNeededNode
		 */
		inline XnStatus RemoveNeededNode(ProductionNode& needed)
		{
			return xnRemoveNeededNode(GetHandle(), needed.GetHandle());
		}

		/**
		 * @brief Gets the node's context.
		 *
		 * @param	[out]	context		The node's context.
		 *
		 * <b>Remarks</b>
		 *
		 * This method is useful for applications that have to take into account the
		 * possibility of multiple contexts. Using this method, the application can access
		 * all other nodes and other resources within the same context.
		 */
		inline void GetContext(Context& context) const;

		/**
		* @brief Gets the node's context
		*
		* <b>Remarks</b>
		*
		* This method is useful for applications that have to take into account the
		* possibility of multiple contexts. Using this method, the application can access
		* all other nodes and other resources within the same context.
		*/
		inline Context GetContext() const;

		/**
		 * @brief Returns whether a production node supports a specific capability.
		 *
		 * @param	[in]	strCapabilityName	The capability name to check.
		 *
		 * <b>Remarks</b>
		 *
		 * The application specifies the capability by supplying the capability's name in
		 * the <i>strCapabilityName</i> parameter. Before attempting to access any capability
		 * this method should be used to check that this node actually supports this capability.
		 */
		inline XnBool IsCapabilitySupported(const XnChar* strCapabilityName) const
		{
			return xnIsCapabilitySupported(GetHandle(), strCapabilityName);
		}

		/**
		 * @copybrief xnSetIntProperty
		 * For full details and usage, see @ref xnSetIntProperty
		 */
		inline XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue)
		{
			return xnSetIntProperty(GetHandle(), strName, nValue);
		}

		/**
		 * @copybrief xnSetRealProperty
		 * For full details and usage, see @ref xnSetRealProperty
		 */
		inline XnStatus SetRealProperty(const XnChar* strName, XnDouble dValue)
		{
			return xnSetRealProperty(GetHandle(), strName, dValue);
		}

		/**
		 * @copybrief xnSetStringProperty
		 * For full details and usage, see @ref xnSetStringProperty
		 */
		inline XnStatus SetStringProperty(const XnChar* strName, const XnChar* strValue)
		{
			return xnSetStringProperty(GetHandle(), strName, strValue);
		}

		/**
		 * @copybrief xnSetGeneralProperty
		 * For full details and usage, see @ref xnSetGeneralProperty
		 */
		inline XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
		{
			return xnSetGeneralProperty(GetHandle(), strName, nBufferSize, pBuffer);
		}

		/**
		 * @copybrief xnGetIntProperty
		 * For full details and usage, see @ref xnGetIntProperty
		 */
		inline XnStatus GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
		{
			return xnGetIntProperty(GetHandle(), strName, &nValue);
		}

		/**
		 * @copybrief xnGetRealProperty
		 * For full details and usage, see @ref xnGetRealProperty
		 */
		inline XnStatus GetRealProperty(const XnChar* strName, XnDouble &dValue) const
		{
			return xnGetRealProperty(GetHandle(), strName, &dValue);
		}

		/**
		 * @copybrief xnGetStringProperty
		 * For full details and usage, see @ref xnGetStringProperty
		 */
		inline XnStatus GetStringProperty(const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize) const
		{
			return xnGetStringProperty(GetHandle(), strName, csValue, nBufSize);
		}

		/**
		 * @copybrief xnGetGeneralProperty
		 * For full details and usage, see @ref xnGetGeneralProperty
		 */
		inline XnStatus GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
		{
			return xnGetGeneralProperty(GetHandle(), strName, nBufferSize, pBuffer);
		}

		/**
		 * @copybrief xnLockNodeForChanges
		 * For full details and usage, see @ref xnLockNodeForChanges
		 */
		inline XnStatus LockForChanges(XnLockHandle* phLock)
		{
			return xnLockNodeForChanges(GetHandle(), phLock);
		}

		/**
		 * @copybrief xnUnlockNodeForChanges
		 * For full details and usage, see @ref xnUnlockNodeForChanges
		 */
		inline void UnlockForChanges(XnLockHandle hLock)
		{
			xnUnlockNodeForChanges(GetHandle(), hLock);
		}

		/**
		 * @copybrief xnLockedNodeStartChanges
		 * For full details and usage, see @ref xnLockedNodeStartChanges
		 */
		inline XnStatus LockedNodeStartChanges(XnLockHandle hLock)
		{
			return xnLockedNodeStartChanges(GetHandle(), hLock);
		}

		/**
		 * @copybrief xnLockedNodeEndChanges
		 * For full details and usage, see @ref xnLockedNodeEndChanges
		 */
		inline void LockedNodeEndChanges(XnLockHandle hLock)
		{
			xnLockedNodeEndChanges(GetHandle(), hLock);
		}

		/**
		 * @brief Gets an @ref xn::ErrorStateCapability object for accessing the functionality
		 * of the Error State capability.
		 *
		 * <b>Remarks:</b>
		 * The application is responsible for first checking if @ref XN_CAPABILITY_ERROR_STATE
		 * is supported by calling @ref xn::ProductionNode::IsCapabilitySupported().
		 */
		inline const ErrorStateCapability GetErrorStateCap() const
		{
			return ErrorStateCapability(GetHandle());
		}

		/**
		 * @brief Gets an @ref xn::ErrorStateCapability object for accessing the functionality
		 * of the Error State capability.
		 *
		 * <b>Remarks:</b>
		 * The application is responsible for first checking if @ref XN_CAPABILITY_ERROR_STATE
		 * is supported by calling @ref xn::ProductionNode::IsCapabilitySupported()
		 */
		inline ErrorStateCapability GetErrorStateCap()
		{
			return ErrorStateCapability(GetHandle());
		}

		/**
		 * @brief Gets an @ref GeneralIntCapability object for accessing the capability
		 * functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application responsibility to check first if the capability is supported
		 * by calling @ref IsCapabilitySupported().
		 *
		 * @param [in]	strCapability	Name of the capability to get
		 */
		inline GeneralIntCapability GetGeneralIntCap(const XnChar* strCapability)
		{
			return GeneralIntCapability(GetHandle(), strCapability);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> The DeviceIdentificationCapability object allows an application to
	 * receive some device-specific information about a hardware device, like its serial
	 * number and its model name.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref Device::GetIdentificationCap()
	 * to obtain an instance.
	 */
	class DeviceIdentificationCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		DeviceIdentificationCapability(XnNodeHandle hNode) : Capability(hNode) {}
		DeviceIdentificationCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @brief Gets the name of the device holding this capability object.
		 *
		 * @param [in]		strBuffer 		Buffer to accept the device information.
		 * @param [in,out]	nBufferSize	 	Size of the buffer.
		 *
		 * @retval XN_STATUS_OK 					Operation succeeded.
		 * @retval XN_STATUS_OUTPUT_BUFFER_OVERFLOW The size of the buffer is not sufficient.
		 */
		inline XnStatus GetDeviceName(XnChar* strBuffer, XnUInt32 nBufferSize)
		{
			return xnGetDeviceName(GetHandle(), strBuffer, &nBufferSize);
		}

		/**
		 * @brief Gets the vendor-specific name of the device holding this capability object.
		 *
		 * @param [in]		strBuffer 		Buffer to accept the device information.
		 * @param [in,out]	nBufferSize		Size of the buffer.
		 *
		 * @retval XN_STATUS_OK 					Operation succeeded.
		 * @retval XN_STATUS_OUTPUT_BUFFER_OVERFLOW The size of the buffer is not sufficient.
		 */
		inline XnStatus GetVendorSpecificData(XnChar* strBuffer, XnUInt32 nBufferSize)
		{
			return xnGetVendorSpecificData(GetHandle(), strBuffer, &nBufferSize);
		}

		/**
		 * @brief Gets the vendor-specific name of the device holding this capability object.
		 *
		 * @param [in]		strBuffer 		Buffer to accept the serial number
		 * @param [in,out]	nBufferSize		Size of the buffer.
		 *
		 * @retval XN_STATUS_OK 					Operation succeeded.
		 * @retval XN_STATUS_OUTPUT_BUFFER_OVERFLOW The size of the buffer is not sufficient.
		 */
		inline XnStatus GetSerialNumber(XnChar* strBuffer, XnUInt32 nBufferSize)
		{
			return xnGetSerialNumber(GetHandle(), strBuffer, &nBufferSize);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> A smart pointer to a device node in the
	 * production graph of a context.
	 *
	 * The device node represents a physical hardware device
	 * currently connected in the system and used for generating data.
	 * The Device node represents the hardware device by providing, for example,
	 * the device's model name, serial number, and some vendor-specific data.
	 * The Device node itself does not generate data.
	 *
	 * <b>Capabilities:</b> @ref DeviceIdentificationCapability
	 */
	class Device : public ProductionNode
	{
	public:
		inline Device(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
		inline Device(const NodeWrapper& other) : ProductionNode(other) {}

		/**
		 * @brief Creates a Device node from available @ref node_alternative "production node alternatives".
		 *
		 * <b>Remarks</b>
		 *
		 * See @ref create_method "Concepts: the Create() Method" for a detailed description
		 * of the operation of this method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * Gets a @ref DeviceIdentificationCapability object for accessing device identification
		 * functionality.
		 *
		 * <b>Remarks</b>
		 *
		 * If this node supports this method then the node can identify which device is
		 * being used right now, for example, its name, serial number, and specific
		 * vendors can place specific data.
		 *
		 * It is the application's responsibility to check first if @ref XN_CAPABILITY_DEVICE_IDENTIFICATION
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline DeviceIdentificationCapability GetIdentificationCap()
		{
			return DeviceIdentificationCapability(GetHandle());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 *
	 * <b>Purpose:</b> Applying the MirrorCapability to a node allows enabling the Mirroring
	 * mode on a per node basis. Compare this with enabling the Global Mirror, which applies
	 * mirroring to any node supporting the Mirroring capability. To understand mirroring
	 * see the Overview below.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref Generator::GetMirrorCap()
	 * to obtain an instance.
	 *
	 * <b>Remarks:</b>
	 *
	 * When you stand facing the sensor device, and the image is processed directly,
	 * the sensor "sees" your left hand at your right-hand side, and your right hand
	 * at your left-hand side.
	 *
	 * If your movements are used to control an avatar, and that avatar is facing you then
	 * the avatar on the screen is opposite: your right hand is facing the left of the avatar
	 * on the screen, and your left hand is facing the right of the avatar.
	 *
	 * The effect of applying and enabling the Mirroring capability on a node makes all
	 * the data replace right with left, and left with right.
	 *
	 * Thus, if mirroring is <i>disabled</i> (default), then when you raise your right hand,
	 * the avatar on the screen opposite you correspondingly also raises its own right hand,
	 * this being to the left of yourself.
	 *
	 * If mirroring is <i>enabled</i>, when you raise your right hand on the screen the
	 * avatar opposite you mirrors you by raising its hand on your right, which is the avatar's
	 * <i>left</i> hand.
	 *
	 * Enabling mirroring is useful when the avatar is facing the player. Disabling mirroring
	 * is useful when the player sees the avatar from behind (for example in third person
	 * shooters or to see yourself kicking a football).
	 *
	 * Changing the GlobalMirror flag affects mirroring in all existing nodes and in all
	 * nodes to be created in the future - see @ref Context::SetGlobalMirror().
	 *
	 * <b>Event: 'Mirror Change'</b>
	 *
	 * Signals that the @ref holder_node "holder node"'s mirror configuration has been changed (also see @ref event_cmn_conf).
	 *
	 * Use @ref RegisterToMirrorChange() and @ref UnregisterFromMirrorChange() for using this event.
	 *
	 * Calling the @ref SetMirror() method raises this event.
	 */
	class MirrorCapability : public Capability
	{
	public:
		inline MirrorCapability(XnNodeHandle hNode) : Capability(hNode) {}
		MirrorCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @brief Sets the current mirror configuration of the @ref holder_node "holder node",
		 * enabling or disabling mirroring. This configuration is used for generating the next frame.
		 *
		 * @param [in]	bMirror 	TRUE for mirroring output, FALSE otherwise.
		 */
		inline XnStatus SetMirror(XnBool bMirror)
		{
			return xnSetMirror(GetHandle(), bMirror);
		}

		/**
		 * @brief Gets the current mirroring configuration of the @ref holder_node "holder node". This configuration is used for generating the next frame.
		 */
		inline XnBool IsMirrored() const
		{
			return xnIsMirrored(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'Mirror Change' event.
		 *
		 * @param [in]	handler 	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie 	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToMirrorChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToMirrorChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'Mirror Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromMirrorChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromMirrorChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 *
	 * <b>Purpose:</b> The AlternativeViewPoint capability enables any type of map generator
	 * (depth, image, or IR) to transform its data to appear as if the sensor is placed
	 * in another location. The other location is represented by a second production node,
	 * usually representing another sensor.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref Generator::GetAlternativeViewPointCap()
	 * to obtain an instance.
	 *
	 * <b>Remarks:</b>
	 *
	 * This capability provides a solution for the situation where you want to impose
	 * color over depth, but there is then the problem that the two sensing media (transducers)
	 * are looking in slightly different directions. This capability superimposes the two
	 * sensing media of one sensor.
	 *
	 * Other uses are also possible.
	 *
	 * @note  The AlternativeViewPoint capability has nothing to do with the actual physical
	 * position of a sensor. You could move the location of the sensor itself, but this
	 * will not change the current viewpoint.
	 *
	 * <b>Event: 'Viewpoint Change' </b>
	 *
	 * Signals that the @ref holder_node "holder node"'s viewpoint has changed (also see @ref event_cmn_conf).
	 *
	 * Use @ref RegisterToViewPointChange() and @ref UnregisterFromViewPointChange() for using this event.
	 *
	 * Calling the @ref SetViewPoint() or @ref ResetViewPoint() methods raises this event.
	 */
	class AlternativeViewPointCapability : public Capability
	{
	public:
		inline AlternativeViewPointCapability(XnNodeHandle hNode) : Capability(hNode) {}
		AlternativeViewPointCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @brief Checks if the generator node holding this capability can change its output to appear
		 * as if the sensor was placed at the viewpoint of another specific @ref xn::ProductionNode
		 * "production node".
		 *
		 * @param [in]	otherNode 	Other production node, whose viewpoint against which this
		 * 							node's viewpoint is being checked.
		 *
		 * <b>Remarks</b>
		 *
		 * This is not the same as @ref xn::ProductionNode::IsCapabilitySupported(). Although
		 * the node implementation supports this capability, it may nevertheless support only
		 * the viewpoints of nodes of some specific sensors and not of other specific sensors.
		 */
		inline XnBool IsViewPointSupported(ProductionNode& otherNode) const
		{
			return xnIsViewPointSupported(GetHandle(), otherNode.GetHandle());
		}

		/**
		 * @brief Sets the current viewpoint of the holding generator node to look as if it
		 * is placed at a different generator location. All further generated output will appear
		 * as if the sensor was placed at the different location.
		 *
		 * @param [in]	otherNode Viewpoint to be set.
		 */
		inline XnStatus SetViewPoint(ProductionNode& otherNode)
		{
			return xnSetViewPoint(GetHandle(), otherNode.GetHandle());
		}

		/**
		 * @brief Sets the viewpoint of the holding generator node to its normal viewpoint.
		 */
		inline XnStatus ResetViewPoint()
		{
			return xnResetViewPoint(GetHandle());
		}

		/**
		 * @brief Returns whether the @ref holder_node "holder node"'s viewpoint is the same
		 * viewpoint as that of another specified node. Thus, this method is somewhat of an equality operator.
		 *
		 * @param [in]	otherNode 	Other node, whose viewpoint is used for comparison with
		 * 							the viewpoint of this node.
		 */
		inline XnBool IsViewPointAs(ProductionNode& otherNode) const
		{
			return xnIsViewPointAs(GetHandle(), otherNode.GetHandle());
		}

		/**
		 * @brief Registers a handler for the 'Viewpoint Change' event (see the above overview to this class).
		 *
		 * @param [in]	handler		Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie		User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToViewPointChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToViewPointChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the the 'Viewpoint Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromViewPointChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromViewPointChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> The Frame Sync capability allows frame synchronization between two
	 * @ref xn::Generator "generator" nodes producing frame data. This lets the two sensors
	 * output their frames at the same time. This allows smaller latency between two maps
	 * of the same scene, for example, a depth map and an image map.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref Generator::GetFrameSyncCap()
	 * to obtain an instance.
	 *
	 * <b>Event: 'Frame Sync Change'</b>
	 *
	 * Signals that the @ref holder_node "holder node"'s frame synchronization setting has
	 * been changed. That is, this setting has changed from activated to deactivated, or
	 * from deactivated to activated, or the frame synchronization has been changed and
	 * is now synchronized with a different 'other node'.
	 *
	 * Use @ref RegisterToFrameSyncChange() and @ref UnregisterFromFrameSyncChange() for using this event.
	 *
	 * Calling the @ref FrameSyncWith() or @ref StopFrameSyncWith() methods raises this event.
	 */
	class FrameSyncCapability : public Capability
	{
	public:
		inline FrameSyncCapability(XnNodeHandle hNode) : Capability(hNode) {}
		FrameSyncCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @brief Returns whether the generator node holding this capability can
		 * synchronize frames with the other specified node.
		 *
		 * @param [in]	other 	Specifies the other node to be checked whether it can
		 * 						frame-sync with the node holding this capability.
		 * @returns TRUE if frame-sync to the other node is supported; FALSE otherwise.
		 */
		inline XnBool CanFrameSyncWith(Generator& other) const;

		/**
		 * @brief Activates frame synchronization with the other generator node.
		 *
		 * @param [in]	other 	Specifies the other node to be frame sync-ed with the node
		 * 						holding this capability.
		 */
		inline XnStatus FrameSyncWith(Generator& other);

		/**
		 * @brief Removes the frame sync between the node holding this capability and the specified other node.
		 * @param [in]	other 	Specifies the other node from which to remove the frame sync.
		 */
		inline XnStatus StopFrameSyncWith(Generator& other);

		/**
		 * @brief Returns whether a frame sync has been applied between the node holding this capability and
		 * another specified node.
		 * @param [in]	other 	Specifies the other node for checking whether frame sync has
		 * 						been applied.
		 */
		inline XnBool IsFrameSyncedWith(Generator& other) const;

		/**
		 * @brief Registers an event handler for the 'Frame Sync Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToFrameSyncChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToFrameSyncChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the 'Frame Sync Change' event handler.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromFrameSyncChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromFrameSyncChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> A generator node produces some type of data. This is in addition
	 * to the node's basic characteristic, as a derivative of the @ref xn::ProductionNode
	 * class, i.e., that it represents a logical entity in the OpenNI @ref prod_graph "Production
	 * Graph".
	 *
	 * <b>Usage summary</b>: Immediate base class for all generator types. It itself is
	 * not usually instantiated.
	 *
	 * <b>Class Overview:</b>
	 *
	 * Each generator can be in Generating state or Non-Generating state. The application makes
	 * the node enter Generating state by calling the @ref xn::Generator::StartGenerating()
	 * method.
	 *
	 * When a generator node is created, by default it is in Non-Generating state, so it
	 * does not generate data.
	 *
	 * Generators have a default configuration (for example the FOV resolution) and an application
	 * can change the default configuration before actual data generation starts.
	 *
	 * Once the application has completed configuring the generator node, the application
	 * can cause the node to start generating data.
	 *
	 * Typically, the application will configure the generator only in the configuration
	 * stage, i.e., in Non-Generating state. The application can usually also configure
	 * the node while generating data, however configuration might then take longer and
	 * the application could also lose some frames of data. For example, if the application
	 * changes the resolution while in the Configuration stage (Non-Generating state), the
	 * resolution change occurs immediately; but if the application changes the configuration
	 * while the generator is generating data, the resolution change can take up to a few
	 * seconds to occur.
	 *
	 * <b>Data Storage:</b>
	 *
	 * Each call to an @ref conc_updating_data "'Update Data'" method updates the generator
	 * node's application buffer, making the latest data available for applications to access.
	 * After a call to an @ref conc_updating_data "'Update Data'" method, all subsequent
	 * calls to @ref GetData() will return exactly the same data until you call @ref conc_updating_data
	 * "'Update Data'" again. The @ref conc_updating_data "'Update Data'" method can be
	 * the generator node's own @ref WaitAndUpdateData() method or one of the Context's
	 * @ref conc_updating_data "'WaitXUpdateAll'" methods.
	 *
	 * <b>Getting the Generated Data:</b>
	 *
	 * Generator nodes will not output data to make it available for getting, until the
	 * application calls the node's WaitAndUpdateData() method (see below) or one of the
	 * context's @ref conc_updating_data "'WaitXUpdateAll'" methods.
	 *
	 * @ref conc_updating_data__sample_code_cmn
	 */
	class Generator : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline Generator(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
		inline Generator(const NodeWrapper& other) : ProductionNode(other) {}

		/**
		 * @brief Enters the node into Generating state.
		 *
		 * Enters the node into Generating state. In this state the node generates new frames.
		 *
		 * After the application has called this method it can call an @ref conc_updating_data
		 * "'Update Data'" method, e.g., @ref xn::Generator::WaitAndUpdateData(), to make
		 * a new frame available for getting. The application can then get the data (for
		 * example, using a metadata GetData() method, or some other mechanism depending
		 * on the type of node).
		 *
		 * <b>Remarks:</b>
		 *
		 * On entering the node into Generating state, OpenNI enters also all dependent
		 * nodes into Generating state. For example, if a UserGenerator node depends on
		 * data input from a DepthGenerator node, and the UserGenerator node is entered
		 * into Generating state, then OpenNI will enter also the DepthGenerator node into
		 * Generating state.
		 */
		inline XnStatus StartGenerating()
		{
			return xnStartGenerating(GetHandle());
		}

		/**
		 * @brief Returns whether the node is currently in Generating state.
		 */
		inline XnBool IsGenerating() const
		{
			return xnIsGenerating(GetHandle());
		}

		/**
		 * @brief Makes the node leave Generating state (enters Non-Generating state).
		 *
		 * <b>Remarks:</b>
		 *
		 * If the node has metadata, after calling this method the data is still available
		 * as a saved @ref glos_frame_object "frame object" in the metadata object.
		 */
		inline XnStatus StopGenerating()
		{
			return xnStopGenerating(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'Generation State Change' event.
		 *
		 * Registers a callback function to be called when generation starts or
		 * stops, i.e., when the generation state toggles between generating and not
		 * generating.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToGenerationRunningChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle &hCallback)
		{
			return _RegisterToStateChange(xnRegisterToGenerationRunningChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the 'Generation State Change' event handler.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromGenerationRunningChange, GetHandle(), hCallback);
		}

		/**
		 * @brief Registers an event handler for the 'New Data Available' event.
		 *
		 * The 'New Data Available' event is raised whenever a generator node has new data available. The new
		 * data is available for updating, but can not yet be accessed by the application. The application
		 * needs to call @ref conc_updating_data "'Update Data'" to gain access to that new data.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToNewDataAvailable(StateChangedHandler handler, void* pCookie, XnCallbackHandle &hCallback)
		{
			return _RegisterToStateChange(xnRegisterToNewDataAvailable, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'New Data Available' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromNewDataAvailable, GetHandle(), hCallback);
		}

		/**
		 * @brief Returns whether the node has new data available.
		 * The new data is available for updating, but can not yet be accessed by the application. The application
		 * needs to call @ref conc_updating_data "'Update Data'" to gain access to that new data.
		 *
		 * @param	[out]	pnTimestamp		[Optional] If new data is available (TRUE is returned from the method),
		 *									this param will hold the timestamp of that new data.
		 */
		inline XnBool IsNewDataAvailable(XnUInt64* pnTimestamp = NULL) const
		{
			return xnIsNewDataAvailable(GetHandle(), pnTimestamp);
		}

		/**
		 * @brief Updates the generator node to the latest available data, first waiting for the node to have new data available.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method requests from OpenNI to cause the node to update its application buffers
		 * with new data, if it has new data available.
		 *
		 * At this stage the generator node has "generated" new data. This method returns a
		 * success status. The application can now read the newly generated data.
		 *
		 * An error situation is defined as: after a preset timeout, the node has not yet notified
		 * OpenNI it has new data available. On error, the method stops waiting and returns
		 * an error status.
		 *
		 * @retval XN_STATUS_INVALID_OPERATION 	This production node is not a generator.
		 * @retval XN_STATUS_WAIT_DATA_TIMEOUT 	No new data available within 2 seconds.
		 */
		inline XnStatus WaitAndUpdateData()
		{
			return xnWaitAndUpdateData(GetHandle());
		}

		/**
		 * Returns whether this node's frame data was updated by the most recent call to
		 * any @ref conc_updating_data "'WaitXUpdateAll()'" function, e.g., @ref xn::Context::WaitAnyUpdateAll().
		 */
		inline XnBool IsDataNew() const
		{
			return xnIsDataNew(GetHandle());
		}

		/**
		 * @brief Gets the @ref frame_data "frame data" from the generator node. This is
		 * the latest data that the generator node has generated.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method gets the data directly from the Generator node; not from the metadata
		 * object.
		 *
		 * Compare this method with the Data() method of the @ref xn::OutputMetaData object,
		 * where the latter gets the data from the metadata object.
		 * GetData() returns the same data as the GetMetaData().Data() method. However,
		 * using the GetData() method is slightly more costly.
		 *
		 * The application usually should not use this method, but call each generator own
		 * data access methods, e.g. DepthGenerator::GetDepthMap().
		 */
		inline const void* GetData()
		{
			return xnGetData(GetHandle());
		}

		/**
		 * @brief Gets the data size of the @ref frame_data "frame data".
		 * This method gets the data size directly from the Generator node.
		 *
		 * <b>Remarks:</b>
		 *
		 * The size of the data is returned as a number of bytes.
		 *
		 * Compare this method with the DataSize() method of the @ref xn::OutputMetaData
		 * object, where the latter gets the data from the metadata object.
		 * GetDataSize() returns the same data as the GetMetaData().DataSize() method. However,
		 * using the GetDataSize() method is slightly more costly.
		 */
		inline XnUInt32 GetDataSize() const
		{
			return xnGetDataSize(GetHandle());
		}

		/**
		 * @brief Gets the frame timestamp from the <b>Generator</b> node
		 * This method gets the timestamp directly from the Generator node.
		 *
		 * <b>Remarks:</b>
		 *
		 * The time is returned in microseconds.
		 *
		 * Compare this method with the <b>GetTimestamp()</b> method of the @ref xn::OutputMetaData
		 * object, where the latter gets the data from the metadata object.
		 * <b>GetTimestamp()</b> returns the same data as the GetMetaData().GetTimestamp()
		 * method. However, using the <b>GetTimestamp()</b> method is slightly more costly.
		 */
		inline XnUInt64 GetTimestamp() const
		{
			return xnGetTimestamp(GetHandle());
		}

		/**
		 * @brief Gets the frame ID of the current frame data from the Generator node.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method gets the frame ID directly from the Generator node.
		 *
		 * Compare this method with the <b>GetFrameID()</b> method of the @ref xn::OutputMetaData
		 * object, where the latter gets the data from the metadata object.
		 * <b>GetFrameID()</b> returns the same data as the GetMetaData().GetFrameID() method.
		 * However, using the <b>GetFrameID()</b> method is slightly more costly.
		 */
		inline XnUInt32 GetFrameID() const
		{
			return xnGetFrameID(GetHandle());
		}

		/**
		 * @brief Gets a @ref MirrorCapability object for accessing Mirror functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application's responsibility to check first if @ref XN_CAPABILITY_MIRROR
		 * is supported by calling @ref xn::ProductionNode::IsCapabilitySupported().
		 */
		inline const MirrorCapability GetMirrorCap() const
		{
			return MirrorCapability(GetHandle());
		}

		/**
		 * @brief Gets a @ref MirrorCapability object for accessing Mirror functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application's responsibility to check first if @ref XN_CAPABILITY_MIRROR
		 * is supported by calling @ref xn::ProductionNode::IsCapabilitySupported().
		 */
		inline MirrorCapability GetMirrorCap()
		{
			return MirrorCapability(GetHandle());
		}

		/**
		 * Gets an @ref AlternativeViewPointCapability object for accessing Alternative
		 * Viewpoint functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application's responsibility to check first if
		 * @ref XN_CAPABILITY_ALTERNATIVE_VIEW_POINT is supported
		 * by calling @ref xn::Generator::IsCapabilitySupported().
		 */
		inline const AlternativeViewPointCapability GetAlternativeViewPointCap() const
		{
			return AlternativeViewPointCapability(GetHandle());
		}

		/**
		 * Gets an @ref AlternativeViewPointCapability object for accessing Alternative
		 * Viewpoint functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application's responsibility to check first if
		 * @ref XN_CAPABILITY_ALTERNATIVE_VIEW_POINT is supported
		 * by calling @ref xn::Generator::IsCapabilitySupported().
		 */
		inline AlternativeViewPointCapability GetAlternativeViewPointCap()
		{
			return AlternativeViewPointCapability(GetHandle());
		}

		/**
		 * Gets an @ref FrameSyncCapability object for accessing Frame Sync functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application's responsibility to check first if
		 * @ref XN_CAPABILITY_FRAME_SYNC is supported
		 * by calling @ref xn::Generator::IsCapabilitySupported().
		 */
		inline const FrameSyncCapability GetFrameSyncCap() const
		{
			return FrameSyncCapability(GetHandle());
		}

		/**
		 * Gets an @ref FrameSyncCapability object for accessing Frame Sync functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application's responsibility to check first if
		 * @ref XN_CAPABILITY_FRAME_SYNC is supported
		 * by calling @ref xn::Generator::IsCapabilitySupported().
		 */
		inline FrameSyncCapability GetFrameSyncCap()
		{
			return FrameSyncCapability(GetHandle());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> To create and manage a Recorder node.
	 *
	 * <b>Usage: </b> Instatiate and create a Recorder node.
	 *
	 * <b>Data output:</b> Records to a specified destination medium the frames of data
	 * from each node that was added to the Recorder node.
	 *
	 * <b>Remarks:</b>
	 *
	 * Recordings are a powerful debug tool. A recording enables a full capture of data
	 * generation and the ability to later play it back by 'regeneration' so that applications
	 * can simulate an exact replica of the situation to be debugged.
	 *
	 * OpenNI supports recordings of the production nodes in the production graph, both
	 * the entire configuration of each node, and all data streamed from a node.
	 *
	 * OpenNI has a framework for recording data and for playing it back (using mock nodes).
	 * It also comes with the nimRecorder module, which defines a new file format (".ONI")
	 * - and implements a Recorder node and a Player node for this format.
	 *
	 * If a recorder exists while generating, the @ref conc_updating_data "'WaitXUpdateAll'"
	 * functions automatically record the data from all nodes added to this recorder.
	 *
	 * To record, an application creates a Recorder node, and set its destination - the
	 * file name to which it should write. The application must add to the Recorder node
	 * every production node that it wants to record. When adding a node to the recorder,
	 * the Recorder stores its configuration. It also registers to every possible event
	 * of the node, so that when any configuration change takes place, it is also recorded.
	 *
	 * Once all required nodes are added, the application can read data from the nodes and
	 * record it. Recording of data can be achieved either by explicitly calling the @ref
	 * xn::Recorder::Record() function, or by using one of the @ref conc_updating_data "'Update
	 * All'" functions.
	 *
	 * Applications that initialize OpenNI using an XML file can easily record their session
	 * without any change to the code. All that is required is that they create an additional
	 * node in the XML file for the recorder, add nodes to it, and when the application
	 * calls one of the 'WaitXUpdateAll' functions, recording will occur. see @ref xmlscripts
	 * for explanation on how to do that.
	 *
	 * Recordings are played back by using the services of the Player class.
	 */
	class Recorder : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	A node handle
		 */
		inline Recorder(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
		inline Recorder(const NodeWrapper& other) : ProductionNode(other) {}

		/**
		 * @brief Creates a Recorder node.
		 *
		 * @param [in]	context 		The context in which to create the Recorder node.
		 * @param [in]	strFormatName 	Optional. The name of the format the recorded file
		 * 								will be in. If NULL, file will be in ONI format.
		 */
		inline XnStatus Create(Context& context, const XnChar* strFormatName = NULL);

		/**
		 * @brief Specifies to where the recorder must send its recording. Typically this is a disk file of a particular file type.
		 *
		 * @param [in]	destType 	The type of medium to record to. Currently only XN_RECORD_MEDIUM_FILE
		 * 							is supported.
		 * @param [in]	strDest 	Recording destination. If destType is XN_RECORD_MEDIUM_FILE,
		 * 							this specifies a file name.
		 */
		inline XnStatus SetDestination(XnRecordMedium destType, const XnChar* strDest)
		{
			return xnSetRecorderDestination(GetHandle(), destType, strDest);
		}

		/**
		 * @brief Gets the destination medium for the Recorder node to record to.
		 *
		 * @param [out]	destType 	The type of medium to record to. Currently only XN_RECORD_MEDIUM_FILE
		 * 							is supported.
		 * @param [out]	strDest 	Recording destination. If destType is XN_RECORD_MEDIUM_FILE,
		 * 							this specifies a file name.
		 * @param [in]	nBufSize	Destination buffer size.
		 */
		inline XnStatus GetDestination(XnRecordMedium& destType, XnChar* strDest, XnUInt32 nBufSize)
		{
			return xnGetRecorderDestination(GetHandle(), &destType, strDest, nBufSize);
		}

		/**
		 * @brief Adds a node to the recording setup, and starts recording data what the node generates.
		 * This method must be called for each node to be recorded with this recorder.
		 *
		 * @param [in]	Node 			Node to add to the recording.
		 * @param [in]	compression 	Type of compression that will be used to encode the node's
		 * 								data (see @ref XnCodecIDs.h). If @ref XN_CODEC_NULL
		 *								is specified, a default compression will be chosen according to the node type.
		 */
		inline XnStatus AddNodeToRecording(ProductionNode& Node, XnCodecID compression = XN_CODEC_NULL)
		{
			return xnAddNodeToRecording(GetHandle(), Node.GetHandle(), compression);
		}

		/**
		 * @brief Removes a node from the Recorder node and stops recording the node output.
		 * This function is called on a node that was added to recording with @ref AddNodeToRecording.
		 *
		 * @param [in]	Node 	The node to remove from recording.
		 */
		inline XnStatus RemoveNodeFromRecording(ProductionNode& Node)
		{
			return xnRemoveNodeFromRecording(GetHandle(), Node.GetHandle());
		}

		/**
		 * @brief Records one frame of data from each node that was added to the recorder with AddNodeToRecording().
		 *
		 * <b>Remarks</b>
		 *
		 * To record continually, the recorder node must be called repeatedly for each frame.
		 *
		 * It is highly recommended that an application use the 'WaitXUpdateAll' methods,
		 * and not call Generator::WaitAndUpdateData() for each generator. When working
		 * with the 'WaitXUpdateAll' methods, OpenNI will automatically call @ref Record(),
		 * and there is no need for the application to call this method.
		 */
		inline XnStatus Record()
		{
			return xnRecord(GetHandle());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 *
	 * <b>Purpose:</b> The Player class is used for creating and managing a Player node.
	 * This node plays a saved recording of an OpenNI data generation session.
	 *
	 * <b>Remarks:</b>
	 *
	 * To play a file recording, use the @ref xn::Context::OpenFileRecording() function.
	 * OpenNI will open the file, create a mock node for each node in the file, and populate
	 * it with the recorded configuration.
	 *
	 * An application may take the nodes it needs by calling the @ref xn::Context::FindExistingNode()
	 * method, and use them normally.
	 *
	 * @note Nodes created by the player are locked, and cannot be changed, as the
	 * configuration must remain according to the recorded configuration.
	 *
	 * Applications that initialize OpenNI using an XML file can easily replace their input.
	 * This means that instead of reading from a physical device, which generates data in
	 * real-time, they read from a recording by replacing the nodes in the XML file with
	 * a recording element (see @ref xmlscripts).
	 *
	 * @note It is not the Player node itself that is generating data. (Accordingly, the
	 * Player node does not provide a GetData() method.) The Player node reads data from
	 * the file, and "pushes" it into the mock nodes.
	 */
	class Player : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Player node handle
		 */
		inline Player(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
		inline Player(const NodeWrapper& other) : ProductionNode(other) {}

		/**
		 * @brief Creates a player node.
		 *
		 * <b>Remarks</b>
		 *
		 * See @ref create_method "Concepts: the Create() Method" for a detailed description
		 * about the operation of this method.
		 */
		inline XnStatus Create(Context& context, const XnChar* strFormatName);


		/**
		 * @brief Specifies whether the player will automatically rewind to the beginning
		 * of the recording after reaching the end of the recording.
		 *
		 * @param [in]	bRepeat 	Determines whether the player will repeat or not.
		 *
		 * If bRepeat is set to TRUE, the player will automatically
		 * rewind when reaching the end.
		 *
		 * If bRepeat is set to FALSE, the player will stop playing
		 * when reaching the end, and will raise the 'End-Of-File Reached'
		 * event. In this state, @ref IsEOF() returns TRUE, and
		 * calls to @ref ReadNext() will fail.
		 */
		inline XnStatus SetRepeat(XnBool bRepeat)
		{
			return xnSetPlayerRepeat(GetHandle(), bRepeat);
		}

		/**
		 * @copybrief xnSetPlayerSource
		 * For full details and usage, see @ref xnSetPlayerSource
		 */
		inline XnStatus SetSource(XnRecordMedium sourceType, const XnChar* strSource)
		{
			return xnSetPlayerSource(GetHandle(), sourceType, strSource);
		}

		/**
		 * @brief Gets the player's source, that is, the type and name of the medium
		 * that the recording is played back from.
		 *
		 * @param [out]	sourceType 	Player's source type.
		 * @param [in]	strSource 	Player's source.
		 * @param [in]	nBufSize 	Size of the buffer specified by @c strSource.
		 */
		inline XnStatus GetSource(XnRecordMedium &sourceType, XnChar* strSource, XnUInt32 nBufSize) const
		{
			return xnGetPlayerSource(GetHandle(), &sourceType, strSource, nBufSize);
		}

		/**
		 * @copybrief xnPlayerReadNext
		 * For full details and usage, see @ref xnPlayerReadNext
		 */
		inline XnStatus ReadNext()
		{
			return xnPlayerReadNext(GetHandle());
		}

		/**
		 * @brief Moves the player to a specific time, so that playback will continue from that point onwards.
		 *
		 * @param [in]	nTimeOffset 	Offset to move, relative to the specified origin. <br>
		 * 								Units in microseconds. <br>  See the remark below.
		 * @param [in]	origin 			Origin to seek from. See the remarks below.
		 *
		 * The meaning of the @c nTimeOffset parameter changes according to the origin parameter:
		 *
		 * <b>XN_PLAYER_SEEK_SET:</b>	@c nTimeOffset specifies the total time passed since
		 * the beginning of the recording. This must be a positive value.
		 *
		 * <b>XN_PLAYER_SEEK_CUR:</b>	@c nTimeOffset specifies a period of time to move, relative
		 * to the current player position. A positive value means to move forward, and a
		 * negative value means to move backwards.
		 *
		 * <b>XN_PLAYER_SEEK_END:</b>	@c nTimeOffset specifies a period of time to move, relative
		 * to the end of the recording. This must be a negative value.
		 *
		 * <b>Remarks</b>
		 *
		 * For the built-in ONI player, this function is not currently supported and always
		 * returns XN_STATUS_NOT_IMPLEMENTED.
		 */
		inline XnStatus SeekToTimeStamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin)
		{
			return xnSeekPlayerToTimeStamp(GetHandle(), nTimeOffset, origin);
		}

		/**
		 * @brief Moves the player to a specific frame of a specific played node, e.g., an
		 * ImageGenerator node, so that playing will continue from that frame onwards.
		 *
		 * @param [in]	strNodeName 	Name of the node.
		 * @param [in]	nFrameOffset 	Number of frames to move, relative to the specified
		 * 								origin. See remarks below.
		 * @param [in]	origin 			Origin to seek from. See remarks below.
		 *
		 * The meaning of the @c nFrameOffset parameter changes according to the origin parameter:
		 *
		 * <b>XN_PLAYER_SEEK_SET:</b> @c nFrameOffset specifies the total number of frames
		 * since the beginning of the node's recording. This must be a positive value.
		 *
		 * <b>XN_PLAYER_SEEK_CUR:</b> @c nFrameOffset specifies the number of frames to move,
		 * relative to the current frame of the specified node. A positive value means to
		 * move forward, and a negative value means to move backwards.
		 *
		 * <b>XN_PLAYER_SEEK_END:</b> @c nFrameOffset specifies the number of frames to move,
		 * relative to the end of the node's recording. This must be a negative value.
		 *
		 * You can get different results using this function for different values of strNodeName,
		 * because the frame numbers of different nodes are not necessarily in sync.
		 */
		inline XnStatus SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin)
		{
			return xnSeekPlayerToFrame(GetHandle(), strNodeName, nFrameOffset, origin);
		}

		/**
		 * @brief Gets the current time of a player, i.e., the time passed since the beginning of the recording.
		 *
		 * @param [out]	nTimestamp 	Retrieved timestamp. Units are in microseconds.
		 */
		inline XnStatus TellTimestamp(XnUInt64& nTimestamp) const
		{
			return xnTellPlayerTimestamp(GetHandle(), &nTimestamp);
		}

		/**
		 * @brief Gets the current frame number of a specific node played by a player,
		 * i.e., the number of frames passed since the beginning of the recording
		 *
		 * @param [in]	strNodeName 	Name of the node.
		 * @param [out]	nFrame 			Retrieved frame number.
		 *
		 * <b>Remarks</b>
		 *
		 * Different nodes that belong to the player can have different frame numbers, because
		 * the nodes are not necessarily in synchronization.
		 */
		inline XnStatus TellFrame(const XnChar* strNodeName, XnUInt32& nFrame) const
		{
			return xnTellPlayerFrame(GetHandle(), strNodeName, &nFrame);
		}

		/**
		 * @brief Gets the total number of frames a specific node has in the recording.
		 *
		 * @param [in]	strNodeName 	Name of the node for which to retrieve the number of frames.
		 * @param [out]	nFrames 		Retrieved number of frames.
		 *
		 */
		inline XnStatus GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames) const
		{
			return xnGetPlayerNumFrames(GetHandle(), strNodeName, &nFrames);
		}

		/**
		 * @copybrief xnGetPlayerSupportedFormat
		 * For full details and usage, see @ref xnGetPlayerSupportedFormat
		 */
		inline const XnChar* GetSupportedFormat() const
		{
			return xnGetPlayerSupportedFormat(GetHandle());
		}

		/**
		 * @copybrief xnEnumeratePlayerNodes
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

		/**
		 * @brief Returns whether the player is at the end-of-file marker.
		 *
		 * <b>Remarks</b>
		 *
		 * When Repeat mode is on, this function never returns TRUE.
		 * However, the 'End of File Reached' event will be raised during rewind.
		 */
		inline XnBool IsEOF() const
		{
			return xnIsPlayerAtEOF(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'End-Of-File Reached' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToEndOfFileReached(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToEndOfFileReached, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'End-Of-File Reached' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromEndOfFileReached(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromEndOfFileReached, GetHandle(), hCallback);
		}

		/**
		 * @brief Sets the player's playback speed, as a ratio of the rate that the recording was made at.
		 *
		 * @param [in]	dSpeed 		The speed ratio.
		 *
		 * <b>Values: </b>
		 *
		 * @c dSpeed = 1.0 - Player will try to output frames at the same rate they were recorded
		 * (according to their timestamps).
		 *
		 * @c dSpeed > 1.0 - Fast-forward
		 *
		 * 0 < @c dSpeed < 1.0 - Slow-motion.
		 *
		 * @c dSpeed = XN_PLAYBACK_SPEED_FASTEST (0.0) - There will be no delay, and frames
		 * will be returned immediately on demand.
		 *
		 */
		inline XnStatus SetPlaybackSpeed(XnDouble dSpeed)
		{
			return xnSetPlaybackSpeed(GetHandle(), dSpeed);
		}

		/**
		 * @brief Gets the playback speed.
		 * @returns Speed ratio, or -1.0 if this node is not a player.
		 */
		inline XnDouble GetPlaybackSpeed() const
		{
			return xnGetPlaybackSpeed(GetHandle());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 *
	 * <b>Purpose:</b> Provides Cropping capability for @ref MapGenerator "map generator nodes".
	 * Cropping capability restricts the size of the data frames to be generated by the
	 * MapGenerator node to a selected (cropped) area of the full field-of-view.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref MapGenerator::GetCroppingCap()
	 * to obtain an instance.
	 *
	 * <b>Remarks:</b>
	 *
	 * When cropping is enabled, the size of the generated map is reduced to fit a lower
	 * resolution (smaller FOV dimensions). For example, if the map generator is working
	 * in VGA resolution (640x480) and the application chooses to crop at 300x200, the next
	 * pixel row will begin after 300 pixels. Pixels outside the cropping area are not returned
	 * in the buffer - they are not blacked out; they are not returned at all. Thus, cropping
	 * is very useful for providing a performance boost.
	 *
	 * <b>Event: 'Cropping Change'</b>
	 *
	 * Signals that the @ref holder_node "holder node"'s cropping area has been changed.
	 *
	 * Use @ref RegisterToCroppingChange() and @ref UnregisterFromCroppingChange() for using this event.
	 *
	 * Calling the @ref SetCropping() method raises this event.
	 */
	class CroppingCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline CroppingCapability(XnNodeHandle hNode) : Capability(hNode) {}
		CroppingCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @brief Sets the @ref holder_node "holder node"'s current cropping
		 * configuration. This configuration is used for generating the next frame.
		 *
		 * @param [in]	Cropping 	Cropping configuration to be set.
		 *
		 * @retval XN_STATUS_INVALID_OPERATION 	The @ref MapGenerator node does not support
		 * 										the Cropping capability.
		 */
		inline XnStatus SetCropping(const XnCropping& Cropping)
		{
			return xnSetCropping(GetHandle(), &Cropping);
		}

		/**
		 * @brief Gets the current cropping configuration of the @ref holder_node "holder node".
		 * This configuration is that set by @ref SetCropping(). This configuration is used
		 * for generating the next frame.
		 *
		 * @param [out]	Cropping 	Retreived cropping configuration.
		 *
		 * @retval XN_STATUS_INVALID_OPERATION 	The @ref MapGenerator node does not
		 * 										support the Cropping capability.
		 */
		inline XnStatus GetCropping(XnCropping& Cropping) const
		{
			return xnGetCropping(GetHandle(), &Cropping);
		}

		/**
		 * @brief Registers an event handler for the 'Cropping Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToCroppingChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToCroppingChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'Cropping Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromCroppingChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromCroppingChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Allows access to <b>Anti Flicker</b> capability functions. see @ref anti_flicker
	 * for additional details.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref MapGenerator::GetAntiFlickerCap()
	 * to obtain an instance.
	 */
	class AntiFlickerCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline AntiFlickerCapability(XnNodeHandle hNode) : Capability(hNode) {}
		AntiFlickerCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @copybrief xnSetPowerLineFrequency
		 * For full details and usage, see @ref xnSetPowerLineFrequency
		 */
		inline XnStatus SetPowerLineFrequency(XnPowerLineFrequency nFrequency)
		{
			return xnSetPowerLineFrequency(GetHandle(), nFrequency);
		}

		/**
		 * @copybrief xnGetPowerLineFrequency
		 * For full details and usage, see @ref xnGetPowerLineFrequency
		 */
		inline XnPowerLineFrequency GetPowerLineFrequency()
		{
			return xnGetPowerLineFrequency(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'Power Line Frequency Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToPowerLineFrequencyChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToPowerLineFrequencyChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'Power Line Frequency Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromPowerLineFrequencyChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromPowerLineFrequencyChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 *
	 * <b>Purpose:</b> The MapGenerator class is the base class for every generator node
	 * that produces 2D map data, for example, depth maps, color image maps, IR maps, and
	 * scene analysis maps.
	 *
	 * <b>Usage: </b> This class might be instantiated to reference any kind of map generator
	 * node (i.e., a node that derives from the MapGenerator class), however it is not intended
	 * to ever be instantiated to create any node of MapGenerator type itself.
	 *
	 * <b>Remarks:</b>
	 *
	 * Since the MapGenerator class is the base class for generator nodes that produce image
	 * maps, it provides the data structures common to all map generators: the current frame's
	 * X and Y dimensions and its frame rate.
	 *
	 * MapGenerator's two principle functions are GetMapOutputMode() and SetMapOutputMode()
	 * for controlling and checking the node's map output mode. The map output mode is the
	 * combination of the node's scene resolution and frame rate. Correspondingly, these
	 * functions read and write an @ref xn::XnMapOutputMode structure, comprising three
	 * parameters: the X and Y resolutions and the frame rate.
	 */
	class MapGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline MapGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}
		inline MapGenerator(const NodeWrapper& other) : Generator(other) {}

		/**
		 * @brief Gets the number of output modes that the generator node supports.
		 *
		 * <b>Remarks</b>
		 *
		 * This is useful for allocating an array that will be passed to GetSupportedMapOutputModes().
		 */
		inline XnUInt32 GetSupportedMapOutputModesCount() const
		{
			return xnGetSupportedMapOutputModesCount(GetHandle());
		}

		/**
		 * @brief Gets a list of all the output modes that the generator node supports.
		 *
		 * <b>Remarks</b>
		 *
		 * Each supported map output mode is represented by a @ref XnMapOutputMode structure
		 * comprising resolution (@c nXRes and @c nYRes) and frames rate (@c nFPS). The application
		 * then chooses one of the output modes. The size of the array that should be passed
		 * can be obtained by calling GetSupportedMapOutputModesCount().
		 */
		inline XnStatus GetSupportedMapOutputModes(XnMapOutputMode* aModes, XnUInt32& nCount) const
		{
			return xnGetSupportedMapOutputModes(GetHandle(), aModes, &nCount);
		}

		/**
		 * @brief Sets the current map output mode of the generator node.
		 *
		 * <b>Remarks</b>
		 *
		 * A map output mode is contained in a @ref xn::XnMapOutputMode structure comprising
		 * the frame rate (in frames per second), and the X and Y dimensions (the number
		 * of elements in each of the X- and Y- axes).
		 *
		 * It is the application programmer's responsibility to check first if the output
		 * mode is supported by calling the @ref xn::MapGenerator::GetSupportedMapOutputModes()
		 * "GetSupportedMapOutputModes()" method.
		 */
		inline XnStatus SetMapOutputMode(const XnMapOutputMode& OutputMode)
		{
			return xnSetMapOutputMode(GetHandle(), &OutputMode);
		}

		/**
		 * @brief Gets the current map output mode of the generator node. This is the
		 * map output mode that the generator node will use to generate the next data frame.
		 *
		 * <b>Remarks</b>
		 *
		 * A map output mode is the combination of the scene resolution and frame rate.
		 * It is contained in an @ref xn::XnMapOutputMode structure comprising the frame
		 * rate (in frames per second), and the X and Y dimensions (the number of elements
		 * in each of the X- and Y- axes).
		 *
		 * For the map output mode of the most recent data frame available in the node's
		 * metadata object, see the @ref xn::MapMetaData class (MapMetaData::Xres(), MapMetaData::YRes(),
		 * and MapMetaData::FPS() methods).
		 *
		 * This map output mode is the mode set by @ref SetMapOutputMode().
		 */
		inline XnStatus GetMapOutputMode(XnMapOutputMode &OutputMode) const
		{
			return xnGetMapOutputMode(GetHandle(), &OutputMode);
		}

		/**
		 * @brief Gets the number of bytes per pixel for the node's map data. This mode
		 * is set by SetPixelFormat() for the next frame that the generator node generates.
		 *
		 * <b>Remarks</b>
		 *
		 * In ImageGenerator this can be set. In other generators, it's pre-defined.
		 */
		inline XnUInt32 GetBytesPerPixel() const
		{
			return xnGetBytesPerPixel(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'Map Output Mode Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToMapOutputModeChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToMapOutputModeChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'Map Output Mode Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromMapOutputModeChange, GetHandle(), hCallback);
		}

		/**
		 * @brief Gets a @ref CroppingCapability object for accessing Cropping functionality.
		 *
		 * <b>Remarks</b>
		 *
		 * It is the application's responsibility to check first if @ref XN_CAPABILITY_CROPPING
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline const CroppingCapability GetCroppingCap() const
		{
			return CroppingCapability(GetHandle());
		}

		/**
		 * @brief Gets a @ref CroppingCapability object for accessing Cropping functionality.
		 *
		 * <b>Remarks</b>
		 *
		 * It is the application's responsibility to check first if @ref XN_CAPABILITY_CROPPING
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline CroppingCapability GetCroppingCap()
		{
			return CroppingCapability(GetHandle());
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Brightness functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_BRIGHTNESS
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetBrightnessCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_BRIGHTNESS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Contrast functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_CONTRAST
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetContrastCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_CONTRAST);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Hue functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_HUE
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetHueCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_HUE);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Saturation functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_SATURATION
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetSaturationCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_SATURATION);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Sharpness functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_SHARPNESS
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetSharpnessCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_SHARPNESS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Gamma functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_GAMMA
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetGammaCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_GAMMA);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing WhiteBalance functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_COLOR_TEMPERATURE
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetWhiteBalanceCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_COLOR_TEMPERATURE);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing BacklightCompensation functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_BACKLIGHT_COMPENSATION
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetBacklightCompensationCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_BACKLIGHT_COMPENSATION);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Gain functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_GAIN
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetGainCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_GAIN);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Pan functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_PAN
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetPanCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_PAN);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Tilt functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_TILT
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetTiltCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_TILT);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Roll functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ROLL
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetRollCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_ROLL);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Zoom functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ZOOM
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetZoomCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_ZOOM);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Exposure functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_EXPOSURE
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetExposureCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_EXPOSURE);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Iris functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_IRIS
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetIrisCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_IRIS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Focus functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_FOCUS
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetFocusCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_FOCUS);
		}

		/**
		 * Gets an @ref GeneralIntCapability object for accessing Low Light Compensation functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_LOW_LIGHT_COMPENSATION
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline GeneralIntCapability GetLowLightCompensationCap()
		{
			return GeneralIntCapability(GetHandle(), XN_CAPABILITY_LOW_LIGHT_COMPENSATION);
		}

		/**
		 * Gets an @ref AntiFlickerCapability object for accessing Anti Flicker functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_ANTI_FLICKER
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline AntiFlickerCapability GetAntiFlickerCap()
		{
			return AntiFlickerCapability(GetHandle());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Allows access to <b>User Position</b> capability functions. see @ref userpos for
	 * additional details.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref DepthGenerator::GetUserPositionCap()
	 * to obtain an instance.
	 */
	class UserPositionCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline UserPositionCapability(XnNodeHandle hNode = NULL) : Capability(hNode) {}
		UserPositionCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @copybrief xnGetSupportedUserPositionsCount
		 * For full details and usage, see @ref xnGetSupportedUserPositionsCount
		 */
		inline XnUInt32 GetSupportedUserPositionsCount() const
		{
			return xnGetSupportedUserPositionsCount(GetHandle());
		}

		/**
		 * @copybrief xnSetUserPosition
		 * For full details and usage, see @ref xnSetUserPosition
		 */
		inline XnStatus SetUserPosition(XnUInt32 nIndex, const XnBoundingBox3D& Position)
		{
			return xnSetUserPosition(GetHandle(), nIndex, &Position);
		}

		/**
		 * @copybrief xnGetUserPosition
		 * For full details and usage, see @ref xnGetUserPosition
		 */
		inline XnStatus GetUserPosition(XnUInt32 nIndex, XnBoundingBox3D& Position) const
		{
			return xnGetUserPosition(GetHandle(), nIndex, &Position);
		}

		/**
		 * @brief Registers an event handler for the 'User Position Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToUserPositionChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToUserPositionChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'User Position Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromUserPositionChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromUserPositionChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> A DepthGenerator node generates depth maps. Each pixel value of
	 * a depth map represents a distance from the sensor's plane in millimeters.
	 *
	 * <b>Usage:</b> Instantiate the class and call Create() to create a DepthGenerator
	 * node.
	 *
	 * This class is at the first level of classes in the OpenNI class hierarchy that is
	 * typically instantiated to create an actual node. (Examples of other classes at the
	 * first level are @ref xn::ImageGenerator and @ref xn::SceneAnalyzer).
	 *
	 * <b>Data output:</b> Depth maps and associated configuration data. Access via the node's
	 * @ref xn::DepthMetaData object.
	 *
	 * <b>Overview:</b>
	 *
	 * A DepthGenerator node generates a depth map as an array of pixels, where each pixel
	 * is a depth value representing a distance from the sensor's plane in millimeters. This is
	 * the Z-coordinate of the X-Y-Z coordinate of each user pixel. A smaller depth value
	 * indicates a scene point that is <i>closer</i> to the hardware sensor device; and
	 * a larger depth value indicates a scene point that is a <i>further away</i> from the
	 * sensor. A zero depth value indicates that the DepthGenerator node did not succeed
	 * in obtaining a valid depth reading.
	 *
	 * Typically, the process of generating 3D data begins by using a sensor device that
	 * produces raw output data. Dedicated middleware is then used to process this raw output,
	 * and produce a higher-level output, which can then be understood and used by the application.
	 *
	 * In a typical OpenNI production graph, a DepthGenerator node takes input directly
	 * from a sensor device and generates a depth map. Other generator nodes further along
	 * the production graph (dependant on the DepthGenerator node) take input from the DepthGenerator
	 * node's output. However, alternative dependency configurations are also possible.
	 *
	 * The DepthGenerator node's data and associated configuration are accessed through
	 * its @ref glos_frame_object "frame object" (i.e., its @ref xn::DepthMetaData object).
	 *
	 * Summary of Main Features:
	 * - Depth map: Provides the data of the depth map
	 * - Sensor device's maximum depth: Provides the maximum distance available for this
	 *   depth generator
	 * - Field of View: Provides the sensor's maximum horizontal and vertical angles
	 * - User Position capability (optional)
	 */
	class DepthGenerator : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline DepthGenerator(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}
		inline DepthGenerator(const NodeWrapper& other) : MapGenerator(other) {}

		/**
		 * @brief Creates a DepthGenerator node from available @ref node_alternative "production node alternatives".
		 *
		 * <b>Remarks:</b>
		 *
		 * For full details and usage, see @ref create_method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * @brief Gets the depth generator node's latest @ref glos_frame_object "frame object", saving
		 * it in the @ref xn::DepthMetaData object. This @ref glos_frame_object "frame object" is a
		 * snapshot of the generated depth map data and its associated configuration information at a
		 * certain point in time. This saved @ref glos_frame_object "frame object" provides fast
		 * and easy access to the depth generator node's data and configuration information.
		 *
		 * @param [out]	metaData	Structure to be filled with the frame object.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method is central in the use of the DepthGenerator node. Once the DepthGenerator
		 * node is generating data, after each @ref conc_updating_data "'Update Data'" call,
		 * the application calls this method to get the node's frame object, which contains
		 * the data and all its associated properties.
		 */
		inline void GetMetaData(DepthMetaData& metaData) const
		{
			xnGetDepthMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/**
		 * @copybrief xnGetDepthMap
		 * For full details and usage, see @ref xnGetDepthMap
		 */
		inline const XnDepthPixel* GetDepthMap() const
		{
			return xnGetDepthMap(GetHandle());
		}

		/**
		 * @brief Gets the maximum depth value that the DepthGenerator node can generate.
		 * The maximum depth value is a hardware specification of the sensor.
		 *
		 * <b>Remarks:</b>
		 *
		 * A typical maximum depth supports up to 10 meters of depth. Units are in millimeters,
		 * so the maximum depth value for 10 meters of depth would range from 0 to 10,000.
		 */
		inline XnDepthPixel GetDeviceMaxDepth() const
		{
			return xnGetDeviceMaxDepth(GetHandle());
		}

		/**
		 * @brief Gets the dimensions of the field of view (FOV) of the hardware sensor.
		 *
		 * @param [out]	FOV  	A struct to be filled with the field of view of the hardware sensor.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method gets the dimensions of the field of view in radians. Radians are
		 * used for the output form since they are a spherical measurement and the FOV is
		 * defined in terms of the angle of width and height that the sensor sees, e.g.,
		 * 0.524 radians (30 degrees) above and below, and 1.047 radians (60 degrees) left
		 * and right.
		 *
		 * This method is useful for developers building algorithms to convert between real
		 * world and productive coordinate systems. Although OpenNI offers its own functionality
		 * for converting real-world and productive coordinate systems, some developers
		 * may wish to build their own conversion functions for reasons of optimization.
		 */
		inline XnStatus GetFieldOfView(XnFieldOfView& FOV) const
		{
			return xnGetDepthFieldOfView(GetHandle(), &FOV);
		}

		/**
		 * @brief Registers an event handler for the 'Field-Of-View Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToFieldOfViewChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToDepthFieldOfViewChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'Field-Of-View Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromDepthFieldOfViewChange, GetHandle(), hCallback);
		}

		/**
		 * @brief Converts a list of points from projective coordinates to real world coordinates.
		 *
		 * @param [in]		nCount		Number of points to convert.
		 * @param [in]		aProjective	Array of projective point coordinates.
		 * @param [in,out]	aRealWorld	Array to be filled with real-world point coordinates.
		 *
		 * See @ref conc_coord for futher information.
		 */
		inline XnStatus ConvertProjectiveToRealWorld(XnUInt32 nCount, const XnPoint3D aProjective[], XnPoint3D aRealWorld[]) const
		{
			return xnConvertProjectiveToRealWorld(GetHandle(), nCount, aProjective, aRealWorld);
		}

		/**
		 * @brief Converts a list of points from real world coordinates to projective coordinates.
		 *
		 * @param [in]		nCount  	Number of points to convert.
		 * @param [in]		aRealWorld  Array of projective point coordinates.
		 * @param [in,out]	aProjective Array to be filled with real-world point coordinates.
		 *
		 * See @ref conc_coord for futher information.
		 */
		inline XnStatus ConvertRealWorldToProjective(XnUInt32 nCount, const XnPoint3D aRealWorld[], XnPoint3D aProjective[]) const
		{
			return xnConvertRealWorldToProjective(GetHandle(), nCount, aRealWorld, aProjective);
		}

		/**
		 * Gets an @ref UserPositionCapability object for accessing User Position functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_USER_POSITION
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline const UserPositionCapability GetUserPositionCap() const
		{
			return UserPositionCapability(GetHandle());
		}

		/**
		 * Gets an @ref UserPositionCapability object for accessing User Position functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_USER_POSITION
		 * is supported by calling @ref IsCapabilitySupported().
		 */
		inline UserPositionCapability GetUserPositionCap()
		{
			return UserPositionCapability(GetHandle());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Represents a mock depth generator
	 */
	class MockDepthGenerator : public DepthGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline MockDepthGenerator(XnNodeHandle hNode = NULL) : DepthGenerator(hNode) {}
		inline MockDepthGenerator(const NodeWrapper& other) : DepthGenerator(other) {}

		/**
		 * Creates a mock depth generator
		 *
		 * @param [in]	context	The context in which the node should be generated
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);

		/**
		 * Creates a mock depth generator, copying the initial state from another depth
		 * generator.
		 *
		 * @param [in]	other	The depth generator to copy state from
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(DepthGenerator& other, const XnChar* strName = NULL);

		/**
		 * @copybrief xnMockDepthSetData
		 * For full details and usage, see @ref xnMockDepthSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnDepthPixel* pDepthMap)
		{
			return xnMockDepthSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pDepthMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and
		 * timestamp
		 *
		 * @param [in]	depthMD		Object to take data from
		 * @param [in]	nFrameID	Frame ID
		 * @param [in]	nTimestamp	Timestamp
		 */
		inline XnStatus SetData(const DepthMetaData& depthMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, depthMD.DataSize(), depthMD.Data());
		}

		/**
		 * Sets the data of the mock node from a metadata object.
		 *
		 * @param [in]	depthMD		Object to take data from
		 */
		inline XnStatus SetData(const DepthMetaData& depthMD)
		{
			return SetData(depthMD, depthMD.FrameID(), depthMD.Timestamp());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> The ImageGenerator node generates color image maps of various formats.
	 *
	 * <b>Usage:</b> Instantiate the class and create an image generator node
	 *
	 * <b>Data output:</b> Image maps
	 *
	 * <b>Overview:</b>
	 *
	 * The ImageGenerator node generates color image maps of various formats, such as the
	 * RGB24 image format. Call its SetPixelFormat() method to set the image format to be
	 * generated.
	 *
	 * The other important function here is GetMetaData(), which, as in other generator
	 * nodes, gets the image frame object, through which you can access all the properties
	 * and the actual data of the saved frame.
	 */
	class ImageGenerator : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline ImageGenerator(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}
		inline ImageGenerator(const NodeWrapper& other) : MapGenerator(other) {}

		/**
		 * @brief Creates and initializes an ImageGenerator node from available @ref node_alternative "production node alternatives".
		 *
		 * For full details and usage, see @ref create_method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * @brief Gets the image generator node's latest @ref glos_frame_object "frame object", saving
		 * it in the @ref xn::ImageMetaData object. This @ref glos_frame_object "frame object" is a
		 * snapshot of the generated image map data and its associated configuration information at a
		 * certain point in time. This saved @ref glos_frame_object "frame object" provides fast
		 * and easy access to the image generator node's data and configuration information.
		 *
		 * @param [out]	metaData	Structure to be filled with the frame object.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method is central in the use of the ImageGenerator node. Once the ImageGenerator
		 * node is generating data, after each @ref conc_updating_data "'Update Data'" call,
		 * the application calls this method to get the node's frame object, which contains
		 * the data and all its associated properties.
		 */
		inline void GetMetaData(ImageMetaData& metaData) const
		{
			xnGetImageMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/**
		 * @copybrief xnGetRGB24ImageMap
		 * For full details and usage, see @ref xnGetRGB24ImageMap
		 */
		inline const XnRGB24Pixel* GetRGB24ImageMap() const
		{
			return xnGetRGB24ImageMap(GetHandle());
		}

		/**
		 * @copybrief xnGetYUV422ImageMap
		 * For full details and usage, see @ref xnGetYUV422ImageMap
		 */
		inline const XnYUV422DoublePixel* GetYUV422ImageMap() const
		{
			return xnGetYUV422ImageMap(GetHandle());
		}

		/**
		 * @copybrief xnGetGrayscale8ImageMap
		 * For full details and usage, see @ref xnGetGrayscale8ImageMap
		 */
		inline const XnGrayscale8Pixel* GetGrayscale8ImageMap() const
		{
			return xnGetGrayscale8ImageMap(GetHandle());
		}

		/**
		 * @copybrief xnGetGrayscale16ImageMap
		 * For full details and usage, see @ref xnGetGrayscale16ImageMap
		 */
		inline const XnGrayscale16Pixel* GetGrayscale16ImageMap() const
		{
			return xnGetGrayscale16ImageMap(GetHandle());
		}

		/**
		 * @copybrief xnGetImageMap()
		 * For full details, see @ref xnGetImageMap().
		 */
		inline const XnUInt8* GetImageMap() const
		{
			return xnGetImageMap(GetHandle());
		}

		/**
		 * @brief Returns whether the ImageGenerator node supports a specific
		 * pixel color format for the image map.
		 *
		 * @param [in]	Format 	The format to check for.
		 *
		 * <b>Remarks</b>
		 *
		 * Use this method before setting the pixel format for the next frame to be generated.
		 */
		inline XnBool IsPixelFormatSupported(XnPixelFormat Format) const
		{
			return xnIsPixelFormatSupported(GetHandle(), Format);
		}

		/**
		 * @brief Sets the ImageGenerator node's current 'pixel color' format.
		 * This format is used for generating the next frame.
		 *
		 * @param [in]	Format 	The format to set.
		 *
		 * <b>Remarks</b>
		 *
		 * As an example of a pixel color format, the RGB24 image format has one byte for
		 * red, one byte for green, and one byte for blue.
		 *
		 * When reading the generated data you must use the appropriate 'get' function,
		 * e.g., call RGB24Data() to read data generated as the RGB24Data format.
		 *
		 * It is the user responsibility to check first if the pixel format is supported
		 * by calling @ref xn::ImageGenerator::IsPixelFormatSupported().
		 */
		inline XnStatus SetPixelFormat(XnPixelFormat Format)
		{
			return xnSetPixelFormat(GetHandle(), Format);
		}

		/**
		 * @brief Gets the current pixel color format. This is the format set
		 * using @ref xn::ImageGenerator node::SetPixelFormat().
		 *
		 * <b>Remarks</b>
		 *
		 * To get the pixel format of the saved frame - in the @ref glos_frame_object "frame
		 * object" - use the appropriate method of the node's associated @ref xn::ImageMetaData
		 * object.
		 */
		inline XnPixelFormat GetPixelFormat() const
		{
			return xnGetPixelFormat(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'Pixel Format Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToPixelFormatChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToPixelFormatChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters the event handler for the 'Pixel Format Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromPixelFormatChange, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Represents a mock image generator
	 */
	class MockImageGenerator : public ImageGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline MockImageGenerator(XnNodeHandle hNode = NULL) : ImageGenerator(hNode) {}
		inline MockImageGenerator(const NodeWrapper& other) : ImageGenerator(other) {}

		/**
		 * Creates a mock image generator
		 *
		 * @param [in]	context	The context in which the node should be generated
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);

		/**
		 * Creates a mock image generator, copying the initial state from another image
		 * generator.
		 *
		 * @param [in]	other	The image generator to copy state from
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(ImageGenerator& other, const XnChar* strName = NULL);

		/**
		 * @copybrief xnMockImageSetData
		 * For full details and usage, see @ref xnMockImageSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pImageMap)
		{
			return xnMockImageSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pImageMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and
		 * timestamp
		 *
		 * @param [in]	imageMD		Object to take data from
		 * @param [in]	nFrameID	Frame ID
		 * @param [in]	nTimestamp	Timestamp
		 */
		inline XnStatus SetData(const ImageMetaData& imageMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, imageMD.DataSize(), imageMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param [in]	imageMD		Object to take data from
		 */
		inline XnStatus SetData(const ImageMetaData& imageMD)
		{
			return SetData(imageMD, imageMD.FrameID(), imageMD.Timestamp());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> An IRGenerator node is a map generator that outputs infra-red maps.
	 *
	 * <b>Usage:</b> Instantiate and create an IR generator node
	 *
	 * <b>Data output:</b> IR maps
	 */
	class IRGenerator : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline IRGenerator(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}
		inline IRGenerator(const NodeWrapper& other) : MapGenerator(other) {}

		/**
		 * @brief Creates an IRGenerator node from available @ref node_alternative "production node alternatives".
		 *
		 * See @ref create_method for a detailed description
		 * about the operation of this method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * @brief Gets the IR generator node's latest @ref glos_frame_object "frame object", saving
		 * it in the @ref xn::IRMetaData object. This @ref glos_frame_object "frame object" is a
		 * snapshot of the generated IR map data and its associated configuration information at a
		 * certain point in time. This saved @ref glos_frame_object "frame object" provides fast
		 * and easy access to the IR generator node's data and configuration information.
		 *
		 * @param [out]	metaData	Structure to be filled with the frame object.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method is central in the use of the IRGenerator node. Once the IRGenerator
		 * node is generating data, after each @ref conc_updating_data "'Update Data'" call,
		 * the application calls this method to get the node's frame object, which contains
		 * the data and all its associated properties.
		 */
		inline void GetMetaData(IRMetaData& metaData) const
		{
			xnGetIRMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/**
		 * @brief Gets a pointer to the latest IR map.
		 */
		inline const XnIRPixel* GetIRMap() const
		{
			return xnGetIRMap(GetHandle());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Represents a mock IR generator
	 */
	class MockIRGenerator : public IRGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline MockIRGenerator(XnNodeHandle hNode = NULL) : IRGenerator(hNode) {}
		inline MockIRGenerator(const NodeWrapper& other) : IRGenerator(other) {}

		/**
		 * Creates a mock IR generator
		 *
		 * @param [in]	context	The context in which the node should be generated
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);
		/**
		 * Creates a mock IR generator, copying the initial state from another IR generator.
		 *
		 * @param [in]	other	The IR generator to copy state from
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(IRGenerator& other, const XnChar* strName = NULL);

		/**
		 * @copybrief xnMockIRSetData
		 * For full details and usage, see @ref xnMockIRSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnIRPixel* pIRMap)
		{
			return xnMockIRSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pIRMap);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and
		 * timestamp
		 *
		 * @param [in]	irMD		Object to take data from
		 * @param [in]	nFrameID	Frame ID
		 * @param [in]	nTimestamp	Timestamp
		 */
		inline XnStatus SetData(const IRMetaData& irMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, irMD.DataSize(), irMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object.
		 *
		 * @param [in]	irMD	Object to take data from
		 */
		inline XnStatus SetData(const IRMetaData& irMD)
		{
			return SetData(irMD, irMD.FrameID(), irMD.Timestamp());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b>A GesturesGenerator node recognizes certain hand gestures and raise
	 * corresponding events accordingly.
	 *
	 * A gesture is a specific hand movement. The gesture generator node scans the FOV to
	 * detect gestures and generates the gesture data.
	 *
	 * <b>Usage:</b> Instantiate to create a gesture generator node.
	 *
	 * An example of a gesture is a specific hand movement. For example, the developer who builds
	 * a gesture generator node can decide that waving a hand can be a gesture; pushing can be a gesture.
	 * The developer can build an OpenNI gesture generator to support any number of gestures.
	 *
	 * <b>Overview with Sample of a Usage Scenario:</b>
	 *
	 * @note Everything in this introduction is just an @e example of usage; there
	 * is no obligation to use the gesture generator node in the particular way described
	 * here.
	 *
	 * An application might want to know who is the person they should interact with. For example,
	 * if currently there are three people in the room, which hand should be tracked?
	 *
	 * The application can use a specific gesture (such as waving to the camera) as a focus gesture,
	 * meaning the person who performs this gesture will be the one interacting with the application.
	 *
	 * To do that, the application can use a GestureGenerator to detect that specific gesture and then
	 * use the position of that gesture to initialize the HandsGenerator, asking it to track the hand
	 * located at that position. (There is no inherent connection between the GestureGenerator and the
	 * HandsGenerator. Detecting a gesture and using that position to initialize the HandsGenerator is
	 * just an example of how applications can use the two).
	 *
	 * <b>Sample Process: Setting up a Gesture Generator</b>
	 *
	 * <b>1. Creating a Gesture Generator </b>
	 *
	 * Assuming there is a gesture generator module (plug-in) installed on the computer, the application
	 * can create a gesture generator node and configure it to recognize gestures. As for many other
	 * types of generator nodes, use the @ref Create() method of the GestureGenerator class to create a
	 * gesture generator node.
	 *
	 * <b>2. Enumerating generators</b>
	 *
	 * Now, once the application has created a gesture generator node, the application can
	 * find out which gestures are supported by enumerating all its gestures. The gestures
	 * are identified by their names, e.g., "Wave" and "Click".
	 *
	 * The gesture generator node maintains a list of active gestures. An active gesture
	 * is a gesture for which the gesture generator is scanning the FOV in an attempt
	 * to recognize the gesture. Thus, to cause the gesture generator node to start scanning
	 * for a gesture, the application must add it to its list of active gestures.
	 *
	 * In a typical application, the application detects one gesture - the focus gesture -
	 * and can then send its location to the hand generator node. The application typically
	 * will then not want to recognize further gestures and so it must remove the gesture from
	 * the list; further gestures in the FOV may cause confusion. For example, if one particular
	 * human user started to control the TV, that user would not want someone else "taking the control".
	 *
	 * <b>3. Recognizing Gestures </b>
	 *
	 * Every gesture has an identification point (the position where the gesture was identified - perhaps
	 * in retrospect) and an end point (the position in which the hand currently is), although in most cases,
	 * both points will be the same. The application must know the position of the end of the gesture
	 * movement; because that's where the hand really is at the time the application gets the 'Gesture Recognized'
	 * event. The gesture generator sends both positions with the 'Gesture Recognized' event to the application.
	 * The application can then pass the end position to the hand generator for tracking that hand.
	 */
	class GestureGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline GestureGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}
		inline GestureGenerator(const NodeWrapper& other) : Generator(other) {}

		/**
		 * @brief Creates a gesture generator node from available @ref node_alternative "production node alternatives".
		 *
		 * <b>Remarks</b>
		 *
		 * The application then uses the @ref AddGesture() method in order to activate the
		 * gesture.
		 *
		 * Typical gestures are focus gestures. Examples of focus gestures are 'Wave' and
		 * 'Click'.
		 *
		 * See @ref create_method for a detailed description of this method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * @brief Activates the GestureGenerator node to start looking for the named gesture in the FOV.
		 * It does this by adding the gesture's name to the list of gestures that the GestureGenerator node
		 * actively scans for in the FOV. If an area is supplied, the gesture will only be recognized if
		 * it is detected in that area.
		 *
		 * @param [in]	strGesture 	Name of the gesture to look for
		 * @param [in]	pArea		Area in which to look for the gesture (in real-world coordinates)
		 *
		 * <b>Remarks</b>
		 *
		 * This gesture must be one of the built-in gestures of the GestureGenerator node.
		 */
		inline XnStatus AddGesture(const XnChar* strGesture, XnBoundingBox3D* pArea)
		{
			return xnAddGesture(GetHandle(), strGesture, pArea);
		}

		/**
		 * @brief Disables the GestureGenerator from looking for the named gesture in the FOV.
		 * It does this by removing the gesture's name from the list of gestures that the
		 * GestureGenerator node actively scans for in the FOV.
		 *
		 * @param [in]	strGesture 	The name of the gesture to not look for anymore.
		 */
		inline XnStatus RemoveGesture(const XnChar* strGesture)
		{
			return xnRemoveGesture(GetHandle(), strGesture);
		}

		/**
		 * @brief Get the names of all gestures that are currently active in this generator node.
		 *
		 * @param [out]		astrGestures 	Preallocated memory, for the gesture names
		 * @param [in]		nNameLength 	Memory size for each gesture name
		 * @param [in,out]	nGestures 		Inputs the total size of the preallocated memory.
		 * 									Outputs the total number of gestures in the list.
		 */
		inline XnStatus GetAllActiveGestures(XnChar** astrGestures, XnUInt32 nNameLength, XnUInt16& nGestures) const
		{
			return xnGetAllActiveGestures(GetHandle(), astrGestures, nNameLength, &nGestures);
		}

        /**
		 * @copybrief xnGetNumberOfAvailableGestures
         * For full details and usage, see @ref xnGetNumberOfAvailableGestures
         */
        inline XnUInt16 GetNumberOfAvailableGestures() const
        {
            return xnGetNumberOfAvailableGestures(GetHandle());
        }

		/**
		 * @brief Gets the names of all active gestures in this generator node.
		 *
		 * @param [out]		astrGestures 	Preallocated memory, for the gesture names
		 * @param [in]		nNameLength 	Memory size for each gesture name
		 * @param [in,out]	nGestures 		The size of the preallocated memory. Changed to number
		 * 									of gestures
		 */
		inline XnStatus EnumerateAllGestures(XnChar** astrGestures, XnUInt32 nNameLength, XnUInt16& nGestures) const
		{
			return xnEnumerateAllGestures(GetHandle(), astrGestures, nNameLength, &nGestures);
		}

		/**
		 * @brief Returns whether a specific gesture is available in this generator node.
		 * @param [in]	strGesture 		Name of the gesture to check
		 *
		 */
		inline XnBool IsGestureAvailable(const XnChar* strGesture) const
		{
			return xnIsGestureAvailable(GetHandle(), strGesture);
		}

		/**
		 * @copybrief xnIsGestureProgressSupported
		 * For full details and usage, see @ref xnIsGestureProgressSupported
		 */
		inline XnBool IsGestureProgressSupported(const XnChar* strGesture) const
		{
			return xnIsGestureProgressSupported(GetHandle(), strGesture);
		}

		/**
		 * Callback for the recognition of a gesture.
		 * Signals that the GestureGenerator node has recognized the named gesture in the scene.

		 * Example of a callback header to handle the event:
		 @code
		 void XN_CALLBACK_TYPE GestureRecognized (GestureGenerator &generator, const XnChar *strGesture,
												const XnPoint3D *pIDPosition, const XnPoint3D *pEndPosition,
												void *pCookie)
		 @endcode
		 *
		 * @param [in]	generator		Node that raised the event.
		 * @param [in]	strGesture		Name of the gesture that was recognized.
		 * @param [in]	pIDPosition		X-Y-Z location coordinates of the hand point position in which the gesture was identified.
		 * @param [in]	pEndPosition	Position of the hand at the end of the gesture. This is the hand that performed the gesture.
		 * @param [in]	pCookie			User-provided cookie that was given when registering to this event.

		 <b>Remarks</b>

		 This event returns the identification point (the position where the gesture was identified - perhaps in retrospect) and an end point (the position in which the hand currently is), although in most cases, both points will be the same. The end position of the gesture movement is where the hand is located at the time the GestureGenerator recognized a complete gesture and raised the event.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureRecognized)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie);

		/**
		 * Callback for indication that a certain gesture is in progress.
		 *
		 * Signals to the application that a gesture is expected to soon be recognized,
		 * that is, a gesture is in progress.
		 *
		 * Example of a callback header to handle the event: @code
		 void XN_CALLBACK_TYPE GestureProgress (GestureGenerator &generator, const XnChar *strGesture,
												const XnPoint3D *pPosition, XnFloat fProgress,
												void *pCookie)
		 @endcode
		 *
		 * @param [in]	generator	The node that raised the event.
		 * @param [in]	strGesture	The gesture that is on its way to being recognized.
		 * @param [in]	pPosition	The current position of the hand that is performing the gesture.
		 * @param [in]	fProgress	The percentage of the gesture that was already performed.
		 * @param [in]	pCookie		A user-provided cookie that was given when registering to this event.
		 *
		 * Gesture progress varies between different types of gestures. For the 'Click'
		 * gesture, for example, the click gesture's first step is the 'Push' movement (assuming
		 * the click gesture is defined as 'Push' and then 'Pull'). When the hand in the
		 * FOV completes the 'Push' the GestureGenerator node raises this event.
		 *
		 * After the hand in the FOV completes the pull motion, the node sends the 'Gesture
		 * Recognized' event, as described above.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureProgress)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie);

		/**
		 * @brief Registers event handlers for the 'Gesture Recognized' and 'Gesture Progress' events.
		 * Pass NULL for any event the application does not need to receive.
		 *
		 * @param [in]	RecognizedCB  	[Optional] Callback function to be invoked when the 'Gesture Recognized' event is raised.
		 * @param [in]	ProgressCB  	[Optional] Callback function to be invoked when the 'Gesture Progress' event is raised.
		 * @param [in]	pCookie  		User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 		Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters the event handlers for the 'Gesture Recognized' and 'Gesture Progress' events.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterGestureCallbacks(XnCallbackHandle hCallback)
		{
			GestureCookie* pGestureCookie = (GestureCookie*)hCallback;
			xnUnregisterGestureCallbacks(GetHandle(), pGestureCookie->hCallback);
			xnOSFree(pGestureCookie);
		}

		/**
		 * @brief Registers an event handler for the 'Gesture Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToGestureChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToGestureChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters an event handler for the 'Gesture Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromGestureChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromGestureChange, GetHandle(), hCallback);
		}

		/**
		 * Callback for indication that an intermediate stage of a gesture has been completed.
		 *
		 * Example of a callback function to handle the event:
		@code
		 void XN_CALLBACK_TYPE GestureIntermediateStageCompleted(GestureGenerator& generator, const XnChar *strGesture,
																const XnPoint3D *pPosition, void *pCookie)
		@endcode
		 *
		 * @param [in]	generator	The node that raised the event.
		 * @param [in]	strGesture	The gesture that is on its way to being recognized.
		 * @param [in]	pPosition	The current position of the hand that is performing the gesture.
		 * @param [in]	pCookie		A user-provided cookie that was given when registering to this event.
		 *
		 * <b>Remarks</b>
		 *
		 * For different types of gestures there is a different point at which the GestureGenerator
		 * node can recognize that a gesture is in progress. For the 'Click' gesture, for
		 * example, the first step is the 'Push' movement (assuming the click gesture is
		 * defined as 'Push' and then 'Pull'). When the hand in the FOV completes the 'Push'
		 * the GestureGenerator node raises this event.
		 *
		 * The difference between the 'Gesture Intermediate Stage Completed' event and the
		 * 'Gesture Progress' event is that the 'Gesture Intermediate Stage Completed' event
		 * does not refer to how much of the gesture is complete. It deals with breaking
		 * the gesture into smaller intermediate stages, and informing about them.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureIntermediateStageCompleted)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie);

		/**
		 * @brief Registers an event handler for the 'Gesture Intermediate Stage Completed' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters an event handler for the 'Gesture Intermediate Stage Completed' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromGestureIntermediateStageCompleted(XnCallbackHandle hCallback)
		{
			GestureIntermediateStageCompletedCookie* pGestureCookie = (GestureIntermediateStageCompletedCookie*)hCallback;
			xnUnregisterFromGestureIntermediateStageCompleted(GetHandle(), pGestureCookie->hCallback);
			xnOSFree(pGestureCookie);
		}

		/**
		 * Callback for indication that the GestureGenerator node recognizes it is ready
		 * for the next intermediate stage. The next stage is specific to the gesture.
		 *
		 * Example of a callback function to handle the event: @code
		 void XN_CALLBACK_TYPE GestureReadyForNextIntermediateStage(GestureGenerator &generator, const XnChar *strGesture,
		 															const XnPoint3D *pPosition, void *pCookie)
		 @endcode
		 *
		 * <b>Remarks</b>
		 *
		 * The number of intermediate stages that there can be is implementation-dependent.
		 *
		 * @param [in]	generator	The node that raised the event.
		 * @param [in]	strGesture	The gesture that is on its way to being recognized.
		 * @param [in]	pPosition	The current position of the hand that is performing the gesture.
		 * @param [in]	pCookie		A user-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* GestureReadyForNextIntermediateStage)(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie);

		/**
		 * @brief Registers an event handler for the 'Gesture Ready For Next Intermediate Stage' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters an event handler for the 'Gesture Ready For Next Intermediate Stage' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromGestureReadyForNextIntermediateStageCallbacks(XnCallbackHandle hCallback)
		{
			GestureReadyForNextIntermediateStageCookie* pGestureCookie = (GestureReadyForNextIntermediateStageCookie*)hCallback;
			xnUnregisterFromGestureReadyForNextIntermediateStage(GetHandle(), pGestureCookie->hCallback);
			xnOSFree(pGestureCookie);
		}

		/** @cond */
		inline XnStatus XN_API_DEPRECATED("Use GetAllActiveGestures() instead") GetActiveGestures(XnChar*& astrGestures, XnUInt16& nGestures) const
		{
			return xnGetActiveGestures(GetHandle(), &astrGestures, &nGestures);
		}
		inline XnStatus XN_API_DEPRECATED("Use EnumerateAllGestures() instead") EnumerateGestures(XnChar*& astrGestures, XnUInt16& nGestures) const
		{
			return xnEnumerateGestures(GetHandle(), &astrGestures, &nGestures);
		}
		/** @endcond */

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
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> Performs an analysis of the scene, identifying different
	 * objects.
	 *
	 * <b>Data output:</b> A label map and the floor plane.
	 *
	 * <b>Remarks:</b>
	 *
	 * The scene analyzer node analyzes a scene, calculating the separation
	 * of the foreground from the background, identification of figures in the scene, and
	 * detection of the floor plane and its coordinates.
	 *
	 * The SceneAnalyzer node's main output is a label map, in which each pixel
	 * is labeled according to the object it belongs to. So, for example,
	 * each human person gets a separate label, and all pixels belonging to him
	 * are labeled with that label.
	 */
	class SceneAnalyzer : public MapGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline SceneAnalyzer(XnNodeHandle hNode = NULL) : MapGenerator(hNode) {}
		inline SceneAnalyzer(const NodeWrapper& other) : MapGenerator(other) {}

		/**
		 * @brief Creates a SceneAnalyzer node from available @ref node_alternative "production node alternatives".
		 *
		 * <b>Remarks</b>
		 *
		 * See @ref create_method for a detailed description of this method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);


		/**
		 * @brief Gets the scene analyzer node's latest @ref glos_frame_object "frame object", saving
		 * it in the @ref xn::SceneMetaData object. This @ref glos_frame_object "frame object" is a
		 * snapshot of the generated label map data and its associated configuration information at a
		 * certain point in time. This saved @ref glos_frame_object "frame object" provides fast
		 * and easy access to the scene analyzer node's data and configuration information.
		 *
		 * @param [out]	metaData	Structure to be filled with the frame object.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method is central in the use of the SceneAnalyzer node. Once the SceneAnalyzer
		 * node is generating data, after each @ref conc_updating_data "'Update Data'" call,
		 * the application calls this method to get the node's frame object, which contains
		 * the data and all its associated properties.
		 *
		 * The frame scene is a labeled depth map, in which each pixel contains a numeric
		 * label indicating whether it represents a user, or it is part of the background.
		 */
		inline void GetMetaData(SceneMetaData& metaData) const
		{
			xnGetSceneMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/**
		 * @copybrief xnGetLabelMap
		 * For full details and usage, see @ref xnGetLabelMap
		 */
		inline const XnLabel* GetLabelMap() const
		{
			return xnGetLabelMap(GetHandle());
		}

		/**
		 * @brief Gets a description of the floor, if it was found.
		 *
		 * @param [out]	Plane 	A description of the floor
		 */
		inline XnStatus GetFloor(XnPlane3D& Plane) const
		{
			return xnGetFloor(GetHandle(), &Plane);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Allows access to <b>Hand touching FOV edge</b> capability functions.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref HandsGenerator::GetHandTouchingFOVEdgeCap()
	 * to obtain an instance.
	 */
	class HandTouchingFOVEdgeCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline HandTouchingFOVEdgeCapability(XnNodeHandle hNode) : Capability(hNode) {}
		HandTouchingFOVEdgeCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * Callback for the hand getting close to the edge of the FOV
		 *
		 * @param [in]	touchingfov		the hand touching fov edge capability that raised this event.
		 * @param [in]	user			ID of the hand that disappeared.
		 * @param [in]	pPosition		Position of the hand.
		 * @param [in]	fTime			Timestamp, in seconds.
		 * @param [in]	eDir			Direction of the FOV touched
		 * @param [in]	pCookie			User-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* HandTouchingFOVEdge)(HandTouchingFOVEdgeCapability& touchingfov, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDir, void* pCookie);

		/**
		 * @brief Registers an event handler for the 'Hand Touching FOV Edge' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters an event handler for the 'Hand Touching FOV Edge' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
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
	 * @ingroup cppref_prd_func
	 * The HandsGenerator node generates tracking data about a single hand or multiple hands
	 * with persistent IDs.
	 *
	 * The user interface (UI) in natural interaction applications is specifically implemented
	 * and performed through hand movements.
	 *
	 * OpenNI's user interface (UI) ability is specifically through hand movements. Thus
	 * it is the HandsGenerator class that provides the main UI ability.
	 *
	 * Using events, the HandsGenerator object notifies the application of various occurrences: a
	 * 'Hand Create' event is sent when an expected hand has been positively identified or when a
	 * new hand appears; a 'Hand Update' event is sent when a tracked hand appears again in a new
	 * frame; a 'Hand Destroy' event is sent when a tracked hand disappears.
	 *
	 * <b>NOTE: </b> The HandsGenerator does not provide the mechanism for initially locating
	 * the hand. Initial location of the hand can be performed by the Gestures class (see
	 * further below). This is a typical scenario but not the only way to locate the hand.
	 *
	 * The following figure illustrates a typical scenario for using the HandsGenerator
	 * class. After hand recognition the HandsGenerator class continues tracking the hand
	 * and sending HandUpdate events until the application sends calls HandsGenerator's
	 * StopTracking method.
	 *
	 * <b>1. Creating a HandsGenerator</b>
	 *
	 * As for many other types of generator nodes, HandsGenerator provides a Create method
	 * to create a hands generator node.
	 *
	 * Assuming you have hands generator module (plug-in) installed on your computer, you
	 * can create a hands generator node. Then you can start tracking for an expected hand.
	 *
	 * @note Creating a HandsGenerator node has nothing to do with the 'Hand Create'
	 * event that OpenNI sends to the app on starting to successfully track the hand in
	 * the FOV.
	 *
	 * <b>2. Starting Tracking</b>
	 *
	 * When starting to track a hand, the application already expects that a person's hand
	 * is already present in the field of view and is at a certain coordinate (x,y) position.
	 *
	 * The source for this information, i.e., the presence of a hand object and its position,
	 * can be obtained, for example, from a gesture generator node.
	 * The application passes this location to the @ref StartTracking()
	 * method of the HandsGenerator object, which starts tracking for
	 * the expected hand. When - and if - the StartTracking() method finds the hand at the
	 * expected position it sends a 'Hand Create' event to the application.
	 *
	 * <b>3. Tracking</b>
	 *
	 * While tracking a hand, 'Hand Update' events are sent to the application notifying that the hand
	 * has moved to such and such a position. A 'Hand Update' event is sent each time a new
	 * frame processed by the input sensors shows the hand is still present. A 'Hand Update'
	 * event is sent even if the hand is still in the same position as it was in the previous
	 * frame.
	 *
	 * If the tracked hand disappears (e.g., when the person puts his hand behind his back)
	 * a 'Hand Destroy' event is sent to the application to notify that the hand has disappeared.
	 *
	 * The 'Hand Create' event is also sent if an additional hand is found internally.
	 *
	 * The tracking mechanism uses an algorithm that uses the depth to obtain points and
	 * track. This algorithm can be somewhat susceptible to jitter of the hand, so HandsGenerator
	 * provides a smoothing capability. Call the SetSmoothing() method to apply a smoothing
	 * factor of none (0) to maximum (1). The default is 0. Recommended is 0.1 when using
	 * the PrimeSense algorithm for hand generation.
	 *
	 * <b>4. Stopping Tracking</b>
	 *
	 * When the hand being tracked is no longer identified, the HandsGenerator object sends
	 * the 'Hand Destroy' event.
	 *
	 * The application can also intentionally stop tracking a specific hand, e.g., when
	 * the hand is no longer of interest. The application does this by calling the @ref StopTracking()
	 * method. When the node stops tracking the hand it sends a 'Hand Destroy' event to the application.
	 *
	 * The application can also stop the tracking of all hands at once by calling the @ref StopTrackingAll()
	 * method. In response, the node stops tracking all hands and for each hand it sends a
	 * 'Hand Destroy' event to the application.
	 * Even when there are no tracked hands, the HandsGenerator object remains functional
	 * for the next tracking request.
	 *
	 * <h2>Events:</h2>
	 *
	 * @image html ClassHandsGenerator_EventSeq-sm.png "Event Sequence for HandsGenerator node"
	 */
	class HandsGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline HandsGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}
		inline HandsGenerator(const NodeWrapper& other) : Generator(other) {}

		/**
		 * @brief Creates a HandsGenerator node from available @ref node_alternative "production node alternatives".
		 *
		 * <b>Remarks:</b>
		 *
		 * See @ref create_method for a detailed description of this method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * Callback for the creation of a new hand.
		 *
		 * Signals that the hands generator has recognized and started tracking a new hand in response
		 * to the application calling the StartTracking() method. This event returns the
		 * ID of the new hand, the time of its recognition, and its position on recognition
		 * in the current frame.
		 *
		 * @param [in]	generator	Generator that raised this event.
		 * @param [in]	user		ID of the new hand.
		 * @param [in]	pPosition	Position at which the hand was created.
		 * @param [in]	fTime		Timestamp, in seconds.
		 * @param [in]	pCookie		User-provided cookie that was given when registering to
		 * this event.
		 */
		typedef void (XN_CALLBACK_TYPE* HandCreate)(HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);

		/**
		 * Signals that an already-existing hand has moved to a new position.
		 *
		 * Signals that a currently tracked hand was recognized at a specific position in
		 * the new frame. OpenNI continues to send this event at each further frame that
		 * the hand is still present. This event returns the ID of the hand, which is the
		 * same ID returned by the HandCreate event, the hand's new position, and the time
		 * of the update.
		 *
		 * @param [in]	generator	the generator that raised this event.
		 * @param [in]	user		The id of the hand that moved.
		 * @param [in]	pPosition	The new position of the relevant hand.
		 * @param [in]	fTime		Timestamp, in seconds.
		 * @param [in]	pCookie		A user-provided cookie that was given when registering to
		 * 							this event.
		 */
		typedef void (XN_CALLBACK_TYPE* HandUpdate)(HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);

		/**
		 * Callback for the disappearance of a hand.
		 *
		 * Signals that an existing hand has disappeared from the frame for any reason.
		 * This event returns the user ID &ndash; still the same user ID as before &ndash;
		 * and the time that it disappeared.
		 *
		 * @param [in]	generator	the generator that raised this event.
		 * @param [in]	user		The id of the hand that disappeared.
		 * @param [in]	fTime		Timestamp, in seconds.
		 * @param [in]	pCookie		A user-provided cookie that was given when registering to
		 * 							this event.
		 */
		typedef void (XN_CALLBACK_TYPE* HandDestroy)(HandsGenerator& generator, XnUserID user, XnFloat fTime, void* pCookie);

		/**
		 * @brief Registers event handlers for the 'Hand Create', 'Hand Update' and 'Hand Destroy' events.
		 * Pass NULL for any event you don't want your application to receive.
		 *
		 * @param [in]	CreateCB  	[Optional] Callback function to be invoked when the 'Hand Create' event is raised.
		 * @param [in]	UpdateCB  	[Optional] Callback function to be invoked when the 'Hand Update' event is raised.
		 * @param [in]	DestroyCB  	[Optional] Callback function to be invoked when the 'Hand Destroy' event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters event handlers for the 'Hand Create', 'Hand Update' and 'Hand Destroy' events.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterHandCallbacks(XnCallbackHandle hCallback)
		{
			HandCookie* pHandCookie = (HandCookie*)hCallback;
			xnUnregisterHandCallbacks(GetHandle(), pHandCookie->hCallback);
			xnOSFree(pHandCookie);
		}

		/**
		 * @brief Stops tracking an existing hand that you are no longer interested in.
		 *
		 * @param [in]	user 	Hand to stop tracking. This parameter is the ID of the hand.
		 *
		 * <b>Remarks</b>
		 *
		 * Typically, the application will stop tracking an existing hand that it is no
		 * longer interested in.
		 *
		 * Calling this method causes the 'Hand Destroy' event to be raised when OpenNI destroys
		 * the hand.
		 */
		inline XnStatus StopTracking(XnUserID user)
		{
			return xnStopTracking(GetHandle(), user);
		}

		/**
		 * @brief Stops tracking all existing hands.
		 *
		 * <b>Remarks</b>
		 *
		 * Calling this method causes the 'Hand Destroy' event to be raised when OpenNI destroys
		 * the hand.
		 */
		inline XnStatus StopTrackingAll()
		{
			return xnStopTrackingAll(GetHandle());
		}

		/**
		 * @brief Starts tracking at the specific position where the application expects a hand.
		 *
		 * @param [in]	ptPosition 	The position at which there is a new hand
		 *
		 * <b>Remarks</b>
		 *
		 * Use the <code>ptPosition</code> parameter to tell the method what is the expected
		 * location (in X-Y-Z coordinates) of the hand.
		 *
		 * This expected location can be obtained from a 'Gesture Recognized' event, but
		 * it is not the only way of getting the hand's expected location.
		 *
		 * Calling this method causes the HandCreate event to be raised when OpenNI recognizes
		 * the hand.
		 */
		inline XnStatus StartTracking(const XnPoint3D& ptPosition)
		{
			return xnStartTracking(GetHandle(), &ptPosition);
		}

		/**
		 * @brief Changes the smoothing factor.
		 *
		 * @param [in]	fSmoothingFactor 	Smoothing factor, in the range 0..1. 0 Means no
		 * 									smoothing, 1 means infinite smoothing.
		 *									Inside the range is generator dependent.
		 * <br>
		 * Recommended:  0.1
		 *
		 * <b>Remarks</b>
		 *
		 * The hand is identified from its physical depth. Since the human hand is not absolutely
		 * steady, the hand point generated may be too jittery to be used for controlling
		 * the user interface, e.g., pressing buttons. This method provides an algorithm
		 * to smooth the jitter. You provide the desired smoothing factor as a parameter.
		 */
		inline XnStatus SetSmoothing(XnFloat fSmoothingFactor)
		{
			return xnSetTrackingSmoothing(GetHandle(), fSmoothingFactor);
		}

		/**
		 * Gets an @ref HandTouchingFOVEdgeCapability object for accessing hand touching
		 * FOV edge callbacks functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_HAND_TOUCHING_FOV_EDGE
		 * is supported
		 * by calling @ref IsCapabilitySupported().
		 */
		inline const HandTouchingFOVEdgeCapability GetHandTouchingFOVEdgeCap() const
		{
			return HandTouchingFOVEdgeCapability(GetHandle());
		}

		/**
		 * Gets an @ref HandTouchingFOVEdgeCapability object for accessing hand touching
		 * FOV edge callbacks functionality.
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_HAND_TOUCHING_FOV_EDGE
		 * is supported
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
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> Provides the 'Skeleton' functionality for a @ref xn::UserGenerator
	 * node. This capability lets the node generate a skeleton representation for each human
	 * user generated by the node.
	 *
	 * @note See @ref meanings_of_user "Meanings of 'User'".
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref UserGenerator::GetSkeletonCap()
	 * to obtain an instance.
	 *
	 * <b>Data output:</b> Skeleton.
	 *
	 * The OpenNI definition of <i>joints</i> refers to a predefined set of points on the
	 * human body, as defined by the @ref xn::XnSkeletonJoint enum type. Examples of these
	 * joints are @ref XN_SKEL_HEAD, @ref XN_SKEL_TORSO, @ref XN_SKEL_LEFT_ELBOW,
	 * @ref XN_SKEL_LEFT_WRIST and @ref XN_SKEL_LEFT_HAND. Note that some of these <i>joints</i>
	 * are actual joints, in the conventional sense as termed by the English language, for
	 * example, @ref XN_SKEL_LEFT_ELBOW and @ref XN_SKEL_LEFT_WRIST; and other joints are in
	 * fact <i>limbs</i>, for example, @ref XN_SKEL_HEAD and @ref XN_SKEL_LEFT_HAND.
	 * However, OpenNI defines all these as <i>joints</i>, each joint with a single position
	 * coordinate. Each detected joint is defined by its <i>position</i>, which is held
	 * as a struct of type @ref xn::XnSkeletonJointPosition, comprising a 3D position coordinate
	 * and confidence. The 3D position coordinate defines a point somewhere in the center
	 * of the 'joint', for example, the position of the torso will be at the center of the
	 * torso, i.e., somewhere in the stomach area.
	 *
	 * To graphically draw a complete skeleton, therefore, each joint must be connected
	 * by a line to the adjacent point according to the form of the human body.
	 *
	 * An application typically uses this skeleton representation to track human motion,
	 * or to graphically display all or part of the user as an avatar on screen, and optionally
	 * graphically enhance it to make it look more like a full humanoid figure.
	 *
	 * @section skeleton_calib Calibration
	 *
	 * Tracking of a human user always starts with a process named 'Calibration'. In this
	 * process the algorithm performs some implementation-specific logic to enable it
	 * to track that user. there is no standard for what calibration is exactly and
	 * each implementation may use whatever it needs to produce and calibrate a skeleton.
	 * However, a typical way of calibrating the skeleton would be to measure distances
	 * between key joints for a particular user and record the data as the calibration data
	 * for that particular user.
	 *
	 * Some implementations of the user generator node require a human user to pose a specific
	 * posture for the calibration process to succeed. The application can check if current implementation
	 * requires such a pose by calling @ref NeedPoseForCalibration() method. If the answer is TRUE,
	 * the application should call @ref GetCalibrationPose() to find out the name of the pose a human user should
	 * take in order to calibrate the algorithm. The application can use the @ref xn::PoseDetectionCapability
	 * to find out if a human user is assuming a specific position, and once he does, calibration can start.
	 *
	 * The application starts the calibration process by calling @ref RequestCalibration(). There
	 * are several events raised during the calibration process, but the most important one is
	 * 'Calibration Complete'. Once calibration is done, the application may call @ref StartTracking()
	 * to make the algorithm start tracking the skeleton of that human user.
	 */
	class SkeletonCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline SkeletonCapability(XnNodeHandle hNode) : Capability(hNode) {}
		SkeletonCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * @brief Returns whether a specific skeleton joint is supported by the SkeletonCapability object.
		 *
		 * @param [in]	eJoint Joint to check.
		 */
		inline XnBool IsJointAvailable(XnSkeletonJoint eJoint) const
		{
			return xnIsJointAvailable(GetHandle(), eJoint);
		}

		/**
		 * @brief Returns whether a specific skeleton profile is supported by the SkeletonCapability object.
		 *
		 * @param [in]	eProfile Profile to check.
		 */
		inline XnBool IsProfileAvailable(XnSkeletonProfile eProfile) const
		{
			return xnIsProfileAvailable(GetHandle(), eProfile);
		}

		/**
		 * @brief Sets the skeleton profile. The skeleton profile specifies which joints are to
		 * be active, and which to be inactive. The @ref UserGenerator node generates output data
		 * for the active joints only. This profile applies to all skeletons that the @ref UserGenerator node generates.
		 *
		 * @param [in]	eProfile Specifies the profile to set.
		 *
		 * <b>Remarks</b>
		 *
		 * The profile (@ref xn::XnSkeletonProfile) enables the application to choose between
		 * generating all joints, just the upper or lower torso, or just the head and hands
		 * only.
		 *
		 * Since this method provides the flexibility to request only the data that the
		 * application requires, this can help to significantly improve response time: setting
		 * a smaller skeleton profile reduces the processing load on the computer, which
		 * can cause a significant increase in avatar responsiveness.
		 *
		 * Use the @ref SetJointActive() method for selecting the profile down to a finer
		 * resolution than possible with this method, e.g., to be able to select a specific
		 * hand or foot.
		 *
		 * OpenNI does not standardize the initial profile set. If no profile is set, each
		 * implementation may have its own default profile. For example, PrimeSense implementation's
		 * default profile is NONE. This means if no profile is set, no joint will be available.
		 *
		 * Setting a profile changes all the joint configuration to that profile. This means
		 * you can set a profile, then refine specific joints (adding or removing). You
		 * can't do it the other way around.
		 */
		inline XnStatus SetSkeletonProfile(XnSkeletonProfile eProfile)
		{
			return xnSetSkeletonProfile(GetHandle(), eProfile);
		}

		/**
		 * @brief Changes the state of a specific skeleton joint to be active or inactive.
		 * The @ref xn::UserGenerator node generates output data for the active joints only.
		 * This state setting applies to all skeletons that the @ref xn::UserGenerator node generates.
		 *
		 * @param [in]	eJoint Specifies the joint.
		 * @param [in]	bState New activation state (true/false).
		 *
		 * <b>Remarks</b>
		 *
		 * This method lets the application set the skeleton profile down to a finer resolution
		 * than is possible with the @ref SetSkeletonProfile() method.
		 *
		 * The joint selector parameter (@ref xn::XnSkeletonJoint) lets the application
		 * choose specific parts of a skeleton, e.g., a specific hand or foot.
		 *
		 * Since this method provides the flexibility to request only the data that the
		 * application requires, this can help to significantly improve response time. Setting
		 * a smaller skeleton profile can reduce the processing load on the computer, which
		 * can cause a significant increase in avatar responsiveness (implementation-dependent).
		 * Some implementations may produce all the data they can, and only filter it at
		 * the end according to the configuration.
		 */
		inline XnStatus SetJointActive(XnSkeletonJoint eJoint, XnBool bState)
		{
			return xnSetJointActive(GetHandle(), eJoint, bState);
		}

		/**
		 * @brief Checks if a specific joint is tracked or not.
		 *
		 * @param [in]	eJoint Specifies the joint.
		 */
		inline XnBool IsJointActive(XnSkeletonJoint eJoint) const
		{
			return xnIsJointActive(GetHandle(), eJoint);
		}

		/**
		 * @brief Registers an event handler for the 'Joint Configuration Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToJointConfigurationChange(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToJointConfigurationChange, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters an event handler for the 'Joint Configuration Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromJointConfigurationChange(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromJointConfigurationChange, GetHandle(), hCallback);
		}

		/**
		 * @copybrief xnEnumerateActiveJoints
		 * For full details and usage, see @ref xnEnumerateActiveJoints
		 */
		inline XnStatus EnumerateActiveJoints(XnSkeletonJoint* pJoints, XnUInt16& nJoints) const
		{
			return xnEnumerateActiveJoints(GetHandle(), pJoints, &nJoints);
		}

		/**
		 * @brief Gets all available information about a specific joint in the skeleton.
		 *
		 * @param [in]	user 	ID of the user.
		 * @param [in]	eJoint 	Joint specifier.
		 * @param [out]	Joint 	Transformation structure for the joint. Includes position
		 * 						and orientation.
		 */
		inline XnStatus GetSkeletonJoint(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation& Joint) const
		{
			return xnGetSkeletonJoint(GetHandle(), user, eJoint, &Joint);
		}

		/**
		 * @brief Gets the position of one of the skeleton joints in the most recently generated user data.
		 *
		 * @param [in]	user 	ID of the user.
		 * @param [in]	eJoint 	Joint specifier.
		 * @param [out]	Joint 	Latest position of the joint.
		 *
		 * <b>Remarks</b>
		 *
		 * The joint position is returned as a @ref XnSkeletonJointPosition structure, comprising
		 * a real-world coordinate together with a confidence rating.
		 *
		 * Confidence can be in the range of 0..1.
		 *
		 * 0 means 'I'm guessing here'.<br>
		 * 1 means 'This is where it is'. <br>
		 *
		 * The API is ready for when an implementation decides it is 'fairly certain'. In
		 * a typical implementation, 0.5 is used when there is an educated guess, or have
		 * run some heuristics to determine the position.
		 */
		inline XnStatus GetSkeletonJointPosition(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition& Joint) const
		{
			return xnGetSkeletonJointPosition(GetHandle(), user, eJoint, &Joint);
		}

		/**
		 * @brief Gets the orientation of a specific skeleton joint.
		 *
		 * @param [in]	user 	ID of the user.
		 * @param [in]	eJoint 	Joint specifier.
		 * @param [out]	Joint	Joint's orientation.
		 */
		inline XnStatus GetSkeletonJointOrientation(XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation& Joint) const
		{
			return xnGetSkeletonJointOrientation(GetHandle(), user, eJoint, &Joint);
		}

		/**
		 * @brief Returns whether a user is currently being tracked.
		 *
		 * @param [in]	user ID of the user.
		 *
		 * <b>Remarks</b>
		 *
		 * This is a utility method. It is an alternative approach to saving the states
		 * from the events that notify stages in calibration and tracking.
		 */
		inline XnBool IsTracking(XnUserID user) const
		{
			return xnIsSkeletonTracking(GetHandle(), user);
		}

		/**
		 * @brief Returns whether a user has been calibrated. see @ref skeleton_calib.
		 *
		 * @param [in]	user ID of the user.
		 *
		 * <b>Remarks</b>
		 *
		 * This is a utility method. It is an alternative approach to saving the states
		 * from the events that notify stages in calibration.
		 */
		inline XnBool IsCalibrated(XnUserID user) const
		{
			return xnIsSkeletonCalibrated(GetHandle(), user);
		}

		/**
		 * @brief Returns whether a user is being calibrated right now. see @ref skeleton_calib.
		 *
		 * @param [in]	user ID of the user.
		 *
		 * <b>Remarks</b>
		 *
		 * This is a utility method. It is an alternative approach to saving the states
		 * from the events that notify stages in calibration.
		 */
		inline XnBool IsCalibrating(XnUserID user) const
		{
			return xnIsSkeletonCalibrating(GetHandle(), user);
		}

		/**
		 * @brief Starts the calibration process to calibrate a user.
		 *
		 * @param [in]	user 	ID of the user.
		 * @param [in]	bForce 	Disregard previous calibration to force a further calibration.<br>
		 * Values:<br>
		 * True:  Disregards any previous calibration and forces a new calibration.<br>
		 * False: Starts the calibration process only if no calibration data is available
		 * for the skeleton.<br>
		 * This means that if the user was previously calibrated (which actually means measuring
		 * the length of its limbs, and estimating its torso, to be used in its skeletal
		 * representation), and another calibration was requested, to either make do with
		 * what we have (false), or forget the previous measurements and perform calibration
		 * again.
		 *
		 * <b>Remarks</b>
		 *
		 * This method is typically called from inside an event handler for the 'Pose Detected'
		 * event, signaling that a pose was detected in the FOV. Once a valid pose has been
		 * detected the application will want to start calibrating the user.
		 *
		 * It can also be called outside of that, in implementations that don't require
		 * a pose.
		 */
		inline XnStatus RequestCalibration(XnUserID user, XnBool bForce)
		{
			return xnRequestSkeletonCalibration(GetHandle(), user, bForce);
		}

		/**
		 * @brief Aborts an existing calibration process currently being executed for a specified user.
		 *
		 * @param [in]	user ID of the user.
		 *
		 * <b>Remarks</b>
		 *
		 * Raises the 'Calibration Complete' event.
		 */
		inline XnStatus AbortCalibration(XnUserID user)
		{
			return xnAbortSkeletonCalibration(GetHandle(), user);
		}

		/**
		 * @brief Saves the skeleton's current calibration data to a file.
		 *
		 * @param [in]	user 		ID of the user.
		 * @param [in]	strFileName Name of the file for saving the calibration data.
		 *
		 * <b>Remarks</b>
		 *
		 * The application is the one that should match the user, however it sees fit. For
		 * example, if a calibration was saved as C:\\john_smith.bin, and then later another
		 * application recognizes John Smith (let's say, through face recognition), the
		 * calibration data can be loaded, instead of performing the entire calibration
		 * process.
		 *
		 * This is a very useful tool for developers. They can save their own calibration,
		 * and test their application without calibrating each time (going into pose, spend
		 * time on calibration).
		 */
		inline XnStatus SaveCalibrationDataToFile(XnUserID user, const XnChar* strFileName)
		{
			return xnSaveSkeletonCalibrationDataToFile(GetHandle(), user, strFileName);
		}

		/**
		 * @brief Loads skeleton calibration data from a file to a skeleton.
		 *
		 * @param [in]	user 		ID of the user.
		 * @param [in]	strFileName File to load the calibration data from.
		 */
		inline XnStatus LoadCalibrationDataFromFile(XnUserID user, const XnChar* strFileName)
		{
			return xnLoadSkeletonCalibrationDataFromFile(GetHandle(), user, strFileName);
		}

		/**
		 * @brief Saves the calibration data of a specified user. The calibration data includes the lengths of the human user's limbs.
		 *
		 * @param [in]	user 	ID of the user.
		 * @param [in]	nSlot 	Slot to use for saving the calibration data
		 *
		 * <b>Remarks</b>
		 *
		 * The slots mechanism allows saving the calibration to memory instead of to files.
		 * When used, a calibration is saved to a slot, and may be reloaded from it.
		 */
		inline XnStatus SaveCalibrationData(XnUserID user, XnUInt32 nSlot)
		{
			return xnSaveSkeletonCalibrationData(GetHandle(), user, nSlot);
		}

		/**
		 * @brief Loads calibration data of a specified user. The calibration data includes the lengths of the human user's limbs.
		 *
		 * @param [in]	user 	ID of the user.
		 * @param [in]	nSlot 	Slot to use for saving the calibration data
		 */
		inline XnStatus LoadCalibrationData(XnUserID user, XnUInt32 nSlot)
		{
			return xnLoadSkeletonCalibrationData(GetHandle(), user, nSlot);
		}

		/**
		 * @brief Clears a specified slot of any saved calibration data.
		 *
		 * @param [in]	nSlot 	Slot specifier.
		 */
		inline XnStatus ClearCalibrationData(XnUInt32 nSlot)
		{
			return xnClearSkeletonCalibrationData(GetHandle(), nSlot);
		}

		/**
		 * @brief Returns whether a specific slot already holds calibration data.
		 *
		 * @param [in]	nSlot 	Slot specifier.
		 */
		inline XnBool IsCalibrationData(XnUInt32 nSlot) const
		{
			return xnIsSkeletonCalibrationData(GetHandle(), nSlot);
		}

		/**
		 * @brief Starts tracking a skeleton.
		 *
		 * @param [in]	user 	ID of the user.
		 *
		 * <b>Remarks</b>
		 *
		 * This method is used for tracking the motion of the skeleton, representing a human
		 * user body, within a real-life (3D) scene for analysis, interpretation, and use
		 * by the application.
		 *
		 * While the skeleton is being tracked, the application can call the @ref GetSkeletonJointPosition()
		 * method to get data about one of the joint positions.
		 */
		inline XnStatus StartTracking(XnUserID user)
		{
			return xnStartSkeletonTracking(GetHandle(), user);
		}

		/**
		 * @brief Stops tracking a skeleton.
		 * @param [in]	user 	ID of the user.
		 */
		inline XnStatus StopTracking(XnUserID user)
		{
			return xnStopSkeletonTracking(GetHandle(), user);
		}

		/**
		 * @brief Discards a skeleton's calibration.
		 * @param [in]	user 	ID of the user.
		 *
		 * <b>Remarks</b>
		 *
		 * Utility method for resetting a user to its initial state. For use in development.
		 */
		inline XnStatus Reset(XnUserID user)
		{
			return xnResetSkeleton(GetHandle(), user);
		}

		/**
		 * @brief Returns whether a specific pose is required for
		 * calibration. This setting is applicable to all users.
		 *
		 * <b>Remarks</b>
		 *
		 * This is a setting made by the developer of the SkeletonCapability plug-in.
		 */
		inline XnBool NeedPoseForCalibration() const
		{
			return xnNeedPoseForSkeletonCalibration(GetHandle());
		}

		/**
		 * @brief Gets the name of the pose that is required for calibration. The
		 * pose and its name reside in the plug-in module that provides the
		 * SkeletonCapability. The pose's name and details are defined by the
		 * developer of the module.
		 *
		 * @param [out]	strPose 	Name of the pose required for calibration
		 * <b>Remarks</b>
		 *
		 * This method is to be used only if the @ref NeedPoseForCalibration() method returns
		 * TRUE, and only then will this method returns a name.
		 */
		inline XnStatus GetCalibrationPose(XnChar* strPose) const
		{
			return xnGetSkeletonCalibrationPose(GetHandle(), strPose);
		}

		/**
		 * @brief Sets the smoothing factor for all users generated by this @ref xn::UserGenerator node.
		 * @param [in]	fSmoothingFactor 	Smoothing factor<br>
		 * Range: 0 to 1<br>
		 *
		 * <b>Remarks</b>
		 *
		 * The human body is identified from its physical depth. Since the human body is
		 * not absolutely steady, the human body being calibrated may be too jittery to
		 * easily obtain its position. This method provides an algorithm to smooth the jitter.
		 * You provide the desired smoothing factor as a parameter.
		 */
		inline XnStatus SetSmoothing(XnFloat fSmoothingFactor)
		{
			return xnSetSkeletonSmoothing(GetHandle(), fSmoothingFactor);
		}

		/**
		 * Signals that a specific user's SkeletonCapability object is now starting the
		 * calibration process.
		 *
		 * Example of a callback function to handle the 'Calibration Start' event:
		 *
		 @code
		 OnCalibrationStart(xn::SkeletonCapability& skeleton, XnUserID user, void* pCookie)
		 @endcode
		 *
		 * @param [in]	skeleton	SkeletonCapability object that raised the event.
		 * @param [in]	user		The id of the user that's being calibrated.
		 * @param [in]	pCookie		A user-provided cookie that was given when registering to
		 * 							this event.
		 *
		 * <b>Remarks</b>
		 *
		 * The application itself initiates calibration by calling the @ref RequestCalibration() method.
		 * When a SkeletonCapability object actually starts to perform the calibration,
		 * it raises this event.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationStart)(SkeletonCapability& skeleton, XnUserID user, void* pCookie);

		/**
		 * Signals that a specific user's SkeletonCapability object has now completed the
		 * calibration process.
		 *
		 * Example of a callback function to handle the 'Calibration Complete' event:
		 @code
		 OnCalibrationEnd(xn::SkeletonCapability &skeleton, XnUserID user,
						XnCalibrationStatus	calibrationError, void *pCookie)
		 @endcode
		 *
		 * <b>Parameters: </b>
		 *
		 * hNode	[in]	Node that raised the event.
		 * user		[in]	ID of the user for which calibration was attempted.
		 * bSuccess	[in]	Indication of whether or not the calibration attempt succeeded.
		 * pCookie	[in]	User-provided cookie that was given when registering to this event.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationEnd)(SkeletonCapability& skeleton, XnUserID user, XnBool bSuccess, void* pCookie);

		/**
		 * @brief Registers an event handler for the 'Calibration Start' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * See @ref skeleton_calib for information about calibration.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters a handler from the 'Calibration Start' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * <b>Remarks</b>
		 *
		 * Due to historical reasons, this function has an XnStatus return value. In practice,
		 * it will always succeed. The user can safely ignore the return value.
		 */
		inline XnStatus UnregisterFromCalibrationStart(XnCallbackHandle hCallback)
		{
			CalibrationStartCookie* pCalibrationCookie = (CalibrationStartCookie*)hCallback;
			xnUnregisterFromCalibrationStart(GetHandle(), pCalibrationCookie->hCallback);
			xnOSFree(pCalibrationCookie);
			return XN_STATUS_OK;
		}

		/**
		 * Signals that a specific user's skeleton is calibrating, and its current status.
		 *
		 * Example of a callback function to handle the 'Calibration Progress' event:
		 @code
		 OnCalibrationProgress(xn::SkeletonCapability &skeleton, XnUserID user,
							XnCalibrationStatus calibrationError, void *pCookie)
		 @endcode
		 *
		 * @param [in]	skeleton 			SkeletonCapability object that raised the event.
		 * @param [in]	user 				ID of the user holding SkeletonCapability object.
		 * @param [in]	calibrationError 	Status of the calibration. Values are defined by
		 * 									XnCalibrationStatus.
		 * @param [in]	pCookie 			User's cookie, to be delivered to the callback.
		 *
		 * <b>Remarks</b>
		 *
		 * After the 'Calibration Start' event, the 'Calibration in Progress' event is raised
		 * every frame until 'Calibration Complete' is raised. It informs the application
		 * as to the current state of the calibration process.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationInProgress)(SkeletonCapability& skeleton, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie);

		/**
		 * @brief Registers an event handler for the 'Calibration In Progress' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters an event handler for the 'Calibration In Progress' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromCalibrationInProgress(XnCallbackHandle hCallback)
		{
			CalibrationInProgressCookie* pSkeletonCookie = (CalibrationInProgressCookie*)hCallback;
			xnUnregisterFromCalibrationInProgress(GetHandle(), pSkeletonCookie->hCallback);
			xnOSFree(pSkeletonCookie);
		}

		/**
		 * @brief Signals that a specific user's skeleton has now completed the calibration
		 * process, and provides a result status.
		 *
		 * Following is an example of a callback header to handle the 'Calibration Complete'
		 * event:
		 @code
		 OnCalibrationComplete(xn::SkeletonCapability &skeleton, XnUserID user,
							XnCalibrationStatus calibrationError, void *pCookie)
		 @endcode
		 *
		 * @param [in]	skeleton 			SkeletonCapability object that raised the event.
		 * @param [in]	user 				ID of the user holding SkeletonCapability object.
		 * @param [in]	calibrationError 	Status of the calibration. Values are defined by
		 * 									XnCalibrationStatus.
		 * @param [in]	pCookie 			User's cookie, to be delivered to the callback.
		 */
		typedef void (XN_CALLBACK_TYPE* CalibrationComplete)(SkeletonCapability& skeleton, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie);

		/**
		 * @brief Registers an event handler for the 'Calibration Complete' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters an event handler for the 'Calibration Complete' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromCalibrationComplete(XnCallbackHandle hCallback)
		{
			CalibrationCompleteCookie* pSkeletonCookie = (CalibrationCompleteCookie*)hCallback;
			xnUnregisterFromCalibrationComplete(GetHandle(), pSkeletonCookie->hCallback);
			xnOSFree(pSkeletonCookie);
		}

		/** @cond */
		XN_API_DEPRECATED("Use the overload with one argument - the bState parameter is useless")
		inline XnBool IsJointActive(XnSkeletonJoint eJoint, XnBool /*bState*/) const
		{
			return xnIsJointActive(GetHandle(), eJoint);
		}

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

		inline void XN_API_DEPRECATED("Please use UnregisterFromCalibrationStart/Complete") UnregisterCalibrationCallbacks(XnCallbackHandle hCallback)
		{
			SkeletonCookie* pSkeletonCookie = (SkeletonCookie*)hCallback;
#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			xnUnregisterCalibrationCallbacks(GetHandle(), pSkeletonCookie->hCallback);
#pragma warning (pop)
			xnOSFree(pSkeletonCookie);
		}
		/** @endcond */

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
	 * @ingroup cppref_prd_func
	 *
	 * <b>Purpose:</b> Provides a 'Pose Detection' functionality for a @ref xn::UserGenerator
	 * node. This capability lets the node recognize when the user is posed in a specific
	 * position.
	 *
	 * <b>Usage:</b> Do not instantiate directly. Instead, use @ref UserGenerator::GetPoseDetectionCap()
	 * to obtain an instance.
	 *
	 * <b>Data output:</b> Raises events.
	 *
	 * <b>Remarks:</b>
	 *
	 * A pose is a specific full body posture involving all the limbs. The PoseDetectionCapability
	 * object scans the FOV to detect users performing poses. The PoseDetectionCapability
	 * then generates the pose data.
	 *
	 * When and if the @ref PoseDetectionCapability object detects a pose it raises the
	 * 'Pose Detected' event.
	 */
	class PoseDetectionCapability : public Capability
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline PoseDetectionCapability(XnNodeHandle hNode) : Capability(hNode) {}
		PoseDetectionCapability(const NodeWrapper& node) : Capability(node) {}

		/**
		 * <b>'Pose Detection' event</b>
		 *
		 * This event is raised to signal that a specific user has entered a pose.
		 *
		 * @param [in]	pose	Reference to the specific @ref xn::UserGenerator node
		 * 						that raised the event.
		 * @param [in]	strPose	Name of the specific pose defined for this event handler.
		 * 						(The PoseDetectionCapability can define more than a single pose that can be detected.)
		 * @param [in]	user	ID of the user that entered the pose.
		 * @param [in]	pCookie	Returns any information ("cookie") that the application
		 * 						placed in this parameter at the time of registering the event..
		 */
		typedef void (XN_CALLBACK_TYPE* PoseDetection)(PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, void* pCookie);

		/**
		 * @brief Get the number of poses supported by this capability.
		 *
		 * <b>Remarks</b>
		 *
		 * The number of supported poses is defined by the developer of the capability and
		 * is common to all @ref xn::UserGenerator nodes using this capability on this OpenNI
		 * deployment.
		 */
		inline XnUInt32 GetNumberOfPoses() const
		{
			return xnGetNumberOfPoses(GetHandle());
		}

		/**
		 * @brief Gets the names of all poses supported by this capability.

		 * @param [out]		pstrPoses 		Pre-allocated memory for the names of the supported poses.
		 * @param [in]		nNameLength 	Memory size for each pose name.
		 * @param [in,out]	nPoses 			In input - size of the preallocated memory, in output
		 * 									- the number of pose names.
		 *
		 * <b>Remarks</b>
		 *
		 * The names of the poses are preset in advance by the developer of this capability.
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

		/**
		 * @brief Starts attempting to detect a pose for a specific user.
		 *
		 * @param [in]	strPose 	Name of the requested pose. Call the GetAvailablePoses()
		 * 							method to get the names of poses supported by the PoseDetectionCapability.
		 * @param [in]	user 		ID of a user to detect a pose for.
		 *
		 * <b>Remarks</b>
		 *
		 * This method will typically be used after a new user has been recognized in the
		 * FOV. This is typically the next stage in the process of calibrating the new user -
		 * @see skeleton_calib.
		 * When and if the @ref PoseDetectionCapability object detects a pose it raises
		 * the 'Pose Detected' event.
		 */
		inline XnStatus StartPoseDetection(const XnChar* strPose, XnUserID user)
		{
			return xnStartPoseDetection(GetHandle(), strPose, user);
		}

		/**
		 * @brief Stops attempting to detect a pose (for a specific user).
		 *
		 * @param [in]	user ID of a user.
		 *
		 * <b>Remarks</b>
		 *
		 * The application will typically call this method when it has already received
		 * a 'Pose Detected' event. Once a pose has been detected it is no longer necessary
		 * to try to detect poses.
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

		/**
		 * @brief Registers an event handler for the 'Pose Detected' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Registers an event handler for the 'Out Of Pose' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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
		/**
		 * @brief Unregisters an event handler for the 'Pose Detected' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromPoseDetected(XnCallbackHandle hCallback)
		{
			PoseDetectionCookie* pPoseCookie = (PoseDetectionCookie*)hCallback;
			xnUnregisterFromPoseDetected(GetHandle(), pPoseCookie->hCallback);
			xnOSFree(pPoseCookie);
		}
		/**
		 * @brief Unregisters an event handler for the 'Out Of Pose' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromOutOfPose(XnCallbackHandle hCallback)
		{
			PoseDetectionCookie* pPoseCookie = (PoseDetectionCookie*)hCallback;
			xnUnregisterFromOutOfPose(GetHandle(), pPoseCookie->hCallback);
			xnOSFree(pPoseCookie);
		}

		/**
		 * <b>'Pose In Progress' event</b>:
		 *
		 * Raised at each frame time from the time StartPoseDetection was called until the
		 * pose is detected. This event returns the current status of pose detection of
		 * a user.
		 *
		 * @param [in]	pose		Reference to the specific @ref UserGenerator node that raised
		 * 							the event.
		 * @param [in]	strPose		Name of the specific pose defined for this event handler.
		 * 							(The PoseDetectionCapability can define more than a single pose that can be detected.
		 * @param [in]	user		ID of the user that entered the pose.
		 * @param [in]	poseError	Current status of the pose detection.
		 * 							Defined statuses:
		 * 							XN_POSE_DETECTION_STATUS_OK
		 * 							XN_POSE_DETECTION_STATUS_NO_USER
		 * 							XN_POSE_DETECTION_STATUS_TOP_FOV
		 * 							XN_POSE_DETECTION_STATUS_SIDE_FOV
		 * 							XN_POSE_DETECTION_STATUS_ERROR
		 * @param [in]	pCookie		Returns any information ("cookie") that the application
		 * 							placed in this parameter at the time of registering the event.
		 */
		typedef void (XN_CALLBACK_TYPE* PoseInProgress)(PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, XnPoseDetectionStatus poseError, void* pCookie);

		/**
		 * @brief Registers an event handler for the 'Pose In Progress' event.
		 * @param [in]	handler  Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 *
		 * <b>Remarks</b>
		 *
		 * This event is raised at each frame time from the time StartPoseDetection() method
		 * was called until the pose is actually detected.
		 *
		 * See the @ref PoseDetection callback type declaration for how to write the callback
		 * (event handler).
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

		/**
		 * @brief Unregisters an event handler for the 'Pose In Progress' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromPoseInProgress(XnCallbackHandle hCallback)
		{
			PoseInProgressCookie* pPoseCookie = (PoseInProgressCookie*)hCallback;
			xnUnregisterFromPoseDetectionInProgress(GetHandle(), pPoseCookie->hCallback);
			xnOSFree(pPoseCookie);
		}

		/** @cond */
		inline XnStatus XN_API_DEPRECATED("Use GetAllAvailablePoses() instead") GetAvailablePoses(XnChar** pstrPoses, XnUInt32& nPoses) const
		{
			return xnGetAvailablePoses(GetHandle(), pstrPoses, &nPoses);
		}

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

		inline void XN_API_DEPRECATED("Please use UnregisterFromPoseDetected/UnregisterFromOutOfPose instead") UnregisterFromPoseCallbacks(XnCallbackHandle hCallback)
		{
			PoseCookie* pPoseCookie = (PoseCookie*)hCallback;
#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			xnUnregisterFromPoseCallbacks(GetHandle(), pPoseCookie->hCallback);
#pragma warning (pop)
			xnOSFree(pPoseCookie);
		}
		/** @endcond */

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
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> The UserGenerator node generates data describing users that it recognizes
	 * in the scene, identifying each user individually and thus allowing actions to be
	 * done on specific users.
	 *
	 * <b>Usage:</b> Instantiate the class and call Create() to create a user generator node.
	 *
	 * <b>Data output:</b>
	 * - CoM from UserGenerator local data
	 * - User pixels from @ref SceneMetaData
	 *
	 * <b>Capabilities:</b>
	 * - @ref SkeletonCapability
	 * - @ref PoseDetectionCapability
	 *
	 * <b>Remarks:</b>
	 *
	 * The UserGenerator node provides access to the recognized users through user IDs it
	 * returns. The user IDs are unique user identification numbers. The application uses
	 * these user IDs to pass them to various methods of this node to access the specific
	 * data of the specified user. For example:
	 *
	 * @ref xn::UserGenerator::GetUserPixels() gets the pixel map of a specific user, and
	 * @ref xn::UserGenerator::GetCoM() gets the location of the center of mass of a specific
	 * user.
	 *
	 * Typically, an application will require only a single UserGenerator node. However,
	 * there are cases where more than one UserGenerator node could be required. For example,
	 * where there are two sensors it may be convenient to build a production graph with
	 * two UserGenerator nodes, one node for each depth output.
	 *
	 * <b>Main Features:</b>
	 *
	 * - User CoM:  Provides the location of the center of mass of a specific user.
	 * - Number of users: Provides the number of users currently detected in the scene.
	 * - Users list: Provides a list of the IDs of currently recognized users in the scene.
	 * - User pixels: Provides the pixels that represent a specific user.
	 * - Events: Provides events about users in the scene:
	 *   - A new user is identified ('New User')
	 *   - An existing user exits temporarily ('User Exit')
	 *   - An exited user returns ('User Reenter')
	 *   - An existing user exits permanently ('Lost User')
	 *
	 * @note Registering to the 'New User' and 'Lost User' events is done by calling @ref RegisterUserCallbacks().
	 * Registering to the 'User Exit' and 'User Reenter' events is done by calling @ref RegisterToUserExit() and @ref RegisterToUserReEnter().
	 *
	 * @section user_events Events
	 *
	 * <b>New User' event</b>
	 *
	 * Signals that a new user has now been recognized in the scene. A new user is a
	 * user that was not previously recognized in the scene, and is now recognized in
	 * the scene.
	 *
	 * <b>'Lost User' event</b>
	 *
	 * Signals that a user has been lost from the list of previously recognized users
	 * in the scene.
	 *
	 * The exact meaning of a lost user is decided by the developer of the user generator.
	 * However, a typical implementation would define that a lost user is a previously
	 * recognized user that then exits the scene and does not return, even after a 'Lost
	 * User' timeout has elapsed. Thus this event is raised only after some delay after
	 * the user actually exited the scene.
	 *
	 * Using the above approach, the difference then between the 'Lost User' event and
	 * the 'User Exit' event (see below) would be that the 'User Exit' event is raised
	 * every time a recognized user leaves the scene, but a 'Lost User' event is raised
	 * only after the user has remained absent ("exit-ed") for a certain timeout period
	 * of time.
	 *
	 * If no timeout is used at all, the 'User Exit' and 'User Reenter' events are redundant.
	 *
	 * <b>'User Exit' event</b>
	 *
	 * Signals that a user now exited from the scene.
	 *
	 * The exact meaning of a user that exited is decided by the developer of the
	 * user generator. However, a typical implementation would define that a user that
	 * exited the scene is a previously recognized user that then leaves the scene.
	 * but returns within a specified time, before the 'Lost User' timeout has elapsed.
	 * In such a case the UserGenerator object still remembers the user and so when
	 * the user comes back, everything known about it is restored. See the 'Lost User'
	 * event above for more explanation of the difference between the 'User Exit' event
	 * and the 'Lost User' event in a typical implementation.
	 *
	 * <b>'User Reenter' event</b>
	 *
	 * Signals that a user has now reentered to the scene after exiting. A user that
	 * reenters the scene after exiting is a recognized user that exited, but now is
	 * returning within a specified permitted time span, before the 'Lost User' timeout
	 * has elapsed. In such a case the UserGenerator node still remembers the user and
	 * so when the user returns, everything known about it is restored.
	 *
	 * <b>Event Sequence</b>
	 *
	 * The following event sequences are typical:
	 *
	 * A:
	 * -# 'New User'
	 * -# 'User Exit'
	 * -# 'Lost User'
	 *
	 * B:
	 * -# 'New User'
	 * -# 'User Exit'
	 * -# 'User Reenter'
	 * -# 'User Exit'
	 * -# 'Lost User'
	 */
	class UserGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline UserGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}
		inline UserGenerator(const NodeWrapper& other) : Generator(other) {}

		/**
		 * @brief Creates a UserGenerator node from available @ref node_alternative "production node alternatives".
		 *
		 * <b>Remarks:</b>
		 *
		 * See @ref create_method for a detailed description of this method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * @brief Event handler signature for all UserGenerator events.
		 *
		 * Example of a handler prototype (for the 'New User' event in this case):
		 @code
		 void XN_CALLBACK_TYPE OnNewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
		 @endcode
		 *
		 * @param [in]	generator 	UserGenerator node that raised the event.
		 * @param [in]	user 		ID of the user.
		 * @param [in]	pCookie 	User's cookie, to be delivered to the callback.
		 */
		typedef void (XN_CALLBACK_TYPE* UserHandler)(UserGenerator& generator, XnUserID user, void* pCookie);

		/**
		 * @brief Gets the number of users currently identified in the scene.
		 *
		 * <b>Remarks:</b>
		 *
		 * The result of this method is required as input for the GetUsers() method.
		 */
		inline XnUInt16 GetNumberOfUsers() const
		{
			return xnGetNumberOfUsers(GetHandle());
		}

		/**
		 * @brief Gets an array of user IDs of all the recognized users in the scene at the current time.
		 *
		 * @param [out]		aUsers 	Preallocated memory for the method to enter the user IDs.
		 * @param [in,out]	nUsers	Number of users.
		 *
		 * @c nUsers is used both as an in parameter and an out one in this method:
		 * [in] Size of the aUsers array (in units of users).
		 * To get the value of nUsers (i.e., the size of array that you want - see
		 * parameter below), you use the GetNumberOfUsers() method.
		 * For example, if you want an array of size 4 - to contain a maximum of 4 users -
		 * you pass 4 as the value of this parameter.
		 *
		 * [out] On completion, the method returns the number of users that were recognized.
		 * E.g., if there is actually only one user, the output will be 1, although the
		 * caller passed a value of 4 (as above) for to allocate an array of size 4.
		 *
		 * <b>Remarks:</b>
		 *
		 * The output user IDs from this method can be used as input to the @ref xn::UserGenerator::GetCoM()
		 * method for getting the center of mass of each user.
		 */
		inline XnStatus GetUsers(XnUserID aUsers[], XnUInt16& nUsers) const
		{
			return xnGetUsers(GetHandle(), aUsers, &nUsers);
		}

		/**
		 * @brief Gets the position of a user's center of mass. This is the single point for
		 * representing the user.
		 *
		 * @param [in]	user 	Specifies a user ID.
		 * @param [out]	com 	The user's center of mass in 3-D coordinates.
		 *
		 * The center of mass is a useful point to represent the user. When you don't have
		 * any other reference point (e.g., you don't have the position of a specific joint,
		 * or of the head, or any other such point), this is an adequate point with which
		 * to start to represent the user.
		 *
		 * <b>Remarks:</b>
		 *
		 * The returned @ref XnPoint3D value is the center of mass of all the shown pixels
		 * that belong to that user, but not necessarily the real center of mass of the
		 * human user.
		 * The application can use this method to request the center of mass for each user.
		 */
		inline XnStatus GetCoM(XnUserID user, XnPoint3D& com) const
		{
			return xnGetUserCoM(GetHandle(), user, &com);
		}

		/**
		 * @brief Gets the pixel map of the specified user in the scene and saves it in the @ref xn::SceneMetaData object.
		 * The pixels that form part of the user's body are labeled with user's ID. Other values are irrelevant.
		 *
		 * @param [in]	user	The ID of the user the pixel map should be returned for.
		 * @param [out]	smd		Struct to be filled with the pixel map.
		 *
		 * <b>Remarks:</b>
		 *
		 * A usage example for this method in gaming is for coloring the avatar on the screen.
		 *
		 * The special value 0 (zero) can be used in the @c user parameter, telling the generator to return a label map
		 * containing all users, where each pixel is either marked as 0 (background pixel) or with the user ID to which
		 * it belongs.
		 */
		inline XnStatus GetUserPixels(XnUserID user, SceneMetaData& smd) const
		{
			return xnGetUserPixels(GetHandle(), user, smd.GetUnderlying());
		}

		/**
		 * @brief Registers event handlers for the 'New User' and 'Lost User' events.
		 *
		 * @param [in]	NewUserCB  	Callback function to be invoked when the 'New User' event is raised.
		 * @param [in]	LostUserCB  Callback function to be invoked when the 'Lost User' event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters event handlers for the 'New User' and 'Lost User' events.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterUserCallbacks(XnCallbackHandle hCallback)
		{
			UserCookie* pUserCookie = (UserCookie*)hCallback;
			xnUnregisterUserCallbacks(GetHandle(), pUserCookie->hCallback);
			xnOSFree(pUserCookie);
		}

		/**
		 * @brief Gets a @ref SkeletonCapability object for accessing Skeleton functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * Once the @ref SkeletonCapability object has been obtained, the application can
		 * work with a skeleton.
		 *
		 * It is the application's responsibility to check first if Skeleton capability
		 * is supported by calling the @ref IsCapabilitySupported() method..
		 */
		inline const SkeletonCapability GetSkeletonCap() const
		{
			return SkeletonCapability(GetHandle());
		}

		/**
		 * @brief Gets a @ref SkeletonCapability object for accessing Skeleton functionality.
		 *
		 * <b>Remarks:</b>
		 *
		 * Once the @ref SkeletonCapability object has been obtained, the application can
		 * work with a skeleton.
		 *
		 * It is the application's responsibility to check first if Skeleton capability
		 * is supported by calling the @ref IsCapabilitySupported() method..
		 */
		inline SkeletonCapability GetSkeletonCap()
		{
			return SkeletonCapability(GetHandle());
		}

		/**
		 * @brief Gets a @ref xn::PoseDetectionCapability object for accessing Pose Detection
		 * functionality
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_POSE_DETECTION
		 * is supported
		 * by calling @ref xn::ProductionNode::IsCapabilitySupported().
		 */
		inline const PoseDetectionCapability GetPoseDetectionCap() const
		{
			return PoseDetectionCapability(GetHandle());
		}

		/**
		 * @brief Gets a @ref xn::PoseDetectionCapability object for accessing Pose Detection
		 * functionality
		 *
		 * <b>Remarks:</b>
		 *
		 * It is the application responsibility to check first if @ref XN_CAPABILITY_POSE_DETECTION
		 * is supported
		 * by calling @ref xn::ProductionNode::IsCapabilitySupported().
		 */
		inline PoseDetectionCapability GetPoseDetectionCap()
		{
			return PoseDetectionCapability(GetHandle());
		}

		/**
		 * @brief Registers an event handler for the 'User Exit' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters an event handler for the 'User Exit' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromUserExit(XnCallbackHandle hCallback)
		{
			UserSingleCookie* pUserCookie = (UserSingleCookie*)hCallback;
			xnUnregisterFromUserExit(GetHandle(), pUserCookie->hCallback);
			xnOSFree(pUserCookie);
		}

		/**
		 * @brief Registers an event handler for the 'User Reenter' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
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

		/**
		 * @brief Unregisters an event handler for the 'User Reenter' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
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
	 * @ingroup cppref_prd_func
	 * <b>Purpose:</b> An audio generator node generates audio data.
	 *
	 * <b>Event: 'Wave Output Mode Change'</b>
	 *
	 * Signals that the node's current configuration's Wave Output Mode has been changed
	 * to some other wave output mode.
	 *
	 * Use @ref RegisterToWaveOutputModeChanges() and @ref UnregisterFromWaveOutputModeChanges() for using this event.
	 *
	 * This event is raised by calling SetWaveOutputMode().
	 */
	class AudioGenerator : public Generator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline AudioGenerator(XnNodeHandle hNode = NULL) : Generator(hNode) {}
		inline AudioGenerator(const NodeWrapper& other) : Generator(other) {}

		/**
		 * @brief Creates an AudioGenerator node from available @ref node_alternative "production node alternatives".
		 *
		 * <b>Remarks</b>
		 *
		 * See @ref create_method for a detailed description of this method.
		 */
		inline XnStatus Create(Context& context, Query* pQuery = NULL, EnumerationErrors* pErrors = NULL);

		/**
		 * @brief Gets the audio generator node's latest @ref glos_frame_object "frame object", saving
		 * it in the @ref xn::AudioMetaData object. This @ref glos_frame_object "frame object" is a
		 * snapshot of the generated label map data and its associated configuration information at a
		 * certain point in time. This saved @ref glos_frame_object "frame object" provides fast
		 * and easy access to the audio generator node's data and configuration information.
		 *
		 * @param [out]	metaData	Structure to be filled with the frame object.
		 *
		 * <b>Remarks:</b>
		 *
		 * This method is central in the use of the AudioGenerator node. Once the AudioGenerator
		 * node is generating data, after each @ref conc_updating_data "'Update Data'" call,
		 * the application calls this method to get the node's frame object, which contains
		 * the data and all its associated properties.
		 */
		inline void GetMetaData(AudioMetaData& metaData) const
		{
			xnGetAudioMetaData(GetHandle(), metaData.GetUnderlying());
		}

		/**
		 * @copybrief xnGetAudioBuffer
		 * For full details and usage, see @ref xnGetAudioBuffer
		 */
		inline const XnUChar* GetAudioBuffer() const
		{
			return xnGetAudioBuffer(GetHandle());
		}

		/**
		 * @brief Gets the node's number of supported wave output modes.
		 */
		inline XnUInt32 GetSupportedWaveOutputModesCount() const
		{
			return xnGetSupportedWaveOutputModesCount(GetHandle());
		}

		/**
		 * @brief Returns a list of the wave output modes that the @ref AudioGenerator node supports.
		 * @param [out]		aSupportedModes Buffer to be filled with the list of wave output modes.
		 * @param [in,out]	nCount 			[in] Size of the array.
											[out] Number of modes returned.
		 */
		inline XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode* aSupportedModes, XnUInt32& nCount) const
		{
			return xnGetSupportedWaveOutputModes(GetHandle(), aSupportedModes, &nCount);
		}

		/**
		 * @brief Sets the current wave output mode of the AudioGenerator node. This output mode
		 * is used for generating the next audio packet.
		 *
		 * @param [in]	OutputMode 	Mode to set as the current wave output mode.
		 *
		 * <b>Remarks</b>
		 *
		 * The wave output mode is contained in the @ref xn::XnWaveOutputMode struct. It
		 * comprises: bits per sample, channel, and sample rate.
		 */
		inline XnStatus SetWaveOutputMode(const XnWaveOutputMode& OutputMode)
		{
			return xnSetWaveOutputMode(GetHandle(), &OutputMode);
		}

		/**
		 * @brief Gets the current AudioGenerator node's wave output mode. This output mode
		 * is used for generating the next audio packet.
		 *
		 * @param [out]	OutputMode Current wave output mode.
		 *
		 * <b>Remarks</b>
		 *
		 * The wave output mode is contained in the XnWaveOutputMode struct. Each wave output
		 * mode comprises settings for bits per sample, channel, and sample rate, as follows:
		 */
		inline XnStatus GetWaveOutputMode(XnWaveOutputMode& OutputMode) const
		{
			return xnGetWaveOutputMode(GetHandle(), &OutputMode);
		}

		/**
		 * @brief Registers an event handler for the 'Wave Output Mode Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToWaveOutputModeChanges(StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return _RegisterToStateChange(xnRegisterToWaveOutputModeChanges, GetHandle(), handler, pCookie, hCallback);
		}

		/**
		 * @brief Unregisters an event handler for the 'Wave Output Mode Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback)
		{
			_UnregisterFromStateChange(xnUnregisterFromWaveOutputModeChanges, GetHandle(), hCallback);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 * Represents a Mock Audio Generator node
	 */
	class MockAudioGenerator : public AudioGenerator
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline MockAudioGenerator(XnNodeHandle hNode = NULL) : AudioGenerator(hNode) {}
		inline MockAudioGenerator(const NodeWrapper& other) : AudioGenerator(other) {}

		/**
		 * Creates a mock audio generator
		 *
		 * @param [in]	context	The context in which the node should be generated
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus Create(Context& context, const XnChar* strName = NULL);

		/**
		 * Creates a mock audio generator, copying the initial state from another audio
		 * generator.
		 *
		 * @param [in]	other	The audio generator to copy state from
		 * @param [in]	strName	Optional. The name of the new node.
		 */
		XnStatus CreateBasedOn(AudioGenerator& other, const XnChar* strName = NULL);

		/**
		 * @copybrief xnMockAudioSetData
		 * For full details and usage, see @ref xnMockAudioSetData
		 */
		inline XnStatus SetData(XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pAudioBuffer)
		{
			return xnMockAudioSetData(GetHandle(), nFrameID, nTimestamp, nDataSize, pAudioBuffer);
		}

		/**
		 * Sets the data of the mock node from a meta data object, overriding frame ID and
		 * timestamp
		 *
		 * @param [in]	audioMD		Object to take data from
		 * @param [in]	nFrameID	Frame ID
		 * @param [in]	nTimestamp	Timestamp
		 */
		inline XnStatus SetData(const AudioMetaData& audioMD, XnUInt32 nFrameID, XnUInt64 nTimestamp)
		{
			return SetData(nFrameID, nTimestamp, audioMD.DataSize(), audioMD.Data());
		}

		/**
		 * Sets the data of the mock node from a meta data object
		 *
		 * @param [in]	audioMD	Object to take data from
		 */
		inline XnStatus SetData(const AudioMetaData& audioMD)
		{
			return SetData(audioMD, audioMD.FrameID(), audioMD.Timestamp());
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 */
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
	 * @ingroup cppref_prd_func
	 * Represents a Codec node.
	 */
	class Codec : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline Codec(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
		inline Codec(const NodeWrapper& other) : ProductionNode(other) {}

		/**
		 * @copybrief xnCreateCodec
		 * For full details and usage, see @ref xnCreateCodec
		 */
		inline XnStatus Create(Context& context, XnCodecID codecID, ProductionNode& initializerNode);

		/**
		 * @copybrief xnGetCodecID
		 * For full details and usage, see @ref xnGetCodecID
		 */
		inline XnCodecID GetCodecID() const
		{
			return xnGetCodecID(GetHandle());
		}

		/**
		 * @copybrief xnEncodeData
		 * For full details and usage, see @ref xnEncodeData
		 */
		inline XnStatus EncodeData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
		{
			return xnEncodeData(GetHandle(), pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
		}

		/**
		 * @copybrief xnDecodeData
		 * For full details and usage, see @ref xnDecodeData
		 */
		inline XnStatus DecodeData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
		{
			return xnDecodeData(GetHandle(), pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
		}
	};

	/**
	 * @ingroup cppref_prd_func
	 *
	 * <b>Purpose:</b> The ScriptNode object loads an XML script from a file or string,
	 * and then runs the XML script to build a production graph. It also references every
	 * node created from that script, so it wouldn't be destroyed.
	 *
	 * <b>Remarks:</b>
	 *
	 * A typical usage of a script node is:
	 * - Create the script node, using the @ref Create() method.
	 * - Load XML script from file or string, using @ref LoadScriptFromFile() or @ref LoadScriptFromString().
	 * - Execute the script, using @ref Run().
	 *
	 * Note that the context's @ref xn::Context::RunXmlScriptFromFile "RunXmlScriptFromFile()" or
	 * @ref xn::Context::RunXmlScript "RunXmlScript()" methods can be used to perform
	 * all above steps.

	 * All production nodes in the production graph use reference count to determine their life time,
	 * but if an application executed a script, it can't know upfront which nodes will be created
	 * by this script, and so, can't take reference to them, so those nodes might be destroyed
	 * immediately. The script node, apart from executing the script, also keeps reference to
	 * all the nodes created by the script. This means that if the script node is destroyed,
	 * every production node that was created by the script, and is unreferenced by the application
	 * will also be destroyed. For this reason, it is recommended for application using XML scripts
	 * to keep a reference to the script node as long as they keep a reference to the context itself.
	 *
	 * A single ScriptNode object is responsible for building the entire production graph,
	 * irrespective of however many node definitions there are in the XML script and how
	 * many production nodes are created.
	 *
	 * For additional information about XML scripts, see @ref xmlscripts.
	 */
	class ScriptNode : public ProductionNode
	{
	public:
		/**
		 * Ctor
		 *
		 * @param [in]	hNode	Node handle
		 */
		inline ScriptNode(XnNodeHandle hNode = NULL) : ProductionNode(hNode) {}
		inline ScriptNode(const NodeWrapper& other) : ProductionNode(other) {}

		inline XnStatus Create(Context& context, const XnChar* strFormat);

		inline const XnChar* GetSupportedFormat()
		{
			return xnScriptNodeGetSupportedFormat(GetHandle());
		}

		/**
		 * @brief Loads an XML script file into the ScriptNode object.
		 *
		 * @param [in]	strFileName Name of file containing an XML script.
		 */
		inline XnStatus LoadScriptFromFile(const XnChar* strFileName)
		{
			return xnLoadScriptFromFile(GetHandle(), strFileName);
		}

		/**
		 * @brief Loads an XML script string into the ScriptNode object.
		 *
		 * @param [in]	strScript String containing an XML script.
		 */
		inline XnStatus LoadScriptFromString(const XnChar* strScript)
		{
			return xnLoadScriptFromString(GetHandle(), strScript);
		}

		/**
		 * @brief Runs the ScriptNode object's XML script to build a production graph.
		 *
		 * @param [out]	pErrors 	List of enumeration errors produced during method execution.
		 *
		 * <b>Remarks</b>
		 *
		 * This method causes the whole production graph to enter Generating state. To read
		 * data you have to run one of the @ref conc_updating_data "'Update Data'" methods
		 * of the Context or of the node itself.
		 */
		inline XnStatus Run(EnumerationErrors* pErrors);
	};

	//---------------------------------------------------------------------------
	// EnumerationErrors
	//---------------------------------------------------------------------------
	/**
	 * @ingroup cppref_graph_mgmt
	 * <b>Purpose:</b> An EnumerationError object returns a full list of modules that failed
	 * to enumerate specifying exactly why each module failed.
	 *
	 * <b>Usage:</b> Instantiate and pass to enumeration methods.
	 *
	 * <b>Remarks:</b>
	 *
	 * Sometimes an application enumerates for a specific node, and receives zero results.
	 * An obvious reason for this failure is that no module implementing the node type was
	 * installed. Another possible reason is that a module may be installed but has no license.
	 * Yet another possible reason is that a required hardware device is currently disconnected.
	 *
	 * Applications can use the EnumerationErrors class to acquire a full list of modules
	 * that failed to enumerate specifying for each exactly why each module failed.
	 *
	 * The methods provided by the EnumerationErrors class let the application search enumeration
	 * lists for specific types of nodes.
	 */
	class EnumerationErrors
	{
	public:
		/**
		 * Ctor
		 */
		inline EnumerationErrors() : m_bAllocated(TRUE), m_pErrors(NULL) { xnEnumerationErrorsAllocate(&m_pErrors); }

		/**
		 * Ctor
		 *
		 * @param [in]	pErrors		underlying C object to wrap
		 * @param [in]	bOwn		TRUE to own the object (i.e. free it upon destruction), FALSE
		 * 							otherwise.
		 */
		inline EnumerationErrors(XnEnumerationErrors* pErrors, XnBool bOwn = FALSE) : m_bAllocated(bOwn), m_pErrors(pErrors) {}

		/**
		 * Dtor
		 */
		~EnumerationErrors() { Free(); }

		/**
		 * An iterator over enumeration errors
		 */
		class Iterator
		{
		public:
			friend class EnumerationErrors;

			/**
			 * Checks if the other iterator points to the same location
			 *
			 * @param [in]	other	another iterator
			 */
			XnBool operator==(const Iterator& other) const
			{
				return m_it == other.m_it;
			}

			/**
			 * Checks if the other iterator points to a different location
			 *
			 * @param [in]	other	another iterator
			 */
			XnBool operator!=(const Iterator& other) const
			{
				return m_it != other.m_it;
			}

			/**
			 * Moves the iterator to the next location. If end of list was reached, iterator
			 * will be equal to @ref EnumerationErrors::End().
			 */
			inline Iterator& operator++()
			{
				m_it = xnEnumerationErrorsGetNext(m_it);
				return *this;
			}

			/**
			 * Returns an iterator to the next location. If end of list was reached, iterator
			 * will be equal to @ref EnumerationErrors::End().
			 */
			inline Iterator operator++(int)
			{
				return Iterator(xnEnumerationErrorsGetNext(m_it));
			}

			/**
			 * @brief Returns the description data of the failing node the iterator points to.
			 *
			 * <b>Remarks</b>
			 *
			 * The description data returned in the @ref XnProductionNodeDescription struct
			 * includes the node name, the vendor, and the version.
			 */
			inline const XnProductionNodeDescription& Description() { return *xnEnumerationErrorsGetCurrentDescription(m_it); }

			/**
			 * @brief Returns the failure error code of the failing node the iterator points to.
			 * For a string representation of this error, call @ref xnGetStatusString().
			 */
			inline XnStatus Error() { return xnEnumerationErrorsGetCurrentError(m_it); }

		private:
			inline Iterator(XnEnumerationErrorsIterator it) : m_it(it) {}

			XnEnumerationErrorsIterator m_it;
		};

		/**
		 * @brief Gets an iterator to the first item in an enumeration errors list.
		 *
		 * <b>Remarks</b>
		 *
		 * To access the contents of the error item, either use the @ref ToString() method
		 * of this class or the methods of the @ref EnumerationErrors::Iterator class.
		 */
		inline Iterator Begin() const { return Iterator(xnEnumerationErrorsGetFirst(m_pErrors)); }

		/**
		 * @brief Returns an iterator referring to the past-the-end element in an enumeration errors list.
		 * This iterator does not point to an actual item.
		 *
		 * <b>Remarks</b>
		 *
		 * Use this as the sentinel for flagging the end of a list when searching.
		 */
		inline Iterator End() const { return Iterator(NULL); }

		/**
		 * @brief Returns a string representation of the Enumeration Errors object. The string
		 * contains a full list of the modules that failed the most recent enumeration operation.
		 * For each failed module, the Enumeration Errors object specifies why exactly the module failed.
		 *
		 * @param [in]	csBuffer 	Character buffer to be filled.
		 * @param [in]	nSize 		Size of the character buffer.
		 */
		inline XnStatus ToString(XnChar* csBuffer, XnUInt32 nSize)
		{
			return xnEnumerationErrorsToString(m_pErrors, csBuffer, nSize);
		}

		/**
		 * @copybrief xnEnumerationErrorsFree
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

		/**
		 * Gets the underlying C object
		 */
		inline XnEnumerationErrors* GetUnderlying() { return m_pErrors; }

	private:
		XnEnumerationErrors* m_pErrors;
		XnBool m_bAllocated;
	};

	//---------------------------------------------------------------------------
	// Context
	//---------------------------------------------------------------------------

	/**
	 * @ingroup cppref_graph_mgmt
	 *
	 * A context is a workspace where the application builds an OpenNI production graph.
	 * It contains methods for managing the production graph.
	 *
	 * In order to use any of the OpenNI functionality you must first construct a Context
	 * object and initialize it. Prior to this, the application cannot do anything with
	 * OpenNI.
	 *
	 * For a comprehensive overview to the Context class and its members, see @ref conc_context
	 * "Overview to Contexts".
	 *
	 * @section context_global_error_event Event: 'Global Error State Change'
	 *
	 * Signals that the error state of any of the nodes has changed.
	 *
	 * This event can be used by the application to get a signal as soon as any error occurs
	 * in one of the production nodes. The global error state aggregates error state from all
	 * the production nodes in the context.
	 *
	 * Use @ref RegisterToErrorStateChange() and @ref UnregisterFromErrorStateChange() for using this event.
	 *
	 * @section context_node_creation_event Event: 'Node Creation'
	 *
	 * Signals that a new node was created in the context production graph.
	 *
	 * Use @ref RegisterToNodeCreation() and @ref UnregisterFromNodeCreation() for using this event.
	 *
	 * @section context_node_destruction_event Event: 'Node Destruction'
	 *
	 * Signals that a node was removed from the context production graph, and destroyed.
	 *
	 * Use @ref RegisterToNodeDestruction() and @ref UnregisterFromNodeDestruction() for using this event.
	 */
	class Context
	{
	public:
		/**
		 * Callback prototype for the @ref context_node_creation_event "'Node Creation'" event handler.
		 *
		 * @param [in]	context		The context that raised this event.
		 * @param [in]	createdNode The id of the hand that disappeared.
		 * @param [in]	pCookie		A user-provided cookie that was given when registering to
		 * 							this event.
		 *
		 * Example for such a handler:
		 @code
		 void XN_CALLBACK_TYPE OnNodeCreation(Context& context, ProductionNode& createdNode, void* pCookie)
		 {}
		 @endcode
		 */
		typedef void (XN_CALLBACK_TYPE* NodeCreationHandler)(Context& context, ProductionNode& createdNode, void* pCookie);

		/**
		 * Callback prototype for the @ref context_node_destruction_event "'Node Destruction'" event handler.
		 *
		 * @param [in]	context					The context that raised this event.
		 * @param [in]	strDestroyedNodeName	The name of the node that was destroyed.
		 * @param [in]	pCookie					A user-provided cookie that was given when registering to
		 * 										this event.
		 *
		 * Example for such a handler:
		 @code
		 void XN_CALLBACK_TYPE OnNodeDestruction(Context& context, const XnChar* strDestroyedNodeName, void* pCookie)
		 {}
		 @endcode
		 */
		typedef void (XN_CALLBACK_TYPE* NodeDestructionHandler)(Context& context, const XnChar* strDestroyedNodeName, void* pCookie);

		/**
		 * Ctor
		 */
		inline Context() : m_pContext(NULL), m_bUsingDeprecatedAPI(FALSE), m_bAllocated(FALSE), m_hShuttingDownCallback(NULL) {}

		/**
		 * Ctor
		 *
		 * @param [in]	pContext	Underlying C object
		 */
		inline Context(XnContext* pContext) : m_pContext(NULL), m_bUsingDeprecatedAPI(FALSE), m_bAllocated(FALSE), m_hShuttingDownCallback(NULL)
		{
			SetHandle(pContext);
		}

		/**
		 * Copy Ctor
		 *
		 * @param [in]	other 	Another context. Note that the context will only be destroyed
		 * 						when the original object is destroyed.
		 */
		inline Context(const Context& other) : m_pContext(NULL), m_bUsingDeprecatedAPI(FALSE), m_bAllocated(FALSE), m_hShuttingDownCallback(NULL)
		{
			SetHandle(other.m_pContext);
		}

		/**
		 * Dtor
		 */
		~Context()
		{
			SetHandle(NULL);
		}

		inline Context& operator=(const Context& other)
		{
			SetHandle(other.m_pContext);
			return *this;
		}

		/**
		 * Gets the underlying C object
		 */
		inline XnContext* GetUnderlyingObject() const { return m_pContext; }

		/**
		 * Checks if two context objects are references to the same context.
		 *
		 * @param [in]	other	Another object
		 */
		inline XnBool operator==(const Context& other)
		{
			return (GetUnderlyingObject() == other.GetUnderlyingObject());
		}

		/**
		 * Checks if two context objects are <i>not</i> references to the same context.
		 *
		 * @param [in]	other	Another object
		 */
		inline XnBool operator!=(const Context& other)
		{
			return (GetUnderlyingObject() != other.GetUnderlyingObject());
		}

		/**
		 * @brief Builds the context's general software environment.
		 *
		 * This method initializes runtime variables and data structures, and examines all
		 * registered plug-ins to learn the purpose and specific capabilities of each. In
		 * particular, during initialization the context initialization examines all registered
		 * plug-ins to learn the purpose and specific capabilities of each. For example,
		 * one plug-in is for creating a skeleton node, and another plug-in is for creating
		 * a depth node. Thus an entire database is built of plug-ins that can be queried
		 * according to vendor, model, and capabilities of each.
		 *
		 * Once you have completed the initialization you can create nodes that are based
		 * on the plug-ins that OpenNI has discovered by this initialization process.
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

		/**
		 * @brief Runs an XML script string to build a production graph.
		 *
		 * @param [in]		strScript	String containing an XML script.
		 * @param [out]		scriptNode	@ref xn::ScriptNode object as the root to all the nodes
		 * 								created from the XML file. When no longer needed, the node can
		 * 								be released, causing all the created nodes to decrement their reference counts.
		 * @param [in,out]	pErrors		Optional. To be filled with enumeration errors.
		 *
		 * <B>Remarks:</B>
		 *
		 * For a full description of the XML structure, see @ref xmlscripts.
		 * Compare this method with initialization methods that run an XML script from a
		 * file.
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

		/**
		 * @brief Runs an XML script file to build a production graph.
		 *
		 * @param [in]		strFileName	Name of file containing an XML script.
		 * @param [out]		scriptNode	scriptNode object as the root to all the nodes created
		 * 								from the XML file. When no longer needed, the node can
		 * 								be released, causing all the created nodes to decrement
		 *								their reference counts.
		 * @param [in,out]	pErrors		Optional. To be filled with enumeration errors.
		 *
		 * <B>Remarks:</B>
		 *
		 * The XML script file describes all the nodes you want to create. For each node
		 * description in the XML file, this method creates a @ref xn::ProductionNode object.
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

		/**
		 * @brief Shorthand combination of two other initialization methods - @ref Init() and then @ref RunXmlScriptFromFile() -
		 * to initialize the context object and then create a production graph.
		 *
		 * @param [in]		strFileName		Name of file containing an XML script.
		 * @param [out]		scriptNode		ScriptNode object as the root to all the nodes created
		 * 									from the XML file. When no longer needed, the node can be
		 * 									released, causing all the created nodes to decrement their reference counts.
		 * @param [out]		pErrors			Optional. To be filled with enumeration errors.
		 *
		 * <B>Remarks:</B>
		 *
		 * The XML script file describes all the nodes you want to create. For each node
		 * description in the XML file, this method creates a @ref xn::ProductionNode object.
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

		/**
		 * @brief Recreates a production graph from a recorded ONI file and then replays the data
		 * generation exactly as it was recorded.
		 *
		 * @param [in]	strFileName	Name of the recorded file to be run.
		 * @param [out]	playerNode	Returns a @ref xn::Player object through which
		 * 							playback can be controlled, e.g., seeking and setting playback speed.
		 *
		 * <B>Remarks:</B>
		 *
		 * OpenNI provides great flexibility of recording. You can decide you want to record
		 * only the final output, e.g., just a skeleton in movement; or record both the
		 * output skeleton and the depth input; or record only the depth input and then
		 * recreate the skeleton at run-time after reading the recorded raw depth data.
		 * You can also choose which nodes to record.
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

		/**
		 * @copybrief xnCreateMockNode
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

		/**
		 * @copybrief xnCreateMockNodeBasedOn
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

		/**
		 * @copybrief xnCreateCodec
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

		/**
		 * @copybrief xnContextAddRef
		 * For full details and usage, see @ref xnContextAddRef
		 */
		inline XnStatus AddRef()
		{
			return xnContextAddRef(m_pContext);
		}

		/**
		 * @copybrief xnContextRelease
		 * For full details and usage, see @ref xnContextRelease
		 */
		inline void Release()
		{
			SetHandle(NULL);
		}


		/**
		 * @copybrief xnAddLicense
		 * For full details and usage, see @ref xnAddLicense
		 */
		inline XnStatus AddLicense(const XnLicense& License)
		{
			return xnAddLicense(m_pContext, &License);
		}

		/**
		 * @copybrief xnEnumerateLicenses
		 * For full details and usage, see @ref xnEnumerateLicenses
		 */
		inline XnStatus EnumerateLicenses(XnLicense*& aLicenses, XnUInt32& nCount) const
		{
			return xnEnumerateLicenses(m_pContext, &aLicenses, &nCount);
		}

		/**
		 * @copybrief xnFreeLicensesList
		 * For full details and usage, see @ref xnFreeLicensesList
		 */
		inline static void FreeLicensesList(XnLicense aLicenses[])
		{
			xnFreeLicensesList(aLicenses);
		}

		/**
		 * @brief Enumerates all available production nodes for a specific node type (e.g., the application wants to create a
		 * depth generator node) and returns a full list of matching production nodes.
		 *
		 * @param [in]		Type		Requested node type.
		 * @param [in]		pQuery 		Optional. A query object that can be used to filter results.
		 * @param [out]		TreesList 	List of possible production nodes.
		 * @param [out]		pErrors 	Optional. To contain enumeration errors.
		 *
		 * <B>Remarks:</B>
		 *
		 * The application chooses one of the nodes from the node list returned by this method and uses it as input
		 * to the @ref xn::Context::CreateProductionTree() method to create a new production node.
		 *
		 * Using this method to help create a production node offers the greatest flexibility since it returns a
		 * complete list of matching production nodes and so the application can then choose the most suitable node.
		 *
		 * By contrast, using @ref xn::Context::CreateAnyProductionTree() "CreateAnyProductionTree()" or a node's <b>Create()</b>
		 * method creates a node from the first matching node that satisfies the query.
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

		/**
		 * @brief Enumerates for production nodes of a specific node type, and creates the first
		 * production node found of that type.
		 *
		 * @param [in]		type	Requested node type.
		 * @param [in]		pQuery 	Optional. A query object that can be used to filter results.
		 * @param [out]		node 	Handle to the newly created node.
		 * @param [in,out]	pErrors Optional. To contain enumeration errors.
		 *
		 * <B>Remarks:</B>
		 *
		 * This method is a 'shortcut' version of using the @ref EnumerateProductionTrees()
		 * method and then passing the first result to @ref CreateProductionTree().
		 * Thus, this method is exactly like the <b>Create()</b> method for nodes.
		 *
		 * Other ways of creating a production graph let the application retrieve a complete
		 * list of alternatives and then choose the most appropriate alternative.
		 *
		 * This method is the method used by the above XML script methods to create the
		 * ProductionNode objects. The XML method runs the XML script, and for each node
		 * description the XML method runs this function to create that node.
		 *
		 * Using this function is called 'creating a node from the context'. You can also
		 * create a node by invoking the <b>Create</b> method of the node itself, e.g.,
		 * the @ref xn::HandsGenerator::Create "Create()" method of a @ref xn::HandsGenerator
		 * "HandsGenerator" node.
		 *
		 * By default, this method does not enable the production nodes to start generating
		 * data immediately on creation.
		 *
		 * @sa @ref create_method for more detail.
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

		/**
		 * @brief Creates a production node from the information supplied in a @ref xn::NodeInfo object.
		 *
		 * @param [in]	Tree	Node properties for creating the new node.
		 * @param [out]	node 	Reference to the new @ref xn::ProductionNode object.
		 *
		 * <B>Remarks:</B>
		 *
		 * The application gets the NodeInfo object by choosing it from the list returned
		 * by the @ref xn::Context::EnumerateProductionTrees() method.
		 *
		 * If the new production node requires additional nodes for its input, and those
		 * nodes do not yet exist, then this method creates those additional nodes also
		 * and associates them with the "original" new production node.
		 *
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

		/**
		 * @brief Returns a list of all the context's existing created nodes.
		 *
		 * @param [out]	list	List of the context's existing created nodes.
		 *
		 * You can use the @ref xn::NodeInfoList::FilterList() method to filter the returned
		 * list of existing nodes.
		 */
		XnStatus EnumerateExistingNodes(NodeInfoList& list) const
		{
			XnNodeInfoList* pList;
			XnStatus nRetVal = xnEnumerateExistingNodes(m_pContext, &pList);
			XN_IS_STATUS_OK(nRetVal);

			list.ReplaceUnderlyingObject(pList);

			return (XN_STATUS_OK);
		}

		/**
		 * @brief Returns a list of the context's existing created nodes filtered by type.
		 *
		 * @param [in]	list	List of the context's existing created nodes.
		 * @param [in]	type	Type to enumerate for.
		 *
		 * <B>Remarks</B>
		 *
		 * Returns the list after filtering them by type, so it returns a list of the created
		 * nodes of the specific type only.
		 *
		 * An example of a typical case of returning a list of two nodes of the same type
		 * is where there are two sensors in a 3D set-up and you need to retrieve the corresponding
		 * two depth nodes to generate data from them.
		 *
		 * You can use the @ref xn::NodeInfoList::FilterList() method to filter the returned
		 * list of existing nodes.
		 */
		XnStatus EnumerateExistingNodes(NodeInfoList& list, XnProductionNodeType type) const
		{
			XnNodeInfoList* pList;
			XnStatus nRetVal = xnEnumerateExistingNodesByType(m_pContext, type, &pList);
			XN_IS_STATUS_OK(nRetVal);

			list.ReplaceUnderlyingObject(pList);

			return (XN_STATUS_OK);
		}

		/**
		 * @brief Searches for an existing created node of a specified type and returns a reference to it.
		 *
		 * @param [in]	type	Type of node to search for.
		 * @param [out]	node	Reference to an existing created node that was found.
		 *
		 * <B>Remarks</B>
		 *
		 * This method returns only the first node it finds; this method does not return
		 * the entire list of matching nodes. Compare this with the @ref xn::Context::EnumerateExistingNodes()
		 * method, which returns the entire list of matching nodes.
		 * A typical usage is that the application already created all the nodes from XML.
		 * Then, when it needs to start generating data from a particular node, e.g., the
		 * skeleton data, it will call FindExistingNode specifying the skeleton type, and
		 * it will get it and start using it.
		 *
		 * This method does not provide a query parameter (compare with @ref xn::Context::EnumerateExistingNodes).
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

		/**
		 * @copybrief xnGetNodeHandleByName
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

		/**
		 * @copybrief xnGetNodeHandleByName
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

		/**
		 * @brief Ensures all created @ref dict_gen_node "generator nodes" are generating data.
		 *
		 */
		inline XnStatus StartGeneratingAll()
		{
			return xnStartGeneratingAll(m_pContext);
		}

		/**
		 * @brief Ensures all @ref dict_gen_node "generator nodes" are not generating data.
		 */
		inline XnStatus StopGeneratingAll()
		{
			return xnStopGeneratingAll(m_pContext);
		}

		/**
		 * @brief Enables/disables the GlobalMirror flag.
		 *
		 * @param [in]	bMirror New mirroring state.
		 *
		 * <B>Remarks:</B>
		 *
		 * See @ref conc_global_mirror for understanding of the 'Global Mirror' concept.
		 */
		inline XnStatus SetGlobalMirror(XnBool bMirror)
		{
			return xnSetGlobalMirror(m_pContext, bMirror);
		}

		/**
		 * @brief  Gets the current state of the GlobalMirror flag.
		 *
		 */
		inline XnBool GetGlobalMirror()
		{
			return xnGetGlobalMirror(m_pContext);
		}

		/**
		 * @copybrief xnGetGlobalErrorState
		 * For full details and usage, see @ref xnGetGlobalErrorState
		 */
		inline XnStatus GetGlobalErrorState()
		{
			return xnGetGlobalErrorState(m_pContext);
		}

		/**
		 * @brief Registers an event handler for the 'Global Error State Change' event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToErrorStateChange(XnErrorStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return xnRegisterToGlobalErrorStateChange(m_pContext, handler, pCookie, &hCallback);
		}

		/**
		 * @brief Unregisters an event handler for the 'Global Error State Change' event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events .
		 */
		inline void UnregisterFromErrorStateChange(XnCallbackHandle hCallback)
		{
			xnUnregisterFromGlobalErrorStateChange(m_pContext, hCallback);
		}

		/**
		 * @brief Registers an event handler for the 'Node Creation' event. see @ref context_node_creation_event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToNodeCreation(NodeCreationHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			NodeCreationCookie* pCreationCookie;
			XN_VALIDATE_ALLOC(pCreationCookie, NodeCreationCookie);
			pCreationCookie->pFunc = handler;
			pCreationCookie->pCookie = pCookie;
			
			nRetVal = xnRegisterToNodeCreation(m_pContext, NodeCreationCallback, pCreationCookie, &pCreationCookie->hUnderlyingCallback);
			XN_IS_STATUS_OK(nRetVal);

			hCallback = pCreationCookie;

			return XN_STATUS_OK;
		}

		/**
		 * @brief Unregisters an event handler for the 'Node Creation' event. see @ref context_node_creation_event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events.
		 */
		inline void UnregisterFromNodeCreation(XnCallbackHandle hCallback)
		{
			NodeCreationCookie* pCreationCookie = (NodeCreationCookie*)hCallback;
			xnUnregisterFromNodeCreation(m_pContext, pCreationCookie->hUnderlyingCallback);
			xnOSFree(pCreationCookie);
		}

		/**
		 * @brief Registers an event handler for the 'Node Destruction' event. see @ref context_node_destruction_event.
		 *
		 * @param [in]	handler  	Callback function to be invoked when the event is raised.
		 * @param [in]	pCookie  	User's cookie, to be delivered to the callback.
		 * @param [out]	hCallback 	Handle to the callback to be used for unregistering it.
		 *
		 * For full details and usage of the parameters, see @ref reg_to_events.
		 */
		inline XnStatus RegisterToNodeDestruction(NodeDestructionHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			XnStatus nRetVal = XN_STATUS_OK;
			
			NodeDestructionCookie* pDestructionCookie;
			XN_VALIDATE_ALLOC(pDestructionCookie, NodeDestructionCookie);
			pDestructionCookie->pFunc = handler;
			pDestructionCookie->pCookie = pCookie;

			nRetVal = xnRegisterToNodeDestruction(m_pContext, NodeDestructionCallback, pDestructionCookie, &pDestructionCookie->hUnderlyingCallback);
			XN_IS_STATUS_OK(nRetVal);

			hCallback = pDestructionCookie;

			return XN_STATUS_OK;
		}

		/**
		 * @brief Unregisters an event handler for the 'Node Destruction' event. see @ref context_node_destruction_event.
		 *
		 * @param [in]	hCallback  Handle received from registration.
		 *
		 * For full details and usage of the parameter, see @ref unreg_from_events.
		 */
		inline void UnregisterFromNodeDestruction(XnCallbackHandle hCallback)
		{
			NodeDestructionCookie* pDestructionCookie = (NodeDestructionCookie*)hCallback;
			xnUnregisterFromNodeDestruction(m_pContext, pDestructionCookie->hUnderlyingCallback);
			xnOSFree(pDestructionCookie);
		}

		/**
		 * @brief Updates all generator nodes in the context to their latest available data,
		 * first waiting for all nodes to have new data available.
		 *
		 * <b>Remarks</b>
		 *
		 * This method requests from OpenNI to cause all nodes in the context's Production
		 * Graph that have new data available to update their application buffers with the
		 * new data, each node updating its own application buffer.
		 *
		 * Before OpenNI causes the data updates, it waits until @b all the nodes have notified
		 * that they have new data available. On receiving all the 'new data available'
		 * notifications, OpenNI then causes all nodes with new data available to update
		 * their application buffers with their new data. At this stage the generator nodes
		 * have "generated" new data. This method then stops waiting and returns a success
		 * status. The application can now read the newly generated data.
		 *
		 * @note Using this method is not recommended for most applications since it requires
		 * <i>all</i> the nodes to have new data. Consider instead using @ref WaitAnyUpdateAll()
		 * or @ref WaitOneUpdateAll(). Typical applications use the WaitAnyUpdateAll()
		 * method.
		 *
		 * An error situation is defined as: after a preset timeout, not all nodes have
		 * yet notified OpenNI they have new data available. On error, the method stops
		 * waiting and returns an error status. None of the nodes update their application
		 * buffers.
		 *
		 * See @ref conc_updating_data__summary_of_wait_fns for an overview to the @ref
		 * conc_updating_data "'WaitXUpdateAll'" methods and how to read the data from the
		 * nodes.
		 *
		 * @retval XN_STATUS_WAIT_DATA_TIMEOUT No new data available within 2 seconds.
		 */
		inline XnStatus WaitAndUpdateAll()
		{
			return xnWaitAndUpdateAll(m_pContext);
		}

		/**
		 * @brief Updates all generator nodes in the context to their latest available data,
		 * first waiting for any of the nodes to have new data available.
		 *
		 * @note Typical applications use this method. Consider using this method for your
		 * application.
		 *
		 * <b>Remarks</b>
		 *
		 * This method requests from OpenNI to cause all nodes with new data available in
		 * the context's Production Graph to update their application buffers with the new
		 * data, each node updating its own application buffer.
		 *
		 * Before OpenNI causes the data updates, it waits for any of the nodes to have
		 * notified that it has new data available. On receiving a 'new data available'
		 * notification, OpenNI then causes all nodes with new data available to update
		 * their application buffers with their new data. At this stage the generator nodes
		 * have "generated" new data. This method then stops waiting and returns a success
		 * status. The application can now read the newly generated data.
		 *
		 * A node that does not have new data available does not update its application
		 * buffer.
		 *
		 * An error situation is defined as: after a preset timeout, none of the nodes have
		 * yet notified OpenNI they have new data available. On error, the method stops
		 * waiting and returns an error status. None of the nodes update their application
		 * buffers.
		 *
		 * See @ref conc_updating_data__summary_of_wait_fns for an overview to the @ref
		 * conc_updating_data "'WaitXUpdateAll'" methods and how to read the data from the
		 * nodes.
		 *
		 * @retval XN_STATUS_WAIT_DATA_TIMEOUT No new data available within 2 seconds.
		 */
		inline XnStatus WaitAnyUpdateAll()
		{
			return xnWaitAnyUpdateAll(m_pContext);
		}

		/**
		 * @brief Updates all generator nodes in the context to their latest available data, first waiting for a specified node to have new data available.
		 *
		 * <b>Remarks</b>
		 *
		 * This method requests from OpenNI to cause all nodes in the context's Production
		 * Graph with new data available to update their application buffers with the new
		 * data, each node updating its own application buffer.
		 *
		 * Before OpenNI causes the data updates, it waits for a specified node to notify
		 * that it has new data available. On receiving the 'new data available' notification,
		 * OpenNI then causes all nodes with new data available to update their application
		 * buffers with their new data. At this stage the generator nodes have "generated"
		 * new data. This method then stops waiting and returns a success status. The application
		 * can now read the newly generated data.
		 *
		 * A node that does not have new data available does not update its application
		 * buffer.
		 *
		 * An error situation is defined as: after a preset timeout, none of the nodes have
		 * yet notified OpenNI they have has new data available. On error, the method stops
		 * waiting and returns an error status. None of the nodes update their application
		 * buffers.
		 *
		 * See @ref conc_updating_data__summary_of_wait_fns for an overview to the @ref
		 * conc_updating_data "'WaitXUpdateAll'" methods and how to read the data from the
		 * nodes.
		 *
		 * @retval XN_STATUS_WAIT_DATA_TIMEOUT No new data available within 2 seconds.
		 */
		inline XnStatus WaitOneUpdateAll(ProductionNode& node)
		{
			return xnWaitOneUpdateAll(m_pContext, node.GetHandle());
		}

		/**
		 * @brief Immediately updates all generator nodes in the context to their latest available data, without waiting for notification that any node has new data available.
		 *
		 * <b>Remarks</b>
		 *
		 * This method requests from OpenNI to cause all nodes in the context's Production
		 * Graph with new data available to update their application buffers with the new
		 * data, each node updating its own application buffer.
		 *
		 * OpenNI causes all nodes with new data available to update their application buffers
		 * with their new data. At this stage the generator nodes have "generated" new data.
		 * This method returns a success status. The application can now read the newly
		 * generated data.
		 *
		 * A node that does not have new data available does not update its application
		 * buffer.
		 *
		 * See @ref conc_updating_data__summary_of_wait_fns for an overview to the @ref
		 * conc_updating_data "'WaitXUpdateAll'" methods and how to read the data from the
		 * nodes.
		 */
		inline XnStatus WaitNoneUpdateAll()
		{
			return xnWaitNoneUpdateAll(m_pContext);
		}

		/**
		 * @copybrief xnAutoEnumerateOverSingleInput
		 * For full details and usage, see @ref xnAutoEnumerateOverSingleInput
		 */
		inline XnStatus AutoEnumerateOverSingleInput(NodeInfoList& List, XnProductionNodeDescription& description, const XnChar* strCreationInfo, XnProductionNodeType InputType, EnumerationErrors* pErrors, Query* pQuery = NULL) const
		{
			return xnAutoEnumerateOverSingleInput(m_pContext, List.GetUnderlyingObject(), &description, strCreationInfo, InputType, pErrors == NULL ? NULL : pErrors->GetUnderlying(), pQuery == NULL ? NULL : pQuery->GetUnderlyingObject());
		}

		/**
		 * Replaces the underlying C object pointed to by this object.
		 */
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
				XN_REFERENCE_VARIABLE(nRetVal);

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

		/** @cond */
		inline XnStatus XN_API_DEPRECATED("Use other overload!") RunXmlScript(const XnChar* strScript, EnumerationErrors* pErrors = NULL)
		{
			m_bUsingDeprecatedAPI = TRUE;
			#pragma warning (push)
			#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			return xnContextRunXmlScript(m_pContext, strScript, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			#pragma warning (pop)
		}

		inline XnStatus XN_API_DEPRECATED("Use other overload!") RunXmlScriptFromFile(const XnChar* strFileName, EnumerationErrors* pErrors = NULL)
		{
			m_bUsingDeprecatedAPI = TRUE;
			#pragma warning (push)
			#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			return xnContextRunXmlScriptFromFile(m_pContext, strFileName, pErrors == NULL ? NULL : pErrors->GetUnderlying());
			#pragma warning (pop)
		}

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

		inline XnStatus XN_API_DEPRECATED("Use other overload!") OpenFileRecording(const XnChar* strFileName)
		{
			m_bUsingDeprecatedAPI = TRUE;
			#pragma warning (push)
			#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)
			return xnContextOpenFileRecording(m_pContext, strFileName);
			#pragma warning (pop)
		}

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

		XnStatus XN_API_DEPRECATED("Please use other overload") CreateProductionTree(NodeInfo& Tree)
		{
			XnStatus nRetVal = XN_STATUS_OK;

			XnNodeHandle hNode;
			nRetVal = xnCreateProductionTree(m_pContext, Tree, &hNode);
			XN_IS_STATUS_OK(nRetVal);

			Tree.m_bOwnerOfNode = TRUE;

			return (XN_STATUS_OK);
		}
		/** @endcond */

	private:
		typedef struct NodeCreationCookie
		{
			NodeCreationHandler pFunc;
			void* pCookie;
			XnCallbackHandle hUnderlyingCallback;
		} NodeCreationCookie;

		typedef struct NodeDestructionCookie
		{
			NodeDestructionHandler pFunc;
			void* pCookie;
			XnCallbackHandle hUnderlyingCallback;
		} NodeDestructionCookie;

		static void XN_CALLBACK_TYPE NodeCreationCallback(XnContext* pContext, XnNodeHandle hCreatedNode, void* pCookie)
		{
			NodeCreationCookie* pNodeCreationCookie = (NodeCreationCookie*)pCookie;
			Context context(pContext);
			ProductionNode createdNode(hCreatedNode);
			pNodeCreationCookie->pFunc(context, createdNode, pNodeCreationCookie->pCookie);
		}

		static void XN_CALLBACK_TYPE NodeDestructionCallback(XnContext* pContext, const XnChar* strDestroyedNodeName, void* pCookie)
		{
			NodeDestructionCookie* pNodeCreationCookie = (NodeDestructionCookie*)pCookie;
			Context context(pContext);
			pNodeCreationCookie->pFunc(context, strDestroyedNodeName, pNodeCreationCookie->pCookie);
		}

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
	 * @ingroup cppref_misc
	 * A utility class for resolution info
	 */
	class Resolution
	{
	public:
		/**
		 * Creates a @ref Resolution object.
		 *
		 * @param [in]	res	Resolution
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
		 * @param [in]	xRes	X resolution
		 * @param [in]	yRes	Y resolution
		 */
		inline Resolution(XnUInt32 xRes, XnUInt32 yRes) : m_nXRes(xRes), m_nYRes(yRes)
		{
			m_Res = xnResolutionGetFromXYRes(xRes, yRes);
			m_strName = xnResolutionGetName(m_Res);
		}

		/**
		 * Creates a @ref Resolution object out of its name
		 *
		 * @param [in]	strName	name of the resolution
		 */
		inline Resolution(const XnChar* strName)
		{
			m_Res = xnResolutionGetFromName(strName);
			m_nXRes = xnResolutionGetXRes(m_Res);
			m_nYRes = xnResolutionGetYRes(m_Res);
			m_strName = xnResolutionGetName(m_Res);
		}

		/**
		 * Gets the resolution
		 */
		inline XnResolution GetResolution() const { return m_Res; }
		/**
		 * Gets the X resolution
		 */
		inline XnUInt32 GetXResolution() const { return m_nXRes; }
		/**
		 * Gets the Y resolution
		 */
		inline XnUInt32 GetYResolution() const { return m_nYRes; }
		/**
		 * Gets the name of the resolution
		 */
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

	inline Context ProductionNode::GetContext() const
	{
		XnContext* pContext = xnGetRefContextFromNodeHandle(GetHandle());
		Context result(pContext);
		xnContextRelease(pContext);
		return result;
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

	inline void XN_API_DEPRECATED("Use xn::Version::Current() instead") GetVersion(XnVersion& Version)
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
};

#endif // __XN_CPP_WRAPPER_H__
