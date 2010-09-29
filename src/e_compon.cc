/*$Id: e_compon.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * Base class for elements of a circuit
 */
//testing=script,sparse 2006.07.12
#include "e_model.h"
//#include "e_compon.h"
#include "e_elemnt.h" // includes e_compon.h
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(const COMMON_COMPONENT& p)
  :_tnom_c(p._tnom_c),
   _dtemp(p._dtemp),
   _temp_c(p._temp_c),
   _m(p._m),
   _modelname(p._modelname),
   _model(p._model),
   _attach_count(0)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(int c)
  :_tnom_c(NOT_INPUT),
   _dtemp(0),
   _temp_c(NOT_INPUT),
   _m(1),
   _modelname(),
   _model(0),
   _attach_count(c)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common", _attachcount);
  assert(_attach_count == 0 || _attach_count == CC_STATIC);
}
/*--------------------------------------------------------------------------*/
const MODEL_CARD* COMMON_COMPONENT::attach_model(const COMPONENT* d,
						 int error_level)const
{
  if (modelname() != "") {
    int bin_count = 0;
    const CARD* c = NULL;
    {
      for (const CARD* scope = d; scope && !c; scope = scope->owner()) {
	c = scope->find_in_parent_scope(modelname(), bDEBUG);
	if (!c) {
	  // didn't find plain model.  try binned models
	  bin_count = 0;
	  for (;;) {
	    std::string extended_name =
	      modelname() + '.' + to_string(++bin_count);
	    c = scope->find_in_parent_scope(extended_name, bDEBUG);
	    if (!c) {
	      // didn't find a binned model
	      break;
	    }
	    const MODEL_CARD* m = dynamic_cast<const MODEL_CARD*>(c);
	    if (m && m->is_valid(this)) {
	      //matching name and correct bin
	      break;
	    }
	  }
	}
      }
    }
    
    if (!c) {
      if (bin_count <= 1) {
	error(error_level, d->long_label() + ": can't find model: "
	      + modelname() + '\n');
      }else{
	error(error_level, d->long_label() + ": no bins match: "
	      + modelname() + '\n');
      }
      assert(!_model);
      return NULL;
    }
    // found something, what is it?
    _model = dynamic_cast<const MODEL_CARD*>(c);
    if (!_model) {itested();
      error(error_level, d->long_label() + ", " +modelname()
	    + " is not a .model\n");
      return NULL;
    }else if (!_model->is_valid(this)) {untested();
      error(bWARNING, d->long_label() + ", " + modelname()
	    + "\nmodel and device parameters are incompatible\n");
    }
    // must be a model, the right kind
    {
      MODEL_CARD* m = const_cast<MODEL_CARD*>(_model);
      m->elabo1();
    }
    return _model;
  }else{untested();
    assert(modelname() == "");
    assert(!_model);
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::parse_modelname(CS& cmd)
{
  set_modelname(cmd.ctos(TOKENTERM));
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::parse_param_list(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  do{
    parse_params(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::parse(CS& cmd)
{
  if (cmd.skip1b('(')) {
    // start with a paren
    int start = cmd.cursor();
    parse_param_list(cmd);
    if (cmd.gotit(start)) {
      // named args before num list
      if (cmd.skip1b('(')) {
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}
      }else{
	parse_numlist(cmd);
      }
      parse_param_list(cmd);
      if (!cmd.skip1b(')')) {untested();
	cmd.warn(bWARNING, "need )");
      }
    }else{
      // no named args before num list
      // but there's a paren
      // not sure whether it belongs to all args or to num list
      if (cmd.skip1b('(')) {
	// two parens
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}
	parse_param_list(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}
      }else{
	// only one paren
	parse_numlist(cmd);
	if (cmd.skip1b(')')) {
	  // assume it belongs to num list
	  // and named params follow
	  parse_param_list(cmd);
	}else{
	  // assume it belongs to all args
	  parse_param_list(cmd);
	  if (!cmd.skip1b(')')) {
	    cmd.warn(bWARNING, "need )");
	  }
	}
      }
    }
  }else{
    // does not start with a paren
    int start = cmd.cursor();
    parse_param_list(cmd);
    if (cmd.skip1b('(')) {
      parse_numlist(cmd);
      if (!cmd.skip1b(')')) {untested();
	cmd.warn(bWARNING, "need )");
      }
    }else{
      parse_numlist(cmd);
    }
    parse_param_list(cmd);
    if (cmd.skip1b(')')) {
      cmd.warn(bWARNING, start, "need (");
    }
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::print(OMSTREAM& o)const
{
  if(_tnom_c.has_value()) {untested();o << " tnom="  <<_tnom_c;}
  if(_dtemp.has_value())  {untested();o << " dtemp=" <<_dtemp;}
  if(_temp_c.has_value()) {	      o << " temp="  <<_temp_c;}
  if(_m.has_value())	  {untested();o << " m="     <<_m;}
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* s = c->scope();
  assert(s);
  _tnom_c.e_val(OPT::tnom_c, s);
  _dtemp.e_val(0., s);
  _temp_c.e_val(SIM::temp_c + _dtemp, s);
  _m.e_val(1, s);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::tr_eval(ELEMENT*x)const
{untested();
  assert(_model);
  _model->tr_eval(x);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::ac_eval(ELEMENT*x)const
{untested();
  assert(_model);
  _model->ac_eval(x);
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::operator==(const COMMON_COMPONENT& x)const
{
  return (_modelname == x._modelname
	  && _model == x._model
	  && _tnom_c == x._tnom_c
	  && _dtemp == x._dtemp
	  && _temp_c == x._temp_c
	  && _m == x._m);
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "TNOM",  &_tnom_c)
    || get(cmd, "DTEMP", &_dtemp)
    || get(cmd, "TEMP",  &_temp_c)
    || get(cmd, "M",     &_m)
    ;
}
/*--------------------------------------------------------------------------*/
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
COMPONENT::~COMPONENT()
{
  detach_common();
}
/*--------------------------------------------------------------------------*/
/* parse_nodes: parse circuit connections from input string
 * fills in the rest of the array with 0
 * returns the number of nodes actually read
 */
int COMPONENT::parse_nodes(CS& cmd, int max_nodes, int min_nodes,
			   int leave_tail, int start, bool all_new)
{
  assert(min_nodes <= max_nodes);

  if (start < max_nodes) {
    int paren = cmd.skip1b('(');
    int num_nodes = 0;
    if (paren) {
      // let the closing paren determine the end of the node list
      num_nodes = max_nodes;
    }else{
      // loop over the tokens to try to guess where the nodes end
      // and other stuff begins
      int here = cmd.cursor();
      int i = start;
      for (;;) {
	cmd.skiparg();
	++i;
	if (cmd.is_end()) {
	  // found the end, no '='
	  if (i <= min_nodes) {untested();
	    num_nodes = i;
	  }else if (i <= min_nodes + leave_tail) {
	    num_nodes = min_nodes;
	  }else if (i <= max_nodes + leave_tail) {
	    num_nodes = i - leave_tail;
	  }else{
	    num_nodes = max_nodes;
	  }
	  if (i == min_nodes) {untested();
	    assert(num_nodes == i);
	    assert(num_nodes == min_nodes);
	  }
	  if (i == min_nodes + leave_tail) {
	    assert(num_nodes == min_nodes);
	    assert(num_nodes == i - leave_tail);
	  }
	  if (i == max_nodes + leave_tail) {
	    assert(num_nodes == i - leave_tail);
	    assert(num_nodes == max_nodes);
	  }
	  break;
	}else if (cmd.skip1b('=')) {
	  // found '=', it's past the end of nodes
	  if (i <= min_nodes) {untested();
	    num_nodes = i;
	  }else if (i <= min_nodes + leave_tail + 1) {
	    num_nodes = min_nodes;
	  }else if (i <= max_nodes + leave_tail + 1) {
	    num_nodes = i - leave_tail - 1;
	  }else{
	    num_nodes = max_nodes;
	  }
	  if (i == min_nodes) {untested();
	    assert(num_nodes == i);
	    assert(num_nodes == min_nodes);
	  }
	  if (i == min_nodes + leave_tail + 1) {
	    assert(num_nodes == min_nodes);
	    assert(num_nodes == i - leave_tail - 1);
	  }
	  if (i == max_nodes + leave_tail + 1) {
	    assert(num_nodes == i - leave_tail - 1);
	    assert(num_nodes == max_nodes);
	  }
	  break;
	}
      }
      cmd.reset(here);
    }
    int ii = start;
    for (;;) {
      int here = cmd.cursor();
      if (paren && cmd.skip1b(')')) {
	--paren;
	break; // done.  have closing paren.
      }else if (ii >= num_nodes) {
	break; // done.  have max_nodes.
      }else if (ii >= max_nodes) {
	unreachable();
	break; // done.  have max_nodes.
      }else if (!cmd.more()) {untested();
	break; // done.  premature end of line.
      }else if (OPT::keys_between_nodes 
		&& cmd.pmatch("POLY")
		|| cmd.pmatch("PWL")
		|| cmd.pmatch("VCCAP")
		|| cmd.pmatch("VCG")
		|| cmd.pmatch("VCR")
		) {
	cmd.reset(here);
	break; // done, found reserved word
      }else{
	//----------------------
	_n[ii].parse(cmd, this);
	//----------------------
	if (_n[ii].e_() == INVALID_NODE) {untested();
	  break; // illegal node name, might be proper exit.
	}else{
	  if (all_new) {itested();
	    if (_n[ii].e_() == 0) {itested();
	      cmd.warn(bDANGER, here, "node 0 not allowed here");
	    }else if (_n[ii].e_() != ii+1) {itested();
	      cmd.warn(bDANGER, here, "duplicate node name");
	    }else{itested();
	    }
	  }else{itested();
	  }
	  ++ii;
	}
      }
    }
    if (ii < min_nodes) {untested();
      cmd.warn(bDANGER, "need " + to_string(min_nodes-ii) +" more nodes");
    }
    if (paren != 0) {untested();
      cmd.warn(bWARNING, "need )");
    }

    // ground unused input nodes
    for (int iii = ii;  iii < min_nodes;  ++iii) {untested();
      _n[iii].set_to_0(this);
    }
    return ii;
  }else{untested();
    return 0;
  }
}
/*--------------------------------------------------------------------------*/
/* printnodes: print a node list
 */
void COMPONENT::printnodes(OMSTREAM& o)const
{
  o << " (";
  for (int ii = 0;  ii < net_nodes();  ++ii) {
    o << " " << _n[ii].n_()->short_label();
  }
  o << " )";
}
/*--------------------------------------------------------------------------*/
void COMPONENT::deflate_common()
{
  unreachable();
  if (has_common()) {
    COMMON_COMPONENT* deflated_common = mutable_common()->deflate();
    if (deflated_common != common()) {untested();
      attach_common(deflated_common);
    }
  }else{
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::elabo1()
{
  if (has_common()) {
    COMMON_COMPONENT* new_common = common()->clone();
    new_common->elabo3(this);
    COMMON_COMPONENT* deflated_common = new_common->deflate();
    if (deflated_common != common()) {
      attach_common(deflated_common);
    }else{untested();
    }
  }
  CARD::elabo1();
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
void COMPONENT::tr_iwant_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt()) {
      subckt()->tr_iwant_matrix();
    }
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::ac_iwant_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt()) {
      subckt()->ac_iwant_matrix();
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
/* set_slave: force evaluation whenever the owner is evaluated.
 */
void COMPONENT::set_slave()
{
  mark_always_q_for_eval();
  if (subckt()) {
    subckt()->set_slave();
  }
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
void COMMON_COMPONENT::attach_common(COMMON_COMPONENT*c, COMMON_COMPONENT**to)
{
  assert(to);
  if (c == *to) {
    // The new and old are the same object.  Do nothing.
  }else if (!c) {untested();
    // There is no new common.  probably a simple element
    detach_common(to);
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
  }else{untested();
    // The new and old are identical.
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::detach_common(COMMON_COMPONENT** from)
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
