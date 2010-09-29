/*$Id: bm_pwl.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * HSPICE compatible PWL
 */
//testing=script 2005.10.06
#include "e_elemnt.h"
#include "m_interp.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
/*--------------------------------------------------------------------------*/
EVAL_BM_PWL::EVAL_BM_PWL(int c)
  :EVAL_BM_ACTION_BASE(c),
   _delta(_default_delta),
   _smooth(_default_smooth),
   _raw_table(),
   _num_table()
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_PWL::EVAL_BM_PWL(const EVAL_BM_PWL& p)
  :EVAL_BM_ACTION_BASE(p),
   _delta(p._delta),
   _smooth(p._smooth),
   _raw_table(p._raw_table),
   _num_table(p._num_table)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PWL::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_PWL* p = dynamic_cast<const EVAL_BM_PWL*>(&x);
  bool rv = p
    && _delta == p->_delta
    && _smooth == p->_smooth
    && _raw_table == p->_raw_table
    && _num_table == p->_num_table
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::print(OMSTREAM& o)const
{
  o << ' ' << name() << '(';
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    o << p->first << ',' << p->second << ' ';
  }
  o << ')';
  if (_delta.has_value())  {o << " delta=" << _delta;  untested();}
  if (_smooth.has_value()) {o << " smooth="<< _smooth; untested();}
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _delta.e_val(_default_delta, par_scope);
  _smooth.e_val(_default_smooth, par_scope);
  double last = -BIGBIG;
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 iterator p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    p->first.e_val(0, par_scope);
    p->second.e_val(0, par_scope);
    if (last > p->first) {
      error(bWARNING, "%s: PWL is out of order: (%g, %g)\n",
	    c->long_label().c_str(), last, double(p->first));
    }else{
      DPAIR x(p->first, p->second);
      _num_table.push_back(x);
    }
    last = p->first;
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::tr_eval(ELEMENT* d)const
{
  double ext = (d->is_source()) ? 0. : NOT_INPUT;
  d->_y0 = interpolate(_num_table.begin(), _num_table.end(), 
		       ioffset(d->_y0.x), ext, ext);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PWL::parse_numlist(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  for (;;) {
    int start_of_pair = here;
    std::pair<PARAMETER<double>, PARAMETER<double> > p;
    //cmd >> key >> value;
    cmd >> p.first;
    if (cmd.stuck(&here)) {
      break;
    }else{
      cmd >> p.second;
      if (cmd.stuck(&here)) {
	cmd.reset(start_of_pair);
	break;
      }else{
	_raw_table.push_back(p);
      }
    }
  }
  if (cmd.gotit(start)) {
  }else{
    untested();
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PWL::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "Delta",  &_delta)
    || get(cmd, "SMooth", &_smooth)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
