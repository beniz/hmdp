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

#ifndef VALUEFUNCTION_H
#define VALUEFUNCTION_H

#include "BspTreeAlpha.h"
#include "MDDiscreteDistribution.h"
#include "ContinuousStateDistribution.h"
#include <set>

namespace hmdp_base
{

/**
 * \class ValueFunction
 * \brief representation of a value function (mapping from resources to utility).
 */
class ValueFunction : public BspTreeAlpha
{
 public:
  /**
   * \brief constructor
   * @param sdim continuous space dimension.
   */
  ValueFunction (const int &sdim);

  /**
   * \brief constructor.
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the position of the partition line.
   */
  ValueFunction (const int &sdim, const int &d, const double &pos);

  /**
   * \brief copy constructor
   */
  ValueFunction (const ValueFunction &vf);

  /**
   * \brief non-recursive destructor, i.e. destroy the current node only.
   * @sa BspTree::deleteBspTree
   */
  ~ValueFunction ();

 public:
  /* virtual */
  /* goals are empty at initialization... don't need it here */
  //void leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
  //double *low, double *high);
  
  /**
   * \brief transfer argument tree's data point to the current tree's data. [VIRTUAL].
   * @param bt tree whose data the current tree should point to.
   */
  void transferData (const BspTree &bt);

  void leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);
  
  /**
   * \brief merge contiguous leaves with similar alpha vector values.
   * @param root reference to the bsp tree node from which the two cells originate.
   * @param lt the lower tree, starting from root,
   * @param ge the greater tree, starting from root.
   * @warning this function can delete both lt and ge.
   */
  virtual void mergeContiguousLeaves (ValueFunction *root, ValueFunction *lt, ValueFunction *ge,
				      double *low, double *high) {};
  
  /**
   * \brief compute expectation value from this value fonction and a
   *        multi-dimensional discrete probability distribution.
   * @param mdd multi-dimensional discrete probability distribution (internally 
   *            converted to a bsp tree),
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space,
   * @return expected value.
   */
  double computeExpectation (const MDDiscreteDistribution &mdd, double *low, double *high);

  /**
   * \brief compute expectation value from this value fonction and a
   *        multi-dimensional discrete probability distribution.
   * @param csd multi-dimensional discrete probability distribution,
   * @param low lower bound on the continuous space,
   * @param high upper bound on the continuous space,
   * @return expected value.
   */
  double computeExpectation (ContinuousStateDistribution *csd, double *low, double *high);

 protected:
  virtual void expectedValueFromLeaves (double *val, double *low, double *high) {};

 public:
  /**
   * \brief navigates through this tree and merges tree leaves with similar alpha vector values.
   * @sa mergeContiguousLeaves
   */
  void mergeTreeLeaves (double *low, double *high);

  /**
   * \brief collect all different actions attached to the tree leaves.
   * @param result set (contains each action index once).
   */
  void collectActions (std::set<int> *actionSet, double *low, double *high);

  void collectActions (std::set<int> *actionSet, double *low, double *high,
		       double *min, double *max);

  /**
   * \brief recursively collect goals from tree leaves.
   * @param achievedGoalsSet set to be filled up with tree leave goals.
   */
  void collectAchievedGoals (std::set<int> *achievedGoalsSet);

  /**
   * \brief get the best action in a tile (leaf).
   * @return the best action index.
   */
  virtual std::set<int> bestTileActions () { return std::set<int> (); };
  
  void breakTiesOnActions (std::map<int, double> *table);

  /* accessors */
  /**
   * \brief accessor to the set of achieved goals.
   * @return the current set of achieved goals.
   */
  std::vector<int>* getAchievedGoals () const { return m_achievedGoals; }

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
   * @param vf value function whose goals are to be added.
   */
  void addGoals (const ValueFunction &vf);
  
  /**
   * \brief sets this function's goal sets to the union of two function' goal sets.
   */
  void unionGoalSets (const ValueFunction &vf1, const ValueFunction &vf2);

  /**
   * \brief tag all pieces in this bsp tree with this action.
   * @param action the action index.
   */
  void tagWithAction (const int &action);

  void clearActionTags ();

  void clearActions ();

  void removeAction (const int &action);

  /**
   * \brief set action to this leaf (only) alpha vector.
   * @param action the action index.
   */
  void addAction (const int &action);

  void tagWithGoal (const int &goalid);

  /* stuff for asymetric operators */
  double updateSubTreeMaxValue () { return 0.0; };

  virtual double updateSubTreeMaxValue (ValueFunction *vflt, ValueFunction *vfge) 
  { return 0.0; }

  /* stats */
  void maxNumberOfAchievedGoals (unsigned int &mnag);
  
  /**
   * \brief returns the set of goals for the maximum value tile
   *        with value > val.
   */
  void maxValueAndGoals (std::vector<int> &mgls, double &mval, 
			 double *low, double *high);

  void maxAbsValue (double &mabsval, double *low, double *high);

  /* printing */
  virtual void plot1DVF (std::ofstream &box, double *low, double *high) {};

  void print (std::ostream &output_values, double *low, double *high) const;

 protected:
  std::vector<int> *m_achievedGoals;  /**< goals achieved by the VF */

 protected:
  bool m_csd; /**< internal flag, true if value function carries information from a csd. */
  
 public:
  bool getCSDFlag () const { return m_csd; }
};

} /* end of namespace */
#endif
