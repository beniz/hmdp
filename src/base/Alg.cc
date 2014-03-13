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

#include "Alg.h"

namespace hmdp_base
{
  double Alg::m_doubleEpsilon = 1e-4;

bool Alg::inVectorInt (const int &g, const std::vector<int> &v)
{
  for (unsigned int i=0; i<v.size (); i++)
    if (g == v[i])
      return true;
  return false;
}

bool Alg::inVectorInt (const std::vector<int> &v1, const std::vector<int> &v2)
{
  for (unsigned int i=0; i<v1.size (); i++)
    {
      bool found = false;
      for (unsigned int j=0; j<v2.size (); j++)
	{
	  if (v1[i] == v2[j])
	    {
	      found = true;
	      break;
	    }
	}
      if (! found) return false;
    }
  return true;
}

bool Alg::eqVectorInt (const std::vector<int> &v1, const std::vector<int> &v2)
{
  if (v1.size () != v2.size ())
    return false;
  
  return Alg::inVectorInt (v1, v2);
}

} /* end of namespace */
