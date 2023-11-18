#include <stdio.h>
#include <stdint.h>

typedef unsigned long time_te;

typedef enum {
    dowInvalid, Nedela, Pondelok, Utorok, Streda, Stvrtok, Piatok, Sobota
} timeDayOfWeek_t;


typedef struct  { 
  uint8_t Second; 
  uint8_t Minute; 
  uint8_t Hour; 
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month; 
  uint8_t Year;   // offset from 1970; 
} 	tmElements_t;


/*==============================================================================*/
/* Useful Constants */
#define SECS_PER_MIN  ((time_te)(60UL))
#define SECS_PER_HOUR ((time_te)(3600UL))
#define SECS_PER_DAY  ((time_te)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((time_te)(7UL))

/*============================================================================*/
/* functions to convert to and from system time */
/* These are for interfacing with time services and are not normally needed in a sketch */
// leap year calculator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

time_te makeTime(const tmElements_t tm);
void breakTime(time_te timeInput, tmElements_t *tm);
