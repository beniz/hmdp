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
#include "predicates.h"

namespace ppddl_parser
{

/* ====================================================================== */
/* PredicateTable */

/* Next predicate. */
Predicate PredicateTable::next = 0;


/* Adds a predicate with the given name to this table and returns
   the predicate. */
Predicate PredicateTable::add_predicate(const std::string& name) {
  Predicate predicate = last_predicate() + 1;
  names_.push_back(name);
  predicates_.insert(std::make_pair(name, predicate));
  parameters_.push_back(TypeList());
  static_predicates_.insert(predicate);
  next++;
  return predicate;
}


/* Returns the predicate with the given name.  If no predicate with
   the given name exists, false is returned in the second part of
   the result. */
std::pair<Predicate, bool>
PredicateTable::find_predicate(const std::string& name) const {
  std::map<std::string, Predicate>::const_iterator pi = predicates_.find(name);
  if (pi != predicates_.end()) {
    return std::make_pair((*pi).second, true);
  } else {
    return std::make_pair(0, false);
  }
}


/* Prints the given predicate on the given stream. */
void PredicateTable::print_predicate(std::ostream& os,
				     Predicate predicate) const {
  os << names_[predicate - first_];
}

void PredicateTable::print_predicate (std::string &str,
				      Predicate predicate) const 
{
  str += names_[predicate - first_];
}

} /* end of namespace */
