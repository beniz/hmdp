/* -*-C++-*- */
/*
 * Terms.
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
#ifndef TERMS_H
#define TERMS_H

#include <config.h>
#include "types.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

namespace ppddl_parser
{

/* Term index. */
typedef int Term;
/* Object index. */
typedef size_t Object;
/* Variable index. */
typedef int Variable;

/* The "null" term. */
const Term NULL_TERM = 0;

/* Tests if the given term is an object. */
inline bool is_object(Term term) { return term > 0; }

/* Tests if the given term is a variable. */
inline bool is_variable(Term term) { return term < 0; }


/* ====================================================================== */
/* SubstitutionMap */

/*
 * Variable substitutions.
 */
class SubstitutionMap : public std::unordered_map<Variable,Object> {
};


/* ====================================================================== */
/* TermList */

/*
 * List of terms.
 */
class TermList : public std::vector<Term> {
};


/* ====================================================================== */
/* ObjectList */

/*
 * List of objects.
 */
class ObjectList : public std::vector<Object> {
};


/* ====================================================================== */
/* VariableList */

/*
 * List of variables.
 */
class VariableList : public std::vector<Variable> {
};


/* ====================================================================== */
/* TermTable */

/*
 * Term table.
 */
class TermTable {
public:
  /* Constructs an empty term table. */
  TermTable() : parent_(NULL) {}

  /* Constructs a term table extending the given term table. */
  TermTable(const TermTable& parent) : parent_(&parent) {}

  /* Adds an object with the given name and type to this term table
     and returns the object. */
  Object add_object(const std::string& name, Type type);

  /* Adds a variable with the given name and type to this term table
     and returns the variable. */
  Variable add_variable(Type type);

  /* Returns the object with the given name. If no object with the
     given name exists, false is returned in the second part of the
     result. */
  std::pair<Object, bool> find_object(const std::string& name) const;

  /* Returns the first object of this term table. */
  Object first_object() const {
    return (parent_ != NULL) ? parent_->last_object() + 1 : 1;
  }

  /* Returns the last object of this term table. */
  Object last_object() const { return first_object() + names_.size() - 1; }

  /* Returns the first variable of this term table. */
  Variable first_variable() const {
    return (parent_ != NULL) ? parent_->last_variable() - 1 : -1;
  }

  /* Returns the last variable of this term table. */
  Variable last_variable() const {
    return first_variable() - variable_types_.size() + 1;
  }

  /* Sets the type of the given term. */
  void set_type(Term term, Type type);

  /* Returns the type of the given term. */
  Type type(Term term) const;

  /* Prints the given term on the given stream. */
  void print_term(std::ostream& os, Term term) const;

  void print_term (std::string &str, Term term) const;

private:
  /* Parent term table. */
  const TermTable* parent_;
  /* Object names. */
  std::vector<std::string> names_;
  /* Mapping of object names to objects. */
  std::map<std::string, Object> objects_;
  /* Object types. */
  TypeList object_types_;
  /* Variable types. */
  TypeList variable_types_;
};

} /* end of namespace */

#endif /* TERMS_H */
