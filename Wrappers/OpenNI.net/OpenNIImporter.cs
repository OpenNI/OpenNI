﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using XnBool = System.Boolean;
using XnCallbackHandle = System.IntPtr;
using XnCodecID = System.UInt32;
using XnConfidence = System.Single;
using XnContext = System.IntPtr;
using XnDepthPixel = System.UInt16;
using XnDouble = System.Double;
using XnEnumerationErrors = System.IntPtr;
using XnEnumerationErrorsIterator = System.IntPtr;
using XnFloat = System.Single;
using XnInt32 = System.Int32;
using XnInt64 = System.Int64;
using XnLockHandle = System.UInt32;
using XnNodeHandle = System.IntPtr;
using XnNodeInfo = System.IntPtr;
using XnNodeInfoList = System.IntPtr;
using XnNodeInfoListIterator = System.IntPtr;
using XnNodeQuery = System.IntPtr;
using XnStatus = System.UInt32;
using XnUInt = System.IntPtr;
using XnUInt16 = System.UInt16;
using XnUInt32 = System.UInt32;
using XnUInt64 = System.UInt64;
using XnUInt8 = System.Byte;
using XnUserID = System.UInt32;

namespace xn
{
	public enum NodeType
	{
		/** A device node **/
		Device = 1,

		/** A depth generator **/
		Depth = 2,
		
		/** An image generator **/
		Image = 3,

		/** An audio generator **/
		Audio = 4,
		
		/** An IR generator **/
		IR = 5,

		/** A user generator **/
		User = 6,
		
		/** A recorder **/
		Recorder = 7,
		
		/** A player **/
		Player = 8,
		
		/** A gesture generator **/
		Gesture = 9,
		
		/** A scene analyzer **/
		Scene = 10,
		
		/** A hands generator **/
		Hands = 11,

		/** A Codec **/
		Codec = 12,
	}

	public enum LogSeverity
	{
		Verbose = 0,
		Info = 1,
		Warning = 2,
		Error = 3
	};

	public enum PixelFormat
	{
		RGB24 = 1,
		YUV422 = 2,
		Grayscale8Bit = 3,
		Grayscale16Bit = 4,
	};

	public enum Resolution
	{
		CUSTOM = 0,
		QQVGA = 1,
		CGA = 2,
		QVGA = 3,
		VGA = 4,
		SVGA = 5,
		XGA = 6,
		P720 = 7,
		SXGA = 8,
		UXGA = 9,
		P1080 = 10,
	};

	public enum SkeletonJoint
	{
		Head			= 1,
		Neck			= 2,
		Torso			= 3,
		Waist			= 4,

		LeftCollar		= 5,
		LeftShoulder	= 6,
		LeftElbow		= 7,
		LeftWrist		= 8,
		LeftHand		= 9,
		LeftFingertip	=10,

		RightCollar		=11,
		RightShoulder	=12,
		RightElbow		=13,
		RightWrist		=14,
		RightHand		=15,
		RightFingertip	=16,

		LeftHip			=17,
		LeftKnee		=18,
		LeftAnkle		=19,
		LeftFoot		=20,

		RightHip		=21,
		RightKnee		=22,
		RightAnkle		=23,
		RightFoot		=24	
	};

	/** A configuration of joints to be used by skeleton (see @ref xnSetSkeletonProfile) **/
	public enum SkeletonProfile
	{
		/** No joints at all **/
		None		= 1,

		/** All joints **/
		All			= 2,
		
		/** All the joints in the upper body (torso and upwards) **/
		Upper		= 3,
		
		/** All the joints in the lower body (torso and downwards) **/
		Lower		= 4,
		
		/** The head and the hands **/
		HeadAndHands	= 5,
	};

	public enum RecordMedium
	{
		/** Recording medium is a file **/
		File = 0,
	};

	public enum PlayerSeekOrigin
	{
		Set = 0,
		Curr = 1,
		End = 2,
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct Version
	{
		public XnUInt8 nMajor;
		public XnUInt8 nMinor;
		public XnUInt16 nMaintenance;
		public XnUInt32 nBuild;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct Cropping
	{
		/** TRUE if cropping is turned on, FALSE otherwise. */
		public XnBool bEnabled;
		/** Offset in the X-axis, in pixels. */
		public XnUInt16 nXOffset;
		/** Offset in the Y-axis, in pixels. */
		public XnUInt16 nYOffset;
		/** Number of pixels in the X-axis. */
		public XnUInt16 nXSize;
		/** Number of pixels in the Y-axis. */
		public XnUInt16 nYSize;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct MapOutputMode
	{
		/** Number of elements in the X-axis. */
		public XnUInt32 nXRes;
		/** Number of elements in the Y-axis. */
		public XnUInt32 nYRes;
		/** Number of frames per second. */
		public XnUInt32 nFPS;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct FieldOfView
	{
		/** Horizontal Field Of View, in radians. */
		public XnDouble fHFOV;
		/** Horizontal Field Of View, in radians. */
		public XnDouble fVFOV;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct Point3D
	{
		public Point3D(XnFloat x, XnFloat y, XnFloat z)
		{
			this.X = x;
			this.Y = y;
			this.Z = z;
		}

		public XnFloat X;
		public XnFloat Y;
		public XnFloat Z;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct BoundingBox3D
	{
		public Point3D LeftBottomNear;
		public Point3D RightTopFar;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct RGB24Pixel
	{
		public XnUInt8 nRed;
		public XnUInt8 nGreen;
		public XnUInt8 nBlue;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct YUV422DoublePixel
	{
		public XnUInt8 nU;
		public XnUInt8 nY1;
		public XnUInt8 nV;
		public XnUInt8 nY2;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct Plane3D
	{
		public Plane3D(Point3D normal, Point3D point)
		{
			this.vNormal = normal;
			this.ptPoint = point;
		}

		/** The normal of the plane. **/
		public Point3D vNormal;

		/** A point on the plane. **/
		public Point3D ptPoint;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct WaveOutputMode
	{
		public XnUInt32 nSampleRate;
		public XnUInt16 nBitsPerSample;
		public XnUInt8 nChannels;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct SkeletonJointPosition
	{
		/** The actual position in real world coordinations */
		public Point3D position;

		/** The confidence in the position */
		public XnConfidence fConfidence;
	};

	[StructLayout(LayoutKind.Sequential)]
	public class Matrix3X3
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 9)]
		public readonly float[] elements = new float[9];
	};

	[StructLayout(LayoutKind.Sequential)]
	public class SkeletonJointOrientation
	{
		/** The actual orientation */
		[MarshalAs(UnmanagedType.Struct)]
		public Matrix3X3 Orientation = new Matrix3X3();
		/** The confidence in the orientation */
		public XnConfidence Confidence;
	};

	[StructLayout(LayoutKind.Sequential)]
	public class SkeletonJointTransformation
	{
		/** The position of the joint */
		public SkeletonJointPosition position;
		/** The orientation of the joint */
		public SkeletonJointOrientation orientation = new SkeletonJointOrientation();
	};

	/** A pair of 32-bit unsigned values. **/
	[StructLayout(LayoutKind.Sequential)]
	public struct UInt32XYPair
	{
		public XnUInt32 X;
		public XnUInt32 Y;
	};

	internal class Defs
	{
		public const int XN_MAX_NAME_LENGTH = 80;
		public const int XN_MAX_LICENSE_LENGTH = 255;
	}
	
	[StructLayout(LayoutKind.Sequential)]
	public class ProductionNodeDescription
	{
		/** Type of the production node. */
		public NodeType Type;
		/** Vendor name */
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_NAME_LENGTH)]
		public string strVendor;
		/** Generator name. Should be unique under Type and Vendor. */
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_NAME_LENGTH)]
		public string strName;
		/** Current version */
		Version Version;
	};

	[StructLayout(LayoutKind.Sequential)]
	public class License
	{
		/** Vendor name **/
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_NAME_LENGTH)]
		public string strVendor;
		/** License key **/
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_LICENSE_LENGTH)]
		public string strKey;
	};

    [SuppressUnmanagedCodeSecurity]
	internal class OpenNIImporter
	{
		/** Holds information about a general frame of data. **/
		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		public class XnOutputMetaData
		{
			/** Represents the time in which the data was received. **/
			public XnUInt64 nTimestamp;

			/** The frame ID, or frame number, of this frame. **/
			public XnUInt32 nFrameID;
			
			/** The size of the data. **/
			public XnUInt32 nDataSize;

			/** Specifies whether the generator updated this data on the last call to one of the XnWaitXXXUpdateXXX() functions. **/
			public XnBool bIsNew;
		};

		/** Holds information about a frame of data that has a map format. **/
		[StructLayout(LayoutKind.Sequential, Pack=1)]
		public class XnMapMetaData
		{
			/** A pointer to general information about this frame. **/
			public IntPtr pOutput;

			/** The resolution of this frame, including any cropping that was set. **/
			public UInt32XYPair Res;
			
			/** The offset of the cropped region within this frame. Set to (0,0) if no cropping was done. **/
			public UInt32XYPair Offset;
			
			/** The full resolution of this frame, disregarding cropping. **/
			public UInt32XYPair FullRes;
			
			/** The pixel format of this frame. **/
			public PixelFormat PixelFormat;
			
			/** The number of frames per second that was set when this frame was received. **/
			public XnUInt32 nFPS;
		};

		/** Holds information about a frame of depth. **/
		[StructLayout(LayoutKind.Sequential)]
		public class XnDepthMetaData
		{
			/** A pointer to the map meta data of this frame. **/
			public IntPtr pMap;
			
			/** A pointer to the depth data of this frame. **/
			public IntPtr pData;
			
			/** The value of the Z resolution of this frame - the maximum depth a pixel can have. **/
			public XnDepthPixel nZRes;
		};

		/** Holds information about an image frame. **/
		[StructLayout(LayoutKind.Sequential)]
		public class XnImageMetaData
		{
			/** A pointer to the map meta data of this frame **/
			public IntPtr pMap;
			
			/** A pointer to the image data of this frame **/
			public IntPtr pData;
		};

		/** Holds information about an IR frame. **/
		[StructLayout(LayoutKind.Sequential)]
		public class XnIRMetaData
		{
			/** A pointer to the map meta data of this frame **/
			public IntPtr pMap;
			
			/** A pointer to the IR data of this frame **/
			public IntPtr pData;
		};

		[StructLayout(LayoutKind.Sequential)]
		public class XnAudioMetaData
		{
			/** A pointer to general information about this chunk of audio. **/
			public IntPtr pOutput;

			/** The wave output mode of this cunk of audio. **/
			public WaveOutputMode Wave;
			
			/** A pointer to audio data of this cunk. **/
			public IntPtr pData;
		};

		[StructLayout(LayoutKind.Sequential)]
		public class XnSceneMetaData
		{
			/** A pointer to the map meta data of this frame. **/
			public IntPtr pMap;

			/** A pointer to the scene data of this frame. **/
			public IntPtr pData;
		};

		public delegate void XnErrorStateChangedHandler(XnStatus errorState, IntPtr pCookie);
		public delegate void XnStateChangedHandler(XnNodeHandle hNode, IntPtr pCookie);
		public delegate void XnGestureRecognized(XnNodeHandle hNode, string strGesture, ref Point3D pIDPosition, ref Point3D pEndPosition, IntPtr pCookie);
		public delegate void XnGestureProgress(XnNodeHandle hNode, string strGesture, ref Point3D pPosition, XnFloat fProgress, IntPtr pCookie);
		public delegate void XnCalibrationStart(XnNodeHandle hNode, XnUserID user, IntPtr pCookie);
		public delegate void XnCalibrationEnd(XnNodeHandle hNode, XnUserID user, XnBool bSuccess, IntPtr pCookie);
		public delegate void XnPoseDetectionCallback(XnNodeHandle hNode, string strPose, XnUserID user, IntPtr pCookie);

		public delegate void XnUserHandler(XnNodeHandle hNode, XnUserID user, IntPtr pCookie);
		public delegate void XnHandCreate(XnNodeHandle hNode, XnUserID user, ref Point3D pPosition, XnFloat fTime, IntPtr pCookie);
		public delegate void XnHandUpdate(XnNodeHandle hNode, XnUserID user, ref Point3D pPosition, XnFloat fTime, IntPtr pCookie);
		public delegate void XnHandDestroy(XnNodeHandle hNode, XnUserID user, XnFloat fTime, IntPtr pCookie);

		[DllImport("OpenNI")]
		public static extern XnStatus xnInit(out XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnContextRunXmlScriptFromFile(XnContext pContext, string strFileName, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnContextRunXmlScript(XnContext pContext, string xmlScript, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnInitFromXmlFile(string strFileName, out XnContext ppContext, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnContextOpenFileRecording(XnContext pContext, string strFileName);
		[DllImport("OpenNI")]
		public static extern void xnShutdown(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateProductionTrees(XnContext pContext, NodeType Type, XnNodeQuery pQuery, out XnNodeInfoList ppTreesList, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateProductionTree(XnContext pContext, XnNodeInfo pTree, out XnNodeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateAnyProductionTree(XnContext pContext, NodeType type, XnNodeQuery pQuery, out XnNodeHandle phNode, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateMockNode(XnContext pContext, NodeType type, string strName, out XnNodeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateMockNodeBasedOn(XnContext pContext, XnNodeHandle hOriginalNode, string strName, out XnNodeHandle phMockNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnProductionNodeAddRef(XnNodeHandle hNode);
		[DllImport("OpenNI")]
		public static extern void xnProductionNodeRelease(XnNodeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateExistingNodes(XnContext pContext, out XnNodeInfoList ppList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateExistingNodesByType(XnContext pContext, NodeType type, out XnNodeInfoList ppList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnFindExistingNodeByType(XnContext pContext, NodeType type, out XnNodeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetNodeHandleByName(XnContext pContext, string strInstanceName, out XnNodeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitAndUpdateAll(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitOneUpdateAll(XnContext pContext, XnNodeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitAnyUpdateAll(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitNoneUpdateAll(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartGeneratingAll(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopGeneratingAll(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetGlobalMirror(XnContext pContext, XnBool bMirror);
		[DllImport("OpenNI")]
		public static extern XnBool xnGetGlobalMirror(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetGlobalErrorState(XnContext pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToGlobalErrorStateChange(XnContext pContext, [MarshalAs(UnmanagedType.FunctionPtr)] XnErrorStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromGlobalErrorStateChange(XnContext pContext, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerationErrorsAllocate(out XnEnumerationErrors ppErrors);
		[DllImport("OpenNI")]
		public static extern void xnEnumerationErrorsFree(XnEnumerationErrors pErrors);
//		[DllImport("OpenNI")]
//		public static extern XnStatus xnEnumerationErrorsAdd(XnEnumerationErrors pErrors, ProductionNodeDescription pDesc, XnStatus nError);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerationErrorsToString(XnEnumerationErrors pErrors, StringBuilder csBuffer, XnUInt32 nSize);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerationErrorsClear(XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnEnumerationErrorsIterator xnEnumerationErrorsGetFirst(XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnEnumerationErrorsIterator xnEnumerationErrorsGetNext(XnEnumerationErrorsIterator it);
		[DllImport("OpenNI")]
		public static extern XnBool xnEnumerationErrorsIteratorIsValid(XnEnumerationErrorsIterator it);
		[DllImport("OpenNI")]
		public static extern ProductionNodeDescription xnEnumerationErrorsGetCurrentDescription(XnEnumerationErrorsIterator it);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerationErrorsGetCurrentError(XnEnumerationErrorsIterator it);

/*		[DllImport("OpenNI")]
		public static extern XnStatus xnFPSInit(XnFPSData pFPS, XnUInt32 nSamplesCount);
		[DllImport("OpenNI")]
		public static extern XnStatus xnFPSMarkFrame(XnFPSData pFPS, XnUInt64 nNow);
		[DllImport("OpenNI")]
		public static extern XnDouble xnFPSCalc(XnFPSData pFPS, XnUInt32 nAverageOver, XnUInt64 nNow);
		[DllImport("OpenNI")]
		public static extern XnStatus xnFPSFree(XnFPSData pFPS);
*/
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddLicense(XnContext pContext, License pLicense);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateLicenses(XnContext pContext, out IntPtr paLicenses, out XnUInt32 pnCount);
		[DllImport("OpenNI")]
		public static extern void xnFreeLicensesList(IntPtr aLicenses);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogInitSystem();
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogInitFromINIFile(string csINIFile, string csSectionName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogInitFromXmlFile(string strFileName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogStartNewFile();
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogClose();
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogSetMaskState(string csMask, XnBool bEnabled);
		[DllImport("OpenNI")]
		public static extern XnStatus xnDumpSetMaskState(string csMask, XnBool bEnabled);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogSetSeverityFilter(LogSeverity nMinSeverity);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogSetConsoleOutput(XnBool bConsoleOutput);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogSetFileOutput(XnBool bFileOutput);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogSetLineInfo(XnBool bLineInfo);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLogSetOutputFolder(string strOutputFolder);
		[DllImport("OpenNI")]
		public static extern XnBool xnLogIsEnabled(string csLogMask, LogSeverity nSeverity);
		[DllImport("OpenNI")]
		public static extern void xnLogWrite(string csLogMask, LogSeverity nSeverity, string csFile, XnUInt32 nLine, string csFormat);
		[DllImport("OpenNI")]
		public static extern void xnLogWriteNoEntry(string csLogMask, LogSeverity nSeverity, string csFormat);
		[DllImport("OpenNI")]
		public static extern void xnLogWriteBinaryData(string csLogMask, LogSeverity nSeverity, string csFile, XnUInt32 nLine, IntPtr pBinData, XnUInt32 nDataSize, string csFormat);
		[DllImport("OpenNI")]
		public static extern XnBool xnLogIsDumpMaskEnabled(string csDumpMask);
/*		[DllImport("OpenNI")]
		public static extern void xnDumpInit(XnDump pDump, string csDumpMask, string csHeader, string csFileNameFormat);
		[DllImport("OpenNI")]
		public static extern void xnDumpForceInit(XnDump pDump, string csHeader, string csFileNameFormat);
		[DllImport("OpenNI")]
		public static extern void xnDumpClose(XnDump pDump);
		[DllImport("OpenNI")]
		public static extern void xnDumpWriteBufferImpl(XnDump dump, IntPtr pBuffer, XnUInt32 nBufferSize);
		[DllImport("OpenNI")]
		public static extern void xnDumpWriteStringImpl(XnDump dump, string csFormat);
		[DllImport("OpenNI")]
		public static extern void xnDumpFlush(XnDump dump);
*/
		[DllImport("OpenNI")]
		public static extern XnNodeInfo xnGetNodeInfo(XnNodeHandle hNode);
		[DllImport("OpenNI")]
		public static extern string xnGetNodeName(XnNodeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnContext xnGetContextFromNodeHandle(XnNodeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsCapabilitySupported(XnNodeHandle hInstance, string strCapabilityName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetIntProperty(XnNodeHandle hInstance, string strName, XnUInt64 nValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetRealProperty(XnNodeHandle hInstance, string strName, XnDouble dValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetStringProperty(XnNodeHandle hInstance, string strName, string strValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetGeneralProperty(XnNodeHandle hInstance, string strName, XnUInt32 nBufferSize, IntPtr pBuffer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetIntProperty(XnNodeHandle hInstance, string strName, out XnUInt64 pnValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetRealProperty(XnNodeHandle hInstance, string strName, out XnDouble pdValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetStringProperty(XnNodeHandle hInstance, string strName, StringBuilder csValue, XnUInt32 nBufSize);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetGeneralProperty(XnNodeHandle hInstance, string strName, XnUInt32 nBufferSize, IntPtr pBuffer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLockNodeForChanges(XnNodeHandle hInstance, out XnLockHandle phLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnUnlockNodeForChanges(XnNodeHandle hInstance, XnLockHandle hLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLockedNodeStartChanges(XnNodeHandle hInstance, XnLockHandle hLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLockedNodeEndChanges(XnNodeHandle hInstance, XnLockHandle hLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRemoveNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetNodeErrorState(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToNodeErrorStateChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromNodeErrorStateChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartGenerating(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsGenerating(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopGenerating(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToGenerationRunningChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromGenerationRunningChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToNewDataAvailable(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromNewDataAvailable(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsNewDataAvailable(XnNodeHandle hInstance, out XnUInt64 pnTimestamp);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitAndUpdateData(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsDataNew(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetDataSize(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt64 xnGetTimestamp(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetFrameID(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetMirror(XnNodeHandle hInstance, XnBool bMirror);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsMirrored(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToMirrorChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromMirrorChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsViewPointSupported(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetViewPoint(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnResetViewPoint(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsViewPointAs(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToViewPointChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromViewPointChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnCanFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsFrameSyncedWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToFrameSyncChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromFrameSyncChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetSupportedMapOutputModesCount(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSupportedMapOutputModes(XnNodeHandle hInstance, [In][Out] MapOutputMode[] aModes, ref XnUInt32 pnCount);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetMapOutputMode(XnNodeHandle hInstance, ref MapOutputMode pOutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetMapOutputMode(XnNodeHandle hInstance, ref MapOutputMode pOutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToMapOutputModeChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromMapOutputModeChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetCropping(XnNodeHandle hInstance, ref Cropping pCropping);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetCropping(XnNodeHandle hInstance, ref Cropping pCropping);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToCroppingChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromCroppingChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateDepthGenerator(XnContext pContext, out XnNodeHandle phDepthGenerator, XnNodeQuery pQuery,  XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnDepthPixel xnGetDeviceMaxDepth(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetDepthFieldOfView(XnNodeHandle hInstance, ref FieldOfView pFOV);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToDepthFieldOfViewChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromDepthFieldOfViewChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnConvertProjectiveToRealWorld(XnNodeHandle hInstance, XnUInt32 nCount, Point3D[] aProjective, [In][Out] Point3D[] aRealWorld);
		[DllImport("OpenNI")]
		public static extern XnStatus xnConvertRealWorldToProjective(XnNodeHandle hInstance, XnUInt32 nCount, Point3D[] aRealWorld, [In][Out] Point3D[] aProjective);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetDepthMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern void xnGetDepthMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetSupportedUserPositionsCount(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, ref BoundingBox3D pPosition);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, ref BoundingBox3D pPosition);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToUserPositionChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromUserPositionChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateImageGenerator(XnContext pContext, out XnNodeHandle phImageGenerator, XnNodeQuery pQuery,  XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetRGB24ImageMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetYUV422ImageMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetGrayscale8ImageMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetGrayscale16ImageMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetImageMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsPixelFormatSupported(XnNodeHandle hInstance, PixelFormat Format);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPixelFormat(XnNodeHandle hInstance, PixelFormat Format);
		[DllImport("OpenNI")]
		public static extern PixelFormat xnGetPixelFormat(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToPixelFormatChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromPixelFormatChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern void xnGetImageMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateIRGenerator(XnContext pContext, out XnNodeHandle phIRGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetIRMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern void xnGetIRMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateGestureGenerator(XnContext pContext, out XnNodeHandle phGestureGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddGesture(XnNodeHandle hInstance, string strGesture, ref BoundingBox3D pArea);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddGesture(XnNodeHandle hInstance, string strGesture, IntPtr pArea);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRemoveGesture(XnNodeHandle hInstance, string strGesture);
		[DllImport("OpenNI")]
        public static extern XnStatus xnGetAllActiveGestures(XnNodeHandle hInstance, [In] [Out] IntPtr[] pstrGestures, XnUInt32 nNameLength, ref XnUInt16 nGestures);
		[DllImport("OpenNI")]
        public static extern XnStatus xnEnumerateAllGestures(XnNodeHandle hInstance, [In] [Out] IntPtr[] pstrGestures, XnUInt32 nNameLength, ref XnUInt16 nGestures);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsGestureAvailable(XnNodeHandle hInstance, string strGesture);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsGestureProgressSupported(XnNodeHandle hInstance, string strGesture);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterGestureCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureRecognized RecognizedCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureProgress ProgressCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterGestureCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToGestureChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromGestureChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateSceneAnalyzer(XnContext pContext, out XnNodeHandle phSceneAnalyzer, XnNodeQuery pQuery,  XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetLabelMap(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetFloor(XnNodeHandle hInstance, ref Plane3D pPlane);
		[DllImport("OpenNI")]
		public static extern void xnGetSceneMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateUserGenerator(XnContext pContext, out XnNodeHandle phUserGenerator, XnNodeQuery pQuery,  XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnUInt16 xnGetNumberOfUsers(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
        public static extern XnStatus xnGetUsers(XnNodeHandle hInstance, [In][Out] XnUserID[] users, ref XnUInt16 pnUsers);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetUserCoM(XnNodeHandle hInstance, XnUserID user, out Point3D pCoM);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetUserPixels(XnNodeHandle hInstance, XnUserID user, IntPtr pScene);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterUserCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler NewUserCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler LostUserCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterUserCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsJointAvailable(XnNodeHandle hInstance, SkeletonJoint eJoint);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsProfileAvailable(XnNodeHandle hInstance, SkeletonProfile eProfile);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetSkeletonProfile(XnNodeHandle hInstance, SkeletonProfile eProfile);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetJointActive(XnNodeHandle hInstance, SkeletonJoint eJoint, XnBool bState);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsJointActive(XnNodeHandle hInstance, SkeletonJoint eJoint);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToJointConfigurationChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromJointConfigurationChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateActiveJoints(XnNodeHandle hInstance, SkeletonJoint pJoints, XnUInt16 pnJoints);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonJoint(XnNodeHandle hInstance, XnUserID user, SkeletonJoint eJoint, [Out] SkeletonJointTransformation pJoint);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonJointPosition(XnNodeHandle hInstance, XnUserID user, SkeletonJoint eJoint, ref SkeletonJointPosition pJoint);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonJointOrientation(XnNodeHandle hInstance, XnUserID user, SkeletonJoint eJoint, [Out] SkeletonJointOrientation pJoint);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonCalibrated(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonCalibrating(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRequestSkeletonCalibration(XnNodeHandle hInstance, XnUserID user, XnBool bForce);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAbortSkeletonCalibration(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSaveSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLoadSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnStatus xnClearSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnResetSkeleton(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnBool xnNeedPoseForSkeletonCalibration(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonCalibrationPose(XnNodeHandle hInstance, StringBuilder sb);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetSkeletonSmoothing(XnNodeHandle hInstance, XnFloat fFactor);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterCalibrationCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationStart CalibrationStartCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationEnd CalibrationEndCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterCalibrationCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetNumberOfPoses(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetAllAvailablePoses(XnNodeHandle hInstance, [In] [Out] IntPtr[] pstrPoses, XnUInt32 nNameLength, ref XnUInt32 pnPoses);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartPoseDetection(XnNodeHandle hInstance, string strPose, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopPoseDetection(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToPoseCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionStartCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionEndCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromPoseCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateHandsGenerator(XnContext pContext, out XnNodeHandle phHandsGenerator, XnNodeQuery pQuery,  XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterHandCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandCreate CreateCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandUpdate UpdateCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandDestroy DestroyCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterHandCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopTrackingAll(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartTracking(XnNodeHandle hInstance, ref Point3D pPosition);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetTrackingSmoothing(XnNodeHandle hInstance, XnFloat fFactor);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateAudioGenerator(XnContext pContext, out XnNodeHandle phAudioGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetAudioBuffer(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetSupportedWaveOutputModesCount(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSupportedWaveOutputModes(XnNodeHandle hInstance, [In][Out] WaveOutputMode[] aSupportedModes, ref XnUInt32 pnCount);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetWaveOutputMode(XnNodeHandle hInstance, ref WaveOutputMode OutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetWaveOutputMode(XnNodeHandle hInstance, ref WaveOutputMode OutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToWaveOutputModeChanges(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromWaveOutputModeChanges(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern void xnGetAudioMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockDepthSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockImageSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockIRSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockAudioSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateCodec(XnContext pContext, XnCodecID codecID, XnNodeHandle hInitializerNode, out XnNodeHandle phCodec);
		[DllImport("OpenNI")]
		public static extern XnCodecID xnGetCodecID(XnNodeHandle hCodec);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEncodeData(XnNodeHandle hCodec, IntPtr pSrc, XnUInt32 nSrcSize, IntPtr pDst, XnUInt32 nDstSize, [MarshalAs(UnmanagedType.SysUInt)] out UInt32 pnBytesWritten);
		[DllImport("OpenNI")]
		public static extern XnStatus xnDecodeData(XnNodeHandle hCodec, IntPtr pSrc, XnUInt32 nSrcSize, IntPtr pDst, XnUInt32 nDstSize, [MarshalAs(UnmanagedType.SysUInt)] out UInt32 pnBytesWritten);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateRecorder(XnContext pContext, string strFormatName, out XnNodeHandle phRecorder);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetRecorderDestination(XnNodeHandle hRecorder, RecordMedium destType, string strDest);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddNodeToRecording(XnNodeHandle hRecorder, XnNodeHandle hNode, XnCodecID compression);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRemoveNodeFromRecording(XnNodeHandle hRecorder, XnNodeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRecord(XnNodeHandle hRecorder);
		[DllImport("OpenNI")]
		public static extern string xnGetRecorderFormat(XnNodeHandle hRecorder);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreatePlayer(XnContext pContext, string strFormatName, out XnNodeHandle phPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPlayerRepeat(XnNodeHandle hPlayer, XnBool bRepeat);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPlayerSource(XnNodeHandle hPlayer, RecordMedium sourceType, string strSource);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetPlayerSource(XnNodeHandle hPlayer, out RecordMedium pSourceType, StringBuilder strSource, XnUInt32 nBufSize);
		[DllImport("OpenNI")]
		public static extern XnStatus xnPlayerReadNext(XnNodeHandle hPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSeekPlayerToTimeStamp(XnNodeHandle hPlayer, XnInt64 nTimeOffset, PlayerSeekOrigin origin);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSeekPlayerToFrame(XnNodeHandle hPlayer, string strNodeName, XnInt32 nFrameOffset, PlayerSeekOrigin origin);
		[DllImport("OpenNI")]
		public static extern XnStatus xnTellPlayerTimestamp(XnNodeHandle hPlayer, out XnUInt64 pnTimestamp);
		[DllImport("OpenNI")]
		public static extern XnStatus xnTellPlayerFrame(XnNodeHandle hPlayer, string strNodeName, out XnUInt32 pnFrame);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetPlayerNumFrames(XnNodeHandle hPlayer, string strNodeName, out XnUInt32 pnFrames);
		[DllImport("OpenNI")]
		public static extern string xnGetPlayerSupportedFormat(XnNodeHandle hPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumeratePlayerNodes(XnNodeHandle hPlayer, out XnNodeInfoList ppList);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsPlayerAtEOF(XnNodeHandle hPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToEndOfFileReached(XnNodeHandle hPlayer, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromEndOfFileReached(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPlaybackSpeed(XnNodeHandle hInstance, XnDouble dSpeed);
		[DllImport("OpenNI")]
		public static extern XnDouble xnGetPlaybackSpeed(XnNodeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoSetInstanceName(XnNodeInfo pNodeInfo, string strInstanceName);
		[DllImport("OpenNI")]
		public static extern ProductionNodeDescription xnNodeInfoGetDescription(XnNodeInfo pNodeInfo);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoGetTreeStringRepresentation(XnNodeInfo pNodeInfo, IntPtr csResult, XnUInt32 nSize);
		[DllImport("OpenNI")]
		public static extern string xnNodeInfoGetInstanceName(XnNodeInfo pNodeInfo);
		[DllImport("OpenNI")]
		public static extern string xnNodeInfoGetCreationInfo(XnNodeInfo pNodeInfo);
		[DllImport("OpenNI")]
		public static extern XnNodeInfoList xnNodeInfoGetNeededNodes(XnNodeInfo pNodeInfo);
		[DllImport("OpenNI")]
		public static extern XnNodeHandle xnNodeInfoGetHandle(XnNodeInfo pNodeInfo);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAllocate(out XnNodeInfoList ppList);
		[DllImport("OpenNI")]
		public static extern void xnNodeInfoListFree(XnNodeInfoList pList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAdd(XnNodeInfoList pList, ProductionNodeDescription pDescription, string strCreationInfo, XnNodeInfoList pNeededNodes);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAddNode(XnNodeInfoList pList, XnNodeInfo pNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAddNodeFromList(XnNodeInfoList pList, XnNodeInfoListIterator otherListIt);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListRemove(XnNodeInfoList pList, XnNodeInfoListIterator it);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListClear(XnNodeInfoList pList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAppend(XnNodeInfoList pList, XnNodeInfoList pOther);
		[DllImport("OpenNI")]
		public static extern XnBool xnNodeInfoListIsEmpty(XnNodeInfoList pList);
		[DllImport("OpenNI")]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetFirst(XnNodeInfoList pList);
		[DllImport("OpenNI")]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetLast(XnNodeInfoList pList);
		[DllImport("OpenNI")]
		public static extern XnBool xnNodeInfoListIteratorIsValid(XnNodeInfoListIterator it);
		[DllImport("OpenNI")]
		public static extern XnNodeInfo xnNodeInfoListGetCurrent(XnNodeInfoListIterator it);
		[DllImport("OpenNI")]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetNext(XnNodeInfoListIterator it);
		[DllImport("OpenNI")]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetPrevious(XnNodeInfoListIterator it);
/*		[DllImport("OpenNI")]
		public static extern XnStatus xnProfilingInit(XnUInt32 nProfilingInterval);
		[DllImport("OpenNI")]
		public static extern XnStatus xnProfilingInitFromINI(string cpINIFileName, string cpSectionName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnProfilingShutdown();
		[DllImport("OpenNI")]
		public static extern XnBool xnProfilingIsActive();
		[DllImport("OpenNI")]
		public static extern XnStatus xnProfilingSectionStart(char csSectionName, XnBool bMT, XnProfilingHandle pHandle);
		[DllImport("OpenNI")]
		public static extern XnStatus xnProfilingSectionEnd(XnProfilingHandle pHandle);
*/
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryAllocate(XnNodeQuery ppQuery);
		[DllImport("OpenNI")]
		public static extern void xnNodeQueryFree(XnNodeQuery pQuery);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetVendor(XnNodeQuery pQuery, string strVendor);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetName(XnNodeQuery pQuery, string strName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetMinVersion(XnNodeQuery pQuery, ref Version pMinVersion);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetMaxVersion(XnNodeQuery pQuery, ref Version pMaxVersion);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryAddSupportedCapability(XnNodeQuery pQuery, string strNeededCapability);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryAddSupportedMapOutputMode(XnNodeQuery pQuery, ref MapOutputMode pMapOutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetSupportedMinUserPositions(XnNodeQuery pQuery, XnUInt32 nCount);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetExistingNodeOnly(XnNodeQuery pQuery, XnBool bExistingNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryAddNeededNode(XnNodeQuery pQuery, string strInstanceName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetCreationInfo(XnNodeQuery pQuery, string strCreationInfo);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryFilterList(XnContext pContext, XnNodeQuery pQuery, XnNodeInfoList pList);
/*		[DllImport("OpenNI")]
		public static extern XnStatus xnSchedulerStart(out XnScheduler ppScheduler);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSchedulerShutdown(out XnScheduler ppScheduler);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSchedulerAddTask(XnScheduler pScheduler, XnUInt64 nInterval, XnTaskCallbackFuncPtr pCallback, IntPtr pCallbackArg, out XnScheduledTask ppTask);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSchedulerRemoveTask(XnScheduler pScheduler, out XnScheduledTask ppTask);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSchedulerRescheduleTask(XnScheduler pScheduler, XnScheduledTask pTask, XnUInt64 nInterval);
*/
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetStatusString(XnStatus Status);
		[DllImport("OpenNI")]
        public static extern IntPtr xnGetStatusName(XnStatus Status);
		[DllImport("OpenNI")]
		public static extern void xnPrintError(XnStatus Status, string csUserMessage);
//		[DllImport("OpenNI")]
//		public static extern XnStatus xnRegisterErrorCodeMessages(XnUInt16 nGroup, XnUInt16 nFirst, XnUInt16 nCount, XnErrorCodeData pErrorCodeData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetVersion(ref Version pVersion);
		[DllImport("OpenNI")]
        public static extern string xnProductionNodeTypeToString(NodeType Type);
		[DllImport("OpenNI")]
        public static extern XnStatus xnProductionNodeTypeFromString(string strType, NodeType pType);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsTypeGenerator(NodeType type);
		[DllImport("OpenNI")]
		public static extern string xnPixelFormatToString(PixelFormat format);
		[DllImport("OpenNI")]
		public static extern XnStatus xnPixelFormatFromString(string strName, PixelFormat pFormat);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnResolutionGetXRes(Resolution resolution);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnResolutionGetYRes(Resolution resolution);
		[DllImport("OpenNI")]
		public static extern Resolution xnResolutionGetFromXYRes(XnUInt32 xRes, XnUInt32 yRes);
		[DllImport("OpenNI")]
		public static extern Resolution xnResolutionGetFromName(string strName);
		[DllImport("OpenNI")]
		public static extern string xnResolutionGetName(Resolution resolution);
		[DllImport("OpenNI")]
		public static extern XnInt32 xnVersionCompare(ref Version pVersion1, ref Version pVersion2);
		[DllImport("OpenNI")]
		public static extern XnStatus xnVersionToString(ref Version pVersion, IntPtr csResult, XnUInt32 nSize);
		[DllImport("OpenNI")]
		public static extern XnStatus XnProductionNodeDescriptionToString(ProductionNodeDescription pDescription, IntPtr csResult, XnUInt32 nSize);
		[DllImport("OpenNI")]
		public static extern Point3D xnCreatePoint3D(XnFloat x, XnFloat y, XnFloat z);
		[DllImport("OpenNI")]
		public static extern Point3D xnCreatePlane3D(Point3D ptPoint, Point3D vNormal);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterModule(string strModule, string strConfigDir);
		[DllImport("OpenNI")]
		public static extern XnStatus xnUnregisterModule(string strModule);
		[DllImport("OpenNI")]
		public static extern XnStatus xnPrintRegisteredModules();
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterGlobalLicense(License pLicense);
		[DllImport("OpenNI")]
		public static extern XnStatus xnUnregisterGlobalLicense(License pLicense);
		[DllImport("OpenNI")]
		public static extern XnStatus xnPrintRegisteredLicenses();
		[DllImport("OpenNI")]
		public static extern IntPtr xnAllocateOutputMetaData();
		[DllImport("OpenNI")]
		public static extern void xnFreeOutputMetaData(IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCopyOutputMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport("OpenNI")]
		public static extern IntPtr xnAllocateMapMetaData();
		[DllImport("OpenNI")]
		public static extern void xnFreeMapMetaData(IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCopyMapMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport("OpenNI")]
		public static extern IntPtr xnAllocateDepthMetaData();
		[DllImport("OpenNI")]
		public static extern void xnFreeDepthMetaData(IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCopyDepthMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport("OpenNI")]
		public static extern IntPtr xnAllocateImageMetaData();
		[DllImport("OpenNI")]
		public static extern void xnFreeImageMetaData(IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCopyImageMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport("OpenNI")]
		public static extern IntPtr xnAllocateIRMetaData();
		[DllImport("OpenNI")]
		public static extern void xnFreeIRMetaData(IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCopyIRMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport("OpenNI")]
		public static extern IntPtr xnAllocateAudioMetaData();
		[DllImport("OpenNI")]
		public static extern void xnFreeAudioMetaData(IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCopyAudioMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport("OpenNI")]
		public static extern IntPtr xnAllocateSceneMetaData();
		[DllImport("OpenNI")]
		public static extern void xnFreeSceneMetaData(IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCopySceneMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAutoEnumerateOverSingleInput(XnContext pContext, XnNodeInfoList pList, ProductionNodeDescription pDescription, string strCreationInfo, NodeType InputType, XnEnumerationErrors pErrors, XnNodeQuery pQuery);
	}
}
