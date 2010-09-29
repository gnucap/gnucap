/*$Id: e_model.h,v 22.12 2002/07/26 08:02:01 al Exp $ -*- C++ -*-
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
 * base class for all models
 */
#ifndef E_MODEL_H
#define E_MODEL_H
#include "e_card.h"
/*--------------------------------------------------------------------------*/
// this file
class MODEL_CARD;
/*--------------------------------------------------------------------------*/
// external
class COMPONENT;
/*--------------------------------------------------------------------------*/
class SDP_CARD {
public:
  explicit SDP_CARD(const COMMON_COMPONENT*) {}
  virtual ~SDP_CARD() {}
};
/*--------------------------------------------------------------------------*/
class TDP_CARD {
public:
  explicit TDP_CARD(const CARD*) {}
};
/*--------------------------------------------------------------------------*/
class MODEL_CARD : public CARD{
protected:
  explicit	MODEL_CARD(const MODEL_CARD& p)
					:CARD(p),_tnom(p._tnom){unreachable();}
public:
  explicit	MODEL_CARD();
		~MODEL_CARD();

protected: // override virtuals
  char	id_letter()const	{return '\0';}
  CARD* clone()const		{unreachable(); return 0;}
  void	parse(CS&);
  void	print(OMSTREAM&,int)const;
  
public:
  virtual void	tr_eval(COMPONENT*)const{unreachable();}
  virtual void	ac_eval(COMPONENT*)const{unreachable();}
  virtual COMMON_COMPONENT* new_common()const {unreachable();return 0;};
  virtual SDP_CARD* new_sdp(const COMMON_COMPONENT*)const
					{unreachable();return 0;};
  virtual bool parse_front(CS&) {unreachable(); return false;};
  virtual void parse_params(CS&) {unreachable();};
  virtual void parse_finish() {unreachable();};
  virtual void print_front(OMSTREAM&)const {unreachable();};
  virtual void print_params(OMSTREAM&)const {unreachable();};
  virtual void print_calculated(OMSTREAM&)const {unreachable();};
  virtual bool is_valid(const COMMON_COMPONENT*)const {return true;}
public:
  double _tnom;
};
extern std::list<CARD*> root_model_list;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
