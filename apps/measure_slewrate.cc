/*$Id: measure_slewrate.cc,v 26.131 2009/11/20 08:22:10 al Exp $ -*- C++ -*-
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
#include "l_compar.h"
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
    PARAMETER<double> start_val;
    PARAMETER<double> stop_val;
    bool last = false;
    bool expression = false;

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
	|| Get(Cmd, "probe",       &probe_name)
	|| Get(Cmd, "before",      &before)
	|| Get(Cmd, "after",       &after)
	|| Get(Cmd, "end",         &before)
	|| Get(Cmd, "begin",       &after)
	|| Get(Cmd, "initial",     &start_val)
	|| Get(Cmd, "final",       &stop_val)
	|| Get(Cmd, "start_val",   &start_val)
	|| Get(Cmd, "to",	   &stop_val)
	|| Set(Cmd, "last",        &last, true)
	|| Set(Cmd, "first",	   &last, false)
	|| Set(Cmd, "expr{ession}",&expression, true)
	;
    }while (Cmd.more() && !Cmd.stuck(&here));

    if (!w) {
      w = find_wave(probe_name);
    }else{
    }
    
    if (w) {
      before.e_val(BIGBIG, Scope);
      after.e_val(-BIGBIG, Scope);
      start_val.e_val(0., Scope);
      stop_val.e_val(0., Scope);

      enum STAT {WAITING, READY, IN_RANGE, DONE} stat = WAITING;
      double try_start_time = BIGBIG;
      double start_time = BIGBIG;
      double stop_time = BIGBIG;
      WAVE::const_iterator begin = lower_bound(w->begin(), w->end(), DPAIR(after, -BIGBIG));
      WAVE::const_iterator end   = upper_bound(w->begin(), w->end(), DPAIR(before, BIGBIG));
      WAVE::const_iterator lower = begin;
      for (WAVE::const_iterator i = begin; i < end && stat != DONE; ++i) {
	double val = i->second;
	switch (stat) {
	case WAITING:
	  if (in_order(val, double(start_val), double(stop_val))) {
	    stat = READY;
	  }else{
	    // still WAITING
	  }
	  break;
	case READY:
	  if (in_order(val, double(start_val), double(stop_val))) {
	    // still READY
	  }else if (in_order(double(start_val), val, double(stop_val))) {
	    stat = IN_RANGE;
	    WAVE::const_iterator upper = i;
	    double position = (start_val - lower->second) / (upper->second - lower->second);
	    try_start_time = lower->first + position * (upper->first - lower->first);
	  }else if (in_order(double(start_val), double(stop_val), val)) {
	    // not accurate, was never IN_RANGE
	    stat = DONE;
	    double position = (start_val - lower->second) / (i->second - lower->second);
	    start_time = lower->first + position * (i->first - lower->first);
	    position = (stop_val - lower->second) / (i->second - lower->second);
	    stop_time = lower->first + position * (i->first - lower->first);
	  }else{untested();
	    // something is wrong
	  }
	  break;
	case IN_RANGE:
	  if (in_order(val, double(start_val), double(stop_val))) {
	    // false start
	    stat = READY;
	  }else if (in_order(double(start_val), val, double(stop_val))) {
	    // still IN_RANGE
	  }else if (in_order(double(start_val), double(stop_val), val)) {
	    stat = (last) ? WAITING : DONE;
	    start_time = try_start_time;
	    double position = (stop_val - lower->second) / (i->second - lower->second);
	    stop_time = lower->first + position * (i->first - lower->first);
	  }else{untested();
	    // something is wrong
	  }
	  break;
	case DONE:
	  break;
	};
	lower = i;
      }
      if (stop_time < BIGBIG) {
	assert(stop_time > start_time);
	if (expression) {
	  return "((" + to_string(stop_val) + "-" + to_string(start_val) + ")/(" 
	    + to_string(stop_time) + "-" + to_string(start_time) + "))";
	}else{
	  return to_string((stop_val-start_val)/(stop_time-start_time));
	}
      }else{
	return to_string(BIGBIG);
      }
    }else{
      throw Exception_No_Match(probe_name);
    }
  }
} p3;
DISPATCHER<FUNCTION>::INSTALL d3(&measure_dispatcher, "ddt|slewrate|slope", &p3);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
