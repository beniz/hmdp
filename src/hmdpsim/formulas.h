/* -*-C++-*- */
/*
 * Formulas.
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
#ifndef FORMULAS_H
#define FORMULAS_H

#include <config.h>
#include "predicates.h"
#include "terms.h"
#include <iostream>
#include <set>
#include <vector>
#include <unordered_set>

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

namespace ppddl_parser
{

class FunctionTable;
class Expression;
class ValueMap;
class Problem;


/* ====================================================================== */
/* AtomSet */

class Atom;

/*
 * A set of atoms.
 */
class AtomSet : public std::unordered_set<const Atom*> {
};


/* ====================================================================== */
/* StateFormula */

enum StfType {
  STF, STF_CST, STF_ATOM, STF_EQ, STF_CMP, STF_NEG, STF_CONJ, STF_DISJ,
  STF_EXISTS, STF_FORALL
};

/*
 * A state formula.
 */
class StateFormula {
public:
  /* A formula true in every state. */
  static const StateFormula& TRUE;
  /* A formula false in every state. */
  static const StateFormula& FALSE;

  /* Register use of the given state formula. */
  static void register_use(const StateFormula* f) {
    if (f != NULL) {
      f->ref_count_++;
    }
  }

  /* Unregister use of the given state formula. */
  static void unregister_use(const StateFormula* f) {
    if (f != NULL) {
      f->ref_count_--;
      if (f->ref_count_ == 0) {
	delete f;
      }
    }
  }

  /* Deletes this state formula. */
  virtual ~StateFormula() {}

  /* Tests if this state formula is a tautology. */
  bool tautology() const { return this == &TRUE; }

  /* Tests if this state formula is a contradiction. */
  bool contradiction() const { return this == &FALSE; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const = 0;
  
  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const = 0;

  StfType getType () const { return stft_; }

protected:
  StfType stft_;  /**< type of the state formula, for external manipulations */
  
  /* Constructs a state formula. */
  StateFormula() : ref_count_(0) {}
  
private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* FormulaList */

/*
 * List of formulas.
 */
class FormulaList : public std::vector<const StateFormula*> {
};


/* ====================================================================== */
/* Constant */

/*
 * A state formula with a constant truth value.
 */
class Constant : public StateFormula {
public:
  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const Constant& instantiation(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Constant representing true. */
  static const Constant TRUE_;
  /* Constant representing false. */
  static const Constant FALSE_;

  /* Value of this constant. */
  bool value_;

  /* Constructs a constant state formula. */
  Constant(bool value);

  friend class StateFormula;
};


/* ====================================================================== */
/* Atom */

/*
 * An atomic state formula.
 */
class Atom : public StateFormula {
public:
  /* Returns an atomic state formula with the given predicate and terms. */
  static const Atom& make_atom(Predicate predicate, const TermList& terms);

  /* Deletes this atomic state formula. */
  virtual ~Atom();

  /* Returns the predicate of this atomic state formula. */
  Predicate predicate() const { return predicate_; }

  /* Returns the number of terms of this atomic state formula. */
  size_t arity() const { return terms_.size(); }

  /* Returns the ith term of this atomic state formula. */
  Term term(size_t i) const { return terms_[i]; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns this atom subject to the given substitution. */
  const Atom& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

  void print (std::string &str,  const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

  /* Prints this object on the given stream in XML. */
  void printXML(std::ostream& os, const PredicateTable& predicates,
		const FunctionTable& functions,
		const TermTable& terms) const;

private:
  /* Less-than comparison function object for atoms. */
  class AtomLess
    : public std::binary_function<const Atom*, const Atom*, bool> {
    /* Comparison function. */
  public:
    bool operator()(const Atom* a1, const Atom* a2) const;
  };

  /* A table of atomic state formulas. */
  class AtomTable : public std::set<const Atom*, AtomLess> {
  };

  /* Table of atomic state formulas. */
  static AtomTable atoms;

  /* Predicate of this atomic state formula. */
  Predicate predicate_;
  /* Terms of this atomic state formula. */
  TermList terms_;

  /* Constructs an atomic state formula with the given predicate. */
  Atom(Predicate predicate) : predicate_(predicate) { stft_ = STF_ATOM; }

  /* Adds a term to this atomic state formula. */
  void add_term(Term term) { terms_.push_back(term); }
};


/* ====================================================================== */
/* Equality */

/*
 * Equality formula.
 */
class Equality : public StateFormula {
public:
  /* Constructs an equality formula. */
  Equality(Term term1, Term term2) : term1_(term1), term2_(term2) 
  { stft_ = STF_EQ; }

  /* Returns the first term of this equality formula. */
  Term term1() const { return term1_; }

  /* Returns the second term of this equality formula. */
  Term term2() const { return term2_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* First term of equality formula. */
  Term term1_;
  /* Second term of equality formula. */
  Term term2_;
};


/* ====================================================================== */
/* Comparison */

/*
 * A comparison.
 */
class Comparison : public StateFormula {
public:
  /* Comparison predicates. */
  typedef enum { LT_CMP, LE_CMP, EQ_CMP, GE_CMP, GT_CMP } CmpPredicate;

  /* Constructs a comparison. */
  Comparison(CmpPredicate predicate,
	     const Expression& expr1, const Expression& expr2);

  /* Deletes this comparison. */
  virtual ~Comparison();

  /* Returns the predicate of this comparison. */
  CmpPredicate predicate() const { return predicate_; }

  /* Returns the first expression of this comparison. */
  const Expression& expr1() const { return *expr1_; }

  /* Returns the second expression of this comparison. */
  const Expression& expr2() const { return *expr2_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Comparison predicate. */
  CmpPredicate predicate_;
  /* The first expression. */
  const Expression* expr1_;
  /* The second expression. */
  const Expression* expr2_;
};


/* ====================================================================== */
/* Negation */

/*
 * A negated state formula.
 */
class Negation : public StateFormula {
public:
  /* Returns the negation of the given state formula. */
  static const StateFormula& make_negation(const StateFormula& formula);

  /* Deletes this negation. */
  virtual ~Negation();

  /* Returns the negand of this negation. */
  const StateFormula& negand() const { return *negand_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The negand of this negation. */
  const StateFormula* negand_;

  /* Constructs a negated state formula. */
  Negation(const StateFormula& negand);
};


/* ====================================================================== */
/* Conjunction */

/*
 * A conjunction of state formulas.
 */
class Conjunction : public StateFormula {
public:
  Conjunction () { stft_ = STF_CONJ; }

  /* Deletes this conjunction. */
  virtual ~Conjunction();

  /* Adds a conjunct to this conjunction. */
  void add_conjunct(const StateFormula& conjunct);

  /* Returns the number of conjuncts of this conjunction. */
  size_t size() const { return conjuncts_.size(); }

  /* Returns the ith conjunct of this conjunction. */
  const StateFormula& conjunct(size_t i) const { return *conjuncts_[i]; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The conjuncts. */
  FormulaList conjuncts_;
};


/* ====================================================================== */
/* Disjunction */

/*
 * A disjunction of state formulas.
 */
class Disjunction : public StateFormula {
public:
  Disjunction () { stft_ = STF_DISJ; }

  /* Deletes this disjunction. */
  virtual ~Disjunction();

  /* Adds a disjunct to this disjunction. */
  void add_disjunct(const StateFormula& disjunct);

  /* Returns the number of disjuncts of this disjunction. */
  size_t size() const { return disjuncts_.size(); }

  /* Returns the ith disjunct of this disjunction. */
  const StateFormula& disjunct(size_t i) const { return *disjuncts_[i]; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The disjuncts. */
  FormulaList disjuncts_;
};


/* ====================================================================== */
/* Exists */

/*
 * Existentially quantified state formula.
 */
class Exists : public StateFormula {
public:
  /* Constructs an existentially quantified formula. */
  Exists();

  /* Deletes this existentially quantified state formula. */
  virtual ~Exists();

  /* Adds a quantified variable to this quantified formula. */
  void add_parameter(Variable parameter) { parameters_.push_back(parameter); }

  /* Sets the body of this quantified formula. */
  void set_body(const StateFormula& body);

  /* Returns the arity of this quantified formula. */
  size_t arity() const { return parameters_.size(); }

  /* Returns the ith quantified variable of this quantified formula. */
  Variable parameter(size_t i) const { return parameters_[i]; }

  /* Returns the body of this quantified formula. */
  const StateFormula& body() const { return *body_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Quanitfied variables. */
  VariableList parameters_;
  /* The quantified formula. */
  const StateFormula* body_;
};


/* ====================================================================== */
/* Forall */

/*
 * Universally quantified state formula.
 */
class Forall : public StateFormula {
public:
  /* Constructs a universally quantified formula. */
  Forall();

  /* Deletes this universally quantified state formula. */
  virtual ~Forall();

  /* Adds a quantified variable to this quantified formula. */
  void add_parameter(Variable parameter) { parameters_.push_back(parameter); }

  /* Sets the body of this quantified formula. */
  void set_body(const StateFormula& body);

  /* Returns the arity of this quantified formula. */
  size_t arity() const { return parameters_.size(); }

  /* Returns the ith quantified variable of this quantified formula. */
  Variable parameter(size_t i) const { return parameters_[i]; }

  /* Returns the body of this quantified formula. */
  const StateFormula& body() const { return *body_; }

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const AtomSet& atoms, const ValueMap& values) const;

  /* Returns an instantiation of this state formula. */
  virtual const StateFormula& instantiation(const SubstitutionMap& subst,
					    const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Quanitfied variables. */
  VariableList parameters_;
  /* The quantified formula. */
  const StateFormula* body_;
};


/* ====================================================================== */
/* AtomList */

/*
 * List of atoms.
 */
class AtomList : public std::vector<const Atom*> {
};

} /* end of namespace */

#endif /* FORMULAS_H */
