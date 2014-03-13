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
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace hmdp_base;

double** createPtrFrom2DArray (double ar[][2])
{
  double **res = (double**) malloc (3 * sizeof (double*));
  for (int i=0; i<3; i++)
    {
      res[i] = (double*) malloc (2 * sizeof (double));
      for (int j=0; j<2; j++)
	res[i][j] = ar[i][j];
    }
  return res;
}

void plot2DCTJointDistrib (ContinuousTransition *ct,
			   double *low, double *high)
{
  if (ct->isLeaf ())
  {
      MDDiscreteDistribution *jdd = 0;
      cout << "leaf: [" << low[0] << "," << high[0] << "], ["
	   << low[1] << "," << high[1] << "]\n";
      //cout << "jdd: " << ct->getLeafDistribution () << endl;
      cout << "tile: " << ct->getNTile () << endl;
      if ((jdd = ct->getLeafDistribution ()))
	 {
	   cout << "jdd npoints: " << jdd->getNPoints () << " -- dim: " << jdd->getDimension () << endl;
	   //cout << *jdd << endl;
	 }
  }
  else
    {
      cout << "dim: " << ct->getDimension () << " -- pos: " << ct->getPosition () << endl;
      double b=low[ct->getDimension ()];
      low[ct->getDimension ()] = ct->getPosition ();
      ContinuousTransition *cgt = dynamic_cast<ContinuousTransition*> (ct->getGreaterTree ());
      plot2DCTJointDistrib (cgt, low, high);
      low[ct->getDimension()] = b;

      b = high[ct->getDimension()];
      high[ct->getDimension ()] = ct->getPosition ();
      ContinuousTransition *clt = dynamic_cast<ContinuousTransition*> (ct->getLowerTree ());
      plot2DCTJointDistrib (clt, low, high);
      high[ct->getDimension ()] = b;
    }
}

int main ()
{
  /* transition tiles 
     todo: draw tiles
  */

  /* domain */
  double low[2] = {0.0,0.0}, high[2] = {1.0,1.0};

  ofstream output ("testCT.box", ios::out);

  double lowCorners[3][2] = {{0.0,0.0},{0.1,0.0},{0.1,0.1}};
  double highCorners[3][2] = {{0.1,0.1},{1.0,0.1},{1.0,1.0}};
  double epsilon[3][2] = {{0.001,0.001},{0.001,0.001},{0.001,0.001}};
  double intervals[3][2] = {{2.0,2.0},{6.0,6.0},{10.0,10.0}};
  double means[3][2] = {{0.0,0.0},{0.0,0.0},{0.0,0.0}};
  double sds[3][2] = {{0.1,0.1},{0.1,0.1},{0.1,0.1}};
  bool relative[3][2] = {{true,true},{true,true},{true,true}};
  discreteDistributionType distrib[3][2] = {{GAUSSIAN,GAUSSIAN},
					    {GAUSSIAN,GAUSSIAN},
					    {GAUSSIAN,GAUSSIAN}};

  double **lowCornersP = createPtrFrom2DArray (lowCorners);
  double **highCornersP = createPtrFrom2DArray (highCorners);
  const double **epsilonP = (const double**)createPtrFrom2DArray (epsilon);
  const double **intervalsP = (const double**) createPtrFrom2DArray (intervals);
  const double **meansP = (const double**) createPtrFrom2DArray (means);
  const double **sdsP = (const double**) createPtrFrom2DArray (sds);
  
  bool **relativeP = (bool**) malloc (3 * sizeof (bool*));
  discreteDistributionType **distribP 
    = (discreteDistributionType**) malloc (3 * sizeof (discreteDistributionType*));
  for (int i=0; i<3; i++)
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
    = new ContinuousTransition (3, 2,
				DISCRETIZATION_WRT_INTERVAL,
				lowCornersP, highCornersP,
				low, high,
				epsilonP, intervalsP,
				meansP, sdsP, relativeP,
				(const discreteDistributionType **) distribP);
  ct->plot2Dbox (0.0, output, low, high);

  /* verify discretize joint distribution in leaves */
  plot2DCTJointDistrib (ct, low, high);

  /* reference tiles */
  std::cout << "testing reference tiles...\n";
  for (int i=0; i<3; i++)
    if (ct->getPtrTile(i))
      std::cout << "tile: " << i << ": " 
		<< ct->getPtrTile (i)->getNContinuousOutcomes () 
		<< std::endl;

  std::cout << "ct:\n";
  ct->print (std::cout, low, high);

  BspTree::deleteBspTree (ct);
}
