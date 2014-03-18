/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 19 "parser.yy"

#include <config.h>
#include "problems.h"
#include "domains.h"
#include "actions.h"
#include "effects.h"
#include "formulas.h"
#include "expressions.h"
#include "functions.h"
#include "predicates.h"
#include "terms.h"
#include "types.h"
#include "rational.h"
#include "exceptions.h"
#include "distributionTypes.h"
#include <iostream>
#include <map>
#include <string>
#include <typeinfo>


/* Workaround for bug in Bison 1.35 that disables stack growth. */
#define YYLTYPE_IS_TRIVIAL 1

using namespace ppddl_parser;

/*
 * Context of free variables.
 */
struct Context {
  void push_frame() {
    frames_.push_back(VariableMap());
  }

  void pop_frame() {
    frames_.pop_back();
  }

  void insert(const std::string& name, Variable v) {
    frames_.back()[name] = v;
  }

  std::pair<Variable, bool> shallow_find(const std::string& name) const {
    VariableMap::const_iterator vi = frames_.back().find(name);
    if (vi != frames_.back().end()) {
      return std::make_pair((*vi).second, true);
    } else {
      return std::make_pair(0, false);
    }
  }

  std::pair<Variable, bool> find(const std::string& name) const {
    for (std::vector<VariableMap>::const_reverse_iterator fi =
	   frames_.rbegin(); fi != frames_.rend(); fi++) {
      VariableMap::const_iterator vi = (*fi).find(name);
      if (vi != (*fi).end()) {
	return std::make_pair((*vi).second, true);
      }
    }
    return std::make_pair(0, false);
  }

private:
  struct VariableMap : public std::map<std::string, Variable> {
  };

  std::vector<VariableMap> frames_;
};


/* The lexer. */
extern int yylex();
/* Current line number. */
extern size_t line_number;
/* Name of current file. */
extern std::string current_file;
/* Level of warnings. */
extern int warning_level;

/* Whether the last parsing attempt succeeded. */
static bool success = true;
/* Current domain. */
static Domain* domain;
/* Domains. */
static std::map<std::string, Domain*> domains;
/* Problem being parsed, or NULL if no problem is being parsed. */
static Problem* problem;
/* Current requirements. */
static Requirements* requirements;
/* The goal probability function, if probabilistic effects are required. */
static Function goal_prob_function; 
/* The reward function, if rewards are required. */
static Function reward_function;
/* Predicate being parsed. */
static Predicate predicate;
/* Whether a predicate is being parsed. */
static bool parsing_predicate;
/* Whether predicate declaration is repeated. */
static bool repeated_predicate;
/* Function being parsed. */
static Function function;
/* Whether a function is being parsed. */
static bool parsing_function;
/* Whether function declaration is repeated. */
static bool repeated_function;
/* Action being parsed, or NULL if no action is being parsed. */
static ActionSchema* action;
/* Current variable context. */
static Context context;
/* Predicate for atomic state formula being parsed. */
static Predicate atom_predicate;
/* Whether the predicate of the currently parsed atom was undeclared. */
static bool undeclared_atom_predicate;
/* Whether parsing effect fluents. */
static bool effect_fluent;
/* Whether parsing metric fluent. */
static bool metric_fluent;
/* Function for function application being parsed. */
static Function appl_function;
/* Whether the function of the currently parsed application was undeclared. */
static bool undeclared_appl_function;
/* Paramerers for atomic state formula or function application being parsed. */
static TermList term_parameters;
/* Whether parsing an atom. */
static bool parsing_atom;
/* Whether parsing a function application. */
static bool parsing_application;
/* Quantified variables for effect or formula being parsed. */
static VariableList quantified;
/* Most recently parsed term for equality formula. */
static Term eq_term;
/* Most recently parsed expression for equality formula. */
static const Expression* eq_expr;
/* The first term for equality formula. */
static Term first_eq_term;
/* The first expression for equality formula. */
static const Expression* first_eq_expr;
/* Kind of name map being parsed. */
static enum { TYPE_KIND, CONSTANT_KIND, OBJECT_KIND, VOID_KIND } name_kind;

/* Outputs an error message. */
static void yyerror(const std::string& s); 
/* Outputs a warning message. */
static void yywarning(const std::string& s);
/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name);
/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
			 const std::string* domain_name);
/* Adds :typing to the requirements. */
static void require_typing();
/* Adds :fluents to the requirements. */
static void require_fluents();
/* Adds :disjunctive-preconditions to the requirements. */
static void require_disjunction();
/* Adds :conditional-effects to the requirements. */ 
static void require_conditional_effects();
/* Returns a simple type with the given name. */
static Type make_type(const std::string* name);
/* Returns the union of the given types. */
static Type make_type(const TypeSet& types);
/* Returns a simple term with the given name. */
static Term make_term(const std::string* name);
/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name);
/* Creates a function with the given name. */
static void make_function(const std::string* name);
/* Creates a continuous variable as a function with the given name. */
 static void make_cvariable (const std::string *name, const Rational *low, 
			    const Rational *high);
/* Creates an action with the given name. */
static void make_action(const std::string* name);
/* Adds the current action to the current domain. */
static void add_action();
/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect();
/* Creates a universally quantified effect. */
static const Effect* make_forall_effect(const Effect& effect);
/* Adds an outcome to the given probabilistic effect. */
static void add_effect_outcome(ProbabilisticEffect& peffect,
			       const Rational* p, const Effect& effect);
/* Creates an add effect. */
static const Effect* make_add_effect(const Atom& atom);
/* Creates a delete effect. */
static const Effect* make_delete_effect(const Atom& atom);
/* Creates an assignment effect. */
static const Effect* make_assignment_effect(Assignment::AssignOp oper,
					    const Application& application,
					    const Expression& expr);
/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names, Type type);
/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
			  Type type);
/* Prepares for the parsing of an atomic state formula. */ 
static void prepare_atom(const std::string* name);
/* Prepares for the parsing of a function application. */ 
static void prepare_application(const std::string* name);
/* Adds a term with the given name to the current atomic state formula. */
static void add_term(const std::string* name);
/* Creates the atomic formula just parsed. */
static const Atom* make_atom();
/* Creates the function application just parsed. */
static const Application* make_application();
/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
					  const Expression* opt_term);
/* Creates an atom or fluent for the given name to be used in an
   equality formula. */
static void make_eq_name(const std::string* name);
/* Creates an equality formula. */
static const StateFormula* make_equality();
/* Creates a negated formula. */
static const StateFormula* make_negation(const StateFormula& negand);
/* Creates an implication. */
static const StateFormula* make_implication(const StateFormula& f1,
					    const StateFormula& f2);
/* Prepares for the parsing of an existentially quantified formula. */
static void prepare_exists();
/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall();
/* Creates an existentially quantified formula. */
static const StateFormula* make_exists(const StateFormula& body);
/* Creates a universally quantified formula. */
static const StateFormula* make_forall(const StateFormula& body);
/* Sets the goal reward for the current problem. */
static GoalReward* make_goal_reward(const Expression& reward);
/* set the goal linear reward for the current problem. */
static GoalLinearReward* make_goal_linear_reward (const Expression &reward);
/* add a component to the argument goal linear reward. */
static void add_linear_reward_comp (const Expression &goal_reward, const GoalLinearReward *glreward); 
/* Adds a goal to the problem */
static void add_goal (const std::string &name, const StateFormula &condition, const GoalLinearReward &gr);
/* Sets the default metric for the current problem. */
static void set_default_metric();

/* Line 371 of yacc.c  */
#line 305 "parser.cc"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
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
     CSPACE = 266,
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
#define CSPACE 266
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
/* Line 387 of yacc.c  */
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


/* Line 387 of yacc.c  */
#line 524 "parser.cc"
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

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 552 "parser.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1764

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  156
/* YYNRULES -- Number of rules.  */
#define YYNRULES  352
/* YYNRULES -- Number of states.  */
#define YYNSTATES  585

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   331

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      77,    78,    84,    83,     2,    79,     2,    85,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      81,    80,    82,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     8,    11,    14,    15,    25,
      26,    28,    31,    33,    35,    38,    40,    42,    44,    46,
      48,    51,    54,    57,    61,    64,    66,    68,    70,    72,
      75,    78,    82,    85,    87,    89,    91,    93,    96,    99,
     103,   106,   108,   110,   112,   115,   118,   120,   122,   124,
     127,   131,   134,   136,   138,   141,   143,   145,   148,   150,
     152,   155,   157,   162,   164,   167,   169,   171,   173,   175,
     177,   179,   181,   183,   185,   187,   189,   191,   193,   195,
     197,   199,   201,   203,   204,   210,   211,   217,   222,   223,
     229,   234,   235,   238,   239,   245,   246,   248,   252,   254,
     257,   258,   262,   263,   269,   270,   273,   275,   276,   284,
     285,   293,   294,   299,   302,   304,   305,   307,   310,   313,
     315,   320,   321,   330,   331,   338,   343,   344,   347,   350,
     354,   356,   358,   363,   364,   371,   373,   375,   377,   379,
     381,   383,   385,   387,   389,   391,   393,   395,   396,   410,
     413,   415,   418,   420,   423,   425,   426,   432,   437,   438,
     441,   443,   449,   454,   459,   462,   466,   468,   473,   474,
     477,   479,   485,   487,   494,   502,   504,   510,   515,   520,
     522,   527,   533,   535,   540,   546,   548,   551,   553,   554,
     557,   558,   563,   564,   565,   572,   573,   580,   582,   583,
     590,   591,   598,   603,   608,   609,   615,   621,   622,   631,
     632,   641,   642,   645,   646,   649,   650,   656,   658,   659,
     665,   667,   669,   671,   673,   675,   677,   683,   689,   695,
     701,   703,   705,   712,   719,   723,   727,   729,   730,   737,
     738,   745,   746,   753,   754,   761,   762,   768,   770,   772,
     773,   775,   776,   782,   784,   786,   792,   798,   804,   810,
     812,   814,   815,   817,   818,   824,   826,   827,   830,   833,
     834,   837,   838,   840,   841,   846,   848,   851,   852,   854,
     855,   860,   862,   865,   866,   870,   872,   874,   879,   881,
     883,   886,   889,   891,   893,   895,   897,   899,   901,   903,
     905,   907,   909,   911,   913,   915,   917,   919,   921,   923,
     925,   927,   929,   931,   933,   935,   937,   939,   941,   943,
     945,   947,   949,   951,   953,   955,   957,   959,   961,   963,
     965,   967,   969,   971,   973,   975,   977,   979,   981,   983,
     985,   987,   989,   991,   993,   995,   997,  1000,  1002,  1004,
    1006,  1008,  1010
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      87,     0,    -1,    -1,    88,    89,    -1,    -1,    89,    90,
      -1,    89,   145,    -1,    -1,    77,   212,    77,   213,   238,
      78,    91,    92,    78,    -1,    -1,   103,    -1,   103,    93,
      -1,    93,    -1,   106,    -1,   106,    94,    -1,    94,    -1,
     108,    -1,   110,    -1,   111,    -1,   113,    -1,   113,    97,
      -1,   108,    95,    -1,   110,    96,    -1,   111,   113,    97,
      -1,   111,    97,    -1,   101,    -1,   110,    -1,   111,    -1,
     113,    -1,   113,    99,    -1,   110,    98,    -1,   111,   113,
      99,    -1,   111,    99,    -1,   101,    -1,   108,    -1,   111,
      -1,   113,    -1,   113,   100,    -1,   108,    98,    -1,   111,
     113,   100,    -1,   111,   100,    -1,   101,    -1,   108,    -1,
     110,    -1,   108,    99,    -1,   110,   100,    -1,   101,    -1,
     111,    -1,   113,    -1,   113,   102,    -1,   111,   113,   101,
      -1,   111,   101,    -1,   101,    -1,   110,    -1,   110,   101,
      -1,   101,    -1,   108,    -1,   108,   101,    -1,   101,    -1,
     102,    -1,   101,   102,    -1,   127,    -1,    77,     6,   104,
      78,    -1,   105,    -1,   104,   105,    -1,    12,    -1,    13,
      -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,    18,
      -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,    23,
      -1,    24,    -1,    25,    -1,    26,    -1,    27,    -1,    28,
      -1,    29,    -1,    -1,    77,     7,   107,   204,    78,    -1,
      -1,    77,     8,   109,   204,    78,    -1,    77,     9,   114,
      78,    -1,    -1,    77,    10,   112,   117,    78,    -1,    77,
      11,   123,    78,    -1,    -1,   114,   115,    -1,    -1,    77,
     236,   116,   201,    78,    -1,    -1,   118,    -1,   118,   119,
     117,    -1,   121,    -1,   118,   121,    -1,    -1,    79,   120,
     211,    -1,    -1,    77,   237,   122,   201,    78,    -1,    -1,
     124,   123,    -1,   125,    -1,    -1,    77,   240,   241,   241,
     126,   201,    78,    -1,    -1,    77,    30,   238,   128,   129,
     130,    78,    -1,    -1,    31,    77,   201,    78,    -1,   132,
     131,    -1,   131,    -1,    -1,   133,    -1,    32,   171,    -1,
      33,   134,    -1,   140,    -1,    77,   217,   137,    78,    -1,
      -1,    77,   221,   135,    77,   201,    78,   134,    78,    -1,
      -1,    77,   215,   136,   171,   134,    78,    -1,    77,   222,
     138,    78,    -1,    -1,   137,   134,    -1,   139,   134,    -1,
     138,   139,   134,    -1,    64,    -1,   179,    -1,    77,   216,
     179,    78,    -1,    -1,    77,   142,   141,   193,   184,    78,
      -1,   223,    -1,   224,    -1,   225,    -1,   226,    -1,   227,
      -1,   228,    -1,   229,    -1,    73,    -1,    74,    -1,    70,
      -1,    71,    -1,    72,    -1,    -1,    77,   212,    77,   214,
     238,    78,    77,    34,   238,    78,   146,   147,    78,    -1,
     103,   148,    -1,   148,    -1,   150,   149,    -1,   149,    -1,
     152,   160,    -1,   160,    -1,    -1,    77,    35,   151,   204,
      78,    -1,    77,    36,   153,    78,    -1,    -1,   153,   154,
      -1,   181,    -1,    77,    80,   197,    64,    78,    -1,    77,
     222,   155,    78,    -1,    77,   197,   185,    78,    -1,   139,
     156,    -1,   155,   139,   156,    -1,   158,    -1,    77,   217,
     157,    78,    -1,    -1,   157,   158,    -1,   181,    -1,    77,
      80,   197,   159,    78,    -1,    64,    -1,    77,    37,   238,
     171,   161,    78,    -1,    77,    37,   238,   171,   161,    78,
     160,    -1,   168,    -1,    77,    38,   195,    78,   168,    -1,
      77,    38,   162,    78,    -1,    77,    39,   163,    78,    -1,
     164,    -1,    77,   217,   166,    78,    -1,    77,   215,   171,
     162,    78,    -1,   165,    -1,    77,   217,   167,    78,    -1,
      77,   215,   171,   163,    78,    -1,   195,    -1,   195,   165,
      -1,   195,    -1,    -1,   166,   162,    -1,    -1,   167,    77,
     163,    78,    -1,    -1,    -1,    77,    40,   231,   169,   195,
      78,    -1,    -1,    77,    40,   230,   170,   195,    78,    -1,
     179,    -1,    -1,    77,    80,   186,   172,   186,    78,    -1,
      -1,    77,   183,   173,   184,   184,    78,    -1,    77,   216,
     171,    78,    -1,    77,   217,   177,    78,    -1,    -1,    77,
     218,   174,   178,    78,    -1,    77,   219,   171,   171,    78,
      -1,    -1,    77,   220,   175,    77,   201,    78,   171,    78,
      -1,    -1,    77,   221,   176,    77,   201,    78,   171,    78,
      -1,    -1,   177,   171,    -1,    -1,   178,   171,    -1,    -1,
      77,   236,   180,   199,    78,    -1,   236,    -1,    -1,    77,
     236,   182,   200,    78,    -1,   236,    -1,    81,    -1,    60,
      -1,    61,    -1,    82,    -1,    64,    -1,    77,    83,   184,
     184,    78,    -1,    77,    79,   184,   192,    78,    -1,    77,
      84,   184,   184,    78,    -1,    77,    85,   184,   184,    78,
      -1,   185,    -1,   193,    -1,   143,   193,   193,   144,   193,
     193,    -1,   143,    64,    64,   144,    64,    64,    -1,   143,
     193,   193,    -1,   143,    64,    64,    -1,    64,    -1,    -1,
      77,    83,   187,   184,   184,    78,    -1,    -1,    77,    79,
     188,   184,   192,    78,    -1,    -1,    77,    84,   189,   184,
     184,    78,    -1,    -1,    77,    85,   190,   184,   184,    78,
      -1,    -1,    77,   237,   191,   199,    78,    -1,   238,    -1,
     239,    -1,    -1,   184,    -1,    -1,    77,   237,   194,   199,
      78,    -1,   237,    -1,    64,    -1,    77,    83,   195,   195,
      78,    -1,    77,    79,   195,   196,    78,    -1,    77,    84,
     195,   195,    78,    -1,    77,    85,   195,   195,    78,    -1,
     197,    -1,    41,    -1,    -1,   195,    -1,    -1,    77,   237,
     198,   200,    78,    -1,   237,    -1,    -1,   199,   238,    -1,
     199,   239,    -1,    -1,   200,   238,    -1,    -1,   203,    -1,
      -1,   203,   207,   202,   201,    -1,   239,    -1,   203,   239,
      -1,    -1,   206,    -1,    -1,   206,   207,   205,   204,    -1,
     238,    -1,   206,   238,    -1,    -1,    79,   208,   209,    -1,
     233,    -1,   235,    -1,    77,   234,   210,    78,    -1,   233,
      -1,   235,    -1,   210,   233,    -1,   210,   235,    -1,   232,
      -1,     3,    -1,     4,    -1,     5,    -1,    42,    -1,    43,
      -1,    44,    -1,    45,    -1,    46,    -1,    47,    -1,    48,
      -1,    49,    -1,    50,    -1,    51,    -1,    52,    -1,    53,
      -1,    54,    -1,    75,    -1,    76,    -1,    55,    -1,    56,
      -1,    57,    -1,    58,    -1,    59,    -1,     3,    -1,     4,
      -1,     5,    -1,    59,    -1,    55,    -1,    56,    -1,    62,
      -1,   235,    -1,    58,    -1,    57,    -1,   238,    -1,     3,
      -1,     4,    -1,     5,    -1,    57,    -1,    58,    -1,    59,
      -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      -1,    52,    -1,    53,    -1,    54,    -1,    75,    76,    -1,
      55,    -1,    56,    -1,    62,    -1,    63,    -1,   238,    -1,
      64,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   326,   326,   326,   330,   331,   332,   339,   339,   343,
     344,   345,   346,   349,   350,   351,   354,   355,   356,   357,
     358,   359,   360,   361,   362,   363,   367,   368,   369,   370,
     371,   372,   373,   374,   378,   379,   380,   381,   382,   383,
     384,   385,   389,   390,   391,   392,   393,   397,   398,   399,
     400,   401,   402,   405,   406,   407,   410,   411,   412,   415,
     416,   419,   422,   425,   426,   429,   430,   431,   433,   435,
     436,   438,   440,   442,   443,   444,   445,   447,   449,   451,
     457,   462,   470,   481,   481,   485,   485,   489,   492,   492,
     495,   500,   501,   504,   504,   508,   509,   510,   513,   514,
     517,   517,   520,   520,   526,   528,   531,   535,   535,   542,
     542,   546,   547,   550,   551,   554,   555,   558,   561,   568,
     569,   570,   570,   572,   572,   574,   577,   578,   581,   586,
     590,   593,   594,   597,   597,   601,   602,   603,   604,   605,
     606,   607,   610,   612,   617,   619,   621,   630,   629,   634,
     635,   638,   639,   642,   643,   646,   646,   650,   653,   654,
     657,   658,   660,   662,   679,   684,   688,   689,   692,   693,
     696,   697,   701,   704,   705,   708,   711,   713,   715,   719,
     720,   721,   725,   726,   727,   731,   735,   738,   742,   743,
     746,   747,   750,   751,   751,   753,   753,   760,   762,   761,
     764,   764,   766,   767,   768,   768,   769,   770,   770,   772,
     772,   776,   777,   780,   781,   784,   784,   786,   789,   789,
     791,   794,   795,   796,   797,   804,   805,   806,   807,   808,
     809,   810,   814,   816,   820,   822,   826,   828,   828,   830,
     830,   832,   832,   834,   834,   836,   836,   838,   839,   842,
     843,   846,   846,   848,   851,   852,   854,   856,   858,   860,
     861,   865,   866,   869,   869,   871,   879,   880,   881,   884,
     885,   888,   889,   890,   890,   893,   894,   897,   898,   899,
     899,   902,   903,   906,   906,   909,   910,   911,   914,   915,
     916,   917,   920,   927,   930,   933,   936,   939,   942,   945,
     948,   951,   954,   957,   960,   963,   966,   969,   972,   975,
     977,   979,   982,   985,   988,   991,   994,   994,   994,   995,
     996,   996,   997,  1000,  1001,  1001,  1004,  1007,  1007,  1007,
    1008,  1008,  1008,  1009,  1009,  1009,  1009,  1009,  1009,  1009,
    1009,  1010,  1010,  1010,  1010,  1010,  1011,  1012,  1012,  1013,
    1016,  1019,  1022
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DEFINE", "DOMAIN_TOKEN", "PROBLEM",
  "REQUIREMENTS", "TYPES", "CONSTANTS", "PREDICATES", "FUNCTIONS",
  "CSPACE", "STRIPS", "TYPING", "NEGATIVE_PRECONDITIONS",
  "DISJUNCTIVE_PRECONDITIONS", "EQUALITY", "EXISTENTIAL_PRECONDITIONS",
  "UNIVERSAL_PRECONDITIONS", "QUANTIFIED_PRECONDITIONS",
  "CONDITIONAL_EFFECTS", "FLUENTS", "ADL", "DURATIVE_ACTIONS",
  "DURATION_INEQUALITIES", "CONTINUOUS_EFFECTS", "PROBABILISTIC_EFFECTS",
  "REWARDS", "MDP", "HMDP", "ACTION", "PARAMETERS", "PRECONDITION",
  "EFFECT", "PDOMAIN", "OBJECTS", "INIT", "GOAL", "GOAL_REWARD",
  "GOAL_LINEAR_REWARD", "METRIC", "GOAL_PROBABILITY", "WHEN", "NOT", "AND",
  "OR", "IMPLY", "EXISTS", "FORALL", "PROBABILISTIC", "ASSIGN", "SCALE_UP",
  "SCALE_DOWN", "INCREASE", "DECREASE", "MINIMIZE", "MAXIMIZE",
  "NUMBER_TOKEN", "OBJECT_TOKEN", "EITHER", "LE", "GE", "NAME", "VARIABLE",
  "NUMBER", "ILLEGAL_TOKEN", "RSC_DIST_DISTRIBUTION",
  "RSC_DIST_DISCRETIZATION", "RSC_DIST_MEAN", "RSC_DIST_STDDEV",
  "DISCZ_POINTS", "DISCZ_THRESHOLD", "DISCZ_INTERVAL", "DIST_NORMAL",
  "DIST_UNIFORM", "INCREASE_PROBABILISTIC", "DECREASE_PROBABILISTIC",
  "'('", "')'", "'-'", "'='", "'<'", "'>'", "'+'", "'*'", "'/'", "$accept",
  "file", "$@1", "domains_and_problems", "domain_def", "$@2",
  "domain_body", "domain_body2", "domain_body3", "domain_body4",
  "domain_body5", "domain_body6", "domain_body7", "domain_body8",
  "domain_body9", "structure_defs", "structure_def", "require_def",
  "require_keys", "require_key", "types_def", "$@3", "constants_def",
  "$@4", "predicates_def", "functions_def", "$@5", "cspace_def",
  "predicate_decls", "predicate_decl", "$@6", "function_decls",
  "function_decl_seq", "function_type_spec", "$@7", "function_decl", "$@8",
  "cvariable_decls", "cvariable_decl_seq", "cvariable_decl", "$@9",
  "action_def", "$@10", "parameters", "action_body", "action_body2",
  "precondition", "effect", "eff_formula", "$@11", "$@12", "eff_formulas",
  "prob_effs", "probability", "p_effect", "$@13", "assign_op",
  "distribution_type", "discretization_type", "problem_def", "$@14",
  "problem_body", "problem_body2", "problem_body3", "object_decl", "$@15",
  "init", "init_elements", "init_element", "prob_inits", "simple_init",
  "one_inits", "one_init", "value", "goal_spec", "goal_reward",
  "reward_formula", "reward_linear_formula", "goal_reward_expr",
  "goal_linear_reward_expr", "reward_formulas", "linear_reward_formulas",
  "metric_spec", "$@16", "$@17", "formula", "$@18", "$@19", "$@20", "$@21",
  "$@22", "conjuncts", "disjuncts", "atomic_term_formula", "$@23",
  "atomic_name_formula", "$@24", "binary_comp", "f_exp",
  "distribution_exp", "term_or_f_exp", "$@25", "$@26", "$@27", "$@28",
  "$@29", "opt_f_exp", "f_head", "$@30", "ground_f_exp",
  "opt_ground_f_exp", "ground_f_head", "$@31", "terms", "names",
  "variables", "$@32", "variable_seq", "typed_names", "$@33", "name_seq",
  "type_spec", "$@34", "type", "types", "function_type", "define",
  "domain", "problem", "when", "not", "and", "or", "imply", "exists",
  "forall", "probabilistic", "assign", "scale_up", "scale_down",
  "increase", "decrease", "increase_probabilistic",
  "decrease_probabilistic", "minimize", "maximize", "number", "object",
  "either", "type_name", "predicate", "function", "name", "variable",
  "cvariable", "bound", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,    40,    41,    45,
      61,    60,    62,    43,    42,    47
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    88,    87,    89,    89,    89,    91,    90,    92,
      92,    92,    92,    93,    93,    93,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    95,    95,    95,    95,
      95,    95,    95,    95,    96,    96,    96,    96,    96,    96,
      96,    96,    97,    97,    97,    97,    97,    98,    98,    98,
      98,    98,    98,    99,    99,    99,   100,   100,   100,   101,
     101,   102,   103,   104,   104,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   107,   106,   109,   108,   110,   112,   111,
     113,   114,   114,   116,   115,   117,   117,   117,   118,   118,
     120,   119,   122,   121,   123,   123,   124,   126,   125,   128,
     127,   129,   129,   130,   130,   131,   131,   132,   133,   134,
     134,   135,   134,   136,   134,   134,   137,   137,   138,   138,
     139,   140,   140,   141,   140,   142,   142,   142,   142,   142,
     142,   142,   143,   143,   144,   144,   144,   146,   145,   147,
     147,   148,   148,   149,   149,   151,   150,   152,   153,   153,
     154,   154,   154,   154,   155,   155,   156,   156,   157,   157,
     158,   158,   159,   160,   160,   160,   161,   161,   161,   162,
     162,   162,   163,   163,   163,   164,   165,   165,   166,   166,
     167,   167,   168,   169,   168,   170,   168,   171,   172,   171,
     173,   171,   171,   171,   174,   171,   171,   175,   171,   176,
     171,   177,   177,   178,   178,   180,   179,   179,   182,   181,
     181,   183,   183,   183,   183,   184,   184,   184,   184,   184,
     184,   184,   185,   185,   185,   185,   186,   187,   186,   188,
     186,   189,   186,   190,   186,   191,   186,   186,   186,   192,
     192,   194,   193,   193,   195,   195,   195,   195,   195,   195,
     195,   196,   196,   198,   197,   197,   199,   199,   199,   200,
     200,   201,   201,   202,   201,   203,   203,   204,   204,   205,
     204,   206,   206,   208,   207,   209,   209,   209,   210,   210,
     210,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   235,   235,   235,
     235,   235,   235,   236,   236,   236,   237,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     239,   240,   241
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     2,     0,     9,     0,
       1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       2,     2,     2,     3,     2,     1,     1,     1,     1,     2,
       2,     3,     2,     1,     1,     1,     1,     2,     2,     3,
       2,     1,     1,     1,     2,     2,     1,     1,     1,     2,
       3,     2,     1,     1,     2,     1,     1,     2,     1,     1,
       2,     1,     4,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     0,     5,     4,     0,     5,
       4,     0,     2,     0,     5,     0,     1,     3,     1,     2,
       0,     3,     0,     5,     0,     2,     1,     0,     7,     0,
       7,     0,     4,     2,     1,     0,     1,     2,     2,     1,
       4,     0,     8,     0,     6,     4,     0,     2,     2,     3,
       1,     1,     4,     0,     6,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,    13,     2,
       1,     2,     1,     2,     1,     0,     5,     4,     0,     2,
       1,     5,     4,     4,     2,     3,     1,     4,     0,     2,
       1,     5,     1,     6,     7,     1,     5,     4,     4,     1,
       4,     5,     1,     4,     5,     1,     2,     1,     0,     2,
       0,     4,     0,     0,     6,     0,     6,     1,     0,     6,
       0,     6,     4,     4,     0,     5,     5,     0,     8,     0,
       8,     0,     2,     0,     2,     0,     5,     1,     0,     5,
       1,     1,     1,     1,     1,     1,     5,     5,     5,     5,
       1,     1,     6,     6,     3,     3,     1,     0,     6,     0,
       6,     0,     6,     0,     6,     0,     5,     1,     1,     0,
       1,     0,     5,     1,     1,     5,     5,     5,     5,     1,
       1,     0,     1,     0,     5,     1,     0,     2,     2,     0,
       2,     0,     1,     0,     4,     1,     2,     0,     1,     0,
       4,     1,     2,     0,     3,     1,     1,     4,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     4,     1,     3,     0,     5,     6,   293,     0,
       0,   294,   295,     0,     0,   327,   328,   329,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   347,   348,   330,   331,   332,   349,     0,     0,     0,
     346,     7,     0,     9,     0,     0,     0,    12,    15,    25,
      59,    10,    13,    16,    17,    18,    19,    61,     0,     0,
      83,    85,    91,    88,   104,     0,     8,     0,    60,     0,
      11,     0,    14,     0,    21,    33,    26,    27,    28,     0,
      22,    41,    34,    35,    36,     0,    24,    46,    42,    43,
       0,     0,    20,     0,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,     0,    63,   277,   277,     0,    95,     0,     0,
     104,   106,   109,     0,    30,    52,    47,    48,     0,    32,
      55,    53,     0,     0,    29,    38,     0,    40,    58,    56,
       0,     0,    37,    44,    45,    23,   147,    62,    64,     0,
     278,   281,     0,     0,    87,    92,     0,     0,    96,    98,
     351,     0,    90,   105,   111,     0,    51,     0,    49,    54,
      31,    57,    39,   192,    84,   283,   279,   282,    86,   316,
     317,   318,   320,   321,   325,   324,   319,   322,   323,    93,
     102,   326,    89,   100,    95,    99,   352,     0,     0,   115,
      50,     0,   192,     0,   150,   152,   192,   192,   154,   175,
       0,   277,   271,   271,     0,    97,   107,   271,     0,     0,
       0,   114,   115,   116,   155,   158,     0,     0,     0,   149,
     148,     0,   151,     0,   153,   314,     0,   284,   285,   286,
     280,   350,     0,   272,   275,     0,   313,   101,   292,   271,
       0,     0,   117,   197,   217,     0,   118,   119,   131,   110,
     113,   277,     0,     0,   311,   312,   195,   193,   315,     0,
      94,   273,   276,   103,     0,   112,   297,   298,   299,   300,
     301,   302,   222,   223,     0,   221,   224,   200,     0,   211,
     204,     0,   207,   209,   215,   296,   303,   304,   305,   306,
     307,   308,   309,   310,   133,   123,     0,   126,   121,     0,
     135,   136,   137,   138,   139,   140,   141,     0,     0,   157,
     159,   160,   220,     0,     0,     0,     0,   288,   289,   271,
     108,   236,     0,   198,   247,   248,     0,     0,     0,   213,
       0,     0,     0,   266,     0,     0,     0,     0,     0,     0,
     130,     0,     0,   156,   316,   317,   318,   340,   320,   321,
     325,   324,   319,   322,     0,     0,     0,     0,   218,   265,
       0,     0,   260,   254,     0,     0,   259,     0,   287,   290,
     291,   274,   239,   237,   241,   243,   245,     0,   225,   142,
     143,     0,     0,     0,   230,   231,   253,   202,   203,   212,
       0,     0,   271,   271,     0,     0,     0,     0,   132,   120,
     127,   271,   125,     0,   128,   263,     0,     0,     0,     0,
     269,     0,     0,   173,     0,     0,     0,     0,   196,   194,
       0,     0,     0,     0,   266,     0,     0,     0,     0,     0,
     251,     0,     0,     0,   205,   214,   206,     0,     0,   216,
     267,   268,     0,     0,     0,   129,   269,     0,   163,     0,
     164,   166,   170,   162,     0,     0,     0,     0,   179,     0,
       0,     0,   182,   187,   174,   261,     0,     0,     0,   249,
       0,     0,     0,     0,   199,   249,     0,     0,     0,   266,
     235,   234,   201,     0,     0,   134,   124,     0,     0,   161,
       0,   168,   165,   219,   270,   333,   298,     0,   188,   177,
     192,     0,   190,   178,   186,   262,     0,     0,     0,     0,
     250,     0,     0,     0,     0,   246,     0,     0,     0,     0,
       0,   144,   145,   146,     0,     0,     0,     0,     0,   264,
       0,     0,     0,     0,     0,   176,     0,     0,   256,   255,
     257,   258,   240,   238,   242,   244,   227,   226,   228,   229,
     252,     0,     0,   208,   210,   122,   172,     0,     0,   167,
     169,     0,   185,   180,   189,     0,     0,   183,   233,   232,
     171,   181,   184,     0,   191
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     4,     6,    43,    46,    47,    48,    74,
      80,    86,   124,   129,   137,   130,    50,    51,   112,   113,
      52,   114,   139,   115,   131,    55,   117,    56,   116,   155,
     212,   157,   158,   194,   214,   159,   213,   119,   120,   121,
     249,    57,   164,   199,   220,   221,   222,   223,   256,   349,
     345,   348,   351,   352,   257,   344,   304,   392,   534,     7,
     173,   203,   204,   205,   206,   261,   207,   262,   320,   419,
     460,   541,   461,   567,   208,   371,   467,   471,   468,   472,
     543,   547,   209,   325,   324,   252,   387,   336,   339,   341,
     342,   338,   400,   253,   343,   462,   420,   287,   520,   394,
     333,   431,   430,   432,   433,   434,   521,   395,   489,   473,
     516,   376,   456,   404,   465,   242,   329,   243,   149,   211,
     150,   176,   210,   237,   326,   247,     9,    13,    14,   305,
     288,   289,   290,   291,   292,   293,   309,   310,   311,   312,
     313,   314,   315,   316,   266,   267,   248,   238,   269,   188,
     254,   396,   191,   244,   161,   197
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -524
static const yytype_int16 yypact[] =
{
    -524,    41,  -524,  -524,   -31,   106,  -524,  -524,  -524,    59,
     120,  -524,  -524,  1622,  1622,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,    65,    66,    72,
    -524,  -524,    76,    83,   136,   378,    77,  -524,  -524,   101,
    -524,   105,   114,   117,   119,   137,   145,  -524,  1622,  1184,
    -524,  -524,  -524,  -524,   149,  1622,  -524,   181,  -524,   368,
    -524,   121,  -524,   270,  -524,   101,   151,   154,   158,    28,
    -524,   101,   151,   168,   170,   131,  -524,   101,   158,   170,
     145,    82,  -524,   177,  -524,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  1686,  -524,  1622,  1622,    88,   186,  1622,   213,
     149,  -524,  -524,    67,  -524,   101,   197,   101,    73,  -524,
     101,   101,   158,    42,  -524,  -524,    78,  -524,   101,   101,
     170,    17,  -524,  -524,  -524,  -524,  -524,  -524,  -524,   218,
     752,  -524,   231,   409,  -524,  -524,  1622,   242,    56,  -524,
    -524,   259,  -524,  -524,   310,    55,   101,   101,  -524,   101,
    -524,   101,  -524,   277,  -524,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,   186,  -524,  -524,   259,   289,   171,
     101,    24,   290,   292,  -524,  -524,   295,   296,  -524,  -524,
     294,  1622,   320,   320,   335,  -524,  -524,   320,   248,  1081,
     317,  -524,   363,  -524,  -524,  -524,  1622,   153,   144,  -524,
    -524,   204,  -524,    79,  -524,  -524,   340,  -524,  -524,  -524,
    -524,  -524,   322,    35,  -524,   323,  -524,  -524,  -524,   320,
     324,   377,  -524,  -524,  -524,  1587,  -524,  -524,  -524,  -524,
    -524,  1622,   711,   248,  -524,  -524,  -524,  -524,  -524,   475,
    -524,  -524,  -524,  -524,   325,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,  1213,  -524,  -524,  -524,   248,  -524,
    -524,   248,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  1289,  -524,  -524,   342,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,   326,   684,  -524,
    -524,  -524,  -524,   330,  1275,  1275,    90,  -524,  -524,   320,
    -524,  -524,   444,  -524,  -524,  -524,  1177,   331,   836,  -524,
     248,   334,   338,  -524,  1497,   248,   409,   348,   898,   339,
    -524,   -24,  1081,  -524,   192,   196,   211,   364,   228,   238,
     245,   266,   272,   274,  1622,  1557,   284,   342,  -524,  -524,
     179,   351,  -524,  -524,   504,   352,  -524,   353,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,  1213,  -524,  -524,
    -524,   564,  1461,  1177,  -524,  -524,  -524,  -524,  -524,  -524,
     959,   362,   320,   320,   874,  1622,  1177,  1081,  -524,  -524,
    -524,   320,  -524,  1081,  -524,  -524,   379,   366,  1298,    32,
    -524,  1337,  1399,   296,  1275,  1275,  1275,  1275,  -524,  -524,
    1177,  1177,  1177,  1177,  -524,   367,  1177,  1177,  1177,  1177,
    -524,   386,  1497,   373,  -524,  -524,  -524,   375,   376,  -524,
    -524,  -524,   382,   383,   384,  -524,  -524,   385,  -524,   202,
    -524,  -524,  -524,  -524,  1298,  1057,   624,   391,  -524,   394,
     624,   395,  -524,  1275,  -524,  1275,  1275,  1275,  1275,  1177,
    1177,  1177,  1177,   935,  -524,  1177,  1177,  1177,  1177,  -524,
     244,   244,  -524,   248,   248,  -524,  -524,  1081,  1117,  -524,
    1557,  -524,  -524,  -524,  -524,  1360,  -524,   248,  -524,  -524,
     365,   248,  -524,  -524,  -524,  -524,   396,   397,   398,   399,
    -524,   403,   404,   405,   406,  -524,   426,   427,   432,   433,
     996,  -524,  -524,  -524,   392,  1497,   434,   435,   436,  -524,
     451,  1020,  1337,   812,   401,  -524,  1399,   282,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -524,
    -524,   452,  1497,  -524,  -524,  -524,  -524,   440,   692,  -524,
    -524,   442,  -524,  -524,  -524,   443,  1399,  -524,  -524,  -524,
    -524,  -524,  -524,   446,  -524
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -524,  -524,  -524,  -524,  -524,  -524,  -524,   471,   473,  -524,
    -524,   -23,   450,   -56,   -60,   279,   -12,   369,  -524,   343,
    -524,  -524,   178,  -524,   134,   -11,  -524,   -27,  -524,  -524,
    -524,   332,  -524,  -524,  -524,   380,  -524,   415,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,   314,  -524,  -524,  -334,  -524,
    -524,  -524,  -524,  -332,  -524,  -524,  -524,  -524,    48,  -524,
    -524,  -524,   341,   358,  -524,  -524,  -524,  -524,  -524,  -524,
      80,  -524,    -1,  -524,  -203,  -524,  -178,  -523,  -524,    68,
    -524,  -524,    60,  -524,  -524,  -257,  -524,  -524,  -524,  -524,
    -524,  -524,  -524,  -214,  -524,   283,  -524,  -524,  -144,   199,
     184,  -524,  -524,  -524,  -524,  -524,    87,  -335,  -524,  -304,
    -524,  -303,  -524,  -421,   118,  -201,  -524,  -524,  -105,  -524,
    -524,   333,  -524,  -524,  -524,  -524,  -524,  -524,  -524,  -308,
     318,  -247,  -524,  -524,  -524,   327,   257,  -524,  -524,  -524,
    -524,  -524,  -524,  -524,  -524,  -524,  -524,  -252,  -524,  -199,
    -151,  -149,   -13,  -240,  -524,   381
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -350
static const yytype_int16 yytable[] =
{
      38,    39,   189,   272,   234,   258,   323,   190,   307,   406,
     152,   239,   245,   483,   410,   366,   250,   327,   414,   413,
     375,   377,   134,   575,   142,    61,    78,    84,    90,   144,
      59,   337,   143,    92,   340,   418,    61,    68,    63,    64,
     350,     3,    77,    83,   335,    93,     5,    65,   274,   127,
     132,    62,   122,   583,   412,   127,   140,   442,    65,   224,
     225,   226,   416,    68,   227,   126,    64,   145,   530,    68,
     328,   126,    65,   453,   379,    68,   170,    63,    64,   455,
     172,   399,    62,   401,    64,    65,    61,   464,   407,    64,
      61,    62,   347,   179,   180,   181,   350,    65,   241,   167,
     294,   151,   151,    65,   294,   160,   240,   491,    65,     8,
     463,   322,    65,    68,   175,   168,   226,   469,    68,   227,
     475,   476,   477,   478,    11,    12,    68,   380,   381,    61,
      62,    63,    64,   156,   258,   193,    10,   177,   258,    61,
      62,    40,    64,   445,    41,   182,   183,   335,   235,   186,
      42,    65,   187,    44,    68,    66,   317,    68,   507,    68,
      45,    65,   511,   538,   451,   153,   154,   368,   378,   369,
      58,   515,   517,   518,   519,   369,   369,    54,    67,   224,
     225,   226,    69,   386,   227,    54,    54,    76,    68,    89,
      89,    71,   393,   258,    73,   294,    79,   540,   151,   258,
     562,   447,   448,   218,   219,   179,   180,   181,   264,   265,
     454,    65,   501,   263,    85,   415,   369,   421,   422,   508,
     474,    53,    91,   512,    89,   415,   118,   579,   123,    53,
      53,   128,    82,    88,    88,   133,   536,   537,   572,   572,
     225,   226,   440,   451,   227,   136,   277,   141,   151,   443,
     542,   179,   180,   181,   546,   146,   440,   182,   183,   184,
     185,   186,   452,   156,   187,  -327,  -327,   322,    88,  -328,
    -328,   334,   369,   369,   165,   369,   369,   369,   369,    62,
      63,    64,   500,   258,  -329,  -329,   479,   480,   481,   482,
     451,   162,   485,   486,   487,   488,   174,   179,   180,   181,
      65,  -347,  -347,   182,   183,   184,   185,   186,   368,   178,
     187,  -348,  -348,   322,   531,   532,   533,   415,  -330,  -330,
     192,   415,    49,   196,   369,   251,   369,   369,   369,   369,
      49,    49,    75,    81,    87,    87,   522,   523,   524,  -331,
    -331,   198,   527,   528,   529,  -332,  -332,  -349,  -349,   182,
     183,   369,   235,   186,   201,   125,   187,   389,   390,   576,
     577,   125,   138,   138,   571,   574,   217,   228,   138,    87,
     230,   236,   231,   233,   334,    60,    61,    62,    63,    64,
     179,   180,   181,   241,    59,    60,    61,    62,    63,    64,
     322,   450,   246,   369,   369,   259,   219,   369,    65,   268,
     270,   273,   275,   330,   353,   166,   350,   370,    65,   397,
     169,   402,   179,   180,   181,   403,   411,   368,   171,   138,
     276,   277,   278,   279,   280,   281,   408,   369,  -303,   423,
     428,   429,   182,   183,   184,   185,   186,   282,   283,   187,
     446,   227,   544,   457,   458,   484,   200,    15,    16,    17,
     490,   492,   504,   493,   494,   148,   561,   284,   285,   286,
     495,   496,   497,   499,   182,   183,   184,   185,   186,   509,
     450,   187,   510,   513,   548,   549,   550,   551,   179,   180,
     181,   552,   553,   554,   555,   504,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,   556,   557,    36,    15,    16,    17,
     558,   559,   563,   564,   565,   566,   578,   450,   580,    37,
     581,   582,    70,   382,   584,    72,   215,   383,   384,   385,
     182,   183,   135,   235,   186,   163,   260,   187,   195,   535,
     570,   514,   202,   229,   502,   321,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,   232,   417,    36,    15,    16,    17,
     545,   435,   526,   306,   498,   367,   271,     0,   216,    37,
       0,     0,   308,   424,     0,     0,     0,   425,   426,   427,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,     0,     0,    36,    15,    16,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    37,
       0,     0,     0,   436,     0,     0,     0,   437,   438,   439,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   505,    19,   506,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,     0,     0,    36,   354,   355,   356,
       0,     0,     0,     0,     0,   179,   180,   181,     0,    37,
       0,     0,     0,   424,     0,     0,     0,   425,   426,   427,
       0,     0,     0,     0,   179,   180,   181,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    18,    19,    20,    21,
      22,    23,    24,   357,    26,    27,    28,    29,    30,   358,
     359,   360,   361,   362,     0,     0,   363,   182,   183,   184,
     185,   186,     0,     0,   187,    15,    16,    17,     0,    37,
       0,   364,     0,     0,   365,     0,   182,   183,   184,   185,
     186,     0,   500,   187,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   318,   319,
       0,     0,     0,     0,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,     0,     0,    36,    15,    16,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    37,     0,     0,
       0,   175,     0,     0,     0,     0,     0,     0,     0,   179,
     180,   181,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   372,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,     0,     0,    36,     0,   373,    15,    16,    17,
       0,     0,     0,     0,     0,     0,     0,    37,     0,   466,
     573,   182,   183,   184,   185,   186,     0,     0,   187,     0,
       0,   179,   180,   181,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   251,   398,     0,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,     0,     0,    36,   241,    15,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,    37,
       0,     0,   449,   182,   183,   184,   185,   186,     0,     0,
     187,     0,   179,   180,   181,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   255,   409,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,    36,   241,    15,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
      37,     0,     0,   525,   182,   183,   184,   185,   186,     0,
       0,   187,     0,   179,   180,   181,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   251,   444,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     0,     0,    36,   241,
      15,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,    37,     0,     0,   560,   182,   183,   184,   185,   186,
       0,     0,   187,     0,   179,   180,   181,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   568,   569,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
      15,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    37,     0,     0,   503,   182,   183,   184,   185,
     186,     0,     0,   187,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   255,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
      15,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    37,     0,     0,   539,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,     0,     0,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
       0,   388,     0,     0,     0,     0,     0,     0,     0,     0,
     389,   390,    37,     0,   391,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,     0,    36,   241,   331,    15,    16,
      17,     0,     0,     0,     0,     0,     0,     0,    37,     0,
     332,     0,   179,   180,   181,     0,     0,     0,     0,     0,
       0,   179,   180,   181,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   372,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,    36,     0,   373,
      15,    16,    17,     0,   182,   183,   184,   185,   186,     0,
      37,   187,   374,   182,   183,   184,   185,   186,     0,     0,
     187,     0,     0,  -296,  -296,  -296,   346,     0,     0,     0,
       0,     0,     0,     0,     0,   459,     0,     0,   372,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
       0,   373,    15,    16,    17,     0,     0,     0,     0,     0,
       0,     0,    37,     0,   466,  -296,  -296,  -296,  -296,  -296,
       0,     0,  -296,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -296,     0,     0,
     372,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,     0,
       0,    36,     0,   373,    15,    16,    17,     0,     0,     0,
       0,     0,     0,     0,    37,     0,   470,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,     0,     0,    36,     0,   441,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    37,     0,   405,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
      15,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    37,     0,   405,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     179,   180,   181,     0,     0,     0,     0,     0,     0,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
       0,     0,     0,     0,     0,    15,    16,    17,     0,   295,
     276,   277,    37,     0,   364,   281,   296,   297,   298,   299,
     300,   301,   182,   183,   184,   185,   186,     0,     0,   187,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   302,   303,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,     0,     0,    36,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    37,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   147
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-524)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      13,    14,   153,   243,   207,   219,   263,   156,   255,   344,
     115,   210,   213,   434,   348,   318,   217,   269,   352,   351,
     324,   325,    78,   546,    84,     8,    53,    54,    55,    89,
       6,   288,    88,    56,   291,   367,     8,    49,    10,    11,
      64,     0,    53,    54,   284,    58,    77,    30,   249,    76,
      77,     9,    65,   576,    78,    82,    83,   392,    30,    35,
      36,    37,   365,    75,    40,    76,    11,    90,   489,    81,
     269,    82,    30,   407,   326,    87,   132,    10,    11,   413,
     140,   338,     9,   340,    11,    30,     8,   419,   345,    11,
       8,     9,   306,     3,     4,     5,    64,    30,    63,   126,
     251,   114,   115,    30,   255,   118,   211,   442,    30,     3,
      78,   262,    30,   125,    79,   127,    37,   421,   130,    40,
     424,   425,   426,   427,     4,     5,   138,   326,   329,     8,
       9,    10,    11,    77,   348,    79,    77,   150,   352,     8,
       9,    76,    11,   400,    78,    55,    56,   387,    58,    59,
      78,    30,    62,    77,   166,    78,   261,   169,   466,   171,
      77,    30,   470,   497,   404,    77,    78,   318,    78,   318,
      34,   475,   476,   477,   478,   324,   325,    43,    77,    35,
      36,    37,    77,   332,    40,    51,    52,    53,   200,    55,
      56,    77,   336,   407,    77,   346,    77,   500,   211,   413,
     535,   402,   403,    32,    33,     3,     4,     5,    55,    56,
     411,    30,   459,   226,    77,   364,   365,    38,    39,   466,
     423,    43,    77,   470,    90,   374,    77,   562,    77,    51,
      52,    77,    54,    55,    56,    77,   493,   494,   542,   543,
      36,    37,   391,   483,    40,    77,    44,    77,   261,   393,
     507,     3,     4,     5,   511,    78,   405,    55,    56,    57,
      58,    59,   406,    77,    62,    73,    74,   418,    90,    73,
      74,   284,   421,   422,    77,   424,   425,   426,   427,     9,
      10,    11,    80,   497,    73,    74,   430,   431,   432,   433,
     530,    78,   436,   437,   438,   439,    78,     3,     4,     5,
      30,    73,    74,    55,    56,    57,    58,    59,   459,    78,
      62,    73,    74,   464,    70,    71,    72,   466,    73,    74,
      78,   470,    43,    64,   473,    77,   475,   476,   477,   478,
      51,    52,    53,    54,    55,    56,   480,   481,   482,    73,
      74,    31,   486,   487,   488,    73,    74,    73,    74,    55,
      56,   500,    58,    59,    77,    76,    62,    73,    74,    77,
      78,    82,    83,    84,   542,   543,    77,    77,    89,    90,
      78,    77,    77,    77,   387,     7,     8,     9,    10,    11,
       3,     4,     5,    63,     6,     7,     8,     9,    10,    11,
     541,   404,    57,   542,   543,    78,    33,   546,    30,    59,
      78,    78,    78,    78,    78,   126,    64,    77,    30,    78,
     131,    77,     3,     4,     5,    77,    77,   568,   139,   140,
      43,    44,    45,    46,    47,    48,    78,   576,    64,    78,
      78,    78,    55,    56,    57,    58,    59,    60,    61,    62,
      78,    40,    77,    64,    78,    78,   167,     3,     4,     5,
      64,    78,   465,    78,    78,   112,    64,    80,    81,    82,
      78,    78,    78,    78,    55,    56,    57,    58,    59,    78,
     483,    62,    78,    78,    78,    78,    78,    78,     3,     4,
       5,    78,    78,    78,    78,   498,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    78,    78,    62,     3,     4,     5,
      78,    78,    78,    78,    78,    64,    64,   530,    78,    75,
      78,    78,    51,    79,    78,    52,   194,    83,    84,    85,
      55,    56,    82,    58,    59,   120,   222,    62,   158,   491,
     541,   473,   173,   202,   464,   262,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,   206,   366,    62,     3,     4,     5,
     510,   387,   485,   255,   456,   318,   243,    -1,   197,    75,
      -1,    -1,   255,    79,    -1,    -1,    -1,    83,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,     3,     4,     5,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,
      -1,    -1,    -1,    79,    -1,    -1,    -1,    83,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,     3,     4,     5,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,    75,
      -1,    -1,    -1,    79,    -1,    -1,    -1,    83,    84,    85,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    55,    56,    57,
      58,    59,    -1,    -1,    62,     3,     4,     5,    -1,    75,
      -1,    77,    -1,    -1,    80,    -1,    55,    56,    57,    58,
      59,    -1,    80,    62,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    78,
      -1,    -1,    -1,    -1,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,     3,     4,     5,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,    -1,
      -1,    79,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    -1,    64,     3,     4,     5,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,    77,
      78,    55,    56,    57,    58,    59,    -1,    -1,    62,    -1,
      -1,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    77,    78,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,     3,     4,
       5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,
      -1,    -1,    78,    55,    56,    57,    58,    59,    -1,    -1,
      62,    -1,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    77,    78,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,     3,
       4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      75,    -1,    -1,    78,    55,    56,    57,    58,    59,    -1,
      -1,    62,    -1,     3,     4,     5,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    77,    78,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
       3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    75,    -1,    -1,    78,    55,    56,    57,    58,    59,
      -1,    -1,    62,    -1,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    78,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
       3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    -1,    -1,    78,    55,    56,    57,    58,
      59,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
       3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    -1,    -1,    78,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    -1,    -1,     3,     4,     5,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      -1,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    -1,    77,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    64,     3,     4,
       5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,
      77,    -1,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    -1,    64,
       3,     4,     5,    -1,    55,    56,    57,    58,    59,    -1,
      75,    62,    77,    55,    56,    57,    58,    59,    -1,    -1,
      62,    -1,    -1,     3,     4,     5,    77,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      -1,    64,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    -1,    77,    55,    56,    57,    58,    59,
      -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    -1,    64,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    -1,    77,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    -1,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,    77,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
       3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    -1,    77,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,    42,
      43,    44,    75,    -1,    77,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    87,    88,     0,    89,    77,    90,   145,     3,   212,
      77,     4,     5,   213,   214,     3,     4,     5,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    62,    75,   238,   238,
      76,    78,    78,    91,    77,    77,    92,    93,    94,   101,
     102,   103,   106,   108,   110,   111,   113,   127,    34,     6,
       7,     8,     9,    10,    11,    30,    78,    77,   102,    77,
      93,    77,    94,    77,    95,   101,   110,   111,   113,    77,
      96,   101,   108,   111,   113,    77,    97,   101,   108,   110,
     113,    77,    97,   238,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,   104,   105,   107,   109,   114,   112,    77,   123,
     124,   125,   238,    77,    98,   101,   111,   113,    77,    99,
     101,   110,   113,    77,    99,    98,    77,   100,   101,   108,
     113,    77,   100,    99,   100,    97,    78,    78,   105,   204,
     206,   238,   204,    77,    78,   115,    77,   117,   118,   121,
     238,   240,    78,   123,   128,    77,   101,   113,   102,   101,
      99,   101,   100,   146,    78,    79,   207,   238,    78,     3,
       4,     5,    55,    56,    57,    58,    59,    62,   235,   236,
     237,   238,    78,    79,   119,   121,    64,   241,    31,   129,
     101,    77,   103,   147,   148,   149,   150,   152,   160,   168,
     208,   205,   116,   122,   120,   117,   241,    77,    32,    33,
     130,   131,   132,   133,    35,    36,    37,    40,    77,   148,
      78,    77,   149,    77,   160,    58,    77,   209,   233,   235,
     204,    63,   201,   203,   239,   201,    57,   211,   232,   126,
     201,    77,   171,   179,   236,    77,   134,   140,   179,    78,
     131,   151,   153,   238,    55,    56,   230,   231,    59,   234,
      78,   207,   239,    78,   201,    78,    43,    44,    45,    46,
      47,    48,    60,    61,    80,    81,    82,   183,   216,   217,
     218,   219,   220,   221,   236,    42,    49,    50,    51,    52,
      53,    54,    75,    76,   142,   215,   216,   217,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   204,    77,    78,
     154,   181,   236,   171,   170,   169,   210,   233,   235,   202,
      78,    64,    77,   186,   238,   239,   173,   171,   177,   174,
     171,   175,   176,   180,   141,   136,    77,   179,   137,   135,
      64,   138,   139,    78,     3,     4,     5,    49,    55,    56,
      57,    58,    59,    62,    77,    80,   197,   222,   236,   237,
      77,   161,    41,    64,    77,   195,   197,   195,    78,   233,
     235,   201,    79,    83,    84,    85,   237,   172,    64,    73,
      74,    77,   143,   184,   185,   193,   237,    78,    78,   171,
     178,   171,    77,    77,   199,    77,   193,   171,    78,    78,
     134,    77,    78,   139,   134,   237,   197,   185,   139,   155,
     182,    38,    39,    78,    79,    83,    84,    85,    78,    78,
     188,   187,   189,   190,   191,   186,    79,    83,    84,    85,
     237,    64,   193,   184,    78,   171,    78,   201,   201,    78,
     238,   239,   184,   134,   201,   134,   198,    64,    78,    77,
     156,   158,   181,    78,   139,   200,    77,   162,   164,   195,
      77,   163,   165,   195,   160,   195,   195,   195,   195,   184,
     184,   184,   184,   199,    78,   184,   184,   184,   184,   194,
      64,   193,    78,    78,    78,    78,    78,    78,   200,    78,
      80,   217,   156,    78,   238,    42,    44,   215,   217,    78,
      78,   215,   217,    78,   165,   195,   196,   195,   195,   195,
     184,   192,   184,   184,   184,    78,   192,   184,   184,   184,
     199,    70,    71,    72,   144,   144,   171,   171,   134,    78,
     197,   157,   171,   166,    77,   168,   171,   167,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    64,   193,    78,    78,    78,    64,   159,    77,    78,
     158,   162,   195,    78,   162,   163,    77,    78,    64,   193,
      78,    78,    78,   163,    78
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YYUSE (yytype);
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1787 of yacc.c  */
#line 326 "parser.yy"
    { success = true; line_number = 1; }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 327 "parser.yy"
    { if (!success) YYERROR; }
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 339 "parser.yy"
    { make_domain((yyvsp[(5) - (6)].str)); }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 429 "parser.yy"
    { requirements->strips = true; }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 430 "parser.yy"
    { requirements->typing = true; }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 432 "parser.yy"
    { requirements->negative_preconditions = true; }
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 434 "parser.yy"
    { requirements->disjunctive_preconditions = true; }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 435 "parser.yy"
    { requirements->equality = true; }
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 437 "parser.yy"
    { requirements->existential_preconditions = true; }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 439 "parser.yy"
    { requirements->universal_preconditions = true; }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 441 "parser.yy"
    { requirements->quantified_preconditions(); }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 442 "parser.yy"
    { requirements->conditional_effects = true; }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 443 "parser.yy"
    { requirements->fluents = true; }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 444 "parser.yy"
    { requirements->adl(); }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 446 "parser.yy"
    { throw Exception("`:durative-actions' not supported"); }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 448 "parser.yy"
    { throw Exception("`:duration-inequalities' not supported"); }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 450 "parser.yy"
    { throw Exception("`:continuous-effects' not supported"); }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 452 "parser.yy"
    {
		  requirements->probabilistic_effects = true;
		  goal_prob_function =
		    domain->functions().add_function("goal-probability");
		}
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 458 "parser.yy"
    {
		  requirements->rewards = true;
		  reward_function = domain->functions().add_function("reward");
		}
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 463 "parser.yy"
    {
		  requirements->probabilistic_effects = true;
		  requirements->rewards = true;
		  goal_prob_function =
		    domain->functions().add_function("goal-probability");
		  reward_function = domain->functions().add_function("reward");
		}
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 471 "parser.yy"
    {
		  requirements->probabilistic_effects = true;
		  requirements->rewards = true;
		  goal_prob_function =
		    domain->functions().add_function("goal-probability");
		  reward_function = domain->functions().add_function("reward");
		}
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 481 "parser.yy"
    { require_typing(); name_kind = TYPE_KIND; }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 482 "parser.yy"
    { name_kind = VOID_KIND; }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 485 "parser.yy"
    { name_kind = CONSTANT_KIND; }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 486 "parser.yy"
    { name_kind = VOID_KIND; }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 492 "parser.yy"
    { require_fluents(); }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 504 "parser.yy"
    { make_predicate((yyvsp[(2) - (2)].str)); }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 505 "parser.yy"
    { parsing_predicate = false; }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 517 "parser.yy"
    { require_typing(); }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 520 "parser.yy"
    { make_function((yyvsp[(2) - (2)].str)); }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 521 "parser.yy"
    { parsing_function = false; }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 535 "parser.yy"
    { make_cvariable ((yyvsp[(2) - (4)].str), (yyvsp[(3) - (4)].num), (yyvsp[(4) - (4)].num)); }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 536 "parser.yy"
    {parsing_function = false;}
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 542 "parser.yy"
    { make_action((yyvsp[(3) - (3)].str)); }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 543 "parser.yy"
    { add_action(); }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 558 "parser.yy"
    { action->set_precondition(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 118:
/* Line 1787 of yacc.c  */
#line 561 "parser.yy"
    { action->set_effect(*(yyvsp[(2) - (2)].effect)); }
    break;

  case 120:
/* Line 1787 of yacc.c  */
#line 569 "parser.yy"
    { (yyval.effect) = (yyvsp[(3) - (4)].ceffect); }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 570 "parser.yy"
    { prepare_forall_effect(); }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 571 "parser.yy"
    { (yyval.effect) = make_forall_effect(*(yyvsp[(7) - (8)].effect)); }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 572 "parser.yy"
    { require_conditional_effects(); }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 573 "parser.yy"
    { (yyval.effect) = &ConditionalEffect::make(*(yyvsp[(4) - (6)].formula), *(yyvsp[(5) - (6)].effect)); }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 574 "parser.yy"
    { (yyval.effect) = (yyvsp[(3) - (4)].peffect); }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 577 "parser.yy"
    { (yyval.ceffect) = new ConjunctiveEffect(); }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 578 "parser.yy"
    { (yyval.ceffect) = (yyvsp[(1) - (2)].ceffect); (yyval.ceffect)->add_conjunct(*(yyvsp[(2) - (2)].effect)); }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 582 "parser.yy"
    {
		(yyval.peffect) = new ProbabilisticEffect();
		add_effect_outcome(*(yyval.peffect), (yyvsp[(1) - (2)].num), *(yyvsp[(2) - (2)].effect));
	      }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 587 "parser.yy"
    { (yyval.peffect) = (yyvsp[(1) - (3)].peffect);add_effect_outcome(*(yyval.peffect), (yyvsp[(2) - (3)].num), *(yyvsp[(3) - (3)].effect)); }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 593 "parser.yy"
    { (yyval.effect) = make_add_effect(*(yyvsp[(1) - (1)].atom)); }
    break;

  case 132:
/* Line 1787 of yacc.c  */
#line 594 "parser.yy"
    { (yyval.effect) = make_delete_effect(*(yyvsp[(3) - (4)].atom)); }
    break;

  case 133:
/* Line 1787 of yacc.c  */
#line 597 "parser.yy"
    { effect_fluent = true; }
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 598 "parser.yy"
    { (yyval.effect) = make_assignment_effect((yyvsp[(2) - (6)].setop), *(yyvsp[(4) - (6)].appl), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 135:
/* Line 1787 of yacc.c  */
#line 601 "parser.yy"
    { (yyval.setop) = Assignment::ASSIGN_OP; }
    break;

  case 136:
/* Line 1787 of yacc.c  */
#line 602 "parser.yy"
    { (yyval.setop) = Assignment::SCALE_UP_OP; }
    break;

  case 137:
/* Line 1787 of yacc.c  */
#line 603 "parser.yy"
    { (yyval.setop) = Assignment::SCALE_DOWN_OP; }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 604 "parser.yy"
    { (yyval.setop) = Assignment::INCREASE_OP; }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 605 "parser.yy"
    { (yyval.setop) = Assignment::DECREASE_OP; }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 606 "parser.yy"
    {(yyval.setop) = Assignment::INCREASE_PROB_OP; }
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 607 "parser.yy"
    {(yyval.setop) = Assignment::DECREASE_PROB_OP; }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 611 "parser.yy"
    { (yyval.pdisttype) = DISTRIBUTION_NORMAL; }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 613 "parser.yy"
    { (yyval.pdisttype) = DISTRIBUTION_UNIFORM; }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 618 "parser.yy"
    { (yyval.pdiscztype) = DISCRETIZATION_POINTS; }
    break;

  case 145:
/* Line 1787 of yacc.c  */
#line 620 "parser.yy"
    { (yyval.pdiscztype) = DISCRETIZATION_THRESHOLD; }
    break;

  case 146:
/* Line 1787 of yacc.c  */
#line 622 "parser.yy"
    { (yyval.pdiscztype) = DISCRETIZATION_INTERVAL; }
    break;

  case 147:
/* Line 1787 of yacc.c  */
#line 630 "parser.yy"
    { make_problem((yyvsp[(5) - (10)].str), (yyvsp[(9) - (10)].str)); }
    break;

  case 148:
/* Line 1787 of yacc.c  */
#line 631 "parser.yy"
    { problem->instantiate_actions(); delete requirements; }
    break;

  case 155:
/* Line 1787 of yacc.c  */
#line 646 "parser.yy"
    { name_kind = OBJECT_KIND; }
    break;

  case 156:
/* Line 1787 of yacc.c  */
#line 647 "parser.yy"
    { name_kind = VOID_KIND; }
    break;

  case 160:
/* Line 1787 of yacc.c  */
#line 657 "parser.yy"
    { problem->add_init_atom(*(yyvsp[(1) - (1)].atom)); }
    break;

  case 161:
/* Line 1787 of yacc.c  */
#line 659 "parser.yy"
    { problem->add_init_value(*(yyvsp[(3) - (5)].appl), *(yyvsp[(4) - (5)].num)); delete (yyvsp[(4) - (5)].num); }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 661 "parser.yy"
    { problem->add_init_effect(*(yyvsp[(3) - (4)].peffect)); }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 663 "parser.yy"
    { problem->add_init_prob_dist ((yyvsp[(2) - (4)].appl), (yyvsp[(3) - (4)].expr));
  /* set a 'static' initial continuous variable value as the MAX bound of the variable.
     This is used only for testing actions applicability in the parser tests,
     and the hmdp internal full engine. Not in CAO*. */
  if (problem->domain ().functions ().isCVariable ((yyvsp[(2) - (4)].appl)->function ()))
    {
      const std::string &rscname 
	= problem->domain ().functions ().name ((yyvsp[(2) - (4)].appl)->function ());
      double rsc_max_value
	= problem->domain ().functions ().getCVariableBounds (rscname).second;
      problem->add_init_value (*(yyvsp[(2) - (4)].appl), Rational (rsc_max_value));
    }
  else yyerror ("Initial state probability distribution must be over a continuous variable.");
}
    break;

  case 164:
/* Line 1787 of yacc.c  */
#line 680 "parser.yy"
    {
		 (yyval.peffect) = new ProbabilisticEffect();
		 add_effect_outcome(*(yyval.peffect), (yyvsp[(1) - (2)].num), *(yyvsp[(2) - (2)].effect));
	       }
    break;

  case 165:
/* Line 1787 of yacc.c  */
#line 685 "parser.yy"
    { (yyval.peffect) = (yyvsp[(1) - (3)].peffect); add_effect_outcome(*(yyval.peffect), (yyvsp[(2) - (3)].num), *(yyvsp[(3) - (3)].effect)); }
    break;

  case 167:
/* Line 1787 of yacc.c  */
#line 689 "parser.yy"
    { (yyval.effect) = (yyvsp[(3) - (4)].ceffect); }
    break;

  case 168:
/* Line 1787 of yacc.c  */
#line 692 "parser.yy"
    { (yyval.ceffect) = new ConjunctiveEffect(); }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 693 "parser.yy"
    { (yyval.ceffect) = (yyvsp[(1) - (2)].ceffect); (yyval.ceffect)->add_conjunct(*(yyvsp[(2) - (2)].effect)); }
    break;

  case 170:
/* Line 1787 of yacc.c  */
#line 696 "parser.yy"
    { (yyval.effect) = make_add_effect(*(yyvsp[(1) - (1)].atom)); }
    break;

  case 171:
/* Line 1787 of yacc.c  */
#line 698 "parser.yy"
    { (yyval.effect) = make_assignment_effect(Assignment::ASSIGN_OP, *(yyvsp[(3) - (5)].appl), *(yyvsp[(4) - (5)].expr)); }
    break;

  case 172:
/* Line 1787 of yacc.c  */
#line 701 "parser.yy"
    { (yyval.expr) = new Value(*(yyvsp[(1) - (1)].num)); delete (yyvsp[(1) - (1)].num); }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 704 "parser.yy"
    { add_goal (*(yyvsp[(3) - (6)].str), *(yyvsp[(4) - (6)].formula), *(yyvsp[(5) - (6)].greward)); }
    break;

  case 174:
/* Line 1787 of yacc.c  */
#line 706 "parser.yy"
    { add_goal (*(yyvsp[(3) - (7)].str), *(yyvsp[(4) - (7)].formula), *(yyvsp[(5) - (7)].greward)); }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 712 "parser.yy"
    { (yyval.greward) = make_goal_reward(*(yyvsp[(3) - (5)].expr)); }
    break;

  case 177:
/* Line 1787 of yacc.c  */
#line 714 "parser.yy"
    { (yyval.greward) = (yyvsp[(3) - (4)].greward); }
    break;

  case 178:
/* Line 1787 of yacc.c  */
#line 716 "parser.yy"
    { (yyval.greward) = (yyvsp[(3) - (4)].greward); }
    break;

  case 179:
/* Line 1787 of yacc.c  */
#line 719 "parser.yy"
    { (yyval.greward) = (yyvsp[(1) - (1)].greward); }
    break;

  case 180:
/* Line 1787 of yacc.c  */
#line 720 "parser.yy"
    { (yyval.greward) = (yyvsp[(3) - (4)].cgreward); }
    break;

  case 181:
/* Line 1787 of yacc.c  */
#line 722 "parser.yy"
    { (yyval.greward) = &ConditionalGoalReward::make (*(yyvsp[(3) - (5)].formula), *(yyvsp[(4) - (5)].greward)); }
    break;

  case 182:
/* Line 1787 of yacc.c  */
#line 725 "parser.yy"
    { (yyval.greward) = (yyvsp[(1) - (1)].greward); }
    break;

  case 183:
/* Line 1787 of yacc.c  */
#line 726 "parser.yy"
    { (yyval.greward) = (yyvsp[(3) - (4)].cgreward); }
    break;

  case 184:
/* Line 1787 of yacc.c  */
#line 728 "parser.yy"
    { (yyval.greward) = &ConditionalGoalReward::make (*(yyvsp[(3) - (5)].formula), *(yyvsp[(4) - (5)].greward)); }
    break;

  case 185:
/* Line 1787 of yacc.c  */
#line 732 "parser.yy"
    { (yyval.greward) = make_goal_reward (*(yyvsp[(1) - (1)].expr)); }
    break;

  case 186:
/* Line 1787 of yacc.c  */
#line 736 "parser.yy"
    { add_linear_reward_comp (*(yyvsp[(1) - (2)].expr), (yyvsp[(2) - (2)].greward)); 
 (yyval.greward) = (yyvsp[(2) - (2)].greward); }
    break;

  case 187:
/* Line 1787 of yacc.c  */
#line 739 "parser.yy"
    { (yyval.greward) = make_goal_linear_reward (*(yyvsp[(1) - (1)].expr)); }
    break;

  case 188:
/* Line 1787 of yacc.c  */
#line 742 "parser.yy"
    { (yyval.cgreward) = new ConjunctiveGoalReward ();}
    break;

  case 189:
/* Line 1787 of yacc.c  */
#line 743 "parser.yy"
    {(yyval.cgreward) = (yyvsp[(1) - (2)].cgreward); (yyval.cgreward)->add_conjunct (*(yyvsp[(2) - (2)].greward)); }
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 746 "parser.yy"
    { (yyval.cgreward) = new ConjunctiveGoalReward (); }
    break;

  case 191:
/* Line 1787 of yacc.c  */
#line 747 "parser.yy"
    { (yyval.cgreward) = (yyvsp[(1) - (4)].cgreward); (yyval.cgreward)->add_conjunct (*(yyvsp[(3) - (4)].greward)); }
    break;

  case 192:
/* Line 1787 of yacc.c  */
#line 750 "parser.yy"
    { set_default_metric(); }
    break;

  case 193:
/* Line 1787 of yacc.c  */
#line 751 "parser.yy"
    { metric_fluent = true; }
    break;

  case 194:
/* Line 1787 of yacc.c  */
#line 752 "parser.yy"
    { problem->set_metric(*(yyvsp[(5) - (6)].expr)); metric_fluent = false; }
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 753 "parser.yy"
    { metric_fluent = true; }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 754 "parser.yy"
    { problem->set_metric(*(yyvsp[(5) - (6)].expr), true); metric_fluent = false; }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 760 "parser.yy"
    { (yyval.formula) = (yyvsp[(1) - (1)].atom); }
    break;

  case 198:
/* Line 1787 of yacc.c  */
#line 762 "parser.yy"
    { first_eq_term = eq_term; first_eq_expr = eq_expr; }
    break;

  case 199:
/* Line 1787 of yacc.c  */
#line 763 "parser.yy"
    { (yyval.formula) = make_equality(); }
    break;

  case 200:
/* Line 1787 of yacc.c  */
#line 764 "parser.yy"
    { require_fluents(); }
    break;

  case 201:
/* Line 1787 of yacc.c  */
#line 765 "parser.yy"
    { (yyval.formula) = new Comparison((yyvsp[(2) - (6)].comp), *(yyvsp[(4) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 202:
/* Line 1787 of yacc.c  */
#line 766 "parser.yy"
    { (yyval.formula) = make_negation(*(yyvsp[(3) - (4)].formula)); }
    break;

  case 203:
/* Line 1787 of yacc.c  */
#line 767 "parser.yy"
    { (yyval.formula) = (yyvsp[(3) - (4)].conj); }
    break;

  case 204:
/* Line 1787 of yacc.c  */
#line 768 "parser.yy"
    { require_disjunction(); }
    break;

  case 205:
/* Line 1787 of yacc.c  */
#line 768 "parser.yy"
    { (yyval.formula) = (yyvsp[(4) - (5)].disj); }
    break;

  case 206:
/* Line 1787 of yacc.c  */
#line 769 "parser.yy"
    { (yyval.formula) = make_implication(*(yyvsp[(3) - (5)].formula), *(yyvsp[(4) - (5)].formula)); }
    break;

  case 207:
/* Line 1787 of yacc.c  */
#line 770 "parser.yy"
    { prepare_exists(); }
    break;

  case 208:
/* Line 1787 of yacc.c  */
#line 771 "parser.yy"
    { (yyval.formula) = make_exists(*(yyvsp[(7) - (8)].formula)); }
    break;

  case 209:
/* Line 1787 of yacc.c  */
#line 772 "parser.yy"
    { prepare_forall(); }
    break;

  case 210:
/* Line 1787 of yacc.c  */
#line 773 "parser.yy"
    { (yyval.formula) = make_forall(*(yyvsp[(7) - (8)].formula)); }
    break;

  case 211:
/* Line 1787 of yacc.c  */
#line 776 "parser.yy"
    { (yyval.conj) = new Conjunction(); }
    break;

  case 212:
/* Line 1787 of yacc.c  */
#line 777 "parser.yy"
    { (yyval.conj)->add_conjunct(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 213:
/* Line 1787 of yacc.c  */
#line 780 "parser.yy"
    { (yyval.disj) = new Disjunction(); }
    break;

  case 214:
/* Line 1787 of yacc.c  */
#line 781 "parser.yy"
    { (yyval.disj)->add_disjunct(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 215:
/* Line 1787 of yacc.c  */
#line 784 "parser.yy"
    { prepare_atom((yyvsp[(2) - (2)].str)); }
    break;

  case 216:
/* Line 1787 of yacc.c  */
#line 785 "parser.yy"
    { (yyval.atom) = make_atom(); }
    break;

  case 217:
/* Line 1787 of yacc.c  */
#line 786 "parser.yy"
    { prepare_atom((yyvsp[(1) - (1)].str)); (yyval.atom) = make_atom(); }
    break;

  case 218:
/* Line 1787 of yacc.c  */
#line 789 "parser.yy"
    { prepare_atom((yyvsp[(2) - (2)].str)); }
    break;

  case 219:
/* Line 1787 of yacc.c  */
#line 790 "parser.yy"
    { (yyval.atom) = make_atom(); }
    break;

  case 220:
/* Line 1787 of yacc.c  */
#line 791 "parser.yy"
    { prepare_atom((yyvsp[(1) - (1)].str)); (yyval.atom) = make_atom(); }
    break;

  case 221:
/* Line 1787 of yacc.c  */
#line 794 "parser.yy"
    { (yyval.comp) = Comparison::LT_CMP; }
    break;

  case 222:
/* Line 1787 of yacc.c  */
#line 795 "parser.yy"
    { (yyval.comp) = Comparison::LE_CMP; }
    break;

  case 223:
/* Line 1787 of yacc.c  */
#line 796 "parser.yy"
    { (yyval.comp) = Comparison::GE_CMP; }
    break;

  case 224:
/* Line 1787 of yacc.c  */
#line 797 "parser.yy"
    {(yyval.comp) = Comparison::GT_CMP; }
    break;

  case 225:
/* Line 1787 of yacc.c  */
#line 804 "parser.yy"
    { (yyval.expr) = new Value(*(yyvsp[(1) - (1)].num)); delete (yyvsp[(1) - (1)].num); }
    break;

  case 226:
/* Line 1787 of yacc.c  */
#line 805 "parser.yy"
    { (yyval.expr) = new Addition(*(yyvsp[(3) - (5)].expr), *(yyvsp[(4) - (5)].expr)); }
    break;

  case 227:
/* Line 1787 of yacc.c  */
#line 806 "parser.yy"
    { (yyval.expr) = make_subtraction(*(yyvsp[(3) - (5)].expr), (yyvsp[(4) - (5)].expr)); }
    break;

  case 228:
/* Line 1787 of yacc.c  */
#line 807 "parser.yy"
    { (yyval.expr) = new Multiplication(*(yyvsp[(3) - (5)].expr), *(yyvsp[(4) - (5)].expr)); }
    break;

  case 229:
/* Line 1787 of yacc.c  */
#line 808 "parser.yy"
    { (yyval.expr) = new Division(*(yyvsp[(3) - (5)].expr), *(yyvsp[(4) - (5)].expr)); }
    break;

  case 230:
/* Line 1787 of yacc.c  */
#line 809 "parser.yy"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); }
    break;

  case 231:
/* Line 1787 of yacc.c  */
#line 810 "parser.yy"
    { (yyval.expr) = (yyvsp[(1) - (1)].appl); }
    break;

  case 232:
/* Line 1787 of yacc.c  */
#line 815 "parser.yy"
    {(yyval.expr) = new ProbabilisticApplication ((yyvsp[(1) - (6)].pdisttype), *(yyvsp[(2) - (6)].appl), *(yyvsp[(3) - (6)].appl), (yyvsp[(4) - (6)].pdiscztype), *(yyvsp[(5) - (6)].appl), *(yyvsp[(6) - (6)].appl));  }
    break;

  case 233:
/* Line 1787 of yacc.c  */
#line 817 "parser.yy"
    {const Value *v1 = new Value (*(yyvsp[(2) - (6)].num)); const Value *v2 = new Value (*(yyvsp[(3) - (6)].num));
 const Value *v3 = new Value (*(yyvsp[(5) - (6)].num)); const Value *v4 = new Value (*(yyvsp[(6) - (6)].num));
 (yyval.expr) = new ProbabilisticApplication ((yyvsp[(1) - (6)].pdisttype), *v1, *v2, (yyvsp[(4) - (6)].pdiscztype), *v3, *v4);}
    break;

  case 234:
/* Line 1787 of yacc.c  */
#line 821 "parser.yy"
    { (yyval.expr) = new ProbabilisticApplication ((yyvsp[(1) - (3)].pdisttype), *(yyvsp[(2) - (3)].appl), *(yyvsp[(3) - (3)].appl)); }
    break;

  case 235:
/* Line 1787 of yacc.c  */
#line 823 "parser.yy"
    { const Value *v1 = new Value (*(yyvsp[(2) - (3)].num)); const Value *v2 = new Value (*(yyvsp[(3) - (3)].num));
  (yyval.expr) = new ProbabilisticApplication ((yyvsp[(1) - (3)].pdisttype), *v1, *v2); }
    break;

  case 236:
/* Line 1787 of yacc.c  */
#line 827 "parser.yy"
    { require_fluents(); eq_expr = new Value(*(yyvsp[(1) - (1)].num)); delete (yyvsp[(1) - (1)].num); }
    break;

  case 237:
/* Line 1787 of yacc.c  */
#line 828 "parser.yy"
    { require_fluents(); }
    break;

  case 238:
/* Line 1787 of yacc.c  */
#line 829 "parser.yy"
    { eq_expr = new Addition(*(yyvsp[(4) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 239:
/* Line 1787 of yacc.c  */
#line 830 "parser.yy"
    { require_fluents(); }
    break;

  case 240:
/* Line 1787 of yacc.c  */
#line 831 "parser.yy"
    { eq_expr = make_subtraction(*(yyvsp[(4) - (6)].expr), (yyvsp[(5) - (6)].expr)); }
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 832 "parser.yy"
    { require_fluents(); }
    break;

  case 242:
/* Line 1787 of yacc.c  */
#line 833 "parser.yy"
    { eq_expr = new Multiplication(*(yyvsp[(4) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 243:
/* Line 1787 of yacc.c  */
#line 834 "parser.yy"
    { require_fluents(); }
    break;

  case 244:
/* Line 1787 of yacc.c  */
#line 835 "parser.yy"
    { eq_expr = new Division(*(yyvsp[(4) - (6)].expr), *(yyvsp[(5) - (6)].expr)); }
    break;

  case 245:
/* Line 1787 of yacc.c  */
#line 836 "parser.yy"
    { require_fluents(); prepare_application((yyvsp[(2) - (2)].str)); }
    break;

  case 246:
/* Line 1787 of yacc.c  */
#line 837 "parser.yy"
    { eq_expr = make_application(); }
    break;

  case 247:
/* Line 1787 of yacc.c  */
#line 838 "parser.yy"
    { make_eq_name((yyvsp[(1) - (1)].str)); }
    break;

  case 248:
/* Line 1787 of yacc.c  */
#line 839 "parser.yy"
    { eq_term = make_term((yyvsp[(1) - (1)].str)); eq_expr = NULL; }
    break;

  case 249:
/* Line 1787 of yacc.c  */
#line 842 "parser.yy"
    { (yyval.expr) = NULL; }
    break;

  case 251:
/* Line 1787 of yacc.c  */
#line 846 "parser.yy"
    { prepare_application((yyvsp[(2) - (2)].str)); }
    break;

  case 252:
/* Line 1787 of yacc.c  */
#line 847 "parser.yy"
    { (yyval.appl) = make_application(); }
    break;

  case 253:
/* Line 1787 of yacc.c  */
#line 848 "parser.yy"
    { prepare_application((yyvsp[(1) - (1)].str)); (yyval.appl) = make_application(); }
    break;

  case 254:
/* Line 1787 of yacc.c  */
#line 851 "parser.yy"
    { (yyval.expr) = new Value(*(yyvsp[(1) - (1)].num)); delete (yyvsp[(1) - (1)].num); }
    break;

  case 255:
/* Line 1787 of yacc.c  */
#line 853 "parser.yy"
    { (yyval.expr) = new Addition(*(yyvsp[(3) - (5)].expr), *(yyvsp[(4) - (5)].expr)); }
    break;

  case 256:
/* Line 1787 of yacc.c  */
#line 855 "parser.yy"
    { (yyval.expr) = make_subtraction(*(yyvsp[(3) - (5)].expr), (yyvsp[(4) - (5)].expr)); }
    break;

  case 257:
/* Line 1787 of yacc.c  */
#line 857 "parser.yy"
    { (yyval.expr) = new Multiplication(*(yyvsp[(3) - (5)].expr), *(yyvsp[(4) - (5)].expr)); }
    break;

  case 258:
/* Line 1787 of yacc.c  */
#line 859 "parser.yy"
    { (yyval.expr) = new Division(*(yyvsp[(3) - (5)].expr), *(yyvsp[(4) - (5)].expr)); }
    break;

  case 259:
/* Line 1787 of yacc.c  */
#line 860 "parser.yy"
    { (yyval.expr) = (yyvsp[(1) - (1)].appl); }
    break;

  case 260:
/* Line 1787 of yacc.c  */
#line 862 "parser.yy"
    { prepare_application((yyvsp[(1) - (1)].str)); (yyval.expr) = make_application(); }
    break;

  case 261:
/* Line 1787 of yacc.c  */
#line 865 "parser.yy"
    { (yyval.expr) = NULL; }
    break;

  case 263:
/* Line 1787 of yacc.c  */
#line 869 "parser.yy"
    { prepare_application((yyvsp[(2) - (2)].str)); }
    break;

  case 264:
/* Line 1787 of yacc.c  */
#line 870 "parser.yy"
    { (yyval.appl) = make_application(); }
    break;

  case 265:
/* Line 1787 of yacc.c  */
#line 872 "parser.yy"
    { prepare_application((yyvsp[(1) - (1)].str)); (yyval.appl) = make_application(); }
    break;

  case 267:
/* Line 1787 of yacc.c  */
#line 880 "parser.yy"
    { add_term((yyvsp[(2) - (2)].str)); }
    break;

  case 268:
/* Line 1787 of yacc.c  */
#line 881 "parser.yy"
    { add_term((yyvsp[(2) - (2)].str)); }
    break;

  case 270:
/* Line 1787 of yacc.c  */
#line 885 "parser.yy"
    { add_term((yyvsp[(2) - (2)].str)); }
    break;

  case 272:
/* Line 1787 of yacc.c  */
#line 889 "parser.yy"
    { add_variables((yyvsp[(1) - (1)].strs), OBJECT_TYPE); }
    break;

  case 273:
/* Line 1787 of yacc.c  */
#line 890 "parser.yy"
    { add_variables((yyvsp[(1) - (2)].strs), (yyvsp[(2) - (2)].type)); }
    break;

  case 275:
/* Line 1787 of yacc.c  */
#line 893 "parser.yy"
    { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[(1) - (1)].str)); }
    break;

  case 276:
/* Line 1787 of yacc.c  */
#line 894 "parser.yy"
    { (yyval.strs) = (yyvsp[(1) - (2)].strs); (yyval.strs)->push_back((yyvsp[(2) - (2)].str)); }
    break;

  case 278:
/* Line 1787 of yacc.c  */
#line 898 "parser.yy"
    { add_names((yyvsp[(1) - (1)].strs), OBJECT_TYPE); }
    break;

  case 279:
/* Line 1787 of yacc.c  */
#line 899 "parser.yy"
    { add_names((yyvsp[(1) - (2)].strs), (yyvsp[(2) - (2)].type)); }
    break;

  case 281:
/* Line 1787 of yacc.c  */
#line 902 "parser.yy"
    { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[(1) - (1)].str)); }
    break;

  case 282:
/* Line 1787 of yacc.c  */
#line 903 "parser.yy"
    { (yyval.strs) = (yyvsp[(1) - (2)].strs); (yyval.strs)->push_back((yyvsp[(2) - (2)].str)); }
    break;

  case 283:
/* Line 1787 of yacc.c  */
#line 906 "parser.yy"
    { require_typing(); }
    break;

  case 284:
/* Line 1787 of yacc.c  */
#line 906 "parser.yy"
    { (yyval.type) = (yyvsp[(3) - (3)].type); }
    break;

  case 285:
/* Line 1787 of yacc.c  */
#line 909 "parser.yy"
    { (yyval.type) = OBJECT_TYPE; }
    break;

  case 286:
/* Line 1787 of yacc.c  */
#line 910 "parser.yy"
    { (yyval.type) = make_type((yyvsp[(1) - (1)].str)); }
    break;

  case 287:
/* Line 1787 of yacc.c  */
#line 911 "parser.yy"
    { (yyval.type) = make_type(*(yyvsp[(3) - (4)].types)); delete (yyvsp[(3) - (4)].types); }
    break;

  case 288:
/* Line 1787 of yacc.c  */
#line 914 "parser.yy"
    { (yyval.types) = new TypeSet(); }
    break;

  case 289:
/* Line 1787 of yacc.c  */
#line 915 "parser.yy"
    { (yyval.types) = new TypeSet(); (yyval.types)->insert(make_type((yyvsp[(1) - (1)].str))); }
    break;

  case 290:
/* Line 1787 of yacc.c  */
#line 916 "parser.yy"
    { (yyval.types) = (yyvsp[(1) - (2)].types); }
    break;

  case 291:
/* Line 1787 of yacc.c  */
#line 917 "parser.yy"
    { (yyval.types) = (yyvsp[(1) - (2)].types); (yyval.types)->insert(make_type((yyvsp[(2) - (2)].str))); }
    break;

  case 293:
/* Line 1787 of yacc.c  */
#line 927 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 294:
/* Line 1787 of yacc.c  */
#line 930 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 295:
/* Line 1787 of yacc.c  */
#line 933 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 296:
/* Line 1787 of yacc.c  */
#line 936 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 297:
/* Line 1787 of yacc.c  */
#line 939 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 298:
/* Line 1787 of yacc.c  */
#line 942 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 299:
/* Line 1787 of yacc.c  */
#line 945 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 300:
/* Line 1787 of yacc.c  */
#line 948 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 301:
/* Line 1787 of yacc.c  */
#line 951 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 302:
/* Line 1787 of yacc.c  */
#line 954 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 303:
/* Line 1787 of yacc.c  */
#line 957 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 304:
/* Line 1787 of yacc.c  */
#line 960 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 305:
/* Line 1787 of yacc.c  */
#line 963 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 306:
/* Line 1787 of yacc.c  */
#line 966 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 307:
/* Line 1787 of yacc.c  */
#line 969 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 308:
/* Line 1787 of yacc.c  */
#line 972 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 309:
/* Line 1787 of yacc.c  */
#line 975 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 310:
/* Line 1787 of yacc.c  */
#line 977 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 311:
/* Line 1787 of yacc.c  */
#line 979 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 312:
/* Line 1787 of yacc.c  */
#line 982 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 313:
/* Line 1787 of yacc.c  */
#line 985 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 314:
/* Line 1787 of yacc.c  */
#line 988 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 315:
/* Line 1787 of yacc.c  */
#line 991 "parser.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;


/* Line 1787 of yacc.c  */
#line 3902 "parser.cc"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2050 of yacc.c  */
#line 1025 "parser.yy"


/* Outputs an error message. */
static void yyerror(const std::string& s) {
  std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	    << std::endl;
  success = false;
}


/* Outputs a warning. */
static void yywarning(const std::string& s) {
  if (warning_level > 0) {
    std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	      << std::endl;
    if (warning_level > 1) {
      success = false;
    }
  }
}


/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name) {
  domain = new Domain(*name);
  domains[*name] = domain;
  requirements = &domain->requirements;
  problem = NULL;
  delete name;
}


/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
			 const std::string* domain_name) {
  std::map<std::string, Domain*>::const_iterator di =
    domains.find(*domain_name);
  if (di != domains.end()) {
    domain = (*di).second;
  } else {
    domain = new Domain(*domain_name);
    domains[*domain_name] = domain;
    yyerror("undeclared domain `" + *domain_name + "' used");
  }
  requirements = new Requirements(domain->requirements);
  problem = new Problem(*name, *domain);
  if (requirements->rewards) {
    const Application& reward_appl =
      Application::make_application(reward_function, TermList());
    const Assignment* reward_assignment =
      new Assignment(Assignment::ASSIGN_OP, reward_appl, *new Value(0));
    problem->add_init_effect(*new AssignmentEffect(*reward_assignment));
  }
  delete name;
  delete domain_name;
}


/* Adds :typing to the requirements. */
static void require_typing() {
  if (!requirements->typing) {
    yywarning("assuming `:typing' requirement");
    requirements->typing = true;
  }
}


/* Adds :fluents to the requirements. */
static void require_fluents() {
  if (!requirements->fluents) {
    yywarning("assuming `:fluents' requirement");
    requirements->fluents = true;
  }
}


/* Adds :disjunctive-preconditions to the requirements. */
static void require_disjunction() {
  if (!requirements->disjunctive_preconditions) {
    yywarning("assuming `:disjunctive-preconditions' requirement");
    requirements->disjunctive_preconditions = true;
  }
}


/* Adds :conditional-effects to the requirements. */ 
static void require_conditional_effects() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
}


/* Returns a simple type with the given name. */
static Type make_type(const std::string* name) {
  std::pair<Type, bool> t = domain->types().find_type(*name);
  if (!t.second) {
    t.first = domain->types().add_type(*name);
    if (name_kind != TYPE_KIND) {
      yywarning("implicit declaration of type `" + *name + "'");
    }
  }
  delete name;
  return t.first;
}


/* Returns the union of the given types. */
static Type make_type(const TypeSet& types) {
  return domain->types().add_type(types);
}


/* Returns a simple term with the given name. */
static Term make_term(const std::string* name) {
  if ((*name)[0] == '?') {
    std::pair<Variable, bool> v = context.find(*name);
    if (!v.second) {
      if (problem != NULL) {
	v.first = problem->terms().add_variable(OBJECT_TYPE);
      } else {
	v.first = domain->terms().add_variable(OBJECT_TYPE);
      }
      context.insert(*name, v.first);
      yyerror("free variable `" + *name + "' used");
    }
    delete name;
    return v.first;
  } else {
    TermTable& terms = (problem != NULL) ? problem->terms() : domain->terms();
    const PredicateTable& predicates = domain->predicates();
    std::pair<Object, bool> o = terms.find_object(*name);
    if (!o.second) {
      size_t n = term_parameters.size();
      if (parsing_atom && predicates.arity(atom_predicate) > n) {
	o.first = terms.add_object(*name,
				   predicates.parameter(atom_predicate, n));
      } else {
	o.first = terms.add_object(*name, OBJECT_TYPE);
      }
      yywarning("implicit declaration of object `" + *name + "'");
    }
    delete name;
    return o.first;
  }
}


/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name) {
  repeated_predicate = false;
  std::pair<Predicate, bool> p = domain->predicates().find_predicate(*name);
  if (!p.second) {
    p.first = domain->predicates().add_predicate(*name);
  } else {
    repeated_predicate = true;
    yywarning("ignoring repeated declaration of predicate `" + *name + "'");
  }
  predicate = p.first;
  parsing_predicate = true;
  delete name;
}


/* Creates a function with the given name. */
static void make_function(const std::string* name) {
  repeated_function = false;
  std::pair<Function, bool> f = domain->functions().find_function(*name);
  if (!f.second) {
    f.first = domain->functions().add_function(*name);
  } else {
    repeated_function = true;
    if (requirements->rewards && f.first == reward_function) {
      yywarning("ignoring declaration of reserved function `reward'");
    } else {
      yywarning("ignoring repeated declaration of function `" + *name + "'");
    }
  }
  function = f.first;
  parsing_function = true;
  delete name;
}

/* Creates a continuous variable as a function with the given name. */
static void make_cvariable (const std::string *name, const Rational *low,
			    const Rational *high)
{
  repeated_function = false;
  std::pair<Function, bool> f = domain->functions().find_function(*name);
  if (!f.second) {
    f.first = domain->functions().add_cvariable(*name, low->double_value (), 
						high->double_value ());
  } 
  else {
    repeated_function = true;
    if (requirements->rewards && f.first == reward_function) {
      yywarning("ignoring declaration of reserved function `reward'");
    } else {
      yywarning("ignoring repeated declaration of function `" + *name + "'");
    }
  }
  function = f.first;
  parsing_function = true;
  delete name;
}

/* Creates an action with the given name. */
static void make_action(const std::string* name) {
  context.push_frame();
  action = new ActionSchema(*name);
  delete name;
}


/* Adds the current action to the current domain. */
static void add_action() {
  context.pop_frame();
  if (domain->find_action(action->name()) == NULL) {
    domain->add_action(*action);
  } else {
    yywarning("ignoring repeated declaration of action `"
	      + action->name() + "'");
    delete action;
  }
  action = NULL;
}


/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  context.push_frame();
  quantified.push_back(NULL_TERM);
}


/* Creates a universally quantified effect. */
static const Effect* make_forall_effect(const Effect& effect) {
  context.pop_frame();
  QuantifiedEffect* qeffect = new QuantifiedEffect(effect);
  size_t n = quantified.size() - 1;
  size_t m = n;
  while (is_variable(quantified[n])) {
    n--;
  }
  for (size_t i = n + 1; i <= m; i++) {
    qeffect->add_parameter(quantified[i]);
  }
  quantified.resize(n);
  return qeffect;
}


/* Adds an outcome to the given probabilistic effect. */
static void add_effect_outcome(ProbabilisticEffect& peffect,
			       const Rational* p, const Effect& effect) {
  if (!requirements->probabilistic_effects) {
    yywarning("assuming `:probabilistic-effects' requirement");
    requirements->probabilistic_effects = true;
  }
  //std::cout << "numerator: " << p->numerator() << " -- denominator: " << p->denominator() << " -- value: " << p->double_value() << std::endl;
  if (*p < 0 || *p > 1) {
    yyerror("outcome probability needs to be in the interval [0,1]");
  }
  /* deactivated for now, too strict wrt to rounded values in the model
     generator. */
  peffect.add_outcome (*p, effect);
  /* if (!peffect.add_outcome(*p, effect)) {
     yyerror("effect outcome probabilities add up to more than 1");
     } */
  delete p;
}


/* Creates an add effect. */
static const Effect* make_add_effect(const Atom& atom) {
  domain->predicates().make_dynamic(atom.predicate());
  return new AddEffect(atom);
}


/* Creates a delete effect. */
static const Effect* make_delete_effect(const Atom& atom) {
  domain->predicates().make_dynamic(atom.predicate());
  return new DeleteEffect(atom);
}


/* Creates an assignment effect. */
static const Effect* make_assignment_effect(Assignment::AssignOp oper,
					    const Application& application,
					    const Expression& expr) {
  if (requirements->rewards && application.function() == reward_function) {
    if ((oper != Assignment::INCREASE_OP && oper != Assignment::DECREASE_OP
	 && oper != Assignment::INCREASE_PROB_OP && oper != Assignment::DECREASE_PROB_OP)
	|| typeid(expr) != typeid(Value)) {
      yyerror("only constant reward increments/decrements allowed");
    }
  } else {
    require_fluents();
  }
  effect_fluent = false;
  domain->functions().make_dynamic(application.function());
  const Assignment& assignment = *new Assignment(oper, application, expr);
  return new AssignmentEffect(assignment);
}

/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names,
		      Type type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (name_kind == TYPE_KIND) {
      if (*s == OBJECT_NAME) {
	yywarning("ignoring declaration of reserved type `object'");
      } else if (*s == NUMBER_NAME) {
	yywarning("ignoring declaration of reserved type `number'");
      } else {
	std::pair<Type, bool> t = domain->types().find_type(*s);
	if (!t.second) {
	  t.first = domain->types().add_type(*s);
	}
	if (!domain->types().add_supertype(t.first, type)) {
	  yyerror("cyclic type hierarchy");
	}
      }
    } else if (name_kind == CONSTANT_KIND) {
      std::pair<Object, bool> o = domain->terms().find_object(*s);
      if (!o.second) {
	domain->terms().add_object(*s, type);
      } else {
	TypeSet components;
	domain->types().components(components, domain->terms().type(o.first));
	components.insert(type);
	domain->terms().set_type(o.first, make_type(components));
      }
    } else { /* name_kind == OBJECT_KIND */
      if (domain->terms().find_object(*s).second) {
	yywarning("ignoring declaration of object `" + *s
		  + "' previously declared as constant");
      } else {
	std::pair<Object, bool> o = problem->terms().find_object(*s);
	if (!o.second) {
	  problem->terms().add_object(*s, type);
	} else {
	  TypeSet components;
	  domain->types().components(components,
				     problem->terms().type(o.first));
	  components.insert(type);
	  problem->terms().set_type(o.first, make_type(components));
	}
      }
    }
    delete s;
  }
  delete names;
}


/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
			  Type type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (parsing_predicate) {
      if (!repeated_predicate) {
	domain->predicates().add_parameter(predicate, type);
      }
    } else if (parsing_function) {
      if (!repeated_function) {
	domain->functions().add_parameter(function, type);
      }
    } else {
      if (context.shallow_find(*s).second) {
	yyerror("repetition of parameter `" + *s + "'");
      } else if (context.find(*s).second) {
	yywarning("shadowing parameter `" + *s + "'");
      }
      Variable var;
      if (problem != NULL) {
	var = problem->terms().add_variable(type);
      } else {
	var = domain->terms().add_variable(type);
      }
      context.insert(*s, var);
      if (!quantified.empty()) {
	quantified.push_back(var);
      } else { /* action != NULL */
	action->add_parameter(var);
      }
    }
    delete s;
  }
  delete names;
}


/* Prepares for the parsing of an atomic state formula. */ 
static void prepare_atom(const std::string* name) {
  std::pair<Predicate, bool> p = domain->predicates().find_predicate(*name);
  if (!p.second) {
    atom_predicate = domain->predicates().add_predicate(*name);
    undeclared_atom_predicate = true;
    if (problem != NULL) {
      yywarning("undeclared predicate `" + *name + "' used");
    } else {
      yywarning("implicit declaration of predicate `" + *name + "'");
    }
  } else {
    atom_predicate = p.first;
    undeclared_atom_predicate = false;
  }
  term_parameters.clear();
  parsing_atom = true;
  delete name;
}


/* Prepares for the parsing of a function application. */ 
static void prepare_application(const std::string* name) {
  std::pair<Function, bool> f = domain->functions().find_function(*name);
  if (!f.second) {
    appl_function = domain->functions().add_function(*name);
    undeclared_appl_function = true;
    if (problem != NULL) {
      yywarning("undeclared function `" + *name + "' used");
    } else {
      yywarning("implicit declaration of function `" + *name + "'");
    }
  } else {
    appl_function = f.first;
    undeclared_appl_function = false;
  }
  if (requirements->rewards && f.first == reward_function) {
    if (!effect_fluent && !metric_fluent) {
      yyerror("reserved function `reward' not allowed here");
    }
  } else {
    require_fluents();
  }
  term_parameters.clear();
  parsing_application = true;
  delete name;
}


/* Adds a term with the given name to the current atomic state formula. */
static void add_term(const std::string* name) {
  Term term = make_term(name);
  const TermTable& terms =
    (problem != NULL) ? problem->terms() : domain->terms();
  if (parsing_atom) {
    PredicateTable& predicates = domain->predicates();
    size_t n = term_parameters.size();
    if (undeclared_atom_predicate) {
      predicates.add_parameter(atom_predicate, terms.type(term));
    } else if (predicates.arity(atom_predicate) > n
	       && !domain->types().subtype(terms.type(term),
					   predicates.parameter(atom_predicate,
								n))) {
      yyerror("type mismatch");
    }
  } else if (parsing_application) {
    FunctionTable& functions = domain->functions();
    size_t n = term_parameters.size();
    if (undeclared_appl_function) {
      functions.add_parameter(appl_function, terms.type(term));
    } else if (functions.arity(appl_function) > n
	       && !domain->types().subtype(terms.type(term),
					   functions.parameter(appl_function,
							       n))) {
      yyerror("type mismatch");
    }
  }
  term_parameters.push_back(term);
}


/* Creates the atomic formula just parsed. */
static const Atom* make_atom() {
  size_t n = term_parameters.size();
  if (domain->predicates().arity(atom_predicate) < n) {
    yyerror("too many parameters passed to predicate `"
	    + domain->predicates().name(atom_predicate) + "'");
  } else if (domain->predicates().arity(atom_predicate) > n) {
    yyerror("too few parameters passed to predicate `"
	    + domain->predicates().name(atom_predicate) + "'");
  }
  parsing_atom = false;
  return &Atom::make_atom(atom_predicate, term_parameters);
}


/* Creates the function application just parsed. */
static const Application* make_application() {
  size_t n = term_parameters.size();

  if (domain->functions().arity(appl_function) < n) {
    yyerror("too many parameters passed to function `"
	    + domain->functions().name(appl_function) + "'");
  } else if (domain->functions().arity(appl_function) > n) {
    yyerror("too few parameters passed to function `"
	    + domain->functions().name(appl_function) + "'");
  }
  parsing_application = false;
  return &Application::make_application(appl_function, term_parameters);
}


/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
					  const Expression* opt_term) {
  if (opt_term != NULL) {
    return new Subtraction(term, *opt_term);
  } else {
    return new Subtraction(*new Value(0), term);
  }
}


/* Creates an atom or fluent for the given name to be used in an
   equality formula. */
static void make_eq_name(const std::string* name) {
  std::pair<Function, bool> f = domain->functions().find_function(*name);
  if (f.second) {
    prepare_application(name);
    eq_expr = make_application();
  } else {
    /* Assume this is a term. */
    eq_term = make_term(name);
    eq_expr = NULL;
  }
}


/* Creates an equality formula. */
static const StateFormula* make_equality() {
  if (!requirements->equality) {
    yywarning("assuming `:equality' requirement");
    requirements->equality = true;
  }
  if (first_eq_expr != NULL && eq_expr != NULL) {
    return new Comparison(Comparison::EQ_CMP, *first_eq_expr, *eq_expr);
  } else if (first_eq_expr == NULL && eq_expr == NULL) {
    const TermTable& terms =
      (problem != NULL) ? problem->terms() : domain->terms();
    if (domain->types().subtype(terms.type(first_eq_term), terms.type(eq_term))
	|| domain->types().subtype(terms.type(eq_term),
				   terms.type(first_eq_term))) {
      return new Equality(first_eq_term, eq_term);
    } else {
      return &StateFormula::FALSE;
    }
  } else {
    yyerror("comparison of term and numeric expression");
    return &StateFormula::FALSE;
  }
}


/* Creates a negated formula. */
static const StateFormula* make_negation(const StateFormula& negand) {
  if (typeid(negand) == typeid(Atom)) {
    if (!requirements->negative_preconditions) {
      yywarning("assuming `:negative-preconditions' requirement");
      requirements->negative_preconditions = true;
    }
  } else if (typeid(negand) != typeid(Equality)
	     && typeid(negand) != typeid(Comparison)) {
    require_disjunction();
  }
  return &Negation::make_negation(negand);
}


/* Creates an implication. */
static const StateFormula* make_implication(const StateFormula& f1,
					    const StateFormula& f2) {
  require_disjunction();
  Disjunction* disj = new Disjunction();
  disj->add_disjunct(Negation::make_negation(f1));
  disj->add_disjunct(f2);
  return disj;
}


/* Prepares for the parsing of an existentially quantified formula. */
static void prepare_exists() {
  if (!requirements->existential_preconditions) {
    yywarning("assuming `:existential-preconditions' requirement");
    requirements->existential_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(NULL_TERM);
}


/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall() {
  if (!requirements->universal_preconditions) {
    yywarning("assuming `:universal-preconditions' requirement");
    requirements->universal_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(NULL_TERM);
}


/* Creates an existentially quantified formula. */
static const StateFormula* make_exists(const StateFormula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (is_variable(quantified[n])) {
    n--;
  }
  if (n < m) {
    Exists* exists = new Exists();
    for (size_t i = n + 1; i <= m; i++) {
      exists->add_parameter(quantified[i]);
    }
    exists->set_body(body);
    quantified.resize(n);
    return exists;
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Creates a universally quantified formula. */
static const StateFormula* make_forall(const StateFormula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (is_variable(quantified[n])) {
    n--;
  }
  if (n < m) {
    Forall* forall = new Forall();
    for (size_t i = n + 1; i <= m; i++) {
      forall->add_parameter(quantified[i]);
    }
    forall->set_body(body);
    quantified.resize(n);
    return forall;
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Sets the goal reward for the current problem. */
GoalReward* make_goal_reward(const Expression& goal_reward) {
  if (!requirements->rewards) {
    yyerror("goal reward only allowed with the `:rewards' requirement");
  } else {
    const Application& reward_appl =
      Application::make_application(reward_function, TermList());
    const Assignment* reward_assignment =
      new Assignment(Assignment::INCREASE_OP, reward_appl, goal_reward);
    return new GoalReward (const_cast<Assignment*> (reward_assignment));
  }
  return 0;
}

GoalLinearReward* make_goal_linear_reward (const Expression &goal_reward)
{
  if (!requirements->rewards) {
    yyerror (("goal reward only allowed with the `:rewards' requirement"));
  } else {
    const Application &reward_appl
      = Application::make_application(reward_function, TermList());
    const Assignment *reward_assignment
      = new Assignment(Assignment::INCREASE_OP, reward_appl, goal_reward);  /* increase_op should not matter */
    return new GoalLinearReward (const_cast<Assignment*> (reward_assignment));
  }
  return 0;
}

void add_linear_reward_comp (const Expression &goal_reward, const GoalLinearReward *glreward)
{
  const Application & reward_appl =
    Application::make_application(reward_function, TermList());
  const Assignment* reward_assignment =
    new Assignment(Assignment::INCREASE_OP, reward_appl, goal_reward);
  const_cast<GoalLinearReward*> (glreward)->addLinearRewardComp (const_cast<Assignment*> (reward_assignment));
}

void add_goal (const std::string &name, const StateFormula &condition, const GoalLinearReward &gr)
{
  Goal *g = new Goal (name, condition, gr);
  problem->add_goal (g);
}

/* Sets the default metric for the current problem. */
static void set_default_metric() {
  if (requirements->rewards) {
    const Application& reward_appl =
      Application::make_application(reward_function, TermList());
    problem->set_metric(reward_appl);
  }
}
