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

#include "ValueFunctionOperations.h"
#include <stdlib.h>

namespace hmdp_base
{

ValueFunction* ValueFunctionOperations::sumValueFunctions (ValueFunction *vf1,
							   ValueFunction *vf2,
							   double *low, double *high)
{
  BspTreeOperations::m_currentIntersectionType = BTI_PLUS;  /* leaf intersection type */
  BspTree *bt = BspTreeOperations::intersectTrees (vf1, vf2, low, high);
  ValueFunction *vf = static_cast<ValueFunction*> (bt);
  if (BspTreeOperations::m_piecesMerging)
    vf->mergeTreeLeaves (low, high);
  return vf;
}

ValueFunction* ValueFunctionOperations::subtractValueFunctions (ValueFunction *vf1,
								ValueFunction *vf2,
								double *low, double *high)
{
  /* can't use the bsp balance. */
  bool isBalance = false;
  if (BspTreeOperations::m_bspBalance)
    {
      BspTreeOperations::m_bspBalance = false;
      isBalance = true;
    }
  BspTreeOperations::m_currentIntersectionType = BTI_MINUS;
  BspTree *bt = BspTreeOperations::intersectTrees (vf1, vf2, low, high);
  if (isBalance)
    BspTreeOperations::m_bspBalance = true;
  ValueFunction *vf = static_cast<ValueFunction*> (bt);
  if (BspTreeOperations::m_piecesMerging)
    vf->mergeTreeLeaves (low, high);
  return vf;
}

ValueFunction* ValueFunctionOperations::maxValueFunction (ValueFunction *vf1,
							  ValueFunction *vf2, 
							  double *low, double *high)
{
  BspTreeOperations::m_currentIntersectionType = BTI_MAX;
  BspTree *bt = BspTreeOperations::intersectTrees (vf1, vf2, low, high);
  ValueFunction *vf = static_cast<ValueFunction*> (bt);
  if (BspTreeOperations::m_piecesMerging)
    vf->mergeTreeLeaves (low, high);
  return vf;
}

ValueFunction* ValueFunctionOperations::minValueFunction (ValueFunction *vf1,
                                                          ValueFunction *vf2,
                                                          double *low, double *high)
{
  BspTreeOperations::m_currentIntersectionType = BTI_MIN;
  BspTree *bt = BspTreeOperations::intersectTrees (vf1, vf2, low, high);
  ValueFunction *vf = static_cast<ValueFunction*> (bt);
  if (BspTreeOperations::m_piecesMerging)
    vf->mergeTreeLeaves (low, high);
  return vf;
}

bool ValueFunctionOperations::compareValueFunctions (ValueFunction *vf1,
						     ValueFunction *vf2,
						     double *low, double *high)
{
  if ((vf1->isLeaf () && ! vf2->isLeaf ())
      || (! vf1->isLeaf () && vf2->isLeaf ()))
    return false;
  else
    {
      if (vf1->isLeaf () && vf2->isLeaf ())
	{
	  /* compare leaves */
	  if (! vf1->getAlphaVectors () || ! vf2->getAlphaVectors ()
	      || ! AlphaVector::isVecEqual (*vf1->getAlphaVectors (), 
					    *vf2->getAlphaVectors ()))
	    return false;
	}
      else
	{
	  ValueFunction *vf1lt = static_cast<ValueFunction*> (vf1->getLowerTree ());
	  ValueFunction *vf2lt = static_cast<ValueFunction*> (vf2->getLowerTree ());
	  bool isEqualLow 
	    = ValueFunctionOperations::compareValueFunctions (vf1lt, vf2lt, low, high);
	  
	  ValueFunction *vf1ge = static_cast<ValueFunction*> (vf1->getGreaterTree ());
	  ValueFunction *vf2ge = static_cast<ValueFunction*> (vf2->getGreaterTree ());
	  bool isEqualHigh 
	    = ValueFunctionOperations::compareValueFunctions (vf1ge, vf2ge, low, high);

	  return (isEqualLow && isEqualHigh);
	}
    }
  return true;
}

PiecewiseLinearValueFunction* ValueFunctionOperations::mergeVFByActions (ValueFunction *vf,
									 double *low, double *high)
{
  /* convert to a pwl with action sets in place of pwl coeffs. */
  PiecewiseLinearValueFunction *pwlactions = new PiecewiseLinearValueFunction (*vf, true);
  
  /* do the merging. */
  bool pm = BspTreeOperations::m_piecesMerging;
  bool pmv = BspTreeOperations::m_piecesMergingByValue;
  
  BspTreeOperations::m_piecesMerging = true;
  BspTreeOperations::m_piecesMergingByValue = false;
  BspTreeOperations::m_piecesMergingByAction = true;
  pwlactions->mergeTreeLeaves (low, high);  /* merging. */
  if (! pmv) BspTreeOperations::m_piecesMergingByValue = false;
  if (! pm) BspTreeOperations::m_piecesMerging = false;
  BspTreeOperations::m_piecesMergingByAction = false;

  return pwlactions;
}

void ValueFunctionOperations::breakTiesOnActionsWithMaxConsumption (ValueFunction *vf, 
								    std::map<int, double> &max_cons)
{
  /* break ties on actions. */
  vf->breakTiesOnActions (&max_cons);

  //if (BspTreeOperations::m_piecesMerging)
    //vf->mergeTreeLeaves (low, high);
}

void ValueFunctionOperations::breakTiesOnActionsWithCoverage (ValueFunction *vf, double *low, double *high)
{
  std::set<int> actions; std::set<int>::const_iterator actit;
  vf->collectActions (&actions, low, high);
  std::map<int, double> coverage; 
  //std::map<int, double>::const_iterator covit;
  
  /* initialize each action coverage. */
  for (actit = actions.begin (); actit != actions.end (); actit++)
    coverage.insert (std::pair<int, double> ((*actit), 0.0));
  
  /* coverage (BspTreeAlpha). */
  vf->getActionsCoverage (&coverage, low, high);

  //debug
  /* std::cout << "coverage table:\n";
     for (covit = coverage.begin (); covit != coverage.end (); covit++)
     std::cout << "action: " << (*covit).first << ", coverage: " << (*covit).second << std::endl; */
  //debug

  /* break ties on actions. */
  vf->breakTiesOnActions (&coverage);
  if (BspTreeOperations::m_piecesMerging)
      vf->mergeTreeLeaves (low, high);
}

std::map<int, ValueFunction*> ValueFunctionOperations::breakVFByActions (ValueFunction *vfactions, const bool &prop, double *low, double *high)
{
  std::map<int, ValueFunction*> vfbyactions;

  /* get actions in this vf. */
  std::set<int> actions;
  vfactions->collectActions (&actions, low, high);

  /**
   * for each action:
   *  - set 'action zone' to 1,
   *  - set rest of the space to 0,
   *  - merge by value.
   */
  std::set<int>::const_iterator actit;
  for (actit = actions.begin (); actit != actions.end (); actit++)
    { 
      ValueFunction *vfbyaction = ValueFunctionOperations::breakVFByAction (vfactions, (*actit), prop, low, high);
      vfbyactions[(*actit)] = vfbyaction;
    }

  /* return table of VF / action. */
  return vfbyactions;
}

ValueFunction* ValueFunctionOperations::breakVFByAction (ValueFunction *vfactions, 
							 const int &action,
							 const bool &prop,
							 double *low, double *high)
{
  ValueFunction *vfbyaction = NULL;
  
  /* set action zone to 1, remaining space to 0. */
  if (vfactions->getType () == PiecewiseConstantVFT)
    {
      PiecewiseConstantValueFunction *pwcvf 
	= static_cast<PiecewiseConstantValueFunction*> (vfactions);
      vfbyaction = new PiecewiseConstantValueFunction (*pwcvf, action, prop);
    } 
  else if (vfactions->getType () == PiecewiseLinearVFT)
    {
      PiecewiseLinearValueFunction *pwlvf
	= static_cast<PiecewiseLinearValueFunction*> (vfactions);
      vfbyaction = new PiecewiseLinearValueFunction (*pwlvf, action, prop);
    }
  else
    {
      std::cout << "[Error]:ValueFunctionOperations::breakVFByAction: unknown value function type: "
		<< vfactions->getType () << ".Exiting.\n";
      exit (-1);
    }

  /* merge by value. */
  bool mp = BspTreeOperations::m_piecesMerging;
  bool mbv = BspTreeOperations::m_piecesMergingByValue;
  BspTreeOperations::m_piecesMerging = true;
  BspTreeOperations::m_piecesMergingByValue = true;
  vfbyaction->mergeTreeLeaves (low, high);
  if (! mp) BspTreeOperations::m_piecesMerging = false;
  if (! mbv) BspTreeOperations::m_piecesMergingByValue = false;

  return vfbyaction;
}

} /* end of namespace */
