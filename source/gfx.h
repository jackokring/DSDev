#ifndef GFX__H
#define GFX__H
#include "ctl.h"
#include <gl2d.h>
//============================== GRAPHICS INCLUDES ====================

class Font {
	public:	
	~Font();
	Font(const u16 tileSize, 
		const u16 *palette,
		const u8 *texture);
	void print(int x, int y, const char *text);
	void printRight(int x, int y, const char *text);
	void printCentered(int y, const char *text);
	int printWidth(const char *text);
	void setExtended(bool low, bool high);//for small font
	void printOutline(int x, int y);
	int getTextureID();
	int getTexturePack(u16 tile, CORNER coordinate, u16 scale = 1);
	
	private:
	glImage *font_sprite;
	int textureID;
	int extended = 0;
};

class BG {
    public:
        static void putMain(int bg, int x, int y, int tile);
        static void putSub(int bg, int x, int y, int tile, int attribute = 0);
        static void clearMain(int bg);
        static void clearSub(int bg);

		//lower display sprites
		static void drawSprite(int number, int x, int y, int glyph);
		static void hideSprite(int number);

		//upper display (main) 2 or 3 D select
		static void setFor2D();
		static void setFor3D();
};

#endif