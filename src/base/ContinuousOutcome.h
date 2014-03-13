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

#ifndef CONTINUOUSOUTCOME_H
#define CONTINUOUSOUTCOME_H

#include "BspTree.h"

#include "MDDiscreteDistribution.h"

namespace hmdp_base
{

class OutcomeOutOfBoundsException
{
 public:
  OutcomeOutOfBoundsException ()
    : m_message ("outcome discarded: out of bounds")
    {}
    
    const char *what () const { return m_message; }

 private:
  const char *m_message;
};

/**
 * \class ContinuousOutcome
 * \brief bsp tree for multi-dimensional discrete probability distribution.
 */
class ContinuousOutcome : public BspTree 
{
 public:
  /**
   * \brief tree leaf constructor.
   * @param dim the continuous space dimension.
   */
  ContinuousOutcome (int dim);

  /**
   * \brief tree node (including root) constructor.
   * @param dim the continuous space dimension.
   * @param d the partitioning dimension.
   * @param pos the position of the partition line.
   */
  ContinuousOutcome (int dim, int d, double pos);

   /**
   * \brief tree node from geometrical box.
   * @param dim the continuous space dimension.
   * @param lowPos the low coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param highPos the high coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param low the domain lower bounds.
   * @param high the domain upper bounds.
   * @param shift the desired shift per dimension, N/A if NULL.
   * @param prob box probability,
   * @param relative boolean flag in each dimension (dim) if the continuous outcome is relative or not
                     in that dimension.
   */
  ContinuousOutcome (int dim, double *lowPos, double *highPos, 
		     const double *low, const double *high,
		     const double *shift, const double &prob, const bool *relative);

  /**
   * \brief copy constructor.
   * @param co continuous outcome to be copied.
   */
  ContinuousOutcome (const ContinuousOutcome &co);

 protected:
  ContinuousOutcome (int dim, const BspTree &bt);

 public:
  ~ContinuousOutcome ();

  /* virtual */
  void transferData (const BspTree &bt);

  /* static */
  /**
   * \brief convert a discrete point from a multi-dimensional discrete distribution
   *        into a continuous outcome (bsp tree).
   * @param dim continuous space dimension,
   * @param lowPos the low coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param highPos the high coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param point the discrete multi-dimensional point to be converted,
   * @param relative boolean flag in each dimension (dim) if the continuous outcome is relative or not
                     in that dimension.
   */
  static ContinuousOutcome* convertDiscretePoint (const int &dim, double *lowPos, double *highPos,
						  const double *low, const double *high,
						  const std::pair<double, double*> &point,
						  const bool *relative);

  static  ContinuousOutcome* convertOppositeDiscretePoint (const int &dim, double *lowPos, double *highPos,
							   const double *low, const double *high,
							   const std::pair<double, double*> &point,
							   const bool *relative);
  
  /**
   * \brief convert a multi-dimensional discrete probability distribution into 
   *        a continuous outcome (bsp tree).
   * @param lowPos the low coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param highPos the high coordinates of the corner points of a hyper-rectangle ('dim' points).
   * @param mdd multi-dimensional discrete probability distribution to be converted,
   * @param relative boolean flag in each dimension (dim) if the continuous outcome is relative or 
   *        not in that dimension,
   * @param noutcomes final number of outcomes (some can be discarded), by reference.
   */
  static ContinuousOutcome** convertMDDiscreteDistribution (double *lowPos, double *highPos,
							    const double *low, const double *high,
							    const MDDiscreteDistribution &mdd,
							    const bool *relative, int &noutcomes);

  static ContinuousOutcome** convertOppositeMDDiscreteDistribution (double *lowPos, double *highPos,
								    const double *low, const double *high,
								    const MDDiscreteDistribution &mdd,
								    const bool *relative, int &noutcomes);

  /* setters */
  void setProbability (const double &prob) { m_leafProbability = prob; }

  /* accessors */
  double getProbability () const { return m_leafProbability; }
  double* getShiftBack () const { return m_shiftBack; }
  double getShiftBack (const int &i) const { return m_shiftBack[i]; }
  double* getLowPos () const { return m_lowPos; }
  double getLowPos (const int &i) const { return m_lowPos[i]; }
  double* getHighPos () const { return m_highPos; }
  double getHighPos (const int &i) const { return m_highPos[i]; }
  
  /* printing */
  void print (std::ostream &out, double *low, double *high);

 private:
  double m_leafProbability; /**< probability attached to a leaf (-1 otherwise). */
  double *m_shiftBack;  /**< shift required to return to the original 
			   (transition tile) position. Array size = continuous space dimension. */
  double *m_lowPos; /**< low coordinates of this shifted rectangle (encoded as this tree). */
  double *m_highPos; /**< high coordinates. */
};

} /* end of namespace */
#endif
