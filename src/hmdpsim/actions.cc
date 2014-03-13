/*
 * Copyright (C) 2003 Carnegie Mellon University and Rutgers University
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 */
#include "actions.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include <utility>
#include <stack>

using namespace std;

namespace ppddl_parser
{

/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema with the given name. */
ActionSchema::ActionSchema(const std::string& name)
  : name_(name), precondition_(&StateFormula::TRUE),
    effect_(new ConjunctiveEffect()) {
  StateFormula::register_use(precondition_);
  Effect::register_use(effect_);
}


/* Deletes this action schema. */
ActionSchema::~ActionSchema() {
  StateFormula::unregister_use(precondition_);
  Effect::unregister_use(effect_);
}


/* Sets the precondition of this action schema. */
void ActionSchema::set_precondition(const StateFormula& precondition) {
  if (&precondition != precondition_) {
    StateFormula::unregister_use(precondition_);
    precondition_ = &precondition;
    StateFormula::register_use(precondition_);
  }
}

/* Sets the effect of this action schema. */
void ActionSchema::set_effect(const Effect& effect) {
  if (&effect != effect_) {
    const Effect* tmp = effect_;
    effect_ = &effect;
    Effect::register_use(effect_);
    Effect::unregister_use(tmp);
  }
}


/* Fills the provided list with instantiations of this action
   schema. */
void ActionSchema::instantiations(ActionList& actions,
				  const Problem& problem) const {
  size_t n = arity();
  bool instantiated = false;
  if (n == 0) {
    SubstitutionMap subst;
    const StateFormula& precond = precondition().instantiation(subst, problem);
    if (!precond.contradiction()) {
      actions.push_back(&instantiation(subst, problem, precond));
      instantiated = true;
    }
  } else {
    SubstitutionMap args;
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.domain().terms().type(parameter(i)));
      if (arguments[i].empty()) {
	return;
      }
      next_arg.push_back(arguments[i].begin());
    }
    std::stack<const StateFormula*> preconds;
    preconds.push(precondition_);
    StateFormula::register_use(preconds.top());
    for (size_t i = 0; i < n; ) {
      args.insert(std::make_pair(parameter(i), *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameter(i), *next_arg[i]));
      const StateFormula& precond =
	preconds.top()->instantiation(pargs, problem);
      preconds.push(&precond);
      StateFormula::register_use(preconds.top());
      if (i + 1 == n || precond.contradiction()) {
	if (!precond.contradiction()) {
	  actions.push_back(&instantiation(args, problem, precond));
	  instantiated = true;
	}
	for (int j = i; j >= 0; j--) {
	  StateFormula::unregister_use(preconds.top());
	  preconds.pop();
	  args.erase(parameter(j));
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j].end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j].begin();
	    }
	  } else {
	    i = j;
	    break;
	  }
	}
      } else {
	i++;
      }
    }
    while (!preconds.empty()) {
      StateFormula::unregister_use(preconds.top());
      preconds.pop();
    }
  }
  if (! instantiated)
    {
      std::cout << "[Warning]:ActionShema::instantiation: action: " 
		<< name () << " not instantiated.\n";
    }
}


/* Returns an instantiation of this action schema. */
Action& ActionSchema::instantiation(const SubstitutionMap& subst,
					  const Problem& problem,
					  const StateFormula& precond) const {
  Action* action = new Action(name());
  size_t n = arity();
  for (size_t i = 0; i < n; i++) {
    SubstitutionMap::const_iterator si = subst.find(parameter(i));
    action->add_argument((*si).second);
  }
  action->set_precondition(precond);
  action->set_effect(effect().instantiation(subst, problem));
  return *action;
}


/* Prints this action schema on the given stream. */
void ActionSchema::print(std::ostream& os, const PredicateTable& predicates,
			 const FunctionTable& functions,
			 const TermTable& terms) const {
  os << "  " << name();
  os << std::endl << "    parameters:";
  for (VariableList::const_iterator vi = parameters_.begin();
       vi != parameters_.end(); vi++) {
    os << ' ';
    terms.print_term(os, *vi);
  }
  os << std::endl << "    precondition: ";
  precondition().print(os, predicates, functions, terms);
  os << std::endl << "    effect: ";
  effect().print(os, predicates, functions, terms);
}

/* ====================================================================== */
/* Action */

/* Next action id. */
size_t Action::next_id = 1;


/* Constructs an action with the given name. */
Action::Action(const std::string& name)
  : id_(next_id++), name_(name), precondition_(&StateFormula::TRUE),
    effect_(new ConjunctiveEffect()) {
  StateFormula::register_use(precondition_);
  Effect::register_use(effect_);
}


/* Deletes this action. */
Action::~Action() {
  StateFormula::unregister_use(precondition_);
  Effect::unregister_use(effect_);
}


/* Sets the precondition of this action. */
void Action::set_precondition(const StateFormula& precondition) {
  if (&precondition != precondition_) {
    StateFormula::unregister_use(precondition_);
    precondition_ = &precondition;
    StateFormula::register_use(precondition_);
  }
}


/* Sets the effect of this action. */
void Action::set_effect(const Effect& effect) {
  if (&effect != effect_) {
    Effect::unregister_use(effect_);
    effect_ = &effect;
    Effect::register_use(effect_);
  }
}


/* Tests if this action is enabled in the given state. */
bool Action::enabled(const AtomSet& atoms, const ValueMap& values) const {
  return precondition_->holds(atoms, values);
}

/* Changes the given state according to the effects of this action. */
void Action::affect(AtomSet& atoms, ValueMap& values) const {
  AtomList adds;
  AtomList deletes;
  AssignmentList assignments;
  effect().state_change(adds, deletes, assignments, atoms, values);
  for (AtomList::const_iterator ai = deletes.begin();
       ai != deletes.end(); ai++) {
    atoms.erase(*ai);
  }
  atoms.insert(adds.begin(), adds.end());
  for (AssignmentList::const_iterator ai = assignments.begin();
       ai != assignments.end(); ai++) {
    (*ai)->affect(values);
  }
}

void Action::print_complete_name (std::ostream &os, const TermTable &terms)
{
  os << name_;
  for (ObjectList::const_iterator oi = arguments_.begin();
       oi != arguments_.end(); oi++) {
    os << '_';
    terms.print_term(os, *oi);
  }
}

/* Prints this action on the given stream. */
void Action::print(std::ostream& os, const TermTable& terms) const {
  os << '(' << name();
  for (ObjectList::const_iterator oi = arguments_.begin();
       oi != arguments_.end(); oi++) {
    os << ' ';
    terms.print_term(os, *oi);
  }
  os << ')';
}

void Action::print (std::ostream &os, const PredicateTable &predicates,
		    const FunctionTable &functions, const TermTable &terms) const
{
  print (os, terms);
  precondition ().print (os, predicates, functions, terms);
  effect ().print (os, predicates, functions, terms);
}

/* Prints this action on the given stream in XML. */
void Action::printXML(std::ostream& os, const TermTable& terms) const {
  os << "<action><name>" << name() << "</name>";
  for (ObjectList::const_iterator oi = arguments_.begin();
       oi != arguments_.end(); oi++) {
    os << "<term>";
    terms.print_term(os, *oi);
    os << "</term>";
  }
  os << "</action>";
}

bool isthereMore(std::list<std::pair<double, double> >* llarr, std::list<std::pair<double, double> >::iterator *itarr, int numResources) {

  int i = numResources - 1;
  bool changed=false;

  while (!changed && i>=0) {
    itarr[i]++;
    if (itarr[i] != llarr[i].end()) changed=true;
    else itarr[i] = llarr[i].begin();
    i--;
  }
  if (changed) return true; else return false;
}

} /* end of namespace */
