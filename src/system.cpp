#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

System::System() {
  kernel_ = LinuxParser::Kernel();
  operating_system_ = LinuxParser::OperatingSystem();
}

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
std::vector<Process>& System::Processes() {
  std::vector<int> pids{LinuxParser::Pids()};
  // Unique pid's
  std::set<int> unique_pids;
  for (auto const& process : processes_) {
    unique_pids.insert(process.Pid());
  }

  // Emplace all new processes
  for (int pid : pids) {
    if (unique_pids.find(pid) == unique_pids.end()) {
      processes_.emplace_back(pid);
    }
  }

  // Update CPU utilization
  for (auto& process : processes_) {
    process.CpuUtilization(LinuxParser::ActiveJiffies(process.Pid()),
                           LinuxParser::Jiffies());
  }
  std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() const { return kernel_; }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() const {
  return LinuxParser::MemoryUtilization();
}

// DONE: Return the operating system name
std::string System::OperatingSystem() const { return operating_system_; }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
