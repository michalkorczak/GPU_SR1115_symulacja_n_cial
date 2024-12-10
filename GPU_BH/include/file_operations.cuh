#ifndef CSV_CUH
#define CSV_CUH

#include <string>

#include "BarnesHut.cuh"

void writeFile(std::string filename, Bodies* bodies, float seconds, int n, bool append = false);

#endif