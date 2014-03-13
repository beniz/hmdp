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

#ifndef PIECEWISECONSTANTREWARD_H
#define PIECEWISECONSTANTREWARD_H

#include "ContinuousReward.h"
#include "ValueFunction.h"

namespace hmdp_base
{
/**
 * \class PiecewiseConstantReward
 * \brief piecewise constant representation of a continuous reward.
 */
class PiecewiseConstantReward : public ContinuousReward
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  PiecewiseConstantReward (const int &dim);
  
  /**
   * \brief tree node (including root) constructor.
   * @param dim the continuous space dimension.
   * @param d the partitioning dimension.
   * @param pos the position of the partition line.
   */
  PiecewiseConstantReward (const int &sdim, const int &d, const double &pos);
  
  /**
   * \brief constructor
   * @param dimension the number of reward tiles in the continuous space,
   * @param sdim the continuous space dimension.
   */
  PiecewiseConstantReward (const int &dimension, const int &sdim);
  
  /**
   * \brief constructor
   * @param dimension the number of reward tiles in the continuous space,
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the partitioning position.
   */
  PiecewiseConstantReward (const int &dimension, const int &sdim, 
			   const int &d, const double &pos);
  
  /**
   * \brief constructor
   * @param dimension the number of reward tiles in the continuous space.
   * @param sdim the continuous space dimension.
   * @param lowPos array of the low coordinates of the corner points of the tiles.
   * @param highPos array of the high coordinates of the corner points of the tiles.
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   * @param value array of constant values, one per tile.
   */
  PiecewiseConstantReward (const int &dimension, const int &sdim, 
			   double **lowPos, double **highPos,
			   double *low, double *high, const double *value);

  /**
   * \brief user constructor: the value function is set to val.
   * @param sdim continuous space dimension,
   * @param low continuous space lower bounds in each dimension,
   * @param high continuous space upper bounds in each dimension.
   * @param val initial value of the function.
   */
  PiecewiseConstantReward (const int &sdim, const double *low, const double *high, 
			   const double &val);


  /**
   * \brief copy constructor
   */
  PiecewiseConstantReward (const PiecewiseConstantReward &pcr);

  PiecewiseConstantReward (const ValueFunction &vf);
  
 private:
  /**
   * \brief copy constructor (from an object of the father's class type)
   * @param number of transition tiles in the continuous space.
   * @param bt a bsp tree.
   */
  PiecewiseConstantReward (const int &dimension, const BspTree &bt);

 public:
  ~PiecewiseConstantReward ();

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
  void mergeContiguousLeaves (ContinuousReward *root, ContinuousReward *lt, ContinuousReward *ge);

  /* stuff for asymetric operators. */
  double updateSubTreeMaxValue ();

  /* accessors */
  double getConstantValue () const 
  { if (m_alphaVectors) return (*m_alphaVectors)[0]->getAlphaNth (0);
    else {
      std::cout << "[Warning]: PiecewiseConstantReward::getConstantValue: no alpha vector found.\n";
      return 0.0;
    }}  /* beware error value */

  /* plotting (virtual) */
  void plot2DVrml2Leaf (const double &space, std::ofstream &output_vrml,
			double *low, double *high, const double &scale0, 
			const double &scale1, const double &max_value);

 protected:
  
 private:

};

} /* end of namespace */

#endif
