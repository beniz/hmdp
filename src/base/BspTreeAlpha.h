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

#ifndef BSPTREEALPHA_H
#define BSPTREEALPHA_H

#include "BspTree.h"
#include "AlphaVector.h"
#include "BspTreeOperations.h"
#include <vector>
#include <map>

namespace hmdp_base
{

/**
 * \class BspTreeAlpha
 * \brief bsp tree whose leaves are alpha vectors, for the representation of
 *        value functions and rewards.
 */
class BspTreeAlpha : public BspTree
{
 public:
  /**
   * \brief (leaf) constructor.
   * @param sdim the continuous space dimension.
   */
  BspTreeAlpha (const int &sdim);

  /**
   * \brief constructor.
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the position of the partition line.
   */
  BspTreeAlpha (const int &sdim, const int &d, const double &pos);

  /**
   * \brief tree constructor, from geometrical box.
   * @param dim the continuous space dimension.
   * @param lowPos the low coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param highPos the high coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   */
  BspTreeAlpha (const int &sdim, double *lowPos, double *highPos,
		const double *low, const double *high);

  /**
   * \brief copy constructor.
   */
  BspTreeAlpha (const BspTreeAlpha &bta);

 protected:
  /**
   * \brief copy constructor from another tree type.
   */
  BspTreeAlpha (const BspTree &bt);

  /**
   * \brief non-recursive destructor, i.e. it destroys the current node only.
   */
  ~BspTreeAlpha ();

 public:
  /**
   * \brief accessor to the set of alpha vectors (in this leaf).
   * @return STL vector of alpha vectors.
   * @sa AlphaVector
   */
  std::vector<AlphaVector*>* getAlphaVectors () const { return m_alphaVectors; }

  /**
   * \brief accessor to the number of alpha vectors (in this leaf).
   * @return size of the set of alpha vectors.
   * @sa AlphaVector
   */
  unsigned int getAlphaVectorsSize () const { return m_alphaVectors->size (); }
  
  /**
   * \brief accessor to an alpha vector (in this leaf).
   * @param i vector index.
   * @return the alpha vector at i.
   * @sa AlphaVector
   */
  AlphaVector* getAlphaVectorNth (int i) const { return (*m_alphaVectors)[i]; }

 protected:
  void addAVector (AlphaVector *av)
  {
    if (! m_alphaVectors)
      m_alphaVectors = new std::vector<AlphaVector*> ();
    m_alphaVectors->push_back (av);
  }

  /* unused. */
  void deleteAVectorNth (int i) const 
  {  /* TODO */  }

  /* virtual */
 public:
  /**
   * \brief add a scalar to the alpha vector values in tree leaves.
   * @param scalar a scalar.
   */
  virtual void addScalar (const double &scalar);  /* virtual because does not apply *as is* to the 
						     linear case */
  /**
   * \brief Recursively multiply alpha vector values in tree leaves by a scalar.
   * @param scalar a scalar.
   */
  void multiplyByScalar (const double &scalar);

  bool isZero ();

  bool isZeroOrBelow ();

  /* actions coverage. */
  void getActionsCoverage (std::map<int, double> *coverage,
			   double *low, double *high);

  void leafCoverage (std::map<int, double> *coverage,
		     double *low, double *high);

  /* printing */
  void print (std::ostream &output_values, double *low, double *high);

  void plotNDPointAction (std::ofstream &out, double *step,
			  double *low, double *high);

  int getPointAction (double *pos);
  
  void plotPointActionMathematica (std::ofstream &output_value, double *pos);
  
  void plotPointActionGnuplot (std::ofstream &output_value, double *pos);

  virtual int getPointActionInLeaf (double *pos) { return 0;};

 protected:
  /**
   * \brief Default data intersection for alpha vectors.
   *
   *  This function is used at initialization, and does not perform
   *  intersection of the vectors among discrete pieces: it selects the non-null
   *  vector from one piece (tree) or the other.
   */
  void leafDataIntersectInit (const BspTree &bt, const BspTree &btr, 
			      double *low, double *high);

  void leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);

  void transferData (const BspTree &bt);

 public:
  double getSubTreeMaxValue () const { return m_maxValue; }

  void setSubTreeMaxValue (const double& mv) { m_maxValue = mv; }

  virtual double updateSubTreeMaxValue () { return 0.0; };

  void setLeavesValue (const double &val, double *low, double *high);

 protected:
  std::vector<AlphaVector*>* m_alphaVectors;  /**< alpha vectors in tree leaves */
  
  double m_maxValue; /**< max value in the subtree from this node, 
			  used in asymetric operators (constant case only). */

 private:
};

} /* end of namespace */
#endif
