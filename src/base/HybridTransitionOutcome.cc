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

#include "HybridTransitionOutcome.h"

namespace hmdp_base
{

HybridTransitionOutcome::HybridTransitionOutcome (const double &prob, ContinuousTransition *ctrans, 
						  ContinuousReward *crew)
  : m_marginalDiscreteProb (prob), m_contTransition (ctrans), m_contReward (crew)
{}

HybridTransitionOutcome::~HybridTransitionOutcome()
{
  if (m_contTransition)
    BspTree::deleteBspTree(m_contTransition);
  if (m_contReward)
    BspTree::deleteBspTree(m_contReward);
}

void HybridTransitionOutcome::print (std::ostream &out, double *low, double *high)
{
  out << "probability: " << m_marginalDiscreteProb << std::endl;
  m_contTransition->print (out, low, high);
  if (m_contReward)
    m_contReward->print (out, low, high);
}

} /* end of namespace */
