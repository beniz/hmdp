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

#include "ContinuousTransition.h"
#include "ContinuousStateDistribution.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace hmdp_base;

#define max_time 10800.0 // 15000.0    // original value was 10800.0
#define max_energy  500000.0 // 750000.0 // original value was 2052000.0

// Discretization
#define time_delta 300.0
#define energy_delta 15000.0
#define epsilon 0.01 // 0.02

// Navigate actions parameters (distance multipliers)
#define dist_mean_dur 420.0 // 500.0      // original value was 420.0
#define dist_mean_cons 17700.0 // 20000.0   // original value was 17700.0
#define dist_stddev_dur 410.0 // 80.0     // original value was 410.0
#define dist_stddev_cons 5000.0 // 2500.0  // original value was 5000.0
#define dist_min_time 420.0 // 500.0      // 420.0 
#define dist_min_energy 17700.0 // 20000.0  // 17700.0
  
// Other actions parameters (consumptions)
#define IP_mean_dur 504.0      // original value was 504.0 
#define IP_mean_cons 34611.0   // original value was 34611.0
#define IP_stddev_dur 120.0    // original value was 120.0
#define IP_stddev_cons 85.0    // original value was 85.0
#define IP_min_time 504.0      // 504.0
#define IP_min_energy 34611.0  // 34611.0

/*========== Array to pointer conversions ==========*/
double** createPtrFrom2DArray (double ar[][2])
{
  double **res = (double**) malloc (4 * sizeof (double*));
  for (int i=0; i<4; i++)
    {
      res[i] = (double*) malloc (2 * sizeof (double));
      for (int j=0; j<2; j++)
        res[i][j] = ar[i][j];
    }
  return res;
}

int main ()
{
  Alg::m_doubleEpsilon = 1e-20;

  /* DOMAIN */
  double space_lc[2] = {0.0, 0.0};
  double space_uc[2] = {max_time, max_energy};
  
  /* step for point value plotting. */
  double step[2] = {300, 15000};

  /* piece merging. */
  BspTreeOperations::m_piecesMerging = false;
  BspTreeOperations::m_piecesMergingByValue = false;

  /* TRANSITIONS */
  //double lowCornersTrans_allActions[1][2] = {{0.0, 0.0}};
  double upCornersTrans_allActions[1][2] = {{max_time, max_energy}};
  double epsilon_allActions[1][2] = {{epsilon, epsilon}};
  double delta_allActions[1][2] = {{time_delta, energy_delta}};
  bool relative_allActions[1][2] = {{true,true}};
  discreteDistributionType distrib_stochasticActions[1][2] = {{GAUSSIAN,GAUSSIAN}};
  discreteDistributionType distrib_deterministicActions[1][2] = {{NONE,NONE}};

  //double **lowCornersTransP_allActions = createPtrFrom2DArray (lowCornersTrans_allActions);
  double **upCornersTransP_allActions = createPtrFrom2DArray (upCornersTrans_allActions);
  const double **epsilonP_allActions = (const double**)createPtrFrom2DArray (epsilon_allActions);
  const double **deltaP_allActions = (const double**) createPtrFrom2DArray (delta_allActions);
  bool **relativeP_allActions = (bool**) malloc (sizeof (bool*));
  discreteDistributionType **distribP_stochasticActions 
    = (discreteDistributionType**) malloc (sizeof (discreteDistributionType*));
  discreteDistributionType **distribP_deterministicActions 
    = (discreteDistributionType**) malloc (sizeof (discreteDistributionType*));
  for (int i=0; i<1; i++)
    {
      relativeP_allActions[i] = (bool*) malloc (2 * sizeof (bool));
      distribP_stochasticActions[i] = (discreteDistributionType*) malloc (2 * sizeof (discreteDistributionType));
      distribP_deterministicActions[i] = (discreteDistributionType*) malloc (2 * sizeof (discreteDistributionType));
      for (int j=0; j<2; j++)
        {
          relativeP_allActions[i][j] = relative_allActions[i][j];
          distribP_stochasticActions[i][j] = distrib_stochasticActions[i][j];
          distribP_deterministicActions[i][j] = distrib_deterministicActions[i][j];
        }
    }     

  /* Navigate Start C4 (distance = 3.236) */
  double means_nav_Start_C4[1][2] = {{-time_delta*round(3.236*dist_mean_dur/time_delta),
                                      -energy_delta*round(3.236*dist_mean_cons/energy_delta)}};
  double sds_nav_Start_C4[1][2] = {{3.236*dist_stddev_dur, 3.236*dist_stddev_cons}};
  double lowCornersTrans_nav_Start_C4[1][2] = {{time_delta*round(3.236*dist_min_time/time_delta),
                                                energy_delta*round(3.236*dist_min_energy/energy_delta)}};

  const double **meansP_nav_Start_C4 = (const double**) createPtrFrom2DArray (means_nav_Start_C4);
  const double **sdsP_nav_Start_C4 = (const double**) createPtrFrom2DArray (sds_nav_Start_C4);
  double **lowCornersTransP_nav_Start_C4 = createPtrFrom2DArray (lowCornersTrans_nav_Start_C4);


  std::cout << lowCornersTrans_nav_Start_C4[0][0] << "\t" << lowCornersTrans_nav_Start_C4[0][1] << std::endl;

  ContinuousTransition *ct_nav_Start_C4
    = new ContinuousTransition (1, 2,
                                DISCRETIZATION_WRT_INTERVAL,
                                //lowCornersTransP_allActions,
                                lowCornersTransP_nav_Start_C4,
                                upCornersTransP_allActions,
                                space_lc, space_uc,
                                epsilonP_allActions,
                                deltaP_allActions,
                                meansP_nav_Start_C4,
                                sdsP_nav_Start_C4,
                                relativeP_allActions,
                                (const discreteDistributionType **) distribP_stochasticActions);

  
  /* Navigate C4 ObsPt5 (distance = 2.941) */
  double means_nav_C4_ObsPt5[1][2] = {{-time_delta*round(2.941*dist_mean_dur/time_delta),
                                       -energy_delta*round(2.941*dist_mean_cons/energy_delta)}};
  double sds_nav_C4_ObsPt5[1][2] = {{2.941*dist_stddev_dur, 2.941*dist_stddev_cons}};
  double lowCornersTrans_nav_C4_ObsPt5[1][2] = {{time_delta*round(2.941*dist_min_time/time_delta),
                                                 energy_delta*round(2.941*dist_min_energy/energy_delta)}};

  const double **meansP_nav_C4_ObsPt5 = (const double**) createPtrFrom2DArray (means_nav_C4_ObsPt5);
  const double **sdsP_nav_C4_ObsPt5 = (const double**) createPtrFrom2DArray (sds_nav_C4_ObsPt5);
  double **lowCornersTransP_nav_C4_ObsPt5 = createPtrFrom2DArray (lowCornersTrans_nav_C4_ObsPt5);

  ContinuousTransition *ct_nav_C4_ObsPt5
    = new ContinuousTransition (1, 2,
                                DISCRETIZATION_WRT_INTERVAL,
                                //lowCornersTransP_allActions,
                                lowCornersTransP_nav_C4_ObsPt5,
                                upCornersTransP_allActions,
                                space_lc, space_uc,
                                epsilonP_allActions,
                                deltaP_allActions,
                                meansP_nav_C4_ObsPt5,
                                sdsP_nav_C4_ObsPt5,
                                relativeP_allActions,
                                (const discreteDistributionType **) distribP_stochasticActions);
  
  /* Navigate ObsPt5 ObsPt2 (distance = 2.252) */
  double means_nav_ObsPt5_ObsPt2[1][2] = {{-time_delta*round(2.252*dist_mean_dur/time_delta),
                                           -energy_delta*round(2.252*dist_mean_cons/energy_delta)}};
  double sds_nav_ObsPt5_ObsPt2[1][2] = {{2.252*dist_stddev_dur, 2.252*dist_stddev_cons}};
  double lowCornersTrans_nav_ObsPt5_ObsPt2[1][2] = {{time_delta*round(2.252*dist_min_time/time_delta),
                                                     energy_delta*round(2.252*dist_min_energy/energy_delta) + 0.1}};

  const double **meansP_nav_ObsPt5_ObsPt2 = (const double**) createPtrFrom2DArray (means_nav_ObsPt5_ObsPt2);
  const double **sdsP_nav_ObsPt5_ObsPt2 = (const double**) createPtrFrom2DArray (sds_nav_ObsPt5_ObsPt2);
  double **lowCornersTransP_nav_ObsPt5_ObsPt2 = createPtrFrom2DArray (lowCornersTrans_nav_ObsPt5_ObsPt2);

  ContinuousTransition *ct_nav_ObsPt5_ObsPt2
    = new ContinuousTransition (1, 2,
                                DISCRETIZATION_WRT_INTERVAL,
                                //lowCornersTransP_allActions,
                                lowCornersTransP_nav_ObsPt5_ObsPt2,
                                upCornersTransP_allActions,
                                space_lc, space_uc,
                                epsilonP_allActions,
                                deltaP_allActions,
                                meansP_nav_ObsPt5_ObsPt2,
                                sdsP_nav_ObsPt5_ObsPt2,
                                relativeP_allActions,
                                (const discreteDistributionType **) distribP_stochasticActions);

  /* initial probability distribution over resources. */
  DiscreteDistribution::m_positiveResourcesConsumptionTruncation = false;
  NormalDiscreteDistribution *ndd1 = new NormalDiscreteDistribution (5400.0, 500.0,
								     epsilon, time_delta,
								     DISCRETIZATION_WRT_INTERVAL);
  NormalDiscreteDistribution *ndd2 = new NormalDiscreteDistribution (250000.0, 20000.0,
								     epsilon, energy_delta,
								     DISCRETIZATION_WRT_INTERVAL);
  DiscreteDistribution::m_positiveResourcesConsumptionTruncation = true;
  
  DiscreteDistribution** dists = new DiscreteDistribution*[2];
  dists[0] = ndd1; dists[1] = ndd2;
  MDDiscreteDistribution *initMdd
    = MDDiscreteDistribution::jointDiscreteDistribution (2, dists);
  
  std::cout << "ndd1: " << *ndd1 << std::endl;
  std::cout << "ndd2: " << *ndd2 << std::endl;
  
  delete ndd1; delete ndd2; delete []dists;

  std::cout << "initMdd:\n" << *initMdd << std::endl;

  ContinuousStateDistribution *initCsd
    = ContinuousStateDistribution::convertMDDiscreteDistribution (*initMdd, space_lc, space_uc);
  
  std::cout << "initCsd:\n";
  initCsd->print (std::cout, space_lc, space_uc);
  
  BspTree::m_plotPointFormat = GnuplotF;
  ofstream output_initcsd_gp ("test_frontup_initcsd.dat", ios::out);
  initCsd->plotNDPointValues (output_initcsd_gp, step, space_lc, space_uc);

  /* frontups. */
  
  ContinuousStateDistribution *convol
    = ContinuousStateDistribution::frontUp (initCsd, ct_nav_Start_C4, space_lc, space_uc, 1.0);
  
  /* std::cout << "convol:\n";
     convol->print (std::cout, space_lc, space_uc); */
  
  BspTree::m_plotPointFormat = GnuplotF;
  ofstream output_convolcsd_gp ("test_frontup_convolcsd.dat", ios::out);
  convol->plotNDPointValues (output_convolcsd_gp, step, space_lc, space_uc);

  ContinuousStateDistribution *convol2
    = ContinuousStateDistribution::frontUp (convol, ct_nav_C4_ObsPt5, space_lc, space_uc, 1.0);
  
  /* std::cout << "convol2:\n";
     convol2->print (std::cout, space_lc, space_uc); */
  
  BspTree::m_plotPointFormat = GnuplotF;
  ofstream output_convolcsd2_gp ("test_frontup_convolcsd2.dat", ios::out);
  convol2->plotNDPointValues (output_convolcsd2_gp, step, space_lc, space_uc);

  ContinuousStateDistribution *convol3
    = ContinuousStateDistribution::frontUp (convol2, ct_nav_ObsPt5_ObsPt2, space_lc, space_uc, 1.0);
  
  BspTree::m_plotPointFormat = GnuplotF;
  ofstream output_convolcsd3_gp ("test_frontup_convolcsd3.dat", ios::out);
  convol3->plotNDPointValues (output_convolcsd3_gp, step, space_lc, space_uc);

  /* std::cout << "convol3:\n";
     convol3->print (std::cout, space_lc, space_uc); */
}
