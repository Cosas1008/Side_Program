/*
	Simple Gauge drawing example - Sandy Barbour 2005
	Combined files - Sandy Barbour 07/12/2009

	This is an example of drawing a gauge using OpenGL.
	The gauge is drawn in a floating panel so that it can be moved around.
	Pressing the F8 key will toggle the display of the window

 
	IMPORTANT NOTES - PLEASE READ
        1.
        On windows you will need to add these to the link settings, otherwise you will get link errors.
        glu32.lib glaux.lib

        2.
	You will need to download the ExampleGauge images.
	These can be found in the library section of the SDK web site.
	Unzip the archive to the plugins folder.
	Make sure that this creates a folder under plugins called ExampleGauge.
*/

#if IBM
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "XPLMDefs.h"
#include "XPLMDisplay.h"
#include "XPLMDataAccess.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"

/// Handle cross platform differences
#if IBM
#include <gl\gl.h>
#include <gl\glu.h>
#elif LIN
#define TRUE 1
#define FALSE 0
#include <GL/gl.h>
#include <GL/glu.h>
#else
#define TRUE 1
#define FALSE 0
#if __GNUC__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
#include <gl.h>
#include <glu.h>
#endif
#include <string.h>
#include <stdlib.h>
#endif

/// Cross Platform Bitmap functions and bitmap data structures - Sandy Barbour 2003
/// These need to be aligned
#pragma pack(push, ident, 2)

typedef struct tagBMPFILEHEADER
{
    short  bfType;
    int	   bfSize;
    short  bfReserved1;
    short  bfReserved2;
    int    bfOffBits;
} BMPFILEHEADER;

typedef struct tagBMPINFOHEADER
{
   int     biSize;
   int     biWidth;
   int     biHeight;
   short   biPlanes;
   short   biBitCount;
   int     biCompression;
   int     biSizeImage;
   int     biXPelsPerMeter;
   int     biYPelsPerMeter;
   int     biClrUsed;
   int     biClrImportant;
} BMPINFOHEADER;

typedef struct	tagIMAGEDATA
{
	unsigned char *	pData;
	int			Width;
	int			Height;
	int			Padding;
	short		Channels;
} IMAGEDATA;

#pragma pack(pop, ident)

static int		BitmapLoader(const char *FilePath, IMAGEDATA *ImageData);
static void		SwapRedBlue(IMAGEDATA *ImageData);

/// Texture stuff
#define MAX_TEXTURES 4

#define PANEL_FILENAME			"Panel.bmp"
#define GAUGE_FILENAME			"Gauge.bmp"
#define NEEDLE_FILENAME			"Needle.bmp"
#define NEEDLE_MASK_FILENAME	"NeedleMask.bmp"

#define PANEL_TEXTURE 0
#define GAUGE_TEXTURE 1
#define NEEDLE_TEXTURE 2
#define NEEDLE_TEXTURE_MASK 3

static XPLMTextureID gTexture[MAX_TEXTURES];

static XPLMDataRef	gEngineN1 = NULL;
static XPLMDataRef	RED = NULL, GREEN = NULL, BLUE = NULL;

static XPLMWindowID	gExampleGaugePanelDisplayWindow = NULL;
static int ExampleGaugeDisplayPanelWindow = 0;
static XPLMHotKeyID gExampleGaugeHotKey = NULL;

static char gPluginDataFile[255];
static float EngineN1;

/// Used for dragging plugin panel window.
static	int	CoordInRect(float x, float y, float l, float t, float r, float b);
static int	CoordInRect(float x, float y, float l, float t, float r, float b)
{	return ((x >= l) && (x < r) && (y < t) && (y >= b)); }

/// Prototypes for callbacks etc.
static void LoadTextures(void);
static int LoadGLTexture(char *pFileName, int TextureId);
static int DrawGLScene(float x, float y);
static int	ExampleGaugeDrawCallback(
                                   XPLMDrawingPhase     inPhase,
                                   int                  inIsBefore,
                                   void *               inRefcon);

static void ExampleGaugeHotKey(void * refCon);

static void ExampleGaugePanelWindowCallback(
                                   XPLMWindowID         inWindowID,
                                   void *               inRefcon);

static void ExampleGaugePanelKeyCallback(
                                   XPLMWindowID         inWindowID,
                                   char                 inKey,
                                   XPLMKeyFlags         inFlags,
                                   char                 inVirtualKey,
                                   void *               inRefcon,
                                   int                  losingFocus);

static int ExampleGaugePanelMouseClickCallback(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   XPLMMouseStatus      inMouse,
                                   void *               inRefcon);
PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{
	/// Handle cross platform differences
#if IBM
    char *pFileName = "Resources\\Plugins\\ExampleGauge\\";
#elif LIN
    char *pFileName = "Resources/plugins/ExampleGauge/";
#else
    char *pFileName = "Resources:Plugins:ExampleGauge:";
#endif
	/// Setup texture file locations
	XPLMGetSystemPath(gPluginDataFile);
	strcat(gPluginDataFile, pFileName);

	strcpy(outName, "ExampleGauge");
	strcpy(outSig, "xpsdk.experimental.ExampleGauge");
	strcpy(outDesc, "A plug-in by Sandy Barbour for displaying a gauge.");

	/// Register so that our gauge is drawing during the Xplane gauge phase
	XPLMRegisterDrawCallback(
					ExampleGaugeDrawCallback,
					xplm_Phase_Gauges,
					0,
					NULL);

	/// Create our window, setup datarefs and register our hotkey.
	gExampleGaugePanelDisplayWindow = XPLMCreateWindow(768, 256, 1024, 0, 1, ExampleGaugePanelWindowCallback, ExampleGaugePanelKeyCallback, ExampleGaugePanelMouseClickCallback, NULL);

	gEngineN1 = XPLMFindDataRef("sim/flightmodel/engine/ENGN_N1_");

	RED = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_r");
	GREEN = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_g");
	BLUE = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_b");

	gExampleGaugeHotKey = XPLMRegisterHotKey(XPLM_VK_F8, xplm_DownFlag,   "F8",   ExampleGaugeHotKey, NULL);

	/// Load the textures and bind them etc.
	LoadTextures();

	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
	/// Clean up
	XPLMUnregisterDrawCallback(ExampleGaugeDrawCallback,xplm_Phase_Gauges,0,NULL);

	XPLMUnregisterHotKey(gExampleGaugeHotKey);
	XPLMDestroyWindow(gExampleGaugePanelDisplayWindow);
}

PLUGIN_API void XPluginDisable(void){}

PLUGIN_API int XPluginEnable(void){return 1;}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID	inFromWho,int inMessage,void * inParam){}

/*
 * ExampleGaugeDrawCallback
 * This will draw our gauge during the Xplane gauge drawing phase.
 */
int	ExampleGaugeDrawCallback(XPLMDrawingPhase inPhase,int inIsBefore,void *inRefcon)
{
	int count;
	float FloatVals[8];

	/// Do the actual drawing, but only if our window is active
	if (ExampleGaugeDisplayPanelWindow)
	{
		count = XPLMGetDatavf(gEngineN1, FloatVals, 0, 8);
		// Convert N1 to rotation to match gauge
		EngineN1 = (270 * FloatVals[0]/100.0) - 135;

		DrawGLScene(512, 250);
	}
	return 1;
}


/*
 * ExampleGaugePanelWindowCallback
 * This callback does not do any drawing as such.
 * We use the mouse callback below to handle dragging of the window
 * X-Plane will automatically do the redraw.
 */
void ExampleGaugePanelWindowCallback(XPLMWindowID inWindowID,void * inRefcon){}

/*
 * ExampleGaugePanelKeyCallback
 * Our key handling callback does nothing in this plugin.  This is ok;
 * we simply don't use keyboard input.
 */
void ExampleGaugePanelKeyCallback(XPLMWindowID inWindowID,char inKey,XPLMKeyFlags inFlags,char inVirtualKey,void * inRefcon,int losingFocus){}

/*
 * ExampleGaugePanelMouseClickCallback
 * Our mouse click callback updates the position that the windows is dragged to.
 */
int ExampleGaugePanelMouseClickCallback(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   XPLMMouseStatus      inMouse,
                                   void *               inRefcon)
{
	static	int	dX = 0, dY = 0;
	static	int	Weight = 0, Height = 0;
	int	Left, Top, Right, Bottom;

	static	int	gDragging = 0;

	if (!ExampleGaugeDisplayPanelWindow)
		return 0;

	/// Get the windows current position
	XPLMGetWindowGeometry(inWindowID, &Left, &Top, &Right, &Bottom);

	switch(inMouse) {
	case xplm_MouseDown:
		/// Test for the mouse in the top part of the window
		if (CoordInRect(x, y, Left, Top, Right, Top-15))
		{
			dX = x - Left;
			dY = y - Top;
			Weight = Right - Left;
			Height = Bottom - Top;
			gDragging = 1;
		}
		break;
	case xplm_MouseDrag:
		/// We are dragging so update the window position
		if (gDragging)
		{
			Left = (x - dX);
			Right = Left + Weight;
			Top = (y - dY);
			Bottom = Top + Height;
			XPLMSetWindowGeometry(inWindowID, Left, Top, Right, Bottom);
		}
		break;
	case xplm_MouseUp:
		gDragging = 0;
		break;
	}
	return 1;
}

/// Toggle between display and non display
void	ExampleGaugeHotKey(void * refCon)
{
	ExampleGaugeDisplayPanelWindow = !ExampleGaugeDisplayPanelWindow;
}

/// Loads all our textures
void LoadTextures(void)
{
	if (!LoadGLTexture(PANEL_FILENAME, PANEL_TEXTURE))
		XPLMDebugString("Panel texture failed to load\n");
	if (!LoadGLTexture(GAUGE_FILENAME, GAUGE_TEXTURE))
		XPLMDebugString("Gauge texture failed to load\n");
	if (!LoadGLTexture(NEEDLE_FILENAME, NEEDLE_TEXTURE))
		XPLMDebugString("Needle texture failed to load\n");
	if (!LoadGLTexture(NEEDLE_MASK_FILENAME, NEEDLE_TEXTURE_MASK))
		XPLMDebugString("Needle texture mask failed to load\n");
}

/// Loads one texture
int LoadGLTexture(char *pFileName, int TextureId)
{
	int Status=FALSE;
	char TextureFileName[255];


	/// Need to get the actual texture path
	/// and append the filename to it.
	strcpy(TextureFileName, gPluginDataFile);
	strcat(TextureFileName, pFileName);

	void *pImageData = 0;
	int sWidth, sHeight;
	IMAGEDATA sImageData;
	/// Get the bitmap from the file
	if (BitmapLoader(TextureFileName, &sImageData))
	{
		Status=TRUE;

		SwapRedBlue(&sImageData);
		pImageData = sImageData.pData;

		/// Do the opengl stuff using XPLM functions for a friendly Xplane existence.
		sWidth=sImageData.Width;
		sHeight=sImageData.Height;
		XPLMGenerateTextureNumbers(&gTexture[TextureId], 1);
		XPLMBindTexture2d(gTexture[TextureId], 0);
	    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, sWidth, sHeight, GL_RGB, GL_UNSIGNED_BYTE, pImageData);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	if (pImageData != NULL)
		free(pImageData);

	return Status;
}

/// Draws the textures that make up the gauge
int DrawGLScene(float x1, float y1)
{
	float Red = XPLMGetDataf (RED);
	float Green = XPLMGetDataf (GREEN);
	float Blue = XPLMGetDataf (BLUE);
	int	PanelWindowLeft, PanelWindowRight, PanelWindowBottom, PanelWindowTop;

    float PanelWidth, PanelHeight;
    float GaugeWidth, GaugeHeight, GaugeWidthRatio, GaugeHeightRatio;
	float PanelLeft, PanelRight, PanelBottom, PanelTop;
	float GaugeLeft, GaugeRight, GaugeBottom, GaugeTop;
	float NeedleLeft, NeedleRight, NeedleBottom, NeedleTop;
	float NeedleTranslationX, NeedleTranslationY;

	/// Setup sizes for panel and gauge
    PanelWidth = 256;
    PanelHeight = 256;
    GaugeWidth = 128;
    GaugeHeight = 128;
    GaugeWidthRatio = GaugeWidth / 256.0;
    GaugeHeightRatio = GaugeHeight / 256.0;

	/// Need to find out where our window is
	XPLMGetWindowGeometry(gExampleGaugePanelDisplayWindow, &PanelWindowLeft, &PanelWindowTop, &PanelWindowRight, &PanelWindowBottom);

	/// Setup our panel and gauge relative to our window
	PanelLeft = PanelWindowLeft; PanelRight = PanelWindowRight; PanelBottom = PanelWindowBottom; PanelTop = PanelWindowTop;
	GaugeLeft = PanelLeft + 64; GaugeRight = GaugeLeft + GaugeWidth; GaugeBottom = PanelBottom + 64; GaugeTop = GaugeBottom  + GaugeHeight;

	/// Setup our needle relative to the gauge
	NeedleLeft = GaugeLeft + 125.0 * GaugeWidthRatio;
    NeedleRight = NeedleLeft + 8.0 * GaugeWidthRatio;
    NeedleBottom = GaugeBottom + 120.0 * GaugeHeightRatio;
    NeedleTop = NeedleBottom + 80.0 * GaugeWidthRatio;;
    NeedleTranslationX = NeedleLeft + ((NeedleRight - NeedleLeft) / 2);
    NeedleTranslationY = NeedleBottom+(5*GaugeHeightRatio);

	/// Tell Xplane what we are doing
	XPLMSetGraphicsState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 0/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);

	/// Handle day/night
	glColor3f(Red, Green, Blue);

	// Draw Panel
    glPushMatrix();
	XPLMBindTexture2d(gTexture[PANEL_TEXTURE], 0);
	glBegin(GL_QUADS);
			glTexCoord2f(1, 0.0f); glVertex2f(PanelRight, PanelBottom);	// Bottom Right Of The Texture and Quad
    		glTexCoord2f(0, 0.0f); glVertex2f(PanelLeft, PanelBottom);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(0, 1.0f); glVertex2f(PanelLeft, PanelTop);	// Top Left Of The Texture and Quad
			glTexCoord2f(1, 1.0f); glVertex2f(PanelRight, PanelTop);	// Top Right Of The Texture and Quad
	glEnd();
    glPopMatrix();

	// Draw Gauge
    glPushMatrix();
	XPLMBindTexture2d(gTexture[GAUGE_TEXTURE], 0);
	glBegin(GL_QUADS);
			glTexCoord2f(1, 0.0f); glVertex2f(GaugeRight, GaugeBottom);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(0, 0.0f); glVertex2f(GaugeLeft, GaugeBottom);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(0, 1.0f); glVertex2f(GaugeLeft, GaugeTop);	// Top Left Of The Texture and Quad
			glTexCoord2f(1, 1.0f); glVertex2f(GaugeRight, GaugeTop);	// Top Right Of The Texture and Quad
	glEnd();
    glPopMatrix();

    glPushMatrix();
	// Turn on Alpha Blending and turn off Depth Testing
	XPLMSetGraphicsState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 1/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);

    glTranslatef(NeedleTranslationX, NeedleTranslationY, 0.0f);
    glRotatef(EngineN1, 0.0f , 0.0f, -1.0f);
    glTranslatef(-NeedleTranslationX, -NeedleTranslationY, 0.0f);

    glBlendFunc(GL_DST_COLOR, GL_ZERO);

	// Draw Needle Mask
	XPLMBindTexture2d(gTexture[NEEDLE_TEXTURE_MASK], 0);
    glBegin(GL_QUADS);
			glTexCoord2f(1, 0.0f); glVertex2f(NeedleRight, NeedleBottom);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(0, 0.0f); glVertex2f(NeedleLeft, NeedleBottom);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(0, 1.0f); glVertex2f(NeedleLeft, NeedleTop);	// Top Left Of The Texture and Quad
			glTexCoord2f(1, 1.0f); glVertex2f(NeedleRight, NeedleTop);	// Top Right Of The Texture and Quad
	glEnd();

    glBlendFunc(GL_ONE, GL_ONE);

	// Draw Needle
	XPLMBindTexture2d(gTexture[NEEDLE_TEXTURE], 0);
	glBegin(GL_QUADS);
			glTexCoord2f(1, 0.0f); glVertex2f(NeedleRight, NeedleBottom);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(0, 0.0f); glVertex2f(NeedleLeft, NeedleBottom);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(0, 1.0f); glVertex2f(NeedleLeft, NeedleTop);	// Top Left Of The Texture and Quad
			glTexCoord2f(1, 1.0f); glVertex2f(NeedleRight, NeedleTop);	// Top Right Of The Texture and Quad
	glEnd();

	// Turn off Alpha Blending and turn on Depth Testing
	XPLMSetGraphicsState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 0/*AlphaBlending*/, 1/*DepthTesting*/, 0/*DepthWriting*/);
    glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFlush();

	return TRUE;
}

/// Swap the red and blue pixels.
void SwapRedBlue(IMAGEDATA *ImageData)
{
	unsigned char  * srcPixel;
	int 	count;
	int		x,y;
	unsigned char sTemp;

	/// Does not support 4 channels.
	if (ImageData->Channels == 4)
		return;

	/// Do the swap
	srcPixel = ImageData->pData;
	count = ImageData->Width * ImageData->Height;
	for (y = 0; y < ImageData->Height; ++y)
		for (x = 0; x < ImageData->Width; ++x)
		{
			sTemp = srcPixel[0];
			srcPixel[0] = srcPixel[2];
			srcPixel[2] = sTemp;

			srcPixel += 3;
			if (x == (ImageData->Width - 1))
				srcPixel += ImageData->Padding;
		}
}

/// Generic bitmap loader to handle all platforms
int BitmapLoader(const char * FilePath, IMAGEDATA * ImageData)
{
	BMPFILEHEADER   Header;
	BMPINFOHEADER	ImageInfo;
	int						Padding;
	FILE *					BitmapFile = NULL;
    int RetCode = 0;

	ImageData->pData = NULL;

	BitmapFile = fopen(FilePath, "rb");
	if (BitmapFile != NULL)
    {
    	if (fread(&Header, sizeof(Header), 1, BitmapFile) == 1)
        {
        	if (fread(&ImageInfo, sizeof(ImageInfo), 1, BitmapFile) == 1)
            {
				/// Handle Header endian.
				SwapEndian(&Header.bfSize);
            	SwapEndian(&Header.bfOffBits);

				/// Handle ImageInfo endian.
            	SwapEndian(&ImageInfo.biWidth);
            	SwapEndian(&ImageInfo.biHeight);
            	SwapEndian(&ImageInfo.biBitCount);

				/// Make sure that it is a bitmap.

            	if (((Header.bfType & 0xff) == 'B') &&(((Header.bfType >> 8) & 0xff) == 'M') &&
            		(ImageInfo.biBitCount == 24) && (ImageInfo.biWidth > 0) && (ImageInfo.biHeight > 0))
                {
                	if ((Header.bfSize + ImageInfo.biSize - Header.bfOffBits) >= (ImageInfo.biWidth * ImageInfo.biHeight * 3))
                    {
                    	Padding = (ImageInfo.biWidth * 3 + 3) & ~3;
                    	Padding -= ImageInfo.biWidth * 3;

                    	ImageData->Width = ImageInfo.biWidth;
                    	ImageData->Height = ImageInfo.biHeight;
                    	ImageData->Padding = Padding;

						/// Allocate memory for the actual image.
                    	ImageData->Channels = 3;
                    	ImageData->pData = (unsigned char *) malloc(ImageInfo.biWidth * ImageInfo.biHeight * ImageData->Channels + ImageInfo.biHeight * Padding);

                    	if (ImageData->pData != NULL)
                        {
							/// Get the actual image.
                        	if (fread(ImageData->pData, ImageInfo.biWidth * ImageInfo.biHeight * ImageData->Channels + ImageInfo.biHeight * Padding, 1, BitmapFile) == 1)
                            {
                                RetCode = 1;
                            }
                        }
                    }
                }
            }
        }
    }
   	if (BitmapFile != NULL)
    	fclose(BitmapFile);
	return RetCode;
}