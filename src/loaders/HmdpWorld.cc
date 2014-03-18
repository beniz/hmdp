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

#include "HmdpWorld.h"
#ifdef HAVE_PPDDL
#include "HmdpPpddlLoader.h"
#endif
#include "domains.h"
#include "BspTreeOperations.h"

//#define DEBUG 1

namespace hmdp_loader
{

SourceType HmdpWorld::m_st = ST_PPDDL;  /* default is ppddl */
std::vector <std::pair<std::string, std::pair<double,double> > > HmdpWorld::m_boundedResources;
double* HmdpWorld::m_rscLow;
double* HmdpWorld::m_rscHigh;
std::map<size_t, HybridTransition*> HmdpWorld::m_actions;
std::map<std::string, ContinuousReward*> HmdpWorld::m_goals;
std::map<double, HmdpState*> HmdpWorld::m_initialStates;
double *HmdpWorld::m_maxInitialResource = NULL;
double *HmdpWorld::m_minInitialResource = NULL;
bool HmdpWorld::m_oneTimeReward = false;
  
void HmdpWorld::loadWorld (const char *filename)
{
#ifdef HAVE_PPDDL
  if (HmdpWorld::m_st == ST_PPDDL)
    {
      /* load file */
      if (! HmdpPpddlLoader::load_file (filename))
	exit (-1);
      
      /* set resources */
      HmdpWorld::m_boundedResources = *HmdpPpddlLoader::getCVariables ();
      m_rscLow = new double[HmdpWorld::m_boundedResources.size ()];
      m_rscHigh = new double[HmdpWorld::m_boundedResources.size ()];
      HmdpPpddlLoader::fillUpBounds (m_rscLow, m_rscHigh);
      
      /* consider the first problem only. */
      const Problem *problem = HmdpPpddlLoader::getFirstProblem ();

      /* convert actions (actions are automatically instantiated 
	 when parsing is complete). */
      const ActionList &al = problem->actions ();
      for (ActionList::const_iterator ai = al.begin ();
	   ai != al.end (); ai++)
	{
	  HybridTransition *ht = HmdpPpddlLoader::convertAction (*(*ai), 
								 HmdpWorld::m_boundedResources.size ());
	  m_actions[(*ai)->id ()] = ht;
	}
      
      /* convert goals to continuous reward */
      const GoalMap &gm = problem->getGoals ();
      for (std::map<std::string,const Goal*>::const_iterator gi = gm.begin ();
	   gi != gm.end (); gi++)
	{
	  //std::cout << "goal: " << (*gi).first << std::endl;
	  ContinuousReward *cr 
	    = HmdpPpddlLoader::convertGoal (*(*gi).second, 
					    HmdpWorld::m_boundedResources.size ());
	  if (BspTreeOperations::m_asymetricOperators)
	    cr->updateSubTreeMaxValue ();
	  HmdpWorld::m_goals[(*gi).first] = cr;
	}

      /* create initial states */
      HmdpWorld::createInitialStates ();      
    }
  else
#endif
    {
      /* if needed... */
    }
}

std::pair<std::string, std::pair<double, double> >& HmdpWorld::getResource (const std::string &rsc)
{
  for (size_t i=0; i<HmdpWorld::m_boundedResources.size (); i++)
    if (HmdpWorld::m_boundedResources[i].first == rsc)
      return HmdpWorld::m_boundedResources[i];
  std::cout << "[Error]:HmdpWorld::getResource: cant't find resource: " 
	    << rsc << ".Exiting.\n";
  exit (-1);
}

double HmdpWorld::getResourceLowBound (const std::string &rsc)
{
  return HmdpWorld::getResource (rsc).second.first;
}

double HmdpWorld::getResourceHighBound (const std::string &rsc)
{
  return HmdpWorld::getResource (rsc).second.second;
}

void HmdpWorld::createInitialStates ()
{
#ifdef HAVE_PPDDL
  if (HmdpWorld::m_st == ST_PPDDL)
    {
      /* consider the first problem only. */
      const Problem *problem = HmdpPpddlLoader::getFirstProblem ();  
      
      /* iterate probabilitic effects in problem, if any */
      /* TODO: combination of probabilistic effects if list size is > 1 */
      EffectList::const_iterator ei = problem->init_effects ().begin ();
      /* for (ei = problem->init_effects.begin ();
	 ei != problem->init_effects.end (); ei++)
	 { */
      if (problem->init_dists ().size ())  /* there are distributions over the initial resource state */
	{
	  HmdpState *hs = new HmdpState ();
	  DiscreteDistribution::m_positiveResourcesConsumptionTruncation = false;
	  HmdpPpddlLoader::createInitialState (hs->getContState (), hs->getDiscState ());
	  MDDiscreteDistribution *initMdd = HmdpPpddlLoader::buildInitialRscDistribution (problem->init_dists ());
	  DiscreteDistribution::m_positiveResourcesConsumptionTruncation = true;

#ifdef DEBUG
	  //debug
	  std::cout << "[Debug]:HmdpWorld::createInitialStates: initial distribution (mdd): "
		    << *initMdd << std::endl;
	  //debug
#endif
	  
	  ContinuousStateDistribution *initCsd 
	    = ContinuousStateDistribution::convertMDDiscreteDistribution (*initMdd,
									  HmdpWorld::getRscLowBounds (),
									  HmdpWorld::getRscHighBounds ());
	  initCsd->normalize (HmdpWorld::getRscLowBounds (),
			      HmdpWorld::getRscHighBounds ());

#ifdef DEBUG
	  //debug
	  std::cout << "[Debug]:HmdpWorld::createInitialStates: initial distribution (csd): ";
	  initCsd->print (std::cout, HmdpWorld::getRscLowBounds (),
			  HmdpWorld::getRscHighBounds ());
	  //debug
#endif
	  
	  /* set up max available resource. */
	  HmdpWorld::m_maxInitialResource = new double[HmdpWorld::getNResources ()];
	  HmdpWorld::m_minInitialResource = new double[HmdpWorld::getNResources ()];
	  for (size_t i=0; i<HmdpWorld::getNResources (); i++)
	    {
	      HmdpWorld::m_maxInitialResource[i] = initMdd->getMaxPosition (i);
	      HmdpWorld::m_minInitialResource[i] = initMdd->getMinPosition (i);
	      HmdpWorld::setFunctionValueInMap (i, hs->getContState (), initMdd->getMaxPosition (i));
	    }

	  delete initMdd;
	  hs->setCSD (initCsd);
	  HmdpWorld::m_initialStates[1.0] = hs;
	}
      else if ((*ei)->getType () != EF_PROB)
	{
	  std::cout << "[Warning]::HmdpWorld::createInitialStates: there is no initial distribution.\nBuilding a uniform distribution over the entire resource space.\nj";

	  HmdpState *hs = new HmdpState ();
	  HmdpPpddlLoader::createInitialState (hs->getContState (), hs->getDiscState ());
	  MDDiscreteDistribution *initMdd = HmdpPpddlLoader::buildInitialRscDistribution ();

#ifdef DEBUG
	  //debug
	  std::cout << "[Debug]:HmdpWorld::createInitialStates: initial distribution (mdd): "
		    << *initMdd << std::endl;
	  //debug
#endif
	  
	  ContinuousStateDistribution *initCsd 
	    = ContinuousStateDistribution::convertMDDiscreteDistribution (*initMdd,
									  HmdpWorld::getRscLowBounds (),
									  HmdpWorld::getRscHighBounds ());
#ifdef DEBUG
	  //debug
	  std::cout << "[Debug]:HmdpWorld::createInitialStates: initial distribution (csd): ";
	  initCsd->print (std::cout, HmdpWorld::getRscLowBounds (),
			  HmdpWorld::getRscHighBounds ());
	  //debug
#endif
	  
	  delete initMdd;
	  hs->setCSD (initCsd);
	  HmdpWorld::m_initialStates[1.0] = hs;
	}
      else
	{
	  const ProbabilisticEffect *peff = dynamic_cast<const ProbabilisticEffect*> ((*ei));
	  for (size_t i=0; i<peff->size (); i++)
	    {
	      double probState = peff->probability (i).double_value ();
	      HmdpState *hs = new HmdpState ();
	      HmdpPpddlLoader::createInitialState (hs->getContState (), hs->getDiscState ());
	      MDDiscreteDistribution *initMdd = HmdpPpddlLoader::buildInitialRscDistribution ();
	      ContinuousStateDistribution *initCsd 
		= ContinuousStateDistribution::convertMDDiscreteDistribution (*initMdd,
									      HmdpWorld::getRscLowBounds (),
									      HmdpWorld::getRscHighBounds ());
	      delete initMdd;
	      initCsd->multiplyByScalar (probState);
	      hs->setCSD (initCsd);
	      HmdpPpddlLoader::applyNonResourceEffectChanges (peff->effect (i), hs->getContState (), hs->getDiscState (), -1);  /* index not use in this call + beware... no effects on resources. */
	      HmdpWorld::m_initialStates[probState] = hs;
	    }
	}
      /*}*/
    }
  else
#endif
    {
      /* TODO if needed */
    }
}

bool HmdpWorld::isActionEnabled (const size_t &id, const HmdpState &hst)
{
#ifdef HAVE_PPDDL
  if (HmdpWorld::m_st == ST_PPDDL)
    {
      const Action &act = HmdpPpddlLoader::getAction (id);
      return act.enabled (hst.getDiscStateConst (), hst.getContStateConst ());
    }
  else
#endif
    {
      /* TODO if needed. */
      return true;
    }
}

bool HmdpWorld::isActionDiscreteEnabled (const size_t &id, const HmdpState &hst)
{
#ifdef HAVE_PPDDL
    if (HmdpWorld::m_st == ST_PPDDL)
    {
	const Action &act = HmdpPpddlLoader::getAction (id);
	ValueMap emptyValMap;
	return act.enabled (hst.getDiscStateConst (), emptyValMap);
    }
    else
#endif
    {
	/* TODO if needed. */
	return true;
    }
}

bool HmdpWorld::isGoalAchieved (const Goal &gl, const HmdpState &hst)
{
#ifdef HAVE_PPDDL
  if (HmdpWorld::m_st == ST_PPDDL)
    {
      const StateFormula *stf = gl.getGoalFormula ();
      return stf->holds (hst.getDiscStateConst (), hst.getContStateConst ());
    }
  else
#endif
    {
      /* TODO if needed. */
      return false;
    }
}

void HmdpWorld::applyNonResourceActionEffects (const size_t &id, HmdpState *hst, 
					       const int &probEfIndex)
{
#ifdef HAVE_PPDDL
  if (HmdpWorld::m_st == ST_PPDDL)
    {
      const Action &action = HmdpPpddlLoader::getAction (id);
      HmdpPpddlLoader::applyNonResourceEffectChanges (action.effect (), 
						      hst->getContState (), 
						      hst->getDiscState (), probEfIndex);
    }
  else
#endif
    {
      /* TODO if needed. */
    }
}

ContinuousReward* HmdpWorld::sumGoalReward (const HmdpState &hst,
					    const std::set<int> *alreadyAchieved)
{
  ContinuousReward *totalR = 0;

#ifdef HAVE_PPDDL
  if (HmdpWorld::m_st == ST_PPDDL)
    {
      std::map<std::string, ContinuousReward*>::const_iterator gi;
      for (gi=HmdpWorld::m_goals.begin (); gi != HmdpWorld::m_goals.end (); gi++)
	{
	  const Goal &gl = HmdpPpddlLoader::getGoal ((*gi).first);
	  
	  /* if goals states are sink states */
	  if (m_oneTimeReward && alreadyAchieved->size ())  /* Warning: this is a goal check at discrete state level. */
	    {
	      if (alreadyAchieved->find (gl.getId ()) != alreadyAchieved->end ())
		{
		  //debug
		  /* std::cout << "[Debug]:HmdpWorld::sumGoalReward: skipping goal: " 
			    << gl.getName () << std::endl; */
		  //debug
		  continue;  /* skip goal */
		}
	    }

	  if (HmdpWorld::isGoalAchieved (gl, hst))
	    {
	      //debug
	      /* std::cout << "[Debug]:HmdpWorld::sumGoalReward: goal achieved: " 
			<< gl.getName () << " in state:\n"; */
	      //const_cast<HmdpState&> (hst).print (std::cout);
	      //debug
	      
	      if (! totalR)
		{
		  totalR = static_cast<ContinuousReward*> (BspTreeOperations::copyTree ((*gi).second));
		  
		  //debug
		  /* std::cout << "[Debug]:HmdpWorld::sumGoalReward: goal reward:\n";
		     totalR->print (std::cout, HmdpWorld::getRscLowBounds (),
		     HmdpWorld::getRscHighBounds ()); */
		  //debug
		}
	      else
		{
		  BspTreeOperations::setIntersectionType (BTI_PLUS);
		  BspTree *bt = BspTreeOperations::intersectTrees (totalR, (*gi).second,
								   HmdpWorld::getRscLowBounds (),
								   HmdpWorld::getRscHighBounds ());
		  BspTree::deleteBspTree (totalR);
		  totalR = static_cast<ContinuousReward*> (bt);
		  
		  //debug
		  /* std::cout << "[Debug]:HmdpWorld::sumGoalReward: goal reward 2:\n";
		     totalR->print (std::cout, HmdpWorld::getRscLowBounds (),
		     HmdpWorld::getRscHighBounds ()); */
		  //debug
		}
	    }
	}
    }
  else
#endif
    {
      /* TODO if needed. */
    }
  if (totalR && BspTreeOperations::m_asymetricOperators)
    totalR->updateSubTreeMaxValue ();
  return totalR;
}

std::string HmdpWorld::getActionName (const size_t &id)
{
  return HmdpPpddlLoader::getAction (id).name ();
}

void HmdpWorld::printActionCompleteName (std::ostream &os, const size_t &id)
{
  const Problem *problem = HmdpPpddlLoader::getFirstProblem ();
  const_cast<Action&> (HmdpPpddlLoader::getAction (id)).print_complete_name (os, problem->terms ());
}

HmdpState* HmdpWorld::getFirstInitialState ()
{
  return (*HmdpWorld::m_initialStates.begin ()).second;
}

ContinuousReward* HmdpWorld::findGoalReward (const std::string &gName)
{
  std::map<std::string, ContinuousReward*>::const_iterator git;
  if ((git = m_goals.find (gName)) != m_goals.end ())
    return (*git).second;
  else return NULL;  /* beware. */
}

/* printing */
void HmdpWorld::printResources (std::ostream &out)
{
  std::vector <std::pair <std::string, std::pair<double, double> > >::const_iterator ri;
  for (ri = HmdpWorld::m_boundedResources.begin ();
       ri != HmdpWorld::m_boundedResources.end (); ri++)
    {
      out << (*ri).first << " -- [" 
	  << (*ri).second.first << "," << (*ri).second.second << "]\n";
    }
}

void HmdpWorld::printInitialStates (std::ostream &out)
{
  if (! HmdpWorld::m_initialStates.size ())
    return;
  
  out << "--- Initial states: ---\n";
  std::map<double, HmdpState*>::const_iterator si;
  for (si = HmdpWorld::m_initialStates.begin (); si != HmdpWorld::m_initialStates.end (); si++)
    {
      out << "probability: " << (*si).first << std::endl;
      (*si).second->print (out);
    }
}

void HmdpWorld::printGoals (std::ostream &out)
{
  if (! HmdpWorld::m_goals.size ())
    return;
  
  out << "--- Goals: ---\n";
  std::map<std::string, ContinuousReward*>::const_iterator gi;
  for (gi = HmdpWorld::m_goals.begin (); gi != HmdpWorld::m_goals.end (); gi++)
    {
      out << "goal name: " << (*gi).first << std::endl;
      (*gi).second->print (out, HmdpWorld::getRscLowBounds (),
			   HmdpWorld::getRscHighBounds ());
    }
}

#ifdef HAVE_PPDDL
void HmdpWorld::printState (std::ostream &out, const ValueMap &values, const AtomSet &atoms)
{
  HmdpPpddlLoader::print (out, values, atoms);
}
#endif

void HmdpWorld::print (std::ostream &out)
{
  HmdpWorld::printResources (out);
  std::map<size_t, HybridTransition*>::const_iterator ait;
  for (ait = HmdpWorld::actionsBegin (); ait != HmdpWorld::actionsEnd (); ait++)
    {
      out << "{ Action id: " << (*ait).first;

#ifdef HAVE_PPDDL
      const Problem *problem = HmdpPpddlLoader::getFirstProblem ();
      if (m_st == ST_PPDDL)
	{
	  std::cout << std::endl;
	  const_cast<Action&> (HmdpPpddlLoader::getAction ((*ait).first)).print_complete_name (out, problem->terms ());
	}
	//out << " -- name: " << HmdpPpddlLoader::getAction ((*ait).first).name ();
      out << std::endl;
#endif
      
      (*ait).second->print (out, HmdpWorld::getRscLowBounds (), 
			    HmdpWorld::getRscHighBounds ());
      out << "}\n";
    }
  HmdpWorld::printInitialStates (out); 
  HmdpWorld::printGoals (out);
}

} /* end of namespace */
