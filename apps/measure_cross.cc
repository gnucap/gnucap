/*$Id: measure_cross.cc,v 26.131 2009/11/20 08:22:10 al Exp $ -*- C++ -*-
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
    PARAMETER<double> cross(0.);
    int slope = 1;
    bool last = false;
    
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
	|| Get(Cmd, "cross",  &cross)
	|| Set(Cmd, "rise",   &slope, 1)
	|| Set(Cmd, "fall",   &slope, -1)
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
      cross.e_val(0., Scope);

      double cross1 = cross * slope;
      enum STAT {WAITING, READY, DONE} stat = WAITING;
      double x_time = (last) ? -BIGBIG : BIGBIG;
      WAVE::const_iterator begin = lower_bound(w->begin(), w->end(), DPAIR(after, -BIGBIG));
      WAVE::const_iterator end   = upper_bound(w->begin(), w->end(), DPAIR(before, BIGBIG));
      WAVE::const_iterator lower = begin;
      for (WAVE::const_iterator i = begin; i < end && stat != DONE; ++i) {
	double val = i->second * slope;
	switch (stat) {
	case WAITING:
	  if (val < cross1) {
	    stat = READY;
	    lower = i;
	  }else{
	  }
	  break;
	case READY:
	  if (val > cross1) {
	    stat = (last) ? WAITING : DONE;
	    WAVE::const_iterator upper = i;
	    double position = (cross1*slope - lower->second) / (upper->second - lower->second);
	    x_time = lower->first + position * (upper->first - lower->first);
	  }else{
	    lower = i;
	  }
	  break;
	case DONE:
	  break;
	};
      }
      return to_string(x_time);
    }else{
      throw Exception_No_Match(probe_name);
    }
  }
} p4;
DISPATCHER<FUNCTION>::INSTALL d4(&measure_dispatcher, "cross", &p4);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
