#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
	auto total_j = (float)LinuxParser::Jiffies();
	auto active_j = LinuxParser::ActiveJiffies();

  	auto curr_total = total_j - prevtotal_;
  	auto curr_active = active_j - prevactive_;
  
  	prevtotal_ = total_j;
    prevactive_ = active_j;
  
	return curr_active / curr_total;
}