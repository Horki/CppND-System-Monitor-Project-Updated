#include "processor.h"

#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  float utilization{0};
  long active_ticks = LinuxParser::ActiveJiffies();
  long idle_ticks = LinuxParser::IdleJiffies();
  long duration_active{active_ticks - prev_active_ticks_};
  long duration_idle{idle_ticks - prev_idle_ticks_};
  long duration{duration_active + duration_idle};
  utilization = static_cast<float>(duration_active) / duration;

  // Store for next
  prev_active_ticks_ = active_ticks;
  prev_idle_ticks_ = idle_ticks;
  return utilization;
}
