/*$Id: bm_cond.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * behavioral modeling conditionals
 * The conditions refer to types of analysis, type SIM_MODE in mode.h.
 * An array "_func" stores handles to commons specific to each mode.
 */
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
static EVAL_BM_VALUE func_zero(CC_STATIC);
/*--------------------------------------------------------------------------*/
EVAL_BM_COND::EVAL_BM_COND(int c)
  :EVAL_BM_BASE(c)
{
  std::fill_n(_func, sCOUNT, static_cast<COMMON_COMPONENT*>(0));
  std::fill_n(_set, sCOUNT, false);
}
/*--------------------------------------------------------------------------*/
EVAL_BM_COND::EVAL_BM_COND(const EVAL_BM_COND& p)
  :EVAL_BM_BASE(p)
{
  untested();
  for (int i=0; i<sCOUNT; ++i) {
    _func[i] = 0;
    attach_common(p._func[i], &(_func[i]));
    _set[i]  = p._set[i];
  }
}
/*--------------------------------------------------------------------------*/
EVAL_BM_COND::~EVAL_BM_COND()
{
  for (int i=0; i<sCOUNT; ++i) {
    detach_common(&(_func[i]));
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COND::parse(CS& cmd)
{
  int here = cmd.cursor();
  cmd.reset().skipbl();
  bool is_source = cmd.match1("viVI") 
    || (cmd.match1('.') && (cmd.pmatch(".VSOurce") || cmd.pmatch(".ISOurce")));
  cmd.reset(here);

  do {
    SIM_MODE mode(sNONE);
    (   set(cmd, "AC",	     &mode, sAC)
     || set(cmd, "OP",	     &mode, sOP)
     || set(cmd, "DC",	     &mode, sDC)
     || set(cmd, "TRANsient",&mode, sTRAN)
     || set(cmd, "FOURier",  &mode, sFOURIER)
     || set(cmd, "ELSE",     &mode, sNONE)
     || set(cmd, "ALL",      &mode, sNONE)
     || (mode = sNONE));
    if (_set[mode]) {
      cmd.warn(bWARNING, (mode != sNONE) ? "duplicate mode" : "what's this?");
    }
    COMMON_COMPONENT* c = EVAL_BM_ACTION_BASE::parse_func_type(cmd);

    if (!c) {
      c = new EVAL_BM_MODEL;
    }

    {if (!c) {
      cmd.warn(bWARNING, "need function or value");
    }else{
      if (is_source		// Spice compatibility
	  && mode == sNONE	// for sources, no condition implies "DC"
	  && !_set[sDC]
	  && dynamic_cast<EVAL_BM_VALUE*>(c)) {
	mode = sDC;
      }
      c->parse(cmd);
      attach_common(c, &(_func[mode]));
      assert(_func[mode]);
      _set[mode] = true;
    }}
  } while (cmd.more() && !cmd.stuck(&here));
  
  // apply rules to determine states not explicitly specified
  if (!_func[sOP] && _set[sDC])    {attach_common(_func[sDC],  &(_func[sOP]));}
  if (!_func[sOP] && _set[sNONE])  {attach_common(_func[sNONE],&(_func[sOP]));}
  if (!_func[sOP] && _set[sTRAN])  {attach_common(_func[sTRAN],&(_func[sOP]));}
  if (!_func[sOP])		   {attach_common(&func_zero,  &(_func[sOP]));}
  
  if (!_func[sDC] && _set[sNONE])  {attach_common(_func[sNONE],&(_func[sDC]));}
  if (!_func[sDC] && _set[sOP])    {attach_common(_func[sOP],  &(_func[sDC]));}
  if (!_func[sDC] && _set[sTRAN])  {attach_common(_func[sTRAN],&(_func[sDC]));}
  if (!_func[sDC])		   {attach_common(&func_zero,  &(_func[sDC]));}

  if (!_func[sTRAN]&&_set[sNONE]){attach_common(_func[sNONE],&(_func[sTRAN]));}
  if (!_func[sTRAN] && _set[sDC])  {attach_common(_func[sDC],&(_func[sTRAN]));}
  if (!_func[sTRAN] && _set[sOP])  {attach_common(_func[sOP],&(_func[sTRAN]));}
  if (!_func[sTRAN])		   {attach_common(&func_zero,&(_func[sTRAN]));}

  if (!_func[sFOURIER])	      {attach_common(_func[sTRAN],&(_func[sFOURIER]));}

  const EVAL_BM_ACTION_BASE* c = 
    prechecked_cast<const EVAL_BM_ACTION_BASE*>(_func[sNONE]);
  if (!_func[sAC] && _set[sNONE] && (!is_source || c->ac_too()))
				   {attach_common(_func[sNONE],&(_func[sAC]));}
  if (!_func[sAC])		   {attach_common(&func_zero,  &(_func[sAC]));}
  
  for (int i = 1; i < sCOUNT; ++i) {
    assert(_func[i]);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COND::print(OMSTREAM& where)const
{
  if (_set[sNONE]) {
    _func[sNONE]->print(where);
  }
  for (int i = sCOUNT-1; i != sNONE; --i) {
    if (_set[i]) {
      where << "  " << static_cast<SIM_MODE>(i);
      _func[i]->print(where);
    }
  }
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* EVAL_BM_COND::deflate()
{
  // If all are the same, there is no use for the COND.
  // Return the one thing that is attached, so the caller can replace it.
  for (int i = 1; i < sCOUNT; ++i) {
    if (_func[i] != _func[sNONE]) {
      return this;
    }
  }
  return _func[sNONE]->deflate();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
