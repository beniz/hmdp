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

#ifndef LP_H
#define LP_H

#include "Alg.h"
#include <vector>
#include "CSVector.h"

using hmdp_csa::CSVector;

namespace hmdp_base
{

  class AlphaVector;

/**
 * \class Lp
 * \brief static class for connecting linear programming solvers with hmdp code.
 */
class Lp : public Alg
{
 public:
  /* in a perfect world, these functions would be virtual... */
  /**
   * \brief checks if an alpha vector is dominated by a set of other alpha vectors,
   *        over a bounded continuous space.
   * @param av alpha vector,
   * @param vav stl vector of alpha vectors,
   * @param low lower bounds on the continuous space,
   * @param high lower bounds on the continuous space,
   * @return true if av is dominated by vectors in vav, false otherwise.
   * @sa AlphaVector
   */
  static bool isLPDominated (AlphaVector *av, std::vector<AlphaVector*> *vav,
			     double *low, double *high) { return true; }

  /**
   * \brief prunes a set of alpha vectors of alpha vectors that are dominated
   *        over a bounded continuous space.
   * @param vav a set of alpha vectors to be pruned,
   * @param low lower bounds on the continuous space,
   * @param high lower bounds on the continuous space.
   * @param res the pruned set of alpha vectors.
   * @warning vav is deleted in the operation !
   * @sa AlphaVector
   */
  static void pruneLP (std::vector<AlphaVector*> *vav, 
		       double *low, double *high,
		       std::vector<AlphaVector*>* res) {}

  static void pruneLP (std::vector<CSVector*> *csv, 
		       double *low, double *high,
		       std::vector<CSVector*>* res) {}
};

}  /* end of namespace */
#endif
