/* -*-C++-*- */
/*
 * Actions.
 *
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
#ifndef ACTIONS_H
#define ACTIONS_H

#include <config.h>
#include "terms.h"
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <assert.h>

namespace ppddl_parser
{

class PredicateTable;
class FunctionTable;
class ValueMap;
class StateFormula;
class AtomSet;
class Effect;
class Problem;


/* ====================================================================== */
/* ActionSchema */

class Action;
class ActionList;

/*
 * Action schema.
 */
class ActionSchema {
public:
  /* Constructs an action schema with the given name. */
  ActionSchema(const std::string& name);

  /* Deletes this action schema. */
  ~ActionSchema();

  /* Adds a parameter to this action schema. */
  void add_parameter(Variable parameter) { parameters_.push_back(parameter); }

  /* Sets the precondition of this action schema. */
  void set_precondition(const StateFormula& precondition);

  /* Sets the resource precondition of this action schema. */
  void set_resprecondition(std::vector<double> *respreondition);

  /* Sets the effect of this action schema. */
  void set_effect(const Effect& effect);
  
  /* Returns the name of this action schema. */
  const std::string& name() const { return name_; }

  /* Returns the arity of this action schema. */
  size_t arity() const { return parameters_.size(); }

  /* Returns the ith parameter of this action schema. */
  Variable parameter(size_t i) const { return parameters_[i]; }

  /* Returns the preconditions of this action schema. */
  const StateFormula& precondition() const { return *precondition_; }

  /* Returns the effect of this action schema. */
  const Effect& effect() const { return *effect_; }

  /* Fills the provided list with instantiations of this action
     schema. */
  void instantiations(ActionList& actions, const Problem& problem) const;

  /* Returns an instantiation of this action schema. */
  Action& instantiation(const SubstitutionMap& subst,
			      const Problem& problem,
			      const StateFormula& precond) const;

  /* Prints this action schema on the given stream. */
  void print(std::ostream& os, const PredicateTable& predicates,
	     const FunctionTable& functions, const TermTable& terms) const;

private:
  /* Action name. */
  std::string name_;
  /* Action parameters. */
  VariableList parameters_;
  /* Action precondition. */
  const StateFormula* precondition_;
  /* Action effect. */
  const Effect* effect_;
  /* Action resource preconditions */
  std::vector<double> *resprecondition_;
};


/* ====================================================================== */
/* ActionSchemaMap */

/*
 * Table of action schemas.
 */
class ActionSchemaMap : public std::map<std::string, const ActionSchema*> {
};


/* ====================================================================== */
/* Action */

/*
 * A fully instantiated action.
 */
class Action {
public:
  /* Constructs an action with the given name. */
  Action(const std::string& name);

  /* Deletes this action. */
  ~Action();

  /* Adds an argument to this action. */
  void add_argument(Object argument) { arguments_.push_back(argument); }

  /* Sets the precondition of this action. */
  void set_precondition(const StateFormula& precondition);

  /* Sets the effect of this action. */
  void set_effect(const Effect& effect);
  
  /* Returns the id for this action. */
  size_t id() const { return id_; }

  /* Returns the name of this action. */
  const std::string& name() const { return name_; }

  void print_complete_name (std::ostream &os, const TermTable &terms);

  /* Returns the arity of this action. */
  size_t arity() const { return arguments_.size(); }

  /* Returns the ith argument of this action. */
  Object argument(size_t i) const { return arguments_[i]; }

  /* Returns the precondition of this action. */
  const StateFormula& precondition() const { return *precondition_; }

  /* Returns the effect of this action. */
  const Effect& effect() const { return *effect_; }

  /* Tests if this action is enabled in the given state. */
  bool enabled(const AtomSet& atoms, const ValueMap& values) const;
  
  /* Changes the given state according to the effects of this action. */
  void affect(AtomSet& atoms, ValueMap& values) const;

  /* Prints this action on the given stream. */
  void print(std::ostream& os, const TermTable& terms) const;

  void print (std::ostream &os, const PredicateTable &predicates,
	      const FunctionTable &functions, const TermTable &terms) const;

  /* Prints this action on the given stream in XML. */
  void printXML(std::ostream& os, const TermTable& terms) const;
  
private:
  /* Next action id. */
  static size_t next_id;

  /* Unique id for actions. */
  size_t id_;
  /* Action name. */
  std::string name_;
  /* Action arguments. */
  ObjectList arguments_;
  /* Action precondition. */
  const StateFormula* precondition_;
  /* Action effect. */
  const Effect* effect_;
};

} /* end of namespace */

/*
 * Less than function object for action pointers.
 */
namespace std {
  template<> struct less<const ppddl_parser::Action*>
    : public binary_function<const ppddl_parser::Action*, const ppddl_parser::Action*, bool> {
    /* Comparison function operator. */
    bool operator()(const ppddl_parser::Action* a1, const ppddl_parser::Action* a2) const {
      if (a1 == NULL) {
	return a2 != NULL;
      } else if (a2 == NULL) {
	return false;
      } else {
	return a1->id() < a2->id();
      }
    }
  };
}

namespace ppddl_parser
{

/* ====================================================================== */
/* ActionList */

/*
 * List of actions.
 */
class ActionList : public std::vector<Action*> {
};

} /* end of namespace */

#endif /* ACTIONS_H */
