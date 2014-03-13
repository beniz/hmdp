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
#include "domains.h"

namespace ppddl_parser
{

/* ====================================================================== */
/* Domain */

/* Table of defined domains. */
Domain::DomainMap Domain::domains = Domain::DomainMap();


/* Returns a const_iterator pointing to the first domain. */
Domain::DomainMap::const_iterator Domain::begin() {
  return domains.begin();
}


/* Returns a const_iterator pointing beyond the last domain. */
Domain::DomainMap::const_iterator Domain::end() {
  return domains.end();
}


/* Returns the domain with the given name, or NULL it is undefined. */
const Domain* Domain::find(const std::string& name) {
  DomainMap::const_iterator di = domains.find(name);
  return (di != domains.end()) ? (*di).second : NULL;
}


/* Removes all defined domains. */
void Domain::clear() {
  DomainMap::const_iterator di = begin();
  while (di != end()) {
    delete (*di).second;
    di = begin();
  }
  domains.clear();
}

/* Number of domains. */
size_t Domain::size()
{
  return domains.size();
}

/* Constructs an empty domain with the given name. */
Domain::Domain(const std::string& name)
  : name_(name) {
  const Domain* d = find(name);
  if (d != NULL) {
    delete d;
  }
  domains[name] = this;
}


/* Deletes a domain. */
Domain::~Domain() {
  domains.erase(name());
}


/* Adds the given action to this domain. */
void Domain::add_action(const ActionSchema& action) {
  actions_.insert(std::make_pair(action.name(), &action));
}


/* Returns the action with the given name, or NULL if there is no
   action with the given name. */
const ActionSchema* Domain::find_action(const std::string& name) const {
  ActionSchemaMap::const_iterator ai = actions_.find(name);
  return (ai != actions_.end()) ? (*ai).second : NULL;
}


/* Fills the provided object list with constants that are compatible
   with the given type. */
void Domain::compatible_constants(ObjectList& constants, Type type) const {
  Object last = terms().last_object();
  for (Object i = terms().first_object(); i <= last; i++) {
    if (types().subtype(terms().type(i), type)) {
      constants.push_back(i);
    }
  }
}


/* Fills the provided lists with instantiated actions. */
void Domain::instantiated_actions(ActionList& actions,
				  const Problem& problem) const {
  for (ActionSchemaMap::const_iterator ai = actions_.begin();
       ai != actions_.end(); ai++) {
    (*ai).second->instantiations(actions, problem);
  }
  std::cout << "[Info]: ppddl_parser::domain: actions successfully instantiated\n";
}


/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d) {
  os << "name: " << d.name();
  os << std::endl << "types:";
  for (Type i = d.types().first_type(); i <= d.types().last_type(); i++) {
    os << std::endl << "  ";
    d.types().print_type(os, i);
    bool first = true;
    for (Type j = d.types().first_type(); j <= d.types().last_type(); j++) {
      if (i != j && d.types().subtype(i, j)) {
	if (first) {
	  os << " <:";
	  first = false;
	}
	os << ' ';
	d.types().print_type(os, j);
      }
    }
  }
  os << std::endl << "constants:";
  for (Object i = d.terms().first_object();
       i <= d.terms().last_object(); i++) {
    os << std::endl << "  ";
    d.terms().print_term(os, i);
    os << " - ";
    d.types().print_type(os, d.terms().type(i));
  }
  os << std::endl << "predicates:";
  for (Predicate i = d.predicates().first_predicate();
       i <= d.predicates().last_predicate(); i++) {
    os << std::endl << "  (";
    d.predicates().print_predicate(os, i);
    size_t arity = d.predicates().arity(i);
    for (size_t j = 0; j < arity; j++) {
      os << " ?v - ";
      d.types().print_type(os, d.predicates().parameter(i, j));
    }
    os << ")";
    if (d.predicates().static_predicate(i)) {
      os << " <static>";
    }
  }
  os << std::endl << "functions:";
  for (Function i = d.functions().first_function();
       i <= d.functions().last_function(); i++) {
    os << std::endl << "  (";
    d.functions().print_function(os, i);
    size_t arity = d.functions().arity(i);
    for (size_t j = 0; j < arity; j++) {
      os << " ?v - ";
      d.types().print_type(os, d.functions().parameter(i, j));
    }
    os << ") - " << NUMBER_NAME;
    if (d.functions().static_function(i)) {
      os << " <static>";
    }
  }
  Domain& nc_d = const_cast<Domain&> (d);
  os << std::endl << "resources:";
  std::vector<std::pair<std::string, std::pair<double,double> > >::iterator it;
  for (it=nc_d.functions().getResources()->begin ();
       it != nc_d.functions().getResources()->end (); it++)
    os << std::endl << "  (" << (*it).first
       << " [" << (*it).second.first << "," << (*it).second.second
       << "]";
  os << ")" << std::endl << "actions:";
  for (ActionSchemaMap::const_iterator ai = d.actions_.begin();
       ai != d.actions_.end(); ai++) {
    os << std::endl;
    (*ai).second->print(os, d.predicates(), d.functions(), d.terms());
  }
  return os;
}

} /* end of namespace */
