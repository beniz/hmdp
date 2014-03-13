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

if PPDDL
MAYBE_PPDDL_CPPFLAGS=-I../hmdpsim
else
MAYBE_PPDDL_LIB=
endif

lib_LIBRARIES=libHmdpLoaders.a
AM_CPPFLAGS=-I../base -I../csa -I../engine -I../hmdpsim
AM_CXXFLAGS=-Wall -g -std=c++11
libHmdpLoaders_a_SOURCES=HmdpWorld.cc HmdpPpddlLoader.cc
