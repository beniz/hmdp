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

#include "LpSolve5.h"
#include "AlphaVector.h"
#include <iostream>
#include <stdio.h>
#include <assert.h>

namespace hmdp_base
{

int LpSolve5::countNonZeros (AlphaVector *av, std::vector<AlphaVector*> *vav)
{
  int non_zeros = 0;
  for (unsigned int i=0; i<vav->size (); i++)
    for (int j=0; j<av->getSize ()-1; j++)
      if (! Alg::REqual (0.0, av->getAlphaNth (j) - (*vav)[i]->getAlphaNth (j),
			 Alg::m_doubleEpsilon))
	non_zeros++;
  return non_zeros;
}

lprec* LpSolve5::prepareLPProblem(AlphaVector *av, std::vector<AlphaVector*> *vav,
				  double *low, double *high)
{
  size_t ncol = av->getSize();

  /* TODO: create the lp structure pb with 0 rows and av->getSize variables (colums). */
  lprec *lp = make_lp(0,ncol);
  assert(lp != NULL);
  
  /* TODO: name variables, for debugging. */
  for (size_t i=1; i<ncol; i++)
    {
      char strN[10];
      sprintf(strN, "v%i", i); 
      set_col_name(lp,i,strN);
    }
  set_col_name(lp,ncol,"d");   //variable introduced into the domination pb.
  
  /* create space large enough for one row */
  int *colno = (int *) malloc(ncol * sizeof(*colno));
  REAL *row = (REAL *) malloc(ncol * sizeof(*row));
  assert(colno != NULL); assert(row != NULL);
  
  /* TODO: add constraints as columns. */
  for (size_t c=0; c<vav->size(); c++)
    {
      for (size_t i=0; i<ncol-1; i++)   /* iterate the variables. */
	{
	  double val = av->getAlphaNth(i)-(*vav)[c]->getAlphaNth (i);

	  colno[i] = i+1;   //column number.
	  row[i] = -val;  /* -d since we're using the <= model instead of >=. */
	}
      
      /* TODO: extra variable 'd'. */
      colno[ncol-1] = ncol;
      row[ncol-1] = 1.0;        // 1.0 because we're doing minimization.

      /* TODO: right hand side. */
      double rhs = av->getAlphaNth(ncol-1)-(*vav)[c]->getAlphaNth(ncol-1);
      
      /* TODO: add the constraint. */
      if (!add_constraintex(lp,ncol,row,colno,LE,rhs))
	{
	  std::cout << "[Error]:LpSolve5: failed to add constraint. Exiting.\n";
	  exit(-1);
	}
    }

  set_add_rowmode(lp,FALSE);  //should be deactivated for model building (lpsolve internals).
  
  /* TODO: objective function is '-1' */
  for (size_t i=0; i<ncol-1; i++)
    {
      colno[i] = i+1;
      row[i] = 0.0;
    }
  colno[ncol-1] = ncol;
  row[ncol-1] = -1.0;
  
  if (!set_obj_fnex(lp,ncol,row,colno))
    {
      std::cout << "[Error]:LpSolve5: failed to add objective function. Exiting.\n";
      exit(-1);
    }

  /* TODO: bounds. */
  for (size_t i=1; i<ncol; i++)
    {
      set_bounds(lp,i,low[i-1],high[i-1]);
    }
  /* bounds on the extra variable: only need to set lower bound
     since the upper bound defaults to +infinite. */
  set_lowbo(lp,ncol,-DEF_INFINITE);

  /* overall: minimization problem. */
  set_minim(lp);
  
  //debug
  //write_LP(lp,stdout);
  //debug
  
  /* I only want to see important messages on screen while solving */
  set_verbose(lp, CRITICAL);

  return lp;
}

#ifdef HAVE_CSA
lprec* LpSolve5::prepareLPProblem(CSVector *av, std::vector<CSVector*> *csv,
				  double *low, double *high)
{
  size_t ncol = av->getSize();

  /* create the lp structure pb with 0 rows and av->getSize variables (colums). */
  lprec *lp = make_lp(0,ncol);
  assert(lp != NULL);
  
  /* name variables, for debugging. */
  for (size_t i=1; i<ncol; i++)
    {
      char strN[10];
      sprintf(strN, "v%i", i); 
      set_col_name(lp,i,strN);
    }
  set_col_name(lp,ncol,"d");   //variable introduced into the domination pb.
  
  /* create space large enough for one row */
  int *colno = new int[ncol];//(int *) malloc(ncol * sizeof(*colno));
  REAL *row = new REAL[ncol];//(REAL *) malloc(ncol * sizeof(*row));
  assert(colno != NULL); assert(row != NULL);
  
  /* add constraints as columns. */
  for (size_t c=0; c<csv->size(); c++)
    {
      for (size_t i=0; i<ncol-1; i++)   /* iterate the variables. */
	{
	  double val = av->getCSNth(i)-(*csv)[c]->getCSNth (i);

	  colno[i] = i+1;   //column number.
	  row[i] = -val;  /* -d since we're using the <= model instead of >=. */
	}
      
      /* extra variable 'd'. */
      colno[ncol-1] = ncol;
      row[ncol-1] = 1.0;        // 1.0 because we're doing minimization.

      /* right hand side. */
      double rhs = av->getCSNth(ncol-1)-(*csv)[c]->getCSNth(ncol-1);
      
      /* add the constraint. */
      if (!add_constraintex(lp,ncol,row,colno,LE,rhs))
	{
	  std::cout << "[Error]:LpSolve5: failed to add constraint. Exiting.\n";
	  exit(-1);
	}
    }

  set_add_rowmode(lp,FALSE);  //should be deactivated for model building (lpsolve internals).
  
  /* objective function is '-1' */
  for (size_t i=0; i<ncol-1; i++)
    {
      colno[i] = i+1;
      row[i] = 0.0;
    }
  colno[ncol-1] = ncol;
  row[ncol-1] = -1.0;
  
  if (!set_obj_fnex(lp,ncol,row,colno))
    {
      std::cout << "[Error]:LpSolve5: failed to add objective function. Exiting.\n";
      exit(-1);
    }

  /* bounds. */
  for (size_t i=1; i<ncol; i++)
    {
      set_bounds(lp,i,low[i-1],high[i-1]);
    }
  /* bounds on the extra variable: only need to set lower bound
     since the upper bound defaults to +infinite. */
  set_lowbo(lp,ncol,-DEF_INFINITE);

  /* overall: minimization problem. */
  set_minim(lp);
  
  //debug
  //write_LP(lp,stdout);
  //debug
  
  /* I only want to see important messages on screen while solving */
  set_verbose(lp, CRITICAL);

  delete[] colno; delete[]row;

  return lp;
}
#endif
  
int LpSolve5::solveLPProblem (lprec *lp)
{
  return solve (lp);
}

bool LpSolve5::isLPDominated (AlphaVector *av, std::vector<AlphaVector*> *vav,
			      double *witness, double *low, double *high)
{
  /* create LP problem */
  lprec *lp = LpSolve5::prepareLPProblem (av, vav, low, high);

  /* solve */
  //int lp_result = LpSolve5::solveLPProblem (lp);
  LpSolve5::solveLPProblem (lp);
  //print_solution (lp);

  /* decide */
  if (-static_cast<double> (lp->best_solution[0]) < Alg::m_doubleEpsilon)
    {
      delete_lp (lp);
      return true;  /* is a dominated vector */
    }
  
  /* copy the witness point */
  if (witness)
    for (int i=0; i<av->getSize ()-1; i++)
      witness[i] = lp->best_solution[lp->rows+i+1];
  
  /* clear the lp problem */
  delete_lp (lp);
  return false;
}

#ifdef HAVE_CSA
bool LpSolve5::isLPDominated (CSVector *av, std::vector<CSVector*> *csv,
			      double *witness, double *low, double *high)
{
  /* create LP problem */
  lprec *lp = LpSolve5::prepareLPProblem (av, csv, low, high);

  /* solve */
  //int lp_result = LpSolve5::solveLPProblem (lp);
  LpSolve5::solveLPProblem (lp);
  //print_solution (lp);

  /* decide */
  if (-static_cast<double> (lp->best_solution[0]) < Alg::m_doubleEpsilon)
    {
      delete_lp (lp);
      return true;  /* is a dominated vector */
    }
  
  /* copy the witness point */
  if (witness)
    for (int i=0; i<av->getSize ()-1; i++)
      witness[i] = lp->best_solution[lp->rows+i+1];
  
  /* clear the lp problem */
  delete_lp (lp);
  return false;
}
#endif
  
bool LpSolve5::areLPDominated (std::vector<AlphaVector*> *vav1, std::vector<AlphaVector*> *vav2,
			       double *witness, double *low, double *high)
{
  for (size_t i=0; i<vav1->size (); i++)
    {
      if (! LpSolve5::isLPDominated ((*vav1)[i], vav2, witness, low, high))
	return false;
    }
  return true;
}

void LpSolve5::pruneLP (std::vector<AlphaVector*> *vav,
			double *low, double *high,
			std::vector<AlphaVector*> *res)
{
  //debug
  /* std::cout << "[Debug]: LpSolve5::pruneLP: ";
     std::cout << "low0: " << low[0] << " -- low1: " << low[1]
     << " -- high0: " << high[0] << " -- high1: " << high[1] << std::endl;
     for (size_t i=0; i<vav->size (); i++)
     std::cout << *(*vav)[i] << ", ";
     std::cout << std::endl; */
  //debug

  /* first pick the vector that's best at the origin of the rectangle */
  double retval;
  AlphaVector* bav = AlphaVector::bestAlphaVector (*vav, low, &retval);

  //debug
  //std::cout << "bav: " << *bav << std::endl;
  //debug

  AlphaVector::removeAvFromVector (bav, vav);

  //debug
  /* std::cout << "vav size (remove): " << vav->size () << std::endl;
     for (size_t i=0; i<vav->size (); i++)
     std::cout << *(*vav)[i] << ", "; */
  //debug

  res->push_back (bav);

  /* do the pruning */
  double *witness = new double [bav->getSize ()-1];
  
  std::vector<AlphaVector *>::iterator it;
  for (it = vav->begin (); it<vav->end (); it++)
    {
      AlphaVector *av = (*it);
      if (! LpSolve5::isLPDominated (av, res, witness, low, high))
	{
	  bav = AlphaVector::bestAlphaVector (*vav, witness, &retval);
	  AlphaVector::removeAvFromVector (bav, vav);
	  res->push_back (bav);
	  
	  /* res->push_back (av);
	     vav->erase (it); */
	}
      else 
	{
	  vav->erase (it);
	  delete av;
	}
    }
  delete []witness;
  //delete vav;

  //debug
  /* if (res->size () >= 1)
     std::cout << "LpSolve5:Prune length: " << res->size () << std::endl;
     for (unsigned int i=0; i<res->size (); i++)
     std::cout << *(*res)[i] << ", ";
     std::cout << std::endl; */
  //debug
}

#ifdef HAVE_CSA
void LpSolve5::pruneLP (std::vector<CSVector*> *csv,
			double *low, double *high,
			std::vector<CSVector*> *res)
{
  //debug
  /* std::cout << "[Debug]: LpSolve5::pruneLP: ";
     std::cout << "low0: " << low[0] << " -- low1: " << low[1]
     << " -- high0: " << high[0] << " -- high1: " << high[1] << std::endl;
     for (size_t i=0; i<csv->size (); i++)
     std::cout << *(*csv)[i] << ", ";
     std::cout << std::endl; */
  //debug

  /* first pick the vector that's best at the origin of the rectangle */
  double retval;
  
  CSVector* bcsv = CSVector::bestCSVector(*csv, low, &retval);
  
  //assert(bcsv != NULL);

  //debug
  //std::cout << "bcsv: " << *bcsv << std::endl;
  //debug

  CSVector::removeAvFromVector(bcsv, csv);

  //debug
  /* std::cout << "csv size (remove): " << csv->size () << std::endl;
     for (size_t i=0; i<csv->size (); i++)
     std::cout << *(*csv)[i] << ", "; */
  //debug

  res->push_back(bcsv);

  /* do the pruning */
  double *witness = new double [bcsv->getSize ()-1];
  
  std::vector<CSVector *>::iterator it;
  for (it = csv->begin (); it<csv->end (); it++)
    {
      CSVector *av = (*it);
      if (! LpSolve5::isLPDominated (av, res, witness, low, high))
	{
	  bcsv = CSVector::bestCSVector (*csv, witness, &retval);
	  CSVector::removeAvFromVector (bcsv, csv);
	  res->push_back (bcsv);
	  
	  /* res->push_back (av);
	     vav->erase (it); */
	}
      else 
	{
	  csv->erase (it);
	  delete av;
	}
    }
  delete []witness;
  //delete vav;

  //debug
  /* if (res->size () >= 1)
     std::cout << "LpSolve5:Prune length: " << res->size () << std::endl;
     for (unsigned int i=0; i<res->size (); i++)
     std::cout << *(*res)[i] << ", ";
     std::cout << std::endl; */
  //debug
}
#endif
  
} /* end of namespace */
