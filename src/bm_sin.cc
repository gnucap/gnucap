/*$Id: bm_sin.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * SPICE compatible SIN
 */
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_offset	(0);
const double _default_amplitude	(1);
const double _default_frequency (NOT_INPUT);
const double _default_delay	(0);
const double _default_damping	(0);
/*--------------------------------------------------------------------------*/
EVAL_BM_SIN::EVAL_BM_SIN(int c)
  :EVAL_BM_ACTION_BASE(c),
   _offset(_default_offset),
   _amplitude(_default_amplitude),
   _frequency(_default_frequency),
   _delay(_default_delay),
   _damping(_default_damping),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_SIN::EVAL_BM_SIN(const EVAL_BM_SIN& p)
  :EVAL_BM_ACTION_BASE(p),
   _offset(p._offset),
   _amplitude(p._amplitude),
   _frequency(p._frequency),
   _delay(p._delay),
   _damping(p._damping),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    int paren = cmd.skiplparen();
    double* i;
    for (i = &_offset;  i < &_end;  ++i){
      double value=NOT_VALID;
      cmd >> value;
      if (cmd.stuck(&here)){
	break;
      }
      *i = value;
    }
    assert(i <= &_end);
    paren -= cmd.skiprparen();
    if (paren != 0){
      untested();
      cmd.warn(bWARNING, "need )");
    }
    get(cmd, "Offset",	 &_offset);
    get(cmd, "Amplitude",&_amplitude);
    get(cmd, "Frequency",&_frequency);
    get(cmd, "DElay",	 &_delay);
    get(cmd, "DAmping",	 &_damping);
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::print(OMSTREAM& where)const
{
  where << "  " << name()
	<< "  offset="	 << _offset
	<< "  amplitude="<< _amplitude
	<< "  frequency="<< _frequency
	<< "  delay="	 << _delay
	<< "  damping="	 << _damping;
  print_base(where);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::tr_eval(ELEMENT* d)const
{
  double reltime = ioffset(SIM::time0);
  double ev = _offset;
  if (reltime > _delay){
    double x = _amplitude * fixzero(sin(kPIx2*_frequency*(reltime-_delay)),1.);
    if (_damping != 0.){
      x *= Exp(-(reltime-_delay)*_damping);
    }
    ev += x;
  }
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
