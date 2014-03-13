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

#ifndef MDDISCRETEDISTRIBUTION_H
#define MDDISCRETEDISTRIBUTION_H

#include "DiscreteDistribution.h"
#include <utility> /* pair */

#if !defined __GNUC__ || __GNUC__ < 3
#include <ostream.h>
#else
#include <ostream>
#endif
#include <vector>

namespace hmdp_base
{

  class ContinuousStateDistribution;

/**
 * \class MDDiscreteDistribution
 * \brief discrete representation of a multi-dimensional continuous distribution.
 *
 */
class MDDiscreteDistribution
{
 public:
  /**
   * \brief constructor.
   * @param dim continuous space dimension.
   * @param npoints discrete points
   */
  MDDiscreteDistribution (const int &dim, const int &npoints);

  /**
   * \brief constructor
   * @param dim continuous space dimension,
   * @param lowPos lower bound on the continuous space, for discretization purposes,
   * @param highPos lower bound on the continuous space, for discretization purposes,
   * @param intervals array of discretization intervals, per continuous state dimension.
   */
  MDDiscreteDistribution (const int &dim, double *lowPos, double *highPos,
			  double *intervals);

  /**
   * \brief copy constructor
   */
  MDDiscreteDistribution (const MDDiscreteDistribution &mdd);
  
  /**
   * \brief constructor that converts a ContinuousStateDistribution (bsp tree).
   * @param csd continuous state distribution,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space.
   */
  MDDiscreteDistribution (ContinuousStateDistribution *csd, double *low, double *high);

  ~MDDiscreteDistribution ();

 private:
  /**
   * \brief create a discrete bin.
   * @param pt point 'number' to be created.
   * @param proba probability of the bin.
   * @param pos array of the bin position in the continuous space dimensions.
   */
  void createMDDiscreteBin (const int &pt, const double &proba, double *pos);

  /**
   * \brief converts a ContinuousStateDistribution (bsp tree).
   * @param csd continuous state distribution,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space.
   */
  void convertContinuousStateDistribution (ContinuousStateDistribution *csd, double *low, double *high);

 public:
  /**
   * \brief a multi-dimensional bin.
   * @param proba bin probability,
   * @param pos multi-dimensional center position of the bin.
   * @return an stl pair (bin probability, center bin position).
   */
  static std::pair<double, double*> createMDDiscreteBinS (const double &proba, double *pos);

 private:
  /**
   * \brief internal discretization procedure.
   */
  void discretize (double *lowPos, double *highPos);

 private:
  void reallocPoints (const int &nnpoints);

  /* static member functions */

  /**
   * \brief compute the joint number of discrete points of a set of 
   *        independent discrete distributions.
   * @param nDists number of independent discrete distributions.
   * @param dists array of independent discrete distributions pointers.
   * @return joint number of discrete points (product).
   * @sa DiscreteDistribution
   */
  static int jointIndependentDiscretePoints (const int &nDists, DiscreteDistribution **dists);
  static int m_counterpoints;  /**< internal counter, here because used in static methods. */
  
 public:
  /**
   * \brief compute the joint discrete distribution of a set of a set of
   *        independent discrete distributions.
   * @param nDists number of independent discrete distributions.
   * @param dists array of independent discrete distributions pointers.
   * @return joint discrete distribution.
   * @sa DiscreteDistribution
   */
  static MDDiscreteDistribution* jointDiscreteDistribution (const int &nDists,
							    DiscreteDistribution **dists);
 private:
  /**
   * \brief recursive computation of the joint discrete distribution of a set 
   *        of independent discrete distributions.
   * @param dists array of independent discrete distributions pointers.
   * @param d currently explored dimension of the continuous space.
   * @param pos array of position points in the continuous space.
   * @param prob current probability for the position above (i.e. conditional probability).
   * @param mdd result joint discrete distribution.
   * @return joint discrete distribution
   * @sa DiscreteDistribution
   */
  static MDDiscreteDistribution* jointDiscreteDistribution (DiscreteDistribution **dists,
							    const int &d, double *pos, const double &prob, 
							    MDDiscreteDistribution *mdd);
 
 public:
  /**
   * \brief convolute two multi-dimensional discrete probability distributions.
   * @param mddist1 multi-dimensional discrete probability distribution,
   * @param mddist2 multi-dimensional discrete probability distribution,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space,
   * @param lowPos1 lower bound on the continuous space for distribution 1,
   * @param highPos1 upper bound on the continuous space for distribution 1,
   * @param lowPos2 lower bound on the continuous space for distribution 2,
   * @param highPos2 upper bound on the continuous space for distribution 2.
   */
  static MDDiscreteDistribution* convoluteMDDiscreteDistributions (const MDDiscreteDistribution &mddist1,
								   const MDDiscreteDistribution &mddist2,
								   double *low, double *high,
								   double *lowPos1, double *highPos1,
								   double *lowPos2, double *highPos2);

 public:
  static MDDiscreteDistribution* resizeDiscretization (const MDDiscreteDistribution &mddist1,
						       double *lowPos, double *highPos,
						       double *intervals);
  
 public:
  /* accessors */
  
  /**
   * \brief continuous space dimension accessor.
   * @return continuous space dimension.
   */
  int getDimension () const { return m_dimension; }

  /**
   * \brief number of discrete points accessor.
   * @return number of discrete points for this distribution.
   */
  int getNPoints () const { return m_nPoints; }

  /**
   * \brief multivariate distribution accessor.
   * @return pair of probability/position in the multidimensional continuous
   *         space, as the discrete multivariate distribution.
   */
  std::vector<std::pair<double, double*> > getMDDistribution () const { return m_dist; }

  std::pair<double, double*> getPoint (const int &i) const { return m_dist[i]; }

  std::pair<double, double*>* getPointAndBinMass (const int &i) const;

  /**
   * \brief discrete bin probability accessor.
   * @param pt discrete bin number.
   * @return probability of this bin.
   */
  double getProbability (const int &pt) const { return m_dist[pt].first; }

  double getProbability (double *pos) const;

  /**
   * \brief discrete bin position accessor.
   * @param pt discrete bin number.
   * @param dim continuous space dimension.
   * @return position of this bin in the argument dimension.
   */
  double getPosition (const int &pt, const int &dim) const { return m_dist[pt].second[dim]; }

  double* getPosition (const int &pt) const { return m_dist[pt].second; }

  double getMinPosition (const int &dim) const { return m_dist[0].second[dim]; }

  double getMaxPosition (const int &dim) const { return m_dist[m_nPoints-1].second[dim]; }

  double* getDiscretizationIntervals () const { return m_intervals; }

  /**
   * \brief accessor to the discretization interval, per dimension.
   * @param d dimension index,
   * @return discretization interval in that dimension.
   */
  double getDiscretizationInterval (const int &d) const { return m_intervals[d]; }

  /**
   * \brief accessor to the number of discretization points, per dimension.
   * @param i dimension index,
   * @return number of discretization points in that dimension.
   */
  int getDimPoints (const int &i) const { return m_dimPoints[i]; }

  /* setters */
  void setDiscretizationInterval (const int &d, const double &interval) { m_intervals[d] = interval; }

  void setProbability (const int &pt, const double &pr) { m_dist[pt].first = pr; }

  void setDimPoint (const int &d, const int &bins) { m_dimPoints[d] = bins; }
  
  double getProbMass () const;

  double getProbMass (const int &i) const;

  void normalize (const double &sum);

  std::ostream& print (std::ostream &output);

 protected:

 private:
  int m_dimension;  /**< dimension of the continuous space */
  int m_nPoints;  /**< number of discrete points */
  /*std::pair<double, double*> *m_dist;*/  /**< discrete distribution of size npoints: 
    proba, position in the space */
  std::vector<std::pair<double,double*> > m_dist;
  double *m_intervals;  /**< discretization intervals (on per dimension). */
  int *m_dimPoints;  /**< number of discrete points in each dimension */
};

 std::ostream &operator<<(std::ostream &output, MDDiscreteDistribution &mdd);

 std::ostream &operator<<(std::ostream &output, MDDiscreteDistribution *mdd);

} /* end of namespace */
#endif
