/*$Id: e_logicmod.h $ -*- C++ -*-
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
 * data structures and defaults for logic model.
 */
//testing=script,sparse 2023.11.22
#ifndef D_LOGICMOD_H
#define D_LOGICMOD_H
#include "e_model.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC : public MODEL_CARD {
private:
  explicit	MODEL_LOGIC(const MODEL_LOGIC& p);
public:
  explicit MODEL_LOGIC(const COMPONENT*);
	   ~MODEL_LOGIC()		{--_count;}
private: // override virtuals
  std::string	dev_type()const override{return "logic";}
  CARD*		clone()const override	{return new MODEL_LOGIC(*this);}
  void		precalc_first()override;
  void		set_param_by_index(int, std::string&, int)override;
  bool		param_is_printable(int)const override;
  std::string	param_name(int)const override;
  std::string	param_name(int,int)const override;
  std::string	param_value(int)const override;
  int		param_count()const override {return (13 + MODEL_CARD::param_count());}
public:
  static int	count()			{untested();return _count;}
public:
			/* ----- digital mode ----- */
  PARAMETER<double> delay;	/* propagation delay */
			/* -- conversion parameters both ways -- */
  PARAMETER<double> vmax;	/* nominal volts for logic 1 */
  PARAMETER<double> vmin;	/* nominal volts for logic 0 */
  PARAMETER<double> unknown;	/* nominal volts for unknown (bogus) */
			/* ---- D to A conversion ---- */
  PARAMETER<double> rise;	/* rise time (time in slope) */
  PARAMETER<double> fall;	/* fall time (time in slope) */
  PARAMETER<double> rs; 	/* series resistance -- strong */
  PARAMETER<double> rw; 	/* series resistance -- weak */
			/* ---- A to D conversion ---- */
  PARAMETER<double> th1;	/* threshold for 1 as fraction of range */
  PARAMETER<double> th0;	/* threshold for 0 as fraction of range */
  	        	/* ---- quality judgement parameters ---- */
  PARAMETER<double> mr;		/* margin rise - how much worse rise can be */
  PARAMETER<double> mf;		/* margin fall - how much worse fall can be */
  PARAMETER<double> over;	/* overshoot limit - as fraction of range */
public: // calculated parameters
  double range;			/* vmax - vmin */
private:
  static int _count;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
