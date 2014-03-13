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
 * \brief class of static functions for bellman backup with hybrid transitions.
 *
 * \author E. Benazera
 */

#ifndef BACKUPOPERATIONS_H
#define BACKUPOPERATIONS_H

#include "ValueFunctionOperations.h"
#include "ContinuousOutcome.h"
#include "ContinuousReward.h"
#include "HybridTransition.h"

namespace hmdp_base
{

/**
 * \class BackupOperations
 * \brief class of static functions for bellman backup.
 */
class BackupOperations : public ValueFunctionOperations
{
 public:  /* TODO: private */
  static ValueFunction* selectCachedTree (const ValueFunction &vf, const ContinuousTransition &ct);
  
  static ValueFunction* intersectCOWithVF (ValueFunction *vf, ContinuousOutcome *co, 
					   double *low, double *high);

  static ValueFunction* intersectVFWithReward (ValueFunction *vf, ContinuousReward *cr,
					       double *low, double *high);

  static ValueFunction* backUpCT (ValueFunction *vf, ContinuousTransition *ct,
				  double *low, double *high);
  
  static ValueFunction* backUpCT2 (ValueFunction *vf, ContinuousTransition *ct,
				   double *low, double *high);
  
  static ValueFunction* backUpOutcomes (const int &h, const int &t,
					ContinuousTransition *ct, ValueFunction *vf,
					double *low, double *high);

  static ValueFunction* backUpSingleOutcome (ValueFunction *vf, ContinuousOutcome *co,
					     double *low, double *high);

 public:
  /**
   * \brief computes the convolution of value function vf with a continuous transition ct,
   *        and reward cr.
   * @param vf value function to back up,
   * @param ct continuous transition (action),
   * @param cr action reward,
   * @param action action index (for tagging the pieces).
   * @param low domain lower bounds,
   * @param high domain upper bounds,
   * @return a value function that is the 'convolution' of vf, ct and cr.
   * @sa ValueFunction, ContinuousTransition, ContinuousReward.
   */
  static ValueFunction* backUp (ValueFunction *vf, ContinuousTransition *ct, ContinuousReward *cr,
				const int &action, double *low, double *high);

  /**
   * \brief backs up an action (hybrid transition) and all its outcomes.
   * @param ht hybrid transition, i.e. an action with probabilistic discrete and continuous outcomes.
   * @param discStatesVF value function pointers, one per discrete state outcome of ht.
   * @param low domain lower bounds,
   * @param high domain upper bounds.
   * @sa BackupOperations::backUp
   */
  static ValueFunction* backUp (const HybridTransition &ht, ValueFunction **discStatesVF, 
				double *low, double *high);

 private:
  

};

} /* end of namespace */
#endif
