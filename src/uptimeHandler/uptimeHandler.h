#ifndef uptimeHandler_h
#define uptimeHandler_h

#include <Arduino.h>

class UptimeHandler {
    private:
        char uptimeString[25];
        int Up_Day  = 0,Up_Hour = 0,Up_Minute   = 0,Up_Second   = 0,HighMillis  = 0,Rollover        = 0,
            Month   = 0,Day     = 0,Hour        = 0,Min         = 0,Sec         = 0,currentDayInt   = 0;
        long lastUptimeCheck    = 0;
    public:
        void run();
        const char* get();
};

#endif