#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  double prevactive_ = 0; // State variable
  double prevtotal_ = 0; // State variable
};

#endif