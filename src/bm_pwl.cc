/*$Id: bm_pwl.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * HSPICE compatible PWL
 */
#include "ap.h"
#include "m_interp.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
/*--------------------------------------------------------------------------*/
EVAL_BM_PWL::EVAL_BM_PWL(int c)
  :EVAL_BM_ACTION_BASE(c),
   _delta(_default_delta),
   _smooth(_default_smooth)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_PWL::EVAL_BM_PWL(const EVAL_BM_PWL& p)
  :EVAL_BM_ACTION_BASE(p),
   _delta(p._delta),
   _smooth(p._smooth),
   _table(p._table)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    int paren = cmd.skiplparen();
    for (;;){
      double key  =NOT_VALID;
      double value=NOT_VALID;
      cmd >> key >> value;
      if (cmd.stuck(&here)){
	break;
      }
      std::pair<double,double> p(key,value);
      _table.push_back(p);
    }
    paren -= cmd.skiprparen();
    if (paren != 0){
      cmd.warn(bWARNING, "need )");
    }
    get(cmd, "Delta",  &_delta);
    get(cmd, "SMooth", &_smooth);
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::print(OMSTREAM& where)const
{
  where << "  " << name() << '(';
  for (std::vector<std::pair<double,double> >::const_iterator
	 p = _table.begin();  p != _table.end();  ++p){
    where << "  " << p->first << ',' << p->second;
  }
  where << ')';
  print_base(where);
  if (_delta  != _default_delta)  {where << "  delta=" << _delta;  untested();}
  if (_smooth != _default_smooth) {where << "  smooth="<< _smooth; untested();}
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::tr_eval(ELEMENT* d)const
{
  double ext = (d->is_source()) ? 0. : NOT_INPUT;
  d->_y0 = interpolate(_table.begin(),_table.end(),ioffset(d->_y0.x),ext,ext);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
