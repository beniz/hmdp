/*
 * Main program.
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
#include "states.h"
#include "problems.h"
#include "domains.h"
#include "exceptions.h"

#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <string.h>
#include <limits.h>

/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;

/* Name of current file. */
std::string current_file;
/* Level of warnings. */
int warning_level;
/* Verbosity level. */
int verbosity;

using namespace ppddl_parser;

/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  yyin = fopen(name, "r");

  if (yyin == NULL) {
    std::cerr << PACKAGE << ':' << name << ": " << strerror(errno)
	      << std::endl;
    return false;
  } else {
    current_file = name;
    bool success = (yyparse() == 0);
    fclose(yyin);
    return success;
  }
}

int main(int argc, char* argv[]) {
  /* config file for problem specific restrictions */
  char *config = 0;
  /* Set default verbosity. */
  verbosity = 2;
  /* Set default warning level. */
  warning_level = 1;
  /* Set default seed. */
  size_t seed = time(NULL);
  /* Set default simulation limit. */
  size_t limit = UINT_MAX;


  try {
    /*
     * Read pddl files.
     */
    if (!read_file(argv[1])) {
      return -1;
    }

    if (verbosity > 0) {
      std::cerr << "using seed " << seed << std::endl;
    }
    if (verbosity > 1) {
      /*
       * Display domains and problems.
       */
      std::cerr << "----------------------------------------"<< std::endl
		<< "domains:" << std::endl;
      for (Domain::DomainMap::const_iterator di = Domain::begin();
	   di != Domain::end(); di++) {
	std::cerr << std::endl << *(*di).second << std::endl;
      }
      std::cerr << "----------------------------------------"<< std::endl
		<< "problems:" << std::endl;
      for (Problem::ProblemMap::const_iterator pi = Problem::begin();
	   pi != Problem::end(); pi++) {
	std::cerr << std::endl << *(*pi).second << std::endl;
      }
      std::cerr << "----------------------------------------"<< std::endl;
    }

      for (Problem::ProblemMap::const_iterator pi = Problem::begin();
	   pi != Problem::end(); pi++) {
	const Problem& problem = *(*pi).second;
	if (verbosity > 0) {
	  std::cerr << "simulating problem `" << problem.name() << "'"
		    << std::endl;
	}
	const State* s = new State(problem);
	size_t time = 0;
	const State *startstate = new State(problem);
	std::cout<< " Start state : " << std::endl;
	startstate->print(std::cout, problem.domain().predicates(), problem.domain().functions(), problem.terms());
	ActionList actions;
	problem.enabled_actions(actions, startstate->atoms(), startstate->values());
	std::cout<< " Selected Action : " << std::endl;
	actions[0]->print(std::cerr, problem.terms());
	for (int i = 0; i < 10; i++) {
	  const State& next_s = startstate->next(actions[0]);
	  std::cout << " Next state " << i << " : " << std::endl;
	  next_s.print(std::cout, problem.domain().predicates(), problem.domain().functions(), problem.terms());
	}
	std::cout << std::endl << time << ": ";
	s->print(std::cout, problem.domain().predicates(),
		 problem.domain().functions(), problem.terms());
	std::cout << std::endl;
	/*if (problem.goal().holds(s->atoms(), s->values())) {
	  std::cout << "  goal achieved" << std::endl;
	} else {
	  std::cout << "  time limit reached" << std::endl;
	}*/
	/* if (!problem.constant_metric()) {
	   std::cout << "  value of maximization metric is "
	   << problem.metric().value(s->values()) << std::endl;
	   } */
	delete s;
      }
  } catch (const Exception& e) {
    std::cerr << PACKAGE ": " << e << std::endl;
    return -1;
  } catch (...) {
    std::cerr << PACKAGE ": fatal error" << std::endl;
    return -1;
  }

  Problem::clear();
  Domain::clear();

  return 0;
}
