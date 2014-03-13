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

#include "CSVector.h"
#include <assert.h>
#include "Alg.h"

using namespace hmdp_base;

namespace hmdp_csa
{

CSVector::CSVector (const int &size)
  : m_size(size)
{
  m_cs = new double[m_size];
  for (int i=0; i<m_size; i++)
    m_cs[i] = 0.0;
  m_exp = new double[m_size];
  for (int i=0; i<m_size; i++)
    m_exp[i] = 0.0;
}

CSVector::CSVector (const int &size, double cs[])
  : m_size (size)
{
  m_cs = new double[m_size];
  for (int i=0; i<m_size; i++)
    m_cs[i] = cs[i];

  //beware: exp should get values too...
  m_exp = new double[m_size];
  for (int i=0; i<m_size; i++)
    m_exp[i] = 0.0;
}

CSVector::CSVector (const double &val)
  : m_size(1), m_exp(0)
{
  m_cs = new double[m_size];
  m_cs[0] = val;

  //beware: exp is NULL.
}

CSVector::CSVector (const CSVector &csv)
  : m_size(csv.getSize())
{
  m_cs = new double[m_size];
  for (int i=0; i<m_size; i++)
    m_cs[i] = csv.getCSNth(i);
  
  m_exp = new double[m_size];
  for (int i=0; i<m_size; i++)
    m_exp[i] = csv.getCSExpNth(i);
}

CSVector::~CSVector()
{
  delete[] m_cs;
  if (m_exp)
    delete[] m_exp;
}

bool CSVector::addVector(CSVector *csv,
			 std::vector<CSVector*> *vav)
{
  for (size_t i=0; i<vav->size(); i++)
    if (csv->isEqual(*(*vav)[i]))
      return false;
  vav->push_back(csv);
  return true;
}

void CSVector::unionCSVectors (const std::vector<CSVector*> &vav1, 
			       const std::vector<CSVector*> &vav2,
			       std::vector<CSVector*> *res)
{
  for (size_t i=0; i<vav1.size(); i++)
    {
      CSVector *csv = new CSVector(*vav1[i]);
      if (!CSVector::addVector(csv,res))
	delete csv;
    }
  for (size_t i=0; i<vav2.size(); i++)
    {
      CSVector *csv = new CSVector(*vav2[i]);
      if (!CSVector::addVector(csv,res))
	delete csv;
    }
}

bool CSVector::strictInclusionCSVectors(const std::vector<CSVector*> &vav1, 
					const std::vector<CSVector*> &vav2)
{
  if (vav1.size() == vav2.size())
    return false;
  else if (vav1.size() > vav2.size())
    {
      for (size_t i=0;i<vav2.size();i++)
	{
	  bool invav1 = false;
	  for (size_t j=0;j<vav1.size();j++)
	    {
	      if (vav2[i]->isEqual(*vav1[j]))
		{
		  invav1 = true;
		  break;
		}
	    }
	  if (!invav1)
	    return false;
	}
      return true;
    }
  else if (vav2.size() > vav1.size())
    {
      for (size_t i=0;i<vav1.size();i++)
	{
	  bool invav2 = false;
	  for (size_t j=0;j<vav2.size();j++)
	    {
	      if (vav1[i]->isEqual(*vav2[j]))
		{
		  invav2 = true;
		  break;
		}
	    }
	  if (!invav2)
	    return false;
	}
      return true;
    }
  else return true;
}

void CSVector::simpleSumCSVectors (const std::vector<CSVector*> &vav1, 
				   const std::vector<CSVector*> &vav2, 
				   std::vector<CSVector*> *res)
{
  CSVector *av1 = vav1[0], *av2 = vav2[0];
  CSVector *avres = new CSVector(av1->getSize());
  for (int i=0; i<av1->getSize(); i++)
    avres->setCSNth(i, av1->getCSNth(i) + av2->getCSNth(i));
  for (int i=0; i<av1->getSize(); i++)
    avres->setCSExpNth(i, av1->getCSExpNth(i) + av2->getCSExpNth(i));
  res->push_back(avres);
}

void CSVector::multiplyVecByScalar(const double &scalar, 
				   std::vector<CSVector*> *res)
{
  for (unsigned int i=0; i<res->size (); i++)
    (*res)[i]->multiplyByScalar (scalar);
}

void CSVector::multiplyByScalar (const double &scalar)
{
  for (int i=0; i<m_size; i++)
    m_cs[i] *= scalar;
}

void CSVector::removeAvFromVector(CSVector *av, std::vector<CSVector*> *vav)
{
  std::vector<CSVector*>::iterator it = vav->begin ();
  while (it != vav->end ())
    {
      if (av == (*it))
	{
	  vav->erase (it);
	  return;
	}
      it++;
    }
}

bool CSVector::isZero() const
{
  for (int i=0; i<m_size; i++)
    if (m_cs[i] != 0.0)
      return false;
  return true;
}

bool CSVector::isEqual (const CSVector &av) const
{
  if (av.getSize () != m_size)
    return false;
  for (int i=0; i<m_size; i++)
    if (m_cs[i] != av.getCSNth (i))
      return false;
  return true;
}

bool CSVector::isEqualEpsilon(CSVector *csv1,
			      CSVector *csv2,
			      const double &epsilon)
{
  assert(csv1->getSize() == csv2->getSize());
  
  for (int i=0; i<csv1->getSize(); i++)
    {
      if (!Alg::REqual(csv1->getCSNth(i),csv2->getCSNth(i),epsilon))
	return false;
    }
  return true;
}

bool CSVector::isEqualEpsilon(std::vector<CSVector*> *vcs1,
			      std::vector<CSVector*> *vcs2,
			      const double &epsilon)
{
  if (vcs1->size() != vcs2->size())
    return false;
  
  for (size_t i=0; i<vcs1->size(); i++)
    {
      int isVecEqual = false;
      for (size_t j=0; j<vcs2->size(); j++)
	{
	  isVecEqual = isVecEqual 
	    || CSVector::isEqualEpsilon((*vcs1)[i],(*vcs2)[j],epsilon);
	}
      if (!isVecEqual)
	return false;
    }
  return true;
}

CSVector* CSVector::bestCSVector(const std::vector<CSVector*> &csv,
				 double *witness, double *retv)
{
  double v = 0.0;
  *retv = -100000000.0;   /* TODO: constant. */
  CSVector *bestAv = 0;

  for (unsigned int i=0; i<csv.size (); i++)
    {
      v = csv[i]->getCSNth(csv[i]->getSize()-1);  /* constant */
      for (int j=0; j<csv[i]->getSize()-1; j++)
	v += (csv[i]->getCSNth(j) * witness[j]);
      
      //debug
      //std::cout << "vec: " << *csv[i] << " -- v: " << v << " -- ";
      //debug

      if (i ==0 || v >= *retv)
 	{
	  bestAv = csv[i];
	  *retv = v;
	}
      /* lexigraphical dominance. */
      /* else if (v == *retv) 
	{
	  for (int k=0; k<csv[i]->getSize ()-1; k++)
	    if (csv[i]->getCSNth (k) > bestAv->getCSNth (k))
	      {
		bestAv = csv[i];
		break;
	      }
	      } */
    }
  return bestAv;
}

std::ostream &operator<<(std::ostream &output, CSVector &av)
{
  output << "cs: { ";
  for (int i=0; i<av.getSize(); i++)
    {
      output << av.getCSNth(i) << " ";
    }
  output << "}\n exp: {";
  for (int i=0; i<av.getSize(); i++)
    {
      output << av.getCSExpNth(i) << " ";
    }
  output << "}";
  return output;
}

}  /* end of namespace. */
