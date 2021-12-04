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

struct current_time get_current_time(long long int current_milliseconds);
