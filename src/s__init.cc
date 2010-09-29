/* $Id: s__init.cc,v 20.10 2001/10/05 01:35:36 al Exp $
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
 * initialization (allocation, node mapping, etc)
 */
#include "e_card.h"
#include "u_status.h"
#include "l_jmpbuf.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
//	 void	SIM::command_base(CS&);
//static void	SIM::init()
//	 void	SIM::reset_timers();
//static void	SIM::count_nodes();
//static void	SIM::alloc_hold_vectors();
//static void	SIM::determine_matrix_structure(const CARD *stop);
//	 void	SIM::alloc_vectors();
//static void	SIM::unalloc_vectors();
//static void	SIM::uninit();
/*--------------------------------------------------------------------------*/
extern JMP_BUF env;
extern NODE* nstat;
/*--------------------------------------------------------------------------*/
void SIM::command_base(CS& cmd)
{
  reset_timers();
  init();
  alloc_vectors();
  aa.allocate().dezero(OPT::gmin).set_min_pivot(OPT::pivtol);
  lu.allocate().dezero(OPT::gmin).set_min_pivot(OPT::pivtol);
  JMP_BUF stash = env;
  if (!sigsetjmp(env.p, true)) {
    //try {
    setup(cmd);
    STATUS::set_up.stop();
    switch (ENV::run_mode) {
    case rINTERACTIVE:
    case rSCRIPT:
    case rBATCH:
      sweep();
      break;
    case rIGNORE:
    case rPRESET:
      /*nothing*/
      break;
    }
    lu.unallocate();
    aa.unallocate();
    unalloc_vectors();
    env = stash;
    finish();
  }else{
    //}catch (...) {
    env = stash;
    finish();
    error(bERROR, "");
  }
  STATUS::total.stop();
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
    CARD_LIST::card_list.expand();
    map_nodes();
    alloc_hold_vectors();
    aa.reinit(STATUS::total_nodes);
    lu.reinit(STATUS::total_nodes);
    acx.reinit(STATUS::total_nodes);
    determine_matrix_structure(CARD_LIST::card_list);
    CARD_LIST::card_list.precalc();  
  }
}
/*--------------------------------------------------------------------------*/
void SIM::reset_timers()
{
  STATUS::advance.reset();
  STATUS::evaluate.reset();
  STATUS::load.reset();
  STATUS::lud.reset();
  STATUS::back.reset();
  STATUS::review.reset();
  STATUS::accept.reset();
  STATUS::output.reset();
  STATUS::aux1.reset();
  STATUS::aux2.reset();
  STATUS::aux3.reset();
  STATUS::set_up.reset().start();
  STATUS::total.reset().start();
  STATUS::iter[mode] = 0;
  STATUS::iter[iPRINTSTEP] = 0;
}
/*--------------------------------------------------------------------------*/
/* count_nodes: count nodes in main ckt (not subckts)
 * update the variables "STATUS::total_nodes" and "STATUS::user_nodes"
 * zeros "STATUS::subckt_nodes" and "STATUS::model_nodes"
 */
/*static*/ void SIM::count_nodes()
{
  STATUS::user_nodes = 0;
  for (CARD_LIST::const_iterator
	 ci=CARD_LIST::card_list.begin();ci!=CARD_LIST::card_list.end();++ci) {
    CARD* brh = *ci;
    if (brh->is_device()) {
      for (int ii = 0;  ii < brh->port_count();  ++ii) {
	if (brh->_n[ii].e > STATUS::user_nodes) {
	  STATUS::user_nodes = brh->_n[ii].e;
	}
      }
    }
  }
  STATUS::total_nodes = STATUS::user_nodes;
  STATUS::subckt_nodes = STATUS::model_nodes = 0;
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

  nstat = new NODE[STATUS::total_nodes+1];
  for (int ii=0;  ii <= STATUS::total_nodes;  ++ii) {
    nstat[nm[ii]].set_number(ii);
  }

  vdc = new double[STATUS::total_nodes+1];
  std::fill_n(vdc, STATUS::total_nodes+1, 0);
}
/*--------------------------------------------------------------------------*/
/* determine_matrix_structure: 
 * scan the list and tell the matrix which entries to allocate
 * recursive for subckts
 */
void SIM::determine_matrix_structure(const CARD_LIST& cl)
{
  for (CARD_LIST::const_iterator ci = cl.begin();  ci != cl.end();  ++ci) {
    const CARD* brh = *ci;
    if (brh->is_device()) {
      for (int ii = 0;  ii < brh->port_count();  ++ii) {
	assert(brh->_n[ii].m != INVALID_NODE);
	if (brh->_n[ii].m != 0) {
	  for (int jj = 0;  jj < ii ;  ++jj) {
	    aa.iwant(brh->_n[ii].m,brh->_n[jj].m);
	    lu.iwant(brh->_n[ii].m,brh->_n[jj].m);
	    acx.iwant(brh->_n[ii].m,brh->_n[jj].m);
	  }
	  nstat[brh->_n[ii].m].set_needs_analog();
	}
      }
      if (brh->subckt().exists()) {
	determine_matrix_structure(brh->subckt());
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/* alloc_vectors:
 * allocate space for the right-side vector
 * (initially current sources, on solution becomes voltages) and copies
 * used for one-time-ago and convergence checking
 * these are new with every run and are discarded after the run.
 */
void SIM::alloc_vectors()
{
  unalloc_vectors();	/* usually unnecessary. */
  assert(!ac);		/* may be needed after exception */
  assert(!i);
  assert(!v0);
  assert(!vt1);
  assert(!fw);
  ac = new COMPLEX[STATUS::total_nodes+1];
  i   = new double[STATUS::total_nodes+1];
  v0  = new double[STATUS::total_nodes+1];
  vt1 = new double[STATUS::total_nodes+1];
  fw  = new double[STATUS::total_nodes+1];
  std::fill_n(ac, STATUS::total_nodes+1, 0);
  std::fill_n(i,  STATUS::total_nodes+1, 0);
  std::fill_n(v0, STATUS::total_nodes+1, 0);
  std::fill_n(vt1,STATUS::total_nodes+1, 0);
  std::fill_n(fw, STATUS::total_nodes+1, 0);
}
/*--------------------------------------------------------------------------*/
/*static*/ void SIM::unalloc_vectors()
{
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
  delete [] nstat;
  nstat = NULL;
  aa.reinit(0);
  lu.reinit(0);
  acx.reinit(0);
  delete [] nm;
  nm = NULL;
  delete [] vdc;
  vdc = NULL;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
