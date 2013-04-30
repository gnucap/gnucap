/*$Id: measure_min.cc,v 26.131 2009/11/20 08:22:10 al Exp $ -*- C++ -*-
 * Copyright (C) 2008 Albert Davis
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
 */
#include "u_parameter.h"
#include "m_wave.h"
#include "u_function.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class MEASURE : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const
  {
    std::string probe_name;
    PARAMETER<double> before(BIGBIG);
    PARAMETER<double> after(-BIGBIG);
    bool last = false;
    bool arg = false;

    unsigned here = Cmd.cursor();
    Cmd >> probe_name;
    WAVE* w = find_wave(probe_name);

    if (!w) {
      Cmd.reset(here);
    }else{
    }

    here = Cmd.cursor();
    do {
      ONE_OF
	|| Get(Cmd, "probe",  &probe_name)
	|| Get(Cmd, "before", &before)
	|| Get(Cmd, "after",  &after)
	|| Get(Cmd, "end",    &before)
	|| Get(Cmd, "begin",  &after)
	|| Set(Cmd, "arg",    &arg, true)
	|| Set(Cmd, "last",   &last, true)
	|| Set(Cmd, "first",  &last, false)
	;
    }while (Cmd.more() && !Cmd.stuck(&here));

    if (!w) {
      w = find_wave(probe_name);
    }else{
    }
    
    if (w) {
      before.e_val(BIGBIG, Scope);
      after.e_val(-BIGBIG, Scope);

      double time = (last) ? -BIGBIG : BIGBIG;
      double m = BIGBIG;
      WAVE::const_iterator begin = lower_bound(w->begin(), w->end(), DPAIR(after, -BIGBIG));
      WAVE::const_iterator end   = upper_bound(w->begin(), w->end(), DPAIR(before, BIGBIG));
      for (WAVE::const_iterator i = begin; i < end; ++i) {
	double val = i->second;
	if (val < m || (last && (val == m))) {
	  time = i->first;
	  m = val;
	}else{
	}
      }
      return to_string((arg) ? (time) : (m));
    }else{
      throw Exception_No_Match(probe_name);
    }
  }
} p2;
DISPATCHER<FUNCTION>::INSTALL d2(&measure_dispatcher, "min", &p2);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
