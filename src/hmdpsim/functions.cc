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
#include "functions.h"

namespace ppddl_parser
{

/* ====================================================================== */
/* FunctionTable */

/* Next function. */
Function FunctionTable::next = 0;


/* Adds a function with the given name to this table and returns the
   function. */
Function FunctionTable::add_function(const std::string& name) {
  Function function = last_function() + 1;
  names_.push_back(name);
  functions_.insert(std::pair<std::string,Function>(name, function));
  parameters_.push_back(TypeList());
  static_functions_.insert(function);
  next++;
  return function;
}

/* a resource is inserted in both the function and the resource table. */
Function FunctionTable::add_resource(const std::string& name, double low, double high) {
  Function resource = last_function() + 1;
  names_.push_back(name);
  std::pair<double, double> bounds = std::pair<double,double> (low, high);
  //resources_.insert(std::make_pair(name, bounds));
  resources_.push_back (std::make_pair(name, bounds));
  functions_.insert(std::make_pair(name, resource));
  parameters_.push_back(TypeList());
  static_functions_.insert(resource);
  next++;
  return resource;
}

/* Returns the function with the given name.  If no function with
   the given name exists, false is returned in the second part of
   the result. */
std::pair<Function, bool>
FunctionTable::find_function(const std::string& name) const {
  std::map<std::string, Function>::const_iterator fi = functions_.find(name);
  if (fi != functions_.end()) {
    return std::make_pair((*fi).second, true);
  } else {
    return std::make_pair(0, false);
  }
}

/* Tests if a function is a resource. */
bool FunctionTable::isResource (const std::string &name) const
{
  std::vector <std::pair<std::string, std::pair<double,double> > >::const_iterator it;
  for (it=resources_.begin (); it != resources_.end (); it++)
    {
      if (name == (*it).first)
	return true;
    }
  return false;
}

const std::pair<double,double>& FunctionTable::getResourceBounds (const std::string &rscname) const
{
  for (size_t i=0; i<resources_.size (); i++)
    {
      if (resources_[i].first == rscname)
	return resources_[i].second;
    }
  std::cout << "[Error]:FunctionTable::getResource: resource not found: " 
	    << rscname << ". Exiting.\n";
  exit (-1);
}

/* Prints the given function on the given stream. */
void FunctionTable::print_function(std::ostream& os, Function function) const {
  
  //debug
  /* os << "function: " << function << std::endl;
     os << "first_: " << first_ << std::endl;
     os << "function - first_: " << function - first_ << std::endl;
     os << "names size: " << names_.size() << std::endl; */
  //debug

  os << names_[function - first_];
}

} /* end of namespace */
