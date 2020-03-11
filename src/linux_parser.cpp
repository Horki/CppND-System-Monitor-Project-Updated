#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

// DONE: An example of how to read data from the filesystem
// grep -i pretty_name /etc/os-release
// ex.: PRETTY_NAME="Arch Linux"
std::string LinuxParser::OperatingSystem() {
  std::string line, key, value;
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

// DONE: An example of how to read data from the filesystem
// cat /proc/version
// Linux version 5.5.7-arch1-1 (linux@archlinux) (gcc version 9.2.1 20200130
// (Arch Linux 9.2.1+20200130-2)) #1 SMP PREEMPT Sat, 29 Feb 2020 19:06:02 +0000
std::string LinuxParser::Kernel() {
  std::string os, version, kernel, line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
// grep -i memtotal /proc/meminfo # fixed
// grep -i memfree  /proc/meminfo
// grep -i buffers  /proc/meminfo
float LinuxParser::MemoryUtilization() {
  float mem_total{1};
  float mem_free{0};
  float buffers{0};
  std::string token;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (stream >> token) {
      if (token == "MemTotal:") {
        if (stream >> token) mem_total = std::stof(token);
      } else if (token == "MemFree:") {
        if (stream >> token) mem_free = std::stof(token);
      } else if (token == "Buffers:") {
        if (stream >> token) buffers = std::stof(token);
      }
    }
  }
  return 1 - mem_free / (mem_total - buffers);
}

// DONE: Read and return the system uptime
// cat /proc/uptime
// ex: 769125.59 4139832.62
long LinuxParser::UpTime() {
  std::string line, token;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream stream(line);
    if (stream >> token) {
      return stoi(token);
    }
  }
  return 0;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

// DONE: Read and return the number of active jiffies for a PID
// cat /proc/$pid/stat
long LinuxParser::ActiveJiffies(int pid) {
  long jiffies{0};
  std::string line, token;
  std::vector<std::string> values;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> token) {
      values.push_back(token);
    }
  }
  if (values.size() > 21) {
    long user = std::stol(values[13]);
    long kernel = std::stol(values[14]);
    long children_user = std::stol(values[15]);
    long children_kernel = std::stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }
  return jiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> time = CpuUtilization();
  return (stol(time[CPUStates::kUser_]) + stol(time[CPUStates::kNice_]) +
          stol(time[CPUStates::kSystem_]) + stol(time[CPUStates::kIRQ_]) +
          stol(time[CPUStates::kSoftIRQ_]) + stol(time[CPUStates::kSteal_]) +
          stol(time[CPUStates::kGuest_]) + stol(time[CPUStates::kGuestNice_]));
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> time = CpuUtilization();
  return (stol(time[CPUStates::kIdle_]) + stol(time[CPUStates::kIOwait_]));
}

// DONE: Read and return CPU utilization
// grep -i cpu /proc/stat | head -n 1 # we are taking first raw
// ex.:
//
// cpu  32078232 190062 16055442 411697204 576168 1325311 732490 0 0 0
// cpu0 4058841 25067 1996131 51452178 71971 122647 118563 0 0 0
// cpu1 4070498 29646 1960711 51509998 65775 126339 87034 0 0 0
// cpu2 4064896 23092 1887298 51608532 79775 122624 76350 0 0 0
// cpu3 4053549 22922 1888345 51609499 70470 125814 78469 0 0 0
// cpu4 4057999 24392 1893796 51598624 70712 154534 63136 0 0 0
// cpu5 4109807 21883 1966474 51486901 70796 133167 66831 0 0 0
// cpu6 3838859 23629 2354810 51244115 73119 208394 74889 0 0 0
// cpu7 3823779 19427 2107875 51187353 73547 331789 167215 0 0 0

std::vector<std::string> LinuxParser::CpuUtilization() {
  std::string line, token;
  std::vector<std::string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> token) {
        if (token == "cpu") {
          while (linestream >> token) {
            values.push_back(token);
          }
          return values;
        }
      }
    }
  }
  return values;
}

// DONE: Read and return the total number of processes
// grep -i processes /proc/stat
// processes 31761886
int LinuxParser::TotalProcesses() {
  std::string line, k, v;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream stream(line);
      while (stream >> k >> v) {
        if (k == "processes") {
          return std::stoi(v);
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
// grep -i procs_running /proc/stat
// ex.: procs_running 15
int LinuxParser::RunningProcesses() {
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::string k, v;
      while (stream >> k >> v) {
        if (k == "procs_running") {
          return std::stoi(v);
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
// cat /proc/$pid/cmdline
// ex.: /usr/bin/kaccess
std::string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  return "";
}

// DONE: Read and return the memory used by a process
// TODO: Merge with ::Uid
// grep -i vmsize /proc/$pid/status
// ex.: VmSize:	  291436 kB
std::string LinuxParser::Ram(int pid) {
  std::string token;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> token) {
      if (token == "VmSize:") {
        if (stream >> token) {
          return std::to_string(std::stoi(token) / 1024);
        }
      }
    }
  }
  return "0";
}

// DONE: Read and return the user ID associated with a process
// TODO: Merge with ::Ram
// grep -i uid /proc/$pid/status
// ex.: Uid:	1000	1000	1000	1000
std::string LinuxParser::Uid(int pid) {
  std::string token;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> token) {
      if (token == "Uid:") {
        if (stream >> token) {
          return token;
        }
      }
    }
  }
  return "";
}

// DONE: Read and return the user associated with a process
// grep -i x:$uid /etc/passwd
// ex.: git:x:975:975:git daemon user:/:/usr/bin/git-shell
std::string LinuxParser::User(int pid) {
  std::ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) {
    std::string line;
    std::string token = "x:" + Uid(pid);
    while (std::getline(stream, line)) {
      auto marker = line.find(token);
      if (marker != std::string::npos) {
        return line.substr(0, marker - 1);
      }
    }
  }
  return "0";
}

// DONE: Read and return the uptime of a process
// cat proc/$pid/stat
// ex.: 1032 (kaccess) S 1014 1014 1014 0 -1 4194304 2464 25 11 0 2037 2332 0 0
// 20 0 3 0 1984 298430464 3121 18446744073709551615 94680157405184
// 94680157409733 140725716618928 0 0 0 0 0 0 0 0 0 17 6 0 0 52 0 0
// 94680157421016 94680157421584 94680163794944 140725716625949 140725716625966
// 140725716625966 140725716627431 0
long int LinuxParser::UpTime(int pid) {
  long int time{0};
  std::string token;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    for (int i = 0; stream >> token; ++i) {
      if (i == 13) {
        time = std::stol(token);
        time /= sysconf(_SC_CLK_TCK);
        return time;
      }
    }
  }
  return time;
}
