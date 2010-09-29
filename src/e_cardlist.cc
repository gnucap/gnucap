/*$Id: e_cardlist.cc,v 22.21 2002/10/06 07:21:50 al Exp $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 * This file contains functions that process a list of cards
 *
 * also contains CARD::find_in_scope, which really belongs elsewhere
 */
#include "l_stlextra.h"
#include "e_card.h" // includes e_cardlist.h
/*--------------------------------------------------------------------------*/
CARD_LIST::iterator CARD_LIST::find(const std::string& short_name)
{
  return notstd::find_ptr(begin(), end(), short_name);
}
/*--------------------------------------------------------------------------*/
CARD_LIST::const_iterator CARD_LIST::find(const std::string& short_name)const
{
  return notstd::find_ptr(begin(), end(), short_name);
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::erase(iterator ci)
{
  assert(ci != end());
  delete *ci;
  _cl.erase(ci);
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::erase(CARD* c)
{
  delete c;
  _cl.remove(c);
  return *this;
}
/*--------------------------------------------------------------------------*/
/* set_slave: set a whole circuit to "slave" mode.
 * Only useful for subckts.
 */
CARD_LIST& CARD_LIST::set_slave()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).set_slave();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* expand: expand (flatten) a list of components (subckts)
 * Scan component list.  Expand each subckt: create actual elements
 * for flat representation to use for simulation.
 */
CARD_LIST& CARD_LIST::expand()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).expand();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* map_nodes: create mapping between user node names and internal numbers
 */
CARD_LIST& CARD_LIST::map_nodes()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).map_nodes();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* precalc: precalculate anything that can be done up front for all modes
 * Also initializes some internal data (the polys, etc),
 * finds out what is constant, etc.
 */
CARD_LIST& CARD_LIST::precalc()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).precalc();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_alloc_matrix: allocate solution matrix
 * also sets some flags for mixed-mode
 */
CARD_LIST& CARD_LIST::tr_alloc_matrix()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_alloc_matrix();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* dc_begin: first pass on a new DC simulation
 */
CARD_LIST& CARD_LIST::dc_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).dc_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_begin: first pass on a new transient simulation (initial DC)
 */
CARD_LIST& CARD_LIST::tr_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_restore: first pass on restarting a transient simulation
 */
CARD_LIST& CARD_LIST::tr_restore()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_restore();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* dc_advance: first pass on a new step in a dc sweep
 */
CARD_LIST& CARD_LIST::dc_advance()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).dc_advance();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_advance: first pass on a new time step
 */
CARD_LIST& CARD_LIST::tr_advance()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_advance();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval: determine if anything needs to be evaluated
 */
bool CARD_LIST::tr_needs_eval()
{
  bool needs = false;
  for (iterator ci=begin(); ci!=end(); ++ci) {
    needs |= (**ci).tr_needs_eval();
  }
  return needs;
}
/*--------------------------------------------------------------------------*/
/* tr_queue_eval: build evaluator queue
 */
CARD_LIST& CARD_LIST::tr_queue_eval()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_queue_eval();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_eval: evaluate a list of models
 * evaluates a list (or sublist), checks convergence, etc.
 * does not load the matrix
 * argument is the head of the netlist.
 * recursively called to evaluate subcircuits
 */
bool CARD_LIST::do_tr()
{
  bool isconverged = true;
  {if (OPT::bypass) {
    for (iterator ci=begin(); ci!=end(); ++ci) {
      if ((**ci).tr_needs_eval()) {
	isconverged &= (**ci).do_tr();
      }
    }
  }else{
    for (iterator ci=begin(); ci!=end(); ++ci) {
      isconverged &= (**ci).do_tr();
    }
  }}
  return isconverged;
}
/*--------------------------------------------------------------------------*/
/* tr_load: load list of models to the matrix
 * recursively called to load subcircuits
 */
CARD_LIST& CARD_LIST::tr_load()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    CARD* brh = *ci;
    if (!brh->constant() || !SIM::inc_mode) {
      brh->tr_load();
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
double CARD_LIST::tr_review()
{
  double worsttime = BIGBIG;
  for (iterator ci=begin(); ci!=end(); ++ci) {
    double thistime = (**ci).tr_review();
    if (thistime < worsttime) {
      worsttime = thistime;
    }
  }
  return worsttime;
}
/*--------------------------------------------------------------------------*/
/* tr_accept: final acceptance of a time step, before moving on
 */
CARD_LIST& CARD_LIST::tr_accept()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_accept();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_unload: remove a list of models from the matrix
 * recursively called to unload subcircuits
 */
CARD_LIST& CARD_LIST::tr_unload()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_unload();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_alloc_matrix: allocate solution matrix
 */
CARD_LIST& CARD_LIST::ac_alloc_matrix()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).ac_alloc_matrix();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_begin: first pass on a new ac simulation
 */
CARD_LIST& CARD_LIST::ac_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).ac_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::do_ac()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    if (!(**ci).evaluated())
      (**ci).do_ac();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_load: load list of models to the matrix
 * recursively called to load subcircuits
 */
CARD_LIST& CARD_LIST::ac_load()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).ac_load();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* destroy: empty the list, destroy contents
 * contrast to detach_all
 * Beware: something else may be pointing to them, leaving dangling ptr.
 */
CARD_LIST& CARD_LIST::destroy()
{
  while (!_cl.empty()) {
    delete _cl.back();
    _cl.pop_back();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
CARD* CARD::find_in_scope(const std::string& name)
{
  CARD_LIST& cl = (owner()) ? owner()->subckt() : CARD_LIST::card_list;
  CARD_LIST::iterator ci = cl.find(name);
  if (ci == cl.end()) {
    error(bERROR, "can't find: " + name + '\n');
    return NULL;
  }else{
    return *ci;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
