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
 * \brief Top class for bsp (Binary Space Partitioning) tree.
 */

#ifndef BSPTREE_H
#define BSPTREE_H

#include <iostream> /* NULL */
#include <fstream>

namespace hmdp_base
{

#ifndef BSPTREETYPE_H
#define BSPTREETYPE_H

enum BspTreeType {
  BspTreeT, ContinuousTransitionT, BspTreeAlphaT, ValueFunctionT, 
  ContinuousRewardT, PiecewiseConstantRewardT, PiecewiseLinearRewardT,
  PiecewiseConstantVFT, PiecewiseLinearVFT, ContinuousOutcomeT,
  ContinuousStateDistributionT, BspTreeCSAT
};

#endif

#ifndef PLOTPOINTFORMAT_
#define PLOTPOINTFORMAT_
enum PlotPointFormat {
  GnuplotF, MathematicaF
};
#endif

/**
 * \class BspTree
 * \brief Bsp tree for partitioning the multi-dimensional continuous space.
 */
class BspTree
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  BspTree (const int &dim);

  /**
   * \brief tree node (including root) constructor.
   * @param dim the continuous space dimension.
   * @param d the partitioning dimension.
   * @param pos the position of the partition line.
   */
  BspTree (const int &dim, const int &d, const double &pos);

  /**
   * \brief tree constructor, from geometrical box.
   * @param dim the continuous space dimension.
   * @param lowPos the low coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param highPos the high coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   * @param shift the desired shift per dimension. N/A if NULL.
   */
  BspTree (const int &dim, double *lowPos, double *highPos, 
	   const double *low, const double *high, const double *shift);

  /**
   * \brief copy constructor.
   */
  BspTree (const BspTree &bt);

  /**
   * \brief destructor (NON recursive).
   * \warning this is a NON recursive destructor, i.e. it destroys the current node only.
   */
  virtual ~BspTree ();
  
  /**
   * \brief RECURSIVE destructor, i.e. destroys the full tree.
   * \sa BspTree::~BspTree ().
   */
  static void deleteBspTree (BspTree *bt);

  /**
   * \brief test if the tree is a leaf.
   * @return true if the current tree is a leaf, false otherwise.
   */
  bool isLeaf () const { return ((m_ge == NULL && m_lt == NULL) ? true : false); }

  /**
   * \brief count leaves in a bsp tree.
   * @return number of leaves in the tree.
   */
  int countLeaves () const;
 
  /* accessors */
  /**
   * \brief tree type accessor.
   * @return the type of the bsp tree.
   */
  BspTreeType getType () const { return m_bspType; }

  /**
   * \brief space dimension accessor.
   * @return the dimension of the continuous space that is partitionned by the tree.
   */
  int getSpaceDimension () const { return m_nDim; }

  /**
   * \brief partitioning dimension accessor.
   * @return the dimension over which this node partitions the continuous space.
   */
  int getDimension () const { return m_d; }

  /**
   * \brief partitioning position accessor.
   * @return the position of the partitioning line for this node.
   */
  double getPosition () const { return m_pos; }

  /**
   * \brief lower tree accessor.
   * @return lower tree of this node, NULL if none.
   */
  BspTree* getLowerTree () const { return m_lt; }
  
  /**
   * \brief greater tree accessor.
   * @return greater tree of this node, NULL if none.
   */
  BspTree* getGreaterTree () const { return m_ge; }
  
  /* setters */
  void setDimension (const int &d) { m_d = d; }
  void setPosition (const double &pos) { m_pos = pos; }
  void shiftPosition (const double &shift) { m_pos += shift; }
  void setLowerTree (BspTree *lt) { m_lt = lt; }
  void setGreaterTree (BspTree *ge) { m_ge = ge; }

  /* printing */
  virtual void print (std::ostream &out, double *low, double *high) const;
  void plot1Dbox (std::ofstream &box, double *low, double *high);
  void plot2Dbox (const double &space, std::ofstream &box,
		  double *low, double *high);
  void plot2DboxWithValue(std::ofstream &box, double *low, double *high);
  void plot2DVrml2 (const double &space, std::ofstream &vrml, double *low, double *high,
		    const double &max_value);

  /**
   * \brief outputs tree data points in a given format.
   * @sa BspTree::m_plotPointFormat
   * @param output_values, the file output,
   * @param step the sampling step along each continuous axis,
   * @param low the lower plotting bounds in the continuous space,
   * @param high the upper plotting bounds in the continuous space.
   */
  void plotNDPointValues (std::ofstream &output_values, double *step, 
			  double *low, double *high);

  void plotPointValueMathematica (std::ofstream &output_value, double *pos);
  void plotPointValueGnuplot (std::ofstream &output_value, double *pos);

  double getPointValue (double *pos);
 
 protected:
  void plot2DVrml2R (const double &space, std::ofstream &vrml, 
		     double *low, double *high, const double &scale0, 
		     const double &scale1, const double &max_value);
  
  /* max_value is unused. */
  virtual void plot2DVrml2Leaf (const double &space, std::ofstream &vrml, 
				double *low, double *high, const double &scale0, const double &scale1,
				const double &max_value) {};

  virtual double getPointValueInLeaf (double *pos) { return 0.0; }

 public:
  /* intersection function at initialization */
  virtual void leafDataIntersectInit (const BspTree &bt, const BspTree &btr,
				      double *low, double *high) 
  { std::cout << "BspTree::leafDataIntersectInit not implemented!\n"; };
  
  virtual void leafDataIntersectMax (const BspTree &bt, const BspTree &btr,
				     double *low, double *high) 
  { std::cout << "BspTree::leafDataIntersectMax not implemented!\n"; };
 
  virtual void leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
				      double *low, double *high) 
  { std::cout << "BspTree::leafDataIntersectPlus not implemented!\n"; };

  virtual void leafDataIntersectMinus (const BspTree &bt, const BspTree &btr,
				       double *low, double *high) 
  { std::cout << "BspTree::leafDataIntersectMinus not implemented!\n"; };

  virtual void leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
				      double *low, double *high) 
  { std::cout << "BspTree::leafDataIntersectMult not implemented!\n"; };
  
  virtual void leafDataIntersectCsdDiff (const BspTree &bt, const BspTree &btr,
					 double *low, double *high) 
  { std::cout << "BspTree::leafDataIntersectCsdDiff not implemented!\n"; };
  
  virtual void leafDataIntersectUnion (const BspTree &bt, const BspTree &btr,
				       double *low, double *high)
  { std::cout << "BspTree::leafDataIntersectUnion not implemented!\n"; };

  virtual void leafDataShift (double *shift) { /* default: data are unchanged. */ };

  /**
   * \brief transfer argument tree's data point to the current tree's data.
   * @param bt tree whose data the current tree should point to.
   */
  virtual void transferData (const BspTree &bt) {};

  void fixSpaceDimension(const int &sdim);

 protected:
  BspTreeType m_bspType;  /**< tree type for differenciating during operations */
  int m_nDim;  /**< continuous space dimension */
  short m_d; /**< dividing dimension */
  double m_pos;  /**< dividing position */
  BspTree *m_lt, *m_ge; /**<  sub trees for greater or equal case (ge),
			   and less than case (lt). Terminal nodes point to NULL. */

 public:
  static PlotPointFormat m_plotPointFormat; /**< output format for point value plotting. */

 private:
 
};

} /* end of namespace */
#endif
