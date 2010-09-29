/*$Id: bm_model.cc,v 24.21 2004/01/21 15:58:10 al Exp $ -*- C++ -*-
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
 * behavioral modeling ".model" stub
 * accepts an unknown name for later linking to a .model
 */
#include "e_model.h"
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
EVAL_BM_MODEL::EVAL_BM_MODEL(int c)
  :EVAL_BM_ACTION_BASE(c),
   _arglist(),
   _func(0)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_MODEL::EVAL_BM_MODEL(const EVAL_BM_MODEL& p)
  :EVAL_BM_ACTION_BASE(p),
   _arglist(p._arglist),
   _func(0)
{
  untested();
  attach_common(p._func, &_func);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_MODEL::parse(CS& cmd)
{
  assert(!_func);
  assert(!has_model());
  parse_modelname(cmd);
  _arglist = cmd.ctos("", '(', ')');
  assert(!_func);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_MODEL::print(OMSTREAM& where)const
{
  {if (_func) {
    _func->print(where);
  }else{
    where << "  " << modelname() << " (" << _arglist << ")";
  }}
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_MODEL::expand(const COMPONENT* d)
{
  const MODEL_CARD* m = attach_model(d);
  assert(m);
  EVAL_BM_ACTION_BASE* c = dynamic_cast<EVAL_BM_ACTION_BASE*>(m->new_common());
  if (!c) {
    untested();
    error(bERROR, d->long_label() + ": model type mismatch");
  }
  c->set_modelname(modelname());
  CS args(_arglist);
  c->parse(args);
  c->expand(d);
  assert(!_func);
  attach_common(c, &_func);
  assert(_func);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
