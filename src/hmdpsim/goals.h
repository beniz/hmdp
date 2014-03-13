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
 * \brief goals with conditional rewards.
 */

#ifndef GOALS_H
#define GOALS_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace ppddl_parser
{

class StateFormula;
class Assignment;
class AtomSet;
class ValueMap;
class FunctionTable;
class TermTable;
class PredicateTable;

enum GoalRewardType {
  GR, GR_COND, GR_CONJ
};

/* piecewise constant / linear rewards */
enum PwRewardType {
  GR_PW_CST, GR_PW_LINEAR
};


/* ====================================================================== */
/* GoalLinearReward */
class GoalLinearReward
{
 public:
  GoalLinearReward (): gType_ (GR), pwRType_ (GR_PW_LINEAR) {}

  GoalLinearReward (std::vector<Assignment*> &glreward);

  GoalLinearReward (Assignment *greward);

  virtual ~GoalLinearReward ();

  virtual bool holds (const AtomSet &atoms,
		      const ValueMap &values) const { return true; }; /* no condition */
  virtual void print (std::ostream &os, const PredicateTable &predicates,
		      const FunctionTable &functions,
		      const TermTable &terms) const;
  
  virtual bool isLinear () const { return true; }

  /* accessors */
  GoalRewardType getType () const { return gType_; }
  PwRewardType getPwType () const { return pwRType_; }

  /* setters */
  void addLinearRewardComp (Assignment *greward) { glreward_.push_back (greward); }

 public:
  const std::vector<Assignment*>& getReward () const { return glreward_; }
  const Assignment* getReward (const int &i) const { return glreward_[i]; }
  
 protected:
  GoalRewardType gType_; /**< goal reward type for external manipulations */
  PwRewardType pwRType_;  /**< piecewise reward type */
  std::vector<Assignment*> glreward_;
};

/* ====================================================================== */
/* GoalReward */

class GoalReward : public GoalLinearReward
{
 public:
  GoalReward (): GoalLinearReward () 
    {
      pwRType_  = GR_PW_CST;
    }
  
  GoalReward (Assignment *greward);

  ~GoalReward ();
  
  void print (std::ostream &os, const PredicateTable &predicates,
	      const FunctionTable &functions,
	      const TermTable &terms) const;
  
  bool isLinear () const { return false; }

  /* accessors */
  /*const Assignment* getReward () const { return glreward_[0]; }*/

};

/* ====================================================================== */
/* GoalRewardList */

class GoalRewardList : public std::vector<const GoalLinearReward*>
{};

/* ====================================================================== */
/* ConditionalGoalReward */

class ConditionalGoalReward : public GoalReward
{
 public:
  ConditionalGoalReward (const StateFormula *condition,
			 const GoalLinearReward *gr);

  virtual ~ConditionalGoalReward ();

  void print (std::ostream &os, const PredicateTable &predicates,
	      const FunctionTable &functions,
	      const TermTable &terms) const;

  bool holds (const AtomSet &atoms, const ValueMap &values) const;
  
  static const GoalLinearReward& make (const StateFormula &condition,
				       const GoalLinearReward &gr);

  const StateFormula& condition () const { return *condition_; }

  const GoalLinearReward& reward () const { return *reward_; }

 private:
  const StateFormula *condition_; /**< reward condition */
  const GoalLinearReward *reward_; /**< reward */
};

/* ====================================================================== */
/* ConjunctiveGoalReward */

class ConjunctiveGoalReward : public GoalReward
{
 public:
  ConjunctiveGoalReward () { gType_ = GR_CONJ; }

  virtual ~ConjunctiveGoalReward ();

  void print (std::ostream &os, const PredicateTable &predicates,
	      const FunctionTable &functions,
	      const TermTable &terms) const;

  bool holds (const AtomSet &atoms, const ValueMap &values) const;
  
  /* Adds a conjunct to this conjunctive reward. */
  void add_conjunct (const GoalLinearReward &conjunct);

  /* Returns the number of conjuncts of this conjunctive reward. */
  size_t size() const { return conjuncts_.size(); }

  /* Returns the ith conjunct of this conjunctive reward. */
  const GoalLinearReward& conjunct(size_t i) const { return *conjuncts_[i]; }

 private:
  GoalRewardList conjuncts_;  /**< the conjuncts */
};

/* ====================================================================== */
/* Goal */

class Goal
{
 public:
  Goal (const std::string name, const StateFormula &g, const GoalLinearReward &gr);
  
  ~Goal ();

  void print (std::ostream &os, const PredicateTable &predicates,
	      const FunctionTable &functions,
	      const TermTable &terms) const;

  /* accessors */
  const int getId () const { return id_; }
  const std::string getName () const { return name_; }
  const StateFormula* getGoalFormula () const { return goal_; }
  const GoalLinearReward* getGoalReward () const { return gr_; }

 private:
  static int goalsCount; /** total number of goals */
  const int id_; /**< unique id for a goal */
  const std::string name_;  /**< goal name */
  const StateFormula *goal_;  /**< goal as a set of fluents and statements */
  const GoalLinearReward *gr_;  /** reward */
};

/* ====================================================================== */
/* GoalMap */

class GoalMap : public std::map<std::string, const Goal*> 
{};

} /* end of namespace */

#endif
