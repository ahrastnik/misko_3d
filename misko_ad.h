/*
 * misko_ad.h
 *
 * Created: 22. 05. 2017 13:13:00
 *  Author: Adam Hrastnik
 */ 


#ifndef MISKO_AD_H_
#define MISKO_AD_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#define AVCC 3.3

struct channelData{
	uint8_t channelNum;
	uint16_t currentValue;
	uint8_t currentPercentage;
	uint8_t prevPercentage;
	uint16_t minValue;
	uint16_t maxValue;
};

extern struct channelData x_axis;
extern struct channelData y_axis;

void init_AD();

uint16_t ADConvert(uint8_t channel);

// Joystick
#define JOYSTICK_SENSITIVITY 1
void loadJoystickCalibration();
void readChannel(struct channelData *channel);
int getChannelPercentage(struct channelData *channel);
uint8_t isNewMax(uint16_t value, uint16_t newMax);
uint8_t isNewMin(uint16_t value, uint16_t newMin);
void resetMinValue(struct channelData *channel);
void resetMaxValue(struct channelData *channel);
extern uint8_t searchMinMax;

#endif /* MISKO_AD_H_ */