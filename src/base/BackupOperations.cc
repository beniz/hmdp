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

#include "BackupOperations.h"

namespace hmdp_base
{
 
ValueFunction* BackupOperations::selectCachedTree (const ValueFunction &vf, const ContinuousTransition &ct)
{
  BspTreeOperations::m_currentOutputType = vf.getType ();
  if (BspTreeOperations::m_currentOutputType == PiecewiseConstantVFT)
    return ct.getPtrPwcVF ();
  else if (BspTreeOperations::m_currentOutputType == PiecewiseLinearVFT)
    return ct.getPtrPwlVF ();
  else 
    {
      std::cerr << "[Error]: BackupOperations::selectCachedTree: unknown tree type: " 
		<< BspTreeOperations::m_currentOutputType << " -- return NULL vf...\n";
      //exit (1);
      return NULL;
    }
}

ValueFunction* BackupOperations::intersectCOWithVF (ValueFunction *vf, ContinuousOutcome *co,
						    double *low, double *high)
{
  BspTreeOperations::m_currentIntersectionType = BTI_MULT;
  BspTree *bt = BspTreeOperations::intersectTrees (vf, co, low, high);
  ValueFunction *res = static_cast<ValueFunction*> (bt);
  if (BspTreeOperations::m_piecesMerging)
    res->mergeTreeLeaves (low, high);
  return res;
}
 
ValueFunction* BackupOperations::intersectVFWithReward (ValueFunction *vf, ContinuousReward *cr,
							double *low, double *high)
{
  BspTreeOperations::m_currentIntersectionType = BTI_PLUS;
  BspTree *bt = BspTreeOperations::intersectTrees (vf, cr, low, high);
  ValueFunction* res = static_cast<ValueFunction*> (bt);
  if (BspTreeOperations::m_piecesMerging)
    res->mergeTreeLeaves (low, high);
  return res;
}

ValueFunction* BackupOperations::backUpCT (ValueFunction *vf, ContinuousTransition *ct, 
					   double *low, double *high)
{
  ValueFunction *res = 0, *tres = 0, *tpart = 0;
  ValueFunction *tilePartition 
    = static_cast<ValueFunction*> (BspTreeOperations::createTree (BackupOperations::selectCachedTree (*vf, *ct)));

  for (int tiles=0; tiles<ct->getTilingDimension (); tiles++)  /* iterate the transition tiles */
    {
      /* fetch pointer to the tree leaf that corresponds to the tile. */
      ContinuousTransition *bspCTTile = ct->getPtrTile (tiles);
      
      /* iterate the 'probabilistic' pieces for that tile */
      for (int i=0; i<bspCTTile->getNContinuousOutcomes (); i++)
	{
	  /* intersect each outcome with the value function */
	  ContinuousOutcome *co = bspCTTile->getContinuousOutcome (i);
	  ValueFunction *piece = BackupOperations::intersectCOWithVF (vf, co, low, high);
	  
	  ValueFunction *cpiece
	    = static_cast<ValueFunction*> (BspTreeOperations::cropTree (piece, co->getLowPos (), co->getHighPos ()));
	  BspTree::deleteBspTree (piece);
	  
	  
	  /* shift the piece back */
	  cpiece 
	    = static_cast<ValueFunction*> (BspTreeOperations::shiftTree (cpiece, co->getShiftBack (), low, high));
	  
	  /* intersect all shifted pieces */
	  if (i > 0)
	    {
	      BspTreeOperations::m_currentIntersectionType = BTI_PLUS; 
	      res = static_cast<ValueFunction*> (BspTreeOperations::intersectTrees (cpiece, tres, low, high));
	      BspTree::deleteBspTree (cpiece); BspTree::deleteBspTree (tres);
	      tres = res;
	      if (BspTreeOperations::m_piecesMerging)
		res->mergeTreeLeaves (low, high);
	    }
	  else res = tres = cpiece;
	}
      
      /* assemble the final partition */
      if (tiles == 0)
	tpart = tilePartition;
      BspTreeOperations::m_currentIntersectionType = BTI_PLUS;
      tilePartition = static_cast<ValueFunction*> (BspTreeOperations::intersectTrees (tpart, res, low, high));
      BspTree::deleteBspTree (tpart); BspTree::deleteBspTree (res);
      tpart = tilePartition;
      if (BspTreeOperations::m_piecesMerging)
	tpart->mergeTreeLeaves (low, high);

    } /* end for tiles */

  return tilePartition;
}

ValueFunction* BackupOperations::backUpSingleOutcome (ValueFunction *vf, ContinuousOutcome *co,
						      double *low, double *high)
{
  
  ValueFunction *piece = BackupOperations::intersectCOWithVF (vf, co, low, high);
  
  ValueFunction *cpiece
    = static_cast<ValueFunction*> (BspTreeOperations::cropTree (piece, co->getLowPos (), co->getHighPos ()));
  BspTree::deleteBspTree (piece);
  
  /* shift the piece back */
  cpiece 
    = static_cast<ValueFunction*> (BspTreeOperations::shiftTree (cpiece, co->getShiftBack (), low, high));
  
  if (BspTreeOperations::m_piecesMerging)
    cpiece->mergeTreeLeaves (low, high);
  
  return cpiece;
}

ValueFunction* BackupOperations::backUpOutcomes (const int &h, const int &t,
						 ContinuousTransition *ct, ValueFunction *vf,
						 double *low, double *high)
{
  if (h == t)
    return BackupOperations::backUpSingleOutcome (vf, ct->getContinuousOutcome (h), low, high);

  ValueFunction *t1 = BackupOperations::backUpOutcomes (h, (h+t)/2, ct, vf, low, high);
  ValueFunction *t2 = BackupOperations::backUpOutcomes ((h+t)/2 + 1, t, ct, vf, low, high);
  
  BspTreeOperations::m_currentIntersectionType = BTI_PLUS; 
  ValueFunction *ret = static_cast<ValueFunction*> (BspTreeOperations::intersectTrees (t1, t2, low, high));

  BspTree::deleteBspTree (t1); BspTree::deleteBspTree (t2);
  if (BspTreeOperations::m_piecesMerging)
    ret->mergeTreeLeaves (low, high);
  return ret;
}

ValueFunction* BackupOperations::backUpCT2 (ValueFunction *vf, ContinuousTransition *ct,
					    double *low, double *high)
{
  ValueFunction *res = 0, *tpart = 0;
  ValueFunction *cachedTree = BackupOperations::selectCachedTree (*vf, *ct);
  ValueFunction *tilePartition = static_cast<ValueFunction*> (BspTreeOperations::createTree (cachedTree));
  
  for (int tiles=0; tiles<ct->getTilingDimension (); tiles++)  /* iterate the transition tiles */
    {
      /* fetch pointer to the tree leaf that corresponds to the tile. */
      ContinuousTransition *bspCTTile = ct->getPtrTile (tiles);
      if (!bspCTTile) // XXX: was crashing on null pointer tiles.
	continue;
      
      /* backup continuous outcomes */
      res = BackupOperations::backUpOutcomes (0, bspCTTile->getNContinuousOutcomes ()-1,
					      bspCTTile, vf, low, high);
      
      BspTreeOperations::m_currentIntersectionType = BTI_PLUS;
      tpart = static_cast<ValueFunction*> (BspTreeOperations::intersectTrees (tilePartition, res, low, high));
      BspTree::deleteBspTree (tilePartition); 
      BspTree::deleteBspTree (res);
      tilePartition = tpart;
      
      if (BspTreeOperations::m_piecesMerging)
	tilePartition->mergeTreeLeaves (low, high);

    } /* end for tiles */

  return tilePartition;
}

ValueFunction* BackupOperations::backUp (ValueFunction *vf, ContinuousTransition *ct,
					 ContinuousReward *cr, const int &action,
					 double *low, double *high)
{
  /* intersect vf with reward. */
  ValueFunction *vfr = NULL;
  if (cr)
    vfr = BackupOperations::intersectVFWithReward (vf, cr, low, high);
  else vfr = vf;
  
  /* backup the transition */
  ValueFunction *qFunction = BackupOperations::backUpCT2 (vfr, ct, low, high);
  if (vfr != vf) BspTree::deleteBspTree (vfr);
  
  /* tag qFunction with action. */
  qFunction->tagWithAction (action);
  
  return qFunction;
}

ValueFunction* BackupOperations::backUp (const HybridTransition &ht, ValueFunction **discStatesVF, 
					 double *low, double *high)
{
  ValueFunction *hqFunction = 0;
  for (int i=0; i<ht.getNOutcomes (); i++)
    { 
      HybridTransitionOutcome *hto = ht.getOutcome (i);
      
      ValueFunction *qai 
	= BackupOperations::backUp (discStatesVF[i], hto->getContTransition (), hto->getContReward (),
				    ht.getActionIndex (), low, high);
      
      if (hto->getOutcomeProbability () < 1.0)
	qai->multiplyByScalar (hto->getOutcomeProbability ());
      
      if (! hqFunction)
	hqFunction = qai;
      else 
	{
	  BspTreeOperations::m_currentIntersectionType = BTI_PLUS;
	  ValueFunction *sum 
	    = static_cast<ValueFunction*> (BspTreeOperations::intersectTrees (qai, hqFunction, low, high));
	  if (BspTreeOperations::m_piecesMerging)
	    sum->mergeTreeLeaves (low, high);
	  BspTree::deleteBspTree (qai); BspTree::deleteBspTree (hqFunction);
	  hqFunction = sum;
	}
    }

  return hqFunction;
}

} /* end of namespace */
