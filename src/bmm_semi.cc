/*$Id: bmm_semi.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * behavioral modeling
 * Spice3 compatible "semiconductor resistor and capacitor"
 */
#include "ap.h"
#include "bmm_semi.h"
/*--------------------------------------------------------------------------*/
EVAL_BM_SEMI_BASE::EVAL_BM_SEMI_BASE(int c)
  :EVAL_BM_ACTION_BASE(c),
   _length(NOT_INPUT),
   _width(NOT_INPUT),
   _temp(NOT_INPUT),
   _value(NOT_INPUT)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_SEMI_BASE::EVAL_BM_SEMI_BASE(const EVAL_BM_SEMI_BASE& p)
  :EVAL_BM_ACTION_BASE(p),
   _length(p._length),
   _width(p._width),
   _temp(p._temp),
   _value(p._value)
{
  untested();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::parse(CS& cmd)
{
  int here = cmd.cursor();
  bool paren = cmd.skiplparen();
  do{
    if (paren) {
      {if (cmd.skiprparen()) {
	untested();
        break;
      }else{
	untested();
      }}
    }
    get(cmd, "L",	&_length);
    get(cmd, "W",	&_width);
    get(cmd, "Temp",	&_temp, mOFFSET, -ABS_ZERO);
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::print(OMSTREAM& o)const
{
  o << "  "   << modelname()
    << "  l=" << _length;
  if (_width != NOT_INPUT)
    o << "  w=" << _width;
  if (_temp != NOT_INPUT)
    o << "  temp=" << _temp + ABS_ZERO;
  print_base(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, _value);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_CAPACITOR::expand(const COMPONENT* d)
{
  const MODEL_SEMI_CAPACITOR* m 
    = dynamic_cast<const MODEL_SEMI_CAPACITOR*>(attach_model(d));
  if (!m) {
    untested();
    error(bERROR, d->long_label() + ": model " + modelname()
	  + " is not a semi-capacitor (C)\n");
  }
  double width = (_width == NOT_INPUT) ? m->_defw : _width;
  double eff_width = width - m->_narrow;
  double eff_length = _length - m->_narrow;
  if (eff_width < 0.) {
    untested();
    error(bWARNING, d->long_label() + ": " + modelname()
	  + ": effective width is negative\n");
  }
  if (eff_length < 0.) {
    untested();
    error(bWARNING, d->long_label() + ": " + modelname()
	  + ": effective length is negative\n");
  }

  _value = m->_cj * eff_length * eff_width
    + 2. * m->_cjsw * (eff_length + eff_width);

  double tempdiff = (_temp - m->_tnom);
  _value *= 1 + m->_tc1*tempdiff + m->_tc2*tempdiff*tempdiff;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_RESISTOR::expand(const COMPONENT* d)
{
  const MODEL_SEMI_RESISTOR* m 
    = dynamic_cast<const MODEL_SEMI_RESISTOR*>(attach_model(d));
  if (!m) {
    untested();
    error(bERROR, d->long_label() + ": " + "model " + modelname()
	  + " is not a semi-resistor (R)\n");
  }
  double width = (_width == NOT_INPUT) ? m->_defw : _width;
  double eff_width = width - m->_narrow;
  double eff_length = _length - m->_narrow;
  if (eff_width <= 0.) {
    untested();
    error(bERROR, d->long_label() + ": " + modelname()
	  + ": effective width is negative\n");
  }
  if (eff_length < 0.) {
    untested();
    error(bWARNING, d->long_label() + ": " + modelname()
	  + ": effective length is negative\n");
  }

  _value = m->_rsh * eff_length / eff_width;

  double tempdiff = (_temp - m->_tnom);
  _value *= 1 + m->_tc1*tempdiff + m->_tc2*tempdiff*tempdiff;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_SEMI_CAPACITOR::MODEL_SEMI_CAPACITOR()
  :MODEL_CARD(),
   _cj(0.),
   _cjsw(0.),
   _narrow(0.),
   _defw(1e-6),
   _tc1(0.),
   _tc2(0.)
{
  _tnom = OPT::tnom;
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_CAPACITOR::parse_front(CS& cmd)
{
  bool dummy;
  return set(cmd, "Cap", &dummy, true);
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::parse_params(CS& cmd)
{
  get(cmd, "TNOM", &_tnom, mOFFSET, -ABS_ZERO);
  get(cmd, "CJ", &_cj, mPOSITIVE);
  get(cmd, "CJSW", &_cjsw, mPOSITIVE);
  get(cmd, "NARROW", &_narrow);
  get(cmd, "DEFW", &_defw, mPOSITIVE);
  get(cmd, "TC1", &_tc1);
  get(cmd, "TC2", &_tc2);
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::print_front(OMSTREAM& o)const
{
  o << "  c";
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::print_params(OMSTREAM& o)const
{
  o << "  tnom=" << _tnom + ABS_ZERO;
  o << "  cj=" << _cj;
  o << "  cjsw=" << _cjsw;
  o << "  narrow=" << _narrow;
  o << "  defw=" << _defw;
  o << "  tc1=" << _tc1;
  o << "  tc2=" << _tc2;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_SEMI_RESISTOR::MODEL_SEMI_RESISTOR()
  :MODEL_CARD(),
   _rsh(NOT_INPUT),
   _narrow(0.),
   _defw(1e-6),
   _tc1(0.),
   _tc2(0.)
{
  _tnom = OPT::tnom;
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_RESISTOR::parse_front(CS& cmd)
{
  bool dummy;
  return set(cmd, "Res", &dummy, true);
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::parse_params(CS& cmd)
{
  get(cmd, "TNOM", &_tnom, mOFFSET, -ABS_ZERO);
  get(cmd, "RSH", &_rsh, mPOSITIVE);
  get(cmd, "NARROW", &_narrow);
  get(cmd, "DEFW", &_defw, mPOSITIVE);
  get(cmd, "TC1", &_tc1);
  get(cmd, "TC2", &_tc2);
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::print_front(OMSTREAM& o)const
{
  o << "  r";
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::print_params(OMSTREAM& o)const
{
  o << "  tnom=" << _tnom + ABS_ZERO;
  o << "  rsh=" << _rsh;
  o << "  narrow=" << _narrow;
  o << "  defw=" << _defw;
  o << "  tc1=" << _tc1;
  o << "  tc2=" << _tc2;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
