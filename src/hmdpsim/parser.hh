/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_HH_INCLUDED
# define YY_YY_PARSER_HH_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DEFINE = 258,
     DOMAIN_TOKEN = 259,
     PROBLEM = 260,
     REQUIREMENTS = 261,
     TYPES = 262,
     CONSTANTS = 263,
     PREDICATES = 264,
     FUNCTIONS = 265,
     RESOURCES = 266,
     STRIPS = 267,
     TYPING = 268,
     NEGATIVE_PRECONDITIONS = 269,
     DISJUNCTIVE_PRECONDITIONS = 270,
     EQUALITY = 271,
     EXISTENTIAL_PRECONDITIONS = 272,
     UNIVERSAL_PRECONDITIONS = 273,
     QUANTIFIED_PRECONDITIONS = 274,
     CONDITIONAL_EFFECTS = 275,
     FLUENTS = 276,
     ADL = 277,
     DURATIVE_ACTIONS = 278,
     DURATION_INEQUALITIES = 279,
     CONTINUOUS_EFFECTS = 280,
     PROBABILISTIC_EFFECTS = 281,
     REWARDS = 282,
     MDP = 283,
     HMDP = 284,
     ACTION = 285,
     PARAMETERS = 286,
     PRECONDITION = 287,
     EFFECT = 288,
     PDOMAIN = 289,
     OBJECTS = 290,
     INIT = 291,
     GOAL = 292,
     GOAL_REWARD = 293,
     GOAL_LINEAR_REWARD = 294,
     METRIC = 295,
     GOAL_PROBABILITY = 296,
     WHEN = 297,
     NOT = 298,
     AND = 299,
     OR = 300,
     IMPLY = 301,
     EXISTS = 302,
     FORALL = 303,
     PROBABILISTIC = 304,
     ASSIGN = 305,
     SCALE_UP = 306,
     SCALE_DOWN = 307,
     INCREASE = 308,
     DECREASE = 309,
     MINIMIZE = 310,
     MAXIMIZE = 311,
     NUMBER_TOKEN = 312,
     OBJECT_TOKEN = 313,
     EITHER = 314,
     LE = 315,
     GE = 316,
     NAME = 317,
     VARIABLE = 318,
     NUMBER = 319,
     ILLEGAL_TOKEN = 320,
     RSC_DIST_DISTRIBUTION = 321,
     RSC_DIST_DISCRETIZATION = 322,
     RSC_DIST_MEAN = 323,
     RSC_DIST_STDDEV = 324,
     DISCZ_POINTS = 325,
     DISCZ_THRESHOLD = 326,
     DISCZ_INTERVAL = 327,
     DIST_NORMAL = 328,
     DIST_UNIFORM = 329,
     INCREASE_PROBABILISTIC = 330,
     DECREASE_PROBABILISTIC = 331
   };
#endif
/* Tokens.  */
#define DEFINE 258
#define DOMAIN_TOKEN 259
#define PROBLEM 260
#define REQUIREMENTS 261
#define TYPES 262
#define CONSTANTS 263
#define PREDICATES 264
#define FUNCTIONS 265
#define RESOURCES 266
#define STRIPS 267
#define TYPING 268
#define NEGATIVE_PRECONDITIONS 269
#define DISJUNCTIVE_PRECONDITIONS 270
#define EQUALITY 271
#define EXISTENTIAL_PRECONDITIONS 272
#define UNIVERSAL_PRECONDITIONS 273
#define QUANTIFIED_PRECONDITIONS 274
#define CONDITIONAL_EFFECTS 275
#define FLUENTS 276
#define ADL 277
#define DURATIVE_ACTIONS 278
#define DURATION_INEQUALITIES 279
#define CONTINUOUS_EFFECTS 280
#define PROBABILISTIC_EFFECTS 281
#define REWARDS 282
#define MDP 283
#define HMDP 284
#define ACTION 285
#define PARAMETERS 286
#define PRECONDITION 287
#define EFFECT 288
#define PDOMAIN 289
#define OBJECTS 290
#define INIT 291
#define GOAL 292
#define GOAL_REWARD 293
#define GOAL_LINEAR_REWARD 294
#define METRIC 295
#define GOAL_PROBABILITY 296
#define WHEN 297
#define NOT 298
#define AND 299
#define OR 300
#define IMPLY 301
#define EXISTS 302
#define FORALL 303
#define PROBABILISTIC 304
#define ASSIGN 305
#define SCALE_UP 306
#define SCALE_DOWN 307
#define INCREASE 308
#define DECREASE 309
#define MINIMIZE 310
#define MAXIMIZE 311
#define NUMBER_TOKEN 312
#define OBJECT_TOKEN 313
#define EITHER 314
#define LE 315
#define GE 316
#define NAME 317
#define VARIABLE 318
#define NUMBER 319
#define ILLEGAL_TOKEN 320
#define RSC_DIST_DISTRIBUTION 321
#define RSC_DIST_DISCRETIZATION 322
#define RSC_DIST_MEAN 323
#define RSC_DIST_STDDEV 324
#define DISCZ_POINTS 325
#define DISCZ_THRESHOLD 326
#define DISCZ_INTERVAL 327
#define DIST_NORMAL 328
#define DIST_UNIFORM 329
#define INCREASE_PROBABILISTIC 330
#define DECREASE_PROBABILISTIC 331



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2053 of yacc.c  */
#line 274 "parser.yy"

  Assignment::AssignOp setop;
  const Effect* effect;
  ConjunctiveEffect* ceffect;
  ProbabilisticEffect* peffect;
  const StateFormula* formula;
  const Atom* atom;
  Conjunction* conj;
  Disjunction* disj;
  const ppddl_parser::Expression* expr;
  const ppddl_parser::Application* appl;
  Comparison::CmpPredicate comp;
  Type type;
  TypeSet* types;
  const std::string* str;
  std::vector<const std::string*>* strs;
  const Rational* num;
  distributionType pdisttype;
  disczType pdiscztype;
  const GoalLinearReward *greward;
  ConjunctiveGoalReward *cgreward;


/* Line 2053 of yacc.c  */
#line 233 "parser.hh"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_PARSER_HH_INCLUDED  */
