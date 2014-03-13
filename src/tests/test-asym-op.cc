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

int main ()
{
  /* domain */
  double low[2]={0.0,0.0}, high[2]={1.0,1.0};

  //BspTreeOperations::m_asy...

  std::cout << "test asymetric max operator.\n";
  /* first reward. */
  double lowCornersCr1[4][2] = {{0.0,0.8},{0.0,0.0},{0.4,0.8},{0.4,0.0}};
  double highCornersCr1[4][2] = {{0.4,1.0},{0.4,0.8},{1.0,1.0},{1.0,0.8}};
  double valuesCr1[4] = {0.2, 0.4, 0.6, 0.8};

  double **lowCornersCr1P = createPtrFrom2DArray (lowCornersCr1);
  double **highCornersCr1P = createPtrFrom2DArray (highCornersCr1);

  PiecewiseConstantReward *cr1 = new PiecewiseConstantReward (4, 2, lowCornersCr1P, 
							     highCornersCr1P,
							     low, high, valuesCr1);
  cr1->updateSubTreeMaxValue ();
  
  std::cout << "cr1:\n";
  cr1->print (std::cout, low, high);

  /* second reward. */
  double lowCornersCr2[1][2] = {{0.0,0.0}};
  double highCornersCr2[1][2] = {{1.0,1.0}};
  double valuesCr2[1] = {1.0};

  double **lowCornersCr2P = createPtrFrom2DArray (lowCornersCr2);
  double **highCornersCr2P = createPtrFrom2DArray (highCornersCr2);

  PiecewiseConstantReward *cr2 = new PiecewiseConstantReward (1, 2, lowCornersCr2P, 
							     highCornersCr2P,
							     low, high, valuesCr2);
  cr2->updateSubTreeMaxValue ();

  /* third reward. */
  double lowCornersCr3[2][2] = {{0.0,0.0}, {0.5, 0.0}};
  double highCornersCr3[2][2] = {{0.5, 1.0}, {1.0,1.0}};
  double valuesCr3[2] = {0.1, 1.0};

  double **lowCornersCr3P = createPtrFrom2DArray (lowCornersCr3);
  double **highCornersCr3P = createPtrFrom2DArray (highCornersCr3);

  PiecewiseConstantReward *cr3 = new PiecewiseConstantReward (2, 2, lowCornersCr3P, 
							     highCornersCr3P,
							     low, high, valuesCr3);
  cr3->updateSubTreeMaxValue ();
  std::cout << "cr3:\n";
  cr3->print (std::cout, low, high);


  std::cout << "\nmax:\n";
  BspTreeOperations::m_asymetricOperators = true;
  BspTreeOperations::setIntersectionType (BTI_MAX);
  BspTree *max_rew = BspTreeOperations::intersectTrees (cr3, cr1, low, high);
  PiecewiseConstantReward *max_cr = static_cast<PiecewiseConstantReward*> (max_rew);

  max_cr->print (std::cout, low, high);
}
