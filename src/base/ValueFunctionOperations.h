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

#ifndef VALUEFUNCTIONOPERATIONS_H
#define VALUEFUNCTIONOPERATIONS_H

#include "BspTreeOperations.h"
#include "PiecewiseConstantValueFunction.h"
#include "PiecewiseLinearValueFunction.h"

namespace hmdp_base
{

/**
 * \class ValueFunctionOperations
 * \brief static functions for handling various operations on value functions.
 */
class ValueFunctionOperations : public BspTreeOperations
{
 public:
  /**
   * \brief sum two value functions.
   * @param vf1 value function,
   * @param vf2 value function,
   * @param low lower domain bound,
   * @param upper domain bound,
   * @returns a new value function as the result of the sum of vf1 and vf2.
   */
  static ValueFunction* sumValueFunctions (ValueFunction *vf1, ValueFunction *vf2,
					   double *low, double *high);

  /**
   * \brief subtract two value functions.
   * @param vf1 value function,
   * @param vf2 value function,
   * @param low lower domain bound,
   * @param upper domain bound,
   * @returns a new value function as the result of the subtraction of vf1 and vf2.
   */
  static ValueFunction* subtractValueFunctions (ValueFunction *vf1,
						ValueFunction *vf2,
						double *low, double *high);

  /**
   * \brief build the max of two value functions.
   * @param vf1 value function,
   * @param vf2 value function,
   * @param low lower domain bound,
   * @param upper domain bound,
   * @returns a new value function that is the max of vf1 and vf2.
   */
  static ValueFunction* maxValueFunction (ValueFunction *vf1, ValueFunction *vf2,
					  double *low, double *high);

  /**
   * \brief build the min of two value functions.
   * @param vf1 value function,
   * @param vf2 value function,
   * @param low lower domain bound,
   * @param upper domain bound,
   * @returns a new value function that is the min of vf1 and vf2.
   */
  static ValueFunction* minValueFunction (ValueFunction *vf1, ValueFunction *vf2,
					  double *low, double *high);
  
  /**
   * \brief compare two value functions.
   * @param vf1 value function,
   * @param vf2 value function,
   * @param low lower domain bound,
   * @param high upper domain bound,
   * @returns true if vf1 and vf2 are identical, false otherwise.
   */
  static bool compareValueFunctions (ValueFunction *vf1, ValueFunction *vf2,
				     double *low, double *high);

  /**
   * \brief merges a value function by action. Due the possibility of having 
   * several actions attached to the same resource space, a pwl value function
   * returned.
   * @param vf vf whose tiles are to merged if they correspond to the same set
   *           of actions,
   * @param low lower domain bound,
   * @param high upper domain bound,
   * @return a pwl value function whose tiles are merged according to identical
   *         action sets.
   */
  static PiecewiseLinearValueFunction* mergeVFByActions (ValueFunction *vf,
							 double *low, double *high);

  static void breakTiesOnActionsWithCoverage (ValueFunction *vf, double *low, double *high);

  static void breakTiesOnActionsWithMaxConsumption (ValueFunction *vf,
						    std::map<int, double> &max_cons);

  static std::map<int, ValueFunction*> breakVFByActions (ValueFunction *vfactions,
							 const bool &prop,
							 double *low, double *high);

  static ValueFunction* breakVFByAction (ValueFunction *vfactions, 
					 const int &action, const bool &prop,
					 double *low, double *high);

};

} /* end of namespace */
#endif
