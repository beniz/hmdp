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

#ifndef ALPHAVECTOR_H
#define ALPHAVECTOR_H

#include "config.h"
#include <vector>
#include <set>
#include <ostream>

#ifdef HAVE_LP  /* linear programming */
#include "LpSolve5.h"
#endif

namespace hmdp_base
{

/**
 * \class AlphaVector
 * \brief alpha vector
 */
class AlphaVector
{
 public:
  /**
   * \brief constructor
   * @param size vector size
   */
  AlphaVector (const int &size);

  /**
   * \brief constructor
   * @param size vector size (equals continuous space dimension + 1).
   * @param alph array of vector factors in each dimension.
   */
  AlphaVector (const int &size, double alph[]);
  
  /**
   * \brief constructor of an alpha-vector of size 1 (constant).
   * @param val vector constant value.
   */
  AlphaVector (const double &val);

  /**
   * \brief copy constructor.
   */
  AlphaVector (const AlphaVector &av);

  /**
   * \brief destructor.
   */
  ~AlphaVector ();

 protected:
 private:

 public:
  /**
   * \brief sum two sets (stl vectors) of alpha vectors.
   * @param vav1 first set,
   * @param vav2 second set,
   * @param res resulting set of sumed alpha vectors.
   */
  static void simpleSumAlphaVectors (const std::vector<AlphaVector*> &vav1, 
				     const std::vector<AlphaVector*> &vav2, 
				     std::vector<AlphaVector*> *res);
  
  /**
   * \brief subtract two sets (stl vectors) of alpha vectors.
   * @param vav1 first set,
   * @param vav2 second set,
   * @param res resulting set of subtracted alpha vectors.
   */
  static void simpleSubtractAlphaVectors (const std::vector<AlphaVector*> &vav1, 
					  const std::vector<AlphaVector*> &vav2, 
					  std::vector<AlphaVector*> *res);
  
  /**
   * \brief cross-sum of two sets (stl vectors) of alpha vectors.
   * @param vav1 first set,
   * @param vav2 second set,
   * @param low lower bounds for pruning (array of space dimension),
   * @param high upper bounds for pruning (array of space dimension),
   * @param res resulting set of cross-sumed alpha vectors.
   */
  static void crossSumAlphaVectors (const std::vector<AlphaVector*> &vav1, 
				    const std::vector<AlphaVector*> &vav2, 
				    double *low, double *high, 
				    std::vector<AlphaVector*> *res);
  
   /**
   * \brief cross-subtract of two sets (stl vectors) of alpha vectors.
   * @param vav1 first set,
   * @param vav2 second set,
   * @param low lower bounds for pruning (array of space dimension),
   * @param high upper bounds for pruning (array of space dimension),
   * @param res resulting set of cross-subtracted alpha vectors.
   */
  static void crossSubtractAlphaVectors (const std::vector<AlphaVector*> &vav1, 
					 const std::vector<AlphaVector*> &vav2, 
					 double *low, double *high, 
					 std::vector<AlphaVector*> *res);

  /**
   * \brief max operator over two sets of alpha vectors of dimension > 1.
   * @param vav1 first set,
   * @param vav2 second set,
   * @param low lower bounds for pruning (array of space dimension),
   * @param high upper bounds for pruning (array of space dimension),
   * @param res resulting set of max alpha vectors.
   */
  static void maxLinearAlphaVector (const std::vector<AlphaVector*> &vav1, 
				    const std::vector<AlphaVector*> &vav2, 
				    double *low, double *high, 
				    std::vector<AlphaVector*> *res);
  
  /**
   * \brief max operator over two sets of alpha vectors of dimension 1.
   * @param vav1 first set,
   * @param vav2 second set,
   * @param ag1 pointer to achieved goals for vector 1 (for tie breaking),
   * @param ag2 pointer to achieved goals for vector 2 (for tie breaking),
   * @param res resulting set of max alpha vectors.
   */
  static void maxConstantAlphaVector (const std::vector<AlphaVector*> &vav1, 
				      const std::vector<AlphaVector*> &vav2,
				      std::vector<int> *ag1,
				      std::vector<int> *ag2,
				      std::vector<AlphaVector*> *res);

  
  static void minLinearAlphaVector (const std::vector<AlphaVector*> &vav1, 
				    const std::vector<AlphaVector*> &vav2, 
				    double *low, double *high,
				    std::vector<AlphaVector*> *res); /* TODO */
  
  /**
   * \brief min operator over two sets of alpha vectors of dimension 1.
   * @param vav1 first set,
   * @param vav2 second set,
   * @param res resulting set of min alpha vectors.
   */
  static void minConstantAlphaVector (const std::vector<AlphaVector*> &vav1, 
				      const std::vector<AlphaVector*> &vav2, 
				      std::vector<AlphaVector*> *res);

  /**
   * \brief add a scalar to a set of alpha vectors.
   * @param scalar scalar to be added,
   * @param res set of alpha vectors.
   */
  static void addVecScalar (const double &scalar, std::vector<AlphaVector*> *res);
  
  /**
   * \brief multiply a set of alpha vectors by a scalar.
   * @param scalar scalar to multiply,
   * @param res set of alpha vectors.
   */
  static void multiplyVecByScalar (const double &scalar, std::vector<AlphaVector*> *res);
  
  /**
   * \brief remove an alpha vector from an stl vector of alpha vectors.
   *        Beware: the comparison is by pointer address.
   * @param alpha vector pointer to be removed from the stl vector.
   * @param vav stl vector of alpha vectors.
   */
  static void removeAvFromVector (AlphaVector *av, std::vector<AlphaVector*> *vav);

  static bool isVecEqual (const std::vector<AlphaVector*> &vav1,
			  const std::vector<AlphaVector*> &vav2);

  /**
   * \brief return the vector with the highest value at the witness point.
   * @param vav vector of alpha vectors,
   * @param witness witness point coordinates,
   * @param retv value at the witness point.
   * @return the vector with the highest value at the witness point.
   */
  static AlphaVector* bestAlphaVector (const std::vector<AlphaVector*> &vav, double *witness, double *retv);
  
  
  /* member functions */
  /**
   * \brief add a scalar to an alpha vector.
   * @param scalar scalar to be added.
   */
  void addScalar (const double &scalar);

  /**
   * \brief multiply an alpha vector by a scalar.
   * @param scalar scalar to multiply.
   */
  void multiplyByScalar (const double &scalar);

  /**
   * \brief return the size of an alpha vector.
   * @return alpha vector size.
   */
  int getSize () const { return m_size; }

  /**
   * \brief return an alpha vectora as an array of double.
   * @return array of alpha vector values.
   */
  double* getAlpha () const { return m_alpha; }

  /**
   * \brief return an alpha vector element.
   * @param i element position,
   * @return i-eme element of this alpha vector.
   */
  double getAlphaNth (const int &i) const { return m_alpha[i]; }

  /* deprecated */
  /**
   * \brief return action index that is attached to this alpha vector.
   * @return action index.
   */
  //short getAction () const { return m_action; }

  size_t getActionsSize () const { return m_actions.size (); }

  std::set<int>::iterator getActionsBegin () { return m_actions.begin (); }
  
  std::set<int>::iterator getActionsEnd () { return m_actions.end (); }

  /**
   * \brief set an element value to this alpha vector.
   * @param i element position,
   * @param val new element value.
   */
  void setAlphaNth (const int &i, const double &val) { m_alpha[i] = val; }

  /**
   * \brief set an action index to this alpha vector.
   * @param a action index.
   */
  void setAction (const int &a) { clearActions (); m_actions.insert (a); }

  void addAction (const int &a) { m_actions.insert (a); }
  
  void removeAction (const int &a);

  void clearActions () { m_actions.clear (); }

  void setActions (const std::set<int> &actions) { clearActions (); m_actions = std::set<int> (actions); }

  bool isEqual (const AlphaVector &av);

  static bool isEqualVecGoals (const std::vector<int> &g1, const std::vector<int> &g2);

  static bool isEqualActionSets (const std::set<int> &a1, const std::set<int> &a2);

  bool isZero ();

  bool isZeroOrBelow ();

 private:
  int m_size;   /**< alpha vector size. */
  double *m_alpha;  /**< alpha vector elements as an array of double */

 public:
  std::set<int> m_actions;  /**< set of action indexes attached to this alpha vector. */
};

 std::ostream &operator<<(std::ostream &output, AlphaVector &av);
 std::ostream &operator<<(std::ostream &output, const std::vector<AlphaVector*> &vav);

} /* end of namespace */
#endif
