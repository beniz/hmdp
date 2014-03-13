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
 * \brief This is the hmdp state for the full engine. A state maintains a discrete and 
 *        continuous parts, plus a mapping from the resource continuous space to 
 *        expected reward (value function) in the plan, plus a continuous (discretized)
 *        probability distribution over the resources.
 */

#ifndef HMDPSTATE_H
#define HMDPSTATE_H

#include "config.h"
#include "ValueFunction.h"
#include "ContinuousStateDistribution.h"

#ifdef HAVE_PPDDL
#include "expressions.h"  /* structures for the non-resource state are
			     reused from the ppddl parser */
#include "formulas.h"
#endif

using namespace hmdp_base;

#ifdef HAVE_PPDDL
using ppddl_parser::ValueMap;
using ppddl_parser::AtomSet;
#endif

namespace hmdp_engine
{

/**
 * \class HmdpState
 * \brief Hybrid state for the hmdp engine (backups).
 */
class HmdpState
{
 public:
  /**
   * \brief constructor. Value function is initialized to a pwc 0 function.
   */
  HmdpState ();

  /**
   * \brief constructor.
   * @param csd is the initial continuous distribution over resources for that state.
   */
  HmdpState (ContinuousStateDistribution *csd);

  /**
   * \brief copy constructor.
   */
  HmdpState (const HmdpState &hst);

  ~HmdpState ();
  
  /**
   * \brief tests if this state is identical to the argument state at the discrete 
   *        and continuous level, but not at the resource level (value functions 
   *        are not compared !).
   * @param hst the argument state.
   * @sa HmdpEngine::DepthFirstSearchBackup
   */
  bool isEqual (const HmdpState &hst);
  
  bool isDiscreteEqual (const HmdpState &hst);

  /* accessors */
  /**
   * \brief accessor the state unique index.
   * @return the state index.
   */
  int getStateIndex () const { return m_stateIndex; }
  
  void setStateIndex (const int &sindex) { m_stateIndex = sindex; }

#ifdef HAVE_PPDDL
  /**
   * \brief accessor to the state continuous values.
   * Beware, value of resources are set to the maximum value. The full resource
   * space appears in the value function, not in the value map.
   * @return the set of continuous values (also known as functions in ppddl).
   */
  ValueMap& getContState () { return m_values; }

  /**
   * \brief accessor to the discrete state.
   * @return the set of atoms that form the discrete state.
   */
  AtomSet& getDiscState () { return m_atoms; }

  /**
   * \brief const accessor to the state continuous values.
   */
  const ValueMap& getContStateConst () const { return m_values; }

  /**
   * \brief const accessor to the discrete state.
   */
  const AtomSet& getDiscStateConst () const { return m_atoms; }
#endif
  
  /**
   * \brief accessor to the state's value function.
   * @return value function.
   * @sa ValueFunction
   */
  ValueFunction* getVF () const { return m_stateVF; }

  /**
   * \brief accessor to the state's probability distribution over resources.
   * @return continuous state distribution.
   * @sa ContinuousStateDistribution
   */
  ContinuousStateDistribution* getCSD () const { return m_stateCSD; }

  static int getStateCounter () { return HmdpState::m_statesCount; }

  /* setters */
  void setVF (ValueFunction *vf);
  void setCSD (ContinuousStateDistribution *csd);
  void setCSDToNull ();
  static void decrementStatesCounter () { HmdpState::m_statesCount--; }

  /* printing */
  void print (std::ostream &out);

 public:
  static int m_statesCount;  /**< hybrid states counter */

  int m_stateIndex; /**< index */
#ifdef HAVE_PPDDL
  ValueMap m_values;  /**< non-resource continuous values in this state. */
  AtomSet m_atoms;  /**< discrete values in this state. */
#endif  
  ValueFunction *m_stateVF;  /**< value function attached to this state */
  ContinuousStateDistribution *m_stateCSD;  /**< state discretized probability distribution
					       over resources. */
};

} /* end of namespace */

#endif
