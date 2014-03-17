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

#ifndef CONTINUOUSTRANSITION_H
#define CONTINUOUSTRANSITION_H

#include "BspTree.h"
#include "NormalDiscreteDistribution.h"
#include "discretizationTypes.h"
#include "MDDiscreteDistribution.h"
#include "ContinuousOutcome.h"
#include "PiecewiseConstantValueFunction.h"
#include "PiecewiseLinearValueFunction.h"
#include <iostream>

namespace hmdp_base
{

/**
 * \class ContinuousTransition
 * \brief continuous transition as a bsp tree. Each sub tree is represented as 
 *        a ContinuousTransition object.
 */
class ContinuousTransition : public BspTree
{
 public:
  /**
   * \brief constructor
   * @param sdim the continuous space dimension.
   */
  ContinuousTransition (const int &sdim);

  /**
   * \brief constructor
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the partitioning position.
   */
  ContinuousTransition (const int &sdim, const int &d, const double &pos);

  /**
   * \brief constructor
   * @param dimension the number of transition tiles in the continuous space,
   * @param sdim the continuous space dimension.
   */
  ContinuousTransition (const int &dimension, const int &sdim);
  
  /**
   * \brief constructor
   * @param dimension the number of transition tiles in the continuous space,
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the partitioning position.
   */
  ContinuousTransition (const int &dimension, const int &sdim, 
			const int &d, const double &pos);

 public:
 /**
   * \brief constructor
   * @param dimension the number of transition tiles in the continuous space.
   * @param sdim the continuous space dimension.
   * @param lowPos array of the low coordinates of the corner points of the tiles.
   * @param highPos array of the high coordinates of the corner points of the tiles.
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   * @param epsilon threshold for the discretization, for each tile, for each dimension.
   * @param intervals interval for the discretization, for each tile, for each dimension
   *                  !!NOTE!!: If dt type is DISCRETIZATION_WRT_POINTS, points are stored in place 
   *                  of intervals.
   * @param dt type of discretization: DISCRETIZATION_WRT_INTERVAL or
   *                                   DISCRETIZATION_WRT_THRESHOLD or
   *                                   DISCRETIZATION_WRT_POINTS.
   * @param means probability distribution mean, for each tile, for each dimension.
   * @param sds probability distribution standard deviation, for each tile, for each dimension.
   * @param relative relative/absolute transition flag, for each tile, for each dimension.
   * @param distrib distribution type: GAUSSIAN or UNIFORM, for each tile, for each dimension.
   */
  /* TODO: uniform distribution */
  ContinuousTransition (const int &dim, const int &sdim, const discretizationType dt,
			double **lowPos, double **highPos,
			double *low, double *high,
			const double **epsilon, const double **intervals, 
			const double **means, const double **sds, bool **relative,
			const discreteDistributionType **distrib);

  /**
   * \brief Copy constructor
   */
  ContinuousTransition (const ContinuousTransition &ct);

 protected:
  /**
   * \brief copy constructor (from an object of the father's class type)
   * @param number of transition tiles in the continuous space.
   * @param bt a bsp tree.
   */
  ContinuousTransition (const int &dimension, const BspTree &bt);

 public:
  /**
   * \brief Destructor.
   */
  virtual ~ContinuousTransition ();

 private:
  void selfReferenceTiles (ContinuousTransition *ct, ContinuousTransition **ptrref);

 protected:
  /* virtual functions */
  void leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);

  void transferData (const BspTree &bt);

 public:
  /* setters */
  /**
   * \brief leaf distribution setter
   * @param jdd a multi-dimension discrete distribution object.
   */
  void setLeafDistribution (MDDiscreteDistribution *jdd) { m_jdd = jdd; }

  /**
   * \brief leaf cached bsp trees for the probability distribution (shifted pieces) (Internal).
   * @param cos an array of ContinuousOutcomes pointers, i.e bsp trees.
   */
  void setLeafContinuousOutcomes (ContinuousOutcome **cos) { m_shiftedProbabilisticOutcomes = cos; }

  void setLeafProjectedContinuousOutcomes (ContinuousOutcome **cos)
  { m_projectedProbabilisticOutcomes = cos; }
  
  /**
   * \brief leaf number of continuous outcomes setter.
   * @param n number of outcomes (pieces of the discretized leaf probability distribution).
   */
  void setNContinuousOutcomes (const int &n) { m_numberContinuousOutcomes = n; }

  void setNProjectedContinuousOutcomes (const int &n) { m_numberProjectedContinuousOutcomes = n; }

  void setNTile (const int &tile) { m_nTile = tile; }
  
  /* accessors */
  /**
   * \brief tiling dimension accessor
   * @return the number of transition tiles in the continuous space.
   */
  int getTilingDimension () const { return m_tilingDimension; }

  /**
   * \brief leaf distribution accessor
   * @return a multi-dimensional discrete distribution object.
   */
  MDDiscreteDistribution* getLeafDistribution () const
    { if (isLeaf () && m_jdd) return m_jdd; 
    else return NULL; }

  /**
   * \brief transition relative/absolute flag accessor
   * @param tdim transition tile number
   * @param cdim continuous dimension
   * @return true if transition is relative in the tile for the dimension, no if absolute.
   */
  bool getRelative (int tdim, int cdim) const { return m_relative[tdim][cdim]; }

  ContinuousOutcome** getContinuousOutcomes () const { return m_shiftedProbabilisticOutcomes; }

  ContinuousOutcome** getProjectedContinuousOutcomes () const { return m_projectedProbabilisticOutcomes; }

  /**
   * \brief cache of continuous outcomes accessor.
   * @param pos position of a continuous outcome.
   * @return a continuous outcome (bsp tree).
   */
  ContinuousOutcome* getContinuousOutcome (const int &pos) const 
    { return m_shiftedProbabilisticOutcomes[pos]; }

  ContinuousOutcome* getProjectedContinuousOutcome (const int &pos) const
    { return m_projectedProbabilisticOutcomes[pos]; }

  /**
   * \brief number of continuous outcomes accessor.
   * @param tile tile index.
   * @return number of continuous outcomes for this transition tile.
   */
  int getNContinuousOutcomes () const { return m_numberContinuousOutcomes; }

  int getNProjectedContinuousOutcomes () const { return m_numberProjectedContinuousOutcomes; }

  /**
   * \brief tile number accessor.
   * @return the tile number (leaf only, -1 otherwise).
   */
  int getNTile () const { return m_nTile; }

  /**
   * \brief quick tile pointer accessor.
   * @param tile tile number.
   * @return tile pointer for this tile number.
   */
  ContinuousTransition* getPtrTile (const int &tile) const 
    { return m_ptrToTiles[tile]; }
  
  /**
   * \brief accessor to the continuous transition tree converted to a pwc vf. 
   *        This is to speed up 'back up' operations.
   * @return a pointer to a pwc vf bsp tree.
   */
  PiecewiseConstantValueFunction* getPtrPwcVF () const { return m_ctpwcVF; }

  /**
   * \brief accessor to the continuous transition tree converted to a pwl vf. 
   *        This is to speed up 'back up' operations.
   * @return a pointer to a pwl vf bsp tree.
   */
  PiecewiseLinearValueFunction* getPtrPwlVF () const { return m_ctpwlVF; }

  ContinuousStateDistribution* getPtrCSD () const { return m_ctCSD; }

  void hasZeroConsumption (bool &isNullOutcome);

  /* printing */
  void print (std::ostream &out, double *low, double *high);
  
 private:
  /**
   * \brief relative array accessor
   * @return array of relative/absolute flags (for all tiles/dimensions).
   */
  bool** getRelative () const { return m_relative; }

 private:
  int m_tilingDimension; /**< number of transition tiles (root node only, 0 otherwise). */
  MDDiscreteDistribution *m_jdd;  /**< joint discrete probability distribution, 
				     if the tree is a leave */
  bool **m_relative;  /**< relative/absolute transition flag, per dimension, per tile. */
  ContinuousOutcome **m_shiftedProbabilisticOutcomes; /**< cache of shifted outcome of speeding up 
							 the backups (a pointer per discrete point). */
  ContinuousOutcome **m_projectedProbabilisticOutcomes; /**< cache of forward projection of 
							   outcomes, for speeding up the front ups
							   (convolutions). */
  int m_numberContinuousOutcomes;  /**< number of continuous outcomes, in leaf. */
  int m_numberProjectedContinuousOutcomes; /**< number of projected continuous outcomes. */
  ContinuousTransition **m_ptrToTiles;  /**< root node stores pointers to the leaves that 
					   correspond to the tiles. */
  int m_nTile;  /**< tile index (in leaf). */
  PiecewiseConstantValueFunction *m_ctpwcVF; /**< root caches the ct structure as a value function (for backup). */
  PiecewiseLinearValueFunction *m_ctpwlVF;
  ContinuousStateDistribution  *m_ctCSD; /**< tiles cache for frontup. */
};

} /* end of namespace */
#endif
