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

#include "DiscreteDistribution.h"
#include "assert.h"
#include "math.h"
#include <stdlib.h>

namespace hmdp_base
{

  bool DiscreteDistribution::m_positiveResourcesConsumptionTruncation = false;  /* truncation by default. */

DiscreteDistribution::DiscreteDistribution (discreteDistributionType type, const double &interval)
  : m_type (type), m_nbins (0), m_high (0), m_low (0), m_interval (interval)
{}

DiscreteDistribution::DiscreteDistribution (discreteDistributionType type, const int &nbins, 
					    const double &interval)
  : m_type (type), m_nbins (nbins), m_high (0), m_low (0), m_interval (interval)
{}

  DiscreteDistribution::DiscreteDistribution (discreteDistributionType type, 
					      const int &nbins,
					      const double &interval, double *values, 
					      double *probs)
    : m_type (type), m_nbins (nbins), m_high (0), m_low (0), m_interval (interval)
  {
    if (m_type == NONE)  /* deterministic: fake a single point distribution */
      {
	assert ( m_nbins == 1);
	m_x = (double *) malloc (sizeof (double)); 
	m_x[0] = values[0];
	m_p = (double *) malloc (sizeof (double));
	m_p[0] = probs[0] / interval;
      }
    else
      {
	m_x = (double *) malloc (m_nbins * sizeof (double));
	m_p = (double *) malloc (m_nbins * sizeof (double));
	for (int i=0; i<m_nbins; i++)
	  {
	    m_x[i] = values[i];
	    m_p[i] = probs[i] / interval;
	  }
      }
  }

DiscreteDistribution::DiscreteDistribution (const DiscreteDistribution &dd)
  : m_type (dd.getType ()), m_nbins (dd.getNBins ()),
    m_high (dd.getUpperBound ()), m_low (dd.getLowerBound ()),
    m_interval (dd.getInterval ())
{
  m_x = (double*) malloc (m_nbins * sizeof (double));
  m_p = (double*) malloc (m_nbins * sizeof (double));
  for (int i=0; i<m_nbins; i++)
    {
      m_x[i] = dd.getXAtBin (i);
      m_p[i] = dd.getProbaAtBin (i);
    }
}

DiscreteDistribution::~DiscreteDistribution ()
{
  free (m_x); free (m_p);
}

std::ostream &operator<<(std::ostream &output, DiscreteDistribution &dd)
{
  output << "nBins: " << dd.getNBins () << " in [" 
	 << dd.getLowerBound () << ", " << dd.getUpperBound () << "]\n";
  for (int i=0; i<dd.getNBins (); i++)
    {
      output << dd.getXAtBin (i) << " -- " << dd.getProbaAtBin (i) << '\n';
    }
  return output;
}

double DiscreteDistribution::integral ()
{
  double integral = 0.0;
  for (int i=0; i<m_nbins; i++)
    {
      if (DiscreteDistribution::m_positiveResourcesConsumptionTruncation
	  && (m_x[i] == 0.0))
	integral += m_p[i] * fabs (m_low + m_interval * i);
      else
	integral += m_p[i] * m_interval;
    }
  return integral;
}

} /* end of namespace */
