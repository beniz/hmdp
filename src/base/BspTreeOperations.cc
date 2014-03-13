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

#include "BspTreeOperations.h"
#include "ContinuousTransition.h"
#include "ContinuousReward.h"
#include "PiecewiseConstantReward.h"
#include "PiecewiseLinearReward.h"
#include "ValueFunction.h"
#include "PiecewiseConstantValueFunction.h"
#include "PiecewiseLinearValueFunction.h"
#include "ContinuousStateDistribution.h"
#ifdef HAVE_CSA
#include "BspTreeCSA.h"
#endif
#include <math.h>
#include <assert.h>

#ifdef HAVE_CSA
using namespace hmdp_csa;
#endif

namespace hmdp_base
{

BspTreeType BspTreeOperations::m_currentOutputType = BspTreeT;  /* default */
bool BspTreeOperations::m_bspBalance = false;  /* TODO: broken (just have to set up symetrical cross-tree construction). Beware with non-symetrical operators !!! */
BspTreeIntersectionType BspTreeOperations::m_currentIntersectionType = BTI_INIT; /* default */

int BspTreeOperations::m_outputTypeTableSize = 12;
BspTreeType BspTreeOperations::m_outputTypeTable[12][3] = {
  {ContinuousOutcomeT, PiecewiseConstantVFT, PiecewiseConstantVFT},
  {ContinuousOutcomeT, PiecewiseLinearVFT, PiecewiseLinearVFT},
  {ContinuousOutcomeT, ContinuousStateDistributionT, ContinuousStateDistributionT},
  {PiecewiseConstantVFT, PiecewiseConstantRewardT, PiecewiseConstantVFT},
  {PiecewiseLinearVFT, PiecewiseConstantRewardT, PiecewiseLinearVFT},
  {PiecewiseConstantVFT, PiecewiseLinearRewardT, PiecewiseLinearVFT},
  {PiecewiseLinearVFT, PiecewiseLinearRewardT, PiecewiseLinearVFT},
  {PiecewiseLinearVFT, ContinuousStateDistributionT, PiecewiseLinearVFT},
  {PiecewiseConstantVFT, ContinuousStateDistributionT, PiecewiseConstantVFT},
  {PiecewiseConstantVFT, PiecewiseLinearVFT, PiecewiseLinearVFT},
  {PiecewiseConstantRewardT, PiecewiseLinearRewardT, PiecewiseLinearRewardT}
#ifdef HAVE_CSA
  ,{BspTreeCSAT, BspTreeCSAT, PiecewiseConstantVFT}
#endif
};

bool BspTreeOperations::m_asymetricOperators = false;
bool BspTreeOperations::m_piecesMerging = false;
bool BspTreeOperations::m_piecesMergingByValue = false;
bool BspTreeOperations::m_piecesMergingByAction = false;
bool BspTreeOperations::m_piecesMergingEquality = false;  /* CHANGED: default but requires m_piecesMerging true. */

BspTree* BspTreeOperations::createTree (const int &dim)
{
  BspTree *res = 0;
  if (BspTreeOperations::m_currentOutputType == BspTreeT)
    res = new BspTree (dim);
  else if (BspTreeOperations::m_currentOutputType == ContinuousTransitionT)
    {
      ContinuousTransition *ct = new ContinuousTransition (dim);
      res = ct;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousRewardT)
    {
      ContinuousReward *cr = new ContinuousReward (dim);
      res = cr;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseConstantRewardT)
    {
      PiecewiseConstantReward *pcr = new PiecewiseConstantReward (dim);
      res = pcr;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseLinearRewardT)
    {
      PiecewiseLinearReward *plr = new PiecewiseLinearReward (dim);
      res = plr;
    }
  else if (BspTreeOperations::m_currentOutputType == ValueFunctionT)
    {
      ValueFunction *vf = new ValueFunction (dim);
      res = vf;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseConstantVFT)
    {
      PiecewiseConstantValueFunction *pcvf = new PiecewiseConstantValueFunction (dim);
      res = pcvf;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseLinearVFT)
    {
      PiecewiseLinearValueFunction *plvf = new PiecewiseLinearValueFunction (dim);
      res = plvf;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousOutcomeT)
    {
      ContinuousOutcome *co = new ContinuousOutcome (dim);
      res = co;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousStateDistributionT)
    {
      ContinuousStateDistribution *csd = new ContinuousStateDistribution (dim);
      res = csd;
    }
#ifdef HAVE_CSA
  else if (BspTreeOperations::m_currentOutputType == BspTreeCSAT)
    {
      BspTreeCSA *csa = new BspTreeCSA(dim);
      res = csa;
  }
#endif
  else res = 0;

  return res;
}

BspTree* BspTreeOperations::createTree (const int &dim, const int &d, const double &pos)
{
  BspTree *res = 0;
  if (BspTreeOperations::m_currentOutputType == BspTreeT)
    res = new BspTree (dim, d, pos);
  else if (BspTreeOperations::m_currentOutputType == ContinuousTransitionT)
    {
      ContinuousTransition *ct = new ContinuousTransition (dim, d, pos);
      res = ct;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousRewardT)
    {
      ContinuousReward *cr = new ContinuousReward (dim, d, pos);
      res = cr;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseConstantRewardT)
    {
      PiecewiseConstantReward *pcr = new PiecewiseConstantReward (dim, d, pos);
      res = pcr;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseLinearRewardT)
    {
      PiecewiseLinearReward *plr = new PiecewiseLinearReward (dim, d, pos);
      res = plr;
    }
  else if (BspTreeOperations::m_currentOutputType == ValueFunctionT)
    {
      ValueFunction *vf = new ValueFunction (dim, d, pos);
      res = vf;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseConstantVFT)
    {
      PiecewiseConstantValueFunction *pcvf = new PiecewiseConstantValueFunction (dim, d, pos);
      res = pcvf;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseLinearVFT)
    {
      PiecewiseLinearValueFunction *plvf = new PiecewiseLinearValueFunction (dim, d, pos);
      res = plvf;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousOutcomeT)
    {
      ContinuousOutcome *co = new ContinuousOutcome (dim, d, pos);
      res = co;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousStateDistributionT)
    {
      ContinuousStateDistribution *csd = new ContinuousStateDistribution (dim, d, pos);
      res = csd;
    }
#ifdef HAVE_CSA
  else if (BspTreeOperations::m_currentOutputType == BspTreeCSAT)
    {
      BspTreeCSA *csa = new BspTreeCSA(dim,d,pos);
      res = csa;
    }
#endif
  else res = 0;
  
  return res;
}

BspTree* BspTreeOperations::createTree (BspTree *bt)
{
  BspTree *res = 0;
  if (BspTreeOperations::m_currentOutputType == BspTreeT)
    res = new BspTree (*bt);
  else if (BspTreeOperations::m_currentOutputType == ContinuousTransitionT)
    {
      ContinuousTransition *cbt = static_cast<ContinuousTransition*> (bt);
      ContinuousTransition *ct = new ContinuousTransition (*cbt);
      res = ct;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousRewardT)
    {
      ContinuousReward *cbr = static_cast<ContinuousReward*> (bt);
      ContinuousReward *cr = new ContinuousReward (*cbr);
      res = cr;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseConstantRewardT)
    {
      PiecewiseConstantReward *cpcr = static_cast<PiecewiseConstantReward*> (bt);
      PiecewiseConstantReward *pcr = new PiecewiseConstantReward (*cpcr);
      res = pcr;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseLinearRewardT)
    {
      PiecewiseLinearReward *cplr = static_cast<PiecewiseLinearReward*> (bt);
      PiecewiseLinearReward *plr = new PiecewiseLinearReward (*cplr);
      res = plr;
    }
  else if (BspTreeOperations::m_currentOutputType == ValueFunctionT)
    {
      ValueFunction *cvf = static_cast<ValueFunction*> (bt);
      ValueFunction *vf = new ValueFunction (*cvf);
      res = vf;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseConstantVFT)
    {
      PiecewiseConstantValueFunction *pcvf = 0;
      if (bt->getType () == PiecewiseConstantVFT)
	{
	  PiecewiseConstantValueFunction *cpcvf = static_cast<PiecewiseConstantValueFunction*> (bt);
	  pcvf = new PiecewiseConstantValueFunction (*cpcvf);
	}
      else if (bt->getType () == PiecewiseConstantRewardT)
	{
	  PiecewiseConstantReward *ccr = static_cast<PiecewiseConstantReward*> (bt);
	  pcvf = new PiecewiseConstantValueFunction (*ccr);
	}
      else if (bt->getType () == ContinuousOutcomeT)
	{
	  ContinuousOutcome *co = static_cast<ContinuousOutcome*> (bt);
	  pcvf = new PiecewiseConstantValueFunction (*co);
	}
      else if (bt->getType () == ContinuousStateDistributionT)
	{
	  ContinuousStateDistribution *csd = static_cast<ContinuousStateDistribution*> (bt);
	  pcvf = new PiecewiseConstantValueFunction (*csd);
	}
      res = pcvf;
    }
  else if (BspTreeOperations::m_currentOutputType == PiecewiseLinearVFT)
    {
      PiecewiseLinearValueFunction *plvf = 0;
      if (bt->getType () == PiecewiseLinearVFT)
	{
	  PiecewiseLinearValueFunction *cplvf = static_cast<PiecewiseLinearValueFunction*> (bt);
	  plvf = new PiecewiseLinearValueFunction (*cplvf);
	}
      else if (bt->getType () == PiecewiseConstantVFT)
	{
	  PiecewiseConstantValueFunction *cpcvf = static_cast<PiecewiseConstantValueFunction*> (bt);
	  plvf = new PiecewiseLinearValueFunction (*cpcvf);
	}
      else if (bt->getType () == PiecewiseConstantRewardT
	       || bt->getType () == PiecewiseLinearRewardT)
	{
	  ContinuousReward *cr = static_cast<ContinuousReward*> (bt);
	  plvf = new PiecewiseLinearValueFunction (*cr);
	}
      else if (bt->getType () == ContinuousOutcomeT)
	{
	  ContinuousOutcome *co = static_cast<ContinuousOutcome*> (bt);
	  plvf = new PiecewiseLinearValueFunction (*co);
	}
      else if (bt->getType () == ContinuousStateDistributionT)
	{
	  ContinuousStateDistribution *csd = static_cast<ContinuousStateDistribution*> (bt);
	  plvf = new PiecewiseLinearValueFunction (*csd);
	}
      res = plvf;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousOutcomeT)
    {
      ContinuousOutcome *cco = static_cast<ContinuousOutcome*> (bt);
      ContinuousOutcome *co = new ContinuousOutcome (*cco);
      res = co;
    }
  else if (BspTreeOperations::m_currentOutputType == ContinuousStateDistributionT)
    {
      ContinuousStateDistribution *csd = 0;
      if (bt->getType () == ContinuousStateDistributionT)
	{
	  ContinuousStateDistribution *ccsd = static_cast<ContinuousStateDistribution*> (bt);
	  csd = new ContinuousStateDistribution (*ccsd);
	}
      else if (bt->getType () == ContinuousOutcomeT)
	{
	  ContinuousOutcome *co = static_cast<ContinuousOutcome*> (bt);
	  csd = new ContinuousStateDistribution (*co);
	}
      res = csd;
  }
#ifdef HAVE_CSA
  else if (BspTreeOperations::m_currentOutputType == BspTreeCSAT)
    {
      BspTreeCSA *ccsa = static_cast<BspTreeCSA*>(bt);
      BspTreeCSA *csa = new BspTreeCSA(*ccsa);
      res = csa;
    }
#endif
  else res = 0;

  return res;
}

BspTree* BspTreeOperations::copyTree (BspTree *bt)
{
  BspTreeOperations::m_currentOutputType = bt->getType ();
  return BspTreeOperations::createTree (bt);
}

BspTreeType BspTreeOperations::lookupOutputTypeTable (BspTreeType btt1, BspTreeType btt2)
{
  for (int i=0; i<BspTreeOperations::m_outputTypeTableSize; i++)
    {
      if ((BspTreeOperations::m_outputTypeTable[i][0] == btt1
	   && BspTreeOperations::m_outputTypeTable[i][1] == btt2)
	  || (BspTreeOperations::m_outputTypeTable[i][0] == btt2
	      && BspTreeOperations::m_outputTypeTable[i][1] == btt1))
	{
	  return BspTreeOperations::m_outputTypeTable[i][2];
	}
    }

    std::cerr << "[Warning]: BspTreeOperations::lookupOutputTypeTable: returning default intersection type.\n";
    std::cerr << "btt1: " << btt1 << " -- btt2: " << btt2 << std::endl;
  return BspTreeT;
}

/* stuff for asymetric operators. */
BspTree* BspTreeOperations::createTree (const int &dim, const int &d, const double &pos,
					const double &max_value)
{
  BspTree *bt = BspTreeOperations::createTree (dim, d, pos);
  
  if (BspTreeOperations::m_asymetricOperators
      && BspTreeOperations::m_currentIntersectionType == BTI_MAX
	&& (bt->getType () == PiecewiseConstantVFT
	    || bt->getType () == PiecewiseConstantRewardT))
    {
      if (bt->getType () == PiecewiseConstantVFT)
	{
	  PiecewiseConstantValueFunction *pcvf = static_cast<PiecewiseConstantValueFunction*> (bt);
	  pcvf->setSubTreeMaxValue (max_value);
	  return pcvf;
	}
      else if (bt->getType () == PiecewiseConstantRewardT)
	{
	  PiecewiseConstantReward *pcr = static_cast<PiecewiseConstantReward*> (bt);
	  pcr->setSubTreeMaxValue (max_value);
	  return pcr;
	}    
    }
  return bt;
}

void BspTreeOperations::setSubTreeMaxValue (BspTree *bt, BspTree *btmv)
{
  if (BspTreeOperations::m_asymetricOperators
      && BspTreeOperations::m_currentIntersectionType == BTI_MAX
      && (bt->getType () == PiecewiseConstantVFT
	  || bt->getType () == PiecewiseConstantRewardT))
    {
      double max_value = 0.0;
      
      if (bt->getType () == PiecewiseConstantVFT)
	{
	  PiecewiseConstantValueFunction *pcvf = static_cast<PiecewiseConstantValueFunction*> (bt);
	  if (btmv->getType () == PiecewiseConstantRewardT)
	    {
	      PiecewiseConstantReward *pcrmv = static_cast<PiecewiseConstantReward*> (btmv);
	      max_value = pcrmv->getSubTreeMaxValue ();
	    }
	  else if (btmv->getType () == PiecewiseConstantVFT)
	    {
	      PiecewiseConstantValueFunction *pcvfmv 
		= static_cast<PiecewiseConstantValueFunction*> (btmv);
	      max_value = pcvfmv->getSubTreeMaxValue ();
	    }
	  pcvf->setSubTreeMaxValue (max_value);
	}
      else if (bt->getType () == PiecewiseConstantRewardT)
	{
	  PiecewiseConstantReward *pcr = static_cast<PiecewiseConstantReward*> (bt);
	  if (btmv->getType () == PiecewiseConstantRewardT)
	    {
	      PiecewiseConstantReward *pcrmv = static_cast<PiecewiseConstantReward*> (btmv);
	      max_value = pcrmv->getSubTreeMaxValue ();
	    }
	  else if (btmv->getType () == PiecewiseConstantVFT)
	    {
	      PiecewiseConstantValueFunction *pcvfmv 
		= static_cast<PiecewiseConstantValueFunction*> (btmv);
	      max_value = pcvfmv->getSubTreeMaxValue ();
	    }
	  pcr->setSubTreeMaxValue (max_value);
	}    
    }
}

void BspTreeOperations::setSubTreeMaxValue (BspTree *bt, const BspTree &btmv)
{
  if (BspTreeOperations::m_asymetricOperators
      && BspTreeOperations::m_currentIntersectionType == BTI_MAX
      && (bt->getType () == PiecewiseConstantVFT
	  || bt->getType () == PiecewiseConstantRewardT))
    {
      double max_value = 0.0;
      
      if (bt->getType () == PiecewiseConstantVFT)
	{
	  PiecewiseConstantValueFunction *pcvf = static_cast<PiecewiseConstantValueFunction*> (bt);
	  if (btmv.getType () == PiecewiseConstantRewardT)
	    {
	      const PiecewiseConstantReward &pcrmv = 
		static_cast<const PiecewiseConstantReward&> (btmv);
	      max_value = pcrmv.getSubTreeMaxValue ();
	    }
	  else if (btmv.getType () == PiecewiseConstantVFT)
	    {
	      const PiecewiseConstantValueFunction &pcvfmv 
		= static_cast<const PiecewiseConstantValueFunction&> (btmv);
	      max_value = pcvfmv.getSubTreeMaxValue ();
	    }
	  pcvf->setSubTreeMaxValue (max_value);
	}
      else if (bt->getType () == PiecewiseConstantRewardT)
	{
	  PiecewiseConstantReward *pcr = static_cast<PiecewiseConstantReward*> (bt);
	  if (btmv.getType () == PiecewiseConstantRewardT)
	    {
	      const PiecewiseConstantReward &pcrmv 
		= static_cast<const PiecewiseConstantReward&> (btmv);
	      max_value = pcrmv.getSubTreeMaxValue ();
	    }
	  else if (btmv.getType () == PiecewiseConstantVFT)
	    {
	      const PiecewiseConstantValueFunction &pcvfmv 
		= static_cast<const PiecewiseConstantValueFunction&> (btmv);
	      max_value = pcvfmv.getSubTreeMaxValue ();
	    }
	  pcr->setSubTreeMaxValue (max_value);
	}    
    }
}

void BspTreeOperations::swapDimensions(BspTree *bt, int swapTable[])
{
  if (bt->getDimension()>=0)
    {
      bt->setDimension(swapTable[bt->getDimension()]);
    }
  
  if (!bt->isLeaf())
    {
      BspTreeOperations::swapDimensions(bt->getLowerTree(),swapTable);
      BspTreeOperations::swapDimensions(bt->getGreaterTree(),swapTable);
    }
}

/* btr is a leaf. */
BspTree* BspTreeOperations::intersectWithCell (BspTree *btr, BspTree *bt, 
					       double *low, double *high)
  {
    BspTree *bsp_n, *nbtr=0, *nbt=0;

    if (bt->isLeaf ())
      {
	/* return a 'dead-end' leaf in case the tile is too small. */
	for (int d=0; d<btr->getSpaceDimension (); d++)
	  {
	    if (Alg::REqual (high[d], low[d], Alg::m_doubleEpsilon))
	      {
		return BspTreeOperations::createTree (btr->getSpaceDimension ());
	      }
	  }

	/* with pieces merging, it happens that cells are immediately intersected,
	   without the need for partitioning. This can lead to tree type mismatch. */
	if (btr->getType () != bt->getType ())
	  {
	    if (btr->getType () != BspTreeOperations::m_currentOutputType
		&& bt->getType () != BspTreeOperations::m_currentOutputType)
	      {
		nbtr = BspTreeOperations::createTree (btr);
		nbt = BspTreeOperations::createTree (bt);
	      }
	    else if (btr->getType () != BspTreeOperations::m_currentOutputType)
	      {
		nbtr = BspTreeOperations::createTree (btr);
		nbt = bt;
	      }
	    else if (bt->getType () != BspTreeOperations::m_currentOutputType)
	      {
		nbt = BspTreeOperations::createTree (bt);
		nbtr = btr;
	      }
	  }
	else 
	  {
	    nbtr = btr;
	    nbt = bt;
	  }

	bsp_n = BspTreeOperations::createTree (btr->getSpaceDimension ());
	
	/* perform the correct intersection of the leaves */
	if (BspTreeOperations::m_currentIntersectionType == BTI_INIT)
	  bsp_n->leafDataIntersectInit (*nbt, *nbtr, low, high);  /* virtual call */
	else if (BspTreeOperations::m_currentIntersectionType == BTI_MAX)
	  bsp_n->leafDataIntersectMax (*nbt, *nbtr, low, high);  /* virtual call */
	else if (BspTreeOperations::m_currentIntersectionType == BTI_PLUS)
	  bsp_n->leafDataIntersectPlus (*nbt, *nbtr, low, high);
	else if (BspTreeOperations::m_currentIntersectionType == BTI_MINUS)
	  bsp_n->leafDataIntersectMinus (*nbt, *nbtr, low, high);
	else if (BspTreeOperations::m_currentIntersectionType == BTI_MULT)
	  bsp_n->leafDataIntersectMult (*nbt, *nbtr, low, high);
	else if (BspTreeOperations::m_currentIntersectionType == BTI_CSD_DIFF)
	  bsp_n->leafDataIntersectCsdDiff (*nbt, *nbtr, low, high);
	else if (BspTreeOperations::m_currentIntersectionType == BTI_UNION)
	  bsp_n->leafDataIntersectUnion (*nbt, *nbtr, low, high);
	else std::cout << "Intersection type unknown: leaves data are not processed !\n";

	if (btr->getType () != BspTreeOperations::m_currentOutputType)
	  {
	    BspTree::deleteBspTree (nbtr);
	    nbtr = 0;
	  }
	if (bt->getType () != BspTreeOperations::m_currentOutputType)
	  {
	    BspTree::deleteBspTree (nbt);
	    nbt = 0;
	  }
	return bsp_n;
      }

    bsp_n = BspTreeOperations::createTree (bt->getSpaceDimension (), bt->getDimension (), bt->getPosition ());

    double bound;

    /* NEW: asymetric max. */
    if (BspTreeOperations::m_asymetricOperators
	&& BspTreeOperations::m_currentIntersectionType == BTI_MAX
	&& (btr->getType () == PiecewiseConstantVFT
	    || btr->getType () == PiecewiseConstantRewardT))
      {
	BspTreeAlpha *btr_leaf = static_cast<BspTreeAlpha*> (btr);
	BspTreeAlpha *bt_subtree = static_cast<BspTreeAlpha*> (bt);

	//debug
	/* std::cout << "btr leaf max value: " << btr_leaf->getSubTreeMaxValue ()
	   << " -- bt subtree max value: " << bt_subtree->getSubTreeMaxValue ()
	   << std::endl; */
	//debug

	if (btr_leaf->getSubTreeMaxValue () > bt_subtree->getSubTreeMaxValue ())
	  {
	    bsp_n->transferData (*btr);
	    return bsp_n;
	  }
      }
    
    /* if leaf is too small, stop here. */
    if (Alg::REqual (bsp_n->getPosition (), low[bsp_n->getDimension ()], Alg::m_doubleEpsilon))
      bsp_n->setLowerTree (BspTreeOperations::createTree (bsp_n->getSpaceDimension ())); /* leaf. */
    else 
      {
	bound = high[bsp_n->getDimension ()];
	high[bsp_n->getDimension ()] = bsp_n->getPosition ();
	bsp_n->setLowerTree (BspTreeOperations::intersectWithCell (btr, bt->getLowerTree (), 
								   low, high));
	high[bsp_n->getDimension ()] = bound;
      }

    /* if leaf is too small, stop here. */
    if (Alg::REqual (bsp_n->getPosition (), high[bsp_n->getDimension ()], Alg::m_doubleEpsilon))
      bsp_n->setGreaterTree (BspTreeOperations::createTree (bsp_n->getSpaceDimension ())); /* leaf. */
    else
      {
	bound = low[bsp_n->getDimension ()];
	low[bsp_n->getDimension ()] = bsp_n->getPosition ();
	bsp_n->setGreaterTree (BspTreeOperations::intersectWithCell (btr, bt->getGreaterTree (), 
								     low, high));
	low[bsp_n->getDimension ()] = bound;
      }    

    return bsp_n;
  }

BspTree* BspTreeOperations::intersectLowerHalf (const BspTree &btr, const int &d, const double &pos,
						double *low, double *high)
{
  BspTree *bsp_n;
  
  if (btr.isLeaf ())
    {
      bsp_n = BspTreeOperations::createTree (btr.getSpaceDimension ()); /* warning pos is set to 0 */
      bsp_n->transferData (btr); /* virtual call */
      return bsp_n;
    }

  if (d == btr.getDimension ())   /* parallel to current tree's plane */
    {
      if (Alg::REqual (pos, btr.getPosition (), Alg::m_doubleEpsilon))
	{
	  if (Alg::REqual (btr.getPosition (), low[d], Alg::m_doubleEpsilon))
	    return BspTreeOperations::createTree (btr.getSpaceDimension ());
	  else 
	    {
	      bsp_n = BspTreeOperations::intersectLowerHalf (*btr.getLowerTree (), d, pos, low, high);
	      BspTreeOperations::setSubTreeMaxValue (bsp_n, btr.getLowerTree ());
	      return bsp_n;
	    }
	}
      else if (btr.getPosition () < pos)
        {
          bsp_n = BspTreeOperations::createTree (btr.getSpaceDimension (), btr.getDimension (), 
						 btr.getPosition ());
          bsp_n->setLowerTree (BspTreeOperations::createTree (btr.getLowerTree ()));
          bsp_n->setGreaterTree (BspTreeOperations::intersectLowerHalf(*btr.getGreaterTree (), d, pos, low, high));
	  BspTreeOperations::setSubTreeMaxValue (bsp_n, btr);
          return bsp_n;
        }
      else  /* position > pos */
        {
          bsp_n = BspTreeOperations::intersectLowerHalf (*btr.getLowerTree (), d, pos, low, high);
	  BspTreeOperations::setSubTreeMaxValue (bsp_n, btr.getLowerTree ());
	  return bsp_n;
        }
    }
  else  /* cuts through this tree partition plane */
    {
      bsp_n = BspTreeOperations::createTree (btr.getSpaceDimension (), 
					     btr.getDimension (), btr.getPosition ());
      bsp_n->setLowerTree (BspTreeOperations::intersectLowerHalf (*btr.getLowerTree (), d, pos, low, high));
      bsp_n->setGreaterTree (BspTreeOperations::intersectLowerHalf (*btr.getGreaterTree (), d, pos, low, high));
      BspTreeOperations::setSubTreeMaxValue (bsp_n, btr);
      return bsp_n;
    }
}

BspTree* BspTreeOperations::intersectGreaterHalf (const BspTree &btr, 
						  const int &d, const double &pos,
						  double *low, double *high)
{
  BspTree *bsp_n;

  if (btr.isLeaf ())
    {
      bsp_n = BspTreeOperations::createTree (btr.getSpaceDimension ()); /* warning pos is set to 0 */
      bsp_n->transferData (btr); /* virtual call */
      return bsp_n;
    }

  if (d == btr.getDimension ()) /* parallel to current tree's plane */
    {
      if (Alg::REqual (pos, btr.getPosition (), Alg::m_doubleEpsilon))
	{
	  if (Alg::REqual (btr.getPosition (), high[d], Alg::m_doubleEpsilon))
	    return createTree (btr.getSpaceDimension ());
	  else {
	    bsp_n = BspTreeOperations::intersectGreaterHalf (*btr.getGreaterTree (), d, pos, low, high);
	    BspTreeOperations::setSubTreeMaxValue (bsp_n, btr.getGreaterTree ());
	    return bsp_n;
	  }
	}
	else if (btr.getPosition () > pos)
        {
          bsp_n = BspTreeOperations::createTree (btr.getSpaceDimension (), 
						 btr.getDimension (), btr.getPosition ());
          bsp_n->setGreaterTree (BspTreeOperations::createTree (btr.getGreaterTree ()));
          bsp_n->setLowerTree (BspTreeOperations::intersectGreaterHalf (*btr.getLowerTree (), d, pos, low, high));
	  BspTreeOperations::setSubTreeMaxValue (bsp_n, btr);
          return bsp_n;
        }
      else  /* position < pos */
        {
          bsp_n = BspTreeOperations::intersectGreaterHalf (*btr.getGreaterTree (), d, pos, low, high);
	  BspTreeOperations::setSubTreeMaxValue (bsp_n, btr.getGreaterTree ());
          return bsp_n;
        }
    }
  else  /* cuts through this tree partition plane */
    {
      bsp_n = BspTreeOperations::createTree (btr.getSpaceDimension (), 
					     btr.getDimension (), btr.getPosition ());
      bsp_n->setLowerTree (BspTreeOperations::intersectGreaterHalf (*btr.getLowerTree (), d, pos, low, high));
      bsp_n->setGreaterTree (BspTreeOperations::intersectGreaterHalf (*btr.getGreaterTree (), d, pos, low, high));
      BspTreeOperations::setSubTreeMaxValue (bsp_n, btr);
      return bsp_n;
    }
}

BspTree* BspTreeOperations::intersectTrees (BspTree *bt1, BspTree *bt2,
					    double *low, double *high)
{
  /* setting the correct output type */
  if (bt1->getType () == bt2->getType ())
    BspTreeOperations::m_currentOutputType = bt1->getType ();
  else
    BspTreeOperations::m_currentOutputType = BspTreeOperations::lookupOutputTypeTable (bt1->getType (),
										       bt2->getType ());

  BspTree *bsp_T1, *bsp_T2;

  if (bt1->isLeaf ())
      /* || BspTreeOperations::m_currentIntersectionType == BTI_MINUS
	 || BspTreeOperations::m_currentIntersectionType == BTI_CSD_DIFF) */
    return BspTreeOperations::intersectWithCell (bt1, bt2, low, high);
  if (bt2->isLeaf ()) 
      /* && BspTreeOperations::m_currentIntersectionType != BTI_MINUS
	 && BspTreeOperations::m_currentIntersectionType != BTI_CSD_DIFF) */
    return BspTreeOperations::intersectWithCell (bt2, bt1, low, high);

  /* Test: bsp balance here */
  if (BspTreeOperations::m_bspBalance)
    {
      double dist1 = fabs ((high[bt1->getDimension ()] - low[bt1->getDimension ()]) / 2.0
                           - bt1->getPosition ());
      double dist2 = fabs ((high[bt2->getDimension ()] - low[bt2->getDimension ()]) / 2.0
                           - bt2->getPosition ());
      if (dist1 < dist2)
        {
          bsp_T1 = bt1;
          bsp_T2 = bt2;
        }
      else
        {
	  bsp_T1 = bt2;
          bsp_T2 = bt1;
        }
    }
  else
    {
      bsp_T1 = bt1;
      bsp_T2 = bt2;
    }

  /* partition T2 with the partition of T1 */
  BspTree *bsp_p = BspTreeOperations::partition (*bsp_T2, bsp_T1->getDimension (), 
						 bsp_T1->getPosition (), low, high);
  
  BspTree *bsp_n = BspTreeOperations::createTree (bsp_T1->getSpaceDimension (), 
						  bsp_T1->getDimension (), bsp_T1->getPosition ());

  /* lower subtree */
  double bound = high[bsp_n->getDimension ()];
  high[bsp_n->getDimension ()] = bsp_n->getPosition ();
  bsp_n->setLowerTree (BspTreeOperations::intersectTrees (bsp_T1->getLowerTree (),
							  bsp_p->getLowerTree (),
							  low, high));
  high[bsp_n->getDimension ()] = bound;

  /* greater subtree */
  bound = low[bsp_n->getDimension ()];
  low[bsp_n->getDimension ()] = bsp_n->getPosition ();
  bsp_n->setGreaterTree (BspTreeOperations::intersectTrees (bsp_T1->getGreaterTree (),
							    bsp_p->getGreaterTree (),
							    low, high));
  low[bsp_n->getDimension ()] = bound;

  BspTreeOperations::setSubTreeMaxValue (bsp_n, bsp_T1);
  
  BspTree::deleteBspTree (bsp_p);
  bsp_p = 0;
  return bsp_n;
}

BspTree* BspTreeOperations::partition (const BspTree &bt, const int &d, const double &pos,
				       double *low, double *high)
{
  BspTree *bsp_n = BspTreeOperations::createTree (bt.getSpaceDimension (), d, pos);

  bsp_n->setLowerTree (BspTreeOperations::intersectLowerHalf (bt, d, pos, low, high));
  bsp_n->setGreaterTree (BspTreeOperations::intersectGreaterHalf (bt, d, pos, low, high));
  BspTreeOperations::setSubTreeMaxValue (bsp_n, bt);
  
  return bsp_n;
}

BspTree* BspTreeOperations::shiftTree (BspTree *bt, double *shift,
				       double *low, double *high)
{
  if (! bt->isLeaf ())
    {
      /* check if shifted position belongs to the domain. */
      int dim = bt->getDimension ();
      if (shift) bt->shiftPosition (shift[dim]);
      
      if (Alg::RSup (bt->getPosition (), high[dim],
		     Alg::m_doubleEpsilon))
	{
	  BspTree::deleteBspTree (bt->getGreaterTree ());
	  bt->setGreaterTree (0);
	  BspTree *lt = bt->getLowerTree ();
	  delete bt;
	  bt = 0;
	  BspTree *res = BspTreeOperations::shiftTree (lt, shift, low, high);
	  BspTreeOperations::setSubTreeMaxValue (res, lt);
	  return res;
	}
      else if (Alg::RInf (bt->getPosition (), low[dim],
			  Alg::m_doubleEpsilon))
	{
	  BspTree::deleteBspTree (bt->getLowerTree ());
	  bt->setLowerTree (0);
	  BspTree *ge = bt->getGreaterTree ();
	  delete bt;
	  bt = 0;
	  BspTree *res = BspTreeOperations::shiftTree (ge, shift, low, high);
	  BspTreeOperations::setSubTreeMaxValue (res, ge);
	  return res;
	}
      else {
	bt->setLowerTree (BspTreeOperations::shiftTree (bt->getLowerTree (), shift, low, high));
	bt->setGreaterTree (BspTreeOperations::shiftTree (bt->getGreaterTree (), shift, low, high));
	return bt;
      }
    }
  else /* is a leaf. */
    {
      bt->leafDataShift (shift); /* virtual */
      return bt;
    }
}

BspTree* BspTreeOperations::cropTree (BspTree *bt, double *low, double *high)
{
  BspTreeOperations::m_currentOutputType = bt->getType ();
  BspTree *bsp_n;
  int dim = bt->getDimension ();
  
  if (! bt->isLeaf ())
    {
      if (Alg::RSupEqual (bt->getPosition (), high[dim], Alg::m_doubleEpsilon))
	{
	  bsp_n = BspTreeOperations::createTree (bt->getSpaceDimension (), bt->getDimension (),
						 high[dim]);
	  BspTree *ge = BspTreeOperations::createTree (bt->getSpaceDimension ()); /* leaf. */
	  BspTree *lt = bt->getLowerTree ();
	  bsp_n->setGreaterTree (ge);
	  bsp_n->setLowerTree (BspTreeOperations::cropTree (lt, low, high));
	  
	  BspTreeOperations::setSubTreeMaxValue (bsp_n, lt);
	}
      else if (Alg::RInfEqual (bt->getPosition (), low[dim], Alg::m_doubleEpsilon))
	{
	  bsp_n = BspTreeOperations::createTree (bt->getSpaceDimension (), bt->getDimension (),
						 low[dim]);
	  BspTree *ge = bt->getGreaterTree ();
	  BspTree *lt = BspTreeOperations::createTree (bt->getSpaceDimension ()); /* leaf. */
	  bsp_n->setGreaterTree (BspTreeOperations::cropTree (ge, low, high));
	  bsp_n->setLowerTree (lt);
	  
	  BspTreeOperations::setSubTreeMaxValue (bsp_n, ge);
	}
      else  /* within the bounds. */ 
	{
	  bsp_n = BspTreeOperations::createTree (bt->getSpaceDimension (), bt->getDimension (),
						 bt->getPosition ());
	  
	  bsp_n->setLowerTree (BspTreeOperations::cropTree (bt->getLowerTree (), low, high));
	  bsp_n->setGreaterTree (BspTreeOperations::cropTree (bt->getGreaterTree (), low, high));
	  
	  BspTreeOperations::setSubTreeMaxValue (bsp_n, bt);
	}
    }
  else
    {
      bsp_n = BspTreeOperations::createTree (bt);
    }
  return bsp_n;
}

} /* end of namespace */
