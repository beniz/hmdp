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
  DiscreteDistribution::m_positiveResourcesConsumptionTruncation = true;
  cout << "------- testing interval based discretization -------\n";
  NormalDiscreteDistribution ndd (-1.0, 10.0, 0.001, 10.0, DISCRETIZATION_WRT_INTERVAL);
  cout << ndd << endl;

  /* DiscreteDistribution::m_positiveResourcesConsumptionTruncation = true;
     cout << "------- testing interval based discretization -------\n";
     NormalDiscreteDistribution ndd1 (-1.0, 10.0, 0.001, 10.0, DISCRETIZATION_WRT_INTERVAL);
     cout << ndd1 << endl; */

  /* DiscreteDistribution::m_positiveResourcesConsumptionTruncation = true;
     cout << "------- testing point based discretization -------\n";
     NormalDiscreteDistribution ndd2a (-1.0, 10.0, 0.001, 10);
     cout << ndd2a << endl; */

  //DiscreteDistribution::m_positiveResourcesConsumptionTruncation = true;
  cout << "------- testing point based discretization -------\n";
  NormalDiscreteDistribution ndd2 (-1.0, 10.0, 0.001, 10);
  cout << ndd2 << endl;
  
  cout << "------- testing threshold based discretization -------\n";
  NormalDiscreteDistribution ndd3 (-1.0, 10.0, 0.001, 10.0, DISCRETIZATION_WRT_THRESHOLD);
  cout << ndd3 << endl;

  /* DiscreteDistribution::m_positiveResourcesConsumptionTruncation = false;
     cout << "------- testing threshold based discretization -------\n";
     NormalDiscreteDistribution ndd3a (-1.0, 10.0, 0.001, 10.0, DISCRETIZATION_WRT_THRESHOLD);
     cout << ndd3a << endl; */

  cout << "------- discrete joint distribution -------\n";
  DiscreteDistribution **ndds 
    = (DiscreteDistribution **) malloc (3 * sizeof (DiscreteDistribution*));
  ndds[0] = &ndd; ndds[1] = &ndd2; ndds[2] = &ndd3;
  MDDiscreteDistribution *mdd 
    = MDDiscreteDistribution::jointDiscreteDistribution (3, ndds);
    cout << *mdd << endl;

  /* double pos[3] = {28.0, 21.0, 10.0};
     cout << mdd->getProbability (pos) << endl; */

  /* free (ndds);
     delete mdd; */
}
