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
 * \brief representation of a piecewise linear value function.
 */

#ifndef PIECEWISELINEARVALUEFUNCTION_H
#define PIECEWISELINEARVALUEFUNCTION_H

#include "ValueFunction.h"
#include "ContinuousReward.h"
#include "ContinuousOutcome.h"
#include "ContinuousStateDistribution.h"
#include "PiecewiseConstantValueFunction.h"
#include <iostream>

namespace hmdp_base
{

  class ContinuousTransition;

/**
 * \class PiecewiseLinearValueFunction
 * \brief representation of a piecewise linear value function.
 */
class PiecewiseLinearValueFunction : public ValueFunction
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  PiecewiseLinearValueFunction (const int &sdim);
  
  /**
   * \brief tree node (including root) constructor.
   * @param dim the continuous space dimension.
   * @param d the partitioning dimension.
   * @param pos the position of the partition line.
   */
  PiecewiseLinearValueFunction (const int &sdim, const int &d, const double &pos);

  /**
   * \brief user constructor: the value function is set to 0.
   * @param sdim continuous space dimension,
   * @param low continuous space lower bounds in each dimension,
   * @param high continuous space upper bounds in each dimension,
   */
  PiecewiseLinearValueFunction (const int &sdim, const double *low, const double *high);

  /**
   * \brief copy constructor
   */
  PiecewiseLinearValueFunction (const PiecewiseLinearValueFunction &plvf);

  /**
   * \brief constructor from a piecewise constant value function.
   * @sa PiecewiseConstantValueFunction
   */
  PiecewiseLinearValueFunction (const PiecewiseConstantValueFunction &pcvf);

  /**
   * \brief constructor from a continuous reward.
   * @sa ContinuousReward
   */
  PiecewiseLinearValueFunction (const ContinuousReward &cr);

  /**
   * \brief constructor from a continuous outcome.
   * @sa ContinuousOutcome
   */
  PiecewiseLinearValueFunction (const ContinuousOutcome &co);

  /**
   * \brief constructor from a continuous transition.
   * @sa ContinuousTransition
   */
  PiecewiseLinearValueFunction (const ContinuousTransition &ct);

  /**
   * \brief constructor from a continuous state distribution.
   * @sa ContinuousStateDistribution
   */
  PiecewiseLinearValueFunction (const ContinuousStateDistribution &csd);

  /* Warning: this is no 'regular' constructor: this is used for merging value
   functions by actions. DO NOT USE to convert value functions. Use dedicated
   constructors above instead. */
  PiecewiseLinearValueFunction (const ValueFunction &vf, const bool &actions);
  
  /* constructor for breaking a vf based on an action... */
  PiecewiseLinearValueFunction (PiecewiseLinearValueFunction &vfactions,
				const int &action, const bool &prop);

  public:
  ~PiecewiseLinearValueFunction ();

  public:
  /* virtual */
  void leafDataIntersectMax (const BspTree &bt, const BspTree &btr,
			     double *low, double *high);
  
  void leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);
  
  void leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
			       double *low, double *high);

  void transferData (const BspTree &bt);

  void leafDataShift (double *shift);
  
  void mergeContiguousLeaves (ValueFunction *root, ValueFunction *lt, ValueFunction *ge,
			      double *low, double *high);

  void addScalar (const double &scalar)
  { std::cout << "Adding scalar to a linear function, are you sure you need it ?\n"; }

  /**
   * \brief compute the expected value of tree leaves.
   * @param val result value,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space.
   * @sa ValueFunction::computeExpectation
   */
  void expectedValueFromLeaves (double *val, double *low, double *high);

  int bestTileAction () { std::cout << "[Warning]: PiecewiseLinearValueFunction::bestTileAction: no implemented yet.\n";
    return 0;
  }

  /* printing */
  void plot1DVF (std::ofstream &box, double *low, double *high) { /* TODO */ }
  
  double getPointValueInLeaf (double *pos);

  int getPointActionInLeaf (double *pos) { /* TODO */ return 0;};
};

} /* end of namespace */
#endif
