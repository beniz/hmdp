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

#ifndef CONTINUOUSREWARD_H
#define CONTINUOUSREWARD_H

#include "BspTreeAlpha.h"
#include <set>

namespace hmdp_base
{
/**
 * \class ContinuousReward
 * \brief discretized representation of a continous reward.
 */
class ContinuousReward : public BspTreeAlpha
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  ContinuousReward (const int &sdim);

  /**
   * \brief tree node (including root) constructor.
   * @param dim the continuous space dimension.
   * @param d the partitioning dimension.
   * @param pos the position of the partition line.
   */
  ContinuousReward (const int &sdim, const int &d, const double &pos);

  /**
   * \brief constructor
   * @param dimension the number of continuous reward tiles in the continuous space,
   * @param sdim the continuous space dimension.
   */
  ContinuousReward (const int &dimension, const int &sdim);

  /**
   * \brief constructor
   * @param dimension the number of continuous reward tiles in the continuous space,
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the partitioning position.
   */
  ContinuousReward (const int &dimension, const int &sdim, const int &d, const double &pos);

  /**
   * \brief copy constructor
   */
  ContinuousReward (const ContinuousReward &cr);

  /* virtual */
  /**
   * \brief transfer argument tree's data point to the current tree's data. [VIRTUAL].
   * @param bt tree whose data the current tree should point to.
   */
  void transferData (const BspTree &bt);

  /**
   * \brief merge contiguous leaves with similar alpha vector values.
   * @param root reference to the bsp tree node from which the two cells originate.
   * @param lt the lower tree, starting from root,
   * @param ge the greater tree, starting from root.
   * @sa BspTreeOperations::m_piecesMerging, BspTreeOperations::m_piecesMergingByValue,
   *     BspTreeOperations::m_piecesMergingByAction, BspTreeOperations::m_piecesMergingEquality.
   * @warning this function can delete both lt and ge.
   */
  virtual void mergeContiguousLeaves (ContinuousReward *root, ContinuousReward *lt, ContinuousReward *ge) {};

  /**
   * \brief navigates through this tree and merges tree leaves with similar alpha vector values,
   *        goals or actions.
   * @sa mergeContiguousLeaves, BspTreeOperations::m_piecesMerging, BspTreeOperations::m_piecesMergingByValue,
   *     BspTreeOperations::m_piecesMergingByAction, BspTreeOperations::m_piecesMergingEquality.
   */
  void mergeTreeLeaves ();

 protected:
  /**
   * \brief copy constructor (from an object of the father's class type)
   * @param number of transition tiles in the continuous space.
   * @param bt a bsp tree.
   */
  ContinuousReward (const int &dimension, const BspTree &bt);
  
 public:
  /**
   * \brief destructor
   */
  ~ContinuousReward ();

 protected:
  
 public:
  /* accessors */
  /**
   * \brief tiling dimension accessor
   * @return the number of continuous reward tiles in the continuous space.
   */
  int getTilingDimension () const { return m_tilingDimension; }

  /**
   * \brief accessor to the set of achieved goals.
   * @return the current set of achieved goals.
   */
  std::vector<int>* getAchievedGoals () const { return m_achievedGoals; }

  /**
   * \brief gets the max value point of the reward. 
   */
  void getMaxValue (double &mval, double *low, double *high);

  /* setters */
  /**
   * \brief add a goal to the set of achieved goals.
   * @param g the goal index.
   */
  void addGoal (const int &g) 
    {
      if (! m_achievedGoals) m_achievedGoals = new std::vector<int> ();
      m_achievedGoals->push_back (g);
    }
  
   /**
   * \brief add a value function's achieved goals to this function.
   * @param cr value function whose goals are to be added.
   */
  void addGoals (const ContinuousReward &cr);
  
  /**
   * \brief sets this function's goal sets to the union of two function' goal sets.
   */
  void unionGoalSets (const ContinuousReward &cr1, const ContinuousReward &cr2);

  /**
   * \brief tag all pieces in this bsp tree with this goal.
   * @param g the goal index.
   */
  void tagWithGoal (const int &g);

  void untagGoals();

  /**
   * \brief recursively collect goals from tree leaves.
   * @param achievedGoalsSet set to be filled up with tree leave goals.
   */
  void collectAchievedGoals (std::set<int> *achievedGoalsSet);

  void deleteAchievedGoals ();
  
  /* stuff for asymetric operators. */
  double updateSubTreeMaxValue () { return 0.0; };
  
 protected:
  int m_tilingDimension; /**< number of continuous reward tiles (root node only, 0 otherwise). */
  std::vector<int> *m_achievedGoals;  /**< goals achieved in this reward (from goal definition). */

 private:
};

} /* end of namespace */
#endif
