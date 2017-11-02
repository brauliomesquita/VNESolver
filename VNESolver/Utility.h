#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

double get_time();

int round(double value);

#endif