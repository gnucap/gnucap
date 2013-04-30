/*$Id: bm_pwl.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * HSPICE compatible PWL
 */
//testing=script 2005.10.06
#include "u_lang.h"
#include "e_elemnt.h"
#include "m_interp.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
/*--------------------------------------------------------------------------*/
class EVAL_BM_PWL : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _delta;
  PARAMETER<int>    _smooth;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _raw_table;
  std::vector<DPAIR> _num_table;
  explicit	EVAL_BM_PWL(const EVAL_BM_PWL& p);
public:
  explicit      EVAL_BM_PWL(int c=0);
		~EVAL_BM_PWL()		{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_PWL(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void		precalc_last(const CARD_LIST*);

  void		tr_eval(ELEMENT*)const;
  TIME_PAIR	tr_review(COMPONENT*);
  std::string	name()const		{return "pwl";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
  void		skip_type_tail(CS& cmd)const {cmd.umatch("(1)");}
};
/*--------------------------------------------------------------------------*/
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
void EVAL_BM_PWL::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name() << '(';
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _raw_table.begin(); p != _raw_table.end(); ++p) {
    o << p->first << ',' << p->second << ' ';
  }
  o << ')';
  print_pair(o, lang, "delta", _delta, _delta.has_hard_value());
  print_pair(o, lang, "smooth",_smooth,_smooth.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _delta.e_val(_default_delta, Scope);
  _smooth.e_val(_default_smooth, Scope);

  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::iterator
	 p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    p->first.e_val(0, Scope);
    p->second.e_val(0, Scope);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PWL::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_last(Scope);

  double last = -BIGBIG;
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::iterator
	 p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    if (last > p->first) {
      throw Exception_Precalc("PWL is out of order: (" + to_string(last)
			      + ", " + to_string(p->first) + ")\n");
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
  d->_y[0] = interpolate(_num_table.begin(), _num_table.end(), 
		       ioffset(d->_y[0].x), ext, ext);
  tr_final_adjust(&(d->_y[0]), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_PWL::tr_review(COMPONENT* d)
{
  if (d->is_source()) {
    // index (x) is time
    ELEMENT* dd = prechecked_cast<ELEMENT*>(d);
    assert(dd);
    double x = dd->_y[0].x + d->_sim->_dtmin * .01;
    DPAIR here(x, BIGBIG);
    std::vector<DPAIR>::iterator begin = _num_table.begin();
    std::vector<DPAIR>::iterator end   = _num_table.end();
    std::vector<DPAIR>::iterator upper = upper_bound(begin, end, here);
    std::vector<DPAIR>::iterator lower = upper - 1;
    assert(x > lower->first);
    d->_time_by.min_event((x < upper->first) ? upper->first : NEVER);
  }else{untested();
    // index (x) is input
    // It's really needed here too, more work needed
  }

  return d->_time_by;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PWL::parse_numlist(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
  for (;;) {
    unsigned start_of_pair = here;
    std::pair<PARAMETER<double>, PARAMETER<double> > p;
    //cmd >> key >> value;
    cmd >> p.first; // key
    if (cmd.stuck(&here)) {
      // no more, graceful finish
      break;
    }else{
      cmd >> p.second; // value
      if (cmd.stuck(&here)) {
	// ran out, but already have half of the pair
	// back up one, hoping somebody else knows what to do with it
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
bool EVAL_BM_PWL::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "delta",  &_delta)
    || Get(cmd, "smooth", &_smooth)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_PWL p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "pwl", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
