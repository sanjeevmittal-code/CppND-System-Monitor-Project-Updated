#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  int64_t TotalMemory;
  int64_t FreeMemory;
  float MemUtil;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      linestream >> key >> value;
      if (kTotalMemory.compare(key) == 0) {
        TotalMemory = stoi(value);
      } else if (kFreeMemory.compare(key) == 0) {
        FreeMemory = stoi(value);
      }
    }
  }

  MemUtil = (float)(TotalMemory - FreeMemory) / TotalMemory;
  return MemUtil;
}

long LinuxParser::UpTime() {
  long idle;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> idle;
  }
  return idle;
}

long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK);
 }

long LinuxParser::ActiveJiffies(int pid) {

 string line;
  vector<string> list;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) list.push_back(value);
  }
  return  stol(list[13]) + stol(list[14]) + stol(list[15]) + stol(list[16]);

}

long LinuxParser::ActiveJiffies() { 
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  string key;
  long user,nice,system,idle,iowait,irq,softirq,steal,guest,guest_nice;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      
      linestream >> key;
      if (LinuxParser::KCpu.compare(key) == 0) {
          linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal>> guest>>guest_nice;
          break;
      }
    }
  }
return (user + nice + system + idle + iowait + irq + softirq + steal+ guest+guest_nice);
  }

long LinuxParser::IdleJiffies() { 
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  string key;
  long user,nice,system,idle,iowait,irq,softirq,steal,guest,guest_nice;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      
      linestream >> key;
      if (LinuxParser::KCpu.compare(key) == 0) {
          linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal>> guest>>guest_nice;
          break;
      }
    }
  }
  return idle + iowait;
}

vector<string> LinuxParser::CpuUtilization() {
  string line;
  vector<string> cpu = vector<string>();
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string value;
      linestream >> value;
      if (KCpu.compare(value) == 0) {
        while (linestream >> value) {
          cpu.push_back(value);
        }
      }
    }
  }
  return cpu;
}

int LinuxParser::TotalProcesses() {
  string line;
  int Processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      linestream >> key >> value;
      if (KProcesses.compare(key) == 0) {
        Processes = stoi(value);
        break;
      }
    }
  }
  return Processes;
}

int LinuxParser::RunningProcesses() {
  string line;
  int Processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      linestream >> key >> value;
      if (KProcessesRun.compare(key) == 0) {
        Processes = stoi(value);
        break;
      }
    }
  }
  return Processes;
}

string LinuxParser::Command(int pid) { 
string result;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) { 
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> result;
  }
  return result;

 }

string LinuxParser::Ram(int pid) {
  string result;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + "/" + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      while (linestream >> key >> value) {
        if (KVmSize.compare(key) == 0) {
          return std::to_string(std::stol(value)/1024);
        }
      }
    }
  }
  return result;
}

string LinuxParser::Uid(int pid) {
  string result;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + "/" + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      while (linestream >> key >> value) {
        if (KUid.compare(key) == 0) {
          return value;
        }
      }
    }
  }
  return result;
}

string LinuxParser::User(int pid) {
  string result;
  string line;
  vector<string> list;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string value;
      string user, username, uidvalue;
      while (linestream >> user >> value >> uidvalue) {
        if (uidvalue == LinuxParser::Uid(pid)) {
          result = user;
          break;
        }
      }
    }
  }
  return result;
}

long LinuxParser::UpTime(int pid) {
  string line;
  vector<string> list;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) list.push_back(value);
  }
  return UpTime() - stol(list[21]) / sysconf(_SC_CLK_TCK);
}
