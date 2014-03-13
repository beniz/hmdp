/* -*-C++-*- */
/*
 * Functions.
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

/**
 * - Added resources as bounded pddl functions.
 * \author Emmanuel Benazera beniz@droidnik.fr
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <config.h>
#include "types.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace ppddl_parser
{

/* Function index. */
typedef int Function;

/* Name of number type. */
const std::string NUMBER_NAME("number");


/* ====================================================================== */
/* FunctionSet */

/* Set of function declarations. */
class FunctionSet : public std::unordered_set<Function> {
};


/* ====================================================================== */
/* FunctionTable */

/*
 * Function table.
 */
class FunctionTable {
public:
  /* Constructs an empty function table. */
  FunctionTable() : first_(next) {}

  /* Adds a function with the given name to this table and returns the
     function. */
  Function add_function(const std::string& name);

  /* Adds a resource with the given name to this table and returns
     the resource. */
  Function add_resource(const std::string& name, double low, double high);

  /* Returns the function with the given name.  If no function with
     the given name exists, false is returned in the second part of
     the result. */
  std::pair<Function, bool> find_function(const std::string& name) const;

  /* Returns the first function of this function table. */
  Function first_function() const { return first_; }

  /* Returns the last function of this function table. */
  Function last_function() const { return first_ + names_.size() - 1; }

  /* Adds a parameter with the given type to the given function. */
  void add_parameter(Function function, Type type) {
    parameters_[function - first_].push_back(type);
  }

  /* Returns the name of the given function. */
  const std::string& name(Function function) const {
    return names_[function - first_];
  }

  /* Returns the arity of the given function. */
  size_t arity(Function function) const {
    return parameters_[function - first_].size();
  }

  /* Returns the ith parameter type of the given function. */
  Type parameter(Function function, size_t i) const {
    return parameters_[function - first_][i];
  }

  /* Makes the given function dynamic. */
  void make_dynamic(Function function) {
    static_functions_.erase(function);
  }

  /* Tests if the given function is static. */
  bool static_function(Function function) const {
    return static_functions_.find(function) != static_functions_.end();
  }

  /* Returns the number of resources among functions */
  size_t getNResources () const { return resources_.size (); }

  /* Returns the map of resources */
  std::vector<std::pair<std::string, std::pair<double,double> > >* getResources ()
  { return &resources_; }

  const std::pair<double,double>& getResourceBounds (const std::string &rscname) const;

  /* Tests if a function is a resource */
  bool isResource (const std::string &name) const;
  bool isResource (Function function) const 
  { return isResource (name (function)); }

  /* Returns function table size. */
  size_t getSize () const { return names_.size (); }

  /* Prints the given function on the given stream. */
  void print_function(std::ostream& os, Function function) const;

protected:
  /* Next function. */
  static Function next;

  /* The first function. */
  Function first_;
  /* Function names. */
  std::vector<std::string> names_;
  /* Mapping of function names to functions. */
  std::map<std::string, Function> functions_;
  /* Function parameters. */
  std::vector<TypeList> parameters_;
  /* Static functions. */
  FunctionSet static_functions_;
  /* Resources (among functions) bounds */
  std::vector <std::pair<std::string, std::pair<double,double> > > resources_;
};

} /* end of namespace */

#endif /* FUNCTIONS_H */
