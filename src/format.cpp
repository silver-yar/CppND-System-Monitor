#include <string>
#include <cmath>

#include "format.h"
#include "linux_parser.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
	string hh = std::to_string(seconds / 3600);
  	if (seconds / 3600 < 10)
      hh = "0" + hh;
  	string mm = std::to_string((seconds % 3600) / 60);
  	if (((seconds % 3600) / 60) < 10)
      mm = "0" + mm;
  	string ss = std::to_string((seconds % 3600) % 60);
  	if (((seconds % 3600) % 60) < 10)
      ss = "0" + ss;
  	return hh + ":" + mm + ":" + ss;
}