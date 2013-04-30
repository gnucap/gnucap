/*$Id: bm_cond.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * behavioral modeling conditionals
 * The conditions refer to types of analysis, type SIM_MODE in mode.h.
 * An array "_func" stores handles to commons specific to each mode.
 */
//testing=script 2006.07.13
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
static EVAL_BM_VALUE func_zero(CC_STATIC);
/*--------------------------------------------------------------------------*/
class EVAL_BM_COND : public EVAL_BM_BASE {
private:
  COMMON_COMPONENT* _func[sCOUNT];
  bool _set[sCOUNT];
  explicit	EVAL_BM_COND(const EVAL_BM_COND& p);
public:
  explicit	EVAL_BM_COND(int c=0);
		~EVAL_BM_COND();
private: // override virtual
  bool  operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_COND(*this);}
  void  parse_common_obsolete_callback(CS&);
  void  print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  
  void  	precalc_first(const CARD_LIST*);
  void		expand(const COMPONENT*);
  COMMON_COMPONENT* deflate();
  void  	precalc_last(const CARD_LIST*);

  void  tr_eval(ELEMENT*d)const
	{assert(_func[d->_sim->sim_mode()]); _func[d->_sim->sim_mode()]->tr_eval(d);}
  void  ac_eval(ELEMENT*d)const
	{assert(_func[s_AC]);	   _func[s_AC]->ac_eval(d);}
  TIME_PAIR tr_review(COMPONENT*d)
	{assert(_func[d->_sim->sim_mode()]); return _func[d->_sim->sim_mode()]->tr_review(d);}
  void  tr_accept(COMPONENT*d)
	{assert(_func[d->_sim->sim_mode()]); _func[d->_sim->sim_mode()]->tr_accept(d);}
  std::string name()const		{itested(); return "????";}
};
/*--------------------------------------------------------------------------*/
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
void EVAL_BM_COND::parse_common_obsolete_callback(CS& cmd) //used
{
  unsigned here = cmd.cursor();
  cmd.reset().skipbl();
  bool is_source = cmd.match1("viVI") 
    || (cmd.match1('.') && (cmd.umatch(".vso{urce} ") || cmd.umatch(".iso{urce} ")));
  cmd.reset(here);

  do {
    SIM_MODE mode(s_NONE);
    ONE_OF
      || Set(cmd, "ac", 	&mode, s_AC)
      || Set(cmd, "op", 	&mode, s_OP)
      || Set(cmd, "dc", 	&mode, s_DC)
      || Set(cmd, "tran{sient}",&mode, s_TRAN)
      || Set(cmd, "four{ier}",  &mode, s_FOURIER)
      || Set(cmd, "else",       &mode, s_NONE)
      || Set(cmd, "all",        &mode, s_NONE)
      || (mode = s_NONE)
      ;
    if (_set[mode]) {itested();
      cmd.warn(bWARNING, (mode != s_NONE) ? "duplicate mode" : "what's this?");
    }


    COMMON_COMPONENT* c = EVAL_BM_ACTION_BASE::parse_func_type(cmd);
    if (!c) {
      // no match for func_type
      if (cmd.more()) {
	if (!cmd.match1("\"'{")) {
	  // quoted means it is a parameter or expression
	  // otherwise assume it's a "model", for now
	  // it might not be, but we fix later
	  c = bm_dispatcher.clone("eval_bm_model");
	}else{
	}
	if (!c) {
	  c = bm_dispatcher.clone("eval_bm_value");
	}else{
	}
      }else{
	// no more
      }
    }else{
      // got bm function
    }
    
    if (is_source		// Spice compatibility
	&& mode == s_NONE	// for sources, no condition implies "DC"
	&& !_set[s_DC]
	&& dynamic_cast<EVAL_BM_VALUE*>(c)) {
      mode = s_DC;
    }else{
    }

    if (c) {
      c->parse_common_obsolete_callback(cmd); //BUG//callback
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
  if (!_func[s_OP] && _set[s_DC])    {attach_common(_func[s_DC],  &(_func[s_OP]));}
  if (!_func[s_OP] && _set[s_NONE])  {attach_common(_func[s_NONE],&(_func[s_OP]));}
  if (!_func[s_OP] && _set[s_TRAN])  {untested();attach_common(_func[s_TRAN],&(_func[s_OP]));}
  if (!_func[s_OP])		     {attach_common(&func_zero,  &(_func[s_OP]));}
  
  if (!_func[s_DC] && _set[s_NONE])  {attach_common(_func[s_NONE],&(_func[s_DC]));}
  if (!_func[s_DC] && _set[s_OP])    {untested();attach_common(_func[s_OP],  &(_func[s_DC]));}
  if (!_func[s_DC] && _set[s_TRAN])  {untested();attach_common(_func[s_TRAN],&(_func[s_DC]));}
  if (!_func[s_DC])		     {attach_common(&func_zero,  &(_func[s_DC]));}

  if (!_func[s_TRAN]&&_set[s_NONE])  {attach_common(_func[s_NONE],&(_func[s_TRAN]));}
  if (!_func[s_TRAN] && _set[s_DC])  {attach_common(_func[s_DC],&(_func[s_TRAN]));}
  if (!_func[s_TRAN] && _set[s_OP])  {untested();attach_common(_func[s_OP],&(_func[s_TRAN]));}
  if (!_func[s_TRAN])		     {attach_common(&func_zero,&(_func[s_TRAN]));}

  if (!_func[s_FOURIER])	     {attach_common(_func[s_TRAN],&(_func[s_FOURIER]));}

  const EVAL_BM_ACTION_BASE* c = prechecked_cast<const EVAL_BM_ACTION_BASE*>(_func[s_NONE]);

  if (!_func[s_AC] && _set[s_NONE] && (!is_source || c->ac_too()))
				   {attach_common(_func[s_NONE],&(_func[s_AC]));}
  if (!_func[s_AC])		   {attach_common(&func_zero,  &(_func[s_AC]));}
  
  for (int i = 1; i < sCOUNT; ++i) {
    assert(_func[i]);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COND::expand(const COMPONENT* d)
{
  EVAL_BM_BASE::expand(d);
  for (int i = 1; i < sCOUNT; ++i) {
    //BUG// makes unnecessary duplicates
    assert(_func[i]);
    COMMON_COMPONENT* new_common = _func[i]->clone();
    new_common->expand(d);
    COMMON_COMPONENT* deflated_common = new_common->deflate();
    if (deflated_common != _func[i]) {
      attach_common(deflated_common, &(_func[i]));
    }else{untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
// If all are the same, there is no use for the COND.
// Return the one thing that is attached, so the caller can replace it.
COMMON_COMPONENT* EVAL_BM_COND::deflate()
{
  for (int i = 1; i < sCOUNT; ++i) {
    if (_func[i] != _func[s_NONE]) {
      // they are not all the same, don't deflate
      return this;
    }
  }
  // they are all the same.  Take one of them.
  return _func[s_NONE]->deflate();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COND::precalc_first(const CARD_LIST* Scope)
{
  //BUG// calls the individual precalc more than once
  // wastes time and makes multiple "has no value" warnings
  // when there should be only one
  COMMON_COMPONENT* did_this = NULL;
  for (int i = 1; i < sCOUNT; ++i) {
    assert(_func[i]);
    if (_func[i] != did_this) {
      _func[i]->precalc_first(Scope);
      did_this = _func[i];
    }else{
      // already did
    }
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COND::precalc_last(const CARD_LIST* Scope)
{
  //BUG// calls the individual precalc more than once
  // wastes time and makes multiple "has no value" warnings
  // when there should be only one
  COMMON_COMPONENT* did_this = NULL;
  for (int i = 1; i < sCOUNT; ++i) {
    assert(_func[i]);
    if (_func[i] != did_this) {
      _func[i]->precalc_last(Scope);
      did_this = _func[i];
    }else{itested();
      // already did
    }
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COND::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  bool more = false;
  if (_set[s_NONE]) {
    _func[s_NONE]->print_common_obsolete_callback(o, lang);
    more = true;
  }else{
  }
  for (int i = sCOUNT-1; i != s_NONE; --i) {
    if (_set[i]) {
      if (more) {
	o << ' ';
      }else{
      }
      o << ' ' << static_cast<SIM_MODE>(i) << ' ';
      _func[i]->print_common_obsolete_callback(o, lang);
      more = true;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_COND p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "eval_bm_cond", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
