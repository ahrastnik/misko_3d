/*
 * misko_ad.c
 *
 * Created: 22. 05. 2017 13:14:02
 *  Author: Adam Hrastnik
 */ 

#include "misko_ad.h"

// Joystick Axis Channels
struct channelData x_axis;
struct channelData y_axis;
// If set to 1 calibrate, the joystick
uint8_t searchMinMax = 0;

void init_AD(){
	ADMUX = (1 << REFS0) | (1 << MUX1);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// Set Joystick Axis AD channels
	x_axis.channelNum = 3;
	y_axis.channelNum = 2;
	loadJoystickCalibration();
}

uint16_t ADConvert(uint8_t channel){
	ADMUX &= ~0x0F; // Reset the AD channel
	ADMUX |= (channel & 0x0F); // Set the new AD channel
	ADCSRA |= (1 << ADSC);
	while((ADCSRA >> ADSC) & 1);
	return ADC;
}

void loadJoystickCalibration(){
	eeprom_busy_wait();
	y_axis.maxValue = eeprom_read_word((uint16_t*)64);
	eeprom_busy_wait();
	y_axis.minValue = eeprom_read_word((uint16_t*)66);
	eeprom_busy_wait();
	x_axis.maxValue = eeprom_read_word((uint16_t*)68);
	eeprom_busy_wait();
	x_axis.minValue = eeprom_read_word((uint16_t*)70);
}

void readChannel(struct channelData *channel){
	channel->currentValue = ADConvert(channel->channelNum);
	if(searchMinMax){
		if(isNewMax(channel->currentValue, channel->maxValue))channel->maxValue = channel->currentValue;
		if(isNewMin(channel->currentValue, channel->minValue))channel->minValue = channel->currentValue;
	}
}

int getChannelPercentage(struct channelData *channel){
	return ((100*(float)(channel->currentValue - channel->minValue))/(channel->maxValue - channel->minValue));
}

uint8_t isNewMax(uint16_t value, uint16_t newMax){
	if(value > newMax)return 1;
	return 0;
}

uint8_t isNewMin(uint16_t value, uint16_t newMin){
	if(value < newMin)return 1;
	return 0;
}

void resetMinValue(struct channelData *channel){
	channel->minValue = 0;
	channel->minValue--;
}

void resetMaxValue(struct channelData *channel){
	channel->maxValue = 0;
}