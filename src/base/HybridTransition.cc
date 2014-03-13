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

#include "HybridTransition.h"

namespace hmdp_base
{
  
HybridTransition::HybridTransition (const int &dim, const int &action, double *prob,
				    ContinuousTransition **ctrans, ContinuousReward **crew)
  : m_dimension (dim), m_action (action)
{
  m_outcomes = new HybridTransitionOutcome*[m_dimension];
  for (int i=0; i<m_dimension; i++)
    {
      m_outcomes[i] = new HybridTransitionOutcome (prob[i], ctrans[i], crew[i]);
    }
}

HybridTransition::~HybridTransition()
{
  for (int i=0; i<m_dimension; i++)
    delete m_outcomes[i];
  delete[] m_outcomes;
}

void  HybridTransition::print (std::ostream &out, double *low, double *high)
{
  out << "action: " << getActionIndex () << " - discrete outcomes: " << getNOutcomes () << std::endl;
  for (int i=0; i<getNOutcomes (); i++)
    {
      out << "outcome " << i << std::endl;
      getOutcome (i)->print (out, low, high);
      out << std::endl;
    }
}

} /* end of namespace */
