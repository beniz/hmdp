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

#include "PiecewiseConstantReward.h"
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

void plot2DCR (ContinuousReward *cr,
	       double *low, double *high)
{
  if (cr->isLeaf ())
  {
    cout << "leaf: [" << low[0] << "," << high[0] << "], ["
	 << low[1] << "," << high[1] << "]\n";
    if (cr->getAlphaVectors ())
      cout << *cr->getAlphaVectorNth (0) << endl;
  }
  else
    {
      cout << "dim: " << cr->getDimension () << " -- pos: " << cr->getPosition () << endl;
      double b=low[cr->getDimension ()];
      low[cr->getDimension ()] = cr->getPosition ();
      ContinuousReward *cgt = dynamic_cast<ContinuousReward*> (cr->getGreaterTree ());
      plot2DCR (cgt, low, high);
      low[cr->getDimension()] = b;

      b = high[cr->getDimension()];
      high[cr->getDimension ()] = cr->getPosition ();
      ContinuousReward *clt = dynamic_cast<ContinuousReward*> (cr->getLowerTree ());
      plot2DCR (clt, low, high);
      high[cr->getDimension ()] = b;
    }
}

int main ()
{
  /* domain */
  double low[2] = {0.0,0.0}, high[2] = {1.0,1.0};

  std::cout << "testing continuous reward pwc\n";
  ofstream output ("testCR1_pwc.box", ios::out);

  double lowCorners[4][2] = {{0.0,0.8},{0.0,0.0},{0.4,0.8},{0.4,0.0}};
  double highCorners[4][2] = {{0.4,1.0},{0.4,0.8},{1.0,1.0},{1.0,0.8}};
  double values[4] = {1.0, 2.0, 3.0, 4.0};

  double **lowCornersP = createPtrFrom2DArray (lowCorners);
  double **highCornersP = createPtrFrom2DArray (highCorners);
  
  PiecewiseConstantReward *pcr = new PiecewiseConstantReward (4, 2, lowCornersP, highCornersP,
							      low, high, values);
  pcr->plot2Dbox (0.0, output, low, high);  
  plot2DCR (pcr, low, high);
  ofstream output_vrml ("testCR1.vrml", ios::out);
  
  double mval = 0.0;
  pcr->getMaxValue (mval, low, high);
  pcr->plot2DVrml2 (0.0, output_vrml, low, high, mval);
}
