/*$Id: bm_cond.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * behavioral modeling conditionals
 * The conditions refer to types of analysis, type SIM_MODE in mode.h.
 * An array "_func" stores handles to commons specific to each mode.
 */
//testing=script 2006.07.13
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
bool EVAL_BM_COND::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_COND* p = dynamic_cast<const EVAL_BM_COND*>(&x);
  bool rv = p
    && EVAL_BM_BASE::operator==(x);
  if (rv) {untested();
    incomplete();
  }
  return rv;
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
    ONE_OF
      || set(cmd, "AC",	     &mode, sAC)
      || set(cmd, "OP",	     &mode, sOP)
      || set(cmd, "DC",	     &mode, sDC)
      || set(cmd, "TRANsient",&mode, sTRAN)
      || set(cmd, "FOURier",  &mode, sFOURIER)
      || set(cmd, "ELSE",     &mode, sNONE)
      || set(cmd, "ALL",      &mode, sNONE)
      || (mode = sNONE)
      ;
    if (_set[mode]) {untested();
      cmd.warn(bWARNING, (mode != sNONE) ? "duplicate mode" : "what's this?");
    }


    COMMON_COMPONENT* c = EVAL_BM_ACTION_BASE::parse_func_type(cmd);

    if (!c) {
      // no match for func_type
      if (cmd.more()) {
	// if there is anything left, assume it's a "model", for now
	//c = new EVAL_BM_MODEL;
	if (cmd.match1("\"'{")) {
	  // quoted means it is a parameter or expression
	  c = new EVAL_BM_VALUE;
	}else{
	  // assume it's a "model", for now
	  // it might not be, but we fix later
	  c = new EVAL_BM_MODEL;
	}
      }
    }
    
    if (is_source		// Spice compatibility
	&& mode == sNONE	// for sources, no condition implies "DC"
	&& !_set[sDC]
	&& dynamic_cast<EVAL_BM_VALUE*>(c)) {
      mode = sDC;
    }
    if (c) {
      c->parse(cmd);
    }else{
      c = &func_zero;
    }
    assert(c);
    attach_common(c, &(_func[mode]));
    assert(_func[mode]);
    _set[mode] = true;
    if (cmd.is_end()) {
      break;
    }
  } while (cmd.more() && !cmd.stuck(&here));
  
  // apply rules to determine states not explicitly specified
  if (!_func[sOP] && _set[sDC])    {attach_common(_func[sDC],  &(_func[sOP]));}
  if (!_func[sOP] && _set[sNONE])  {attach_common(_func[sNONE],&(_func[sOP]));}
  if (!_func[sOP] && _set[sTRAN])  {untested();attach_common(_func[sTRAN],&(_func[sOP]));}
  if (!_func[sOP])		   {attach_common(&func_zero,  &(_func[sOP]));}
  
  if (!_func[sDC] && _set[sNONE])  {attach_common(_func[sNONE],&(_func[sDC]));}
  if (!_func[sDC] && _set[sOP])    {untested();attach_common(_func[sOP],  &(_func[sDC]));}
  if (!_func[sDC] && _set[sTRAN])  {untested();attach_common(_func[sTRAN],&(_func[sDC]));}
  if (!_func[sDC])		   {attach_common(&func_zero,  &(_func[sDC]));}

  if (!_func[sTRAN]&&_set[sNONE]){attach_common(_func[sNONE],&(_func[sTRAN]));}
  if (!_func[sTRAN] && _set[sDC])  {attach_common(_func[sDC],&(_func[sTRAN]));}
  if (!_func[sTRAN] && _set[sOP])  {untested();attach_common(_func[sOP],&(_func[sTRAN]));}
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
void EVAL_BM_COND::elabo3(const COMPONENT* c)
{
  for (int i = 1; i < sCOUNT; ++i) {
    // makes unnecessary duplicates
    assert(_func[i]);
    COMMON_COMPONENT* new_common = _func[i]->clone();
    new_common->elabo3(c);
    COMMON_COMPONENT* deflated_common = new_common->deflate();
    if (deflated_common != _func[i]) {
      attach_common(deflated_common, &(_func[i]));
    }else{untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COND::print(OMSTREAM& o)const
{
  if (_set[sNONE]) {
    _func[sNONE]->print(o);
  }
  for (int i = sCOUNT-1; i != sNONE; --i) {
    if (_set[i]) {
      o << "  " << static_cast<SIM_MODE>(i);
      _func[i]->print(o);
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
      // they are not all the same, don't deflate
      return this;
    }
  }
  // they are all the same.  Take one of them.
  return _func[sNONE]->deflate();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
