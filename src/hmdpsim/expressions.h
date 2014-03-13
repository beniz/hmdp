/* -*-C++-*- */
/*
 * Expressions.
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
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <config.h>
#include "functions.h"
#include "terms.h"
#include "rational.h"
#include "distributionTypes.h"
#include <iostream>
#include <set>
#include <unordered_map>

namespace ppddl_parser
{

class Problem;


/* ====================================================================== */
/* ValueMap */

class Application;

/*
 * Mapping from function applications to values.
 */
class ValueMap : public std::unordered_map<const Application*,Rational> {
};


/* ====================================================================== */
/* Expression. */

enum ExprType {
  EXPR, EXPR_VAL, EXPR_APP, EXPR_ADD, EXPR_SUB, EXPR_MULT, EXPR_DIV, EXPR_PROBAPP
};

/*
 * An abstract expression.
 */
class Expression {
public:
  /* Register use of the given expression. */
  static void register_use(const Expression* e) {
    if (e != NULL) {
      e->ref_count_++;
    }
  }

  /* Unregister use of the given expression. */
  static void unregister_use(const Expression* e) {
    if (e != NULL) {
      e->ref_count_--;
      if (e->ref_count_ == 0) {
	delete e;
      }
    }
  }

  /* Deletes this expression. */
  virtual ~Expression() {}

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const = 0;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const = 0;

  ExprType getType () const { return exprt_; }

protected:
  /* Expression type, for external manipulations */
  ExprType exprt_;
  /* Constructs an expression. */
  Expression() : ref_count_(0) {}

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* Value */

/*
 * A constant value.
 */
class Value : public Expression {
public:
  /* Constructs a constant value. */
  Value(const Rational& value) : value_(value) { exprt_ = EXPR_VAL; }

  /* Returns the value of this expression. */
  const Rational& value() const { return value_; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Value& instantiation(const SubstitutionMap& subst,
				     const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The value. */
  Rational value_;
};


/* ====================================================================== */
/* Application */

/*
 * A function application.
 */
class Application : public Expression {
public:
  /* Returns a function application with the given function and terms. */
  static const Application& make_application(Function function,
					     const TermList& terms);

  /* Deletes this function application. */
  virtual ~Application();

  /* Returns the function of this function application. */
  Function function() const { return function_; }
  
  /* Returns the number of terms of this function application. */
  size_t arity() const { return terms_.size(); }

  /* Returns the ith term of this function application. */
  Term term(size_t i) const { return terms_[i]; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this application subject to the given substitution. */
  const Application& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

  /* Prints this object on the given stream in XML. */
  void printXML(std::ostream& os, const FunctionTable& functions,
		const TermTable& terms) const;

private:
  /* Less-than comparison function object for function applications. */
  class ApplicationLess
    : public std::binary_function<const Application*, const Application*,
				  bool> {
    /* Comparison function. */
  public:
    bool operator()(const Application* a1, const Application* a2) const;
  };
  
  /* A table of function applications. */
  class ApplicationTable : public std::set<const Application*, ApplicationLess> {
  };

  /* Table of function applications. */
  static ApplicationTable applications;

  /* Function of this function application. */
  Function function_;
  /* Terms of this function application. */
  TermList terms_;

  /* Constructs a function application with the given function. */
  Application(Function function) : function_(function) { exprt_ = EXPR_APP; }

  /* Adds a term to this function application. */
  void add_term(Term term) { terms_.push_back(term); }
};


/* ====================================================================== */
/* Addition */

/*
 * An addition.
 */
class Addition : public Expression {
public:
  /* Constructs an addition. */
  Addition(const Expression& term1, const Expression& term2);

  /* Deletes this addition. */
  virtual ~Addition();

  /* Returns the first term of this addition. */
  const Expression& term1() const { return *term1_; }

  /* Returns the second term of this addition. */
  const Expression& term2() const { return *term2_; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The first term. */
  const Expression* term1_;
  /* The second term. */
  const Expression* term2_;
};


/* ====================================================================== */
/* Subtraction */

/*
 * A subtraction.
 */
class Subtraction : public Expression {
public:
  /* Constructs a subtraction. */
  Subtraction(const Expression& term1, const Expression& term2);

  /* Deletes this subtraction. */
  virtual ~Subtraction();

  /* Returns the first term of this subtraction. */
  const Expression& term1() const { return *term1_; }

  /* Returns the second term of this subtraction. */
  const Expression& term2() const { return *term2_; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The first term. */
  const Expression* term1_;
  /* The second term. */
  const Expression* term2_;
};


/* ====================================================================== */
/* Multiplication */

/*
 * A multiplication.
 */
class Multiplication : public Expression {
public:
  /* Constructs a multiplication. */
  Multiplication(const Expression& factor1, const Expression& factor2);

  /* Deletes this multiplication. */
  virtual ~Multiplication();

  /* Returns the first factor of this multiplication. */
  const Expression& factor1() const { return *factor1_; }

  /* Returns the second factor of this multiplication. */
  const Expression& factor2() const { return *factor2_; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The first factor. */
  const Expression* factor1_;
  /* The second factor. */
  const Expression* factor2_;
};


/* ====================================================================== */
/* Division */

/*
 * A division.
 */
class Division : public Expression {
public:
  /* Constructs a division. */
  Division(const Expression& factor1, const Expression& factor2);

  /* Deletes this division. */
  virtual ~Division();

  /* Returns the first factor of this division. */
  const Expression& factor1() const { return *factor1_; }

  /* Returns the second factor of this division. */
  const Expression& factor2() const { return *factor2_; }

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The first factor. */
  const Expression* factor1_;
  /* The second factor. */
  const Expression* factor2_;
};

/* ====================================================================== */
/* Probabilistic Application */
class ProbabilityDistributionFunc;

class ProbabilisticApplication : public Expression
{
public:
  /* Constructs a probabilistic function application with the given function
     and probability distribution terms. */
  ProbabilisticApplication (distributionType dt,
			    const Expression &mean, 
			    const Expression &stddev);

  ProbabilisticApplication (distributionType dt,
			    const Expression &mean,
			    const Expression &stddev,
			    disczType dzt,
			    const Expression &arg1,
			    const Expression &arg2);

  /* destructor */
  ~ProbabilisticApplication ();

  /* Returns this probabilistic application subject to the given substitution. */
  const ProbabilisticApplication& substitution (const SubstitutionMap &subst) const;

   /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Returns the value of this expression in the given state. */
  virtual Rational value(const ValueMap& values) const { /* NOT APPLICABLE */
    Rational rat (0, 1); return rat;};

   /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

  const ProbabilityDistributionFunc* getProbFunc () const { return pdfunc_; }

private:
  ProbabilityDistributionFunc *pdfunc_;

};


} /* end of namespace */

#endif /* EXPRESSIONS_H */
