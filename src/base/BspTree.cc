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

#include "BspTree.h"
#include "BspTreeOperations.h"
#include <math.h>
#include <stdlib.h>

namespace hmdp_base
{

PlotPointFormat BspTree::m_plotPointFormat = GnuplotF;  /**< gnuplot is default. */

BspTree::BspTree (const int &dim)
  : m_bspType (BspTreeT), m_nDim (dim), m_d (-1), m_pos (0), 
    m_lt (0), m_ge (0) /* should find a better value for mpos... */
{}

BspTree::BspTree (const int &dim, const int &d, const double &pos)
  : m_bspType (BspTreeT), m_nDim (dim), m_d (d), m_pos (pos), m_lt (0), m_ge (0)
{}

/* Beware: may need domain bounds for checking the shift is valid. */
BspTree::BspTree (const int &dim, double *lowPos, double *highPos, 
		  const double *low, const double *high, const double *shift)
  : m_bspType (BspTreeT), m_nDim (dim)
{
  BspTree *bsp_n = this;
  
  /* setting bounds */
  for (int i=0; i<m_nDim; i++)  /* iterate the dimensions */
    {
      /* higher bound */
      bsp_n->setDimension (i);

      /* check if higher bound belongs to the domain. */
      if (highPos[i] > high[i]
	  || (shift && highPos[i] + shift[i] > high[i]))
	{
	  std::cerr << "[Warning]: BspTree constructor: upper bound is outside the domain.\n";
	  if (! shift) highPos[i] = high[i];
	  else highPos[i] = high[i] - shift[i];
	}

      bsp_n->setPosition (highPos[i]);
      if (shift) bsp_n->shiftPosition (shift[i]);

      /* check if lower bound belongs to the domain. */
      if (lowPos[i] < low[i]
	  || (shift && lowPos[i] + shift[i] < low[i]))
	{
	  std::cerr << "[Warning]: BspTree constructor: lower bound is outside the domain.\n";
	  if (! shift) lowPos[i] = low[i];
	  else lowPos[i] = low[i] - shift[i];
	}

      bsp_n->setLowerTree (new BspTree (m_nDim, i, lowPos[i]));
      bsp_n->setGreaterTree (new BspTree (m_nDim));
      bsp_n = bsp_n->getLowerTree ();
      
      /* lower bound */
      if (shift) bsp_n->shiftPosition (shift[i]);
      bsp_n->setLowerTree (new BspTree (m_nDim));
      bsp_n->setGreaterTree (new BspTree (m_nDim));
      bsp_n = bsp_n->getGreaterTree ();
    }
}

BspTree::BspTree (const BspTree &bt)
  : m_bspType (bt.getType ()), m_nDim (bt.getSpaceDimension ()), 
    m_d (bt.getDimension ()), m_pos (bt.getPosition ()) 
{
  if (! bt.isLeaf ())
    {
      m_lt = new BspTree (*bt.getLowerTree ());
      m_ge = new BspTree (*bt.getGreaterTree ());
    }
  else m_lt = m_ge = 0;
}

/* non recursive destructor */
BspTree::~BspTree ()
{}

/* recursive destructor (static) */
void BspTree::deleteBspTree (BspTree *bt)
{
  if (! bt->isLeaf ())
    {
      BspTree::deleteBspTree (bt->getLowerTree ());
      BspTree::deleteBspTree (bt->getGreaterTree ());
      bt->m_lt = 0;
      bt->m_ge = 0;
    }
  if (bt)
    delete bt;  /* destroy node (virtual) */
  bt = 0;
}

int BspTree::countLeaves () const
{
  if (isLeaf ())
    {
      return 1;
    }
  else
    {
      return getLowerTree ()->countLeaves () 
	+ getGreaterTree ()->countLeaves ();
    }
  
}

/* printing */
void BspTree::print (std::ostream &out,
		     double *low, double *high) const
{
  if (isLeaf ())
    {
      out << "{";
      for (int i=0; i<getSpaceDimension (); i++)
	{
	  out << "[" << low[i] << "," << high[i];
	  if (i == getSpaceDimension () - 1)
	    out << "]}\n";
	  else out << "],";
	}
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      getGreaterTree ()->print (out, low, high);
      low[getDimension ()] = b;
      
      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      getLowerTree ()->print (out, low, high);
      high[getDimension ()] = b;
    }
}

void BspTree::plot1Dbox (std::ofstream &box,
			 double *low, double *high)
{
  if (isLeaf ())
    {
      box << low[0] << ' ' << high[0] << '\n';
    }
  else 
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      getGreaterTree ()->plot1Dbox (box, low, high);
      low[getDimension()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      getLowerTree ()->plot1Dbox (box, low, high);
      high[getDimension ()] = b;
    }
}

void BspTree::plot2Dbox (const double &space, std::ofstream &box,
			 double *low, double *high)
{
  if (isLeaf ())
    {
      box << (low[0]+high[0]) / 2.0 
	  << ' ' << (low[1]+high[1]) / 2.0
	  << ' ' << (high[0]-low[0]) / 2.0 - space
	  << ' ' << (high[1]-low[1]) / 2.0 - space << '\n';
    }
  else 
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      getGreaterTree ()->plot2Dbox (space, box, low, high);
      low[getDimension()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      getLowerTree ()->plot2Dbox (space, box, low, high);
      high[getDimension ()] = b;
    }
}

void BspTree::plot2DboxWithValue(std::ofstream &box,
				 double *low, double *high)
{
  if (isLeaf())
    {
      if (low[0] == high[0]
	  || low[1] == high[1])
	return;
      
      double pos[2] = {low[0],low[1]};
      box << low[0] << ' ' << low[1] << ' ' << getPointValueInLeaf(pos) << "\n";
      
      pos[0]=high[0],pos[1]=high[1];
      box << high[0] << ' ' << high[1] << ' ' << getPointValueInLeaf(pos) << "\n";
      
      pos[0]=low[0],pos[1]=high[1];
      box << low[0] << ' ' << high[1] << ' ' << getPointValueInLeaf(pos) << "\n";
      
      pos[0]=low[1],pos[1]=high[0];
      box << low[1] << ' ' << high[0] << ' ' << getPointValueInLeaf(pos) << "\n";
      box << "\n";
    }
  else
    {
      double b = low[getDimension ()];
      low[getDimension ()] = getPosition ();
      getGreaterTree ()->plot2DboxWithValue(box, low, high);
      low[getDimension()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      getLowerTree ()->plot2DboxWithValue(box, low, high);
      high[getDimension ()] = b;
    }
}

void BspTree::plot2DVrml2 (const double &space, std::ofstream &vrml_out,
			   double *low, double *high, const double &max_value)
{
  double scale0 = high[0]; double scale1 = high[1];  /* for better graphics, we scale everything down to [0;1] */
  double width0 = (high[0] - low[0]) / scale0, width1 = (high[1] - low[1]) / scale1;
  
  /* vrml headers */
  vrml_out << "#VRML V2.0 utf8\n"
	   << "Transform\n{\n\trotation 1 0 0 3.14\n\tchildren [\n\t\tTransform {\n\t\t\ttranslation "
	   << width0/2.0 << " 0.0 " << width1/2.0
	   << "\n\t\t\tchildren [\n\t\t\t\tShape {\n\t\t\t\t\tappearance Appearance {material Material {} }\n\t\t\t\t\tgeometry Box {size "
	   << width0 << " 0.0 " << width1 << "}\n\t\t\t}\n\t\t]\n\t}\n";

  plot2DVrml2R (space, vrml_out, low, high, scale0, scale1, max_value);

  /* vrml end */
  vrml_out << "]\n}\nNavigationInfo { type \"EXAMINE\" }\n";
}

void BspTree::plot2DVrml2R (const double &space, std::ofstream &output_vrml, 
			    double* low, double *high, const double &scale0, 
			    const double &scale1, const double &max_value)
{
  if (isLeaf ())
    {
      plot2DVrml2Leaf (space, output_vrml, low, high, scale0, scale1, max_value);  /* virtual call */
    }
  else 
    {
      double b=low[getDimension ()];
      low[getDimension ()] = getPosition ();
      getGreaterTree ()->plot2DVrml2R (space, output_vrml, low, high, scale0, scale1, max_value);
      low[getDimension()] = b;

      b = high[getDimension ()];
      high[getDimension ()] = getPosition ();
      getLowerTree ()->plot2DVrml2R (space, output_vrml, low, high, scale0, scale1, max_value);
      high[getDimension ()] = b;
    }
}

void BspTree::plotNDPointValues (std::ofstream &output_values, double *step,
				 double *low, double *high)
{
  int *npts = new int[m_nDim];
  int *bins = new int[m_nDim];
  int *counters = new int[m_nDim];
  double *pos = new double[m_nDim];
  int tpts = 1;

  for (int i=0; i<m_nDim; i++)
    {
#if !defined __GNUC__ || __GNUC__ < 3
      npts[i] = ceil ((high[i] - low[i]) / step[i]);
#else
      npts[i] = lround ((high[i] - low[i]) / step[i]);
#endif
      if (i == 0) bins[0] = 1;
      else bins[i] = bins[i-1] * npts[i-1];

      tpts *= npts[i];

      counters[i] = 0;
      pos[i] = low[i];
    }

  int cpts = 0;
  while (cpts < tpts)
    {
      /* write point coordinates and value. */
      if (BspTree::m_plotPointFormat == GnuplotF)
	plotPointValueGnuplot (output_values, pos);
      else if (BspTree::m_plotPointFormat == MathematicaF)
	plotPointValueMathematica (output_values, pos);	
      else
	{
	  std::cerr << "[Error]:BspTree::plotNDPointValues: unknown output format: "
		    << BspTree::m_plotPointFormat << ". Exiting.\n";
	  exit (-1);
	}

      /* update coordinates. */
      for (int i=m_nDim-1; i>=0; i--)
	{
	  counters[i]++;
	  if (counters[i] == bins[i])
	    {
	      if (i == m_nDim-1)
		output_values << std::endl;

	      counters[i] = 0;
	      pos[i] += step[i];
	      
	      /* reset lower dimensions. */
	      for (int j=0; j<i; j++)
		{
		  counters[j] = 0;
		  pos[j] = low[j];
		}
	      break;
	    }
	}
      cpts++;
    }
  
  delete []npts; delete []bins; delete []counters; delete []pos;
}

void BspTree::plotPointValueMathematica (std::ofstream &output_value, double *pos)
{
  output_value << getPointValue (pos) << " ";
}

void BspTree::plotPointValueGnuplot (std::ofstream &output_value, double *pos)
{
  for (int k=m_nDim-1; k>=0; k--)
    output_value << pos[k] << " ";
  output_value << getPointValue (pos) << std::endl;
}

double BspTree::getPointValue (double *pos)
{
  if (isLeaf ())
    return getPointValueInLeaf (pos);
  else
    {
      if (pos[getDimension ()] < getPosition ())
	return getLowerTree ()->getPointValue (pos);
      else return getGreaterTree ()->getPointValue (pos);
    }
}

void BspTree::fixSpaceDimension(const int &sdim)
{
  m_nDim = sdim;
  if (!isLeaf())
    {
      m_lt->fixSpaceDimension(sdim);
      m_ge->fixSpaceDimension(sdim);
    }
}

} /* end of namespace */
