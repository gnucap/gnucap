/*$Id: e_model.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * base class for all models
 */
//testing=script 2006.07.12
#ifndef E_MODEL_H
#define E_MODEL_H
#include "e_card.h"
/*--------------------------------------------------------------------------*/
// external
class COMPONENT;
/*--------------------------------------------------------------------------*/
class SDP_CARD {
private:
  explicit SDP_CARD() {unreachable();}
  explicit SDP_CARD(const SDP_CARD&) {unreachable();}  
public:
  explicit SDP_CARD(const COMMON_COMPONENT*) {}
  virtual ~SDP_CARD() {}
};
/*--------------------------------------------------------------------------*/
#if 0
class TDP_CARD {
private:
  explicit TDP_CARD() {untested();}
  explicit TDP_CARD(const TDP_CARD&) {untested();}
  virtual ~TDP_CARD() {untested();}
public:
  explicit TDP_CARD(const CARD*) {untested();}
};
#endif
/*--------------------------------------------------------------------------*/
class MODEL_CARD : public CARD{
protected:
  explicit	MODEL_CARD(const MODEL_CARD& p)
				:CARD(p),_tnom_c(p._tnom_c) {unreachable();}
public:
  explicit	MODEL_CARD();
		~MODEL_CARD();

public: // override virtuals
  char	id_letter()const	{untested();return '\0';}
  CARD* clone()const		{unreachable(); return 0;}
  void	parse_spice(CS&);
  void	print_spice(OMSTREAM&,int)const;
  void  elabo1();
  
public:
  virtual void	tr_eval(COMPONENT*)const{unreachable();}
  virtual void	ac_eval(COMPONENT*)const{unreachable();}
  virtual COMMON_COMPONENT* new_common()const {unreachable();return 0;};
  virtual SDP_CARD* new_sdp(const COMMON_COMPONENT*)const
					{unreachable();return 0;};
  virtual bool parse_front(CS&) {unreachable(); return false;};
  virtual bool parse_params(CS&);
  virtual void parse_finish() {};
  virtual void print_front(OMSTREAM&)const {unreachable();};
  virtual void print_params(OMSTREAM&)const;
  virtual void print_calculated(OMSTREAM&)const {unreachable();};
  virtual bool is_valid(const COMMON_COMPONENT*)const {return true;}
public:
  PARAMETER<double> _tnom_c;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
