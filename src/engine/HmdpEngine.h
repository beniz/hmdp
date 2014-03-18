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

#ifndef HMDPENGINE_H
#define HMDPENGINE_H

#include "HmdpWorld.h"
#include "HmdpState.h"
#include <chrono>

using namespace hmdp_base;
using namespace hmdp_loader;

namespace hmdp_engine
{

class HmdpEngine
{
 public:
  /**
   * \brief depth first search back up that computes each discrete
   *        state's probability distribution over resources during the search.
   *        For this reason, this procedure is slower than the previous one.
   * @param initState the initial to start the search from.
   * @param backups whether to perform backups.
   * @param csd whether to propagate probability distribution forward with actions.
   * @param max_dfs_recur maximum number of recursive calls (default is -1 for unlimited).
   * @sa HmdpState
   */
  static void DepthFirstSearchBackupCSD (HmdpState *initState,
					 const bool &backups=true,
					 const bool &csd=false,
					 const int &max_dfs_recur=-1);

  static void ValueIteration(HmdpState *initState,
			     const double &gamma,
			     const double &epsilon,
			     const int &T,
			     const int &max_dfs_recur=-1);
  
  static void BspBackup (HmdpState *hst,
			 double &residual,
			 const bool &with_residual=false,
			 const double &gamma=1.0);

  /* accessors */
  static size_t getNStates () { return HmdpEngine::m_nextStates.size (); }

 private:
  static ContinuousReward* computeRewardFromGoals (HybridTransitionOutcome *hto,
						   HmdpState *nextState);
  
  /**
   * \brief average rewards from goals over an action outcomes.
   * @param hst the hmdp state from which the action is applied,
   * @param ht the hybrid transition (i.e. action).
   * @return the averaged reward (bsp tree).
   * @sa HmdpWorld
   */
  static ContinuousReward* computeRewardFromGoals (HmdpState *hst, HybridTransition *ht);

  /**
   * \brief tests if a state has already been visited (dfs), and return
   *        the existing pointer, if any.
   * @param hst state.
   */
  static HmdpState* hasBeenVisited (const HmdpState *hst);

  /* states dag setter/accessor */
  static void addNextState (HmdpState *hst, const short &action, HmdpState *nextState);
  static HmdpState* getNextState (HmdpState *hst, const short &action, const size_t &pos);

 public:
  static std::unordered_map<unsigned int,std::unordered_map<int,std::vector<HmdpState*> > > m_nextStates; /**< map of successor states, for each state, filled up during the dfs search. */
  static std::unordered_map<unsigned int,HmdpState*> m_states;
  
  
  static int m_nbackups;
  static int m_vf_nbackups;
  static int m_mean_backup_time;
  static int m_leaves;
  static std::chrono::time_point<std::chrono::system_clock> m_tstart;
  static std::chrono::time_point<std::chrono::system_clock> m_tend;
};

} /* end of namespace */

#endif
