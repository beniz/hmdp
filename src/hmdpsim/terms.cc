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
#include "terms.h"

namespace ppddl_parser
{

/* ====================================================================== */
/* TermTable */

/* Adds an object with the given name and type to this term table
   and returns the object. */
Object TermTable::add_object(const std::string& name, Type type) {
  Object object = last_object() + 1;
  names_.push_back(name);
  objects_.insert(std::make_pair(name, object));
  object_types_.push_back(type);
  return object;
}


/* Adds a variable with the given name and type to this term table
   and returns the variable. */
Variable TermTable::add_variable(Type type) {
  Variable variable = last_variable() - 1;
  variable_types_.push_back(type);
  return variable;
}


/* Returns the object with the given name. If no object with the
   given name exists, false is returned in the second part of the
   result. */
std::pair<Object, bool> TermTable::find_object(const std::string& name) const {
  std::map<std::string, Object>::const_iterator oi = objects_.find(name);
  if (oi != objects_.end()) {
    return std::make_pair((*oi).second, true);
  } else if (parent_ != NULL) {
    return parent_->find_object(name);
  } else {
    return std::make_pair(0, false);
  }
}


/* Sets the type of the given term. */
void TermTable::set_type(Term term, Type type) {
  if (is_object(term)) {
    object_types_[term - first_object()] = type;
  } else {
    variable_types_[first_variable() - term] = type;
  }
}


/* Returns the type of the given term. */
Type TermTable::type(Term term) const {
  if (is_object(term)) {
    if (Object(term) < first_object()) {
      return parent_->type(term);
    } else {
      return object_types_[term - first_object()];
    }
  } else {
    if (term > first_variable()) {
      return parent_->type(term);
    } else {
      return variable_types_[first_variable() - term];
    }
  }
}


/* Prints the given term on the given stream. */
void TermTable::print_term(std::ostream& os, Term term) const {
  if (is_object(term)) {
    if (Object(term) < first_object()) {
      parent_->print_term(os, term);
    } else {
      os << names_[term - first_object()];
    }
  } else {
    os << "?v" << -term;
  }
}

void TermTable::print_term (std::string &str, Term term) const
{
  if (is_object(term)) {
    if (Object(term) < first_object()) {
      parent_->print_term(str, term);
    } else {
      str += names_[term - first_object()];
    }
  } 
  /* else {
     str += "?v" + -term;
     } */
}

} /* end of namespace */
