#include "game.h"
//============================== GAME INCLUDES ====================
u32 randVal;

u16 pRand(u16 range) {//level generator random as seed can be set and independant
	randVal = randVal * 134775813 + 1;
  	return ((randVal >> 16) * range) >> 16;
}

void qRand(u32 seed) {
	randVal = seed;
}

void rRand() {
	randVal = (randVal - 1) * 3645876429;//reverse by multiplicative inverse
}

void GameLogic::initGame() {

}
    
void GameLogic::startGame() {

}

void GameLogic::loadGame(int slot) {

}
    
void GameLogic::saveGame(int slot) {
	
}

void GameLogic::drawMain() {

}

void GameLogic::drawSub() {

}

void draw2D() {//a 2D overlay on 3D
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

void GameLogic::draw3D() {
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

	//scanKeys();
	
	u16 keys = 0;//keysHeld();
	
	if((keys & KEY_UP)) rotateX += 3;
	if((keys & KEY_DOWN)) rotateX -= 3;
	if((keys & KEY_LEFT)) rotateY += 3;
	if((keys & KEY_RIGHT)) rotateY -= 3;
	
	//glBindTexture(0, textureID[0]);
	//font.

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
	draw2D();
}

void GameLogic::processInputs(u16 keysMasked) {
	/* if(keysMasked & KEY_START) paused = true;// <- handled elsewhere */
}

void GameLogic::processMotions() {

}

void GameLogic::processCollisions() {

}

void GameLogic::processStateMachine() {
	
}
