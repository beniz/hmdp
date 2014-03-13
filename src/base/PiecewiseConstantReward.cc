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

#include "PiecewiseConstantReward.h"
#include "ValueFunction.h"

namespace hmdp_base
{

PiecewiseConstantReward::PiecewiseConstantReward (const int &sdim)
  : ContinuousReward (0, sdim)
{
  m_bspType = PiecewiseConstantRewardT;
}

PiecewiseConstantReward::PiecewiseConstantReward (const int &sdim, const int &d, const double &pos)
  : ContinuousReward (0, sdim, d, pos)
{
  m_bspType = PiecewiseConstantRewardT;
}

PiecewiseConstantReward::PiecewiseConstantReward (const int &dimension, const int &sdim)
  : ContinuousReward (dimension, sdim)
{
  m_bspType = PiecewiseConstantRewardT;
}

PiecewiseConstantReward::PiecewiseConstantReward (const int &dimension, const int &sdim, 
						  const int &d, const double &pos)
  : ContinuousReward (dimension, sdim, d, pos)
{
  m_bspType = PiecewiseConstantRewardT;
}

PiecewiseConstantReward::PiecewiseConstantReward (const int &dimension, const int &sdim, double **lowPos, 
						  double **highPos, double *low, double *high,
						  const double *value)
  : ContinuousReward (dimension, sdim)
{
  m_bspType = PiecewiseConstantRewardT;

  /* create a bsp tree from the transition tiling:
     create a tree for each tile, and intersect them. */
  PiecewiseConstantReward *bsp_n = this;
  PiecewiseConstantReward *bsp_c = this;
  BspTreeOperations::setIntersectionType (BTI_INIT); /* leaf intersection type */

  for (int tiles=0; tiles<m_tilingDimension; tiles++)
    {
      for (int i=0; i<m_nDim; i++)
	{
	  /* higher bound */
	  bsp_n->setDimension (i);
	  
	  /* check if higher bound belongs to the domain. */
	  if (highPos[tiles][i] > high[i])
	    {
	      std::cerr << "[Warning]: PiecewiseConstantReward constructor: upper bound is outside the domain in tile: " 
			<< tiles << ".\n";
	      highPos[tiles][i] = high[i];
	    }

	  bsp_n->setPosition (highPos[tiles][i]);

	  /* check if lower bound belongs to the domain. */
	  if (lowPos[tiles][i] < low[i])
	    {
	      std::cerr << "[Warning]: PiecewiseConstantReward constructor: lower bound is outside the domain in tile: "
			<< tiles << ".\n";
	      lowPos[tiles][i] = low[i];
	    }

	  bsp_n->setLowerTree (new PiecewiseConstantReward (m_tilingDimension, m_nDim, i, lowPos[tiles][i]));
	  bsp_n->setGreaterTree (new PiecewiseConstantReward (m_tilingDimension, m_nDim));
	  bsp_n = static_cast<PiecewiseConstantReward*> (bsp_n->getLowerTree ());
	  
	  /* lower bound */
	  bsp_n->setLowerTree (new PiecewiseConstantReward (m_tilingDimension, m_nDim));
	  bsp_n->setGreaterTree (new PiecewiseConstantReward (m_tilingDimension, m_nDim));
	  bsp_n = static_cast<PiecewiseConstantReward*> (bsp_n->getGreaterTree ());
	  
	  if (i == m_nDim - 1)  /* we now can fill the leaf (corresponds to a single tile) */
	    {
	      //std::cout << "adding vector of value: " << value[tiles] << std::endl;
	      AlphaVector *av = new AlphaVector (value[tiles]);
	      bsp_n->addAVector (av);
	    }
	}
      if (tiles > 0)
	{
	  BspTree *intersectedTree = BspTreeOperations::intersectTrees (this, bsp_c, low, high);
	  BspTree::deleteBspTree (bsp_c); bsp_c = 0; 
	  if (m_lt) BspTree::deleteBspTree (m_lt);
	  if (m_ge) BspTree::deleteBspTree (m_ge);
	  m_lt = m_ge = 0;
	  
	  /* copy because subtrees because we can't replace C++ prevents from replacing 'this' */
	  if (intersectedTree->getLowerTree ())
	    setLowerTree (new PiecewiseConstantReward (*static_cast<PiecewiseConstantReward*> (intersectedTree->getLowerTree ())));
	  if (intersectedTree->getGreaterTree ())
	    setGreaterTree (new PiecewiseConstantReward (*static_cast<PiecewiseConstantReward*> (intersectedTree->getGreaterTree ())));
	  BspTree::deleteBspTree (intersectedTree);
	}
      if (tiles < m_tilingDimension - 1) 
	{
	  bsp_c = new PiecewiseConstantReward (m_tilingDimension, m_nDim); 
	  bsp_n = bsp_c;
	}
    } /* end for tiles */
}

PiecewiseConstantReward::PiecewiseConstantReward(const int &sdim, const double *low, 
						 const double *high, const double &val)
  : ContinuousReward (sdim)
{
  m_bspType = PiecewiseConstantRewardT;
  PiecewiseConstantReward *cr_n = this;

  for (int i=0; i<m_nDim; i++)
    {
      /* upper bound */
      cr_n->setDimension (i);
      cr_n->setPosition (high[i]);
      cr_n->setLowerTree (new PiecewiseConstantReward (m_nDim, i, low[i]));
      cr_n->setGreaterTree (new PiecewiseConstantReward (m_nDim));
      cr_n = static_cast<PiecewiseConstantReward*> (cr_n->getLowerTree ());
    
      /* lower bound */
      cr_n->setLowerTree (new PiecewiseConstantReward (m_nDim));
      cr_n->setGreaterTree (new PiecewiseConstantReward (m_nDim));
      cr_n = static_cast<PiecewiseConstantReward*> (cr_n->getGreaterTree ());
    }
  
  /* set vf initial value to zero */
  AlphaVector *av = new AlphaVector (val);
  cr_n->addAVector (av);
}

PiecewiseConstantReward::PiecewiseConstantReward (const PiecewiseConstantReward &cr)
  : ContinuousReward (cr.getTilingDimension (), cr.getSpaceDimension (), cr.getDimension (), cr.getPosition ())
{
  m_bspType = PiecewiseConstantRewardT;
  m_maxValue = cr.getSubTreeMaxValue ();

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
      PiecewiseConstantReward *blt = static_cast<PiecewiseConstantReward*> (cr.getLowerTree ());
      m_lt = new PiecewiseConstantReward (*blt);
      PiecewiseConstantReward *bge = static_cast<PiecewiseConstantReward*> (cr.getGreaterTree ());
      m_ge = new PiecewiseConstantReward (*bge);
    }
  else m_lt = m_ge = 0;
}

PiecewiseConstantReward::PiecewiseConstantReward (const int &dimension, const BspTree &bt)
  : ContinuousReward (dimension, bt.getSpaceDimension (), bt.getDimension (), bt.getPosition ())
{
  m_bspType = PiecewiseConstantRewardT;

  if (! bt.isLeaf ())
    {
      m_lt = new PiecewiseConstantReward (0, *bt.getLowerTree ());
      m_ge = new PiecewiseConstantReward (0, *bt.getGreaterTree ());
    }
  else m_lt = m_ge = 0;

}

PiecewiseConstantReward::PiecewiseConstantReward(const ValueFunction &vf)
  : ContinuousReward(vf.getSpaceDimension (),vf.getDimension (),vf.getPosition ())
{
  m_bspType = PiecewiseConstantRewardT;
  
  /* copy alpha vectors. */
  if (vf.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> (1);
      (*m_alphaVectors)[0] = new AlphaVector (*vf.getAlphaVectorNth (0));
    }

  if (vf.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int>(*vf.getAchievedGoals ());
    }
  
  if (! vf.isLeaf())
    {
      ValueFunction *vflt = static_cast<ValueFunction*>(vf.getLowerTree());
      m_lt = new PiecewiseConstantReward(*vflt);
      ValueFunction *vfge = static_cast<ValueFunction*>(vf.getGreaterTree());
      m_ge = new PiecewiseConstantReward(*vfge);
    }
}

PiecewiseConstantReward::~PiecewiseConstantReward ()
{}

void PiecewiseConstantReward::leafDataIntersectMax (const BspTree &bt, const BspTree &btr,
							   double *low, double *high)
{
  const PiecewiseConstantReward &pcra = static_cast<const PiecewiseConstantReward&> (bt);
  const PiecewiseConstantReward &pcrb = static_cast<const PiecewiseConstantReward&> (btr);
  
  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> (1);
  else delete (*m_alphaVectors)[0];

  if (pcra.getAlphaVectors () && pcrb.getAlphaVectors ())
    {
      AlphaVector::maxConstantAlphaVector (*pcra.getAlphaVectors (), 
					   *pcrb.getAlphaVectors (), 0, 0, m_alphaVectors);
    }
  else if (pcra.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcra.getAlphaVectorNth (0));
  else if (pcrb.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcrb.getAlphaVectorNth (0));
  else { delete m_alphaVectors; m_alphaVectors = 0; }
  
  /* goals */
  if (m_achievedGoals) 
    {
      delete m_achievedGoals;
      m_achievedGoals = 0;
    }
  if ((pcra.getAchievedGoals () && pcrb.getAchievedGoals ())
      && (pcra.getConstantValue () == pcrb.getConstantValue ())
      && (pcra.getAchievedGoals ()->size () == pcrb.getAchievedGoals ()->size ()))
    {
      m_achievedGoals = new std::vector<int> (*pcrb.getAchievedGoals ());
    }
  else 
    {
      if (pcra.getAchievedGoals () 
	  && pcra.getConstantValue () == getConstantValue ())
	m_achievedGoals = new std::vector<int> (*pcra.getAchievedGoals ());
      else if (pcrb.getAchievedGoals ()
	       && pcrb.getConstantValue () == getConstantValue ())
	m_achievedGoals = new std::vector<int> (*pcrb.getAchievedGoals ());
    }
}

void PiecewiseConstantReward::leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
                                                           double *low, double *high)
{
  const PiecewiseConstantReward &pcra = static_cast<const PiecewiseConstantReward&> (bt);
  const PiecewiseConstantReward &pcrb = static_cast<const PiecewiseConstantReward&> (btr);

  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> (1);
  else delete (*m_alphaVectors)[0];
  
  if (pcra.getAlphaVectors () && pcrb.getAlphaVectors ())
    AlphaVector::simpleSumAlphaVectors (*pcra.getAlphaVectors (), *pcrb.getAlphaVectors (), m_alphaVectors);
  else if (pcra.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcra.getAlphaVectorNth (0));
  else if (pcrb.getAlphaVectors ())
    (*m_alphaVectors)[0] = new AlphaVector (*pcrb.getAlphaVectorNth (0));
  else { delete m_alphaVectors; m_alphaVectors = 0; }

  /* unionize goal sets */
  unionGoalSets (pcra, pcrb);
}

void PiecewiseConstantReward::leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
                                                           double *low, double *high)
{
  const PiecewiseConstantReward &pcra = static_cast<const PiecewiseConstantReward&> (bt);
  const PiecewiseConstantReward &pcrb = static_cast<const PiecewiseConstantReward&> (btr);

  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> ();
  if (pcra.getAlphaVectors () && pcrb.getAlphaVectors ())
    AlphaVector::simpleSubtractAlphaVectors (*pcra.getAlphaVectors (), *pcrb.getAlphaVectors (), m_alphaVectors);
  else if (pcra.getAlphaVectors ())
    {
      AlphaVector *av = new AlphaVector (*pcra.getAlphaVectorNth (0));
      av->multiplyByScalar (-1.0);
      m_alphaVectors->push_back (av);
    }
  else if (pcrb.getAlphaVectors ())
    {
      AlphaVector *av = new AlphaVector (*pcrb.getAlphaVectorNth (0));
      av->multiplyByScalar (-1.0);
      m_alphaVectors->push_back (av);
    }
  else { delete m_alphaVectors; m_alphaVectors = 0; }

  /* unionize goal sets */
  unionGoalSets (pcra, pcrb);
}

void PiecewiseConstantReward::mergeContiguousLeaves (ContinuousReward *root, ContinuousReward *lt,
						     ContinuousReward *ge)
{
  PiecewiseConstantReward *pcrlt = static_cast<PiecewiseConstantReward*> (lt);
  PiecewiseConstantReward *pcrge = static_cast<PiecewiseConstantReward*> (ge);
  
  /* get constants */
  double c1 = pcrlt->getConstantValue ();
  double c2 = pcrge->getConstantValue ();
  
  if ((BspTreeOperations::m_piecesMergingByValue
       && Alg::REqual (c1, c2, Alg::m_doubleEpsilon))     /* merge by value only. */
      || (BspTreeOperations::m_piecesMergingEquality
	  && Alg::REqual (c1, c2, Alg::m_doubleEpsilon)
	  && (pcrlt->getAchievedGoals () && pcrge->getAchievedGoals ())
	  && (Alg::eqVectorInt (*pcrlt->getAchievedGoals (),
				*pcrge->getAchievedGoals ()))))
    {
      /* transfer data to root and delete leaves. */
      root->transferData (*pcrlt);
      delete lt; lt = 0;
      delete ge; ge = 0;
      root->setLowerTree (0);
      root->setGreaterTree (0);
    }
}

/* stuff for asymetric operators. Warning: this applies to the constant case only (for now). */
double PiecewiseConstantReward::updateSubTreeMaxValue ()
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
      PiecewiseConstantReward *crlt 
	= static_cast<PiecewiseConstantReward*> (getLowerTree ());
      double low_max = crlt->updateSubTreeMaxValue ();
      PiecewiseConstantReward *crge 
	= static_cast<PiecewiseConstantReward*> (getGreaterTree ());
      double high_max = crge->updateSubTreeMaxValue ();
      m_maxValue = std::max (low_max, high_max);
      return m_maxValue;
    }
}

/* plot of a leaf uses a vrml box */
void PiecewiseConstantReward::plot2DVrml2Leaf (const double &space, std::ofstream &output_vrml, 
					       double *low, double *high,
					       const double &scale0, const double &scale1,
					       const double &max_value)
{
  if (! m_alphaVectors)
    return;
  
  double width0 = (high[0]-low[0])/scale0, width1 = (high[1]-low[1])/scale1;
  double pos0 = (high[0]+low[0])/(2.0 * scale0), pos1 = (high[1]+low[1])/(2.0 * scale1);
  output_vrml << "\t\tTransform {\n\t\t\ttranslation "
	      << pos0 << ' ' << -getConstantValue () / 2.0  << ' ' << pos1 << ' '
	      << "\n\t\t\tchildren [\n\t\t\t\tShape {\n\t\t\t\t\tappearance Appearance {material Material {} }\n\t\t\t\t\tgeometry Box {size " 
	      << width0-space << ' ' << getConstantValue () << ' ' << width1-space << "}\n\t\t\t}\n\t\t]\n\t}\n";
}

} /* end of namespace */
