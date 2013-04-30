/*$Id: s__.h,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * base class for simulation methods
 */
//testing=script,complete 2006.07.14
#ifndef S___H
#define S___H
#include "u_opt.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
class CARD;
class CARD_LIST;
class CS;
class PROBELIST;
class COMPONENT;
class WAVE;
/*--------------------------------------------------------------------------*/
class INTERFACE SIM : public CMD {
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:
  enum TRACE { // how much diagnostics to show
    tNONE      = 0,	/* no extended diagnostics			*/
    tUNDER     = 1,	/* show underlying analysis, important pts only	*/
    tALLTIME   = 2,	/* show every time step, including hidden 	*/
    tREJECTED  = 3,	/* show rejected time steps			*/
    tITERATION = 4,	/* show every iteration, including nonconverged	*/
    tVERBOSE   = 5	/* show extended diagnostics			*/
  };
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CARD_LIST* _scope;
  OMSTREAM   _out;		/* places to send the results		*/
public:
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
private:
  const std::string long_label()const {unreachable(); return "";}
private:
  virtual void	setup(CS&)	= 0;
  virtual void	sweep()		= 0;
  virtual void	finish()	{}
  virtual bool	is_step_rejected()const {return false;}

  explicit SIM(const SIM&):CMD(),_scope(NULL) {unreachable(); incomplete();}
protected:
  explicit SIM(): CMD(),_scope(NULL) {}
public:
  ~SIM();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:
  	 void	command_base(CS&);	/* s__init.cc */
	 void	reset_timers();	
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:
	 const PROBELIST& alarmlist()const;	/* s__out.cc */
	 const PROBELIST& plotlist()const;
	 const PROBELIST& printlist()const;
	 const PROBELIST& storelist()const;
  virtual void	outdata(double);
  virtual void	head(double,double,const std::string&);
  virtual void	print_results(double);
  virtual void	alarm();
  virtual void	store_results(double);
public:
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:				/* s__solve.cc */
  bool	solve(OPT::ITL,TRACE);
  bool	solve_with_homotopy(OPT::ITL,TRACE);
protected:
	void	finish_building_evalq();
	void	advance_time();
	void	set_flags();
	void	clear_arrays();
	void	evaluate_models();
	void	set_damp();
	void	load_matrix();
	void	solve_equations();
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
