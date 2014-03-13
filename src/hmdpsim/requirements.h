/* -*-C++-*- */
/*
 * PDDL requirements.
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
#ifndef REQUIREMENTS_H
#define REQUIREMENTS_H

#include <config.h>

namespace ppddl_parser
{

/* ====================================================================== */
/* Requirements */

/*
 * PDDL requirements.
 */
class Requirements {
public:
  /* Required action style. */
  bool strips;
  /* Whether support for types is required. */
  bool typing;
  /* Whether support for negative preconditions is required. */
  bool negative_preconditions;
  /* Whether support for disjunctive preconditions is required. */
  bool disjunctive_preconditions;
  /* Whether support for equality predicate is required. */
  bool equality;
  /* Whether support for existentially quantified preconditions is
     required. */
  bool existential_preconditions;
  /* Whether support for universally quantified preconditions is
     required. */
  bool universal_preconditions;
  /* Whether support for conditional effects is required. */
  bool conditional_effects;
  /* Whether support for fluents is required. */
  bool fluents;
  /* Whether support for probabilistic effects is required. */
  bool probabilistic_effects;
  /* Whether support for rewards is required. */
  bool rewards;
  /* Whether support for hmdp (discrete tiles and distributions over the 
     continuous space) is required. */
  bool m_hmdp;

  /* Constructs a default requirements object. */
  Requirements();

  /* Enables quantified preconditions. */
  void quantified_preconditions();

  /* Enables ADL style actions. */
  void adl();

  /* Enables hmdp options */
  void hmdp ();
};

} /* end of namespace */

#endif /* REQUIREMENTS_H */
