/*$Id: bm_sffm.cc,v 24.16 2004/01/11 02:47:28 al Exp $ -*- C++ -*-
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
 * SPICE compatible SFFM
 */
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_offset	(0);
const double _default_amplitude	(1);
const double _default_carrier	(NOT_INPUT);
const double _default_modindex	(NOT_INPUT);
const double _default_signal	(NOT_INPUT);
/*--------------------------------------------------------------------------*/
EVAL_BM_SFFM::EVAL_BM_SFFM(int c)
  :EVAL_BM_ACTION_BASE(c),
   _offset(_default_offset),
   _amplitude(_default_amplitude),
   _carrier(_default_carrier),
   _modindex(_default_modindex),
   _signal(_default_signal),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_SFFM::EVAL_BM_SFFM(const EVAL_BM_SFFM& p)
  :EVAL_BM_ACTION_BASE(p),
   _offset(p._offset),
   _amplitude(p._amplitude),
   _carrier(p._carrier),
   _modindex(p._modindex),
   _signal(p._signal),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::parse_numlist(CS& cmd)
{
  int here = cmd.cursor();
  for (double* i = &_offset;  i < &_end;  ++i){
    double value=NOT_VALID;
    cmd >> value;
    {if (cmd.stuck(&here)){
      break;
    }else{
      untested();
    }}
    *i = value;
  }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SFFM::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "Offset",	&_offset)
    || get(cmd, "Amplitude",	&_amplitude)
    || get(cmd, "Carrier",	&_carrier)
    || get(cmd, "Modindex",	&_modindex)
    || get(cmd, "Signal",	&_signal)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::print(OMSTREAM& where)const
{
  where << "  " << name()
	<< "  offset="	 << _offset
	<< "  amplitude="<< _amplitude
	<< "  carrier="	 << _carrier
	<< "  modindex=" << _modindex
	<< "  signal="	 << _signal;
  print_base(where);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::tr_eval(ELEMENT* d)const
{
  double time = SIM::time0;
  double mod = (_modindex * sin(kPIx2*_signal*time));
  double ev = _offset + _amplitude * sin(kPIx2*_carrier*time + mod);
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
