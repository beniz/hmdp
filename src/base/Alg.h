/**
 * Copyright 2014 Emmanuel Benazera beniz@droidnik.fr
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ALG_H
#define ALG_H

#include "config.h"
#include <math.h>
#include <vector>

namespace hmdp_base
{

/**
 * \class Alg
 * \brief Root static class for algorithms.
 */
class Alg
{
 public:
  /* comparison functions */
  static bool REqual (const double &x, const double &y, const double &e)
    { return (fabs ((x)-(y)) <= (e)) ? true : false; }

  static bool RSupEqual (const double &x, const double &y, const double &e)
    { return (Alg::REqual (x, y, e) || (x > y)) ? true : false; }

  static bool RInfEqual (const double &x, const double &y, const double &e)
    { return (Alg::REqual (x, y, e) || (x < y)) ? true : false; }

  static bool RSup (const double &x, const double &y, const double &e)
      { return (x > y + e) ? true : false; }

  static bool RSup (const double &x, const double &y)
      {  return (x > y) ? true : false; }

  static bool RInf (const double &x, const double &y, const double &e)
      { return (x < y - e) ? true : false; }

  static bool RInf (const double &x, const double &y)
      { return (x < y) ? true : false; }

  /* vector int operations (used in the handling of goal sets) */
  static bool inVectorInt (const int &g, const std::vector<int> &v);

  /**
   * \brief test if v1 is included in v2.
   */ 
  static bool inVectorInt (const std::vector<int> &v1, const std::vector<int> &v2);

  /**
   * \brief test if v1 contains the same numbers as v2.
   */
  static bool eqVectorInt (const std::vector<int> &v1, const std::vector<int> &v2);

 protected:
 private:
  
 public:
  static double m_doubleEpsilon; /**< tolerance for comparing double */
};

} /*end of namespace */
#endif
