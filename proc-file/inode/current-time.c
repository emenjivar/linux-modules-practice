#include <linux/string.h>
#include <linux/slab.h> // kmalloc
#include "current-time.h"

#define DATETIME_SIZE 22
#define SECONDS_ON_YEAR 31556952 // Acording to gregorian calendar
#define SECONDS_ON_DAY 86400
#define SECONDS_ON_HOUR 3600
#define SECONDS_ON_MINUTE 60
#define MILLISECONDS_ON_SECOND 1000

/**
* Number of days in each month
*/
int months[12] = {
    31, 28, 31, // Jan, Feb, Mar
    30, 31, 30, // Apr, May, Jun
    31, 31, 30, // Jul, Ago, Sep
    31, 30, 31  // Oct, Nov, Dic
};

int months_leap_year[12] = {
    31, 29, 31, // Jan, Feb, Mar
    30, 31, 30, // Apr, May, Jun
    31, 31, 30, // Jul, Ago, Sep
    31, 30, 31  // Oct, Nov, Dic
};

/**
* This array contains the progressive sum of days
* for every month during a normal year (365 days)
*/
int acum_days_month[12] = {
    31 ,  59,  90, // Jan, Feb, Mar
    120, 151, 181, // Apr, May, Jun
    212, 242, 273, // Jul, Ago, Sep
    304, 334, 365  // Oct, Nov, Dic
};

/**
* This array contains the progressive sum of days
* for every month during a leap year (366 days)
*/
int acum_days_month_leap_year[12] = {
    31,   60,  91, // Jan, Feb, Mar
    121, 152, 182, // Apr, May, Jun
    213, 243, 274, // Jul, Ago, Sep
    305, 335, 366  // Oct, Nov, Dic
};

int is_leap_year(unsigned short int year) {
    if(year % 4 == 0 && year % 100 == 0 && year % 400 == 0) {
        return 1;
    } else if(year % 4 == 0 && year % 100 != 0) {
        return 1;
    } else {
        return 0;
    }
}

struct current_time *get_current_time(const long long int current_milliseconds) {
    struct current_time *now = kmalloc(sizeof(struct current_time), GFP_KERNEL);
    int leap_year;
    int *array_months;
    int *array_months_acum;
    int day_of_month = 0;
    int current_month = 0;

    // Unix time start from 1970
    now->year = 1970 + current_milliseconds / SECONDS_ON_YEAR;
    leap_year = is_leap_year(now->year);

    // Choosing the appropriate array according to the type of year
    array_months = leap_year ? months_leap_year : months;
    array_months_acum = leap_year ? acum_days_month_leap_year : acum_days_month;

    now->day_of_year = current_milliseconds % SECONDS_ON_YEAR / SECONDS_ON_DAY;

    for(current_month=0; current_month<12; current_month++) {
        if(array_months_acum[current_month] >= now->day_of_year) {
            day_of_month = array_months[current_month] - (array_months_acum[current_month] - now->day_of_year);
            break;
        }
    }

    now->month = current_month;
    now->day = day_of_month;
    now->hours = current_milliseconds % SECONDS_ON_DAY / SECONDS_ON_HOUR;
    now->minutes = current_milliseconds % SECONDS_ON_HOUR / SECONDS_ON_MINUTE;
    now->seconds = current_milliseconds % SECONDS_ON_MINUTE;
    now->milliseconds = current_milliseconds % MILLISECONDS_ON_SECOND;

    return now;
}

char *format_time(const long long int current_milliseconds) {
    char *string_date = kmalloc(DATETIME_SIZE, GFP_KERNEL);
    struct current_time *now = get_current_time(current_milliseconds);

    // Formatting datetime on string
    snprintf(
        string_date,
        DATETIME_SIZE,
        "[%d-%d-%d %d:%d:%d] ",
        now->year,
        now->month,
        now->day,
        now->hours,
        now->minutes,
        now->seconds
    );

    return string_date;
}

char *format_current_time(const int utc_offset) {
    struct timespec64 time;
    long long int utc_offset_hours;

    // Get the current unix time
    ktime_get_real_ts64(&time);
    utc_offset_hours = SECONDS_ON_HOUR * utc_offset;

    return format_time(time.tv_sec + utc_offset_hours);
}
