#include <Eigen/Dense>
#include <iostream>
#include <math.h>
#include "Spline1D.h"


  template <typename T>
constexpr auto type_name()
{
  std::string_view name;
#ifdef __clang__
  name = __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
  name = __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
  name = __FUNCSIG__;
#endif
  return name;
}

using namespace Eigen;
int main()
{
  Array<double,Dynamic,1> x;
  Array<double,Dynamic,1> y;
  // Spline::Spline<double,100> Spl; //default initialization
  // Spline::Spline<double,100>::MaximaArray<10> M; //default initialization

  x.setLinSpaced(100,0.0,1.0);

  y.setRandom(100,1);
  Spline::Spline<double, 100> Spl(x,y);                             //explicit initialization
  auto M=Spl.maxima<10>();

  std::cout << "x = [" << x.transpose() << "]" << std::endl << std::endl;
  std::cout << "y = [" << y.transpose() << "]" << std::endl << std::endl;

  std::cout << "10 ordered maxima values of sline [x, y]: " << std::endl;
  std::cout << M << std::endl;
}
