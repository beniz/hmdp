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
 * \brief Cover Set vector for Cover Set algorithms in kd-tree leaves.
 */

#ifndef CSVECTOR_H
#define CSVECTOR_H

#include <vector>
#include <ostream>

#ifdef HAVE_LP  /* linear programming */
#include "LpSolve5.h"
#endif

namespace hmdp_csa
{

  class CSVector
  {
  public:
    CSVector(const int &size);
  
    CSVector(const int &size, double cs[]);

    CSVector(const double &val);

    CSVector(const CSVector &csv);

    ~CSVector();

  public:
    //TODO.
    /**
     * \brief union of two sets (stl vectors) of cs vectors.
     * @param vav1 first set,
     * @param vav2 second set,
     * @param low lower bounds for pruning (array of space dimension),
     * @param high upper bounds for pruning (array of space dimension),
     * @param res resulting set of union of cs vector sets.
     */
    static void unionCSVectors(const std::vector<CSVector*> &vav1, 
			       const std::vector<CSVector*> &vav2, 
			       std::vector<CSVector*> *res);
    
    //inclusion test.
    static bool strictInclusionCSVectors(const std::vector<CSVector*> &vav1,
					 const std::vector<CSVector*> &vav2);

    /**
     * \brief sum two sets (stl vectors) of alpha vectors.
     * @param vav1 first set,
     * @param vav2 second set,
     * @param res resulting set of sumed alpha vectors.
     */
    static void simpleSumCSVectors(const std::vector<CSVector*> &vav1, 
				   const std::vector<CSVector*> &vav2, 
				   std::vector<CSVector*> *res);
    
    /**
     * \brief multiply a set of cs vecotrs by a scalar.
     * @param scalar scalar to multiply,
     * @param res set of cs vectors.
     */
    static void multiplyVecByScalar(const double &scalar, 
				    std::vector<CSVector*> *res);
    
    /**
     * \brief remove a cs vector from an stl vector of cs vectors.
     *        Beware: the comparison is on pointer address.
     * @param cs vector pointer to be removed from the stl vector.
     * @param vav stl vector of cs vectors.
     */
    static void removeAvFromVector(CSVector *av, 
				   std::vector<CSVector*> *vav);
    

    static bool addVector(CSVector *csv,
			  std::vector<CSVector*> *vav);
    
    //TODO: might need it.
    /**
     * \brief return the vector with the highest value at the witness point.
     * @param vav vector of cs vectors,
     * @param witness witness point coordinates,
     * @param retv value at the witness point.
     * @return the vector with the highest value at the witness point.
     */
    static CSVector* bestCSVector(const std::vector<CSVector*> &vav, 
				  double *witness, double *retv);
    
    /* member functions */
    
    /**
     * \brief multiply a cs vector by a scalar.
     * @param scalar scalar to multiply.
     */
    void multiplyByScalar(const double &scalar);
    
    /**
     * \brief return the size of a cs vector.
     * @return cs vector size.
     */
    int getSize() const { return m_size; }
    
    /**
     * \brief return a cs vector as an array of double.
     * @return array of cs vector values.
     */
    double* getCS() const { return m_cs; }
    
    /**
     * \brief return an alpha vector element.
     * @param i element position,
     * @return i-eme element of this alpha vector.
     */
    double getCSNth(const int &i) const { return m_cs[i]; }
    
    /**
     * \brief set an element value to this cs vector.
     * @param i element position,
     * @param val new element value.
     */
    void setCSNth(const int &i, const double &val) { m_cs[i] = val; }
    
    double getCSExpNth(const int &i) const { return m_exp[i]; }

    void setCSExpNth(const int &i, const double &val) { m_exp[i] = val; }
    
    bool isEqual (const CSVector &av) const;
    
    static bool isEqualEpsilon(CSVector *csv1,
			       CSVector *csv2, const double &epsilon);

    static bool isEqualEpsilon(std::vector<CSVector*> *vcs1,
			       std::vector<CSVector*> *vcs2,
			       const double &epsilon);

    bool isZero() const;
    
  private:
    int m_size;  /**< cs vector size. */
    double *m_cs;  /**< cover set vector (hyperplane). */
    double *m_exp; /**< local expectations for this policy in the cs. */
  };
  
  std::ostream &operator<<(std::ostream &output, CSVector &av);
  std::ostream &operator<<(std::ostream &output, const std::vector<CSVector*> &vav);

}  /* end of namespace. */

#endif
