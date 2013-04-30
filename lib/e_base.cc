/*$Id: e_base.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
//testing=script 2006.07.12
#include "u_sim_data.h"
#include "m_wave.h"
#include "u_prblst.h"
#include "u_xprobe.h"
#include "e_base.h"
/*--------------------------------------------------------------------------*/
static char fix_case(char c)
{
  return ((OPT::case_insensitive) ? (static_cast<char>(tolower(c))) : (c));
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::tr_probe_num(const std::string&)const {return NOT_VALID;}
XPROBE CKT_BASE::ac_probe_ext(const std::string&)const {return XPROBE(NOT_VALID, mtNONE);}
/*--------------------------------------------------------------------------*/
SIM_DATA sim_data;
SIM_DATA* CKT_BASE::_sim = &sim_data;
/*--------------------------------------------------------------------------*/
CKT_BASE::~CKT_BASE()
{
  trace1("~CKT_BASE", _probes);
  PROBE_LISTS::purge(this);
  trace1("", _probes);
  assert(_probes==0);
}
/*--------------------------------------------------------------------------*/
const std::string CKT_BASE::long_label()const
{
  //incomplete();
  std::string buffer(short_label());
  //for (const CKT_BASE* brh = owner(); exists(brh); brh = brh->owner()) {
  //  buffer += '.' + brh->short_label();
  //}
  return buffer;
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::probe_num(const std::string& what)const
{
  double x;
  if (_sim->analysis_is_ac()) {
    x = ac_probe_num(what);
  }else{
    x = tr_probe_num(what);
  }
  return (std::abs(x)>=1) ? x : floor(x/OPT::floor + .5) * OPT::floor;
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::ac_probe_num(const std::string& what)const
{
  size_t length = what.length();
  mod_t modifier = mtNONE;
  bool want_db = false;
  char parameter[BUFLEN+1];
  strcpy(parameter, what.c_str());

  if (length > 2  &&  Umatch(&parameter[length-2], "db ")) {
    want_db = true;
    length -= 2;
  }
  if (length > 1) { // selects modifier based on last letter of parameter
    switch (fix_case(parameter[length-1])) {
      case 'm': modifier = mtMAG;   length--;	break;
      case 'p': modifier = mtPHASE; length--;	break;
      case 'r': modifier = mtREAL;  length--;	break;
      case 'i': modifier = mtIMAG;  length--;	break;
      default:  modifier = mtNONE;		break;
    }
  }
  parameter[length] = '\0'; // chop
  
  // "p" is "what" with the modifier chopped off.
  // Try that first.
  XPROBE xp = ac_probe_ext(parameter);

  // If we don't find it, try again with the full string.
  if (!xp.exists()) {
    xp = ac_probe_ext(what);
    if (!xp.exists()) {
      // Still didn't find anything.  Print "??".
    }else{
      untested();
      // The second attempt worked.
    }
  }
  return xp(modifier, want_db);
}
/*--------------------------------------------------------------------------*/
/*static*/ double CKT_BASE::probe(const CKT_BASE *This, const std::string& what)
{
  if (exists(This)) {
    return This->probe_num(what);
  }else{				/* return 0 if doesn't exist */
    return 0.0;				/* happens when optimized models */
  }					/* don't have all parts */
}
/*--------------------------------------------------------------------------*/
/*static*/ WAVE* CKT_BASE::find_wave(const std::string& probe_name)
{
  int ii = 0;
  for (PROBELIST::const_iterator
       p  = PROBE_LISTS::store[_sim->_mode].begin();
       p != PROBE_LISTS::store[_sim->_mode].end();
       ++p) {
    if (wmatch(p->label(), probe_name)) {
      return &(_sim->_waves[ii]);
    }else{
    }
    ++ii;
  }
  return NULL;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
