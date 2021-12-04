struct current_time {
    unsigned short int year;
    unsigned char month;
    unsigned char day;
    unsigned short int day_of_year;
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned short int milliseconds;
};

/**
* return a formatting time structure pointer
* @param current_milliseconds must be unix-time
*/
struct current_time *get_current_time(const long long int current_milliseconds);

/**
* return a formatted datetime, followindg the next format
* [yyyy-MM-dd hh:mm:ss] => [2021-11-4 20:43:33]
*/
char *format_time(const long long int current_milliseconds);

/**
* return a formatted current datetime following the next format:
* [yyyy-MM-dd hh:mm:ss] => [2021-11-4 20:43:33]
* The date is in no UTC offset (yet)
*/
char *format_current_time(void);
