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

int main()
{
/* domain */
  double low[2]={0.0,0.0}, high[2]={1.0,1.0};
  
  /* tiles */
  //double low1[2] = {0.0,0.0}, high1[2] = {1.0,1.0};
  //double val1 = 5.0, val2 = 7.0;

  /* ------------------------------------------------------------------------------------- */
  std::cout << "testing intersection pwc value function / pwc reward...\n";
  //ofstream output_pwc_cr ("test_cross_dim.box", ios::out);
  double lowCorners[4][2] = {{0.0,0.8},{0.0,0.0},{0.4,0.8},{0.4,0.0}};
  double highCorners[4][2] = {{0.4,1.0},{0.4,0.8},{1.0,1.0},{1.0,0.8}};
  double values[4] = {3.0, 5.0, 6.0, 10.0};

  double **lowCornersP = createPtrFrom2DArray (lowCorners);
  double **highCornersP = createPtrFrom2DArray (highCorners);

  
  PiecewiseConstantReward *pcr = new PiecewiseConstantReward (4, 2, lowCornersP, highCornersP,
							      low, high, values);
  PiecewiseConstantValueFunction *pcvf1 = new PiecewiseConstantValueFunction(*pcr);
  BspTree::deleteBspTree(pcr);
  
  //debug
  std::cout << "pcvf1:\n";
  pcvf1->print(std::cout,low,high);
  std::cout << std::endl;
  //debug

  double lowCorners2[4][2] = {{0.0,0.0},{0.1,0.0},{0.1,0.1},{0.0,0.1}};
  double highCorners2[4][2] = {{0.1,0.1},{1.0,0.1},{1.0,1.0},{0.1,1.0}};
  double values2[4] = {2.0, 3.0, 4.0, 5.0};
  double **lowCornersP2 = createPtrFrom2DArray(lowCorners2);
  double **highCornersP2 = createPtrFrom2DArray(highCorners2);
  pcr = new PiecewiseConstantReward(4,2,lowCornersP2,highCornersP2,low,high,values2);
    PiecewiseConstantValueFunction *pcvf2 = new PiecewiseConstantValueFunction(*pcr);
  BspTree::deleteBspTree(pcr);

  //debug
  std::cout << "pcvf2:\n";
  pcvf2->print(std::cout,low,high);
  std::cout << std::endl;
  //debug


  std::cout << "Testing cross dimensional sum:\n";
  double low1[4] = {0.0,0.0,0.0,0.0}, high1[4] = {1.0,1.0,1.0,1.0};
  const int &fdim = 4;
  PiecewiseConstantValueFunction *fvf
    = new PiecewiseConstantValueFunction(fdim,low1,high1);

  std::cout << "Testing sum over spaces of different dimensions:\n";
  ValueFunction *res1
    = ValueFunctionOperations::sumValueFunctions(fvf,pcvf1,low1,high1);
  res1->fixSpaceDimension(4);

  //debug
  std::cout << "res1:\n";
  res1->print(std::cout,low1,high1);
  //debug

  ValueFunction *res2
    = ValueFunctionOperations::sumValueFunctions(fvf,pcvf2,low1,high1);
  res2->fixSpaceDimension(4);

  //debug
  std::cout << "res2:\n";
  res2->print(std::cout,low1,high1);
  //debug

  std::cout << "Testing dimensional swap:\n";
  int swap_table[4] = {2,3,0,1};
  BspTreeOperations::swapDimensions(res2,swap_table);
  
  //debug
  std::cout << "res2 with swapped dimensions:\n";
  res2->print(std::cout,low1,high1);
  //debug

  std::cout << "testing sum:\n";
  ValueFunction *res
    = ValueFunctionOperations::sumValueFunctions(res1,res2,low1,high1);

  //debug
  res->print(std::cout,low1,high1);
  //debug
}
