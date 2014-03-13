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

#include "ContinuousTransition.h"
#include "BspTreeOperations.h"
#include <stdlib.h>

//#define DEBUG 1

namespace hmdp_base
{

ContinuousTransition::ContinuousTransition (const int &sdim)
  : BspTree (sdim), m_tilingDimension (0), m_jdd (0), m_relative (0),
    m_shiftedProbabilisticOutcomes (0), m_projectedProbabilisticOutcomes (0),
    m_numberContinuousOutcomes (0), m_numberProjectedContinuousOutcomes (0),
    m_ptrToTiles (0), m_nTile (-1), m_ctpwcVF (0), m_ctpwlVF (0), m_ctCSD (0)
{
  m_bspType = ContinuousTransitionT;
}

ContinuousTransition::ContinuousTransition (const int &sdim, const int &d, const double &pos)
  : BspTree (sdim, d, pos), m_tilingDimension (0), m_jdd (0), m_relative (0),
    m_shiftedProbabilisticOutcomes (0), m_projectedProbabilisticOutcomes (0),
    m_numberContinuousOutcomes (0), m_numberProjectedContinuousOutcomes (0),
    m_ptrToTiles (0), m_nTile (-1), m_ctpwcVF (0), m_ctpwlVF (0), m_ctCSD (0)
{
  m_bspType = ContinuousTransitionT;
}

ContinuousTransition::ContinuousTransition (const int &dimension, const int &sdim)
  : BspTree (sdim), m_tilingDimension (dimension), m_jdd (0), m_relative (0),
    m_shiftedProbabilisticOutcomes (0), m_projectedProbabilisticOutcomes (0),
    m_numberContinuousOutcomes (0), m_numberProjectedContinuousOutcomes (0),
    m_ptrToTiles (0), m_nTile (-1), m_ctpwcVF (0), m_ctpwlVF (0), m_ctCSD (0)
{
  m_bspType = ContinuousTransitionT;
}

ContinuousTransition::ContinuousTransition (const int &dimension, const int &sdim, 
					    const int &d, const double &pos)
  : BspTree (sdim, d, pos), m_tilingDimension (dimension), m_jdd (0), m_relative (0),
    m_shiftedProbabilisticOutcomes (0), m_projectedProbabilisticOutcomes (0),
    m_numberContinuousOutcomes (0), m_numberProjectedContinuousOutcomes (0),
    m_ptrToTiles (0), m_nTile (-1), m_ctpwcVF (0), m_ctpwlVF (0), m_ctCSD (0)
{
  m_bspType = ContinuousTransitionT;
}

ContinuousTransition::ContinuousTransition (const int &dimension, const int &sdim, 
					    const discretizationType dt,
					    double **lowPos, double **highPos,
					    double *low, double *high,
					    const double **epsilon, const double **intervals,
					    const double **means, const double **sds, bool **relative,
					    const discreteDistributionType **distrib)
  : BspTree (sdim), m_tilingDimension (dimension), m_jdd (0), m_relative (0),
    m_shiftedProbabilisticOutcomes (0), m_projectedProbabilisticOutcomes (0),
    m_numberContinuousOutcomes (0), m_numberProjectedContinuousOutcomes (0),
    m_ptrToTiles (0), m_nTile (-1), m_ctpwcVF (0), m_ctpwlVF (0), m_ctCSD (0)
{
  m_bspType = ContinuousTransitionT;
  
  DiscreteDistribution **dds = new DiscreteDistribution*[m_nDim];
  
  m_relative = new bool*[dimension];
  for (int i=0;i<dimension;i++)
    {
      m_relative[i] = new bool[sdim];
      for (int j=0;j<sdim;j++)
	m_relative[i][j]=relative[i][j];
    }

  /* create a bsp tree from the transition tiling:
     create a tree for each tile, and intersect them. */
  ContinuousTransition *bsp_n = this;
  ContinuousTransition *bsp_c = this;
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
	      std::cerr << "[Warning]: ContinuousTransition constructor: upper bound is outside the domain in tile: " 
			<< tiles << ".\n";
	      highPos[tiles][i] = high[i];
	    }

	  bsp_n->setPosition (highPos[tiles][i]);
	  
	  /* check if lower bound belongs to the domain. */
	  if (lowPos[tiles][i] < low[i])
	    {
	      std::cerr << "[Warning]: ContinuousTransition constructor: lower bound is outside the domain in tile: "
			<< tiles << ".\n";
	      lowPos[tiles][i] = low[i];
	    }
	  
	  bsp_n->setLowerTree (new ContinuousTransition (m_tilingDimension, m_nDim, i, lowPos[tiles][i]));
	  bsp_n->setGreaterTree (new ContinuousTransition (m_tilingDimension, m_nDim));
	  bsp_n = static_cast<ContinuousTransition*> (bsp_n->getLowerTree ());
	  
	  /* lower bound */
	  bsp_n->setLowerTree (new ContinuousTransition (m_tilingDimension, m_nDim));
	  bsp_n->setGreaterTree (new ContinuousTransition (m_tilingDimension, m_nDim));
	  bsp_n = static_cast<ContinuousTransition*> (bsp_n->getGreaterTree ());
	  
	  /* discretize distribution for this tile, for this dimension */
	  if (distrib[tiles][i] == NONE)  /* deterministic */
	    {
	      double *determ = new double[1];
	      determ[0] = means[tiles][i];
	      double *prob_one = new double[1];
	      prob_one[0] = 1.0;
	      DiscreteDistribution *dd 
		= new DiscreteDistribution (NONE, 1, high[i] - low[i], determ, prob_one);
	      delete []determ; delete []prob_one;
	      dds[i] = dd;
	      //std::cout << "dds: " << *dds[i] << std::endl;
	    }
	  else if (distrib[tiles][i] == GAUSSIAN)
	    {
	      NormalDiscreteDistribution *ndd;
	      if (dt != DISCRETIZATION_WRT_POINTS)
		ndd = new NormalDiscreteDistribution (means[tiles][i], sds[tiles][i],
						      epsilon[tiles][i], intervals[tiles][i], dt);
	      else ndd = new NormalDiscreteDistribution (means[tiles][i], sds[tiles][i],
							 epsilon[tiles][i], lround (intervals[tiles][i]));
	      /* in that last case, intervals contains the number of points... */
	      dds[i] = ndd;

#ifdef DEBUG
	      std::cout << *dds[i] << std::endl;
#endif
	    }
	  else if (distrib[tiles][i] == UNIFORM)
	    {
	      /* TODO */
	    }

	  if (i == m_nDim - 1)  /* we now can fill the leaf (corresponds to one tile) */
	    {
	      /* joint distribution over all the dimensions */
	      MDDiscreteDistribution *mdd = MDDiscreteDistribution::jointDiscreteDistribution (m_nDim, dds);
	      /* need to sum to 1. */
	      mdd->normalize (mdd->getProbMass ());

	      bsp_n->setLeafDistribution (mdd);

#ifdef DEBUG
	      std::cout << "mdd: " << *mdd << std::endl;
#endif

	      for (int k=0; k<m_nDim; k++)
		delete dds[k];
	      
	      /* create the cache of continuous outcomes as bsp trees. */
	      int nOutcomes = mdd->getNPoints ();
	      bsp_n->setLeafContinuousOutcomes (ContinuousOutcome::convertMDDiscreteDistribution (lowPos[tiles], highPos[tiles], low, high, *mdd, relative[tiles], nOutcomes));
	      bsp_n->setNContinuousOutcomes (nOutcomes);
	      nOutcomes = mdd->getNPoints ();
	      bsp_n->setLeafProjectedContinuousOutcomes (ContinuousOutcome::convertOppositeMDDiscreteDistribution (low, high, low, high, *mdd, relative[tiles], nOutcomes));
	      bsp_n->setNProjectedContinuousOutcomes (nOutcomes);
	      bsp_n->setNTile (tiles); /* reference tile index */
	    }
	}
      if (tiles > 0)
	{
	  BspTree *intersectedTree = BspTreeOperations::intersectTrees (this, bsp_c, lowPos[tiles], highPos[tiles]);
	  BspTree::deleteBspTree (bsp_c); bsp_c = 0; 
	  if (m_lt) BspTree::deleteBspTree (m_lt);
	  if (m_ge) BspTree::deleteBspTree (m_ge);
	  m_lt = m_ge = 0;

	  /* copy subtrees because C++ prevents from replacing 'this' */
	  if (intersectedTree->getLowerTree ())
	    setLowerTree (new ContinuousTransition (*static_cast<ContinuousTransition*> (intersectedTree->getLowerTree ())));
	  if (intersectedTree->getGreaterTree ())
	    setGreaterTree (new ContinuousTransition (*static_cast<ContinuousTransition*> (intersectedTree->getGreaterTree ())));
	  BspTree::deleteBspTree (intersectedTree);
	  intersectedTree = 0;
	}
      if (tiles < m_tilingDimension - 1) 
	{
	  bsp_c = new ContinuousTransition (m_tilingDimension, m_nDim); 
	  bsp_n = bsp_c;
	}
    } /* end for tiles */
  
  delete[] dds;

  selfReferenceTiles (this, 0);  /* auto-reference tiles trees at root tree level. */
  m_ctpwcVF = new PiecewiseConstantValueFunction (*this, 0.0); /* cache */
  m_ctpwlVF = new PiecewiseLinearValueFunction (*this);
  m_ctCSD = new ContinuousStateDistribution (*this);
}

ContinuousTransition::ContinuousTransition (const ContinuousTransition &ct)
  : BspTree (ct.getSpaceDimension (), ct.getDimension (), ct.getPosition ()),
    m_tilingDimension (ct.getTilingDimension ()), m_jdd (0), m_relative (0),
    m_shiftedProbabilisticOutcomes (0), 
    m_numberContinuousOutcomes (ct.getNContinuousOutcomes ()), 
    m_numberProjectedContinuousOutcomes (ct.getNProjectedContinuousOutcomes ()),
    m_ptrToTiles (0), m_nTile (ct.getNTile ()), m_ctpwcVF (0), m_ctpwlVF (0),
    m_ctCSD (0)
{
  m_bspType = ContinuousTransitionT;
  
  /* copy multi-dimensional distribution */
  if (ct.getLeafDistribution ())
    m_jdd = new MDDiscreteDistribution (*ct.getLeafDistribution ());

  /* copy relative */
  if (ct.getRelative ())
    {
      m_relative = new bool*[m_tilingDimension];
      for (int i=0; i<m_tilingDimension; i++)
	{
	  m_relative[i] = new bool[m_nDim];
	  for (int j=0; j<m_nDim; j++)
	    m_relative[i][j] = ct.getRelative (i,j);
	}
    }

  /* copy shifted outcomes */
  if (ct.getContinuousOutcomes ())
    {
      m_shiftedProbabilisticOutcomes = new ContinuousOutcome*[ct.getNContinuousOutcomes()];
      m_projectedProbabilisticOutcomes = new ContinuousOutcome*[ct.getNContinuousOutcomes()];
      for (int i=0; i<ct.getNContinuousOutcomes (); i++)
	{
	  m_shiftedProbabilisticOutcomes[i] = new ContinuousOutcome (*ct.getContinuousOutcome (i));
	  m_projectedProbabilisticOutcomes[i] = new ContinuousOutcome (*ct.getProjectedContinuousOutcome (i));
	}
    }

  /* copy cached value functions */
  if (ct.getPtrPwcVF () && ct.getPtrPwlVF () && ct.getPtrCSD ())
    {
      m_ctpwcVF = new PiecewiseConstantValueFunction (*ct.getPtrPwcVF ());
      m_ctpwlVF = new PiecewiseLinearValueFunction (*ct.getPtrPwlVF ());
      m_ctCSD = new ContinuousStateDistribution (*ct.getPtrCSD ());
    }

  if (! ct.isLeaf ())
    {
      ContinuousTransition *blt = static_cast<ContinuousTransition*> (ct.getLowerTree ());
      m_lt = new ContinuousTransition (*blt);
      ContinuousTransition *bge = static_cast<ContinuousTransition*> (ct.getGreaterTree ());
      m_ge = new ContinuousTransition (*bge);
    }
  else m_lt = m_ge = 0;

  selfReferenceTiles (this, 0);
}

/* if a tree is copied from the 'root' class, then each node has to belong 
   to the current class */
ContinuousTransition::ContinuousTransition (const int &dimension, const BspTree &bt)
  : BspTree (bt.getSpaceDimension (), bt.getDimension (), bt.getPosition ()), 
    m_tilingDimension (dimension), m_jdd (0), m_relative (0),
    m_shiftedProbabilisticOutcomes (0), m_projectedProbabilisticOutcomes (0),
    m_numberContinuousOutcomes (0), m_numberProjectedContinuousOutcomes (0),
    m_ptrToTiles (0), m_nTile (-1), m_ctpwcVF (0), m_ctpwlVF (0), m_ctCSD (0)
{
  m_bspType = ContinuousTransitionT;

  if (! bt.isLeaf ())
    {
      m_lt = new ContinuousTransition (0, *bt.getLowerTree ());
      m_ge = new ContinuousTransition (0, *bt.getGreaterTree ());
    }
  else m_lt = m_ge = 0;
}

ContinuousTransition::~ContinuousTransition ()
{
  if (m_jdd)
    delete m_jdd;
  m_jdd = 0;
  
  if (m_relative)
    {
      for (int i=0; i<m_tilingDimension; i++)
	delete[] m_relative[i];
      delete[] m_relative;
      m_relative = 0;
    }

  if (m_shiftedProbabilisticOutcomes)
    {
      for (int j=0; j<getNContinuousOutcomes (); j++)
	{
	  BspTree::deleteBspTree(m_shiftedProbabilisticOutcomes[j]);
	}
      delete[] m_shiftedProbabilisticOutcomes;
      m_shiftedProbabilisticOutcomes = 0;
    }

  if (m_projectedProbabilisticOutcomes)
    {
      for (int j=0; j<getNContinuousOutcomes (); j++)
	{
	  BspTree::deleteBspTree(m_projectedProbabilisticOutcomes[j]);
	}
      if (m_numberProjectedContinuousOutcomes) // XXX: bug, sometimes the pointer isn't null.
	delete[] m_projectedProbabilisticOutcomes;
      m_projectedProbabilisticOutcomes = 0;
    }

  if (m_ptrToTiles)
    {
      delete[] m_ptrToTiles;
      m_ptrToTiles = 0;
    }

  if (m_ctpwcVF && m_ctpwlVF && m_ctCSD)
    {
      BspTree::deleteBspTree(m_ctpwcVF); 
      BspTree::deleteBspTree(m_ctpwlVF); 
      BspTree::deleteBspTree(m_ctCSD);
      m_ctpwcVF = 0;
      m_ctpwlVF = 0;
      m_ctCSD = 0;
    }
}

void ContinuousTransition::selfReferenceTiles (ContinuousTransition *ct, 
					       ContinuousTransition **ptrref)
{
  if (! m_tilingDimension && ! ptrref)
      return;
  
  if (m_tilingDimension && ! ptrref)
    {
      m_ptrToTiles = new ContinuousTransition*[m_tilingDimension]();
      ptrref = m_ptrToTiles;
    }
  
  if (ct->isLeaf () && ct->getNTile () != -1)
    ptrref [ct->getNTile ()] = ct;
  else if (! ct->isLeaf ())
    {
      ContinuousTransition *cgt = static_cast<ContinuousTransition*> (ct->getGreaterTree ());
      selfReferenceTiles (cgt, ptrref);
      ContinuousTransition *clt = static_cast<ContinuousTransition*> (ct->getLowerTree ());
      selfReferenceTiles (clt, ptrref);
    }
}

/* virtual functions */
void ContinuousTransition::leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
						  double *low, double *high) {
  const ContinuousTransition &cta = static_cast<const ContinuousTransition&> (bt);
  const ContinuousTransition &ctb = static_cast<const ContinuousTransition&> (btr);

  if (cta.getLeafDistribution () && ctb.getLeafDistribution ())
    {
      std::cerr << "[Error]: ContinuousTransition::leafDataIntersectInit: Both cells have distributions ! Exiting...\n";
      exit (1);
    }
  else if (cta.getLeafDistribution ())
    {
      m_jdd = new MDDiscreteDistribution (*cta.getLeafDistribution ());
      m_nTile = cta.getNTile ();
      m_numberContinuousOutcomes = cta.getNContinuousOutcomes ();
      m_numberProjectedContinuousOutcomes = cta.getNProjectedContinuousOutcomes ();
      m_shiftedProbabilisticOutcomes = new ContinuousOutcome*[getNContinuousOutcomes()];
      m_projectedProbabilisticOutcomes = new ContinuousOutcome*[getNContinuousOutcomes()];
      
      for (int i=0; i<getNContinuousOutcomes (); i++)
	{
	  m_shiftedProbabilisticOutcomes[i] = new ContinuousOutcome (*cta.getContinuousOutcome (i));
	  m_projectedProbabilisticOutcomes[i] = new ContinuousOutcome (*cta.getProjectedContinuousOutcome (i));
	}
    }
  else if (ctb.getLeafDistribution ())
    {
      m_jdd = new MDDiscreteDistribution (*ctb.getLeafDistribution ());
      m_nTile = ctb.getNTile ();
      m_numberContinuousOutcomes = ctb.getNContinuousOutcomes ();
      m_numberProjectedContinuousOutcomes = ctb.getNProjectedContinuousOutcomes ();
      m_shiftedProbabilisticOutcomes = new ContinuousOutcome*[getNContinuousOutcomes()];
      m_projectedProbabilisticOutcomes = new ContinuousOutcome*[getNContinuousOutcomes()];
      
      for (int i=0; i<getNContinuousOutcomes (); i++)
	{
	  m_shiftedProbabilisticOutcomes[i] = new ContinuousOutcome (*ctb.getContinuousOutcome (i));
	   m_projectedProbabilisticOutcomes[i] = new ContinuousOutcome (*ctb.getProjectedContinuousOutcome (i));
	}
    }
}

void ContinuousTransition::transferData (const BspTree &bt) {
  if (m_jdd)
    delete m_jdd; /* secure... */
  const ContinuousTransition &ct = static_cast<const ContinuousTransition&> (bt); /* not secure... */
  if (ct.getLeafDistribution ())
    {
      m_numberContinuousOutcomes = ct.getNContinuousOutcomes ();
      m_numberProjectedContinuousOutcomes = ct.getNProjectedContinuousOutcomes ();
      if (m_jdd) delete m_jdd;
      m_jdd = new MDDiscreteDistribution (*ct.getLeafDistribution ());
      if (m_shiftedProbabilisticOutcomes)
	{
	  for (int j=0; j<getNContinuousOutcomes (); j++)
	    {
	      delete m_shiftedProbabilisticOutcomes[j];
	      delete m_projectedProbabilisticOutcomes[j];
	    }	  
	  delete[] m_shiftedProbabilisticOutcomes;
	  delete[] m_projectedProbabilisticOutcomes;
	}
      m_shiftedProbabilisticOutcomes = new ContinuousOutcome*[getNContinuousOutcomes()];
      m_projectedProbabilisticOutcomes = new ContinuousOutcome*[getNContinuousOutcomes()];
      
      for (int i=0; i<getNContinuousOutcomes (); i++)
	{
	  m_shiftedProbabilisticOutcomes[i] = new ContinuousOutcome (*ct.getContinuousOutcome (i));
	  m_projectedProbabilisticOutcomes[i] = new ContinuousOutcome (*ct.getProjectedContinuousOutcome (i));
	}
    }
  m_nTile = ct.getNTile ();
}

/* parameter must be initialized to 'true'. */
void ContinuousTransition::hasZeroConsumption (bool &isNullOutcome)
{
  if (isLeaf ())
    {
      if (getNTile () != -1)
	{
	  if (getLeafDistribution ()->getNPoints () > 1)
	    {
	      isNullOutcome = false;
	      return;
	    }
	  
	  /* only a single point. */
	  for (int d=0; d<m_nDim; d++)
	    if (getLeafDistribution ()->getPosition (0, d) != 0.0)
	      {
	        isNullOutcome = false;
		break;
	      }
	}
    }
  else if (isNullOutcome)
    {
      static_cast<ContinuousTransition*> (getGreaterTree ())->hasZeroConsumption (isNullOutcome);
      if (! isNullOutcome)
	return;
      static_cast<ContinuousTransition*> (getLowerTree ())->hasZeroConsumption (isNullOutcome);
    }
}

/* printing */
void ContinuousTransition::print (std::ostream &out, double *low, double *high)
{
  if (isLeaf ())
    {
      if (getNTile () != -1)
	{
	  /* other information */
	  out << "Tile " << getNTile () << ":";
	  out << "{";
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      out << "[" << low[i] << "," << high[i];
	      if (i == getSpaceDimension () - 1)
		out << "]}";
	      else out << "],";
	    }
	  out << "\n\tdistribution points: " << getLeafDistribution ()->getNPoints ()
	      << "\n\tnumber of outcomes: " << getNContinuousOutcomes ()
	      << std::endl;
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<ContinuousTransition*> (getGreaterTree ())->print (out, low, high);
      low[getDimension ()] = b;
      
      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<ContinuousTransition*> (getLowerTree ())->print (out, low, high);
      high[getDimension ()] = b;
    }
}

} /* end of namespace */
