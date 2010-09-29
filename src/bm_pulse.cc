/*$Id: bm_pulse.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * SPICE compatible PULSE
 */
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_iv    (NOT_INPUT);
const double _default_pv    (NOT_INPUT);
const double _default_delay (0);
const double _default_rise  (0);
const double _default_fall  (0);
const double _default_width (BIGBIG);
const double _default_period(BIGBIG);
/*--------------------------------------------------------------------------*/
EVAL_BM_PULSE::EVAL_BM_PULSE(int c)
  :EVAL_BM_ACTION_BASE(c),
   _iv(_default_iv),
   _pv(_default_pv),
   _delay(_default_delay),
   _rise(_default_rise),
   _fall(_default_fall),
   _width(_default_width),
   _period(_default_period),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_PULSE::EVAL_BM_PULSE(const EVAL_BM_PULSE& p)
  :EVAL_BM_ACTION_BASE(p),
   _iv(p._iv),
   _pv(p._pv),
   _delay(p._delay),
   _rise(p._rise),
   _fall(p._fall),
   _width(p._width),
   _period(p._period),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    int paren = cmd.skiplparen();
    double* i;
    for (i = &_iv;  i < &_end;  ++i){
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
    get(cmd, "IV",	&_iv);
    get(cmd, "PV",	&_pv);
    get(cmd, "DELAY",	&_delay);
    get(cmd, "RISE",	&_rise);
    get(cmd, "FALL",	&_fall);
    get(cmd, "WIDTH",	&_width);
    get(cmd, "PERIOD",	&_period);
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  if (_width == 0.) {
    _width = _default_width;
  }
  if (_period == 0.) {
    _period = _default_period;
  }
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::print(OMSTREAM& where)const
{
  where << "  " << name()
	<< "  iv="	<< _iv
	<< "  pv="	<< _pv
	<< "  delay="	<< _delay
	<< "  rise="	<< _rise
	<< "  fall="	<< _fall
	<< "  width="	<< _width
	<< "  period="	<< _period;
  print_base(where);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::tr_eval(ELEMENT* d)const
{
  double time = SIM::time0;
  if (0 < _period && _period < BIGBIG){
    time = fmod(time,_period);
  }
  
  double ev = 0; // effective value
  {if (time > _delay+_rise+_width+_fall){	/* past pulse	*/
    ev = _iv;
  }else if (time > _delay+_rise+_width){	/* falling 	*/
    double interp = (time - (_delay+_rise+_width)) / _fall;
    ev = _pv + interp * (_iv - _pv);
  }else if (time > _delay+_rise){		/* pulse val 	*/
    ev = _pv;
  }else if (time > _delay){			/* rising 	*/
    double interp = (time - _delay) / _rise;
    ev = _iv + interp * (_pv - _iv);
  }else{					/* init val	*/
    ev = _iv;
  }}

  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
