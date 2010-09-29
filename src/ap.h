/*$Id: ap.h,v 24.16 2004/01/11 02:47:28 al Exp $  -*- C++ -*-
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
 * stuff for the "ap" family of parsing functions
 */
#ifndef AP_H
#define AP_H
#include "md.h"
/*--------------------------------------------------------------------------*/
enum AP_MOD{
  mNONE,	/* nothing special */
  mSCALE,	/* scale it after reading */
  mOFFSET,	/* add an offset */
  mINVERT,	/* save 1 / the number */
  mPOSITIVE,	/* store absolute value */
  mOCTAL,	/* read the number in octal */
  mHEX		/* read the number in hex */
};

const bool ONE_OF = false;

class CS_FILE {};

class CS {
private:
  char* _name;
  char* _cmd;
  int  _cnt;
  bool _ok;
  int  _length;
  int  _begin_match;
  int  _end_match;
  static enum MATCH_STYLE {msPARTIAL, msIGNORE_CASE, msEXACT} _ms;
public:
  // control
  static void set_partial() {_ms = msPARTIAL;}
  static void set_ignore_case() {_ms = msIGNORE_CASE;}
  static void set_exact() {_ms = msEXACT;}

  // construction, destruction, and re-construction
  explicit    CS(CS_FILE, const std::string& name, int i=0);
  explicit    CS(const std::string& s,int i=0);
  explicit    CS(const CS& p);
  CS&	      operator=(const std::string& s);
  CS&	      operator=(const CS& p);
	      ~CS()		{delete [] _name; delete [] _cmd;}

  // status - non-consuming
  int	      cursor()const	{return _cnt;}
  bool        stuck(int* last)	{bool ok=*last<_cnt; *last=_cnt; return !ok;}
  bool        gotit(int last)	{return last<_cnt;}
	    operator bool()const{return _ok;}

  // get -- non-consuming
  const char* fullstring()const	{return _cmd;}
  const char* tail()const	{return &_cmd[_cnt];}
  char	      peek()const	{return _cmd[_cnt];}

  // status - may consume whitespace only
  bool	      ns_more()const	{return peek()!='\0';}
  bool	      more()		{skipbl(); return ns_more();}
  bool	      end()		{return !more();}

  // control
  CS&	      reset(int c=0)	{_cnt=c; _ok=true; return *this;}

  // exception handling (ap_error.cc) non-consuming
  CS&	      check(int, const std::string&);
  CS&	      warn(int, int, const std::string&);
  CS&         warn(int i, const std::string& s)	{return warn(i,cursor(), s);}

  // string matching (ap_match.cc) possibly consuming, sets _ok
  CS&	      pmatch(const std::string&);	// partial match
  CS&	      icmatch(const std::string&);	// ignore case match
  CS&	      ematch(const std::string&);	// exact match
  CS&	      dmatch(const std::string&);	// match in default style
  CS&	      pscan(const std::string&);
  CS&	      icscan(const std::string&);
  CS&	      escan(const std::string&);
  CS&	      dscan(const std::string&);
  std::string      last_match()const;

  // character tests - non-consuming, no _ok
  bool	      match1(char c)const{return (peek()==c);}
  bool	      match1(const std::string& c)const
			{return ns_more() && strchr(c.c_str(),peek());}
  bool	      is_xdigit()const
		{untested(); return (match1("0123456789abcdefABCDEF"));}
  bool	      is_digit()const	{return (match1("0123456789"));}
  bool	      is_pfloat()const	{return (match1(".0123456789"));}
  bool	      is_float()const	{return (match1("+-.0123456789"));}
  bool	      is_argsym()const	{return (match1("*?$%_&@"));}
  bool	      is_alpha()const	{return !!isalpha(toascii(peek()));}
  bool	      is_term(const std::string& t = ",=(){};")
	{char c=peek(); return (c=='\0' || isspace(c) || match1(t));}

  // conversions (ap_convert.cc) always consuming
  char	      ctoc() {char c=_cmd[_cnt]; if(_cnt<=_length) {++_cnt;} return c;}
  void        ctostr(char*,int,const std::string&);
  std::string ctos(const std::string& term=",=(){};", char b='"', char e='"');
  std::string get_to(const std::string& term);

  // conversions (ap_convert.cc) consumes if successful, sets _ok
  double      ctof();
  int	      ctoi();
  unsigned    ctou();
  int	      ctoo();
  int	      ctox();
  double      ctopf()			{return std::abs(ctof());}
  CS&	      operator>>(char&x)	{x=ctoc();return *this;}
  CS&         operator>>(int&x)		{x=ctoi();return *this;}
  CS&         operator>>(unsigned&x)	{untested(); x=ctou();return *this;}
  CS&         operator>>(double&x)	{x=ctof();return *this;}
  CS&	      operator>>(std::string&x) {x=ctos();return *this;}

  // skip (ap_skip.cc) possibly consuming, sets _ok
  CS&	      skip(int c=1)	{_cnt+=c; _ok=_cnt<=_length; return *this;}
  CS&	      skipbl();
  CS&	      skip1b(char);
  CS&	      skip1(char);
  CS&	      skip1b(const std::string&);
  CS&	      skip1(const std::string&);
  CS&	      skiparg();
  CS&	      skipto1(const std::string&);
  CS&	      skipto1(char);
  CS&	      skipcom()		{return skip1b(",=");}
  CS&	      skiplparen()	{return skip1b("([");}
  CS&	      skiprparen()	{return skip1b(")]");}
  CS&	      skipequal()	{return skip1b("=");}
};	
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// these are non-member to provide a consistent interface,
// like the templates to follow
bool get(CS& cmd, const std::string&, bool*, AP_MOD=mNONE);
bool get(CS& cmd, const std::string&, int*,  AP_MOD=mNONE, int=0);
bool get(CS& cmd, const std::string& key, double* val, AP_MOD, double=0);
/*--------------------------------------------------------------------------*/
template <class T>
inline bool get(CS& cmd, const std::string& key, T* val)
{
  {if (cmd.dmatch(key)) {
    cmd >> *val;
    return true;
  }else{
    return false;
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
inline bool scan_get(CS& cmd, const std::string& key, T* val)
{
  {if (cmd.dscan(key)) {
    cmd >> *val;
    return true;
  }else{
    return false;
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
inline bool set(CS& cmd, const std::string& key, T* val, T newval)
{
  {if (cmd.dmatch(key)) {
    *val = newval;
    return true;
  }else{
    return false;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
