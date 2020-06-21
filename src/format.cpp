#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
    string uptime;
    
    int min = int(seconds)/60;
    int sec = int(seconds) % 60;
    int hr = min / 60;
    min = min %  60;
    uptime = std::to_string(hr) + ":" + std::to_string(min) + ":" + std::to_string(sec);
    return uptime; 
    
    }
