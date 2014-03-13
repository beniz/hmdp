/* -*-C++-*- */
/*
 * Problem descriptions.
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
#ifndef PROBLEMS_H
#define PROBLEMS_H

#include <config.h>
#include "actions.h"
#include "effects.h"
#include "formulas.h"
#include "expressions.h"
#include "terms.h"
#include "types.h"
#include "goals.h"
#include "probabilityDistribution.h"
#include <iostream>
#include <map>
#include <string>

namespace ppddl_parser
{

class Domain;

/*
 * Problem definition.
 */
class Problem {
public:
  /* Table of problem definitions. */
  class ProblemMap : public std::map<std::string, const Problem*> {
  };
  /* Returns a const_iterator pointing to the first problem. */
  static ProblemMap::const_iterator begin();
  
  /* Returns a const_iterator pointing beyond the last problem. */
  static ProblemMap::const_iterator end();

  /* Returns the problem with the given name, or NULL if it is undefined. */
  static const Problem* find(const std::string& name);

  /* Removes all defined problems. */
  static void clear();

  /* number of problems. */
  static size_t size();

  /* Constructs a problem. */
  Problem(const std::string& name, const Domain& domain);

  /* Deletes a problem. */
  ~Problem();

  /* Returns the name of this problem. */
  const std::string& name() const { return name_; }

  /* Returns the domain of this problem. */
  const Domain& domain() const { return *domain_; }

  /* Returns the term table of this problem. */
  TermTable& terms() { return terms_; }

  /* Returns the term table of this problem. */
  const TermTable& terms() const { return terms_; }

  /* Adds an atomic state formula to the initial conditions of this
     problem. */
  void add_init_atom(const Atom& atom);

  /* Adds a function application value to the initial conditions of
     this problem. */
  void add_init_value(const Application& application, const Rational& value);

  /* Adds an initial effect for this problem. */
  void add_init_effect(const Effect& effect);

  /* Add an initial probability distribution for this problem. */
  void add_init_prob_dist (const Application *rsc, const Expression *expr);

  /* Sets the metric to maximize for this problem. */
  void set_metric(const Expression& metric, bool negate = false);

  /* Instantiates all actions. */
  void instantiate_actions();

  /* Fills the provided object list with objects (including constants
     declared in the domain) that are compatible with the given
     type. */
  void compatible_objects(ObjectList& objects, Type type) const;

  /* Returns the initial atoms of this problem. */
  const AtomSet& init_atoms() const { return init_atoms_; }

  /* Returns the initial values of this problem. */
  const ValueMap& init_values() const { return init_values_; }

  /* Returns the initial list of distributions of this problem. */
  const ProbabilityDistMap& init_dists() const { return init_prob_dists_; }

  /* Returns the initial effects of this problem. */
  const EffectList& init_effects() const { return init_effects_; }

  /* Returns the set of goals */
  const GoalMap& getGoals () const { return goals_; }
  
  /* Adds a goal to this problem. */
  void add_goal (Goal *g);

  /* Returns the metric to maximize for this problem. */
  const Expression& metric() const { return *metric_; }

  /* Tests if the metric is constant. */
  bool constant_metric() const;

  /* Returns a list of instantiated actions. */
  const ActionList& actions() const { return actions_; }

  /* Fills the given list with actions enabled in the given state. */
  void enabled_actions(ActionList& actions, const AtomSet& atoms,
		       const ValueMap& values) const;

private:
  /* Table of defined problems. */
  static ProblemMap problems;

  /* Name of problem. */
  std::string name_;
  /* Problem domain. */
  const Domain* domain_;
  /* Problem terms. */
  TermTable terms_;
  /* Initial atoms. */
  AtomSet init_atoms_;
  /* Initial function application values. */
  ValueMap init_values_;
  /* Initial effects. */
  EffectList init_effects_;
  /* Initial distributions over resources. */
  ProbabilityDistMap init_prob_dists_;

  GoalMap goals_;  /**< set of goals in this problem */

  /* Metric to maximize. */
  const Expression* metric_;
  /* Instantiated actions. */
  ActionList actions_;

  friend std::ostream& operator<<(std::ostream& os, const Problem& p);
};

/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p);

} /* end of namespace */

#endif /* PROBLEMS_H */
