/*$Id: ap_convert.cc,v 26.125 2009/10/15 20:58:21 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
//testing=script 2006.07.17
#include "u_opt.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
//	char	 CS::ctoc();
//	void	 CS::ctostr(char* d, int l, const string& t);
//	string	 CS::ctos(const string& term);
//	int	 CS::ctoi();
//	unsigned CS::ctou();
//	int	 CS::ctoo();
//	int	 CS::ctox();
//	double	 CS::ctof();
/*--------------------------------------------------------------------------*/
/* ctoc: character input to character
 */
char CS::ctoc()
{
  char c=_cmd[_cnt];
  if(_cnt<=_length) {
    ++_cnt;
  }else{untested();
  }
  return c;
}
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
  for (ii = 0;  ii < len && !is_term(term);  ++ii) {
    des[ii] = ctoc();
  }
  des[ii] = '\0';

  while (!is_term(term)) {untested();
    skip();
  }
  skipcom();
}
/*--------------------------------------------------------------------------*/
std::string CS::ctos(const std::string& term, 
		     const std::string& begin_quote,
		     const std::string& end_quote,
		     const std::string& trap)
{
  assert(begin_quote.length() == end_quote.length());

  skipbl();
  unsigned begin_string = cursor();
  unsigned end_string = cursor();

  std::string s;
  std::string::size_type which_quote = find1(begin_quote);
  if (which_quote != std::string::npos) {
    int quotes = 1;
    skip(); // the quote
    begin_string = cursor();
    char the_begin_quote = begin_quote[which_quote];
    char the_end_quote = end_quote[which_quote];
    for (;;) {
      if (!ns_more()) {itested();
	end_string = cursor();
	warn(bDANGER, std::string("need ") + the_end_quote);
	break;
      }else if (skip1(the_end_quote)) {
	if (--quotes <= 0) {
	  end_string = cursor() - 1;
	  break;
	}else{
	}
      }else if (skip1(the_begin_quote)) {
	++quotes;
	skip();
      }else if (skip1('\\')) {
	end_string = cursor() - 1;
	s += _cmd.substr(begin_string, end_string-begin_string);
	begin_string = cursor();
	skip1(the_end_quote);
      }else{
	skip();
      }
    }
    s += _cmd.substr(begin_string, end_string-begin_string);
  }else{
    while(ns_more() && !is_term(term)) {
      skip();
    }
    if (match1(trap)) {untested();
      warn(bDANGER, "ap_convert trap-exit");
    }else{
    }
    end_string = cursor();
    s = _cmd.substr(begin_string, end_string-begin_string);
  }

  skipcom();
  _ok = end_string > begin_string;
  return s;
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
/* ctob: character input to bool
 * no match makes it true;
 * Mismatch belongs to next token
 */
bool CS::ctob()
{
  skipbl();
  unsigned here = cursor();
  bool val = true;
  ONE_OF
    || Set(*this, "1",       &val, true)
    || Set(*this, "0",       &val, false)
    || Set(*this, "t{rue}",  &val, true)
    || Set(*this, "f{alse}", &val, false)
    || Set(*this, "y{es}",   &val, true)
    || Set(*this, "n{o}",    &val, false)
    || Set(*this, "#t{rue}", &val, true)
    || Set(*this, "#f{alse}",&val, false)
    ;
  skipcom();
  _ok = cursor() > here;
  return val;
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
  unsigned here = cursor();
  if (skip1("-")) {
    sign = -1;
  }else{
    skip1("+");
  }

  while (is_digit()) {
    val = 10 * val + (ctoc()-'0');
  }
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
  unsigned val = 0;

  skipbl();
  unsigned here = cursor();
  while (is_digit()) {
    val = 10 * val + static_cast<unsigned>(ctoc()-'0');
  }
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
  unsigned here = cursor();
  while (is_digit()) {
    val = 8 * val + (ctoc()-'0');
  }
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
{untested();
  int val = 0;

  skipbl();
  unsigned here = cursor();
  while (is_xdigit()) {untested();
    if (is_digit()) {untested();
      val = 16 * val + (ctoc()-'0');
    }else{untested();
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
  }else{
  }

  if (skip1("-")) {			// sign
    sign = -1;
  }else{
    skip1("+");
  }

  while (is_digit()) {			// up to dec pt
    val = 10.0 * val + (ctoc()-'0');
  }
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
  }else if ((OPT::units == uSPICE) && skip1("mM")) {		// M is special
    if (skip1("eE")) {			// meg
      power *= 1e6;
    }else if (skip1("iI")) {		// mil
      power *= 25.4e-6;
    }else{				// plain m (milli)
      power *= 1e-3;
    }
  }else if (skip1("M")) {
    assert(OPT::units == uSI);
    power *= 1e6;
  }else if (skip1("m")) {
    assert(OPT::units == uSI);
    power *= 1e-3;
  }else if (skip1("uU")) {		// other letters
    power *= 1e-6;
  }else if (skip1("nN")) {
    power *= 1e-9;
  }else if (skip1("p")) {
    power *= 1e-12;
  }else if (skip1("P")) {
    power *= ((OPT::units == uSI) ? (1e15) : 1e-12);
  }else if (skip1("fF")) {
    power *= 1e-15;
  }else if (skip1("kK")) {
    power *= 1e3;
  }else if (skip1("gG")) {
    power *= 1e9;
  }else if (skip1("tT")) {
    power *= 1e12;
  }else if (skip1("%")) {untested();
    power *= 1e-2;
  }else{
  }
  while (is_alpha()) {			// skip letters
    skip();
  }
  skipcom();
  _ok = true;
  return (sign*val*power);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
