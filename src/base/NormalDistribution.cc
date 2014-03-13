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

#include "NormalDistribution.h"
#include <math.h>

namespace hmdp_base
{

int NormalDistribution::ptableRows = 40;
int NormalDistribution::ptableCols = 10;
double NormalDistribution::ptableMin = 0.0;
double NormalDistribution::ptableMinEntry = 0.500;
double NormalDistribution::ptableMax = 3.99;
double NormalDistribution::ptableMaxEntry = 1.000;

double NormalDistribution::ptable[40][10] = {
/* Z  0.00   0.01   0.02   0.03   0.04   0.05   0.06   0.07   0.08   0.09 */
  /* 0.0  */
  {0.5000,0.5040,0.5080,0.5120,0.5160,0.5199,0.5239,0.5279,0.5319,0.5359},
  /* 0.1 */
  {0.5398,0.5438,0.5478,0.5517,0.5557,0.5596,0.5636,0.5675,0.5714,0.5753},
  /* 0.2 */
  {0.5793,0.5832,0.5871,0.5910,0.5948,0.5987,0.6026,0.6064,0.6103,0.6141},
  /* 0.3 */
  {0.6179,0.6217,0.6255,0.6293,0.6331,0.6368,0.6406,0.6443,0.6480,0.6517},
  /* 0.4 */
  {0.6554,0.6591,0.6628,0.6664,0.6700,0.6736,0.6772,0.6808,0.6844,0.6879},
  /* 0.5 */
  {0.6915,0.6950,0.6985,0.7019,0.7054,0.7088,0.7123,0.7157,0.7190,0.7224},
  /* 0.6 */
  {0.7257,0.7291,0.7324,0.7357,0.7389,0.7422,0.7454,0.7486,0.7517,0.7549},
  /* 0.7 */
  {0.7580,0.7611,0.7642,0.7673,0.7704,0.7734,0.7764,0.7794,0.7823,0.7852},
  /* 0.8 */
  {0.7881,0.7910,0.7939,0.7967,0.7995,0.8023,0.8051,0.8078,0.8106,0.8133},
  /* 0.9 */
  {0.8159,0.8186,0.8212,0.8238,0.8264,0.8289,0.8315,0.8340,0.8365,0.8389},
  /* 1.0 */
  {0.8413,0.8438,0.8461,0.8485,0.8508,0.8531,0.8554,0.8577,0.8599,0.8621},
  /* 1.1 */
  {0.8643,0.8665,0.8686,0.8708,0.8729,0.8749,0.8770,0.8790,0.8810,0.8830},
  /* 1.2 */
  {0.8849,0.8869,0.8888,0.8907,0.8925,0.8944,0.8962,0.8980,0.8997,0.9015},
  /* 1.3 */
  {0.9032,0.9049,0.9066,0.9082,0.9099,0.9115,0.9131,0.9147,0.9162,0.9177},
  /* 1.4 */
  {0.9192,0.9207,0.9222,0.9236,0.9251,0.9265,0.9279,0.9292,0.9306,0.9319},
  /* 1.5 */
  {0.9332,0.9345,0.9357,0.9370,0.9382,0.9394,0.9406,0.9418,0.9429,0.9441},
  /* 1.6 */
  {0.9452,0.9463,0.9474,0.9484,0.9495,0.9505,0.9515,0.9525,0.9535,0.9545},
  /* 1.7 */
  {0.9554,0.9564,0.9573,0.9582,0.9591,0.9599,0.9608,0.9616,0.9625,0.9633},
  /* 1.8 */
  {0.9641,0.9649,0.9656,0.9664,0.9671,0.9678,0.9686,0.9693,0.9699,0.9706},
  /* 1.9 */
  {0.9713,0.9719,0.9726,0.9732,0.9738,0.9744,0.9750,0.9756,0.9761,0.9767},
  /* 2.0 */
  {0.9772,0.9778,0.9783,0.9788,0.9793,0.9798,0.9803,0.9808,0.9812,0.9817},
  /* 2.1 */
  {0.9821,0.9826,0.9830,0.9834,0.9838,0.9842,0.9846,0.9850,0.9854,0.9857},
  /* 2.2 */
  {0.9861,0.9864,0.9868,0.9871,0.9875,0.9878,0.9881,0.9884,0.9887,0.9890},
  /* 2.3 */
  {0.9893,0.9896,0.9898,0.9901,0.9904,0.9906,0.9909,0.9911,0.9913,0.9916},
  /* 2.4 */
  {0.9918,0.9920,0.9922,0.9925,0.9927,0.9929,0.9931,0.9932,0.9934,0.9936},
  /* 2.5 */
  {0.9938,0.9940,0.9941,0.9943,0.9945,0.9946,0.9948,0.9949,0.9951,0.9952},
  /* 2.6 */
  {0.9953,0.9955,0.9956,0.9957,0.9959,0.9960,0.9961,0.9962,0.9963,0.9964},
  /* 2.7 */
  {0.9965,0.9966,0.9967,0.9968,0.9969,0.9970,0.9971,0.9972,0.9973,0.9974},
  /* 2.8 */
  {0.9974,0.9975,0.9976,0.9977,0.9977,0.9978,0.9979,0.9979,0.9980,0.9981},
  /* 2.9 */
  {0.9981,0.9982,0.9982,0.9983,0.9984,0.9984,0.9985,0.9985,0.9986,0.9986},
  /* 3.0 */
  {0.9987,0.9987,0.9987,0.9988,0.9988,0.9989,0.9989,0.9989,0.9990,0.9990},
  /* 3.10 */
  {0.9990,0.9991,0.9991,0.9991,0.9992,0.9992,0.9992,0.9992,0.9993,0.9993},
  /* 3.20 */
  {0.9993,0.9993,0.9994,0.9994,0.9994,0.9994,0.9994,0.9995,0.9995,0.9995},
  /* 3.30 */
  {0.9995,0.9995,0.9995,0.9996,0.9996,0.9996,0.9996,0.9996,0.9996,0.9997},
  /* 3.40 */
  {0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9997,0.9998},
  /* 3.50 */
  {0.9998,0.9998,0.9998,0.9998,0.9998,0.9998,0.9998,0.9998,0.9998,0.9998},
  /* 3.60 */
  {0.9998,0.9998,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999},
  /* 3.70 */
  {0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999},
  /* 3.80 */
  {0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999,0.9999},
  /* 3.90 */
  {1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000}
};

NormalDistribution::NormalDistribution (double xmean, double xsd, double xinterval)
  : DiscreteDistribution (GAUSSIAN, xinterval), m_mean (xmean), m_sd (xsd)
{}

NormalDistribution::NormalDistribution (double xmean, double xsd, int nbins, double xinterval)
  : DiscreteDistribution (GAUSSIAN, nbins, xinterval), m_mean (xmean), m_sd (xsd)
{}

NormalDistribution::NormalDistribution (const NormalDistribution &nd)
  : DiscreteDistribution (nd), m_mean (nd.getMean ()), m_sd (nd.getSd ())
{}

NormalDistribution::~NormalDistribution () {}

double NormalDistribution::pdf (const double &x)
{
  if (m_sd == 0) return ((x == m_mean) ? 1.0 : 0.0);
  else 
    {
      double normalized = (x - m_mean) / m_sd;
      return (exp ((normalized * normalized) / -2.0) / (m_sd * sqrt(2.0 * M_PI)));
    }
}

double NormalDistribution::pdfToX (const double &pdf, const bool &higherVal)
{
  if (m_sd == 0) return m_mean;
  else 
    {
      double offset = m_sd * sqrt(-2.0 * log(pdf * m_sd * sqrt(2.0 * M_PI)));
      if (higherVal) return m_mean + offset;
      else return m_mean - offset;
    }
}

double NormalDistribution::cdfStandardCase (const double &x)
{
  int row, col, row2, col2;
  double val, val2, diff;

  if (x < NormalDistribution::ptableMin)
    {
      if (-x < NormalDistribution::ptableMin)
	  return 0.0;
      else return (1.0 - NormalDistribution::cdfStandardCase (-x));
    }
  else if (x > NormalDistribution::ptableMax)
    return NormalDistribution::ptableMaxEntry;

  /* table lookup. */
  row = (int) ((x - NormalDistribution::ptableMin) * 10.0);  /* truncates */
  col = ((int) (((x - NormalDistribution::ptableMin) * 10.0 - row)
		* NormalDistribution::ptableCols)) % NormalDistribution::ptableCols; /* truncates */
  row2 = row;
  col2 = col + 1;
  if (col2 >= NormalDistribution::ptableCols)
    {
      col2 = 0;
      row2++;
    }
  if (row2 >= NormalDistribution::ptableRows)
    return NormalDistribution::ptableMaxEntry;
  val = NormalDistribution::ptable[row][col];
  val2 = NormalDistribution::ptable[row2][col2];
  diff = x * 100 - (static_cast<double> (row * 10.0 + col));
  return ((1 - diff) * val + diff * val2);
}

double NormalDistribution::cdfToXStandardCase (const double &integral)
{
  int lastr, lastc;
  double high_x, low_x, high_integral, low_integral, ratio, returnval;

  /* Since the table only has 0.5 - 1, if we have something < 0.5, we */
  /* need to take the -cdfToXStandardCase(1-integral). */
  if (integral < 0.5) return -NormalDistribution::cdfToXStandardCase (1.0 - integral);
  
  for (int r=0; r<NormalDistribution::ptableRows; r++)
    {
      for (int c=0; c<NormalDistribution::ptableCols; c++)
	{
	  if (NormalDistribution::ptable[r][c] >= integral)
	    {
	      lastr = r;
	      lastc = c - 1;
	      if (lastc < 0)
		{
		  lastc = NormalDistribution::ptableCols - 1;
		  lastr--;
		}
	      if (lastr < 0)
		return (NormalDistribution::ptableMin + r / 10.0
			+ (static_cast<double> (c) / static_cast<double> (NormalDistribution::ptableCols)));
	      high_x = NormalDistribution::ptableMin
		+ (r + static_cast<double> (c) / static_cast<double> (NormalDistribution::ptableCols)) / 10.0;
	      low_x = NormalDistribution::ptableMin 
		+ (lastr + (static_cast<double> (lastc) / static_cast<double> (NormalDistribution::ptableCols))) / 10.0;
	      high_integral = NormalDistribution::ptable[r][c];
	      low_integral = NormalDistribution::ptable[lastr][lastc];
	      ratio = (integral - low_integral) / (high_integral - low_integral);
	      returnval = low_x + ratio * (high_x - low_x);
	      return returnval;
	    }
	}
    }
  return NormalDistribution::ptableMax;
}

double NormalDistribution::cdf (const double &x)
{
  double result = NormalDistribution::cdfStandardCase ((x-m_mean) / m_sd);
  return result;
}

double NormalDistribution::cdfToX (const double &integral)
{
  double result = NormalDistribution::cdfToXStandardCase (integral) * m_sd + m_mean;
  return result;
}

} /* end of namespace */
