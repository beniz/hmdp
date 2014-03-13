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

#include "LpSolve5.h"
#include "AlphaVector.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace hmdp_base;

int main ()
{
  /* domain */
  double low[2] = {0.1,0.1}, high[2] = {1.0,1.0};

  std::cout << "testing lp solving with lp_solve_5.5\n";
  double alph1[3] = {2.0,2.0,3.0}, alph2[3] = {2.0,-1.0,1.0}, alph3[3] = {1.0,-0.5,3.0};
  
  AlphaVector *av1 = new AlphaVector (3, alph1);
  AlphaVector *av2 = new AlphaVector (3, alph2);
  AlphaVector *av3 = new AlphaVector (3, alph3);
  std::vector<AlphaVector*> *vav = new std::vector<AlphaVector*> ();
  //vav->push_back (av1); 
  vav->push_back (av2); 
  vav->push_back (av3);

  double witness[2];
  std::cout << "av1 " << *av1 << " dominated: " 
	    << LpSolve5::isLPDominated(av1, vav, witness, low, high)
	    << std::endl;
  
  vav->push_back(av1);
  std::vector<AlphaVector*>* pvav = new std::vector<AlphaVector*> ();
  LpSolve5::pruneLP (vav, low, high, pvav);
  std::cout << "pruned set:\n";
  for (unsigned int i=0; i<pvav->size (); i++)
  std::cout << *(*pvav)[i] << std::endl;
}
