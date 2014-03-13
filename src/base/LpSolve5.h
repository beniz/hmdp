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

/**
 * \brief Linear function domination test and pruning.
 */

#ifndef LPSOLVE5_H
#define LPSOLVE5_H

#include "Lp.h"
#include <lpsolve/lp_lib.h>

namespace hmdp_base
{

/**
 * \class LpSolve5
 * \brief linear functions domination tests and pruning using lp_solve4.
 */
class LpSolve5
{
 public:
  /**
   * \brief prepare the linear programming problem, using lp_solve4 structures.
   * @param av alpha vector,
   * @param vav set of alpha vectors,
   * @param low domain lower bounds,
   * @param high domain higher bounds.
   * @return a linear programming problem in lp_solve4 format / structure.
   */
  static lprec* prepareLPProblem (AlphaVector *av, std::vector<AlphaVector*> *vav,
				  double *low, double *high);

#ifdef HAVE_CSA
  static lprec* prepareLPProblem (CSVector *av, std::vector<CSVector*> *vav,
				  double *low, double *high);
#endif
  
  /**
   * \brief solves the linear program.
   * @param lp linear programming problem in lp_solve5 format / structure.
   */
  static int solveLPProblem (lprec *lp);

  /**
   * \brief counts non zeros elements in alpha vectors parameters.
   * @param av alpha vector,
   * @param vav set of alpha vectors.
   * @return number of non zero elements.
   */
  static int countNonZeros (AlphaVector *av, std::vector<AlphaVector*> *vav);

  /**
   * \brief test if alpha vector av is dominated by alpha vectors in vav by
   *        solving a linear program (also sets the witness point).
   * @param av alpha vector,
   * @param vav set of alpha vectors,
   * @param witness point of interest (from the lp solution),
   * @param low domain lower bounds,
   * @param high domain higher bounds.
   * @return true if dominated, false otherwise.
   */
  static bool isLPDominated (AlphaVector *av, std::vector<AlphaVector*> *vav,
			     double *witness, double *low, double *high);

#ifdef HAVE_CSA
  static bool isLPDominated (CSVector *av, std::vector<CSVector*> *csv,
			     double *witness, double *low, double *high);
#endif
  
  /**
   * \brief test if all alpha vectors in vav1 are dominated by all alpha vectors
   *        in vav2.
   * @param vav1 set of alpha vectors,
   * @param vav2 set of alpha vectors,
   * @param witness point of interest (from the lp solution),
   * @param low domain lower bounds,
   * @param high domain higher bounds.
   * @return true if dominated, false otherwise.
   * @sa isLPDominated
   */
  static bool areLPDominated (std::vector<AlphaVector*> *vav1, std::vector<AlphaVector*> *vav2,
			      double *witness, double *low, double *high);

  /**
   * \brief prune a set of alpha vectors from its dominated elements by
   *        solving a set of linear programs.
   * @param vav set of alpha vectors,
   * @param low domain lower bounds,
   * @param high domain higher bounds.
   * @param res pruned set of alpha vectors.
   */
  static void pruneLP (std::vector<AlphaVector*> *vav, 
		       double *low, double *high,
		       std::vector<AlphaVector*> *res);

#ifdef HAVE_CSA  
  static void pruneLP (std::vector<CSVector*> *csv,
		       double *low, double *high,
		       std::vector<CSVector*> *res);
#endif
};

} /* end of namespace */
#endif
