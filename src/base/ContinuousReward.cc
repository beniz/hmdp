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

#include "ContinuousReward.h"
#include "ValueFunction.h"  /* for pieces merging... */

namespace hmdp_base
{

ContinuousReward::ContinuousReward (const int &sdim)
  : BspTreeAlpha (sdim), m_tilingDimension (0), m_achievedGoals (0)
{
  m_bspType = ContinuousRewardT;
}

ContinuousReward::ContinuousReward (const int &sdim, const int &d, const double &pos)
  : BspTreeAlpha (sdim, d, pos), m_tilingDimension (0), m_achievedGoals (0)
{
  m_bspType = ContinuousRewardT;
}

ContinuousReward::ContinuousReward (const int &dimension, const int &sdim)
  : BspTreeAlpha (sdim), m_tilingDimension (dimension), m_achievedGoals (0)
{
  m_bspType = ContinuousRewardT;
}

ContinuousReward::ContinuousReward (const int &dimension, const int &sdim, 
				    const int &d, const double &pos)
  : BspTreeAlpha (sdim, d, pos), m_tilingDimension (dimension), m_achievedGoals (0)
{
  m_bspType = ContinuousRewardT;
}


ContinuousReward::ContinuousReward (const ContinuousReward &cr)
  : BspTreeAlpha (cr.getSpaceDimension (), cr.getDimension (), cr.getPosition ()),
    m_tilingDimension (cr.getTilingDimension ()), m_achievedGoals (0)
{
  m_bspType = ContinuousRewardT;

  /* copy alpha vectors */
  if (cr.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> ();
      for (unsigned int i=0; i<cr.getAlphaVectorsSize (); i++)
	m_alphaVectors->push_back (new AlphaVector (*cr.getAlphaVectorNth (i)));
    }

  if (cr.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*cr.getAchievedGoals ());
    }

  if (! cr.isLeaf ())
    {
      ContinuousReward *blt = static_cast<ContinuousReward*> (cr.getLowerTree ());
      m_lt = new ContinuousReward (*blt);
      ContinuousReward *bge = static_cast<ContinuousReward*> (cr.getGreaterTree ());
      m_ge = new ContinuousReward (*bge);
    }
  else m_lt = m_ge = 0;
}

ContinuousReward::ContinuousReward (const int &dimension, const BspTree &bt)
  : BspTreeAlpha (bt.getSpaceDimension (), bt.getDimension (), bt.getPosition ()),
    m_tilingDimension (dimension), m_achievedGoals (0)
{
  m_bspType = ContinuousRewardT;

  if (! bt.isLeaf ())
    {
      m_lt = new ContinuousReward (0, *bt.getLowerTree ());
      m_ge = new ContinuousReward (0, *bt.getGreaterTree ());
    }
  else m_lt = m_ge = 0;

}

ContinuousReward::~ContinuousReward ()
{
  delete m_achievedGoals;
  m_achievedGoals = 0;
}

void ContinuousReward::transferData (const BspTree &bt)
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
  
  if (bt.getType () == ValueFunctionT || bt.getType () == PiecewiseConstantVFT
      || bt.getType () == PiecewiseLinearVFT)  /* we need a dedicated transfer if the 
						   tree does not belong to that 'inherited branch'. */
    {
      const ValueFunction &vf = static_cast<const ValueFunction&> (bt);
      
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

      /* set max value. */
      m_maxValue = vf.getSubTreeMaxValue ();
    }
  else
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
      
      m_maxValue = cr.getSubTreeMaxValue ();
    }
}

void ContinuousReward::mergeTreeLeaves ()
{
  if (! isLeaf ())
    {
      ContinuousReward *crlt = static_cast<ContinuousReward*> (getLowerTree ());
      crlt->mergeTreeLeaves ();
      ContinuousReward *crge = static_cast<ContinuousReward*> (getGreaterTree ());
      crge->mergeTreeLeaves ();
      
      /* find two contiguous rectangular cells */
      if (crlt->isLeaf () && crge->isLeaf ())
	mergeContiguousLeaves (this, crlt, crge); /* virtual call */
    }
}

void ContinuousReward::addGoals (const ContinuousReward &cr)
{
  std::vector<int> *ag = cr.getAchievedGoals ();
  for (unsigned int i=0; i<ag->size (); i++)
    if (! Alg::inVectorInt ((*ag)[i], *m_achievedGoals))
      m_achievedGoals->push_back ((*ag)[i]);
}

void ContinuousReward::unionGoalSets (const ContinuousReward &cr1, const ContinuousReward &cr2)
{
  /* delete current alpha vectors if any */
  if (m_achievedGoals) 
    {
      delete m_achievedGoals;
      m_achievedGoals = 0;
    }
  /* copy vector of goals */
  if (cr1.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*cr1.getAchievedGoals ());
      if (cr2.getAchievedGoals ())
	addGoals (cr2); /* union of the goal sets */
    }
  else if (cr2.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*cr2.getAchievedGoals ());
      /* addGoals (cr1); */  /* union of the goal sets */
    }
}

void ContinuousReward::untagGoals()
{
  if (isLeaf() && m_achievedGoals)
    m_achievedGoals->clear();
  else
    {
      ContinuousReward *crlt = static_cast<ContinuousReward*> (getLowerTree ());
      crlt->untagGoals();
      ContinuousReward *crge = static_cast<ContinuousReward*> (getGreaterTree ());
      crge->untagGoals();
    }
}

void ContinuousReward::tagWithGoal (const int &g)
{
  if (isLeaf ())
    {
      addGoal (g);
    }
  else
    {
      ContinuousReward *crlt = static_cast<ContinuousReward*> (getLowerTree ());
      crlt->tagWithGoal (g);
      ContinuousReward *crge = static_cast<ContinuousReward*> (getGreaterTree ());
      crge->tagWithGoal (g);
    }
}

void ContinuousReward::collectAchievedGoals (std::set<int> *achievedGoalsSet)
{
  if (isLeaf ())
    {
      if (m_achievedGoals)
	for (size_t i=0; i<m_achievedGoals->size (); i++)
	  achievedGoalsSet->insert ((*m_achievedGoals)[i]);
    }
  else
    {
      ContinuousReward *crlt = static_cast<ContinuousReward*> (getLowerTree ());
      crlt->collectAchievedGoals (achievedGoalsSet);
      ContinuousReward *crge = static_cast<ContinuousReward*> (getGreaterTree ());
      crge->collectAchievedGoals (achievedGoalsSet);
    }
}

void ContinuousReward::deleteAchievedGoals ()
{
  if (isLeaf ())
    {
      if (m_achievedGoals)
	{
	  delete m_achievedGoals;
	  m_achievedGoals = 0;
	}
    }
  else
    {
      ContinuousReward *crlt = static_cast<ContinuousReward*> (getLowerTree ());
      crlt->deleteAchievedGoals ();
      ContinuousReward *crge = static_cast<ContinuousReward*> (getGreaterTree ());
      crge->deleteAchievedGoals ();
    }
}

void ContinuousReward::getMaxValue (double &mval, double *low, double *high)
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
	    if ((lval = getPointValueInLeaf (pos)) > mval)
		mval = lval;
	    delete []pos;
	}
    }
    else
    {
	double b = high[getDimension ()];
	high[getDimension ()] = getPosition ();
	ContinuousReward *crlt = static_cast<ContinuousReward*> (getLowerTree ());
	crlt->getMaxValue (mval, low, high);
	high[getDimension ()] = b;

	b = low[getDimension ()];
	low[getDimension ()] = getPosition ();
	ContinuousReward *crge = static_cast<ContinuousReward*> (getGreaterTree ());
	crge->getMaxValue (mval, low, high);
	low[getDimension ()] = b;
    }
}

} /* end of namespace */
