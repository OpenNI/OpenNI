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
// Application Usage:
// 1 - Switch to the depth map view mode.
// 2 - Switch to the psychedelic depth map view mode. In this mode each centimeter will have a difference color.
// 3 - Switch to the psychedelic depth map view mode. In this mode each centimeter will have a difference color and millimeters will have different shades.
// 4 - Switch to the depth map with rainbow colors view mode.
// 5 - Switch to the depth map with RGB registration view mode.
// 6 - Switch to the depth map with RGB registration view mode and a background image.
// 7 - Switch to the side by side of depth and image view mode.
// 8 - Switch to the image map with RGB registration view mode.
// 9 - Switch to the image map with RGB registration view mode. In this mode the depth will be semi transparent.
// 0 - Switch to the image map with RGB registration view mode. In this mode the depth will be semi transparent and use rainbow colors.
// - - Switch to the image map with RGB registration view mode. In this mode the depth will be semi transparent and use depth values color codes.
// = - Switch to the image map only view mode.
// p - Show the laser pointer and the cutoff parameters.
// m - Enable/Disable the mirror mode.
// q - Decrease the minimum depth cutoff by 1.
// Q - Decrease the minimum depth cutoff by 50.
// w - Increase the minimum depth cutoff by 1.
// W - Increase the minimum depth cutoff by 50.
// e - Decrease the maximum depth cutoff by 1.
// E - Decrease the maximum depth cutoff by 50.
// r - Increase the maximum depth cutoff by 1.
// R - Increase the maximum depth cutoff by 50.
// ESC - exit.

// --------------------------------
// Includes
// --------------------------------
#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM)
	#define UNIX
	#define GLX_GLXEXT_LEGACY
#endif

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#define MACOS
#endif

#define GLH_EXT_SINGLE_FILE
#pragma warning(push, 3)
#include <glh/glh_obs.h>
#include <glh/glh_glut2.h>
#pragma warning(pop)

using namespace glh;

#include <XnLog.h>
#include "Device.h"
#include "Capture.h"
#include "Draw.h"
#include "Audio.h"
#include "Keyboard.h"
#include "Menu.h"
#include "Statistics.h"
#include "MouseInput.h"

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#include <conio.h>
	#include <direct.h>	
#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_MACOSX)
	#define _getch() getchar()
#endif

// --------------------------------
// Defines
// --------------------------------
#define SAMPLE_XML_PATH "../../../../Data/SamplesConfig.xml"

// --------------------------------
// Types
// --------------------------------
enum {
	ERR_OK,
	ERR_USAGE,
	ERR_DEVICE,
	ERR_UNKNOWN
};

// --------------------------------
// Global Variables
// --------------------------------
/* When true, frames will not be read from the device. */
bool g_bPause = false;
/* When true, only a single frame will be read, and then reading will be paused. */
bool g_bStep = false;

glut_perspective_reshaper reshaper;
glut_callbacks cb;

IntPair mouseLocation;
IntPair windowSize;

// --------------------------------
// Utilities
// --------------------------------
void MotionCallback(int x, int y)
{
	mouseInputMotion(int((double)x/windowSize.X*WIN_SIZE_X), int((double)y/windowSize.Y*WIN_SIZE_Y));
}

void MouseCallback(int button, int state, int x, int y)
{
	mouseInputButton(button, state, int((double)x/windowSize.X*WIN_SIZE_X), int((double)y/windowSize.Y*WIN_SIZE_Y));
}

void KeyboardCallback(unsigned char key, int /*x*/, int /*y*/)
{
	if (isCapturing())
	{
		captureStop(0);
	}
	else
	{
		handleKey(key);
	}

	glutPostRedisplay();
}

void ReshapeCallback(int width, int height)
{
	windowSize.X = width;
	windowSize.Y = height;
	windowReshaped(width, height);
}

void IdleCallback()
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (g_bPause != TRUE)
	{
		// read a frame
		readFrame();

		// play the audio
		audioPlay();

		// capture if needed
		nRetVal = captureFrame();
		if (nRetVal != XN_STATUS_OK)
		{
			displayMessage("Error capturing frame: '%s'", xnGetStatusString(nRetVal));
		}

		// add to statistics
		statisticsAddFrame();
	}

	if (g_bStep == TRUE)
	{
		g_bStep = FALSE;
		g_bPause = TRUE;
	}

	glutPostRedisplay();
}

void seek(int nDiff)
{
	if (!isPlayerOn())
	{
		displayMessage("Seeking is only supported in playback mode!");
		return;
	}

	seekFrame(nDiff);

	// now step the last one (that way, if seek is not supported, as in sensor, at least one frame
	// will be read).
	g_bPause = false;
	g_bStep = true;
}

void init_opengl()
{
	glClearStencil(128);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	GLfloat ambient[] = {0.5, 0.5, 0.5, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1f);
}

void closeSample(int errCode)
{
	captureStop(0);
	closeDevice();

	if (errCode != ERR_OK)
	{
		printf("Press any key to continue . . .\n");
		_getch();
	}

	exit(errCode);
}

void togglePause(int)
{
	g_bPause = !g_bPause;
}

void resetCropping(MapGenerator* pGenerator)
{
	XnCropping crop;
	crop.bEnabled = FALSE;
	crop.nXOffset = crop.nYOffset = 0;
	crop.nXSize = crop.nYSize = 0;
	setStreamCropping(pGenerator, &crop);
}

void resetDepthCropping(int)
{
	resetCropping(getDepthGenerator());
}

void resetImageCropping(int)
{
	resetCropping(getImageGenerator());
}

void resetIRCropping(int)
{
	resetCropping(getIRGenerator());
}

void resetAllCropping(int)
{
	if (getDepthGenerator() != NULL)
		resetDepthCropping(0);

	if (getImageGenerator() != NULL)
		resetImageCropping(0);

	if (getIRGenerator() != NULL)
		resetIRCropping(0);
}

void step(int)
{
	g_bStep = true;
	g_bPause = false;
}

void startCapture(int delay)
{
	if (g_bPause)
	{
		displayMessage("Cannot record when paused!");
	}
	else
	{
		captureStart(delay);
	}
}

void createKeyboardMap()
{
	startKeyboardMap();
	{
		startKeyboardGroup(KEYBOARD_GROUP_PRESETS);
		{
			registerKey('`', getPresetName(0), setPreset, 0);
			registerKey('1', getPresetName(1), setPreset, 1);
			registerKey('2', getPresetName(2), setPreset, 2);
			registerKey('3', getPresetName(3), setPreset, 3);
			registerKey('4', getPresetName(4), setPreset, 4);
			registerKey('5', getPresetName(5), setPreset, 5);
			registerKey('6', getPresetName(6), setPreset, 6);
			registerKey('7', getPresetName(7), setPreset, 7);
			registerKey('8', getPresetName(8), setPreset, 8);
			registerKey('9', getPresetName(9), setPreset, 9);
			registerKey('0', getPresetName(10), setPreset, 10);
			registerKey('-', getPresetName(11), setPreset, 11);
			registerKey('=', getPresetName(12), setPreset, 12);
		}
		endKeyboardGroup();
		startKeyboardGroup(KEYBOARD_GROUP_DEVICE);
		{
			registerKey('m', "Mirror on/off", toggleMirror, 0);
			registerKey('/', "Reset all croppings", resetAllCropping, 0);
		}
		endKeyboardGroup();
		startKeyboardGroup(KEYBOARD_GROUP_CAPTURE);
		{
			registerKey('s', "Start", startCapture, 0);
			registerKey('d', "Start (5 sec delay)", startCapture, 5);
			registerKey('x', "Stop", captureStop, 0);
			registerKey('c', "Capture current frame only", captureSingleFrame, 0);
		}
		endKeyboardGroup();
		startKeyboardGroup(KEYBOARD_GROUP_DISPLAY);
		{
			registerKey('p', "Pointer Mode On/Off", togglePointerMode, 0);
			registerKey('f', "Full Screen On/Off", toggleFullScreen, 0);
			registerKey('h', "Reset IR histogram", resetIRHistogram, 0);
			registerKey('?', "Show/Hide Help screen", toggleHelpScreen, 0);
		}
		endKeyboardGroup();
		startKeyboardGroup(KEYBOARD_GROUP_GENERAL);
		{
			registerKey('z', "Start/Stop Collecting Statistics", toggleStatistics, 0);
			registerKey(27, "Exit", closeSample, ERR_OK);
		}
		endKeyboardGroup();
		startKeyboardGroup(KEYBOARD_GROUP_PLAYER);
		{
			registerKey('o', "Pause/Resume", togglePause, 0);
			registerKey('l', "Jump 1 frame forward", seek, 1);
			registerKey('L', "Jump 10 frames forward", seek, 10);
			registerKey('k', "Jump 1 frame backwards", seek, -1);
			registerKey('K', "Jump 10 frames backwards", seek, -10);
			registerKey(';', "Read one frame", step, 0);
			registerKey('[', "Decrease playback speed", setPlaybackSpeed, -1);
			registerKey(']', "Increase playback speed", setPlaybackSpeed, 1);
		}
		endKeyboardGroup();
	}
	endKeyboardMap();
}

void createMenu()
{
	startMenu();
	{
		startSubMenu("View");
		{
			startSubMenu("Presets");
			{
				for (int i = 0; i < PRESET_COUNT; ++i)
				{
					createMenuEntry(getPresetName(i), setPreset, i);
				}
			}
			endSubMenu();
			startSubMenu("Screen Layout");
			{
				createMenuEntry("Side by Side", setScreenLayout, (int)SIDE_BY_SIDE);
				createMenuEntry("Overlay", setScreenLayout, (int)OVERLAY);
			}
			endSubMenu();
			startSubMenu("Depth");
			{
				for (int i = 0; i < NUM_OF_DEPTH_TYPES; ++i)
				{
					createMenuEntry(g_DepthColoring[i], setDepthDrawing, i);
				}
			}
			endSubMenu();
			startSubMenu("Image");
			{
				for (int i = 0; i < NUM_OF_IMAGE_TYPES; ++i)
				{
					createMenuEntry(g_ImageColoring[i], setImageDrawing, i);
				}
			}
			endSubMenu();
			createMenuEntry("Reset IR histogram", resetIRHistogram, 0);
			createMenuEntry("Pointer Mode On/Off", togglePointerMode, 0);
			createMenuEntry("Show/Hide Background", toggleBackground, 0);
			createMenuEntry("Show/Hide Help Screen", toggleHelpScreen, 0);
		}
		endSubMenu();
		startSubMenu("Device");
		{
			startSubMenu("Streams");
			{
				startSubMenu("Depth");
				{
					createMenuEntry("On/Off", toggleDepthState, 0);
					startSubMenu("Registration");
					{
						for (int i = 0; i < g_Registration.nValuesCount; ++i)
						{
							unsigned int nValue = g_Registration.pValues[i];
							createMenuEntry(g_Registration.pValueToName[nValue], changeRegistration, nValue);
						}
					}
					endSubMenu();
					startSubMenu("Resolution");
					{
						createMenuEntry("QQVGA", setDepthResolution, XN_RES_QQVGA);
						createMenuEntry("QVGA", setDepthResolution, XN_RES_QVGA);
						createMenuEntry("VGA", setDepthResolution, XN_RES_VGA);
					}
					endSubMenu();
					startSubMenu("FPS");
					{
						createMenuEntry("25", setDepthFPS, 25);
						createMenuEntry("30", setDepthFPS, 30);
						createMenuEntry("60", setDepthFPS, 60);
					}
					endSubMenu();
					createMenuEntry("Reset Cropping", resetDepthCropping, 0);
				}
				endSubMenu();
				startSubMenu("Image");
				{
					createMenuEntry("On/Off", toggleImageState, 0);
					startSubMenu("Resolution");
					{
						createMenuEntry("QVGA", setImageResolution, XN_RES_QVGA);
						createMenuEntry("VGA", setImageResolution, XN_RES_VGA);
						createMenuEntry("SXGA", setImageResolution, XN_RES_SXGA);
						createMenuEntry("UXGA", setImageResolution, XN_RES_UXGA);
					}
					endSubMenu();
					startSubMenu("FPS");
					{
						createMenuEntry("25", setImageFPS, 25);
						createMenuEntry("30", setImageFPS, 30);
						createMenuEntry("60", setImageFPS, 60);
					}
					endSubMenu();
					createMenuEntry("Reset Cropping", resetImageCropping, 0);
				}
				endSubMenu();
				startSubMenu("IR");
				{
					createMenuEntry("On/Off", toggleIRState, 0);
					startSubMenu("Resolution");
					{
						createMenuEntry("QVGA", setIRResolution, XN_RES_QVGA);
						createMenuEntry("VGA", setIRResolution, XN_RES_VGA);
						createMenuEntry("SXGA", setIRResolution, XN_RES_SXGA);
					}
					endSubMenu();
					startSubMenu("FPS");
					{
						createMenuEntry("25", setIRFPS, 25);
						createMenuEntry("30", setIRFPS, 30);
						createMenuEntry("60", setIRFPS, 60);
					}
					endSubMenu();
					createMenuEntry("Reset Cropping", resetIRCropping, 0);
				}
				endSubMenu();
				startSubMenu("Audio");
				{
					createMenuEntry("On/Off", toggleAudioState, 0);
				}
				endSubMenu();
				startSubMenu("Primary Stream");
				{
					for (int i = 0; i < g_PrimaryStream.nValuesCount; ++i)
					{
						createMenuEntry(g_PrimaryStream.pValues[i], changePrimaryStream, i);
					}
				}
				endSubMenu();
			}
			endSubMenu();

			createMenuEntry("Mirror", toggleMirror, 0);
		}
		endSubMenu();
		startSubMenu("Capture");
		{
			startSubMenu("Depth Format");
			{
				for (int i = 0; i < g_DepthFormat.nValuesCount; ++i)
				{
					unsigned int nValue = g_DepthFormat.pValues[i];
					createMenuEntry(g_DepthFormat.pIndexToName[i], captureSetDepthFormat, nValue);
				}
			}
			endSubMenu();
			startSubMenu("Image Format");
			{
				for (int i = 0; i < g_ImageFormat.nValuesCount; ++i)
				{
					unsigned int nValue = g_ImageFormat.pValues[i];
					createMenuEntry(g_ImageFormat.pIndexToName[i], captureSetImageFormat, nValue);
				}
			}
			endSubMenu();
			startSubMenu("IR Format");
			{
				for (int i = 0; i < g_IRFormat.nValuesCount; ++i)
				{
					unsigned int nValue = g_IRFormat.pValues[i];
					createMenuEntry(g_IRFormat.pIndexToName[i], captureSetIRFormat, nValue);
				}
			}
			endSubMenu();
			startSubMenu("Audio Format");
			{
				for (int i = 0; i < g_AudioFormat.nValuesCount; ++i)
				{
					unsigned int nValue = g_AudioFormat.pValues[i];
					createMenuEntry(g_AudioFormat.pIndexToName[i], captureSetAudioFormat, nValue);
				}
			}
			endSubMenu();
			createMenuEntry("Browse", captureBrowse, 0);
			createMenuEntry("Start", captureStart, 0);
			createMenuEntry("Start (5 sec delay)", captureStart, 5);
			createMenuEntry("Restart", captureRestart, 0);
			createMenuEntry("Stop", captureStop, 0);
		}
		endSubMenu();
		startSubMenu("Statistics");
		{
			createMenuEntry("Start Collecting", statisticsStart, 0);
			createMenuEntry("Stop Collecting", statisticsStop, 0);
			createMenuEntry("Clear", statisticsClear, 0);
		}
		endSubMenu();
		startSubMenu("Player");
		{
			createMenuEntry("Pause/Resume", togglePause, 0);
			createMenuEntry("Skip 1 frame forward", seek, 1);
			createMenuEntry("Skip 10 frame forward", seek, 10);
			createMenuEntry("Skip 1 frame backwards", seek, -1);
			createMenuEntry("Skip 10 frame backwards", seek, -10);
		}
		endSubMenu();
		createMenuEntry("Quit", closeSample, ERR_OK);
	}
	endMenu();
}

void onExit()
{
	captureStop(0);
}

int changeDirectory(char* arg0)
{
	// get dir name
	XnChar strDirName[XN_FILE_MAX_PATH];
	XnStatus nRetVal = xnOSGetDirName(arg0, strDirName, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	// now set current directory to it
	nRetVal = xnOSSetCurrentDir(strDirName);
	XN_IS_STATUS_OK(nRetVal);

	return 0;
}

int main(int argc, char **argv)
{
	XnBool bChooseDevice = false;
	const char* csRecordingName = NULL;

	if (argc > 1)
	{
		if (strcmp(argv[1], "-devices") == 0)
		{
			bChooseDevice = TRUE;
		}
		else
		{
			csRecordingName = argv[1];
		}
	}

	if (csRecordingName != NULL)
	{	
		// check if running from a different directory. If so, we need to change directory
		// to the real one, so that path to INI file will be OK (for log initialization, for example)
		if (0 != changeDirectory(argv[0]))
		{
			return(ERR_DEVICE);
		}
	}

	// Xiron Init
	XnStatus rc = XN_STATUS_OK;
	EnumerationErrors errors;

	if (csRecordingName != NULL)
	{	
		xnLogInitFromXmlFile(SAMPLE_XML_PATH);
		rc = openDeviceFile(argv[1]);
	}
	else if (bChooseDevice)
	{
		rc = openDeviceFromXmlWithChoice(SAMPLE_XML_PATH, errors);
	}
	else
	{
		rc = openDeviceFromXml(SAMPLE_XML_PATH, errors);
	}

	if (rc == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		closeSample(ERR_DEVICE);
		return (rc);
	}
	else if (rc != XN_STATUS_OK)
	{
		printf("Open failed: %s\n", xnGetStatusString(rc));
		closeSample(ERR_DEVICE);
	}

	audioInit();
	captureInit();
	statisticsInit();

	reshaper.zNear = 1;
	reshaper.zFar = 100;
	glut_add_interactor(&reshaper);

	cb.mouse_function = MouseCallback;
	cb.motion_function = MotionCallback;
	cb.passive_motion_function = MotionCallback;
	cb.keyboard_function = KeyboardCallback;
	cb.reshape_function = ReshapeCallback;
	glut_add_interactor(&cb);

	glutInit(&argc, argv);
	glutInitDisplayString("stencil double rgb");
	glutInitWindowSize(WIN_SIZE_X, WIN_SIZE_Y);
	glutCreateWindow("OpenNI Viewer");
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	init_opengl();

	glut_helpers_initialize();

	glutIdleFunc(IdleCallback);
	glutDisplayFunc(drawFrame);

	drawInit();
	createKeyboardMap();
	createMenu();

	atexit(onExit);
	
	// Per frame code is in drawFrame()
	glutMainLoop();

	audioShutdown();

	closeSample(ERR_OK);

	return (ERR_OK);
}
