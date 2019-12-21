/*
 * MISKO_3D.c
 *
 * Created: 29. 08. 2017 14:03:42
 * Author : Adam Hrastnik
 */ 

#include <avr/io.h>
#include <string.h>
#include "LCD_Ili9341.h"
#include "systime.h"
#include "kbd.h"
#include "misko_ad.h"
#include "ugui.h"
#include "misko3d.h"

//#define DEBUG_DATA

// Modes
#define MODE_NUM	3
#define OBJECT		0
#define EDIT		1
#define SCULPT		2

char modes[MODE_NUM][16] = {"Mode: Object", "Mode: Edit  ", "Mode: Sculpt"};
uint8_t mode = 0; // Choose object modifing mode

// Tools
#define TOOL_NUM	4
#define SELECT		0
#define TRANSLATE	1
#define ROTATE		2
#define SCALE		3

char tools[TOOL_NUM][16] = {"Tool: Select   ", "Tool: Translate", "Tool: Rotate   ", "Tool: Scale    "};
uint8_t tool = 1; // Choose Translation as Default Tool

#define OPERATION_MODE_NUM	3
#define XY					0
#define YZ					1
#define XZ					2

char operationModes[OPERATION_MODE_NUM][16] = {"Mode: XY", "Mode: YZ", "Mode: XZ"};
uint8_t operationMode = YZ;

void Init_IO();
// Interface
#define START_SCREEN		0
#define EDITOR				1
#define START_SCREEN_DELAY	5000
#define START_SCREEN_COLOR	C_ORANGE
char state = START_SCREEN;
uint32_t timer = 0;
void startScreen();
void drawLogo(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t bgClr, uint16_t fgClr, const uint8_t *data);
const uint8_t PROGMEM logo[32][32] = {{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0xe8,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec}
	,{0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec}
	,{0x00,0x00,0x00,0xe8,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec}
	,{0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec}
	,{0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x09,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec}
	,{0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec}
	,{0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x0a,0x03,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00}
	,{0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00}
	,{0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
	,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

void selectMode(uint8_t newMode);
void selectTool(uint8_t newTool);
void selectOperationMode(uint8_t newMode);
void switchViewMode(uint8_t newViewMode);
void redraw();
uint8_t cycle(uint8_t index, uint8_t maxIndex, uint8_t direction);

// Cursor
#define CURSOR_COLOR			C_WHITE
#define CURSOR_SIZE				18
#define CURSOR_RADIUS			CURSOR_SIZE/2
#define CURSOR_THICNESS			1
#define CURSOR_SELECT_RADIUS	10

// Cursor Types
#define CURSOR_CROSS	0
#define CURSOR_BRUSH	1
uint8_t brushSize = 40;

struct cursor{
	uint16_t posX, posY;
	uint8_t bVisible;
	uint8_t type;
}Cursor;

void redrawCursor();
uint8_t isVertexUnderCursor(struct vector3d *vert);
void selectVertex();
float gaussian(float x, float width);
void sculpt();
uint8_t bSculpting = 0;

// Menu
#define MENU				C_DARK_GRAY
#define MENU_BORDER			C_SLATE_GRAY
#define MENU_BORDER_WIDTH	5
void openMenu();
void closeMenu();
void openSubMenu();
void closeSubMenu();
void fillSubMenu();
void clearSubMenu();
uint8_t bIsMenuOpen = 0;
uint8_t bIsSubMenuOpen = 0;

struct button{
	unsigned int pos[2];
	char text[16];
	unsigned int index;
	uint8_t bSelected;
	uint8_t browsingMenu;
};
#define BUTTON_COLOR						C_DIM_GRAY
#define BUTTON_PRESSED_COLOR				C_SKY_BLUE
#define BUTTON_PRESSED_NOT_ACTIVE_COLOR		C_SLATE_GRAY
#define BUTTON_TEXT_COLOR					C_WHITE
#define BUTTON_PRESSED_TEXT_COLOR			C_BLACK
#define BUTTON_WIDTH						120
#define BUTTON_HEIGHT						20
#define MAIN_MENU							0
#define SUB_MENU							1
uint8_t browsingMenu = MAIN_MENU;
struct button *menuButtons;
unsigned int numOfMenuButtons = 0;
unsigned int selectedMenuBtn = 0;
struct button *subMenuButtons;
unsigned int numOfSubMenuButtons = 0;
unsigned int selectedSubMenuBtn = 0;
void createButton(char *text, unsigned int pos[2], uint8_t browsingMenu, struct button *buttonGroup, unsigned int *btnCnt);
void updateButton(struct button *btn);
void selectNextButton(struct button *buttonGroup, unsigned int btnCnt, unsigned int *currentSelectedBtn, uint8_t direction);
void executeSelection();

// Object
#define OBJECT_EEPROM_ADDRESS	72
void SaveObject(struct object *obj);
void LoadObject(struct object *obj);

int main(void)
{
    Init_IO();
	init_AD();
    LCD_Init();
    Systime_Init();
	init3D();
    sei();

	// Init Cursor
	Cursor.posX = SCREEN_X_MID;
	Cursor.posY = SCREEN_Y_MID;
	Cursor.bVisible = 0;
	Cursor.type = CURSOR_CROSS;

	// Select Translate tool as Default
	selectMode(OBJECT);
	selectTool(TRANSLATE);
	selectOperationMode(XY);
	switchViewMode(PRESPECTIVE);

	startScreen();

    while (1)
    {
		switch(state){
			case START_SCREEN:
				if(Has_X_MillisecondsPassed(START_SCREEN_DELAY, &timer)){
					// Clear start Screen
					clearViewport();
					// Add Cube as Default Object
					AddCube(50);
					active_object.rotation[0] = 0.785;
					active_object.rotation[1] = 0.785;
					active_object.rotation[2] = 0.785;
					setRotation(&active_object);
					redrawObject(&active_object, 1);

					state++;
				}
				break;

			case EDITOR:
				if(HasOneMillisecondPassed()){
					KBD_Read();

					// Handle Keys
					switch(mode){
						case OBJECT:
						switch(bIsMenuOpen){
							case 0:
							switch(KBD_GetKey()){
								// Handel Keyboard in Editor
								case BTN_OK:
								// Change Mode
								selectMode(EDIT);
								selectTool(SELECT);
								break;
								case BTN_ESC:
								// Switch Tools
								selectTool(cycle(tool, TOOL_NUM, 1));
								break;
								case BTN_A:
								// Change Operation Axis
								selectOperationMode(cycle(operationMode, OPERATION_MODE_NUM, 1));
								break;
								case BTN_B:
								if(!bIsMenuOpen){
									openMenu();
								}
								break;
							}
							break;

							case 1:
							switch(KBD_GetKey()){
								// Handel Keyboard in Menu
								case BTN_OK:
								switch(browsingMenu){
									case MAIN_MENU:
									browsingMenu = SUB_MENU;
									updateButton(&menuButtons[selectedMenuBtn]);
									updateButton(&subMenuButtons[selectedSubMenuBtn]);
									break;
									case SUB_MENU:
									
									executeSelection();
									closeMenu();
									break;
								}
								break;
								case BTN_ESC:
								switch(browsingMenu){
									case MAIN_MENU:
									closeMenu();
									break;
									case SUB_MENU:
									browsingMenu = MAIN_MENU;
									updateButton(&menuButtons[selectedMenuBtn]);
									updateButton(&subMenuButtons[selectedSubMenuBtn]);
									break;
								}
								break;
								case BTN_A:
								
								switch(browsingMenu){
									case MAIN_MENU:
									clearSubMenu();
									selectNextButton(menuButtons, numOfMenuButtons, &selectedMenuBtn, 0);
									fillSubMenu();
									break;
									case SUB_MENU:
									selectNextButton(subMenuButtons, numOfSubMenuButtons, &selectedSubMenuBtn, 0);
									break;
								}
								break;
								case BTN_B:
								switch(browsingMenu){
									case MAIN_MENU:
									clearSubMenu();
									selectNextButton(menuButtons, numOfMenuButtons, &selectedMenuBtn, 1);
									fillSubMenu();
									break;
									case SUB_MENU:
									selectNextButton(subMenuButtons, numOfSubMenuButtons, &selectedSubMenuBtn, 1);
									break;
								}
								break;
							}
							break;
						}
						break;

						case EDIT:
						switch(KBD_GetKey()){
							// Handel Keyboard in Editor
							case BTN_OK:
							// Change Mode
							selectMode(SCULPT);
							//selectTool(TRANSLATE);
							break;

							case BTN_ESC:
							// Switch Tools
							selectTool(cycle(tool, TOOL_NUM, 1));
							break;

							case BTN_A:
							// Change Operation Axis
							selectOperationMode(cycle(operationMode, OPERATION_MODE_NUM, 1));
							break;

							case BTN_B:
							if(tool == SELECT){
								selectVertex();
								}else{
								selectTool(SELECT);
							}
							break;
						}
						break;

						case SCULPT:
							switch(KBD_GetKey()){
								case BTN_OK:
									selectMode(OBJECT);
									selectTool(TRANSLATE);
									break;
								case BTN_ESC:
									break;
								case BTN_A:
									break;
								case BTN_B:
									bSculpting = 1;
									break;
							}
							switch(KBD_GetReleasedKey()){
								case BTN_OK:
									break;
								case BTN_ESC:
									break;
								case BTN_A:
									break;
								case BTN_B:
									bSculpting = 0;
									break;
							}
							break;
					}

					
					if(!bIsMenuOpen){
						readChannel(&x_axis);
						readChannel(&y_axis);

						x_axis.currentPercentage = getChannelPercentage(&x_axis);
						y_axis.currentPercentage = getChannelPercentage(&y_axis);

						if((x_axis.prevPercentage != x_axis.currentPercentage) || (y_axis.prevPercentage != y_axis.currentPercentage)){
							x_axis.prevPercentage = x_axis.currentPercentage;
							y_axis.prevPercentage = y_axis.currentPercentage;

							char tempStr[32];
							sprintf(tempStr, "X:%3d Y:%3d", x_axis.currentPercentage, y_axis.currentPercentage);
							UG_PutString(220, 10, tempStr);

							switch(mode){
								case OBJECT:
								switch(tool){
									case TRANSLATE:
									// Translate Object
									switch(operationMode){
										case XY:
										active_object.location[0] += ((float)x_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
										active_object.location[1] += ((float)y_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
										break;
										case YZ:
										active_object.location[1] += ((float)y_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
										active_object.location[2] += ((float)x_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
										break;
										case XZ:
										active_object.location[0] += ((float)x_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
										active_object.location[2] += ((float)y_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
										break;
									}
									#ifdef DEBUG_DATA
									sprintf(tempStr, "X:%3d Y:%3d Z:%3d", (int)active_object.location[0], (int)active_object.location[1], (int)active_object.location[2]);
									UG_PutString(SCREEN_CLAMP_X_MIN+2, SCREEN_CLAMP_Y_MAX-12, tempStr);
									#endif
									setLocation(&active_object);
									break;

									case ROTATE:
									// Rotate Object

									switch(operationMode){
										case XY:
										active_object.rotation[0] += (((float)y_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY);
										active_object.rotation[1] += (((float)-x_axis.currentPercentage+50)*0.01*JOYSTICK_SENSITIVITY);
										break;
										case YZ:
										active_object.rotation[1] += (((float)x_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY);
										active_object.rotation[2] += (((float)y_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY);
										break;
										case XZ:
										active_object.rotation[0] += (((float)-y_axis.currentPercentage+50)*0.01*JOYSTICK_SENSITIVITY);
										active_object.rotation[2] += (((float)x_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY);
										break;
									}
									#ifdef DEBUG_DATA
									sprintf(tempStr, "X:%f Y:%f Z:%f", active_object.rotation[0], active_object.rotation[1], active_object.rotation[2]);
									UG_PutString(SCREEN_CLAMP_X_MIN+2, SCREEN_CLAMP_Y_MAX-12, tempStr);
									#endif
									setRotation(&active_object);
									break;

									case SCALE:
									// Scale Object
									switch(operationMode){
										case XY:
										active_object.scale[0] += ((float)x_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY;
										active_object.scale[1] += ((float)y_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY;
										break;
										case YZ:
										active_object.scale[1] += ((float)y_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY;
										active_object.scale[2] += ((float)x_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY;
										break;
										case XZ:
										active_object.scale[0] += ((float)x_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY;
										active_object.scale[2] += ((float)y_axis.currentPercentage-50)*0.01*JOYSTICK_SENSITIVITY;
										break;
									}
									#ifdef DEBUG_DATA
									sprintf(tempStr, "X:%f Y:%f Z:%f", active_object.scale[0], active_object.scale[1], active_object.scale[2]);
									UG_PutString(SCREEN_CLAMP_X_MIN+2, SCREEN_CLAMP_Y_MAX-12, tempStr);
									#endif
									setScale(&active_object);
									break;
									default:
									selectTool(TRANSLATE);
									break;
								}
								redrawObject(&active_object, 1);
								break;

								case EDIT:
								switch(tool){
									case SELECT:
									break;

									case TRANSLATE:
									if(doesObjectHaveSelectedVertices(&active_object)){
										uint16_t i;
										for(i = 0; i < active_object.numOfVertecies; i++){
											if(active_object.selectedVertecies[i]){
												active_object.verteciesLocal[i].x += ((float)x_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
												active_object.verteciesLocal[i].y += ((float)y_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
											}
										}
									}
									break;

									case ROTATE:
									break;

									case SCALE:
									if(doesObjectHaveSelectedVertices(&active_object)){
										uint16_t i;
										for(i = 0; i < active_object.numOfVertecies; i++){
											if(active_object.selectedVertecies[i]){
												if(x_axis.currentPercentage-50 > 0){
													active_object.verteciesLocal[i].x *= ((float)x_axis.currentPercentage-50)*0.5*JOYSTICK_SENSITIVITY;
													}else if(x_axis.currentPercentage-50 < 0){
													active_object.verteciesLocal[i].x /= abs((float)-x_axis.currentPercentage+50)*JOYSTICK_SENSITIVITY;
												}
												
												if(y_axis.currentPercentage-50 > 0){
													active_object.verteciesLocal[i].y *= ((float)y_axis.currentPercentage-50)*0.5*JOYSTICK_SENSITIVITY;
													}else if(y_axis.currentPercentage-50 < 0){
													active_object.verteciesLocal[i].y /= abs((float)-y_axis.currentPercentage+50)*JOYSTICK_SENSITIVITY;
												}
											}
										}
									}
									break;
								}

								Cursor.bVisible = 0;
								redrawCursor();
								Cursor.posX += (x_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
								Cursor.posY += (y_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
								Cursor.bVisible = 1;
								redrawCursor();
								break;
							
							case SCULPT:
								if(bSculpting){
									sculpt();
								}

								Cursor.bVisible = 0;
								redrawCursor();
								Cursor.posX += (x_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
								Cursor.posY += (y_axis.currentPercentage-50)*JOYSTICK_SENSITIVITY;
								Cursor.bVisible = 1;
								redrawCursor();
								break;
							}
						}
					}
				}
				break;
		}
    }
}

void Init_IO()
{
	//DDR: 1-out, 0-in
	PORTB = 0x12;	//0-LCD_DC, 1-buzzer, 2-LCD_CS,
	DDRB = 0x2D;	//3-MOSI, 4-MISO, 5-SCK, 6,7-not implemented
	
	PORTC = 0xF0;	//0..3-analog inputs
	DDRC = 0x00;	//4-SDA(not connected), 5-SCL(not connected), 6,7-not implemented
	
	PORTD = 0xBF;	//0-Rx, 1-Tx, 2..5-buttons
	DDRD = 0xC2;	//6-LCD_BACKLIGHT, 7-LCD_RESET
	
	PORTE = 0x00;	//LEDs
	DDRE = 0xFF;
}

void redrawCursor(){
	// Clamp Cursor to viewport
	if(Cursor.posX < SCREEN_CLAMP_X_MIN){
		Cursor.posX = SCREEN_CLAMP_X_MAX;
	}else if(Cursor.posX > SCREEN_CLAMP_X_MAX){
		Cursor.posX = SCREEN_CLAMP_X_MIN;
	}
	if(Cursor.posY < SCREEN_CLAMP_Y_MIN){
		Cursor.posY = SCREEN_CLAMP_Y_MAX;
	}else if(Cursor.posY > SCREEN_CLAMP_Y_MAX){
		Cursor.posY = SCREEN_CLAMP_Y_MIN;
	}

	if(Cursor.bVisible){
			if(tool == SELECT && !bSculpting){
				redrawObject(&active_object, 0);
			}else{
				redrawObject(&active_object, 1);
			}
		
		switch(Cursor.type){
			case CURSOR_CROSS:
				UG_FillFrame(Cursor.posX-CURSOR_RADIUS, Cursor.posY-CURSOR_THICNESS, Cursor.posX+CURSOR_RADIUS, Cursor.posY+CURSOR_THICNESS, CURSOR_COLOR);
				UG_FillFrame(Cursor.posX-CURSOR_THICNESS, Cursor.posY-CURSOR_RADIUS, Cursor.posX+CURSOR_THICNESS, Cursor.posY+CURSOR_RADIUS, CURSOR_COLOR);
				break;

			case CURSOR_BRUSH:
				UG_DrawCircle(Cursor.posX, Cursor.posY, brushSize, CURSOR_COLOR);
				break;
		}
	}else{
		switch(Cursor.type){
			case CURSOR_CROSS:
				UG_FillFrame(Cursor.posX-CURSOR_RADIUS, Cursor.posY-CURSOR_THICNESS, Cursor.posX+CURSOR_RADIUS, Cursor.posY+CURSOR_THICNESS, BACKGROUND);
				UG_FillFrame(Cursor.posX-CURSOR_THICNESS, Cursor.posY-CURSOR_RADIUS, Cursor.posX+CURSOR_THICNESS, Cursor.posY+CURSOR_RADIUS, BACKGROUND);
				break;

			case CURSOR_BRUSH:
				UG_DrawCircle(Cursor.posX, Cursor.posY, brushSize, BACKGROUND);
				break;
		}
		
	}
}

uint8_t isVertexUnderCursor(struct vector3d *vert){
	struct vector2d vert2d;
	_project(vert, &vert2d);
	vert2d.x -= Cursor.posX;
	vert2d.y -= Cursor.posY;
	vert2d.x = abs(vert2d.x);
	vert2d.y = abs(vert2d.y);

	if(Cursor.type == CURSOR_CROSS){
		if((vert2d.x < CURSOR_SELECT_RADIUS) && (vert2d.y < CURSOR_SELECT_RADIUS)){
			return 1;
		}
	}else{
		if((vert2d.x < brushSize) && (vert2d.y < brushSize)){
			return 1;
		}
	}
	
	return 0;
}

void selectVertex(){
	uint16_t i;
	for(i = 0; i < active_object.numOfVertecies; i++){
		if(isVertexUnderCursor(&active_object.verteciesWorld[i])){
			active_object.selectedVertecies[i] ^= 1;
			redrawObject(&active_object, 0);
			redrawCursor();
			break;
		}
	}
}

float gaussian(float x, float width){
	return exp(-pow((x/width), 2));
}

void sculpt(){
	uint16_t i;
	struct vector2d vert2d;
	for(i = 0; i < active_object.numOfVertecies; i++){
		_project(&active_object.verteciesWorld[i], &vert2d);
		vert2d.x -= Cursor.posX;
		vert2d.y -= Cursor.posY;
		vert2d.x = abs(vert2d.x);
		vert2d.y = abs(vert2d.y);
		if((vert2d.x < brushSize) && (vert2d.y < brushSize)){
			active_object.verteciesLocal[i].x += ((float)x_axis.currentPercentage-50)*gaussian(vert2d.x, brushSize/3)*JOYSTICK_SENSITIVITY;
			active_object.verteciesLocal[i].y += ((float)y_axis.currentPercentage-50)*gaussian(vert2d.y, brushSize/3)*JOYSTICK_SENSITIVITY;
		}
	}
}

void startScreen(){
	char text1[8] = "Blender";
	char text2[12] = "AVR Edition";
	UG_SetForecolor(START_SCREEN_COLOR);
	UG_PutString(SCREEN_X_MID-((strlen(text1)*9)/2), SCREEN_Y_MID-10, text1);
	UG_PutString(SCREEN_X_MID-((strlen(text2)*9)/2), SCREEN_Y_MID+10, text2);
	//drawLogo(SCREEN_X_MID-16, SCREEN_Y_MID-48, 32, 32, BACKGROUND, C_GRAY, &logo[0][0]);
	UG_SetForecolor(FOREGROUND);
	// Draw Gauss Curve
	struct vector2d P1, P2;
	P1.x = SCREEN_X_MID-50;
	P1.y = SCREEN_Y_MID+80;
	uint8_t i;
	for(i = 0; i < 100; i++){
		P2.x = i-50+SCREEN_X_MID;
		P2.y = (-30*gaussian(i-50, 20))+80+SCREEN_Y_MID;
		UG_DrawLine(P1.x, P1.y, P2.x, P2.y, START_SCREEN_COLOR);
		P1.x = P2.x;
		P1.y = P2.y;
	}
	// Set Start Timer
	timer = GetSysTick();
}

void drawLogo(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t bgClr, uint16_t fgClr, const uint8_t *data)
{
	uint16_t clr;
	int i,j,idx=0;
	char d=0,bit;
	
	ILI9341_setAddrWindow(x,y,x+w-1,y+h-1);
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			bit = i & 0x07;
			if (bit == 0)
			{
				d=pgm_read_byte((uint16_t)data+idx);
				if (w-i>8) idx++;
			}
			clr = d & (0x80>>bit) ? fgClr : bgClr;
			ILI9341_pushColor(clr);
		}
		idx++;
	}
}

void selectMode(uint8_t newMode){
	PORTE = 0;
	PORTE = 1 << newMode;
	UG_PutString(10, 10, modes[newMode]);
	if(newMode != mode){
		mode = newMode;
		
		if(mode == EDIT){
			bEditing = 1;
		}else{
			bEditing = 0;
		}
		clearViewport();
		redrawObject(&active_object, 0);
		
		switch(mode){
			case OBJECT:
				Cursor.bVisible = 0;
				break;

			case EDIT:
				Cursor.bVisible = 1;
				Cursor.type = CURSOR_CROSS;
				redrawCursor();
				break;

			case SCULPT:
				Cursor.bVisible = 1;
				Cursor.type = CURSOR_BRUSH;
				redrawCursor();
				break;
		}
	}
}

void selectTool(uint8_t newTool){
	// Select new Tool
	if((mode == OBJECT) && (newTool == SELECT)){
		tool = TRANSLATE;
	}else{
		tool = newTool;
	}
	
	UG_PutString(10, 220, tools[tool]);
}

void selectOperationMode(uint8_t newMode){
	operationMode = newMode;
	UG_PutString(160, 220, operationModes[newMode]);
}

void switchViewMode(uint8_t newViewMode){
	viewMode = newViewMode;
	/*clearViewport();
	redrawObject(&active_object, 0);*/
}

void openMenu(){
	bIsMenuOpen = 1;
	UG_FillFrame(SCREEN_CLAMP_X_MIN+10, SCREEN_CLAMP_Y_MIN+10, SCREEN_X_MID, SCREEN_CLAMP_Y_MAX-10, MENU_BORDER);
	UG_FillFrame(SCREEN_CLAMP_X_MIN+10+MENU_BORDER_WIDTH, SCREEN_CLAMP_Y_MIN+10+MENU_BORDER_WIDTH, SCREEN_X_MID-MENU_BORDER_WIDTH, SCREEN_CLAMP_Y_MAX-MENU_BORDER_WIDTH-10, MENU);
	uint16_t pos[2] = {30, 55};
	menuButtons = calloc(4, sizeof(struct button));
	createButton("Switch Object", pos, MAIN_MENU, menuButtons, &numOfMenuButtons);
	pos[1] += 25;
	createButton("Object", pos, MAIN_MENU, menuButtons, &numOfMenuButtons);
	pos[1] += 25;
	createButton("View", pos, MAIN_MENU, menuButtons, &numOfMenuButtons);
	pos[1] += 25;
	createButton("Reset Object", pos, MAIN_MENU, menuButtons, &numOfMenuButtons);
	menuButtons[selectedMenuBtn].bSelected = 1;
	// Draw Buttons
	uint8_t i;
	for(i = 0; i < numOfMenuButtons; i++){
		updateButton(&menuButtons[i]);
	}

	// Open First Sub Menu
	openSubMenu();
}

void closeMenu(){
	bIsMenuOpen = 0;
	free(menuButtons);
	numOfMenuButtons = 0;
	browsingMenu = MAIN_MENU;
	closeSubMenu();
	clearViewport();
	redrawObject(&active_object, 0);
}

void openSubMenu(){
	if(bIsSubMenuOpen){
		closeSubMenu();
	}else{
		bIsSubMenuOpen = 1;
		UG_FillFrame(SCREEN_X_MID, SCREEN_CLAMP_Y_MIN+10, SCREEN_CLAMP_X_MAX-10, SCREEN_CLAMP_Y_MAX-10, MENU_BORDER);
		UG_FillFrame(SCREEN_X_MID+MENU_BORDER_WIDTH, SCREEN_CLAMP_Y_MIN+10+MENU_BORDER_WIDTH, SCREEN_CLAMP_X_MAX-MENU_BORDER_WIDTH-10, SCREEN_CLAMP_Y_MAX-MENU_BORDER_WIDTH-10, MENU);
		fillSubMenu();
	}
}

void closeSubMenu(){
	if(bIsSubMenuOpen){
		free(subMenuButtons);
		numOfSubMenuButtons = 0;
		selectedSubMenuBtn = 0;
		bIsSubMenuOpen = 0;
	}
}

void fillSubMenu(){
	uint16_t pos[2] = {SCREEN_X_MID + 10, 55};

	switch(selectedMenuBtn){
		case 0:
			subMenuButtons = calloc(5, sizeof(struct button));
			createButton("Rectangle", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Circle", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Cube", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Cylinder", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Axis", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			break;

		case 1:
			subMenuButtons = calloc(2, sizeof(struct button));
			createButton("Save", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Load", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			break;

		case 2:
			subMenuButtons = calloc(2, sizeof(struct button));
			createButton("Perspective", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Orthographic", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			break;

		case 3:
			subMenuButtons = calloc(4, sizeof(struct button));
			createButton("Location", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Rotation", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("Scale", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			pos[1] += 25;
			createButton("All", pos, SUB_MENU, subMenuButtons, &numOfSubMenuButtons);
			break;
	}
	
	subMenuButtons[selectedSubMenuBtn].bSelected = 1;
	// Draw Buttons
	uint8_t i;
	for(i = 0; i < numOfSubMenuButtons; i++){
		updateButton(&subMenuButtons[i]);
	}
}

void clearSubMenu(){
	UG_FillFrame(SCREEN_X_MID+MENU_BORDER_WIDTH, SCREEN_CLAMP_Y_MIN+10+MENU_BORDER_WIDTH, SCREEN_CLAMP_X_MAX-MENU_BORDER_WIDTH-10, SCREEN_CLAMP_Y_MAX-MENU_BORDER_WIDTH-10, MENU);
	free(subMenuButtons);
	numOfSubMenuButtons = 0;
	selectedSubMenuBtn = 0;
}

void createButton(char *text, unsigned int pos[2], uint8_t browsingMenu, struct button *buttonGroup, unsigned int *btnCnt){
	struct button btn;
	btn.pos[0] = pos[0];
	btn.pos[1] = pos[1];
	strcpy(btn.text, text);
	btn.index = *btnCnt;
	btn.bSelected = 0;
	btn.browsingMenu = browsingMenu;
	buttonGroup[*btnCnt] = btn;

	*btnCnt = (*btnCnt) + 1;
}

void updateButton(struct button *btn){
	if(btn->bSelected){
		if(btn->browsingMenu == browsingMenu){
			UG_FillFrame(btn->pos[0], btn->pos[1], btn->pos[0]+BUTTON_WIDTH, btn->pos[1]+BUTTON_HEIGHT, BUTTON_PRESSED_COLOR);
			UG_SetBackcolor(BUTTON_PRESSED_COLOR);
		}else{
			UG_FillFrame(btn->pos[0], btn->pos[1], btn->pos[0]+BUTTON_WIDTH, btn->pos[1]+BUTTON_HEIGHT, BUTTON_PRESSED_NOT_ACTIVE_COLOR);
			UG_SetBackcolor(BUTTON_PRESSED_NOT_ACTIVE_COLOR);
		}
		
		UG_SetForecolor(BUTTON_PRESSED_TEXT_COLOR);
		UG_PutString(btn->pos[0]+(BUTTON_WIDTH/2)-((strlen(btn->text)*9)/2), btn->pos[1]+5, btn->text);
	}else{
		UG_FillFrame(btn->pos[0], btn->pos[1], btn->pos[0]+BUTTON_WIDTH, btn->pos[1]+BUTTON_HEIGHT, BUTTON_COLOR);
		UG_SetBackcolor(BUTTON_COLOR);
		UG_SetForecolor(BUTTON_TEXT_COLOR);
		UG_PutString(btn->pos[0]+(BUTTON_WIDTH/2)-((strlen(btn->text)*9)/2), btn->pos[1]+5, btn->text);
	}
	UG_SetBackcolor(BACKGROUND);
	UG_SetForecolor(FOREGROUND);
}

void selectNextButton(struct button *buttonGroup, unsigned int btnCnt, unsigned int *currentSelectedBtn, uint8_t direction){
	unsigned int selectedBtn = *currentSelectedBtn;
	buttonGroup[selectedBtn].bSelected = 0;
	updateButton(&buttonGroup[selectedBtn]);
	*currentSelectedBtn = cycle(selectedBtn, btnCnt, direction);
	selectedBtn = *currentSelectedBtn;
	buttonGroup[selectedBtn].bSelected = 1;
	updateButton(&buttonGroup[selectedBtn]);
}

void executeSelection(){
	switch(selectedMenuBtn){
		case 0:
			switch(selectedSubMenuBtn){
				case 0:
					AddRectangle(50);
					break;
				case 1:
					AddCircle(25, 12);
					break;
				case 2:
					AddCube(50);
					break;
				case 3:
					AddCylinder(25, 50, 8);
					break;
				case 4:
					AddAxis(40);
					break;
			}
			break;
		case 1:
			switch(selectedSubMenuBtn){
				case 0:
					SaveObject(&active_object);
					break;

				case 1:
					LoadObject(&active_object);
					break;
			}
			break;
		case 2:
			switch(selectedSubMenuBtn){
				case 0:
					switchViewMode(PRESPECTIVE);
					break;
				case 1:
					switchViewMode(ORTHOGRAPHIC);
					break;
			}
			break;
		case 3:
			switch(selectedSubMenuBtn){
				case 0:
				resetLocation(&active_object);
				break;
				case 1:
				resetRotation(&active_object);
				break;
				case 2:
				resetScale(&active_object);
				break;
				case 3:
				resetObject(&active_object);
				break;
			}
			break;
	}
}

void redraw(){
	UG_FillScreen(BACKGROUND);
	UG_PutString(10, 220, tools[tool]);
	UG_PutString(160, 220, operationModes[operationMode]);
	UG_PutString(50, 10, modes[mode]);
	redrawObject(&active_object, 0);
}

uint8_t cycle(uint8_t index, uint8_t maxIndex, uint8_t direction){
	if(direction){
		if(index >= (maxIndex - 1)){
			return 0;
		}else{
			return index+1;
		}
	}else{
		if(index <= 0){
			return (maxIndex - 1);
		}else{
			return index-1;
		}
	}
}

void SaveObject(struct object *obj){
	uint16_t addr = OBJECT_EEPROM_ADDRESS;// Start address
	uint16_t i;
	eeprom_busy_wait();
	eeprom_write_block((void*)obj, (void*)addr, sizeof(struct object));
	eeprom_busy_wait();
	addr += sizeof(struct object);

	for(i = 0; i < obj->numOfVertecies; i++){
		eeprom_write_block((void*)&obj->verteciesLocal[i], (void*)addr, sizeof(struct vector3d));
		eeprom_busy_wait();
		addr += sizeof(struct vector3d);
	}

	for(i = 0; i < obj->numOfVertecies; i++){
		eeprom_write_block((void*)&obj->verteciesWorld[i], (void*)addr, sizeof(struct vector3d));
		eeprom_busy_wait();
		addr += sizeof(struct vector3d);
	}

	for(i = 0; i < obj->numOfVertecies; i++){
		eeprom_write_block((void*)&obj->selectedVertecies[i], (void*)addr, 1);
		eeprom_busy_wait();
		addr += 1;
	}

	for(i = 0; i < obj->numOfEdges; i++){
		eeprom_write_block((void*)&obj->edges[i], (void*)addr, sizeof(struct edge));
		eeprom_busy_wait();
		addr += sizeof(struct edge);
	}
}

void LoadObject(struct object *obj){
	removeObject(obj);
	uint16_t addr = OBJECT_EEPROM_ADDRESS;// Start address
	uint16_t i;
	eeprom_busy_wait();
	eeprom_read_block((void*)obj, (void*)addr, sizeof(struct object));
	eeprom_busy_wait();
	addr += sizeof(struct object);
	obj->verteciesLocal = calloc(obj->numOfVertecies, sizeof(struct vector3d));
	obj->verteciesWorld = calloc(obj->numOfVertecies, sizeof(struct vector3d));
	obj->selectedVertecies = calloc(obj->numOfVertecies, sizeof(char));
	obj->edges = calloc(obj->numOfEdges, sizeof(struct edge));

	for(i = 0; i < obj->numOfVertecies; i++){
		eeprom_read_block((void*)&obj->verteciesLocal[i], (void*)addr, sizeof(struct vector3d));
		eeprom_busy_wait();
		addr += sizeof(struct vector3d);
	}

	for(i = 0; i < obj->numOfVertecies; i++){
		eeprom_read_block((void*)&obj->verteciesWorld[i], (void*)addr, sizeof(struct vector3d));
		eeprom_busy_wait();
		addr += sizeof(struct vector3d);
	}

	for(i = 0; i < obj->numOfVertecies; i++){
		eeprom_read_block((void*)&obj->selectedVertecies[i], (void*)addr, 1);
		eeprom_busy_wait();
		addr += 1;
	}

	for(i = 0; i < obj->numOfEdges; i++){
		eeprom_read_block((void*)&obj->edges[i], (void*)addr, sizeof(struct edge));
		eeprom_busy_wait();
		addr += sizeof(struct edge);
	}
}