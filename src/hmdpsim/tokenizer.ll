/* -*-C++-*- */
/*
 * Tokenizer.
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
%{
#include <config.h>
#include "effects.h"
#include "formulas.h"
#include "terms.h"
#include "types.h"
#include "rational.h"
#include "distributionTypes.h"
#include "goals.h"
#include <cctype>
#include <string>

using namespace ppddl_parser;

class Expression;
class Application;

#include "parser.h"

/* Current line number. */
size_t line_number;

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token);
/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s);

%}

%option case-insensitive never-interactive nounput noyywrap

IDENT	[A-Za-z]([A-Za-z0-9\-_])*

%%

define				return make_string(yytext, DEFINE);
domain				return make_string(yytext, DOMAIN_TOKEN);
problem				return make_string(yytext, PROBLEM);
:requirements			return REQUIREMENTS;
:types				return TYPES;
:constants			return CONSTANTS;
:predicates			return PREDICATES;
:functions			return FUNCTIONS;
:resources                      return RESOURCES;
:strips				return STRIPS;
:typing				return TYPING;
:negative-preconditions		return NEGATIVE_PRECONDITIONS;
:disjunctive-preconditions	return DISJUNCTIVE_PRECONDITIONS;
:equality			return EQUALITY;
:existential-preconditions	return EXISTENTIAL_PRECONDITIONS;
:universal-preconditions	return UNIVERSAL_PRECONDITIONS;
:quantified-preconditions	return QUANTIFIED_PRECONDITIONS;
:conditional-effects		return CONDITIONAL_EFFECTS;
:fluents			return FLUENTS;
:adl				return ADL;
:durative-actions		return DURATIVE_ACTIONS;
:duration-inequalities		return DURATION_INEQUALITIES;
:continuous-effects		return CONTINUOUS_EFFECTS;
:probabilistic-effects		return PROBABILISTIC_EFFECTS;
:rewards			return REWARDS;
:mdp				return MDP;
:hmdp                           return HMDP;
:action				return ACTION;
:parameters			return PARAMETERS;
:precondition			return PRECONDITION;
:effect				return EFFECT;
:domain				return PDOMAIN;
:objects			return OBJECTS;
:init				return INIT;
:goal-linear-reward             return GOAL_LINEAR_REWARD;
:goal-reward			return GOAL_REWARD;
:goal				return GOAL;
:metric				return METRIC;
goal-probability		return make_string(yytext, GOAL_PROBABILITY);
number				return make_string(yytext, NUMBER_TOKEN);
object				return make_string(yytext, OBJECT_TOKEN);
either				return make_string(yytext, EITHER);
when				return make_string(yytext, WHEN);
not				return make_string(yytext, NOT);
and				return make_string(yytext, AND);
or				return make_string(yytext, OR);
imply				return make_string(yytext, IMPLY);
exists				return make_string(yytext, EXISTS);
forall				return make_string(yytext, FORALL);
probabilistic			return make_string(yytext, PROBABILISTIC);
assign				return make_string(yytext, ASSIGN);
scale-up			return make_string(yytext, SCALE_UP);
scale-down			return make_string(yytext, SCALE_DOWN);
increase			return make_string(yytext, INCREASE);
decrease			return make_string(yytext, DECREASE);
increase-probabilistic          return make_string(yytext, INCREASE_PROBABILISTIC);
decrease-probabilistic          return make_string(yytext, DECREASE_PROBABILISTIC);
minimize			return make_string(yytext, MINIMIZE);
maximize			return make_string(yytext, MAXIMIZE);
points                          return make_string(yytext, DISCZ_POINTS);
threshold                       return make_string(yytext, DISCZ_THRESHOLD);
interval                        return make_string(yytext, DISCZ_INTERVAL);
normal                          return make_string(yytext, DIST_NORMAL);
uniform                         return make_string(yytext, DIST_UNIFORM);
{IDENT}				return make_string(yytext, NAME);
\?{IDENT}			return make_string(yytext, VARIABLE);
[0-9]*[./]?[0-9]+		return make_number(yytext);
\<=				return LE;
>=				return GE;
[()\-=<>+*/]			return yytext[0];
;.*$				/* comment */
[ \t\r]+			/* whitespace */
\n				line_number++;
.				return ILLEGAL_TOKEN;

%%

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token) {
  std::string* result = new std::string();
  for (const char* p = s; *p != '\0'; p++) {
    *result += tolower(*p);
  }
  yylval.str = result;
  return token;
}


/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s) {
  yylval.num = new Rational(s);
  return NUMBER;
}


