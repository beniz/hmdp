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

#include "ValueFunction.h"
#include "ContinuousOutcome.h"
#include "ContinuousReward.h"
#include "ContinuousStateDistribution.h"
#include <algorithm> /* max */
#include <assert.h>

namespace hmdp_base
{

ValueFunction::ValueFunction (const int &sdim)
  : BspTreeAlpha (sdim), m_achievedGoals (0), m_csd (false)
{ 
  m_bspType = ValueFunctionT;
}

ValueFunction::ValueFunction (const int &sdim, const int &d, const double &pos)
  : BspTreeAlpha (sdim, d, pos), m_achievedGoals (0), m_csd (false)
{
  m_bspType = ValueFunctionT;
}

ValueFunction::ValueFunction (const ValueFunction &vf)
  : BspTreeAlpha (vf.getSpaceDimension (), vf.getDimension (), vf.getPosition ()),
    m_achievedGoals (0), m_csd (vf.getCSDFlag ())
{
  m_bspType = ValueFunctionT;
  m_maxValue = vf.getSubTreeMaxValue ();
  
  /* copy alpha vectors */
  if (vf.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> ();
      for (unsigned int i=0; i<vf.getAlphaVectorsSize (); i++)
	m_alphaVectors->push_back (new AlphaVector (*vf.getAlphaVectorNth (i)));
    }

  if (vf.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*vf.getAchievedGoals ());
    }

  if (! vf.isLeaf ())
    {
      ValueFunction *vlt = static_cast<ValueFunction*> (vf.getLowerTree ());
      m_lt = new ValueFunction (*vlt);
      ValueFunction *vge = static_cast<ValueFunction*> (vf.getGreaterTree ());
      m_ge = new ValueFunction (*vge);
    }
  else m_lt = m_ge = 0;
}

ValueFunction::~ValueFunction ()
{
  if (m_achievedGoals)
    delete m_achievedGoals;
  m_achievedGoals = 0;
}

void ValueFunction::transferData (const BspTree &bt)
{
  /* delete current goal set if any */
  if (m_achievedGoals)
    {
      delete m_achievedGoals;
      m_achievedGoals = 0;
    }
  
   /* delete current alpha vectors if any */
  if (m_alphaVectors) 
    {
      for (unsigned int i=0; i<m_alphaVectors->size (); i++)
	delete (*m_alphaVectors)[i];
      m_alphaVectors = 0;
    }

  if (bt.getType () == ContinuousOutcomeT)  /* we need a dedicated transfer if the 
					       tree does not belong to that 'inherited branch'. */
    {
      const ContinuousOutcome &co = static_cast<const ContinuousOutcome&> (bt);
      if (co.getProbability () > 0.0)
	{
	  m_alphaVectors = new std::vector<AlphaVector*> ();
	  m_alphaVectors->push_back (new AlphaVector (co.getProbability ()));
	}

      /* set max value. */
      m_maxValue = co.getProbability ();
    }
  else if (bt.getType () == PiecewiseConstantRewardT
	   || bt.getType () == PiecewiseLinearRewardT)
    {
      const ContinuousReward &cr = static_cast<const ContinuousReward&> (bt);
       
      /* copy vector of goals */
      if (cr.getAchievedGoals ())
	{
	  if (! m_achievedGoals)
	    m_achievedGoals = new std::vector<int> (*cr.getAchievedGoals ());
	}
      
      /* copy alpha vectors */
      if (cr.getAlphaVectors ())
	{
	  if (! m_alphaVectors)
	    m_alphaVectors = new std::vector<AlphaVector*> ();
	  for (unsigned int i=0; i<cr.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*cr.getAlphaVectorNth (i)));
	}

       /* copy max value. */
      m_maxValue = cr.getSubTreeMaxValue ();
    }
  else if (bt.getType () == ContinuousStateDistributionT)
    {
      const ContinuousStateDistribution &csd = static_cast<const ContinuousStateDistribution&> (bt); 
      if (csd.getProbability () > 0.0)
	{
	  m_csd = true;
	  m_alphaVectors = new std::vector<AlphaVector*> ();
	  m_alphaVectors->push_back (new AlphaVector (csd.getProbability ()));

	  /* set max value. */
	  m_maxValue = csd.getProbability ();
	}
    }
  else
    {
      const ValueFunction &vf = static_cast<const ValueFunction&> (bt);
      m_csd = vf.getCSDFlag ();

      /* copy vector of goals */
      if (vf.getAchievedGoals ())
	{
	  if (! m_achievedGoals)
	    m_achievedGoals = new std::vector<int> (*vf.getAchievedGoals ());
	}
      
      /* copy alpha vectors */
      if (vf.getAlphaVectors ())
	{
	  if (! m_alphaVectors)
	    m_alphaVectors = new std::vector<AlphaVector*> ();
	  for (unsigned int i=0; i<vf.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*vf.getAlphaVectorNth (i)));
	}

      /* copy max value. */
      m_maxValue = vf.getSubTreeMaxValue ();
    }
}

void ValueFunction::leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
					   double *low, double *high)
{
  const ValueFunction &vfa = static_cast<const ValueFunction&> (bt);
  const ValueFunction &vfb = static_cast<const ValueFunction&> (btr);
  
  /* delete current alpha vectors if any */
  if (m_alphaVectors) 
    {
      for (unsigned int i=0; i<m_alphaVectors->size (); i++)
	delete (*m_alphaVectors)[i];
      m_alphaVectors = 0;
    }

  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> ();
  
  if (vfa.getAlphaVectors () && vfb.getAlphaVectors ())
    {
      double prob = 1.0;
      if (vfa.getAlphaVectorsSize () == 1 && vfa.getCSDFlag ()) 
	{
	  for (int d=0; d<m_nDim; d++)
	    prob *= (high[d] - low[d]);
	  prob *= vfa.getAlphaVectorNth (0)->getAlphaNth (0);
	  for (unsigned int i=0; i<vfb.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*vfb.getAlphaVectorNth (i)));
	}
      else if (vfb.getAlphaVectorsSize () == 1 && vfb.getCSDFlag ())
	{
	  for (int d=0; d<m_nDim; d++)
	    prob *= (high[d] - low[d]);
	  prob *= vfb.getAlphaVectorNth (0)->getAlphaNth (0);
	  for (unsigned int i=0; i<vfa.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*vfa.getAlphaVectorNth (i)));
	}
      else if (vfa.getAlphaVectorsSize () == 1) 
	{
	  prob *= vfa.getAlphaVectorNth (0)->getAlphaNth (0);
	  for (unsigned int i=0; i<vfb.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*vfb.getAlphaVectorNth (i)));
	}
      else if (vfb.getAlphaVectorsSize () == 1)
	{
	  prob *= vfb.getAlphaVectorNth (0)->getAlphaNth (0);
	  for (unsigned int i=0; i<vfa.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*vfa.getAlphaVectorNth (i)));
	}
      else 
	{
	  std::cerr << "[Error]:ValueFunction: leafDataIntersectMult: requires a probability. Exiting...\n";
	  exit (1);
	}
      multiplyByScalar (prob);  /* multiply (by continuous outcome probability). */
    }
   else { delete m_alphaVectors; m_alphaVectors = 0; }

  /* unionize goal sets */
  unionGoalSets (vfa, vfb);
}

void ValueFunction::mergeTreeLeaves (double *low, double *high)
{
  if (! isLeaf ())
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->mergeTreeLeaves (low, high);
      high[getDimension ()] = b;

      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->mergeTreeLeaves (low, high);
      low[getDimension ()] = b;
      
      /* find two contiguous rectangular cells */
      if (vflt->isLeaf () && vfge->isLeaf ())
	mergeContiguousLeaves (this, vflt, vfge, low, high); /* virtual call */

      /* update subtree max value. */
       if (BspTreeOperations::m_asymetricOperators)
	 m_maxValue = updateSubTreeMaxValue (vflt, vfge);
    }
  else return;
}

void ValueFunction::addGoals (const ValueFunction &vf)
{
  if (! m_achievedGoals)
    m_achievedGoals = new std::vector<int> ();
  std::vector<int> *ag = vf.getAchievedGoals ();
  for (unsigned int i=0; i<ag->size (); i++)
    if (! Alg::inVectorInt ((*ag)[i], *m_achievedGoals))
      m_achievedGoals->push_back ((*ag)[i]);
}

void ValueFunction::unionGoalSets (const ValueFunction &vf1, const ValueFunction &vf2)
{
  /* delete current alpha vectors if any */
  if (m_achievedGoals) 
    {
      delete m_achievedGoals;
      m_achievedGoals = 0;
    }
  /* copy vector of goals */
  if (vf1.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*vf1.getAchievedGoals ());
      if (vf2.getAchievedGoals ())
	addGoals (vf2); /* union of the goal sets */
    }
  else if (vf2.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*vf2.getAchievedGoals ());
    }
}

void ValueFunction::tagWithAction (const int &action)
{
  if (isLeaf ())
    addAction (action);
  else 
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->tagWithAction (action);
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->tagWithAction (action);
    }
}

void ValueFunction::clearActionTags ()
{
  if (isLeaf ())
    clearActions ();
  else 
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->clearActionTags ();
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->clearActionTags ();
    }
}

void ValueFunction::clearActions ()
{
  if (! m_alphaVectors)
    return;
  else
    {
      for (unsigned int i=0; i<m_alphaVectors->size (); i++)
	(*m_alphaVectors)[i]->clearActions ();
    }
}

void ValueFunction::removeAction (const int &action)
{
  if (isLeaf ())
    {
    if (m_alphaVectors)
      {
	for (unsigned int i=0; i<getAlphaVectorsSize (); i++)
	  getAlphaVectorNth (i)->removeAction (action);
      }
    }
  else
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->removeAction (action);
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->removeAction (action);
    }
}

void ValueFunction::tagWithGoal (const int &goalid)
{
  if (isLeaf ())
    addGoal (goalid);
  else 
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->tagWithGoal (goalid);
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->tagWithGoal (goalid);
    }
}

void ValueFunction::addAction (const int &action)
{
  if (! m_alphaVectors)
    return;
    //std::cout << "[Error]:ValueFunction::addAction: " << action
    //      << ": vf has no alpha vector !\n";
  else
    {
      for (unsigned int i=0; i<m_alphaVectors->size (); i++)
	(*m_alphaVectors)[i]->addAction (action);
    }
}

double ValueFunction::computeExpectation (const MDDiscreteDistribution &mdd,
					  double *low, double *high)
{
  /* convert distribution to a continuous state distribution */
  ContinuousStateDistribution *csd
    = ContinuousStateDistribution::convertMDDiscreteDistribution (mdd, low, high);
  double expectation = computeExpectation (csd, low, high);
  deleteBspTree (csd);
  return expectation;
}

double ValueFunction::computeExpectation (ContinuousStateDistribution *csd, 
					  double *low, double *high)
{
  /* proceed with intersection */
  BspTreeOperations::setIntersectionType (BTI_MULT);
  ValueFunction* vf 
    = static_cast<ValueFunction*> (BspTreeOperations::intersectTrees (this, csd, low, high));
  /* sum leaves values (virtual call) */
  double expect = 0.0;
  vf->expectedValueFromLeaves (&expect, low, high);
  BspTree::deleteBspTree (vf);
  return expect;
}

void ValueFunction::collectActions (std::set<int> *actionSet, double *low, double *high)
{
  if (isLeaf ())
    {
      /* don't collect actions that lead to zero value. */
      double leafVal = 0.0;
      expectedValueFromLeaves (&leafVal, low, high);
      if (leafVal > 0.0) 
	{
	  std::set<int> bta = bestTileActions ();  /* virtual call to bestTileAction () */
	  std::set<int>::const_iterator ait;
	  for (ait = bta.begin (); ait != bta.end (); ait++)
	    actionSet->insert ((*ait));
	}
    }
  else 
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->collectActions (actionSet, low, high);
      high[getDimension ()] = b;
      
      b = low[getDimension ()];
      low[getDimension()] = getPosition ();
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->collectActions (actionSet, low, high);
      low[getDimension ()] = b;
    }
}

void ValueFunction::collectActions (std::set<int> *actionSet, double *low, double *high,
				    double *min, double *max)
{
    if (isLeaf ())
    {
	/* don't collect actions that lead to zero value. */
	double leafVal = 0.0;
	expectedValueFromLeaves (&leafVal, low, high);
	if (leafVal > 0.0)
        {
	    std::set<int> bta = bestTileActions ();  /* virtual call to bestTileAction () */
	    std::set<int>::const_iterator ait;
	    for (ait = bta.begin (); ait != bta.end (); ait++)
		actionSet->insert ((*ait));
        }
    }
    else
    {
	double b = high[getDimension ()];
	if (getPosition () >= min[getDimension ()])
	{
	    high[getDimension ()] = getPosition ();
	    ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
	    vflt->collectActions (actionSet, low, high);
	    high[getDimension ()] = b;
	}

	b = low[getDimension ()];
	if (getPosition () <= max[getDimension ()])
	{
	    low[getDimension()] = getPosition ();
	    ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
	    vfge->collectActions (actionSet, low, high);
	    low[getDimension ()] = b;
	}
    }
}


void ValueFunction::collectAchievedGoals (std::set<int> *achievedGoalsSet)
{
  if (isLeaf ())
    {
      if (m_achievedGoals)
	for (size_t i=0; i<m_achievedGoals->size (); i++)
	  achievedGoalsSet->insert ((*m_achievedGoals)[i]);
    }
  else
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->collectAchievedGoals (achievedGoalsSet);
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->collectAchievedGoals (achievedGoalsSet);
    }
}

void ValueFunction::breakTiesOnActions (std::map<int, double> *table)
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
	  std::set<int> bta = bestTileActions ();
	  std::set<int>::const_iterator actit;
	  
	  /* get action with max attached value in the table. */
	  std::pair<int, double> max_action = std::make_pair<int, double> (-1, -1.0);
	  for (actit = bta.begin (); actit != bta.end (); actit++)
	    {
	      double oldmax = max_action.second;
	      max_action.second = std::max (oldmax, (*table)[(*actit)]);
	      if (oldmax != max_action.second)
		max_action.first = (*actit);
	    }

	  /* clear all actions, set max action as the only action in this leaf. */
	  for (unsigned int i=0; i<getAlphaVectorsSize (); i++)
	    {
	      AlphaVector *av = getAlphaVectorNth (i);
	      av->clearActions ();
	      if (max_action.first != -1)
		av->addAction (max_action.first);
	    }
	}
    }
  else
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->breakTiesOnActions (table);
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->breakTiesOnActions (table);
    }
}

/* stats */
void ValueFunction::maxNumberOfAchievedGoals (unsigned int &mnag)
{
  if (isLeaf ())
    {
      if (m_achievedGoals)
	{
	  if (m_achievedGoals->size () > mnag)
	    mnag = m_achievedGoals->size ();
	}
    }
  else
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->maxNumberOfAchievedGoals (mnag);
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->maxNumberOfAchievedGoals (mnag);
    }
}

void ValueFunction::maxValueAndGoals (std::vector<int> &mgls, double &mval, 
				      double *low, double *high)
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
	  double *pos = new double[m_nDim];
	  for (int d=0; d<m_nDim; d++)
	    pos[d] = (high[d] - low[d]) / 2.0;   /* for pwl vf, we take the value at the
						    center of the tile. */
	  double lval = 0.0;
	  if ((lval = getPointValueInLeaf (pos)) > mval
	      && m_achievedGoals)
	    {
	      mgls = std::vector<int> (*m_achievedGoals);
	      mval = lval;
	    }
	  delete []pos;
	}
    }
  else
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->maxValueAndGoals (mgls, mval, low, high);
      high[getDimension ()] = b;
      
      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->maxValueAndGoals (mgls, mval, low, high);
      low[getDimension ()] = b;
    }
}

void ValueFunction::maxAbsValue (double &mval, double *low, double *high)
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
	  double *pos = new double[m_nDim];
	  for (int d=0; d<m_nDim; d++)
	    pos[d] = (high[d] - low[d]) / 2.0;   /* for pwl vf, we take the value at the
						    center of the tile. */
	  double lval = 0.0;
	  if ((lval = fabs (getPointValueInLeaf (pos))) > mval)
	    {
	      mval = lval;
	    }
	  delete []pos;
	}
    }
  else
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ValueFunction *vflt = static_cast<ValueFunction*> (getLowerTree ());
      vflt->maxAbsValue (mval, low, high);
      high[getDimension ()] = b;
      
      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      ValueFunction *vfge = static_cast<ValueFunction*> (getGreaterTree ());
      vfge->maxAbsValue (mval, low, high);
      low[getDimension ()] = b;
    }
}

/* printing */
void ValueFunction::print (std::ostream &out, double *low, double *high) const
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
		out<<"Position "<<getPosition ()<<"\n";
	  out << "{";
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      out << "[" << low[i] << "," << high[i];
	      if (i == getSpaceDimension () - 1)
		out << "]}\n";
	      else out << "],";
	    }
	  
	  /* alpha vectors */
	  for (size_t i=0; i<m_alphaVectors->size (); i++)
	    out << *(*m_alphaVectors)[i] << std::endl;
	
	  /* achieved goals. */
	  if (m_achievedGoals)
	    {
	      out << " {";
	      for (size_t i=0; i<m_achievedGoals->size (); i++)
		out << (*m_achievedGoals)[i] << ",";
	      out << "}\n";
	    }
	  //std::cout << "max value leaf: " << getSubTreeMaxValue () << std::endl;
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<ValueFunction*> (getGreaterTree ())->print (out, low, high);
      low[getDimension ()] = b;
      
      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<ValueFunction*> (getLowerTree ())->print (out, low, high);
      high[getDimension ()] = b;

      //std::cout << "max value: " << getSubTreeMaxValue () << std::endl;
    }
}

} /* end of namespace */
