#
# Copyright 2014 Emmanuel Benazera beniz@droidnik.fr
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

if LP
BINLP5=test_lp5
LP5_LD=-llpsolve55
else
BINLP5=
LP5_LD=
endif

bin_PROGRAMS=test_discrete_distribution test_bsp_tree test_continuous_transition test_continuous_reward test_value_function test_asym_op test_backup test_frontup test_continuous_state_distribution test_vrml test_convolution test_cross_dim
if LP
bin_PROGRAMS+=$(BINLP5)
endif
if PPDDL
bin_PROGRAMS+=test_ppddl_loader
endif

test_discrete_distribution_SOURCES=test-discrete-distribution.cc
test_bsp_tree_SOURCES=test-bsp-tree.cc
test_continuous_transition_SOURCES=test-continuous-transition.cc
test_continuous_reward_SOURCES=test-continuous-reward.cc
test_value_function_SOURCES=test-valuefunction.cc
test_asym_op_SOURCES=test-asym-op.cc
test_backup_SOURCES=test-backup.cc
test_frontup_SOURCES=test-frontup.cc
test_continuous_state_distribution_SOURCES=test-continuous-state-distribution.cc
test_vrml_SOURCES=test-vrml.cc
test_cross_dim_SOURCES=test-cross-dim.cc
if LP
test_lp5_SOURCES=test-lp5.cc
endif
test_convolution_SOURCES=test-convolution.cc
if PPDDL
test_ppddl_loader_SOURCES=test-ppddl-loader.cc
endif

AM_CPPFLAGS=-I../base -I../csa -I../loaders -I../engine -I../hmdpsim
AM_CXXFLAGS=-g -Wall -std=c++11
AM_LDFLAGS=-L../base -L../loaders -L../hmdpsim -L../engine
LDADD=-lHmdpLoaders -lHmdpEngine -lHmdpBase -lgflags

if LP
LDADD+=$(LP5_LD) -ldl -lcolamd
endif
if PPDDL
LDADD+=-lppddl
endif
if CSA
AM_LDFLAGS+=-L../csa
LDADD+=-lHmdpCsa
endif
