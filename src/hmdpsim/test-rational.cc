#include "rational.h"
#include <stdlib.h>

using namespace std;
using namespace ppddl_parser;

int main (int argc, char *argv[])
{
  double num = atof (argv[1]);
 
  Rational rat (num);
  
  std::cout << "num: " << rat.numerator () << " -- den: " << rat.denominator () << std::endl;
  std::cout << rat.double_value () << std::endl;
}
