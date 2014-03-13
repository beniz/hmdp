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

#ifndef HMDPPPDDLLOADER_H
#define HMDPPPDDLLOADER_H

#include "HmdpState.h"
#include "HybridTransition.h"
#include "problems.h"  /* from ppddl_parser */
#include <string>
#include <map>

using namespace std;
using namespace hmdp_base;
using namespace hmdp_engine;
using namespace ppddl_parser;

namespace hmdp_loader
{

class HmdpPpddlLoader
{
 public:
  /**
   * \brief load a .ppddl file.
   * @param filename ppddl filename (e.g. plan.ppddl).
   * @return loading status.
   */
  static bool load_file (const char *filename);

  static std::vector<std::pair<std::string, std::pair<double, double> > >* getResources ();

  static std::vector<std::pair<std::string, std::pair<double, double> > >* getResources (const int &dm);

  static HybridTransition* convertAction (const Action &act, const size_t &nrsc);

  static HybridTransition* convertAction (const Action &act, const size_t &nrsc,
					  const int &dm);

  static ContinuousReward* convertGoal (const Goal &goal, const size_t &nrsc);
  
  static void fillUpBounds (double *low, double *high, const int &dm);

  static void fillUpBounds (double *low, double *high);
  
  static const Domain* getDomain(const int &dm);

  static const Problem* getFirstProblem ();

  static const Problem* getProblem(const int &i);

  static size_t getProblemSize();

  static size_t getDomainSize();

  static void createInitialState (ValueMap &values, AtomSet &atoms);

  static void createInitialState (ValueMap &values, AtomSet &atoms,
				  const int &pb);

  static MDDiscreteDistribution* buildInitialRscDistribution (const ProbabilityDistMap &pdm);

  static MDDiscreteDistribution* buildInitialRscDistribution (const ProbabilityDistMap &pdm, const int &dm);

  static MDDiscreteDistribution* buildInitialRscDistribution ();

  static MDDiscreteDistribution* buildInitialRscDistribution(const int &pb);

  /* Important: replace resources with their MAX value so action precondition
     tests are independent on the level of resources. */
  static void applyNonResourceEffectChanges (const Effect &ef, 
					     ValueMap &values, AtomSet &atoms, 
					     const int &probEfIndex);

  static void applyNonResourceEffectChanges (const Effect &ef, 
					     ValueMap &values, AtomSet &atoms, 
					     const int &probEfIndex,
					     const int &pb);

  static const Action& getAction (const size_t &id);

  static const Action& getAction (const size_t &id, const int &pb);

  static const Goal& getGoal (const std::string &name);

  static const Goal& getGoal (const std::string &name, const int &pb);

  static const std::string& getGoalName(const int &id);
  
  static const std::string& getGoalName(const int &id, const int &pb);

   static std::pair<std::string, std::pair<double, double> >& getResource (const std::string &rsc);

   static std::pair<std::string, std::pair<double, double> >& getResource (const std::string &rsc, const int &dm);

   static std::pair<std::string, std::pair<double, double> >& getResource (const int &pos);

   static std::pair<std::string, std::pair<double, double> >& getResource (const int &pos, const int &dm);

  /* printing */
  static void print (std::ostream &out, const ValueMap &values, const AtomSet &atoms);

  static void print (std::ostream &out, const ValueMap &values, const AtomSet &atoms,
		     const int &pb);

  static void printAtom (std::string &str, const Atom &tom);

  static void printAtom (std::string &str, const Atom &tom, const int &pb);

 private:
  static bool parse_file (const char *filename);
  
  static void convertActionEffects (const Action &act, const size_t &nrsc,
				    double *prob,
				    ContinuousTransition **ctrans, 
				    ContinuousReward **crew,
				    double *prec_low, double *prec_high);

  static void convertActionEffects (const Action &act, const size_t &nrsc,
				    double *prob,
				    ContinuousTransition **ctrans, 
				    ContinuousReward **crew,
				    double *prec_low, double *prec_high,
				    const int &dm);

  static size_t getNActionEffects (const Action &act);

  static int convertRscSFToBounds (const StateFormula &stf, 
				   double *low, double *high,
				   const int &dm);

  static int convertRscSFToBounds (const StateFormula &stf, 
				   double *low, double *high);

  static int convertRscExprToBounds (const Comparison &comp,
				     double *low, double *high);  

  static int convertRscExprToBounds (const Comparison &comp,
				     double *low, double *high,
				     const int &dm);

  static double findFunctionValue (const Function &fct);

  static double findFunctionValue (const Function &fct, const int &pb);

  static ContinuousTransition* convertContinuousActionEffect (const Effect &eff,
							      const size_t &nrsc,
							      std::vector<double*> &low, 
							      std::vector<double*> &high,
							      double *prec_low, double *prec_high);

  static ContinuousTransition* convertContinuousActionEffect (const Effect &eff,
							      const size_t &nrsc,
							      std::vector<double*> &low, 
							      std::vector<double*> &high,
							      double *prec_low, double *prec_high, const int &dm);

  static int convertContinuousEffect (const Effect &eff, const size_t &nrsc,
				      std::vector<double*> &low, std::vector<double*> &high,
				      std::vector<double*> &means,
				      std::vector<double*> &sds,
				      std::vector<discreteDistributionType*> &distribs,
				      std::vector<discretizationType*> &disczs,
				      std::vector<double*> &discz_arg1,
				      std::vector<double*> &discz_arg2);

  static int convertContinuousEffect (const Effect &eff, const size_t &nrsc,
				      std::vector<double*> &low, std::vector<double*> &high,
				      std::vector<double*> &means,
				      std::vector<double*> &sds,
				      std::vector<discreteDistributionType*> &distribs,
				      std::vector<discretizationType*> &disczs,
				      std::vector<double*> &discz_arg1,
				      std::vector<double*> &discz_arg2,
				      const int &dm);

  static void convertRscPdf (const AssignmentEffect &assignEff,
			     std::string &rsc, 
			     discreteDistributionType &dt,
			     double &mean, double &variance, discretizationType &dzt,
			     double &disczarg1, double &disczarg2, bool &found);

  static void convertRscPdf (const AssignmentEffect &assignEff,
			     std::string &rsc, 
			     discreteDistributionType &dt,
			     double &mean, double &variance, discretizationType &dzt,
			     double &disczarg1, double &disczarg2, bool &found,
			     const int &dm);

  static int getRscPosition (const std::string &rsc);

  static int getRscPosition (const std::string &rsc, const int &dm);
  
 public:
  static void setFunctionValueInMap (const int &pos, ValueMap &values, const double &val);
  
  static void setFunctionValueInMap (const int &pos, ValueMap &values, const double &val,
				     const int &pb);

  static double getFunctionValueInMap (const int &pos, const ValueMap &values);

  static double getFunctionValueInMap (const int &pos, const ValueMap &values,
				       const int &pb);

 private:
  static PwRewardType convertGoalReward (const GoalLinearReward &gr, const size_t &nrsc,
					 std::vector<double*> &low, std::vector<double*> &high, 
					 std::vector<std::vector<std::map<std::string, double>* > > &values);

  static PwRewardType convertGoalReward (const GoalLinearReward &gr, const size_t &nrsc,
					 std::vector<double*> &low, std::vector<double*> &high, 
					 std::vector<std::vector<std::map<std::string, double>* > > &values, const int &dm);

  static void checkCTTiles (const size_t &nrsc, double *prec_low, double *prec_high,
			    std::vector<double*> &low, std::vector<double*> &high,
			    std::vector<double*> &means, std::vector<double*> &sds,
			    std::vector<discreteDistributionType*> &distribs,
			    std::vector<discretizationType*> &disczs);

  //TODO:
  static void clearLoader();

 private:
  static bool m_defaultDiscretizationRelative;  /**< default 'relative' flag for action continuous effects */
  static discretizationType m_defaultDiscretizationType;
  static double m_defaultDiscretizationInterval;
  static double m_defaultDiscretizationEpsilon;
  static const Problem* m_firstProblem;  /**< first problem. */
};

} /* end of namespace */

#endif
