/* -*-C++-*- */
/*
 * Types.
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

#ifndef TYPES_H
#define TYPES_H

#include <config.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>

namespace ppddl_parser
{

/* Type index. */
typedef int Type;

/* The object type. */
const Type OBJECT_TYPE = 0;
/* Name of object type. */
const std::string OBJECT_NAME("object");


/* ====================================================================== */
/* TypeList */

struct TypeList : public std::vector<Type> {
};


/* ====================================================================== */
/* TypeSet */

struct TypeSet : public std::unordered_set<Type> {
};


/* ====================================================================== */
/* TypeTable */

/*
 * Type table.
 */
class TypeTable {
public:
  /* Adds a simple type with the given name to this table and returns
     the type. */
  Type add_type(const std::string& name);

  /* Adds the union of the given types to this table and returns the
     type. */
  Type add_type(const TypeSet& types);

  /* Returns the type with the given name.  If no type with the given
     name exists, false is returned in the second part of the
     result. */
  std::pair<Type, bool> find_type(const std::string& name) const;

  /* Returns the first type of this type table. */
  Type first_type() const { return OBJECT_TYPE; }

  /* Returns the last type of this type table. */
  Type last_type() const { return names_.size(); }

  /* Adds the second type as a supertype of the first type.  Returns
     false if the second type is a proper subtype of the first
     type. */
  bool add_supertype(Type type1, Type type2);

  /* Checks if the first type is a subtype of the second type. */
  bool subtype(Type type1, Type type2) const;

  /* Fills the provided set with the components of the given type. */
  void components(TypeSet& components, Type type) const;

  /* Prints the given type on the given stream. */
  void print_type(std::ostream& os, Type type) const;

private:
  /* Type names. */
  std::vector<std::string> names_;
  /* Mapping of type names to types. */
  std::map<std::string, Type> types_;
  /* Transitive closure of subtype relation. */
  std::vector<std::vector<bool> > subtype_;
  /* Union types. */
  std::vector<TypeSet> utypes_;
};

} /* end of namespace */

#endif /* TYPES_H */
