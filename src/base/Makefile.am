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

BASE_CCFILES=DiscreteDistribution.cc NormalDistribution.cc NormalDiscreteDistribution.cc MDDiscreteDistribution.cc BspTree.cc ContinuousTransition.cc Alg.cc BspTreeOperations.cc BspTreeAlpha.cc ContinuousReward.cc AlphaVector.cc PiecewiseConstantReward.cc PiecewiseLinearReward.cc HybridTransitionOutcome.cc HybridTransition.cc ValueFunction.cc PiecewiseConstantValueFunction.cc PiecewiseLinearValueFunction.cc ValueFunctionOperations.cc ContinuousOutcome.cc BackupOperations.cc ContinuousStateDistribution.cc

if LP
BASE_CCFILES+=LpSolve5.cc Lp.h
endif

lib_LIBRARIES=libHmdpBase.a $(LIB_LPSOLVE5)
AM_CPPFLAGS=-I../csa $(LP5_INC)
AM_CXXFLAGS=-Wall -g -std=c++11
libHmdpBase_a_SOURCES=$(BASE_CCFILES)
