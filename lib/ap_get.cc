/*$Id: ap_get.cc,v 26.85 2008/06/19 05:01:15 al Exp $ -*- C++ -*-
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
 * get value for matching keyword
 */
//testing=script 2006.07.17
#include "ap.h"
/*--------------------------------------------------------------------------*/
/* special version of "get" for "bool"
 * so "nofoo" works as an equivalent to foo=false
 */
bool Get(CS& cmd, const std::string& key, bool* val)
{
  if (cmd.umatch(key + ' ')) {
    if (cmd.skip1b('=')) {itested();
      cmd >> *val;
    }else{
      *val = true;
    }
    return true;
  }else if (cmd.umatch("no" + key)) {
    *val = false;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, int* val, AP_MOD mod, int scale)
{
  if (cmd.umatch(key + " {=}")) {
    switch(mod) {
    case mNONE:			*val = int(cmd.ctof());		break;
    case mSCALE:    untested(); *val = int(cmd.ctof())*scale;	break;
    case mOFFSET:   untested(); *val = int(cmd.ctof())+scale;	break;
    case mINVERT:   untested(); *val = 1 / int(cmd.ctof());	break;
    case mPOSITIVE: untested(); *val = std::abs(int(cmd.ctof())); break;
    case mOCTAL:		*val = cmd.ctoo();		break;
    case mHEX:      untested(); *val = cmd.ctox();		break;
    }
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, double* val, AP_MOD mod, double scale)
{
  if (cmd.umatch(key + " {=}")) {
    switch(mod) {
    case mNONE:	    untested();	*val = cmd.ctof();		break;
    case mSCALE:    untested(); *val = cmd.ctof()*scale;	break;
    case mOFFSET:   untested(); *val = cmd.ctof()+scale;	break;
    case mINVERT:   untested(); *val = 1 / cmd.ctof();		break;
    case mPOSITIVE:		*val = std::abs(cmd.ctof());	break;
    case mOCTAL:    untested();	*val = cmd.ctoo();		break;
    case mHEX:      untested(); *val = cmd.ctox();		break;
    }
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
