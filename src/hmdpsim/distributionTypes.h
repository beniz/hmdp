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

#ifndef DISTRIBUTIONTYPES_H
#define DISTRIBUTIONTYPES_H

namespace ppddl_parser
{

enum distributionType {
  DISTRIBUTION_NORMAL = 0,
  DISTRIBUTION_UNIFORM = 1
};

enum disczType {
  DISCRETIZATION_INTERVAL = 0,
  DISCRETIZATION_THRESHOLD = 1,
  DISCRETIZATION_POINTS = 2,
  DISCRETIZATION_DEFAULT = 100  /* defaulted to HmdpPpddlLoader -> interval */
};

} /* end of namespace */

#endif
