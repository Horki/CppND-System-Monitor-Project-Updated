#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  void CpuUtilization(long, long);
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const&) const;
  bool operator>(Process const&) const;

 private:
  int pid_{1};
  float cpu_{0};
  long prev_active_ticks_{0};
  long prev_system_ticks_{0};
};

#endif
