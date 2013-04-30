/*$Id: bmm_semi.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * behavioral modeling
 * Spice3 compatible "semiconductor resistor and capacitor"
 */
//testing=script 2006.07.13
#include "u_lang.h"
#include "e_model.h" 
#include "bm.h"
/*--------------------------------------------------------------------------*/
class EVAL_BM_SEMI_BASE : public EVAL_BM_ACTION_BASE {
protected:
  PARAMETER<double> _length;
  PARAMETER<double> _width;
  double _value;
private:
  static double const _default_length;
  static double const _default_width;
  static double const _default_value;
protected:
  explicit EVAL_BM_SEMI_BASE(const EVAL_BM_SEMI_BASE& p);
  explicit EVAL_BM_SEMI_BASE(int c=0);
  ~EVAL_BM_SEMI_BASE() {}
protected: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const = 0;
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void  	expand(const COMPONENT*);
  void		tr_eval(ELEMENT*)const;
  std::string	name()const	{untested();return modelname().c_str();}
  bool		ac_too()const		{untested();return false;}
  bool  	parse_params_obsolete_callback(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SEMI_CAPACITOR : public EVAL_BM_SEMI_BASE {
private:
  explicit EVAL_BM_SEMI_CAPACITOR(const EVAL_BM_SEMI_CAPACITOR& p)
    :EVAL_BM_SEMI_BASE(p) {}
public:
  explicit EVAL_BM_SEMI_CAPACITOR(int c=0)
    :EVAL_BM_SEMI_BASE(c) {}
  ~EVAL_BM_SEMI_CAPACITOR() {}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {return new EVAL_BM_SEMI_CAPACITOR(*this);}
  void  	expand(const COMPONENT*);
  void		precalc_last(const CARD_LIST*);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SEMI_RESISTOR : public EVAL_BM_SEMI_BASE {
private:
  explicit EVAL_BM_SEMI_RESISTOR(const EVAL_BM_SEMI_RESISTOR& p)
    :EVAL_BM_SEMI_BASE(p) {}
public:
  explicit EVAL_BM_SEMI_RESISTOR(int c=0)
    :EVAL_BM_SEMI_BASE(c) {}
  ~EVAL_BM_SEMI_RESISTOR() {}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {return new EVAL_BM_SEMI_RESISTOR(*this);}
  void  	expand(const COMPONENT*);
  void		precalc_last(const CARD_LIST*);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_BASE : public MODEL_CARD {
public:
  PARAMETER<double> _narrow;
  PARAMETER<double> _defw;
  PARAMETER<double> _tc1;
  PARAMETER<double> _tc2;
private:
  static double const _default_narrow;
  static double const _default_defw;
  static double const _default_tc1;
  static double const _default_tc2;
protected:
  explicit MODEL_SEMI_BASE();
  explicit MODEL_SEMI_BASE(const MODEL_SEMI_BASE& p);
protected: // override virtual
  void  precalc_first();
  //void  precalc_last();
  //CARD* clone()const //MODEL_CARD/pure
  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int param_count()const {return (4 + MODEL_CARD::param_count());}
};
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_CAPACITOR : public MODEL_SEMI_BASE {
public:
  PARAMETER<double> _cj;
  PARAMETER<double> _cjsw;
private:
  static double const _default_cj;
  static double const _default_cjsw;
private:
  explicit MODEL_SEMI_CAPACITOR(const MODEL_SEMI_CAPACITOR& p);
public:
  explicit MODEL_SEMI_CAPACITOR();
private: // override virtual
  std::string dev_type()const		{return "c";}
  void  precalc_first();
  //void  precalc_last();
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_SEMI_CAPACITOR;}
  CARD* clone()const		{return new MODEL_SEMI_CAPACITOR(*this);}
  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int param_count()const {return (2 + MODEL_SEMI_BASE::param_count());} 
};
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_RESISTOR : public MODEL_SEMI_BASE {
public:
  PARAMETER<double> _rsh;
private:
  static double const _default_rsh;
private:
  explicit MODEL_SEMI_RESISTOR(const MODEL_SEMI_RESISTOR& p);
public:
  explicit MODEL_SEMI_RESISTOR();
private: // override virtual
  std::string dev_type()const		{return "r";}
  void  precalc_first();
  //void  precalc_last();
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_SEMI_RESISTOR;}
  CARD* clone()const		{return new MODEL_SEMI_RESISTOR(*this);}
  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int param_count()const {return (1 + MODEL_SEMI_BASE::param_count());}
};
/*--------------------------------------------------------------------------*/
double const EVAL_BM_SEMI_BASE::_default_length = NOT_INPUT;
double const EVAL_BM_SEMI_BASE::_default_width = NOT_INPUT;
double const EVAL_BM_SEMI_BASE::_default_value = NOT_INPUT;
/*--------------------------------------------------------------------------*/
static MODEL_SEMI_RESISTOR  p1;
static MODEL_SEMI_CAPACITOR p2;
static DISPATCHER<MODEL_CARD>::INSTALL
  d1(&model_dispatcher, "r|res", &p1),
  d2(&model_dispatcher, "c|cap", &p2);
/*--------------------------------------------------------------------------*/
EVAL_BM_SEMI_BASE::EVAL_BM_SEMI_BASE(int c)
  :EVAL_BM_ACTION_BASE(c),
   _length(_default_length),
   _width(_default_width),
   _value(_default_value)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_SEMI_BASE::EVAL_BM_SEMI_BASE(const EVAL_BM_SEMI_BASE& p)
  :EVAL_BM_ACTION_BASE(p),
   _length(p._length),
   _width(p._width),
   _value(p._value)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_BASE::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_SEMI_BASE* p = dynamic_cast<const EVAL_BM_SEMI_BASE*>(&x);
  bool rv = p
    && _length == p->_length
    && _width == p->_width
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << modelname();
  print_pair(o, lang, "l", _length);
  print_pair(o, lang, "w", _width, _width.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::expand(const COMPONENT* d)
{
  EVAL_BM_ACTION_BASE::expand(d);
  attach_model(d);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _length.e_val(_default_length, Scope);
  _width.e_val(_default_width, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, _value);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_BASE::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "l",	&_length)
    || Get(cmd, "w",	&_width)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_CAPACITOR::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_SEMI_CAPACITOR*
    p = dynamic_cast<const EVAL_BM_SEMI_CAPACITOR*>(&x);
  bool rv = p
    && EVAL_BM_SEMI_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_CAPACITOR::expand(const COMPONENT* d)
{
  EVAL_BM_SEMI_BASE::expand(d);

  const MODEL_SEMI_CAPACITOR* m = dynamic_cast<const MODEL_SEMI_CAPACITOR*>(model());
  if (!m) {
    unreachable();
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "semi-capacitor (C)");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_CAPACITOR::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_SEMI_BASE::precalc_last(Scope);

  const MODEL_SEMI_CAPACITOR* m = prechecked_cast<const MODEL_SEMI_CAPACITOR*>(model());

  double width = (_width == NOT_INPUT) ? m->_defw : _width;
  double eff_width = width - m->_narrow;
  double eff_length = _length - m->_narrow;
  _value = m->_cj * eff_length * eff_width + 2. * m->_cjsw * (eff_length + eff_width);
  double tempdiff = (_temp_c - m->_tnom_c);
  _value *= 1 + m->_tc1*tempdiff + m->_tc2*tempdiff*tempdiff;

  if (eff_width <= 0.) {untested();
    throw Exception_Precalc(modelname() + ": effective width is negative or zero\n");
  }else{
  }
  if (eff_length <= 0.) {untested();
    throw Exception_Precalc(modelname() + ": effective length is negative or zero\n");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_RESISTOR::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_SEMI_RESISTOR*
    p = dynamic_cast<const EVAL_BM_SEMI_RESISTOR*>(&x);
  bool rv = p
    && EVAL_BM_SEMI_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_RESISTOR::expand(const COMPONENT* d)
{
  EVAL_BM_SEMI_BASE::expand(d);

  const MODEL_SEMI_RESISTOR* m = dynamic_cast<const MODEL_SEMI_RESISTOR*>(model());
  if (!m) {
    unreachable();
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "semi-resistor (R)");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_RESISTOR::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_SEMI_BASE::precalc_last(Scope);

  const MODEL_SEMI_RESISTOR* m = prechecked_cast<const MODEL_SEMI_RESISTOR*>(model());

  double width = (_width == NOT_INPUT) ? m->_defw : _width;
  double eff_width = width - m->_narrow;
  double eff_length = _length - m->_narrow;

  if (eff_width != 0.) {
    _value = m->_rsh * eff_length / eff_width;
  }else{untested();
    _value = BIGBIG;
  }
  double tempdiff = (_temp_c - m->_tnom_c);
  _value *= 1 + m->_tc1*tempdiff + m->_tc2*tempdiff*tempdiff;

  if (eff_width <= 0.) {untested();
    throw Exception_Precalc(modelname() + ": effective width is negative or zero\n");
  }else{
  }
  if (eff_length <= 0.) {
    throw Exception_Precalc(modelname() + ": effective length is negative or zero\n");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double const MODEL_SEMI_BASE::_default_narrow = 0.;
double const MODEL_SEMI_BASE::_default_defw = 1e-6;
double const MODEL_SEMI_BASE::_default_tc1 = 0.;
double const MODEL_SEMI_BASE::_default_tc2 = 0.;
/*--------------------------------------------------------------------------*/
MODEL_SEMI_BASE::MODEL_SEMI_BASE()
  :MODEL_CARD(NULL),
   _narrow(_default_narrow),
   _defw(_default_defw),
   _tc1(_default_tc1),
   _tc2(_default_tc2)
{
}
/*--------------------------------------------------------------------------*/
MODEL_SEMI_BASE::MODEL_SEMI_BASE(const MODEL_SEMI_BASE& p)
  :MODEL_CARD(p),
   _narrow(p._narrow),
   _defw(p._defw),
   _tc1(p._tc1),
   _tc2(p._tc2)
{
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_BASE::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_SEMI_BASE::param_count() - 1 - i) {
  case 0: _narrow = value; break;
  case 1: _defw = value; break;
  case 2: _tc1 = value; break;
  case 3: _tc2 = value; break;
  default: MODEL_CARD::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_BASE::param_is_printable(int i)const
{
  switch (MODEL_SEMI_BASE::param_count() - 1 - i) {
  case 0: 
  case 1: 
  case 2: 
  case 3: return true;
  default: return MODEL_CARD::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_BASE::param_name(int i)const
{
  switch (MODEL_SEMI_BASE::param_count() - 1 - i) {
  case 0: return "narrow";
  case 1: return "defw";
  case 2: return "tc1";
  case 3: return "tc2";
  default: return MODEL_CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_BASE::param_name(int i, int j)const
{
  if (j == 0) {untested();
    return param_name(i);
  }else if (i >= MODEL_CARD::param_count()) {
    return "";
  }else{
    return MODEL_CARD::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_BASE::param_value(int i)const
{
  switch (MODEL_SEMI_BASE::param_count() - 1 - i) {
  case 0: return _narrow.string();
  case 1: return _defw.string();
  case 2: return _tc1.string();
  case 3: return _tc2.string();
  default: return MODEL_CARD::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_BASE::precalc_first()
{
  MODEL_CARD::precalc_first();

  const CARD_LIST* s = scope();
  assert(s);

  _narrow.e_val(_default_narrow, s);
  _defw.e_val(_default_defw, s);
  _tc1.e_val(_default_tc1, s);
  _tc2.e_val(_default_tc2, s);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double const MODEL_SEMI_CAPACITOR::_default_cj = 0.;
double const MODEL_SEMI_CAPACITOR::_default_cjsw = 0.;
/*--------------------------------------------------------------------------*/
MODEL_SEMI_CAPACITOR::MODEL_SEMI_CAPACITOR()
  :MODEL_SEMI_BASE(),
   _cj(_default_cj),
   _cjsw(_default_cjsw)
{
}
/*--------------------------------------------------------------------------*/
MODEL_SEMI_CAPACITOR::MODEL_SEMI_CAPACITOR(const MODEL_SEMI_CAPACITOR& p)
  :MODEL_SEMI_BASE(p),
   _cj(p._cj),
   _cjsw(p._cjsw)
{
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_SEMI_CAPACITOR::param_count() - 1 - i) {
  case 0: _cj = value; break;
  case 1: _cjsw = value; break;
  default: MODEL_SEMI_BASE::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_CAPACITOR::param_is_printable(int i)const
{
  switch (MODEL_SEMI_CAPACITOR::param_count() - 1 - i) {
  case 0: 
  case 1: return true;
  default: return MODEL_SEMI_BASE::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_CAPACITOR::param_name(int i)const
{
  switch (MODEL_SEMI_CAPACITOR::param_count() - 1 - i) {
  case 0: return "cj";
  case 1: return "cjsw";
  default: return MODEL_SEMI_BASE::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_CAPACITOR::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (i >= MODEL_SEMI_BASE::param_count()) {
    return "";
  }else{
    return MODEL_SEMI_BASE::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_CAPACITOR::param_value(int i)const
{
  switch (MODEL_SEMI_CAPACITOR::param_count() - 1 - i) {
  case 0: return _cj.string();
  case 1: return _cjsw.string();
  default: return MODEL_SEMI_BASE::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::precalc_first()
{
  MODEL_SEMI_BASE::precalc_first();

  const CARD_LIST* s = scope();
  assert(s);

  _cj.e_val(_default_cj, s);
  _cjsw.e_val(_default_cjsw, s);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double const MODEL_SEMI_RESISTOR::_default_rsh = NOT_INPUT;
/*--------------------------------------------------------------------------*/
MODEL_SEMI_RESISTOR::MODEL_SEMI_RESISTOR()
  :MODEL_SEMI_BASE(),
   _rsh(_default_rsh)
{
}
/*--------------------------------------------------------------------------*/
MODEL_SEMI_RESISTOR::MODEL_SEMI_RESISTOR(const MODEL_SEMI_RESISTOR& p)
  :MODEL_SEMI_BASE(),
   _rsh(p._rsh)
{
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_SEMI_RESISTOR::param_count() - 1 - i) {
  case 0: _rsh = value; break;
  default: MODEL_SEMI_BASE::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_RESISTOR::param_is_printable(int i)const
{
  switch (MODEL_SEMI_RESISTOR::param_count() - 1 - i) {
  case 0: return true;
  default: return MODEL_SEMI_BASE::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_RESISTOR::param_name(int i)const
{
  switch (MODEL_SEMI_RESISTOR::param_count() - 1 - i) {
  case 0: return "rsh";
  default: return MODEL_SEMI_BASE::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_RESISTOR::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (i >= MODEL_SEMI_BASE::param_count()) {
    return "";
  }else{
    return MODEL_SEMI_BASE::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SEMI_RESISTOR::param_value(int i)const
{
  switch (MODEL_SEMI_RESISTOR::param_count() - 1 - i) {
  case 0: return _rsh.string();
  default: return MODEL_SEMI_BASE::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::precalc_first()
{
  MODEL_SEMI_BASE::precalc_first();

  const CARD_LIST* par_scope = scope();
  assert(par_scope);

  _rsh.e_val(_default_rsh, par_scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
