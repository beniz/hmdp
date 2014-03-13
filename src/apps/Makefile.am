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

bin_PROGRAMS=
if PPDDL
bin_PROGRAMS+=hmdp
hmdp_SOURCES=hmdp.cc
endif

if LP
LP5_LD=-llpsolve55
else
LP5_LD=
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
