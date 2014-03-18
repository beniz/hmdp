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

#include "HmdpEngine.h"
#include "BackupOperations.h"
#include "ValueFunctionOperations.h"
#include <iomanip>
#include <limits>

namespace hmdp_engine
{
std::unordered_map<unsigned int,std::unordered_map<int,std::vector<HmdpState*> > > HmdpEngine::m_nextStates;
std::unordered_map<unsigned int,HmdpState*> HmdpEngine::m_states;
int HmdpEngine::m_nbackups = -1;
int HmdpEngine::m_vf_nbackups = 0;
int HmdpEngine::m_mean_backup_time = 0;
int HmdpEngine::m_leaves = 0;
std::chrono::time_point<std::chrono::system_clock> HmdpEngine::m_tstart = std::chrono::system_clock::now();
std::chrono::time_point<std::chrono::system_clock> HmdpEngine::m_tend = std::chrono::system_clock::now();
  
int owidth = 15;
  
void HmdpEngine::DepthFirstSearchBackupCSD (HmdpState *hst,
					    const bool &backups,
					    const bool &csd,
					    const int &max_dfs_recur)
{
  static int s_calls = 0;
  
  double residual = 0.0; // unused in dfs.
  if (m_nbackups == -1)
    {
      std::cout << "Total time" << std::setw(owidth) << fixed << "#discs" << std::setw(owidth) << fixed << "#vf_backups"
		<< std::setw(owidth) << fixed << "lbtime" << std::setw(owidth) << fixed << "mbtime" << std::setw(owidth) << fixed << "ntiles\n";;
      m_nbackups = 0;
    }
  s_calls++;
  if (max_dfs_recur != -1 && s_calls >= max_dfs_recur) // if there's an upper limit to the number of recursive call, leave.
    return;
  
  unsigned int hst_uint = hst->to_uint();
  HmdpEngine::m_states.insert(std::pair<unsigned int,HmdpState*>(hst_uint,hst));
  HmdpEngine::m_nextStates.insert(std::pair<unsigned int,std::unordered_map<int,std::vector<HmdpState*> > >(hst_uint,std::unordered_map<int,std::vector<HmdpState*> >()));
  
  /* iterate all actions in the world */
  std::map<size_t, HybridTransition*>::const_iterator ai;
  for (ai = HmdpWorld::actionsBegin (); ai != HmdpWorld::actionsEnd (); ai++)
    {
      /* test if action is applicable to this state, check on the discrete state,
	 and check on max resources (equivalent to not check on resources). */
      if (HmdpWorld::isActionEnabled ((*ai).first, *hst))
	{
	  
	  /* iterate action discrete outcomes */
	  HybridTransition *ht = (*ai).second;
	  for (int i=0; i<ht->getNOutcomes (); i++)
	    {
	      /* apply discrete outcome effect */
	      HmdpState *nextState = new HmdpState (*hst);
	      HmdpWorld::applyNonResourceActionEffects ((*ai).first, nextState, i);

	      /* test if state has been already visited. */
	      HmdpState *existingState = NULL;
	      if ((existingState = HmdpEngine::hasBeenVisited (nextState)) != NULL)
		{
		  HmdpEngine::addNextState (hst, ht->getActionIndex (), existingState);
		  delete nextState;
		  HmdpState::decrementStatesCounter ();

		  if (csd)
		    {
		      /* 
			 update existing state's distribution over resources:
			 - compute the arrival state distribution,
			 - add it to the existing state's distribution.
		      */
		      HybridTransitionOutcome *hto = ht->getOutcome (i);
		      ContinuousStateDistribution *nextStateCSD
			= ContinuousStateDistribution::frontUp (hst->getCSD (), 
								hto->getContTransition (),
								HmdpWorld::getRscLowBounds (),
								HmdpWorld::getRscHighBounds (),
								hto->getOutcomeProbability ());
		      ContinuousStateDistribution *stateCSD
			= ContinuousStateDistribution::addContinuousStateDistributions (nextStateCSD,
											existingState->getCSD (),
											HmdpWorld::getRscLowBounds (),
											HmdpWorld::getRscHighBounds ());
		      existingState->setCSD (stateCSD); /* previous csd is automatically deleted */
		    }
		  
		  break;  /* skip that outcome == depth reached here. */
		}

	      if (csd)
		{
		  /* compute new state's distribution over resources */
		  HybridTransitionOutcome *hto = ht->getOutcome (i);
		  ContinuousStateDistribution *nextStateCSD 
		    = ContinuousStateDistribution::frontUp (hst->getCSD (), hto->getContTransition (),
							    HmdpWorld::getRscLowBounds (),
							    HmdpWorld::getRscHighBounds (),
							    hto->getOutcomeProbability ());
		  nextState->setCSD (nextStateCSD);
		}

	      /* TODO: instantiate actions that could not be instantiated before */

	      /* add state to successors */
	      HmdpEngine::addNextState (hst, ht->getActionIndex (), nextState);

	      /* dfs recursive backup */
	      HmdpEngine::DepthFirstSearchBackupCSD (nextState,backups,csd,max_dfs_recur);
	    }
	}  /* end if enabled */
    }  /* end for actions */

  /* backup */
  if (backups)
    {
      std::chrono::time_point<std::chrono::system_clock> bu_start, bu_end;
      bu_start = std::chrono::system_clock::now();
      HmdpEngine::BspBackup (hst,residual);
      bu_end = m_tend = std::chrono::system_clock::now();
      int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_tend-m_tstart).count();
      int elapsed_bu_ms = std::chrono::duration_cast<std::chrono::milliseconds>(bu_end-bu_start).count();
      m_nbackups++;
      m_mean_backup_time += elapsed_bu_ms / 1000.0;
      m_leaves += HmdpWorld::getFirstInitialState()->getVF()->countLeaves();
      
      // log total time, total number of backup states, total number of vf backups, last state backup time, mean state backup time
      std::cout << "\r" << std::setprecision(5) << elapsed_ms/1000.0 << std::setw(owidth) << fixed << m_nbackups << std::setw(owidth) << fixed << m_vf_nbackups
		<< std::setw(owidth) << fixed << elapsed_bu_ms / 1000.0 << std::setw(owidth) << fixed << m_mean_backup_time / static_cast<double>(m_nbackups) << std::setw(10) << m_leaves;
    }
      
  //debug
  /*std::cout << "[Debug]:backup done for state:\n";
    hst->print (std::cout);*/
  //debug
}

void HmdpEngine::ValueIteration(HmdpState *initState,
				const double &gamma,
				const double &epsilon,
				const int &T,
				const int &max_dfs_recur)
{
  int i = 0;

  // fillup the set of all states with dfs.
  HmdpEngine::DepthFirstSearchBackupCSD(initState,false,false,max_dfs_recur);
  
  double residual = std::numeric_limits<double>::min();
  while(i == 0 || residual > epsilon)
    {
      //debug
      /*std::string state_filename_dat = "vi_iter_" + std::to_string(i) + ".dat";
      BspTree::m_plotPointFormat = GnuplotF;
      ofstream output_state0_values_gp (state_filename_dat.c_str(), ios::out);
      double steps[2] = {1.0,1.0};
      hst->getVF ()->plotNDPointValues (output_state0_values_gp, steps,
					HmdpWorld::getRscLowBounds (),
					HmdpWorld::getRscHighBounds ());
					output_state0_values_gp.close();*/
      //debug

      /*std::unordered_map<HmdpState*,std::unordered_map<int,std::vector<HmdpState*> > >::iterator mit
	= HmdpEngine::m_nextStates.begin();
	while(mit!=HmdpEngine::m_nextStates.end())*/
      std::unordered_map<unsigned int,HmdpState*>::iterator mit = HmdpEngine::m_states.begin();
      while(mit!=HmdpEngine::m_states.end())
	{
	  HmdpState *hst = (*mit).second;
	  residual = std::numeric_limits<double>::min();
	  HmdpEngine::BspBackup(hst,residual,true,gamma);
	  ++mit;
	}
      double expectation = initState->getVF()->computeExpectation(initState->getCSD(),
								  HmdpWorld::getRscLowBounds (),
								  HmdpWorld::getRscHighBounds ());
      std::cerr << "iteration #" << i << " -- residual: " << residual << " -- expected: " << expectation << std::endl;
      ++i;
      if (T > 0 && i >= T)
	break;
    }
}

void HmdpEngine::BspBackup (HmdpState *hst,
			    double &residual,
			    const bool &with_residual,
			    const double &gamma)
{ 
  ValueFunction *maxActionVF 
    = new PiecewiseConstantValueFunction (static_cast<int> (HmdpWorld::getNResources ()),
					  HmdpWorld::getRscLowBounds (), 
					  HmdpWorld::getRscHighBounds (), 0.0);
  bool firstaction = true;

  //debug
  /* std::cout << "[Debug]:HmdpEngine::BspBackup: state:\n";
     hst->print (std::cout); */
  //debug

  /* iterate all actions in the world */
  std::map<size_t, HybridTransition*>::const_iterator ai;
  for (ai = HmdpWorld::actionsBegin (); ai != HmdpWorld::actionsEnd (); ai++)
    {
      if (HmdpWorld::isActionEnabled ((*ai).first, *hst))
	{
	  //debug
	  /* std::cout << "[Debug]:HmdpEngine::BspBackup: action enabled: "
	     << HmdpPpddlLoader::getAction ((*ai).first).name () << std::endl; */
	  /* (*ai).second->print (std::cout,
	     HmdpWorld::getRscLowBounds (), 
	     HmdpWorld::getRscHighBounds ()); */
	  //debug
	  
	  HybridTransition *ht = (*ai).second;
	  ValueFunction **discStatesVF = new ValueFunction*[ht->getNOutcomes ()];
	  for (int i=0; i<ht->getNOutcomes (); i++)  /* fill up array */
	    {
	      HmdpState *nextState =  HmdpEngine::getNextState (hst, ht->getActionIndex (), i);
	      if (gamma == 1.0)
		discStatesVF[i] = nextState->getVF ();
	      else
		{
		  discStatesVF[i] = static_cast<ValueFunction*>(BspTreeOperations::copyTree(nextState->getVF()));
		  discStatesVF[i]->multiplyByScalar(gamma);
		}
	      HybridTransitionOutcome *hto = ht->getOutcome (i);
	      ContinuousReward *actionGoalR = HmdpEngine::computeRewardFromGoals (hto, nextState);
	      if (actionGoalR)
		{
		  ValueFunction *vfr = BackupOperations::intersectVFWithReward (discStatesVF[i], actionGoalR,
										HmdpWorld::getRscLowBounds (),
										HmdpWorld::getRscHighBounds ());
		  BspTree::deleteBspTree (actionGoalR);
		  discStatesVF[i] = vfr;
		}
	    }
	  
	  /* back it up and get q-value for this action */
	  ValueFunction *htVF = BackupOperations::backUp (*ht, discStatesVF, 
							  HmdpWorld::getRscLowBounds (), 
							  HmdpWorld::getRscHighBounds ());
	  if (gamma != 1.0)
	    {
	      for (int i=0; i<ht->getNOutcomes (); i++)  /* fill up array */
		BspTree::deleteBspTree(discStatesVF[i]);
	    }
	  delete []discStatesVF;
	  m_vf_nbackups++;
	  
	  //debug
	  /* std::cout << "[Debug]: htVF:\n";
	     htVF->print (std::cout, HmdpWorld::getRscLowBounds (), 
	     HmdpWorld::getRscHighBounds ()); */
	  //debug
	  
	  /* intersect q-values: max over the actions */
 	  if (firstaction)
	    {
	      BspTree::deleteBspTree(maxActionVF);
	      maxActionVF = htVF;
	      firstaction = false;
	    }
	  else
	    {
	      ValueFunction *tempVF
		= ValueFunctionOperations::maxValueFunction (htVF, maxActionVF,
							     HmdpWorld::getRscLowBounds (),
							     HmdpWorld::getRscHighBounds ());
	      //debug
	      /* std::cout << "[Debug]: temVF (max):\n";
		 tempVF->print (std::cout, HmdpWorld::getRscLowBounds (), 
		 HmdpWorld::getRscHighBounds ()); */
	      //debug

	      BspTree::deleteBspTree (htVF); BspTree::deleteBspTree (maxActionVF);
	      maxActionVF = tempVF;
	    }
	  
	}  /* end if enabled */
    }  /* end loop over actions */
  
  /* set hybrid state vf */
  //debug
  /*std::cout << "[Debug]::HmdpEngine::BspBackup: maxActionVF:\n";
  maxActionVF->print (std::cout, HmdpWorld::getRscLowBounds (),
  HmdpWorld::getRscHighBounds ());*/
  //std::cout << "set it to state: " << hst->getStateIndex () << std::endl;
  //debug

  if (with_residual)
    {
      ValueFunction *rVF = ValueFunctionOperations::subtractValueFunctions(hst->getVF(),maxActionVF,
									   HmdpWorld::getRscLowBounds (),
									   HmdpWorld::getRscHighBounds ());

      /*std::cout << "[Debug]::HmdpEngine::BspBackup: rVF:\n";
      rVF->print (std::cout, HmdpWorld::getRscLowBounds (),
      HmdpWorld::getRscHighBounds ());*/
      
      rVF->maxAbsValue(residual,
		       HmdpWorld::getRscLowBounds (),
		       HmdpWorld::getRscHighBounds ());
      //std::cout << "residual: " << residual << std::endl;
      BspTree::deleteBspTree(rVF);
    }
  
  hst->setVF (maxActionVF);
}

HmdpState* HmdpEngine::hasBeenVisited (const HmdpState *hst)
{
  std::unordered_map<unsigned int,HmdpState*>::const_iterator nsi;
  
    //std::unordered_map<unsigned int, std::unordered_map<short, std::vector<HmdpState*> > >::const_iterator nsi;
  /*for (nsi = HmdpEngine::m_nextStates.begin (); 
       nsi != HmdpEngine::m_nextStates.end (); nsi++)
    {
      if ((*nsi).first->isEqual (hst))
	return (*nsi).first;
	}*/
  if ((nsi = HmdpEngine::m_states.find(hst->to_uint()))!=HmdpEngine::m_states.end())
    {
      return (*nsi).second;
    }
  return NULL;
}

ContinuousReward* HmdpEngine::computeRewardFromGoals (HybridTransitionOutcome *hto,
							  HmdpState *nextState)
{
  ValueFunction *stateVF = nextState->getVF ();
  
  /* test goals and return total reward for this outcome (sum of achieved goal). */
  std::set<int> *achievedGoals = new std::set<int> ();
  stateVF->collectAchievedGoals (achievedGoals);
  
  //debug
  /* std::cout << "[Debug]:HmdpEngine::computeRewardFromGoals: checking for goals in state:\n";
     nextState->print (std::cout); */
  //debug
  
  /* Warning: for now, we control goal achievements
     (sink goals) at discrete state level and not
     at bsp tree leaf level. This can lead to 
     under-valued value functions and sub-optimal 
     policies ! TODO... */
  ContinuousReward *totalStateReward = HmdpWorld::sumGoalReward (*nextState, 
								 achievedGoals); 
  
  delete achievedGoals;
  
  if (totalStateReward)
    totalStateReward->multiplyByScalar (hto->getOutcomeProbability ());

  return totalStateReward;
}

ContinuousReward* HmdpEngine::computeRewardFromGoals (HmdpState *hst, HybridTransition *ht)
{
  /* iterate action outcomes */
  std::vector<ContinuousReward*> outcomesR;
  for (int i=0; i<ht->getNOutcomes (); i++)
    {
      HybridTransitionOutcome *hto = ht->getOutcome (i);

      HmdpState *nextState = HmdpEngine::getNextState (hst, ht->getActionIndex (), i);
      
      ContinuousReward *totalStateReward
	= HmdpEngine::computeRewardFromGoals (hto, nextState); 
      
      if (totalStateReward)
	outcomesR.push_back (totalStateReward);
    }
  
  /* if no goal achieved in that state. */
  if (! outcomesR.size ())
    return NULL;
  
  /* average total reward over probabilistic discrete outcomes.
     TODO: put in within the previous loop. */
  ContinuousReward *finalReward = outcomesR[0];
  BspTreeOperations::setIntersectionType (BTI_PLUS);
  for (unsigned int i=1; i<outcomesR.size (); i++)
    {
      ContinuousReward *sumR 
	= dynamic_cast<ContinuousReward*> (BspTreeOperations::intersectTrees (finalReward, outcomesR[i],
									      HmdpWorld::getRscLowBounds (),
									      HmdpWorld::getRscHighBounds ()));
      
      if (BspTreeOperations::m_piecesMerging)
	sumR->mergeTreeLeaves ();
      BspTree::deleteBspTree (finalReward);
      finalReward = sumR;
      BspTree::deleteBspTree (outcomesR[i]);
    }
  if (finalReward != outcomesR[0])
    BspTree::deleteBspTree (outcomesR[0]);
  return finalReward;
}

void HmdpEngine::addNextState (HmdpState *hst, const short &action, HmdpState *nextState)
{
  HmdpEngine::m_nextStates[hst->to_uint()][action].push_back(nextState);
}

HmdpState* HmdpEngine::getNextState (HmdpState *hst, const short &action, const size_t &pos)
{
  return HmdpEngine::m_nextStates[hst->to_uint()][action][pos];
}

} /* end of namespace */
