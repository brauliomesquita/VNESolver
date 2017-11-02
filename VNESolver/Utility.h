#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

float get_time();

int round(float value);

inline float max(float a, float b) { return (a > b)? a : b; }
inline int max(int a, int b) { return (a > b)? a : b; }

#endif