/*$Id: l_ftos.cc,v 22.17 2002/08/26 04:30:28 al Exp $ -*- C++ -*-
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
 * BUG:
 * returns a pointer to static space where the string is.
 * there is a finite pool, so repeated calls work, to a point.
 * after that, the space is overwritten, every POOLSIZE calls
 */
#include "l_lib.h"
#include "constant.h"
/*--------------------------------------------------------------------------*/
	char*	utos(unsigned,char*,int);
	char*	itos(int,char*,int,int);
	char*	ftos(double,int,int,int);
/*--------------------------------------------------------------------------*/
const int POOLSIZE = 100;
const int MAXLENGTH = 40;
static double ftos_floor = 1e-99;
/*--------------------------------------------------------------------------*/
char* utos(unsigned num, char *str, int len)
{
  assert(str);
  if (len==0) {
    return str;				/* reject zero length  */
  }

  int ii = std::abs(len);
  do{					/* build string starting at tail  */
    str[--ii] = static_cast<char>(num % 10 + '0');
  }while ( (num/=10)>0 && ii>0 );
  
  {if (len > 0) {			/* if right justify, fill with blank */
    while (ii > 0) {
      str[--ii] = ' ';
    }
  }else{
    int jj;				/* else if left justify, move left  */
    for (jj=0;  ii<-len;  ) {		/*    then fill with blanks	    */
      str[jj++] = str[ii++];
    }
    while (jj < -len) {
      str[jj++] = ' ';
    }
  }}
  return str;
}
/*--------------------------------------------------------------------------*/
std::string to_string(unsigned n)
{
  char s[100];
  sprintf(s, "%u", n);
  return s;
}
/*--------------------------------------------------------------------------*/
char* itos(int num, char *str, int len, int fmt)
{
  assert(str);
  char sign;
  {if (num < 0) {
    sign = '-';
    num = -num;
  }else if (fmt & ftos_SIGN) {
    sign = '+';
  }else{
    sign = ' ';
  }}
  
  utos(static_cast<unsigned>(num), &str[1], len);
  int ii;
  for (ii=1; str[ii]==' '; ++ii) {
    ;
  }
  str[ii-1] = sign;
  return str;
}
/*--------------------------------------------------------------------------*/
char* ftos(double num, int fieldwidth, int len, int fmt)
	/* num = number to convert			*/
	/* fieldwidth = size for fixed width		*/
	/* len = max length of new string		*/
	/* fmt = how to format it			*/
{
  if (len < 3) {
    len = 3;
  }
  if (len > MAXLENGTH-6) {
    len = MAXLENGTH-6;
  }
  if (fieldwidth > MAXLENGTH-1) {
    fieldwidth = MAXLENGTH-1;
  }

  int expo = 0;	/* exponent				*/
  int nnn = 0; 	/* char counter -- pos in string	*/

  char *str;
  {
    static char strpool[POOLSIZE][MAXLENGTH];	/* destination string pool */
    static int poolindex = 0;
    ++poolindex;
    if (poolindex >= POOLSIZE) {
      poolindex = 0;
    }
    str = strpool[poolindex];
  }
  
  {
    int iii;
    for (iii=0; iii<fieldwidth; ++iii) { /* build a clean blank string */
      str[iii] = ' ';
    }
    for (assert(iii==fieldwidth); iii<MAXLENGTH; ++iii) {
      str[iii] = '\0';
    }
  }
  
  {if (isinf(num) > 0) {
    strncpy(str," Over", 5);
  }else if (isinf(num) < 0) {
    strncpy(str,"-Over", 5);
  }else if (isnan(num)) {
    strncpy(str," NaN", 4);
  }else if (num == NOT_VALID) {
    strncpy(str," ??", 3);
  }else if (num == NOT_INPUT) {
    strncpy(str," NA", 3);
  }else if (num >= BIGBIG) {
    strncpy(str," Inf", 4);
  }else if (num <= -BIGBIG) {
    strncpy(str,"-Inf", 4);
  }else{
    if (std::abs(num) < ftos_floor) {	/* hide noise */
      num = 0.;
    }
    
    for (int iii=len+5; iii>=0; --iii) {
      str[iii] = ' ';			/* fill with blanks */
    }
    
    {if (num == 0.) {
      strcpy(str, " 0.");
      nnn = strlen(str);		/* num==0 .. build string 0.000...  */
      while (--len)
	str[nnn++] = '0';
      assert(expo == 0);
    }else{				/* num != 0 */
      {if (num < 0.) {
	str[0] = '-';			/* sign */
	num = -num;
      }else if (fmt & ftos_SIGN) {
	str[0] = '+';
      }else{
	assert(str[0] == ' ');
      }}
      
      expo = -3;
      while (num < .001) {		/* scale to .001 - 1.0 */
	num *= 1000.;
	expo -= 3;
      }
      while (num >= 1.) {
	num *= .001;
	expo += 3;
      }
      if ((fmt&ftos_EXP && expo<-9) || expo>10 || expo<-16) {
	--len;				/* one less digit if 'e' notation */
      }					/* and exp is 2 digits */
      if (len < 3) {
	++len;
      }
      
      {
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
      
      nnn = 1;
      {if (expo == -3) {		/* exp is -3. */
	int flg = 0;
	expo = 0;			/* print in fixed point, no exponent*/
	str[nnn++] = '0';
	str[nnn++] = '.';
	while (len > 0) {
	  num *= 10.;
	  int dig = static_cast<int>(floor(num));
	  num -= static_cast<double>(dig);
	  str[nnn++] = static_cast<char>(dig + '0');
	  if ((flg += dig))
	    --len;
	}
      }else{
	int flg = 0;
	for (int iii=2; len>0; --iii) {	/* mantissa			    */
	  num *= 10.;			/* get next digit		    */
	  int dig = static_cast<int>(floor(num));
	  num -= static_cast<double>(dig);/* subtract off last digit	    */
	  if ((flg += dig)) {		/* if int part !=0		    */
	    str[nnn++]=static_cast<char>(dig)+'0';/*(not all zeros so far)  */
	    --len;			/* stuff the digit into the string  */
	  }
	  if (iii==0) {			/* if we found the dec.pt. and	    */
	    str[nnn++] = '.';		/*   haven't used up all the space  */
	  }				/* put a dec.pt. in the string	    */
	}
      }}
    }}
    
    if (!(fmt&ftos_FILL)) {		/* supress trailing zeros */
      while (str[nnn-1]=='0') {
	str[--nnn] = static_cast<char>((nnn<fieldwidth) ? ' ' : '\0');
      }
    }
    
    if (expo == 0) {
      return str;
    }
    
    {if (fmt&ftos_EXP || expo>10 || expo<-16) {	/* if exponential format  */
      str[nnn++] = 'E';				/* put the letter 'E' and */
      {if (expo < 100) {			/* convert the exponent   */
	itos(expo, &str[nnn], -2, ftos_SIGN);
      }else{
	utos(static_cast<unsigned>(expo), &str[nnn], -3);
      }}
    }else{				   /* if letter-scale format	    */
      str[nnn++] = "fpnum KMGT"[(expo+15)/3];/* put the appropriate letter  */
    }}				/* note that letter-scale is not valid	    */
				/* for exp==-3 or exp not in -15..+12	    */
				/* this is trapped but letter-scale is also */
				/* not valid if exp not divisible by 3.     */
				/* This is not trapped, since it supposedly */
				/* cant happen.				    */
    if (str[nnn-1] == 'M') {
      str[nnn++] = 'e';
      str[nnn++] = 'g';
    }
  }}
  if (fieldwidth==0) {		/* BUG: this cleans up stray trailing	*/
    trim(str);			/* blanks.  I don't know why they exist	*/
  }
  
  return str;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
