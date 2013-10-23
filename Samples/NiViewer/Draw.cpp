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
// --------------------------------
// Includes
// --------------------------------
#include "Draw.h"
#include "Device.h"
#include "Keyboard.h"
#include "Capture.h"
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
	#include <GL/glut.h>
#endif
#include "Statistics.h"
#include "MouseInput.h"

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#ifdef __INTEL_COMPILER
		#include <ia32intrin.h>
	#else
		#include <intrin.h>
	#endif
#endif

// --------------------------------
// Defines
// --------------------------------
#define YUV422_U  0
#define YUV422_Y1 1
#define YUV422_V  2
#define YUV422_Y2 3
#define YUV422_BPP 4
#define YUV_RED   0
#define YUV_GREEN 1
#define YUV_BLUE  2
#define YUV_ALPHA  3
#define YUV_RGBA_BPP 4

// --------------------------------
// Types
// --------------------------------
typedef struct  
{
	StreamsDrawConfig Streams; 
	bool bShowPointer;
	bool bShowMessage;
	bool bHelp;
	const XnChar* strErrorState;
	IntRect DepthLocation;
	IntRect ImageLocation;
} DrawConfig;

typedef struct
{
	const char* csName;
	StreamsDrawConfig Config;
} DrawConfigPreset;

typedef struct XnTextureMap
{
	IntPair Size;
	IntPair OrigSize;
	unsigned char* pMap;
	unsigned int nBytesPerPixel;
	GLuint nID;
	GLenum nFormat;
	bool bInitialized;
	IntPair CurSize;
} XnTextureMap;

// --------------------------------
// Global Variables
// --------------------------------
DrawConfig g_DrawConfig;

XnUInt8 PalletIntsR [256] = {0};
XnUInt8 PalletIntsG [256] = {0};
XnUInt8 PalletIntsB [256] = {0};

unsigned short g_nMaxGrayscale16Value = 0;
/* Linear Depth Histogram */
float* g_pDepthHist;

const char* g_DepthColoring[NUM_OF_DEPTH_TYPES];
const char* g_ImageColoring[NUM_OF_IMAGE_TYPES];

typedef struct DrawUserInput
{
	SelectionState State;
	IntRect Rect;
	IntPair Cursor;
} DrawUserInput;

DrawUserInput g_DrawUserInput;

int g_nMaxDepth = 0;

DrawConfigPreset g_Presets[PRESET_COUNT] = 
{
	// NAME,								BACKGRD, { Depth_Type, Transparency}, { Image_Type  }			Arrangement }}
	{ "Standard Deviation",					{ false, { STANDARD_DEVIATION,	1 }, { IMAGE_OFF },				OVERLAY } },
	{ "Depth Histogram",					{ false, { LINEAR_HISTOGRAM,	1 }, { IMAGE_OFF },				OVERLAY } },
	{ "Psychedelic Depth [Centimeters]",	{ false, { PSYCHEDELIC,			1 }, { IMAGE_OFF },				OVERLAY } },
	{ "Psychedelic Depth [Millimeters]",	{ false, { PSYCHEDELIC_SHADES,	1 }, { IMAGE_OFF },				OVERLAY } },
	{ "Rainbow Depth",						{ false, { CYCLIC_RAINBOW_HISTOGRAM,1 },{ IMAGE_OFF },			OVERLAY } },
	{ "Depth masked Image",					{ false, { DEPTH_OFF,			1 }, { DEPTH_MASKED_IMAGE },	OVERLAY } },
	{ "Background Removal",					{ true,	 { DEPTH_OFF,			1 }, { DEPTH_MASKED_IMAGE },	OVERLAY } },
	{ "Side by Side",						{ false, { LINEAR_HISTOGRAM,	1 }, { IMAGE_NORMAL },			SIDE_BY_SIDE } },
	{ "Depth on Image",						{ false, { LINEAR_HISTOGRAM,	1 }, { IMAGE_NORMAL },			OVERLAY } },
	{ "Transparent Depth on Image",			{ false, { LINEAR_HISTOGRAM,  0.6 }, { IMAGE_NORMAL },			OVERLAY } },
	{ "Rainbow Depth on Image",				{ false, { RAINBOW,			  0.6 }, { IMAGE_NORMAL },			OVERLAY } },
	{ "Cyclic Rainbow Depth on Image",		{ false, { CYCLIC_RAINBOW,	  0.6 }, { IMAGE_NORMAL },			OVERLAY } },
	{ "Image Only",							{ false, { DEPTH_OFF,			1 }, { IMAGE_NORMAL },			OVERLAY } },
};

/* Texture maps for depth and image */
XnTextureMap g_texDepth = {0};
XnTextureMap g_texImage = {0};
XnTextureMap g_texBackground = {0};

/* A user message to be displayed. */
char g_csUserMessage[256];

bool g_bFullScreen = true;
bool g_bFirstTimeNonFull = true;
IntPair g_NonFullWinSize = { WIN_SIZE_X, WIN_SIZE_Y };

// --------------------------------
// Textures
// --------------------------------
int GetPowerOfTwo(int num)
{
	int result = 1;

	while (result < num)
		result <<= 1;

	return result;
}

void TextureMapInit(XnTextureMap* pTex, int nSizeX, int nSizeY, unsigned int nBytesPerPixel, int nCurX, int nCurY)
{
	// check if something changed
	if (pTex->bInitialized && pTex->OrigSize.X == nSizeX && pTex->OrigSize.Y == nSizeY)
	{
		if (pTex->CurSize.X != nCurX || pTex->CurSize.Y != nCurY)
		{
			// clear map
			xnOSMemSet(pTex->pMap, 0, pTex->Size.X * pTex->Size.Y * pTex->nBytesPerPixel);

			// update
			pTex->CurSize.X = nCurX;
			pTex->CurSize.Y = nCurY;
			return;
		}
	}

	// free memory if it was allocated
	if (pTex->pMap != NULL)
		delete pTex->pMap;

	// update it all
	pTex->OrigSize.X = nSizeX;
	pTex->OrigSize.Y = nSizeY;
	pTex->Size.X = GetPowerOfTwo(nSizeX);
	pTex->Size.Y = GetPowerOfTwo(nSizeY);
	pTex->nBytesPerPixel = nBytesPerPixel;
	pTex->CurSize.X = nCurX;
	pTex->CurSize.Y = nCurY;
	pTex->pMap = new unsigned char[pTex->Size.X * pTex->Size.Y * nBytesPerPixel];
	xnOSMemSet(pTex->pMap, 0, pTex->Size.X * pTex->Size.Y * nBytesPerPixel);
	
	if (!pTex->bInitialized)
	{
		glGenTextures(1, &pTex->nID);
		glBindTexture(GL_TEXTURE_2D, pTex->nID);

		switch (pTex->nBytesPerPixel)
		{
		case 3:
			pTex->nFormat = GL_RGB;
			break;
		case 4:
			pTex->nFormat = GL_RGBA;
			break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		pTex->bInitialized = TRUE;
	}
}

inline unsigned char* TextureMapGetLine(XnTextureMap* pTex, unsigned int nLine)
{
	return &pTex->pMap[nLine * pTex->Size.X * pTex->nBytesPerPixel];
}

void TextureMapSetPixel(XnTextureMap* pTex, int x, int y, int red, int green, int blue)
{
	if (x < 0 || y < 0 || x >= (int)pTex->OrigSize.X || y >= (int)pTex->OrigSize.Y)
		return;

	unsigned char* pPixel = TextureMapGetLine(pTex, y) + x * pTex->nBytesPerPixel;
	pPixel[0] = red;
	pPixel[1] = green;
	pPixel[2] = blue;

	if (pTex->nBytesPerPixel > 3)
		pPixel[3] = 255;
}

void TextureMapDrawCursor(XnTextureMap* pTex, IntPair cursor, int red = 255, int green = 0, int blue = 0)
{
	// marked pixel
	TextureMapSetPixel(pTex, cursor.X, cursor.Y, red, green, 0);

	// top left marker
	TextureMapSetPixel(pTex, cursor.X-2, cursor.Y-2, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X-2, cursor.Y-1, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X-1, cursor.Y-2, red, green, blue);

	// top right marker
	TextureMapSetPixel(pTex, cursor.X+2, cursor.Y-2, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X+2, cursor.Y-1, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X+1, cursor.Y-2, red, green, blue);

	// bottom left marker
	TextureMapSetPixel(pTex, cursor.X-2, cursor.Y+2, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X-2, cursor.Y+1, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X-1, cursor.Y+2, red, green, blue);

	// bottom right marker
	TextureMapSetPixel(pTex, cursor.X+2, cursor.Y+2, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X+2, cursor.Y+1, red, green, blue);
	TextureMapSetPixel(pTex, cursor.X+1, cursor.Y+2, red, green, blue);
}

void TextureMapUpdate(XnTextureMap* pTex)
{
	// set current texture object
	glBindTexture(GL_TEXTURE_2D, pTex->nID);

	// set the current image to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, pTex->nFormat, pTex->Size.X, pTex->Size.Y, 0, pTex->nFormat, GL_UNSIGNED_BYTE, pTex->pMap);
}

void TextureMapDraw(XnTextureMap* pTex, IntRect* pLocation)
{
	// set current texture object
	glBindTexture(GL_TEXTURE_2D, pTex->nID);

	// turn on texture mapping
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set drawing mode to rectangles
	glBegin(GL_QUADS);

	// set the color of the polygon
	glColor4f(1, 1, 1, 1);

	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(pLocation->uLeft, pLocation->uBottom);
	// upper right
	glTexCoord2f((float)pTex->OrigSize.X/(float)pTex->Size.X, 0);
	glVertex2f(pLocation->uRight, pLocation->uBottom);
	// bottom right
	glTexCoord2f((float)pTex->OrigSize.X/(float)pTex->Size.X, (float)pTex->OrigSize.Y/(float)pTex->Size.Y);
	glVertex2f(pLocation->uRight, pLocation->uTop);
	// bottom left
	glTexCoord2f(0, (float)pTex->OrigSize.Y/(float)pTex->Size.Y);
	glVertex2f(pLocation->uLeft, pLocation->uTop);

	glEnd();

	// turn off texture mapping
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
}

// --------------------------------
// Code
// --------------------------------
void CreateRainbowPallet()
{
	unsigned char r, g, b;
	for (int i=1; i<255; i++)
	{
		if (i<=29)
		{
			r = (unsigned char)(129.36-i*4.36);
			g = 0;
			b = (unsigned char)255;
		}
		else if (i<=86)
		{
			r = 0;
			g = (unsigned char)(-133.54+i*4.52);
			b = (unsigned char)255;
		}
		else if (i<=141)
		{
			r = 0;
			g = (unsigned char)255;
			b = (unsigned char)(665.83-i*4.72);
		}
		else if (i<=199)
		{
			r = (unsigned char)(-635.26+i*4.47);
			g = (unsigned char)255;
			b = 0;
		}
		else
		{
			r = (unsigned char)255;
			g = (unsigned char)(1166.81-i*4.57);
			b = 0;
		}

		PalletIntsR[i] = r;
		PalletIntsG[i] = g;
		PalletIntsB[i] = b;
	}
}

void glPrintString(void *font, const char *str)
{
	int i,l = (int)strlen(str);

	for(i=0; i<l; i++)
	{
		glutBitmapCharacter(font,*str++);
	}
}

void drawConfigChanged()
{
	// recalculate registration
	bool bRegistration = 
		(g_DrawConfig.Streams.ScreenArrangement == OVERLAY) && 
		(g_DrawConfig.Streams.Image.Coloring != IMAGE_OFF) &&
		(g_DrawConfig.Streams.Depth.Coloring != DEPTH_OFF || g_DrawConfig.Streams.Image.Coloring == DEPTH_MASKED_IMAGE);

	changeRegistration(bRegistration);
}

void setPreset(int preset)
{
	g_DrawConfig.Streams = g_Presets[preset].Config;
	drawConfigChanged();
}

const char* getPresetName(int preset)
{
	return g_Presets[preset].csName;
}

void setScreenLayout(int layout)
{
	g_DrawConfig.Streams.ScreenArrangement = (ScreenArrangementType)layout;
	drawConfigChanged();
}

void windowReshaped(int width, int height)
{
	g_NonFullWinSize.X = width;
	g_NonFullWinSize.Y = height;
}

void toggleFullScreen(int)
{
	if (g_bFullScreen)
	{
		if (g_bFirstTimeNonFull)
		{
			g_NonFullWinSize.X = WIN_SIZE_X/2;
			g_NonFullWinSize.Y = WIN_SIZE_Y/2;
			g_bFirstTimeNonFull = false;
		}

		glutReshapeWindow(g_NonFullWinSize.X, g_NonFullWinSize.Y);
		g_bFullScreen = false;
	}
	else
	{
		glutFullScreen();
		g_bFullScreen = true;
	}
}

void displayMessage(const char* csFormat, ...)
{
	g_DrawConfig.bShowMessage = true;
	va_list args;
	va_start(args, csFormat);
	vsprintf(g_csUserMessage, csFormat, args);
	va_end(args);
}

void setErrorState(const char* strMessage)
{
	g_DrawConfig.strErrorState = strMessage;
}

void drawCropStream(MapGenerator* pGenerator, IntRect location, IntRect selection, int dividedBy)
{
	if (!pGenerator->IsCapabilitySupported(XN_CAPABILITY_CROPPING))
	{
		return;
	}

	XnMapOutputMode Mode;
	pGenerator->GetMapOutputMode(Mode);

	// check if entire selection is in location
	if (selection.uLeft >= location.uLeft &&
		selection.uRight <= location.uRight &&
		selection.uBottom >= location.uBottom &&
		selection.uTop <= location.uTop)
	{
		IntRect cropRect;
		cropRect.uBottom = Mode.nYRes * (selection.uBottom - location.uBottom) / (location.uTop - location.uBottom);
		cropRect.uTop = Mode.nYRes * (selection.uTop - location.uBottom) / (location.uTop - location.uBottom);
		cropRect.uLeft = Mode.nXRes * (selection.uLeft - location.uLeft) / (location.uRight - location.uLeft);
		cropRect.uRight = Mode.nXRes * (selection.uRight - location.uLeft) / (location.uRight - location.uLeft);

		XnCropping cropping;
		cropping.bEnabled = TRUE;
		cropping.nXOffset = cropRect.uLeft;
		cropping.nYOffset = cropRect.uBottom;
		cropping.nXSize = cropRect.uRight - cropRect.uLeft;
		cropping.nYSize = cropRect.uTop	- cropRect.uBottom;

		if ((cropping.nXOffset % dividedBy) != 0)
			cropping.nXOffset -= (cropping.nXOffset % dividedBy);
		if ((cropping.nXSize % dividedBy) != 0)
			cropping.nXSize += dividedBy - (cropping.nXSize % dividedBy);

		setStreamCropping(pGenerator, &cropping);
	}
}

void drawSelectionChanged(SelectionState state, IntRect selection)
{
	g_DrawUserInput.State = state;
	g_DrawUserInput.Rect = selection;

	if (state == SELECTION_DONE)
	{
		// Crop depth
		if (getDepthGenerator() != NULL && g_DrawConfig.Streams.Depth.Coloring != DEPTH_OFF)
		{
			drawCropStream(getDepthGenerator(), g_DrawConfig.DepthLocation, selection, 2);
		}

		// Crop image
		if (getImageGenerator() != NULL && g_DrawConfig.Streams.Image.Coloring != IMAGE_OFF)
		{
			drawCropStream(getImageGenerator(), g_DrawConfig.ImageLocation, selection, 4);
		}

		// Crop IR
		if (getIRGenerator() != NULL && g_DrawConfig.Streams.Image.Coloring != IMAGE_OFF)
		{
			drawCropStream(getIRGenerator(), g_DrawConfig.ImageLocation, selection, 4);
		}
	}
}

void drawCursorMoved(IntPair location)
{
	g_DrawUserInput.Cursor = location;
}

void drawInit()
{
	g_DepthColoring[DEPTH_OFF] = "Off";
	g_DepthColoring[LINEAR_HISTOGRAM] = "Linear Histogram";
	g_DepthColoring[PSYCHEDELIC] = "Psychedelic";
	g_DepthColoring[PSYCHEDELIC_SHADES] = "Psychedelic (Millimeters)";
	g_DepthColoring[RAINBOW] = "Rainbow";
	g_DepthColoring[CYCLIC_RAINBOW] = "Cyclic Rainbow";
	g_DepthColoring[CYCLIC_RAINBOW_HISTOGRAM] = "Cyclic Rainbow Histogram";
	g_DepthColoring[STANDARD_DEVIATION] = "Standard Deviation";

	g_ImageColoring[IMAGE_OFF] = "Off";
	g_ImageColoring[IMAGE_NORMAL] = "Normal";
	g_ImageColoring[DEPTH_MASKED_IMAGE] = "Depth Masked Image";

	CreateRainbowPallet();

	setPreset(7);

	TextureMapInit(&g_texBackground, 1024, 1024, 3, 1024, 1024);

	// load background image
	xnOSLoadFile("..\\..\\..\\..\\Data\\RGBViewer\\back.raw", TextureMapGetLine(&g_texBackground, 0), 1024*1024*3);

	TextureMapUpdate(&g_texBackground);

	mouseInputRegisterForSelectionRectangle(drawSelectionChanged);
	mouseInputRegisterForCursorMovement(drawCursorMoved);
}

void togglePointerMode(int)
{
	g_DrawConfig.bShowPointer = !g_DrawConfig.bShowPointer;
}

void toggleHelpScreen(int)
{
	g_DrawConfig.bHelp = !g_DrawConfig.bHelp;
}

void toggleBackground(int)
{
	g_DrawConfig.Streams.bBackground = !g_DrawConfig.Streams.bBackground;
}

void calculateHistogram()
{
	DepthGenerator* pDepthGen = getDepthGenerator();

	if (pDepthGen == NULL)
		return;

	XnUInt32 nZRes = pDepthGen->GetDeviceMaxDepth() + 1;
	if (g_pDepthHist == NULL)
	{
		g_pDepthHist = new float[nZRes];
	}

	xnOSMemSet(g_pDepthHist, 0, nZRes*sizeof(float));
	int nNumberOfPoints = 0;

	XnDepthPixel nValue;

	const XnDepthPixel* pDepth = pDepthGen->GetDepthMap();
	const XnDepthPixel* pDepthEnd = pDepth + (pDepthGen->GetDataSize() / sizeof(XnDepthPixel));

	while (pDepth != pDepthEnd)
	{
		nValue = *pDepth;

		XN_ASSERT(nValue <= nZRes);

		if (nValue != 0)
		{
			g_pDepthHist[nValue]++;
			nNumberOfPoints++;
		}

		pDepth++;
	}

	XnUInt32 nIndex;
	for (nIndex = 1; nIndex < nZRes; nIndex++)
	{
		g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
	}
	for (nIndex = 1; nIndex < nZRes; nIndex++)
	{
		if (g_pDepthHist[nIndex] != 0)
		{
			g_pDepthHist[nIndex] = (nNumberOfPoints-g_pDepthHist[nIndex]) / nNumberOfPoints;
		}
	}
}

// --------------------------------
// Drawing
// --------------------------------
#if (XN_PLATFORM == XN_PLATFORM_WIN32)

void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBAImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize)
{
	const XnUInt8* pYUVLast = pYUVImage + nYUVSize - 8;
	XnUInt8* pRGBLast = pRGBAImage + nRGBSize - 16;

	const __m128 minus128 = _mm_set_ps1(-128);
	const __m128 plus113983 = _mm_set_ps1(1.13983F);
	const __m128 minus039466 = _mm_set_ps1(-0.39466F);
	const __m128 minus058060 = _mm_set_ps1(-0.58060F);
	const __m128 plus203211 = _mm_set_ps1(2.03211F);
	const __m128 zero = _mm_set_ps1(0);
	const __m128 plus255 = _mm_set_ps1(255);

	// define YUV floats
	__m128 y;
	__m128 u;
	__m128 v;

	__m128 temp;

	// define RGB floats
	__m128 r;
	__m128 g;
	__m128 b;

	// define RGB integers
	__m128i iR;
	__m128i iG;
	__m128i iB;

	XnUInt32* piR = (XnUInt32*)&iR;
	XnUInt32* piG = (XnUInt32*)&iG;
	XnUInt32* piB = (XnUInt32*)&iB;

	while (pYUVImage <= pYUVLast && pRGBAImage <= pRGBLast)
	{
		// process 4 pixels at once (values should be ordered backwards)
		y = _mm_set_ps(pYUVImage[YUV422_Y2 + YUV422_BPP], pYUVImage[YUV422_Y1 + YUV422_BPP], pYUVImage[YUV422_Y2], pYUVImage[YUV422_Y1]);
		u = _mm_set_ps(pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U],  pYUVImage[YUV422_U]);
		v = _mm_set_ps(pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V],  pYUVImage[YUV422_V]);

		u = _mm_add_ps(u, minus128); // u -= 128
		v = _mm_add_ps(v, minus128); // v -= 128

		/*

		http://en.wikipedia.org/wiki/YUV

		From YUV to RGB:
		R =     Y + 1.13983 V
		G =     Y - 0.39466 U - 0.58060 V
		B =     Y + 2.03211 U

		*/ 

		temp = _mm_mul_ps(plus113983, v);
		r = _mm_add_ps(y, temp);

		temp = _mm_mul_ps(minus039466, u);
		g = _mm_add_ps(y, temp);
		temp = _mm_mul_ps(minus058060, v);
		g = _mm_add_ps(g, temp);

		temp = _mm_mul_ps(plus203211, u);
		b = _mm_add_ps(y, temp);

		// make sure no value is smaller than 0
		r = _mm_max_ps(r, zero);
		g = _mm_max_ps(g, zero);
		b = _mm_max_ps(b, zero);

		// make sure no value is bigger than 255
		r = _mm_min_ps(r, plus255);
		g = _mm_min_ps(g, plus255);
		b = _mm_min_ps(b, plus255);

		// convert floats to int16 (there is no conversion to uint8, just to int8).
		iR = _mm_cvtps_epi32(r);
		iG = _mm_cvtps_epi32(g);
		iB = _mm_cvtps_epi32(b);

		// extract the 4 pixels RGB values.
		// because we made sure values are between 0 and 255, we can just take the lower byte
		// of each INT16
		pRGBAImage[0] = piR[0];
		pRGBAImage[1] = piG[0];
		pRGBAImage[2] = piB[0];
		pRGBAImage[3] = 255;

		pRGBAImage[4] = piR[1];
		pRGBAImage[5] = piG[1];
		pRGBAImage[6] = piB[1];
		pRGBAImage[7] = 255;

		pRGBAImage[8] = piR[2];
		pRGBAImage[9] = piG[2];
		pRGBAImage[10] = piB[2];
		pRGBAImage[11] = 255;

		pRGBAImage[12] = piR[3];
		pRGBAImage[13] = piG[3];
		pRGBAImage[14] = piB[3];
		pRGBAImage[15] = 255;

		// advance the streams
		pYUVImage += 8;
		pRGBAImage += 16;
	}
}

#else // not Win32

void YUV444ToRGBA(XnUInt8 cY, XnUInt8 cU, XnUInt8 cV,
					XnUInt8& cR, XnUInt8& cG, XnUInt8& cB, XnUInt8& cA)
{
	XnInt32 nC = cY - 16;
	XnInt16 nD = cU - 128;
	XnInt16 nE = cV - 128;

	nC = nC * 298 + 128;

	cR = XN_MIN(XN_MAX((nC            + 409 * nE) >> 8, 0), 255);
	cG = XN_MIN(XN_MAX((nC - 100 * nD - 208 * nE) >> 8, 0), 255);
	cB = XN_MIN(XN_MAX((nC + 516 * nD           ) >> 8, 0), 255);
	cA = 255;
}

void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize)
{
	const XnUInt8* pCurrYUV = pYUVImage;
	XnUInt8* pCurrRGB = pRGBImage;
	const XnUInt8* pLastYUV = pYUVImage + nYUVSize - YUV422_BPP;
	XnUInt8* pLastRGB = pRGBImage + nRGBSize - YUV_RGBA_BPP;

	while (pCurrYUV <= pLastYUV && pCurrRGB <= pLastRGB)
	{
		YUV444ToRGBA(pCurrYUV[YUV422_Y1], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE], pCurrRGB[YUV_ALPHA]);
		pCurrRGB += YUV_RGBA_BPP;
		YUV444ToRGBA(pCurrYUV[YUV422_Y2], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE], pCurrRGB[YUV_ALPHA]);
		pCurrRGB += YUV_RGBA_BPP;
		pCurrYUV += YUV422_BPP;
	}
}

#endif

void drawClosedStream(IntRect* pLocation, const char* csStreamName)
{
	char csMessage[512];
	sprintf(csMessage, "%s stream is OFF", csStreamName);
	void* pFont = GLUT_BITMAP_TIMES_ROMAN_24;

	int nWidth = glutBitmapLength(pFont, (const unsigned char*)csMessage);
	int nXLocation = (pLocation->uRight + pLocation->uLeft - nWidth) / 2;
	int nYLocation = (pLocation->uTop + pLocation->uBottom) / 2;

	glColor3f(1.0, 0, 0);
	glRasterPos2i(nXLocation, nYLocation);
	glPrintString(pFont, csMessage);
}

void drawColorImage(IntRect* pLocation, IntPair* pPointer, int pointerRed, int pointerGreen, int pointerBlue)
{
	if (g_DrawConfig.Streams.bBackground)
		TextureMapDraw(&g_texBackground, pLocation);

	if (g_DrawConfig.Streams.Image.Coloring == IMAGE_OFF)
		return;

	if (!isImageOn() && !isIROn())
	{
		drawClosedStream(pLocation, "Image");
		return;
	}

	const MapMetaData* pImageMD;
	const XnUInt8* pImage = NULL;

	if (isImageOn())
	{
		pImageMD = getImageMetaData();
		pImage = getImageMetaData()->Data();
	}
	else if (isIROn())
	{
		pImageMD = getIRMetaData();
		pImage = (const XnUInt8*)getIRMetaData()->Data();
	}
	else
		return;

	if (pImageMD->FrameID() == 0)
	{
		return;
	}

	const DepthMetaData* pDepthMetaData = getDepthMetaData();

	double grayscale16Factor = 1.0;
	if (pImageMD->PixelFormat() == XN_PIXEL_FORMAT_GRAYSCALE_16_BIT)
	{
		int nPixelsCount = pImageMD->XRes()*pImageMD->YRes();
		XnUInt16* pPixel = (XnUInt16*)pImage;
		for (int i = 0; i < nPixelsCount; ++i,++pPixel)
		{
			if (*pPixel > g_nMaxGrayscale16Value)
				g_nMaxGrayscale16Value = *pPixel;
		}

		if (g_nMaxGrayscale16Value > 0)
		{
			grayscale16Factor = 255.0 / g_nMaxGrayscale16Value;
		}
	}

	for (XnUInt16 nY = pImageMD->YOffset(); nY < pImageMD->YRes() + pImageMD->YOffset(); nY++)
	{
		XnUInt8* pTexture = TextureMapGetLine(&g_texImage, nY) + pImageMD->XOffset()*4;

		if (pImageMD->PixelFormat() == XN_PIXEL_FORMAT_YUV422)
		{
			YUV422ToRGB888(pImage, pTexture, pImageMD->XRes()*2, g_texImage.Size.X*g_texImage.nBytesPerPixel);
			pImage += pImageMD->XRes()*2;
		}
		else
		{
			for (XnUInt16 nX = 0; nX < pImageMD->XRes(); nX++, pTexture+=4)
			{
				XnInt32 nDepthIndex = 0;

				if (pDepthMetaData != NULL)
				{
					XnDouble dRealX = (nX + pImageMD->XOffset()) / (XnDouble)pImageMD->FullXRes();
					XnDouble dRealY = nY / (XnDouble)pImageMD->FullYRes();

					XnUInt32 nDepthX = dRealX * pDepthMetaData->FullXRes() - pDepthMetaData->XOffset();
					XnUInt32 nDepthY = dRealY * pDepthMetaData->FullYRes() - pDepthMetaData->YOffset();

					if (nDepthX >= pDepthMetaData->XRes() || nDepthY >= pDepthMetaData->YRes())
					{
						nDepthIndex = -1;
					}
					else
					{
						nDepthIndex = nDepthY*pDepthMetaData->XRes() + nDepthX;
					}
				}

				switch (pImageMD->PixelFormat())
				{
				case XN_PIXEL_FORMAT_RGB24:
					pTexture[0] = pImage[0];
					pTexture[1] = pImage[1];
					pTexture[2] = pImage[2];
					pImage+=3; 
					break;
				case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
					pTexture[0] = pTexture[1] = pTexture[2] = *pImage;
					pImage+=1; 
					break;
				case XN_PIXEL_FORMAT_GRAYSCALE_16_BIT:
					XnUInt16* p16 = (XnUInt16*)pImage;
					XnUInt8 textureValue = 0;
					textureValue = (XnUInt8)((*p16) * grayscale16Factor);
					pTexture[0] = pTexture[1] = pTexture[2] = textureValue;
					pImage+=2; 
					break;
				}

				// decide if pixel should be lit or not
				if (g_DrawConfig.Streams.Image.Coloring == DEPTH_MASKED_IMAGE &&
					(pDepthMetaData == NULL || nDepthIndex == -1 || pDepthMetaData->Data()[nDepthIndex] == 0))
				{
					pTexture[3] = 0;
				}
				else
				{
					pTexture[3] = 255;
				}
			}
		}
	}

	if (pPointer != NULL)
	{
		TextureMapDrawCursor(&g_texImage, *pPointer, pointerRed, pointerGreen, pointerBlue);
	}

	TextureMapUpdate(&g_texImage);
	TextureMapDraw(&g_texImage, pLocation);
}

void drawDepth(IntRect* pLocation, IntPair* pPointer)
{
	if (g_DrawConfig.Streams.Depth.Coloring != DEPTH_OFF)
	{
		if (!isDepthOn())
		{
			drawClosedStream(pLocation, "Depth");
			return;
		}

		const DepthMetaData* pDepthMD = getDepthMetaData();
		const XnDepthPixel* pDepth = pDepthMD->Data();
		XN_ASSERT(pDepth);
		
		if (pDepthMD->FrameID() == 0)
		{
			return;
		}

		if (g_DrawConfig.Streams.Depth.Coloring == STANDARD_DEVIATION)
		{
			XnPixelStatistics* pStatistics = g_PixelStatistics;

			for (XnUInt16 nY = pDepthMD->YOffset(); nY < pDepthMD->YRes() + pDepthMD->YOffset(); nY++)
			{
				XnUInt8* pTexture = TextureMapGetLine(&g_texDepth, nY) + pDepthMD->XOffset()*4;
				for (XnUInt16 nX = 0; nX < pDepthMD->XRes(); nX++, pTexture+=4, pStatistics++)
				{
					pTexture[0] = pTexture[1] = XN_MIN((int)pStatistics->dStdDev, 255);
					pTexture[2] = 0;
					pTexture[3] = g_DrawConfig.Streams.Depth.fTransparency*255;
				}
			}
		}
		else
		{
			// copy depth into texture-map
			for (XnUInt16 nY = pDepthMD->YOffset(); nY < pDepthMD->YRes() + pDepthMD->YOffset(); nY++)
			{
				XnUInt8* pTexture = TextureMapGetLine(&g_texDepth, nY) + pDepthMD->XOffset()*4;
				for (XnUInt16 nX = 0; nX < pDepthMD->XRes(); nX++, pDepth++, pTexture+=4)
				{
					XnUInt8 nRed = 0;
					XnUInt8 nGreen = 0;
					XnUInt8 nBlue = 0;
					XnUInt8 nAlpha = g_DrawConfig.Streams.Depth.fTransparency*255;

					XnUInt16 nColIndex;

					switch (g_DrawConfig.Streams.Depth.Coloring)
					{
					case LINEAR_HISTOGRAM:
						nRed = nGreen = g_pDepthHist[*pDepth]*255;
						break;
					case PSYCHEDELIC_SHADES:
						nAlpha *= (((XnFloat)(*pDepth % 10) / 20) + 0.5);
					case PSYCHEDELIC:

						switch ((*pDepth/10) % 10)
						{
						case 0:
							nRed = 255;
							break;
						case 1:
							nGreen = 255;
							break;
						case 2:
							nBlue = 255;
							break;
						case 3:
							nRed = 255;
							nGreen = 255;
							break;
						case 4:
							nGreen = 255;
							nBlue = 255;
							break;
						case 5:
							nRed = 255;
							nBlue = 255;
							break;
						case 6:
							nRed = 255;
							nGreen = 255;
							nBlue = 255;
							break;
						case 7:
							nRed = 127;
							nBlue = 255;
							break;
						case 8:
							nRed = 255;
							nBlue = 127;
							break;
						case 9:
							nRed = 127;
							nGreen = 255;
							break;
						}
						break;
					case RAINBOW:
						nColIndex = (XnUInt16)((*pDepth / (g_nMaxDepth / 256.)));
						nRed = PalletIntsR[nColIndex];
						nGreen = PalletIntsG[nColIndex];
						nBlue = PalletIntsB[nColIndex];
						break;
					case CYCLIC_RAINBOW:
						nColIndex = (*pDepth % 256);
						nRed = PalletIntsR[nColIndex];
						nGreen = PalletIntsG[nColIndex];
						nBlue = PalletIntsB[nColIndex];
						break;
					case CYCLIC_RAINBOW_HISTOGRAM:
						float fHist = g_pDepthHist[*pDepth];
						nColIndex = (*pDepth % 256);
						nRed = PalletIntsR[nColIndex]   * fHist;
						nGreen = PalletIntsG[nColIndex] * fHist;
						nBlue = PalletIntsB[nColIndex]  * fHist;
						break;
					}

					pTexture[0] = nRed;
					pTexture[1] = nGreen;
					pTexture[2] = nBlue;

					if (*pDepth == 0)
						pTexture[3] = 0;
					else
						pTexture[3] = nAlpha;
				}
			}
		} // not STANDRARD_DEVIATION

		if (pPointer != NULL)
		{
			TextureMapDrawCursor(&g_texDepth, *pPointer);
		}

		TextureMapUpdate(&g_texDepth);
		TextureMapDraw(&g_texDepth, pLocation);
	}
}

void drawPointerMode(IntPair* pPointer)
{
	char buf[512] = "";
	int nCharWidth = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, '0');
	int nPointerValue = 0;

	XnDouble dTimestampDivider = 1E6;

	const DepthMetaData* pDepthMD = getDepthMetaData();

	if (pDepthMD != NULL)
	{
		// Print the scale black background
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_QUADS);
		glColor4f(0, 0, 0, 0.7);
		glVertex2i(0, WIN_SIZE_Y); // lower left
		glVertex2i(WIN_SIZE_X, WIN_SIZE_Y);
		glVertex2i(WIN_SIZE_X, WIN_SIZE_Y - 135);
		glVertex2i(0, WIN_SIZE_Y - 135);
		glEnd();

		glDisable(GL_BLEND);

		// set a large point size (for the scale)
		glPointSize(15);

		// Print the scale data
		glBegin(GL_POINTS);
		for (int i = 0; i < pDepthMD->ZRes(); i+=1)
		{
			float fNewColor = g_pDepthHist[i];
			if ((fNewColor > 0.004) && (fNewColor < 0.996))
			{
				glColor3f(fNewColor, fNewColor, 0);
				glVertex3f(((i/10)*2), WIN_SIZE_Y - 23, 1);
			}
		}
		glEnd();

		// Print the pointer scale data
		if (pPointer != NULL)
		{
			// make sure pointer in on a depth pixel (take in mind cropping might be in place)
			IntPair pointerInDepth = *pPointer;
			pointerInDepth.X -= pDepthMD->XOffset();
			pointerInDepth.Y -= pDepthMD->YOffset();

			if (pointerInDepth.X < (int)pDepthMD->XRes() && pointerInDepth.Y < (int)pDepthMD->YRes())
			{
				nPointerValue = (*pDepthMD)(pointerInDepth.X, pointerInDepth.Y);

				glBegin(GL_POINTS);
				glColor3f(1,0,0);
				glVertex3f(10 + ((nPointerValue/10)*2), WIN_SIZE_Y - 70, 1);
				glEnd();
			}
		}

		// Print the scale texts
		for (int i = 0; i < pDepthMD->ZRes()/10; i+=25)
		{
			int xPos = i*2 + 10;

			// draw a small line in this position
			glBegin(GL_LINES);
			glColor3f(0, 1, 0);
			glVertex2i(xPos, WIN_SIZE_Y - 54);
			glVertex2i(xPos, WIN_SIZE_Y - 62);
			glEnd();

			// place a label under, and in the middle of, that line.
			int chars = sprintf(buf, "%d", i);
			glColor3f(1,0,0);
			glRasterPos2i(xPos - chars*nCharWidth/2, WIN_SIZE_Y - 40);
			glPrintString(GLUT_BITMAP_HELVETICA_18,buf);
		}

		sprintf(buf, "%s - Frame %4u, Timestamp %.3f", getDepthGenerator()->GetInfo().GetInstanceName(), pDepthMD->FrameID(), (double)pDepthMD->Timestamp()/dTimestampDivider);
	}

	const ImageMetaData* pImageMD = getImageMetaData();
	if (pImageMD != NULL)
	{
		if (buf[0] != '\0')
			sprintf(buf + strlen(buf), " | ");

		sprintf(buf + strlen(buf), "%s - Frame %4u, Timestamp %.3f", getImageGenerator()->GetInfo().GetInstanceName(), pImageMD->FrameID(), (double)pImageMD->Timestamp()/dTimestampDivider);
	}

	const IRMetaData* pIRMD = getIRMetaData();
	if (pIRMD != NULL)
	{
		if (buf[0] != '\0')
			sprintf(buf + strlen(buf), " | ");

		sprintf(buf + strlen(buf), "%s - Frame %4u, Timestamp %.3f", getIRGenerator()->GetInfo().GetInstanceName(), pIRMD->FrameID(), (double)pIRMD->Timestamp()/dTimestampDivider);
	}

	const AudioMetaData* pAudioMD = getAudioMetaData();
	if (pAudioMD != NULL)
	{
		if (buf[0] != '\0')
			sprintf(buf + strlen(buf), " | ");

		sprintf(buf + strlen(buf), "%s - Timestamp %.3f", getAudioGenerator()->GetInfo().GetInstanceName(), (double)pAudioMD->Timestamp()/dTimestampDivider);
	}

	int nYLocation = WIN_SIZE_Y - 88;
	glColor3f(1,0,0);
	glRasterPos2i(10,nYLocation);
	glPrintString(GLUT_BITMAP_HELVETICA_18, buf);
	nYLocation -= 26;

	if (pPointer != NULL && isStatisticsActive())
	{
		XnPixelStatistics* pStatistics = &g_PixelStatistics[pPointer->Y * pDepthMD->XRes() + pPointer->X];
		sprintf(buf, "Collected: %3u, Min: %4u Max: %4u Avg: %6.2f StdDev: %6.2f", 
			pStatistics->nCount, pStatistics->nMin, pStatistics->nMax, pStatistics->dAverage, pStatistics->dStdDev);
		glRasterPos2i(10,nYLocation);
		glPrintString(GLUT_BITMAP_HELVETICA_18, buf);
		nYLocation -= 26;
	}

	if (pPointer != NULL)
	{
		// Print the pointer text
		XnUInt64 nCutOffMin = 0;
		XnUInt64 nCutOffMax = (pDepthMD != NULL) ? g_nMaxDepth : 0;

		XnChar sPointerValue[100];
		if (nPointerValue != g_nMaxDepth)
		{
			sprintf(sPointerValue, "%.1f", (float)nPointerValue/10);
		}
		else
		{
			sprintf(sPointerValue, "-");
		}

		sprintf(buf, "Pointer Value: %s (X:%d Y:%d) Cutoff: %llu-%llu.", 
			sPointerValue, pPointer->X, pPointer->Y, nCutOffMin, nCutOffMax);

		glRasterPos2i(10,nYLocation);
		glPrintString(GLUT_BITMAP_HELVETICA_18, buf);
		nYLocation -= 26;
	}
}

void drawCenteredMessage(void* font, int y, const char* message, float fRed, float fGreen, float fBlue)
{
	const XnUInt32 nMaxLines = 5;
	XnChar buf[512];
	XnChar* aLines[nMaxLines];
	XnUInt32 anLinesWidths[nMaxLines];
	XnUInt32 nLine = 0;
	XnUInt32 nLineLengthChars = 0;
	XnUInt32 nLineLengthPixels = 0;
	XnUInt32 nMaxLineLength = 0;
	
	aLines[0] = buf;
	
	// parse message to lines
	const char* pChar = message;
	for (;;)
	{
		if (*pChar == '\n' || *pChar == '\0')
		{
			if (nLineLengthChars > 0)
			{
				aLines[nLine][nLineLengthChars++] = '\0';
				aLines[nLine+1] = &aLines[nLine][nLineLengthChars];
				anLinesWidths[nLine] = nLineLengthPixels;
				nLine++;
				if (nLineLengthPixels > nMaxLineLength)
				{
					nMaxLineLength = nLineLengthPixels;
				}
				nLineLengthPixels = 0;
				nLineLengthChars = 0;
			}

			if (nLine >= nMaxLines || *pChar == '\0')
			{
				break;
			}
		}
		else
		{
			aLines[nLine][nLineLengthChars++] = *pChar;
			nLineLengthPixels += glutBitmapWidth(font, *pChar);
		}
		pChar++;
	}
	
	XnUInt32 nHeight = 26;
	int nXLocation = XN_MAX(0, (WIN_SIZE_X - nMaxLineLength) / 2);
	int nYLocation = y;

	// Draw black background
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 0.6);
	glVertex2i(nXLocation - 5, nYLocation - nHeight - 5);
	glVertex2i(nXLocation + nMaxLineLength + 5, nYLocation - nHeight - 5);
	glVertex2i(nXLocation + nMaxLineLength + 5, nYLocation + nHeight * nLine + 5);
	glVertex2i(nXLocation - 5, nYLocation + nHeight * nLine + 5);
	glEnd();

	glDisable(GL_BLEND);

	// show message
	glColor3f(fRed, fGreen, fBlue);
	for (XnUInt32 i = 0; i < nLine; ++i)
	{
		glRasterPos2i(nXLocation + (nMaxLineLength - anLinesWidths[i])/2, nYLocation + i * nHeight);
		glPrintString(font, aLines[i]);
	}
}

void drawUserMessage()
{
	static XnUInt64 nStartShowMessage = 0;
	if (g_DrawConfig.bShowMessage)
	{
		g_DrawConfig.bShowMessage = false;
		xnOSGetTimeStamp(&nStartShowMessage);
	}
	
	XnUInt64 nNow;
	xnOSGetTimeStamp(&nNow);

	if (nNow - nStartShowMessage < 3000)
	{
		drawCenteredMessage(GLUT_BITMAP_TIMES_ROMAN_24, WIN_SIZE_Y * 4 / 5, g_csUserMessage, 0, 1, 0);
	}
}

void printRecordingInfo()
{
	char csMessage[256];
	getCaptureMessage(csMessage);

	if (csMessage[0] != 0)
		drawCenteredMessage(GLUT_BITMAP_TIMES_ROMAN_24, 30, csMessage, 1, 0, 0);

	sprintf(csMessage, "Capture Formats - Depth: %s | Image: %s | IR: %s | Audio: %s",
		captureGetDepthFormatName(), 
		captureGetImageFormatName(), 
		captureGetIRFormatName(), 
		captureGetAudioFormatName());

	drawCenteredMessage(GLUT_BITMAP_HELVETICA_12, WIN_SIZE_Y - 3, csMessage, 0, 1, 0);
}

void printStatisticsInfo()
{
	char csMessage[256];
	getStatisticsMessage(csMessage);

	if (csMessage[0] != 0)
		drawCenteredMessage(GLUT_BITMAP_HELVETICA_18, 20, csMessage, 0, 1, 0);
}

void printHelpGroup(int nXLocation, int* pnYLocation, const char* csGroup)
{
	int nYLocation = *pnYLocation;

	unsigned char aKeys[20];
	const char* aDescs[20];
	int nCount;

	getGroupItems(csGroup, aKeys, aDescs, &nCount);

	glColor3f(0, 1, 0);
	glRasterPos2i(nXLocation, nYLocation);
	glPrintString(GLUT_BITMAP_TIMES_ROMAN_24, csGroup);
	nYLocation += 30;

	for (int i = 0; i < nCount; ++i, nYLocation += 22)
	{
		char buf[256];
		switch (aKeys[i])
		{
		case 27:
			sprintf(buf, "Esc");
			break;
		default:
			sprintf(buf, "%c", aKeys[i]);
			break;
		}

		glColor3f(1, 0, 0);
		glRasterPos2i(nXLocation, nYLocation);
		glPrintString(GLUT_BITMAP_HELVETICA_18, buf);

		glRasterPos2i(nXLocation + 40, nYLocation);
		glPrintString(GLUT_BITMAP_HELVETICA_18, aDescs[i]);
	}

	*pnYLocation = nYLocation + 20;
}

void drawErrorState()
{
	if (g_DrawConfig.strErrorState == NULL)
		return;

	// place a black rect on entire screen
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 0.8);
	glVertex2i(0, 0);
	glVertex2i(WIN_SIZE_X, 0);
	glVertex2i(WIN_SIZE_X, WIN_SIZE_Y);
	glVertex2i(0, WIN_SIZE_Y);
	glEnd();
	glDisable(GL_BLEND);

	int nYLocation = WIN_SIZE_Y/2 - 30;

	drawCenteredMessage(GLUT_BITMAP_TIMES_ROMAN_24, nYLocation, "ERROR!", 1, 0, 0);
	nYLocation += 40;
	drawCenteredMessage(GLUT_BITMAP_TIMES_ROMAN_24, nYLocation, g_DrawConfig.strErrorState, 1, 0, 0);
}

void drawHelpScreen()
{
	int nXStartLocation = WIN_SIZE_X/8;
	int nYStartLocation = WIN_SIZE_Y/5;
	int nXEndLocation = WIN_SIZE_X*7/8;
	int nYEndLocation = WIN_SIZE_Y*4/5;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		

	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 0.8);
	glVertex2i(nXStartLocation, nYStartLocation);
	glVertex2i(nXStartLocation, nYEndLocation);
	glVertex2i(nXEndLocation, nYEndLocation);
	glVertex2i(nXEndLocation, nYStartLocation);
	glEnd();

	glDisable(GL_BLEND);

	// set color to red
	glColor3f(1, 0, 0);

	// leave some margins
	nYStartLocation += 30;
	nXStartLocation += 30;

	// print left pane
	int nXLocation = nXStartLocation;
	int nYLocation = nYStartLocation;
	printHelpGroup(nXLocation, &nYLocation, KEYBOARD_GROUP_PRESETS);
	printHelpGroup(nXLocation, &nYLocation, KEYBOARD_GROUP_DISPLAY);
	printHelpGroup(nXLocation, &nYLocation, KEYBOARD_GROUP_DEVICE);

	// print right pane
	nXLocation = WIN_SIZE_X/2;
	nYLocation = nYStartLocation;
	printHelpGroup(nXLocation, &nYLocation, KEYBOARD_GROUP_PLAYER);
	printHelpGroup(nXLocation, &nYLocation, KEYBOARD_GROUP_CAPTURE);
	printHelpGroup(nXLocation, &nYLocation, KEYBOARD_GROUP_GENERAL);
}

void drawUserInput(bool bCursor)
{
	if (bCursor)
	{
		// draw cursor
		IntPair cursor = g_DrawUserInput.Cursor;
		glPointSize(1);
		glBegin(GL_POINTS);
		glColor3f(1,0,0);
		glVertex2i(cursor.X, cursor.Y);

		// upper left marker
		glVertex2i(cursor.X - 2, cursor.Y - 2);
		glVertex2i(cursor.X - 2, cursor.Y - 1);
		glVertex2i(cursor.X - 1, cursor.Y - 2);

		// bottom left marker
		glVertex2i(cursor.X - 2, cursor.Y + 2);
		glVertex2i(cursor.X - 2, cursor.Y + 1);
		glVertex2i(cursor.X - 1, cursor.Y + 2);

		// upper right marker
		glVertex2i(cursor.X + 2, cursor.Y - 2);
		glVertex2i(cursor.X + 2, cursor.Y - 1);
		glVertex2i(cursor.X + 1, cursor.Y - 2);

		// lower right marker
		glVertex2i(cursor.X + 2, cursor.Y + 2);
		glVertex2i(cursor.X + 2, cursor.Y + 1);
		glVertex2i(cursor.X + 1, cursor.Y + 2);

		glEnd();
	}

	// draw selection frame
	if (g_DrawUserInput.State == SELECTION_ACTIVE)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		

		glBegin(GL_QUADS);
		glColor4f(1, 0, 0, 0.5);
		glVertex2i(g_DrawUserInput.Rect.uLeft, g_DrawUserInput.Rect.uTop); // Upper left
		glVertex2i(g_DrawUserInput.Rect.uRight, g_DrawUserInput.Rect.uTop); // upper right
		glVertex2i(g_DrawUserInput.Rect.uRight, g_DrawUserInput.Rect.uBottom); // lower right
		glVertex2i(g_DrawUserInput.Rect.uLeft, g_DrawUserInput.Rect.uBottom); // lower left
		glEnd();

		glDisable(GL_BLEND);
	}
}

void fixLocation(IntRect* pLocation, int xRes, int yRes)
{
	double resRatio = (double)xRes / yRes;

	double locationRatio = double(pLocation->uRight - pLocation->uLeft) / (pLocation->uTop - pLocation->uBottom);

	if (locationRatio > resRatio) 
	{
		// location is wider. use height as reference.
		double width = (pLocation->uTop - pLocation->uBottom) * resRatio;
		pLocation->uLeft += (pLocation->uRight - pLocation->uLeft - width) / 2;
		pLocation->uRight = (pLocation->uLeft + width);
	}
	else if (locationRatio < resRatio)
	{
		// res is wider. use width as reference.
		double height = (pLocation->uRight - pLocation->uLeft) / resRatio;
		pLocation->uBottom += (pLocation->uTop - pLocation->uBottom - height) / 2;
		pLocation->uTop = (pLocation->uBottom + height);
	}
}

bool isPointInRect(IntPair point, IntRect* pRect)
{
	return (point.X >= pRect->uLeft && point.X <= pRect->uRight &&
		point.Y >= pRect->uBottom && point.Y <= pRect->uTop);
}

void drawPlaybackSpeed()
{
	XnDouble dSpeed = getPlaybackSpeed();
	if (dSpeed != 1.0)
	{
		XnChar strSpeed[30];
		int len = sprintf(strSpeed, "x%g", dSpeed);
		int width = 0;
		for (int i = 0; i < len; ++i)
			width += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, strSpeed[i]);

		glColor3f(0, 1, 0);
		glRasterPos2i(WIN_SIZE_X - width - 3, 30);
		glPrintString(GLUT_BITMAP_TIMES_ROMAN_24, strSpeed);
	}
}

void drawFrame()
{
	// calculate locations
	g_DrawConfig.DepthLocation.uBottom = 0;
	g_DrawConfig.DepthLocation.uTop = WIN_SIZE_Y - 1;
	g_DrawConfig.DepthLocation.uLeft = 0;
	g_DrawConfig.DepthLocation.uRight = WIN_SIZE_X - 1;

	g_DrawConfig.ImageLocation.uBottom = 0;
	g_DrawConfig.ImageLocation.uTop = WIN_SIZE_Y - 1;
	g_DrawConfig.ImageLocation.uLeft = 0;
	g_DrawConfig.ImageLocation.uRight = WIN_SIZE_X - 1;

	if (g_DrawConfig.Streams.ScreenArrangement == SIDE_BY_SIDE)
	{
		g_DrawConfig.DepthLocation.uTop = WIN_SIZE_Y / 2 - 1;
		g_DrawConfig.DepthLocation.uRight = WIN_SIZE_X / 2 - 1;
		g_DrawConfig.ImageLocation.uTop = WIN_SIZE_Y / 2 - 1;
		g_DrawConfig.ImageLocation.uLeft = WIN_SIZE_X / 2;
	}

	// Texture map init
	const DepthMetaData* pDepthMD = getDepthMetaData();
	if (isDepthOn())
	{
		g_nMaxDepth = getDepthGenerator()->GetDeviceMaxDepth();
		TextureMapInit(&g_texDepth, pDepthMD->FullXRes(), pDepthMD->FullYRes(), 4, pDepthMD->XRes(), pDepthMD->YRes());
		fixLocation(&g_DrawConfig.DepthLocation, pDepthMD->FullXRes(), pDepthMD->FullYRes());
	}

	const MapMetaData* pImageMD = NULL;

	if (isImageOn())
	{
		pImageMD = getImageMetaData();
	}
	else if (isIROn())
	{
		pImageMD = getIRMetaData();
	}

	if (pImageMD != NULL)
	{
		TextureMapInit(&g_texImage, pImageMD->FullXRes(), pImageMD->FullYRes(), 4, pImageMD->XRes(), pImageMD->YRes());
		fixLocation(&g_DrawConfig.ImageLocation, pImageMD->FullXRes(), pImageMD->FullYRes());
	}

	// check if pointer is over a map
	bool bOverDepth = (pDepthMD != NULL) && isPointInRect(g_DrawUserInput.Cursor, &g_DrawConfig.DepthLocation);
	bool bOverImage = (pImageMD != NULL) && isPointInRect(g_DrawUserInput.Cursor, &g_DrawConfig.ImageLocation);
	bool bDrawDepthPointer = false;
	bool bDrawImagePointer = false;
	int imagePointerRed = 255;
	int imagePointerGreen = 0;
	int imagePointerBlue = 0;

	IntPair pointerInDepth;
	IntPair pointerInImage;

	if (bOverImage)
	{
		pointerInImage.X = (double)(g_DrawUserInput.Cursor.X - g_DrawConfig.ImageLocation.uLeft) / (g_DrawConfig.ImageLocation.uRight - g_DrawConfig.ImageLocation.uLeft + 1) * pImageMD->FullXRes();
		pointerInImage.Y = (double)(g_DrawUserInput.Cursor.Y - g_DrawConfig.ImageLocation.uBottom) / (g_DrawConfig.ImageLocation.uTop - g_DrawConfig.ImageLocation.uBottom + 1) * pImageMD->FullYRes();
		bDrawImagePointer = true;
	}

	if (bOverDepth)
	{
		pointerInDepth.X = (double)(g_DrawUserInput.Cursor.X - g_DrawConfig.DepthLocation.uLeft) / (g_DrawConfig.DepthLocation.uRight - g_DrawConfig.DepthLocation.uLeft + 1) * pDepthMD->FullXRes();
		pointerInDepth.Y = (double)(g_DrawUserInput.Cursor.Y - g_DrawConfig.DepthLocation.uBottom) / (g_DrawConfig.DepthLocation.uTop - g_DrawConfig.DepthLocation.uBottom + 1) * pDepthMD->FullYRes();

		// make sure we're in cropped area
		if (pointerInDepth.X >= pDepthMD->XOffset() && pointerInDepth.X < (pDepthMD->XOffset() + pDepthMD->XRes()) &&
			pointerInDepth.Y >= pDepthMD->YOffset() && pointerInDepth.Y < (pDepthMD->YOffset() + pDepthMD->YRes()))
		{
			bDrawDepthPointer = true;
			if (!bOverImage && g_DrawConfig.bShowPointer)
			{
				// try to translate depth pixel to image
				if (getImageCoordinatesForDepthPixel(pointerInDepth.X, pointerInDepth.Y, pointerInImage.X, pointerInImage.Y))
				{
					bDrawImagePointer = true;
					imagePointerRed = 0;
					imagePointerGreen = 0;
					imagePointerBlue = 255;
				}
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the opengl env for fixed location view
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,WIN_SIZE_X,WIN_SIZE_Y,0,-1.0,1.0);
	glDisable(GL_DEPTH_TEST); 

	if (g_DrawConfig.Streams.Depth.Coloring == CYCLIC_RAINBOW_HISTOGRAM || g_DrawConfig.Streams.Depth.Coloring == LINEAR_HISTOGRAM || g_DrawConfig.bShowPointer)
		calculateHistogram();

	drawColorImage(&g_DrawConfig.ImageLocation, bDrawImagePointer ? &pointerInImage : NULL, imagePointerRed, imagePointerGreen, imagePointerBlue);

	drawDepth(&g_DrawConfig.DepthLocation, bDrawDepthPointer ? &pointerInDepth : NULL);

	printStatisticsInfo();
	printRecordingInfo();

	if (g_DrawConfig.bShowPointer)
		drawPointerMode(bDrawDepthPointer ? &pointerInDepth : NULL);

	drawUserInput(!bOverDepth && !bOverImage);

	drawUserMessage();
	drawPlaybackSpeed();

	if (g_DrawConfig.strErrorState != NULL)
		drawErrorState();

	if (g_DrawConfig.bHelp)
		drawHelpScreen();

	glutSwapBuffers();
}

void setDepthDrawing(int nColoring)
{
	g_DrawConfig.Streams.Depth.Coloring	= (DepthColoringType)nColoring;
}

void setImageDrawing(int nColoring)
{
	g_DrawConfig.Streams.Image.Coloring	= (ImageColoringType)nColoring;
}

void resetIRHistogram(int /*dummy*/)
{
	g_nMaxGrayscale16Value = 0;
}
