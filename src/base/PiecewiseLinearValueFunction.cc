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

#include "PiecewiseLinearValueFunction.h"
#include "ContinuousTransition.h"
#include <sstream>
#include <assert.h>
#include <stdlib.h>

namespace hmdp_base
{

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const int &sdim)
  : ValueFunction (sdim)
{
  m_bspType = PiecewiseLinearVFT;
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const int &sdim, const int &d, const double &pos)
  : ValueFunction (sdim, d, pos)
{
  m_bspType = PiecewiseLinearVFT;
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const int &sdim, const double *low, const double *high)
  : ValueFunction (sdim)
{
  m_bspType = PiecewiseLinearVFT;
  PiecewiseLinearValueFunction *vf_n = this;

  for (int i=0; i<m_nDim; i++)
    {
      /* upper bound */
      vf_n->setDimension (i);
      vf_n->setPosition (high[i]);
      vf_n->setLowerTree (new PiecewiseLinearValueFunction (m_nDim, i, low[i]));
      vf_n->setGreaterTree (new PiecewiseLinearValueFunction (m_nDim));
      vf_n = static_cast<PiecewiseLinearValueFunction*> (vf_n->getLowerTree ());
    
      /* lower bound */
      vf_n->setLowerTree (new PiecewiseLinearValueFunction (m_nDim));
      vf_n->setGreaterTree (new PiecewiseLinearValueFunction (m_nDim));
      vf_n = static_cast<PiecewiseLinearValueFunction*> (vf_n->getGreaterTree ());
    }
  
  /* set vf initial value to zero */
  double *zero_pwl_vf = (double*) calloc (m_nDim + 1, sizeof (double));  /* zero linear function */
  AlphaVector *av = new AlphaVector (m_nDim + 1, zero_pwl_vf);
  free (zero_pwl_vf);
  vf_n->addAVector (av);
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const PiecewiseLinearValueFunction &plvf)
  : ValueFunction (plvf.getSpaceDimension (), plvf.getDimension (), plvf.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;
  m_csd = plvf.getCSDFlag ();

  /* copy alpha vectors */
  if (plvf.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> ();
      for (unsigned int i=0; i<plvf.getAlphaVectorsSize (); i++)
	{
	  m_alphaVectors->push_back (new AlphaVector (*plvf.getAlphaVectorNth (i)));
	}
    }

  if (plvf.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*plvf.getAchievedGoals ());
    }

  if (! plvf.isLeaf ())
    {
      PiecewiseLinearValueFunction *vlt = static_cast<PiecewiseLinearValueFunction*> (plvf.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*vlt);
      PiecewiseLinearValueFunction *vge = static_cast<PiecewiseLinearValueFunction*> (plvf.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*vge);
    }
  else m_lt = m_ge = 0;
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const PiecewiseConstantValueFunction &pcvf)
  : ValueFunction (pcvf.getSpaceDimension (), pcvf.getDimension (), pcvf.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;

  /* copy alpha vectors */
  if (pcvf.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      double *pwl_fct = (double*) calloc (m_nDim+1, sizeof (double));
      pwl_fct[m_nDim] = pcvf.getConstantValue ();
      AlphaVector *av = new AlphaVector (m_nDim+1, pwl_fct);
      free (pwl_fct);
      (*m_alphaVectors)[0] = av;
    }

  if (pcvf.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*pcvf.getAchievedGoals ());
    }

  if (! pcvf.isLeaf ())
    {
      PiecewiseConstantValueFunction *vlt = static_cast<PiecewiseConstantValueFunction*> (pcvf.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*vlt);
      PiecewiseConstantValueFunction *vge = static_cast<PiecewiseConstantValueFunction*> (pcvf.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*vge);
    }
  else m_lt = m_ge = 0;
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const ContinuousReward &cr)
  : ValueFunction (cr.getSpaceDimension (), cr.getDimension (), cr.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;
  
  /* copy alpha vectors. */
  if (cr.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> ();
      if (cr.getType () == PiecewiseConstantRewardT)
	{
	  double *pwl_fct = (double*) calloc (m_nDim+1, sizeof (double));
	  pwl_fct[m_nDim] = cr.getAlphaVectorNth (0)->getAlphaNth (0);
	  AlphaVector *av = new AlphaVector (m_nDim + 1, pwl_fct);
	  free (pwl_fct);
	  m_alphaVectors->push_back (av);
	}
      else if (cr.getType () == PiecewiseLinearRewardT)
	{
	  for (unsigned int i=0; i<cr.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*cr.getAlphaVectorNth (i)));
	}
    }
  else if (cr.isLeaf ())
    {
      double *zero_pwl_vf = (double*) calloc (m_nDim + 1, sizeof (double)); 
      AlphaVector *av = new AlphaVector (m_nDim + 1, zero_pwl_vf);
      free (zero_pwl_vf);
      addAVector (av);
      std::cout << "added vector (leaf): " << *av << std::endl;
    }

  if (! cr.isLeaf ())
    {
      ContinuousReward *crlt = static_cast<ContinuousReward*> (cr.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*crlt);
      ContinuousReward *crge = static_cast<ContinuousReward*> (cr.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*crge);
    }
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const ContinuousOutcome &co)
  : ValueFunction (co.getSpaceDimension (), co.getDimension (), co.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;
  
  /* create an alpha vector from the continuous outcome probability */
  if (co.isLeaf ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      double *pwl_fct = (double*) calloc (1, sizeof (double));
      if (co.getProbability () >= 0.0)
	pwl_fct[0] = co.getProbability ();
      AlphaVector *av = new AlphaVector (1, pwl_fct);
      free (pwl_fct);
      (*m_alphaVectors)[0] = av;
    }  
  else
    {
      ContinuousOutcome *colt = static_cast<ContinuousOutcome*> (co.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*colt);
      ContinuousOutcome *coge = static_cast<ContinuousOutcome*> (co.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*coge);
    }
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const ContinuousTransition &ct)
  : ValueFunction (ct.getSpaceDimension (), ct.getDimension (), ct.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;

  /* create an alpha vector with zero value. */
  if (ct.isLeaf ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> ();
      double vals[m_nDim + 1];
      for (int i=0; i<m_nDim+1; i++)
	vals[i] = 0.0;
      AlphaVector *av = new AlphaVector (m_nDim + 1, vals);
      m_alphaVectors->push_back (av);
    }
  else 
    {
      ContinuousTransition *ctlt = static_cast<ContinuousTransition*> (ct.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*ctlt);
      ContinuousTransition *ctge = static_cast<ContinuousTransition*> (ct.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*ctge);
    }
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const ContinuousStateDistribution &csd)
  : ValueFunction (csd.getSpaceDimension (), csd.getDimension (), csd.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;

  if (csd.getProbability () >= 0.0)
    {
      m_csd = true;
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      double *pwl_fct = (double*) calloc (1, sizeof (double));
      pwl_fct[0] = csd.getProbability ();
      AlphaVector *av = new AlphaVector (1, pwl_fct);
      free (pwl_fct);
      (*m_alphaVectors)[0] = av;
    }
  
  if (! csd.isLeaf ())
    {
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (csd.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*csdlt);
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (csd.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*csdge);
    }
}

/* Warning: this is no 'regular' constructor: this is used for merging value
   functions by actions. DO NOT USE to convert value functions. Use dedicated
   constructors above, instead. */
PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (const ValueFunction &vf, 
							    const bool &actions)
  : ValueFunction (vf.getSpaceDimension (), vf.getDimension (), vf.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;

  if (vf.isLeaf ())
    {
      m_lt = m_ge = 0;
      
      if (actions)
	{
	  if (vf.getAlphaVectors ())
	    {
	      /* create an alpha vector that contains all the actions. */
	      std::set<int> allactions;
	      std::set<int>::const_iterator actit;
	      for (unsigned int i=0; i<vf.getAlphaVectorsSize (); i++)
		{
		  AlphaVector *av = vf.getAlphaVectorNth (i);
		  for (actit = av->getActionsBegin (); actit != av->getActionsEnd (); actit++)
		    allactions.insert ((*actit));
		}
	      AlphaVector *pwlav = new AlphaVector (static_cast<int> (allactions.size ()));
	      int j = 0;
	      for (actit = allactions.begin (); actit != allactions.end (); actit++)
		{
		  pwlav->setAlphaNth (j, static_cast<double> ((*actit)));
		  j++;
		}
	      m_alphaVectors = new std::vector<AlphaVector*> ();
	      m_alphaVectors->push_back (pwlav);
	    } /* end if alpha vectors. */
	} /* end if actions. */
      else
	{
	  /* TODO, but not required... */
	}
    }
  else 
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (vf.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*vflt, actions);
      ValueFunction *vfge = static_cast<ValueFunction*> (vf.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*vfge, actions);
    }
}

PiecewiseLinearValueFunction::PiecewiseLinearValueFunction (PiecewiseLinearValueFunction &vfactions,
							    const int &action, const bool &prop)
  : ValueFunction (vfactions.getSpaceDimension (), vfactions.getDimension (),
		   vfactions.getPosition ())
{
  m_bspType = PiecewiseLinearVFT;

  if (vfactions.isLeaf ())
    {
      m_lt = m_ge = 0;
      
      if (vfactions.getAlphaVectors ())
	{
	  /* count all actions and check if the action 
	     of interest belongs to this leaf. */
	  bool found = false;
	  std::set<int> allactions;
	  std::set<int>::const_iterator actit;
	  for (unsigned int i=0; i<vfactions.getAlphaVectorsSize (); i++)
	    {
	      AlphaVector *av = vfactions.getAlphaVectorNth (i);
	      
	      for (actit = av->getActionsBegin (); actit != av->getActionsEnd (); actit++)
		{
		  allactions.insert ((*actit));
		  if ((*actit) == action)
		    {
		      found = true;
		      if (! prop) break;
		    }
		}
	      if (found && ! prop) break;
	    }
	  double value = 1.0;
	  if (prop) value = 1.0 / static_cast<double> (allactions.size ());
	  m_alphaVectors = new std::vector<AlphaVector*> (1);
	  (*m_alphaVectors)[0] = new AlphaVector (value);
	} /* end if alpha vectors. */
    }
  else
    {
      PiecewiseLinearValueFunction *pwllt = static_cast<PiecewiseLinearValueFunction*> (vfactions.getLowerTree ());
      m_lt = new PiecewiseLinearValueFunction (*pwllt, action, prop);
      PiecewiseLinearValueFunction *pwlge = static_cast<PiecewiseLinearValueFunction*> (vfactions.getGreaterTree ());
      m_ge = new PiecewiseLinearValueFunction (*pwlge, action, prop);
    }
}

PiecewiseLinearValueFunction::~PiecewiseLinearValueFunction ()
{}

void PiecewiseLinearValueFunction::leafDataIntersectMax (const BspTree &bt, const BspTree &btr,
							 double *low, double *high)
{
  const PiecewiseLinearValueFunction &plvfa = static_cast<const PiecewiseLinearValueFunction&> (bt);
  const PiecewiseLinearValueFunction &plvfb = static_cast<const PiecewiseLinearValueFunction&> (btr);

  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> ();
  else 
    {
      for (size_t i=0; i<m_alphaVectors->size (); i++)
	delete (*m_alphaVectors)[i];
    }

  if (plvfa.getAlphaVectors () && plvfb.getAlphaVectors ())
    {
      if (! m_alphaVectors)
        m_alphaVectors = new std::vector<AlphaVector*> ();
      AlphaVector::maxLinearAlphaVector (*plvfa.getAlphaVectors (), *plvfb.getAlphaVectors (), 
					 low, high, m_alphaVectors);
    }
  else if (plvfa.getAlphaVectors ())
    {
      for (unsigned int i=0; i<plvfa.getAlphaVectorsSize (); i++)
	m_alphaVectors->push_back (new AlphaVector (*plvfa.getAlphaVectorNth (i)));
    }
  else if (plvfb.getAlphaVectors ())
    {
      for (unsigned int i=0; i<plvfb.getAlphaVectorsSize (); i++)
	  m_alphaVectors->push_back (new AlphaVector (*plvfb.getAlphaVectorNth (i)));
    }
  else { delete m_alphaVectors; m_alphaVectors = 0; }
  //m_alphaVectors->push_back (new AlphaVector (0.0));

  /* TODO: goals. For now, union. */
  unionGoalSets (plvfa, plvfb);
}

void PiecewiseLinearValueFunction::leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
							  double *low, double *high)
{
  const PiecewiseLinearValueFunction &plvfa = static_cast<const PiecewiseLinearValueFunction&> (bt);
  const PiecewiseLinearValueFunction &plvfb = static_cast<const PiecewiseLinearValueFunction&> (btr);

  if (plvfa.getAlphaVectors () && plvfb.getAlphaVectors ())
    {
      if (! m_alphaVectors)
        m_alphaVectors = new std::vector<AlphaVector*> ();
      AlphaVector::crossSumAlphaVectors (*plvfa.getAlphaVectors (), *plvfb.getAlphaVectors (), 
					 low, high, m_alphaVectors);
    }
}

void PiecewiseLinearValueFunction::leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
							  double *low, double *high)
{
  const PiecewiseLinearValueFunction &plvfa = static_cast<const PiecewiseLinearValueFunction&> (bt);
  const PiecewiseLinearValueFunction &plvfb = static_cast<const PiecewiseLinearValueFunction&> (btr);

  if (plvfa.getAlphaVectors () && plvfb.getAlphaVectors ())
    {
      if (! m_alphaVectors)
        m_alphaVectors = new std::vector<AlphaVector*> ();
      AlphaVector::crossSubtractAlphaVectors (*plvfa.getAlphaVectors (), *plvfb.getAlphaVectors (), 
					      low, high, m_alphaVectors);
    }
}

void PiecewiseLinearValueFunction::transferData (const BspTree &bt)
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
	    {
	      AlphaVector *av = cr.getAlphaVectorNth (i);
	      if (av->getSize () == m_nDim+1)
		m_alphaVectors->push_back (new AlphaVector (*cr.getAlphaVectorNth (i)));
	      else
		{
		  assert (av->getSize () == 1);  /* it must be a constant value then. */
		  double *pwl_fct = (double*) calloc (m_nDim+1, sizeof (double));
		  pwl_fct[m_nDim] = cr.getAlphaVectorNth (0)->getAlphaNth (0);
		  AlphaVector *nav = new AlphaVector (m_nDim+1, pwl_fct);
		  free (pwl_fct);
		  m_alphaVectors->push_back (nav);
		}
	    }
	}
    }
  else if (bt.getType () == ContinuousStateDistributionT)
    {
      const ContinuousStateDistribution &csd = static_cast<const ContinuousStateDistribution&> (bt); 
      if (csd.getProbability () > 0.0)
	{
	  m_csd = true;
	  m_alphaVectors = new std::vector<AlphaVector*> ();
	  m_alphaVectors->push_back (new AlphaVector (csd.getProbability ()));
	}
    }
  else
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
    }
}

void PiecewiseLinearValueFunction::leafDataShift (double *shift)
{
  if (m_alphaVectors)
    {
      /* check for converted rewards or outcomes */
      if (getAlphaVectorsSize () == 1
	  && getAlphaVectorNth (0)->getSize () == 1)
	{
	  /* do nothing */
	}
      else  /* linear functions over the full space */ 
	{
	  for (unsigned int i=0; i<getAlphaVectorsSize (); i++)
	    {
	      double newc = getAlphaVectorNth (i)->getAlphaNth (m_nDim);
	      for (int j=0; j<m_nDim; j++)
		newc += getAlphaVectorNth (i)->getAlphaNth (j) * shift[j];
	      getAlphaVectorNth (i)->setAlphaNth (m_nDim, newc);
	    }
	}
    }
}

void PiecewiseLinearValueFunction::mergeContiguousLeaves (ValueFunction *root, ValueFunction *lt, ValueFunction *ge,
							  double *low, double *high)
{
  PiecewiseLinearValueFunction *plvflt = static_cast<PiecewiseLinearValueFunction*> (lt);
  PiecewiseLinearValueFunction *plvfge = static_cast<PiecewiseLinearValueFunction*> (ge);

  std::vector<AlphaVector*> *vavlt = plvflt->getAlphaVectors ();
  std::vector<AlphaVector*> *vavge = plvfge->getAlphaVectors ();

  if (! vavlt || ! vavge)
    return;

  if (BspTreeOperations::m_piecesMergingByValue
      && AlphaVector::isVecEqual (*vavlt, *vavge))   /* TODO: merge by action & merge on equality */
    {
      /* transfer data to root and detete leaves. */
      transferData (*plvfge);
      
      /* unionize goal sets */
      /* if (BspTreeOperations::m_piecesMergingByValue
	 || BspTreeOperations::m_piecesMergingByAction) */
      unionGoalSets (*plvflt, *plvfge);
      
      delete plvflt; plvflt = 0;
      delete plvfge; plvfge = 0;
      setLowerTree (0);
      setGreaterTree (0);
    }
  else if (BspTreeOperations::m_piecesMergingByAction
	   && AlphaVector::isVecEqual (*vavlt, *vavge))
    {
      /* transfer data to root and detete leaves. */
      transferData (*plvfge);
      unionGoalSets (*plvflt, *plvfge);
      
      delete plvflt; plvflt = 0;
      delete plvfge; plvfge = 0;
      setLowerTree (0);
      setGreaterTree (0);
    }
#ifdef HAVE_LP
  else if (BspTreeOperations::m_piecesMergingByValue)
    {
      bool pwl_merge = true;
      double low_piece[getSpaceDimension ()], high_piece[getSpaceDimension ()];
      
      /* prepare lower piece bounds */
      for (int i=0; i<getSpaceDimension (); i++)
	{
	  low_piece[i] = low[i];
	  high_piece[i] = getPosition ();
	}
      if (LpSolve5::areLPDominated (plvfge->getAlphaVectors (), plvflt->getAlphaVectors (),
				    0, low_piece, high_piece))
	{
	  /* prepare greater piece bounds */
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      low_piece[i] = getPosition ();
	      high_piece[i] = high[i];
	    }
	  pwl_merge = LpSolve5::areLPDominated (plvflt->getAlphaVectors (), plvfge->getAlphaVectors (),
						0, low_piece, high_piece);
	  
	  if (pwl_merge)
	    {
	      /* transfer data to root and detete leaves. */
	      transferData (*plvfge);
	      
	      /* unionize goal sets */
	      if (BspTreeOperations::m_piecesMergingByValue
		  || BspTreeOperations::m_piecesMergingByAction)
		unionGoalSets (*plvflt, *plvfge);
	      
	      delete plvflt; plvflt = 0;
	      delete plvfge; plvfge = 0;
	      setLowerTree (0);
	      setGreaterTree (0);
	    }
	}
      
    }
#endif
}

void PiecewiseLinearValueFunction::expectedValueFromLeaves (double *val, 
							    double *low, double *high)
{
  if (isLeaf ())
    {
      /* fetch a 'witness' vector at tile center point. */
      double *cpt = (double*) malloc (m_nDim * sizeof (double));
      for (int i=0; i<m_nDim; i++)
	cpt[i] = (high[i] - low[i]) / 2.0;
      double bval;
      AlphaVector *wtAv = AlphaVector::bestAlphaVector (*getAlphaVectors (), cpt, &bval);
      
      double expect_ct = 1.0, expect_fct = 1.0;
      for (int i=0; i<m_nDim; i++)
	{
	  expect_ct *= (high[i] - low[i]);
	  expect_fct *= 0.5*wtAv->getAlphaNth (i) * (high[i]*high[i] - low[i]*low[i]);
	}
      expect_ct *= wtAv->getAlphaNth (m_nDim); /* constant */
      *val += expect_ct; *val += expect_fct;
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      PiecewiseLinearValueFunction *plge
	= static_cast<PiecewiseLinearValueFunction*> (getGreaterTree ());
      plge->expectedValueFromLeaves (val, low, high);
      low[getDimension ()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      PiecewiseLinearValueFunction *pcge
	= static_cast<PiecewiseLinearValueFunction*> (getLowerTree ());
      pcge->expectedValueFromLeaves (val, low, high);
      high[getDimension ()] = b;
    }
}

double PiecewiseLinearValueFunction::getPointValueInLeaf (double *pos)
{
  double val = 0.0;
  if (m_alphaVectors)
    AlphaVector::bestAlphaVector (*m_alphaVectors, pos, &val);
  return val;
}

} /* end of namespace */
