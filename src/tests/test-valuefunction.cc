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

#include "PiecewiseConstantValueFunction.h"
#include "PiecewiseLinearValueFunction.h"
#include "PiecewiseConstantReward.h"
#include "ContinuousTransition.h"
#include "BackupOperations.h"
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
  
  /* tiles */
  double low1[2] = {0.0,0.0}, high1[2] = {1.0,1.0};
  //double low2[2] = {0.3,0.2}, high2[2] = {0.9,0.5};
  double val1 = 5.0, val2 = 7.0;

  /* ------------------------------------------------------------------------------------- */
  std::cout << "testing intersection pwc value function / pwc reward...\n";
  ofstream output_pwc_cr ("testVF_pwc_cr.box", ios::out);
  PiecewiseConstantValueFunction *pcvf1 = new PiecewiseConstantValueFunction (2, low1, high1, val1);
  //PiecewiseConstantValueFunction *pcvf2 = new PiecewiseConstantValueFunction (2, low2, high2, val2);
  double lowCorners[4][2] = {{0.0,0.8},{0.0,0.0},{0.4,0.8},{0.4,0.0}};
  double highCorners[4][2] = {{0.4,1.0},{0.4,0.8},{1.0,1.0},{1.0,0.8}};
  double values[4] = {3.0, 5.0, 6.0, 10.0};

  double **lowCornersP = createPtrFrom2DArray (lowCorners);
  double **highCornersP = createPtrFrom2DArray (highCorners);

  PiecewiseConstantReward *pcr = new PiecewiseConstantReward (4, 2, lowCornersP, highCornersP,
							      low, high, values);

  ValueFunction *pcvf3 
    = BackupOperations::intersectVFWithReward (pcvf1, pcr, low, high);
  
  pcvf3->plot2Dbox (0.0, output_pwc_cr, low, high);
  output_pwc_cr.close ();
  plot2DVF (pcvf3, low, high);

  /* ------------------------------------------------------------------------------------- */
  std::cout << "testing intersection pwc value function / continuous outcome...\n";
  ofstream output_ct ("testVF_ct.box", ios::out);
  ofstream output_pwc_co ("testVF_pwc_co.box", ios::out);
  ofstream output_pwc_vf ("testVF_pwc_vf.box", ios::out);
  ofstream output_pwc_vf_crop ("testVF_pwc_vf_crop.box", ios::out);
  ofstream output_pwc_vf_shift ("testVF_pwc_vf_shift.box", ios::out);
  double lowCornersTrans[4][2] = {{0.0,0.0},{0.1,0.0},{0.1,0.1},{0.0,0.1}};
  double highCornersTrans[4][2] = {{0.1,0.1},{1.0,0.1},{1.0,1.0},{0.1,1.0}};
  double epsilon[4][2] = {{0.001,0.001},{0.001,0.001},{0.001,0.001},{0.001,0.001}};
  double intervals[4][2] = {{0.01,0.01},{0.01,0.01},{0.01,0.01},{0.01,0.01}};
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
				DISCRETIZATION_WRT_THRESHOLD,
				lowCornersTransP, highCornersTransP,
				low, high,
				epsilonP, intervalsP,
				meansP, sdsP, relativeP,
				(const discreteDistributionType **) distribP);
  ct->plot2Dbox (0.0, output_ct, low, high);
  /* fetch a tile */
  ContinuousTransition *ct_tile = ct->getPtrTile (2);
  std::cout << "nTile: " << ct_tile->getNTile () << std::endl;
  std::cout << "number of continuous outcomes: " << ct_tile->getNContinuousOutcomes () << std::endl;

  ContinuousOutcome *co = ct_tile->getContinuousOutcome (20);
  co->plot2Dbox (0.0, output_pwc_co, low, high);
  output_pwc_co.close ();
  PiecewiseConstantValueFunction *pcvf2 
    = dynamic_cast<PiecewiseConstantValueFunction*> (BackupOperations::intersectCOWithVF (pcvf1, co, co->getLowPos (), co->getHighPos ()));
  pcvf2->plot2Dbox (0.0, output_pwc_vf, low, high);
  PiecewiseConstantValueFunction *testcrop
    = dynamic_cast<PiecewiseConstantValueFunction*> (BspTreeOperations::cropTree (pcvf2, co->getLowPos (), co->getHighPos ()));
  testcrop->plot2Dbox (0.0, output_pwc_vf_crop, low, high);
  testcrop = dynamic_cast<PiecewiseConstantValueFunction*> (BspTreeOperations::shiftTree (testcrop, co->getShiftBack (), low, high));
  testcrop->plot2Dbox (0.0, output_pwc_vf_shift, low, high);
  output_pwc_vf.close ();
  output_pwc_vf_shift.close ();

  BspTree::deleteBspTree (ct);
  BspTree::deleteBspTree (pcr); 
  BspTree::deleteBspTree (pcvf1); 
  BspTree::deleteBspTree (pcvf2); 
  BspTree::deleteBspTree (pcvf3);
}
