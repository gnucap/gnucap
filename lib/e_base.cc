/*$Id: e_base.cc 2015/02/05 al $ -*- C++ -*-
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
//testing=script 2014.07.04
#include "u_sim_data.h"
#include "m_wave.h"
#include "u_prblst.h"
/*--------------------------------------------------------------------------*/
SIM_DATA* CKT_BASE::_sim = nullptr; 
PROBE_LISTS* CKT_BASE::_probe_lists = nullptr;
/*--------------------------------------------------------------------------*/
CKT_BASE::~CKT_BASE()
{
  if (has_attributes(id_tag())) {untested();
    unreachable();    // needs purge();
    erase_attributes(id_tag());
  }else{
  }
  assert(!has_attributes(id_tag()));
}
/*--------------------------------------------------------------------------*/
const std::string CKT_BASE::long_label()const
{
  //incomplete();
  std::string buffer(short_label());
  //for (const CKT_BASE* brh = owner(); exists(brh); brh = brh->owner()) {untested();
  //  buffer += '.' + brh->short_label();
  //}
  return buffer;
}
/*--------------------------------------------------------------------------*/
bool CKT_BASE::help(CS& Cmd, OMSTREAM& Out)const
{
  if (help_text() != "") {
    size_t here = Cmd.cursor();
    std::string keyword;
    Cmd >> keyword;
    CS ht(CS::_STRING, help_text());
    if (keyword == "") {
      Out << ht.get_to("@@");
    }else if (ht.scan("@@" + keyword + ' ')) {
      Out << ht.get_to("@@");
    }else if (keyword == "?") {
      while (ht.scan("@@")) {
	Out << "  " << ht.get_to("\n") << '\n';
      }
    }else{
      Cmd.warn(bWARNING, here, "no help on subtopic " + Cmd.substr(here));
    }
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*static*/ WAVE* CKT_BASE::find_wave(const std::string& probe_name)
{
  int ii = 0;
  for (PROBELIST::const_iterator
       p  = _probe_lists->store[_sim->_mode].begin();
       p != _probe_lists->store[_sim->_mode].end();
       ++p) {
    if (wmatch(p->label(), probe_name)) {
      return &(_sim->_waves[ii]);
    }else{
    }
    ++ii;
  }
  return nullptr;
}
/*--------------------------------------------------------------------------*/
bool CKT_BASE::operator!=(const std::string& n)const
{
  if(OPT::case_insensitive){
    return strcasecmp(_label.c_str(),n.c_str())!=0;
  }else{
    return strcmp(_label.c_str(),n.c_str())!=0;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
