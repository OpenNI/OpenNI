using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using XnBool = System.Boolean;
using XnCallbackHandle = System.IntPtr;
using XnCodecID = System.UInt32;
using XnConfidence = System.Single;
using XnDepthPixel = System.UInt16;
using XnDouble = System.Double;
using XnEnumerationErrorsIterator = System.IntPtr;
using XnFloat = System.Single;
using XnInt32 = System.Int32;
using XnInt64 = System.Int64;
using XnLockHandle = System.UInt32;
using XnNodeHandle = System.IntPtr;
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
		public delegate void XnStateChangedHandler(NodeSafeHandle hNode, IntPtr pCookie);
		public delegate void XnGestureRecognized(NodeSafeHandle hNode, string strGesture, ref Point3D pIDPosition, ref Point3D pEndPosition, IntPtr pCookie);
		public delegate void XnGestureProgress(NodeSafeHandle hNode, string strGesture, ref Point3D pPosition, XnFloat fProgress, IntPtr pCookie);
		public delegate void XnCalibrationStart(NodeSafeHandle hNode, XnUserID user, IntPtr pCookie);
		public delegate void XnCalibrationEnd(NodeSafeHandle hNode, XnUserID user, XnBool bSuccess, IntPtr pCookie);
		public delegate void XnPoseDetectionCallback(NodeSafeHandle hNode, string strPose, XnUserID user, IntPtr pCookie);

		public delegate void XnUserHandler(NodeSafeHandle hNode, XnUserID user, IntPtr pCookie);
		public delegate void XnHandCreate(NodeSafeHandle hNode, XnUserID user, ref Point3D pPosition, XnFloat fTime, IntPtr pCookie);
		public delegate void XnHandUpdate(NodeSafeHandle hNode, XnUserID user, ref Point3D pPosition, XnFloat fTime, IntPtr pCookie);
		public delegate void XnHandDestroy(NodeSafeHandle hNode, XnUserID user, XnFloat fTime, IntPtr pCookie);

		[DllImport("OpenNI")]
		public static extern XnStatus xnInit(out ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnContextRunXmlScriptFromFile(ContextSafeHandle pContext, string strFileName, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnContextRunXmlScript(ContextSafeHandle pContext, string xmlScript, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnInitFromXmlFile(string strFileName, out ContextSafeHandle ppContext, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnContextOpenFileRecording(ContextSafeHandle pContext, string strFileName);
		[DllImport("OpenNI")]
		public static extern void xnShutdown(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateProductionTrees(ContextSafeHandle pContext, NodeType Type, QuerySafeHandle pQuery, out NodeInfoListSafeHandle ppTreesList, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateProductionTree(ContextSafeHandle pContext, NodeInfoSafeHandle pTree, out NodeSafeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateAnyProductionTree(ContextSafeHandle pContext, NodeType type, QuerySafeHandle pQuery, out NodeSafeHandle phNode, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateMockNode(ContextSafeHandle pContext, NodeType type, string strName, out NodeSafeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateMockNodeBasedOn(ContextSafeHandle pContext, NodeSafeHandle hOriginalNode, string strName, out NodeSafeHandle phMockNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnProductionNodeAddRef(NodeSafeHandle hNode);
		[DllImport("OpenNI")]
		public static extern void xnProductionNodeRelease(NodeSafeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateExistingNodes(ContextSafeHandle pContext, out NodeInfoListSafeHandle ppList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateExistingNodesByType(ContextSafeHandle pContext, NodeType type, out NodeInfoListSafeHandle ppList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnFindExistingNodeByType(ContextSafeHandle pContext, NodeType type, out NodeSafeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetNodeHandleByName(ContextSafeHandle pContext, string strInstanceName, out NodeSafeHandle phNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitAndUpdateAll(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitOneUpdateAll(ContextSafeHandle pContext, NodeSafeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitAnyUpdateAll(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitNoneUpdateAll(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartGeneratingAll(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopGeneratingAll(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetGlobalMirror(ContextSafeHandle pContext, XnBool bMirror);
		[DllImport("OpenNI")]
		public static extern XnBool xnGetGlobalMirror(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetGlobalErrorState(ContextSafeHandle pContext);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToGlobalErrorStateChange(ContextSafeHandle pContext, [MarshalAs(UnmanagedType.FunctionPtr)] XnErrorStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromGlobalErrorStateChange(ContextSafeHandle pContext, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
        public static extern XnStatus xnEnumerationErrorsAllocate(out EnumerationErrorsSafeHandle ppErrors);
		[DllImport("OpenNI")]
		public static extern void xnEnumerationErrorsFree(EnumerationErrorsSafeHandle pErrors);
//		[DllImport("OpenNI")]
//		public static extern XnStatus xnEnumerationErrorsAdd(EnumerationErrorsSafeHandle pErrors, ProductionNodeDescription pDesc, XnStatus nError);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerationErrorsToString(EnumerationErrorsSafeHandle pErrors, StringBuilder csBuffer, XnUInt32 nSize);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerationErrorsClear(EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnEnumerationErrorsIterator xnEnumerationErrorsGetFirst(EnumerationErrorsSafeHandle pErrors);
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
		public static extern XnStatus xnAddLicense(ContextSafeHandle pContext, License pLicense);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateLicenses(ContextSafeHandle pContext, out IntPtr paLicenses, out XnUInt32 pnCount);
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
		public static extern NodeInfoSafeHandle xnGetNodeInfo(NodeSafeHandle hNode);
		[DllImport("OpenNI")]
		public static extern string xnGetNodeName(NodeSafeHandle hNode);
		[DllImport("OpenNI")]
		public static extern ContextSafeHandle xnGetContextFromNodeHandle(NodeSafeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsCapabilitySupported(NodeSafeHandle hInstance, string strCapabilityName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetIntProperty(NodeSafeHandle hInstance, string strName, XnUInt64 nValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetRealProperty(NodeSafeHandle hInstance, string strName, XnDouble dValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetStringProperty(NodeSafeHandle hInstance, string strName, string strValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetGeneralProperty(NodeSafeHandle hInstance, string strName, XnUInt32 nBufferSize, IntPtr pBuffer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetIntProperty(NodeSafeHandle hInstance, string strName, out XnUInt64 pnValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetRealProperty(NodeSafeHandle hInstance, string strName, out XnDouble pdValue);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetStringProperty(NodeSafeHandle hInstance, string strName, StringBuilder csValue, XnUInt32 nBufSize);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetGeneralProperty(NodeSafeHandle hInstance, string strName, XnUInt32 nBufferSize, IntPtr pBuffer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLockNodeForChanges(NodeSafeHandle hInstance, out XnLockHandle phLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnUnlockNodeForChanges(NodeSafeHandle hInstance, XnLockHandle hLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLockedNodeStartChanges(NodeSafeHandle hInstance, XnLockHandle hLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLockedNodeEndChanges(NodeSafeHandle hInstance, XnLockHandle hLock);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddNeededNode(NodeSafeHandle hInstance, NodeSafeHandle hNeededNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRemoveNeededNode(NodeSafeHandle hInstance, NodeSafeHandle hNeededNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetNodeErrorState(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToNodeErrorStateChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromNodeErrorStateChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartGenerating(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsGenerating(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopGenerating(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToGenerationRunningChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromGenerationRunningChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToNewDataAvailable(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromNewDataAvailable(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsNewDataAvailable(NodeSafeHandle hInstance, out XnUInt64 pnTimestamp);
		[DllImport("OpenNI")]
		public static extern XnStatus xnWaitAndUpdateData(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsDataNew(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetDataSize(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt64 xnGetTimestamp(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetFrameID(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetMirror(NodeSafeHandle hInstance, XnBool bMirror);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsMirrored(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToMirrorChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromMirrorChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsViewPointSupported(NodeSafeHandle hInstance, NodeSafeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetViewPoint(NodeSafeHandle hInstance, NodeSafeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnResetViewPoint(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsViewPointAs(NodeSafeHandle hInstance, NodeSafeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToViewPointChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromViewPointChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnCanFrameSyncWith(NodeSafeHandle hInstance, NodeSafeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnFrameSyncWith(NodeSafeHandle hInstance, NodeSafeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopFrameSyncWith(NodeSafeHandle hInstance, NodeSafeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsFrameSyncedWith(NodeSafeHandle hInstance, NodeSafeHandle hOther);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToFrameSyncChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromFrameSyncChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetSupportedMapOutputModesCount(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSupportedMapOutputModes(NodeSafeHandle hInstance, [Out] MapOutputMode[] aModes, ref XnUInt32 pnCount);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetMapOutputMode(NodeSafeHandle hInstance, ref MapOutputMode pOutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetMapOutputMode(NodeSafeHandle hInstance, ref MapOutputMode pOutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToMapOutputModeChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromMapOutputModeChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetCropping(NodeSafeHandle hInstance, ref Cropping pCropping);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetCropping(NodeSafeHandle hInstance, ref Cropping pCropping);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToCroppingChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromCroppingChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateDepthGenerator(ContextSafeHandle pContext, out NodeSafeHandle phDepthGenerator, QuerySafeHandle pQuery,  EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnDepthPixel xnGetDeviceMaxDepth(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetDepthFieldOfView(NodeSafeHandle hInstance, ref FieldOfView pFOV);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToDepthFieldOfViewChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromDepthFieldOfViewChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnConvertProjectiveToRealWorld(NodeSafeHandle hInstance, XnUInt32 nCount, Point3D[] aProjective, [Out] Point3D[] aRealWorld);
		[DllImport("OpenNI")]
		public static extern XnStatus xnConvertRealWorldToProjective(NodeSafeHandle hInstance, XnUInt32 nCount, Point3D[] aRealWorld, [Out] Point3D[] aProjective);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetDepthMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern void xnGetDepthMetaData(NodeSafeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetSupportedUserPositionsCount(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetUserPosition(NodeSafeHandle hInstance, XnUInt32 nIndex, ref BoundingBox3D pPosition);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetUserPosition(NodeSafeHandle hInstance, XnUInt32 nIndex, ref BoundingBox3D pPosition);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToUserPositionChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromUserPositionChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateImageGenerator(ContextSafeHandle pContext, out NodeSafeHandle phImageGenerator, QuerySafeHandle pQuery,  EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetRGB24ImageMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetYUV422ImageMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetGrayscale8ImageMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetGrayscale16ImageMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetImageMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsPixelFormatSupported(NodeSafeHandle hInstance, PixelFormat Format);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPixelFormat(NodeSafeHandle hInstance, PixelFormat Format);
		[DllImport("OpenNI")]
		public static extern PixelFormat xnGetPixelFormat(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToPixelFormatChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromPixelFormatChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern void xnGetImageMetaData(NodeSafeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateIRGenerator(ContextSafeHandle pContext, out NodeSafeHandle phIRGenerator, QuerySafeHandle pQuery, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetIRMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern void xnGetIRMetaData(NodeSafeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
        public static extern XnStatus xnCreateGestureGenerator(ContextSafeHandle pContext, out NodeSafeHandle phGestureGenerator, QuerySafeHandle pQuery, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddGesture(NodeSafeHandle hInstance, string strGesture, ref BoundingBox3D pArea);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddGesture(NodeSafeHandle hInstance, string strGesture, IntPtr pArea);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRemoveGesture(NodeSafeHandle hInstance, string strGesture);
		[DllImport("OpenNI")]
        public static extern XnStatus xnGetAllActiveGestures(NodeSafeHandle hInstance,  [Out] IntPtr[] pstrGestures, XnUInt32 nNameLength, ref XnUInt16 nGestures);
		[DllImport("OpenNI")]
        public static extern XnStatus xnEnumerateAllGestures(NodeSafeHandle hInstance,  [Out] IntPtr[] pstrGestures, XnUInt32 nNameLength, ref XnUInt16 nGestures);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsGestureAvailable(NodeSafeHandle hInstance, string strGesture);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsGestureProgressSupported(NodeSafeHandle hInstance, string strGesture);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterGestureCallbacks(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureRecognized RecognizedCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureProgress ProgressCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterGestureCallbacks(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToGestureChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromGestureChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateSceneAnalyzer(ContextSafeHandle pContext, out NodeSafeHandle phSceneAnalyzer, QuerySafeHandle pQuery,  EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetLabelMap(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetFloor(NodeSafeHandle hInstance, ref Plane3D pPlane);
		[DllImport("OpenNI")]
		public static extern void xnGetSceneMetaData(NodeSafeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
        public static extern XnStatus xnCreateUserGenerator(ContextSafeHandle pContext, out NodeSafeHandle phUserGenerator, QuerySafeHandle pQuery, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnUInt16 xnGetNumberOfUsers(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
        public static extern XnStatus xnGetUsers(NodeSafeHandle hInstance, [Out] XnUserID[] users, ref XnUInt16 pnUsers);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetUserCoM(NodeSafeHandle hInstance, XnUserID user, out Point3D pCoM);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetUserPixels(NodeSafeHandle hInstance, XnUserID user, IntPtr pScene);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterUserCallbacks(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler NewUserCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler LostUserCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterUserCallbacks(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsJointAvailable(NodeSafeHandle hInstance, SkeletonJoint eJoint);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsProfileAvailable(NodeSafeHandle hInstance, SkeletonProfile eProfile);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetSkeletonProfile(NodeSafeHandle hInstance, SkeletonProfile eProfile);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetJointActive(NodeSafeHandle hInstance, SkeletonJoint eJoint, XnBool bState);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsJointActive(NodeSafeHandle hInstance, SkeletonJoint eJoint);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToJointConfigurationChange(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromJointConfigurationChange(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumerateActiveJoints(NodeSafeHandle hInstance, SkeletonJoint pJoints, XnUInt16 pnJoints);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonJoint(NodeSafeHandle hInstance, XnUserID user, SkeletonJoint eJoint, [Out] SkeletonJointTransformation pJoint);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonJointPosition(NodeSafeHandle hInstance, XnUserID user, SkeletonJoint eJoint, ref SkeletonJointPosition pJoint);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonJointOrientation(NodeSafeHandle hInstance, XnUserID user, SkeletonJoint eJoint, [Out] SkeletonJointOrientation pJoint);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonTracking(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonCalibrated(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonCalibrating(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRequestSkeletonCalibration(NodeSafeHandle hInstance, XnUserID user, XnBool bForce);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAbortSkeletonCalibration(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSaveSkeletonCalibrationData(NodeSafeHandle hInstance, XnUserID user, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnStatus xnLoadSkeletonCalibrationData(NodeSafeHandle hInstance, XnUserID user, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnStatus xnClearSkeletonCalibrationData(NodeSafeHandle hInstance, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsSkeletonCalibrationData(NodeSafeHandle hInstance, XnUInt32 nSlot);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartSkeletonTracking(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopSkeletonTracking(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnResetSkeleton(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnBool xnNeedPoseForSkeletonCalibration(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSkeletonCalibrationPose(NodeSafeHandle hInstance, StringBuilder sb);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetSkeletonSmoothing(NodeSafeHandle hInstance, XnFloat fFactor);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterCalibrationCallbacks(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationStart CalibrationStartCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationEnd CalibrationEndCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterCalibrationCallbacks(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetNumberOfPoses(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetAllAvailablePoses(NodeSafeHandle hInstance,  [Out] IntPtr[] pstrPoses, XnUInt32 nNameLength, ref XnUInt32 pnPoses);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartPoseDetection(NodeSafeHandle hInstance, string strPose, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopPoseDetection(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToPoseCallbacks(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionStartCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionEndCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromPoseCallbacks(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateHandsGenerator(ContextSafeHandle pContext, out NodeSafeHandle phHandsGenerator, QuerySafeHandle pQuery,  EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterHandCallbacks(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandCreate CreateCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandUpdate UpdateCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandDestroy DestroyCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterHandCallbacks(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopTracking(NodeSafeHandle hInstance, XnUserID user);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStopTrackingAll(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnStartTracking(NodeSafeHandle hInstance, ref Point3D pPosition);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetTrackingSmoothing(NodeSafeHandle hInstance, XnFloat fFactor);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateAudioGenerator(ContextSafeHandle pContext, out NodeSafeHandle phAudioGenerator, QuerySafeHandle pQuery, EnumerationErrorsSafeHandle pErrors);
		[DllImport("OpenNI")]
		public static extern IntPtr xnGetAudioBuffer(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnUInt32 xnGetSupportedWaveOutputModesCount(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetSupportedWaveOutputModes(NodeSafeHandle hInstance, [Out] WaveOutputMode[] aSupportedModes, ref XnUInt32 pnCount);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetWaveOutputMode(NodeSafeHandle hInstance, ref WaveOutputMode OutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetWaveOutputMode(NodeSafeHandle hInstance, ref WaveOutputMode OutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToWaveOutputModeChanges(NodeSafeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromWaveOutputModeChanges(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern void xnGetAudioMetaData(NodeSafeHandle hInstance, IntPtr pMetaData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockDepthSetData(NodeSafeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockImageSetData(NodeSafeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockIRSetData(NodeSafeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
		public static extern XnStatus xnMockAudioSetData(NodeSafeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport("OpenNI")]
        public static extern XnStatus xnCreateCodec(ContextSafeHandle pContext, XnCodecID codecID, NodeSafeHandle hInitializerNode, out NodeSafeHandle phCodec);
		[DllImport("OpenNI")]
		public static extern XnCodecID xnGetCodecID(NodeSafeHandle hCodec);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEncodeData(NodeSafeHandle hCodec, IntPtr pSrc, XnUInt32 nSrcSize, IntPtr pDst, XnUInt32 nDstSize, [MarshalAs(UnmanagedType.SysUInt)] out UInt32 pnBytesWritten);
		[DllImport("OpenNI")]
		public static extern XnStatus xnDecodeData(NodeSafeHandle hCodec, IntPtr pSrc, XnUInt32 nSrcSize, IntPtr pDst, XnUInt32 nDstSize, [MarshalAs(UnmanagedType.SysUInt)] out UInt32 pnBytesWritten);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreateRecorder(ContextSafeHandle pContext, string strFormatName, out NodeSafeHandle phRecorder);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetRecorderDestination(NodeSafeHandle hRecorder, RecordMedium destType, string strDest);
		[DllImport("OpenNI")]
		public static extern XnStatus xnAddNodeToRecording(NodeSafeHandle hRecorder, NodeSafeHandle hNode, XnCodecID compression);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRemoveNodeFromRecording(NodeSafeHandle hRecorder, NodeSafeHandle hNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRecord(NodeSafeHandle hRecorder);
		[DllImport("OpenNI")]
		public static extern string xnGetRecorderFormat(NodeSafeHandle hRecorder);
		[DllImport("OpenNI")]
		public static extern XnStatus xnCreatePlayer(ContextSafeHandle pContext, string strFormatName, out NodeSafeHandle phPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPlayerRepeat(NodeSafeHandle hPlayer, XnBool bRepeat);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPlayerSource(NodeSafeHandle hPlayer, RecordMedium sourceType, string strSource);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetPlayerSource(NodeSafeHandle hPlayer, out RecordMedium pSourceType, StringBuilder strSource, XnUInt32 nBufSize);
		[DllImport("OpenNI")]
		public static extern XnStatus xnPlayerReadNext(NodeSafeHandle hPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSeekPlayerToTimeStamp(NodeSafeHandle hPlayer, XnInt64 nTimeOffset, PlayerSeekOrigin origin);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSeekPlayerToFrame(NodeSafeHandle hPlayer, string strNodeName, XnInt32 nFrameOffset, PlayerSeekOrigin origin);
		[DllImport("OpenNI")]
		public static extern XnStatus xnTellPlayerTimestamp(NodeSafeHandle hPlayer, out XnUInt64 pnTimestamp);
		[DllImport("OpenNI")]
		public static extern XnStatus xnTellPlayerFrame(NodeSafeHandle hPlayer, string strNodeName, out XnUInt32 pnFrame);
		[DllImport("OpenNI")]
		public static extern XnStatus xnGetPlayerNumFrames(NodeSafeHandle hPlayer, string strNodeName, out XnUInt32 pnFrames);
		[DllImport("OpenNI")]
		public static extern string xnGetPlayerSupportedFormat(NodeSafeHandle hPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnEnumeratePlayerNodes(NodeSafeHandle hPlayer, out NodeInfoListSafeHandle ppList);
		[DllImport("OpenNI")]
		public static extern XnBool xnIsPlayerAtEOF(NodeSafeHandle hPlayer);
		[DllImport("OpenNI")]
		public static extern XnStatus xnRegisterToEndOfFileReached(NodeSafeHandle hPlayer, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport("OpenNI")]
		public static extern void xnUnregisterFromEndOfFileReached(NodeSafeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport("OpenNI")]
		public static extern XnStatus xnSetPlaybackSpeed(NodeSafeHandle hInstance, XnDouble dSpeed);
		[DllImport("OpenNI")]
		public static extern XnDouble xnGetPlaybackSpeed(NodeSafeHandle hInstance);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoSetInstanceName(NodeInfoSafeHandle pNodeInfo, string strInstanceName);
		[DllImport("OpenNI")]
		public static extern ProductionNodeDescription xnNodeInfoGetDescription(NodeInfoSafeHandle pNodeInfo);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoGetTreeStringRepresentation(NodeInfoSafeHandle pNodeInfo, IntPtr csResult, XnUInt32 nSize);
		[DllImport("OpenNI")]
		public static extern string xnNodeInfoGetInstanceName(NodeInfoSafeHandle pNodeInfo);
		[DllImport("OpenNI")]
		public static extern string xnNodeInfoGetCreationInfo(NodeInfoSafeHandle pNodeInfo);
		[DllImport("OpenNI")]
		public static extern NodeInfoListSafeHandle xnNodeInfoGetNeededNodes(NodeInfoSafeHandle pNodeInfo);
		[DllImport("OpenNI")]
		public static extern NodeSafeHandle xnNodeInfoGetHandle(NodeInfoSafeHandle pNodeInfo);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAllocate(out NodeInfoListSafeHandle ppList);
		[DllImport("OpenNI")]
		public static extern void xnNodeInfoListFree(NodeInfoListSafeHandle pList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAdd(NodeInfoListSafeHandle pList, ProductionNodeDescription pDescription, string strCreationInfo, NodeInfoListSafeHandle pNeededNodes);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAddNode(NodeInfoListSafeHandle pList, NodeInfoSafeHandle pNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAddNodeFromList(NodeInfoListSafeHandle pList, XnNodeInfoListIterator otherListIt);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListRemove(NodeInfoListSafeHandle pList, XnNodeInfoListIterator it);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListClear(NodeInfoListSafeHandle pList);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeInfoListAppend(NodeInfoListSafeHandle pList, NodeInfoListSafeHandle pOther);
		[DllImport("OpenNI")]
		public static extern XnBool xnNodeInfoListIsEmpty(NodeInfoListSafeHandle pList);
		[DllImport("OpenNI")]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetFirst(NodeInfoListSafeHandle pList);
		[DllImport("OpenNI")]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetLast(NodeInfoListSafeHandle pList);
		[DllImport("OpenNI")]
		public static extern XnBool xnNodeInfoListIteratorIsValid(XnNodeInfoListIterator it);
		[DllImport("OpenNI")]
		public static extern NodeInfoSafeHandle xnNodeInfoListGetCurrent(XnNodeInfoListIterator it);
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
        public static extern XnStatus xnNodeQueryAllocate(out QuerySafeHandle ppQuery);
		[DllImport("OpenNI")]
		public static extern void xnNodeQueryFree(QuerySafeHandle pQuery);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetVendor(QuerySafeHandle pQuery, string strVendor);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetName(QuerySafeHandle pQuery, string strName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetMinVersion(QuerySafeHandle pQuery, ref Version pMinVersion);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetMaxVersion(QuerySafeHandle pQuery, ref Version pMaxVersion);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryAddSupportedCapability(QuerySafeHandle pQuery, string strNeededCapability);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryAddSupportedMapOutputMode(QuerySafeHandle pQuery, ref MapOutputMode pMapOutputMode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetSupportedMinUserPositions(QuerySafeHandle pQuery, XnUInt32 nCount);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetExistingNodeOnly(QuerySafeHandle pQuery, XnBool bExistingNode);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryAddNeededNode(QuerySafeHandle pQuery, string strInstanceName);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQuerySetCreationInfo(QuerySafeHandle pQuery, string strCreationInfo);
		[DllImport("OpenNI")]
		public static extern XnStatus xnNodeQueryFilterList(ContextSafeHandle pContext, QuerySafeHandle pQuery, NodeInfoListSafeHandle pList);
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
		public static extern XnStatus xnAutoEnumerateOverSingleInput(ContextSafeHandle pContext, NodeInfoListSafeHandle pList, ProductionNodeDescription pDescription, string strCreationInfo, NodeType InputType, EnumerationErrorsSafeHandle pErrors, QuerySafeHandle pQuery);
	}
}
