/*$Id: s__solve.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * solve one step of a transient or dc analysis
 */
//testing=script 2006.07.14
#include "e_node.h"
//#include "s__.h"
/*--------------------------------------------------------------------------*/
//	bool	SIM::solve(int,int);
//	void	SIM::finish_building_evalq();
//	void	SIM::advance_time();
//	void	SIM::set_flags();
//	void	SIM::clear_arrays();
//	void	SIM::evaluate_models();
//	void	SIM::set_damp();
//	void	SIM::load_matrix();
//	void	SIM::solve_equations();
/*--------------------------------------------------------------------------*/
static bool converged = false;
/*--------------------------------------------------------------------------*/
bool SIM::solve(OPT::ITL itl, TRACE trace)
{
  converged = false;
  int convergedcount = 0;
  
  reset_iteration_counter(iSTEP);
  advance_time();
  
  inc_mode = (inc_mode == tsNO) ? tsBAD : inc_mode;
  damp = OPT::dampmax;
  
  do{
    if (trace >= tITERATION) {
      print(static_cast<double>(-iteration_number()));
    }
    set_flags();
    clear_arrays();
    finish_building_evalq();
    
    count_iterations(iPRINTSTEP);
    count_iterations(iSTEP);
    count_iterations(mode);
    count_iterations(iTOTAL);
    
    evaluate_models();

    if (converged) {
      if (limiting) {
	error(bDEBUG, "converged beyond limit, resetting limit\n");
	set_limit();
	convergedcount = 0;
      }else{
	++convergedcount;
      }
    }else{
      convergedcount = 0;
    }
    if (convergedcount <= OPT::itermin) {
      converged = false;
    }
      
    if (!converged || !OPT::fbbypass || damp < .99) {
      set_damp();
      load_matrix();
      solve_equations();
      if (exceeds_iteration_limit(OPT::TRACE)) {itested();
	IO::suppresserrors = false;
      }
    }
  }while (!converged && !exceeds_iteration_limit(itl));
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
  ::status.queue.start();
  CARD_LIST::card_list.tr_queue_eval();
  ::status.queue.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::advance_time(void)
{
  ::status.advance.start();
  static double last_iter_time;
  if (SIM::time0 > 0) {
    if (SIM::time0 > last_iter_time) {	/* moving forward */
      notstd::copy_n(v0, ::status.total_nodes+1, vt1);
    }else{				/* moving backward */
      /* don't save voltages.  They're wrong! */
      /* instead, restore a clean start for iteration */
      notstd::copy_n(vt1, ::status.total_nodes+1, v0);
    }
    CARD_LIST::card_list.tr_advance();
  }else{
    CARD_LIST::card_list.dc_advance();
  }
  last_iter_time = SIM::time0;
  ::status.advance.stop();
}
/* last_iter_time is initially 0 by C definition.
 * On subsequent runs it will start with an arbitrary positive value.
 * SIM::time0 starts at either 0 or the ending time of the last run.
 * In either case, (time0 > last_iter_time) is false on the first step.
 * This correctly results in "don't save voltages..."
 */
/*--------------------------------------------------------------------------*/
void SIM::set_flags()
{
  limiting = false;
  fulldamp = false;
  inc_mode = (inc_mode == tsBAD || OPT::incmode == false
	      || is_iteration_number(OPT::itl[OPT::TRLOW]))
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
  ::status.evaluate.start();
  if (OPT::bypass) {
    converged = true;
    swap(evalq, evalq_uc);
    while (!evalq->empty()) {
      converged &= evalq->front()->do_tr();
      evalq->pop_front();
    }
  }else{
    evalq_uc->clear();
    converged = CARD_LIST::card_list.do_tr();
  }
  ::status.evaluate.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::set_damp()
{
  if (is_second_iteration() && !converged && OPT::dampstrategy&dsINIT) {
    damp = OPT::dampmin;
  }else if (is_first_iteration()  ||  converged) {
    damp = OPT::dampmax;
  }else if (fulldamp) {
    damp = OPT::dampmin;
  }else{
    damp = OPT::dampmax;
  }
  trace1("", damp);
}
/*--------------------------------------------------------------------------*/
void SIM::load_matrix()
{
  ::status.load.start();
  if (OPT::traceload && SIM::inc_mode) {
    while (!loadq.empty()) {
      loadq.back()->tr_load();
      loadq.pop_back();
    }
  }else{
    loadq.clear();
    CARD_LIST::card_list.tr_load();
  }
  ::status.load.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::solve_equations()
{
  ::status.lud.start();
  lu.lu_decomp(aa, bool(OPT::lubypass && SIM::inc_mode));
  ::status.lud.stop();

  ::status.back.start();
  lu.fbsub(v0, i, fw);
  ::status.back.stop();
  
  extern LOGIC_NODE* nstat;
  if (nstat) {
    // mixed mode
    for (int ii = lu.size(); ii >= 1; --ii) {
      nstat[ii].set_a_iter();
    }
  }else{
    // pure analog
    untested();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
