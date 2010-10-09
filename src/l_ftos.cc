/*$Id: l_ftos.cc,v 26.96 2008/10/09 05:36:27 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * float to string
 * builds string representing floating number
 * num = number to convert.
 * str = string to put it in.  Must be big enough, or else!!
 *	    Must have length at least len+6.
 *	    sig digits + dec.pt.(1) + sign(1) + exp(4)
 * len = max number of displayed digits. (left + right of dp)
 *	    (includes d.p. if 'e' notation and 2 digit exp)
 * fmt = format : 0	  = alpha for exp notation
 *		  FMTEXP  = exponential notation
 *		  FMTSIGN = always inlcude sign
 *		  FMTFILL = fill in zeros
 * //BUG//
 * returns a pointer to static space where the string is.
 * there is a finite pool, so repeated calls work, to a point.
 * after that, the space is overwritten, every POOLSIZE calls
 */
//testing=script 2005.10.11
#include "l_lib.h"
#include "constant.h"
/*--------------------------------------------------------------------------*/
	char*	ftos(double,int,int,int);
/*--------------------------------------------------------------------------*/
const int POOLSIZE = 100;
const int MAXLENGTH = 40;
static double ftos_floor = 1e-99;
/*--------------------------------------------------------------------------*/
std::string to_string(unsigned n)
{
  char s[100];
  sprintf(s, "%u", n);
  return s;
}
/*--------------------------------------------------------------------------*/
std::string to_string(int n)
{
  char s[100];
  sprintf(s, "%d", n);
  return s;
}
/*--------------------------------------------------------------------------*/
std::string to_string(double n)
{
  return ftos(n, 0, 7, 0);
}
/*--------------------------------------------------------------------------*/
char* ftos(double num, int fieldwidth, int len, int fmt)
	// num = number to convert
	// fieldwidth = size for fixed width, 0 for variable width
	// len = max length of new string
	// fmt = how to format it
{
  if (len < 3) {
    untested();
    len = 3;
  }
  if (len > MAXLENGTH-6) {
    untested();
    len = MAXLENGTH-6;
  }
  if (fieldwidth > MAXLENGTH-1) {
    untested();
    fieldwidth = MAXLENGTH-1;
  }
  
  char *str;
  { /* get a buffer from the pool */
    //BUG// It is possible to have too many buffers active
    // then the extras are overwritten, giving bad output
    // There are no known cases, but it is not checked.
    static char strpool[POOLSIZE][MAXLENGTH];
    static int poolindex = 0;
    ++poolindex;
    if (poolindex >= POOLSIZE) {
      poolindex = 0;
    }
    str = strpool[poolindex];
  }
  
  { /* build a clean blank string */
    int string_size = std::max(fieldwidth, len+6);
    for (int iii=0; iii<string_size; ++iii) {
      str[iii] = ' ';
    }
    for (int iii=string_size; iii<MAXLENGTH; ++iii) {
      str[iii] = '\0';
    }
  }
  
#ifdef HAS_NUMERIC_LIMITS
  if (num == std::numeric_limits<double>::infinity()) {
    untested();
    strncpy(str," Over", 5);
  }else if (num == -std::numeric_limits<double>::infinity()) {
    untested();
    strncpy(str,"-Over", 5);
  }else if (num == std::numeric_limits<double>::quiet_NaN()) {
    untested();
    strncpy(str," NaN", 4);
  }else if (num == std::numeric_limits<double>::signaling_NaN()) {
    untested();
    strncpy(str," NaN", 4);
  }else
#endif
  if (num == NOT_VALID) {
    strncpy(str," ??", 3);
  }else if (num == NOT_INPUT) {
    strncpy(str," NA", 3);
  }else if (num >= BIGBIG) {
    strncpy(str," Inf", 4);
  }else if (num <= -BIGBIG) {
    strncpy(str,"-Inf", 4);
  }else if (num != num) {
    strncpy(str," NaN", 4);
  }else{
    if (std::abs(num) < ftos_floor) {	/* hide noise */
      num = 0.;
    }
    
    int expo = 0;	/* exponent				*/
    int nnn = 0; 	/* char counter -- pos in string	*/
    if (num == 0.) {
      strcpy(str, " 0.");
      nnn = static_cast<int>(strlen(str)); /* num==0 .. build string 0.000...  */
      while (--len) {
	str[nnn++] = '0';
      }
      assert(expo == 0);
    }else{				/* num != 0 */
      { // insert sign
	if (num < 0.) {
	  str[0] = '-';
	  num = -num;
	}else if (fmt & ftos_SIGN) {
	  untested();
	  str[0] = '+';
	}else{
	  assert(str[0] == ' ');
	}
      }
      { // scale to .001 - 1.0.  adjust expo.
	expo = -3;
	while (num < .001) {
	  num *= 1000.;
	  expo -= 3;
	}
	while (num >= 1.) {
	  num *= .001;
	  expo += 3;
	}
      }
      { // adjust len to compensate for length of printed exponent
	if ((fmt&ftos_EXP && expo<-9) || expo>10 || expo<-16) {
	  --len;			/* one less digit if 'e' notation */
	}				/* and exp is 2 digits */
	if (len < 3) {
	  untested();
	  ++len;
	}
      }
      { // round to correct number of digits
	double rnd = .5 / pow(10., len); /* find amt to add to round */
	if (num < .01) {
	  rnd /= 100.;
	}else if (num < .1) {
	  rnd /= 10.;
	}
	num += rnd;			/* add it */
	if (num >= 1.) {
	  num *= .001;			/* created an extra digit: rescale */
	  expo += 3;
	}
      }
      { // build mantissa
	nnn = 1;
	if (expo == -3) {		/* .001 is preferable to 1e-3 */
	  int flg = 0;			/* print in fixed point, no exponent*/
	  expo = 0;
	  str[nnn++] = '0';
	  str[nnn++] = '.';
	  while (len > 0) {
	    num *= 10.;
	    int digit = static_cast<int>(floor(num));
	    num -= static_cast<double>(digit);
	    str[nnn++] = static_cast<char>(digit + '0');
	    if ((flg += digit)) {
	      --len;
	    }
	  }
	}else{
	  int flg = 0;
	  for (int iii=2; len>0; --iii) {/* mantissa			    */
	    num *= 10.;			/* get next digit		    */
	    int digit = static_cast<int>(floor(num));
	    num -= static_cast<double>(digit);/* subtract off last digit    */
	    if ((flg += digit)) {	/* if int part !=0		    */
	      str[nnn++]=static_cast<char>(digit+'0');/*(not all zeros so far)*/
	      --len;			/* stuff the digit into the string  */
	    }
	    if (iii==0) {		/* if we found the dec.pt. and	    */
	      str[nnn++] = '.';		/*   haven't used up all the space  */
	    }				/* put a dec.pt. in the string	    */
	  }
	}
      }
    }
    assert(nnn > 0);
    assert(str[nnn] == ' ' || str[nnn] == '\0');
    
    { // suppress trailing zeros
      if (!(fmt&ftos_FILL)) {
	while (str[--nnn]=='0') {
	  str[nnn] = static_cast<char>((nnn < fieldwidth) ? ' ' : '\0');
	}
	++nnn;
      }else{
	untested();
      }
    }
    
    { // append exponent
      if (expo == 0) {
	// nothing;
      }else if (fmt&ftos_EXP || expo>10 || expo<-16) {/* exponential format  */
	char c = str[nnn+4];
	sprintf(&str[nnn], ((expo < 100) ? "E%+-3d" : "E%3u"), expo);
	nnn+=4;
	str[nnn++] = c;
      }else{				   /* if letter-scale format	    */
	str[nnn++] = "fpnum KMGT"[(expo+15)/3];/* put the appropriate letter*/
      }				/* note that letter-scale is not valid	    */
				/* for exp==-3 or exp not in -15..+12	    */
				/* this is trapped but letter-scale is also */
				/* not valid if exp not divisible by 3.     */
				/* This is not trapped, since it supposedly */
				/* cant happen.				    */
      if (str[nnn-1] == 'M') {
	str[nnn++] = 'e';	/* Spice kluge "MEG" */
	str[nnn++] = 'g';
      }
    }
  }
  { // clean up trailing blanks
    if (fieldwidth==0) {
      trim(str);
    }
  }
  
  return str;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
