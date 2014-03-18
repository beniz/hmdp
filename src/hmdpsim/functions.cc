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

/* a continuous variable is inserted in both the function and the continuous variable table. */
Function FunctionTable::add_cvariable(const std::string& name, double low, double high) {
  Function cvariable = last_function() + 1;
  names_.push_back(name);
  std::pair<double, double> bounds = std::pair<double,double> (low, high);
  cvariables_.push_back (std::make_pair(name, bounds));
  functions_.insert(std::make_pair(name, cvariable));
  parameters_.push_back(TypeList());
  static_functions_.insert(cvariable);
  next++;
  return cvariable;
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

/* Tests if a function is a continuous variable. */
bool FunctionTable::isCVariable (const std::string &name) const
{
  std::vector <std::pair<std::string, std::pair<double,double> > >::const_iterator it;
  for (it=cvariables_.begin (); it != cvariables_.end (); it++)
    {
      if (name == (*it).first)
	return true;
    }
  return false;
}

const std::pair<double,double>& FunctionTable::getCVariableBounds (const std::string &rscname) const
{
  for (size_t i=0; i<cvariables_.size (); i++)
    {
      if (cvariables_[i].first == rscname)
	return cvariables_[i].second;
    }
  std::cout << "[Error]:FunctionTable::getCVariable: continuous variable not found: " 
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
