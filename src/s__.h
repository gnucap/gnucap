/*$Id: s__.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * base class for simulation methods
 */
//testing=script,complete 2006.07.14
#ifndef S___H
#define S___H
#include "e_base.h"
#include "constant.h"
/*--------------------------------------------------------------------------*/
class CARD;
class CARD_LIST;
class CS;
class PROBELIST;
/*--------------------------------------------------------------------------*/
class SIM : public CKT_BASE {
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
public:
  enum PHASE { // which of the many steps...
    pNONE,	/* not doing anything, reset by cmd interpreter */
    pINIT_DC,	/* initial DC analysis				*/
    pDC_SWEEP,	/* DC analysis sweep, in progress		*/
    pTRAN 	/* transient, in progress			*/
  };
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
public:
  static double freq;		/* AC frequency to analyze at (Hertz) */
  static COMPLEX jomega;	/* AC frequency to analyze at (radians) */
  static double time0;		/* time now */
  static double time1;		/* time at previous time step */
  static double dtmin;		/* min internal step size */
  static double temp_c;		/* ambient temperature */
  static double damp;		/* Newton-Raphson damping coefficient actual */
  static bool uic;		/* flag: use initial conditions (spice-like) */
  static bool bypass_ok;	/* flag: ok to bypass model evaluation */
  static TRI_STATE inc_mode;	/* flag: make incremental changes (3 state) */
  static bool fulldamp; 	/* flag: big iter. jump. use full (min) damp */
  static bool limiting;		/* flag: node limiting */
  static SIM_MODE mode;		/* simulation type (AC, DC, ...) */
  static PHASE phase;		/* phase of simulation (iter, init-dc,) */
  static bool freezetime;	/* flag: don't advance stored time */
  static double genout;		/* tr dc input to circuit (generator) */

  static std::priority_queue<double, std::vector<double> > eq; /*event queue*/
  static std::deque<CARD*> evalq1; /* evaluate queues -- alternate between */
  static std::deque<CARD*> evalq2; /* build one while other is processed */
  static std::vector<CARD*> loadq;
  static std::vector<CARD*> acceptq;
  static std::deque<CARD*>* evalq;   /* pointer to evalq to process */
  static std::deque<CARD*>* evalq_uc;/* pointer to evalq under construction */
public: 
  static double last_time;	/* time at which "volts" is valid */
  static int	*nm;		/* node map (external to internal)	*/
  static double	*i;		/* dc-tran current (i) vector		*/
  static double	*v0;		/* dc-tran voltage, new			*/
  static double	*vt1;		/* dc-tran voltage, 1 time ago		*/
				/*    used only for logic status check	*/
  static double	*fw;		/* dc-tran fwd sub intermediate values	*/
  static double	*vdc;		/* saved dc voltages			*/
  static COMPLEX *ac;		/* ac right side			*/
  OMSTREAM out;			/* places to send the results		*/
 
public:
	  int stepno;		/* count of visible (saved) steps */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
private:
  const std::string long_label()const {unreachable(); return "";}
private:
  virtual void	setup(CS&)	= 0;
  virtual void	sweep()		= 0;
  virtual void	finish()	{}
  virtual bool	is_step_rejected()const {return false;}

  explicit SIM(const SIM&):CKT_BASE() {unreachable(); incomplete();}
protected:
  explicit SIM(): CKT_BASE() {}
public:
		~SIM()		{uninit();}
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
public:					/* inline here */
  static bool	uic_now()	{return uic && phase==pINIT_DC && time0==0.;}
  static void	new_event(double etime)	{eq.push(etime);}
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
public:					/* s__init.cc */
  static void	init();
  static void	uninit();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:				/* s__aux.cc */
	 void	set_limit();
	 void	keep();
	 void	restore();
	 void   zero_voltages();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:
  	 void	command_base(CS&);	/* s__init.cc */
	 void	reset_timers();	
  static void	alloc_hold_vectors();
  static void	determine_matrix_structure(const CARD_LIST&);
	 void	alloc_vectors();
  static void	unalloc_vectors();
  static void	count_nodes();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  static void	map_nodes();		/* s__map.cc */
  static void	order_reverse();
  static void	order_forward();
  static void	order_auto();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
	 PROBELIST& alarmlist();	/* s__out.cc */
	 PROBELIST& plotlist();
	 PROBELIST& printlist();
	 PROBELIST& storelist();
	 void	outdata(double);
	 void	head(double,double,const char*);
	 void	print(double);
	 void	alarm();
  virtual void	store();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:				/* s__solve.cc */
  bool	solve(OPT::ITL,TRACE);
private:
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
