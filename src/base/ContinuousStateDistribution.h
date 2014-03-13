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
 * \brief representation of a multidimensional probability distribution over
 *        the continuous state, as a binary space partitioning tree (bsp tree).
 */

#ifndef CONTINUOUSSTATEDISTRIBUTION_H
#define CONTINUOUSSTATEDISTRIBUTION_H

#include "BspTree.h"
#include "MDDiscreteDistribution.h"
#include "ContinuousOutcome.h"

namespace hmdp_base
{

  class ContinuousTransition;
  class HybridTransition;
  class ValueFunction;

/**
 * \class ContinuousStateDistribution
 * \brief probability distribution over the continuous state as a bsp tree.
 */
class ContinuousStateDistribution : public BspTree
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  ContinuousStateDistribution (const int &sdim);
  
  /**
   * \brief constructor
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the partitioning position.
   */
  ContinuousStateDistribution (const int &sdim, const int &d, const double &pos);

  /**
   * \brief constructor
   * @param dimension the number of distribution tiles in the continuous space,
   * @param sdim the continuous space dimension.
   */
  ContinuousStateDistribution (const int &dimension, const int &sdim);

  /**
   * \brief constructor
   * @param dimension the number of transition tiles in the continuous space,
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the partitioning position.
   */
  ContinuousStateDistribution (const int &dimension, const int &sdim,
			       const int &d, const double &pos);

  /**
   * \brief constructor
   * @param dimension the number of distribution tiles in the continuous space.
   * @param sdim the continuous space dimension.
   * @param lowPos array of the low coordinates of the corner points of the tiles.
   * @param highPos array of the high coordinates of the corner points of the tiles.
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   * @param prob array of probabilities, per tile.
   */
  ContinuousStateDistribution (const int &dimension, const int &sdim, 
			       double **lowPos, double **highPos, double *low, double *high,
			       double *prob);

  /**
   * \brief copy constructor.
   */
  ContinuousStateDistribution (const ContinuousStateDistribution &csd);

  /* inversion: turn probabilities to 0, and null probabilities to 1. */
  ContinuousStateDistribution (const ContinuousStateDistribution &csd, 
			       double *low, double *high, const bool &invert);

  ContinuousStateDistribution (const ContinuousOutcome &co);

  ContinuousStateDistribution (const ContinuousTransition &ct);

  /* Warning: the vf must have proper (probabilities) values. */
  ContinuousStateDistribution (const ValueFunction &vf);

 public:
  /**
   * \brief copy constructor (from an object of the father's class type)
   * @param dimension number of distribution tiles in the continuous space.
   * @param bt a bsp tree.
   */
  ContinuousStateDistribution (const int &dimension, const BspTree &bt);

 public:
  virtual ~ContinuousStateDistribution ();

 protected:
  /* virtual functions */
  void leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);

  void transferData (const BspTree &bt);

 public:
  /**
   * \brief normalize the distribution by performing two passes through the tree.
   */
  void normalize (double *low, double *high);

  /**
   * \brief sum up all probabilities in the tree.
   * @param norm probability mass (result).
   */
  void sumUpProbabilities (double *norm, double *low, double *high);

  /**
   * \brief normalize the distribution.
   * @param probability mass to normalize with.
   */
  void normalize (const double &norm);

  /* set all leaves to prob. */
  void setCsdProbability (const double &prob);

  /**
   * \brief multiply the tree leaf probabilities by a scalar.
   * @param scalar scalar to multiply with.
   */
  void multiplyByScalar (const double &scalar);

  /**
   * \brief count the number of leaves with non zero probability.
   * @return number of non zero probability leaves.
   */
  int countNonZeroLeaves () const;

  /**
   * \brief count the number of leaves with positive probability.
   * @return number of leaves with positive values.
   */
  int countPositiveLeaves () const;

  /**
   * \brief navigates through this tree and merges tree leaves with similar probabilities.
   * @sa mergeContiguousLeaves
   */
  void mergeTreeLeaves (double *low, double *high);

  /* static */
  /**
   * \brief convert a multi-dimensional discrete distribution to a bsp tree.
   * @param mdd multi-dimensional discrete distribution to be converted,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space,
   * @return a ContinuousStateDistribution object.
   */
  static ContinuousStateDistribution* convertMDDiscreteDistribution (const MDDiscreteDistribution &mdd,
								     double *low, double *high);

 public:
  /**
   * \brief add two continuous distributions (bsp trees intersection).
   * @param csd1 continuous distribution,
   * @param csd2 continuous distribution,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space,
   * @return a pointer to a new distribution that is the sum of csd1 and csd2.
   */
  static ContinuousStateDistribution* addContinuousStateDistributions (ContinuousStateDistribution *csd1,
								       ContinuousStateDistribution *csd2,
								       double *low, double *high);

  static ContinuousStateDistribution* subtractContinuousStateDistributions (ContinuousStateDistribution *csd1,
									    ContinuousStateDistribution *csd2,
									    double *low, double *high);

  static ContinuousStateDistribution* diffContinuousStateDistributions (ContinuousStateDistribution *csd1,
									ContinuousStateDistribution *csd2,
									double *low, double *high);
  
  /**
   * \brief multiply two continuous distributions (bsp trees intersection).
   * @param csd1 continuous distribution,
   * @param csd2 continuous distribution,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space,
   * @return a pointer to a new distribution that is the sum of csd1 and csd2.
   */
  static ContinuousStateDistribution* multContinuousStateDistributions (ContinuousStateDistribution *csd1,
									ContinuousStateDistribution *csd2,
									double *low, double *high);

  static bool compareContinuousStateDistributions (ContinuousStateDistribution *csd1,
						   ContinuousStateDistribution *csd2,
						   double *low, double *high);
  
 public:
  /* new forward projection... */
  static ContinuousStateDistribution* frontUp (ContinuousStateDistribution *csd,
					       ContinuousTransition *ct,
					       double *low, double *high,
					       const double &scalar);

 private:
  static ContinuousStateDistribution* frontUp (ContinuousStateDistribution *csd,
					       ContinuousTransition *ct,
					       double *lowPos, double *highPos,
					       double *low, double *high,
					       ContinuousStateDistribution *res);

  static ContinuousStateDistribution* frontUpOutcomes (const int &h, const int &t,
						       ContinuousTransition *ct,
						       ContinuousStateDistribution *csd,
						       double *lowPos, double *highPos,
						       double *low, double *high);
    
 public:
  static ContinuousStateDistribution* frontUpSingleOutcome (ContinuousStateDistribution *csd,
							    ContinuousOutcome *co,
							    double *lowPos, double *highPos,
							    double *low, double *high);
  
 public:
  /* virtual */
  void leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);

  void leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);

  void leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
			       double *low, double *high);

  void leafDataIntersectCsdDiff (const BspTree &bt, const BspTree &btr,
				 double *low, double *high);

 public:
  /* setters */
  void setProbability (const double &p) { m_probability = p; }
  
  /* accessors */
  int getTilingDimension () const { return m_tilingDimension; }
  double getProbability () const { return m_probability; }

  void getMinPosition (double *mpos, double *low, double *high);

  void getMaxPosition (double *mpos, double *low, double *high);

  /* printing */
  void print (std::ostream &out, double *low, double *high);
  
  /* plotting */
  void plot2DVrml2Leaf (const double &space, std::ofstream &output_vrml,
			double *low, double *high,
			const double &scale0, const double &scale1,
			const double &max_value);
  void plot1DCSD (std::ofstream &box, double *low, double *high);

  double getPointValueInLeaf (double *pos);

  static double m_doubleProbaPrecision;  /**< double precision on probabilities. */
  
 private:
  int m_tilingDimension; /** number of distribution tiles (root node only) */
  double m_probability;  /**< leaf probability */
};

} /* end of namespace */
#endif
