/*$Id: e_compon.cc,v 20.11 2001/10/07 05:22:34 al Exp $ -*- C++ -*-
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
 * Base class for elements of a circuit
 */
#include "l_stlextra.h"
#include "ap.h"
#include "e_model.h"
//#include "e_compon.h"
#include "e_elemnt.h" // includes e_compon.h
/*--------------------------------------------------------------------------*/
static	int	name2number(CS&);
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common", _attachcount);
  assert(_attach_count == 0 || _attach_count == CC_STATIC);
}
/*--------------------------------------------------------------------------*/
const MODEL_CARD* COMMON_COMPONENT::attach_model()const
{
  {if (modelname() != "") {
    std::list<CARD*>::const_iterator mi =
      find_ptr(root_model_list.begin(), root_model_list.end(), modelname());
    {if (mi == root_model_list.end()) {
      error(bERROR, "can't find model: " + modelname() + '\n');
      _model = 0;
    }else{
      _model = dynamic_cast<const MODEL_CARD*>(*mi);
      if (!_model) {
	untested();
	error(bERROR, modelname() + " is not a .model\n");
      }
    }}
  }else{
    untested();
    assert(!_model);
  }}
  return _model;
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::parse_modelname(CS& cmd)
{
  set_modelname(cmd.ctos(TOKENTERM));
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::tr_eval(ELEMENT*x)const
{
  untested();
  assert(_model);
  _model->tr_eval(x);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::ac_eval(ELEMENT*x)const
{
  untested();
  assert(_model);
  _model->ac_eval(x);
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::is_equal(const COMMON_COMPONENT& x)const
{
  bool rv = _tnom == x._tnom
    && _modelname == x._modelname;
  if (rv) {
    assert(!_model || !x._model || _model == x._model);
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT()
  :CARD(),
   _common(0),
   _converged(false),
   _queued_for_eval(-1)
{
}
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT(const COMPONENT& p)
  :CARD(p),
   _common(0), 
   _converged(p._converged),
   _queued_for_eval(-1)
{
  attach_common(p._common);
  assert(_common == p._common);
}
/*--------------------------------------------------------------------------*/
/* parse_nodes: parse circuit connections from input string
 * n array must hold at least numnodes+1
 * fills in the rest of the array with INVALIDNODE
 * returns the number of nodes actually read
 */
int COMPONENT::parse_nodes(CS& cmd, int numnodes, int minfill, int start)
{
  int count = 0;
  if (start < numnodes) {
    int paren = cmd.skiplparen();
    for (int ii = start;  ii < numnodes;  ++ii) {
      _n[ii].t = _n[ii].e = name2number(cmd);
      if (_n[ii].e != INVALID_NODE) {
	count = ii+1;
      }else{
	if (ii <= minfill) {
	  cmd.warn(bDANGER, "need more nodes");
	  _n[ii].t = _n[ii].e = 0;
	}
      }
    }
    paren -= cmd.skiprparen();
    if (paren != 0) {
      untested();
      cmd.warn(bWARNING, "need )");
    }
  }
  set_port_count(count);
  return count;
}
/*--------------------------------------------------------------------------*/
/* printnodes: print a node list
 */
void COMPONENT::printnodes(OMSTREAM& where, int numnodes, int start)const
{
  for (int ii = start;  ii < numnodes && ii < port_count();  ++ii) {
    where << "  " << _n[ii];
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::deflate_common()
{
  if (has_common()) {
    COMMON_COMPONENT* dc = mutable_common()->deflate();
    if (dc != common()) {
      untested();
      attach_common(dc);
    }
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::expand()
{
  if (has_common()) {
    mutable_common()->expand();
  }
  deflate_common();
}
/*--------------------------------------------------------------------------*/
void COMPONENT::map_nodes()
{
  assert(is_device());
  for (int ii = -inode_count();  ii < port_count();  ++ii) {
    _n[ii].map();
  }
}
/*--------------------------------------------------------------------------*/
/* set: set parameters, used in model building
 */
void COMPONENT::set_parameters(const std::string& Label, CARD *Owner,
			       COMMON_COMPONENT *Common, double Value,
			       int , double [],
			       int Port_Count, const node_t Nodes[])
{
  set_Label(Label);
  set_owner(Owner);
  set_value(Value);
  attach_common(Common);
  for (int i=0; i<Port_Count; ++i) {
    _n[i] = Nodes[i];
  }
  set_port_count(Port_Count);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_value(double v, COMMON_COMPONENT* c)
{
  if (c != _common) {
    detach_common();
    attach_common(c);
  }
  set_value(v);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void attach_common(COMMON_COMPONENT *c, COMMON_COMPONENT** to)
{
  assert(to);
  {if (c == *to) {
    // The new and old are the same.  Do nothing.
  }else if (!c) {
    // There is no new common.  probably a simple element
    untested();
    detach_common(to);
  }else if (!*to) {
    // No old one, but have a new one.
    *to = c->attach();
  }else if (*c != **to) {
    // They are different, usually by edit.
    detach_common(to);
    *to = c->attach();
  }else if (c->_attach_count == 0) {
    // The one already attached matches the new one. 
    // Use the old one.
    // The new one is not used anywhere, so throw it away.
    delete c;
  }else{
    untested();
    // The one already attached matches the new one. 
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }}
}
/*--------------------------------------------------------------------------*/
void detach_common(COMMON_COMPONENT** from)
{
  assert(from);
  if (*from) {
    if ((**from).detach() == 0) {
      trace1("delete", (**from)._attachcount);
      delete *from;
    }else{
      trace1("nodelete", (**from)._attachcount);
    }
    *from = NULL;
  }
}
/*--------------------------------------------------------------------------*/
/* name2number: convert node name to node number
 * returns node number
 * cnt updated
 */
static int name2number(CS& cmd)
{
  int test = cmd.cursor();
  int node = cmd.ctoi();
  {if (test == cmd.cursor()) {
    return INVALID_NODE;
  }else{
    return node;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
