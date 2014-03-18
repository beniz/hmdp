/* -*-C++-*- */
/*
 * Rational numbers.
 *
 * Copyright (C) 2003 Carnegie Mellon University and Rutgers University
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 */

#ifndef RATIONAL_H
#define RATIONAL_H

#include <config.h>
#include <iostream>
#include <utility>

namespace ppddl_parser
{

/* ====================================================================== */
/* Rational */

/*
 * A rational number.
 */
class Rational {
public:
  /* Returns the multipliers for the two integers. */
  static std::pair<int, int> multipliers(int n, int m);

  /* Constructs a rational number. */
  Rational(int n = 0) : numerator_(n), denominator_(1) {}

  /* Constructs a rational number. */
  Rational(int n, int m);

  /* Constructs a rational number. */
  //Rational(const char* s);

  /* Constructs a rational number. */
  Rational(const double &x);
  
  static Rational toRational (const double &x, int iterations);

  static Rational toRational (const double &x, const double &limit, int iterations);

  /* Returns the numerator of this rational number. */
  int numerator() const { return numerator_; }

  /* Returns the denominator of this rational number. */
  int denominator() const { return denominator_; }

  /* Returns the double value of this rational number. */
  double double_value() const { return double(numerator())/denominator(); }

private:
  /* The numerator. */
  int numerator_;
  /* The denominator. */
  int denominator_;

  static int iterations_;  /**< max number of iterations for conversion from double */
};

/* Less-than comparison operator for rational numbers. */
bool operator<(const Rational& q, const Rational& p);

/* Less-than-or-equal comparison operator for rational numbers. */
bool operator<=(const Rational& q, const Rational& p);

/* Equality comparison operator for rational numbers. */
bool operator==(const Rational& q, const Rational& p);

/* Inequality comparison operator for rational numbers. */
bool operator!=(const Rational& q, const Rational& p);

/* Greater-than-or-equal comparison operator for rational numbers. */
bool operator>=(const Rational& q, const Rational& p);

/* Greater-than comparison operator for rational numbers. */
bool operator>(const Rational& q, const Rational& p);

/* Addition operator for rational numbers. */
Rational operator+(const Rational& q, const Rational& p);

/* Subtraction operator for rational numbers. */
Rational operator-(const Rational& q, const Rational& p);

/* Multiplication operator for rational numbers. */
Rational operator*(const Rational& q, const Rational& p);

/* Division operator for rational numbers. */
Rational operator/(const Rational& q, const Rational& p);

/* Output operator for rational numbers. */
std::ostream& operator<<(std::ostream& os, const Rational& q);

} /* end of namespace */

#endif /* RATIONAL_H */
