/*$Id: ap_get.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * get value for matching keyword
 */
#include "ap.h"
/*--------------------------------------------------------------------------*/
bool get(CS& cmd, const std::string& key, bool* val, AP_MOD m)
{
  assert(m==mNONE || m==mINVERT);
  std::string false_key("NO" + key);

  {if (cmd.dmatch(key)){
    /**/ set(cmd, "1",     val, true)
      || set(cmd, "0",     val, false)
      || set(cmd, "True",  val, true)
      || set(cmd, "False", val, false)
      || set(cmd, "Yes",   val, true)
      || set(cmd, "No",    val, false)
      || set(cmd, "#True", val, true)
      || set(cmd, "#False",val, false)
      || (*val = true);
    cmd.reset(cmd.cursor()); // cheat: set "ok" flag to true
    if (m==mINVERT){
      untested();
      *val = !*val;
    }
    return true;
  }else if (cmd.dmatch(false_key)){
    *val = (m==mINVERT);
    return true;
  }else{
    return false;
  }}
}
/*--------------------------------------------------------------------------*/
bool get(CS& cmd, const std::string& key, int* val, AP_MOD mod, int scale)
{
  {if (cmd.dmatch(key)){
    switch(mod){
      case mNONE:		  *val = int(cmd.ctof());	break;
      case mSCALE:    untested(); *val = int(cmd.ctof())*scale;	break;
      case mOFFSET:   untested(); *val = int(cmd.ctof())+scale;	break;
      case mINVERT:   untested(); *val = 1 / int(cmd.ctof());	break;
      case mPOSITIVE: untested(); *val = abs(int(cmd.ctof()));	break;
      case mOCTAL:		  *val = cmd.ctoo();		break;
      case mHEX:      untested(); *val = cmd.ctox();		break;
    }
    return true;
  }else{
    return false;
  }}
}
/*--------------------------------------------------------------------------*/
bool get(CS& cmd, const std::string& key,double* val,AP_MOD mod,double scale)
{
  {if (cmd.dmatch(key)){
    cmd >> *val;
    switch(mod){
    case mNONE: 	/*nothing*/		break;
    case mSCALE:	*val *= scale;		break;
    case mOFFSET:	*val += scale;		break;
    case mINVERT: untested(); *val = 1 / *val;	break;
    case mPOSITIVE:	*val = std::abs(*val);	break;
    case mOCTAL:	unreachable();		break;
    case mHEX:  	unreachable();		break;
    }
    return true;
  }else{
    return false;
  }}
}    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
