#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::ElapsedTime(long seconds) {
  std::stringstream r;
  r.fill('0');
  r << std::setw(2) << (seconds / 3600) << ":" << std::setw(2) << (seconds / 60)
    << ":" << std::setw(2) << (seconds % 60);
  return r.str();
}
