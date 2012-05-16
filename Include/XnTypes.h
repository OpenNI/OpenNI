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
#ifndef __XN_TYPES_H__
#define __XN_TYPES_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStatus.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/** The maximum length of names of objects in OpenNI **/
#define XN_MAX_NAME_LENGTH					80

/** The maximum length of a creation info string **/
#define XN_MAX_CREATION_INFO_LENGTH			255

/** The maxumim length of a license key **/
#define XN_MAX_LICENSE_LENGTH				255

/** The maximum time to wait for node new data with the xnWaitXXXUpdateXXX functions **/
#define XN_NODE_WAIT_FOR_DATA_TIMEOUT		2000

/** OpenNI vendor ID, used by nodes supplied by OpenNI itself. **/
#define XN_VENDOR_OPEN_NI	"OpenNI"

/** The name of the OpenNI recording format. **/
#define XN_FORMAT_NAME_ONI	"oni"

/** The name of the OpenNI XML script format. **/
#define XN_SCRIPT_FORMAT_XML	"xml"

/** represents playback speed which does not consider file timestamps. **/
#define XN_PLAYBACK_SPEED_FASTEST	0.0

/** represents a value for automatic control for nodes supporting it, as part of the @ref general_int. **/
#define XN_AUTO_CONTROL				XN_MIN_INT32

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
struct XnInternalNodeData;

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

#if XN_PLATFORM != XN_PLATFORM_ARC
#pragma pack (push, 1)
#endif

/**
 * The context of an OpenNI library.
 */
typedef struct XnContext XnContext;

/**
 * A handle to a production node in the OpenNI context. A value of NULL represents an invalid handle.
 */
typedef struct XnInternalNodeData* XnNodeHandle;

/**
 * A handle to a lock. A value of NULL represents an invalid handle.
 */
typedef XnUInt32 XnLockHandle;

/**
 * Type of the production node.
 */
typedef XnInt32 XnProductionNodeType;

/**
 * Predefined types of production nodes.
 */
typedef enum XnPredefinedProductionNodeType
{
	/** An invalid node type **/
	XN_NODE_TYPE_INVALID = -1,

	/** A device node **/
	XN_NODE_TYPE_DEVICE = 1,
	
	/** A depth generator **/
	XN_NODE_TYPE_DEPTH = 2,
	
	/** An image generator **/
	XN_NODE_TYPE_IMAGE = 3,

	/** An audio generator **/
	XN_NODE_TYPE_AUDIO = 4,
	
	/** An IR generator **/
	XN_NODE_TYPE_IR = 5,

	/** A user generator **/
	XN_NODE_TYPE_USER = 6,
	
	/** A recorder **/
	XN_NODE_TYPE_RECORDER = 7,
	
	/** A player **/
	XN_NODE_TYPE_PLAYER = 8,
	
	/** A gesture generator **/
	XN_NODE_TYPE_GESTURE = 9,
	
	/** A scene analyzer **/
	XN_NODE_TYPE_SCENE = 10,
	
	/** A hands generator **/
	XN_NODE_TYPE_HANDS = 11,

	/** A Codec **/
	XN_NODE_TYPE_CODEC = 12,

	/** Abstract types **/
	XN_NODE_TYPE_PRODUCTION_NODE = 13,
	XN_NODE_TYPE_GENERATOR = 14,
	XN_NODE_TYPE_MAP_GENERATOR = 15,
	XN_NODE_TYPE_SCRIPT = 16,

	XN_NODE_TYPE_FIRST_EXTENSION,

} XnPredefinedProductionNodeType;

/** 
 * A Version. 
 */
typedef struct XnVersion
{
	XnUInt8 nMajor;
	XnUInt8 nMinor;
	XnUInt16 nMaintenance;
	XnUInt32 nBuild;
} XnVersion;

/** 
 * An exported generator description.
 */
typedef struct XnProductionNodeDescription
{
	/** Type of the production node. */
	XnProductionNodeType Type;
	/** Vendor name */
	XnChar strVendor[XN_MAX_NAME_LENGTH];
	/** Generator name. Should be unique under Type and Vendor. */
	XnChar strName[XN_MAX_NAME_LENGTH];
	/** Current version */
	XnVersion Version;
} XnProductionNodeDescription;

/**
 * Information about a single production node.
 */
typedef struct XnNodeInfo XnNodeInfo;

/**
 * A single node in a node info list. 
 */
typedef struct XnNodeInfoListNode XnNodeInfoListNode;

/**
 * A (linked) list of production node info objects.
 */
typedef struct XnNodeInfoList XnNodeInfoList;

/**
 * An iterator in a linked list of production node info objects.
 */
typedef struct XnNodeInfoListIterator
{
	XnNodeInfoListNode* pCurrent;
} XnNodeInfoListIterator;

typedef struct XnNodeQuery XnNodeQuery;

/**
 * A license key.
 */
typedef struct XnLicense
{
	/** Vendor name **/
	XnChar strVendor[XN_MAX_NAME_LENGTH];
	/** License key **/
	XnChar strKey[XN_MAX_LICENSE_LENGTH];
} XnLicense;

/**
 * @ingroup enum_errors
 * An object that holds errors during an enumeration process.
 */
typedef struct XnEnumerationErrors XnEnumerationErrors;

/** 
 * A handle to an instance of a generator. 
 **/
typedef void* XnModuleNodeHandle;

/** 
 * Prototype for state change callback function. 
 *
 * @param	hNode	[in]	A handle to the node that raised the event.
 * @param	pCookie	[in]	A user-provided cookie that was given when registering to the event.
 **/
typedef void (XN_CALLBACK_TYPE* XnStateChangedHandler)(XnNodeHandle hNode, void* pCookie);

/** 
 * Prototype for error state change callback function. 
 * 
 * @param	errorState	[in]	The new error state. A value of XN_STATUS_OK means there is no longer an error.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to the event.
 **/
typedef void (XN_CALLBACK_TYPE* XnErrorStateChangedHandler)(XnStatus errorState, void* pCookie);

/** 
 * Prototype for freeing data callbacks.
 *
 * @param	pData	[in]	The data to be freed.
 **/
typedef void (XN_CALLBACK_TYPE* XnFreeHandler)(const void* pData);

typedef void (XN_CALLBACK_TYPE* XnContextShuttingDownHandler)(XnContext* pContext, void* pCookie);

/**
* Prototype for the 'Node Creation' event handler.
*
* @param	[in]	pContext		The context raising the event.
* @param	[in]	hCreatedNode	A handle to the newly created node.
* @param	[in]	pCookie			A user-provided cookie that was given when registering to the event.
*
* @remark The passed handle (@c hCreatedNode) is not referenced. If your handler keeps this handle for later use,
*		  it must call @ref xnProductionNodeAddRef().
*/
typedef void (XN_CALLBACK_TYPE* XnNodeCreationHandler)(XnContext* pContext, XnNodeHandle hCreatedNode, void* pCookie);

/**
* Prototype for the 'Node Destruction' event handler.
*
* @param	[in]	pContext				The context raising the event.
* @param	[in]	strDestroyedNodeName	The name of the destroyed node.
* @param	[in]	pCookie					A user-provided cookie that was given when registering to the event.
*/
typedef void (XN_CALLBACK_TYPE* XnNodeDestructionHandler)(XnContext* pContext, const XnChar* strDestroyedNodeName, void* pCookie);

/** Handle to a registered callback function. **/
typedef void* XnCallbackHandle;

typedef struct XnModuleExportedProductionNodeInterface XnModuleExportedProductionNodeInterface; // Forward Declaration

//---------------------------------------------------------------------------
// 3D Vision Types
//---------------------------------------------------------------------------
/** Defines the depth values type (16-bit values). **/
typedef XnUInt16 XnDepthPixel;

/** Defines the value of a no-sample depth pixel. */
#define XN_DEPTH_NO_SAMPLE_VALUE	((XnDepthPixel)0)

/** Defines the image RGB24 pixel type. **/
typedef struct XnRGB24Pixel
{
	XnUInt8 nRed;
	XnUInt8 nGreen;
	XnUInt8 nBlue;
} XnRGB24Pixel;

/** Defines the image YUV422 double-pixel type. **/
typedef struct XnYUV422DoublePixel
{
	XnUInt8 nU;
	XnUInt8 nY1;
	XnUInt8 nV;
	XnUInt8 nY2;
} XnYUV422DoublePixel;

/** Defines the image Grayscale8 pixel type. **/
typedef XnUInt8 XnGrayscale8Pixel;

/** Defines the image Grayscale16 pixel type. **/
typedef XnUInt16 XnGrayscale16Pixel;

/** Defines the IR values type (16-bit grayscale). **/
typedef XnGrayscale16Pixel XnIRPixel;

/** Defines the label type */
typedef XnUInt16 XnLabel;

//---------------------------------------------------------------------------
// Generators Capabilities
//---------------------------------------------------------------------------
#define XN_CAPABILITY_EXTENDED_SERIALIZATION	"ExtendedSerialization"
#define XN_CAPABILITY_MIRROR					"Mirror"
#define XN_CAPABILITY_ALTERNATIVE_VIEW_POINT	"AlternativeViewPoint"
#define XN_CAPABILITY_CROPPING					"Cropping"
#define XN_CAPABILITY_USER_POSITION				"UserPosition"
#define XN_CAPABILITY_SKELETON					"User::Skeleton"
#define XN_CAPABILITY_POSE_DETECTION			"User::PoseDetection"
#define XN_CAPABILITY_LOCK_AWARE				"LockAware"
#define XN_CAPABILITY_ERROR_STATE				"ErrorState"
#define XN_CAPABILITY_FRAME_SYNC				"FrameSync"
#define XN_CAPABILITY_DEVICE_IDENTIFICATION		"DeviceIdentification"
#define XN_CAPABILITY_BRIGHTNESS				"Brightness"
#define XN_CAPABILITY_CONTRAST					"Contrast"
#define XN_CAPABILITY_HUE						"Hue"
#define XN_CAPABILITY_SATURATION				"Saturation"
#define XN_CAPABILITY_SHARPNESS					"Sharpness"
#define XN_CAPABILITY_GAMMA						"Gamma"
#define XN_CAPABILITY_COLOR_TEMPERATURE			"ColorTemperature"
#define XN_CAPABILITY_BACKLIGHT_COMPENSATION	"BacklightCompensation"
#define XN_CAPABILITY_GAIN						"Gain"
#define XN_CAPABILITY_PAN						"Pan"
#define XN_CAPABILITY_TILT						"Tilt"
#define XN_CAPABILITY_ROLL						"Roll"
#define XN_CAPABILITY_ZOOM						"Zoom"
#define XN_CAPABILITY_EXPOSURE					"Exposure"
#define XN_CAPABILITY_IRIS						"Iris"
#define XN_CAPABILITY_FOCUS						"Focus"
#define XN_CAPABILITY_LOW_LIGHT_COMPENSATION	"LowLightCompensation"
#define XN_CAPABILITY_ANTI_FLICKER				"AntiFlicker"
#define XN_CAPABILITY_HAND_TOUCHING_FOV_EDGE	"Hands::HandTouchingFOVEdge"

// Backwards compatibility - typo was fixed
#define XN_CAPABILITY_ANTI_FILCKER				XN_CAPABILITY_ANTI_FLICKER

// deprecated pragma is only supported in Visual Studio
#if (XN_PLATFORM == XN_PLATFORM_WIN32)
#pragma deprecated("XN_CAPABILITY_ANTI_FILCKER")
#endif

//---------------------------------------------------------------------------
// Generators API Structs
//---------------------------------------------------------------------------

#define XN_QQVGA_X_RES	160
#define XN_QQVGA_Y_RES	120

#define XN_CGA_X_RES	320
#define XN_CGA_Y_RES	200

#define XN_QVGA_X_RES	320
#define XN_QVGA_Y_RES	240

#define XN_VGA_X_RES	640
#define XN_VGA_Y_RES	480

#define XN_SVGA_X_RES	800
#define XN_SVGA_Y_RES	600

#define XN_XGA_X_RES	1024
#define XN_XGA_Y_RES	768

#define XN_720P_X_RES	1280
#define XN_720P_Y_RES	720

#define XN_SXGA_X_RES	1280
#define XN_SXGA_Y_RES	1024

#define XN_UXGA_X_RES	1600
#define XN_UXGA_Y_RES	1200

#define XN_1080P_X_RES	1920
#define XN_1080P_Y_RES	1080

#define XN_QCIF_X_RES	176
#define XN_QCIF_Y_RES	144

#define XN_240P_X_RES	423
#define XN_240P_Y_RES	240

#define XN_CIF_X_RES	352
#define XN_CIF_Y_RES	288

#define XN_WVGA_X_RES	640
#define XN_WVGA_Y_RES	360

#define XN_480P_X_RES	864
#define XN_480P_Y_RES	480

#define XN_576P_X_RES	1024
#define XN_576P_Y_RES	576

#define XN_DV_X_RES		960
#define XN_DV_Y_RES		720

typedef enum XnResolution
{
	XN_RES_CUSTOM = 0,
	XN_RES_QQVGA = 1,
	XN_RES_CGA = 2,
	XN_RES_QVGA = 3,
	XN_RES_VGA = 4,
	XN_RES_SVGA = 5,
	XN_RES_XGA = 6,
	XN_RES_720P = 7,
	XN_RES_SXGA = 8,
	XN_RES_UXGA = 9,
	XN_RES_1080P = 10,
	XN_RES_QCIF = 11,
	XN_RES_240P = 12,
	XN_RES_CIF = 13,
	XN_RES_WVGA = 14,
	XN_RES_480P = 15,
	XN_RES_576P = 16,
	XN_RES_DV = 17,
} XnResolution;

/**
 * The output mode of a map generator.
 */
typedef struct XnMapOutputMode
{
	/** Number of elements in the X-axis. */
	XnUInt32 nXRes;
	/** Number of elements in the Y-axis. */
	XnUInt32 nYRes;
	/** Number of frames per second. */
	XnUInt32 nFPS;
} XnMapOutputMode;

typedef enum XnSampleRate
{
	XN_SAMPLE_RATE_8K = 8000,
	XN_SAMPLE_RATE_11K = 11025,
	XN_SAMPLE_RATE_12K = 12000,
	XN_SAMPLE_RATE_16K = 16000,
	XN_SAMPLE_RATE_22K = 22050,
	XN_SAMPLE_RATE_24K = 24000,
	XN_SAMPLE_RATE_32K = 32000,
	XN_SAMPLE_RATE_44K = 44100,
	XN_SAMPLE_RATE_48K = 48000,
} XnSampleRate;

typedef struct XnWaveOutputMode
{
	XnUInt32 nSampleRate;
	XnUInt16 nBitsPerSample;
	XnUInt8 nChannels;
} XnWaveOutputMode;

/**
 * Representation of a vector in 3D space
 */
typedef struct XnVector3D
{
	XnFloat X;
	XnFloat Y;
	XnFloat Z;
} XnVector3D;

typedef XnVector3D XnPoint3D;

/**
 * Representation of an axis-aligned box in 3D space
 */
typedef struct XnBoundingBox3D
{
	XnPoint3D LeftBottomNear;
	XnPoint3D RightTopFar;
} XnBoundingBox3D;

/**
 * Cropping configuration
 */
typedef struct XnCropping
{
	/** TRUE if cropping is turned on, FALSE otherwise. */
	XnBool bEnabled;
	/** Offset in the X-axis, in pixels. */
	XnUInt16 nXOffset;
	/** Offset in the Y-axis, in pixels. */
	XnUInt16 nYOffset;
	/** Number of pixels in the X-axis. */
	XnUInt16 nXSize;
	/** Number of pixels in the Y-axis. */
	XnUInt16 nYSize;
} XnCropping;

/**
 * Field-Of-View
 */
typedef struct XnFieldOfView
{
	/** Horizontal Field Of View, in radians. */
	XnDouble fHFOV;
	/** Vertical Field Of View, in radians. */
	XnDouble fVFOV;
} XnFieldOfView;

typedef enum XnPixelFormat
{
	XN_PIXEL_FORMAT_RGB24 = 1,
	XN_PIXEL_FORMAT_YUV422 = 2,
	XN_PIXEL_FORMAT_GRAYSCALE_8_BIT = 3,
	XN_PIXEL_FORMAT_GRAYSCALE_16_BIT = 4,
	XN_PIXEL_FORMAT_MJPEG = 5,
} XnPixelFormat;

typedef struct XnSupportedPixelFormats
{
	XnBool m_bRGB24 : 1;
	XnBool m_bYUV422 : 1;
	XnBool m_bGrayscale8Bit : 1;
	XnBool m_bGrayscale16Bit : 1;
	XnBool m_bMJPEG : 1;
	XnUInt m_nPadding : 3;
	XnUInt m_nReserved : 24;
} XnSupportedPixelFormats;

typedef enum XnPlayerSeekOrigin
{
	XN_PLAYER_SEEK_SET = 0,
	XN_PLAYER_SEEK_CUR = 1,
	XN_PLAYER_SEEK_END = 2,
} XnPlayerSeekOrigin;

typedef enum XnPowerLineFrequency
{
	XN_POWER_LINE_FREQUENCY_OFF = 0,
	XN_POWER_LINE_FREQUENCY_50_HZ = 50,
	XN_POWER_LINE_FREQUENCY_60_HZ = 60,
} XnPowerLineFrequency;

// User
typedef XnUInt32 XnUserID;
typedef XnFloat  XnConfidence;

/** A 3x3 matrix of floating point values **/
typedef struct XnMatrix3X3
{
	/** The elements of the matrix. **/
	XnFloat elements[9];
} XnMatrix3X3;

/**
* A representation of a 3D plane. 
* A plane is described by a point on it and a normal
*/
typedef struct XnPlane3D
{
	/** The normal of the plane. **/
	XnVector3D vNormal;

	/** A point on the plane. **/
	XnPoint3D ptPoint;
} XnPlane3D;

/**
* Position of a specific joint. A joint position is described by its actual position
* and the confidence we have in that position.
*/
typedef struct XnSkeletonJointPosition
{
	/** The actual position in real world coordinations */
	XnVector3D		position;

	/** The confidence in the position */
	XnConfidence	fConfidence;
} XnSkeletonJointPosition;

/**
 * Orientation of a specific joint. A joint orientation is described by its actual rotation
 * and the confidence we have in that rotation
 * The first column is the X orientation, where the value increases from left to right.
 * The second column is the Y orientation, where the value increases from bottom to top.
 * The third column is the Z orientation, where the value increases from near to far.
 */
typedef struct XnSkeletonJointOrientation
{
	/** The actual orientation */
	XnMatrix3X3		orientation;
	/** The confidence in the orientation */
	XnConfidence	fConfidence;
} XnSkeletonJointOrientation;

/**
* Transformation of a specific joint is its full description: both its position and orientation.
*/
typedef struct XnSkeletonJointTransformation
{
	/** The position of the joint */
	XnSkeletonJointPosition		position;
	/** The orientation of the joint */
	XnSkeletonJointOrientation	orientation;
} XnSkeletonJointTransformation;

/**
* A full list of available joints.
*/
typedef enum XnSkeletonJoint
{
	XN_SKEL_HEAD			= 1,
	XN_SKEL_NECK			= 2,
	XN_SKEL_TORSO			= 3,
	XN_SKEL_WAIST			= 4,

	XN_SKEL_LEFT_COLLAR		= 5,
	XN_SKEL_LEFT_SHOULDER	= 6,
	XN_SKEL_LEFT_ELBOW		= 7,
	XN_SKEL_LEFT_WRIST		= 8,
	XN_SKEL_LEFT_HAND		= 9,
	XN_SKEL_LEFT_FINGERTIP	=10,

	XN_SKEL_RIGHT_COLLAR	=11,
	XN_SKEL_RIGHT_SHOULDER	=12,
	XN_SKEL_RIGHT_ELBOW		=13,
	XN_SKEL_RIGHT_WRIST		=14,
	XN_SKEL_RIGHT_HAND		=15,
	XN_SKEL_RIGHT_FINGERTIP	=16,

	XN_SKEL_LEFT_HIP		=17,
	XN_SKEL_LEFT_KNEE		=18,
	XN_SKEL_LEFT_ANKLE		=19,
	XN_SKEL_LEFT_FOOT		=20,

	XN_SKEL_RIGHT_HIP		=21,
	XN_SKEL_RIGHT_KNEE		=22,
	XN_SKEL_RIGHT_ANKLE		=23,
	XN_SKEL_RIGHT_FOOT		=24	
} XnSkeletonJoint;

/** A configuration of joints to be used by skeleton (see @ref xnSetSkeletonProfile) **/
typedef enum XnSkeletonProfile
{
	/** No joints at all **/
	XN_SKEL_PROFILE_NONE		= 1,

	/** All joints **/
	XN_SKEL_PROFILE_ALL			= 2,
	
	/** All the joints in the upper body (torso and upwards) **/
	XN_SKEL_PROFILE_UPPER		= 3,
	
	/** All the joints in the lower body (torso and downwards) **/
	XN_SKEL_PROFILE_LOWER		= 4,
	
	/** The head and the hands **/
	XN_SKEL_PROFILE_HEAD_HANDS	= 5,
} XnSkeletonProfile;

/** Possible statuses for pose detection */
typedef enum XnPoseDetectionStatus
{
	XN_POSE_DETECTION_STATUS_OK			= 0,
	XN_POSE_DETECTION_STATUS_NO_USER	= 1,
	XN_POSE_DETECTION_STATUS_TOP_FOV	= 2,
	XN_POSE_DETECTION_STATUS_SIDE_FOV	= 3,
	XN_POSE_DETECTION_STATUS_ERROR		= 4,
    XN_POSE_DETECTION_STATUS_NO_TRACKING = 5
} XnPoseDetectionStatus;


/** Possible pose detection states */
typedef enum XnPoseDetectionState
{
    XN_POSE_DETECTION_STATE_IN_POSE     =0,
    XN_POSE_DETECTION_STATE_OUT_OF_POSE =1,
    XN_POSE_DETECTION_STATE_UNDEFINED   =2
} XnPoseDetectionState;
/** Possible statuses for calibration */
typedef enum XnCalibrationStatus
{
	XN_CALIBRATION_STATUS_OK		    = 0,
	XN_CALIBRATION_STATUS_NO_USER	    = 1,
	XN_CALIBRATION_STATUS_ARM		    = 2,
	XN_CALIBRATION_STATUS_LEG		    = 3,
	XN_CALIBRATION_STATUS_HEAD		    = 4,
	XN_CALIBRATION_STATUS_TORSO		    = 5,
	XN_CALIBRATION_STATUS_TOP_FOV	    = 6,
	XN_CALIBRATION_STATUS_SIDE_FOV	    = 7,
	XN_CALIBRATION_STATUS_POSE		    = 8,
    XN_CALIBRATION_STATUS_MANUAL_ABORT  = 9,
    XN_CALIBRATION_STATUS_MANUAL_RESET  = 10,
    XN_CALIBRATION_STATUS_TIMEOUT_FAIL = 11
} XnCalibrationStatus;

typedef enum XnDirection
{
	XN_DIRECTION_ILLEGAL	= 0,
	XN_DIRECTION_LEFT		= 1,
	XN_DIRECTION_RIGHT		= 2,
	XN_DIRECTION_UP			= 3,
	XN_DIRECTION_DOWN		= 4,
	XN_DIRECTION_FORWARD	= 5,
	XN_DIRECTION_BACKWARD	= 6,
} XnDirection;

// User
/**
 * Callback for a general user-related event. It is used for either creation or destruction of users.
 *
 * @param	hNode	[in]	A handle to the user generator that raised this event.
 * @param	user	[in]	The id of the user that is relevant to this event.
 * @param	pCookie	[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnUserHandler)(XnNodeHandle hNode, XnUserID user, void* pCookie);

// Hands
/**
 * Callback for the creation of a new hand.
 *
 * @param	hNode		[in]	A handle to the hands generator that raised this event.
 * @param	user		[in]	The id of the new hand.
 * @param	pPosition	[in]	The position in which the hand was created.
 * @param	fTime		[in]	Timestamp, in seconds.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnHandCreate)(XnNodeHandle hNode, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);

/**
 * Callback for a new position of an already-existing hand.
 *
 * @param	hNode		[in]	A handle to the hands generator that raised this event.
 * @param	user		[in]	The id of the hand that moved.
 * @param	pPosition	[in]	The new position of the relevant hand.
 * @param	fTime		[in]	Timestamp, in seconds.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnHandUpdate)(XnNodeHandle hNode, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);

/**
 * Callback for the disappearance of a hand.
 *
 * @param	hNode		[in]	A handle to the hands generator that raised this event.
 * @param	user		[in]	The id of the hand that disappeared.
 * @param	fTime		[in]	Timestamp, in seconds.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnHandDestroy)(XnNodeHandle hNode, XnUserID user, XnFloat fTime, void* pCookie);

/**
 * Callback for when a hand reaches the edge of the FOV
 *
 * @param	hNode		[in]	A handle to the hand generator that raised this event.
 * @param	user		[in]	The id of the hand that reached FOV
 * @param	pPosition	[in]	The current position of the hand
 * @param	fTime		[in]	Timestamp, in seconds
 * @param	eDir		[in]	The direction of the edge that is being reached
 * @param	pCookie		[in]	A user-provided cookie that was given when reigstering to this event
 */
typedef void (XN_CALLBACK_TYPE* XnHandTouchingFOVEdge)(XnNodeHandle hNode, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDir, void* pCookie);
// Gesture Module
/**
 * Callback for the recognition of a gesture
 * 
 * @param	hNode		[in]	A handle to the gesture generator that raised this event.
 * @param	strGesture	[in]	The gesture that was recognized.
 * @param	pIDPosition	[in]	The position in which the gesture was identified.
 * @param	pEndPosition	[in]	The position of the hand that performed the gesture at the end of the gesture.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnGestureRecognized)(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie);
/**
 * Callback for indication that a certain gesture is in progress
 *
 * @param	hNode		[in]	A handle to the gesture generator that raised this event.
 * @param	strGesture	[in]	The gesture that is on its way to being recognized.
 * @param	pPosition	[in]	The current position of the hand that is performing the gesture.
 * @param	fProgress	[in]	The percentage of the gesture that was already performed.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnGestureProgress)(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie);

typedef void (XN_CALLBACK_TYPE* XnGestureIntermediateStageCompleted)(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnGestureReadyForNextIntermediateStage)(XnNodeHandle hNode, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie);

// Skeleton
/**
 * Callback for indication that a specific user's skeleton is now starting the calibration process
 * 
 * @param	hNode		[in]	A handle to the skeleton capability that raised this event.
 * @param	user		[in]	The id of the user that's being calibrated.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnCalibrationStart)(XnNodeHandle hNode, XnUserID user, void* pCookie);
/**
 * Callback for indication that a specific user's skeleton has now completed the calibration process
 * 
 * @param	hNode		[in]	A handle to the skeleton capability that raised this event.
 * @param	user		[in]	The id of the user for which calibration was attempted.
 * @param	bSuccess	[in]	An indication of whether or not the calibration attempt succeeded.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnCalibrationEnd)(XnNodeHandle hNode, XnUserID user, XnBool bSuccess, void* pCookie);

typedef void (XN_CALLBACK_TYPE* XnCalibrationInProgress)(XnNodeHandle hNode, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnCalibrationComplete)(XnNodeHandle hNode, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie);

// Pose Detection
/**
 * Callback for indication that a specific user has entered a pose, or left it.
 *
 * @param	hNode		[in]	A handle to the pose capability that raised this event.
 * @param	strPose		[in]	The pose that is relevant to the callback.
 * @param	user		[in]	The id of the user that entered the pose or left it.
 * @param	pCookie		[in]	A user-provided cookie that was given when registering to this event.
 */
typedef void (XN_CALLBACK_TYPE* XnPoseDetectionCallback)(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, void* pCookie);

typedef void (XN_CALLBACK_TYPE* XnPoseDetectionInProgress)(XnNodeHandle hNode, const XnChar* strPose, XnUserID user, XnPoseDetectionStatus poseDetectionError, void* pCookie);

//---------------------------------------------------------------------------
// Recorder Types
//---------------------------------------------------------------------------

/**
 * @ingroup recorder
 */

/** Defines the type of medium used for recording. See @ref xnAddNodeToRecording. */
typedef enum XnRecordMedium
{
	/** Recording medium is a file **/
	XN_RECORD_MEDIUM_FILE = 0,
} XnRecordMedium;

/** An ID of a codec. See @ref xnCreateCodec. **/
typedef XnUInt32 XnCodecID;

/** Define a Codec ID by 4 characters, e.g. XN_CODEC_ID('J','P','E','G') **/
#define XN_CODEC_ID(c1, c2, c3, c4) (XnCodecID)((c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

/** 
 * An interface used for communication between OpenNI and a recorder module. This interface is used by a recorder
 * module to send recorded data to OpenNI, which then knows how to store them according to one of the values of
 * @ref XnRecordMedium.
 **/
typedef struct XnRecorderOutputStreamInterface
{
	/**
	 * Opens the stream for writing.
	 *
	 * @param	pCookie		 [in]	A token that was received with this interface.
	 */
	XnStatus (XN_CALLBACK_TYPE* Open)(void* pCookie);

	/**
	 * Writes data to the stream
	 *
	 * @param	pCookie		 [in]	A token that was received with this interface.
	 * @param	strNodeName  [in]	Optional. Specifies the node that initiated the write action.
	 * @param	pData		 [in]	A pointer to the data to write.
	 * @param	nSize		 [in]	Number of bytes to be written.
	 */
	XnStatus (XN_CALLBACK_TYPE* Write)(void* pCookie, const XnChar* strNodeName, 
		const void* pData, XnUInt32 nSize);

	/**
	 * Sets the stream's pointer to the specified position.
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	seekType	 [in]	Specifies how to seek - according to current position, end or beginning.
	 * @param	nOffset		 [in]	Specifies how many bytes to move
	 */
	XnStatus (XN_CALLBACK_TYPE* Seek)(void* pCookie, XnOSSeekType seekType, const XnInt32 nOffset);

	/**
	 * Tells the current position in the stream.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	pPos		[out]	The position of the stream.
	 *
	 * @returns (XnUInt32)-1 on error.
	 */
	XnUInt32 (XN_CALLBACK_TYPE* Tell)(void* pCookie);

	/**
	 * Closes the stream.
	 *
	 * @param	pCookie		[in]	A token that was received with this interface.
	 */
	void (XN_CALLBACK_TYPE* Close)(void* pCookie);

	/**
	 * Sets the stream's pointer to the specified position. (64bit version, for large files)
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	seekType	 [in]	Specifies how to seek - according to current position, end or beginning.
	 * @param	nOffset		 [in]	Specifies how many bytes to move
	 */
	XnStatus (XN_CALLBACK_TYPE* Seek64)(void* pCookie, XnOSSeekType seekType, const XnInt64 nOffset);

	/**
	 * Tells the current position in the stream. (64bit version, for large files)
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	pPos		[out]	The position of the stream.
	 *
	 * @returns (XnUInt64)-1 on error.
	 */
	XnUInt64 (XN_CALLBACK_TYPE* Tell64)(void* pCookie);

} XnRecorderOutputStreamInterface;

/** 
 * An interface used for communication between OpenNI and a player module. This interface is used by a player
 * module to receive recorded data from OpenNI, which knows where to get them according to one of the values of
 * @ref XnRecordMedium.
 **/
typedef struct XnPlayerInputStreamInterface
{
	/**
	 * Opens the stream for reading.
	 *
	 * @param	pCookie [in]	A cookie that was received with this interface.
	 */
	XnStatus (XN_CALLBACK_TYPE* Open)(void* pCookie);

	/**
	 * Reads data from the stream. May read less data than asked, if the stream is near its end. This is not 
	 * considered an error.
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	pBuffer		 [out]	A pointer to the buffer to read into.
	 * @param	nSize		 [in]	Number of bytes to read.
	 * @param	pnBytesRead	 [out]	Optional. Number of bytes actually read.
	 */
	XnStatus (XN_CALLBACK_TYPE* Read)(void* pCookie, void* pBuffer, XnUInt32 nSize, XnUInt32* pnBytesRead);

	/**
	 * Sets the stream's pointer to the specified position.
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	seekType	 [in]	Specifies how to seek - according to current position, end or beginning.
	 * @param	nOffset		 [in]	Specifies how many bytes to move
	 */
	XnStatus (XN_CALLBACK_TYPE* Seek)(void* pCookie, XnOSSeekType seekType, const XnInt32 nOffset);
	
	/**
	 * Tells the current stream position
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 *
	 * @returns (XnUInt32)-1 if there was an error in the stream.
	 */
	XnUInt32 (XN_CALLBACK_TYPE* Tell)(void* pCookie);

	/**
	 * Closes the stream.
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 */
	void (XN_CALLBACK_TYPE* Close)(void* pCookie);

	/**
	 * Sets the stream's pointer to the specified position. (64bit version, for large files)
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	seekType	 [in]	Specifies how to seek - according to current position, end or beginning.
	 * @param	nOffset		 [in]	Specifies how many bytes to move
	 */
	XnStatus (XN_CALLBACK_TYPE* Seek64)(void* pCookie, XnOSSeekType seekType, const XnInt64 nOffset);

	/**
	 * Tells the current position in the stream. (64bit version, for large files)
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	pPos		[out]	The position of the stream.
	 *
	 * @returns (XnUInt64)-1 on error.
	 */
	XnUInt64 (XN_CALLBACK_TYPE* Tell64)(void* pCookie);

} XnPlayerInputStreamInterface;

/** 
 * An interface that is used for notifications about node events.
 **/
typedef struct XnNodeNotifications
{
	/**
	 * Notifies the object that a production node was added
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The instance name of the added node.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeAdded)
		(void* pCookie, const XnChar* strNodeName, XnProductionNodeType type,
		XnCodecID compression);

	/**
	 * Notifies the object that a production node has been removed
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The instance name of the removed node.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeRemoved)
		(void* pCookie, const XnChar* strNodeName);

	/**
	 * Notifies the object that an integer type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	nValue		[in]	The new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeIntPropChanged)
		(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, XnUInt64 nValue);

	/**
	 * Notifies the object that a real type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	dValue		[in]	The new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeRealPropChanged)
		(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, XnDouble dValue);

	/**
	 * Notifies the object that a string type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	strValue	[in]	The new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeStringPropChanged)
		(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, const XnChar* strValue);

	/**
	 * Notifies the object that a general type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	nBufferSize	[in]	The size of the buffer that holds the new value.
	 * @param	pBuffer		[in]	The buffer that holds the new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeGeneralPropChanged)
		(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer);

	/**
	 * Notifies the object that a node has finished sending all the initial 'property changed' notifications.
	 * 
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose state is ready.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeStateReady)
		(void* pCookie, const XnChar* strNodeName);

	/**
	 * Notifies the object that it has received new data.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strName		[in]	The name of the property that changed.
	 * @param	nBufferSize	[in]	The size of the buffer that holds the new value.
	 * @param	pBuffer		[in]	The buffer that holds the new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeNewData)
		(void* pCookie, const XnChar* strNodeName,
		XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize);

} XnNodeNotifications;

/** A pair of 32-bit unsigned values. **/
typedef struct XnUInt32XYPair
{
	XnUInt32 X;
	XnUInt32 Y;
} XnUInt32XYPair;

/** Holds information about a general frame of data. **/
typedef struct XnOutputMetaData
{
	/** Represents the time in which the data was received. **/
	XnUInt64 nTimestamp;

	/** The frame ID, or frame number, of this frame. **/
	XnUInt32 nFrameID;
	
	/** The size of the data. **/
	XnUInt32 nDataSize;

	/** Specifies whether the generator updated this data on the last call to one of the XnWaitXXXUpdateXXX() functions. **/
	XnBool bIsNew;

} XnOutputMetaData;

/** Holds information about a frame of data that has a map format. **/
typedef struct XnMapMetaData
{
	/** A pointer to general information about this frame. **/
	XnOutputMetaData* pOutput;

	/** The resolution of this frame, including any cropping that was set. **/
	XnUInt32XYPair Res;
	
	/** The offset of the cropped region within this frame. Set to (0,0) if no cropping was done. **/
	XnUInt32XYPair Offset;
	
	/** The full resolution of this frame, disregarding cropping. **/
	XnUInt32XYPair FullRes;
	
	/** The pixel format of this frame. **/
	XnPixelFormat PixelFormat;
	
	/** The number of frames per second that was set when this frame was received. **/
	XnUInt32 nFPS;
} XnMapMetaData;

/** Holds information about a frame of depth. **/
typedef struct XnDepthMetaData
{
	/** A pointer to the map meta data of this frame. **/
	XnMapMetaData* pMap;
	
	/** A pointer to the depth data of this frame. **/
	const XnDepthPixel* pData;
	
	/** The value of the Z resolution of this frame - the maximum depth a pixel can have. **/
	XnDepthPixel nZRes;
} XnDepthMetaData;

/** Holds information about an image frame. **/
typedef struct XnImageMetaData
{
	/** A pointer to the map meta data of this frame **/
	XnMapMetaData* pMap;
	
	/** A pointer to the image data of this frame **/
	const XnUInt8* pData;
} XnImageMetaData;

/** Holds information about an IR frame. **/
typedef struct XnIRMetaData
{
	/** A pointer to the map meta data of this frame **/
	XnMapMetaData* pMap;
	
	/** A pointer to the IR data of this frame **/
	const XnIRPixel* pData;
} XnIRMetaData;

typedef struct XnAudioMetaData
{
	/** A pointer to general information about this chunk of audio. **/
	XnOutputMetaData* pOutput;

	/** The wave output mode of this cunk of audio. **/
	XnWaveOutputMode Wave;
	
	/** A pointer to audio data of this cunk. **/
	const XnUInt8* pData;
} XnAudioMetaData;

typedef struct XnSceneMetaData
{
	/** A pointer to the map meta data of this frame. **/
	XnMapMetaData* pMap;

	/** A pointer to the scene data of this frame. **/
	const XnLabel* pData;
} XnSceneMetaData;

#if XN_PLATFORM != XN_PLATFORM_ARC
#pragma pack (pop)
#endif

#endif //__XN_TYPES_H__
