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

#ifndef NORMALDISCRETEDISTRIBUTION_H
#define NORMALDISCRETEDISTRIBUTION_H

#include "NormalDistribution.h"
#include "discretizationTypes.h"

namespace hmdp_base
{

/**
 * \class NormalDiscreteDistribution
 * \brief Uni-dimensional discretized Normal distribution.
 */
class NormalDiscreteDistribution: public NormalDistribution
{
 public:
  /**
   * \brief constructor for discretization with threshold (absolute or probability mass).
   * @param mean distribution mean
   * @param sd distribution standard deviation
   * @param epsilon threshold (absolute, or in probability mass)
   * @param interval interval for discretization.
   * @param dt type of discretization: DISCRETIZATION_WRT_INTERVAL or
   *                                   DISCRETIZATION_WRT_THRESHOLD.
   */
  NormalDiscreteDistribution (double mean, double sd, 
			      double epsilon, double interval,
			      discretizationType dt);

  /**
   * \brief constructor for discretization based on the number of points.
   * @param mean distribution mean
   * @param sd distribution standard deviation
   * @param epsilon threshold, in probability mass
   * @param nbins number of discrete points.
   */
  NormalDiscreteDistribution (double mean, double sd,
			      double epsilon, int nbins);

  /**
   * \brief copy constructor
   */
  NormalDiscreteDistribution (const NormalDiscreteDistribution &ndd);
  
  ~NormalDiscreteDistribution ();
  
  /* accessors */
  discretizationType getDiscretizationType () const { return m_dtype; }
  double getEpsilon () const { return m_epsilon; }
  double getThreshold () const { return m_threshold; }
  
 private:
  void discretize();
  
  /**
   * \brief The discretization is specified by an interval size,
   *        and a lowest probability.
   */
  void discretizeWrtInterval ();

  /**
   * \brief The discretization is specified by a number of points.
   */
  void discretizeWrtPoints ();

  /**
   * \brief The discretization is specified by a threshold and 
   *        an interval size.
   */
  void discretizeWrtThreshold ();

 protected:

 private:
  discretizationType m_dtype;  /**< type of discretization */
  double m_epsilon;  /**< */
  double m_threshold; /**< threshold for discretization */
};

} /* end of namespace */
#endif
