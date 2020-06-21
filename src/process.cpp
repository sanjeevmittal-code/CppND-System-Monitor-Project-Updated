#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <linux_parser.h>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { pid_ = pid;}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { 
  string line;
  vector<string> list;
  string value;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) list.push_back(value);
  }
  float usertime,scheduletime,cutime,cstime,startime,totaltime;
  usertime = stof(list[13]);
  scheduletime = stof(list[14]);
  cutime = stof(list[15]);
  cstime = stof(list[16]);
  startime = stof(list[21]);
  totaltime = usertime + scheduletime + cutime + cstime;
  float seconds = LinuxParser::UpTime() - (startime/sysconf(_SC_CLK_TCK));
  cpuutil_=  totaltime/sysconf(_SC_CLK_TCK)/seconds;
  return cpuutil_;

}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_);  }

bool Process::operator<(Process const& a) const { return a.cpuutil_ < cpuutil_; }