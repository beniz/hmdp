/* -*-C++-*- */
/*
 * States.
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
#ifndef STATES_H
#define STATES_H

#include <config.h>
#include "formulas.h"
#include "expressions.h"
#include <iostream>

namespace ppddl_parser
{

class Action;
class Problem;


/* ====================================================================== */
/* State */

/*
 * A state.
 */
class State {
public:
  /* Constructs an initial state for the given problem. */
  explicit State(const Problem& problem);

  /* G. Sakkis: Constructs a state empty of atoms and values */
  State() {}
  
  void addAtom(const Atom& atom) { atoms_.insert(&atom); }
  
  void addValueEntry(const Application& application, Rational value) {
        values_[&application] = value;
  }

  /* Returns the set of atoms that hold in this state. */
  const AtomSet& atoms() const { return atoms_; }

  /* Returns the function applications values for this state. */
  const ValueMap& values() const { return values_; }

  /* Returns a sampled successor of this state. */
  const State& next(const Action* action) const;

  /* Prints this object on the given stream. */
  void print(std::ostream& os, const PredicateTable& predicates,
	     const FunctionTable& functions, const TermTable& terms) const;

  /* Prints this object on the given stream in XML. */
  void printXML(std::ostream& os, const PredicateTable& predicates,
		const FunctionTable& functions, const TermTable& terms,
		bool goal) const;

  /* Serializes the state and sends it on a stream */
  void send(std::ostream& os, const PredicateTable& predicates,
	    const FunctionTable& functions, const TermTable& terms) const;

private:
  /* The problem that this state is associated with. */
  const Problem* problem_;
  /* Atomic state formulas that hold in this state. */
  AtomSet atoms_;
  /* Function application values in this state. */
  ValueMap values_;
};

} /* end of namespace */

#endif /* STATES_H */
