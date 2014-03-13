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

#include "AlphaVector.h"
#include "Alg.h"
#include <stdlib.h>
#include <iterator>
#include <iostream>
#include <assert.h>
#include <algorithm>

namespace hmdp_base
{

AlphaVector::AlphaVector (const int &size)
  : m_size (size)
{
  m_alpha = (double *) calloc (m_size, sizeof (double));
}

AlphaVector::AlphaVector (const int &size, double alph[])
  : m_size (size)
{
  m_alpha = (double *) malloc (m_size * sizeof (double));
  for (int i=0; i<m_size; i++)
    m_alpha[i] = alph[i];
}

AlphaVector::AlphaVector (const double &val)
  : m_size (1)
{
  m_alpha = (double *) malloc (m_size * sizeof (double));
  m_alpha[0] = val;
}

AlphaVector::AlphaVector (const AlphaVector &av)
  : m_size (av.getSize ()) 
{
  m_alpha = (double *) malloc (m_size * sizeof (double));
  for (int i=0; i<m_size; i++)
    m_alpha[i] = av.getAlphaNth (i);
  if (av.m_actions.size ())
    m_actions = std::set<int> (av.m_actions);
}

AlphaVector::~AlphaVector ()
{
  free (m_alpha);
}

void AlphaVector::simpleSumAlphaVectors (const std::vector<AlphaVector*> &vav1, const std::vector<AlphaVector*> &vav2, std::vector<AlphaVector*> *res)
{
  AlphaVector *av1 = vav1[0], *av2 = vav2[0];
  AlphaVector *avres = new AlphaVector (1);
  avres->setAlphaNth (0, av1->getAlphaNth (0) + av2->getAlphaNth (0));
  (*res)[0] = avres;
}

void AlphaVector::simpleSubtractAlphaVectors (const std::vector<AlphaVector*> &vav1, const std::vector<AlphaVector*> &vav2, std::vector<AlphaVector*> *res)
{
  AlphaVector *av1 = vav1[0], *av2 = vav2[0];
  AlphaVector *avres = new AlphaVector (1);
  avres->setAlphaNth (0, av1->getAlphaNth (0) - av2->getAlphaNth (0));
  (*res)[0] = avres;
}

//Beware: actions
void AlphaVector::crossSumAlphaVectors (const std::vector<AlphaVector*> &vav1, 
					const std::vector<AlphaVector*> &vav2, 
					double *low, double *high, std::vector<AlphaVector*> *res)
{
#ifdef HAVE_LP
  std::vector<AlphaVector*> *vtmp = new std::vector<AlphaVector*> ();
  for (unsigned int i=0; i<vav1.size (); i++)
    {
      for (unsigned int j=0; j<vav2.size (); j++)
	{
	  assert (vav1[i]->getSize () == vav2[j]->getSize ());
	  AlphaVector *av = new AlphaVector (vav1[i]->getSize ());
	  for (int k=0; k<av->getSize (); k++)
	    av->setAlphaNth (k, vav1[i]->getAlphaNth (k) + vav2[j]->getAlphaNth (k));
	  vtmp->push_back (av);
	}
    }
  if (vtmp->size () > 1)
    LpSolve5::pruneLP (vtmp, low, high, res);
  else res->push_back ((*vtmp)[0]);
  delete vtmp;
#else
  std::cout << "[Warning]:AlphaVector::crossSumAlphaVectors: lp optimization not compiled, the set of linear functions can grow infinitely !\n"; 
  for (unsigned int i=0; i<vav1.size (); i++)
    {
      for (unsigned int j=0; j<vav2.size (); j++)
	{
	  AlphaVector *av = new AlphaVector (vav1[i]->getSize ());
	  for (int k=0; k<av->getSize (); k++)
	    av->setAlphaNth (k, vav1[i]->getAlphaNth (k) + vav2[j]->getAlphaNth (k));
	  res->push_back (av);
	}
    }
#endif
}

void AlphaVector::crossSubtractAlphaVectors (const std::vector<AlphaVector*> &vav1, 
					     const std::vector<AlphaVector*> &vav2, 
					     double *low, double *high, std::vector<AlphaVector*> *res)
{
#ifdef HAVE_LP
  std::vector<AlphaVector*> *vtmp = new std::vector<AlphaVector*> ();
  for (unsigned int i=0; i<vav1.size (); i++)
    {
      for (unsigned int j=0; j<vav2.size (); j++)
	{
	  AlphaVector *av = new AlphaVector (vav1[i]->getSize ());
	  for (int k=0; k<av->getSize (); k++)
	    av->setAlphaNth (k, vav1[i]->getAlphaNth (k) - vav2[j]->getAlphaNth (k));
	  vtmp->push_back (av);
	}
    }
  if (vtmp->size () > 1)
    LpSolve5::pruneLP (vtmp, low, high, res);
  else res->push_back ((*vtmp)[0]);
  delete vtmp;
#else
  std::cout << "[Warning]:AlphaVector::crossSubtractAlphaVectors: lp optimization not compiled, the set of linear functions can grow infinitely !\n"; 
  for (unsigned int i=0; i<vav1.size (); i++)
    {
      for (unsigned int j=0; j<vav2.size (); j++)
	{
	  AlphaVector *av = new AlphaVector (vav1[i]->getSize ());
	  for (int k=0; k<av->getSize (); k++)
	    av->setAlphaNth (k, vav1[i]->getAlphaNth (k) - vav2[j]->getAlphaNth (k));
	  res->push_back (av);
	}
    }
#endif
}

void AlphaVector::maxConstantAlphaVector (const std::vector<AlphaVector*> &vav1, 
					  const std::vector<AlphaVector*> &vav2,
					  std::vector<int> *ag1, std::vector<int> *ag2,
					  std::vector<AlphaVector*> *res)
{
  AlphaVector *av1 = vav1[0], *av2 = vav2[0];
  AlphaVector *avres = new AlphaVector (1);
  
  /* break ties based on achieved goals. */
  if (Alg::REqual (av1->getAlphaNth (0), av2->getAlphaNth (0),
		   Alg::m_doubleEpsilon)) 
    {
      avres->setAlphaNth (0, av2->getAlphaNth (0));  
      std::set_union (av1->getActionsBegin (), av1->getActionsEnd (),
		      av2->getActionsBegin (), av2->getActionsEnd (),
		      std::insert_iterator<std::set<int> > (avres->m_actions, avres->getActionsBegin ()), std::less<int>());
    }
  else if (Alg::RSup (av1->getAlphaNth (0), av2->getAlphaNth (0),
		      Alg::m_doubleEpsilon))
    {
      avres->setAlphaNth (0, av1->getAlphaNth (0));
      if (av1->m_actions.size ())
	avres->setActions (av1->m_actions);
    }
  else if (Alg::RInf (av1->getAlphaNth (0), av2->getAlphaNth (0),
		      Alg::m_doubleEpsilon))
    {
      avres->setAlphaNth (0, av2->getAlphaNth (0));
      if (av2->m_actions.size ())
	avres->setActions (av2->m_actions);
    }
  
  (*res)[0] = avres;
}

void AlphaVector::minConstantAlphaVector (const std::vector<AlphaVector*> &vav1, const std::vector<AlphaVector*> &vav2, std::vector<AlphaVector*> *res)
{
  AlphaVector *av1 = vav1[0], *av2 = vav2[0];
  AlphaVector *avres = new AlphaVector (1);

  if (av1->getAlphaNth (0) < av2->getAlphaNth (0))
    {
      avres->setAlphaNth (0, av1->getAlphaNth (0));
      avres->setActions (av1->m_actions);
    }
  else if (av1->getAlphaNth (0) > av2->getAlphaNth (0))
    {
      avres->setAlphaNth (0, av2->getAlphaNth (0));
      avres->setActions (av2->m_actions);
    }
  else /* equality */
    {
      avres->setAlphaNth (0, av2->getAlphaNth (0));
      std::set_union (av1->getActionsBegin (), av1->getActionsEnd (),
		      av2->getActionsBegin (), av2->getActionsEnd (),
		      std::insert_iterator<std::set<int> > (avres->m_actions, avres->getActionsBegin ()), std::less<int>());
    }

  (*res)[0] = avres;
}

void AlphaVector::maxLinearAlphaVector (const std::vector<AlphaVector*> &vav1, 
					const std::vector<AlphaVector*> &vav2, 
					double *low, double *high, std::vector<AlphaVector*> *res)
{
#ifdef HAVE_LP
  std::vector<AlphaVector*> *vtmp = new std::vector<AlphaVector*> ();
  for (unsigned int i=0; i<vav1.size (); i++)
    vtmp->push_back (new AlphaVector (*vav1[i]));
  for (unsigned int i=0; i<vav2.size (); i++)
    vtmp->push_back (new AlphaVector (*vav2[i]));
  
  LpSolve5::pruneLP (vtmp, low, high, res);
  delete vtmp;
#else
  std::cout << "[Warning]:AlphaVector::maxLinearAlphaVector: lp optimization not compiled, the set of linear functions can grow infinitely !\n";
  for (unsigned int i=0; i<vav1.size (); i++)
    res->push_back (new AlphaVector (*vav1[i]));
  for (unsigned int i=0; i<vav2.size (); i++)
    res->push_back (new AlphaVector (*vav2[i]));
#endif
}

void AlphaVector::minLinearAlphaVector (const std::vector<AlphaVector*> &vav1, const std::vector<AlphaVector*> &vav2, double *low, double *high, std::vector<AlphaVector*> *res)
{
  /* TODO: requires linear programming and pruning */
  std::cout << "AlphaVector::minLinearAlphaVector: non implemented yet.\n";
}

void AlphaVector::addScalar (const double &scalar)
{
  for (int i=0; i<m_size; i++)
    m_alpha[i] += scalar;
}

void AlphaVector::multiplyByScalar (const double &scalar)
{
  for (int i=0; i<m_size; i++)
    m_alpha[i] *= scalar;
}

void AlphaVector::addVecScalar (const double &scalar, std::vector<AlphaVector*> *res)
{
  for (unsigned int i=0; i<res->size (); i++)
    (*res)[i]->addScalar (scalar);
}

void AlphaVector::multiplyVecByScalar (const double &scalar, std::vector<AlphaVector*> *res)
{
  for (unsigned int i=0; i<res->size (); i++)
    (*res)[i]->multiplyByScalar (scalar);
}

AlphaVector* AlphaVector::bestAlphaVector (const std::vector<AlphaVector*> &vav,
					   double *witness, double *retv)
{
  double v = 0.0;
  *retv = -100000000.0;   /* TODO */
  AlphaVector *bestAv = 0;

  for (unsigned int i=0; i<vav.size (); i++)
    {
      v = vav[i]->getAlphaNth (vav[i]->getSize ()-1);  /* constant */
      for (int j=0; j<vav[i]->getSize ()-1; j++)
	v += (vav[i]->getAlphaNth (j) * witness[j]);
      
      //debug
      //std::cout << "vec: " << *vav[i] << " -- v: " << v << " -- ";
      //debug

      if (v > *retv)
 	{
	  bestAv = vav[i];
	  *retv = v;
	}
      else if (v == *retv)  /* lexigraphical dominance. */
	{
	  for (int k=0; k<vav[i]->getSize ()-1; k++)
	    if (vav[i]->getAlphaNth (k) > bestAv->getAlphaNth (k))
	      {
		bestAv = vav[i];
		break;
	      }
	}
    }
  return bestAv;
}

void AlphaVector::removeAvFromVector(AlphaVector *av, std::vector<AlphaVector*> *vav)
{
  std::vector<AlphaVector*>::iterator it = vav->begin ();
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

void AlphaVector::removeAction (const int &action)
{
  std::set<int>::iterator it;
  if ((it = m_actions.find (action)) != m_actions.end ())
    m_actions.erase (it);
}

bool AlphaVector::isEqual (const AlphaVector &av)
{
  if (av.getSize () != m_size)
    return false;
  for (int i=0; i<m_size; i++)
    if (m_alpha[i] != av.getAlphaNth (i))
      return false;
  return true;
}

bool AlphaVector::isEqualActionSets (const std::set<int> &a1, const std::set<int> &a2)
{
  std::set<int> diffa;
  if (a1.size () != a2.size ())
    return false;
  
  std::set_difference (a1.begin (), a1.end (), a2.begin (), a2.end (),
		       std::insert_iterator<std::set<int> > (diffa, diffa.begin ()), std::less<int> ());
  
  if (diffa.empty ())
    return true;
  return false;
}

bool AlphaVector::isEqualVecGoals (const std::vector<int> &g1, 
				   const std::vector<int> &g2)
{
  if (g1.size () != g2.size ())
    return false;
  for (size_t i=0; i<g1.size (); i++)
    {
      bool found = false;
      for (size_t j=0; j<g2.size (); j++)
	if (g1[i] == g2[j])
	  {
	    found = true;
	    break;
	  }
      if (! found) return false;
    }
  return true;
}

bool AlphaVector::isVecEqual (const std::vector<AlphaVector*> &vav1, 
			      const std::vector<AlphaVector*> &vav2)
{
  if (vav1.size () != vav2.size ())
    return false;
  for (size_t i=0; i<vav1.size (); i++)
    {
      bool found = false;
      for (size_t j=0; j<vav2.size (); j++)
	if ((found = vav1[i]->isEqual (*vav2[j])))
	  break;
      if (! found) return false;
    }
  return true;
}

bool AlphaVector::isZero ()
{
  for (int i=0; i<m_size; i++)
    if (m_alpha[i] != 0)
      return false;
  return true;
}

bool AlphaVector::isZeroOrBelow ()
{
  for (int i=0; i<m_size; i++)
    if (m_alpha[i] > 0)
      return false;
  return true;
}

std::ostream &operator<<(std::ostream &output, AlphaVector &av)
{
  output << "{ ";
  for (int i=0; i<av.getSize (); i++)
    {
      output << av.getAlphaNth (i) << " ";
    }
  output << "}";
  if (av.getActionsSize ())
    {
      output << "\n|";
      std::set<int>::const_iterator it;
      for (it = av.getActionsBegin (); it != av.getActionsEnd (); it++)
	output << (*it) << " ";
      output << "|";
    } 
  return output;
}

} /* end of namespace */
