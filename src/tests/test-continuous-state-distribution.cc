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

#include "ContinuousStateDistribution.h"
#include "NormalDiscreteDistribution.h"
#include "PiecewiseConstantValueFunction.h"
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace hmdp_base;

int main ()
{
  Alg::m_doubleEpsilon = 1e-20;
  DiscreteDistribution::m_positiveResourcesConsumptionTruncation = false;
  NormalDiscreteDistribution ndd1 (500.0, 40, 0.001, 10.0, DISCRETIZATION_WRT_INTERVAL);
  cout << "ndd1: " << ndd1 << endl;
  
  NormalDiscreteDistribution ndd2 (300.0, 10.0, 0.001, 10.0, DISCRETIZATION_WRT_INTERVAL);
  cout << "ndd2: " << ndd2 << endl;

  cout << "------- building discrete joint distribution -------\n";
  DiscreteDistribution **ndds
    = (DiscreteDistribution **) malloc (2 * sizeof (DiscreteDistribution*));
  ndds[0] = &ndd1; ndds[1] = &ndd2;
  MDDiscreteDistribution *mdd
    = MDDiscreteDistribution::jointDiscreteDistribution (2, ndds);
  free (ndds);
  cout << *mdd << endl;

  /* domain */
  double low[2] = {0.0, 0.0}, high[2] = {10000.0, 10000.0};

  cout << "------- building continuous state distribution -------\n";
  ContinuousStateDistribution *csd 
    = ContinuousStateDistribution::convertMDDiscreteDistribution (*mdd, low, high);

  cout << "csd:\n";
  csd->print (std::cout, low, high);

  double *mpos = new double[2];
  mpos[0] = mpos[1] = 0.0;
  csd->getMaxPosition (mpos, low, high);
  cout << "csd max pos:\n";
  for (int i=0; i<2; i++)
    cout << "mpos[" << i <<"]: " << mpos[i] << endl;
  

  ofstream output ("testCSD_mdd.box", ios::out);
  csd->plot2Dbox (0.0, output, low, high);
  output.close ();

  ofstream output_vrml ("testCSD_mdd.vrml", ios::out);
  csd->plot2DVrml2 (0.0, output_vrml, low, high, 1.0);
  output_vrml.close ();

  cout << "------- computing expectation -------\n";
  PiecewiseConstantValueFunction *pcvf = new PiecewiseConstantValueFunction (2, low, high, 5.0);
  double expect = pcvf->computeExpectation (csd, low, high);
  cout << "expectation: " << expect << endl;

  /* cout << "------- re-building multi-dimensionnal discrete distribution ------\n";
     MDDiscreteDistribution *mddc = new MDDiscreteDistribution (csd, low, high);
     cout << *mddc << endl; */
}
