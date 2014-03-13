/* -*-C++-*- */
/*
 * Parser.
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
/* Creates a resource as a function with the given name. */
 static void make_resource (const std::string *name, const Rational *low, 
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
%}

%token DEFINE DOMAIN_TOKEN PROBLEM
%token REQUIREMENTS TYPES CONSTANTS PREDICATES FUNCTIONS RESOURCES
%token STRIPS TYPING NEGATIVE_PRECONDITIONS DISJUNCTIVE_PRECONDITIONS EQUALITY
%token EXISTENTIAL_PRECONDITIONS UNIVERSAL_PRECONDITIONS
%token QUANTIFIED_PRECONDITIONS CONDITIONAL_EFFECTS FLUENTS ADL
%token DURATIVE_ACTIONS DURATION_INEQUALITIES CONTINUOUS_EFFECTS
%token PROBABILISTIC_EFFECTS REWARDS MDP HMDP
%token ACTION PARAMETERS PRECONDITION EFFECT
%token PDOMAIN OBJECTS INIT GOAL GOAL_REWARD GOAL_LINEAR_REWARD METRIC GOAL_PROBABILITY
%token WHEN NOT AND OR IMPLY EXISTS FORALL PROBABILISTIC
%token ASSIGN SCALE_UP SCALE_DOWN INCREASE DECREASE MINIMIZE MAXIMIZE
%token NUMBER_TOKEN OBJECT_TOKEN EITHER
%token LE GE NAME VARIABLE NUMBER
%token ILLEGAL_TOKEN
%token RSC_DIST_DISTRIBUTION RSC_DIST_DISCRETIZATION 
%token RSC_DIST_MEAN RSC_DIST_STDDEV DISCZ_POINTS DISCZ_THRESHOLD DISCZ_INTERVAL
%token DIST_NORMAL DIST_UNIFORM INCREASE_PROBABILISTIC DECREASE_PROBABILISTIC

%union {
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
}

%type <setop> assign_op
%type <effect> eff_formula p_effect simple_init one_init
%type <ceffect> eff_formulas one_inits
%type <peffect> prob_effs prob_inits
%type <formula> formula
%type <atom> atomic_name_formula atomic_term_formula
%type <conj> conjuncts
%type <disj> disjuncts
%type <expr> value f_exp opt_f_exp ground_f_exp opt_ground_f_exp distribution_exp
%type <appl> ground_f_head f_head
%type <comp> binary_comp
%type <strs> name_seq variable_seq
%type <type> type_spec type
%type <types> types
%type <str> type_name predicate function name variable resource 
%type <str> DEFINE DOMAIN_TOKEN PROBLEM GOAL_PROBABILITY
%type <str> WHEN NOT AND OR IMPLY EXISTS FORALL PROBABILISTIC
%type <str> ASSIGN SCALE_UP SCALE_DOWN INCREASE DECREASE MINIMIZE MAXIMIZE 
%type <str> INCREASE_PROBABILISTIC DECREASE_PROBABILISTIC
%type <str> NUMBER_TOKEN OBJECT_TOKEN EITHER
%type <str> NAME VARIABLE
%type <num> probability NUMBER bound
%type <pdisttype> distribution_type
%type <pdiscztype> discretization_type
%type <greward> goal_reward_expr reward_formula reward_linear_formula goal_linear_reward_expr goal_reward
%type <cgreward> reward_formulas linear_reward_formulas

%%

file : { success = true; line_number = 1; } domains_and_problems
         { if (!success) YYERROR; }
     ;

domains_and_problems : /* empty */
                     | domains_and_problems domain_def
                     | domains_and_problems problem_def
                     ;


/* ====================================================================== */
/* Domain definitions. */

domain_def : '(' define '(' domain name ')' { make_domain($5); }
               domain_body ')'
           ;

domain_body : /* empty */
            | require_def
            | require_def domain_body2
            | domain_body2
            ;

domain_body2 : types_def
             | types_def domain_body3
             | domain_body3
             ;

domain_body3 : constants_def
             | predicates_def
             | functions_def
             | resources_def
             | resources_def domain_body6   /* resources comes after functions */
             | constants_def domain_body4
             | predicates_def domain_body5
             | functions_def resources_def domain_body6
             | functions_def domain_body6
             | structure_defs
             ;

/* no more constants */
domain_body4 : predicates_def
             | functions_def
             | resources_def
             | resources_def domain_body8
             | predicates_def domain_body7
             | functions_def resources_def domain_body8
             | functions_def domain_body8
             | structure_defs
             ;

/* no more predicates */
domain_body5 : constants_def
             | functions_def
             | resources_def
             | resources_def domain_body9
             | constants_def domain_body7
             | functions_def resources_def domain_body9
             | functions_def domain_body9
             | structure_defs
             ;

/* no more functions or resources */
domain_body6 : constants_def
             | predicates_def
             | constants_def domain_body8
             | predicates_def domain_body9
             | structure_defs
             ;

/* only functions or structure defs */
domain_body7 : functions_def 
             | resources_def
             | resources_def structure_def
             | functions_def resources_def structure_defs
             | functions_def structure_defs
             | structure_defs
             ;

domain_body8 : predicates_def
             | predicates_def structure_defs
             | structure_defs
             ;

domain_body9 : constants_def
             | constants_def structure_defs
             | structure_defs
             ;

structure_defs : structure_def
               | structure_defs structure_def
               ;

structure_def : action_def
              ;

require_def : '(' REQUIREMENTS require_keys ')'
            ;

require_keys : require_key
             | require_keys require_key
             ;

require_key : STRIPS { requirements->strips = true; }
            | TYPING { requirements->typing = true; }
            | NEGATIVE_PRECONDITIONS
                { requirements->negative_preconditions = true; }
            | DISJUNCTIVE_PRECONDITIONS
                { requirements->disjunctive_preconditions = true; }
            | EQUALITY { requirements->equality = true; }
            | EXISTENTIAL_PRECONDITIONS
                { requirements->existential_preconditions = true; }
            | UNIVERSAL_PRECONDITIONS
                { requirements->universal_preconditions = true; }
            | QUANTIFIED_PRECONDITIONS
                { requirements->quantified_preconditions(); }
            | CONDITIONAL_EFFECTS { requirements->conditional_effects = true; }
            | FLUENTS { requirements->fluents = true; }
            | ADL { requirements->adl(); }
            | DURATIVE_ACTIONS
                { throw Exception("`:durative-actions' not supported"); }
            | DURATION_INEQUALITIES
                { throw Exception("`:duration-inequalities' not supported"); }
            | CONTINUOUS_EFFECTS
                { throw Exception("`:continuous-effects' not supported"); }
            | PROBABILISTIC_EFFECTS
                {
		  requirements->probabilistic_effects = true;
		  goal_prob_function =
		    domain->functions().add_function("goal-probability");
		}
            | REWARDS
                {
		  requirements->rewards = true;
		  reward_function = domain->functions().add_function("reward");
		}
            | MDP
                {
		  requirements->probabilistic_effects = true;
		  requirements->rewards = true;
		  goal_prob_function =
		    domain->functions().add_function("goal-probability");
		  reward_function = domain->functions().add_function("reward");
		}
            | HMDP
                {
		  requirements->probabilistic_effects = true;
		  requirements->rewards = true;
		  goal_prob_function =
		    domain->functions().add_function("goal-probability");
		  reward_function = domain->functions().add_function("reward");
		}

            ;

types_def : '(' TYPES { require_typing(); name_kind = TYPE_KIND; }
              typed_names ')' { name_kind = VOID_KIND; }
          ;

constants_def : '(' CONSTANTS { name_kind = CONSTANT_KIND; } typed_names ')'
                  { name_kind = VOID_KIND; }
              ;

predicates_def : '(' PREDICATES predicate_decls ')'
               ;

functions_def : '(' FUNCTIONS { require_fluents(); } function_decls ')'
              ;

resources_def : '(' RESOURCES resource_decls ')'

/* ====================================================================== */
/* Predicate and function declarations. */

predicate_decls : /* empty */
                | predicate_decls predicate_decl
                ;

predicate_decl : '(' predicate { make_predicate($2); } variables ')'
                   { parsing_predicate = false; }
               ;

function_decls : /* empty */
               | function_decl_seq
               | function_decl_seq function_type_spec function_decls
               ;

function_decl_seq : function_decl
                  | function_decl_seq function_decl
                  ;

function_type_spec : '-' { require_typing(); } function_type
                   ;

function_decl : '(' function { make_function($2); } variables ')'
                  { parsing_function = false; }
              ;

/* Resources as functions */

resource_decls : /* empty */
               | resource_decl_seq
               | resource_decl_seq resource_decls
               ;

resource_decl_seq : resource_decl
                  | resource_decl_seq resource_decl
                  ;

resource_decl : '(' resource bound bound { make_resource ($2, $3, $4); } variables ')'
                    {parsing_function = false;} 
              ;

/* ====================================================================== */
/* Action definitions. */

action_def : '(' ACTION name { make_action($3); }
               parameters action_body ')' { add_action(); }
           ;

parameters : /* empty */
           | PARAMETERS '(' variables ')'
           ;

action_body : precondition action_body2
            | action_body2
            ;

action_body2 : /* empty */
             | effect
             ;

precondition : PRECONDITION formula { action->set_precondition(*$2); }
             ;

effect : EFFECT eff_formula { action->set_effect(*$2); }
       ;


/* ====================================================================== */
/* Effect formulas. */

eff_formula : p_effect
            | '(' and eff_formulas ')' { $$ = $3; }
            | '(' forall { prepare_forall_effect(); } '(' variables ')'
                eff_formula ')' { $$ = make_forall_effect(*$7); }
            | '(' when { require_conditional_effects(); } formula
                eff_formula ')' { $$ = &ConditionalEffect::make(*$4, *$5); }
            | '(' probabilistic prob_effs ')' { $$ = $3; }
            ;

eff_formulas : /* empty */ { $$ = new ConjunctiveEffect(); }
             | eff_formulas eff_formula { $$ = $1; $$->add_conjunct(*$2); }
             ;

prob_effs : probability eff_formula
              {
		$$ = new ProbabilisticEffect();
		add_effect_outcome(*$$, $1, *$2);
	      }
          | prob_effs probability eff_formula
              { $$ = $1; add_effect_outcome(*$$, $2, *$3); }
          ;

probability : NUMBER
            ;

p_effect : atomic_term_formula { $$ = make_add_effect(*$1); }
         | '(' not atomic_term_formula ')' { $$ = make_delete_effect(*$3); }
/* | '(' assign_op { effect_fluent = true; } assign_op_prob_args ')'
   { $$ = make_probabilistic_assignment_effect ($2, *$4, *$5); } */
         | '(' assign_op { effect_fluent = true; } f_head f_exp ')'
             { $$ = make_assignment_effect($2, *$4, *$5); }
         ;

assign_op : assign { $$ = Assignment::ASSIGN_OP; }
          | scale_up { $$ = Assignment::SCALE_UP_OP; }
          | scale_down { $$ = Assignment::SCALE_DOWN_OP; }
          | increase { $$ = Assignment::INCREASE_OP; }
          | decrease { $$ = Assignment::DECREASE_OP; }
          | increase_probabilistic {$$ = Assignment::INCREASE_PROB_OP; }
          | decrease_probabilistic {$$ = Assignment::DECREASE_PROB_OP; }
          ;

distribution_type : DIST_NORMAL
{ $$ = DISTRIBUTION_NORMAL; }
                  | DIST_UNIFORM
{ $$ = DISTRIBUTION_UNIFORM; }
                  ;

/* TODO */
discretization_type : DISCZ_POINTS
{ $$ = DISCRETIZATION_POINTS; }
                    | DISCZ_THRESHOLD
{ $$ = DISCRETIZATION_THRESHOLD; }
                    | DISCZ_INTERVAL
{ $$ = DISCRETIZATION_INTERVAL; }
;
       

/* ====================================================================== */
/* Problem definitions. */

problem_def : '(' define '(' problem name ')' '(' PDOMAIN name ')'
                { make_problem($5, $9); } problem_body ')'
                { problem->instantiate_actions(); delete requirements; }
            ;

problem_body : require_def problem_body2
             | problem_body2
             ;

problem_body2 : object_decl problem_body3
              | problem_body3
              ;

problem_body3 : init goal_spec
              | goal_spec
              ;

object_decl : '(' OBJECTS { name_kind = OBJECT_KIND; } typed_names ')'
                { name_kind = VOID_KIND; }
            ;

init : '(' INIT init_elements ')'
     ;

init_elements : /* empty */
              | init_elements init_element
              ;

init_element : atomic_name_formula { problem->add_init_atom(*$1); }
             | '(' '=' ground_f_head NUMBER ')'
                 { problem->add_init_value(*$3, *$4); delete $4; }
             | '(' probabilistic prob_inits ')'
                 { problem->add_init_effect(*$3); }
             | '(' ground_f_head distribution_exp ')'
{ problem->add_init_prob_dist ($2, $3);
  /* set a 'static' initial resource value as the MAX bound of the resource.
     This is used only for testing actions applicability in the parser tests,
     and the hmdp internal full engine. Not in CAO*. */
  if (problem->domain ().functions ().isResource ($2->function ()))
    {
      const std::string &rscname 
	= problem->domain ().functions ().name ($2->function ());
      double rsc_max_value
	= problem->domain ().functions ().getResourceBounds (rscname).second;
      problem->add_init_value (*$2, Rational (rsc_max_value));
    }
  else yyerror ("Initial state probability distribution must be over a resource.");
}
;

prob_inits : probability simple_init
               {
		 $$ = new ProbabilisticEffect();
		 add_effect_outcome(*$$, $1, *$2);
	       }
           | prob_inits probability simple_init
               { $$ = $1; add_effect_outcome(*$$, $2, *$3); }
           ;

simple_init : one_init
            | '(' and one_inits ')' { $$ = $3; }
            ;

one_inits : /* empty */ { $$ = new ConjunctiveEffect(); }
          | one_inits one_init { $$ = $1; $$->add_conjunct(*$2); }
          ;

one_init : atomic_name_formula { $$ = make_add_effect(*$1); }
         | '(' '=' ground_f_head value ')'
             { $$ = make_assignment_effect(Assignment::ASSIGN_OP, *$3, *$4); }
         ;

value : NUMBER { $$ = new Value(*$1); delete $1; }
      ;

goal_spec :  '(' GOAL name formula goal_reward ')' { add_goal (*$3, *$4, *$5); }
             | '(' GOAL name formula goal_reward ')' goal_spec
             { add_goal (*$3, *$4, *$5); }
          /* | '(' GOAL formula goal_reward ')' { add_goal("noname", *$3, *$4); } */
          | metric_spec
          ;

goal_reward : /* metric_spec */ '(' GOAL_REWARD ground_f_exp ')' metric_spec
               { $$ = make_goal_reward(*$3); }
            | '(' GOAL_REWARD reward_formula ')'
               { $$ = $3; }
            | '(' GOAL_LINEAR_REWARD reward_linear_formula ')'
               { $$ = $3; }
            ;

reward_formula : goal_reward_expr { $$ = $1; }
| '(' and reward_formulas ')' { $$ = $3; }
| '(' when formula reward_formula ')'
{ $$ = &ConditionalGoalReward::make (*$3, *$4); }
;

reward_linear_formula : goal_linear_reward_expr { $$ = $1; }
| '(' and linear_reward_formulas ')' { $$ = $3; }
| '(' when formula reward_linear_formula ')'
{ $$ = &ConditionalGoalReward::make (*$3, *$4); }
;

goal_reward_expr : ground_f_exp
{ $$ = make_goal_reward (*$1); }
;

goal_linear_reward_expr : ground_f_exp goal_linear_reward_expr
{ add_linear_reward_comp (*$1, $2); 
 $$ = $2; }
| ground_f_exp
{ $$ = make_goal_linear_reward (*$1); }
;

reward_formulas : /* empty */ { $$ = new ConjunctiveGoalReward ();}
| reward_formulas reward_formula {$$ = $1; $$->add_conjunct (*$2); }
;

linear_reward_formulas : { $$ = new ConjunctiveGoalReward (); }
| linear_reward_formulas '(' reward_linear_formula ')' { $$ = $1; $$->add_conjunct (*$3); }
;

metric_spec : /* empty */ { set_default_metric(); }
            | '(' METRIC maximize { metric_fluent = true; } ground_f_exp ')'
                { problem->set_metric(*$5); metric_fluent = false; }
            | '(' METRIC minimize { metric_fluent = true; } ground_f_exp ')'
                { problem->set_metric(*$5, true); metric_fluent = false; }
            ;

/* ====================================================================== */
/* Formulas. */

formula : atomic_term_formula { $$ = $1; }
        | '(' '=' term_or_f_exp
            { first_eq_term = eq_term; first_eq_expr = eq_expr; }
            term_or_f_exp ')' { $$ = make_equality(); }
        | '(' binary_comp { require_fluents(); } f_exp f_exp ')'
            { $$ = new Comparison($2, *$4, *$5); }
        | '(' not formula ')' { $$ = make_negation(*$3); }
        | '(' and conjuncts ')' { $$ = $3; }
        | '(' or { require_disjunction(); } disjuncts ')' { $$ = $4; }
        | '(' imply formula formula ')' { $$ = make_implication(*$3, *$4); }
        | '(' exists { prepare_exists(); } '(' variables ')' formula ')'
            { $$ = make_exists(*$7); }
        | '(' forall { prepare_forall(); } '(' variables ')' formula ')'
            { $$ = make_forall(*$7); }
        ;

conjuncts : /* empty */ { $$ = new Conjunction(); }
          | conjuncts formula { $$->add_conjunct(*$2); }
          ;

disjuncts : /* empty */ { $$ = new Disjunction(); }
          | disjuncts formula { $$->add_disjunct(*$2); }
          ;

atomic_term_formula : '(' predicate { prepare_atom($2); } terms ')'
                        { $$ = make_atom(); }
                    | predicate { prepare_atom($1); $$ = make_atom(); }
                    ;

atomic_name_formula : '(' predicate { prepare_atom($2); } names ')'
                        { $$ = make_atom(); }
                    | predicate { prepare_atom($1); $$ = make_atom(); }
                    ;

binary_comp : '<' { $$ = Comparison::LT_CMP; }
            | LE { $$ = Comparison::LE_CMP; }
            | GE { $$ = Comparison::GE_CMP; }
            | '>' {$$ = Comparison::GT_CMP; }
            ;


/* ====================================================================== */
/* Function expressions. */

f_exp : NUMBER { $$ = new Value(*$1); delete $1; }
      | '(' '+' f_exp f_exp ')' { $$ = new Addition(*$3, *$4); }
      | '(' '-' f_exp opt_f_exp ')' { $$ = make_subtraction(*$3, $4); }
      | '(' '*' f_exp f_exp ')' { $$ = new Multiplication(*$3, *$4); }
      | '(' '/' f_exp f_exp ')' { $$ = new Division(*$3, *$4); }
| distribution_exp { $$ = $1; }
      | f_head { $$ = $1; }
      ;

/* TODO: this is a bit of a hack... f_head should include f_exp... */
distribution_exp : distribution_type f_head f_head discretization_type f_head f_head
{$$ = new ProbabilisticApplication ($1, *$2, *$3, $4, *$5, *$6);  }
| distribution_type NUMBER NUMBER discretization_type NUMBER NUMBER
{const Value *v1 = new Value (*$2); const Value *v2 = new Value (*$3);
 const Value *v3 = new Value (*$5); const Value *v4 = new Value (*$6);
 $$ = new ProbabilisticApplication ($1, *v1, *v2, $4, *v3, *v4);}
| distribution_type f_head f_head
  { $$ = new ProbabilisticApplication ($1, *$2, *$3); }
  | distribution_type NUMBER NUMBER
  { const Value *v1 = new Value (*$2); const Value *v2 = new Value (*$3);
  $$ = new ProbabilisticApplication ($1, *v1, *v2); }

term_or_f_exp : NUMBER
                  { require_fluents(); eq_expr = new Value(*$1); delete $1; }
              | '(' '+' { require_fluents(); } f_exp f_exp ')'
                  { eq_expr = new Addition(*$4, *$5); }
              | '(' '-' { require_fluents(); } f_exp opt_f_exp ')'
                  { eq_expr = make_subtraction(*$4, $5); }
              | '(' '*' { require_fluents(); } f_exp f_exp ')'
                  { eq_expr = new Multiplication(*$4, *$5); }
              | '(' '/' { require_fluents(); } f_exp f_exp ')'
                  { eq_expr = new Division(*$4, *$5); }
              | '(' function { require_fluents(); prepare_application($2); }
                  terms ')' { eq_expr = make_application(); }
              | name { make_eq_name($1); }
              | variable { eq_term = make_term($1); eq_expr = NULL; }
              ;

opt_f_exp : /* empty */ { $$ = NULL; }
          | f_exp
          ;

f_head : '(' function { prepare_application($2); } terms ')'
           { $$ = make_application(); }
       | function { prepare_application($1); $$ = make_application(); }
       ;

ground_f_exp : NUMBER { $$ = new Value(*$1); delete $1; }
             | '(' '+' ground_f_exp ground_f_exp ')'
                 { $$ = new Addition(*$3, *$4); }
             | '(' '-' ground_f_exp opt_ground_f_exp ')'
                 { $$ = make_subtraction(*$3, $4); }
             | '(' '*' ground_f_exp ground_f_exp ')'
                 { $$ = new Multiplication(*$3, *$4); }
             | '(' '/' ground_f_exp ground_f_exp ')'
                 { $$ = new Division(*$3, *$4); }
             | ground_f_head { $$ = $1; }
             | GOAL_PROBABILITY
                 { prepare_application($1); $$ = make_application(); }
             ;

opt_ground_f_exp : /* empty */ { $$ = NULL; }
                 | ground_f_exp
                 ;

ground_f_head : '(' function { prepare_application($2); } names ')'
                  { $$ = make_application(); }
                | function
                    { prepare_application($1); $$ = make_application(); }
              ;


/* ====================================================================== */
/* Terms and types. */

terms : /* empty */
      | terms name { add_term($2); }
      | terms variable { add_term($2); }
      ;

names : /* empty */
      | names name { add_term($2); }
      ;

variables : /* empty */
          | variable_seq { add_variables($1, OBJECT_TYPE); }
          | variable_seq type_spec { add_variables($1, $2); } variables
          ;

variable_seq : variable { $$ = new std::vector<const std::string*>(1, $1); }
             | variable_seq variable { $$ = $1; $$->push_back($2); }
             ;

typed_names : /* empty */
            | name_seq { add_names($1, OBJECT_TYPE); }
            | name_seq type_spec { add_names($1, $2); } typed_names
            ;

name_seq : name { $$ = new std::vector<const std::string*>(1, $1); }
         | name_seq name { $$ = $1; $$->push_back($2); }
         ;

type_spec : '-' { require_typing(); } type { $$ = $3; }
          ;

type : object { $$ = OBJECT_TYPE; }
     | type_name { $$ = make_type($1); }
     | '(' either types ')' { $$ = make_type(*$3); delete $3; }
     ;

types : object { $$ = new TypeSet(); }
      | type_name { $$ = new TypeSet(); $$->insert(make_type($1)); }
      | types object { $$ = $1; }
      | types type_name { $$ = $1; $$->insert(make_type($2)); }
      ;

function_type : number
              ;


/* ====================================================================== */
/* Tokens. */

define : DEFINE { delete $1; }
       ;

domain : DOMAIN_TOKEN { delete $1; }
       ;

problem : PROBLEM { delete $1; }
        ;

when : WHEN { delete $1; }
     ;

not : NOT { delete $1; }
    ;

and : AND { delete $1; }
    ;

or : OR { delete $1; }
   ;

imply : IMPLY { delete $1; }
      ;

exists : EXISTS { delete $1; }
       ;

forall : FORALL { delete $1; }
       ;

probabilistic : PROBABILISTIC { delete $1; }
              ;

assign : ASSIGN { delete $1; }
       ;

scale_up : SCALE_UP { delete $1; }
         ;

scale_down : SCALE_DOWN { delete $1; }
           ;

increase : INCREASE { delete $1; }
         ;

decrease : DECREASE { delete $1; }
         ;

increase_probabilistic : INCREASE_PROBABILISTIC { delete $1; }

decrease_probabilistic : DECREASE_PROBABILISTIC { delete $1; }

minimize : MINIMIZE { delete $1; }
         ;

maximize : MAXIMIZE { delete $1; }
         ;

number : NUMBER_TOKEN { delete $1; }
       ;

object : OBJECT_TOKEN { delete $1; }
       ;

either : EITHER { delete $1; }
       ;

type_name : DEFINE | DOMAIN_TOKEN | PROBLEM
          | EITHER
          | MINIMIZE | MAXIMIZE
          | NAME
          ;

predicate : type_name
          | OBJECT_TOKEN | NUMBER_TOKEN
          ;

function : name
         ;

name : DEFINE | DOMAIN_TOKEN | PROBLEM
     | NUMBER_TOKEN | OBJECT_TOKEN | EITHER
     | WHEN | NOT | AND | OR | IMPLY | EXISTS | FORALL | PROBABILISTIC
     | ASSIGN | SCALE_UP | SCALE_DOWN | INCREASE | DECREASE 
     | INCREASE_PROBABILISTIC DECREASE_PROBABILISTIC
     | MINIMIZE | MAXIMIZE
     | NAME
     ;

variable : VARIABLE
         ;

resource : name
         ;

bound : NUMBER
       ;

%%

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

/* Creates a resource as a function with the given name. */
static void make_resource (const std::string *name, const Rational *low,
			   const Rational *high)
{
  repeated_function = false;
  std::pair<Function, bool> f = domain->functions().find_function(*name);
  if (!f.second) {
    f.first = domain->functions().add_resource(*name, low->double_value (), 
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
