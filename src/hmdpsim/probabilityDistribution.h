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

#ifndef PROBABILITYDISTRIBUTION_H
#define PROBABILITYDISTRIBUTION_H

#include "distributionTypes.h"
#include <map>
#include <string>
#include <ostream>

namespace ppddl_parser
{

class Expression; 
class FunctionTable;
class TermTable;

/**
 * \brief unidimensional probability distribution.
 */
class ProbabilityDistributionFunc
{
 public:
  ProbabilityDistributionFunc (distributionType dt,
			       const Expression &mean, 
			       const Expression &stddev);

  ProbabilityDistributionFunc (distributionType dt,
			       const Expression &mean,
			       const Expression &stddev,
			       disczType dzt,
			       const Expression &arg1,
			       const Expression &arg2);

  /* accessors */
  distributionType getType () const { return dt_; }
  const Expression* getMean () const { return func_mean_; }
  const Expression* getStdDev () const { return func_stddev_; }
  disczType getDisczType () const { return dzt_; }
  const Expression* getInterval () const { return discz_interval_; }
  const Expression* getThreshold () const { return discz_threshold_; }
  const Expression* getPoints () const { return discz_points_; }

  /* print */
  void print (std::ostream &os, const FunctionTable &functions, 
	      const TermTable &terms) const;

 private:
  distributionType dt_; /**< type of the distribution */
  const Expression *func_mean_;
  const Expression *func_stddev_; 
  disczType dzt_;
  const Expression *discz_interval_;
  const Expression *discz_threshold_;
  const Expression *discz_points_;
};

class ProbabilityDistMap : public std::map<std::string, const ProbabilityDistributionFunc*> {
};

} /* end of namespace */

#endif
