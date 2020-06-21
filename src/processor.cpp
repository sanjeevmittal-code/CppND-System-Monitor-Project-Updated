#include "processor.h"
#include <string>
#include <fstream>
#include "linux_parser.h"
using namespace std;
float Processor::Utilization() { 

  string line;
  ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  string key;
  float user,nice,system,idle,iowait,irq,softirq,steal,guest,guest_nice,nonidle;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      
      linestream >> key;
      if (LinuxParser::KCpu.compare(key) == 0) {
          linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal>> guest>>guest_nice;
        
      }
    }
  }
  idle = idle + iowait;
  nonidle = user + nice + system + irq + softirq + steal;
  float total = idle + nonidle;

  float TotalDiff = total - prevTotal_;
  float IdleDiff = idle - prevIdle_;

  prevTotal_ = total;
  prevIdle_ = idle;

  return (TotalDiff - IdleDiff) / TotalDiff;
 }