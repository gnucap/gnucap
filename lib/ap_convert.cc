/*$Id: ap_convert.cc,v 26.138 2013/04/24 03:03:11 al Exp $ -*- C++ -*-
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
#include <fenv.h>
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
  size_t begin_string = cursor();
  size_t end_string = cursor();

  std::string s;
  std::string::size_type which_quote = find1(begin_quote);
  if (which_quote != std::string::npos) {
    int quotes = 1;
    skip(); // the quote
    begin_string = cursor();
    char the_begin_quote = begin_quote[which_quote];
    char the_end_quote = end_quote[which_quote];
    for (;;) {
      if (!ns_more()) {
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
	//skip();
      }else if (skip1('\\') && skip1(the_end_quote)) {
	end_string = cursor() - 2;
	s += _cmd.substr(begin_string, end_string-begin_string);
	begin_string = cursor() - 1;
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
  size_t here = cursor();
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
  size_t here = cursor();
  if (skip1("-")) {itested();
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
  size_t here = cursor();
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
  size_t here = cursor();
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
  size_t here = cursor();
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
  int    expon = 0;
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
    --expon;
  }

  if (skip1("eE")) {			// exponent: E form
    int expo = 0;
    int es = 1;
    if (skip1("-")) {
      es = -1;
    }else{
      skip1("+");
    }
    while (is_digit()) {
      expo = 10 * expo + (ctoc()-'0');
    }
    expon += expo * es;
  }else if ((OPT::units == uSPICE) && skip1("mM")) {		// M is special
    if (skip1("eE")) {			// meg
      expon += 6;
    }else if (skip1("iI")) {		// mil
      val *= 25.4e-6;
    }else{				// plain m (milli)
      expon -= 3;
    }
  }else if (skip1("M")) {itested();
    assert(OPT::units == uSI);
    expon += 6;
  }else if (skip1("m")) {
    assert(OPT::units == uSI);
    expon -= 3;
  }else if (skip1("uU")) {		// other letters
    expon -= 6;
  }else if (skip1("nN")) {
    expon -= 9;
  }else if (skip1("p")) {
    expon -= 12;
  }else if (skip1("P")) {
    expon += ((OPT::units == uSI) ? (15) : -12);
  }else if (skip1("fF")) {
    expon -= 15;
  }else if (skip1("aA")) { untested();
    expon -= 18;
  }else if (skip1("kK")) {
    expon += 3;
  }else if (skip1("gG")) {
    expon += 9;
  }else if (skip1("tT")) {
    expon += 12;
  }else if (skip1("%")) {untested();
    expon -= 2;
  }else{
  }
  while (is_alpha()) {			// skip letters
    skip();
  }
  skipcom();
  _ok = true;

  // powers of 10 representable as double
  static double pos_pwr[22] = {
    std::pow(10, 1), std::pow(10, 2), std::pow(10, 3), std::pow(10, 4),
    std::pow(10, 5), std::pow(10, 6), std::pow(10, 7), std::pow(10, 8),
    std::pow(10, 9), std::pow(10,10), std::pow(10,11), std::pow(10,12),
    std::pow(10,13), std::pow(10,14), std::pow(10,15), std::pow(10,16),
    std::pow(10,17), std::pow(10,18), std::pow(10,19), std::pow(10,20),
    std::pow(10,21), std::pow(10,22) };
  // negative powers, long double multiplication is faster than double division
  static long double neg_pwr[22] = {
    std::powl(10, -1), std::powl(10, -2), std::powl(10, -3), std::powl(10, -4),
    std::powl(10, -5), std::powl(10, -6), std::powl(10, -7), std::powl(10, -8),
    std::powl(10, -9), std::powl(10,-10), std::powl(10,-11), std::powl(10,-12),
    std::powl(10,-13), std::powl(10,-14), std::powl(10,-15), std::powl(10,-16),
    std::powl(10,-17), std::powl(10,-18), std::powl(10,-19), std::powl(10,-20),
    std::powl(10,-21), std::powl(10,-22)
  };

  // compute sign * val * 10^{expon}.
  double ret = sign * val;
  if(expon > 22){ // 38
    ret = double(ret * std::powl(10., expon));
  }else if(expon > 0){
    ret *= pos_pwr[expon-1];
  }else if(expon == 0){
    //
  }else if(expon > -23){
    ret = double(ret * neg_pwr[-expon-1]);
  }else{ // 20
    // fallback to simple long double (slow)
    ret = double(ret * std::powl(10., expon));
  }

#ifdef DEBUG
  // check against plain but slow long double arithmetics
  assert(ret == sign * double(val * std::powl(10., expon)));
  if(expon<0 && expon>-23){ itested();
    // division is also exact, but slower.
    assert(ret == sign * double(val / std::pow(10., -expon)));
  }else{ itested();
  }
#endif

  return ret;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
