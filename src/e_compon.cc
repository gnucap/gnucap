/*$Id: e_compon.cc,v 24.13 2003/12/16 06:51:57 al Exp $ -*- C++ -*-
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
 * Base class for elements of a circuit
 */
#include "l_stlextra.h"
#include "ap.h"
#include "e_model.h"
//#include "e_compon.h"
#include "e_elemnt.h" // includes e_compon.h
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common", _attachcount);
  assert(_attach_count == 0 || _attach_count == CC_STATIC);
}
/*--------------------------------------------------------------------------*/
const MODEL_CARD* COMMON_COMPONENT::attach_model(const COMPONENT* d)const
{
  {if (modelname() != "") {
    // search for matching model
    std::list<CARD*>::const_iterator mi =
      notstd::find_ptr(root_model_list.begin(), root_model_list.end(),
		       modelname());
    if (mi == root_model_list.end()) {
      // didn't find.  try binned models
      int bin_count = 0;
      for (;;) {
	std::string extended_name = modelname() + '.' + to_string(++bin_count);
	mi = notstd::find_ptr(root_model_list.begin(), root_model_list.end(),
		      extended_name);
	{if (mi == root_model_list.end()) {
	  {if (bin_count <= 1) {
	    error(bERROR, d->long_label() + ": can't find model: "
		  + modelname() + '\n');
	  }else{
	    error(bERROR,d->long_label()+": no bins match: "+modelname()+'\n');
	  }}
	  _model = 0;
	}else{
	  _model = dynamic_cast<const MODEL_CARD*>(*mi);
	  if (_model && _model->is_valid(this)) {
	    //matching name and correct bin
	    break;
	  }
	}}
      }
    }

    // found something, what is it?
    _model = dynamic_cast<const MODEL_CARD*>(*mi);
    {if (!_model) {
      untested();
      error(bERROR, d->long_label() +": " +modelname() +" is not a .model\n");
    }else if (!_model->is_valid(this)) {
      error(bWARNING, d->long_label() + ", " + modelname() +
	    "\nmodel and device parameters are incompatible\n");
    }}
    // must be a model, the right kind
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
  return (_tnom == x._tnom
    && _modelname == x._modelname
    && _model == x._model);
}
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT()
  :CARD(),
   _common(0),
   _converged(false),
   _q_for_eval(-1)
{
}
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT(const COMPONENT& p)
  :CARD(p),
   _common(0), 
   _converged(p._converged),
   _q_for_eval(-1)
{
  attach_common(p._common);
  assert(_common == p._common);
}
/*--------------------------------------------------------------------------*/
/* parse_nodes: parse circuit connections from input string
 * n array must hold at least max_nodes+1
 * fills in the rest of the array with INVALIDNODE
 * returns the number of nodes actually read
 */
int COMPONENT::parse_nodes(CS& cmd, int max_nodes, int min_nodes, int start)
{
  int count = 0;
  if (start < max_nodes) {
    int paren = cmd.skiplparen();
    int ii = start;
    for ( ; ii < max_nodes; ++ii) {
      _n[ii].parse(cmd, this);
      if (_n[ii].e_() != INVALID_NODE) {
	count = ii+1;
      }else{
	break;
      }
    }
    if (ii < min_nodes) {
      cmd.warn(bDANGER, "need " + to_string(min_nodes-ii) +" more nodes");
    }
    for ( ; ii < min_nodes && ii < max_nodes; ++ii) {
      _n[ii].set_to_0();
    }
    paren -= cmd.skiprparen();
    if (paren != 0) {
      cmd.warn(bWARNING, "need )");
    }
  }
  return count;
}
/*--------------------------------------------------------------------------*/
/* printnodes: print a node list
 */
void COMPONENT::printnodes(OMSTREAM& o)const
{
  if (OPT::named_nodes) {
    o << " (";
  }
  for (int ii = 0;  ii < net_nodes();  ++ii) {
    o << "  " << _n[ii].name(this);
  }
  if (OPT::named_nodes) {
    o << ")";
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
    mutable_common()->expand(this);
  }
  deflate_common();
}
/*--------------------------------------------------------------------------*/
void COMPONENT::map_nodes()
{
  assert(is_device());
  int ext_nodes = std::max(net_nodes(), matrix_nodes());
  for (int ii = -int_nodes();  ii < ext_nodes;  ++ii) {
    _n[ii].map();
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::tr_alloc_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt().exists()) {
      subckt().tr_alloc_matrix();
    }
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::ac_alloc_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt().exists()) {
      subckt().ac_alloc_matrix();
    }
  }
}
/*--------------------------------------------------------------------------*/
/* set: set parameters, used in model building
 */
void COMPONENT::set_parameters(const std::string& Label, CARD *Owner,
			       COMMON_COMPONENT *Common, double Value,
			       int , double [],
			       int node_count, const node_t Nodes[])
{
  set_Label(Label);
  set_owner(Owner);
  set_value(Value);
  attach_common(Common);

  assert(node_count <= net_nodes());
  notstd::copy_n(Nodes, node_count, _n);
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
void attach_common(COMMON_COMPONENT* c, COMMON_COMPONENT** to)
{
  assert(to);
  {if (c == *to) {
    // The new and old are the same object.  Do nothing.
  }else if (!c) {
    // There is no new common.  probably a simple element
    untested();
    detach_common(to);
    untested();    
  }else if (!*to) {
    // No old one, but have a new one.
    ++(c->_attach_count);
    *to = c;
  }else if (*c != **to) {
    // They are different, usually by edit.
    detach_common(to);
    ++(c->_attach_count);
    *to = c;
  }else if (c->_attach_count == 0) {
    // The new and old are identical.
    // Use the old one.
    // The new one is not used anywhere, so throw it away.
    delete c;
  }else{
    untested();
    // The new and old are identical.
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }}
}
/*--------------------------------------------------------------------------*/
void detach_common(COMMON_COMPONENT** from)
{
  assert(from);
  if (*from) {
    assert((**from)._attach_count > 0);
    --((**from)._attach_count);
    if ((**from)._attach_count == 0) {
      trace1("delete", (**from)._attachcount);
      delete *from;
    }else{
      trace1("nodelete", (**from)._attachcount);
    }
    *from = NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
