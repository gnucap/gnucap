/*$Id: ap_convert.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * get integer from string, in a variety of formats
 * update string pointer
 * return integer if got, else 0
 * pointer points to char following number just got
 * or first non-space
 */
#include "ap.h"
/*--------------------------------------------------------------------------*/
//	void	 CS::ctostr(char* d, int l, const string& t);
//	string	 CS::ctos(const string& term);
//	int	 CS::ctoi();
//	unsigned CS::ctou();
//	int	 CS::ctoo();
//	int	 CS::ctox();
//	double	 CS::ctof();
/*--------------------------------------------------------------------------*/
/* ctostr: character input to string
 * scan (and eat) an input string (cmd) using index (cnt).
 * result in (des)  (null terminated).
 * max length (actual char count) is (len).
 * (des) must be at least (len)+1 characters long.
 * (cmd) unchanged.  (*cnt) updated to point to next argument.
 * skips leading whitespace.  skips trailing whitespace and comma
 * skips parts of input word too big for destination
 */
void CS::ctostr(char* des, int len, const std::string& term)
{
  skipbl();
  int ii;
  {for (ii = 0;  ii < len && !is_term(term);  ii++) {
    des[ii] = ctoc();
  }}
  des[ii] = '\0';

  while (!is_term(term))
    skip();
  skipcom();
}
/*--------------------------------------------------------------------------*/
std::string CS::ctos(const std::string& term, char begin_quote, char end_quote)
{
  std::string des;
  skipbl();
  {if (skip1(begin_quote)) {
    while (ns_more() && !skip1(end_quote)) {
      {if (skip1('\\')) {
	{if (skip1(end_quote)) {
	  des += end_quote;
	}else{
	  des += '\\';
	}}
      }else{
	des += ctoc();
      }}
    }
  }else{
    while(ns_more() && !is_term(term)) {
      des += ctoc();
    }
  }}  
  skipcom();
  return des;
}
/*--------------------------------------------------------------------------*/
std::string CS::get_to(const std::string& term)
{
  std::string des;
  while(ns_more() && !match1(term)) {
    des += ctoc();
  }
  return des;
}
/*--------------------------------------------------------------------------*/
/* ctoi: character input to integer
 * Returns signed integer, or 0 if the string is not a number.
 * Input must be integer: no multipliers, no decimal point.
 * Dot or letter belongs to the next token.
 */
int CS::ctoi()
{
  int val = 0;
  int sign = 1;

  skipbl();
  int here = cursor();
  if (skip1("-")) {
    sign = -1;
  }else{
    skip1("+");
  }

  while (is_digit())
    val = 10 * val + (ctoc()-'0');

  skipcom();
  _ok = cursor() > here;
  return val * sign;
}
/*--------------------------------------------------------------------------*/
/* ctou: character input to unsigned integer
 * Returns unsigned integer, or 0 if the string is not a number.
 * Input must be integer: no multipliers, no decimal point.
 * Dot or letter belongs to the next token.
 */
unsigned CS::ctou()
{
  untested();
  int val = 0;

  skipbl();
  int here = cursor();
  while (is_digit())
    val = 10 * val + (ctoc()-'0');
  skipcom();
  _ok = cursor() > here;
  return val;
}
/*--------------------------------------------------------------------------*/
/* ctoo: character octal input to integer
 * Returns integer, or 0 if the string is not a number.
 * Input must be integer: no multipliers, no decimal point.
 * Dot or letter belongs to the next token.
 * There is no check against '8' and '9'.
 */
int CS::ctoo()
{
  int val = 0;

  skipbl();
  int here = cursor();
  while (is_digit())
    val = 8 * val + (ctoc()-'0');
  skipcom();
  _ok = cursor() > here;
  return val;
}
/*--------------------------------------------------------------------------*/
/* ctox: character hex input to unsigned integer
 * Returns integer, or 0 if the string is not a number.
 * Input must be hex integer: no multipliers, no decimal point.
 * Dot or letter belongs to the next token.
 */
int CS::ctox()
{
  untested();
  int val = 0;

  skipbl();
  int here = cursor();
  while (is_xdigit()) {
    if (is_digit()) {
      val = 16 * val + (ctoc()-'0');
    }else{
      val = 16 * val + (tolower(ctoc())-'a'+10);
    }
  }
  skipcom();
  _ok = cursor() > here;
  return val;
}
/*--------------------------------------------------------------------------*/
/* ctof: floating point input
 * return double number if got, else 0
 * supports letter multipliers (spice style)
 * skips trailing letters (10uhenries == 10u)
 * skips trailing spaces and one comma
 * pointer points to char following comma
 * or first non-space following number just got
 * or first non-space (if non-number)
 */
double CS::ctof()
{
  double val = 0.0;
  double power = 1.0;
  int    sign = 1;

  skipbl();
  if (!is_float()) {
    skipcom();
    _ok = false;
    return 0.;
  }

  if (skip1("-")) {			// sign
    sign = -1;
  }else{
    skip1("+");
  }

  while (is_digit())			// up to dec pt
    val = 10.0 * val + (ctoc()-'0');

  skip1(".");				// dec pt

  while (is_digit()) {			// after dec pt
    val = 10.0 * val + (ctoc()-'0');
    power *= .1;
  }

  if (skip1("eE")) {			// exponent: E form
    int expo = 0;
    int es = 1;
    if (skip1("-")) {
      es = -1;
    }else{
      skip1("+");
    }
    while (is_digit())
      expo = 10 * expo + (ctoc()-'0');
    expo *= es;
    power *= pow(10., expo);
  }else if (skip1("mM")) {		// M is special
    if (skip1("eE")) {			// meg
      power *= 1e6;
    }else if (skip1("iI")) {		// mil
      power *= 25.4e-6;
    }else{				// plain m (milli)
      power *= 1e-3;
    }
  }else if (skip1("uU")) {		// other letters
    power *= 1e-6;
  }else if (skip1("nN")) {
    power *= 1e-9;
  }else if (skip1("pP")) {
    power *= 1e-12;
  }else if (skip1("fF")) {
    power *= 1e-15;
  }else if (skip1("kK")) {
    power *= 1e3;
  }else if (skip1("gG")) {
    power *= 1e9;
  }else if (skip1("tT")) {
    power *= 1e12;
  }else if (skip1("%")) {
    power *= 1e-2;
  }
  while (is_alpha())			// skip letters
    skip();

  skipcom();
  _ok = true;
  return (sign*val*power);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
