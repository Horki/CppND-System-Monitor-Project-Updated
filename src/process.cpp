#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

Process::Process(int pid) : pid_(pid) {}

// DONE: Return this process's ID
int Process::Pid() const { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_; }

// DONE: Set this process's CPU utilization
void Process::CpuUtilization(long active_ticks, long system_ticks) {
  long duration_active{active_ticks - prev_active_ticks_};
  long duration{system_ticks - prev_system_ticks_};
  cpu_ = static_cast<float>(duration_active) / duration;
  prev_active_ticks_ = active_ticks;
  prev_system_ticks_ = system_ticks;
}

// DONE: Return the command that generated this process
std::string Process::Command() const { return LinuxParser::Command(Pid()); }

// DONE: Return this process's memory utilization
std::string Process::Ram() const { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
std::string Process::User() const { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const {
  return CpuUtilization() < other.CpuUtilization();
}

// DONE: Overload the "greater than" comparison operator for Process objects
bool Process::operator>(Process const& other) const {
  return CpuUtilization() > other.CpuUtilization();
}
