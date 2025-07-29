/*
 * DS1307.h
 * Kyle Isom <isomk@kyleisom.net>
 *
 * Arduino library to use DS1307
 * I2C address is hardcoded as 110100 - the data sheet implicitly states
 * that only one DS1307 can be on the bus.
 *
 * Should load the Wire library if it is not already loaded.
 */

#ifndef __DS1307_H
#define __DS1307_H

#include <WProgram.h> 


#define    DS1307_ADDR     0x68
#define    DS1307_CTRL_ID	0x68
#define    DS1307CH        0x07
#define    DS1307_SEC      0x7F
#define    DS1307_MODE     0x06
#define    DS1307_24HR     0x3F   
#define    DS1307_AMPM     0x20
#define    DS1307_12HR     0x1F
#define    DS1307_EPOCH    2000
#define    DS1307_BASE_YR	2000

#define	   DS1307_RAM_START	0x08
#define    DX1307_RAM_SIZE  56			// 56 bytes of RAM

typedef enum _SQWE_values {
  SQWE_1hz = 0x10,
  SQWE_4khz = 0x11,
  SQWE_8khz = 0x12,
  SQWE_32khz = 0x13
} SQWE_values;


class DS1307 {
	public:

        /* constructor */
        // Pass a true if the DS1307 is the only I2C device on the bus
        DS1307();
		void begin();
        
        /* read function */
		boolean isEnabled(void);
        boolean check24Hour(void);
        //void getTime(boolean *AMPM, byte *seconds, byte *minute, byte *hour);
        void getTime(byte *seconds, byte *minutes, byte *hours);
        void getDate(byte *day, byte *month, byte *year);
        void dayOfWeek(byte *weekday);
        //void getDTC(byte *AMPM, byte *seconds, byte *minute, byte *hour, byte *weekday, byte *day, byte *month, byte *year);
        void getDTC(byte *seconds, byte *minutes, byte *hours, byte *weekday, byte *day, byte *month, byte *year);
		
		/* DS1307 Memory Functions */
		byte readRAM(int);
		void writeRAM(byte, int);
		int readIntRAM( int );
		void writeIntRAM( int, int );
		long readLongRAM( int );
		void writeLongRAM( long, int );
	   
        /* write functions */
        void enableClock(void);
        void disableClock(void);
        void set24(void);
        void set12(void);
        void setTime(byte seconds, byte minute, byte hour);
        //void setTime(byte seconds, byte minute, byte hour) { setTime(seconds, minute, hour, NULL); }
        void setDate(byte day, byte month, byte year);
        void setDayOfWeek(byte weekday);
        void setDTC(byte seconds, byte minute, byte hour, byte weekday, byte day, byte month, byte year);
        void setSQWE(SQWE_values sqwe);
		
        /* conversion functions */
        byte bcd_to_dec(byte data);
        byte dec_to_bcd(byte data);

    private:
        boolean twentyfour;
        
        void getHour24(byte hour);
        void getHour12(boolean AMPM, byte hour);
};
 
#endif
