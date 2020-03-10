#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long prev_active_ticks_{0};
  long prev_idle_ticks_{0};
};

#endif