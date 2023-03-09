#include <uptimeHandler/uptimeHandler.h>

void UptimeHandler::run(){
    if(millis() - lastUptimeCheck >= 1000){
        lastUptimeCheck = millis();
        if (millis() >= 3000000000) {
            HighMillis = 1;
        }
        if (millis() <= 100000 && HighMillis == 1) {
            Rollover++;
            HighMillis = 0;
        }
        long secsUp = millis() / 1000;
        Up_Second = secsUp % 60;
        Up_Minute = (secsUp / 60) % 60;
        Up_Hour = (secsUp / (60 * 60)) % 24;
        Up_Day = (Rollover * 50) + (secsUp / (60 * 60 * 24));
        sprintf(
            uptimeString,
            "%dD %02d:%02d:%02d",
            Up_Day, Up_Hour, Up_Minute, Up_Second
        );
    }
}

const char* UptimeHandler::get(){
    return uptimeString;
}