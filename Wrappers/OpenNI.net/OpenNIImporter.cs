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
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using XnStatus = System.Int32;
using XnContext = System.IntPtr;
using XnEnumerationErrors = System.IntPtr;
using XnNodeQuery = System.IntPtr;
using XnNodeInfoList = System.IntPtr;
using XnNodeInfo = System.IntPtr;
using XnNodeHandle = System.IntPtr;
using XnCallbackHandle = System.IntPtr;
using XnEnumerationErrorsIterator = System.IntPtr;
using XnLockHandle = System.Int32;
using XnUserID = System.Int32;
using XnCodecID = System.Int32;
using XnConfidence = System.Single;
using XnNodeInfoListIterator = System.IntPtr;

using XnBool = System.Boolean;
using XnUInt8 = System.Byte;
using XnUInt16 = System.UInt16;
using XnUInt32 = System.UInt32;
using XnInt32 = System.Int32;
using XnUInt64 = System.UInt64;
using XnInt64 = System.Int64;
using XnDouble = System.Double;
using XnFloat = System.Single;
using XnDepthPixel = System.UInt16;
using XnUInt = System.IntPtr;

namespace OpenNI
{
	public enum NodeType
	{
		Invalid = 0,

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

		/** A general production node **/
		ProductionNode = 13,

		/** A general generator **/
		Generator = 14,

		/** A general map generator **/
		MapGenerator = 15,

		ScriptNode = 16,
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
		Invalid = 0,
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
		Invalid = 0,
		Head = 1,
		Neck = 2,
		Torso = 3,
		Waist = 4,

		LeftCollar = 5,
		LeftShoulder = 6,
		LeftElbow = 7,
		LeftWrist = 8,
		LeftHand = 9,
		LeftFingertip = 10,

		RightCollar = 11,
		RightShoulder = 12,
		RightElbow = 13,
		RightWrist = 14,
		RightHand = 15,
		RightFingertip = 16,

		LeftHip = 17,
		LeftKnee = 18,
		LeftAnkle = 19,
		LeftFoot = 20,

		RightHip = 21,
		RightKnee = 22,
		RightAnkle = 23,
		RightFoot = 24
	};

	/** A configuration of joints to be used by skeleton (see @ref xnSetSkeletonProfile) **/
	public enum SkeletonProfile
	{
		Invalid = 0,

		/** No joints at all **/
		None = 1,

		/** All joints **/
		All = 2,

		/** All the joints in the upper body (torso and upwards) **/
		Upper = 3,

		/** All the joints in the lower body (torso and downwards) **/
		Lower = 4,

		/** The head and the hands **/
		HeadAndHands = 5,
	};

	public enum RecordMedium
	{
		/** Recording medium is a file **/
		File = 0,
	};

	public enum PlayerSeekOrigin
	{
		Set = 0,
		Current = 1,
		End = 2,
	};

	public enum PowerLineFrequency
	{
		Off = 0,
		Hz50 = 50,
		Hz60 = 60,
	};

    public enum Direction
    {
        Illegal = 0,
        Left = 1,
        Right = 2,
        Up = 3,
        Down = 4,
        Forward = 5,
        Backward = 6,
    }

    /** Possible statuses for pose detection */
    public enum PoseDetectionStatus
    {
	    OK = 0,
	    NoUser = 1,
	    TopFOV = 2,
	    SideFoV = 3,
	    Error = 4,
        NoTracking = 5
    }

    public enum PoseDetectionState
    {
        InPose = 0,
        OutOfPose = 1,
        Undefined = 2

    }

/** Possible statuses for calibration */
    public enum CalibrationStatus
    {
	    OK = 0,
	    NoUser = 1,
	    Arm = 2,
	    Leg = 3,
	    Head = 4,
	    Torso = 5,
	    TopFOV = 6,
	    SideFoV = 7,
	    Pose = 8,
        ManualAbort = 9,
        ManualReset = 10,
        Timeout = 11
    }

    public enum SkeletonPostProcessing
    {
    	None	= 0,
    	All		= 1,
    }

	public static class Capabilities
	{
		public static string ExtendedSerialization = "ExtendedSerialization";
		public static string Mirror = "Mirror";
		public static string AlternativeViewPoint = "AlternativeViewPoint";
		public static string Cropping = "Cropping";
		public static string UserPosition = "UserPosition";
		public static string Skeleton = "User::Skeleton";
		public static string PoseDetection = "User::PoseDetection";
		public static string LockAware = "LockAware";
		public static string ErrorState = "ErrorState";
		public static string FrameSync = "FrameSync";
		public static string DeviceIdentification = "DeviceIdentification";
		public static string Brightness = "Brightness";
		public static string Contrast = "Contrast";
		public static string Hue = "Hue";
		public static string Saturation = "Saturation";
		public static string Sharpness = "Sharpness";
		public static string Gamma = "Gamma";
		public static string WhiteBalance = "ColorTemperature";
		public static string BacklightCompensation = "BacklightCompensation";
		public static string Gain = "Gain";
		public static string Pan = "Pan";
		public static string Tilt = "Tilt";
		public static string Roll = "Roll";
		public static string Zoom = "Zoom";
		public static string Exposure = "Exposure";
		public static string Iris = "Iris";
		public static string Focus = "Focus";
		public static string LowLightCompensation = "LowLightCompensation";
		public static string AntiFlicker = "AntiFlicker";
        public static string HandTouchingFOVEdge = "Hands::HandTouchingFOVEdge";
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct Version
	{
		public static Version Current
		{
			get
			{
				Version retVal = Version.Zero;
				int status = SafeNativeMethods.xnGetVersion(ref retVal);
				WrapperUtils.ThrowOnError(status);
				return retVal;
			}
		}

        public static Version Zero
        {
            get
            {
                Version retVal;
                retVal.build = 0;
                retVal.major = 0;
                retVal.maintenance = 0;
                retVal.minor = 0;
                return retVal;
            }
        }
        
        public byte Major
		{
			get { return major; }
			set { major = value; }
		}

		public byte Minor
		{
			get { return minor; }
			set { minor = value; }
		}

		public Int32 Maintenance
		{
			get { return maintenance; }
			set { maintenance = (UInt16)value; }
		}

		public Int32 Build
		{
			get { return (Int32)build; }
			set { build = (UInt32)value; }
		}

		private XnUInt8 major;
		private XnUInt8 minor;
		private XnUInt16 maintenance;
		private XnUInt32 build;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct Cropping
	{
		public bool Enabled
		{
			get { return bEnabled; }
			set { bEnabled = value; }
		}

		public int XOffset
		{
			get { return nXOffset; }
			set { nXOffset = (UInt16)value; }
		}

		public int YOffset
		{
			get { return nYOffset; }
			set { nYOffset = (UInt16)value; }
		}

		public int XSize
		{
			get { return nXSize; }
			set { nXSize = (UInt16)value; }
		}

		public int YSize
		{
			get { return nYSize; }
			set { nYSize = (UInt16)value; }
		}

		private XnBool bEnabled;
		private XnUInt16 nXOffset;
		private XnUInt16 nYOffset;
		private XnUInt16 nXSize;
		private XnUInt16 nYSize;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct MapOutputMode
	{
		public Int32 XRes
		{
			get { return (Int32)nXRes; }
			set { nXRes = (UInt32)value; }
		}
		public Int32 YRes
		{
			get { return (Int32)nYRes; }
			set { nYRes = (UInt32)value; }
		}
		public Int32 FPS
		{
			get { return (Int32)nFPS; }
			set { nFPS = (UInt32)value; }
		}

		private XnUInt32 nXRes;
		private XnUInt32 nYRes;
		private XnUInt32 nFPS;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct FieldOfView
	{
		public XnDouble HorizontalAngle
		{
			get { return fHFOV; }
			set { fHFOV = value; }
		}
		public XnDouble VerticalAngle
		{
			get { return fVFOV; }
			set { fVFOV = value; }
		}

		private XnDouble fHFOV;
		private XnDouble fVFOV;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct Point3D
	{
		public static Point3D ZeroPoint = new Point3D(0,0,0);
		public Point3D(XnFloat x, XnFloat y, XnFloat z)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public XnFloat X
		{
			get { return x; }
			set { x = value; }
		}
		public XnFloat Y
		{
			get { return y; }
			set { y = value; }
		}
		public XnFloat Z
		{
			get { return z; }
			set { z = value; }
		}

		private XnFloat x;
		private XnFloat y;
		private XnFloat z;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct BoundingBox3D
	{
		public OpenNI.Point3D LeftBottomNear
		{
			get { return leftBottomNear; }
			set { leftBottomNear = value; }
		}
		public OpenNI.Point3D RightTopFar
		{
			get { return rightTopFar; }
			set { rightTopFar = value; }
		}

		private Point3D leftBottomNear;
		private Point3D rightTopFar;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct RGB24Pixel
	{
		public XnUInt8 Red
		{
			get { return nRed; }
			set { nRed = value; }
		}
		public XnUInt8 Green
		{
			get { return nGreen; }
			set { nGreen = value; }
		}
		public XnUInt8 Blue
		{
			get { return nBlue; }
			set { nBlue = value; }
		}

		private XnUInt8 nRed;
		private XnUInt8 nGreen;
		private XnUInt8 nBlue;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct YUV422DoublePixel
	{
		public XnUInt8 U
		{
			get { return nU; }
			set { nU = value; }
		}
		public XnUInt8 Y1
		{
			get { return nY1; }
			set { nY1 = value; }
		}
		public XnUInt8 V
		{
			get { return nV; }
			set { nV = value; }
		}
		public XnUInt8 Y2
		{
			get { return nY2; }
			set { nY2 = value; }
		}

		private XnUInt8 nU;
		private XnUInt8 nY1;
		private XnUInt8 nV;
		private XnUInt8 nY2;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct Plane3D
	{
		public Plane3D(Point3D normal, Point3D point)
		{
			this.vNormal = normal;
			this.ptPoint = point;
		}

		public OpenNI.Point3D Normal
		{
			get { return vNormal; }
			set { vNormal = value; }
		}

		public OpenNI.Point3D Point
		{
			get { return ptPoint; }
			set { ptPoint = value; }
		}

		private Point3D vNormal;
		private Point3D ptPoint;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct WaveOutputMode
	{
		public Int32 SampleRate
		{
			get { return (Int32)nSampleRate; }
			set { nSampleRate = (UInt32)value; }
		}

		public Int32 BitsPerSample
		{
			get { return nBitsPerSample; }
			set { nBitsPerSample = (UInt16)value; }
		}

		public Byte Channels
		{
			get { return nChannels; }
			set { nChannels = value; }
		}

		private XnUInt32 nSampleRate;
		private XnUInt16 nBitsPerSample;
		private XnUInt8 nChannels;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct SkeletonJointPosition
	{
		public OpenNI.Point3D Position
		{
			get { return position; }
			set { position = value; }
		}

		public XnConfidence Confidence
		{
			get { return confidence; }
			set { confidence = value; }
		}

		private Point3D position;
		private XnConfidence confidence;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct SkeletonJointOrientation
	{
		/** The actual orientation */
		public float X1
		{
			get { return x1; }
			set { x1 = value; }
		}

		public float Y1
		{
			get { return y1; }
			set { y1 = value; }
		}

		public float Z1
		{
			get { return z1; }
			set { z1 = value; }
		}

		public float X2
		{
			get { return x2; }
			set { x2 = value; }
		}

		public float Y2
		{
			get { return y2; }
			set { y2 = value; }
		}

		public float Z2
		{
			get { return z2; }
			set { z2 = value; }
		}

		public float X3
		{
			get { return x3; }
			set { x3 = value; }
		}

		public float Y3
		{
			get { return y3; }
			set { y3 = value; }
		}

		public float Z3
		{
			get { return z3; }
			set { z3 = value; }
		}

		/** The confidence in the orientation */
		public XnConfidence Confidence
		{
			get { return confidence; }
			set { confidence = value; }
		}

		private float x1;
		private float y1;
		private float z1;
		private float x2;
		private float y2;
		private float z2;
		private float x3;
		private float y3;
		private float z3;
		private XnConfidence confidence;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct SkeletonJointTransformation
	{
		public OpenNI.SkeletonJointPosition Position
		{
			get { return position; }
			set { position = value; }
		}

		public OpenNI.SkeletonJointOrientation Orientation
		{
			get { return orientation; }
			set { orientation = value; }
		}

		private SkeletonJointPosition position;
		private SkeletonJointOrientation orientation;
	};

	/** A pair of 32-bit unsigned values. **/
	[StructLayout(LayoutKind.Sequential)]
	public struct UInt32XYPair
	{
		public Int32 X
		{
			get { return (Int32)x; }
			set { x = (UInt32)value; }
		}

		public Int32 Y
		{
			get { return (Int32)y; }
			set { y = (UInt32)value; }
		}

		private XnUInt32 x;
		private XnUInt32 y;
	};

	internal static class Defs
	{
		public const int XN_MAX_NAME_LENGTH = 80;
		public const int XN_MAX_LICENSE_LENGTH = 255;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct ProductionNodeDescription
	{
		public NodeType Type
		{
			get { return type; }
			set { type = value; }
		}
		public string Vendor
		{
			get { return strVendor; }
			set { strVendor = value; }
		}
		public string Name
		{
			get { return strName; }
			set { strName = value; }
		}
		public OpenNI.Version Version
		{
			get { return version; }
			set { version = value; }
		}

        public static ProductionNodeDescription Empty
        {
            get
            {
                ProductionNodeDescription description;
                description.type = NodeType.Invalid;
                description.strVendor = "";
                description.strName = "";
                description.version = Version.Zero;
                return description;
            }
        }

		private NodeType type;
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_NAME_LENGTH)]
		private string strVendor;
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_NAME_LENGTH)]
		private string strName;
		private Version version;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct License
	{
		public string Vendor
		{
			get { return strVendor; }
			set { strVendor = value; }
		}
		public string Key
		{
			get { return strKey; }
			set { strKey = value; }
		}

		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_NAME_LENGTH)]
		private string strVendor;
		[MarshalAs(UnmanagedType.ByValTStr, SizeConst = Defs.XN_MAX_LICENSE_LENGTH)]
		private string strKey;
	};

	internal class SafeNativeMethods
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
		[StructLayout(LayoutKind.Sequential, Pack = 1)]
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
		public delegate void XnNodeCreationHandler(XnContext pContext, XnNodeHandle hCreatedNode, IntPtr pCookie);
		public delegate void XnNodeDestructionHandler(XnContext pContext, string strDestroyedNodeName, IntPtr pCookie);
		public delegate void XnContextShuttingDownHandler(XnContext pContext, IntPtr pCookie);
		public delegate void XnStateChangedHandler(XnNodeHandle hNode, IntPtr pCookie);
		public delegate void XnGestureRecognized(XnNodeHandle hNode, string strGesture, ref Point3D pIDPosition, ref Point3D pEndPosition, IntPtr pCookie);
		public delegate void XnGestureProgress(XnNodeHandle hNode, string strGesture, ref Point3D pPosition, XnFloat fProgress, IntPtr pCookie);
        public delegate void XnGestureIntermediateStageCompleted(XnNodeHandle hNode, string strGesture, ref Point3D pPosition, IntPtr pCookie);
        public delegate void XnGestureReadyForNextIntermediateStage(XnNodeHandle hNode, string strGesture, ref Point3D pPosition, IntPtr pCookie);
        public delegate void XnCalibrationStart(XnNodeHandle hNode, XnUserID user, IntPtr pCookie);
		public delegate void XnCalibrationEnd(XnNodeHandle hNode, XnUserID user, XnBool bSuccess, IntPtr pCookie);
        public delegate void XnCalibrationComplete(XnNodeHandle hNode, XnUserID user, CalibrationStatus status, IntPtr pCookie);
        public delegate void XnCalibrationInProgress(XnNodeHandle hNode, XnUserID user, CalibrationStatus status, IntPtr pCookie);
		public delegate void XnPoseDetectionCallback(XnNodeHandle hNode, string strPose, XnUserID user, IntPtr pCookie);
        public delegate void XnPoseDetectionInProgress(XnNodeHandle hNode, string strPose, XnUserID user, PoseDetectionStatus status, IntPtr pCookie);

		public delegate void XnUserHandler(XnNodeHandle hNode, XnUserID user, IntPtr pCookie);
		public delegate void XnHandCreate(XnNodeHandle hNode, XnUserID user, ref Point3D pPosition, XnFloat fTime, IntPtr pCookie);
		public delegate void XnHandUpdate(XnNodeHandle hNode, XnUserID user, ref Point3D pPosition, XnFloat fTime, IntPtr pCookie);
		public delegate void XnHandDestroy(XnNodeHandle hNode, XnUserID user, XnFloat fTime, IntPtr pCookie);
        public delegate void XnHandTouchingFOVEdge(XnNodeHandle hNode, XnUserID user, ref Point3D pPosition, XnFloat fTime, Direction dir, IntPtr pCookie);

#if Win_x64
		private const string openNILibraryName = "OpenNI64";
#else
		private const string openNILibraryName = "OpenNI";
#endif

		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnInit(out XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnStatus xnContextRunXmlScriptFromFile(XnContext pContext, string strFileName, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnContextRunXmlScriptFromFileEx(XnContext pContext, string strFileName, XnEnumerationErrors pErrors, out XnNodeHandle hScriptNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnStatus xnContextRunXmlScript(XnContext pContext, string xmlScript, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnContextRunXmlScriptEx(XnContext pContext, string xmlScript, XnEnumerationErrors pErrors, out XnNodeHandle hScriptNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnStatus xnInitFromXmlFile(string strFileName, out XnContext ppContext, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnInitFromXmlFileEx(string strFileName, out XnContext ppContext, XnEnumerationErrors pErrors, out XnNodeHandle hScriptNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnStatus xnContextOpenFileRecording(XnContext pContext, string strFileName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnContextOpenFileRecordingEx(XnContext pContext, string strFileName, out XnNodeHandle hScriptNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnContextAddRef(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnContextRelease(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern void xnShutdown(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnForceShutdown(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnContextRegisterForShutdown(XnContext pContext, [MarshalAs(UnmanagedType.FunctionPtr)] XnContextShuttingDownHandler pHandler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnContextUnregisterFromShutdown(XnContext pContext, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerateProductionTrees(XnContext pContext, NodeType Type, XnNodeQuery pQuery, out XnNodeInfoList ppTreesList, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateProductionTree(XnContext pContext, XnNodeInfo pTree, out XnNodeHandle phNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateAnyProductionTree(XnContext pContext, NodeType type, XnNodeQuery pQuery, out XnNodeHandle phNode, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateMockNode(XnContext pContext, NodeType type, string strName, out XnNodeHandle phNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateMockNodeBasedOn(XnContext pContext, XnNodeHandle hOriginalNode, string strName, out XnNodeHandle phMockNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnProductionNodeAddRef(XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnProductionNodeRelease(XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerateExistingNodes(XnContext pContext, out XnNodeInfoList ppList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerateExistingNodesByType(XnContext pContext, NodeType type, out XnNodeInfoList ppList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnStatus xnFindExistingNodeByType(XnContext pContext, NodeType type, out XnNodeHandle phNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnFindExistingRefNodeByType(XnContext pContext, NodeType type, out XnNodeHandle phNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnStatus xnGetNodeHandleByName(XnContext pContext, string strInstanceName, out XnNodeHandle phNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetRefNodeHandleByName(XnContext pContext, string strInstanceName, out XnNodeHandle phNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnWaitAndUpdateAll(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnWaitOneUpdateAll(XnContext pContext, XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnWaitAnyUpdateAll(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnWaitNoneUpdateAll(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStartGeneratingAll(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStopGeneratingAll(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetGlobalMirror(XnContext pContext, XnBool bMirror);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnGetGlobalMirror(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetGlobalErrorState(XnContext pContext);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToGlobalErrorStateChange(XnContext pContext, [MarshalAs(UnmanagedType.FunctionPtr)] XnErrorStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromGlobalErrorStateChange(XnContext pContext, XnCallbackHandle hCallback);

        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToNodeCreation(XnContext pContext, [MarshalAs(UnmanagedType.FunctionPtr)] XnNodeCreationHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromNodeCreation(XnContext pContext, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToNodeDestruction(XnContext pContext, [MarshalAs(UnmanagedType.FunctionPtr)] XnNodeDestructionHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromNodeDestruction(XnContext pContext, XnCallbackHandle hCallback);

        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerationErrorsAllocate(out XnEnumerationErrors ppErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnEnumerationErrorsFree(XnEnumerationErrors pErrors);
		//		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		//		public static extern XnStatus xnEnumerationErrorsAdd(XnEnumerationErrors pErrors, ProductionNodeDescription pDesc, XnStatus nError);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerationErrorsToString(XnEnumerationErrors pErrors, StringBuilder csBuffer, XnUInt32 nSize);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerationErrorsClear(XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnEnumerationErrorsIterator xnEnumerationErrorsGetFirst(XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnEnumerationErrorsIterator xnEnumerationErrorsGetNext(XnEnumerationErrorsIterator it);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnEnumerationErrorsIteratorIsValid(XnEnumerationErrorsIterator it);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern ProductionNodeDescription xnEnumerationErrorsGetCurrentDescription(XnEnumerationErrorsIterator it);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerationErrorsGetCurrentError(XnEnumerationErrorsIterator it);

		/*		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnFPSInit(XnFPSData pFPS, XnUInt32 nSamplesCount);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnFPSMarkFrame(XnFPSData pFPS, XnUInt64 nNow);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnDouble xnFPSCalc(XnFPSData pFPS, XnUInt32 nAverageOver, XnUInt64 nNow);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnFPSFree(XnFPSData pFPS);
		*/
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnAddLicense(XnContext pContext, IntPtr pLicense);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerateLicenses(XnContext pContext, out IntPtr paLicenses, out XnUInt32 pnCount);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeLicensesList(IntPtr aLicenses);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogInitSystem();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogInitFromINIFile(string csINIFile, string csSectionName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogInitFromXmlFile(string strFileName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogStartNewFile();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogClose();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogSetMaskState(string csMask, XnBool bEnabled);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnDumpSetMaskState(string csMask, XnBool bEnabled);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogSetSeverityFilter(LogSeverity nMinSeverity);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogSetConsoleOutput(XnBool bConsoleOutput);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogSetFileOutput(XnBool bFileOutput);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogSetLineInfo(XnBool bLineInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLogSetOutputFolder(string strOutputFolder);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnLogIsEnabled(string csLogMask, LogSeverity nSeverity);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnLogWrite(string csLogMask, LogSeverity nSeverity, string csFile, XnUInt32 nLine, string csFormat);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnLogWriteNoEntry(string csLogMask, LogSeverity nSeverity, string csFormat);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnLogWriteBinaryData(string csLogMask, LogSeverity nSeverity, string csFile, XnUInt32 nLine, IntPtr pBinData, XnUInt32 nDataSize, string csFormat);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnLogIsDumpMaskEnabled(string csDumpMask);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeInfo xnGetNodeInfo(XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetNodeName(XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnContext xnGetContextFromNodeHandle(XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnContext xnGetRefContextFromNodeHandle(XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsCapabilitySupported(XnNodeHandle hInstance, string strCapabilityName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetIntProperty(XnNodeHandle hInstance, string strName, XnUInt64 nValue);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetRealProperty(XnNodeHandle hInstance, string strName, XnDouble dValue);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetStringProperty(XnNodeHandle hInstance, string strName, string strValue);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetGeneralProperty(XnNodeHandle hInstance, string strName, XnUInt32 nBufferSize, IntPtr pBuffer);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetIntProperty(XnNodeHandle hInstance, string strName, out XnUInt64 pnValue);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetRealProperty(XnNodeHandle hInstance, string strName, out XnDouble pdValue);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetStringProperty(XnNodeHandle hInstance, string strName, StringBuilder csValue, XnUInt32 nBufSize);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetGeneralProperty(XnNodeHandle hInstance, string strName, XnUInt32 nBufferSize, IntPtr pBuffer);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLockNodeForChanges(XnNodeHandle hInstance, out XnLockHandle phLock);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnUnlockNodeForChanges(XnNodeHandle hInstance, XnLockHandle hLock);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLockedNodeStartChanges(XnNodeHandle hInstance, XnLockHandle hLock);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLockedNodeEndChanges(XnNodeHandle hInstance, XnLockHandle hLock);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnAddNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRemoveNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode);

		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnCreateDevice(XnContext pContext, out XnNodeHandle phDevice, XnNodeQuery pQuery, XnEnumerationErrors pErrors);

        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnGetDeviceName(XnNodeHandle hInstance, StringBuilder buffer, ref XnUInt32 pnBufferSize);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnGetVendorSpecificData(XnNodeHandle hInstance, StringBuilder buffer, ref XnUInt32 pnBufferSize);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnGetSerialNumber(XnNodeHandle hInstance, StringBuilder buffer, ref XnUInt32 pnBufferSize);

		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetNodeErrorState(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToNodeErrorStateChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromNodeErrorStateChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern  XnStatus xnGetGeneralIntRange(XnNodeHandle hNode, string strCap, out XnInt32 pnMin, out XnInt32 pnMax, out XnInt32 pnStep, out XnInt32 pnDefault, out XnBool pbIsAutoSupported);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern  XnStatus xnGetGeneralIntValue(XnNodeHandle hNode, string strCap, out XnInt32 pnValue);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern  XnStatus xnSetGeneralIntValue(XnNodeHandle hNode, string strCap, XnInt32 nValue);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern  XnStatus xnRegisterToGeneralIntValueChange(XnNodeHandle hNode, string strCap, XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern  void xnUnregisterFromGeneralIntValueChange(XnNodeHandle hNode, string strCap, XnCallbackHandle hCallback);

        // Generator
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStartGenerating(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsGenerating(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStopGenerating(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToGenerationRunningChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromGenerationRunningChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToNewDataAvailable(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromNewDataAvailable(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsNewDataAvailable(XnNodeHandle hInstance, out XnUInt64 pnTimestamp);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnWaitAndUpdateData(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsDataNew(XnNodeHandle hInstance);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr xnGetData(XnNodeHandle hInstance);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnGetDataSize(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt64 xnGetTimestamp(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnGetFrameID(XnNodeHandle hInstance);

        // Mirror
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetMirror(XnNodeHandle hInstance, XnBool bMirror);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsMirrored(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToMirrorChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromMirrorChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsViewPointSupported(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetViewPoint(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnResetViewPoint(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsViewPointAs(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToViewPointChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromViewPointChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnGetPixelCoordinatesInViewPoint(XnNodeHandle hInstance, XnNodeHandle hOther, XnUInt32 x, XnUInt32 y, out XnUInt32 pAltX, out XnUInt32 pAltY);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnCanFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStopFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsFrameSyncedWith(XnNodeHandle hInstance, XnNodeHandle hOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToFrameSyncChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromFrameSyncChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);

        // MapGenerator
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnGetSupportedMapOutputModesCount(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetSupportedMapOutputModes(XnNodeHandle hInstance, [In][Out] MapOutputMode[] aModes, ref XnUInt32 pnCount);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetMapOutputMode(XnNodeHandle hInstance, ref MapOutputMode pOutputMode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetMapOutputMode(XnNodeHandle hInstance, ref MapOutputMode pOutputMode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToMapOutputModeChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromMapOutputModeChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnUInt32 xnGetBytesPerPixel(XnNodeHandle hInstance);

		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetCropping(XnNodeHandle hInstance, ref Cropping pCropping);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetCropping(XnNodeHandle hInstance, ref Cropping pCropping);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToCroppingChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromCroppingChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetPowerLineFrequency(XnNodeHandle hGenerator, PowerLineFrequency nFrequency);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern PowerLineFrequency xnGetPowerLineFrequency(XnNodeHandle hGenerator);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToPowerLineFrequencyChange(XnNodeHandle hGenerator, XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromPowerLineFrequencyChange(XnNodeHandle hGenerator, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateDepthGenerator(XnContext pContext, out XnNodeHandle phDepthGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnDepthPixel xnGetDeviceMaxDepth(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetDepthFieldOfView(XnNodeHandle hInstance, ref FieldOfView pFOV);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToDepthFieldOfViewChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromDepthFieldOfViewChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnConvertProjectiveToRealWorld(XnNodeHandle hInstance, XnUInt32 nCount, Point3D[] aProjective, [In][Out] Point3D[] aRealWorld);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnConvertRealWorldToProjective(XnNodeHandle hInstance, XnUInt32 nCount, Point3D[] aRealWorld, [In][Out] Point3D[] aProjective);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetDepthMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnGetDepthMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnGetSupportedUserPositionsCount(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, ref BoundingBox3D pPosition);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, ref BoundingBox3D pPosition);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToUserPositionChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromUserPositionChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateImageGenerator(XnContext pContext, out XnNodeHandle phImageGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetRGB24ImageMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetYUV422ImageMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetGrayscale8ImageMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetGrayscale16ImageMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetImageMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsPixelFormatSupported(XnNodeHandle hInstance, PixelFormat Format);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetPixelFormat(XnNodeHandle hInstance, PixelFormat Format);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern PixelFormat xnGetPixelFormat(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToPixelFormatChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromPixelFormatChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnGetImageMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateIRGenerator(XnContext pContext, out XnNodeHandle phIRGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetIRMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnGetIRMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateGestureGenerator(XnContext pContext, out XnNodeHandle phGestureGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnAddGesture(XnNodeHandle hInstance, string strGesture, ref BoundingBox3D pArea);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnAddGesture(XnNodeHandle hInstance, string strGesture, IntPtr pArea);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRemoveGesture(XnNodeHandle hInstance, string strGesture);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetAllActiveGestures(XnNodeHandle hInstance, [In] [Out] IntPtr[] pstrGestures, XnUInt32 nNameLength, ref XnUInt16 nGestures);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerateAllGestures(XnNodeHandle hInstance, [In] [Out] IntPtr[] pstrGestures, XnUInt32 nNameLength, ref XnUInt16 nGestures);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsGestureAvailable(XnNodeHandle hInstance, string strGesture);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnUInt32 xnGetNumberOfAvailableGestures(XnNodeHandle hInstance);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnBool xnIsGestureProgressSupported(XnNodeHandle hInstance, string strGesture);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterGestureCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureRecognized RecognizedCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureProgress ProgressCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterGestureCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToGestureIntermediateStageCompleted(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureIntermediateStageCompleted GestureIntermediateStageCompletedCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromGestureIntermediateStageCompleted(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToGestureReadyForNextIntermediateStage(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnGestureReadyForNextIntermediateStage GestureReadyForNextIntermediateStageCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromGestureReadyForNextIntermediateStage(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToGestureChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromGestureChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateSceneAnalyzer(XnContext pContext, out XnNodeHandle phSceneAnalyzer, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetLabelMap(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetFloor(XnNodeHandle hInstance, ref Plane3D pPlane);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnGetSceneMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateUserGenerator(XnContext pContext, out XnNodeHandle phUserGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt16 xnGetNumberOfUsers(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetUsers(XnNodeHandle hInstance, [In][Out] XnUserID[] users, ref XnUInt16 pnUsers);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetUserCoM(XnNodeHandle hInstance, XnUserID user, out Point3D pCoM);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetUserPixels(XnNodeHandle hInstance, XnUserID user, IntPtr pScene);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterUserCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler NewUserCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler LostUserCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterUserCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToUserExit(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler UserExitCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromUserExit(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToUserReEnter(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnUserHandler UserReEnterCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromUserReEnter(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsJointAvailable(XnNodeHandle hInstance, SkeletonJoint eJoint);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsProfileAvailable(XnNodeHandle hInstance, SkeletonProfile eProfile);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetSkeletonProfile(XnNodeHandle hInstance, SkeletonProfile eProfile);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetJointActive(XnNodeHandle hInstance, SkeletonJoint eJoint, XnBool bState);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsJointActive(XnNodeHandle hInstance, SkeletonJoint eJoint);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToJointConfigurationChange(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromJointConfigurationChange(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumerateActiveJoints(XnNodeHandle hInstance, SkeletonJoint pJoints, XnUInt16 pnJoints);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetSkeletonJoint(XnNodeHandle hInstance, XnUserID user, SkeletonJoint joint, ref SkeletonJointTransformation transformation);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetSkeletonJointPosition(XnNodeHandle hInstance, XnUserID user, SkeletonJoint joint, ref SkeletonJointPosition position);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetSkeletonJointOrientation(XnNodeHandle hInstance, XnUserID user, SkeletonJoint joint, ref SkeletonJointOrientation orientation);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsSkeletonCalibrated(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsSkeletonCalibrating(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRequestSkeletonCalibration(XnNodeHandle hInstance, XnUserID user, XnBool bForce);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnAbortSkeletonCalibration(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSaveSkeletonCalibrationDataToFile(XnNodeHandle hInstance, XnUserID user, string strFileName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLoadSkeletonCalibrationDataFromFile(XnNodeHandle hInstance, XnUserID user, string strFileName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSaveSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLoadSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnClearSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStartSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStopSkeletonTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnResetSkeleton(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnNeedPoseForSkeletonCalibration(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetSkeletonCalibrationPose(XnNodeHandle hInstance, StringBuilder sb);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetSkeletonSmoothing(XnNodeHandle hInstance, XnFloat fFactor);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterCalibrationCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationStart CalibrationStartCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationEnd CalibrationEndCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterCalibrationCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToCalibrationStart(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationStart CalibrationStartCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromCalibrationStart(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToCalibrationInProgress(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationInProgress CalibrationInProgressCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromCalibrationInProgress(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToCalibrationComplete(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnCalibrationComplete CalibrationCompleteCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromCalibrationComplete(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnGetNumberOfPoses(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetAllAvailablePoses(XnNodeHandle hInstance, [In] [Out] IntPtr[] pstrPoses, XnUInt32 nNameLength, ref XnUInt32 pnPoses);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnBool xnIsPoseSupported(XnNodeHandle hInstance, string strPose);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnGetPoseStatus(XnNodeHandle hInstance, XnUserID userID, string poseName, out XnUInt64 poseTime, out PoseDetectionStatus eStatus,out PoseDetectionState eState);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStartPoseDetection(XnNodeHandle hInstance, string strPose, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStopPoseDetection(XnNodeHandle hInstance, XnUserID user);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnStopSinglePoseDetection(XnNodeHandle hInstance, XnUserID user, string poseName);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToPoseCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionStartCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionEndCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromPoseCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToPoseDetected(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionStartCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromPoseDetected(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToOutOfPose(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionCallback PoseDetectionStartCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromOutOfPose(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToPoseDetectionInProgress(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnPoseDetectionInProgress PoseDetectionInProgressCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromPoseDetectionInProgress(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateHandsGenerator(XnContext pContext, out XnNodeHandle phHandsGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterHandCallbacks(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandCreate CreateCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandUpdate UpdateCB, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandDestroy DestroyCB, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterHandCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnRegisterToHandTouchingFOVEdge(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnHandTouchingFOVEdge TouchingCB, IntPtr pCookie, out XnCallbackHandle phCallback);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void xnUnregisterFromHandTouchingFOVEdge(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStopTracking(XnNodeHandle hInstance, XnUserID user);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStopTrackingAll(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnStartTracking(XnNodeHandle hInstance, ref Point3D pPosition);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetTrackingSmoothing(XnNodeHandle hInstance, XnFloat fFactor);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateAudioGenerator(XnContext pContext, out XnNodeHandle phAudioGenerator, XnNodeQuery pQuery, XnEnumerationErrors pErrors);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetAudioBuffer(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnGetSupportedWaveOutputModesCount(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetSupportedWaveOutputModes(XnNodeHandle hInstance, [In][Out] WaveOutputMode[] aSupportedModes, ref XnUInt32 pnCount);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetWaveOutputMode(XnNodeHandle hInstance, ref WaveOutputMode OutputMode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetWaveOutputMode(XnNodeHandle hInstance, ref WaveOutputMode OutputMode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToWaveOutputModeChanges(XnNodeHandle hInstance, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromWaveOutputModeChanges(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnGetAudioMetaData(XnNodeHandle hInstance, IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnMockDepthSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnMockImageSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnMockIRSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnMockAudioSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, IntPtr pData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateCodec(XnContext pContext, XnCodecID codecID, XnNodeHandle hInitializerNode, out XnNodeHandle phCodec);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnCodecID xnGetCodecID(XnNodeHandle hCodec);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEncodeData(XnNodeHandle hCodec, IntPtr pSrc, XnUInt32 nSrcSize, IntPtr pDst, XnUInt32 nDstSize, [MarshalAs(UnmanagedType.SysUInt)] out UInt32 pnBytesWritten);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnDecodeData(XnNodeHandle hCodec, IntPtr pSrc, XnUInt32 nSrcSize, IntPtr pDst, XnUInt32 nDstSize, [MarshalAs(UnmanagedType.SysUInt)] out UInt32 pnBytesWritten);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateRecorder(XnContext pContext, string strFormatName, out XnNodeHandle phRecorder);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetRecorderDestination(XnNodeHandle hRecorder, RecordMedium destType, string strDest);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnAddNodeToRecording(XnNodeHandle hRecorder, XnNodeHandle hNode, XnCodecID compression);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRemoveNodeFromRecording(XnNodeHandle hRecorder, XnNodeHandle hNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRecord(XnNodeHandle hRecorder);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetRecorderFormat(XnNodeHandle hRecorder);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreatePlayer(XnContext pContext, string strFormatName, out XnNodeHandle phPlayer);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetPlayerRepeat(XnNodeHandle hPlayer, XnBool bRepeat);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetPlayerSource(XnNodeHandle hPlayer, RecordMedium sourceType, string strSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetPlayerSource(XnNodeHandle hPlayer, out RecordMedium pSourceType, StringBuilder strSource, XnUInt32 nBufSize);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnPlayerReadNext(XnNodeHandle hPlayer);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSeekPlayerToTimeStamp(XnNodeHandle hPlayer, XnInt64 nTimeOffset, PlayerSeekOrigin origin);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSeekPlayerToFrame(XnNodeHandle hPlayer, string strNodeName, XnInt32 nFrameOffset, PlayerSeekOrigin origin);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnTellPlayerTimestamp(XnNodeHandle hPlayer, out XnUInt64 pnTimestamp);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnTellPlayerFrame(XnNodeHandle hPlayer, string strNodeName, out XnUInt32 pnFrame);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetPlayerNumFrames(XnNodeHandle hPlayer, string strNodeName, out XnUInt32 pnFrames);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetPlayerSupportedFormat(XnNodeHandle hPlayer);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnEnumeratePlayerNodes(XnNodeHandle hPlayer, out XnNodeInfoList ppList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsPlayerAtEOF(XnNodeHandle hPlayer);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterToEndOfFileReached(XnNodeHandle hPlayer, [MarshalAs(UnmanagedType.FunctionPtr)] XnStateChangedHandler handler, IntPtr pCookie, out XnCallbackHandle phCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnUnregisterFromEndOfFileReached(XnNodeHandle hInstance, XnCallbackHandle hCallback);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnSetPlaybackSpeed(XnNodeHandle hInstance, XnDouble dSpeed);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnDouble xnGetPlaybackSpeed(XnNodeHandle hInstance);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoSetInstanceName(XnNodeInfo pNodeInfo, string strInstanceName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnNodeInfoGetDescription(XnNodeInfo pNodeInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnStatus xnNodeInfoGetTreeStringRepresentation(XnNodeInfo pNodeInfo, StringBuilder csBuffer, XnUInt32 nSize);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnNodeInfoGetInstanceName(XnNodeInfo pNodeInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnNodeInfoGetCreationInfo(XnNodeInfo pNodeInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeInfoList xnNodeInfoGetNeededNodes(XnNodeInfo pNodeInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		[Obsolete]
		public static extern XnNodeHandle xnNodeInfoGetHandle(XnNodeInfo pNodeInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeHandle xnNodeInfoGetRefHandle(XnNodeInfo pNodeInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoListAllocate(out XnNodeInfoList ppList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnNodeInfoListFree(XnNodeInfoList pList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoListAdd(XnNodeInfoList pList, ProductionNodeDescription pDescription, string strCreationInfo, XnNodeInfoList pNeededNodes);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoListAddNode(XnNodeInfoList pList, XnNodeInfo pNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoListAddNodeFromList(XnNodeInfoList pList, XnNodeInfoListIterator otherListIt);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoListRemove(XnNodeInfoList pList, XnNodeInfoListIterator it);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoListClear(XnNodeInfoList pList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeInfoListAppend(XnNodeInfoList pList, XnNodeInfoList pOther);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnNodeInfoListIsEmpty(XnNodeInfoList pList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetFirst(XnNodeInfoList pList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetLast(XnNodeInfoList pList);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnNodeInfoListIteratorIsValid(XnNodeInfoListIterator it);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeInfo xnNodeInfoListGetCurrent(XnNodeInfoListIterator it);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetNext(XnNodeInfoListIterator it);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnNodeInfoListIterator xnNodeInfoListGetPrevious(XnNodeInfoListIterator it);
		/*		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnProfilingInit(XnUInt32 nProfilingInterval);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnProfilingInitFromINI(string cpINIFileName, string cpSectionName);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnProfilingShutdown();
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnBool xnProfilingIsActive();
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnProfilingSectionStart(char csSectionName, XnBool bMT, XnProfilingHandle pHandle);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnProfilingSectionEnd(XnProfilingHandle pHandle);
		*/
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQueryAllocate(out XnNodeQuery ppQuery);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnNodeQueryFree(XnNodeQuery pQuery);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQuerySetVendor(XnNodeQuery pQuery, string strVendor);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQuerySetName(XnNodeQuery pQuery, string strName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQuerySetMinVersion(XnNodeQuery pQuery, ref Version pMinVersion);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQuerySetMaxVersion(XnNodeQuery pQuery, ref Version pMaxVersion);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQueryAddSupportedCapability(XnNodeQuery pQuery, string strNeededCapability);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQueryAddSupportedMapOutputMode(XnNodeQuery pQuery, ref MapOutputMode pMapOutputMode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQuerySetSupportedMinUserPositions(XnNodeQuery pQuery, XnUInt32 nCount);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQuerySetExistingNodeOnly(XnNodeQuery pQuery, XnBool bExistingNode);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQueryAddNeededNode(XnNodeQuery pQuery, string strInstanceName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQuerySetCreationInfo(XnNodeQuery pQuery, string strCreationInfo);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnNodeQueryFilterList(XnContext pContext, XnNodeQuery pQuery, XnNodeInfoList pList);
		/*		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnSchedulerStart(out XnScheduler ppScheduler);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnSchedulerShutdown(out XnScheduler ppScheduler);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnSchedulerAddTask(XnScheduler pScheduler, XnUInt64 nInterval, XnTaskCallbackFuncPtr pCallback, IntPtr pCallbackArg, out XnScheduledTask ppTask);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnSchedulerRemoveTask(XnScheduler pScheduler, out XnScheduledTask ppTask);
				[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
				public static extern XnStatus xnSchedulerRescheduleTask(XnScheduler pScheduler, XnScheduledTask pTask, XnUInt64 nInterval);
		*/
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetStatusString(XnStatus Status);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnGetStatusName(XnStatus Status);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnPrintError(XnStatus Status, string csUserMessage);
		//		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		//		public static extern XnStatus xnRegisterErrorCodeMessages(XnUInt16 nGroup, XnUInt16 nFirst, XnUInt16 nCount, XnErrorCodeData pErrorCodeData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnGetVersion(ref Version pVersion);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnProductionNodeTypeToString(NodeType Type);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnProductionNodeTypeFromString(string strType, NodeType pType);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnBool xnIsTypeGenerator(NodeType type);
        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern XnBool xnIsTypeDerivedFrom(NodeType type, NodeType baseType);

        [DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnPixelFormatToString(PixelFormat format);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnPixelFormatFromString(string strName, PixelFormat pFormat);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnResolutionGetXRes(Resolution resolution);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnUInt32 xnResolutionGetYRes(Resolution resolution);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Resolution xnResolutionGetFromXYRes(XnUInt32 xRes, XnUInt32 yRes);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Resolution xnResolutionGetFromName(string strName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnResolutionGetName(Resolution resolution);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnInt32 xnVersionCompare(ref Version pVersion1, ref Version pVersion2);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnVersionToString(ref Version pVersion, IntPtr csResult, XnUInt32 nSize);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus XnProductionNodeDescriptionToString(ProductionNodeDescription pDescription, IntPtr csResult, XnUInt32 nSize);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Point3D xnCreatePoint3D(XnFloat x, XnFloat y, XnFloat z);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Point3D xnCreatePlane3D(Point3D ptPoint, Point3D vNormal);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterModule(string strModule, string strConfigDir);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnUnregisterModule(string strModule);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnPrintRegisteredModules();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnRegisterGlobalLicense(License pLicense);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnUnregisterGlobalLicense(License pLicense);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnPrintRegisteredLicenses();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnAllocateOutputMetaData();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeOutputMetaData(IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCopyOutputMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnAllocateMapMetaData();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeMapMetaData(IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCopyMapMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnAllocateDepthMetaData();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeDepthMetaData(IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCopyDepthMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnAllocateImageMetaData();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeImageMetaData(IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCopyImageMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnAllocateIRMetaData();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeIRMetaData(IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCopyIRMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnAllocateAudioMetaData();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeAudioMetaData(IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCopyAudioMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnAllocateSceneMetaData();
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void xnFreeSceneMetaData(IntPtr pMetaData);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCopySceneMetaData(IntPtr pDestination, IntPtr pSource);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnAutoEnumerateOverSingleInput(XnContext pContext, XnNodeInfoList pList, ProductionNodeDescription pDescription, string strCreationInfo, NodeType InputType, XnEnumerationErrors pErrors, XnNodeQuery pQuery);

		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnCreateScriptNode(XnContext pContext, string strFormat, out XnNodeHandle phScript);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr xnScriptNodeGetSupportedFormat(XnNodeHandle hScript);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLoadScriptFromFile(XnNodeHandle hScript, string strFileName);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnLoadScriptFromString(XnNodeHandle hScript, string strScript);
		[DllImport(openNILibraryName, CallingConvention = CallingConvention.Cdecl)]
		public static extern XnStatus xnScriptNodeRun(XnNodeHandle hScript);
	}
}
