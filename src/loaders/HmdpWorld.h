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
 * \brief World for the hmdp.
 */

#ifndef HMDPWORLD_H
#define HMDPWORLD_H

#include "config.h"
#include "HybridTransition.h"
#include "HmdpState.h"
#include <map>

#ifdef HAVE_PPDDL
#include "HmdpPpddlLoader.h"
#endif

using namespace hmdp_base;
using hmdp_engine::HmdpState;

namespace hmdp_loader
{

enum SourceType {
  ST_PPDDL
};

class HmdpWorld
{
 public:
  static void loadWorld (const char *filename);

  static bool isActionEnabled (const size_t &id, const HmdpState &hst);

  static bool isActionDiscreteEnabled (const size_t &id, const HmdpState &hst);

#ifdef HAVE_PPDDL
  static bool isGoalAchieved (const Goal &gl, const HmdpState &hst);
#endif
  
  static void applyNonResourceActionEffects (const size_t &id, HmdpState *hst, const int &probEfIndex);

  /**
   * \brief sum and returns total reward from goal achieved in a given state.
   * @param hst the hybrid state,
   * @param alreadyAchieved goals already achieved, if NULL, then goals can be achieved multiple times,
   * @param newlyAchieved pointer to a set of newly achieved goals.
   * @return a pointer to the resulting total state reward.
   */
  static ContinuousReward* sumGoalReward (const HmdpState &hst, 
					  const std::set<int> *alreadyAchieved);
  
  static void createInitialStates ();

  static void cleanWorld () { /* TODO */ }

#ifdef HAVE_PPDDL
  static void setFunctionValueInMap (const int &pos, ValueMap &values, const double &val)
    { HmdpPpddlLoader::setFunctionValueInMap (pos, values, val); }
  
  static double getFunctionValueInMap (const int &pos, const ValueMap &values)
    { return HmdpPpddlLoader::getFunctionValueInMap (pos, values); }
#endif
  
  /* accessors */
  static unsigned int getNActions () { return m_actions.size (); }
  static unsigned int getNResources () { return m_boundedResources.size (); }
  static std::pair<std::string, std::pair<double, double> >& getResource (const std::string &rsc);
  static double getResourceLowBound (const std::string &rsc);
  static double getResourceHighBound (const std::string &rsc);
  static HybridTransition* getAction (const size_t &id) { return m_actions[id]; }
  static std::string getActionName (const size_t &id);
  static void printActionCompleteName (std::ostream &os, const size_t &id);
  static std::map<size_t, HybridTransition*>::const_iterator actionsBegin () 
    { return m_actions.begin (); }
  static std::map<size_t, HybridTransition*>::const_iterator actionsEnd () 
    { return m_actions.end (); }
  static std::map<std::string, ContinuousReward*>::const_iterator goalsBegin ()
    { return m_goals.begin (); }
  static std::map<std::string, ContinuousReward*>::const_iterator goalsEnd ()
    { return m_goals.end (); }
  static ContinuousReward* findGoalReward (const std::string &gName);
  static double* getRscLowBounds () { return m_rscLow; }
  static double* getRscHighBounds () { return m_rscHigh; }
  static HmdpState* getFirstInitialState ();
  static double getInitialMaxRsc (const int &dim) { return HmdpWorld::m_maxInitialResource[dim]; }
  static double getInitialMinRsc (const int &dim) { return HmdpWorld::m_minInitialResource[dim]; }
  static void setInitialMaxRsc (const int &dim, const double &value)
    { HmdpWorld::m_maxInitialResource[dim] = value; }
  static void setInitialMinRsc (const int &dim, const double &value)
    { HmdpWorld::m_minInitialResource[dim] = value; }

  /* printing */
  static void printResources (std::ostream &out);
  static void printInitialStates (std::ostream &out);
  static void printGoals (std::ostream &out);
#ifdef HAVE_PPDDL
  static void printState (std::ostream &out, const ValueMap &values, const AtomSet &atoms);
#endif
  static void print (std::ostream &out);

 protected:
  static SourceType m_st; /**< world source type */
  static std::vector <std::pair<std::string, std::pair<double,double> > > m_boundedResources; /**< continuous resources */
  static double *m_rscLow;  /**< global resource lower bounds (same order as m_boundedResources) */
  static double *m_rscHigh; /**< global resource upper bounds */
  static std::map<size_t, HybridTransition*> m_actions; /**< world actions */
  static std::map<std::string, ContinuousReward*> m_goals;
  static std::map<double, HmdpState*> m_initialStates; /**< probability / initial state */
  static double *m_maxInitialResource;
  static double *m_minInitialResource;

 public:
  static bool m_oneTimeReward;
};

} /* end of namespace */

#endif
