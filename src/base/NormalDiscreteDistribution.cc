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

#include "NormalDiscreteDistribution.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

using std::cout;
using std::endl;

//#define DEBUG 1

namespace hmdp_base
{
NormalDiscreteDistribution::NormalDiscreteDistribution (double mean, double sd,
							double epsilon, double interval,
							discretizationType dt)
  : NormalDistribution (mean, sd, interval), m_dtype (dt)
{
  if (m_dtype == DISCRETIZATION_WRT_INTERVAL)
    {

#ifdef DEBUG
      std::cout << "interval discretization: " << m_interval << std::endl;
#endif

      m_epsilon = epsilon; m_threshold = 0.0;
      discretizeWrtInterval ();
    }  
  else if (m_dtype == DISCRETIZATION_WRT_THRESHOLD)
    {
      m_threshold = epsilon; m_epsilon = 0.0;
      discretizeWrtThreshold ();
    }
  else 
    {
      cout << "[Error]: unknown discretization type:" << dt << endl;
      exit (-1);
    }

#ifdef DEBUG
  //debug
  std::cout << "[Debug]:NormalDiscreteDistribution: integral value: "
	    << integral () << std::endl;
  //debug
#endif

  assert (integral () > 0.0);
}

NormalDiscreteDistribution::NormalDiscreteDistribution (double mean, double sd,
							double epsilon, int nbins)
  : NormalDistribution (mean, sd, nbins, 0.0), m_dtype (DISCRETIZATION_WRT_POINTS),
    m_epsilon (epsilon)
{

#ifdef DEBUG
  std::cout << "points discretization\n";
#endif

  discretizeWrtPoints ();

#ifdef DEBUG
   //debug
  std::cout << "[Debug]:NormalDiscreteDistribution: integral value: "
	    << integral () << std::endl;
  //debug
#endif
}

NormalDiscreteDistribution::NormalDiscreteDistribution (const NormalDiscreteDistribution &ndd)
  : NormalDistribution (ndd), m_dtype (ndd.getDiscretizationType ()),
    m_epsilon (ndd.getEpsilon ()), m_threshold (ndd.getThreshold ())
{}

NormalDiscreteDistribution::~NormalDiscreteDistribution ()
{}

void NormalDiscreteDistribution::discretizeWrtInterval ()
{
  m_low = cdfToX (m_epsilon);
  m_high = cdfToX (1.0-m_epsilon);
  
  m_high = ceil ((m_high - m_mean + m_interval * 0.5)/m_interval)
		* m_interval + m_mean + m_interval * 0.5;
  m_low = m_mean - m_interval * 0.5
    - ceil ((m_mean - m_interval * 0.5 - m_low)/m_interval) * m_interval;
#if !defined __GNUC__ || __GNUC__ < 3
  m_nbins = ceil ((m_high - m_low) / m_interval - 1);
#else
  m_nbins = lround ((m_high - m_low) / m_interval - 1);
#endif
  if (m_nbins == 0) m_nbins = 1;
  
  discretize ();
}

void NormalDiscreteDistribution::discretizeWrtPoints ()
{
  m_low = cdfToX (m_epsilon);
  m_high = cdfToX (1.0-m_epsilon);
  
  m_interval = (m_high - m_low) / m_nbins;

  discretize ();
}

void NormalDiscreteDistribution::discretizeWrtThreshold ()
{
  /* evaluate number of points and adjust bounds */
  double x = m_mean, pdfx;
  m_nbins = 1;  /* one point at least for the mean */
  while ((pdfx = pdf (x)) > m_threshold)
    {
      x += m_interval * 0.5;
      m_nbins++;
    }
  if (pdfx < m_threshold)
    {
      m_threshold = pdfx;  /* so we're sure we include the
			      threshold in the discretized distribution */
    }
  m_high = x;
  m_low = m_mean - (x - m_mean);

  double normalize = 0.0;
  m_x = (double*) calloc (m_nbins,sizeof (double));
  m_p = (double*) calloc (m_nbins,sizeof (double));

  int bins = 0;
  for (int i=0; i<m_nbins; i++)
    {
      m_x[i] = m_low + m_interval * i;

      if ((DiscreteDistribution::m_positiveResourcesConsumptionTruncation 
	   //&& (m_low + m_interval * (i+1)) <= 0.0)
	   && (m_x[i] < 0.0))
	  || !DiscreteDistribution::m_positiveResourcesConsumptionTruncation)
	{
	  m_p[i] = pdf (m_x[i]) / m_interval;
	  normalize += m_p[i] * m_interval;
	  bins++;
	}
      else if (DiscreteDistribution::m_positiveResourcesConsumptionTruncation)
	{
	  /* create one last point at the fringe. */
	  /* m_p[i] = pdf (0.0) / fabs (m_low + m_interval*(i-1));
	     bins++;
	     normalize += m_p[i] * fabs (m_low + m_interval*(i-1)); */
	  for (int j=i; j<m_nbins; j++)
	    normalize += pdf (m_low + m_interval * j);
	  //m_x[i] = 0.0;
	  break;
	}
    } 
  m_nbins = bins;
  
  for (int i=0; i<m_nbins; i++)
    m_p[i] /= normalize;
}

void NormalDiscreteDistribution::discretize ()
{
  //double normalize = cdf (m_high) - cdf (m_low);
  //cdf (m_low+m_interval/2.0);
  m_x = (double*) calloc (m_nbins,sizeof (double));
  m_p = (double*) calloc (m_nbins,sizeof (double));

  double normalize = 0.0;
  int bins = 0;

#ifdef DEBUG
  std::cout << "m_nbins: " << m_nbins << std::endl;
#endif

  for  (int i=0; i<m_nbins; i++)
    {
      m_x[i] = m_low + m_interval * (2.0 * i + 1) / 2.0;
      
      if ((DiscreteDistribution::m_positiveResourcesConsumptionTruncation 
	   && (m_x[i] <= 0.0))
	  || !DiscreteDistribution::m_positiveResourcesConsumptionTruncation)
	{
	  m_p[i] = (cdf (m_low + m_interval * (i+1))
		    - cdf (m_low + m_interval * i)) / m_interval; // / normalize;
	  assert (m_p[i] <= 1.0);
	  assert (m_p[i] >= 0.0);
	  normalize += m_p[i] * m_interval;
	  bins++;
	}
      else if (DiscreteDistribution::m_positiveResourcesConsumptionTruncation)
	{
	  /* create one last point at the fringe. */
	  /* m_p[i] = (cdf (0.0) - cdf (m_low + m_interval * i)) / fabs (m_low + m_interval*i);
	     assert (m_p[i] <= 1.0);
	     bins++; */
	  normalize += m_p[i] * fabs (m_low + m_interval * i) + cdf (m_high) - cdf (0.0);
	  //m_x[i] = 0.0;
	  break;
	}
    }
  m_nbins = bins;

  for (int i=0; i<m_nbins; i++)
    {
      m_p[i] /= normalize;
      assert (m_p[i] <= 1.0);
    }
}

} /* end of namespace */
