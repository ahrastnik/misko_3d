/*
 * BackgroundServices.h
 *
 * Created: 04-Apr-17 14:55:29
 *  Author: maticpi
 */ 

#ifndef BACKGROUNDSERVICES_H_
#define BACKGROUNDSERVICES_H_

#include <avr/io.h>
#include <string.h>
#include "LCD_Ili9341.h"
#include "systime.h"
#include "kbd.h"

extern int score;
extern int level;
extern int lives;
extern char display_msg;
#define DISP_NO_CHANGE			0
#define DISP_MOLE1				1
#define DISP_MOLE2				2
#define DISP_MOLE3				3
#define DISP_MOLE4				4
#define DISP_MOLE_NONE			5
#define DISP_MOLE_ALL			6
#define DISP_REFRESH_SCORE		7
#define DISP_REFRESH_LEVEL		8
#define DISP_REFRESH_LIVES		9
#define DISP_REFRESH_ALL		10
#define DISP_START_SCREEN		11
#define DISP_GAME_PLAY_SCREEN	12
#define DISP_END_SCREEN			13
#define DSIP_END_SCORE			14

void BackgroundServices();
void RefreshLCDandLEDs();
//void LED_ShowMoleNr(char mole_nr);
void LCD_ShowMoleNr(char mole_nr);
void LCD_RefreshScore();
void LCD_RefreshLives();
void LCD_RefreshLevel();
void DrawStartScreen();
void DrawGamePlayScreen();
void DrawGameOver();
void DrawGameScore();
void DrawIcon_P(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t bgClr, uint16_t fgClr, const uint8_t *data);
void DrawSprite_P(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t bgClr, const uint16_t *data);
extern const PROGMEM uint16_t sprite[30][31];
extern const PROGMEM uint8_t lifeIco[11][2];
#endif /* BACKGROUNDSERVICES_H_ */