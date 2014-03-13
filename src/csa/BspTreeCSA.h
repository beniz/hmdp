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
 * \brief Bsp-tree for Cover Set Algorithm: CSA is the leaves, that partition the 
 *        continuous resource dimensions.
 */

#ifndef BSPTREECSA_H
#define BSPTREECSA_H

#include "BspTree.h"
#include "CSVector.h"
#include "ValueFunction.h"
#include "ContinuousStateDistribution.h"
#include <vector>

using namespace hmdp_base;

namespace hmdp_csa
{

class BspTreeCSA : public BspTree
{
 public:
  /**
   * \brief (leaf) constructor.
   * @param sdim the continuous space dimension.
   */
  BspTreeCSA(const int &sdim);

  /**
   * \brief constructor.
   * @param sdim the continuous space dimension,
   * @param d the partitioning dimension,
   * @param pos the position of the partition line.
   */
  BspTreeCSA(const int &sdim, const int &d, const double &pos);

  /**
   * \brief tree constructor, from geometrical box.
   * @param dim the continuous space dimension.
   * @param lowPos the low coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param highPos the high coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   */
  BspTreeCSA(const int &sdim, double *lowPos, double *highPos,
	     const double *low, const double *high);

  /**
   * \brief copy constructor.
   */
  BspTreeCSA(const BspTreeCSA &bta);

 protected:
  /**
   * \brief copy constructor from another tree type.
   */
  BspTreeCSA(const BspTree &bt);

 public:
  /**
   * \brief initial constructor, from a value function.
   */
  BspTreeCSA(const ValueFunction &vf, 
	     const int &covdim, const int &coeff_dim,
	     double *low, double *high);

  /**
   * \brief initial constructor, from a csd.
   */
  //BspTreeCSA(const ContinuousStateDistribution &csd,
  //     const int &covdim, const int &coeff_dim);

  /**
   * \brief creates a BspTreeCSA tree initialized to 0.
   */
  BspTreeCSA(const int &sdim, const int &covdim,
	     const double *low, const double *high);
  
  /**
   * \brief non-recursive destructor, i.e. it destroys the current node only.
   */
  ~BspTreeCSA();

  public:
  /**
   * \brief accessor to the set of cs vectors (in this leaf).
   * @return STL vector of cs vectors.
   * @sa CSVector
   */
  std::vector<CSVector*>* getCSVectors () const { return m_csVectors; }

  void setCSVectors(std::vector<CSVector*> *csv) { m_csVectors = csv; }

  /**
   * \brief accessor to the number of cs vectors (in this leaf).
   * @return size of the set of cs vectors.
   * @sa CSVector
   */
  unsigned int getCSVectorsSize() const { return m_csVectors->size (); }
  
  /**
   * \brief accessor to a cs vector (in this leaf).
   * @param i vector index.
   * @return the cs vector at i.
   * @sa CSVector
   */
  CSVector* getCSVectorNth(int i) const { return (*m_csVectors)[i]; }

  std::vector<std::vector<double>* >* getPoints() const { return m_cs_points; }
  
  size_t getPointsSize() const { return m_cs_points->size(); }

  void setPoints(std::vector<std::vector<double>*> *pts)
  {
    if (pts != m_cs_points)
      m_cs_points = pts;
  }

  std::vector<double>* getPoint(const int &i) const { return (*m_cs_points)[i]; }

  int getNCSPoints() const { return m_n_cs_points; }

 protected:
  void addCSVector(CSVector *av)
  {
    if (! m_csVectors)
      m_csVectors = new std::vector<CSVector*> ();
    m_csVectors->push_back(av);
  }

  /* unused. */
  void deleteCSVectorNth(int i) const 
  {  /* TODO */  }

  /* virtual */
 public:
  //TODO: do we need it ??
  /**
   * \brief add a scalar to the alpha vector values in tree leaves.
   * @param scalar a scalar.
   */
  //virtual void addScalar(const double &scalar);  /* virtual because does not apply *as is* to the linear case */
  
  /**
   * \brief Recursively multiply alpha vector values in tree leaves by a scalar.
   * @param scalar a scalar.
   */
  void multiplyByScalar (const double &scalar);

  protected:
  /**
   * \brief Default data intersection for cs vectors.
   *
   *  This function is used at initialization, and does not perform
   *  intersection of the vectors among discrete pieces: it selects the non-null
   *  vector from one piece (tree) or the other.
   */
  void leafDataIntersectInit (const BspTree &bt, const BspTree &btr, 
			      double *low, double *high);

  //TODO: ?.
  //void leafDataIntersectMult (const BspTree &bt, const BspTree &btr,
  //		      double *low, double *high);
  
  void leafDataIntersectPlus (const BspTree &bt, const BspTree &btr,
			      double *low, double *high);
  
  void leafDataIntersectUnion(const BspTree &bt, const BspTree &btr,
			      double *low, double *high);
  
  void leafDataIntersectMinus(const BspTree &bt, const BspTree &btr,
			      double *low, double *high);
  
  void transferData (const BspTree &bt);

  void rescaleCoeffs(double *low, double *high);

 public:
  void hasPlans(bool &hp);

 protected:
  void addAVector(CSVector *av)
  {
    if (! m_csVectors)
      m_csVectors = new std::vector<CSVector*> ();
    m_csVectors->push_back (av);
  }

  void mergeTreeLeaves(double *low, double *high);

  void mergeContiguousLeaves(BspTreeCSA *root,
			     BspTreeCSA *lt, BspTreeCSA *ge,
			     double *low, double *high);

 public:
  void pruneUnreachablePlans(const size_t &covdim);

  void pruneDominatedPlans(double *lowpb, double *highpb);

  void collectPoints(std::vector<std::vector<double>*> &pts);

  int countPlanes() const;

  void countNonZeroCSVectors(int &nz) const;

  /* static */
 public:
  static BspTreeCSA* sumBspTreeCSAs(BspTreeCSA *csa, BspTreeCSA *csb,
				    double *low, double *high);
  
  static BspTreeCSA* unionBspTreeCSAs(BspTreeCSA *csa, BspTreeCSA *csb,
				      double *low, double *high);

  static bool hasPoint(std::vector<std::vector<double>*> &pts,
		       std::vector<double> &pt);

  /* printing. */
  double getPointValueInLeaf(double *pos);

  void print(std::ostream &out, double *low, double *high);

  void plot1D(std::ofstream &box,
	      double *low, double *high);

 protected:
  std::vector<CSVector*>* m_csVectors;
  std::vector<std::vector<double>* >* m_cs_points; /**< local current cover set points. */
  
 public:
  //stats
  static int m_prunedPlans;
  int m_n_cs_points;
};

}  /* end of namespace. */

#endif
