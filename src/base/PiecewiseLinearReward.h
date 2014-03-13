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

#ifndef PIECEWISELINEARREWARD_H
#define PIECEWISELINEARREWARD_H

#include "ContinuousReward.h"

namespace hmdp_base
{

/**
 * \class PiecewiseLinearReward
 * \brief piecewise linear representation of a continuous reward.
 */
class PiecewiseLinearReward : public ContinuousReward 
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  PiecewiseLinearReward (const int &dim);
  
  /**
   * \brief tree node (including root) constructor.
   * @param dim the continuous space dimension.
   * @param d the partitioning dimension.
   * @param pos the position of the partition line.
   */
  PiecewiseLinearReward (const int &sdim, const int &d, const double &pos);
  
  /**
   * \brief constructor
   * @param dimension the number of reward tiles in the continuous space,
   * @param sdim the continuous space dimension.
   */
  PiecewiseLinearReward (const int &dimension, const int &sdim);
  
  /**
   * \brief constructor
   * @param dimension the number of reward tiles in the continuous space,
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the partitioning position.
   */
  PiecewiseLinearReward (const int &dimension, const int &sdim, const int &d, const double &pos);
  
  /**
   * \brief constructor
   * @param dimension the number of reward tiles in the continuous space.
   * @param sdim the continuous space dimension.
   * @param lowPos array of the low coordinates of the corner points of the tiles.
   * @param highPos array of the high coordinates of the corner points of the tiles.
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   * @param linNum array of number of linear functions per tile,
   * @param value array of array of linear function values, per tile, per linear function number.
   */
  PiecewiseLinearReward (const int &dimension, const int &sdim, double **lowPos, double **highPos,
			 double *low, double *high, int *linNum, 
			 double ***value);

  /**
   * \brief copy constructor
   */
  PiecewiseLinearReward (const PiecewiseLinearReward &pcr);
  
 private:
  /**
   * \brief copy constructor (from an object of the father's class type)
   * @param number of transition tiles in the continuous space.
   * @param bt a bsp tree.
   */
  PiecewiseLinearReward (const int &dimension, const BspTree &bt);

 public:
  ~PiecewiseLinearReward ();

   /* virtual */
  void leafDataIntersectMax (const BspTree &bt, const BspTree &btr,
			     double *low, double *high)
  {
    std::cout << "[Warning]: PiecewiseLinearReward::leafDataIntersectMax: you should not need it!\n";
  }
  
  void leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);
  
  void leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
			       double *low, double *high);

  /* TODO */
  void mergeContiguousLeaves (ContinuousReward *root, ContinuousReward *lt, ContinuousReward *ge)
  { std::cout << "[Warning]: PiecewiseLinearReward::mergeContiguousLeaves: not implemented yet.\n"; }

 protected:
  
 private:


};

} /* end of namespace */
#endif
