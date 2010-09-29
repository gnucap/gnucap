/*$Id: s__solve.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * solve one step of a transient or dc analysis
 */
#include "e_card.h"
#include "constant.h"
#include "u_status.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
//	bool	SIM::solve(int,int);
//	void	SIM::finish_building_evalq();
//	void	SIM::advance_time();
//	void	SIM::count_iterations();
//	void	SIM::set_flags();
//	void	SIM::clear_arrays();
//	void	SIM::evaluate_models();
//	void	SIM::set_damp();
//	void	SIM::load_matrix();
//	void	SIM::solve_equations();
/*--------------------------------------------------------------------------*/
static bool converged = false;
/*--------------------------------------------------------------------------*/
bool SIM::solve(int itl, int trace)
{
  converged = false;
  int convergedcount = 0;
  
  STATUS::iter[iSTEP] = 0;
  advance_time();
  
  inc_mode = (inc_mode == tsNO) ? tsBAD : inc_mode;
  damp = OPT::dampmax;
  
  do{
    if (trace >= tITERATION) {
      print(static_cast<double>(-STATUS::iter[iSTEP]));
    }
    set_flags();
    clear_arrays();
    finish_building_evalq();
    count_iterations();
    evaluate_models();

    {if (converged) {
      {if (limiting) {
	error(bDEBUG, "converged beyond limit, resetting limit\n");
	set_limit();
	convergedcount = 0;
      }else{
	++convergedcount;
      }}
    }else{
      convergedcount = 0;
    }}
    if (convergedcount <= OPT::itermin) {
      converged = false;
    }
      
    if (!converged || !OPT::fbbypass || damp < .99) {
      set_damp();
      load_matrix();
      solve_equations();
      if (STATUS::iter[iSTEP] >= OPT::itl[OPT::TRACE]) {
	untested();
	IO::suppresserrors = false;
      }
    }
  }while (!converged && STATUS::iter[iSTEP]<=itl);
  return converged;
}
/*--------------------------------------------------------------------------*/
/* finish_building_evalq
 * This function scans the circuit to queue for evaluation anything 
 * that is relevant that the devices missed themselves.
 * For now, it scans the whole circuit, but this will change.
 * This is slow, but the result is still faster than not having a queue.
 * The plan is .. every node knows whether it needs eval or not, and
 * only those nodes needing eval will be scanned.
 * Its purpose is to catch nodes that wake up after being dormant
 */
void SIM::finish_building_evalq(void)
{
  STATUS::aux1.start();
  CARD_LIST::card_list.tr_queue_eval();
  STATUS::aux1.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::advance_time(void)
{
  STATUS::advance.start();
  static double last_iter_time;
  {if (SIM::time0 > 0) {
    {if (SIM::time0 > last_iter_time) {	/* moving forward */
      std::copy_n(v0, STATUS::total_nodes+1, vt1);
    }else{				/* moving backward */
      /* don't save voltages.  They're wrong! */
      /* instead, restore a clean start for iteration */
      std::copy_n(vt1, STATUS::total_nodes+1, v0);
    }}
    CARD_LIST::card_list.tr_advance();
  }else{
    CARD_LIST::card_list.dc_advance();
  }}
  last_iter_time = SIM::time0;
  STATUS::advance.stop();
}
/* last_iter_time is initially 0 by C definition.
 * On subsequent runs it will start with an arbitrary positive value.
 * SIM::time0 starts at either 0 or the ending time of the last run.
 * In either case, (time0 > last_iter_time) is false on the first step.
 * This correctly results in "don't save voltages..."
 */
/*--------------------------------------------------------------------------*/
void SIM::count_iterations()
{
  ++STATUS::iter[iPRINTSTEP];
  ++STATUS::iter[iSTEP];
  ++STATUS::iter[mode];
  ++STATUS::iter[iTOTAL];
}
/*--------------------------------------------------------------------------*/
void SIM::set_flags()
{
  limiting = false;
  fulldamp = false;
  inc_mode = (inc_mode == tsBAD || OPT::incmode == false
	      || STATUS::iter[iSTEP]==OPT::itl[OPT::TRLOW])
    ? tsNO
    : tsYES;
  bypass_ok = 
    (is_step_rejected()  ||  damp < OPT::dampmax*OPT::dampmax)
    ? false : bool(OPT::bypass);
}
/*--------------------------------------------------------------------------*/
void SIM::clear_arrays(void)
{
  if (!SIM::inc_mode) {			/* Clear working array */
    SIM::aa.zero();
    SIM::aa.dezero(OPT::gmin);		/* gmin fudge */
    std::fill_n(i, aa.size()+1, 0);
  }
  loadq.clear();
}
/*--------------------------------------------------------------------------*/
void SIM::evaluate_models()
{
  STATUS::evaluate.start();
  {if (OPT::bypass) {
    converged = true;
    swap(evalq, evalq_uc);
    while (!evalq->empty()) {
      converged &= evalq->front()->do_tr();
      evalq->pop_front();
    }
  }else{
    evalq_uc->clear();
    converged = CARD_LIST::card_list.do_tr();
  }}
  STATUS::evaluate.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::set_damp()
{
  {if (STATUS::iter[iSTEP] == 2 && !converged && OPT::dampstrategy&dsINIT) {
    damp = OPT::dampmin;
  }else if (STATUS::iter[iSTEP] == 1  ||  converged) {
    damp = OPT::dampmax;
  }else if (fulldamp) {
    damp = OPT::dampmin;
  }else{
    damp = OPT::dampmax;
  }}
  trace1("", damp);
}
/*--------------------------------------------------------------------------*/
void SIM::load_matrix()
{
  STATUS::load.start();
  {if (OPT::traceload && SIM::inc_mode) {
    while (!loadq.empty()) {
      loadq.back()->tr_load();
      loadq.pop_back();
    }
  }else{
    loadq.clear();
    CARD_LIST::card_list.tr_load();
  }}
  STATUS::load.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::solve_equations()
{
  lu.lu_decomp(aa, bool(OPT::lubypass && SIM::inc_mode));
  lu.fbsub(v0, i, fw);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
