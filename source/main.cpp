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
#include <sprite.h>
#include "ctl.h"
#include "gfx.h"
#include "lang.h"
#include "progress.h"
#include "game.h"

//=================== SELF TEST LOADING =================================
void progressMessage(PROGRESS x) {
	consoleClear();
	switch(x) {
		case INITIAL_LOAD:
			iprintf(CREDITS);
			break;
		case GAME_INTRO_SCREEN:
			iprintf(GOALS);
			break;
		case GAME_WIN_SCREEN:
			iprintf(YOU_WIN);
			break;
		case GAME_LOSE_SCREEN:
			iprintf(YOU_LOSE);
			break;
		default:
			break;
	}
}

//============= SOUND BUILD =========================
#include "mmsolution.h"		// solution definitions
#include "mmsolution_bin.h"	// solution binary reference 

int32 frame = 0;//frame counter

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

u16 audioEffects[] = {
    //primary feedback sound set
    SFX_ZAP,//primary action feedback
    SFX_CRISPERROR_C4,//displeasure or cancel sound
    SFX_CHIPPULSE_C4,//working on it sound, tick, tick ..
    SFX_DRONE_C4,//buzzing about somewhere

    //must have game sounds
	SFX_EXPLODE,//classic bang reverb

	SFX_ALERTZONE_C3,//bubbly query
    SFX_ELECTROGLICK_C2,//boing electro thing
    SFX_GRISTLE_C4,//distorted chopper
    SFX_POWERX_C2,//electro buzz stab
    SFX_ROBOZ_C4//vocoder-ish
};

//===================== SOUND PROCESSING =======================
#define numberOfEffects sizeof(audioEffects) / sizeof(u16)

mm_sound_effect effectHandles[numberOfEffects];
bool effectTrigger[2][numberOfEffects];
int32 lastTriggered[numberOfEffects];
int32 soundHoldLength[numberOfEffects] = {
	0, 0, 0, 0,
	0,
	0, 0, 0, 0, 0
};

u8 curentAudioMod = -1;
bool sheduleAudio = true;
int32 volumeModPercent = 100;
int32 volumeEffectPercent = 100;

void Audio::playMod(u8 current) {
	if(curentAudioMod >= 0 && !sheduleAudio) mmStop();
	mmSetModuleVolume(volumeModPercent * 1024 / 100);
	mmStart(current, MM_PLAY_ONCE);
	curentAudioMod = current;
	sheduleAudio = false;
}

mm_sfxhand Audio::playEffect(int effect, bool foreground) {
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

void unloadMods() {
	for(u8 i = 0; i < MSL_NSONGS; ++i) {
		mmUnload(audioMods[i]);
	}
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
		effectTrigger[0][i] = effectTrigger[1][i] = false;
	}
}

void unloadEffects() {
	for(u8 i = 0; i < numberOfEffects; ++i) {
		mmUnloadEffect(audioEffects[i]);	
	}
}

void processAudio() {
	if(sheduleAudio) Audio::playMod(rand() % MSL_NSONGS);
	for(u8 i = 0; i < numberOfEffects; ++i) {
		if(effectTrigger[0][i] == true) {
			effectTrigger[0][i] = false;
			if(frame - lastTriggered[i] > soundHoldLength[i])
				Audio::playEffect(i, true);
		}
		if(effectTrigger[1][i] == true) {
			effectTrigger[1][i] = false;
			Audio::playEffect(i);
		}
	}
}

void Audio::cueEffect(int effect, bool foreground) {//multiple effect trigger group
	effectTrigger[foreground ? 0 : 1][effect] = true;
	lastTriggered[effect] = frame;
}

//============= FONT CLASS ==========================
Font::~Font() { 
	if(font_sprite != NULL) {
		delete[] font_sprite;
	}	
}

Font::Font(const u16 tileSize,
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

void Font::setExtended(bool low, bool high) {
	extended = ((low ? 1 : 0) | (high ? 2 : 0)) << 8;
}

void Font::print(int x, int y, const char *text) {
	uint font_char;
	while(*text) { 
		font_char = (*(unsigned char*)text++) | extended;
		glSprite(x, y, GL_FLIP_NONE, &font_sprite[font_char]);
		x += font_sprite[font_char].width; 
	}
}

void Font::printOutline(int x, int y) {
	glSprite(x, y, GL_FLIP_NONE, &font_sprite[0]);//use for null
}

void Font::printRight(int x, int y, const char *text) {
	x -= printWidth(text); 
	print(x, y, text);
}

void Font::printCentered(int y, const char *text) {
	int x = (SCREEN_WIDTH - printWidth(text)) / 2; 
	print(x, y, text);
}

int Font::printWidth(const char *text) {
	uint font_char;
	int total_width = 0;
	while(*text) {
		font_char = (*(unsigned char*)text++) | extended;
		total_width += font_sprite[font_char].width; 
	}
	return total_width;
}

int Font::getTextureID() {
	return textureID;//useful for 3D text
}

int Font::getTexturePack(u16 tile, CORNER coordinate, u16 scale) {
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

Font *font;//1024
Font *fontBig;//256

//================== TILES AND TEXTURES =======================
#include "font_si.h"
#include "font_16x16.h"
#include "threeDtex0.h"
#include "threeDtex1.h"
#include "subTiles.h"
#include "mainTiles.h"
#include "spriteTiles.h"
#include "logo.h"
#include "intro.h"
#include <decompress.h>

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
int32 stepFrames;
bool baulkAI = false;
bool paused = true;
bool exiting = false;
bool newGame = true;//loop until official exit
bool completeReset = false;

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
	Audio::playEffect(ACTION_FX);
}

int textureID[2];
View *subViewRXInput;
u16 keyIntercepted = 0;
u16 keyHoldAllow = 0;
int subBG[2];//2 sub backgrounds of 64 by 32 for clear space all around
int mainBG[2];//2 main backgrounds of 64 by 64 for clear space and fill (plus 3D NO!)
int32 gameSaveLoc = 0;
int32 numSaveLocs = 4 + 1;//last for default

//================ TILES, LAYERS AND PALETTES ==================
bool currently2D = true;
bool glInitialized = false;

void BG::setFor2D() {
	videoSetMode(MODE_5_2D);
	// Allocate VRAM bank for all the main palettes (32 K)
	mainBG[0] = bgInit(2, BgType_ExRotation, BgSize_ER_512x512, 24, 0);
	mainBG[1] = bgInit(3, BgType_ExRotation, BgSize_ER_512x512, 28, 0);
	//mainBG[2] = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0, 0);
	//clearMain(1);
	//ready for priorities
	//bgSetPriority(mainBG[2], 3);
	//3D ---> 0
	bgSetPriority(mainBG[0], 1);//SKY?
	bgSetPriority(mainBG[1], 2);
	//BG1 is not in use
	//bgHide(mainBG[2]);//hide 3D plane
	bgShow(mainBG[0]);
	bgShow(mainBG[1]);
	currently2D = true;
}

void loadCompressedTex(u8 *text, u16 *pal) {
	decompress(text, memory, LZ77Vram);//to buffer
	decompress(pal, strings, LZ77Vram);
}

void BG::setFor3D() {
	videoSetMode(MODE_5_3D);
	if(!glInitialized) {
		glInit();	
		// Load font textures
		//use memory and strings from lang.h to load
		loadCompressedTex((u8*)font_siBitmap, (u16*)font_siPal);
		font = new Font(8, (u16 *)strings, (u8 *)memory);
		loadCompressedTex((u8*)font_16x16Bitmap, (u16*)font_16x16Pal);
		fontBig = new Font(16, (u16 *)strings, (u8 *)memory);
		glGenTextures(4, (int *)&textureID);//make 4 textures
		glBindTexture(0, textureID[0]);//bind it
		loadCompressedTex((u8*)threeDtex0Bitmap, (u16*)threeDtex0Pal);
		glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_256, TEXTURE_SIZE_256,
			0, TEXGEN_TEXCOORD, memory);
		glColorTableEXT(0, 0, 255, 0, 0, (uint16 *)strings);
		glBindTexture(0, textureID[1]);//bind it
		loadCompressedTex((u8*)threeDtex1Bitmap, (u16*)threeDtex1Pal);
		glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_256, TEXTURE_SIZE_256,
			0, TEXGEN_TEXCOORD, memory);
		glColorTableEXT(0, 0, 255, 0, 0, (uint16 *)strings);
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

void BG::putMain(int bg, int x, int y, int tile) {
	u16 *map = bgGetMapPtr(mainBG[bg]);
	x += y * 64;
	x &= 4095;
	map += x;
	*map = tile;
}

void BG::putSub(int bg, int x, int y, int tile, int attribute) {
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

void BG::clearMain(int bg) {
	u16 *map = bgGetMapPtr(mainBG[bg]);
	for(int i = 0; i < 4096; ++i) {
		*map++ = 0;
	}
}

void BG::clearSub(int bg) {
	u16 *map = bgGetMapPtr(subBG[bg]) + 2048;//advanced attributes ...
	for(int i = 0; i < 1024; ++i) {
		*map++ = 0;
	}
	map = bgGetMapPtr(subBG[bg]);
	for(int i = 0; i < 2048; ++i) {
		*map++ = 0;
	}
}

void loadTitleMain(const unsigned int *tiles,
		const unsigned short *pal) {
	BG::setFor2D();
	decompress(tiles, bgGetGfxPtr(mainBG[0]), LZ77Vram);
	decompress(pal, BG_PALETTE, LZ77Vram);
	for(int x = 0; x < 32 * 24; ++x) {
		BG::putMain(0, x % 32, x / 32, x);
	}
	bgSetScroll(mainBG[0], 0, 0);//origin
	bgSetRotateScale(mainBG[0], 0, 1 << 8, 1 << 8);
	bgUpdate();
	bgExtPaletteDisable();//not needed?
}

void defaultTilesMain() {
	loadTitleMain(mainTilesTiles, mainTilesPal);
	bgExtPaletteEnable();//option
	BG::clearMain(0);
}

u16 *sprites[2048];//pointers to VRAM_D
int hiSprites = 0;
int layerSprite = 0;

void initSprites() {
	decompress(spriteTilesPal, memory, LZ77Vram);
	dmaCopy(memory, SPRITE_PALETTE_SUB, 512);//non extended palette for sprites
	//decompress(subTilesTiles, bgGetGfxPtr(subBG[0]), LZ77Vram);
	//128kB divided by 64 is 2048 glyphs
	decompress(spriteTilesTiles, strings, LZ77Vram);
	for(int i = 0; i < 2048; ++i) {
		sprites[i] = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_256Color);
		dmaCopy(strings + i * 64, sprites[i], 64);
	}
}

void BG::drawSprite(int number, int x, int y, int glyph) {
	oamSet(&oamSub, number & 127, x, y, layerSprite, glyph >> 12,//same as tile format
			SpriteSize_8x8, SpriteColorFormat_256Color, 
			sprites[(glyph & 1023) + hiSprites], -1, false, false,
			TILE_FLIP_H & glyph, TILE_FLIP_V & glyph, false);
}

void BG::setHighSprites(bool hi) {
	hiSprites = hi ? 1024 : 0;
}

void BG::setSpriteLayer(bool foreground) {
	layerSprite = foreground ? 2 : 3;
}

void BG::hideSprite(int number) {
	oamSetHidden(&oamSub, number, true);
}

void extendedPalettes() {
	vramSetBankH(VRAM_H_LCD);
	decompress(subTilesPal, memory, LZ77Vram);
	u16 palette[256];
	//16 palette slots for BG1 and BG2
	//as console and keyboard are 4bpp backgrounds
	//they are not affected with extended palettes
	//USE LAST 2 PALETTE LINES AS LIGHT ADJUSTMENT
	for(int i = 1; i < 3; ++i) {
		for(int p = 0; p < 16; ++p) {
			int col = memory[256 - 32 + (i - 1) * 16 + p];//multiplyer of light
			for(int k = 0; k < 256; ++k) {
				int r = RED(col) * RED(memory[k]) / 31;
				int b = BLUE(col) * BLUE(memory[k]) / 31;
				int g = GREEN(col) * GREEN(memory[k]) /31;
				palette[k] = RGB15(r, g, b);
			}
			DC_FlushAll();
			dmaCopy(palette, &VRAM_H_EXT_PALETTE[i][p], 512);//4096 colours max
		}
	}
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);//extended palette
	vramSetBankG(VRAM_G_LCD);
	decompress(mainTilesPal, memory, LZ77Vram);
	for(int i = 1; i < 3; ++i) {
		for(int p = 0; p < 16; ++p) {
			int col = memory[256 - 32 + (i - 1) * 16 + p];//multiplyer of light
			for(int k = 0; k < 256; ++k) {
				int r = RED(col) * RED(memory[k]) / 31;
				int b = BLUE(col) * BLUE(memory[k]) / 31;
				int g = GREEN(col) * GREEN(memory[k]) /31;
				palette[k] = RGB15(r, g, b);
			}
			DC_FlushAll();
			dmaCopy(palette, &VRAM_G_EXT_PALETTE[i + 1][p], 512);//4096 colours max
		}
	}
	vramSetBankG(VRAM_G_BG_EXT_PALETTE_SLOT23);
	vramSetBankI(VRAM_I_LCD);
	decompress(spriteTilesPal, memory, LZ77Vram);
	//16 palette slots for sprites
	for(int p = 0; p < 16; ++p) {
		int col = memory[256 - 16 + p];//multiplyer of light
		for(int k = 0; k < 256; ++k) {
			int r = RED(col) * RED(memory[k]) / 31;
			int b = BLUE(col) * BLUE(memory[k]) / 31;
			int g = GREEN(col) * GREEN(memory[k]) /31;
			palette[k] = RGB15(r, g, b);
		}
		DC_FlushAll();
		dmaCopy(palette, &VRAM_I_EXT_SPR_PALETTE[p], 512);
	}
	vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
}

//=========================== DRAWING ROUTINES ============================
GameLogic *game = new GameLogic();

//===================== INTERNAL AUTOMATION DRAWING HELP =====================
u16 drawSubMeta() {
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
	game->drawSub();
	oamUpdate(&oamSub);//sprites
	//int keyCode = keyboardUpdate();//for later
	//return masked keys
	scanKeys();
	if(keysDown() & KEY_START) {
		paused = !paused;//open pause state
		consoleClear();
	}
	//if(subViewRXInput != NULL);//process
	uint activeKeys = keysDown();
	//held keys check
	if(!paused) activeKeys |= (keyHoldAllow & keysHeld());
	return ~keyIntercepted & activeKeys;
	//return keysDown();
}

void indent() {
	const char *indent = "\n    ";
	iprintf(indent);
	iprintf(indent);
}

#define NUM_OPTIONS_MENU 2
int currentOption = 0;

int32 * const addressOpt[] = {
	&volumeModPercent,
	&volumeEffectPercent
};

const char *nameOpt[] = {
	"MUSIC VOLUME",
	"FX VOLUME"
};

const int32 incrementsOpt[] = {//100% scale int32 for all passed to decimal conversion
	5, 5
};

const FORMAT_PRINT printAsOpt[] = {
	PERCENT, PERCENT
};

void optionsText() {
	iprintf("[<] ");
	iprintf(nameOpt[currentOption]);
	iprintf(" ");
	switch(printAsOpt[currentOption]) {
		case(PERCENT):
			iprintf(printValue(addressOpt[currentOption]));
			iprintf("%%");
			break;
		case(ON_OFF):
			if(*(addressOpt[currentOption]) > 50) {
				iprintf(ANSI_GRN "ON" ANSI_WHT);
			} else {
				iprintf(ANSI_RED "OFF" ANSI_WHT);
			}
			break;
		default:
			break;
	}
	iprintf(" [>]");
}

void menuText(const char *abxy[], const char *lrWhat, char *lr,
		const char *start, const char *select) {
	consoleClear();
	const char *butABXY[] = {
		ANSI_RED "A",
		ANSI_YEL "B",
		ANSI_BLU "X",
		ANSI_GRN "Y"
	};
	indent();
	indent();
	for(int i = 0; i < 4; ++i) {
		iprintf(ANSI_WHT "[");
		iprintf(butABXY[i]);
		iprintf(ANSI_WHT "] ");
		iprintf(abxy[i]);
		indent();
	}
	iprintf("[L] ");
	iprintf(lrWhat);
	iprintf(" ");
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
	int slot = gameSaveLoc;
	if(defaultGame) slot = numSaveLocs - 1;
	game->loadGame(slot);
	Audio::playEffect(INFO_FX);
	enterFrameWhile();
}

void saveGame(bool defaultGame = false) {
	int slot = gameSaveLoc;
	if(defaultGame) slot = numSaveLocs - 1;
	game->saveGame(slot);
	Audio::playEffect(INFO_FX);
	if(!defaultGame) enterFrameWhile();//not the last save before exit?
}

void gameSplash() {
	//setFor2D();
	loadTitleMain(introTiles, introPal);//a screen
	progressMessage(GAME_INTRO_SCREEN);
}

void initGame() {
	gameSplash();
	game->initGame();
	//get default
	if(!completeReset) loadGame(true);
	completeReset = false;
	waitForKey(KEY_A_OR_START);
}

void startGame() {
	defaultTilesMain();//clears automatic (not sure if these are then useable)
	// initialize gl?
	//BG::setFor3D();//??
	game->startGame();
	enterFrameWhile();
}

void winSplash() {
	loadTitleMain(introTiles, introPal);//a screen
	progressMessage(GAME_WIN_SCREEN);
}

void CTL::gameCompleteRestart() {//call when ending happened
	winSplash();
	completeReset = true;
	exiting = true;
	paused = true;
}

void loseSplash() {
	loadTitleMain(introTiles, introPal);//a screen
	progressMessage(GAME_LOSE_SCREEN);
}

void CTL::gameLostResume() {//call when ending happened
	loseSplash();
	exiting = true;
	paused = true;
}

void applyInfrequentlyAccessedSettings() {
	mmSetModuleVolume(volumeModPercent * 1024 / 100);
	mmSetJingleVolume(volumeEffectPercent * 1024 / 100);//an effect
	//actual effects are short and setting used on playEffect()
}

void drawAndProcessMenu(u16 keysMasked) {
	if(subViewRXInput == NULL) {//intercept menu view for show special instead?
		//menu display
		const char *buttons[] = { "NEW GAME", "LOAD GAME", "EXIT", "SAVE GAME" };
		menuText(buttons, "SLOT", printValue(&gameSaveLoc),	"CONTINUE", "NEXT MUSIC TRACK");
		//cursor??
		optionsText();
		//menu keys
		if(keysMasked & (KEY_ALL_BUTTONS)) Audio::playEffect(ACTION_FX);
		if(keysMasked & (KEY_DPAD_X)) Audio::playEffect(OPTION_FX);
		if(keysMasked & (KEY_DPAD_Y)) Audio::playEffect(ACTION_FX);
		if(keysMasked & KEY_A) {
			newGame = true;//A
			completeReset = true;
			exiting = true;//just an exit back to ?
		}
		if(keysMasked & KEY_B) loadGame();//B
		if(keysMasked & KEY_X) {
			newGame = false;//X
			exiting = true;//just an exit back to ?
			saveGame(true);
			Audio::playEffect(SFX_EXPLODE);
		}
		if(keysMasked & KEY_Y) saveGame();//Y
		if(keysMasked & KEY_L) {
			gameSaveLoc -= 1;
			if(gameSaveLoc < 0) gameSaveLoc = numSaveLocs - 2;//save slot?
		}
		if(keysMasked & KEY_R) {
			gameSaveLoc += 1;
			//a general slot too
			if(gameSaveLoc > numSaveLocs - 2) gameSaveLoc = 0;//save slot?
		}
		if(keysMasked & KEY_LEFT) {
			currentOption = (currentOption - 1);//setting index
			if(currentOption < 0) currentOption += NUM_OPTIONS_MENU;
		}
		if(keysMasked & KEY_RIGHT) {
			currentOption = (currentOption + 1);//setting index
			currentOption %= NUM_OPTIONS_MENU;
		}
		if(keysMasked & KEY_UP) {
			int32 *opt = addressOpt[currentOption];
			*opt = (*opt + incrementsOpt[currentOption]);;//setting value
			if(*opt > 100) *opt = 100;
			applyInfrequentlyAccessedSettings();
		}
		if(keysMasked & KEY_DOWN) {
			int32 *opt = addressOpt[currentOption];
			*opt = (*opt - incrementsOpt[currentOption]);;//setting value
			if(*opt < 0) *opt = 0;
			applyInfrequentlyAccessedSettings();
		};
		/* if(keysMasked & KEY_START) {//unpause
			paused = false;//A
			consoleClear();//remove menu
		} */
		if(keysMasked & KEY_SELECT) {//BO SALECTA!!
			Audio::playMod(++curentAudioMod);
		}
	}
}

//=================== ANYTHING BAD FOR CHAIN LOADS ==================
void cleanUp() {
	consoleClear();
	unloadMods();
	while(mmActive()) swiWaitForVBlank();
	mmStop();
	unloadEffects();
	irqClear(IRQ_VBLANK);
	BG::setFor2D();
	glResetTextures();
	if(fontBig != NULL) delete fontBig;
	if(font != NULL) delete font;
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
    //D 128 -> SUB SPRITE
    //E 64 -> MAIN BG (EE) -> uses main palette as 3D doesn't
    //F 16 -> TEXTURE PALETTE (4 * 512 (2K of 16K) used) -> hi-colour
    //G 16 -> MAIN BG EXT PALETTE
    //H 32 -> SUB BG EXT PALETTE -> 4096 colours (auto palette by last 32 colour lights)
    //I 16 -> SUB SPRITE EXT PALETTE

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
	decompress(subTilesTiles, bgGetGfxPtr(subBG[0]), LZ77Vram);
	BG::clearSub(0);
	BG::clearSub(1);

	//sound
	mmInitDefaultMem((mm_addr)mmsolution_bin);
	irqSet(IRQ_VBLANK, updateFrame);
	loadMods();
	loadEffects();

	//upper screen
	bgExtPaletteEnableSub();
	extendedPalettes();
	// Set Bank A+B to texture (256 K)
	vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	vramSetBankE(VRAM_E_MAIN_BG);//for intro screens and ...
	vramSetBankF(VRAM_F_TEX_PALETTE_SLOT0);
	oamInit(&oamSub, SpriteMapping_1D_64, false);//only 16kB so could go as low as 1D_16
	//if it were possible
	initSprites();

	//Console
	bgSetPriority(console->bgId, 0);
	//keyboard ---> hack to 1
	bgSetPriority(k->background, 1);
	bgSetPriority(subBG[0],2);
	bgSetPriority(subBG[1],3);

	BG::setFor3D();//does preload through buffers of compressed textures once
	loadTitleMain(logoTiles, logoPal);
	progressMessage(INITIAL_LOAD);	
	Audio::playEffect(SFX_EXPLODE);
	//openAudioStream();// <-- can't have .mod aswell
	
	waitForKey(KEY_A_OR_START);
	//setPowerButtonCB(powerButtonPressed);//?? NO REFERENCE
	do {
		initGame();
		startGame();
		while(!exiting) {
			processAudio();
			//3D? Draw main screen
			if(!currently2D) {
				game->draw3D();
				glFlush(0);
			} else {
				game->drawMain();
			}
			//Draw sub screen
			if(!paused) {
				game->processInputs(drawSubMeta());//keysIntercepted?
				game->processMotions();
				game->processCollisions();
				game->processStateMachine();
			} else {
				drawAndProcessMenu(drawSubMeta());
			}
			/* while(inFrameCount()) {
				if(paused) {
					swiWaitForVBlank();//low power
				} else {
					//perform AI?? section
					//check baulkAI in intensive search
					swiWaitForVBlank();//low power
				}
			}	*/
			//test hack
			swiWaitForVBlank();
		}
	} while(newGame);
	cleanUp();
	exit(0);
}