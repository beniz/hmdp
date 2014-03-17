/**
 * Copyright 2014 Emmanuel Benazera beniz@droidnik.fr
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "HmdpPpddlLoader.h"
#include <errno.h>

/* parser structures */
#include "states.h"
#include "problems.h"
#include "domains.h"
#include "exceptions.h"
#include "probabilityDistribution.h"
#include "DiscreteDistribution.h"  /* distribution type */
#include "PiecewiseConstantReward.h"
#include "PiecewiseLinearReward.h"
#include <string.h>
#include <unordered_map>

//#define LOADER_VERBOSE 1
//#define DEBUG 1

using namespace ppddl_parser;

extern int yyparse (); /**< the parse function */
extern FILE *yyin;  /**< File to parse */

/* Name of current file. */
std::string current_file;
/* Level of warnings. */
int warning_level;

namespace hmdp_loader
{

bool HmdpPpddlLoader::m_defaultDiscretizationRelative = true;
discretizationType HmdpPpddlLoader::m_defaultDiscretizationType = DISCRETIZATION_WRT_INTERVAL;
double HmdpPpddlLoader::m_defaultDiscretizationInterval = 5.0;
double HmdpPpddlLoader::m_defaultDiscretizationEpsilon = 0.1;
const Problem* HmdpPpddlLoader::m_firstProblem = 0;

bool HmdpPpddlLoader::load_file (const char *filename)
{
  /* load from file */
  try {
    if (! HmdpPpddlLoader::parse_file (filename))
      return false;
    else 
      {
#ifdef DEBUG
	std::cout << "File " << filename << " successfully parsed.\n";
#endif
	return true;
      }
  } catch (const Exception &e) {
    std::cerr << PACKAGE ": " << e << std::endl;
    return false;
  } catch (...) {
    std::cerr << PACKAGE ": fatal error" << std::endl;
    return false;
  }
}

bool HmdpPpddlLoader::parse_file (const char *filename)
{
  yyin = fopen(filename, "r");

  if (yyin == NULL) {
    std::cerr << PACKAGE << ':' << filename << ": " << strerror(errno)
	      << std::endl;
    return false;
  } else {
    bool success = (yyparse() == 0);
    fclose(yyin);
    return success;
  }
}

const Domain* HmdpPpddlLoader::getDomain(const int &dm)
{
  int d = 0;
  for (Domain::DomainMap::const_iterator di = Domain::begin();
       di != Domain::end(); di++)
    {
      if (d == dm)
	return (*di).second;
      d++;
    }
  std::cout << "[Error]:HmdpPpddlLoader: no ppddl domain found! Exiting.\n";
  exit (-1);
}

std::vector <std::pair<std::string, std::pair<double, double> > >* HmdpPpddlLoader::getResources()
{
  return HmdpPpddlLoader::getResources(0);
}

std::vector <std::pair<std::string, std::pair<double, double> > >* HmdpPpddlLoader::getResources (const int &dm)
{
  /* consider the first domain only */
  //Domain::DomainMap::const_iterator di = Domain::begin ();
  //const Domain *domain = (*di).second;
  const Domain *domain = HmdpPpddlLoader::getDomain(dm);
  const FunctionTable &functable = domain->functions ();
  if (functable.getNResources ())
    {
      return const_cast<FunctionTable&> (functable).getResources ();
    }
  else
    {
      std::cerr << "[Error]: HmdpPpddlLoader::getResources: no resources found. Exiting.\n";
      exit (-1);
    }
}

std::pair<std::string, std::pair<double, double> >& HmdpPpddlLoader::getResource (const std::string &rsc)
{
  return HmdpPpddlLoader::getResource(rsc, 0);
}

std::pair<std::string, std::pair<double, double> >& HmdpPpddlLoader::getResource (const std::string &rsc, const int &dm)
{
  std::vector <std::pair<std::string, std::pair<double, double> > > *br
    = HmdpPpddlLoader::getResources (dm);
  for (size_t i=0; i<br->size (); i++)
    if ((*br)[i].first == rsc)
      return (*br)[i];
  std::cout << "[Error]:HmdpPpddlLoader::getResource: cant't find resource: " 
	    << rsc << ".Exiting.\n";
  exit (-1);
}

std::pair<std::string, std::pair<double, double> >& HmdpPpddlLoader::getResource (const int &pos)
{
  return HmdpPpddlLoader::getResource(pos, 0);
}

std::pair<std::string, std::pair<double, double> >& HmdpPpddlLoader::getResource (const int &pos, const int &dm)
{
  std::vector <std::pair<std::string, std::pair<double, double> > > *br
    = HmdpPpddlLoader::getResources (dm);
  return (*br)[pos];
}

HybridTransition* HmdpPpddlLoader::convertAction(const Action &act, const size_t &nrsc)
{
  return HmdpPpddlLoader::convertAction(act, nrsc, 0);
}

HybridTransition* HmdpPpddlLoader::convertAction (const Action &act, const size_t &nrsc,
						  const int &dm)
{
  //debug
#ifdef LOADER_VERBOSE
  std::cout << "[Debug]:HmdpPpddlLoader::convertAction: start conversion of action: " 
	    << act.name () << std::endl;
#endif
  //debug
  
  /* action id */
  const short aid = static_cast<const short> (act.id ());

    /* action precondition on resources */
  double *prec_low = new double[nrsc]; double *prec_high = new double[nrsc];
  HmdpPpddlLoader::fillUpBounds (prec_low, prec_high, dm);
  HmdpPpddlLoader::convertRscSFToBounds (act.precondition (), prec_low, prec_high, dm);
  
  /* convert action effects */
  size_t dim = HmdpPpddlLoader::getNActionEffects (act); 
  double *prob = new double[dim];
  ContinuousTransition **ctrans = new ContinuousTransition*[dim]; 
  ContinuousReward **crew = new ContinuousReward*[dim];
  for (size_t i=0; i<dim; i++)
    {
      ctrans[i] = NULL; crew[i] = NULL;
    }

  HmdpPpddlLoader::convertActionEffects (act, nrsc, prob, ctrans, crew, prec_low, prec_high,
					 dm);
  
  /* create hybrid transition */
  HybridTransition *htrans = new HybridTransition (dim, aid, prob, ctrans, crew);
  
  //debug
#ifdef LOADER_VERBOSE
  if (HmdpPpddlLoader::m_firstProblem)
    {
      std::cout << "[Debug]:HmdpPpddlLoader::convertAction: successfully converted action: ";
      const_cast<Action&>(act).print_complete_name (std::cout, HmdpPpddlLoader::m_firstProblem->terms ());         
      std::cout << std::endl;
    }
#endif
  //debug
  
  delete []prob;
  delete []prec_low; delete []prec_high;
  delete []ctrans; delete []crew;

  return htrans;
}

size_t HmdpPpddlLoader::getNActionEffects (const Action &act)
{
  const Effect &eff = act.effect ();
  if (eff.getType () == EF_PROB)
    {
      const ProbabilisticEffect &peff = static_cast<const ProbabilisticEffect&> (eff);
      return peff.size ();
    }
  else return 1;
}

void HmdpPpddlLoader::convertActionEffects (const Action &act, const size_t &nrsc,
					    double *prob,
					    ContinuousTransition **ctrans, 
					    ContinuousReward **crew,
					    double *prec_low, double *prec_high)
{
  HmdpPpddlLoader::convertActionEffects(act, nrsc, prob, ctrans, crew,
					prec_low, prec_high, 0);
}

void HmdpPpddlLoader::convertActionEffects (const Action &act, const size_t &nrsc,
					    double *prob,
					    ContinuousTransition **ctrans, 
					    ContinuousReward **crew,
					    double *prec_low, double *prec_high,
					    const int &dm)
{
  /* number of probabilistic effects and probabilities */
  const Effect &eff = act.effect ();
  if (eff.getType () == EF_PROB)
    {
      const ProbabilisticEffect &peff 
	= static_cast<const ProbabilisticEffect&> (eff);
      for (size_t i=0; i<peff.size (); i++)
	{
	  prob[i] = peff.probability (i).double_value ();
	  
	  /* convert each continuous effect */
	  std::vector<double*> low, high;
	  ContinuousTransition *ct 
	    = HmdpPpddlLoader::convertContinuousActionEffect (peff.effect (i), nrsc, low, high,
							      prec_low, prec_high, dm);
	  if (ctrans[i]!=NULL)
	    BspTree::deleteBspTree(ctrans[i]);
	  ctrans[i] = ct;

#ifdef DEBUG
	  //debug
	  ct->print (std::cout, prec_low, prec_high);
	  std::cout << std::endl;
	  //debug
#endif
	}
    }
  else 
    {
      prob[0] = 1.0;

      /* convert continuous effect */
      std::vector<double*> low, high;
      ContinuousTransition *ct
	= HmdpPpddlLoader::convertContinuousActionEffect (eff, nrsc, low, high, prec_low, prec_high, dm);
      if (ctrans[0]!=NULL)
	BspTree::deleteBspTree(ctrans[0]);
      ctrans[0] = ct;

#ifdef DEBUG
      //debug
      ct->print (std::cout, prec_low, prec_high);
      std::cout << std::endl;
      //debug
#endif
    }
  
  /* TODO: convert continuous action rewards */
  
}

int HmdpPpddlLoader::convertRscSFToBounds(const StateFormula &stf,
					  double *low, double *high)
{
  return HmdpPpddlLoader::convertRscSFToBounds(stf, low, high, 0);
}

int HmdpPpddlLoader::convertRscSFToBounds(const StateFormula &stf, 
					  double *low, double *high,
					  const int &dm)
{
  /* iterate subformulas */
  StfType stft = stf.getType ();
  if (stft == STF_CONJ)  /* Conjunction */
    {
      /* convert all formulas */
      int rscbounds = 0;
      const Conjunction &conj = static_cast<const Conjunction&> (stf);
      for (size_t i=0; i<conj.size (); i++)
	rscbounds += HmdpPpddlLoader::convertRscSFToBounds (conj.conjunct (i), low, high, dm);
      return rscbounds;
    }
  else if (stft == STF_CMP)  /* Comparison */
    {
      const Comparison &comp = static_cast<const Comparison&> (stf);
      return HmdpPpddlLoader::convertRscExprToBounds (comp, low, high, dm);
    }
  else 
    {
#ifdef DEBUG
      std::cout << "[Warning]:HmdpPpddlLoader::convertRscSFToBounds: comparison type not handled: " << stft << std::endl;
#endif
      return 0;
    }
}

int HmdpPpddlLoader::convertRscExprToBounds(const Comparison &comp,
					    double *low, double *high)
{
  return HmdpPpddlLoader::convertRscExprToBounds(comp, low, high, 0);
}

int HmdpPpddlLoader::convertRscExprToBounds (const Comparison &comp,
					     double *low, double *high,
					     const int &dm)
{
  /* comparison predicate */
  Comparison::CmpPredicate compPred = comp.predicate ();
  const Expression &e1 = comp.expr1 ();
  const Expression &e2 = comp.expr2 ();

  /* check for resources */
  double rvalue = 0.0; std::string resource;
  
  if (e1.getType () == EXPR_APP)
    {
      const Application &app1 = static_cast<const Application&> (e1);
      //Domain::DomainMap::const_iterator di = Domain::begin ();
      const Domain *domain = HmdpPpddlLoader::getDomain(dm);
      if (e2.getType () == EXPR_APP)
	{
	  const Application& app2 = static_cast<const Application&> (e2);
	  if (domain->functions ().isResource (app1.function ()))
	    {
	      resource = domain->functions ().name (app1.function ());
	      rvalue = HmdpPpddlLoader::findFunctionValue (app2.function (), dm);
	    }
	  else if (domain->functions ().isResource (app2.function ()))
	    {
	      resource = domain->functions ().name (app2.function ());
	      rvalue = HmdpPpddlLoader::findFunctionValue (app1.function (), dm);
	    }
	  else return 0;  /* expression does not contain a resource */
	}
      else if (e2.getType () == EXPR_VAL)
	{
	  if (domain->functions ().isResource (app1.function ()))
	    {
	      const Value &val2 = static_cast<const Value&> (e2);
	      resource = domain->functions ().name (app1.function ());
	      rvalue = val2.value ().double_value ();
	    }
	  else return 0; /* no resource */
	}
    }
  else 
    {
      std::cout << "[Error]:HmdpPpddlLoader::convertRscExprToBounds: expression types not understood: " 
		<< "e1: " << e1.getType () << " -- e2: " << e2.getType () 
		<< ".Exiting.\n";
      exit (-1);
    }
  
  //debug
#ifdef LOADER_VERBOSE
  std::cout << "[Debug]::HmdpPpddlLoader::convertRscExprToBounds: rvalue: " 
	    << rvalue << std::endl;
#endif
  //debug

  /* get resources */
  int pos = HmdpPpddlLoader::getRscPosition (resource, dm);
  
  /* set bound */
  if (compPred == Comparison::LT_CMP || compPred == Comparison::LE_CMP)  /* set upper bound */
    {
      high[pos] = rvalue;
    }
  else if (compPred == Comparison::GT_CMP || compPred == Comparison::GE_CMP)  /* set lower bound */
    {
      low[pos] = rvalue;
    }
  
  return 1;
}

double HmdpPpddlLoader::findFunctionValue(const Function &fct)
{
  return HmdpPpddlLoader::findFunctionValue(fct, 0);
}

double HmdpPpddlLoader::findFunctionValue (const Function &fct, const int& pb)
{
  //const Problem *problem = (*Problem::begin ()).second;
  const Problem *problem = HmdpPpddlLoader::getProblem(pb);
  for (ValueMap::const_iterator vi = problem->init_values().begin ();
       vi != problem->init_values ().end (); vi++)
    if ((*vi).first->function () == fct)
      return (*vi).second.double_value ();
  
  std::cerr << "[Error]:HmdpPpddlLoader::findFunctionValue: can't find function value: "
	    << fct << " in problem " << pb << ". Exiting.\n";
  exit (-1);
}

int HmdpPpddlLoader::convertContinuousEffect (const Effect &eff,
					      const size_t &nrsc,
					      std::vector<double*> &low,
					      std::vector<double*> &high,
					      std::vector<double*> &means,
					      std::vector<double*> &sds,
					      std::vector<discreteDistributionType*> &distribs,
					      std::vector<discretizationType*> &disczs,
					      std::vector<double*> &discz_arg1,
					      std::vector<double*> &discz_arg2)
{
  return HmdpPpddlLoader::convertContinuousEffect(eff, nrsc, low, high, means, sds,
						  distribs, disczs, discz_arg1, discz_arg2,
						  0);
}

/* BEWARE: this is limited conversion only... ultimately, we should a
   fully recursive function... */
int HmdpPpddlLoader::convertContinuousEffect (const Effect &eff,
					      const size_t &nrsc,
					      std::vector<double*> &low,
					      std::vector<double*> &high,
					      std::vector<double*> &means,
					      std::vector<double*> &sds,
					      std::vector<discreteDistributionType*> &distribs,
					      std::vector<discretizationType*> &disczs,
					      std::vector<double*> &discz_arg1,
					      std::vector<double*> &discz_arg2,
					      const int &dm)
{
  EffectType efftype = eff.getType ();
  double mean; double variance; double arg1; double arg2; std::string rscName; bool found = false;
  discreteDistributionType dt;
  discretizationType dzt;

  if (efftype == EF_ASSIGN)
    {
      const AssignmentEffect &assignEffect 
	= static_cast<const AssignmentEffect&> (eff);
      double *effMeans = new double[nrsc]; double *effSds = new double[nrsc];
      double *effArg1 = new double[nrsc]; double *effArg2 = new double[nrsc];
      discretizationType *effDzt = new discretizationType[nrsc];
      discreteDistributionType *effDistribs = new discreteDistributionType[nrsc];
      for (size_t j=0; j<nrsc; j++)
	{
	  effDistribs[j] = NONE;
	  effDzt[j] = HmdpPpddlLoader::m_defaultDiscretizationType;
	}
      HmdpPpddlLoader::convertRscPdf (assignEffect, rscName, dt, mean, variance, dzt, arg1, arg2, found, dm);
      if (found)
	{
	  /* fill up distributions parameters (uni-dimensionnal) */
	  int pos = HmdpPpddlLoader::getRscPosition (rscName, dm);
	  effMeans[pos] = mean;
	  effSds[pos] = variance;
	  effDistribs[pos] = dt;
	  effDzt[pos] = dzt;
	  effArg1[pos] = arg1;
	  effArg2[pos] = arg2;
	  
	  means.push_back (effMeans);
	  sds.push_back (effSds);
	  distribs.push_back (effDistribs);
	  disczs.push_back (effDzt);
	  discz_arg1.push_back (effArg1);
	  discz_arg2.push_back (effArg2);

	  /* no condition */
	  if (low.size () < means.size ())
	    {
	      double *predlow = new double[nrsc]; double *predhigh = new double[nrsc];
	      HmdpPpddlLoader::fillUpBounds (predlow, predhigh, dm);
	      low.push_back (predlow); high.push_back (predhigh);
	    }

	  return 1;
	}
      else 
	{
	  delete []effMeans; delete []effSds; delete []effDistribs;
	  delete []effArg1; delete []effArg2;
	  return 0;
	}
    }
  else if (efftype == EF_COND)
    {
      const ConditionalEffect& ceff = static_cast<const ConditionalEffect&> (eff);
       /* convert effect condition */
      double *predlow = new double[nrsc]; double *predhigh = new double[nrsc];
      HmdpPpddlLoader::fillUpBounds (predlow, predhigh, dm);
      int rscbounds = 0;
      if ((rscbounds = HmdpPpddlLoader::convertRscSFToBounds (ceff.condition (), predlow, predhigh, dm)))
	{
	  low.push_back (predlow); high.push_back (predhigh); /* i.e. 'creates' the tile. */
	}
      else 
	{
	  delete []predlow; delete []predhigh;
	}
	  /* convert effect */
      if (HmdpPpddlLoader::convertContinuousEffect (ceff.effect (), nrsc, low, high,
						    means, sds, distribs, disczs, discz_arg1, discz_arg2))
	{
	  return 1;
	}
      else if (rscbounds)
	{
	  std::vector<double*>::iterator ie = low.end ();
	  ie--; low.erase (ie);
	  ie = high.end (); ie--; high.erase (ie);
	  delete []predlow; delete []predhigh;
	  return 0;
	}
      else return 0;
    }
  else if (efftype == EF_CONJ)
    {
      int rsceffect = 0;
      int crsc = 0;
      bool condition = false;
      const ConjunctiveEffect &cjeff = static_cast<const ConjunctiveEffect&> (eff);
      double *effMeans = new double[nrsc]; double *effSds = new double[nrsc];
      double *effArg1 = new double[nrsc]; double *effArg2 = new double[nrsc];
      discretizationType *effDzt = new discretizationType[nrsc];
      discreteDistributionType *effDistribs = new discreteDistributionType[nrsc];
      for (size_t j=0; j<nrsc; j++)
	{
	  effMeans[j] = 0.0;
	  effDistribs[j] = NONE;
	  effDzt[j] = HmdpPpddlLoader::m_defaultDiscretizationType;
	}
      for (size_t i=0; i<cjeff.size (); i++)
	{
	  /* convert each effect */
	  if (cjeff.conjunct (i).getType () == EF_ASSIGN)
	    {
	      const AssignmentEffect &assignEffect 
		= static_cast<const AssignmentEffect&> (cjeff.conjunct (i));
	      HmdpPpddlLoader::convertRscPdf (assignEffect, rscName, dt, mean, variance, dzt, arg1, arg2, found, dm);
	      if (found)
		{
		  /* fill up distributions parameters (uni-dimensionnal) */
		  int pos = HmdpPpddlLoader::getRscPosition (rscName, dm);
		  effMeans[pos] = mean;
		  effSds[pos] = variance;
		  effDistribs[pos] = dt;
		  effDzt[pos] = dzt;
		  effArg1[pos] = arg1;
		  effArg2[pos] = arg2;
		  rsceffect++;
		  found = false;
		}
	      else
		{}
	    }
	  /* deactivated to allow when on non resource types in the action effects. */
	  /* else if (cjeff.conjunct (i).getType () == EF_COND)
	    { */
	      /* condition */
	      /* crsc += HmdpPpddlLoader::convertContinuousEffect (cjeff.conjunct(i), nrsc, low, high,
								means, sds, distribs, disczs, 
								discz_arg1, discz_arg2);
	      condition = true;
	    } */
	  else continue;
	}
      
      if (condition && crsc)  /* we clean up */
	{
	  delete []effMeans; delete []effSds; delete []effArg1; delete []effArg2;
	  delete []effDzt; delete []effDistribs;
	  return rsceffect;
	}
      
      if (! crsc)
	{
	  means.push_back (effMeans);
	  sds.push_back (effSds);
	  distribs.push_back (effDistribs);
	  disczs.push_back (effDzt);
	  discz_arg1.push_back (effArg1);
	  discz_arg2.push_back (effArg2);
	  
	  /* no condition */
	  if (low.size () < means.size ())
	    {
	      double *predlow = new double[nrsc]; double *predhigh = new double[nrsc];
	      HmdpPpddlLoader::fillUpBounds (predlow, predhigh, dm);
	      low.push_back (predlow); high.push_back (predhigh);
	    }
	}
      
      return rsceffect;
    }
  else { return 0; }
}

ContinuousTransition* HmdpPpddlLoader::convertContinuousActionEffect (const Effect &eff,
								      const size_t &nrsc,
								      std::vector<double*> &low, 
								      std::vector<double*> &high,
								      double *prec_low,
								      double *prec_high)
{
  return HmdpPpddlLoader::convertContinuousActionEffect(eff, nrsc, low, high, 
							prec_low, prec_high, 0);
}

//TODO: add domain.
ContinuousTransition* HmdpPpddlLoader::convertContinuousActionEffect (const Effect &eff,
								      const size_t &nrsc,
								      std::vector<double*> &low, 
								      std::vector<double*> &high,
								      double *prec_low,
								      double *prec_high,
								      const int &dm)
{
  std::vector<double*> means, sds, discz_arg1, discz_arg2;
  std::vector<discreteDistributionType*> distribs;
  std::vector<discretizationType*> disczs;
  HmdpPpddlLoader::convertContinuousEffect (eff, nrsc, low, high,
					    means, sds, distribs,
					    disczs, discz_arg1, discz_arg2,
					    dm);

  /* TODO: check the tiles */
  //HmdpPpddlLoader::checkCTTiles (nrsc, prec_low, prec_high, low, high, means, sds, 
  //distribs, disczs);
			       
  
  /* convert vectors to arrays */
  size_t neff = means.size ();
  double **ct_lowPos = new double*[neff];
  double **ct_highPos = new double*[neff];
  const double **ct_means = new const double*[neff];
  const double **ct_sds = new const double*[neff];
  const discreteDistributionType **ct_distrib = new const discreteDistributionType*[neff];
  bool **ct_relative = new bool*[neff];
  const double **ct_epsilon = new const double*[neff];
  const double **ct_intervals = new const double*[neff];
  
  for (size_t i=0; i<neff; i++)
    {
      ct_lowPos[i] = low[i];
      ct_highPos[i] = high[i];
      
      /* check for tiles bounds here, wrt. prec_low, prec_high */     
      for (size_t j=0; j<nrsc; j++)
	{
	  if (prec_low[j] > ct_lowPos[i][j])
	    ct_lowPos[i][j] = prec_low[j];
	  if (prec_high[j] < ct_highPos[i][j])
	    ct_highPos[i][j] = prec_high[j];
	}

      ct_distrib[i] = distribs[i];
      ct_means[i] = means[i];
      ct_sds[i] = sds[i];
      
      ct_relative[i] = new bool[nrsc];
      ct_epsilon[i] = new double[nrsc];
      ct_intervals[i] = new double[nrsc];
      
      
      for (size_t j=0; j<nrsc; j++)
	{
	  ct_relative[i][j] = HmdpPpddlLoader::m_defaultDiscretizationRelative;   /* TODO !!! */
	  if (ct_distrib[i][j] != NONE)
	    {
	      const_cast<double*> (ct_intervals[i])[j] = discz_arg1[i][j];
	      const_cast<double*> (ct_epsilon[i])[j] = discz_arg2[i][j];
	    }
	}
    }

  for (size_t i=0;i<discz_arg1.size();i++)
    delete[] discz_arg1[i];
  for (size_t i=0;i<discz_arg2.size();i++)
    delete[] discz_arg2[i];
  
  const discretizationType dt = HmdpPpddlLoader::m_defaultDiscretizationType;
  for (size_t k=0;k<disczs.size();k++)
    delete[] disczs[k];
  
  double *ct_low = new double[nrsc]; double *ct_high = new double[nrsc];
  HmdpPpddlLoader::fillUpBounds (ct_low, ct_high, dm);
  ContinuousTransition *ct = new ContinuousTransition (neff, nrsc, dt,
						       ct_lowPos, ct_highPos,
						       ct_low, ct_high,
						       ct_epsilon, ct_intervals,
						       ct_means, ct_sds, ct_relative,
						       ct_distrib);

  for (size_t i=0; i<neff; i++)
    {
      delete []ct_lowPos[i]; delete []ct_highPos[i]; delete []ct_distrib[i];
      delete []ct_means[i]; delete []ct_sds[i]; delete []ct_relative[i];
      delete []ct_epsilon[i]; delete []ct_intervals[i];
    }
  delete []ct_lowPos; delete []ct_highPos; delete []ct_means; delete []ct_sds;
  delete []ct_distrib; delete []ct_relative; delete []ct_epsilon; delete []ct_intervals;
  delete []ct_low; delete []ct_high;

  return ct;
}

void HmdpPpddlLoader::checkCTTiles (const size_t &nrsc, double *prec_low, double *prec_high,
				    std::vector<double*> &low, std::vector<double*> &high,
				    std::vector<double*> &means, std::vector<double*> &sds,
				    std::vector<discreteDistributionType*> &distribs,
				    std::vector<discretizationType*> &disczs)
{
  size_t neff = means.size ();
  bool fsTileLow = false;  /* if tile covers all resource space: lower bound. */
  bool fsTileHigh = false;  /* upper bound */

  double *minpos = new double[nrsc];
  double *maxpos = new double[nrsc];

  for (size_t r=0; r<nrsc; r++)
    {
      minpos[r] = prec_high[r];
      maxpos[r] = prec_low[r];
    }

  for (size_t i=0; i<neff; i++)
    {
      size_t flag_low = 0, flag_high = 0;
      
      /* check for tile bounds & resource space coverage */
      for (size_t j=0; j<nrsc; j++)
	{
	  if (prec_low[j] > low[i][j])
	    {
	      low[i][j] = prec_low[j];
	      flag_low++;
	    }
	  if (prec_high[j] < high[i][j])
	    {
	      high[i][j] = prec_high[j];
	      flag_high++;
	    }

	  if (low[i][j] == prec_low[j])
	    {
	      flag_low++;
	    }
	  if (high[i][j] == prec_high[j])
	    {
	      flag_high++;
	    }

	  /* fill up max/min pos for these action tiles */
	  if (low[i][j] < minpos[j])
	    minpos[j] = low[i][j];
	  if (high[i][j] > maxpos[j])
	    maxpos[j] = high[i][j];
	}
      
      if (! fsTileLow && flag_low == neff) 
	{
	  fsTileLow = true;
	}
      if (! fsTileHigh && flag_high == neff) 
	{
	  fsTileHigh = true;
	}
    }
  
  /* constuct complementary tiles, if needed. */
  if (! fsTileLow || ! fsTileHigh)
    {
      if (! fsTileLow)  /* a multi-dimensional tile is needed to meet 
			   the lower boundary. */
	{
	  double *lpos = new double[nrsc];
	  double *hpos = new double[nrsc];
	  double *mean = new double[nrsc];
	  double *variance = new double[nrsc];
	  discreteDistributionType *distrib = new discreteDistributionType[nrsc];
	  discretizationType *discz = new discretizationType[nrsc];
	  for (size_t j=0; j<nrsc; j++)
	    {
	      lpos[j] = prec_low[j];
	      hpos[j] = minpos[j];
	      
	      /* fake a distribution over resources */
	      mean[j] = 0.0; variance[j] = 0.0; distrib[j] = NONE;
	      discz[j] = HmdpPpddlLoader::m_defaultDiscretizationType;
	    }
	  low.push_back (lpos);
	  high.push_back (hpos);
	  means.push_back (mean); sds.push_back (variance);
	  distribs.push_back (distrib); disczs.push_back (discz);	  
	}

      if (! fsTileHigh) /* a multi-dimensional tile is needed to meet the
			   higher boundary. */
	{
	  
	}
      
    }

  delete []minpos; delete []maxpos;
}

void HmdpPpddlLoader::convertRscPdf (const AssignmentEffect &assignEff,
				     std::string &rsc, discreteDistributionType &dt,
				     double &mean, double &variance, discretizationType &dzt,
				     double &disczarg1, double &disczarg2, bool &found)
{
  HmdpPpddlLoader::convertRscPdf(assignEff, rsc, dt, mean, variance, dzt,
				 disczarg1, disczarg2, found, 0);
}

//BEWARE: using domain number as problem number here...
void HmdpPpddlLoader::convertRscPdf (const AssignmentEffect &assignEff,
				     std::string &rsc, discreteDistributionType &dt,
				     double &mean, double &variance, discretizationType &dzt,
				     double &disczarg1, double &disczarg2, bool &found,
				     const int &dm)
{
  found = false;
  const Assignment &assign = assignEff.assignment ();
  //Domain::DomainMap::const_iterator di = Domain::begin ();
  const Domain *domain = getDomain(dm);
  rsc = domain->functions ().name (assign.application ().function ());
  int rscpos;
  if ((rscpos = HmdpPpddlLoader::getRscPosition (rsc, dm)) == -1)  /* not a resource */
      return;
  
  Assignment::AssignOp aop = assign.getOperator ();

  if (aop == Assignment::INCREASE_OP || aop == Assignment::DECREASE_OP)  /* non probabilistic */
    {
      if (assign.expression ().getType () != EXPR_VAL)
	{
#ifdef LOADER_VERBOSE
	  std::cout << "[Debug]:HmdpPpddlLoader: converting resource consumption: deterministic operator but no value !\n";
#endif
	  return;
	}
      
      const Value &valExpr = static_cast<const Value&> (assign.expression ());
	mean = valExpr.value ().double_value ();
      if (aop == Assignment::DECREASE_OP)
	mean = -mean;
      variance = 0.0;  /* default... */
      dt = NONE;
      found = true;
    }
  else if (aop == Assignment::INCREASE_PROB_OP || aop == Assignment::DECREASE_PROB_OP) /* probabilistic */
    {
      if (assign.expression ().getType () != EXPR_PROBAPP)
	{
#ifdef LOADER_VERBOSE
	  std::cout << "[Debug]:HmdpPpddlLoader: converting resource consumption: probabilistic operator but no probabilitistic application !\n";
#endif
	  return;
	}
      
      const ProbabilisticApplication &probExpr 
	= static_cast<const ProbabilisticApplication&> (assign.expression ());
      const ProbabilityDistributionFunc *pdfunc = probExpr.getProbFunc ();
      
      const Expression *meanExpr = pdfunc->getMean ();
      const Expression *stdDevExpr = pdfunc->getStdDev ();

      if (meanExpr->getType () == EXPR_APP
	  && stdDevExpr->getType () == EXPR_APP)   /* non instantiated */
	{
	  mean 
	    = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*meanExpr).function (), dm);
	  if (aop == Assignment::DECREASE_PROB_OP)
	    mean = -mean;
	  variance 
	    = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*stdDevExpr).function (), dm);
	  if (pdfunc->getType () == DISTRIBUTION_NORMAL) dt = GAUSSIAN;
	  else if (pdfunc->getType () == DISTRIBUTION_UNIFORM) dt = UNIFORM;
	  
	  if (pdfunc->getDisczType () != DISCRETIZATION_DEFAULT)
	    {
	      const Expression *thresholdExpr = pdfunc->getThreshold ();
	      disczarg2 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*thresholdExpr).function (), dm);
	      if (pdfunc->getDisczType () == DISCRETIZATION_INTERVAL) 
		{
		  dzt = hmdp_base::DISCRETIZATION_WRT_INTERVAL;
		  const Expression *intervalExpr = pdfunc->getInterval ();
		  disczarg1 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*intervalExpr).function (), dm);
		}
	      else if (pdfunc->getDisczType () == DISCRETIZATION_THRESHOLD) 
		{
		  dzt = hmdp_base::DISCRETIZATION_WRT_THRESHOLD;
		  const Expression *intervalExpr = pdfunc->getInterval ();
		  disczarg1 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*intervalExpr).function (), dm);
		}
	      else if (pdfunc->getDisczType () == DISCRETIZATION_POINTS) 
		{
		  dzt = hmdp_base::DISCRETIZATION_WRT_POINTS;
		  const Expression *pointsExpr = pdfunc->getPoints ();
		  disczarg1 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*pointsExpr).function (), dm);
		}
	    }	      
	  else
	    {
	      dzt = HmdpPpddlLoader::m_defaultDiscretizationType;
	      disczarg1 = HmdpPpddlLoader::m_defaultDiscretizationInterval;
	      disczarg2 = HmdpPpddlLoader::m_defaultDiscretizationEpsilon;
	    }
	  found = true;
	}
      else if (meanExpr->getType () == EXPR_VAL
	       && stdDevExpr->getType () == EXPR_VAL)  /* non instantiated */
	{
	  const Value *meanval = static_cast<const Value*> (meanExpr);
	  const Value *stdval = static_cast<const Value*> (stdDevExpr);
	  mean = meanval->value ().double_value ();
	  if (aop == Assignment::DECREASE_PROB_OP)
	    mean = -mean;
	  variance = stdval->value ().double_value ();
	  if (pdfunc->getType () == DISTRIBUTION_NORMAL) dt = GAUSSIAN;
	  else if (pdfunc->getType () == DISTRIBUTION_UNIFORM) dt = UNIFORM;
	  found = true;
	  
	  if (pdfunc->getDisczType () != DISCRETIZATION_DEFAULT)
	    {
	      const Expression *thresholdExpr = pdfunc->getThreshold ();
	      disczarg2 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*thresholdExpr).function (), dm);
	      if (pdfunc->getDisczType () == DISCRETIZATION_INTERVAL) 
		{
		  dzt = hmdp_base::DISCRETIZATION_WRT_INTERVAL;
		  const Expression *intervalExpr = pdfunc->getInterval ();
		  disczarg1 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*intervalExpr).function (), dm);
		}
	      else if (pdfunc->getDisczType () == DISCRETIZATION_THRESHOLD) 
		{
		  dzt = hmdp_base::DISCRETIZATION_WRT_THRESHOLD;
		  const Expression *intervalExpr = pdfunc->getInterval ();
		  disczarg1 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*intervalExpr).function (), dm);
		}
	      else if (pdfunc->getDisczType () == DISCRETIZATION_POINTS) 
		{
		  dzt = hmdp_base::DISCRETIZATION_WRT_POINTS;
		  const Expression *pointsExpr = pdfunc->getPoints ();
		  disczarg1 = HmdpPpddlLoader::findFunctionValue (static_cast<const Application&> (*pointsExpr).function (), dm);
		}
	    }
	  else
	    {
	      dzt = HmdpPpddlLoader::m_defaultDiscretizationType;
	      disczarg1 = HmdpPpddlLoader::m_defaultDiscretizationInterval;
	      disczarg2 = HmdpPpddlLoader::m_defaultDiscretizationEpsilon;
	    }
	}
      else 
	{
	  std::cout << "[Error]:HmdpPpddlLoader: converting resource consumptions: probability distribution expressions not understood. Exiting.\n";
	  exit (-1);
	}
      
      //debug
#ifdef LOADER_VERBOSE
      std::cout << "[Debug]::HmdpPpddlLoader: converting probabilistic resource consumptions: "
		<< "mean: " << mean << " -- stdvar: " << variance 
		<< " -- interval: " << disczarg1 << " -- epsilon: " << disczarg2 << std::endl;
#endif
      //debug
      
    }
  /* else 
     {
     mean = 0.0;
     variance = 0.0;
     dt = NONE;
     found = false;
     } */
}

int HmdpPpddlLoader::getRscPosition(const std::string &rsc)
{
  return HmdpPpddlLoader::getRscPosition(rsc,0);
}

int HmdpPpddlLoader::getRscPosition (const std::string &rsc,
				     const int &dm)
{
  std::vector <std::pair<std::string, std::pair<double,double> > >* rscs = HmdpPpddlLoader::getResources(dm);
  
  //debug
  //std::cout << "rscs size: " << rscs->size () << std::endl;
  //debug

  int pos = 0; bool foundRsc = false;
  std::vector <std::pair<std::string, std::pair<double,double> > >::iterator it;
  for (it = rscs->begin (); it != rscs->end (); it++)
    {
      //debug
      //std::cout << "first: " << (*it).first << std::endl;
      //debug
      
      if ((*it).first == rsc)
	{
	  foundRsc = true;
	  break;
	}
      pos++;
    }
  if (foundRsc)
    return pos;
  else
    {
      std::cout << "[Error]: HmdpPpddlLoader: resource position not found: " << rsc << ".\n";
      return -1;
    }
}

void HmdpPpddlLoader::setFunctionValueInMap(const int &pos, ValueMap &values,
					    const double &val)
{
  HmdpPpddlLoader::setFunctionValueInMap(pos, values, val, 0);
}

void HmdpPpddlLoader::setFunctionValueInMap (const int &pos, ValueMap &values,
					     const double &val, const int &pb)
{
  //BEWARE: same domain as problem number...
  std::string fname = HmdpPpddlLoader::getResource (pos, pb).first;
  const Problem *problem = HmdpPpddlLoader::getProblem (pb);
  std::pair<Function, bool> fct = problem->domain ().functions ().find_function (fname);
  
  if (! fct.second)
    {
      std::cerr << "[Error]: HmdpPpddlLoader::setFunctionValueInMap: unknown function: "
		<< fname << ". Exiting.\n";
      exit (-1);
    }
  
  //hashing::hash_map<const Application*, Rational>::iterator vmi;
  std::unordered_map<const Application*,Rational>::iterator vmi;
  for (vmi = values.begin (); vmi != values.end (); vmi++)
    {
      if ((*vmi).first->function () == fct.first)
	{
	  (*vmi).second = Rational (val);
	  
	  //debug
	  /* if (val != (*vmi).second.double_value ())
	     {
	     Rational wrr = (*vmi).second;
	     std::cout << "wrong rat\n";
	     }
	     assert (val == (*vmi).second.double_value ()); */
	  
	  break;
	}
    }
}

double HmdpPpddlLoader::getFunctionValueInMap (const int &pos, const ValueMap &values)
{
  return HmdpPpddlLoader::getFunctionValueInMap(pos, values, 0);
}

double HmdpPpddlLoader::getFunctionValueInMap (const int &pos, const ValueMap &values,
					       const int &pb)
{
  //BEWARE: same domain as problem number...
  std::string fname = HmdpPpddlLoader::getResource (pos, pb).first;
  const Problem *problem = HmdpPpddlLoader::getProblem(pb);
  std::pair<Function, bool> fct = problem->domain ().functions ().find_function (fname);
  
  if (! fct.second)
    {
      std::cerr << "[Error]: HmdpPpddlLoader::getFunctionValueInMap: unknown function: "
		<< fname << ". Exiting.\n";
      exit (-1);
    }

  std::unordered_map<const Application*, Rational>::const_iterator vmi;
  for (vmi = values.begin (); vmi != values.end (); vmi++)
    {
      if ((*vmi).first->function () == fct.first)
	{
	  return (*vmi).second.double_value ();
	}
    }
  return 0.0; /* beware. */
}

void HmdpPpddlLoader::fillUpBounds (double *low, double *high)
{
  HmdpPpddlLoader::fillUpBounds(low, high, 0);
}

void HmdpPpddlLoader::fillUpBounds (double *low, double *high,
				    const int &dm)
{
  std::vector <std::pair<std::string, std::pair<double,double> > > *bds = HmdpPpddlLoader::getResources(dm);
  std::vector <std::pair<std::string, std::pair<double,double> > >::iterator it;
  size_t i=0;
  for (it = bds->begin (); it != bds->end (); it++)
    {
      low[i] = (*it).second.first;
      high[i] = (*it).second.second;
      i++;
    }
}

ContinuousReward* HmdpPpddlLoader::convertGoal (const Goal &goal, const size_t &nrsc)
{
  std::vector<double*> low, high;
  std::vector<std::vector<std::map<std::string, double>* > > values;

  const GoalLinearReward *gr = goal.getGoalReward ();
  PwRewardType pwt = HmdpPpddlLoader::convertGoalReward (*gr, nrsc, low, high, values);
  const int nTiles = static_cast<const int> (low.size ());
  double **cr_lowPos = new double*[nTiles];
  double **cr_highPos = new double*[nTiles];
  
  ContinuousReward *cr = 0;

  /* convert (mostly vectors to arrays) */
  if (pwt == GR_PW_CST)  /* pw constant reward */
    {  
      double *cr_values = new double[nTiles];
      for (int i=0; i<nTiles; i++)
	{
	  cr_lowPos[i] = low[i];
	  cr_highPos[i] = high[i];
	  cr_values[i] = (*values[i][0])["none"];
	}
      
      double *cr_low = new double[nrsc]; double *cr_high = new double[nrsc];
      HmdpPpddlLoader::fillUpBounds (cr_low, cr_high);
       
      cr = new PiecewiseConstantReward (nTiles, static_cast<const int> (nrsc), 
					cr_lowPos, cr_highPos, cr_low, cr_high, cr_values);
      
      delete []cr_values; delete []cr_high; delete []cr_low;
    }
  else if (pwt == GR_PW_LINEAR)  /* pw linear */
    {
      double ***cr_values = new double**[nTiles];
      int *linum = new int[nTiles];
      for (int i=0; i<nTiles; i++)
	{
	  cr_lowPos[i] = low[i];
	  cr_highPos[i] = high[i];
	  linum[i] = values[i].size ();

	  /* fill up the linear functions parameters */
	  //std::cout << "number of functions: " << values[i].size () << std::endl;
	  cr_values[i] = new double*[values[i].size ()];  /* number of functions in that tile */
	  for (size_t j=0; j<values[i].size (); j++)  /* iterate functions */
	    {
	      cr_values[i][j] = new double[nrsc + 1];  
	      for (size_t l=0; l<nrsc+1; l++)  /* initialize parameters */
		cr_values[i][j][l] = 0.0;
	      
	      std::map<std::string, double>::iterator mit;
	      size_t k = 0;
	      for (mit = values[i][j]->begin (); mit != values[i][j]->end (); mit++)
		{
		  if ((*mit).first != "none")
		    {
		      k = HmdpPpddlLoader::getRscPosition ((*mit).first);
		      cr_values[i][j][k] = (*mit).second;
		    }
		  else cr_values[i][j][nrsc] = (*mit).second;
		}
	    }
	}
      
      double *cr_low = new double[nrsc]; double *cr_high = new double[nrsc];
      HmdpPpddlLoader::fillUpBounds (cr_low, cr_high);
      
      cr = new PiecewiseLinearReward (nTiles, static_cast<const int> (nrsc), 
				      cr_lowPos, cr_highPos, cr_low, cr_high, 
				      linum, cr_values);
      delete []cr_values; delete []cr_low; delete []cr_high; delete []linum;
    }
  else
    {
      std::cout << "[Error]:HmdpPpddlLoader::convertGoal: unknown reward type. Exiting\n";
      exit (-1);
    }

  /* tag reward with goal index */
  cr->tagWithGoal (goal.getId ());
 
  delete []cr_lowPos; delete []cr_highPos;
  for (size_t i=0; i<values.size (); i++)
    for (size_t j=0; j<values[i].size (); j++)
      delete values[i][j];

  return cr;
}

PwRewardType HmdpPpddlLoader::convertGoalReward (const GoalLinearReward &gr, const size_t &nrsc,
						 std::vector<double*> &low, 
						 std::vector<double*> &high, 
						 std::vector<std::vector<std::map<std::string, double>* > > &values)
{
  return HmdpPpddlLoader::convertGoalReward(gr, nrsc, low, high, values, 0);
}

PwRewardType HmdpPpddlLoader::convertGoalReward (const GoalLinearReward &gr, const size_t &nrsc,
						 std::vector<double*> &low, 
						 std::vector<double*> &high, 
						 std::vector<std::vector<std::map<std::string, double>* > > &values, const int &dm)
{
  GoalRewardType grt = gr.getType ();

  if (grt == GR)
    {
      if (gr.getPwType () == GR_PW_CST)  /* pwc reward */
	{
	  const Assignment *assign = gr.getReward (0);
	  const Expression &expr = assign->expression ();
	  if (expr.getType () == EXPR_VAL)
	    {
	      const Value &val = static_cast<const Value&> (expr);
	      std::map<std::string, double> *valmap = new std::map<std::string, double> ();
	      valmap->insert (std::pair<std::string, double> ("none", val.value ().double_value ()));
	      /* if (low.size () < values.size ()) */  /* no condition */
	      if (low.size () == 0 && values.size () == 0)
		{
		  std::vector<std::map<std::string, double>* > vecmap;
		  vecmap.push_back (valmap);
		  values.push_back (vecmap);
		  double *cond_low = new double[nrsc];
		  double *cond_high = new double[nrsc];
		  HmdpPpddlLoader::fillUpBounds (cond_low, cond_high, dm);
		  low.push_back (cond_low); high.push_back (cond_high);
		}
	      else 
		{
		  /* std::vector<std::map<std::string, double>* > vecmap;
		     vecmap.push_back (valmap);
		     values.push_back (vecmap); */
		  values.back ().push_back (valmap);
		}
	    }
	  else 
	    {
	      std::cout << "[Error]:HmdpPpddlLoader::convertGoal: expression is no value. Exiting.\n";
	      exit (-1);
	    }
	  return GR_PW_CST;
	}
      else if (gr.getPwType () == GR_PW_LINEAR)  /* pwl reward */
	{
	  //Domain::DomainMap::const_iterator di = Domain::begin ();
	  const Domain *domain = getDomain(dm);
	  std::map<std::string, double> *valmap = new std::map<std::string, double> ();
	  const std::vector<Assignment*> &vecassign = gr.getReward ();
	  for (size_t i=0; i<vecassign.size (); i++)
	    {
	      Assignment *assign = vecassign[i];
	      const Expression &expr = assign->expression ();

	      if (expr.getType () == EXPR_MULT)
		{
		  const Multiplication &mult = static_cast<const Multiplication&> (expr);
		  const Expression &expr1 = mult.factor1 ();
		  const Expression &expr2 = mult.factor2 ();
		  
		  if (expr1.getType () == EXPR_APP
		      && expr2.getType () == EXPR_VAL)
		    {
		      const Application &app = static_cast<const Application&> (expr1);
		      const Value &val = static_cast<const Value&> (expr2);
		      std::string rscname = domain->functions ().name (app.function ());
		      double rscvalue = val.value ().double_value ();
		      valmap->insert (std::pair<std::string, double> (rscname, rscvalue));
		    }
		  else if (expr2.getType () == EXPR_APP
			   && expr1.getType () == EXPR_VAL)
		    {
		      const Application &app = static_cast<const Application&> (expr2);
		      const Value &val = static_cast<const Value&> (expr1);
		      std::string rscname = domain->functions ().name (app.function ());
		      double rscvalue = val.value ().double_value ();
		      valmap->insert (std::pair<std::string, double> (rscname, rscvalue));
		    }
		  else if (expr2.getType () == EXPR_APP
			   && expr1.getType () == EXPR_SUB)
		    {
		      const Subtraction &sub = static_cast<const Subtraction&> (expr1);
		      const Application &app = static_cast<const Application&> (expr2);
		      std::string rscname = domain->functions ().name (app.function ());
		      const Value &sub_val2 = static_cast<const Value&> (sub.term2 ());
		      
		      /* number is second term... */
		      double rscvalue = - sub_val2.value ().double_value ();
		      valmap->insert (std::pair<std::string, double> (rscname, rscvalue));
		    }
		  else if (expr1.getType () == EXPR_APP
			   && expr2.getType () == EXPR_SUB)
		    {
		      const Subtraction &sub = static_cast<const Subtraction&> (expr2);
		      const Application &app = static_cast<const Application&> (expr1);
		      std::string rscname = domain->functions ().name (app.function ());
		      const Value &sub_val2 = static_cast<const Value&> (sub.term2 ());
		      
		      /* number is second term... */
		      double rscvalue = - sub_val2.value ().double_value ();
		      valmap->insert (std::pair<std::string, double> (rscname, rscvalue));
		    }
		  else 
		    {
		      std::cout << "[Error]:HmdpPpddlLoader::convertGoal: linear reward types not handled. Exiting.\n";
		      std::cout << "expr1 type: " << expr1.getType () 
				<< " -- expr2 type: " << expr2.getType () << std::endl;
		      exit (-1);
		    }
		}  /* end if MULT */
	      else if (expr.getType () == EXPR_VAL)
		{
		  const Value &val = static_cast<const Value&> (expr);
		  valmap->insert (std::pair<std::string, double> ("none", val.value ().double_value ()));
		}  /* end if VAL */
	      else 
		{
		  std::cout << "[Error]:HmdpPpddlLoader::convertGoal: expr not handled: "
			    << expr.getType () << ". Exiting.\n";
		  exit (-1);
		}
	    } /* end for */
	  
	  if (low.size () < values.size ())  	/* no condition */
	    {
	      std::vector<std::map<std::string, double>* > vecmap;
	      vecmap.push_back (valmap);
	      values.push_back (vecmap);
	      double *cond_low = new double[nrsc];
	      double *cond_high = new double[nrsc];
	      HmdpPpddlLoader::fillUpBounds (cond_low, cond_high, dm);
	      low.push_back (cond_low); high.push_back (cond_high);
	    }
	  else
	    {
	      /* std::vector<std::map<std::string, double>* > vecmap;
		 vecmap.push_back (valmap);
		 values.push_back (vecmap); */
	      values.back ().push_back (valmap);
	    }
	}  /* end if LINEAR */
      else 
	{
	  std::cout << "[Error]:HmdpPpddlLoader::convertGoal: reward type not understood: " 
		    << gr.getPwType () << ". Exiting.\n";
	  exit (-1);
	}
      return GR_PW_LINEAR;
    }
  else if (grt == GR_COND)  /* this creates a new tile */
    {
      const ConditionalGoalReward &cgr = static_cast<const ConditionalGoalReward&> (gr);
      double *cond_low = new double[nrsc];
      double *cond_high = new double[nrsc];
      HmdpPpddlLoader::fillUpBounds (cond_low, cond_high, dm);
      HmdpPpddlLoader::convertRscSFToBounds (cgr.condition (), cond_low, cond_high, dm);
      low.push_back (cond_low); high.push_back (cond_high);
      std::vector<std::map<std::string, double>* > vecmap;
      values.push_back (vecmap);
      return HmdpPpddlLoader::convertGoalReward (cgr.reward (), nrsc, low, high, values);  /* TODO: beware linear case */
      delete[] cond_low; delete[] cond_high;
    }
  else if (grt == GR_CONJ)
    {
      PwRewardType resPrt = GR_PW_CST;  /* default */
      const ConjunctiveGoalReward &cjgr = static_cast<const ConjunctiveGoalReward&> (gr);
      if (low.size () < values.size ()) 	/* no condition */
	{
	  std::vector<std::map<std::string, double>* > vecmap;
	  values.push_back (vecmap);
	  double *cond_low = new double[nrsc];
	  double *cond_high = new double[nrsc];
	  HmdpPpddlLoader::fillUpBounds (cond_low, cond_high, dm);
	  low.push_back (cond_low); high.push_back (cond_high);
      }
      for (size_t i=0; i<cjgr.size (); i++)
	{
	  PwRewardType prt 
	    = HmdpPpddlLoader::convertGoalReward (cjgr.conjunct (i), nrsc, low, high, values);
	  if (prt != GR_PW_CST)
	    resPrt = prt;  /* constant unless linear */
	}
      return resPrt;
    }
  else
    {
      std::cerr << "[Error]::HmdpPpddlLoader::convertGoal: Unknown goal reward type: " 
		<< grt << ".Exiting.\n";
      exit (-1);
    }
  return GR_PW_CST; /* we should not get there. */
}

const Problem* HmdpPpddlLoader::getFirstProblem ()
{
  if (HmdpPpddlLoader::m_firstProblem)
    return HmdpPpddlLoader::m_firstProblem;
  
  for (Problem::ProblemMap::const_iterator pi = Problem::begin ();
       pi != Problem::end (); pi++)
    {
      if (pi != Problem::begin ())
	{
	  std::cout << "[Warning]:HmdpPpddlLoader: consider the first problem only !\n";
	  break;
	}
      HmdpPpddlLoader::m_firstProblem = (*pi).second;
      return (*pi).second;
    }
  
  std::cout << "[Error]:HmdpPpddlLoader: no ppddl problem found! Exiting.\n";
  exit (-1);
}

const Problem* HmdpPpddlLoader::getProblem(const int &i)
{
  int p = 0;
  for (Problem::ProblemMap::const_iterator pi = Problem::begin ();
       pi != Problem::end (); pi++)
    {
      if (p == i)
	return (*pi).second;
      p++;
    }
  std::cout << "[Error]:HmdpPpddlLoader: no ppddl problem found! Exiting.\n";
  exit (-1);
}

size_t HmdpPpddlLoader::getProblemSize()
{
  return Problem::size();
}

size_t HmdpPpddlLoader::getDomainSize()
{
  return Domain::size();
}

void HmdpPpddlLoader::createInitialState(ValueMap &values, AtomSet &atoms)
{
  HmdpPpddlLoader::createInitialState(values, atoms, 0);
}

void HmdpPpddlLoader::createInitialState (ValueMap &values, AtomSet &atoms,
					  const int &pb)
{
  const Problem *problem = HmdpPpddlLoader::getProblem(pb);
  
  /* non-static predicates */
  for (AtomSet::const_iterator ai = problem->init_atoms().begin();
       ai != problem->init_atoms().end(); ai++) {
    if (! problem->domain().predicates().static_predicate((*ai)->predicate())) {
      atoms.insert(*ai);
    }
  }
  
  /* non-static functions / set MAX value to resources. */
  for (ValueMap::const_iterator vi = problem->init_values().begin();
       vi != problem->init_values().end(); vi++) {
    Function function = (*vi).first->function();
    //if (! problem->domain().functions().static_function(function)) {
      values.insert(*vi);
      //}
    
    /* resources are set to their MAX value. */
    if (problem->domain().functions().isResource(function))
      { 
	std::unordered_map<const Application*, Rational>::iterator vmi;
	for (vmi = values.begin (); vmi != values.end (); vmi++)
	  if ((*vmi).first->function () == function)
	    {
	      std::string rscname 
		= HmdpPpddlLoader::getProblem(pb)->domain().functions().name (function);
	      //(*vmi).second = Rational (lround (HmdpPpddlLoader::getResource (rscname).second.second), 1);
	      //BEWARE: same domain as problem number...
	      (*vmi).second = Rational (HmdpPpddlLoader::getResource (rscname,pb).second.second);
	      break;
	    }
      }
  }
}

MDDiscreteDistribution* HmdpPpddlLoader::buildInitialRscDistribution (const ProbabilityDistMap &pdm)
{
  return HmdpPpddlLoader::buildInitialRscDistribution(pdm,0);
}

MDDiscreteDistribution* HmdpPpddlLoader::buildInitialRscDistribution (const ProbabilityDistMap &pdm, const int &dm)
{
  std::vector<std::pair<std::string, std::pair<double, double> > > *rscs
    = HmdpPpddlLoader::getResources(dm);
  DiscreteDistribution **ndds = new DiscreteDistribution*[rscs->size ()];
  
  for (ProbabilityDistMap::const_iterator pi = pdm.begin ();
       pi != pdm.end (); pi++)
    {
      int pos = HmdpPpddlLoader::getRscPosition ((*pi).first, dm);
      const ProbabilityDistributionFunc *pdfunc = (*pi).second;
      double mean = 0.0; double variance = -1.0;
      discreteDistributionType dt;
      
      const Expression *meanExpr = pdfunc->getMean ();
      const Expression *stdDevExpr = pdfunc->getStdDev ();

      if (meanExpr->getType () == EXPR_VAL
	  && stdDevExpr->getType () == EXPR_VAL)  /* non instantiated */
	{
	  const Value *meanval = static_cast<const Value*> (meanExpr);
	  const Value *stdval = static_cast<const Value*> (stdDevExpr);
	  mean = meanval->value ().double_value ();
	  variance = stdval->value ().double_value ();
	  if (pdfunc->getType () == DISTRIBUTION_NORMAL) dt = GAUSSIAN;
	  else if (pdfunc->getType () == DISTRIBUTION_UNIFORM) dt = UNIFORM;
	}
      else {
	std::cout << "[Error]:HmdpPpddlLoader::buildInitialRscDistribution: probability distribution parameters must be real numbers. No call to functions. Exiting.\n";
	exit (-1);
      }

      if (dt == GAUSSIAN)
	{
	  const Expression *expr_interval = pdfunc->getInterval ();
	  const Expression *expr_threshold = pdfunc->getThreshold ();
	  const Expression *expr_points = pdfunc->getPoints ();

	  double val1 = 0.0; double val2 = 0.0;
	  const Value *v1 = NULL; const Value *v2 = NULL;

	  discretizationType dzt = HmdpPpddlLoader::m_defaultDiscretizationType;
	  
	  if (pdfunc->getDisczType () == DISCRETIZATION_INTERVAL
	      || dzt == DISCRETIZATION_WRT_INTERVAL)
	    {
	      dzt = DISCRETIZATION_WRT_INTERVAL;
	      if (expr_interval->getType () == EXPR_VAL
		  && expr_threshold->getType () == EXPR_VAL)
		{
		  v1 = static_cast<const Value*> (expr_threshold);
		  v2 = static_cast<const Value*> (expr_interval);
		}
	      else 
		{
		  std::cout << "[Error]:HmdpPpddlLoader::buildInitialRscDistribution: probability distribution parameters must be numbers. No call to functions. Exiting.\n";
		  exit (-1);
		}
	    }
	  else if (pdfunc->getDisczType () == DISCRETIZATION_THRESHOLD
		   || dzt == DISCRETIZATION_WRT_THRESHOLD)
	    {
	      dzt = DISCRETIZATION_WRT_THRESHOLD;
	      if (expr_interval->getType () == EXPR_VAL
		  && expr_threshold->getType () == EXPR_VAL)
		{
		  v1 = static_cast<const Value*> (expr_threshold);
		  v2 = static_cast<const Value*> (expr_interval);
		}
	      else 
		{
		  std::cout << "[Error]:HmdpPpddlLoader::buildInitialRscDistribution: probability distribution parameters must be numbers. No call to functions. Exiting.\n";
		  exit (-1);
		}
	    }
	  else if (pdfunc->getDisczType () == DISCRETIZATION_POINTS
		   || dzt == DISCRETIZATION_WRT_POINTS)
	    {
	      dzt = DISCRETIZATION_WRT_POINTS;
	      if (expr_threshold->getType () == EXPR_VAL
		  && expr_points->getType () == EXPR_VAL)
		{
		  v1 = static_cast<const Value*> (expr_threshold);
		  v2 = static_cast<const Value*> (expr_points);
		}
	      else 
		{
		  std::cout << "[Error]:HmdpPpddlLoader::buildInitialRscDistribution: probability distribution parameters must be numbers. No call to functions. Exiting.\n";
		  exit (-1);
		}
	    }
	  val1 = v1->value ().double_value ();
	  val2 = v2->value ().double_value ();
	  NormalDiscreteDistribution *nd 
	    = new NormalDiscreteDistribution (mean, variance, val1, val2, dzt);
	  ndds[pos] = nd;

#ifdef LOADER_VERBOSE	  
	  std::cout << "[Debug]: nd:\n" << *nd << std::endl;
#endif	
	}
      else if (dt == UNIFORM)
	{
	  std::cout << "[Error]:HmdpPpddlLoader::buildInitialRscDistribution: uniform initial distribution not yet implemented. Exiting.\n";
	  exit (-1);
	}
    }
  
  MDDiscreteDistribution *jdist 
    = MDDiscreteDistribution::jointDiscreteDistribution (rscs->size (), ndds);
  for (size_t i=0; i<rscs->size(); i++)
    delete ndds[i];
  delete []ndds;
  return jdist;
}

MDDiscreteDistribution* HmdpPpddlLoader::buildInitialRscDistribution()
{
  return HmdpPpddlLoader::buildInitialRscDistribution(0);
}

MDDiscreteDistribution* HmdpPpddlLoader::buildInitialRscDistribution (const int &pb)
{
  //BEWARE: same domain as problem number...
  std::vector<std::pair<std::string, std::pair<double, double> > > *rscs
    = HmdpPpddlLoader::getResources (pb);
  int i=0; double one = 1.0;
  DiscreteDistribution **ndds = new DiscreteDistribution*[rscs->size ()];
  std::vector<std::pair<std::string, std::pair<double, double> > >::iterator it;
  for (it = rscs->begin (); it != rscs->end (); it++)
    {
      int fct = HmdpPpddlLoader::getProblem(pb)->domain ().functions ().find_function ((*it).first).first;
      double rval = HmdpPpddlLoader::findFunctionValue (fct, pb);
      ndds[i] = new DiscreteDistribution (NONE, 1,
					 (*it).second.second - (*it).second.first, &rval, &one);
      i++;
    }

  MDDiscreteDistribution *jdist 
    = MDDiscreteDistribution::jointDiscreteDistribution (i, ndds);
  delete []ndds;
  return jdist;
}

void HmdpPpddlLoader::applyNonResourceEffectChanges (const Effect& ef, ValueMap &values, AtomSet &atoms, const int &probEfIndex)
{
  HmdpPpddlLoader::applyNonResourceEffectChanges(ef, values, atoms, probEfIndex, 0);
}

void HmdpPpddlLoader::applyNonResourceEffectChanges (const Effect& ef, ValueMap &values, AtomSet &atoms, const int &probEfIndex, const int &pb)
{
  if (ef.getType () != EF_PROB)
    {
      AtomList adds;
      AtomList deletes;
      AssignmentList assignments;
      
      /* get the changes */
      ef.state_change (adds, deletes, assignments, atoms, values);
      
      /* apply the changes */
      for (AtomList::const_iterator ai = deletes.begin ();
	   ai != deletes.end (); ai++)
	atoms.erase (*ai);
      atoms.insert(adds.begin (), adds.end ());
      for (AssignmentList::const_iterator ai = assignments.begin ();
	   ai != assignments.end (); ai++)
	{
	  Function function = (*ai)->application ().function (); 
	  if (!HmdpPpddlLoader::getProblem(pb)->domain().functions().isResource(function))
	    (*ai)->affect (values);
	  /*else
	    { */ /* resources are set to their MAX value. */
	      /*std::unordered_map<const Application*, Rational>::iterator vmi;
		for (vmi = values.begin (); vmi != values.end (); vmi++)
		if ((*vmi).first->function () == function)
		{
		std::string rscname 
		= HmdpPpddlLoader::getFirstProblem()->domain().functions().name (function);
		(*vmi).second
		= Rational (lround (HmdpPpddlLoader::getResource (rscname).second.second), 1);
		break;
		}
		} */
	}
    }
  else
    {
      const ProbabilisticEffect &pef = static_cast<const ProbabilisticEffect&> (ef);
      HmdpPpddlLoader::applyNonResourceEffectChanges (pef.effect (probEfIndex),
						      values, atoms, probEfIndex, pb);
    }
}

/* printing */
void HmdpPpddlLoader::print (std::ostream &out, const ValueMap &values, const AtomSet &atoms)
{
  HmdpPpddlLoader::print(out, values, atoms, 0);
}

void HmdpPpddlLoader::print (std::ostream &out, const ValueMap &values, const AtomSet &atoms, const int &pb)
{
  bool first = true;
  const Problem *problem = HmdpPpddlLoader::getProblem(pb);
  for (AtomSet::const_iterator ai = atoms.begin (); ai != atoms.end (); ai++)
    {
      if (first)
	first = false;
      else out << ' ';
      (*ai)->print (out, problem->domain ().predicates (), problem->domain ().functions (), 
		    problem->terms ());
    }
  for (ValueMap::const_iterator vi = values.begin (); 
       vi != values.end (); vi++)
    {
      if (! first)
	first = true;
      else out << ' ';
      out << "(= ";
      (*vi).first->print (out, problem->domain ().functions (), problem->terms ());
      out << ' ' << (*vi).second << ")";
    }
}

void HmdpPpddlLoader::printAtom (std::string &str, const Atom &tom)
{
  HmdpPpddlLoader::printAtom(str, tom, 0);
}

void HmdpPpddlLoader::printAtom (std::string &str, const Atom &tom,
				 const int &pb)
{
  const Problem *problem = HmdpPpddlLoader::getProblem (pb);
  tom.print (str, problem->domain ().predicates (), problem->domain ().functions (),
	     problem->terms ());
}

const Action& HmdpPpddlLoader::getAction(const size_t &id)
{
  return HmdpPpddlLoader::getAction(id,0);
}

const Action& HmdpPpddlLoader::getAction (const size_t &id, const int &pb)
{
  const Problem *problem = HmdpPpddlLoader::getProblem(pb);
  const ActionList &al = problem->actions ();
  for (ActionList::const_iterator ai = al.begin (); ai != al.end (); ai++)
    {
      if ((*ai)->id () == id)
	return *(*ai);
    }
  std::cout << "[Error]:HmdpPpddlLoader::getAction: " << id 
	    << " not found in the current problem.Exiting.\n";
  exit (-1);
}

const Goal& HmdpPpddlLoader::getGoal(const std::string &name)
{
  return HmdpPpddlLoader::getGoal(name, 0);
}

const Goal& HmdpPpddlLoader::getGoal (const std::string &name,
				      const int &pb)
{
  const Problem *problem = HmdpPpddlLoader::getProblem(pb);
  const GoalMap &gm = problem->getGoals ();
  GoalMap::const_iterator gi;
  if ((gi = gm.find (name)) != gm.end ())
    return *(*gi).second;
  else
    {
      std::cout << "[Error]:HmdpPpddlLoader::getGoal: " << name 
		<< " not found in the current problem.Exiting.\n";
      exit (-1);
    }
}

const std::string& HmdpPpddlLoader::getGoalName(const int &id)
{
  return HmdpPpddlLoader::getGoalName(id, 0);
}

const std::string& HmdpPpddlLoader::getGoalName (const int &id,
						 const int &pb)
{
  const Problem *problem = HmdpPpddlLoader::getProblem(pb);
  const GoalMap &gm = problem->getGoals ();
  GoalMap::const_iterator gi;
  for (gi = gm.begin (); gi != gm.end (); gi++)
    {
      if ((*gi).second->getId () == id)
	return (*gi).first;
    }
  std::cout << "[Error]:HmdpPpddlLoader::getGoalName: " << id 
	    << " not found in the current problem.Exiting.\n";
  exit (-1);
}

} /* end of namespace */
