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
 * \brief representation of a piecewise constant value function.
 */

#ifndef PIECEWISECONSTANTVALUEFUNCTION_H
#define PIECEWISECONSTANTVALUEFUNCTION_H

#include "ValueFunction.h"
#include "ContinuousReward.h"
#include "ContinuousOutcome.h"
#include "ContinuousStateDistribution.h"

namespace hmdp_base
{

  class ContinuousTransition;

/**
 * \class PiecewiseConstantValueFunction
 * \brief representation of a piecewise constant value function.
 */
class PiecewiseConstantValueFunction : public ValueFunction
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  PiecewiseConstantValueFunction (const int &sdim);
  
  /**
   * \brief tree node (including root) constructor.
   * @param dim the continuous space dimension.
   * @param d the partitioning dimension.
   * @param pos the position of the partition line.
   */
  PiecewiseConstantValueFunction (const int &sdim, const int &d, const double &pos);

  /**
   * \brief user constructor: the value function is set to val.
   * @param sdim continuous space dimension,
   * @param low continuous space lower bounds in each dimension,
   * @param high continuous space upper bounds in each dimension.
   * @param val initial value of the function.
   */
  PiecewiseConstantValueFunction (const int &sdim, const double *low, const double *high, 
				  const double &val);

  /**
   * \brief user constructor: the value function is set to 0.
   * @param sdim continuous space dimension,
   * @param low continuous space lower bounds in each dimension,
   * @param high continuous space upper bounds in each dimension.
   */
  PiecewiseConstantValueFunction (const int &sdim, const double *low, const double *high);

  /**
   * \brief copy constructor
   */
  PiecewiseConstantValueFunction (const PiecewiseConstantValueFunction &pcvf);

  /**
   * \brief constructor from a continuous reward.
   * @sa ContinuousReward
   */
  PiecewiseConstantValueFunction (const ContinuousReward &cr);

  /**
   * \brief constructor from a continuous outcome.
   * @sa ContinuousOutcome
   */
  PiecewiseConstantValueFunction (const ContinuousOutcome &co);

  /**
   * \brief constructor from a continuous transition.
   * @sa ContinuousTransition
   */
  PiecewiseConstantValueFunction (const ContinuousTransition &ct,
				  const double &tileval);

  /**
   * \brief constructor from a continuous state distribution.
   * @sa ContinuousStateDistribution
   */
  PiecewiseConstantValueFunction (const ContinuousStateDistribution &csd);

 public:
  /* constructor for breaking a vf based on an action... */
  PiecewiseConstantValueFunction (PiecewiseConstantValueFunction &pwlactions,
				  const int &action, const bool &prop);

 public:
  ~PiecewiseConstantValueFunction ();

  public:
  /* virtual */
  void leafDataIntersectMax (const BspTree &bt, const BspTree &btr,
			     double *low, double *high);

  void leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);

  void leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
			       double *low, double *high);
  
  /**
   * \brief merge contiguous leaves with similar alpha vector values.
   * @param root reference to the bsp tree node from which the two cells originate.
   * @param lt the lower tree, starting from root,
   * @param ge the greater tree, starting from root.
   * @sa BspTreeOperations::m_piecesMerging, BspTreeOperations::m_piecesMergingByValue,
   *     BspTreeOperations::m_piecesMergingByAction, BspTreeOperations::m_piecesMergingEquality.
   * @warning this function can delete lt and ge.
   */
  void mergeContiguousLeaves (ValueFunction *root, ValueFunction *lt, ValueFunction *ge,
			      double *low, double *high);
  
 protected:
  /**
   * \brief compute the expected value of tree leaves.
   * @param val result value,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space.
   * @sa ValueFunction::computeExpectation
   */
  void expectedValueFromLeaves (double *val, double *low, double *high);
  
  /**
   * \brief get the best action in a tile (leaf).
   * @return the best action index.
   */
  std::set<int> bestTileActions ();

  /* accessors */
 public:
  double getConstantValue () const 
  { if (m_alphaVectors) return (*m_alphaVectors)[0]->getAlphaNth (0);
    else {
      //std::cout << "[Warning]: PiecewiseConstantValueFunction::getConstantValue: no alpha vector found.\n";
      return 0.0;
    }}  /* beware error value */

  /* stuff for asymetric operators. */
  double updateSubTreeMaxValue ();

  double updateSubTreeMaxValue (ValueFunction *vflt, ValueFunction *vfge);

 protected:
  /* printing */
  void plot1DVF (std::ofstream &box, double *low, double *high);
  void plot2DVrml2Leaf (const double &space, std::ofstream &output_vrml,
			double *low, double *high, const double &scale0, const double &scale1,
			const double &max_value);
  
  double getPointValueInLeaf (double *pos);

  int getPointActionInLeaf (double *pos);
};

} /* end of namespace */
#endif
