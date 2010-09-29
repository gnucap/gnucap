/*$Id: e_base.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
#include "ap.h"
#include "s__.h"
#include "u_opt.h"
#include "u_prblst.h"
#include "u_xprobe.h"
#include "constant.h"
#include "e_base.h"
/*--------------------------------------------------------------------------*/
double CKT_BASE::tr_probe_num(CS&)const {return NOT_VALID;}
XPROBE CKT_BASE::ac_probe_ext(CS&)const {return XPROBE(NOT_VALID);}
/*--------------------------------------------------------------------------*/
BSMATRIX<double>  CKT_BASE::aa;
BSMATRIX<double>  CKT_BASE::lu;
BSMATRIX<COMPLEX> CKT_BASE::acx;
int	CKT_BASE::_devcount = 0;
double	CKT_BASE::_vmax =  .5;
double	CKT_BASE::_vmin = -.5;
/*--------------------------------------------------------------------------*/
CKT_BASE::~CKT_BASE()
{
  trace1("~CKT_BASE", _probes);
  PROBE_LISTS::purge(this);
  trace1("", _probes);
  assert(_probes==0);
  --_devcount;
}
/*--------------------------------------------------------------------------*/
void CKT_BASE::set_Label(const std::string& s)
{
  set_label(s);
  _label[0] = toupper(_label[0]);
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::probe_num(const std::string& what)const
{
  double x;
  if (SIM::mode == sAC){
    x = ac_probe_num(what);
  }else{
    x = tr_probe_num_str(what);
  }
  return (std::abs(x)>=1) ? x : rint(x/OPT::floor) * OPT::floor;
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::tr_probe_num_str(const std::string& what)const
{
  CS cmd(what);
  return tr_probe_num(cmd);
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::ac_probe_num(const std::string& what)const
{
  int length = what.length();
  mod_t modifier = mtNONE;
  bool want_db = false;
  char parameter[BUFLEN+1];
  strcpy(parameter, what.c_str());

  if (length > 2  &&  pmatch(&parameter[length-2], "DB")){
    want_db = true;
    length -= 2;
  }
  if (length > 1){ // selects modifier based on last letter of parameter
    switch (tolower(parameter[length-1])){
      case 'm': modifier = mtMAG;   length--;	break;
      case 'p': modifier = mtPHASE; length--;	break;
      case 'r': modifier = mtREAL;  length--;	break;
      case 'i': modifier = mtIMAG;  length--;	break;
      default:  modifier = mtNONE;		break;
    }
  }
  parameter[length] = '\0'; // chop
  
  CS p(parameter);
  XPROBE xp = ac_probe_ext(p); // chopped form
  if (!xp.OK()){
    CS w(what);
    xp = ac_probe_ext(w); // full form
  }
  return xp(modifier, want_db);
}
/*--------------------------------------------------------------------------*/
void CKT_BASE::set_limit(double v)
{
  if (v+.4 > _vmax) {
    _vmax = v+.5;
    error(bTRACE, "new max = %g, new limit = %g\n", v, _vmax);
  }
  if (v-.4 < _vmin) {
    _vmin = v-.5;
    error(bTRACE, "new min = %g, new limit = %g\n", v, _vmin);
  }
}
/*--------------------------------------------------------------------------*/
void CKT_BASE::clear_limit()
{
  _vmax = OPT::vmax;
  _vmin = OPT::vmin;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
