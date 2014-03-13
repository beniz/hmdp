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

#include "HmdpWorld.h"

/* parser structures */
#include "states.h"
#include "problems.h"
#include "domains.h"
#include "exceptions.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace hmdp_loader;
using namespace ppddl_parser;

int main (int argc, char *argv[])
{
  /*
   * Read pddl file and convert to hmdp structures.
   */
  HmdpWorld::loadWorld (argv[1]);

  /* visualize results */
  HmdpWorld::print (std::cout);
}
