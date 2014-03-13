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

#include "BspTree.h"
#include "BspTreeOperations.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace hmdp_base;

int main ()
{
  double a1[2] = {0.4,0.1}, b1[2] = {0.8,0.4};
  double a2[2] = {0.1,0.6}, b2[2] = {0.7,0.2};

  /* domain */
  double low[2]={0.0,0.0}, high[2]={1.0,1.0};

  ofstream output ("test.box", ios::out);

  BspTree *bsp1 = new BspTree (2, a1, b1, low, high, 0);
  BspTree *bsp2 = new BspTree (2, a2, b2, low, high, 0);

  double low1[2]={0.3,0.3}, high1[2]={0.6,0.5};
  
  BspTree *bsp3 = BspTreeOperations::intersectTrees (bsp1, bsp2, low, high);
  bsp3->plot2Dbox (0.0, output, low, high);

  ofstream output1 ("test1.box", ios::out);
  BspTree *bsp4 = BspTreeOperations::intersectTrees (bsp1, bsp2, low1, high1);
  bsp4->plot2Dbox (0.0, output1, low, high);

  ofstream output2 ("test2.box", ios::out);
  BspTree *bsp5 = BspTreeOperations::cropTree (bsp3, low1, high1);
  bsp5->plot2Dbox (0.0, output2, low, high);
  

  BspTree::deleteBspTree (bsp1);
  BspTree::deleteBspTree (bsp2);
  BspTree::deleteBspTree (bsp3);
  BspTree::deleteBspTree (bsp4); 
  BspTree::deleteBspTree (bsp5);
}
