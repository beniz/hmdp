## Process this file with automake to produce Makefile.in
##
## Copyright (C) 2003 Carnegie Mellon University and Rutgers University
##
## Permission is hereby granted to distribute this software for
## non-commercial research purposes, provided that this copyright
## notice is included with any such distribution.
##
## THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
## EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
## SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
## ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
##
## Modified by Emmanuel Benazera beniz@droidnik.fr
##

lib_LIBRARIES=libppddl.a
libppddl_a_SOURCES=hashing.h exceptions.cc exceptions.h requirements.cc requirements.h rational.cc rational.h types.cc types.h terms.cc terms.h predicates.cc predicates.h functions.cc functions.h expressions.cc expressions.h formulas.cc formulas.h effects.cc effects.h actions.cc actions.h domains.cc domains.h problems.cc problems.h states.cc states.h probabilityDistribution.h probabilityDistribution.cc goals.h goals.cc parser.yy tokenizer.ll

bin_PROGRAMS = mdpsim test_rat
mdpsim_SOURCES = mdpsim.cc
mdpsim_LDADD = -lppddl
test_rat_SOURCES = test-rational.cc
test_rat_LDADD = -lppddl

CLEANFILES = logs/*
MAINTAINERCLEANFILES = parser.cc tokenizer.cc config.h.in~
EXTRA_DIST = getopt.c getopt1.c getopt.h comp.cfg examples logs PROTOCOL

AM_CXXFLAGS=-Wall -std=c++11
AM_LDFLAGS = @LDFLAGS@ -L.

AM_YFLAGS = --verbose -d
