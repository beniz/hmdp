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

#include "HmdpEngine.h"

/* parser structures */
#include "states.h"
#include "problems.h"
#include "domains.h"
#include "exceptions.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>

//#define STRIP_FLAG_HELP 1
#include <gflags/gflags.h>

using namespace std;
using namespace hmdp_loader;
using namespace ppddl_parser;

DEFINE_string(ppddl_file,"model.ppddl","PPDDL model input file");
DEFINE_string(output_prefix,"","Prefix of output files, prefix+model.dat, ...");
DEFINE_bool(with_convol,false,"Uses convolutions to compute the reachable continuous space within every discrete state (slower)");
DEFINE_string(vf_output_formats,"dat","Comma-separated list of value function output formats, from 'dat' (default, point-based from the dynamically discretized vf), 'mat' (Matlab, point-based), 'vrml' and 'box' (1D or 2D only, tile-based output for gnuplot)");
DEFINE_double(prec,1e-20,"Numerical precision (below which tiles are merged), default is 1e-20");
DEFINE_bool(output_first_state_only,true,"Outputs the value function of the initial state only (default is true)");
DEFINE_bool(show_discrete_states,false,"Whether to output the discrete state value for every state (default is false)");
DEFINE_string(point_based_output_step,"","Comma-separated list of stepsizes for point-based discretized output of value functions in dat and mat format (default is 1.0 in every dimension)");
DEFINE_bool(print_world,false,"Prints full MDP world loaded from PPDDL (default is false)");
DEFINE_bool(truncate_negative_ct_outcomes,false,"Truncates the negative part of continuous transition outcomes: this is useful when continuous spate-space models non-replenishable resources");
DEFINE_string(algo,"dfs","Algorithm for value function approximation among dfs (default, depth-first search for finite-horizon and resource problems), vi (value iteration)");
DEFINE_int32(T,-1,"Horizon for value iteration (-1 for infinite is default)");
DEFINE_bool(one_time_reward,false,"Whether reward can only be reaped once (can be part of the model, here to simplify testing and modeling");
DEFINE_double(gamma,1.0,"Discount factor");
DEFINE_double(vi_epsilon,1e-3,"Precision on value iteration convergence");

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

int main (int argc, char *argv[])
{
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  Alg::m_doubleEpsilon = FLAGS_prec;
  DiscreteDistribution::m_positiveResourcesConsumptionTruncation = FLAGS_truncate_negative_ct_outcomes;
  HmdpWorld::m_oneTimeReward = FLAGS_one_time_reward;
  
  /*
   * Read pddl file and convert to hmdp structures.
   */
  HmdpWorld::loadWorld (FLAGS_ppddl_file.c_str());

  /* visualize results */
  if (FLAGS_print_world)
    HmdpWorld::print (std::cout);

  /* plan for first initial state, dfs backup */
  BspTreeOperations::m_piecesMerging = true;
  BspTreeOperations::m_piecesMergingByValue = false;
  BspTreeOperations::m_piecesMergingByAction = false;
  BspTreeOperations::m_piecesMergingEquality = true;
  BspTreeOperations::m_bspBalance = false;
  
  clock_t backup_start, backup_stop;
  backup_start = clock ();
  if (FLAGS_algo == "dfs")
    {
      HmdpEngine::DepthFirstSearchBackupCSD (HmdpWorld::getFirstInitialState (),true,FLAGS_with_convol);
    }
  else if (FLAGS_algo == "vi")
    {
      HmdpEngine::ValueIteration(HmdpWorld::getFirstInitialState(),FLAGS_gamma,FLAGS_vi_epsilon,FLAGS_T);
    }
  else
    {
      std::cout << "Error: unknown algorithm " << FLAGS_algo << ". Exiting\n";
      exit(1);
    }
  backup_stop = clock ();
  double time = (double) (backup_stop - backup_start) / (double) (CLOCKS_PER_SEC);
  std::cout << "\nbackup ";
  if (FLAGS_with_convol)
    std::cout << "+ convolutions ";
  std::cout << "time: " << time << std::endl;
  std::cout << "total number of discrete states (dfs): " << HmdpEngine::getNStates () << std::endl;

  // discretization for point based vf output (dat & mat).
  std::string output_file_head = FLAGS_output_prefix + FLAGS_ppddl_file;
  int ndims = HmdpWorld::getFirstInitialState()->getVF()->getSpaceDimension();
  std::vector<double> step(ndims,1.0);
  if (!FLAGS_point_based_output_step.empty())
    {
      std::vector<std::string> tokens = split(FLAGS_point_based_output_step,',');
      if ((int)tokens.size() != ndims)
	{
	  std::cerr << "Warning: point_based_output_step does not contain enough steps for discretizing across all dimensions: using default everywhere\n";
	}
      else
	{
	  for (size_t i=0;i<tokens.size();i++)
	    step[i] = strtod(tokens.at(i).c_str(),NULL);
	}
    }

  std::map<HmdpState*, std::map<short, std::vector<HmdpState*> > >::const_iterator it;
  for (it = HmdpEngine::m_nextStates.begin (); 
       it != HmdpEngine::m_nextStates.end (); it++)
    {
      std::string numstr = std::to_string((*it).first->getStateIndex());
      std::string state_filename = FLAGS_output_prefix + FLAGS_ppddl_file + "_state_" + numstr;

      if ((*it).first->getStateIndex() == 0 || !FLAGS_output_first_state_only)
	{
	  if (FLAGS_vf_output_formats.find("box") != std::string::npos)
	    {
	      std::string state_filename_box = state_filename + ".box";
	      ofstream output_state (state_filename_box.c_str (), ios::out);
	      if (HmdpWorld::getNResources () == 1)
		(*it).first->getVF ()->plot1DVF (output_state,
						 HmdpWorld::getRscLowBounds (),
						 HmdpWorld::getRscHighBounds ());
	      else if (HmdpWorld::getNResources () == 2)
		(*it).first->getVF ()->plot2Dbox (0.0, output_state,
						  HmdpWorld::getRscLowBounds (),
						  HmdpWorld::getRscHighBounds ());
	      std::cout << "written file " << state_filename_box << std::endl;
	    }
	  
	  // value function output.
	  if (FLAGS_vf_output_formats.find("dat") != std::string::npos)
	    {
	      std::string state_filename_dat = state_filename + ".dat";
	      BspTree::m_plotPointFormat = GnuplotF;
	      ofstream output_state0_values_gp (state_filename_dat.c_str(), ios::out);
	      HmdpWorld::getFirstInitialState ()->getVF ()->plotNDPointValues (output_state0_values_gp, &step[0],
									       HmdpWorld::getRscLowBounds (),
									       HmdpWorld::getRscHighBounds ());
	      std::cout << "written file " << state_filename_dat << std::endl;
	    }
	  
	  if (FLAGS_vf_output_formats.find("mat") != std::string::npos)
	    {
	      std::string state_filename_mat = state_filename + ".mat";
	      BspTree::m_plotPointFormat = MathematicaF;
	      ofstream output_state0_values_mat (state_filename_mat.c_str(), ios::out);
	      HmdpWorld::getFirstInitialState ()->getVF ()->plotNDPointValues (output_state0_values_mat, &step[0],
									       HmdpWorld::getRscLowBounds (),
									       HmdpWorld::getRscHighBounds ());
	      std::cout << "written file " << state_filename_mat << std::endl;
	    }
	  
	  if (FLAGS_vf_output_formats.find("vrml") != std::string::npos)
	    {
	      std::string state_filename_vrml = state_filename + ".vrml";
	      ofstream output_state0_vrml (state_filename_vrml.c_str(), ios::out);
	      double mval = 0.0; std::vector<int> gls;
	      HmdpWorld::getFirstInitialState ()->getVF ()->maxValueAndGoals (gls, mval, HmdpWorld::getRscLowBounds (),
									      HmdpWorld::getRscHighBounds ());
	      HmdpWorld::getFirstInitialState ()->getVF ()->plot2DVrml2 (0.0, output_state0_vrml, 
									 HmdpWorld::getRscLowBounds (),
									 HmdpWorld::getRscHighBounds (), mval);
	      std::cout << "written file " << state_filename_vrml << std::endl;
	    }
	  
	  /* printing convolutions */
	  if (FLAGS_with_convol)
	    {
	      if (FLAGS_vf_output_formats.find("box") != std::string::npos)
		{
		  std::string state_conv_fn = output_file_head + "_state_" + numstr + "_csd.box";
		  ofstream output_csd (state_conv_fn.c_str (), ios::out);
		  if (HmdpWorld::getNResources () == 1)
		    (*it).first->getCSD ()->plot1DCSD (output_csd,
						       HmdpWorld::getRscLowBounds (),
						       HmdpWorld::getRscHighBounds ());
		  else if (HmdpWorld::getNResources () == 2)
		    (*it).first->getCSD ()->plot2Dbox (0.0, output_csd,
						       HmdpWorld::getRscLowBounds (),
						       HmdpWorld::getRscHighBounds ());
		  std::cout << "written file " << state_conv_fn << std::endl;
		}
	      
	      /* std::cout << "state csd:\n";
		 (*it).first->getCSD ()->print (std::cout, HmdpWorld::getRscLowBounds (),
		 HmdpWorld::getRscHighBounds ()); */
	      
	      if (FLAGS_vf_output_formats.find("vrml") != std::string::npos)
		{
		  std::string state_conv_fn_vrml = output_file_head + "_state_" + numstr + "_csd.vrml";
		  ofstream output_csd_vrml (state_conv_fn_vrml.c_str (), ios::out);
		  if (HmdpWorld::getNResources () == 2)
		    {
		      (*it).first->getCSD ()->plot2DVrml2 (0.0, output_csd_vrml,
							   HmdpWorld::getRscLowBounds (),
							   HmdpWorld::getRscHighBounds (), 1.0);
		      std::cout << "written file " << state_conv_fn_vrml << std::endl;
		    }
		}
	      
	      if (FLAGS_vf_output_formats.find("mat") != std::string::npos)
		{
		  BspTree::m_plotPointFormat = MathematicaF;
		  std::string state_conv_fn_mat = output_file_head + "_state_" + numstr + "_csd.mat";
		  ofstream output_state_values_mat (state_conv_fn_mat.c_str (), ios::out);
		  (*it).first->getCSD ()->plotNDPointValues (output_state_values_mat, &step[0],
							     HmdpWorld::getRscLowBounds (),
							     HmdpWorld::getRscHighBounds ());
		  std::cout << "written file " << state_conv_fn_mat << std::endl;
		}
	      
	      if (FLAGS_vf_output_formats.find("dat") != std::string::npos)
		{
		  BspTree::m_plotPointFormat = GnuplotF;
		  std::string state_conv_fn_gp = output_file_head + "_state_" + numstr + "_csd.dat";
		  ofstream output_state_values_dat (state_conv_fn_gp.c_str (), ios::out);
		  (*it).first->getCSD ()->plotNDPointValues (output_state_values_dat, &step[0],
							     HmdpWorld::getRscLowBounds (),
							     HmdpWorld::getRscHighBounds ());
		  std::cout << "written file " << state_conv_fn_gp << std::endl;
		}
	    }
	}

      if (FLAGS_show_discrete_states)
	(*it).first->print (std::cout);
    }
}
