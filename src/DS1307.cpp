/*
 * DS1307.cpp
 * Kyle Isom <isomk@kyleisom.net>
 *
 * Arduino library to use DS1307
 * I2C address is hardcoded as 110100 - the data sheet implicitly states
 * that only one DS1307 can be on the bus. I was inspired by some of the
 * code at http://www.glacialwanderer.com/hobbyrobotics/?p=12
 */
 
#include <Wprogram.h>
#include <Wire.h>

#include "DS1307.h"

DS1307::DS1307() {
}

void DS1307::begin() {
    Wire.begin();
	if (!isEnabled()) { 
		enableClock(); 
	}
	// Can't set 24 w/o also setting the hour, per datasheet...
	//set24();
}
void DS1307::enableClock() {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x00);
    Wire.send( 0 << DS1307CH );
    Wire.endTransmission();
}

void DS1307::disableClock() {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x00);
    Wire.send( 1 << DS1307CH );
    Wire.endTransmission();
}

boolean DS1307::isEnabled() {
    byte sec;
    
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x00);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDR, 1);
    
	//Fetch the CH (Clock Halted) bit - left most bit in first clock byte.
    sec = Wire.receive();
    return sec & (1 << DS1307CH) ? false : true;
}

void DS1307::getTime(byte *seconds, byte *minutes, byte *hours) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x00);
    Wire.endTransmission();
    
    Wire.requestFrom(DS1307_ADDR, 3);
    *seconds = bcd_to_dec(Wire.receive() & DS1307_SEC);
    *minutes = bcd_to_dec(Wire.receive() );
    *hours = bcd_to_dec(Wire.receive() & DS1307_24HR);
    
    //check24Hour() ? getHour24(*hour) : getHour12(*AMPM, *hour);
    //*hour = bcd_to_dec(*hour);
}

void DS1307::getHour12(boolean AMPM, byte hour) {
    AMPM = (hour | ( 1 << DS1307_AMPM )) ? true : false;  
    hour &= DS1307_12HR;
}

void DS1307::getHour24(byte hour) { hour = bcd_to_dec(hour & DS1307_24HR); }

byte DS1307::bcd_to_dec(byte data) { return (((data / 16) * 10) + (data % 16)); }

byte DS1307::dec_to_bcd(byte data) { return (((data / 10) * 16) + (data % 10)); }

 
boolean DS1307::check24Hour() {
    byte mode24_12;
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x02);
    Wire.endTransmission();
    
    Wire.requestFrom(DS1307_ADDR, 1);
    mode24_12= Wire.receive();
	
    return ( (mode24_12 >> 6) == 1 ? false : true ) ; 
}

void DS1307::set24(void) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x02);
    Wire.send(0x00);
    Wire.endTransmission();
}

void DS1307::set12(void) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x02);
    Wire.send(1 << DS1307_MODE);
    Wire.endTransmission();
}

void DS1307::getDate(byte *day, byte *month, byte *year) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x04);
    Wire.endTransmission();
    
    Wire.requestFrom(DS1307_ADDR, 3);
    *day = bcd_to_dec(Wire.receive());
    *month = bcd_to_dec(Wire.receive());
    *year = bcd_to_dec(Wire.receive());
}

void DS1307::dayOfWeek(byte *weekday) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x03);
    Wire.endTransmission();
    
    Wire.requestFrom(DS1307_ADDR, 1);
    *weekday = bcd_to_dec(Wire.receive());
}

void DS1307::setDTC(byte seconds, byte minutes, byte hours, byte weekday, byte day, byte month, byte year) {
    setTime(seconds, minutes, hours);
    setDayOfWeek(weekday);
    setDate(day, month, year);
}


//void DS1307::getDTC(byte *AMPM, byte *seconds, byte *minute, byte *hour, byte *weekday, byte *day, byte *month, byte *year) {

//}


void DS1307::getDTC(byte *seconds, byte *minutes, byte *hours, byte *weekday, byte *day, byte *month, byte *year) { 
    getTime(seconds, minutes, hours);
    dayOfWeek(weekday);
    getDate(day, month, year);
}

void DS1307::setTime(byte seconds, byte minutes, byte hours) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x00);
    Wire.send(dec_to_bcd(seconds));
    Wire.send(dec_to_bcd(minutes));
    Wire.send(dec_to_bcd(hours));
    Wire.endTransmission();
}

void DS1307::setDate(byte day, byte month, byte year) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x04);
    Wire.send(dec_to_bcd(day));
    Wire.send(dec_to_bcd(month));
    Wire.send(dec_to_bcd(year));
    Wire.endTransmission();
}

void DS1307::setDayOfWeek(byte weekday) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x03);
    Wire.send(dec_to_bcd(weekday));
    Wire.endTransmission();
}

void DS1307::setSQWE(const SQWE_values sqwe) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.send(0x07);		// Control Register: Address 0x07
    Wire.send(sqwe);		// Ex: 0x10 
    Wire.endTransmission();
}

byte DS1307::readRAM(int p) {
	if ( p < DX1307_RAM_SIZE ) {
		// set the register to a specific the sram location and read a single byte
		Wire.beginTransmission(DS1307_CTRL_ID);
		Wire.send(DS1307_RAM_START+p);
		Wire.endTransmission();  
		Wire.requestFrom(DS1307_CTRL_ID, 1);
		return Wire.receive();
	} else {
		return 0;
	}
}

void DS1307::writeRAM(byte b, int p) {
	if ( p < DX1307_RAM_SIZE ) {
		// set the register to a specific the sram location and save a single byte
		Wire.beginTransmission(DS1307_CTRL_ID);
		Wire.send(DS1307_RAM_START+p);
		Wire.send(b);
		Wire.endTransmission();  
	}
}

void DS1307::writeIntRAM(int b, int p) {
	writeRAM( lowByte(b), p );
	writeRAM( highByte(b), p+1);
}

int DS1307::readIntRAM(int p) {
	byte msb, lsb;
	msb = readRAM( p+1 );
	lsb = readRAM( p );
	int i =  (msb << 8) + lsb;
	return i;
}


void DS1307::writeLongRAM(long b, int p) {
	byte aByte;
	for (int i=0; i < 4; i++) {
		aByte = (b >> 8 * 4) & 0xff;
		writeRAM( aByte, p+i );
	} 
}

long DS1307::readLongRAM(int p) {
	long longVal = 0;
	
	for (int i=0; i < 4; i++) {
		longVal = (longVal << 4) + readRAM( p+i );
	} 	
	return longVal;
}

