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
 * \brief representation of the outcome for HybridTransition, ie.:
 *        - a (marginal) probability of the discrete outcome,
 *        - a conditional probability on continuous variables (ContinuousTransition),
 *        - a reward function of the action for this discrete outcome.
 */

#ifndef HYBRIDTRANSITIONOUTCOME_H
#define HYBRIDTRANSITIONOUTCOME_H

#include "ContinuousTransition.h"
#include "ContinuousReward.h"

namespace hmdp_base
{

/**
 * \class HybridTransitionOutcome
 * \brief representation of the outcome for HybridTransition.
 */
class HybridTransitionOutcome
{
 public:
  /**
   * \brief constructor
   * @param prob probability of this hybrid outcome (marginal probability on the 
   *             arrival discrete state).
   * @param ctrans continuous transition that corresponds to this outcome,
   * @param crew continuous reward that corresponds to this outcome.
   * @sa ContinuousTransition, ContinuousReward
  */
  HybridTransitionOutcome(const double &prob, ContinuousTransition *ctrans, ContinuousReward *crew);

  ~HybridTransitionOutcome();

 public:
  /* accessors */
  /**
   * \brief accessor to the marginal probability on the arrival state.
   * @return the marginal probability on the arrival state.
   */
  double getOutcomeProbability () const { return m_marginalDiscreteProb; }
  
  ContinuousTransition* getContTransition () const { return m_contTransition; }
  ContinuousReward* getContReward () const { return m_contReward; }

  /* printing */
  void print (std::ostream &out, double *low, double *high);

 private:
  double m_marginalDiscreteProb;
  ContinuousTransition *m_contTransition;
  ContinuousReward *m_contReward;
};

} /* end of namespace */
#endif
