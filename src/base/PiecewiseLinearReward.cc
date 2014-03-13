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

#include "PiecewiseLinearReward.h"

namespace hmdp_base
{

PiecewiseLinearReward::PiecewiseLinearReward (const int &sdim)
  : ContinuousReward (0, sdim)
{
  m_bspType = PiecewiseLinearRewardT;
}

PiecewiseLinearReward::PiecewiseLinearReward (const int &sdim, const int &d, const double &pos)
  : ContinuousReward (0, sdim, d, pos)
{
  m_bspType = PiecewiseLinearRewardT;
}

PiecewiseLinearReward::PiecewiseLinearReward (const int &dimension, const int &sdim)
  : ContinuousReward (dimension, sdim)
{
  m_bspType = PiecewiseLinearRewardT;
}

PiecewiseLinearReward::PiecewiseLinearReward (const int &dimension, const int &sdim, 
					      const int &d, const double &pos)
  : ContinuousReward (dimension, sdim, d, pos)
{
  m_bspType = PiecewiseLinearRewardT;
}

PiecewiseLinearReward::PiecewiseLinearReward (const int &dimension, const int &sdim, double **lowPos, 
					      double **highPos, double *low, double *high, 
					      int *linNum, double ***value)
  : ContinuousReward (dimension, sdim)
{
  m_bspType = PiecewiseLinearRewardT;

  /* create a bsp tree from the transition tiling:
     create a tree for each tile, and intersect them. */
  PiecewiseLinearReward *bsp_n = this;
  PiecewiseLinearReward *bsp_c = this;
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
	      std::cerr << "[Warning]: PiecewiseLinearReward constructor: upper bound is outside the domain in tile: " 
			<< tiles << ".\n";
	      highPos[tiles][i] = high[i];
	    }

	  bsp_n->setPosition (highPos[tiles][i]);

	  /* check if lower bound belongs to the domain. */
	  if (lowPos[tiles][i] < low[i])
	    {
	      std::cerr << "[Warning]: PiecewiseLinearReward constructor: lower bound is outside the domain in tile: "
			<< tiles << ".\n";
	      lowPos[tiles][i] = low[i];
	    }

	  bsp_n->setLowerTree (new PiecewiseLinearReward (m_tilingDimension, m_nDim, i, lowPos[tiles][i]));
	  bsp_n->setGreaterTree (new PiecewiseLinearReward (m_tilingDimension, m_nDim));
	  bsp_n = dynamic_cast<PiecewiseLinearReward*> (bsp_n->getLowerTree ());
	  
	  /* lower bound */
	  bsp_n->setLowerTree (new PiecewiseLinearReward (m_tilingDimension, m_nDim));
	  bsp_n->setGreaterTree (new PiecewiseLinearReward (m_tilingDimension, m_nDim));
	  bsp_n = dynamic_cast<PiecewiseLinearReward*> (bsp_n->getGreaterTree ());
	  
	  if (i == m_nDim - 1)  /* we now can fill the leaf (corresponds to a single tile) */
	    {
	      for (int j=0; j<linNum[tiles]; j++) /* iterates linear functions */
		{
		  AlphaVector *av = new AlphaVector (m_nDim + 1, value[tiles][j]);
		  bsp_n->addAVector (av);
		  
		  //debug
		  //std::cout << "Added alpha vector: " << *av << std::endl;
		  //debug
		}
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
	    setLowerTree (new PiecewiseLinearReward (*dynamic_cast<PiecewiseLinearReward*> (intersectedTree->getLowerTree ())));
	  if (intersectedTree->getGreaterTree ())
	    setGreaterTree (new PiecewiseLinearReward (*dynamic_cast<PiecewiseLinearReward*> (intersectedTree->getGreaterTree ())));
	  BspTree::deleteBspTree (intersectedTree);
	}
      if (tiles < m_tilingDimension - 1) 
	{
	  bsp_c = new PiecewiseLinearReward (m_tilingDimension, m_nDim); 
	  bsp_n = bsp_c;
	}
    } /* end for tiles */
}

PiecewiseLinearReward::PiecewiseLinearReward (const PiecewiseLinearReward &cr)
  : ContinuousReward (cr.getTilingDimension (), cr.getSpaceDimension (), cr.getDimension (), cr.getPosition ())
{
  m_bspType = PiecewiseLinearRewardT;

  /* copy alpha vectors */
  if (cr.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> ();
      for (unsigned int i=0; i<cr.getAlphaVectorsSize (); i++)
	{
	  m_alphaVectors->push_back (new AlphaVector (*cr.getAlphaVectorNth (i)));
	  std::cout << "copied alpha vectors: " << *cr.getAlphaVectorNth (i) << std::endl;
	}
    }
  
  if (cr.getAchievedGoals ())
    {
      m_achievedGoals = new std::vector<int> (*cr.getAchievedGoals ());
    }
  
  if (! cr.isLeaf ())
    {
      PiecewiseLinearReward *blt = static_cast<PiecewiseLinearReward*> (cr.getLowerTree ());
      m_lt = new PiecewiseLinearReward (*blt);
      PiecewiseLinearReward *bge = static_cast<PiecewiseLinearReward*> (cr.getGreaterTree ());
      m_ge = new PiecewiseLinearReward (*bge);
    }
  else m_lt = m_ge = 0;
}

PiecewiseLinearReward::PiecewiseLinearReward (const int &dimension, const BspTree &bt)
  : ContinuousReward (dimension, bt.getSpaceDimension (), bt.getDimension (), bt.getPosition ())
{
  m_bspType = PiecewiseLinearRewardT;

  if (! bt.isLeaf ())
    {
      m_lt = new PiecewiseLinearReward (0, *bt.getLowerTree ());
      m_ge = new PiecewiseLinearReward (0, *bt.getGreaterTree ());
    }
  else m_lt = m_ge = 0;

}

PiecewiseLinearReward::~PiecewiseLinearReward ()
{}

void PiecewiseLinearReward::leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
						   double *low, double *high)
{
  const PiecewiseLinearReward &plra = static_cast<const PiecewiseLinearReward&> (bt);
  const PiecewiseLinearReward &plrb = static_cast<const PiecewiseLinearReward&> (btr);

  if (plra.getAlphaVectors () && plrb.getAlphaVectors ())
    {
      if (! m_alphaVectors)
        m_alphaVectors = new std::vector<AlphaVector*> ();
      AlphaVector::crossSumAlphaVectors (*plra.getAlphaVectors (), *plrb.getAlphaVectors (), 
					 low, high, m_alphaVectors);
    }

  /* unionize goal sets */
  unionGoalSets (plra, plrb);
}

void PiecewiseLinearReward::leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
						    double *low, double *high)
{
  const PiecewiseLinearReward &plra = static_cast<const PiecewiseLinearReward&> (bt);
  const PiecewiseLinearReward &plrb = static_cast<const PiecewiseLinearReward&> (btr);

  if (plra.getAlphaVectors () && plrb.getAlphaVectors ())
    {
      if (! m_alphaVectors)
        m_alphaVectors = new std::vector<AlphaVector*> ();
      AlphaVector::crossSubtractAlphaVectors (*plra.getAlphaVectors (), *plrb.getAlphaVectors (), 
					      low, high, m_alphaVectors);
    }

  /* unionize goal sets */
  unionGoalSets (plra, plrb);
}


} /* end of namespace */
