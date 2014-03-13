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

#include "PiecewiseConstantValueFunction.h"
#include "ContinuousTransition.h"
#include "BspTreeOperations.h"
#include <assert.h>

namespace hmdp_base
{

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const int &sdim)
  : ValueFunction (sdim)
{
  m_bspType = PiecewiseConstantVFT;
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const int &sdim, const int &d, 
								const double &pos)
  : ValueFunction (sdim, d, pos)
{
  m_bspType = PiecewiseConstantVFT;
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const int &sdim, const double *low, 
								const double *high, const double &val)
  : ValueFunction (sdim)
{
  m_bspType = PiecewiseConstantVFT;
  PiecewiseConstantValueFunction *vf_n = this;

  for (int i=0; i<m_nDim; i++)
    {
      /* upper bound */
      vf_n->setDimension (i);
      vf_n->setPosition (high[i]);
      vf_n->setLowerTree (new PiecewiseConstantValueFunction (m_nDim, i, low[i]));
      vf_n->setGreaterTree (new PiecewiseConstantValueFunction (m_nDim));
      vf_n = static_cast<PiecewiseConstantValueFunction*> (vf_n->getLowerTree ());
    
      /* lower bound */
      vf_n->setLowerTree (new PiecewiseConstantValueFunction (m_nDim));
      vf_n->setGreaterTree (new PiecewiseConstantValueFunction (m_nDim));
      vf_n = static_cast<PiecewiseConstantValueFunction*> (vf_n->getGreaterTree ());
    }
  
  /* set vf initial value to zero */
  AlphaVector *av = new AlphaVector (val);
  vf_n->addAVector (av);
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const int &sdim, 
								const double *low, const double *high)
  : ValueFunction (sdim)
{
  m_bspType = PiecewiseConstantVFT;
  PiecewiseConstantValueFunction *vf_n = this;

  for (int i=0; i<m_nDim; i++)
    {
      /* upper bound */
      vf_n->setDimension (i);
      vf_n->setPosition (high[i]);
      vf_n->setLowerTree (new PiecewiseConstantValueFunction (m_nDim, i, low[i]));
      vf_n->setGreaterTree (new PiecewiseConstantValueFunction (m_nDim));
      vf_n = static_cast<PiecewiseConstantValueFunction*> (vf_n->getLowerTree ());
    
      /* lower bound */
      vf_n->setLowerTree (new PiecewiseConstantValueFunction (m_nDim));
      vf_n->setGreaterTree (new PiecewiseConstantValueFunction (m_nDim));
      vf_n = static_cast<PiecewiseConstantValueFunction*> (vf_n->getGreaterTree ());
    }
  
  /* set vf initial value to zero */
  AlphaVector *av = new AlphaVector (0.0);
  vf_n->addAVector (av);
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const PiecewiseConstantValueFunction &pcvf)
  : ValueFunction (pcvf.getSpaceDimension (), pcvf.getDimension (), pcvf.getPosition ())
{
  m_bspType = PiecewiseConstantVFT;
  m_csd = pcvf.getCSDFlag ();
  m_maxValue = pcvf.getSubTreeMaxValue ();

  /* copy alpha vectors */
  if (pcvf.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      (*m_alphaVectors)[0] = new AlphaVector (*pcvf.getAlphaVectorNth (0));
    }

  if (pcvf.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*pcvf.getAchievedGoals ());
    }
  
  if (! pcvf.isLeaf ())
    {
      PiecewiseConstantValueFunction *vlt = static_cast<PiecewiseConstantValueFunction*> (pcvf.getLowerTree ());
      m_lt = new PiecewiseConstantValueFunction (*vlt);
      PiecewiseConstantValueFunction *vge = static_cast<PiecewiseConstantValueFunction*> (pcvf.getGreaterTree ());
      m_ge = new PiecewiseConstantValueFunction (*vge);
    }
  else m_lt = m_ge = 0;
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const ContinuousReward &cr)
  : ValueFunction (cr.getSpaceDimension (), cr.getDimension (), cr.getPosition ())
{
  m_bspType = PiecewiseConstantVFT;
  m_maxValue = cr.getSubTreeMaxValue ();

  /* set max value. */
  if (cr.getType () == PiecewiseConstantRewardT)
    m_maxValue = cr.getSubTreeMaxValue ();

  /* copy alpha vectors. */
  if (cr.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      (*m_alphaVectors)[0] = new AlphaVector (*cr.getAlphaVectorNth (0));
    }

  if (cr.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*cr.getAchievedGoals ()); /* ... verify... */
    }

  if (! cr.isLeaf ())
    {
      ContinuousReward *crlt = static_cast<ContinuousReward*> (cr.getLowerTree ());
      m_lt = new PiecewiseConstantValueFunction (*crlt);
      ContinuousReward *crge = static_cast<ContinuousReward*> (cr.getGreaterTree ());
      m_ge = new PiecewiseConstantValueFunction (*crge);
    }
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const ContinuousOutcome &co)
  : ValueFunction (co.getSpaceDimension (), co.getDimension (), co.getPosition ())
{
  m_bspType = PiecewiseConstantVFT;
  
  /* create an alpha vector from the continuous outcome probability */
  if (co.getProbability () > 0.0)
    {
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      (*m_alphaVectors)[0] = new AlphaVector (co.getProbability ());
    }
  
  if (! co.isLeaf ())
    {
      ContinuousOutcome *colt = static_cast<ContinuousOutcome*> (co.getLowerTree ());
      m_lt = new PiecewiseConstantValueFunction (*colt);
      ContinuousOutcome *coge = static_cast<ContinuousOutcome*> (co.getGreaterTree ());
      m_ge = new PiecewiseConstantValueFunction (*coge);
    }
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const ContinuousTransition &ct,
								const double &tileval)
  : ValueFunction (ct.getSpaceDimension (), ct.getDimension (), ct.getPosition ())
{
  m_bspType = PiecewiseConstantVFT;

  /* create an alpha vector with zero value */
  if (ct.isLeaf ())
    {
      if (ct.getNTile () != -1)
	{
	  m_alphaVectors = new std::vector<AlphaVector*> (1);
	  (*m_alphaVectors)[0] = new AlphaVector (tileval);
	}
    }
  else 
    {
      ContinuousTransition *ctlt = static_cast<ContinuousTransition*> (ct.getLowerTree ());
      m_lt = new PiecewiseConstantValueFunction (*ctlt, tileval);
      ContinuousTransition *ctge = static_cast<ContinuousTransition*> (ct.getGreaterTree ());
      m_ge = new PiecewiseConstantValueFunction (*ctge, tileval);
    }
}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (const ContinuousStateDistribution &csd)
  : ValueFunction (csd.getSpaceDimension (), csd.getDimension (), csd.getPosition ())
{
  m_bspType = PiecewiseConstantVFT;

  /* create an alpha vector from the continuous state distribution 'leaf' probability. */
  if (csd.getProbability () > 0.0)
    {
      m_csd = true;
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      (*m_alphaVectors)[0] = new AlphaVector (csd.getProbability ());
    }

  if (! csd.isLeaf ())
    {
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (csd.getLowerTree ());
      m_lt = new PiecewiseConstantValueFunction (*csdlt);
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (csd.getGreaterTree ());
      m_ge = new PiecewiseConstantValueFunction (*csdge);
    }
}

PiecewiseConstantValueFunction::~PiecewiseConstantValueFunction ()
{}

PiecewiseConstantValueFunction::PiecewiseConstantValueFunction (PiecewiseConstantValueFunction &pwlactions,
								const int &action,
								const bool &prop)
  : ValueFunction (pwlactions.getSpaceDimension (), pwlactions.getDimension (), pwlactions.getPosition ())
{
  m_bspType = PiecewiseConstantVFT;
  
  if (pwlactions.isLeaf ())
    {
      if (pwlactions.getAlphaVectors ())
	{
	  /* create leaf with value of 1 if optimal policy is 'action', 0 otherwise. */
	  std::set<int> actions = pwlactions.bestTileActions ();
	  double value = 0.0;
	  std::set<int>::const_iterator actit;
	  for (actit = actions.begin (); actit != actions.end (); actit++)
	    if ((*actit) == action)
	      {
		if (! prop)
		  value = 1.0;
		else value = 1.0 / static_cast<double> (actions.size ());
		break;
	      }
	  m_alphaVectors = new std::vector<AlphaVector*> (1);
	  (*m_alphaVectors)[0] = new AlphaVector (value);
	}
    }
  else
    {
      PiecewiseConstantValueFunction *pwllt = static_cast<PiecewiseConstantValueFunction*> (pwlactions.getLowerTree ());
      m_lt = new PiecewiseConstantValueFunction (*pwllt, action, prop);
      PiecewiseConstantValueFunction *pwlge = static_cast<PiecewiseConstantValueFunction*> (pwlactions.getGreaterTree ());
      m_ge = new PiecewiseConstantValueFunction (*pwlge, action, prop);
    }
}

void PiecewiseConstantValueFunction::leafDataIntersectMax (const BspTree &bt, const BspTree &btr,
							   double *low, double *high)
{
  const PiecewiseConstantValueFunction &pcvfa = static_cast<const PiecewiseConstantValueFunction&> (bt);
  const PiecewiseConstantValueFunction &pcvfb = static_cast<const PiecewiseConstantValueFunction&> (btr);
  
  
  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> (1);
  else 
    {
      delete (*m_alphaVectors)[0];
    }
  
  if (pcvfa.getAlphaVectors () && pcvfb.getAlphaVectors ())
    {
      AlphaVector::maxConstantAlphaVector (*pcvfa.getAlphaVectors (), 
					   *pcvfb.getAlphaVectors (), 
					   pcvfa.getAchievedGoals (),
					   pcvfb.getAchievedGoals (),
					   m_alphaVectors);
    }
  else if (pcvfa.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcvfa.getAlphaVectorNth (0));
  else if (pcvfb.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcvfb.getAlphaVectorNth (0));
  else { delete m_alphaVectors; m_alphaVectors = 0; }
  
  /* goals */
  if (m_achievedGoals) 
    {
      delete m_achievedGoals;
      m_achievedGoals = 0;
    }
  if ((pcvfa.getAchievedGoals () && pcvfb.getAchievedGoals ())
      && (pcvfa.getConstantValue () == pcvfb.getConstantValue ())
      && (pcvfa.getAchievedGoals ()->size () == pcvfb.getAchievedGoals ()->size ()))
    {
      //unionGoalSets (pcvfa, pcvfb);
      m_achievedGoals = new std::vector<int> (*pcvfb.getAchievedGoals ());
    }
  else 
    {
      if (pcvfa.getConstantValue () == getConstantValue ()
	  && pcvfa.getAchievedGoals ())
	m_achievedGoals = new std::vector<int> (*pcvfa.getAchievedGoals ());
      else if (pcvfb.getConstantValue () == getConstantValue ()
	       && pcvfb.getAchievedGoals ())
	m_achievedGoals = new std::vector<int> (*pcvfb.getAchievedGoals ());
    }
  
  if (m_alphaVectors && getConstantValue () == 0.0)
    (*m_alphaVectors)[0]->clearActions ();

  if (BspTreeOperations::m_asymetricOperators)
    m_maxValue = getConstantValue ();
}

void PiecewiseConstantValueFunction::leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
                                                           double *low, double *high)
{
  const PiecewiseConstantValueFunction &pcvfa = static_cast<const PiecewiseConstantValueFunction&> (bt);
  const PiecewiseConstantValueFunction &pcvfb = static_cast<const PiecewiseConstantValueFunction&> (btr);

  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> (1);
  else 
    {
      delete (*m_alphaVectors)[0];
    }
  
  if (pcvfa.getAlphaVectors () && pcvfb.getAlphaVectors ())
    AlphaVector::simpleSumAlphaVectors (*pcvfa.getAlphaVectors (), *pcvfb.getAlphaVectors (), m_alphaVectors);
  else if (pcvfa.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcvfa.getAlphaVectorNth (0));
  else if (pcvfb.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcvfb.getAlphaVectorNth (0));
  else { delete m_alphaVectors; m_alphaVectors = 0;}

  /* unionize goal sets */
  unionGoalSets (pcvfa, pcvfb);

  if (BspTreeOperations::m_asymetricOperators)
    m_maxValue = getConstantValue ();
}

void PiecewiseConstantValueFunction::leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
                                                           double *low, double *high)
{
  const PiecewiseConstantValueFunction &pcvfa = static_cast<const PiecewiseConstantValueFunction&> (bt);
  const PiecewiseConstantValueFunction &pcvfb = static_cast<const PiecewiseConstantValueFunction&> (btr);

  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> (1);
  else 
    {
      delete (*m_alphaVectors)[0];
    }
  
  if (pcvfa.getAlphaVectors () && pcvfb.getAlphaVectors ())
    AlphaVector::simpleSubtractAlphaVectors (*pcvfa.getAlphaVectors (), *pcvfb.getAlphaVectors (), m_alphaVectors);
  /* Warning: should be abs... because tree operations can screw up symmetry ! */
  else if (pcvfa.getAlphaVectors ())
    {
      AlphaVector *av = new AlphaVector (*pcvfa.getAlphaVectorNth (0));
      av->multiplyByScalar (-1.0);
      (*m_alphaVectors)[0] = av;
    }
  else if (pcvfb.getAlphaVectors ())
    {
      AlphaVector *av = new AlphaVector (*pcvfb.getAlphaVectorNth (0));
      av->multiplyByScalar (-1.0);
      (*m_alphaVectors)[0] = av;
    }
  else { delete m_alphaVectors; m_alphaVectors = 0;}

  /* unionize goal sets */
  unionGoalSets (pcvfa, pcvfb);

  if (BspTreeOperations::m_asymetricOperators)
    m_maxValue = getConstantValue ();
}

void PiecewiseConstantValueFunction::mergeContiguousLeaves (ValueFunction *root, ValueFunction *lt,
							    ValueFunction *ge, double *low, double *high)
{
  PiecewiseConstantValueFunction *pcvflt = static_cast<PiecewiseConstantValueFunction*> (lt);
  PiecewiseConstantValueFunction *pcvfge = static_cast<PiecewiseConstantValueFunction*> (ge);
  
  /* get constants */
  double c1 = pcvflt->getConstantValue ();
  double c2 = pcvfge->getConstantValue ();

  if ((BspTreeOperations::m_piecesMergingByValue
       && Alg::REqual (c1, c2, Alg::m_doubleEpsilon))     /* merge by value only. */
      || (BspTreeOperations::m_piecesMergingByAction
	  && AlphaVector::isEqualActionSets (pcvflt->bestTileActions (),
					     pcvfge->bestTileActions ()))
      || (BspTreeOperations::m_piecesMergingEquality
	  && Alg::REqual (c1, c2, Alg::m_doubleEpsilon)
	  && AlphaVector::isEqualActionSets (pcvflt->bestTileActions (),
					     pcvfge->bestTileActions ())))
	  /* && (pcvflt->getAchievedGoals () && pcvfge->getAchievedGoals ())
	  && (Alg::eqVectorInt (*pcvflt->getAchievedGoals (),     
	  *pcvfge->getAchievedGoals ())))) */   /* merge based on equality (value, action, goals) */
    {
      /* transfer data to root and delete leaves. */
      if (c1 < c2)
	{
	  transferData (*pcvflt);
	  m_maxValue = c1;
	}
      else
	{
	  transferData (*pcvfge);
	  m_maxValue = c2;
	}

      /* unionize goal sets */
      if (BspTreeOperations::m_piecesMergingByValue
	  || BspTreeOperations::m_piecesMergingByAction)
	{
	  unionGoalSets (*pcvflt, *pcvfge);
	}

      delete pcvflt; pcvflt = 0;
      delete pcvfge; pcvfge = 0;
      setLowerTree (0);
      setGreaterTree (0);
    }
}

void PiecewiseConstantValueFunction::expectedValueFromLeaves (double *val, 
							      double *low, double *high)
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
	  *val += getConstantValue ();
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension()] = getPosition ();
      PiecewiseConstantValueFunction *pcge 
	= static_cast<PiecewiseConstantValueFunction*> (getGreaterTree ());
      pcge->expectedValueFromLeaves (val, low, high);
      low[getDimension ()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      PiecewiseConstantValueFunction *pclt 
	= static_cast<PiecewiseConstantValueFunction*> (getLowerTree ());
      pclt->expectedValueFromLeaves (val, low, high);
      high[getDimension ()] = b;
    }
}

std::set<int> PiecewiseConstantValueFunction::bestTileActions ()
{
  if (m_alphaVectors)
    return getAlphaVectorNth (0)->m_actions;
  else return std::set<int> (); 
}

/* stuff for asymetric operators. Warning: this applies to the constant case only (for now). */
double PiecewiseConstantValueFunction::updateSubTreeMaxValue ()
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
	  m_maxValue = getConstantValue ();
	  return m_maxValue;
	}
      else return 0.0; /* beware. */
    }
  else
    {
      PiecewiseConstantValueFunction *vflt 
	= static_cast<PiecewiseConstantValueFunction*> (getLowerTree ());
      double low_max = vflt->updateSubTreeMaxValue ();
      PiecewiseConstantValueFunction *vfge 
	= static_cast<PiecewiseConstantValueFunction*> (getGreaterTree ());
      double high_max = vfge->updateSubTreeMaxValue ();
      m_maxValue = std::max (low_max, high_max);
      return m_maxValue;
    }
}

double PiecewiseConstantValueFunction::updateSubTreeMaxValue (ValueFunction *vflt,
							      ValueFunction *vfge)
{
  PiecewiseConstantValueFunction *pcvflt 
    = static_cast<PiecewiseConstantValueFunction*> (vflt);
  PiecewiseConstantValueFunction *pcvfge 
    = static_cast<PiecewiseConstantValueFunction*> (vfge);
  return std::max (pcvflt->getSubTreeMaxValue (), pcvfge->getSubTreeMaxValue ());
}

/* printing */
void PiecewiseConstantValueFunction::plot1DVF (std::ofstream &box,
					       double *low, double *high)
{
  if (isLeaf ())
    box << low[0] << ' ' << high[0] << ' ' << getConstantValue () << '\n';
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<PiecewiseConstantValueFunction*> (getGreaterTree ())->plot1DVF (box, low, high);
      low[getDimension ()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<PiecewiseConstantValueFunction*> (getLowerTree ())->plot1DVF (box, low, high);
      high[getDimension ()] = b;
    }
}

/* plot of a leaf uses a vrml box */
void PiecewiseConstantValueFunction::plot2DVrml2Leaf (const double &space, std::ofstream &output_vrml, 
						      double *low, double *high,
						      const double &scale0, const double &scale1,
						      const double &max_value)
{
  if (! m_alphaVectors)
    return;
  
  double width0 = (high[0]-low[0])/scale0, width1 = (high[1]-low[1])/scale1;
  double pos0 = (high[0]+low[0])/(2.0 * scale0), pos1 = (high[1]+low[1])/(2.0 * scale1);
  output_vrml << "\t\tTransform {\n\t\t\ttranslation "
	      << pos0 << ' ' << -getConstantValue () / (2.0 * max_value)  << ' ' << pos1 << ' '
	      << "\n\t\t\tchildren [\n\t\t\t\tShape {\n\t\t\t\t\tappearance Appearance {material Material {} }\n\t\t\t\t\tgeometry Box {size " 
	      << width0-space << ' ' << getConstantValue () / max_value << ' ' << width1-space << "}\n\t\t\t}\n\t\t]\n\t}\n";
}

double PiecewiseConstantValueFunction::getPointValueInLeaf (double *pos)
{
  return getConstantValue ();
}

int PiecewiseConstantValueFunction::getPointActionInLeaf (double *pos)
{
  if (m_alphaVectors)
    return (*(*m_alphaVectors)[0]->m_actions.begin ());  /* TODO: this is a hack that returns only one
							    action in the set. */
  else return -1;
}

} /* end of namespace */
