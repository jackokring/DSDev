/*============================================================================

    DSDev Template:
	Simon Jackson
	
	Check credits: progress.h
	
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
#include "progress.h"

//============= SOUND BUILD =========================
#include "mmsolution.h"		// solution definitions
#include "mmsolution_bin.h"	// solution binary reference 

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

#include "sfx.h"

//===================== SOUND PROCESSING =======================
#define numberOfEffects sizeof(audioEffects) / sizeof(u16)

mm_sound_effect effectHandles[numberOfEffects];

u8 curentAudioMod = -1;
bool sheduleAudio = true;
u8 volumeModPercent = 99;
u8 volumeEffectPercent = 99;

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

/* mm_word fill_stream(mm_word length, mm_addr dest, mm_stream_formats format) {
    s8* output = (s8*)dest;
    for( ; length; length--) {
        *output++ = rand();//L
		*output++ = rand();//R
    }
    return length;
}

void openAudioStream() {
    mm_stream stream;
    stream.sampling_rate = 22000;        // 22khz
    stream.buffer_length = 8096;         // should be adequate
    stream.callback = fill_stream;       // give fill routine
    stream.format = MM_STREAM_16BIT_STEREO;
    stream.timer = MM_TIMER0;            // use timer0
    stream.manual = 0;                   // auto filling
    mmStreamOpen(&stream);
} */

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
		default:
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

//============= FONT CLASS ==========================
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
	int getTexturePack(uint tile, CORNER coordinate, uint scale = 1);
	
	private:
	glImage *font_sprite;
	int textureID;
	int extended = 0;
};

Cglfont::~Cglfont() { 
	if(font_sprite != NULL) {
		delete[] font_sprite;
	}	
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

int Cglfont::getTexturePack(uint tile, CORNER coordinate, uint scale) {
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

Cglfont *Font;//1024
Cglfont *FontBig;//256

//================== TILES AND TEXTURES =======================
#include "font_si.h"
#include "font_16x16.h"
#include "threeDtex0.h"
#include "threeDtex1.h"
#include "threeDtex2.h"
#include "threeDtex3.h"
#include "subTiles.h"
#include "mainTiles.h"
#include "logo.h"

//================ DECIMAL STRINGS ==========================
void overflowDefault(int32 *value) {
	//will cause a stack overflow on too many digits
}

char *printValue(int32 *value, bool comma = false,
	u8 digits = 10, void (*fn)(int32 *) = overflowDefault) {
	//buffer
	static char _str[32];//long!
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

//=================== MASTER GLOBAL STATE =================
int32 frame = 0;//frame counter
int32 stepFrames;
bool baulkAI = false;
bool paused = true;
bool inGame = false;
bool exiting = false;
bool newGame = true;//loop until official exit

void updateFrame() {
	frame++;
	baulkAI = true;
}

bool inFrameCount() {
	static u16 last = 0;
	u16 x = frame - last;
	if(x > 1) baulkAI = false;
	last += x;
	stepFrames = x;
	return x == 0;
}

void enterFrameWhile() {
	exiting = false;
	scanKeys();//just to catch buffer state for held
	while(!inFrameCount());
}

void waitForKey(int keys) {
	enterFrameWhile();
	while(!exiting) {
		if(keysDown() & keys) exiting = true;//next
		scanKeys();
		swiWaitForVBlank();//low power
	}
}

int textureID[4];
View *subViewRXInput;
uint keyNoAuto = 0;//bitmask for one shot keys
uint keyIntercepted = 0;
int subBG[2];//2 sub backgrounds of 64 by 32 for clear space all around
int mainBG[2];//2 main backgrounds of 64 by 64 for clear space and fill
int32 gameSaveLoc = 0;

//================ TILES, LAYERS AND PALETTES ==================
void putMain(int bg, int x, int y, int tile) {
	u16 *map = bgGetMapPtr(mainBG[bg]);
	x += y * 64;
	x &= 4095;
	map += x;
	*map = tile;
}

void putSub(int bg, int x, int y, int tile, int attribute = 0) {
	u16 *map = bgGetMapPtr(subBG[bg]);
	x += y * 64;
	x &= 2047;
	map += x;
	*map = tile;
	u8 *at = (u8 *)bgGetMapPtr(subBG[bg]);
	at += 4096;
	at += x;
	*at = (u8)attribute;
}

void clearMain(int bg) {
	u16 *map = bgGetMapPtr(mainBG[bg]);
	for(int i = 0; i < 4096; ++i) {
		*map++ = 0;
	}
}

void clearSub(int bg) {
	u16 *map = bgGetMapPtr(subBG[bg]) + 2048;//advanced attributes ...
	for(int i = 0; i < 1024; ++i) {
		*map++ = 0;
	}
	map = bgGetMapPtr(subBG[bg]);
	for(int i = 0; i < 2048; ++i) {
		*map++ = 0;
	}
}

void loadTitleMain(const unsigned int *tiles, int len,
		const unsigned short *pal, int palLen) {
	dmaCopy(tiles, bgGetGfxPtr(mainBG[0]), len);
	dmaCopy(pal, BG_PALETTE, palLen);
	for(int x = 0; x < 32 * 24; ++x) {
		putMain(0, x % 32, x / 32, x);
	}
	bgSetScroll(mainBG[0], 0, 0);//origin
	bgSetRotateScale(mainBG[0], 0, 1 << 8, 1 << 8);
	bgUpdate(); 
}

void defaultTilesMain() {
	clearMain(0);
	dmaCopy(mainTilesTiles, bgGetGfxPtr(mainBG[0]), mainTilesTilesLen);
	dmaCopy(mainTilesPal, BG_PALETTE, mainTilesPalLen);
}

void extendedPalettes(const unsigned short *pal, int len) {
	vramSetBankH(VRAM_H_LCD);
	u16 palette[len / 2];
	//16 palette slots for BG1 and BG2
	//as console and keyboard are 4bpp backgrounds
	//they are not affected with extended palettes
	//USE LAST 2 PALETTE LINES AS LIGHT ADJUSTMENT
	for(int i = 1; i < 3; ++i) {
		for(int p = 0; p < 16; ++p) {
			int col = palette[15 + i * 16 + p];//multiplyer of light
			for(int k = 0; k < len / 2; ++k) {
				int r = RED(col) * RED(pal[k]) / 31;
				int b = BLUE(col) * BLUE(pal[k]) / 31;
				int g = GREEN(col) * GREEN(pal[k]) /31;
				palette[k] = RGB15(r, g, b);
			}
			DC_FlushAll();
			dmaCopy(palette, &VRAM_H_EXT_PALETTE[i][p], len);//4096 colours max
		}
	}
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);//extended palette
}

bool currently2D = true;
bool glInitialized = false;


void setFor2D() {
	videoSetMode(MODE_5_2D);
	currently2D = true;
}

void setFor3D() {
	videoSetMode(MODE_5_3D);
	if(!glInitialized) {
		glInit();	
		// Load font textures
		Font = new Cglfont(8, (u16*)font_siPal,	(u8*)font_siBitmap);
		FontBig = new Cglfont(16, (u16*)font_16x16Pal, (u8*)font_16x16Bitmap);
		glGenTextures(4, (int *)&textureID);//make 4 textures
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
	}
	//enable textures
	glEnable(GL_TEXTURE_2D);
	// enable antialiasing
	glEnable(GL_ANTIALIAS);
	//this should work the same as the normal gl call
	glViewport(0,0,255,191);
	
	//any floating point gl call is being converted to fixed prior to being implemented
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 40);
	
	gluLookAt(	0.0, 0.0, 0.5,		//camera possition 
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up
	currently2D = false;
}

//=========================== DRAWING ROUTINES ============================
void drawMain() {

}

void drawSub() {

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
	/*		
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
		*/
	glEnd2D();
}

//===================== INTERNAL AUTOMATION DRAWING HELP =====================
uint drawSubMeta() {
	for(int i = 0; i < 2; ++i) {
		u16 *map = bgGetMapPtr(subBG[i]);
		u8 *at = (u8 *)(map + 2048);//attribute pointer
		for(int j = frame & 15; j < 2048; j+= 16) {
			//process attribute
			int a = *at;
			if(((frame ^ rand()) & 0xF0) <= a) {//test percent fast (BITS 4-7)
				int t = *map;
				int mask = ((1 << (a & 7)) - 1);
				int low = t & mask;
				low = ((low + 1) & mask) + ((a & 8) << 8);//extended colour? (BIT 3)
				*map = ((*map) & ~mask) + low;//auto rotate nth low bits (BIT 0-2)
			}
			map++;
			at++;
		}
	}
	drawSub();
	int keyCode = keyboardUpdate();//for later
	//return masked keys
	scanKeys();
	if(subViewRXInput != NULL);//process
	return (keysHeld() & ~keyIntercepted) &
		~(~keysDown() & keyNoAuto);//one shots
}

void indent() {
	const char *indent = "\n    ";
	iprintf(indent);
}

void menuText(const char *abxy[], const char *lrWhat, char *lr,
		const char *start, const char *select) {
	consoleClear();
	const char *butABXY[] = {
		"[A] ",
		"[B] ",
		"[X] ",
		"[Y] "
	};
	indent();
	indent();
	indent();
	indent();
	for(int i = 0; i < 4; ++i) {
		iprintf(butABXY[i]);
		iprintf(abxy[i]);
		indent();
	}
	iprintf("[L] ");
	iprintf(lr);
	iprintf(" [R]");
	indent();
	iprintf("[START] ");
	iprintf(start);
	indent();
	iprintf("[SELECT] ");
	iprintf(select);
	indent();
}

//===================== GAME LOOP PROCESS FUNCTIONS ==================
void loadGame(bool defaultGame = false) {

	//then after delay
	enterFrameWhile();
}

void saveGame(bool defaultGame = false) {
	
	//then after delay
	enterFrameWhile();
}

void gameSplash() {

}

void initGame() {
	gameSplash();
	defaultTilesMain();//clears automatic
	// initialize gl
	setFor3D();

	//get default
	loadGame(true);
}

void processInputs(uint keysMasked) {
	if(keysMasked & KEY_START) paused = true;//enter menu
}

void drawAndProcessMenu(uint keysMasked) {
	if(subViewRXInput == NULL) {//intercept menu view for show special instead?
		//menu display
		const char *buttons[] = { "PLAY", "LOAD GAME", "EXIT", "SAVE GAME" };
		menuText(buttons, "SLOT", printValue(&gameSaveLoc),	"PLAY", "NEXT MUSIC TRACK");
		//menu keys
		if(keysMasked & KEY_A_OR_START) paused = false;//A
		if(keysMasked & KEY_B) loadGame();//B
		if(keysMasked & KEY_X) {
			newGame = false;//X
			exiting = true;//just an exit back to ?
			saveGame(true);
		}
		if(keysMasked & KEY_Y) saveGame();//Y
		//TODO:
		if(keysMasked & KEY_L);//save slot?
		if(keysMasked & KEY_R);//save slot?

		if(keysMasked & KEY_LEFT);//setting index
		if(keysMasked & KEY_RIGHT);
		if(keysMasked & KEY_UP);//setting value
		if(keysMasked & KEY_DOWN);

		if(keysMasked & KEY_SELECT) {//BO SALECTA!!
			playMod(++curentAudioMod);
		}
	}
}

void processMotions() {

}

void processCollisions() {

}

void processStateMachine() {
	
}

//=================== SELF TEST LOADING =================================
void progressMessage(PROGRESS x) {
	switch(x) {
		case INITIAL_LOAD:
			iprintf(CREDITS);
			break;

		default:
			break;
	}
}

//=================== ANYTHING BAD FOR CHAIN LOADS ==================
void cleanUp() {
	mmStop();
	while(mmActive());
	irqClear(IRQ_VBLANK);
	setFor2D();
	glResetTextures();
	if(FontBig != NULL) delete FontBig;
	if(Font != NULL) delete Font;
	glInitialized = false;
}

void powerButtonPressed() {
	cleanUp();
	exit(0);
}

//=================== MAIN ENTRY ========================================
int main(int argc, char *argv[]) {

    //VRAM ALLOCATIONS
    //A 128 -> PRIMARY TEXTURES (BIG + SMALL FONTS)
    //B 128 -> PRIMARY TEXTURES (TextureID[0,1])
    //C 128 -> SUB CONSOLE, KEYBOARD, 2 * BG, (map 23 free)
    //D 128 -> PRIMARY TEXTURES (TextureID[2,3])
    //E 64 -> MAIN BG (EE) -> uses main palette as 3D doesn't
    //F 16 -> TEXTURE PALETTE (6 * 512 (3K of 32K) used) -> hi-colour
    //G 16 -> TEXTURE PALETTE -> overflow from above
    //H 32 -> SUB BG EXT PALETTE -> 4096 colours (auto palette by last 32 colour lights)
    //I 16 -> ?SUB SPRITE? -> vramSetBankI(VRAM_I_SUB_SPRITE);

    //lower screen
	//x, y, w, h in chars
	PrintConsole *console;
	consoleSetWindow(console = consoleDemoInit(), 0, 0, 32, 24);//does the following
    //videoSetModeSub(MODE_0_2D);
	//vramSetBankC(VRAM_C_SUB_BG);
    //BG0, mapbase 22 (2K), tilebase 3 (16K) = 48K, load-font
    //47,104 ============> (map 23) is an extra free map
    //4096 byte tiles in default font 4bpp
    //tilebase 4 is free @ 64K, map 30 and 31 free in lower 64K of VRAM_C
    //BG1, BG2 not used
	Keyboard * k = keyboardDemoInit();
    //keyboardShow();
    //BG3, mapbase 20 (2K) -> just above tiles, tilebase 0 (16K) = 0K
    //40,960 byte tiles for keyboard (256 * 320 / 2) 4bpp
	subBG[0] = bgInitSub(1, BgType_Text8bpp, BgSize_T_512x256, 26, 4);
	subBG[1] = bgInitSub(2, BgType_Text8bpp, BgSize_T_512x256, 29, 4);
	dmaCopy(subTilesTiles, bgGetGfxPtr(subBG[0]), subTilesTilesLen);
	clearSub(0);
	clearSub(1);

	//sound
	mmInitDefaultMem((mm_addr)mmsolution_bin);
	irqSet(IRQ_VBLANK, updateFrame);
	loadMods();
	loadEffects();

	//upper screen
	setFor2D();
	bgExtPaletteEnableSub();
	extendedPalettes(subTilesPal, subTilesPalLen);
	// Set Bank A+B+D to texture (256 K + 128K)
	vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
	vramSetBankD(VRAM_D_TEXTURE);
	vramSetBankE(VRAM_E_MAIN_BG);//for intro screens and ...
	vramSetBankF(VRAM_F_TEX_PALETTE_SLOT0);
	vramSetBankG(VRAM_G_TEX_PALETTE_SLOT1);
	// Allocate VRAM bank for all the main palettes (32 K)
	mainBG[0] = bgInit(2, BgType_ExRotation, BgSize_ER_512x512, 24, 0);
	mainBG[1] = bgInit(3, BgType_ExRotation, BgSize_ER_512x512, 28, 0);	
	//clearMain(1);

	//ready for priorities
	//3D ---> 0
	bgSetPriority(mainBG[0],1);//SKY?
	bgSetPriority(mainBG[1],2);
	//BG1 is not in use

	//Console
	bgSetPriority(console->bgId, 0);
	//keyboard ---> hack to 1
	bgSetPriority(k->background, 1);
	bgSetPriority(subBG[0],2);
	bgSetPriority(subBG[1],3);

	loadTitleMain(logoTiles, logoTilesLen, logoPal, logoPalLen);
	progressMessage(INITIAL_LOAD);	
	playEffect(SFX_EXPLODE);
	//openAudioStream();
	
	waitForKey(KEY_A_OR_START);
	//setPowerButtonCB(powerButtonPressed);//?? NO REFERENCE
	do {
		initGame();
		while(!exiting) {
			if(sheduleAudio) playMod(rand() % MSL_NSONGS);
			while(inFrameCount()) {
				if(paused) {
					swiWaitForVBlank();//low power
				} else {
					//perform AI?? section
					//check baulkAI in intensive search
				}
			}	
			//3D
			draw3D();
			//2D
			draw2D();
			glFlush(0);
			drawMain();
			if(!paused) {
				processInputs(drawSubMeta());//keysIntercepted?
				processMotions();
				processCollisions();
				processStateMachine();
			} else {
				drawAndProcessMenu(drawSubMeta());
			}
		}
	} while(newGame);
	cleanUp();
	exit(0);
}