/**

DS1337.cpp

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
#include "DS1337.h"

// ********** Constructor of class Date *********
Date::Date() {
	setDate(0, 1, 1);
	setTime(0, 0, 0);
}

// Constructor of class Date with given time
Date::Date(int hour, int minutes, int seconds) {
	setDate(1, 1, 0);
	setTime(hour,  minutes, seconds);
}

// Constructor of class Date with given date and time
Date::Date(int year, int month, int day, int hour, int minutes, int seconds) {
	setDate(year, month, day);
	setTime(hour,  minutes, seconds);
}

//Set time
void Date::setTime(int hour, int minutes, int seconds) {
	setHour(hour);
	setMinutes(minutes);
	setSeconds(seconds);
}

// Set date
void Date::setDate(int year, int month, int day) {
	setDay(day);
	setMonth(month);
	setYear(year);
}

// Get/Set parts of date and time
int Date::getSeconds() { return _seconds; }
int Date::getMinutes() { return _minutes; }
int Date::getHour() { return _hour; }
int Date::getDay() { return _day; }
int Date::getWday() { return _wday; }
int Date::getMonth() { return _month; }
int Date::getYear() { return _year; }
void Date::setSeconds(int seconds) { _seconds = seconds; }
void Date::setMinutes(int minutes) { _minutes = minutes; }
void Date::setHour(int hour) { _hour = hour; }
void Date::setDay(int day) { _day = day; }
void Date::setWday(int wday) { _wday = wday; }
void Date::setMonth(int month) { _month = month; }
void Date::setYear(int year) { _year = year; }

// **************** Constructor of class DS1337 ***************
DS1337::DS1337() {
	init();
}

// Init
void DS1337::init() {
	ds1337.TWIInit();
	clear();
	_date = Date();
	_alarm = Date();
	_tickMode = DS1337_TICK_UNKNOWN;
	_alarmMode = DS1337_ALARM_UNKNOWN;
}

// Clear registers
void DS1337::clear() {
	for (int i=0; i<DS1337_REGISTERS; i++) {
		_register[i] = 0;
	}
}

// Clear registers
void DS1337::clear_dev() {
	for (int i=0; i<DS1337_REGISTERS; i++) {
		_register[i] = 0;
		dsreg[i] = 0;
	}
}

// Set the alarm mode
void DS1337::setAlarmMode(int alarmMode) {
	//readAlarm1();
	switch(alarmMode) {
		case DS1337_ALARM_EVERY_SECOND:
			bitSet(_register[DS1337_A1_SECONDS], DS1337_A1M1);
			bitSet(_register[DS1337_A1_MINUTES], DS1337_A1M2);
			bitSet(_register[DS1337_A1_HOUR], DS1337_A1M3);
			bitSet(_register[DS1337_A1_DAY], DS1337_A1M4);
			bitClear(_register[DS1337_A1_DAY], DS1337_A1DYDT);
			break;
		case DS1337_ALARM_ON_SECOND:
			bitClear(_register[DS1337_A1_SECONDS], DS1337_A1M1);
			bitSet(_register[DS1337_A1_MINUTES], DS1337_A1M2);
			bitSet(_register[DS1337_A1_HOUR], DS1337_A1M3);
			bitSet(_register[DS1337_A1_DAY], DS1337_A1M4);
			bitClear(_register[DS1337_A1_DAY], DS1337_A1DYDT);
			break;
		case DS1337_ALARM_ON_SECOND_MINUTE:
			bitClear(_register[DS1337_A1_SECONDS], DS1337_A1M1);
			bitClear(_register[DS1337_A1_MINUTES], DS1337_A1M2);
			bitSet(_register[DS1337_A1_HOUR], DS1337_A1M3);
			bitSet(_register[DS1337_A1_DAY], DS1337_A1M4);
			bitClear(_register[DS1337_A1_DAY], DS1337_A1DYDT);
			break;
		case DS1337_ALARM_ON_SECOND_MINUTE_HOUR:
			bitClear(_register[DS1337_A1_SECONDS], DS1337_A1M1);
			bitClear(_register[DS1337_A1_MINUTES], DS1337_A1M2);
			bitClear(_register[DS1337_A1_HOUR], DS1337_A1M3);
			bitSet(_register[DS1337_A1_DAY], DS1337_A1M4);
			bitClear(_register[DS1337_A1_DAY], DS1337_A1DYDT);
			break;
		case DS1337_ALARM_ON_SECOND_MINUTE_HOUR_DATE:
			bitClear(_register[DS1337_A1_SECONDS], DS1337_A1M1);
			bitClear(_register[DS1337_A1_MINUTES], DS1337_A1M2);
			bitClear(_register[DS1337_A1_HOUR], DS1337_A1M3);
			bitClear(_register[DS1337_A1_DAY], DS1337_A1M4);
			bitClear(_register[DS1337_A1_DAY], DS1337_A1DYDT);
			break;
		case DS1337_ALARM_ON_SECOND_MINUTE_HOUR_DAY:
			bitClear(_register[DS1337_A1_SECONDS], DS1337_A1M1);
			bitClear(_register[DS1337_A1_MINUTES], DS1337_A1M2);
			bitClear(_register[DS1337_A1_HOUR], DS1337_A1M3);
			bitClear(_register[DS1337_A1_DAY], DS1337_A1M4);
			bitSet(_register[DS1337_A1_DAY], DS1337_A1DYDT);
			break;
	}
	_alarmMode = alarmMode;
	writeAlarm1();
}

// Get unix timestamp
uint32_t DS1337::getTimestamp() {
	return getTimestamp(_date.getYear(), _date.getMonth(), _date.getDay(), _date.getHour(), _date.getMinutes(), _date.getSeconds());
}

// Get unix timestamp
uint32_t DS1337::getTimestamp(int year, int month, int day, int hour, int minute, int second) {
	// assemble Date elements into time_te
    // note year argument is offset from 1970
	int i;
    uint32_t seconds;
	year = year + 30;	//korekcia, preotoze v RTC registry musi byt absolutny datum ale pre tento algo treba pocet rokov od r. 1970 
    // seconds from 1970 till 1 jan 00:00:00 of the given year
    seconds = year * (SECS_PER_DAY * 365);
    for (i = 0; i < year; i++) {
        if (LEAP_YEAR(i)) {
            seconds += SECS_PER_DAY;   // add extra days for leap years
        }
    }
    // add days for this year, months start from 1
    for (i = 1; i < month;    i++) {
        if ((i == 2) && LEAP_YEAR(year)) {
            seconds += SECS_PER_DAY * 29;
        }
        else {
            seconds +=
            SECS_PER_DAY *monthDays[i - 1];  //monthDay array starts from 0
        }
    }  
    seconds+= (day-1) *  SECS_PER_DAY;
    seconds+= hour *SECS_PER_HOUR;
    seconds+= minute *SECS_PER_MIN;
    seconds+= second;
    return (uint32_t) seconds;
}

void DS1337::setDateTimeFromTimestamp(uint32_t timestamp){
    // break the given timestamp into Date ant Time components y, m, d, wday,hour, min, sec
    // note that year is offset from 1970 !!!

    uint8_t year;
    uint8_t month, monthLength;
    uint32_t time;
    unsigned long days;

    time = (uint32_t)timestamp;
    _date.setSeconds(time % 60);
    time /= 60; // now it is minutes
    _date.setMinutes(time % 60);
    time /= 60; // now it is hours
    _date.setHour(time % 24);
    time /= 24; // now it is days
    _date.setWday( ((time + 3) % 7) + 1 ); // Pondelok je day 1

    year = 0;
    days = 0;
    while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
        year++;
    }
    _date.setYear(year - 30); // year is offset from 1970, ale do RTC registra musi ist absolutny cas, t.j. robime korekciu -30 rokov

    days -= LEAP_YEAR(year) ? 366 : 365;
    time  -= days; // now it is days in this year, starting at 0

    days=0;
    month=0;
    monthLength=0;
    for (month=0; month<12; month++) {
        if (month==1) { // february
            if (LEAP_YEAR(year)) {
                monthLength=29;
            }
            else {
                monthLength=28;
            }
        }
        else {
            monthLength = monthDays[month];
        }

        if (time >= monthLength) {
            time -= monthLength;
        }
        else {
            break;
        }
    }
    _date.setMonth(month + 1);	// jan is month 1
    _date.setDay(time + 1);	// day of month	
	setDate(_date);	//zapise datum do RTC cipu
}

// Set the current date
void DS1337::setDate(Date date) {
	_date = date;
	_register[DS1337_SECONDS] = (date.getSeconds() % 10) + ((date.getSeconds() / 10) << 4);
	_register[DS1337_MINUTES] = (date.getMinutes() % 10) + ((date.getMinutes() / 10) << 4);
	_register[DS1337_HOUR] = (date.getHour() % 10) + ((date.getHour() / 10) << 4);
	_register[DS1337_DAY_OF_WEEK] = date.getWday();
	_register[DS1337_DAY] = (date.getDay() % 10) + ((date.getDay() / 10) << 4);
	_register[DS1337_MONTH] = (date.getMonth() % 10) + ((date.getMonth() / 10) << 4);
	_register[DS1337_YEAR] = (date.getYear() % 10) + ((date.getYear() / 10) << 4);
	writeDate();
}

// Read alarm 1 registers from DS1337
void DS1337::readAlarm1() {
	read(DS1337_A1_SECONDS, DS1337_REGISTERS_A1);
}

// Write alarm 1 registers from DS1337
void DS1337::writeAlarm1() {
	write(DS1337_A1_SECONDS, DS1337_REGISTERS_A1);
}

// Clear alarm
void DS1337::clearAlarm() {
	readStatus();
	bitClear(_register[DS1337_STATUS], DS1337_A1F);
	writeStatus();
}

// Read status and control registers from DS1337
void DS1337::readStatus() {
	read(DS1337_CONTROL, DS1337_REGISTERS_STATUS);
}

// Write status and control registers from DS1337
void DS1337::writeStatus() {
	write(DS1337_CONTROL, DS1337_REGISTERS_STATUS);
}

// Read registers from DS1337
TWIState DS1337::read(uint8_t startRegister, uint8_t countRegister) {
	//TWIReadMultipleBytesFrom ma bug - nepouzivat!!!
	//return ds1337.TWIReadMultipleBytesFrom(DS1337_ID, startRegister, _register, startRegister, countRegister, DS1337_REGISTERS);
	return ds1337.TWIReadMultipleBytes(DS1337_ID, startRegister, _register, countRegister);
}

// Read registers from DS1337 - len testujem preco nefunguje ukladanie do _register
TWIState DS1337::readDev(uint8_t startRegister, uint8_t countRegister) {
	return ds1337.TWIReadMultipleBytesFrom(DS1337_ID, startRegister, dsreg, startRegister, countRegister, DS1337_REGISTERS);
}

// Write registers to DS1337
TWIState DS1337::write(uint8_t startRegister, uint8_t countRegister) {
	return ds1337.TWIWriteMultipleBytesFrom(DS1337_ID, startRegister, _register, startRegister, countRegister, DS1337_REGISTERS); 
}

// Write date/time registers from DS1337
void DS1337::writeDate() {
	write(DS1337_SECONDS, DS1337_REGISTERS_DATE);
}

void DS1337::customSettingsCNTRL(){
	// DS1337_CONTROL
	// EOSC	0	0	RS2	RS1	INTCN	A2IE	A1IE
	// 0	0	0	0	0	1		0		1

	// DS1337_STATUS
	// OSF	0	0	0	0	0		A2F		A1F
	// 0	0	0	0	0	0		0		0

	//start oscilatora
	bitClear(_register[DS1337_CONTROL], DS1337_EOSC);
	//INTB output 1Hz
	bitClear(_register[DS1337_CONTROL], DS1337_RS1);
	bitClear(_register[DS1337_CONTROL], DS1337_RS2);
	//alarm1 aj alarm2 mozu ovladat INTA a INTB piny
	bitSet(_register[DS1337_CONTROL], DS1337_INTCN);
	//zakazanie alarmu2 + vymazanie flagu prerusenia
	bitClear(_register[DS1337_CONTROL], DS1337_A2IE);
	bitClear(_register[DS1337_STATUS], DS1337_A2F);
	//povolenie alarmu1 + vymazanie flagu prerusenia
	bitSet(_register[DS1337_CONTROL], DS1337_A1IE);
	bitClear(_register[DS1337_STATUS], DS1337_A1F);
	//vymazanie flagu zastavenia oscilatora - pre istotu
	bitClear(_register[DS1337_STATUS], DS1337_OSF);
	writeStatus();
}

 // Read date/time registers from DS1337
void DS1337::readDate() {
	read(DS1337_SECONDS, DS1337_REGISTERS_DATE);
	//readDev(DS1337_SECONDS, DS1337_REGISTERS_DATE);
}


 // Read alarm 2 registers from DS1337

void DS1337::readAlarm2() {
	read(DS1337_A2_MINUTES, DS1337_REGISTERS_A2);
}

 // Write alarm 2 registers from DS1337

void DS1337::writeAlarm2() {
	write(DS1337_A2_MINUTES, DS1337_REGISTERS_A2);
}


 // Get day of week (1..7)

int DS1337::getDayOfWeek() {
	readDate();
    return _register[DS1337_DAY_OF_WEEK];
}


 // Set day of week (1..7)

void DS1337::setDayOfWeek(int day) {
	readDate();
    _register[DS1337_DAY_OF_WEEK] = day;
    writeDate();
}


 // Set time (hh:mm)
void DS1337::setTime(int hour, int minutes) {
	setTime(hour, minutes, 0);
}


 // Set time (hh:mm:ss)
void DS1337::setTime(int hour, int minutes, int seconds) {
	Date d = getDate();
	d.setHour(hour);
	d.setMinutes(minutes);
	d.setSeconds(seconds);
	setDate(d);
}



 // Set date (yy,mm,dd)

void DS1337::setDate(int year, int month, int day) {
	Date d = getDate();
	d.setDay(day);
	d.setMonth(month);
	d.setYear(year);
	setDate(d);
}


 // Start clock

void DS1337::start() {
	readStatus();
	bitClear(_register[DS1337_CONTROL], DS1337_EOSC);
	writeStatus();
}


 // Stop clock

void DS1337::stop() {
	readStatus();
	bitSet(_register[DS1337_CONTROL], DS1337_EOSC);
	writeStatus();
}


 // Check if clock is running

boolean DS1337::isRunning() {
	readStatus();
	return !bitRead(_register[DS1337_CONTROL], DS1337_EOSC);
}


 // Get the current date
Date DS1337::getDate() {
	readDate();	//precita datum z RTC do lokalnej premennej _register
	//vyskladanie objektu Datum (celeho datumu) z registrov
	_date.setSeconds((_register[DS1337_SECONDS] & 0x0F) + 10 * (_register[DS1337_SECONDS] >> 4));
	_date.setMinutes((_register[DS1337_MINUTES] & 0x0F) + 10 * (_register[DS1337_MINUTES] >> 4));
	_date.setHour((_register[DS1337_HOUR] & 0x0F) + 10 * (_register[DS1337_HOUR] >> 4));
	_date.setDay((_register[DS1337_DAY] & 0x0F) + 10 * (_register[DS1337_DAY] >> 4));
	_date.setMonth((_register[DS1337_MONTH] & 0x0F) + 10 * ((_register[DS1337_MONTH] >> 4) & 0x01));
	_date.setYear((_register[DS1337_YEAR] & 0x0F) + 10 * (_register[DS1337_YEAR] >> 4));
	return _date;
}

 // Get the current date
void DS1337::getDate2() {
	readDate();	//precita datum z RTC do lokalnej premennej _register
/*	
	Serial.print(_register[4], HEX);
    Serial.print(".");
    Serial.print(_register[5], HEX);
    Serial.print(".");
    Serial.print(_register[6], HEX);
    Serial.print(" ");
    Serial.print(_register[2], HEX);
    Serial.print(":");
    Serial.print(_register[1], HEX);
    Serial.print(":");
    Serial.println(_register[0], HEX);
*/	
	//vyskladanie objektu Datum (celeho datumu) z registrov
	_date.setSeconds((_register[DS1337_SECONDS] & 0x0F) + 10 * (_register[DS1337_SECONDS] >> 4));
	_date.setMinutes((_register[DS1337_MINUTES] & 0x0F) + 10 * (_register[DS1337_MINUTES] >> 4));
	_date.setHour((_register[DS1337_HOUR] & 0x0F) + 10 * (_register[DS1337_HOUR] >> 4));
	_date.setDay((_register[DS1337_DAY] & 0x0F) + 10 * (_register[DS1337_DAY] >> 4));
	_date.setMonth((_register[DS1337_MONTH] & 0x0F) + 10 * ((_register[DS1337_MONTH] >> 4) & 0x01));
	_date.setYear((_register[DS1337_YEAR] & 0x0F) + 10 * (_register[DS1337_YEAR] >> 4));
}
uint8_t DS1337::getRegister(uint8_t index){
	return _register[index];
}

 // Set the current date and time

void DS1337::setDateTime(int year, int month, int day, int hour, int minutes, int seconds) {
	readDate();
	_register[DS1337_SECONDS] = (seconds % 10) + ((seconds / 10) << 4);;
	_register[DS1337_MINUTES] = (minutes % 10) + ((minutes / 10) << 4);
	_register[DS1337_HOUR] = (hour % 10) + ((hour / 10) << 4);
	_register[DS1337_DAY] = (day % 10) + ((day / 10) << 4);
	_register[DS1337_MONTH] = (month % 10) + ((month / 10) << 4);
	_register[DS1337_YEAR] = (year % 10) + ((year / 10) << 4);
	writeDate();
}


// Set the current date and time with a timestamp
void DS1337::setDateTime(unsigned long timestamp) {
    int y,m,d,h,mm,s;
	DS1337::getTime(timestamp, &y, &m, &d, &h, &mm, &s);
    setDateTime(y, m, d, h, mm, s);
}

 // Get the current alarm
Date DS1337::getAlarm() {
	readAlarm1();
	_alarm.setSeconds((_register[DS1337_A1_SECONDS] & 0x0F) + 10 * ((_register[DS1337_A1_SECONDS] >> 4) & 0x07));
	_alarm.setMinutes((_register[DS1337_A1_MINUTES] & 0x0F) + 10 * ((_register[DS1337_A1_MINUTES] >> 4) & 0x07));
	_alarm.setHour((_register[DS1337_A1_HOUR] & 0x0F) + 10 * ((_register[DS1337_A1_HOUR] >> 4) & 0x03));
	_alarm.setDay((_register[DS1337_A1_DAY] & 0x0F) + 10 * ((_register[DS1337_A1_DAY] >> 4) & 0x03));
	return _alarm;
}


// Set the current alarm

void DS1337::setAlarm(Date date) {
	readAlarm1();
	_alarm = date;
	_register[DS1337_A1_SECONDS] = (_register[DS1337_A1_SECONDS] & 0x80) + (date.getSeconds() % 10) + ((date.getSeconds() / 10) << 4);
	_register[DS1337_A1_MINUTES] = (_register[DS1337_A1_MINUTES] & 0x80) + (date.getMinutes() % 10) + ((date.getMinutes() / 10) << 4);
	_register[DS1337_A1_HOUR] = (_register[DS1337_A1_HOUR] & 0x80) + (date.getHour() % 10) + ((date.getHour() / 10) << 4);
	_register[DS1337_A1_DAY] = (_register[DS1337_A1_DAY] & 0xC0) + (date.getDay() % 10) + ((date.getDay() / 10) << 4);
	writeAlarm1();
}


// Set the alarm time with string in form of hh:mm or dd.hh:mm

void DS1337::setAlarm(String time) {
  time.trim();
  if (time.length()==5)
	  setAlarm(time.substring(0, 2).toInt(), time.substring(3, 5).toInt(), 0);
  else if (time.length()==8)
	  setAlarm(time.substring(0, 2).toInt(), time.substring(3, 5).toInt(), time.substring(6, 8).toInt(), 0);
}


// Saves the current alarm

void DS1337::saveAlarm() {
	_savedAlarm = Date();
	Date _currentAlarm = getAlarm();
	_savedAlarm.setDay(_currentAlarm.getDay());
	_savedAlarm.setHour(_currentAlarm.getHour());
	_savedAlarm.setMinutes(_currentAlarm.getMinutes());
	_savedAlarm.setSeconds(_currentAlarm.getSeconds());
}


 // Restore the current to the saved alarm

void DS1337::restoreAlarm() {
	setAlarm(_savedAlarm);
}


 // Set the current alarm

void DS1337::setAlarm(int hour, int minutes, int seconds) {
	Date d = getAlarm();
	d.setHour(hour);
	d.setMinutes(minutes);
	d.setSeconds(seconds);
	setAlarm(d);
}


 // Set the current alarm

void DS1337::setAlarm(int day, int hour, int minutes, int seconds) {
	Date d = getAlarm();
	d.setDay(day);
	d.setHour(hour);
	d.setMinutes(minutes);
	d.setSeconds(seconds);
	setAlarm(d);
}


 // Snooze some minutes (between 1 .. 60)
 // Alarm is enabled at current alarm + minutes
void DS1337::snooze(int minutes) {
	clearAlarm();
	if (minutes>0 && minutes<=60) {
		Date _currentAlarm = getAlarm();
		_currentAlarm.setMinutes(_currentAlarm.getMinutes()+minutes);
		if (_currentAlarm.getMinutes()>60) {
			_currentAlarm.setMinutes(_currentAlarm.getMinutes()-60);
			_currentAlarm.setHour(_currentAlarm.getHour()+1);
			if (_currentAlarm.getHour()>23) {
				_currentAlarm.setHour(0);
				_currentAlarm.setDay(_currentAlarm.getDay()+1);
			}
		}
		setAlarm(_currentAlarm);
	}
}


 // Enable alarm

void DS1337::enableAlarm() {
	readStatus();
	bitSet(_register[DS1337_CONTROL], DS1337_A1IE);
	writeStatus();
}


 // Disable alarm

void DS1337::disableAlarm() {
	readStatus();
	bitClear(_register[DS1337_CONTROL], DS1337_A1IE);
	writeStatus();
}

 // Toggle alarm

 void DS1337::toggleAlarm() {
	 if (isAlarmEnabled())
		 disableAlarm();
	 else
		 enableAlarm();
 }


 // Check, if alarm is enabled

boolean DS1337::isAlarmEnabled() {
	readStatus();
	return bitRead(_register[DS1337_CONTROL], DS1337_A1IE);
}


 // Check, if alarm is active

boolean DS1337::isAlarmActive() {
	readStatus();
	return bitRead(_register[DS1337_STATUS], DS1337_A1F);
}


 // Check, if tick is active

boolean DS1337::isTickActive() {
	readStatus();
	return bitRead(_register[DS1337_STATUS], DS1337_A2F);
}



 // Set the tick mode (every second or minuzte)

void DS1337::setTickMode(int tickMode) {
	if (tickMode==DS1337_NO_TICKS) {
		readStatus();
		bitSet(_register[DS1337_CONTROL], DS1337_INTCN);
		bitClear(_register[DS1337_CONTROL], DS1337_A2IE);
		bitClear(_register[DS1337_STATUS], DS1337_A2F);
		writeStatus();
		_tickMode = tickMode;
	}
	else if (tickMode==DS1337_TICK_EVERY_SECOND) {
		readStatus();
		bitClear(_register[DS1337_CONTROL], DS1337_INTCN);
		bitClear(_register[DS1337_CONTROL], DS1337_A2IE);
		bitClear(_register[DS1337_CONTROL], DS1337_RS1);
		bitClear(_register[DS1337_CONTROL], DS1337_RS2);
		bitClear(_register[DS1337_STATUS], DS1337_A2F);
		writeStatus();
		_tickMode = tickMode;
	}
	else if (tickMode==DS1337_TICK_EVERY_MINUTE) {
		readStatus();
		readAlarm2();
		bitSet(_register[DS1337_CONTROL], DS1337_INTCN);
		bitSet(_register[DS1337_CONTROL], DS1337_A2IE);
		bitClear(_register[DS1337_STATUS], DS1337_A2F);
		bitSet(_register[DS1337_A2_MINUTES], DS1337_A2M2);
		bitSet(_register[DS1337_A2_HOUR], DS1337_A2M3);
		bitSet(_register[DS1337_A2_DAY], DS1337_A2M4);
		writeAlarm2();
		writeStatus();
		_tickMode = tickMode;
	}
	else if (tickMode==DS1337_TICK_EVERY_HOUR) {
		readStatus();
		readAlarm2();
		bitSet(_register[DS1337_CONTROL], DS1337_INTCN);
		bitSet(_register[DS1337_CONTROL], DS1337_A2IE);
		bitClear(_register[DS1337_STATUS], DS1337_A2F);
		//bitClear(_register[DS1337_A2_MINUTES], DS1337_A2M2);
		_register[DS1337_A2_MINUTES] = 0;
		bitSet(_register[DS1337_A2_HOUR], DS1337_A2M3);
		bitSet(_register[DS1337_A2_DAY], DS1337_A2M4);
		writeAlarm2();
		writeStatus();
		_tickMode = tickMode;
	}
}


 // Get the tick mode (second | minute)

int DS1337::getTickMode() {
	readStatus();
	readAlarm2();
	bool intcn = bitRead(_register[DS1337_CONTROL], DS1337_INTCN);
	bool a1ie = bitRead(_register[DS1337_CONTROL], DS1337_A1IE);
	bool a2ie = bitRead(_register[DS1337_CONTROL], DS1337_A2IE);
	bool rs1 = bitRead(_register[DS1337_CONTROL], DS1337_RS1);
	bool rs2 = bitRead(_register[DS1337_CONTROL], DS1337_RS2);
	bool a2m2 = bitRead(_register[DS1337_A2_MINUTES], DS1337_A2M2);
	bool a2m3 = bitRead(_register[DS1337_A2_HOUR], DS1337_A2M3);
	bool a2m4 = bitRead(_register[DS1337_A2_DAY], DS1337_A2M4);
	bool dydt = bitRead(_register[DS1337_A2_DAY], DS1337_A2DYDT);
	if (!intcn && !rs1 && !rs2 && !a2ie)
		_tickMode = DS1337_TICK_EVERY_SECOND;
	else if (intcn && !a2ie)
		_tickMode = DS1337_NO_TICKS;
	else if (intcn && a2ie && a2m4 && a2m3 && a2m2)
		_tickMode = DS1337_TICK_EVERY_MINUTE;
	else if (intcn && a2ie && a2m4 && a2m3 && _register[DS1337_A2_MINUTES]==0)
		_tickMode = DS1337_TICK_EVERY_HOUR;
	else
		_tickMode = DS1337_TICK_UNKNOWN;
	return _tickMode;
}


 // Reset the tick flag (must be done in hour and minite tick mode)

void DS1337::resetTick() {
	readStatus();
	bitClear(_register[DS1337_STATUS], DS1337_A2F);
	writeStatus();
}


 // Get the value of one of the sixteen DS1337 register
int DS1337::getRegister(int i) {
	read(i, 1);
	return _register[i];
}


 // Check, if clock has stopped (Oscillator Stop Flag is set)
boolean DS1337::hasStopped() {
	readStatus();
	return bitRead(_register[DS1337_STATUS], DS1337_OSF);
}


 // Clear the OSF (Oscillator Stop Flag)
void DS1337::clearOSF() {
	readStatus();
	bitClear(_register[DS1337_STATUS], DS1337_OSF);
	writeStatus();
}


 // Clear all flags
void DS1337::clearFlags() {
	readStatus();
	bitClear(_register[DS1337_STATUS], DS1337_OSF);
	bitClear(_register[DS1337_STATUS], DS1337_A2F);
	bitClear(_register[DS1337_STATUS], DS1337_A1F);
	writeStatus();
}

 // Get the alarm mode

int DS1337::getAlarmMode() {
	readAlarm1();
	bool a1m1 = bitRead(_register[DS1337_A1_SECONDS], DS1337_A1M1);
	bool a1m2 = bitRead(_register[DS1337_A1_MINUTES], DS1337_A1M2);
	bool a1m3 = bitRead(_register[DS1337_A1_HOUR], DS1337_A1M3);
	bool a1m4 = bitRead(_register[DS1337_A1_DAY], DS1337_A1M4);
	bool dydt = bitRead(_register[DS1337_A1_DAY], DS1337_A1DYDT);
	if (a1m1 && a1m2 && a1m3 && a1m4)
		_alarmMode = DS1337_ALARM_EVERY_SECOND;
	else if (!a1m1 && a1m2 && a1m3 && a1m4)
		_alarmMode = DS1337_ALARM_ON_SECOND;
	else if (!a1m1 && !a1m2 && a1m3 && a1m4)
		_alarmMode = DS1337_ALARM_ON_SECOND_MINUTE;
	else if (!a1m1 && !a1m2 && !a1m3 && a1m4)
		_alarmMode = DS1337_ALARM_ON_SECOND_MINUTE_HOUR;
	else if (!a1m1 && !a1m2 && !a1m3 && !a1m4 && !dydt)
		_alarmMode = DS1337_ALARM_ON_SECOND_MINUTE_HOUR_DATE;
	else if (!a1m1 && !a1m2 && !a1m3 && !a1m4 && dydt)
		_alarmMode = DS1337_ALARM_ON_SECOND_MINUTE_HOUR_DAY;
	else
		_alarmMode = DS1337_ALARM_UNKNOWN;
	return _alarmMode;
}


// Convert from unix timestamp to Date
void DS1337::getTime(uint32_t timestamp, int *year, int *month, int *day, int *hour, int *minute, int *second) {
	timestamp-= T2000UTC;
	*second = (int)(timestamp % 60);
	timestamp = timestamp / 60;
	*minute = (int)(timestamp % 60);
	timestamp = timestamp / 60;
	*hour = (int)(timestamp % 24);
	timestamp = timestamp / 24;
	*year = 0;
	uint16_t dYear = 0;
	if (*year % 4 == 0)
		dYear = 366;
	else
		dYear = 365;
	while (timestamp >= dYear) {
		*year++;
		timestamp-=dYear;
		if (*year % 4 == 0)
			dYear = 366;
		else
			dYear = 365;
	}
	*day = (int) timestamp;
	int dayMonth[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
	*month = 0;
	while (*day > dayMonth[*month+1])
		*month++;
	*day-=dayMonth[*month];
	*month++;
	*day++;
}
void DS1337::printDateTime(){
	Serial.print(_date.getDay());
	Serial.print(".");
	Serial.print(_date.getMonth());
	Serial.print(".");
	Serial.print(_date.getYear());
	Serial.print(" ");
	Serial.print(_date.getHour());
	Serial.print(":");
	Serial.print(_date.getMinutes());
	Serial.print(":");
	Serial.println(_date.getSeconds());
}

Date DS1337::getDateObj(){
	return _date;
}
/*
//Get unix timestamp - old
unsigned long DS1337::getTimestamp() {
	Date d = getDate();
	return getTimestamp(d.getYear(), d.getMonth(), d.getDay(), d.getHour(), d.getMinutes(), d.getSeconds());
}
*/
/*
void breakTime(time_te timeInput, tmElements_t *tm){
    // break the given time_te into time components y,m,d,hour,min,sec
    // note that year is offset from 1970 !!!

    uint8_t year;
    uint8_t month, monthLength;
    uint32_t time;
    unsigned long days;

    time = (uint32_t)timeInput;
    tm->Second = time % 60;
    time /= 60; // now it is minutes
    tm->Minute = time % 60;
    time /= 60; // now it is hours
    tm->Hour = time % 24;
    time /= 24; // now it is days
    tm->Wday = ((time + 4) % 7) + 1;  // Sunday is day 1

    year = 0;
    days = 0;
    while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
        year++;
    }
    tm->Year = year; // year is offset from 1970

    days -= LEAP_YEAR(year) ? 366 : 365;
    time  -= days; // now it is days in this year, starting at 0

    days=0;
    month=0;
    monthLength=0;
    for (month=0; month<12; month++) {
        if (month==1) { // february
            if (LEAP_YEAR(year)) {
                monthLength=29;
            }
            else {
                monthLength=28;
            }
        }
        else {
            monthLength = monthDays[month];
        }

        if (time >= monthLength) {
            time -= monthLength;
        }
        else {
            break;
        }
    }
    tm->Month = month + 1;  // jan is month 1
    tm->Day = time + 1;     // day of month
}

*/