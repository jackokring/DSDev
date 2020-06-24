/*============================================================================

    DSDev Template:
	Simon Jackson
	
	Thanks to source examples from:
	Adigun A. Polack, Richard Eric M. Lope BSN RN
	
	Music from Aminet:
	Neurodancer
	
=============================================================================*/ 

#include <nds.h>
#include <stdio.h>
#include <gl2d.h>
#include <maxmod9.h>
#include <console.h>
#include <keyboard.h>
#include <input.h>
#include "ai.h"
#include "lang.h"

//============= SOUND BUILD =========================
#include "mmsolution.h"		// solution definitions
#include "mmsolution_bin.h"	// solution binary reference 

// Our font class
// I decided to use c++ because of function overloading
class Cglfont {
	public:	
	~Cglfont();
	Cglfont(const uint tileSize, 
		const u16 *palette,
		const u8 *texture);
	void print(int x, int y, const char *text);
	void printRight(int x, int y, const char *text);
	void printCentered(int y, const char *text);
	int printWidth(const char *text);
	void setExtended(bool low, bool high);//for small font
	void printOutline(int x, int y);
	int getTextureID();
	int getTexturePack(uint tile, uint coordinate, uint scale = 1);
	
	private:
	glImage *font_sprite;
	int textureID;
	int extended = 0;
};

Cglfont::~Cglfont() { 
	if(font_sprite != NULL) delete[] font_sprite;
}

Cglfont::Cglfont(const uint tileSize,
	const u16 *palette,
	const u8 *texture) {
		int sz = 256 / tileSize;
		sz *= sz; 
		font_sprite = new glImage[sz];
		textureID = glLoadTileSet(font_sprite,
			tileSize, tileSize,//glLoadTileSet -> tileWidth, tileHieght 
			256, 256,//glLoadTileSet -> totalWidth, totalHeight
			GL_RGB256,
			TEXTURE_SIZE_256,
			TEXTURE_SIZE_256,
			GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT,
			256,
			palette,
			texture);					   
}

void Cglfont::setExtended(bool low, bool high) {
	extended = ((low ? 1 : 0) | (high ? 2 : 0)) << 8;
}

void Cglfont::print(int x, int y, const char *text) {
	uint font_char;
	while(*text) { 
		font_char = (*(unsigned char*)text++) | extended;
		glSprite(x, y, GL_FLIP_NONE, &font_sprite[font_char]);
		x += font_sprite[font_char].width; 
	}
}

void Cglfont::printOutline(int x, int y) {
	glSprite(x, y, GL_FLIP_NONE, &font_sprite[0]);//use for null
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
	uint font_char;
	int total_width = 0;
	while(*text) {
		font_char = (*(unsigned char*)text++) | extended;
		total_width += font_sprite[font_char].width; 
	}
	return total_width;
}

int Cglfont::getTextureID() {
	return textureID;//useful for 3D text
}

int Cglfont::getTexturePack(uint tile, uint coordinate, uint scale) {
	uint sz = font_sprite[0].width;
	uint x = (256 / sz) * (tile % (256 / sz));
	uint y = (256 / sz) * (tile / (256 / sz));
	switch(coordinate) {
		case TOP_LEFT_FONT:
			return TEXTURE_PACK(inttot16(x),inttot16(y));
		case TOP_RIGHT_FONT:
			return TEXTURE_PACK(inttot16(x + sz * scale),inttot16(y));
		case BOTTOM_LEFT_FONT:
			return TEXTURE_PACK(inttot16(x),inttot16(y + sz * scale));
		case BOTTOM_RIGHT_FONT:
			return TEXTURE_PACK(inttot16(x + sz * scale),inttot16(y + sz * scale));
		default:
			return 0;
	}
}

// GRIT auto-genrated arrays of images
#include "font_si.h"
#include "font_16x16.h"
#include "threeDtex0.h"
#include "threeDtex1.h"
#include "threeDtex2.h"
#include "threeDtex3.h"
#include "subTiles.h"

// Our fonts
Cglfont *Font;//1024
Cglfont *FontBig;//256

void overflowDefault(int32 *value) {
	//will cause a stack overflow on too many digits
}

char *printValue(int32 *value, bool comma = false,
	u8 digits = 10, void (*fn)(int32 *) = overflowDefault) {
	//buffer
	static char _str[16];
	sprintf(_str, "%li", *value);
	int off = 0;
	if(*value < 0) off = 1;
	int l = strlen(_str);
	if(l - off > digits) {
		fn(value);//for things like preventing score overflows
		return printValue(value, comma, digits, fn);
	}
	if(comma) {
		int k = l + ((l - 1 - off) / 3);
		int m = 0;
		for(int p = l; p >= 0 + off; --p) {
			if(p == k) break;
			_str[k--] = _str[p];
			if((m++ % 3) == 2) {
				_str[k--] = ',';
			}
		}
	}
	return _str;
}

int32 frame = 0;//frame counter
bool baulkAI = false;
bool paused = false;
bool inGame = false;
bool exiting = false;

void updateFrame() {
	frame++;
	baulkAI = true;
}

u16 frameCount() {
	static u16 last = 0;
	u16 x = frame - last;
	if(x > 1) baulkAI = false;
	last += x;
	return x;
}

int textureID[4];
View *subViewRXInput;
uint keyNoAuto = 0;//bitmask for one shot keys
uint keyIntercepted = 0;
int subBG[2];//2 sub backgrounds of 64 by 32 for clear space all around

void clearSub(int idx) {
	idx = subBG[idx];
	u16 *map = bgGetMapPtr(idx);
	for(int i = 0; i < 2048; ++i) {
		*map++ = 0;
	}
}

void extendedPalettes(const unsigned short *pal, int len, u16 *vram) {
	vramSetBankH(VRAM_H_LCD);
	u16 palette[len / 2];
	for(int i = 0; i < len / 2; ++i) {
		palette[i] = pal[i];//copy for processing
	}
	dmaCopy(subTilesPal, &VRAM_H_EXT_PALETTE[2][0], subTilesPalLen);
	//15 other palette slots for BG1 and BG2
	//as console and keyboard are 4bpp backgrounds
	//they are not affected with extended palettes
	for(int i = 1; i < 3; ++i) {
		for(int p = 0; p < 16; ++p) {
			for(int i = 0; i < len / 2; ++i) {
				palette[i] = pal[i];//copy for processing
			}
			//TODO: process depending on i and p
			if(i == 1) {//BG1

			} else { //BG2

			}
			dmaCopy(palette, &vram[i][p], len);//4096 colours max
		}
	}
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);//extended palette
}

uint drawSub() {

	//return masked keys
	scanKeys();
	if(subViewRXInput != NULL);//process
	return (keysHeld() & ~keyIntercepted) &
		~(~keysDown() & keyNoAuto);//one shots
}

void draw3D() {
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
		
		glBindTexture(0, textureID[0]);

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

void draw2D() {
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
		FontBig->printCentered(0, "EASY GL2D");
		glColor(RGB15((frame*6)&31,(-frame*4)&31, (frame*2)&31));
		FontBig->printCentered(20,  "FONT EXAMPLE");

		// Fixed-point sinusoidal movement
		int x = (sinLerp(frame * 400) * 30) >> 12;
	
		// Make the fonts sway left and right
		// Also change coloring of fonts
		glColor(RGB15(31,0,0));
		FontBig->print(25 + x, 50, "hfDEVKITPROfh");
		glColor(RGB15(31,0,31) );
		glColor(RGB15(x, 31 - x, x * 2));
		FontBig->print(50 - x, 70, "dcLIBNDScd");

		// change fontsets and  print some spam
		glColor(RGB15(0,31,31));
		Font->printCentered(100, "FONTS BY ADIGUN A. POLACK" );
		Font->printCentered(120, "CODE BY RELMINATOR" );
		
		// Restore normal coloring
		glColor(RGB15(31,31,31));
		
		// Change opacity relative to frame
		int opacity = abs(sinLerp(frame * 245) * 30) >> 12;
		
		// translucent mode 
		// Add 1 to opacity since at 0 we will get into wireframe mode
		glPolyFmt(POLY_ALPHA(1 + opacity) | POLY_CULL_NONE | POLY_ID(1));
		FontBig->print(35 + x, 140, "ANYA THERESE");
		
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(2));
		// Print the number of frames
		Font->print(10, 170, "FRAMES = ");
		Font->print(10 + 72, 170, printValue(&frame));		
	glEnd2D();
}

void processInputs(uint keysMasked) {

	if(keysMasked & KEY_START) exiting = true;
}

void processMotions() {

}

void processCollisions() {

}

void processStateMachine() {
	
}

u8 audioMods[MSL_NSONGS] = {
	MOD_WF_COURIER,
	MOD_MATHHYSTERIA,
	MOD_TIMETRAP,
	MOD_QUASAR,
	MOD_ALIENTEMPLE,
	MOD_LILHOUSE,
	MOD_AGRESSOR8,
	MOD_FLOOR_23,
	MOD_TRICKTOP
};

u16 audioEffects[] = {
	SFX_AMBULANCE,
	SFX_BOOM
};

#define numberOfEffects sizeof(audioEffects) / sizeof(u16)

mm_sound_effect effectHandles[numberOfEffects];

u8 curentAudioMod = -1;
bool sheduleAudio = true;
u8 volumeModPercent = 100;
u8 volumeEffectPercent = 100;

void playMod(u8 current) {
	if(curentAudioMod >= 0 && !sheduleAudio) mmStop();
	mmSetModuleVolume(volumeModPercent * 1024 / 100);
	mmStart(current, MM_PLAY_ONCE);
	curentAudioMod = current;
	sheduleAudio = false;
}

mm_sfxhand playEffect(int effect, bool foreground = false) {
	mm_sound_effect *snd = NULL;
	for(uint i = 0; i < numberOfEffects; ++i) {
		if(audioEffects[i] == effect) {
			snd = &effectHandles[i];
			break;
		}
	}	
	if(snd == NULL) return 0;
	mmSetEffectsVolume(volumeEffectPercent * 1024 / 100);
	mm_sfxhand hand = mmEffectEx(snd);
	if(!foreground) {
		mmEffectRelease(hand);//for low priority
	}
	return hand;
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
		sheduleAudio = true;
		break;
    }
	return 0;
}

void loadMods() {
	for(u8 i = 0; i < MSL_NSONGS; ++i) {
		mmLoad(audioMods[i]);
	}
	// setup maxmod to use the song event handler
	mmSetEventHandler(myEventHandler);
}

void loadEffects() {
	for(u8 i = 0; i < numberOfEffects; ++i) {
		mmLoadEffect(audioEffects[i]);	
		effectHandles[i] = {
			{ audioEffects[i] } ,	// id
			(int)(1.0f * (1<<10)),	// rate
			0,		// handle (for recycling)
			255,	// volume
			128,	// panning
		};
	}
}

void cleanUp() {
	delete FontBig;
	delete Font;
}
	
int main(int argc, char *argv[]) {

    //VRAM ALLOCATIONS
    //A 128 -> PRIMARY TEXTURES (BIG + SMALL FONTS)
    //B 128 -> PRIMARY TEXTURES (TextureID[0,1])
    //C 128 -> SUB CONSOLE, KEYBOARD, 2 * BG, (map 28 to 31 free)
    //D 128 -> PRIMARY TEXTURES (TextureID[2,3])
    //E 64 -> TEXTURE PALETTE (6 * 512 (3K) used fades??) -> full colour
    //F 16 -> ?MAIN SPRITE?
    //G 16 -> ?MAIN SPRITE?
    //H 32 -> SUB BG EXT PALETTE
    //I 16 -> ?SUB SPRITE?

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
	bgExtPaletteEnableSub();

	extendedPalettes(subTilesPal, subTilesPalLen, &VRAM_H_EXT_PALETTE);

    //lower screen
	//x, y, w, h in chars
	PrintConsole *console;
	consoleSetWindow(console = consoleDemoInit(), 0, 0, 32, 24);//does the following
    //videoSetModeSub(MODE_0_2D);
	//vramSetBankC(VRAM_C_SUB_BG);
    //BG0, mapbase 22 (2K) -> 21 extra free map, tilebase 3 (16K) = 48K, load-font
    //47,104 (map 23) -> also an extra free map
    //4096 byte tiles in default font 4bpp
    //tilebase 4 is free @ 64K, map 30 and 31 free in lower 64K of VRAM_C
    //BG1, BG2 not used
	keyboardDemoInit();
    keyboardShow();
    //BG3, mapbase 20 (2K) -> just above tiles, tilebase 0 (16K) = 0K
    //40,960 byte tiles for keyboard (256 * 320 / 2) 4bpp
	subBG[0] = bgInitSub(1, BgType_Text8bpp, BgSize_T_512x256, 26, 4);
	subBG[1] = bgInitSub(2, BgType_Text8bpp, BgSize_T_512x256, 28, 4);
	
	dmaCopy(subTilesTiles, bgGetGfxPtr(subBG[0]), sizeof(subTilesTilesLen));
	clearSub(0);
	clearSub(1);

	mmInitDefaultMem((mm_addr)mmsolution_bin);
	irqSet(IRQ_VBLANK, updateFrame);

	loadMods();
	loadEffects();

	// Set Bank A+B+D to texture (256 K + 128K)
	vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
	vramSetBankD(VRAM_D_TEXTURE);
	vramSetBankE(VRAM_E_TEX_PALETTE);  // Allocate VRAM bank for all the palettes (64 K)
	
	// Load our font textures
	Font = new Cglfont(8, 		// tile pixels square
		(u16*)font_siPal,		// Palette Data
		(u8*)font_siBitmap		// image data generated by GRIT 
	);

	// Do the same with our bigger texture
	FontBig = new Cglfont(16, (u16*)font_16x16Pal, (u8*)font_16x16Bitmap);

	iprintf("\x1b[1;1HEasy GL2D Font Example");
	iprintf("\x1b[3;1HFonts by Adigun A. Polack");
	
	iprintf("\x1b[6;1HRelminator");
	iprintf("\x1b[7;1HHttp://Rel.Phatcode.Net");
	
	// calculate the amount of 
	// memory uploaded to VRAM in KB
	int TextureSize = font_siBitmapLen + font_16x16BitmapLen;			  
	iprintf("\x1b[10;1HTotal Texture size= %i kb", TextureSize / 1024);
	
	// initialize gl
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
	
	glGenTextures(4, (int *)&textureID);//make 2 textures
	glBindTexture(0, textureID[0]);//bind it
	glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_256, TEXTURE_SIZE_256,
		0, TEXGEN_TEXCOORD, (u8*)threeDtex0Bitmap);
	glColorTableEXT(0, 0, 255, 0, 0, (u16*)threeDtex0Pal);
	glBindTexture(0, textureID[1]);//bind it
	glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_256, TEXTURE_SIZE_256,
		0, TEXGEN_TEXCOORD, (u8*)threeDtex1Bitmap);
	glColorTableEXT(0, 0, 255, 0, 0, (u16*)threeDtex1Pal);
	glBindTexture(0, textureID[2]);//bind it
	glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_256, TEXTURE_SIZE_256,
		0, TEXGEN_TEXCOORD, (u8*)threeDtex2Bitmap);
	glColorTableEXT(0, 0, 255, 0, 0, (u16*)threeDtex2Pal);
	glBindTexture(0, textureID[3]);//bind it
	glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_256, TEXTURE_SIZE_256,
		0, TEXGEN_TEXCOORD, (u8*)threeDtex3Bitmap);
	glColorTableEXT(0, 0, 255, 0, 0, (u16*)threeDtex3Pal);
	
	//any floating point gl call is being converted to fixed prior to being implemented
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 40);
	
	gluLookAt(	0.0, 0.0, 1.0,		//camera possition 
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up	
	
	while(!exiting) {
		if(sheduleAudio) playMod(++curentAudioMod);
		u16 step;
		while((step = frameCount()) < 1) {
			//perform AI?? section
			//check baulkAI in intensive search
			swiWaitForVBlank();//or low power
		}	
		//3D
		draw3D();
		//2D
		draw2D();
		glFlush(0);
		processInputs(drawSub());//keysIntercepted?
		processMotions();
		processCollisions();
		processStateMachine();
	}
	cleanUp();
	return 0;
}