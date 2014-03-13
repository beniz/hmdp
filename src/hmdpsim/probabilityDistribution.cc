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

#include "probabilityDistribution.h"
#include "expressions.h"

namespace ppddl_parser
{

ProbabilityDistributionFunc::ProbabilityDistributionFunc (distributionType dt,
							  const Expression &mean, 
							  const Expression &stddev)
  : dt_ (dt), func_mean_ (&mean), func_stddev_ (&stddev), dzt_ (DISCRETIZATION_DEFAULT)
{}

ProbabilityDistributionFunc::ProbabilityDistributionFunc (distributionType dt,
							  const Expression &mean, 
							  const Expression &stddev,
							  disczType dzt,
							  const Expression &arg1,
							  const Expression &arg2)
  : dt_ (dt), func_mean_ (&mean), func_stddev_ (&stddev), dzt_ (dzt)
{    
  if (dzt == DISCRETIZATION_INTERVAL)   /* interval first, epsilon second */
    {
      discz_interval_ = &arg1;
    }
  else if (dzt == DISCRETIZATION_THRESHOLD)  /* interval first, epsilon second */
    {
      discz_interval_ = &arg1;
    }
  else if (dzt == DISCRETIZATION_POINTS)   /* points first, epsilon second */
    {
      discz_points_ = &arg1;
    }
  
  discz_threshold_ = &arg2;
}

void ProbabilityDistributionFunc::print (std::ostream &os, 
					 const FunctionTable &functions,
					 const TermTable &terms) const
{
  os << "type: ";
  if (dt_ == DISTRIBUTION_NORMAL)
    os << "normal ";
  else if (dt_ == DISTRIBUTION_UNIFORM)
    os << "uniform ";
  else os << "unknown distribution ";
  
  os << " mean: "; func_mean_->print (os, functions, terms);
  os << " -- stddev: "; func_stddev_->print (os, functions, terms);
  
  os << " -- discretization type: ";
  if (dzt_ != DISCRETIZATION_DEFAULT)
    {
      if (dzt_ == DISCRETIZATION_INTERVAL)
	{
	  os << "interval: ";
	  discz_interval_->print (os, functions, terms);
	}
      else if (dzt_ == DISCRETIZATION_THRESHOLD)
	{
	  os << "threshold ";
	  discz_interval_->print (os, functions, terms);
	}
      else if (dzt_ == DISCRETIZATION_POINTS)
	{
	  os << "points ";
	  discz_points_->print (os, functions, terms);
	}
      os << "\nthreshold: ";
      discz_threshold_->print (os, functions, terms);
    }
  else os << "default ";
}

} /* end of namespace */
