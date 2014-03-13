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

#include "NormalDiscreteDistribution.h"
#include "MDDiscreteDistribution.h"
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace hmdp_base;

int main ()
{
  
  /* distributions */
  NormalDiscreteDistribution ndd1 (0.5, 0.01, 0.001, 0.1, DISCRETIZATION_WRT_THRESHOLD);
  NormalDiscreteDistribution ndd2 (0.5, 0.01, 0.001, 0.1, DISCRETIZATION_WRT_THRESHOLD);
  NormalDiscreteDistribution ndd5 (0.5, 0.01, 0.001, 0.1, DISCRETIZATION_WRT_THRESHOLD);

  NormalDiscreteDistribution ndd3 (0.3, 0.01, 0.001, 0.1, DISCRETIZATION_WRT_THRESHOLD);
  NormalDiscreteDistribution ndd4 (0.3, 0.01, 0.001, 0.1, DISCRETIZATION_WRT_THRESHOLD);
  NormalDiscreteDistribution ndd6 (0.3, 0.01, 0.001, 0.1, DISCRETIZATION_WRT_THRESHOLD);

  cout << "Testing bi-dimensional convolution.\n";
  DiscreteDistribution **ndds1 
    = (DiscreteDistribution **) malloc (2 * sizeof (DiscreteDistribution*));
  ndds1[0] = &ndd1; ndds1[1] = &ndd2;
  DiscreteDistribution **ndds2 
    = (DiscreteDistribution **) malloc (2 * sizeof (DiscreteDistribution*));
  ndds2[0] = &ndd3; ndds2[1] = &ndd4;
  
  MDDiscreteDistribution *mdd1 = MDDiscreteDistribution::jointDiscreteDistribution (2, ndds1);
  MDDiscreteDistribution *mdd2 = MDDiscreteDistribution::jointDiscreteDistribution (2, ndds2);
  free (ndds1); free (ndds2);
  /* cout << "points: " << mdd1->getNPoints () 
     << ", " << mdd2->getNPoints () << endl; */
  cout << *mdd1 << *mdd2 << endl;

  /* distribution convolution domains */
  double low1[2] = {0.2, 0.2}, high1[2] = {0.7, 0.7}, low2[2] = {0.2, 0.2}, high2[2] = {0.7,0.7};
  double low[2] = {0.0, 0.0}, high[2]={2.0, 2.0};

  MDDiscreteDistribution *mddconv 
    = MDDiscreteDistribution::convoluteMDDiscreteDistributions (*mdd1, *mdd2,
								low, high,
								low1, high1,
								low2, high2);
  cout << *mddconv << endl;
  //delete mdd1; delete mdd2; delete mddconv;

  cout << "Testing tri-dimensional convolution.\n";
  DiscreteDistribution **ndds3 = (DiscreteDistribution **) malloc (3 * sizeof (DiscreteDistribution*));
  DiscreteDistribution **ndds4 = (DiscreteDistribution **) malloc (3 * sizeof (DiscreteDistribution*));
  ndds3[0] = &ndd1; ndds3[1] = &ndd2; ndds3[2] = &ndd5;
  ndds4[0] = &ndd3; ndds4[1] = &ndd4; ndds4[2] = &ndd6;
  
  MDDiscreteDistribution *mdd3 = MDDiscreteDistribution::jointDiscreteDistribution (3, ndds3);
  MDDiscreteDistribution *mdd4 = MDDiscreteDistribution::jointDiscreteDistribution (3, ndds4);
  free (ndds3); free (ndds4);

  double low31[3] = {0.2, 0.2, 0.2}, high31[3] = {0.7, 0.7, 0.7}, low32[3] = {0.2, 0.2, 0.2}, high32[3] = {0.7, 0.7, 0.7};
  double low3[3] = {0.0, 0.0, 0.0}, high3[3] = {2.0, 2.0, 2.0};
  MDDiscreteDistribution *mddconv3 
    = MDDiscreteDistribution::convoluteMDDiscreteDistributions (*mdd3, *mdd4, low3, high3,
								low31, high31, low32, high32);
  cout << *mddconv3 << endl;
  delete mdd3; delete mdd4; delete mddconv3;

  cout << "Testing uniform/normal convolution.\n";
  double mean = 0.3, prob = 1.0;
  DiscreteDistribution dd1 (NONE, 1, 0.1, &mean, &prob); 
  DiscreteDistribution dd2 (NONE, 1, 0.1, &mean, &prob);
  DiscreteDistribution **ndds = new DiscreteDistribution*[2];
  ndds[0] = &dd1; ndds[1] = &dd2;

  MDDiscreteDistribution *mddn = MDDiscreteDistribution::jointDiscreteDistribution (2, ndds);
  delete []ndds;
  
  mddconv 
    = MDDiscreteDistribution::convoluteMDDiscreteDistributions (*mddn, *mdd1,
								low, high,
								low1, high1,
								low2, high2);
  cout << *mddconv << endl;
  delete mddn;
}
