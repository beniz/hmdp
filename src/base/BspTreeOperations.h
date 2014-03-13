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
 * \brief class of static algorithms for operating with bsp trees of
 *        different types.
 */

#ifndef BSPTREEOPERATIONS_H
#define BSPTREEOPERATIONS_H

#include "Alg.h"
#include "BspTree.h"

namespace hmdp_base
{

#ifndef BSPTREEINTERSECTIONTYPE_H
#define BSPTREEINTERSECTIONTYPE_H

enum BspTreeIntersectionType {
  BTI_INIT, BTI_MAX, BTI_MIN, BTI_PLUS, BTI_MINUS, BTI_MULT, BTI_CSD_DIFF, BTI_UNION
};

#endif

/**
 * \class BspTreeOperations
 * \brief static algorithms of operating with bsp trees of different types.
 */
class BspTreeOperations : public Alg
{
 public:
  /**
   * /brief create bsp tree according to current output type.
   * @param dim continuous space dimension,
   * @return newly created bsp tree.
   * @sa BspTreeOperations::m_currentOuputType.
   */
  static BspTree* createTree (const int &dim);

  /**
   * \brief create bsp tree according to current output type.
   * @param dim continuous space dimension,
   * @param d partitioning dimension,
   * @param pos partitioning position,
   * @return newly created bsp tree.
   * @sa BspTreeOperations::m_currentOutputType.
   */
  static BspTree* createTree (const int &dim, const int &d, const double &pos);
     
  static BspTree* createTree (const int &dim, const int &d, const double &pos,
			      const double &max_value);

  /**
   * \brief create bsp tree according to current output type.
   * @param bt bsp tree to be copied.
   * @return newly created bsp tree.
   * @sa BspTreeOperations::m_currentOutputType.
   */
  static BspTree* createTree (BspTree *bt);
  
  /**
   * \brief copy bsp tree. Output tree is of the same type as the input tree.
   * @param bt bsp tree to be copied.
   * @return newly created bsp tree of same type as bt.
   */
  static BspTree* copyTree (BspTree *bt);

 private:
  static BspTreeType lookupOutputTypeTable (BspTreeType btt1, BspTreeType btt2);

 public:
  /**                                                  
   * \brief bsp trees intersection, intersects bt1 with bt2.                                    
   * @param bt1 tree to be intersected,                                                         
   * @param bt2 tree to be intersected,                                                       
   * @param low array of domain lower bounds, of continuous space dimension.                 
   * @param high array of domain upper bounds, of continuous space dimension.                  
   * @return create a new tree, where bt1 and bt2 are intersected.                               
   */
  static BspTree* intersectTrees (BspTree *bt1, BspTree *bt2,
				  double *low, double *high);
  
 private:
  /**                                                                                        
   * \brief partition a bsp tree of dimension d at position pos,                        
   * assuming the partitioning planes are in the same region.                          
   * @param bt tree to be partitioned (result is in a new tree).                       
   * @param d dimension of the partitioning plane.                                          
   * @param pos position of the partition.                                                     
   */
  static BspTree* partition (const BspTree &bt, const int &d, const double &pos,
			     double *low, double *high);
     
  static BspTree* intersectLowerHalf (const BspTree &btr, const int &d, const double &pos,
				      double *low, double *high);
  static BspTree* intersectGreaterHalf (const BspTree &btr, const int &d, const double &pos,
					double *low, double *high);
  
  static BspTree* intersectWithCell (BspTree *btr, BspTree *bt, 
				     double *low, double *high);

 public:
  /**
   * \brief shift a tree in all dimensions.
   * @param bt tree to be shifted.
   * @param shift array of the shifting factor (size = continuous space dimension).
   * @param low array of domain lower bounds, of continuous space dimension.                 
   * @param high array of domain upper bounds, of continuous space dimension.  
   */
  static BspTree* shiftTree (BspTree *bt, double *shift,
			     double *low, double *high);
 
  /**
   * \brief 
   * @warning the resulting tree is not optimal in the number of branches and leaves !
   */
  static BspTree* cropTree (BspTree *bt, double *low, double *high);

  static void setSubTreeMaxValue (BspTree *bt, BspTree *btmv);

  static void setSubTreeMaxValue (BspTree *bt, const BspTree &btmv);

  static void swapDimensions(BspTree *bt, int swapTable[]);

 public:
  /* setters */
  /**
   * \brief set intersection type
   * @param btit intersection type.
   */
  static void setIntersectionType (BspTreeIntersectionType btit) 
    { BspTreeOperations::m_currentIntersectionType = btit; }

  /**
   * \brief activate the merging of pieces within the bsp tree.
   * @param merging boolean flag for merging/not merging pieces.
   */
  static void setPiecesMerging (const bool &merging) 
    { BspTreeOperations::m_piecesMerging = merging; }

 protected:
  static BspTreeType m_currentOutputType; /**< bsp tree output type */
  static BspTreeType m_outputTypeTable[12][3];
  static int m_outputTypeTableSize;
  
 public:
  /* user options */
  static bool m_bspBalance;  /**< tree balancing flag */
  static bool m_asymetricOperators; /**< whether we're using asymetric min/max (default no) */
  
 public:
  static bool m_piecesMerging;  /**< whether we're merging the pieces or not (default no) */
  static bool m_piecesMergingByValue;  /**< force merge pieces based on their attached values. */
  static bool m_piecesMergingByAction;  /**< force merge pieces based on their attached actions. */
  static bool m_piecesMergingEquality;  /**< merge pieces if they're equal only: same achieved goals,
					   same actions, same value. */
  
 protected:
  static BspTreeIntersectionType m_currentIntersectionType;  /**< intersection type */
};

} /* end of namespace */
#endif
