#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    string uptime;
    
    int min = int(seconds)/60;
    int sec = int(seconds) % 60;
    int hr = min / 60;
    min = min %  60;
    uptime = std::to_string(hr) + ":" + std::to_string(min) + ":" + std::to_string(sec);
    return uptime; 
    
    }