/* $Id: s__init.cc,v 25.96 2006/08/28 05:45:51 al Exp $
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
 * initialization (allocation, node mapping, etc)
 */
//testing=script 2006.07.14
#include "u_nodemap.h"
#include "e_node.h"
#include "l_jmpbuf.h"
//#include "s__.h"
/*--------------------------------------------------------------------------*/
//	 void	SIM::command_base(CS&);
//static void	SIM::init()
//	 void	SIM::reset_timers();
//static void	SIM::count_nodes();
//static void	SIM::alloc_hold_vectors();
//	 void	SIM::alloc_vectors();
//static void	SIM::unalloc_vectors();
//static void	SIM::uninit();
/*--------------------------------------------------------------------------*/
extern JMP_BUF env;
extern LOGIC_NODE* nstat;
/*--------------------------------------------------------------------------*/
void SIM::command_base(CS& cmd)
{
  reset_timers();
  reset_iteration_counter(mode);
  reset_iteration_counter(iPRINTSTEP);
  
  init();
  alloc_vectors();
  
  aa.reallocate();
  aa.dezero(OPT::gmin);
  aa.set_min_pivot(OPT::pivtol);
  
  lu.reallocate();
  lu.dezero(OPT::gmin);
  lu.set_min_pivot(OPT::pivtol);
  
  JMP_BUF stash = env;
  if (!sigsetjmp(env.p, true)) {
    //try {
    setup(cmd);
    ::status.set_up.stop();
    switch (ENV::run_mode) {
    case rBATCH:
      untested();
    case rINTERACTIVE:
      itested();
    case rSCRIPT:
      sweep();
      break;
    case rIGNORE:
      untested();
    case rPRESET:
      /*nothing*/
      break;
    }
    unalloc_vectors();
    env = stash;
    finish();
  }else{itested();
    //}catch (...) {
    count_iterations(iTOTAL);
    lu.unallocate();
    aa.unallocate();
    unalloc_vectors();
    env = stash;
    finish();
    error(bERROR, "");
  }
  ::status.total.stop();
}
/*--------------------------------------------------------------------------*/
/* init: allocate, set up, etc ... for any type of simulation
 * also called by status and probe for access to internals and subckts
 */
/*static*/ void SIM::init()
{
  if (!nstat) {
    uninit();
    count_nodes();
    CARD_LIST::card_list.elabo2();
    map_nodes();
    alloc_hold_vectors();
    aa.reinit(::status.total_nodes);
    lu.reinit(::status.total_nodes);
    acx.reinit(::status.total_nodes);
    CARD_LIST::card_list.tr_iwant_matrix();
    CARD_LIST::card_list.ac_iwant_matrix();
    CARD_LIST::card_list.precalc();
    last_time = 0;
  }
}
/*--------------------------------------------------------------------------*/
void SIM::reset_timers()
{
  ::status.advance.reset();
  ::status.queue.reset();
  ::status.evaluate.reset();
  ::status.load.reset();
  ::status.lud.reset();
  ::status.back.reset();
  ::status.review.reset();
  ::status.accept.reset();
  ::status.output.reset();
  ::status.aux1.reset();
  ::status.aux2.reset();
  ::status.aux3.reset();
  ::status.set_up.reset().start();
  ::status.total.reset().start();
}
/*--------------------------------------------------------------------------*/
/* count_nodes: count nodes in main ckt (not subckts)
 * update the variables "::status.total_nodes" and "::status.user_nodes"
 * zeros "::status.subckt_nodes" and "::status.model_nodes"
 */
/*static*/ void SIM::count_nodes()
{
  ::status.total_nodes = ::status.user_nodes =
    CARD_LIST::card_list.nodes()->how_many();
  ::status.subckt_nodes = ::status.model_nodes = 0;
}
/*--------------------------------------------------------------------------*/
/* alloc_hold_vectors:
 * allocate space to hold data between commands.
 * for restart, convergence assistance, bias for AC, post-processing, etc.
 * must be done BEFORE deciding what array elements to allocate,
 * but after mapping
 * if they already exist, leave them alone to save data
 */
void SIM::alloc_hold_vectors()
{
  assert(!nstat);
  assert(!vdc);
  
  nstat = new LOGIC_NODE[::status.total_nodes+1];
  for (int ii=0;  ii <= ::status.total_nodes;  ++ii) {
    nstat[nm[ii]].set_user_number(ii);
  }

  vdc = new double[::status.total_nodes+1];
  std::fill_n(vdc, ::status.total_nodes+1, 0);
}
/*--------------------------------------------------------------------------*/
/* alloc_vectors:
 * these are new with every run and are discarded after the run.
 */
void SIM::alloc_vectors()
{
  assert(evalq1.empty());
  assert(evalq2.empty());
  assert(evalq != evalq_uc);

  assert(!ac);
  assert(!i);
  assert(!v0);
  assert(!vt1);
  assert(!fw);

  ac = new COMPLEX[::status.total_nodes+1];
  i   = new double[::status.total_nodes+1];
  v0  = new double[::status.total_nodes+1];
  vt1 = new double[::status.total_nodes+1];
  fw  = new double[::status.total_nodes+1];
  std::fill_n(ac, ::status.total_nodes+1, 0);
  std::fill_n(i,  ::status.total_nodes+1, 0);
  std::fill_n(v0, ::status.total_nodes+1, 0);
  std::fill_n(vt1,::status.total_nodes+1, 0);
  std::fill_n(fw, ::status.total_nodes+1, 0);
}
/*--------------------------------------------------------------------------*/
/*static*/ void SIM::unalloc_vectors()
{
  evalq1.clear();
  evalq2.clear();
  delete [] i;
  i = NULL;
  delete [] v0;
  v0 = NULL;
  delete [] vt1;
  vt1 = NULL;
  delete [] fw;
  fw = NULL;
  delete [] ac;
  ac = NULL;
}
/*--------------------------------------------------------------------------*/
/* uninit: undo all the allocation associated with any simulation
 * called when the circuit changes after a run, so it needs a restart
 * may be called multiple times without damage to make sure it is clean
 */
/*static*/ void SIM::uninit()
{
  acx.reinit(0);
  lu.reinit(0);
  aa.reinit(0);
  delete [] vdc;
  vdc = NULL;
  delete [] nstat;
  nstat = NULL;
  delete [] nm;
  nm = NULL;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
