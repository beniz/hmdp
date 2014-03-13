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
#include "requirements.h"

namespace ppddl_parser
{

/* ====================================================================== */
/* Requirements */

/* Constructs a default requirements object. */
Requirements::Requirements()
  : strips(true), typing(false), negative_preconditions(false),
    disjunctive_preconditions(false), equality(false),
    existential_preconditions(false), universal_preconditions(false),
    conditional_effects(false), fluents(false), probabilistic_effects(false),
    rewards(false), m_hmdp(false) {}


/* Enables quantified preconditions. */
void Requirements::quantified_preconditions() {
  existential_preconditions = true;
  universal_preconditions = true;
}


/* Enables ADL style actions. */
void Requirements::adl() {
  strips = true;
  typing = true;
  negative_preconditions = true;
  disjunctive_preconditions = true;
  equality = true;
  quantified_preconditions();
  conditional_effects = true;
}

/* Enables hmdp options */
void Requirements::hmdp()
{
  conditional_effects = true;
  rewards = true;
  probabilistic_effects = true;
  m_hmdp = true;
}

} /* end of namespace */
