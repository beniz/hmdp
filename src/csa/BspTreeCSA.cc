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

#include "BspTreeCSA.h"
#include "Alg.h"
#include "LpSolve5.h"
#include <assert.h>
#include <stdlib.h>

namespace hmdp_csa
{

int BspTreeCSA::m_prunedPlans = 0;

BspTreeCSA::BspTreeCSA (const int &sdim)
  : BspTree (sdim), m_csVectors (0), m_cs_points(0), m_n_cs_points(0)
{
  m_bspType = BspTreeCSAT;
}

BspTreeCSA::BspTreeCSA (const int &sdim, const int &d, const double &pos)
  : BspTree (sdim, d, pos), m_csVectors (0), m_cs_points(0), m_n_cs_points(0)
{
  m_bspType = BspTreeCSAT;
}

BspTreeCSA::BspTreeCSA (const int &sdim, double *lowPos, 
			double *highPos, const double *low, const double *high)
  : BspTree (sdim), m_csVectors (0), m_cs_points(0), m_n_cs_points(0)
{
  m_bspType = BspTreeCSAT;
  BspTreeCSA *bsp_n = this;

  /* setting bounds */
  for (int i=0; i<m_nDim; i++)  /* iterate the dimensions */
    {
      /* higher bound */
      bsp_n->setDimension (i);

      /* check if higher bound belongs to the domain. */
      if (highPos[i] > high[i])
	{
	  std::cerr << "[Warning]: BspTreeCSA constructor: upper bound is outside the domain.\n";
	  highPos[i] = high[i];
	}

      bsp_n->setPosition (highPos[i]);

      /* check if lower bound belongs to the domain. */
      if (lowPos[i] < low[i])
	{
	  std::cerr << "[Warning]: BspTreeCSA constructor: lower bound is outside the domain.\n";
	  lowPos[i] = low[i];
	}
      
      bsp_n->setLowerTree (new BspTreeCSA (m_nDim, i, lowPos[i]));
      bsp_n->setGreaterTree (new BspTreeCSA (m_nDim));
      bsp_n = static_cast<BspTreeCSA*> (bsp_n->getLowerTree ());
      
      /* lower bound */
      bsp_n->setLowerTree (new BspTreeCSA (m_nDim));
      bsp_n->setGreaterTree (new BspTreeCSA (m_nDim));
      bsp_n = static_cast<BspTreeCSA*> (bsp_n->getGreaterTree ());
    }
}

BspTreeCSA::BspTreeCSA (const BspTreeCSA &bta)
  : BspTree (bta.getSpaceDimension (), bta.getDimension (), bta.getPosition ()),
    m_csVectors (0), m_cs_points(0), m_n_cs_points(bta.getNCSPoints())
{
  m_bspType = BspTreeCSAT;

  if (bta.getCSVectors ())
    {
      m_csVectors = new std::vector<CSVector*>();
      for (unsigned int i=0; i<bta.getCSVectorsSize(); i++)
	m_csVectors->push_back (new CSVector(*bta.getCSVectorNth (i)));
      
      if (bta.getPoints())
	{
	  m_cs_points = new std::vector<std::vector<double>*>();
	  for (unsigned int i=0; i<bta.getPointsSize(); i++)
	    m_cs_points->push_back(new std::vector<double>(*bta.getPoint(i)));
	}
    }

  if (! bta.isLeaf ())
    {
      BspTreeCSA *blt = static_cast<BspTreeCSA*> (bta.getLowerTree ());
      m_lt = new BspTreeCSA (*blt);
      BspTreeCSA *bge = static_cast<BspTreeCSA*> (bta.getGreaterTree ());
      m_ge = new BspTreeCSA (*bge);
    }
  else m_lt = m_ge = 0;
}

BspTreeCSA::BspTreeCSA(const ValueFunction &vf,
		       const int &covdim, const int &coeff_dim,
		       double *low, double *high)
  : BspTree(vf.getSpaceDimension(),vf.getDimension(),vf.getPosition()),
    m_csVectors(0), m_cs_points(0), m_n_cs_points(0)
{
  m_bspType = BspTreeCSAT;

  if (vf.getAlphaVectors())
    {
      //transfer the values etc...
      //test: should be a constant value.
      assert(vf.getAlphaVectorsSize() == 1);
      AlphaVector *av = vf.getAlphaVectorNth(0);
      double val = av->getAlphaNth(0);

      //cs vectors.
      m_csVectors = new std::vector<CSVector*>();
      CSVector *csv = new CSVector(covdim);
      csv->setCSNth(coeff_dim,val);

      //if (covdim-1 != coeff_dim)  //coeffs, not constant.
      //{
	  double surf = 1.0;
	  for (int i=0; i<m_nDim; i++)
	    surf *= (high[i] - low[i]);
	  double exp = val / surf;  //local expectation.
	  csv->setCSExpNth(coeff_dim,exp);
	  //	}

      m_csVectors->push_back(csv);
    }
  
  if (!vf.isLeaf())
    {
      double b = high[vf.getDimension()];
      high[vf.getDimension()] = vf.getPosition();
      ValueFunction *vflt = static_cast<ValueFunction*>(vf.getLowerTree());
      high[vf.getDimension()] = b;
      
      b = low[vf.getDimension()];
      low[vf.getDimension()] = vf.getPosition();
      m_lt = new BspTreeCSA(*vflt,covdim,coeff_dim,low,high);
      ValueFunction *vfge = static_cast<ValueFunction*>(vf.getGreaterTree());
      m_ge = new BspTreeCSA(*vfge,covdim,coeff_dim,low,high);
      low[vf.getDimension()] = b;
    }
  else m_lt = m_ge = 0;
}

/* BspTreeCSA::BspTreeCSA(const ContinuousStateDistribution &csd,
		       const int &covdim, const int &coeff_dim)
  : BspTree(csd.getSpaceDimension(),csd.getDimension(),csd.getPosition()),
    m_csVectors(0)
{
  m_bspType = BspTreeCSAT;

  if (csd.getProbability() > 0.0)
    {
      //transfer the probability.
      double val = csd.getProbability();

      //cs vectors.
      m_csVectors = new std::vector<CSVector*>();
      CSVector *csv = new CSVector(covdim);
      csv->setCSNth(coeff_dim,val);
      m_csVectors->push_back(csv);
    }
  
  if (!csd.isLeaf())
    {
      ContinuousStateDistribution *csdlt 
	= static_cast<ContinuousStateDistribution*>(csd.getLowerTree());
      m_lt = new BspTreeCSA(*csdlt,covdim,coeff_dim);
      ContinuousStateDistribution *csdge 
	= static_cast<ContinuousStateDistribution*>(csd.getGreaterTree());
      m_ge = new BspTreeCSA(*csdge,covdim,coeff_dim);
    }
  else m_lt = m_ge = 0;
  } */

BspTreeCSA::BspTreeCSA(const int &sdim,
		       const int &covdim,
		       const double *low, const double *high)
  : BspTree(sdim), m_csVectors(0), m_cs_points(0), m_n_cs_points(0)
{
  m_bspType = BspTreeCSAT;
  BspTreeCSA *cs = this;
  
  for (int i=0; i<m_nDim; i++)
    {
      /* upper bound. */
      cs->setDimension(i);
      cs->setPosition(high[i]);
      cs->setLowerTree(new BspTreeCSA(m_nDim,i,low[i]));
      cs->setGreaterTree(new BspTreeCSA(m_nDim));
      cs = static_cast<BspTreeCSA*>(cs->getLowerTree());
      
      /* lower bound. */
      cs->setLowerTree(new BspTreeCSA(m_nDim));
      cs->setGreaterTree(new BspTreeCSA(m_nDim));
      cs = static_cast<BspTreeCSA*>(cs->getGreaterTree());
    }

  /* set initial vector to 0.0 */
  CSVector *csv = new CSVector(covdim);
  cs->addAVector(csv);
}

BspTreeCSA::~BspTreeCSA ()
{
  if (m_csVectors)
    {
      for (unsigned int i=0; i<m_csVectors->size (); i++)
	delete (*m_csVectors)[i];
      delete m_csVectors;
      m_csVectors = 0;
    }

  if (m_cs_points)
    {
      for (unsigned int i=0; i<m_cs_points->size(); i++)
	delete (*m_cs_points)[i];
      delete m_cs_points; m_cs_points = 0;
    }
}

void BspTreeCSA::transferData (const BspTree &bt) 
{
  /* delete current alpha vectors if any */
  if (m_csVectors) 
    {
      for (unsigned int i=0; i<m_csVectors->size (); i++)
	delete (*m_csVectors)[i];
      delete m_csVectors;
      m_csVectors = 0;
    }
  
  /* TODO: do we need a dedicated transfer, w.r.t. tree type ? */
  
  const BspTreeCSA &bta = static_cast<const BspTreeCSA&> (bt);
  
  m_n_cs_points = bta.getNCSPoints();

  /* copy cs vectors */
  if (bta.getCSVectors() && !bta.getCSVectors()->empty())
    {
      if (! m_csVectors)
	m_csVectors = new std::vector<CSVector*> ();
      for (unsigned int i=0; i<bta.getCSVectorsSize (); i++)
	m_csVectors->push_back(new CSVector (*bta.getCSVectorNth (i)));
    
      if (bta.getPoints() && !bta.getPoints()->empty())
	{
	  m_cs_points = new std::vector<std::vector<double>*>();
	  for (unsigned int i=0; i<bta.getPointsSize(); i++)
	    m_cs_points->push_back(new std::vector<double>(*bta.getPoint(i)));
	}
    }
  else 
    {
      m_csVectors = NULL;
      m_cs_points = NULL;
    }
}


void BspTreeCSA::leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
					double *low, double *high) {
  const BspTreeCSA &bta = static_cast<const BspTreeCSA&> (bt);
  const BspTreeCSA &btb = static_cast<const BspTreeCSA&> (btr);
  
  if (bta.getCSVectors () && btb.getCSVectors ())
    {
      std::cerr << "[Error]: BspTreeCSA::leafDataIntersectInit: both cells have values ! Exiting...\n";
      exit (1);
    }
  else if (bta.getCSVectors())
    {
      if (! m_csVectors)
	m_csVectors = new std::vector<CSVector*> ();
      for (unsigned int i=0; i<bta.getCSVectorsSize (); i++)
	  m_csVectors->push_back(new CSVector(*bta.getCSVectorNth (i)));
    }
  else if (btb.getCSVectors())
    {
      if (! m_csVectors)
	m_csVectors = new std::vector<CSVector*> ();
      for (unsigned int i=0; i<btb.getCSVectorsSize (); i++)
	  m_csVectors->push_back(new CSVector(*btb.getCSVectorNth (i)));
    }
}

void BspTreeCSA::leafDataIntersectPlus(const BspTree &bt, const BspTree &btr,
				       double *low, double *high)
{
  const BspTreeCSA &csa = static_cast<const BspTreeCSA&>(bt);
  const BspTreeCSA &csb = static_cast<const BspTreeCSA&>(btr);

  if (! m_csVectors)
    m_csVectors = new std::vector<CSVector*>();
  else
    { 
      for (size_t i=0; i<m_csVectors->size(); i++)
	delete (*m_csVectors)[i];
      m_csVectors->clear();
    }
  
  if (csa.getCSVectors() && csb.getCSVectors())
    //TODO: replace with a cross-sum operator...
    CSVector::simpleSumCSVectors(*csa.getCSVectors(), 
				 *csb.getCSVectors(), m_csVectors);
  else if (csa.getCSVectors())
    {
      for (size_t i=0; i<csa.getCSVectors()->size(); i++)
	{
	  CSVector *cs = new CSVector(*csa.getCSVectorNth(i));
	  m_csVectors->push_back(cs);
	}
    }
  else if (csb.getCSVectors())
    {
      for (size_t i=0; i<csb.getCSVectors()->size(); i++)
	{
	  CSVector *cs = new CSVector(*csb.getCSVectorNth(i));
	  m_csVectors->push_back(cs);
	}
    }
  else { delete m_csVectors; m_csVectors = 0;}

  //TODO: re-scale coeffs.
  if (m_csVectors)
    {
      rescaleCoeffs(low, high);
    }

  //BEWARE: there should be no need to transfer the cs points here...
}

//BEWARE: there should be no need to transfer the new cs points here...
void BspTreeCSA::leafDataIntersectUnion(const BspTree &bt, const BspTree &btr,
					double *low, double *high)
{
  const BspTreeCSA &csa = static_cast<const BspTreeCSA&>(bt);
  const BspTreeCSA &csb = static_cast<const BspTreeCSA&>(btr);

  if (! m_csVectors)
    m_csVectors = new std::vector<CSVector*>();
  else
    { 
      for (size_t i=0; i<m_csVectors->size(); i++)
	delete (*m_csVectors)[i];
      m_csVectors->clear();
    }

  if (csa.getCSVectors() && csb.getCSVectors())
    {
      CSVector::unionCSVectors(*csa.getCSVectors(),
			       *csb.getCSVectors(), m_csVectors);

      if (csa.getPoints() || csb.getPoints())
	{
	  m_cs_points = new std::vector<std::vector<double>*>();
	  if (csa.getPoints())
	    {
	      for (size_t i=0; i<csa.getPointsSize(); i++)
		{
		  m_cs_points->push_back(new std::vector<double>(*csa.getPoint(i)));
		}
	    }
	  if (csb.getPoints())
	    {
	      for (size_t i=0; i<csb.getPointsSize(); i++)
		{
		  m_cs_points->push_back(new std::vector<double>(*csa.getPoint(i)));
		}
	    }
	} /* end if points. */
    }
  else if (csa.getCSVectors())
    {
      for (size_t i=0; i<csa.getCSVectors()->size(); i++)
	{
	  CSVector *cs = new CSVector(*csa.getCSVectorNth(i));
	  m_csVectors->push_back(cs);
	} 

      if (csa.getPoints())
	{
	  m_cs_points = new std::vector<std::vector<double>*>();
	  for (size_t i=0; i<csa.getPointsSize(); i++)
	    {
	      m_cs_points->push_back(new std::vector<double>(*csa.getPoint(i)));
	    }
	}
    }
  else if (csb.getCSVectors())
    {
      for (size_t i=0; i<csb.getCSVectors()->size(); i++)
	{
	  CSVector *cs = new CSVector(*csb.getCSVectorNth(i));
	  m_csVectors->push_back(cs);
	}
      
      if (csb.getPoints())
	{
	  m_cs_points = new std::vector<std::vector<double>*>();
	  for (size_t i=0; i<csb.getPointsSize(); i++)
	    {
	      m_cs_points->push_back(new std::vector<double>(*csb.getPoint(i)));
	    }
	}
    }
  else { delete m_csVectors; m_csVectors = 0;}

  //TODO: re-scale coeffs. -> re-scale cs points ????
  /* if (m_csVectors)
    {
    rescaleCoeffs(low, high);
    } */

  m_n_cs_points = csa.getNCSPoints() + csb.getNCSPoints();
}

void BspTreeCSA::leafDataIntersectMinus(const BspTree &bt, const BspTree &btr,
					double *low, double *high)
{
  const BspTreeCSA &csa = static_cast<const BspTreeCSA&>(bt);
  const BspTreeCSA &csb = static_cast<const BspTreeCSA&>(btr);

  if (m_csVectors)
    {
      for (size_t i=0; i<m_csVectors->size(); i++)
	delete (*m_csVectors)[i];
      delete m_csVectors; m_csVectors = NULL;
    }
  
  if (csa.getCSVectors() && csb.getCSVectors())
    {
      bool eq = CSVector::isEqualEpsilon(csa.getCSVectors(),
					 csb.getCSVectors(),
					 Alg::m_doubleEpsilon);
     
      if (eq)
	{
	  m_csVectors = NULL;
	}
      else 
	{
	  m_csVectors = new std::vector<CSVector*>();
	  for (size_t i=0; i<csb.getCSVectors()->size(); i++)
	    {
	      CSVector *cs = new CSVector(*csb.getCSVectorNth(i));
	      m_csVectors->push_back(cs);
	    }
	}
    }
  else m_csVectors = NULL;
}

void BspTreeCSA::hasPlans(bool &hp)
{
  if (isLeaf())
    {
      if (m_csVectors)
	hp = true;
    }
  else
    {
      if (hp) return;
      BspTreeCSA *csa = static_cast<BspTreeCSA*>(getLowerTree());
      csa->hasPlans(hp);
      if (hp) return;
      BspTreeCSA *csb = static_cast<BspTreeCSA*>(getGreaterTree());
      csb->hasPlans(hp);
    }
}

int BspTreeCSA::countPlanes() const
{
    if (isLeaf())
    {
	if (m_csVectors)
	{
	    return m_csVectors->size();
	}
	else return 0;
    }
    else
    {
	return static_cast<BspTreeCSA*>(getLowerTree())->countPlanes()
	    + static_cast<BspTreeCSA*>(getGreaterTree())->countPlanes();
    }
}

void BspTreeCSA::countNonZeroCSVectors(int &nz) const
{
  if (isLeaf())
    {
      if (m_csVectors)
	{
	  for (size_t i=0;i<m_csVectors->size();i++)
	    if (!(*m_csVectors)[i]->isZero())
	      nz++;
	}
    }
  else
    {
      BspTreeCSA *csa = static_cast<BspTreeCSA*>(getLowerTree());
      csa->countNonZeroCSVectors(nz);
      BspTreeCSA *csb = static_cast<BspTreeCSA*>(getGreaterTree());
      csb->countNonZeroCSVectors(nz);
    }
}

void BspTreeCSA::rescaleCoeffs(double *low, double *high)
{
  /* double surf=1.0;
  for (int i=0; i<m_nDim; i++)
    surf *= (high[i]-low[i]);
  
  for (size_t i=0;i<m_csVectors->size(); i++)
    {
      CSVector *csv = (*m_csVectors)[i];
      
      //re-scale.
      for (int j=0; j<csv->getSize(); j++) 
	{
	  double exp = csv->getCSExpNth(j);
	  csv->setCSNth(j,surf*exp);
	}
    }
  */
  //BEWARE: re-scale points ?????
}

void BspTreeCSA::multiplyByScalar(const double &scalar)
{
  if (! isLeaf ())
    {
      BspTreeCSA *btalt = static_cast<BspTreeCSA*>(getLowerTree ());
      btalt->multiplyByScalar(scalar);
      BspTreeCSA *btage = static_cast<BspTreeCSA*>(getGreaterTree ());
      btage->multiplyByScalar(scalar);
    }
  else if (m_csVectors) 
    CSVector::multiplyVecByScalar(scalar,m_csVectors);
}

/* static */
BspTreeCSA* BspTreeCSA::sumBspTreeCSAs(BspTreeCSA *csa, BspTreeCSA *csb,
				       double *low, double *high)
{
  BspTreeOperations::setIntersectionType(BTI_PLUS);  /* leaf intersection type */
  BspTree *bt = BspTreeOperations::intersectTrees (csa, csb, low, high);
  BspTreeCSA *cs = static_cast<BspTreeCSA*>(bt);
  
  if (BspTreeOperations::m_piecesMerging)
    cs->mergeTreeLeaves (low, high);
  return cs;
}

BspTreeCSA* BspTreeCSA::unionBspTreeCSAs(BspTreeCSA *csa, BspTreeCSA *csb,
					 double *low, double *high)
{
  BspTreeOperations::setIntersectionType(BTI_UNION);
  BspTree *bt = BspTreeOperations::intersectTrees(csa,csb,low,high);
  BspTreeCSA *cs = static_cast<BspTreeCSA*>(bt);
  
  if (BspTreeOperations::m_piecesMerging)
    cs->mergeTreeLeaves (low, high);
  return cs;
}

/* leaves merging. */
void BspTreeCSA::mergeTreeLeaves(double *low, double *high)
{
  if (! isLeaf ())
    {
      double b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      BspTreeCSA *csalt = static_cast<BspTreeCSA*>(getLowerTree ());
      csalt->mergeTreeLeaves(low, high);
      high[getDimension ()] = b;
      
      b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      BspTreeCSA *csage = static_cast<BspTreeCSA*>(getGreaterTree ());
      csage->mergeTreeLeaves(low, high);
      low[getDimension ()] = b;
      
      /* find two contiguous rectangular cells */
      if (csalt->isLeaf () && csage->isLeaf ())
	mergeContiguousLeaves(this, csalt, csage, low, high);
    }
  else return;
}

void BspTreeCSA::mergeContiguousLeaves(BspTreeCSA *root,
				       BspTreeCSA *lt, BspTreeCSA *ge,
				       double *low, double *high)
{ 
  std::vector<CSVector*> *lt_csv = lt->getCSVectors();
  std::vector<CSVector*> *ge_csv = ge->getCSVectors();
  
  if (!lt_csv || !ge_csv)
    return;

  if (CSVector::isEqualEpsilon(lt_csv,ge_csv,
			       ContinuousStateDistribution::m_doubleProbaPrecision))
    {
      
      transferData(*lt);
      
      //if (root->getCSVectors())
      //root->rescaleCoeffs(low,high);
      
      delete lt; lt = 0;
      delete ge; ge = 0;
      setLowerTree(0); setGreaterTree(0);
    }
}

void BspTreeCSA::pruneUnreachablePlans(const size_t &covdim)
{
  if (isLeaf())
    {
      if (!m_csVectors)
	return;
      
      std::vector<std::vector<CSVector*>::iterator> baditr;
      std::vector<CSVector*>::iterator csit;
      for (csit = m_csVectors->begin(); csit != m_csVectors->end(); ++csit)
	{
	  CSVector *csv = (*csit);

	  //debug
	  //std::cout << "reachability pruning: csv: " << *csv;
	  //debug

	  bool unreachable = true;
	  for (size_t j=0; j<covdim; j++)  //do not look up the constant component.
	    {
	      if (csv->getCSNth(j) > 0.0)  //TODO: use precision on double.
		{
		  unreachable = false;
		  break;
		}
	    }
	  if (unreachable)
	    {
	      //debug
	      //std::cout << " -> pruned!\n";
	      //debug
	      
	      baditr.push_back(csit);
	    }
	  //else std::cout << std::endl;
	  
	} //end for cs vectors.
      
      //remove iterators.
      for (size_t i=0; i<baditr.size(); i++)
	m_csVectors->erase(baditr[i]);

      //if empty, remove plans, as they're unreachable.
      if (m_csVectors->empty())
	{
	  delete m_csVectors; 
	  m_csVectors = 0;
	}
    }
  else
    {
      BspTreeCSA *csa = static_cast<BspTreeCSA*>(getLowerTree());
      csa->pruneUnreachablePlans(covdim);
      BspTreeCSA *csb = static_cast<BspTreeCSA*>(getGreaterTree());
      csb->pruneUnreachablePlans(covdim);
    }
}

void BspTreeCSA::pruneDominatedPlans(double *lowpb, double *highpb)
{
  if (isLeaf())
    {
      if (m_csVectors)
	{
	  const size_t nplans = m_csVectors->size();
	  std::vector<CSVector*> *res = new std::vector<CSVector*>();
	  LpSolve5::pruneLP(m_csVectors,lowpb,highpb,res);

	  //stats
	  assert(nplans >= res->size());
	  BspTreeCSA::m_prunedPlans += (nplans - res->size());
	  //stats

	  delete m_csVectors;
	  setCSVectors(res);
	}
    }
  else
    {
      BspTreeCSA *csa = static_cast<BspTreeCSA*>(getLowerTree());
      csa->pruneDominatedPlans(lowpb,highpb);
      BspTreeCSA *csb = static_cast<BspTreeCSA*>(getGreaterTree());
      csb->pruneDominatedPlans(lowpb,highpb);
    }
}

void BspTreeCSA::collectPoints(std::vector<std::vector<double>*> &pts)
{
  if (isLeaf())
    {
      if (m_cs_points)
	{
	  //size_t csize = pts.size();

	  //test for inclusion and add new points.
	  for (size_t i=0; i<m_cs_points->size(); i++)
	    {
	      std::vector<double> *pt = (*m_cs_points)[i];
	      if (!BspTreeCSA::hasPoint(pts,*pt))
		pts.push_back(pt);
	      else delete pt;
	    }

	  //stats
	  m_n_cs_points += m_cs_points->size();//pts.size()-csize;
	  //stats

	  m_cs_points->clear();
	}
    }
  else
    {
      BspTreeCSA *csa = static_cast<BspTreeCSA*>(getLowerTree());
      csa->collectPoints(pts);
      BspTreeCSA *csb = static_cast<BspTreeCSA*>(getGreaterTree());
      csb->collectPoints(pts);
    }
}

bool BspTreeCSA::hasPoint(std::vector<std::vector<double>*> &pts,
			  std::vector<double> &pt)
{
  for (size_t i=0; i<pts.size(); i++)
    {
      std::vector<double>* vec = pts[i];
      bool included = true;
      for (size_t j=0; j<pt.size()-1; j++)  //do not check on the constant !
	{
	  if (!Alg::REqual(pt[j],(*vec)[j],Alg::m_doubleEpsilon))
	    {
	      included = false;
	      break; //skip this vec.
	    }
	}
      if (included)
	return true;
    }
  return false;
}

/* printing */
double BspTreeCSA::getPointValueInLeaf(double *pos)
{
  return static_cast<double>(m_n_cs_points);
}

void BspTreeCSA::print (std::ostream &out, double *low, double *high)
{
  if (isLeaf ())
    {
      if (m_csVectors)
	{
	  out << "{";
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      out << "[" << low[i] << "," << high[i];
	      if (i == getSpaceDimension() - 1)
		out << "]}\n";
	      else out << "],";
	    }
	  
	  /* alpha vectors */
	  for (size_t i=0; i<m_csVectors->size (); i++)
	    if ((*m_csVectors)[i])
	      out << *(*m_csVectors)[i] << std::endl;
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<BspTreeCSA*> (getGreaterTree ())->print (out, low, high);
      low[getDimension ()] = b;
      
      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<BspTreeCSA*> (getLowerTree ())->print (out, low, high);
      high[getDimension ()] = b;
    }
}

void BspTreeCSA::plot1D(std::ofstream &box,
			double *low, double *high)
{
  if (isLeaf())
    box << low[0] << ' ' << high[0] << ' ' << static_cast<double>(m_n_cs_points) << std::endl;
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<BspTreeCSA*> (getGreaterTree ())->plot1D(box, low, high);
      low[getDimension ()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<BspTreeCSA*> (getLowerTree ())->plot1D(box, low, high);
      high[getDimension ()] = b;
    }
}

} /* end of namespace. */
