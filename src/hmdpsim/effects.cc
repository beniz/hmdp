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
#include "effects.h"
#include "problems.h"
#include "formulas.h"
#include "expressions.h"
#include <stack>
#include <stdexcept>

namespace ppddl_parser
{

/* ====================================================================== */
/* Assignment */

/* Constructs an assignment. */
Assignment::Assignment(AssignOp oper, const Application& application,
		       const Expression& expr)
  : operator_(oper), application_(&application), expr_(&expr) {
  Expression::register_use(application_);
  Expression::register_use(expr_);
}

/* Deletes this assignment. */
Assignment::~Assignment() {
  Expression::unregister_use(application_);
  Expression::unregister_use(expr_);
}


/* Changes the given state according to this assignment. */
void Assignment::affect(ValueMap& values) const {
  if (operator_ == ASSIGN_OP) {
    values[application_] = expr_->value(values);
  } else {
    ValueMap::const_iterator vi = values.find(application_);
    if (vi == values.end()) {
      throw std::logic_error("changing undefined value");
    } else if (operator_ == SCALE_UP_OP) {
      values[application_] = (*vi).second * expr_->value(values);
    } else if (operator_ == SCALE_DOWN_OP) {
      values[application_] = (*vi).second / expr_->value(values);
    } else if (operator_ == INCREASE_OP) {
      values[application_] = (*vi).second + expr_->value(values);
    } else { /* operator_ == DECREASE_OP */
      values[application_] = (*vi).second - expr_->value(values);
    }
  }
}


/* Returns an instantiation of this assignment. */
const Assignment& Assignment::instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const {
  return *new Assignment(operator_,
			 application_->substitution(subst),
			 expr_->instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void Assignment::print(std::ostream& os, const FunctionTable& functions,
		       const TermTable& terms) const {
  os << '(';
  if (operator_ == ASSIGN_OP) {
    os << "assign ";
  } else if (operator_ == SCALE_UP_OP) {
    os << "scale-up ";
  } else if (operator_ == SCALE_DOWN_OP) {
    os << "scale-down ";
  } else if (operator_ == INCREASE_OP) {
    os << "increase ";
  } else if (operator_ == DECREASE_OP) {
    os << "decrease ";
  }
  else if (operator_ == INCREASE_PROB_OP)
    {
      os << "increase_prob";
    }
  else if (operator_ == DECREASE_PROB_OP)
    {
      os << "decrease_prob";
    }
  else {}

  application_->print(os, functions, terms);
  os << ' ';
  if (expr_)
    expr_->print(os, functions, terms);
  os << ')';
}


/* ====================================================================== */
/* SimpleEffect */

/* Constructs a simple effect. */
SimpleEffect::SimpleEffect(const Atom& atom)
  : atom_(&atom) {
  eft_ = EF_SMP;
  StateFormula::register_use(atom_);
}


/* Deletes this simple effect. */
SimpleEffect::~SimpleEffect() {
  StateFormula::unregister_use(atom_);
}


/* ====================================================================== */
/* AddEffect */

/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void AddEffect::state_change(AtomList& adds, AtomList& deletes,
			     AssignmentList& assignments,
			     const AtomSet& atoms,
			     const ValueMap& values) const {
  adds.push_back(&atom());
}


/* Returns an instantiation of this effect. */
const Effect& AddEffect::instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const {
  const Atom* inst_atom = &atom().substitution(subst);
  if (inst_atom == &atom()) {
    return *this;
  } else {
    return *new AddEffect(*inst_atom);
  }
}


/* Prints this object on the given stream. */
void AddEffect::print(std::ostream& os, const PredicateTable& predicates,
		      const FunctionTable& functions,
		      const TermTable& terms) const {
  atom().print(os, predicates, functions, terms);
}


/* ====================================================================== */
/* DeleteEffect */

/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void DeleteEffect::state_change(AtomList& adds, AtomList& deletes,
				AssignmentList& assignments,
				const AtomSet& atoms,
				const ValueMap& values) const {
  deletes.push_back(&atom());
}


/* Returns an instantiation of this effect. */
const Effect& DeleteEffect::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  const Atom* inst_atom = &atom().substitution(subst);
  if (inst_atom == &atom()) {
    return *this;
  } else {
    return *new DeleteEffect(*inst_atom);
  }
}


/* Prints this object on the given stream. */
void DeleteEffect::print(std::ostream& os, const PredicateTable& predicates,
			 const FunctionTable& functions,
			 const TermTable& terms) const {
  os << "(not ";
  atom().print(os, predicates, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* AssignmentEffect */

/* Constructs an assignment effect. */
AssignmentEffect::AssignmentEffect(const Assignment& assignment)
  : assignment_(&assignment) {
  eft_ = EF_ASSIGN;
}


/* Deletes this assignment effect. */
AssignmentEffect::~AssignmentEffect() {
  delete assignment_;
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void AssignmentEffect::state_change(AtomList& adds, AtomList& deletes,
				    AssignmentList& assignments,
				    const AtomSet& atoms,
				    const ValueMap& values) const {
  assignments.push_back(assignment_);
}


/* Returns an instantiation of this effect. */
const Effect& AssignmentEffect::instantiation(const SubstitutionMap& subst,
					      const Problem& problem) const {
  return *new AssignmentEffect(assignment().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void AssignmentEffect::print(std::ostream& os,
			     const PredicateTable& predicates,
			     const FunctionTable& functions,
			     const TermTable& terms) const {
  assignment().print(os, functions, terms);
}


/* ====================================================================== */
/* ConjunctiveEffect */

/* Deletes this conjunctive effect. */
ConjunctiveEffect::~ConjunctiveEffect() {
  for (EffectList::const_iterator ei = conjuncts_.begin();
       ei != conjuncts_.end(); ei++) {
    unregister_use(*ei);
  }
}


/* Adds a conjunct to this conjunctive effect. */
void ConjunctiveEffect::add_conjunct(const Effect& conjunct) {
  const ConjunctiveEffect* conj_effect =
    dynamic_cast<const ConjunctiveEffect*>(&conjunct);
  if (conj_effect != NULL) {
    for (EffectList::const_iterator ei = conj_effect->conjuncts_.begin();
	 ei != conj_effect->conjuncts_.end(); ei++) {
      conjuncts_.push_back(*ei);
      register_use(*ei);
    }
    register_use(&conjunct);
    unregister_use(&conjunct);
  } else {
    conjuncts_.push_back(&conjunct);
    register_use(&conjunct);
  }
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void ConjunctiveEffect::state_change(AtomList& adds, AtomList& deletes,
				     AssignmentList& assignments,
				     const AtomSet& atoms,
				     const ValueMap& values) const {
  for (EffectList::const_iterator ei = conjuncts_.begin();
       ei != conjuncts_.end(); ei++) {
    (*ei)->state_change(adds, deletes, assignments, atoms, values);
  }
}


/* Returns an instantiation of this effect. */
const Effect& ConjunctiveEffect::instantiation(const SubstitutionMap& subst,
					       const Problem& problem) const {
  ConjunctiveEffect& inst_effect = *new ConjunctiveEffect();
  for (EffectList::const_iterator ei = conjuncts_.begin();
       ei != conjuncts_.end(); ei++) {
    inst_effect.add_conjunct((*ei)->instantiation(subst, problem));
  }
  return inst_effect;
}


/* Prints this object on the given stream. */
void ConjunctiveEffect::print(std::ostream& os,
			      const PredicateTable& predicates,
			      const FunctionTable& functions,
			      const TermTable& terms) const {
  if (size() == 1) {
    conjunct(0).print(os, predicates, functions, terms);
  } else {
    os << "(and";
    for (EffectList::const_iterator ei = conjuncts_.begin();
	 ei != conjuncts_.end(); ei++) {
      os << ' ';
      (*ei)->print(os, predicates, functions, terms);
    }
    os << ")";
  }
}


/* ====================================================================== */
/* ConditionalEffect */

/* Returns a conditional effect. */
const Effect& ConditionalEffect::make(const StateFormula& condition,
				      const Effect& effect) {
  if (condition.tautology()) {
    return effect;
  } else if (condition.contradiction()) {
    return *new ConjunctiveEffect();
  } else {
    return *new ConditionalEffect(condition, effect);
  }
}


/* Constructs a conditional effect. */
ConditionalEffect::ConditionalEffect(const StateFormula& condition,
				     const Effect& effect)
  : condition_(&condition), effect_(&effect) {
  eft_ = EF_COND;
  StateFormula::register_use(condition_);
  register_use(effect_);
}


/* Deletes this conditional effect. */
ConditionalEffect::~ConditionalEffect() {
  StateFormula::unregister_use(condition_);
  unregister_use(effect_);
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void ConditionalEffect::state_change(AtomList& adds, AtomList& deletes,
				     AssignmentList& assignments,
				     const AtomSet& atoms,
				     const ValueMap& values) const {
  if (condition().holds(atoms, values)) {
    /* Effect condition holds. */
    effect().state_change(adds, deletes, assignments, atoms, values);
  }
}


/* Returns an instantiation of this effect. */
const Effect& ConditionalEffect::instantiation(const SubstitutionMap& subst,
					       const Problem& problem) const {
  return make(condition().instantiation(subst, problem),
	      effect().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void ConditionalEffect::print(std::ostream& os,
			      const PredicateTable& predicates,
			      const FunctionTable& functions,
			      const TermTable& terms) const {
  os << "(when ";
  condition().print(os, predicates, functions, terms);
  os << ' ';
  effect().print(os, predicates, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* ProbabilisticEffect */

/* Deletes this probabilistic effect. */
ProbabilisticEffect::~ProbabilisticEffect() {
  for (EffectList::const_iterator ei = effects_.begin();
       ei != effects_.end(); ei++) {
    unregister_use(*ei);
  }
}


/* Adds an outcome to this probabilistic effect. */
bool ProbabilisticEffect::add_outcome(const Rational& p,
				      const Effect& effect) {
  const ProbabilisticEffect* prob_effect =
    dynamic_cast<const ProbabilisticEffect*>(&effect);
  if (prob_effect != NULL) {
    size_t n = prob_effect->size();
    for (size_t i = 0; i < n; i++) {
      if (!add_outcome(p*prob_effect->probability(i),
		       prob_effect->effect(i))) {
	return false;
      }
    }
    register_use(&effect);
    unregister_use(&effect);
  } else if (p != 0) {
    effects_.push_back(&effect);
    register_use(&effect);
    if (weight_sum_ == 0) {
      weights_.push_back(p.numerator());
      weight_sum_ = p.denominator();
      return true;
    } else {
      std::pair<int, int> m =
	Rational::multipliers(weight_sum_, p.denominator());
      int sum = 0;
      size_t n = size();
      for (size_t i = 0; i < n; i++) {
	sum += weights_[i] *= m.first;
      }
      weights_.push_back(p.numerator()*m.second);
      sum += p.numerator()*m.second;
      weight_sum_ *= m.first;
      return sum <= weight_sum_;
    }
  }
  return true;
}


/* Returns the ith outcome's probability. */
Rational ProbabilisticEffect::probability(size_t i) const {
  return Rational(weights_[i], weight_sum_);
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void ProbabilisticEffect::state_change(AtomList& adds, AtomList& deletes,
				       AssignmentList& assignments,
				       const AtomSet& atoms,
				       const ValueMap& values) const {
  if (size() != 0) {
    int w = int(rand()/(RAND_MAX + 1.0)*weight_sum_);
    int wtot = 0;
    size_t n = size();
    for (size_t i = 0; i < n; i++) {
      wtot += weights_[i];
      if (w < wtot) {
	effect(i).state_change(adds, deletes, assignments, atoms, values);
	return;
      }
    }
  }
}


/* Returns an instantiation of this effect. */
const Effect&
ProbabilisticEffect::instantiation(const SubstitutionMap& subst,
				   const Problem& problem) const {
  ProbabilisticEffect& inst_effect = *new ProbabilisticEffect();
  size_t n = size();
  for (size_t i = 0; i < n; i++) {
    inst_effect.add_outcome(probability(i),
			    effect(i).instantiation(subst, problem));
  }
  return inst_effect;
}


/* Prints this object on the given stream. */
void ProbabilisticEffect::print(std::ostream& os,
				const PredicateTable& predicates,
				const FunctionTable& functions,
				const TermTable& terms) const {
  if (weight_sum_ == 0) {
    os << "(and)";
  } else if (weight_sum_ == weights_.back()) {
    effect(0).print(os, predicates, functions, terms);
  } else {
    os << "(probabilistic";
    size_t n = size();
    for (size_t i = 0; i < n; i++) {
      os << ' ' << probability(i) << ' ';
      effect(i).print(os, predicates, functions, terms);
    }
    os << ")";
  }
}


/* ====================================================================== */
/* QuantifiedEffect */

/* Constructs a universally quantified effect. */
QuantifiedEffect::QuantifiedEffect(const Effect& effect)
  : effect_(&effect) {
  eft_ = EF_QUANT;
  register_use(effect_);
}


/* Deletes this universally quantifed effect. */
QuantifiedEffect::~QuantifiedEffect() {
  unregister_use(effect_);
}


/* Fills the provided lists with a sampled state change for this
   effect in the given state. */
void QuantifiedEffect::state_change(AtomList& adds, AtomList& deletes,
				    AssignmentList& assignments,
				    const AtomSet& atoms,
				    const ValueMap& values) const {
  effect().state_change(adds, deletes, assignments, atoms, values);
}


/* Returns an instantiation of this effect. */
const Effect& QuantifiedEffect::instantiation(const SubstitutionMap& subst,
					      const Problem& problem) const {
  int n = arity();
  if (n == 0) {
    return effect().instantiation(subst, problem);
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (int i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.terms().type(parameter(i)));
      if (arguments[i].empty()) {
	return *new ConjunctiveEffect();
      }
      next_arg.push_back(arguments[i].begin());
    }
    ConjunctiveEffect* conj = new ConjunctiveEffect();
    std::stack<const Effect*> conjuncts;
    conjuncts.push(&effect().instantiation(args, problem));
    register_use(conjuncts.top());
    for (int i = 0; i < n; ) {
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameter(i), *next_arg[i]));
      const Effect& conjunct = conjuncts.top()->instantiation(pargs, problem);
      conjuncts.push(&conjunct);
      if (i + 1 == n) {
	conj->add_conjunct(conjunct);
	for (int j = i; j >= 0; j--) {
	  if (j < i) {
	    unregister_use(conjuncts.top());
	  }
	  conjuncts.pop();
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
	register_use(conjuncts.top());
	i++;
      }
    }
    while (!conjuncts.empty()) {
      unregister_use(conjuncts.top());
      conjuncts.pop();
    }
    return *conj;
  }
}


/* Prints this object on the given stream. */
void QuantifiedEffect::print(std::ostream& os,
			     const PredicateTable& predicates,
			     const FunctionTable& functions,
			     const TermTable& terms) const {
  if (parameters_.empty()) {
    effect().print(os, predicates, functions, terms);
  } else {
    os << "(forall (";
    VariableList::const_iterator vi = parameters_.begin();
    terms.print_term(os, *vi);
    for (vi++; vi != parameters_.end(); vi++) {
      os << ' ';
      terms.print_term(os, *vi);
    }
    os << ") ";
    effect().print(os, predicates, functions, terms);
    os << ")";
  }
}

} /* end of namespace */
