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

#include "MDDiscreteDistribution.h"
#include "Alg.h" /* double precision */
#include "ContinuousStateDistribution.h"
#include <algorithm> /* min */
#include <stdlib.h>
#include <assert.h>
#include <iostream>

//#define DEBUG 1

namespace hmdp_base
{

int MDDiscreteDistribution::m_counterpoints = -1;

MDDiscreteDistribution::MDDiscreteDistribution (const int &dim, const int &npoints)
    : m_dimension (dim), m_nPoints (npoints) 
{
  //m_dist = new std::pair<double,double*>();
  m_dist.resize(m_nPoints);
  //= (std::pair<double,double*> *) malloc (m_nPoints * sizeof (std::pair<double, double*>));
  m_intervals = new double[m_dimension];//(double *) calloc (m_dimension, sizeof (double));
  m_dimPoints = new int[m_dimension]();//(int *) calloc (m_dimension, sizeof (int));
}

MDDiscreteDistribution::MDDiscreteDistribution (const int &dim,
						double *lowPos, double *highPos,
						double *intervals)
  : m_dimension (dim), m_nPoints (1)
{
  m_intervals = new double[m_dimension];//(double *) calloc (m_dimension, sizeof (double));
  m_dimPoints = new int[m_dimension]();//(int *) calloc (m_dimension, sizeof (int));
  
  for (int d=0; d<m_dimension; d++)
    {
      m_intervals [d] = intervals[d];
#if !defined __GNUC__ || __GNUC__ < 3
      m_dimPoints[d] = static_cast<int> (ceil ((highPos[d] - lowPos[d]) / m_intervals[d]));
#else
      m_dimPoints[d] = lround ((highPos[d] - lowPos[d]) / m_intervals[d]);
#endif
      if (m_dimPoints[d] == 0) m_dimPoints[0] = 1;
      m_nPoints *= m_dimPoints[d];
    }
  //m_dist = new std::pair<double,double*>();
  /*m_dist 
    = (std::pair<double,double*> *) malloc (m_nPoints * sizeof (std::pair<double, double*>));*/
  m_dist.resize(m_nPoints);
  
  /* discretize */
  discretize (lowPos, highPos);
}

MDDiscreteDistribution::MDDiscreteDistribution (const MDDiscreteDistribution &mdd)
  : m_dimension (mdd.getDimension ()), m_nPoints (mdd.getNPoints ())
{
  /* copy the distribution and intervals */
  //m_dist = new std::pair<double,double*>();
  m_dist.resize(m_nPoints);
  //= (std::pair<double,double*> *) malloc (m_nPoints * sizeof (std::pair<double, double*>));
  m_intervals = new double[m_dimension];//(double *) calloc (m_dimension, sizeof (double));
  m_dimPoints = new int[m_dimension]();//(int *) calloc (m_dimension, sizeof (int));
  /*m_dist = (std::pair<double,double*> *) malloc (m_nPoints * sizeof (std::pair<double,double*>));
  m_intervals = (double*) calloc (m_dimension, sizeof (double));
  m_dimPoints = (int *) calloc (m_dimension, sizeof (int));*/
  
  for (int i=0; i<m_dimension; i++)
    {
      m_intervals[i] = mdd.getDiscretizationInterval (i);
      m_dimPoints[i] = mdd.getDimPoints (i);
    }

  for (int i=0; i<m_nPoints; i++)
    {
      double prob = mdd.getProbability (i);
      double *tab = (double*) malloc (m_dimension * sizeof (double));
      for (int j=0; j<m_dimension; j++)
	  tab[j] = mdd.getPosition (i,j);
      m_dist[i] = std::pair<double,double*> (prob, tab);
    }
}

MDDiscreteDistribution::MDDiscreteDistribution (ContinuousStateDistribution *csd,
						double *low, double *high)
  : m_dimension (csd->getSpaceDimension ()), m_nPoints (0)
{
  /* count csd leaves */
  int dist_size = csd->countPositiveLeaves ();
  //m_dist = (std::pair<double, double*> *) malloc (dist_size * sizeof (std::pair<double,double*>));
  m_dist.resize(dist_size);
  m_intervals = (double*) calloc (m_dimension, sizeof (double));
  m_dimPoints = (int*) malloc (m_dimension * sizeof (int));
  for (int i=0; i<m_dimension; i++)
    m_dimPoints[i] = -1;

  /* convert leaves */
  convertContinuousStateDistribution (csd, low, high);
}

MDDiscreteDistribution::~MDDiscreteDistribution ()
{
  /*for (int i=0; i<m_nPoints; i++) 
    delete[] m_dist[i].second;
    delete[] m_dist;*/
  delete[] m_intervals;
  delete[] m_dimPoints;
}

void MDDiscreteDistribution::createMDDiscreteBin (const int &pt, const double &proba, double *pos)
{
  m_dist[pt] = std::pair<double, double*> (proba, pos);
}

void MDDiscreteDistribution::convertContinuousStateDistribution (ContinuousStateDistribution *csd, double *low, double *high)
{
  if (csd->isLeaf () && csd->getProbability ())
    {
      double *pos = (double*) malloc (getDimension () * sizeof (double));
      for (int i=0; i<getDimension (); i++) 
	{
	  pos[i] = (high[i] + low[i]) / 2.0;
	  
	  /* the discretization interval in dimension i is set to the size
	     of the smallest cell (in that dimension). Beware, in fact the discretization
	     interval is not uniform in a given dimension (following the bsp tree...). */
	  if (m_intervals[i])
	    m_intervals[i] = std::min (m_intervals[i], high[i] - low[i]);
	  else m_intervals[i] = high[i] - low[i];
	}
      m_dist[m_nPoints++] = std::pair<double, double*> (csd->getProbability (), pos);
    }
  else if (csd->isLeaf ()) {}
  else
    {
      m_dimPoints[csd->getDimension ()]++;

      double b=high[csd->getDimension ()];
      high[csd->getDimension ()] = csd->getPosition ();
      ContinuousStateDistribution *csdlt = static_cast<ContinuousStateDistribution*> (csd->getLowerTree ());
      convertContinuousStateDistribution (csdlt, low, high);
      high[csd->getDimension ()] = b;

      b = low[csd->getDimension ()];
      low[csd->getDimension ()] = csd->getPosition ();
      ContinuousStateDistribution *csdge = static_cast<ContinuousStateDistribution*> (csd->getGreaterTree ());
      convertContinuousStateDistribution (csdge, low, high);
      low[csd->getDimension ()] = b;
    }
}

/* static version */
std::pair<double, double*> MDDiscreteDistribution::createMDDiscreteBinS (const double &proba,
									  double *pos)
{
  return std::pair<double, double*> (proba, pos);
}

void MDDiscreteDistribution::discretize (double *lowPos, double *highPos)
{
  double bins[m_dimension], counters[m_dimension], pts[m_dimension];
  for (int d=0; d<m_dimension; d++)
    {
      if (d == 0)
	bins[d] = 1;
      else bins[d] = m_dimPoints[d] * bins[d-1];
      counters[d] = 0; pts[d] = 0;
      
#ifdef DEBUG
      //debug
      std::cout << "d: " << d << std::endl;
      std::cout << "interval: " << m_intervals[d] 
		<< " -- dimPoints: " << m_dimPoints[d]
		<< " -- bins: " << bins[d] << std::endl;
      //debug
#endif
    }

  int count = 0;
  while (count < m_nPoints)
    {
      double *pos = (double*) malloc (m_dimension * sizeof (double));
      for (int d=m_dimension-1; d>=0; d--)
	{
	  counters[d]++;
	  if (count == 0) 
	    {
	      //pos[d] = lowPos[d] + m_intervals[d] / 2.0;
	      pos[d] = lowPos[d];
	      pts[d]++;
	      counters[d] = 0;
	    }
	  else if (counters[d] == bins[d])
	    {
	      counters[d] = 0;
	      //pos[d] = lowPos[d] + m_intervals[d] * (2.0 * pts[d] + 1) / 2.0;
	      pos[d] = lowPos[d] + m_intervals[d] * pts[d];
	      pts[d]++;
	      if (d > 0)
		/* pts[d-1] = 0; */
		for (int j=0; j<d; j++)
		  pts[j] = 0;
	    }
	  else if (count > 0) pos[d] = getPosition (count-1, d);  /* remain at the same position 
								     in this dimension */
	  
	}
      createMDDiscreteBin (count, 0.0, pos);
      count++;
    }
}

/* TODO: speed up */
double MDDiscreteDistribution::getProbability (double *pos) const
{
  bool not_in_bounds = true;
  for (int i=0; i<m_dimension; i++)
    {
      if (pos[i] > getMinPosition (i) - m_intervals[i]/2.0  
      && pos[i] < getMaxPosition (i) + m_intervals[i]/2.0)
	{
	  not_in_bounds = false;
	  break;
	}
    }
  if (not_in_bounds)
  return 0.0;

  for (int i=0; i<m_nPoints; i++)
    {
      int d = 0;
      while (d < m_dimension)
	{
	  if (m_dist[i].second[d] - m_intervals[d] / 2.0 <= pos[d]
	      && m_dist[i].second[d] + m_intervals[d] / 2.0 >= pos[d])
	    d++;
	  else break;
	  if (d == m_dimension)
	    {
	      return m_dist[i].first;
	    }
	}
    }
  return 0.0;
}

/* static methods */
int MDDiscreteDistribution::jointIndependentDiscretePoints (const int &nDists, DiscreteDistribution **dists)
{
  int jDpts = 1;
  for (int i=0; i<nDists; i++)
    jDpts *= dists[i]->getNBins ();
  return jDpts;
}

MDDiscreteDistribution* MDDiscreteDistribution::jointDiscreteDistribution (const int &nDists,
									   DiscreteDistribution **dists)
{
  MDDiscreteDistribution::m_counterpoints = -1;
  int jidd = MDDiscreteDistribution::jointIndependentDiscretePoints (nDists, dists);
  MDDiscreteDistribution *jointD = new MDDiscreteDistribution (nDists, jidd);
  //double *pos = (double *) malloc (nDists * sizeof (double));
  double *pos = new double[nDists];
  jointD = MDDiscreteDistribution::jointDiscreteDistribution (dists, 0, pos, 1.0, jointD);
  if (MDDiscreteDistribution::m_counterpoints+1 != jidd)
    {
      jointD->reallocPoints (MDDiscreteDistribution::m_counterpoints+1);
    }
  for (int i=0; i<nDists; i++)
    {
      jointD->setDimPoint (i, dists[i]->getNBins ());
      jointD->setDiscretizationInterval (i, dists[i]->getInterval ());
    }  
  delete[] pos;
  return jointD;
}

MDDiscreteDistribution* MDDiscreteDistribution::jointDiscreteDistribution (DiscreteDistribution **dists,
									   const int &d, double *pos, 
									   const double &prob,
									   MDDiscreteDistribution *mdd)
{
  double pr;
  
  for (int i=0; i<dists[d]->getNBins (); i++)  /* iterate points in dimension d */
    {
      pos[d]=dists[d]->getXAtBin (i);
      pr = prob * dists[d]->getProbaAtBin (i);
      
      if (d == mdd->getDimension () - 1)  /* end of a joint product */
	{
	  if (pr > 0.0)
	    {
	      double *positions = (double *) malloc (mdd->getDimension () * sizeof (double));
	      for (int j=0; j<mdd->getDimension (); j++)
		positions[j] = pos[j];
	      mdd->createMDDiscreteBin (++m_counterpoints, pr, positions);
	    }
	}
      else MDDiscreteDistribution::jointDiscreteDistribution (dists, d+1, pos, pr, mdd);
    }
  return mdd;
}

MDDiscreteDistribution* MDDiscreteDistribution::resizeDiscretization (const MDDiscreteDistribution &mddist,
								      double *lowPos, double *highPos,
								      double *intervals)
{
  MDDiscreteDistribution *rmddist = new MDDiscreteDistribution (mddist.getDimension (), lowPos, highPos,
								intervals);
  //std::cout << "rmddist: " << *rmddist << std::endl;

  //double pos[rmddist->getDimension ()];
  double sum = 0.0;
  for (int pt=0; pt<rmddist->getNPoints (); pt++)
    {
      /* for (int d=0; d<rmddist->getDimension (); d++)
	pos[d] = rmddist->getPosition (pt, d); */
      //rmddist->setProbability (pt, mddist.getProbability (pos));
      rmddist->setProbability (pt, mddist.getProbability (rmddist->getPosition (pt)));
      sum += rmddist->getProbability (pt);
    }
  if (sum != 1.0)
    rmddist->normalize (sum);
  return rmddist;
}

MDDiscreteDistribution* MDDiscreteDistribution::convoluteMDDiscreteDistributions (const MDDiscreteDistribution &mddist1,
										  const MDDiscreteDistribution &mddist2,
										  double *low, double *high,
										  double *lowPos1, double *highPos1, 
										  double *lowPos2, double *highPos2)
{
  if (mddist1.getDimension () != mddist2.getDimension ())
    {
      std::cout << "[Error]:MDDiscreteDistribution::convoluteMDDiscreteDistributions: distributions are on continuous spaces of different dimensions. Exiting...\n";
      exit (-1);
    }

  int dim = mddist1.getDimension ();
  
  /* create multi-dimensional distribution of proper size */
  double interval[dim], convLowPos[dim], convHighPos[dim];
  bool nullprob = false;
  for (int i=0; i<dim; i++)
    {
#ifdef DEBUG
      //debug
      std::cout << i << " mddist1 min: " << mddist1.getMinPosition (i)
		<< " -- mddist2 min: " << mddist2.getMinPosition (i)
		<< " -- lowPos1: " << lowPos1[i] << " -- lowPos2: " << lowPos2[i]
		<< std::endl;
      std::cout << i << " mddist1 max: " << mddist1.getMaxPosition (i)
		<< " -- mddist2 max: " << mddist2.getMaxPosition (i)
		<< " -- highPos1: " << highPos1[i] << " -- highPos2: " << highPos2[i]
		<< std::endl;
       //debug
#endif
      
      /* bounds */
      convLowPos[i] = std::max (std::max (mddist1.getMinPosition (i) - mddist1.getDiscretizationInterval (i)/2.0 
					  + mddist2.getMinPosition (i) - mddist2.getDiscretizationInterval (i)/2.0,
					  lowPos1[i] + lowPos2[i]), low[i]);
      convHighPos[i] = std::min (std::min (mddist1.getMaxPosition (i) + mddist1.getDiscretizationInterval (i)/2.0 
					   + mddist2.getMaxPosition (i) + mddist2.getDiscretizationInterval (i)/2.0,
					   highPos1[i] + highPos2[i]), high[i]);
      /* select the minimal discretization interval */
      interval[i] = std::min (mddist1.getDiscretizationInterval (i),
			    mddist2.getDiscretizationInterval (i));

#ifdef DEBUG
      //debug
      std::cout << i << " convLowPos: " << convLowPos[i]
		<< " -- convHighPos: " << convHighPos[i] 
		<< " -- inverval: " << interval[i]
		<< std::endl;
      //debug
#endif
      
      if (nullprob
	  || convLowPos[i] >= convHighPos[i])
	{
	  nullprob = true;
	  convLowPos[i] = low[i];
	  convHighPos[i] = high[i];
#if !defined __GNUC__ || __GNUC__ < 3
	  interval[i] = ceil (high[i] - low[i]);
#else
	  interval[i] = lround (high[i] - low[i]);
#endif
	}
    }

  /* we consider that if probability is null on a dimension, then 
     it should be null over the full continuous space. Therefore we return
     an empty distribution over the full space. */
  /* TODO: should not be empty. Should proceed with the convolution. */
  if (nullprob)
    {
      //debug
      //std::cout << "returning 'zero' convolution\n";
      //debug
      return new MDDiscreteDistribution (dim, convLowPos, convHighPos, interval);
    }

  MDDiscreteDistribution *cmddist = new MDDiscreteDistribution (dim, convLowPos, 
								convHighPos, interval);

#ifdef DEBUG
  //debug
  std::cout << "discrete convolution points: " << cmddist->getNPoints () << std::endl;
  /* std::cout << *cmddist << std::endl; */
  //debug
#endif
  
  /* sync discrete distributions and prepare for the convolution */
  double lowConv[dim], highConv[dim], intervalsConv[dim];
  int bins[dim], counters[dim];
  for (int j=0; j<dim; j++)
    {
      lowConv[j] = std::min (lowPos1[j], lowPos2[j]);      /* lower bounds for convolving values. */
      highConv[j] = std::max (highPos1[j], highPos2[j]);
      intervalsConv[j] = std::min (mddist1.getDiscretizationInterval (j),
				   mddist2.getDiscretizationInterval (j));  /* sync the discretization intervals */
      
      //debug
      /* std::cout << "rsc " << j << ": lowConv: " << lowConv[j]
	 << " -- highConv: " << highConv[j] << " -- interval: " << intervalsConv[j]
	 << std::endl; */
      //debug
    }
  
  /* resize discrete distributions: cmddist1, cmddist2 */
  /* MDDiscreteDistribution *cmddist1 
     = MDDiscreteDistribution::resizeDiscretization (mddist1, low, high, intervalsConv); */
  /*   MDDiscreteDistribution *cmddist2 
     = MDDiscreteDistribution::resizeDiscretization (mddist2, lowConv, highConv, intervalsConv); */
  MDDiscreteDistribution *cmddist1 
    = MDDiscreteDistribution::resizeDiscretization (mddist1, lowPos1, highPos1, intervalsConv);
  /* MDDiscreteDistribution *cmddist2 
    = MDDiscreteDistribution::resizeDiscretization (mddist2, lowPos2, highPos2, intervalsConv); */

  /* MDDiscreteDistribution *cmddist1 = new MDDiscreteDistribution (mddist1); */
  MDDiscreteDistribution *cmddist2 = new MDDiscreteDistribution (mddist2);

  //debug
  //std::cout << "resizing: " << *cmddist1 << *cmddist2 << std::endl;
  //debug

  for (int j=0; j<dim; j++)
    {
      if (j == 0)
	bins[j] = 1;
      else
	bins[j] = cmddist1->getDimPoints (j) * bins[j-1];
      counters[j] = 0;
      
      //debug
      //std::cout << "bins " << j << ": " << bins[j] << std::endl;
      //debug
    }
  
  int totalPos = cmddist1->getNPoints ();
  
  double sum = 0.0;
  
#ifdef DEBUG
  //debug
  std::cout << "convolute: totalPos: " << totalPos << std::endl;
  //debug
#endif

  /* multi-dimensional convolution in between bounds */
  for (int p=0; p<cmddist->getNPoints (); p++)  /* iterate points */
    {
      /* std::cout << "point: " << p << " -- position: " 
		<< cmddist->getPosition (p, 0) << ", " << cmddist->getPosition (p, 1)
		<< std::endl; */
      
      /* initialization */
      double val = 0.0;
      double pos1[dim], pos2[dim];
      for (int j=0; j<dim; j++)
	{
	  pos1[j] = cmddist1->getMinPosition (j);
	  //pos2[j] = cmddist->getPosition (p, j) - cmddist1->getMinPosition (j);
	  pos2[j] = cmddist->getPosition (p, j) - pos1[j];
	}
      
      int count = 0;
      while (count < totalPos)
	{
	  double val1 = cmddist1->getProbability (pos1);
	  double val2 = cmddist2->getProbability (pos2);
	  val += val1 * val2;
	  /* std::cout << "pos1: " << pos1[0] << ", " << pos1[1] << std::endl;
	     std::cout << "pos2: " << pos2[0] << ", " << pos2[1] << std::endl;
	     std::cout << "val1: " << val1 << " -- val2: " << val2 << std::endl; */
	  
	  for (int i=dim-1; i>=0; i--)
	    {
	      counters[i]++;
	      if (counters[i] == bins[i])
		{
		  counters[i] = 0;
		  pos1[i] += cmddist1->getDiscretizationInterval (i);
		  //pos2[i] -= cmddist2->getDiscretizationInterval (i);
		  pos2[i] = cmddist->getPosition (p,i) - pos1[i];

		  if (i > 0) 
		    {
		      for (int j=0; j<i; j++)
			{
			  counters[j] = 0;
			  pos1[j] = cmddist1->getMinPosition (j);
			  pos2[j] = cmddist->getPosition (p, j) - cmddist1->getMinPosition (j);
			}
		      /* pos1[i-1] = cmddist1->getMinPosition (i-1);
			 pos2[i-1] = cmddist->getPosition (p, i-1) - cmddist1->getMinPosition (i-1); */
		      break;
		    }
		}
	    }  /* end for */

	  count++;
	}
      //std::cout << " -- val: " << val << std::endl;
      sum += val;
      cmddist->setProbability (p, val);
    }

  /* normalize */
  if (sum)
    cmddist->normalize (sum);

  //debug
  /* std::cout << "MDDiscreteDistribution::convolute: result: "
     << *cmddist << std::endl; */
  //debug

  delete cmddist1; delete cmddist2;
  return cmddist;
}

void MDDiscreteDistribution::normalize (const double &sum)
{
  for (int i=0; i<m_nPoints; i++)
    setProbability (i, getProbability (i) / sum);
}

double MDDiscreteDistribution::getProbMass () const
{
  double mass = 0.0;
  for (int i=0; i<m_nPoints; i++)
    {
      mass += getProbMass (i);
    }
  return mass;
}

double MDDiscreteDistribution::getProbMass (const int &i) const
{
  double volume = 1.0;
  for (int j=0; j<m_dimension; j++)
    volume *= m_intervals[j];
  return volume * getProbability (i);
}

std::pair<double, double*>* MDDiscreteDistribution::getPointAndBinMass (const int &i) const
{
  std::pair<double, double*> *res 
    = new std::pair<double, double*> (std::pair<double, double*> (getProbMass (i), m_dist[i].second));
  return res;
}

  void MDDiscreteDistribution::reallocPoints (const int &nnpoints)
{
  m_nPoints = nnpoints;
  //m_dist = (std::pair<double,double*> *) realloc (m_dist, nnpoints * sizeof (std::pair<double, double*>));
  m_dist.resize(m_nPoints);
}

std::ostream&  MDDiscreteDistribution::print (std::ostream &output)
{
  output << "nPoints: " << getNPoints () 
	 << " -- dim: " << getDimension () << '\n';
  output << "intervals: ";
  for (int j=0; j<getDimension (); j++)
    output << "(" << j << ", " << m_intervals[j] << ") ";
  output << std::endl;
  
  for (int i=0; i<getNPoints (); i++)  /* iterate discrete points */
    {
      output << i << ": proba: " << getProbability (i) << "  [";
      for (int j=0; j<getDimension (); j++)
	output << getPosition (i, j) << ' ';
      output << "]\n";
    }

#ifdef DEBUG
  std::cout << "total probability mass: " << getProbMass () << std::endl;
#endif

  return output;
}

std::ostream &operator<<(std::ostream &output, MDDiscreteDistribution &mdd)
{
  return mdd.print (output);
}

std::ostream &operator<<(std::ostream &output, MDDiscreteDistribution *mdd)
{
  return mdd->print (output);
}

} /* end of namespace */
