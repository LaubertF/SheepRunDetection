/**

DS1337.h

Copyright by Christian Paul, 2014
modify by Ondrej Gallo, 2022

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

 */

// includes
#include <Arduino.h>

#ifndef DS1337_H
#define DS1337_H
#include "twiLight.h"

// constants
#define SECONDS_PER_MINUTE    UL60
#define SECONDS_PER_HOUR    UL3600
#define SECONDS_PER_DAY    UL86400

// DS1337 I2C BUS ID
#define DS1337_ID  0x68

// DS1337 registers
#define DS1337_REGISTERS   		16
#define DS1337_REGISTERS_DATE    7
#define DS1337_REGISTERS_A1      4
#define DS1337_REGISTERS_A2      3
#define DS1337_REGISTERS_STATUS  2
#define DS1337_SECONDS     0x00
#define DS1337_MINUTES     0x01
#define DS1337_HOUR        0x02
#define DS1337_DAY_OF_WEEK 0x03
#define DS1337_DAY         0x04
#define DS1337_MONTH       0x05
#define DS1337_YEAR        0x06
#define DS1337_A1_SECONDS  0x07
#define DS1337_A1_MINUTES  0x08
#define DS1337_A1_HOUR     0x09
#define DS1337_A1_DAY      0x0A
#define DS1337_A2_MINUTES  0x0B
#define DS1337_A2_HOUR     0x0C
#define DS1337_A2_DAY      0x0D
#define DS1337_CONTROL     0x0E
#define DS1337_STATUS      0x0F

// DS1337 control register flags
#define DS1337_A1IE 	0x00
#define DS1337_A2IE 	0x01
#define DS1337_INTCN	0x02
#define DS1337_RS1		0x03
#define DS1337_RS2		0x04
#define DS1337_EOSC		0x07

// DS1337 status register flags
#define DS1337_A1F 		0x00
#define DS1337_A2F 		0x01
#define DS1337_OSF		0x07

// DS1337 tick modes
#define DS1337_TICK_UNKNOWN			0xFF
#define DS1337_NO_TICKS				0x00
#define DS1337_TICK_EVERY_SECOND  	0x01
#define DS1337_TICK_EVERY_MINUTE  	0x02
#define DS1337_TICK_EVERY_HOUR  	0x03

// DS1337 alarm modes
#define DS1337_ALARM_EVERY_SECOND 				0x00
#define DS1337_ALARM_ON_SECOND 					0x01
#define DS1337_ALARM_ON_SECOND_MINUTE 			0x02
#define DS1337_ALARM_ON_SECOND_MINUTE_HOUR 		0x03
#define DS1337_ALARM_ON_SECOND_MINUTE_HOUR_DATE	0x04
#define DS1337_ALARM_ON_SECOND_MINUTE_HOUR_DAY  0x05
#define DS1337_ALARM_UNKNOWN					0xFF

// DS1337 alarm mode register
#define DS1337_A1M1		7
#define DS1337_A1M2		7
#define DS1337_A1M3		7
#define DS1337_A1M4		7
#define DS1337_A1DYDT	6
#define DS1337_A2M2		7
#define DS1337_A2M3		7
#define DS1337_A2M4		7
#define DS1337_A2DYDT	6

// Helpers - old
#define	T2000UTC 	946684800UL
// Helpers - new
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

/* Useful Constants */
#define SECS_PER_MIN  ((uint32_t )(60UL))
#define SECS_PER_HOUR ((uint32_t)(3600UL))
#define SECS_PER_DAY  ((uint32_t)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((uint32_t)(7UL))
#define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )

// nove DateTime struktury
typedef enum {
    dowInvalid, Nedela, Pondelok, Utorok, Streda, Stvrtok, Piatok, Sobota
} timeDayOfWeek_t;

// class definition of Date object
class Date {
	public:
		Date();
		Date(int hour, int minutes, int seconds);
		Date(int year, int month, int day, int hour, int minutes, int seconds);
		int getSeconds();
		int getMinutes();
		int getHour();
		int getDay();
		int getWday();
		int getMonth();
		int getYear();
		void setSeconds(int seconds);
		void setMinutes(int minutes);
		void setHour(int hour);
		void setDay(int day);
		void setWday(int wday);
		void setMonth(int month);
		void setYear(int year);
		void setTime(int hour, int minutes, int seconds);
		void setDate(int year, int month, int day);
		//uint32_t getUnixTime();
		//uint32_t getUnixTime(int year, int month, int day, int hour, int minute, int second);
	private:
		int _seconds;
		int _minutes;
		int _hour;
		int _wday;
		int _day;
		int _month;
		int _year;
};

// class definition of DS1337 RTC
class DS1337 {
	public:
		DS1337();
		TWILight ds1337;
		uint8_t dsreg[DS1337_REGISTERS];
		void clear_dev();
		void init();
		void customSettingsCNTRL();				//zapisuje priamo do RTC cipu
		void setTime(int hour, int minutes);	//zapisuje priamo do RTC cipu
		void setTime(int hour, int minutes, int seconds);	//zapisuje priamo do RTC cipu
		void setDate(int year, int month, int day);			//zapisuje priamo do RTC cipu
		void setDateTime(int year, int month, int day, int hour, int minutes, int seconds);	//zapisuje priamo do RTC cipu
		void setDateTime(unsigned long timestamp);			//zapisuje priamo do RTC cipu
		void setDateTimeFromTimestamp(uint32_t timestamp); //zapisuje priamo do RTC cipu
		void start();
		void stop();
		boolean isRunning();
		void setDate(Date date);	//zapisuje priamo do RTC cipu
		Date getDate();				//cita priamo z RTC cipu
		Date getDateObj();
		void getDate2();
		void setAlarm(int day, int hour, int minutes, int seconds);	//zapisuje priamo do RTC cipu
		void setAlarm(int hour, int minutes, int seconds);			//zapisuje priamo do RTC cipu
		void setAlarm(Date date);		//zapisuje priamo do RTC cipu
		void setAlarm(String date);		//zapisuje priamo do RTC cipu
		void snooze(int minutes);		//zapisuje priamo do RTC cipu
		void saveAlarm();		//cita priamo z RTC cipu
		void restoreAlarm();	//cita priamo z RTC cipu
		Date getAlarm();		//cita priamo z RTC cipu
		void enableAlarm();
		void disableAlarm();
		void clearAlarm();		
		void toggleAlarm();
		boolean isAlarmEnabled();
		boolean isAlarmActive();
		int getRegister(int i);
		boolean isTickActive();
		void setTickMode(int tickMode);
		int getTickMode();
		void resetTick();
		int getDayOfWeek();			//cita priamo z RTC cipu
		void setDayOfWeek(int day);	//zapisuje priamo do RTC cipu
		boolean hasStopped();
		void clearOSF();
		void clearFlags();
		void setAlarmMode(int alarmMode);	//zapisuje priamo do RTC cipu
		int getAlarmMode();					//cita priamo z RTC cipu
		void getTime(uint32_t timestamp, int *year, int *month, int *day, int *hour, int *minute, int *second);
		uint32_t getTimestamp();
		uint32_t getTimestamp(int year, int month, int day, int hour, int minute, int second);
		void printDateTime();
		uint8_t getRegister(uint8_t index);

	protected:
		uint8_t _register[DS1337_REGISTERS];
		int _tickMode;
		void readStatus();
		void writeStatus();
		void readAlarm2();
		void writeAlarm2();
		void clear();
		TWIState read(uint8_t startRegister, uint8_t countRegister);
		TWIState readDev(uint8_t startRegister, uint8_t countRegister);
	private:
		void readDate();
		void readAlarm1();
		TWIState write(uint8_t startRegister, uint8_t countRegister);
		void writeDate();
		void writeAlarm1();
		Date _date;
		Date _alarm;
		Date _savedAlarm;
		int _alarmMode;
};

#endif // DS1337_H