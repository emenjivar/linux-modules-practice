#include "current-time.h"

#define SECONDS_ON_YEAR 31556952
#define SECONDS_ON_DAY 86400
#define SECONDS_ON_HOUR 3600
#define SECONDS_ON_MINUTE 60
#define MILLISECONDS_ON_SECOND 1000

int months[12] = {
    31, 28, 31,
    30, 31, 30,
    31, 31, 30,
    31, 30, 31
};

struct current_time get_current_time(long long int current_milliseconds) {
    struct current_time now;

    int acum = 0;
    int day_of_month = 0;
    int current_month = 0;

    now.year = 1970 + current_milliseconds / SECONDS_ON_YEAR;;
    now.day_of_year = current_milliseconds % SECONDS_ON_YEAR / SECONDS_ON_DAY;

    for(current_month=0; current_month<12; current_month++) {
        acum += months[current_month];
        if(acum >= now.day_of_year) {
            day_of_month = months[current_month] - (acum - now.day_of_year);
            break;
        }
    }

    now.month = current_month;
    now.day = day_of_month;
    now.hours = current_milliseconds % SECONDS_ON_DAY / SECONDS_ON_HOUR;
    now.minutes = current_milliseconds % SECONDS_ON_HOUR / SECONDS_ON_MINUTE;
    now.seconds = current_milliseconds % SECONDS_ON_MINUTE;
    now.milliseconds = current_milliseconds % MILLISECONDS_ON_SECOND;

    return now;
}
