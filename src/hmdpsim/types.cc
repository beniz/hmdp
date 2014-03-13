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

#include "types.h"

namespace ppddl_parser
{

/* ====================================================================== */
/* TypeTable */

/* Adds a simple type with the given name to this table and returns
   the type. */
Type TypeTable::add_type(const std::string& name) {
  Type type = last_type() + 1;
  names_.push_back(name);
  types_.insert(std::make_pair(name, type));
  if (type > 1) {
    subtype_.push_back(std::vector<bool>(2*(type - 1), false));
  }
  return type;
}


/* Adds the union of the given types to this table and returns the
   type. */
Type TypeTable::add_type(const TypeSet& types) {
  if (types.empty()) {
    return OBJECT_TYPE;
  } else if (types.size() == 1) {
    return *types.begin();
  } else {
    Type type = utypes_.size();
    utypes_.push_back(types);
    return -type - 1;
  }
}


/* Returns the type with the given name.  If no type with the given
   name exists, false is returned in the second part of the
   result. */
std::pair<Type, bool> TypeTable::find_type(const std::string& name) const {
  std::map<std::string, Type>::const_iterator ti = types_.find(name);
  if (ti != types_.end()) {
    return std::make_pair((*ti).second, true);
  } else {
    return std::make_pair(0, false);
  }
}


/* Adds the second type as a supertype of the first type.  Returns
   false if the intended supertype is a subtype of the first
   type. */
bool TypeTable::add_supertype(Type type1, Type type2) {
  if (type2 < 0) {
    /*
     * Add all component types of type2 as supertypes of type1.
     */
    const TypeSet& types = utypes_[-type2 - 1];
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      if (!add_supertype(type1, *ti)) {
	return false;
      }
    }
    return true;
  } else if (subtype(type1, type2)) {
    /* The first type is already a subtype of the second type. */
    return true;
  } else if (subtype(type2, type1)) {
    /* The second type is already a subtype of the first type. */
    return false;
  } else {
    /*
     * Make all subtypes of type1 subtypes of all supertypes of type2.
     */
    Type n = last_type();
    for (Type k = 1; k <= n; k++) {
      if (subtype(k, type1) && !subtype(k, type2)) {
	for (Type l = 1; l <= n; l++) {
	  if (subtype(type2, l)) {
	    if (k > l) {
	      subtype_[k - 2][2*k - l - 2] = true;
	    } else {
	      subtype_[l - 2][k - 1] = true;
	    }
	  }
	}
      }
    }
    return true;
  }
}


/* Checks if the first type is a subtype of the second type. */
bool TypeTable::subtype(Type type1, Type type2) const {
  if (type1 == type2) {
    /* Same types. */
    return true;
  } else if (type1 < 0) {
    /* Every component type of type1 must be a subtype of type2. */
    const TypeSet& types = utypes_[-type1 - 1];
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      if (!subtype(*ti, type2)) {
	return false;
      }
    }
    return true;
  } else if (type2 < 0) {
    /* type1 one must be a subtype of some component type of type2. */
    const TypeSet& types = utypes_[-type2 - 1];
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      if (subtype(type1, *ti)) {
	return true;
      }
    }
    return false;
  } else if (type1 == OBJECT_TYPE) {
    return false;
  } else if (type2 == OBJECT_TYPE) {
    return true;
  } else if (type1 > type2) {
    return subtype_[type1 - 2][2*type1 - type2 - 2];
  } else {
    return subtype_[type2 - 2][type1 - 1];
  }
}


/* Fills the provided set with the components of the given type. */
void TypeTable::components(TypeSet& components, Type type) const {
  if (type < 0) {
    components = utypes_[-type - 1];
  } else if (type != OBJECT_TYPE) {
    components.insert(type);
  }
}


/* Prints the given type of the given stream. */
void TypeTable::print_type(std::ostream& os, Type type) const {
  if (type < 0) {
    const TypeSet& types = utypes_[-type - 1];
    os << "(either";
    for (TypeSet::const_iterator ti = types.begin(); ti != types.end(); ti++) {
      os << ' ' << names_[*ti];
    }
    os << ")";
  } else if (type == OBJECT_TYPE) {
    os << OBJECT_NAME;
  } else {
    os << names_[type - 1];
  }
}

} /* end of namespace */
