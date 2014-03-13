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

#ifndef DISCRETEDISTRIBUTION_H
#define DISCRETEDISTRIBUTION_H

#if !defined __GNUC__ || __GNUC__ < 3
#include <ostream.h>
#include <stdlib.h>
#else
#include <ostream>
#endif

namespace hmdp_base
{

enum discreteDistributionType
{
  GAUSSIAN = 0,
  UNIFORM = 1,
  NONE = 100    /* no distribution, used for representing deterministic consumptions. */
};

/**
 * \class DiscreteDistribution
 * \brief discrete representation of a uni-dimensional continuous distribution.
 */
class DiscreteDistribution
{
 public:
  /**
   * \brief constructor
   * @param type discrete distribution type (GAUSSIAN, UNIFORM, ...),
   * @param interval discretization interval.
   */
  DiscreteDistribution (discreteDistributionType type, const double &interval);

  /**
   * \brief constructor
   * @param type discrete distribution type (GAUSSIAN, UNIFORM, ...),
   * @param nbins number of discrete bins,
   * @param interval discretization interval.
   */
  DiscreteDistribution (discreteDistributionType type, const int &nbins, const double &interval);

  /**
   * \brief constructor
   * @param type discrete distribution type (GAUSSIAN, UNIFORM, ...),
   * @param nbins number of discrete bins,
   * @param interval discretization interval,
   * @param values bins values,
   * @param probs bins probabilities.
   */
  DiscreteDistribution (discreteDistributionType type, const int &nbins, 
			const double &interval, double *values, double *probs);
  
  /**
   * \brief copy constructor.
   */
  DiscreteDistribution (const DiscreteDistribution &dd);

  ~DiscreteDistribution ();
  
  /* accessors */
  discreteDistributionType getType () const { return m_type; }
  int getNBins () const { return m_nbins; }
  double getXAtBin (int i) const { return m_x[i]; }
  double getProbaAtBin (int i) const { return m_p[i]; }
  double getLowerBound () const { return m_low; }
  double getUpperBound () const { return m_high; }
  double getInterval () const { return m_interval; }
  
  double integral ();

 protected:
  discreteDistributionType m_type;
  int m_nbins;  /**< number of discretization bins */
  double *m_x;  /**< discretized bins (a single value per bin) */
  double *m_p;  /**< probability per bin */
  double m_high; /**< upper bound */
  double m_low; /**< lower bound */
  double m_interval; /**< discretization interval */

 public:
  static bool m_positiveResourcesConsumptionTruncation; /**< Boolean flag for truncation of positive
							   'consumptions' (i.e. no replenishment). False is the default. */

};

std::ostream &operator<<(std::ostream &output, DiscreteDistribution &dd);

} /* end of namespace */
#endif
