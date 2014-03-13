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
#include "formulas.h"
#include "problems.h"
#include "domains.h"
#include "expressions.h"
#include "exceptions.h"
#include "strxml.h"
#include <stack>

namespace ppddl_parser 
{

/* ====================================================================== */
/* StateFormula */

/* A formula true in every state. */
const StateFormula& StateFormula::TRUE = Constant::TRUE_;
/* A formula false in every state. */
const StateFormula& StateFormula::FALSE = Constant::FALSE_;


/* ====================================================================== */
/* Constant */

/* Constant representing true. */
const Constant Constant::TRUE_(true);
/* Constant representing false. */
const Constant Constant::FALSE_(false);


/* Constructs a constant state formula. */
Constant::Constant(bool value)
  : value_(value) {
  stft_ = STF_CST;
  register_use(this);
}


/* Tests if this state formula holds in the given state. */
bool Constant::holds(const AtomSet& atoms, const ValueMap& values) const {
  return value_;
}

/* Returns an instantiation of this state formula. */
const Constant& Constant::instantiation(const SubstitutionMap& subst,
					const Problem& problem) const {
  return *this;
}


/* Prints this object on the given stream. */
void Constant::print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const {
  os << (value_ ? "(and)" : "(or)");
}


/* ====================================================================== */
/* Atom */

/* Table of atomic state formulas. */
Atom::AtomTable Atom::atoms;


/* Comparison function. */
bool Atom::AtomLess::operator()(const Atom* a1, const Atom* a2) const {
  if (a1->predicate() < a2->predicate()) {
    return true;
  } else if (a1->predicate() > a2->predicate()) {
    return false;
  } else {
    for (size_t i = 0; i < a1->arity(); i++) {
      if (a1->term(i) < a2->term(i)) {
	return true;
      } else if (a1->term(i) > a2->term(i)) {
	return false;
      }
    }
    return false;
  }
}


/* Returns an atomic state formula with the given predicate and terms. */
const Atom& Atom::make_atom(Predicate predicate, const TermList& terms) {
  Atom* atom = new Atom(predicate);
  bool ground = true;
  for (TermList::const_iterator ti = terms.begin(); ti != terms.end(); ti++) {
    atom->add_term(*ti);
    if (ground && is_variable(*ti)) {
      ground = false;
    }
  }
  if (!ground) {
    return *atom;
  } else {
    std::pair<AtomTable::const_iterator, bool> result = atoms.insert(atom);
    if (!result.second) {
      delete atom;
      return **result.first;
    } else {
      return *atom;
    }
  }
}


/* Deletes this atomic state formula. */
Atom::~Atom() {
  AtomTable::const_iterator ai = atoms.find(this);
  if (*ai == this) {
    atoms.erase(ai);
  }
}


/* Tests if this state formula holds in the given state. */
bool Atom::holds(const AtomSet& atoms, const ValueMap& values) const {
  return atoms.find(this) != atoms.end();
}


/* Returns this atom subject to the given substitution. */
const Atom& Atom::substitution(const SubstitutionMap& subst) const {
  TermList inst_terms;
  bool substituted = false;
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    SubstitutionMap::const_iterator si =
      is_variable(*ti) ? subst.find(*ti) : subst.end();
    if (si != subst.end()) {
      inst_terms.push_back((*si).second);
      substituted = true;
    } else {
      inst_terms.push_back(*ti);
    }
  }
  if (substituted) {
    return make_atom(predicate(), inst_terms);
  } else {
    return *this;
  }
}


/* Returns an instantiation of this state formula. */
const StateFormula& Atom::instantiation(const SubstitutionMap& subst,
					const Problem& problem) const {
  if (terms_.empty()) {
    if (problem.domain().predicates().static_predicate(predicate())) {
      if (problem.init_atoms().find(this) != problem.init_atoms().end()) {
	return TRUE;
      } else {
	return FALSE;
      }
    } else {
      return *this;
    }
  }
  TermList inst_terms;
  bool substituted = false;
  size_t objects = 0;
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    SubstitutionMap::const_iterator si =
      is_variable(*ti) ? subst.find(*ti) : subst.end();
    if (si != subst.end()) {
      inst_terms.push_back((*si).second);
      substituted = true;
      objects++;
    } else {
      inst_terms.push_back(*ti);
      if (is_object(*ti)) {
	objects++;
      }
    }
  }
  if (substituted) {
    const Atom& inst_atom = make_atom(predicate(), inst_terms);
    if (problem.domain().predicates().static_predicate(predicate())
	&& objects == inst_terms.size()) {
      if (problem.init_atoms().find(&inst_atom)
	  != problem.init_atoms().end()) {
	return TRUE;
      } else {
	return FALSE;
      }
    } else {
      return inst_atom;
    }
  } else { 
    return *this;
  }
}


/* Prints this object on the given stream. */
void Atom::print(std::ostream& os, const PredicateTable& predicates,
		 const FunctionTable& functions,
		 const TermTable& terms) const {
  os << '(';
  predicates.print_predicate(os, predicate());
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    os << ' ';
    terms.print_term(os, *ti);
  }
  os << ')';
}

void Atom::print (std::string &str, const PredicateTable& predicates,
		  const FunctionTable& functions,
		  const TermTable& terms) const 
{
  str += "(";
  predicates.print_predicate(str, predicate());
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    str += " ";
    terms.print_term(str, *ti);
  }
  str += ")";
}

/* Prints this object on the given stream in XML. */
void Atom::printXML(std::ostream& os, const PredicateTable& predicates,
		    const FunctionTable& functions,
		    const TermTable& terms) const {
  os << "<atom><predicate>";
  predicates.print_predicate(os, predicate());
  os << "</predicate>";
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    os << "<term>";
    terms.print_term(os, *ti);
    os << "</term>";
  }
  os << "</atom>";
}

/* ====================================================================== */
/* Equality */

/* Tests if this state formula holds in the given state. */
bool Equality::holds(const AtomSet& atoms, const ValueMap& values) const {
  throw Exception("Equality::holds erroneously called");
}


/* Returns an instantiation of this state formula. */
const StateFormula& Equality::instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const {
  Term inst_term1 = term1_;
  if (is_variable(inst_term1)) {
    const SubstitutionMap::const_iterator si = subst.find(inst_term1);
    if (si != subst.end()) {
      inst_term1 = (*si).second;
    }
  }
  Term inst_term2 = term2_;
  if (is_variable(inst_term2)) {
    const SubstitutionMap::const_iterator si = subst.find(inst_term2);
    if (si != subst.end()) {
      inst_term2 = (*si).second;
    }
  }
  if (is_object(inst_term1) && is_object(inst_term2)) {
    return (inst_term1 == inst_term2) ? TRUE : FALSE;
  } else if (inst_term1 == term1_ && inst_term2 == term2_) {
    return *this;
  } else {
    return *new Equality(inst_term1, inst_term2);
  }
}


/* Prints this object on the given stream. */
void Equality::print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const {
  os << "(= ";
  terms.print_term(os, term1());
  os << ' ';
  terms.print_term(os, term2());
  os << ")";
}


/* ====================================================================== */
/* Comparison */

/* Constructs a comparison. */
Comparison::Comparison(CmpPredicate predicate,
		       const Expression& expr1, const Expression& expr2)
  : predicate_(predicate), expr1_(&expr1), expr2_(&expr2) {
  stft_ = STF_CMP;
  Expression::register_use(expr1_);
  Expression::register_use(expr2_);
}


/* Deletes this comparison. */
Comparison::~Comparison() {
  Expression::unregister_use(expr1_);
  Expression::unregister_use(expr2_);
}


/* Tests if this state formula holds in the given state. */
bool Comparison::holds(const AtomSet& atoms, const ValueMap& values) const {
  if (predicate() == LT_CMP) {
    return expr1().value(values) < expr2().value(values);
  } else if (predicate() == LE_CMP) {
    return expr1().value(values) <= expr2().value(values);
  } else if (predicate() == EQ_CMP) {
    return expr1().value(values) == expr2().value(values);
  } else if (predicate() == GE_CMP) {
    return expr1().value(values) >= expr2().value(values);
  } else { /* predicate()_ == GT_CMP */
    return expr1().value(values) > expr2().value(values);
  }
}


/* Returns an instantiation of this state formula. */
const StateFormula& Comparison::instantiation(const SubstitutionMap& subst,
					      const Problem& problem) const {
  const Expression& inst_expr1 = expr1().instantiation(subst, problem);
  const Expression& inst_expr2 = expr2().instantiation(subst, problem);
  const Value* v1 = dynamic_cast<const Value*>(&inst_expr1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&inst_expr2);
    if (v2 != NULL) {
      bool value;
      if (predicate() == LT_CMP) {
	value = v1->value() < v2->value();
      } else if (predicate() == LE_CMP) {
	value = v1->value() <= v2->value();
      } else if (predicate() == EQ_CMP) {
	value = v1->value() == v2->value();
      } else if (predicate() == GE_CMP) {
	value = v1->value() >= v2->value();
      } else { /* predicate() == GT_CMP */
	value = v1->value() > v2->value();
      }
      Expression::register_use(v1);
      Expression::register_use(v2);
      Expression::unregister_use(v1);
      Expression::unregister_use(v2);
      return value ? TRUE : FALSE;
    }
  }
  return *new Comparison(predicate(), inst_expr1, inst_expr2);
}


/* Prints this object on the given stream. */
void Comparison::print(std::ostream& os, const PredicateTable& predicates,
		       const FunctionTable& functions,
		       const TermTable& terms) const {
  os << '(';
  if (predicate() == LT_CMP) {
    os << '<';
  } else if (predicate() == LE_CMP) {
    os << "<=";
  } else if (predicate() == EQ_CMP) {
    os << '=';
  } else if (predicate() == GE_CMP) {
    os << ">=";
  } else { /* predicate() == GT_CMP */
    os << '>';
  }
  os << ' ';
  expr1().print(os, functions, terms);
  os << ' ';
  expr2().print(os, functions, terms);
  os << ')';
}


/* ====================================================================== */
/* Negation */

/* Returns the negation of the given state formula. */
const StateFormula& Negation::make_negation(const StateFormula& formula) {
  if (formula.tautology()) {
    return FALSE;
  } else if (formula.contradiction()) {
    return TRUE;
  } else {
    return *(new Negation(formula));
  }
}


/* Constructs a negated state formula. */
Negation::Negation(const StateFormula& negand)
  : negand_(&negand) {
  stft_ = STF_NEG;
  register_use(negand_);
}


/* Deletes this negation. */
Negation::~Negation() {
  unregister_use(negand_);
}


/* Tests if this state formula holds in the given state. */
bool Negation::holds(const AtomSet& atoms, const ValueMap& values) const {
  return !negand().holds(atoms, values);
}


/* Returns an instantiation of this state formula. */
const StateFormula& Negation::instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const {
  const StateFormula& inst_negand = negand().instantiation(subst, problem);
  if (inst_negand.contradiction()) {
    return TRUE;
  } else if (inst_negand.tautology()) {
    return FALSE;
  } else {
    return *new Negation(inst_negand);
  }
}


/* Prints this object on the given stream. */
void Negation::print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const {
  os << "(not ";
  negand().print(os, predicates, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* Conjunction. */

/* Deletes this conjunction. */
Conjunction::~Conjunction() {
  for (FormulaList::const_iterator fi = conjuncts_.begin();
       fi != conjuncts_.end(); fi++) {
    unregister_use(*fi);
  }
}


/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const StateFormula& conjunct) {
  if (!conjunct.tautology()) {
    conjuncts_.push_back(&conjunct);
    register_use(&conjunct);
  }
}


/* Tests if this state formula holds in the given state. */
bool Conjunction::holds(const AtomSet& atoms, const ValueMap& values) const {
  for (FormulaList::const_iterator fi = conjuncts_.begin();
       fi != conjuncts_.end(); fi++) {
    if (!(*fi)->holds(atoms, values)) {
      return false;
    }
  }
  return true;
}


/* Returns an instantiation of this state formula. */
const StateFormula& Conjunction::instantiation(const SubstitutionMap& subst,
					       const Problem& problem) const {
  Conjunction* conj = NULL;
  const StateFormula* first_c = &TRUE;
  for (FormulaList::const_iterator fi = conjuncts_.begin();
       fi != conjuncts_.end(); fi++) {
    const StateFormula& c = (*fi)->instantiation(subst, problem);
    if (c.contradiction()) {
      if (conj == NULL) {
	register_use(first_c);
	unregister_use(first_c);
      } else {
	register_use(conj);
	unregister_use(conj);
      }
      return FALSE;
    } else if (!c.tautology()) {
      if (first_c->tautology()) {
	first_c = &c;
      } else if (conj == NULL) {
	conj = new Conjunction();
	conj->add_conjunct(*first_c);
      }
      if (conj != NULL) {
	conj->add_conjunct(c);
      }
    }
  }
  if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Prints this object on the given stream. */
void Conjunction::print(std::ostream& os, const PredicateTable& predicates,
			const FunctionTable& functions,
			const TermTable& terms) const {
  if (size() == 1) {
    conjunct(0).print(os, predicates, functions, terms);
  } else {
    os << "(and";
    for (FormulaList::const_iterator fi = conjuncts_.begin();
	 fi != conjuncts_.end(); fi++) {
      os << ' ';
      (*fi)->print(os, predicates, functions, terms);
    }
    os << ")";
  }
}


/* ====================================================================== */
/* Disjunction. */

/* Deletes this disjunction. */
Disjunction::~Disjunction() {
  for (FormulaList::const_iterator fi = disjuncts_.begin();
       fi != disjuncts_.end(); fi++) {
    unregister_use(*fi);
  }
}


/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const StateFormula& disjunct) {
  if (!disjunct.contradiction()) {
    disjuncts_.push_back(&disjunct);
    register_use(&disjunct);
  }
}


/* Tests if this state formula holds in the given state. */
bool Disjunction::holds(const AtomSet& atoms, const ValueMap& values) const {
  for (FormulaList::const_iterator fi = disjuncts_.begin();
       fi != disjuncts_.end(); fi++) {
    if ((*fi)->holds(atoms, values)) {
      return true;
    }
  }
  return false;
}


/* Returns an instantiation of this formula. */
const StateFormula& Disjunction::instantiation(const SubstitutionMap& subst,
					       const Problem& problem) const {
  Disjunction* disj = NULL;
  const StateFormula* first_d = &FALSE;
  for (FormulaList::const_iterator fi = disjuncts_.begin();
       fi != disjuncts_.end(); fi++) {
    const StateFormula& d = (*fi)->instantiation(subst, problem);
    if (d.tautology()) {
      if (disj == NULL) {
	register_use(first_d);
	unregister_use(first_d);
      } else {
	register_use(disj);
	unregister_use(disj);
      }
      return TRUE;
    } else if (!d.contradiction()) {
      if (first_d->contradiction()) {
	first_d = &d;
      } else if (disj == NULL) {
	disj = new Disjunction();
	disj->add_disjunct(*first_d);
      }
      if (disj != NULL) {
	disj->add_disjunct(d);
      }
    }
  }
  if (disj != NULL) {
    return *disj;
  } else {
    return *first_d;
  }
}


/* Prints this object on the given stream. */
void Disjunction::print(std::ostream& os, const PredicateTable& predicates,
			const FunctionTable& functions,
			const TermTable& terms) const {
  if (size() == 1) {
    disjunct(0).print(os, predicates, functions, terms);
  } else {
    os << "(or";
    for (FormulaList::const_iterator fi = disjuncts_.begin();
	 fi != disjuncts_.end(); fi++) {
      os << ' ';
      (*fi)->print(os, predicates, functions, terms);
    }
    os << ")";
  }
}


/* ====================================================================== */
/* Exists */

/* Constructs an existentially quantified formula. */
Exists::Exists()
  : body_(&StateFormula::FALSE) {
  stft_ = STF_EXISTS;
  register_use(body_);
}


/* Deletes this existentially quantified state formula. */
Exists::~Exists() {
  unregister_use(body_);
}


/* Sets the body of this quantified formula. */
void Exists::set_body(const StateFormula& body) {
  if (&body != body_) {
    unregister_use(body_);
    body_ = &body;
    register_use(body_);
  }
}


/* Tests if this state formula holds in the given state. */
bool Exists::holds(const AtomSet& atoms, const ValueMap& values) const {
  throw Exception("Exists::holds erroneously called");
}


/* Returns an instantiation of this formula. */
const StateFormula& Exists::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  int n = arity();
  if (n == 0) {
    return body().instantiation(subst, problem);
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (int i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.terms().type(parameter(i)));
      if (arguments[i].empty()) {
	return FALSE;
      }
      next_arg.push_back(arguments[i].begin());
    }
    Disjunction* disj = NULL;
    const StateFormula* first_d = &FALSE;
    std::stack<const StateFormula*> disjuncts;
    disjuncts.push(&body().instantiation(args, problem));
    register_use(disjuncts.top());
    for (int i = 0; i < n; ) {
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameter(i), *next_arg[i]));
      const StateFormula& disjunct =
	disjuncts.top()->instantiation(pargs, problem);
      disjuncts.push(&disjunct);
      if (i + 1 == n) {
	if (disjunct.tautology()) {
	  if (disj == NULL) {
	    register_use(first_d);
	    unregister_use(first_d);
	  } else {
	    register_use(disj);
	    unregister_use(disj);
	  }
	  first_d = &TRUE;
	  break;
	} else if (!disjunct.contradiction()) {
	  if (first_d->contradiction()) {
	    first_d = &disjunct;
	  } else if (disj == NULL) {
	    disj = new Disjunction();
	    disj->add_disjunct(*first_d);
	  }
	  if (disj != NULL) {
	    disj->add_disjunct(disjunct);
	  }
	}
	for (int j = i; j >= 0; j--) {
	  if (j < i) {
	    unregister_use(disjuncts.top());
	  }
	  disjuncts.pop();
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
	register_use(disjuncts.top());
	i++;
      }
    }
    while (!disjuncts.empty()) {
      unregister_use(disjuncts.top());
      disjuncts.pop();
    }
    if (disj != NULL) {
      return *disj;
    } else {
      return *first_d;
    }
  }
}


/* Prints this object on the given stream. */
void Exists::print(std::ostream& os, const PredicateTable& predicates,
		   const FunctionTable& functions,
		   const TermTable& terms) const {
  if (parameters_.empty()) {
    body().print(os, predicates, functions, terms);
  } else {
    os << "(exists (";
    VariableList::const_iterator vi = parameters_.begin();
    terms.print_term(os, *vi);
    for (vi++; vi != parameters_.end(); vi++) {
      os << ' ';
      terms.print_term(os, *vi);
    }
    os << ") ";
    body().print(os, predicates, functions, terms);
    os << ")";
  }
}


/* ====================================================================== */
/* Forall */

/* Constructs a universally quantified formula. */
Forall::Forall()
  : body_(&StateFormula::TRUE) {
  stft_ = STF_FORALL;
  register_use(body_);
}


/* Deletes this universally quantified state formula. */
Forall::~Forall() {
  unregister_use(body_);
}


/* Sets the body of this quantified formula. */
void Forall::set_body(const StateFormula& body) {
  if (&body != body_) {
    unregister_use(body_);
    body_ = &body;
    register_use(body_);
  }
}


/* Tests if this state formula holds in the given state. */
bool Forall::holds(const AtomSet& atoms, const ValueMap& values) const {
  throw Exception("Forall::holds erroneously called");
}


/* Returns an instantiation of this formula. */
const StateFormula& Forall::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  int n = arity();
  if (n == 0) {
    return body().instantiation(subst, problem);
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (int i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.terms().type(parameter(i)));
      if (arguments[i].empty()) {
	return TRUE;
      }
      next_arg.push_back(arguments[i].begin());
    }
    Conjunction* conj = NULL;
    const StateFormula* first_c = &TRUE;
    std::stack<const StateFormula*> conjuncts;
    conjuncts.push(&body().instantiation(args, problem));
    register_use(conjuncts.top());
    for (int i = 0; i < n; ) {
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameter(i), *next_arg[i]));
      const StateFormula& conjunct =
	conjuncts.top()->instantiation(pargs, problem);
      conjuncts.push(&conjunct);
      if (i + 1 == n) {
	if (conjunct.contradiction()) {
	  if (conj == NULL) {
	    register_use(first_c);
	    unregister_use(first_c);
	  } else {
	    register_use(conj);
	    unregister_use(conj);
	  }
	  break;
	} else if (!conjunct.tautology()) {
	  if (first_c->tautology()) {
	    first_c = &conjunct;
	  } else if (conj == NULL) {
	    conj = new Conjunction();
	    conj->add_conjunct(*first_c);
	  }
	  if (conj != NULL) {
	    conj->add_conjunct(conjunct);
	  }
	}
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
    if (conj != NULL) {
      return *conj;
    } else {
      return *first_c;
    }
  }
}


/* Prints this object on the given stream. */
void Forall::print(std::ostream& os, const PredicateTable& predicates,
		   const FunctionTable& functions,
		   const TermTable& terms) const {
  if (parameters_.empty()) {
    body().print(os, predicates, functions, terms);
  } else {
    os << "(forall (";
    VariableList::const_iterator vi = parameters_.begin();
    terms.print_term(os, *vi);
    for (vi++; vi != parameters_.end(); vi++) {
      os << ' ';
      terms.print_term(os, *vi);
    }
    os << ") ";
    body().print(os, predicates, functions, terms);
    os << ")";
  }
}

} /* end of namespace */
