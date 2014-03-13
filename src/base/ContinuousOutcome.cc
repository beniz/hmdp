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

#include "ContinuousOutcome.h"
#include <math.h>
#include <cstdlib>

namespace hmdp_base
{

ContinuousOutcome::ContinuousOutcome (int dim)
  : BspTree (dim), m_leafProbability (-1.0), m_shiftBack (0),
    m_lowPos (0), m_highPos (0)
{
  m_bspType = ContinuousOutcomeT;
}

ContinuousOutcome::ContinuousOutcome (int dim, int d, double pos)
  : BspTree (dim, d, pos), m_leafProbability (-1.0), m_shiftBack (0),
    m_lowPos (0), m_highPos (0)
{
  m_bspType = ContinuousOutcomeT;
}

ContinuousOutcome::ContinuousOutcome (int dim, double *lowPos, double *highPos,
				      const double *low, const double *high,
				      const double *shift, const double &prob, const bool *relative)
  : BspTree (dim), m_leafProbability (-1.0)
{
  m_bspType = ContinuousOutcomeT;
  
  m_shiftBack = new double[m_nDim];
  m_lowPos = new double[m_nDim];
  m_highPos = new double[m_nDim];

  ContinuousOutcome *co_n = this;

  for (int i=0; i<m_nDim; i++)
    {
      if (shift)
	m_shiftBack[i] = -shift[i];
      m_highPos[i] = highPos[i];
      m_lowPos[i] = lowPos[i];
      
      /* higher bound */
      co_n->setDimension (i);

      /* check if higher bound belongs to the domain. */
      if (m_highPos[i] > high[i]
	  || (shift && (m_highPos[i] + shift[i] > high[i])))
	{
	  if (shift && (fabs (m_highPos[i] + shift[i] - high[i])
			>= fabs (lowPos[i] - highPos[i])))
	    {
	      /* skip the piece and update the number of continuous outcomes */
	      throw OutcomeOutOfBoundsException ();
	    }
	  if (! shift) m_highPos[i] = high[i];
	  else m_highPos[i] = high[i] - shift[i];
	}

      if (shift)
	{
	  if (relative[i])  /* transition is relative */
	    {
	      co_n->setPosition (m_highPos[i]);
	      co_n->shiftPosition (shift[i]);
	    }
	  else co_n->setPosition (shift[i]);  /* transition is absolute */
	}
      else co_n->setPosition (m_highPos[i]);
      m_highPos[i] = co_n->getPosition ();

      /* check if lower bound belongs to the domain. */
      if (m_lowPos[i] < low[i]
	  || (shift && (m_lowPos[i] + shift[i] < low[i])))
	{
	  if (shift && (fabs (m_lowPos[i] + shift[i] - low[i])
			>= fabs (lowPos[i] - highPos[i])))
	    {
	      /* skip the piece and update the number of continuous outcomes */
	       throw OutcomeOutOfBoundsException ();
	    }
	   if (! shift) m_lowPos[i] = low[i];
	   else m_lowPos[i] = low[i] - shift[i];
	}

      co_n->setLowerTree (new ContinuousOutcome (m_nDim, i, m_lowPos[i]));
      co_n->setGreaterTree (new ContinuousOutcome (m_nDim));
      co_n = static_cast<ContinuousOutcome*> (co_n->getLowerTree ());

      /* lower bound */
      if (shift)
	{
	  if (relative[i])  /* transition is relative */
	    co_n->shiftPosition (shift[i]);
	  else co_n->setPosition (shift[i]);  /* transition is absolute */
	}
      m_lowPos[i] = co_n->getPosition ();
      
      co_n->setLowerTree (new ContinuousOutcome (m_nDim));
      co_n->setGreaterTree (new ContinuousOutcome (m_nDim));
      co_n = static_cast<ContinuousOutcome*> (co_n->getGreaterTree ());
    }
  co_n->setProbability (prob);
}

ContinuousOutcome::ContinuousOutcome (const ContinuousOutcome &co)
  : BspTree (co.getSpaceDimension (), co.getDimension (), co.getPosition ()),
    m_leafProbability (co.getProbability ()), m_shiftBack (0),
    m_lowPos (0), m_highPos (0)
{
  m_bspType = ContinuousOutcomeT;
  
  /* copy shiftBack and pos */
  if (co.getShiftBack ())
    {
      m_shiftBack = new double[m_nDim];
      m_lowPos = new double[m_nDim];
      m_highPos = new double[m_nDim];
      for (int i=0; i<m_nDim; i++)
	{
	  m_shiftBack[i] = co.getShiftBack (i);
	  m_lowPos[i] = co.getLowPos (i);
	  m_highPos[i] = co.getHighPos (i);
	}
    }
  
  if (! co.isLeaf ())
    {
      ContinuousOutcome *colt = static_cast<ContinuousOutcome*> (co.getLowerTree ());
      m_lt = new ContinuousOutcome (*colt);
      ContinuousOutcome *coge = static_cast<ContinuousOutcome*> (co.getGreaterTree ());
      m_ge = new ContinuousOutcome (*coge);
    }
  else m_lt = m_ge = 0;
}

ContinuousOutcome::~ContinuousOutcome ()
{
  if (m_shiftBack)
    {
      delete []m_shiftBack; delete []m_lowPos; delete []m_highPos;
    }
}

void ContinuousOutcome::transferData (const BspTree &bt)
{
  std::cout << "Continuous outcome transfer data: TODO\n";
}

/* static functions */
ContinuousOutcome* ContinuousOutcome::convertDiscretePoint (const int &dim, double *lowPos, double *highPos,
							    const double *low, const double *high,
							    const std::pair<double, double*> &point,
							    const bool *relative)
{
  try 
    {
      return new ContinuousOutcome (dim, lowPos, highPos, low, high, point.second, point.first, relative);
    }
  catch (OutcomeOutOfBoundsException ex)
    {
      //std::cout << "Exception occured: " << ex.what () << std::endl;
      return 0;
    }
}

ContinuousOutcome* ContinuousOutcome::convertOppositeDiscretePoint (const int &dim, double *lowPos, double *highPos,
								    const double *low, const double *high,
								    const std::pair<double, double*> &point,
								    const bool *relative)
{
  try 
    {
      double pos[dim];
      for (int i=0; i<dim; i++)
	pos[i] = - point.second[i];
      return new ContinuousOutcome (dim, lowPos, highPos, low, high, pos, point.first, relative);
    }
  catch (OutcomeOutOfBoundsException ex)
    {
      //std::cout << "Exception occured: " << ex.what () << std::endl;
      return 0;
    }
}

ContinuousOutcome** ContinuousOutcome::convertMDDiscreteDistribution (double *lowPos, double *highPos,
								      const double *low, const double *high,
								      const MDDiscreteDistribution &mdd,
								      const bool *relative,
								      int &noutcomes)
{
  ContinuousOutcome **res = new ContinuousOutcome*[mdd.getNPoints()];
  int counter = 0;
  for (int i=0; i<mdd.getNPoints (); i++)
    {
      std::pair<double, double*> *pointPM = mdd.getPointAndBinMass (i);
      res[counter] = ContinuousOutcome::convertDiscretePoint (mdd.getDimension (), lowPos, highPos,
							      //low, high, mdd.getPoint (i), relative);
							      low, high, *pointPM, relative);
      delete pointPM;
      
      //debug
      /* if (res[i])
	{
	std::cout << "outcome:\n";
	res[i]->print (std::cout, low, high);
	} */
	
      if (res[counter])
	counter++;
    }
  
  /* realloc memory because some outcomes can have been discarded (out of bound). */
  if (counter != mdd.getNPoints ())
    {
      res = (ContinuousOutcome**) realloc (res, counter * sizeof (ContinuousOutcome *));
      noutcomes = counter;  /* set the valid number of outcomes */
    }
  
  return res;
}

ContinuousOutcome** ContinuousOutcome::convertOppositeMDDiscreteDistribution (double *lowPos, double *highPos,
									      const double *low, const double *high,
									      const MDDiscreteDistribution &mdd,
									      const bool *relative,
									      int &noutcomes)
{
  ContinuousOutcome **res = new ContinuousOutcome*[mdd.getNPoints()];
  int counter = 0;
  for (int i=0; i<mdd.getNPoints (); i++)
    {
      std::pair<double, double*> *pointPM = mdd.getPointAndBinMass (i);
      res[counter] = ContinuousOutcome::convertOppositeDiscretePoint (mdd.getDimension (), lowPos, highPos,
								      low, high, *pointPM, relative);
      delete pointPM;

      //debug
      /* if (res[i])
	 {
	 std::cout << "outcome:\n";
	 res[i]->print (std::cout, low, high);
	 } */
	 
      if (res[i])
	counter++;
    }
  
  /* realloc memory because some outcomes can have been discarded (out of bound). */
  if (counter != mdd.getNPoints ())
    {
      res = (ContinuousOutcome**) realloc (res, counter * sizeof (ContinuousOutcome *));
      noutcomes = counter;  /* set the valid number of outcomes */
    }
  
  return res;
}

/* printing */
void ContinuousOutcome::print (std::ostream &out,
			       double *low, double *high)
{
  if (isLeaf ())
    {
      if (getProbability () >= 0.0)
	{
	  out << "{";
	  for (int i=0; i<getSpaceDimension (); i++)
	    {
	      out << "[" << low[i] << "," << high[i];
	      if (i == getSpaceDimension () - 1)
		out << "]} - probability: ";
	      else out << "],";
	    }
	  std::cout << getProbability () << std::endl;
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      getGreaterTree ()->print (out, low, high);
      low[getDimension ()] = b;
      
      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      getLowerTree ()->print (out, low, high);
      high[getDimension ()] = b;
    }
}

} /* end of namespace */
