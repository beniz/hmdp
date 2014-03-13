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

#include "ContinuousStateDistribution.h"
#include "BspTreeOperations.h"
#include "HybridTransition.h"
#include "ContinuousTransition.h"
#include "ValueFunction.h"
#include <assert.h>
#include <stdlib.h>

namespace hmdp_base
{

double ContinuousStateDistribution::m_doubleProbaPrecision = 1e-10;

ContinuousStateDistribution::ContinuousStateDistribution (const int &sdim)
  : BspTree (sdim), m_tilingDimension (0), m_probability (-1.0) 
{
  m_bspType = ContinuousStateDistributionT;
}

ContinuousStateDistribution::ContinuousStateDistribution (const int &sdim, const int &d, const double &pos)
  : BspTree (sdim, d, pos), m_tilingDimension (0), m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;
}

ContinuousStateDistribution::ContinuousStateDistribution (const int &dimension, const int &sdim)
  : BspTree (sdim), m_tilingDimension (dimension), m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;
}

ContinuousStateDistribution::ContinuousStateDistribution (const int &dimension, const int &sdim,
							  const int &d, const double &pos)
  : BspTree (sdim, d, pos), m_tilingDimension (dimension), m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;
}

ContinuousStateDistribution::ContinuousStateDistribution (const int &dimension, const int &sdim,
							  double **lowPos, double **highPos,
							  double *low, double *high,
							  double *prob)
  : BspTree (sdim), m_tilingDimension (dimension), m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;
  
  /* create a bsp tree from the transition tiling:                                                    create a tree for each tile, and intersect them. */
  ContinuousStateDistribution *bsp_n = this;
  ContinuousStateDistribution *bsp_c = this;
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
	      highPos[tiles][i] = high[i];
	    }
	  
	  bsp_n->setPosition (highPos[tiles][i]);

	  /* check if lower bound belongs to the domain. */
          if (lowPos[tiles][i] < low[i])
            {
              lowPos[tiles][i] = low[i];
            }
	  
	  bsp_n->setLowerTree (new ContinuousStateDistribution (m_tilingDimension, m_nDim, i, lowPos[tiles][i]));
	  bsp_n->setGreaterTree (new ContinuousStateDistribution (m_tilingDimension, m_nDim));
	  bsp_n = static_cast<ContinuousStateDistribution*> (bsp_n->getLowerTree ());

	  /* lower bounds */
	  bsp_n->setLowerTree (new ContinuousStateDistribution (m_tilingDimension, m_nDim));
	  bsp_n->setGreaterTree (new ContinuousStateDistribution (m_tilingDimension, m_nDim));
	  bsp_n = static_cast<ContinuousStateDistribution*> (bsp_n->getGreaterTree ());

	  if (i == m_nDim - 1)  /* now we can fill the leaf (equiv. one tile) */
	    {
	      bsp_n->setProbability (prob[tiles]);
	    }
	}
      if (tiles > 0)
	{
	  BspTree *intersectedTree = BspTreeOperations::intersectTrees (this, bsp_c, low, high);
	  BspTree::deleteBspTree (bsp_c); bsp_c = 0;
	  if (m_lt) BspTree::deleteBspTree (m_lt);
	  if (m_ge) BspTree::deleteBspTree (m_ge);
	  m_lt = m_ge = 0;

	  /* copy subtrees because C++ prevents from replacing 'this' */
          if (intersectedTree->getLowerTree ())
	    setLowerTree (new ContinuousStateDistribution (*static_cast<ContinuousStateDistribution*> (intersectedTree->getLowerTree ())));
	  if (intersectedTree->getGreaterTree ())
	    setGreaterTree (new ContinuousStateDistribution (*static_cast<ContinuousStateDistribution*> (intersectedTree->getGreaterTree ())));
	  BspTree::deleteBspTree (intersectedTree);
	}
      if (tiles < m_tilingDimension - 1)
	{
	  bsp_c = new ContinuousStateDistribution (m_tilingDimension, m_nDim);
	  bsp_n = bsp_c;
	}
    } /* end for tiles */
}

ContinuousStateDistribution::ContinuousStateDistribution (const ContinuousStateDistribution &csd)
  : BspTree (csd.getSpaceDimension (), csd.getDimension (), csd.getPosition ()),
    m_tilingDimension (csd.getTilingDimension ()), m_probability (csd.getProbability ())
{
  m_bspType = ContinuousStateDistributionT;

  if (! csd.isLeaf ())
    {
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (csd.getLowerTree ());
      m_lt = new ContinuousStateDistribution (*csdlt);
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (csd.getGreaterTree ());
      m_ge = new ContinuousStateDistribution (*csdge);
    }
  else m_lt = m_ge = 0;
}

ContinuousStateDistribution::ContinuousStateDistribution (const ContinuousStateDistribution &csd,
							  double *low, double *high,
							  bool const &invert)
  : BspTree (csd.getSpaceDimension (), csd.getDimension (), csd.getPosition ()),
    m_tilingDimension (csd.getTilingDimension ())
{
  m_bspType = ContinuousStateDistributionT;

  if (! csd.isLeaf ())
    {
      double b = high[csd.getDimension ()];
      high[csd.getDimension ()] = csd.getPosition ();
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (csd.getLowerTree ());
      m_lt = new ContinuousStateDistribution (*csdlt, low, high, invert);
      high[csd.getDimension ()] = b;

      b = low[csd.getDimension ()];
      low[csd.getDimension ()] = csd.getPosition ();
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (csd.getGreaterTree ());
      m_ge = new ContinuousStateDistribution (*csdge, low, high, invert);
      low[csd.getDimension ()] = b;
    }
  else 
    {
      if (invert)
	{
	  if (csd.getProbability () > 0.0)
	    m_probability = 0.0;
	  else 
	    {
	      m_probability = 1.0;
	      for (int i=0; i<m_nDim; i++)
		m_probability *= (high[i] - low[i]);
	      if (m_probability != 0.0)
		m_probability = 1.0 / m_probability;
	      else m_probability = -1.0;  /* empty volume. */
	    }
	}
      m_lt = m_ge = 0;
    }
}

ContinuousStateDistribution::ContinuousStateDistribution (const ContinuousOutcome &co)
  : BspTree (co.getSpaceDimension (), co.getDimension (), co.getPosition ()),
    m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;
  
  if (co.getProbability () >= 0.0)
    {
      m_probability = co.getProbability ();
    }
  
  if (! co.isLeaf ())
    {
      ContinuousOutcome *colt = static_cast<ContinuousOutcome*> (co.getLowerTree ());
      m_lt = new ContinuousStateDistribution (*colt);
      ContinuousOutcome *coge = static_cast<ContinuousOutcome*> (co.getGreaterTree ());
      m_ge = new ContinuousStateDistribution (*coge);
    }
}

/* DEPRECATED ? */
ContinuousStateDistribution::ContinuousStateDistribution (const ContinuousTransition &ct)
  : BspTree (ct.getSpaceDimension (), ct.getDimension (), ct.getPosition ()),
    m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;

  if (ct.isLeaf ())
    {
      if (ct.getNTile () != -1)
	{
	  m_probability = 1.0;  /* probability is set to 1.0 because the resulting csd is 
				   later used for action precondition testing (in CAO*). */
	}
    }
  else 
    {
      ContinuousTransition *ctlt = static_cast<ContinuousTransition*> (ct.getLowerTree ());
      m_lt = new ContinuousStateDistribution (*ctlt);
      ContinuousTransition *ctge = static_cast<ContinuousTransition*> (ct.getGreaterTree ());
      m_ge = new ContinuousStateDistribution (*ctge);
    }
}

ContinuousStateDistribution::ContinuousStateDistribution (const ValueFunction &vf)
  : BspTree (vf.getSpaceDimension (), vf.getDimension (), vf.getPosition ()), m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;
  
  if (vf.isLeaf ())
    {
      if (vf.getAlphaVectors ())
	{
	  assert (vf.getAlphaVectorsSize () == 1);  /* we want an unique alpha vector for 
						       this kind of conversion. 
						       Could do better for pwl... */
	  AlphaVector *av = vf.getAlphaVectorNth (0);
	  double value = av->getAlphaNth (av->getSize () - 1);
	  assert (value <= 1.0);  /* we want a probability. */
	  m_probability = value;
	}
    }
  else
    {
      ValueFunction *vflt = static_cast<ValueFunction*> (vf.getLowerTree ());
      m_lt = new ContinuousStateDistribution (*vflt);
      ValueFunction *vfge = static_cast<ValueFunction*> (vf.getGreaterTree ());
      m_ge = new ContinuousStateDistribution (*vfge);
    }
}

ContinuousStateDistribution::ContinuousStateDistribution (const int &dimension, const BspTree &bt)
  : BspTree (bt.getSpaceDimension (), bt.getDimension (), bt.getPosition ()),
    m_tilingDimension (dimension), m_probability (-1.0)
{
  m_bspType = ContinuousStateDistributionT;

  if (! bt.isLeaf ())
    {
      m_lt = new ContinuousStateDistribution (0, *bt.getLowerTree ());
      m_ge = new ContinuousStateDistribution (0, *bt.getGreaterTree ());
    }
  else m_lt = m_ge = 0;
}

ContinuousStateDistribution::~ContinuousStateDistribution ()
{}

int ContinuousStateDistribution::countPositiveLeaves () const
{
  if (isLeaf () && getProbability () 
      > ContinuousStateDistribution::m_doubleProbaPrecision)
    {
      return 1;
    }
  else if (isLeaf ())
    return 0;
  else 
    {
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (getLowerTree ());
      int cnzl_lt = csdlt->countPositiveLeaves ();
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      int cnzl_ge = csdge->countPositiveLeaves ();
      
      return cnzl_lt + cnzl_ge;
    }
}

int ContinuousStateDistribution::countNonZeroLeaves () const
{
  if (isLeaf () && getProbability () != 0.0)
    {
      return 1;
    }
  else if (isLeaf ())
    return 0;
  else 
    {
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (getLowerTree ());
      int cnzl_lt = csdlt->countNonZeroLeaves ();
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      int cnzl_ge = csdge->countNonZeroLeaves ();
      
      return cnzl_lt + cnzl_ge;
    }
}

void ContinuousStateDistribution::leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
							 double *low, double *high)
{
  const ContinuousStateDistribution &csda = static_cast<const ContinuousStateDistribution&> (bt);
  const ContinuousStateDistribution &csdb = static_cast<const ContinuousStateDistribution&> (btr);

  if (csda.getProbability () >= 0.0 && csdb.getProbability () >= 0.0)
    {
      std::cerr << "[Error]: ContinuousStateDistribution::leafDataIntersectInit: Both cells have probability: " << csda.getProbability () << " -- " << csdb.getProbability () << ". Exiting...\n";
      exit (1);
    }
  else if (csda.getProbability () >= 0.0)
    m_probability = csda.getProbability ();
  else if (csdb.getProbability () >= 0.0)
    m_probability = csdb.getProbability ();
}

void ContinuousStateDistribution::transferData (const BspTree &bt)
{
  if (bt.getType () == ContinuousOutcomeT)
    {
      const ContinuousOutcome &co = static_cast<const ContinuousOutcome&> (bt);
      m_probability = co.getProbability ();
    }
  else
    {
      const ContinuousStateDistribution &csd = static_cast<const ContinuousStateDistribution&> (bt);
      m_probability = csd.getProbability ();
    }
}

ContinuousStateDistribution* ContinuousStateDistribution::convertMDDiscreteDistribution (const MDDiscreteDistribution &mdd,
											 double *low, double *high)
{
  double **lowPos = (double **) malloc (mdd.getNPoints () * sizeof (double *));
  double **highPos = (double **) malloc (mdd.getNPoints () * sizeof (double *));
  double *prob = (double *) malloc (mdd.getNPoints () * sizeof (double));

  /* we need to make tiles from the discrete distribution points. */
  for (int i=0; i<mdd.getNPoints (); i++)
    {
      lowPos[i] = (double*) malloc (mdd.getDimension () * sizeof (double));
      highPos[i] = (double*) malloc (mdd.getDimension () * sizeof (double));
      prob[i] = mdd.getProbability (i);
      for (int j=0; j<mdd.getDimension (); j++)
	{
	  lowPos[i][j] = mdd.getPosition (i, j) - mdd.getDiscretizationInterval (j) / 2.0;
	  highPos[i][j] = mdd.getPosition (i, j) + mdd.getDiscretizationInterval (j) / 2.0;
	}
    }

  ContinuousStateDistribution *res 
    = new ContinuousStateDistribution (mdd.getNPoints (), mdd.getDimension (), 
				       lowPos, highPos, low, high, prob);

  if (BspTreeOperations::m_piecesMerging)
    res->mergeTreeLeaves (low, high);

  free (prob);
  for (int i=0; i<mdd.getNPoints (); i++)
    {
      free (lowPos[i]); free (highPos[i]);
    }
  free (lowPos); free (highPos);
  
  return res;
}

ContinuousStateDistribution* ContinuousStateDistribution::frontUp (ContinuousStateDistribution *csd,
								   ContinuousTransition *ct,
								   double *low, double *high,
								   const double &scalar)
{

  //debug
  /* std::cout << "csd frontup: ";
     csd->print (std::cout, low, high); */
  //debug

  /* perform the convolutions */
  double lowPos[csd->getSpaceDimension ()]; double highPos[csd->getSpaceDimension ()];
  for (int i=0; i<csd->getSpaceDimension (); i++)
    {
      lowPos[i] = low[i];
      highPos[i] = high[i];
    }  
  
  ContinuousStateDistribution *projectedDistribution = 0;
  projectedDistribution = ContinuousStateDistribution::frontUp (csd, ct, lowPos, highPos, low, high, projectedDistribution);

  /* multiply by scalar */
  if (scalar != 1.0)
    projectedDistribution->multiplyByScalar (scalar);

  return projectedDistribution;
}

ContinuousStateDistribution* ContinuousStateDistribution::frontUp (ContinuousStateDistribution *csd,
								   ContinuousTransition *ct,
								   double *lowPos, double *highPos,
								   double *low, double *high,
								   ContinuousStateDistribution *res)
{
  if (ct->isLeaf ())
    {
      if (ct->getNTile () >= 0)
	{
	  //debug
	  //std::cout << "number of projected outcomes: " << ct->getNContinuousOutcomes () << std::endl;
	  //debug
	  
	  //debug
	  /* std::cout <<"csd tile:\n";
	     csd->print (std::cout, low, high);
	     std::cout << std::endl; */
	  //debug

	  ContinuousStateDistribution *csd_cr
	    = static_cast<ContinuousStateDistribution*> (BspTreeOperations::cropTree (csd, lowPos, highPos));

	  //debug
	  /* std::cout << "csd_cr in leaf: ";
	     csd_cr->print (std::cout, low, high);
	     std::cout << std::endl; */
      //debug

	  /* front up */
	  ContinuousStateDistribution *convTile
	    = ContinuousStateDistribution::frontUpOutcomes (0, ct->getNProjectedContinuousOutcomes ()-1, 
							    ct, csd_cr, lowPos, highPos, low, high);
	  BspTree::deleteBspTree (csd_cr);
	  
	  //debug
	  /* std::cout <<"convTile in leaf:\n";
	     convTile->print (std::cout, low, high);
	     double pmass = 0.0;
	     convTile->sumUpProbabilities (&pmass, low, high);
	     std::cout << "total probability mass: " << pmass << std::endl; */
	  //debug

	  if (res)
	    {
	      //debug
	      /* std::cout << "res before add:\n";
		 res->print (std::cout, low, high); */
	      //debug

	      ContinuousStateDistribution *intersectedTrees
		= ContinuousStateDistribution::addContinuousStateDistributions (convTile, res, low, high);
	      BspTree::deleteBspTree (res); BspTree::deleteBspTree (convTile);
	      res = intersectedTrees;

	      //debug
	      /* std::cout << "res after add:\n";
		 res->print (std::cout, low, high); */
	      //debug

	    }
	  else res = convTile;
	} /* end if tile */
      else /* state distribution on that tile remains unchanged. */
	{
	  //debug
	  /* std::cout <<"csd:\n";
	     csd->print (std::cout, low, high); */
	  //debug

	  /* check if the csd is within the tile bounds (otherwise the cropping returns
	     a 'collapsed' tree. */
	  ContinuousStateDistribution *csd_cr 
	    = static_cast<ContinuousStateDistribution*> (BspTreeOperations::cropTree (csd, lowPos, highPos));

	  //debug
	  /* std::cout <<"csd_cr:\n";
	     csd_cr->print (std::cout, low, high); */
	  //debug

	  if (res)
	    {
	      ContinuousStateDistribution *addedTrees
		= ContinuousStateDistribution::addContinuousStateDistributions (csd_cr, res, low, high);
	      BspTree::deleteBspTree (res); BspTree::deleteBspTree (csd_cr);
	      res = addedTrees;
	    }
	  else res = csd_cr;
	}
      return res;
    } /* end if leaf */
  else 
    {
      double b = highPos[ct->getDimension ()];
      highPos[ct->getDimension ()] = ct->getPosition ();
      ContinuousTransition *ctlt = static_cast<ContinuousTransition*> (ct->getLowerTree ());
      res = ContinuousStateDistribution::frontUp (csd, ctlt, lowPos, highPos, low, high, res);
      highPos[ct->getDimension ()] = b;

      b = lowPos[ct->getDimension ()];
      lowPos[ct->getDimension ()] = ct->getPosition ();
      ContinuousTransition *ctge = static_cast<ContinuousTransition*> (ct->getGreaterTree ());
      res = ContinuousStateDistribution::frontUp (csd, ctge, lowPos, highPos, low, high, res);
      lowPos[ct->getDimension ()] = b;

      return res;
    }
}
		
ContinuousStateDistribution* ContinuousStateDistribution::frontUpOutcomes (const int &h, const int &t,
									   ContinuousTransition *ct,
									   ContinuousStateDistribution *csd,
									   double *lowPos, double *highPos,
									   double *low, double *high)
{
  if (h == t)
    return ContinuousStateDistribution::frontUpSingleOutcome (csd, ct->getProjectedContinuousOutcome (h),
							      lowPos, highPos,
							      low, high);
  
  ContinuousStateDistribution *csd1 
    = ContinuousStateDistribution::frontUpOutcomes (h, (h+t)/2, ct, csd, lowPos, highPos, low, high);
  ContinuousStateDistribution *csd2 
    = ContinuousStateDistribution::frontUpOutcomes ((h+t)/2+1, t, ct, csd, lowPos, highPos, low, high);
  
  //debug
  /* std::cout << "csd1:\n";
     csd1->print (std::cout, low, high);
     std::cout << "csd2:\n";
     csd2->print (std::cout, low, high); */
  //debug

  ContinuousStateDistribution *ret 
    = ContinuousStateDistribution::addContinuousStateDistributions (csd1, csd2, low, high);
  
  //debug
  /* std::cout << "ret:\n";
     ret->print (std::cout, low, high); */
  //debug

  BspTree::deleteBspTree (csd1); BspTree::deleteBspTree (csd2);
  return ret;
}
	
ContinuousStateDistribution* ContinuousStateDistribution::frontUpSingleOutcome (ContinuousStateDistribution *csd,
										ContinuousOutcome *co,
										double *lowPos, double *highPos,
										double *low, double *high)
{
  //debug
  /* std::cout << "projected outcome: ";
     co->print (std::cout, low, high); */
  //debug
  
  /* intersect and multiply */
  BspTreeOperations::setIntersectionType (BTI_MULT);
  BspTree *bt = BspTreeOperations::intersectTrees (csd, co, low, high);
  ContinuousStateDistribution *piece = static_cast<ContinuousStateDistribution*> (bt);
  if (BspTreeOperations::m_piecesMerging)
    piece->mergeTreeLeaves (low, high);
  
  //debug
  /* std::cout << "piece: ";
     piece->print (std::cout, low, high); */
  //debug
  
  /* crop piece */
  ContinuousStateDistribution *cpiece
    = static_cast<ContinuousStateDistribution*> (BspTreeOperations::cropTree (piece, co->getLowPos (),
									      co->getHighPos ()));
  BspTree::deleteBspTree (piece);

  //debug
  /* std::cout << "cpiece1: ";
     cpiece->print (std::cout, low, high); */
  //debug
  
  /* shift the piece back */
  cpiece = static_cast<ContinuousStateDistribution*> (BspTreeOperations::shiftTree (cpiece, 
										    co->getShiftBack (),
										    low, high));
  //debug
  /* std::cout << "cpiece2: ";
     cpiece->print (std::cout, low, high); */
  //debug

  if (BspTreeOperations::m_piecesMerging)
    cpiece->mergeTreeLeaves (low, high);
  
  return cpiece;
}
							 
ContinuousStateDistribution* ContinuousStateDistribution::addContinuousStateDistributions (ContinuousStateDistribution *csd1,
											   ContinuousStateDistribution *csd2,
											   double *low, double *high)
{
  BspTreeOperations::setIntersectionType (BTI_PLUS);
  ContinuousStateDistribution *res
    = static_cast<ContinuousStateDistribution*> (BspTreeOperations::intersectTrees (csd1, csd2, low, high));
  if (BspTreeOperations::m_piecesMerging)
    res->mergeTreeLeaves (low, high);
  return res;
}

ContinuousStateDistribution* ContinuousStateDistribution::subtractContinuousStateDistributions (ContinuousStateDistribution *csd1,
												ContinuousStateDistribution *csd2,
												double *low, double *high)
{
  BspTreeOperations::setIntersectionType (BTI_MINUS);
  ContinuousStateDistribution *res
    = static_cast<ContinuousStateDistribution*> (BspTreeOperations::intersectTrees (csd1, csd2, low, high));
  if (BspTreeOperations::m_piecesMerging)
    res->mergeTreeLeaves (low, high);
  return res;
}

ContinuousStateDistribution* ContinuousStateDistribution::multContinuousStateDistributions (ContinuousStateDistribution *csd1,
											    ContinuousStateDistribution *csd2,
											    double *low, double *high)
{
  BspTreeOperations::setIntersectionType (BTI_MULT);
  ContinuousStateDistribution *res
    = static_cast<ContinuousStateDistribution*> (BspTreeOperations::intersectTrees (csd1, csd2, low, high));
  if (BspTreeOperations::m_piecesMerging)
    res->mergeTreeLeaves (low, high);
  return res;
}

ContinuousStateDistribution* ContinuousStateDistribution::diffContinuousStateDistributions (ContinuousStateDistribution *csd1,
											    ContinuousStateDistribution *csd2,
											    double *low, double *high)
{
  /* can't use the bsp balance. */
  bool isBalance = false;
  if (BspTreeOperations::m_bspBalance)
    {
      BspTreeOperations::m_bspBalance = false;
      isBalance = true;
    }
  BspTreeOperations::setIntersectionType (BTI_CSD_DIFF);
  ContinuousStateDistribution *res
    = static_cast<ContinuousStateDistribution*> (BspTreeOperations::intersectTrees (csd1, csd2, low, high));
  if (isBalance)
    BspTreeOperations::m_bspBalance = true;
  if (BspTreeOperations::m_piecesMerging)
    res->mergeTreeLeaves (low, high);
  return res;
}

bool ContinuousStateDistribution::compareContinuousStateDistributions (ContinuousStateDistribution *csd1,
								       ContinuousStateDistribution *csd2,
								       double *low, double *high)
{
  if ((csd1->isLeaf () && ! csd2->isLeaf ())
      || (! csd1->isLeaf () && csd2->isLeaf ()))
    return false;
  else
    {
      if (csd1->isLeaf () && csd2->isLeaf ())
	{
	  /* compare leaves. */
	  if (csd1->getProbability () != csd2->getProbability ())
	    return false;
	}
      else
	{
	  ContinuousStateDistribution *csd1lt
	    = static_cast<ContinuousStateDistribution*> (csd1->getLowerTree ());
	  ContinuousStateDistribution *csd2lt
	    = static_cast<ContinuousStateDistribution*> (csd2->getLowerTree ());
	  bool isEqualLow
	    = ContinuousStateDistribution::compareContinuousStateDistributions (csd1lt, csd2lt, low, high);
	  
	  ContinuousStateDistribution *csd1ge
	    = static_cast<ContinuousStateDistribution*> (csd1->getGreaterTree ());
	  ContinuousStateDistribution *csd2ge
	    = static_cast<ContinuousStateDistribution*> (csd2->getGreaterTree ());
	  bool isEqualHigh
	    = ContinuousStateDistribution::compareContinuousStateDistributions (csd1ge, csd2ge, low, high);

	  return (isEqualLow && isEqualHigh);
	}
    }
  return true;
}

void ContinuousStateDistribution::normalize (double *low, double *high)
{
  double sum = 0.0;
  sumUpProbabilities (&sum, low, high);
  normalize (sum); 
}

void ContinuousStateDistribution::setCsdProbability (const double &prob)
{
  if (! isLeaf ())
    {
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (getLowerTree ());
      csdlt->setCsdProbability (prob);
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      csdge->setCsdProbability (prob);
    }
  else 
    {
      m_probability = prob;
    }
}

void ContinuousStateDistribution::sumUpProbabilities (double *norm,
						      double *low, double *high)
{
  if (isLeaf ())
    {
      if (getProbability () > 0.0)
	{
	  double prob = 1.0;
	  for (int d=0; d<m_nDim; d++)
	    prob *= (high[d] - low[d]);
	  *norm += prob * getProbability ();
	}
    }
  else
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdlt 
	= static_cast<ContinuousStateDistribution*> (getLowerTree ());
      csdlt->sumUpProbabilities (norm, low, high);
      high[getDimension ()] = b;

      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdge
	= static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      csdge->sumUpProbabilities (norm, low, high);
      low[getDimension ()] = b;
    }
}

void ContinuousStateDistribution::normalize (const double &norm)
{
  if (isLeaf ())
    {
      if (getProbability () > 0.0)
	setProbability (getProbability () / norm);
    }  
  else 
    {
      ContinuousStateDistribution *csdlt 
	= static_cast<ContinuousStateDistribution*> (getLowerTree ());
      csdlt->normalize (norm);
      ContinuousStateDistribution *csdge
	= static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      csdge->normalize (norm);
    }
}

void ContinuousStateDistribution::mergeTreeLeaves (double *low, double *high)
{
  if (! isLeaf ())
    {
      /* double *highlt, *lowge;
	 highlt = new double[getSpaceDimension ()];
	 lowge = new double[getSpaceDimension ()];
	 for (int d=0; d<getSpaceDimension (); d++)
	 {
	 highlt[d] = high[d];
	 lowge[d] = low[d];
	 }

	 highlt[getDimension ()] = getPosition ();
	 lowge[getDimension ()] = getPosition (); */
      
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdlt 
	= static_cast<ContinuousStateDistribution*> (getLowerTree ());
      csdlt->mergeTreeLeaves (low, high);
      high[getDimension ()] = b;
      
      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdge
	= static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      csdge->mergeTreeLeaves (low, high);
      low[getDimension ()] = b;

      /* find two contiguous rectangular cells */
      if (csdlt->isLeaf () && csdge->isLeaf ())
	{
	  double problt = csdlt->getProbability (), probge = csdge->getProbability ();
	  if (csdlt->getProbability () == -1) problt = 0.0;
	  else if (csdge->getProbability () == -1) probge = 0.0;
	  
	  if (Alg::REqual (problt, probge,
			   ContinuousStateDistribution::m_doubleProbaPrecision))
	    {
	      /* transfer data to root and delete leaves. */
	      //transferData (*csdlt);
	      
	      /* merge to min proba. */
	      setProbability (std::min (problt, probge));

	      /* NEW: probability mass must remain unchanged. */
	      /* double vollt = 1.0, volge = 1.0;
		 for (int d=0; d<getSpaceDimension (); d++)
		 {
		 vollt *= (highlt[d] - low[d]);
		 volge *= (high[d] - lowge[d]);
		 }
		 double proba = (problt*vollt + probge*volge) / (vollt + volge);
		 m_probability = proba; */

	      //std::cout << "old proba: " << problt << " -- new proba: " << proba << std::endl;

	      delete csdlt; csdlt = 0;
	      delete csdge; csdge = 0;
	      setLowerTree (0);
	      setGreaterTree (0);
	    }
	}
      // delete []highlt; delete []lowge;
    }
}

void ContinuousStateDistribution::leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
							 double *low, double *high)
{
  const ContinuousStateDistribution &csda = static_cast<const ContinuousStateDistribution&> (bt);
  const ContinuousStateDistribution &csdb = static_cast<const ContinuousStateDistribution&> (btr);
  
  if (csda.getProbability () > 0.0 && csdb.getProbability () > 0.0)
    setProbability (csda.getProbability () + csdb.getProbability ());
  else if (csda.getProbability () > 0.0)
    setProbability (csda.getProbability ());
  else if (csdb.getProbability () > 0.0)
    setProbability (csdb.getProbability ());
}

void ContinuousStateDistribution::leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
							 double *low, double *high)
{ 
  const ContinuousStateDistribution &csda = static_cast<const ContinuousStateDistribution&> (bt);
  const ContinuousStateDistribution &csdb = static_cast<const ContinuousStateDistribution&> (btr);
  
  if (csda.getProbability () >= 0.0 && csdb.getProbability () >= 0.0)
    setProbability (csda.getProbability () * csdb.getProbability ());
  else if (csda.getProbability () >= 0.0)
    setProbability (0.0);
  else if (csdb.getProbability () >= 0.0)
    setProbability (0.0);
}

void ContinuousStateDistribution::leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
							  double *low, double *high)
{
  const ContinuousStateDistribution &csda = static_cast<const ContinuousStateDistribution&> (bt);
  const ContinuousStateDistribution &csdb = static_cast<const ContinuousStateDistribution&> (btr);
  
  if (csda.getProbability () >= 0.0 && csdb.getProbability () >= 0.0)
    setProbability (csda.getProbability () - csdb.getProbability ());
  else if (csda.getProbability () >= 0.0)
    setProbability (csda.getProbability ());
  else if (csdb.getProbability () >= 0.0)
    setProbability (csdb.getProbability ());
}

/* Beware: this operation relies on preserving the arguments symmetry during tree operations. */
void ContinuousStateDistribution::leafDataIntersectCsdDiff (const BspTree &bt, const BspTree &btr,
							    double *low, double *high)
{
  const ContinuousStateDistribution &csda = static_cast<const ContinuousStateDistribution&> (bt);
  const ContinuousStateDistribution &csdb = static_cast<const ContinuousStateDistribution&> (btr);
  
  if (csda.getProbability () > 0.0 && csdb.getProbability () > 0.0)
    {
      setProbability (0.0);
    }
  else if (csda.getProbability () > 0.0 && csdb.getProbability () <= 0.0)
    {
      setProbability (csda.getProbability ());
    }
}

void ContinuousStateDistribution::multiplyByScalar (const double &scalar)
{
  if (isLeaf ())
    {
      if (getProbability () > 0.0)
	setProbability (getProbability () * scalar);
    }  
  else
    {
      ContinuousStateDistribution *csdlt 
	= static_cast<ContinuousStateDistribution*> (getLowerTree ());
      csdlt->multiplyByScalar (scalar);
      ContinuousStateDistribution *csdge
	= static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      csdge->multiplyByScalar (scalar);
    }
}

void ContinuousStateDistribution::getMinPosition (double *mpos, double *low, double *high)
{
  if (isLeaf ())
    {
      if (getProbability () > ContinuousStateDistribution::m_doubleProbaPrecision)
	{
	  bool minp = true;
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      if (mpos[i] < low[i])
		{
		  minp = false;
		  break;
		}
	    }
	  if (minp)
	    {
	      for (int i=0; i<getSpaceDimension (); i++)
		{
		  mpos[i] = low[i];
		}
	    }
	}
    }
  else
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (getLowerTree ());
      csdlt->getMinPosition (mpos, low, high);
      high[getDimension ()] = b;

      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      csdge->getMinPosition (mpos, low, high);
      low[getDimension ()] = b;
    }
}

void ContinuousStateDistribution::getMaxPosition (double *mpos, double *low, double *high)
{
  if (isLeaf ())
    {
      if (getProbability () > ContinuousStateDistribution::m_doubleProbaPrecision)
	{
	  bool maxp = true;
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      if (mpos[i] > high[i])
		{
		  maxp = false;
		  break;
		}
	    }
	  if (maxp)
	    {
	      for (int i=0; i<getSpaceDimension (); i++)
		{
		  mpos[i] = high[i];
		}
	    }
	}
    }
  else
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (getLowerTree ());
      csdlt->getMaxPosition (mpos, low, high);
      high[getDimension ()] = b;

      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (getGreaterTree ());
      csdge->getMaxPosition (mpos, low, high);
      low[getDimension ()] = b;
    }
}

/* printing */
void ContinuousStateDistribution::print (std::ostream &out, double *low, double *high)
{
  if (isLeaf ())
    {
      if (m_probability >= 0.0)
	{
	  out << "{";
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      out << "[" << low[i] << "," << high[i];
	      if (i == getSpaceDimension () - 1)
		out << "]}\n";
	      else out << "],";
	    }
	  std::cout << "probability: " << m_probability << std::endl;
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<ContinuousStateDistribution*> (getGreaterTree ())->print (out, low, high);
      low[getDimension ()] = b;
      
      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<ContinuousStateDistribution*> (getLowerTree ())->print (out, low, high);
      high[getDimension ()] = b;
    }
}

void ContinuousStateDistribution::plot1DCSD (std::ofstream &box,
					     double *low, double *high)
{
  if (isLeaf ())
    {
      if (getProbability () >= 0.0)
	box << low[0] << ' ' << high[0] << ' ' << getProbability () << '\n';
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<ContinuousStateDistribution*> (getGreaterTree ())->plot1DCSD (box, low, high);
      low[getDimension ()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<ContinuousStateDistribution*> (getLowerTree ())->plot1DCSD (box, low, high);
      high[getDimension ()] = b;
    }
}

void ContinuousStateDistribution::plot2DVrml2Leaf (const double &space, std::ofstream &output_vrml,
						   double *low, double *high, const double &scale0,
						   const double &scale1, const double &max_value)
{
  if (getProbability () >= 0.0)
    {
      double width0 = (high[0]-low[0])/scale0, width1 = (high[1]-low[1])/scale1;
      double pos0 = (high[0]+low[0])/(2.0 * scale0), pos1 = (high[1]+low[1])/(2.0 * scale1);
      output_vrml << "\t\tTransform {\n\t\t\ttranslation "
		  << pos0 << ' ' << - (getProbability ()) / 2.0  << ' ' << pos1 << ' '
		  << "\n\t\t\tchildren [\n\t\t\t\tShape {\n\t\t\t\t\tappearance Appearance {material Material {} }\n\t\t\t\t\tgeometry Box {size " 
		  << width0-space << ' ' << getProbability () << ' ' << width1-space << "}\n\t\t\t}\n\t\t]\n\t}\n";
    }
}

double ContinuousStateDistribution::getPointValueInLeaf (double *pos)
{
  /* if (getProbability () >= 0.0) return getProbability ();
  else return 0.0; */
  return getProbability ();
}

} /* end of namespace */
