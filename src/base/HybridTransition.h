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
 * \brief hybrid transition, represents discrete outcomes of an action (the underlying
 *        continuous transformations are represented by a ContinuousTransition.
 */

#ifndef HYBRIDTRANSITION_H
#define HYBRIDTRANSITION_H

#include "HybridTransitionOutcome.h"

namespace hmdp_base
{

/**
 * \class HybridTransition
 * \brief hybrid transition, represents discrete outcomes of an action
 */
class HybridTransition
{
 public:
  /**
   * \brief Constructor
   * @param dim number of discrete outcomes,
   * @param action action index,
   * @param prob array of discrete outcome probabilities (one per outcome),
   * @param ctrans array of continuous transitions (one per discrete outcome),
   * @param crew array of continuous rewards (one per discrete outcome).
   */
  HybridTransition (const int &dim, const int &action, double *prob,
		    ContinuousTransition **ctrans, ContinuousReward **crew);

  ~HybridTransition();

  /* accessors */
  /**
   * \brief accessor to the number of discrete outcomes.
   * @return number of discrete outcomes.
   */
  int getNOutcomes () const { return m_dimension; }
  
  /**
   * \brief accessor to a discrete outcome.
   * @param i outcome index,
   * @return pointer to a discrete outcome.
   */
  HybridTransitionOutcome* getOutcome (const int &i) const { return m_outcomes[i]; }
  
  /**
   * \brief accessor to the action index.
   * @return action index attached to this hybrid transition.
   */
  int getActionIndex () const { return m_action; }

  /* printing */
  void print (std::ostream &out, double *low, double *high);

 private:
  int m_dimension; /**< number of discrete outcomes for this transition */
  int m_action;  /**< action index */
  HybridTransitionOutcome **m_outcomes; /**< hybrid outcomes for this action */
};

} /* end of namespace */
#endif
