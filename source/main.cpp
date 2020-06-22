/*============================================================================

    DSDev Template
	
	Thanks to source examples from:
	Adigun A. Polack, Richard Eric M. Lope BSN RN
	
=============================================================================*/ 

#include <nds.h>
#include <stdio.h>
#include <gl2d.h>
#include <maxmod9.h>
#include <console.h>
#include <keyboard.h>
#include <input.h>

//============= SOUND BUILD =========================
#include "mmsolution.h"		// solution definitions
#include "mmsolution_bin.h"	// solution binary reference 

// Our font class
// I decided to use c++ because of function overloading
class Cglfont {
	public:	
	Cglfont();
	int load(glImage *_font_sprite,
		const unsigned int numframes, 
		const unsigned int *texcoords,
		GL_TEXTURE_TYPE_ENUM type,
		int sizeX,
		int sizeY,
		int param,
		int	pallette_width,
		const u16 *palette,
		const u8 *texture);
	void print(int x, int y, const char *text);
	void printRight(int x, int y, const char *text);
	char *printValue(int value);
	void printCentered(int y, const char *text);
	int printWidth(const char *text);
	
	private:
	glImage *font_sprite;
};

Cglfont::Cglfont() { }

int Cglfont::load(glImage *_font_sprite,
		const unsigned int numframes, 
		const unsigned int *texcoords,
		GL_TEXTURE_TYPE_ENUM type,
		int sizeX,
		int sizeY,
		int param,
		int pallette_width,
		const u16 *palette,
		const u8 *texture) {
	font_sprite = _font_sprite;
	int textureID = 
		glLoadSpriteSet(font_sprite,
			numframes,//glLoadTileSet -> tileWidth, tileHieght 
			texcoords,//glLoadTileSet -> totalWidth, totalHeight
			type,
			sizeX,
			sizeY,
			param,
			pallette_width,
			palette,
			texture);					   
	return textureID;
}

void Cglfont::print(int x, int y, const char *text) {
	unsigned char font_char;
	while(*text) { 
		font_char = (*(unsigned char*)text++) - 32;
		glSprite(x, y, GL_FLIP_NONE, &font_sprite[font_char]);
		x += font_sprite[font_char].width; 
	}
}

void Cglfont::printRight(int x, int y, const char *text) {
	x -= printWidth(text); 
	print(x, y, text);
}

void Cglfont::printCentered(int y, const char *text) {
	int x = (SCREEN_WIDTH - printWidth(text)) / 2; 
	print(x, y, text);
}

int Cglfont::printWidth(const char *text) {
	unsigned char font_char;
	int total_width = 0;
	while(*text) {
		font_char = (*(unsigned char*)text++) - 32;
		total_width += font_sprite[font_char].width; 
	}
	return total_width;
}

// GRIT auto-genrated arrays of images
#include "font_si.h"
#include "font_16x16.h"

// Texture Packer auto-generated UV coords
#include "uvcoord_font_si.h"
#include "uvcoord_font_16x16.h"

// This imageset would use our texture packer generated coords so it's kinda
// safe and easy to use 
// FONT_SI_NUM_IMAGES is a value #defined from "uvcoord_font_si.h"
glImage  FontImages[FONT_SI_NUM_IMAGES];
glImage  FontBigImages[FONT_16X16_NUM_IMAGES];

// Our fonts
Cglfont Font;
Cglfont FontBig;

char *printValue(int value) {
	//buffer
	static char _str[256];
	sprintf(_str, "%i", value);
	return _str;
}

u16 frame = 0;//frame counter

void updateFrame() {
	frame++;
}

u16 frameCount() {
	static u16 last = 0;
	u16 x = frame - last;
	last += x;
	return x;
}
	
int main( int argc, char *argv[] ) {

    //VRAM ALLOCATIONS
    //A 128 -> PRIMARY TEXTURES
    //B 128 -> PRIMARY TEXTURES
    //C 128 -> 50% used for console and keyboard (sub-screen) BG0, BG3 (BG1 and BG2 free)
    //D 128 -> 
    //E 64 -> TEXTURE PALETTE
    //F 16 ->
    //G 16 ->
    //H 32 ->
    //I 16 ->

    //BG_PALETTE(_SUB)[x] -> 1K (* 2) -> lower 1K is Main 512 colours, 256 BG, 256 OBJ, upper is Sub
    //console fills black 0, and (n * 16 - 1) for text colours (on SUB)
	/*
			palette[1 * 16 - 1] = RGB15(0,0,0); //30 normal black
			palette[2 * 16 - 1] = RGB15(15,0,0); //31 normal red
			palette[3 * 16 - 1] = RGB15(0,15,0); //32 normal green
			palette[4 * 16 - 1] = RGB15(15,15,0); //33 normal yellow

			palette[5 * 16 - 1] = RGB15(0,0,15); //34 normal blue
			palette[6 * 16 - 1] = RGB15(15,0,15); //35 normal magenta
			palette[7 * 16 - 1] = RGB15(0,15,15); //36 normal cyan
			palette[8 * 16 - 1] = RGB15(24,24,24); //37 normal white

			palette[9 * 16 - 1 ] = RGB15(15,15,15); //40 bright black
			palette[10 * 16 - 1] = RGB15(31,0,0); //41 bright red
			palette[11 * 16 - 1] = RGB15(0,31,0); //42 bright green
			palette[12 * 16 - 1] = RGB15(31,31,0);	//43 bright yellow

			palette[13 * 16 - 1] = RGB15(0,0,31); //44 bright blue
			palette[14 * 16 - 1] = RGB15(31,0,31);	//45 bright magenta
			palette[15 * 16 - 1] = RGB15(0,31,31);	//46 bright cyan
			palette[16 * 16 - 1] = RGB15(31,31,31); //47 & 39 bright white
	*/
	
	//upper screen
	videoSetMode(MODE_5_3D);
	
    //lower screen
	//x, y, w, h in chars
	PrintConsole *console;
	consoleSetWindow(console = consoleDemoInit(), 0, 0, 32, 24);//does the following
    //videoSetModeSub(MODE_0_2D);
	//vramSetBankC(VRAM_C_SUB_BG);
    //BG0, mapbase 22 (2K) -> 21 extra free map, tilebase 3 (16K) = 48K, load-font
    //47,104 (map 23) -> also an extra free map
    //4096 byte tiles in default font 4bpp
    //tilebase 4 is free @ 64K, map 26 to 31 free in lower 64K of VRAM_C
    //BG1, BG2 not used
	keyboardDemoInit();
    keyboardShow();
    //BG3, mapbase 20 (2K) -> just above tiles, tilebase 0 (16K) = 0K
    //40,960 byte tiles for keyboard (256 * 320 / 2) 4bpp

	mmInitDefaultMem((mm_addr)mmsolution_bin);
	// setup maxmod to use the song event handler
	mmSetEventHandler(myEventHandler);
	irqSet(IRQ_VBLANK, updateFrame);
	
	// load the module
	mmLoad(MOD_FLATOUTLIES);

	// load sound effects
	mmLoadEffect(SFX_AMBULANCE);
	mmLoadEffect(SFX_BOOM);

	// Start playing module
	mmStart(MOD_FLATOUTLIES, MM_PLAY_LOOP);

	mm_sound_effect ambulance = {
		{ SFX_AMBULANCE } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		0,		// panning
	};

	mm_sound_effect boom = {
		{ SFX_BOOM } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		255,	// panning
	};

/*
// Play looping ambulance sound effect out of left speaker if A button is pressed
		if (keys_pressed & KEY_A) {
			amb = mmEffectEx(&ambulance);
		}

		// stop ambulance sound when A button is released
		if (keys_released & KEY_A) {
			mmEffectCancel(amb);//also mmEffectReleae??
		}

		// Play explosion sound effect out of right speaker if B button is pressed
		if (keys_pressed & KEY_B) {
			mmEffectEx(&boom);
		}
*/	
	// Set Bank A+B to texture (256 K)
	vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
	vramSetBankE(VRAM_E_TEX_PALETTE);  // Allocate VRAM bank for all the palettes (64 K)
	
	// Load our font texture
	// We used glLoadSpriteSet since the texture was made
	// with my texture packer.
	// no need to save the return value since
	// we don't need  it at all
	Font.load(FontImages,				// pointer to glImage array
		FONT_SI_NUM_IMAGES, 		// Texture packer auto-generated #define
		font_si_texcoords,		// Texture packer auto-generated array
		GL_RGB256,				// texture type for glTexImage2D() in videoGL.h 
		TEXTURE_SIZE_64,			// sizeX for glTexImage2D() in videoGL.h
		TEXTURE_SIZE_128,		// sizeY for glTexImage2D() in videoGL.h
		GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
		256,						// Length of the palette (256 colors)
		(u16*)font_siPal,		// Palette Data
		(u8*)font_siBitmap		// image data generated by GRIT 
	);

	// Do the same with our bigger texture
	FontBig.load(FontBigImages,
		FONT_16X16_NUM_IMAGES, 
		font_16x16_texcoords,
		GL_RGB256,
		TEXTURE_SIZE_64,
		TEXTURE_SIZE_512,
		GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT,
		256,
		(u16*)font_siPal,
		(u8*)font_16x16Bitmap	 
	);

	iprintf("\x1b[1;1HEasy GL2D Font Example");
	iprintf("\x1b[3;1HFonts by Adigun A. Polack");
	
	iprintf("\x1b[6;1HRelminator");
	iprintf("\x1b[7;1HHttp://Rel.Phatcode.Net");
	
	// calculate the amount of 
	// memory uploaded to VRAM in KB
	int TextureSize = font_siBitmapLen + font_16x16BitmapLen;			  
	iprintf("\x1b[10;1HTotal Texture size= %i kb", TextureSize / 1024);
	
	// initialize gl
	int textureID;
	glInit();
	
	//enable textures
	glEnable(GL_TEXTURE_2D);
	
	// enable antialiasing
	glEnable(GL_ANTIALIAS);
	
	// setup the rear plane
	glClearColor(0,0,0,31); // BG must be opaque for AA to work
	glClearPolyID(63); // BG must have a unique polygon ID for AA to work
	glClearDepth(0x7FFF);

	//this should work the same as the normal gl call
	glViewport(0,0,255,191);
	
	glGenTextures(1, &textureID);
	glBindTexture(0, textureID);
	glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_512,
		0, TEXGEN_TEXCOORD, (u8*)font_16x16Bitmap);//TODO: just easier to reuse
	
	//any floating point gl call is being converted to fixed prior to being implemented
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 40);
	
	gluLookAt(	0.0, 0.0, 1.0,		//camera possition 
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up	
	
	while(true) {
		u16 step;
		while((step = frameCount()) < 1) {
			//perform AI?? section
			swiWaitForVBlank();//or low power
		}	
		//3D
		draw3D(textureID);
		// set up GL2D for 2d mode
		glBegin2D();			
			// fill the whole screen with a gradient box
			glBoxFilledGradient(0, 0, 255, 191,
				RGB15(31, 0, 0),
				RGB15(0, 31, 0),
				RGB15(31, 0, 31),
				RGB15(0, 31, 31)
			);
			
			// Center print the title
			glColor(RGB15(0,0,0));
			FontBig.printCentered(0, "EASY GL2D");
			glColor(RGB15((frame*6)&31,(-frame*4)&31, (frame*2)&31));
			FontBig.printCentered(20,  "FONT EXAMPLE");

			// Fixed-point sinusoidal movement
			int x = (sinLerp(frame * 400) * 30) >> 12;
	   
			// Make the fonts sway left and right
			// Also change coloring of fonts
			glColor(RGB15(31,0,0));
			FontBig.print(25 + x, 50, "hfDEVKITPROfh");
			glColor(RGB15(31,0,31) );
			glColor(RGB15(x, 31 - x, x * 2));
			FontBig.print(50 - x, 70, "dcLIBNDScd");

			// change fontsets and  print some spam
			glColor(RGB15(0,31,31));
			Font.printCentered(100, "FONTS BY ADIGUN A. POLACK" );
			Font.printCentered(120, "CODE BY RELMINATOR" );
			
			// Restore normal coloring
			glColor(RGB15(31,31,31));
			
			// Change opacity relative to frame
			int opacity = abs(sinLerp(frame * 245) * 30) >> 12;
			
			// translucent mode 
			// Add 1 to opacity since at 0 we will get into wireframe mode
			glPolyFmt(POLY_ALPHA(1 + opacity) | POLY_CULL_NONE | POLY_ID(1));
			FontBig.print(35 + x, 140, "ANYA THERESE");
			
			glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(2));
			// Print the number of frames
			Font.print(10, 170, "FRAMES = ");
			Font.print(10 + 72, 170, printValue(frame));		
		glEnd2D();
		glFlush(0);
		scanKeys();
		if (keysDown() & KEY_START) break;
	}
	return 0;
}

void draw3D(int textureID) {
		int rotateX = 0;
		int rotateY = 0;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		//move it away from the camera
		glTranslatef32(0, 0, floattof32(-1));
				
		glRotateXi(rotateX);
		glRotateYi(rotateY);
		
		glMaterialf(GL_AMBIENT, RGB15(16,16,16));
		glMaterialf(GL_DIFFUSE, RGB15(16,16,16));
		glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8,8,8));
		glMaterialf(GL_EMISSION, RGB15(16,16,16));

		//ds uses a table for shinyness..this generates a half-ass one
		glMaterialShinyness();

		//not a real gl function and will likely change
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);

		scanKeys();
		
		u16 keys = keysHeld();
		
		if((keys & KEY_UP)) rotateX += 3;
		if((keys & KEY_DOWN)) rotateX -= 3;
		if((keys & KEY_LEFT)) rotateY += 3;
		if((keys & KEY_RIGHT)) rotateY -= 3;
		
		glBindTexture(0, textureID);

		//draw the obj
		glBegin(GL_QUAD);
			glNormal(NORMAL_PACK(0,inttov10(-1),0));

			GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(512)));
			glVertex3v16(floattov16(-0.5),	floattov16(-0.5), 0 );
	
			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(64),inttot16(512)));
			glVertex3v16(floattov16(0.5),	floattov16(-0.5), 0 );
	
			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(64), 0));
			glVertex3v16(floattov16(0.5),	floattov16(0.5), 0 );

			GFX_TEX_COORD = (TEXTURE_PACK(0,0));
			glVertex3v16(floattov16(-0.5),	floattov16(0.5), 0 );
		
		glEnd();
		
		glPopMatrix(1);
}

//---------------------------------------------------------------------------------
// callback function to handle song events
//---------------------------------------------------------------------------------
mm_word myEventHandler(mm_word msg, mm_word param) {
	//Avoid sheduling new music direct, use flag and do later after exit this
	switch(msg) {
	case MMCB_SONGMESSAGE:	// process song messages
		// if song event 1 is triggered, set sprite's y velocity to make it jump
		//if (param == 1) spriteDy = -16;
		//EFx EFFECT MESSAGE (param = x)
        break;	
	case MMCB_SONGFINISHED:	// process song finish message (only triggered in songs played with MM_PLAY_ONCE)
		break;
    }
	return 0;
}