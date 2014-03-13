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

/**
 * Types for discretization:
 * - WrtInterval: 0: discretization is specified by an interval size, and a lowest probability mass.
 * - WrtPoints: 1: discretization is specified by a number of points and a lowest probability mass.
 * - WrtThreshold: 2: discretization is specified by an interval size, and a threshold.
 */

#ifndef DISCRETIZATIONTYPES_H
#define DISCRETIZATIONTYPES_H

namespace hmdp_base
{

enum discretizationType {
  DISCRETIZATION_WRT_INTERVAL = 0,
  DISCRETIZATION_WRT_POINTS = 1,
  DISCRETIZATION_WRT_THRESHOLD = 2
};

}  /* end of namespace */
#endif
