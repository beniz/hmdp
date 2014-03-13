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

#include "goals.h"
#include "formulas.h"
#include "effects.h"  /* Assignment */

namespace ppddl_parser
{

/* ====================================================================== */
/* GoalLinearReward */

GoalLinearReward::GoalLinearReward (std::vector<Assignment*> &glreward)
  : gType_ (GR), pwRType_ (GR_PW_LINEAR), glreward_ (glreward)
{}

GoalLinearReward::GoalLinearReward (Assignment *greward)
: gType_ (GR), pwRType_ (GR_PW_LINEAR) 
{
  glreward_.push_back (greward);
}

GoalLinearReward::~GoalLinearReward ()
{
  for (size_t i=0; i<glreward_.size (); i++)
    delete glreward_[i];
}

void GoalLinearReward::print (std::ostream &os, const PredicateTable &predicates,
			      const FunctionTable &functions,
			      const TermTable &terms) const
{
  std::cout << "number of functions: " << glreward_.size () << std::endl;
  for (size_t i=0; i<glreward_.size (); i++)
    glreward_[i]->print (os, functions, terms);
}

/* ====================================================================== */
/* GoalReward */

GoalReward::GoalReward (Assignment *greward)
  : GoalLinearReward ()
{
  glreward_.push_back (greward);
  pwRType_ = GR_PW_CST;  /* piecewise constant is the default here... */
}

GoalReward::~GoalReward ()
{}

void GoalReward::print (std::ostream &os, const PredicateTable &predicates,
			const FunctionTable &functions,
			const TermTable &terms) const
{
  glreward_[0]->print (os, functions, terms);
}

/* ====================================================================== */
/* ConditionalGoalReward */

ConditionalGoalReward::ConditionalGoalReward (const StateFormula *condition,
					      const GoalLinearReward *gr)
  : GoalReward (), condition_ (condition), reward_ (gr)
{
  gType_ = GR_COND;
  StateFormula::register_use (condition_);
}

ConditionalGoalReward::~ConditionalGoalReward ()
{
  delete condition_;
  delete reward_;
}

const GoalLinearReward& ConditionalGoalReward::make (const StateFormula &condition,
						     const GoalLinearReward &gr)
{
  if (condition.tautology ())
    return gr;
  else if (condition.contradiction ())
    return *new ConjunctiveGoalReward ();
  else return *new ConditionalGoalReward (&condition, &gr);
}

bool ConditionalGoalReward::holds (const AtomSet &atoms, const ValueMap &values) const
{ 
  return condition_->holds (atoms, values); 
}

void ConditionalGoalReward::print (std::ostream &os, const PredicateTable &predicates,
				   const FunctionTable &functions,
				   const TermTable &terms) const
{
  os << "(when ";
  condition_->print(os, predicates, functions, terms);
  os << ' ';
  reward_->print(os, predicates, functions, terms);
  os << ")";
}

/* ====================================================================== */
/* ConjunctiveGoalReward */

/* Deletes this conjunctive goal reward */
ConjunctiveGoalReward::~ConjunctiveGoalReward ()
{
  for (GoalRewardList::const_iterator gri = conjuncts_.begin ();
       gri != conjuncts_.end (); gri++)
    delete (*gri);
}

void ConjunctiveGoalReward::add_conjunct (const GoalLinearReward &conjunct)
{
  conjuncts_.push_back (&conjunct);
}

bool ConjunctiveGoalReward::holds (const AtomSet &atoms, const ValueMap &values) const
{
  for (GoalRewardList::const_iterator gri = conjuncts_.begin ();
       gri != conjuncts_.end (); gri++)
    if (! (*gri)->holds (atoms, values))
      return false;
  return true;
}

void ConjunctiveGoalReward::print (std::ostream &os, const PredicateTable &predicates,
				   const FunctionTable &functions,
				   const TermTable &terms) const
{
  if (size () == 1)
    conjunct (0).print (os, predicates, functions, terms);
  else {
    os << "(and";
    for (GoalRewardList::const_iterator gi = conjuncts_.begin();
	 gi != conjuncts_.end(); gi++) {
      os << ' ';
      (*gi)->print(os, predicates, functions, terms);
    }
    os << ")";
  }
}

/* ====================================================================== */
/* Goal */

int Goal::goalsCount = 0;

Goal::Goal (const std::string name, const StateFormula &g, const GoalLinearReward &gr)
  : id_ (Goal::goalsCount), name_ (name), goal_ (&g), gr_ (&gr)
{
  Goal::goalsCount++;
  StateFormula::register_use (goal_);
}

Goal::~Goal ()
{
  delete goal_;
  delete gr_;
}

void Goal::print (std::ostream &os, const PredicateTable &predicates,
		  const FunctionTable &functions,
		  const TermTable &terms) const
{
  os << "( goal: " << name_ << " -- " << id_ << " ";
  goal_->print (os, predicates, functions, terms);
  gr_->print (os, predicates, functions, terms);
  os << ")";
}

} /* end of namespace */
