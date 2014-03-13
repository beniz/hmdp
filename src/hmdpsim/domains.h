/* -*-C++-*- */
/*
 * Domain descriptions.
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
#ifndef DOMAINS_H
#define DOMAINS_H

#include <config.h>
#include "actions.h"
#include "functions.h"
#include "predicates.h"
#include "terms.h"
#include "types.h"
#include "requirements.h"
#include <iostream>
#include <map>
#include <string>

namespace ppddl_parser
{

class Problem;


/* ====================================================================== */
/* Domain */

/*
 * Domain definition.
 */
class Domain {
public:
  /* Table of domain definitions. */
  class DomainMap : public std::map<std::string, const Domain*> {
  };

  /* Requirements for this domain. */
  Requirements requirements;

  /* Returns a const_iterator pointing to the first domain. */
  static DomainMap::const_iterator begin();

  /* Returns a const_iterator pointing beyond the last domain. */
  static DomainMap::const_iterator end();

  /* Returns the domain with the given name, or NULL it is undefined. */
  static const Domain* find(const std::string& name);

  /* Removes all defined domains. */
  static void clear();

  /* Number of domains. */
  static size_t size();

  /* Constructs an empty domain with the given name. */
  Domain(const std::string& name);

  /* Deletes a domain. */
  ~Domain();

  /* Returns the name of this domain. */
  const std::string& name() const { return name_; }

  /* Returns the type table of this domain. */
  TypeTable& types() { return types_; }

  /* Returns the type table of this domain. */
  const TypeTable& types() const { return types_; }

  /* Returns the predicate table of this domain. */
  PredicateTable& predicates() { return predicates_; }

  /* Returns the predicate table of this domain. */
  const PredicateTable& predicates() const { return predicates_; }
  
  /* Returns the function table of this domain. */
  FunctionTable& functions() { return functions_; }

  /* Returns the function table of this domain. */
  const FunctionTable& functions() const { return functions_; }
  
  /* Returns the term table of this domain. */
  TermTable& terms() { return terms_; }

  /* Returns the term table of this domain. */
  const TermTable& terms() const { return terms_; }

  /* Adds the given action to this domain. */
  void add_action(const ActionSchema& action);

  /* Returns the action with the given name, or NULL if there is no
     action with the given name. */
  const ActionSchema* find_action(const std::string& name) const;

  /* Fills the provided object list with constants that are compatible
     with the given type. */
  void compatible_constants(ObjectList& constants, Type type) const;

  /* Fills the provided list with instantiated actions. */
  void instantiated_actions(ActionList& actions, const Problem& problem) const;

private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Name of this domain. */
  std::string name_;
  /* Domain types. */
  TypeTable types_;
  /* Domain predicates. */
  PredicateTable predicates_;
  /* Domain functions. */
  FunctionTable functions_;
  /* Domain terms. */
  TermTable terms_;
  /* Domain actions. */
  ActionSchemaMap actions_;

  friend std::ostream& operator<<(std::ostream& os, const Domain& d);
};

/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d);

} /* end of namespace */

#endif /* DOMAINS_H */
