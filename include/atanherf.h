#ifndef ATANHERF_H
#define ATANHERF_H

#include <cmath>
#include <fstream>
#include <algorithm>
#include <memory>
#include <numeric>

#ifdef VERBOSE
#include <iostream>
#endif

namespace AtanhErf
{
  void getinp(const double &mm, const double &st, double *&inp);
  double atanherf_largex(const double &x);
  double atanherf_interp(const double &x);
  double evalpoly(const double &x);
  double atanherf(const double &x);
}

#endif // ATANHERF_H
