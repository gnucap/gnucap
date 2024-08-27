/*$Id: e_model.h 2018/05/27 al $ -*- C++ -*-
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
 * base class for all models
 */
//testing=script 2014.07.04
#ifndef E_MODEL_H
#define E_MODEL_H
#include "u_parameter.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
// external
class COMPONENT;
class COMMON_COMPONENT;
/*--------------------------------------------------------------------------*/
class SDP_CARD {
private:
  explicit SDP_CARD() {unreachable();}
  explicit SDP_CARD(const SDP_CARD&) {unreachable();}  
public:
  explicit SDP_CARD(const COMMON_COMPONENT*) {}
  virtual ~SDP_CARD() {}
  virtual void init(const COMMON_COMPONENT*) {}
};
/*--------------------------------------------------------------------------*/
class INTERFACE MODEL_CARD : public CARD{
private:
  explicit	MODEL_CARD() {unreachable();}
protected:
  explicit	MODEL_CARD(const MODEL_CARD& p);
public:
  explicit	MODEL_CARD(const COMPONENT* p);
		~MODEL_CARD();

public: // override virtuals
  char	id_letter()const override	{untested();return '\0';}
  CARD*	clone_instance()const override	{return (_component_proto) ? _component_proto->clone() : nullptr;}
  void	precalc_first()override;
  void	set_param_by_index(int, std::string&, int)override;
  bool  param_is_printable(int)const override;
  std::string value_name()const override {untested();return "";}
  std::string param_name(int)const override;
  std::string param_name(int,int)const override;
  std::string param_value(int)const override;
  int param_count()const override {return (1 + CARD::param_count());}
public:
  virtual void	tr_eval(COMPONENT*)const{unreachable();}
  virtual void	ac_eval(COMPONENT*)const{unreachable();}
  virtual COMMON_COMPONENT* new_common()const {return 0;}
  virtual SDP_CARD* new_sdp(COMMON_COMPONENT*)const {unreachable();return 0;};
  virtual bool parse_params_obsolete_callback(CS&) {unreachable(); return false;}
  virtual bool is_valid(const COMPONENT*)const {return true;}
  const CARD* component_proto()const {untested(); return _component_proto;}
protected:
  const CARD* _component_proto;
public:
  PARAMETER<double> _tnom_c;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
