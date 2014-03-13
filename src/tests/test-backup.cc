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

#include "BackupOperations.h"
#include "ContinuousTransition.h"
#include "PiecewiseConstantReward.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace hmdp_base;

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

void plot2DVF (ValueFunction *vf,
	       double *low, double *high)
{
  if (vf->isLeaf ())
  {
    cout << "leaf: [" << low[0] << "," << high[0] << "], ["
	 << low[1] << "," << high[1] << "]\n";
    if (vf->getAlphaVectors ())
      {
	cout << "alpha vectors size: " << vf->getAlphaVectorsSize () << endl;
	cout << *vf->getAlphaVectorNth (0) << endl;
      }
  }
  else
    {
      double b=low[vf->getDimension ()];
      low[vf->getDimension ()] = vf->getPosition ();
      ValueFunction *cgt = dynamic_cast<ValueFunction*> (vf->getGreaterTree ());
      plot2DVF (cgt, low, high);
      low[vf->getDimension()] = b;

      b = high[vf->getDimension()];
      high[vf->getDimension ()] = vf->getPosition ();
      ValueFunction *clt = dynamic_cast<ValueFunction*> (vf->getLowerTree ());
      plot2DVF (clt, low, high);
      high[vf->getDimension ()] = b;
    }
}

int main ()
{
  /* domain */
  double low[2]={0.0,0.0}, high[2]={1.0,1.0};

  //BspTreeOperations::m_bspBalance = true;

  /* ----------------------------------------------------------------------------------- */
  std::cout << "testing one-step pwc backup on continuous transition...\n";
  double lowCornersTrans[4][2] = {{0.0,0.0},{0.1,0.0},{0.1,0.1},{0.0,0.1}};
  double highCornersTrans[4][2] = {{0.1,0.1},{1.0,0.1},{1.0,1.0},{0.1,1.0}};
  double epsilon[4][2] = {{0.001,0.001},{0.001,0.001},{0.001,0.001},{0.001,0.001}};
  double intervals[4][2] = {{1,1},{1,1},{1,1},{1,1}};
  double means[4][2] = {{-0.03,-0.01},{-0.07,-0.03},{-0.05,-0.05}, {-0.04,-0.04}};
  double sds[4][2] = {{0.01,0.01},{0.01,0.01},{0.01,0.01},{0.01,0.01}};
  bool relative[4][2] = {{true,true},{true,true},{true,true},{true,true}};
  discreteDistributionType distrib[4][2] = {{GAUSSIAN,GAUSSIAN},
					    {GAUSSIAN,GAUSSIAN},
					    {GAUSSIAN,GAUSSIAN},
					    {GAUSSIAN,GAUSSIAN}};

  double **lowCornersTransP = createPtrFrom2DArray (lowCornersTrans);
  double **highCornersTransP = createPtrFrom2DArray (highCornersTrans);
  const double **epsilonP = (const double**)createPtrFrom2DArray (epsilon);
  const double **intervalsP = (const double**) createPtrFrom2DArray (intervals);
  const double **meansP = (const double**) createPtrFrom2DArray (means);
  const double **sdsP = (const double**) createPtrFrom2DArray (sds);
  
  bool **relativeP = (bool**) malloc (4 * sizeof (bool*));
  discreteDistributionType **distribP 
    = (discreteDistributionType**) malloc (4 * sizeof (discreteDistributionType*));
  for (int i=0; i<4; i++)
    {
      relativeP[i] = (bool*) malloc (2 * sizeof (bool));
      distribP[i] = (discreteDistributionType*) malloc (2 * sizeof (discreteDistributionType));
      for (int j=0; j<2; j++)
	{
	  relativeP[i][j] = relative[i][j];
	  distribP[i][j] = distrib[i][j];
	}
    }	  
  ContinuousTransition *ct 
    = new ContinuousTransition (4, 2,
				DISCRETIZATION_WRT_INTERVAL,
				lowCornersTransP, highCornersTransP,
				low, high,
				epsilonP, intervalsP,
				meansP, sdsP, relativeP,
				(const discreteDistributionType **) distribP);

  PiecewiseConstantValueFunction *vf 
    = new PiecewiseConstantValueFunction (2, low, high, 5.0);

  double lowCornersCr[4][2] = {{0.0,0.8},{0.0,0.0},{0.4,0.8},{0.4,0.0}};
  double highCornersCr[4][2] = {{0.4,1.0},{0.4,0.8},{1.0,1.0},{1.0,0.8}};
  double valuesCr[4] = {0.2, 0.4, 0.6, 0.8};

  double **lowCornersCrP = createPtrFrom2DArray (lowCornersCr);
  double **highCornersCrP = createPtrFrom2DArray (highCornersCr);

  PiecewiseConstantReward *cr = new PiecewiseConstantReward (4, 2, lowCornersCrP, 
							     highCornersCrP,
							     low, high, valuesCr);
  
  BspTreeOperations::setPiecesMerging (true);
  ValueFunction *bvf_merged = BackupOperations::backUp (vf, ct, cr, 0, low, high);
  
  ofstream output_backup_pwc_ct_merged ("test_backup_pwc_ct_merged.box", ios::out);
  bvf_merged->plot2Dbox (0.0, output_backup_pwc_ct_merged, low, high);
  
  /* vrml output necessitates max vf's value. */
  /*double max_vf_value = 0.0; std::vector<int> gls;
  bvf_merged->maxValueAndGoals (gls, max_vf_value, low, high);
  ofstream output_backup_pwc_merged_vrml ("test_backup_pwc_merged.vrml", ios::out);
  bvf_merged->plot2DVrml2 (0.0, output_backup_pwc_merged_vrml, low, high, max_vf_value);*/
  //plot2DVF (bvf, low, high);

  std::cout << "bvf_merged:\n"; 
  bvf_merged->print (std::cout, low, high);
}
