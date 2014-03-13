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

#include "BspTreeAlpha.h"
#include "ContinuousOutcome.h"
#include <stdlib.h>

namespace hmdp_base
{

BspTreeAlpha::BspTreeAlpha (const int &sdim)
  : BspTree (sdim), m_alphaVectors (0), m_maxValue (0.0)
{
  m_bspType = BspTreeAlphaT;
}  

BspTreeAlpha::BspTreeAlpha (const int &sdim, const int &d, const double &pos)
  : BspTree (sdim, d, pos), m_alphaVectors (0), m_maxValue (0.0)
{
  m_bspType = BspTreeAlphaT;
}

BspTreeAlpha::BspTreeAlpha (const int &sdim, double *lowPos, 
			    double *highPos, const double *low, const double *high)
  : BspTree (sdim), m_alphaVectors (0), m_maxValue (0.0)
{
  m_bspType = BspTreeAlphaT;
  BspTreeAlpha *bsp_n = this;

  /* setting bounds */
  for (int i=0; i<m_nDim; i++)  /* iterate the dimensions */
    {
      /* higher bound */
      bsp_n->setDimension (i);

      /* check if higher bound belongs to the domain. */
      if (highPos[i] > high[i])
	{
	  std::cerr << "[Warning]: BspTreeAlpha constructor: upper bound is outside the domain.\n";
	  highPos[i] = high[i];
	}

      bsp_n->setPosition (highPos[i]);

      /* check if lower bound belongs to the domain. */
      if (lowPos[i] < low[i])
	{
	  std::cerr << "[Warning]: BspTreeAlpha constructor: lower bound is outside the domain.\n";
	  lowPos[i] = low[i];
	}
      
      bsp_n->setLowerTree (new BspTreeAlpha (m_nDim, i, lowPos[i]));
      bsp_n->setGreaterTree (new BspTreeAlpha (m_nDim));
      bsp_n = static_cast<BspTreeAlpha*> (bsp_n->getLowerTree ());
      
      /* lower bound */
      bsp_n->setLowerTree (new BspTreeAlpha (m_nDim));
      bsp_n->setGreaterTree (new BspTreeAlpha (m_nDim));
      bsp_n = static_cast<BspTreeAlpha*> (bsp_n->getGreaterTree ());
    }
}

BspTreeAlpha::BspTreeAlpha (const BspTreeAlpha &bta)
  : BspTree (bta.getSpaceDimension (), bta.getDimension (), bta.getPosition ()),
    m_alphaVectors (0), m_maxValue (bta.getSubTreeMaxValue ())
{
  m_bspType = BspTreeAlphaT;

  if (bta.getAlphaVectors ())
    {
      m_alphaVectors = new std::vector<AlphaVector*> ();
      for (unsigned int i=0; i<bta.getAlphaVectorsSize (); i++)
	m_alphaVectors->push_back (new AlphaVector (*bta.getAlphaVectorNth (i)));
    }

  if (! bta.isLeaf ())
    {
      BspTreeAlpha *blt = static_cast<BspTreeAlpha*> (bta.getLowerTree ());
      m_lt = new BspTreeAlpha (*blt);
      BspTreeAlpha *bge = static_cast<BspTreeAlpha*> (bta.getGreaterTree ());
      m_ge = new BspTreeAlpha (*bge);
    }
  else m_lt = m_ge = 0;
}

BspTreeAlpha::~BspTreeAlpha ()
{
  if (m_alphaVectors)
    {
      for (unsigned int i=0; i<m_alphaVectors->size (); i++)
	delete (*m_alphaVectors)[i];
      delete m_alphaVectors;
      m_alphaVectors = 0;
    }
}

/* TODO: a 'while' could break and would be faster... */
bool BspTreeAlpha::isZero ()
{
  if (isLeaf ())
    {
      if (! m_alphaVectors)
	return true;
      else 
	{
	  for (size_t i=0; i<m_alphaVectors->size (); i++)
	    if (! (*m_alphaVectors)[i]->isZero ())
	      return false;
	  return true;
	}
    }
  else
    {
      BspTreeAlpha *btalt = static_cast<BspTreeAlpha*> (getLowerTree ());
      bool isZeroLow = btalt->isZero ();
      BspTreeAlpha *btage = static_cast<BspTreeAlpha*> (getGreaterTree ());
      bool isZeroHigh = btage->isZero ();
      return (isZeroLow && isZeroHigh);
    }
}

bool BspTreeAlpha::isZeroOrBelow ()
{
  if (isLeaf ())
    {
      if (! m_alphaVectors)
	return true;
      else 
	{
	  for (size_t i=0; i<m_alphaVectors->size (); i++)
	    if (! (*m_alphaVectors)[i]->isZeroOrBelow ())
	      return false;
	  return true;
	}
    }
  else
    {
      BspTreeAlpha *btalt = static_cast<BspTreeAlpha*> (getLowerTree ());
      bool isZeroLow = btalt->isZeroOrBelow ();
      BspTreeAlpha *btage = static_cast<BspTreeAlpha*> (getGreaterTree ());
      bool isZeroHigh = btage->isZeroOrBelow ();
      return (isZeroLow && isZeroHigh);
    }
}

void BspTreeAlpha::transferData (const BspTree &bt) 
{
  /* delete current alpha vectors if any */
  if (m_alphaVectors) 
    {
      for (unsigned int i=0; i<m_alphaVectors->size (); i++)
	delete (*m_alphaVectors)[i];
      delete m_alphaVectors;
      m_alphaVectors = 0;
    }
  
  if (bt.getType () == ContinuousOutcomeT)  /* we need a dedicated transfer if the 
					       tree does not belong to that 'inherited branch'. */
    {
      const ContinuousOutcome &co = static_cast<const ContinuousOutcome&> (bt);
      if (co.getProbability () >= 0.0)
	{
	  m_alphaVectors = new std::vector<AlphaVector*> ();
	  m_alphaVectors->push_back (new AlphaVector (co.getProbability ()));
	}
      
      /* set max value. */
      m_maxValue = co.getProbability ();
    }
  else 
    {
      const BspTreeAlpha &bta = static_cast<const BspTreeAlpha&> (bt);
      
      /* copy alpha vectors */
      if (bta.getAlphaVectors ())
	{
	  if (! m_alphaVectors)
	    m_alphaVectors = new std::vector<AlphaVector*> ();
	  for (unsigned int i=0; i<bta.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*bta.getAlphaVectorNth (i)));
	}
      
      /* copy max value. */
      m_maxValue = bta.getSubTreeMaxValue ();
    }
}

void BspTreeAlpha::leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
					  double *low, double *high) {
  const BspTreeAlpha &bta = static_cast<const BspTreeAlpha&> (bt);
  const BspTreeAlpha &btb = static_cast<const BspTreeAlpha&> (btr);
  
  if (bta.getAlphaVectors () && btb.getAlphaVectors ())
    {
      std::cerr << "[Error]: BspTreeAlpha::leafDataIntersectInit: both cells have values ! Exiting...\n";
      exit (1);
    }
  else if (bta.getAlphaVectors ())
    {
      if (! m_alphaVectors)
	m_alphaVectors = new std::vector<AlphaVector*> ();
      for (unsigned int i=0; i<bta.getAlphaVectorsSize (); i++)
	  m_alphaVectors->push_back (new AlphaVector (*bta.getAlphaVectorNth (i)));
    }
  else if (btb.getAlphaVectors ())
    {
      if (! m_alphaVectors)
	m_alphaVectors = new std::vector<AlphaVector*> ();
      for (unsigned int i=0; i<btb.getAlphaVectorsSize (); i++)
	  m_alphaVectors->push_back (new AlphaVector (*btb.getAlphaVectorNth (i)));
    }
}


void BspTreeAlpha::addScalar (const double &scalar)
{
  if (! isLeaf ())
    {
      BspTreeAlpha *btalt = static_cast<BspTreeAlpha*> (getLowerTree ());
      btalt->addScalar (scalar);
      BspTreeAlpha *btage = static_cast<BspTreeAlpha*> (getGreaterTree ());
      btage->addScalar (scalar);
    }
  else
    {
      AlphaVector::addVecScalar (scalar, getAlphaVectors ());
    }
}

void BspTreeAlpha::multiplyByScalar (const double &scalar)
{
  if (! isLeaf ())
    {
      BspTreeAlpha *btalt = static_cast<BspTreeAlpha*> (getLowerTree ());
      btalt->multiplyByScalar (scalar);
      BspTreeAlpha *btage = static_cast<BspTreeAlpha*> (getGreaterTree ());
      btage->multiplyByScalar (scalar);
    }
  else if (m_alphaVectors) 
    AlphaVector::multiplyVecByScalar (scalar, m_alphaVectors);
}


void BspTreeAlpha::leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
					   double *low, double *high)
{
  const BspTreeAlpha &bta = static_cast<const BspTreeAlpha&> (bt);
  const BspTreeAlpha &btb = static_cast<const BspTreeAlpha&> (btr);

  /* delete current alpha vectors if any */
  if (m_alphaVectors) 
    {
      for (unsigned int i=0; i<m_alphaVectors->size (); i++)
	delete (*m_alphaVectors)[i];
      m_alphaVectors = 0;
    }

  if (! m_alphaVectors)
    m_alphaVectors = new std::vector<AlphaVector*> ();
  
  if (bta.getAlphaVectors () && btb.getAlphaVectors ())
    {
      double prob;
      if (bta.getAlphaVectorsSize () == 1) 
	{
	  prob = bta.getAlphaVectorNth (0)->getAlphaNth (0);
	  for (unsigned int i=0; i<btb.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*btb.getAlphaVectorNth (i)));
	}
      else if (btb.getAlphaVectorsSize () == 1)
	{
	  prob = btb.getAlphaVectorNth (0)->getAlphaNth (0);
	  for (unsigned int i=0; i<bta.getAlphaVectorsSize (); i++)
	    m_alphaVectors->push_back (new AlphaVector (*bta.getAlphaVectorNth (i)));
	}
      else 
	{
	  std::cerr << "[Error]: BspTreeAlpha: leafDataIntersectMult: requires a probability. Exiting...\n";
	  exit (1);
	}
      multiplyByScalar (prob);  /* multiply (by continuous outcome probability). */
    }
  /* else if (bta.getAlphaVectors ())
    {
      if (! m_alphaVectors)
	m_alphaVectors = new std::vector<AlphaVector*> ();
      for (size_t i = 0; i<bta.getAlphaVectorsSize (); i++)
	m_alphaVectors->push_back (new AlphaVector (*bta.getAlphaVectorNth (i)));
				   }
  else if (btb.getAlphaVectors ())
    {
      if (! m_alphaVectors)
	m_alphaVectors = new std::vector<AlphaVector*> ();
      for (size_t i=0; i<btb.getAlphaVectorsSize (); i++)
	m_alphaVectors->push_back (new AlphaVector (*btb.getAlphaVectorNth (i)));
	} */

  /* else { 
    m_alphaVectors->push_back (new AlphaVector (0.0));*/ 
  else { delete m_alphaVectors; m_alphaVectors = 0; }

    //delete m_alphaVectors;
}

void BspTreeAlpha::getActionsCoverage (std::map<int, double> *coverage,
				       double *low, double *high)
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
	  leafCoverage (coverage, low, high);
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension()] = getPosition ();
      BspTreeAlpha *btage = static_cast<BspTreeAlpha*> (getGreaterTree ());
      btage->getActionsCoverage (coverage, low, high);
      low[getDimension ()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      BspTreeAlpha *btalt = static_cast<BspTreeAlpha*> (getLowerTree ());
      btalt->getActionsCoverage (coverage, low, high);
      high[getDimension ()] = b;
    }
}

void BspTreeAlpha::leafCoverage (std::map<int, double> *coverage, 
				 double *low, double *high)
{
  double cov = 1.0;
  for (int i=0; i<m_nDim; i++)
    cov *= (high[i] - low[i]);

  for (unsigned int i=0; i<getAlphaVectorsSize (); i++)
    {
      AlphaVector *av = getAlphaVectorNth (i);
      std::set<int>::const_iterator actit;
      for (actit = av->getActionsBegin (); actit != av->getActionsEnd (); actit++)
	{
	  (*coverage)[(*actit)] += cov; /* add up local coverage for each action. */
	}
    }
}

void BspTreeAlpha::setLeavesValue (const double &val,
				   double *low, double *high)
{
    if (isLeaf ())
    {
	if (m_alphaVectors)
	{
	    for (size_t i=0; i<m_alphaVectors->size (); i++)
		(*m_alphaVectors)[i]->setAlphaNth (0, val);  /* Beware: only change first value
							     within the alpha vector. */
	}
    }
    else
    {
	double b = low[getDimension ()];
	low[getDimension()] = getPosition ();
	BspTreeAlpha *btage = static_cast<BspTreeAlpha*> (getGreaterTree ());
	btage->setLeavesValue (val, low, high);
	low[getDimension ()] = b;

	b = high[getDimension ()];
	high[getDimension ()] = getPosition ();
	BspTreeAlpha *btalt = static_cast<BspTreeAlpha*> (getLowerTree ());
	btalt->setLeavesValue (val, low, high);
	high[getDimension ()] = b; 
    }
}

/* printing */
void BspTreeAlpha::print (std::ostream &out, double *low, double *high)
{
  if (isLeaf ())
    {
      if (m_alphaVectors)
	{
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
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      static_cast<BspTreeAlpha*> (getGreaterTree ())->print (out, low, high);
      low[getDimension ()] = b;
      
      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      static_cast<BspTreeAlpha*> (getLowerTree ())->print (out, low, high);
      high[getDimension ()] = b;

      //std::cout << "max value: " << getSubTreeMaxValue () << std::endl;
    }
}

void BspTreeAlpha::plotNDPointAction (std::ofstream &output_values, double *step,
				      double *low, double *high)
{
  int *npts = new int[m_nDim];
  int *bins = new int[m_nDim];
  int *counters = new int[m_nDim];
  double *pos = new double[m_nDim];
  int tpts = 1;

  for (int i=0; i<m_nDim; i++)
    {
#if !defined __GNUC__ || __GNUC__ < 3
      npts[i] = ceil ((high[i] - low[i]) / step[i]);
#else
      npts[i] = lround ((high[i] - low[i]) / step[i]);
#endif
      if (i == 0) bins[0] = 1;
      else bins[i] = bins[i-1] * npts[i-1];

      tpts *= npts[i];

      counters[i] = 0;
      pos[i] = low[i];
    }

  int cpts = 0;
  while (cpts < tpts)
    {
      /* write point coordinates and value. */
      if (BspTree::m_plotPointFormat == GnuplotF)
	plotPointActionGnuplot (output_values, pos);
      else if (BspTree::m_plotPointFormat == MathematicaF)
	plotPointActionMathematica (output_values, pos);
      else
	{
	  std::cerr << "[Error]:BspTree::plotNDPointValues: unknown output format: "
		    << BspTree::m_plotPointFormat << ". Exiting.\n";
	  exit (-1);
	}

      /* update coordinates. */
      for (int i=m_nDim-1; i>=0; i--)
	{
	  counters[i]++;
	  if (counters[i] == bins[i])
	    {
	      if (i == m_nDim-1)
		output_values << std::endl;

	      counters[i] = 0;
	      pos[i] += step[i];
	      
	      /* reset lower dimensions. */
	      for (int j=0; j<i; j++)
		{
		  counters[j] = 0;
		  pos[j] = low[j];
		}
	      break;
	    }
	}
      cpts++;
    }
  
  delete []npts; delete []bins; delete []counters; delete []pos;
}

void BspTreeAlpha::plotPointActionMathematica (std::ofstream &output_value, double *pos)
{
  output_value << getPointAction (pos) << " ";
}

void BspTreeAlpha::plotPointActionGnuplot (std::ofstream &output_value, double *pos)
{
  for (int k=m_nDim-1; k>=0; k--)
    output_value << pos[k] << " ";
  output_value << getPointAction (pos) << std::endl;
}

int BspTreeAlpha::getPointAction (double *pos)
{
  if (isLeaf ())
    return getPointActionInLeaf (pos);
  else
    {
      if (pos[getDimension ()] < getPosition ())
	return static_cast<BspTreeAlpha*> (getLowerTree ())->getPointAction (pos);
      else return static_cast<BspTreeAlpha*> (getGreaterTree ())->getPointAction (pos);
    }
}

} /* end of namespace */
