#include <linux/string.h>
#include <linux/slab.h> // kmalloc
#include "current-time.h"

#define DATETIME_SIZE 22
#define SECONDS_ON_YEAR 31556952 // Acording to gregorian calendar
#define SECONDS_ON_DAY 86400
#define SECONDS_ON_HOUR 3600
#define SECONDS_ON_MINUTE 60
#define MILLISECONDS_ON_SECOND 1000

int months[12] = {
    31, 28, 31, // Jan, Feb, Mar
    30, 31, 30, // Apr, May, Jun
    31, 31, 30, // Jul, Ago, Sep
    31, 30, 31  // Oct, Nov, Dic
};

struct current_time *get_current_time(const long long int current_milliseconds) {
    struct current_time *now = kmalloc(sizeof(struct current_time), GFP_KERNEL);

    int acum = 0;
    int day_of_month = 0;
    int current_month = 0;

    // Unix time start from 1970
    now->year = 1970 + current_milliseconds / SECONDS_ON_YEAR;
    now->day_of_year = current_milliseconds % SECONDS_ON_YEAR / SECONDS_ON_DAY;

    for(current_month=0; current_month<12; current_month++) {
        acum += months[current_month];
        if(acum >= now->day_of_year) {
            day_of_month = months[current_month] - (acum - now->day_of_year);
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

char *format_current_time(void) {
    struct timespec64 time;

    // Get the current unix time
    ktime_get_real_ts64(&time);

    return format_time(time.tv_sec);
}
