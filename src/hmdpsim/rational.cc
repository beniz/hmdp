/* -*-C++-*- */
/*
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
#include "rational.h"
#include "exceptions.h"
#include <limits>
#include <math.h>
#include <stdlib.h>

namespace ppddl_parser
{

/* ====================================================================== */
/* Rational */

int Rational::iterations_ = 100;

/* Returns the greatest common devisor of the two integers. */
static int gcd(int n, int m) {
  int a = abs(n);
  int b = abs(m);
  while (b > 0) {
    int c = b;
    b = a % b;
    a = c;
  }
  return a;
}

/* Returns the least common multiplier of the two integers. */
static int lcm(int n, int m) {
  return n/gcd(n, m)*m;
}


/* Returns the multipliers for the two integers. */
std::pair<int, int> Rational::multipliers(int n, int m) {
  int f = lcm(n, m);
  return std::make_pair(f/n, f/m);
}


/* Constructs a rational number. */
Rational::Rational(int n, int m) {
  if (m == 0) {
    throw Exception("division by zero");
  } else {
    int d = gcd(n, m);
    numerator_ = n/d;
    denominator_ = m/d;
    if (denominator_ < 0) {
      numerator_ *= -1;
      denominator_ *= -1;
    }
  }
}

/* Constructs a rational number. */
Rational::Rational (const double &x)
{
  Rational r = Rational::toRational (x, Rational::iterations_);
  numerator_ = r.numerator ();
  denominator_ = r.denominator ();
}

Rational Rational::toRational (const double &x, int iterations)
{
  if (x == 0.0
      || x < std::numeric_limits<long>::min ()
      || x > std::numeric_limits<long>::max ())
    return Rational (0,1);
  else
    {
      int sign  = x < 0.0 ? -1 : 1;
      return sign * Rational::toRational (sign * x, 1.0e9, iterations); // was 1.0e12
    }
}

Rational Rational::toRational (const double &x, const double &limit, int iterations)
{
  double intpart;
  //std::cout << "x: " << x << std::endl;
  double fractpart = modf(x, &intpart);
  
  //debug
  //std::cout << "fractpart: " << fractpart << std::endl;
  //debug

  double d = 1.0 / fractpart;
  int left = static_cast<int> (intpart);

  //debug
  /* std::cout << "left: " << left << " -- d: " << d 
     << " -- limit: " << limit << std::endl; */
  //debug

  if (d > limit || iterations == 0)
    return Rational (left, 1);
  else
    {
      Rational tempr = Rational::toRational (d, limit * 0.1, --iterations);
      Rational r = Rational (tempr.denominator (), tempr.numerator ());
      return Rational (left, 1) + r;
    }
}

/* Constructs a rational number. */
Rational::Rational(const char* s)
  : numerator_(0) {
  const char* si = s;
  for (; *si != '\0' && *si != '.' && *si != '/'; si++) {
    numerator_ = 10*numerator_ + (*si - '0');
  }
  if (*si == '/') {
    denominator_ = 0;
    for (si++; *si != '\0'; si++) {
      denominator_ = 10*denominator_ + (*si - '0');
    }
    if (denominator_ == 0) {
      throw Exception("division by zero");
    }
    int d = gcd(numerator_, denominator_);
    numerator_ /= d;
    denominator_ /= d;
  } else if (*si == '.') {
    int a = numerator_;
    numerator_ = 0;
    denominator_ = 1;
    for (si++; *si != '\0'; si++) {
      numerator_ = 10*numerator_ + (*si - '0');
      denominator_ *= 10;
    }
    int d = gcd(numerator_, denominator_);
    numerator_ /= d;
    denominator_ /= d;
    numerator_ += a*denominator_;
  } else {
    denominator_ = 1;
  }
}


/* Less-than comparison operator for rational numbers. */
bool operator<(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first < p.numerator()*m.second;
}


/* Less-than-or-equal comparison operator for rational numbers. */
bool operator<=(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first <= p.numerator()*m.second;
}


/* Equality comparison operator for rational numbers. */
bool operator==(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first == p.numerator()*m.second;
}


/* Inequality comparison operator for rational numbers. */
bool operator!=(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first != p.numerator()*m.second;
}


/* Greater-than-or-equal comparison operator for rational numbers. */
bool operator>=(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first >= p.numerator()*m.second;
}


/* Greater-than comparison operator for rational numbers. */
bool operator>(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return q.numerator()*m.first > p.numerator()*m.second;
}


/* Addition operator for rational numbers. */
Rational operator+(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return Rational(q.numerator()*m.first + p.numerator()*m.second,
		  q.denominator()*m.first);
}


/* Subtraction operator for rational numbers. */
Rational operator-(const Rational& q, const Rational& p) {
  std::pair<int, int> m =
    Rational::multipliers(q.denominator(), p.denominator());
  return Rational(q.numerator()*m.first - p.numerator()*m.second,
		  q.denominator()*m.first);
}


/* Multiplication operator for rational numbers. */
Rational operator*(const Rational& q, const Rational& p) {
  int d1 = gcd(q.numerator(), p.denominator());
  int d2 = gcd(p.numerator(), q.denominator());
  return Rational((q.numerator()/d1)*(p.numerator()/d2),
		  (q.denominator()/d2)*(p.denominator()/d1));
}


/* Division operator for rational numbers. */
Rational operator/(const Rational& q, const Rational& p) {
  if (p == 0) {
    throw Exception("division by zero");
  }
  int d1 = gcd(q.numerator(), p.numerator());
  int d2 = gcd(p.denominator(), q.denominator());
  return Rational((q.numerator()/d1)*(p.denominator()/d2),
		  (q.denominator()/d2)*(p.numerator()/d1));
}


/* Output operator for rational numbers. */
std::ostream& operator<<(std::ostream& os, const Rational& q) {
  os << q.numerator();
  if (q.denominator() != 1) {
    os << '/' << q.denominator();
  }
  return os;
}

} /* end of namespace */
