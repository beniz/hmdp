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

/**
 * \brief Uni-dimensional Normal distribution and associated methods, from Rich Washington's C code.
 *        This class inherits DiscreteDistribution. That is weird, this is done to avoid 
 *        multiple inheritance.
 */

#ifndef NORMALDISTRIBUTION_H
#define NORMALDISTRIBUTION_H

#include "DiscreteDistribution.h"

namespace hmdp_base
{

/**
 * \class NormalDistribution
 * \brief Uni-dimensional Normal distribution and associated methods.
 */
class NormalDistribution : public DiscreteDistribution
{
 public:
  /**
   * \brief Constructor (default number of points to 0).
   * @param xmean distribution mean,
   * @param xsd distribution standard deviation.
   */
  NormalDistribution (double xmean, double xsd, double xinterval);

  /**
   * \brief Constructor
   * @param xmean distribution mean,
   * @param xsd distribution standard deviation,
   * @param nbins number of discrete points.
   */
  NormalDistribution (double xmean, double xsd, int nbins, double xinterval);

  /**
   * \brief copy constructor
   */
  NormalDistribution (const NormalDistribution &nd);

  ~NormalDistribution ();

  /* accessors */
  double getMean () const { return m_mean; }
  double getSd () const { return m_sd; }

 protected:
  /**
   * \brief Standard case assumes mean 0, sd 1.
   * Returns the integral from -inf to x.
   */
  static double cdfStandardCase (const double &x);

  /**
   * \brief Standard case assumes mean 0, sd 1.
   * Returns the X for which cdf(X) = integral.
   */
  static double cdfToXStandardCase (const double &integral);

  /**
   * \brief Returns the integral from -inf to x.
   */
  double cdf (const double &x);
  
  /**
   * \brief Returns the X for which cdf(X) = integral.
   */
  double cdfToX (const double &integral);

  /**
   * \brief probability density function.
   */
  double pdf (const double &x);

  /**
   * \brief 
   * @param pdf the pdf of the distribution to X.
   * @param higherVal return value on the right of the mean if true, 
   *                  on the left otherwise.
   * @sa NormalDistribution::pdf
   */
  double pdfToX (const double &pdf, const bool &higherVal);

 protected:
  /* static members */
  static double ptableMin;
  static double ptableMinEntry;
  static double ptableMax;
  static double ptableMaxEntry;
  static int ptableRows;
  static int ptableCols;
  static double ptable[40][10];

  double m_mean;  /**< distribution mean */
  double m_sd;  /**< distribution standard deviation */

 private:

};

} /* end of namespace */
#endif
