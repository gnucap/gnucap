/*$Id: e_compon.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * Base class for elements of a circuit
 */
//testing=script,noswitch 2009.07.13
#include "u_lang.h"
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(const COMMON_COMPONENT& p)
  :_tnom_c(p._tnom_c),
   _dtemp(p._dtemp),
   _temp_c(p._temp_c),
   _mfactor(p._mfactor),
   _value(p._value),
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
   _mfactor(1),
   _value(0),
   _modelname(),
   _model(0),
   _attach_count(c)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common,destruct", _attach_count);
  assert(_attach_count == 0 || _attach_count == CC_STATIC);
}
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
    trace1("++1", c->_attach_count);
    *to = c;
  }else if (*c != **to) {
    // They are different, usually by edit.
    detach_common(to);
    ++(c->_attach_count);
    trace1("++2", c->_attach_count);
    *to = c;
  }else if (c->_attach_count == 0) {
    // The new and old are identical.
    // Use the old one.
    // The new one is not used anywhere, so throw it away.
    trace1("delete", c->_attach_count);    
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
    trace1("--", (**from)._attach_count);
    if ((**from)._attach_count == 0) {
      trace1("delete", (**from)._attach_count);
      delete *from;
    }else{
      trace1("nodelete", (**from)._attach_count);
    }
    *from = NULL;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::attach_model(const COMPONENT* d)const
{
  assert(d);
  _model = d->find_model(modelname());
  assert(_model);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::parse_modelname(CS& cmd)
{
  set_modelname(cmd.ctos(TOKENTERM));
}
/*--------------------------------------------------------------------------*/
// called only by COMMON_COMPONENT::parse_obsolete
bool COMMON_COMPONENT::parse_param_list(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
  do{
    parse_params_obsolete_callback(cmd); //BUG//callback
  }while (cmd.more() && !cmd.stuck(&here));
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::parse_common_obsolete_callback(CS& cmd) //used
{
  if (cmd.skip1b('(')) {
    // start with a paren
    unsigned start = cmd.cursor();
    parse_param_list(cmd);
    if (cmd.gotit(start)) {		// ( params ( ....
      // named args before num list
      if (cmd.skip1b('(')) {		// ( params ( list ) params )
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}else{
	}
      }else{				// ( params list params )
	parse_numlist(cmd);		//BUG//
      }
      parse_param_list(cmd);
      if (!cmd.skip1b(')')) {untested();
	cmd.warn(bWARNING, "need )");
      }else{
      }
    }else{
      // no named args before num list
      // but there's a paren
      // not sure whether it belongs to all args or to num list
      if (cmd.skip1b('(')) {		// ( ( list ) params )
	// two parens
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}else{
	}
	parse_param_list(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}else{
	}
      }else{				// ( list ...
	// only one paren
	parse_numlist(cmd);
	if (cmd.skip1b(')')) {		// ( list ) params
	  // assume it belongs to num list
	  // and named params follow
	  parse_param_list(cmd);
	}else{				// ( list params )
	  // assume it belongs to all args
	  parse_param_list(cmd);
	  if (!cmd.skip1b(')')) {
	    cmd.warn(bWARNING, "need )");
	  }else{
	  }
	}
      }
    }
  }else{
    // does not start with a paren
    unsigned start = cmd.cursor();
    parse_param_list(cmd);
    if (cmd.gotit(start)) {
      if (cmd.skip1b('(')) {		// params ( list ) params
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}else{
	}
      }else if (!(cmd.is_alpha())) {	// params list params
	parse_numlist(cmd);
      }else{				// params   (only)
      }
    }else{				// list params
      assert(!(cmd.skip1b('(')));
      parse_numlist(cmd);
    }
    parse_param_list(cmd);
    if (cmd.skip1b(')')) {
      cmd.warn(bWARNING, start, "need (");
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  print_pair(o, lang, "tnom", _tnom_c,  _tnom_c.has_hard_value());
  print_pair(o, lang, "dtemp",_dtemp,   _dtemp.has_hard_value());
  print_pair(o, lang, "temp", _temp_c,  _temp_c.has_hard_value());
  print_pair(o, lang, "m",    _mfactor, _mfactor.has_hard_value());
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::set_param_by_index(int i, std::string& Value, int Offset)
{
  switch (i) {
  case 0:  _tnom_c = Value; break;
  case 1:  _dtemp = Value; break;
  case 2:  _temp_c = Value; break;
  case 3:  _mfactor = Value; break;
  default: throw Exception_Too_Many(i, 3, Offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::param_is_printable(int i)const
{
  switch (i) {
  case 0:  return _tnom_c.has_hard_value();
  case 1:  return _dtemp.has_hard_value();
  case 2:  return _temp_c.has_hard_value();
  case 3:  return _mfactor.has_hard_value();
  default: return false;
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_name(int i)const
{
  switch (i) {
  case 0:  return "tnom";
  case 1:  return "dtemp";
  case 2:  return "temp";
  case 3:  return "m";
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_name(int i, int j)const
{
  return (j==0) ? param_name(i) : "";
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_value(int i)const
{
  switch (i) {
  case 0:  return _tnom_c.string();
  case 1:  return _dtemp.string();
  case 2:  return _temp_c.string();
  case 3:  return _mfactor.string();
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  _tnom_c.e_val(OPT::tnom_c, Scope);
  _dtemp.e_val(0., Scope);
  _temp_c.e_val(CKT_BASE::_sim->_temp_c + _dtemp, Scope);
  _mfactor.e_val(1, Scope);
  _value.e_val(0, Scope);
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
	  && _mfactor == x._mfactor
	  && _value == x._value);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::set_param_by_name(std::string Name, std::string Value)
{
  if (has_parse_params_obsolete_callback()) {untested();
    std::string args(Name + "=" + Value);
    CS cmd(CS::_STRING, args); //obsolete_callback
    bool ok = parse_params_obsolete_callback(cmd); //BUG//callback
    if (!ok) {untested();
      throw Exception_No_Match(Name);
    }else{untested();
    }
  }else{
    //BUG// ugly linear search
    for (int i = param_count() - 1;  i >= 0;  --i) {
      for (int j = 0;  param_name(i,j) != "";  ++j) {
	if (Umatch(Name, param_name(i,j) + ' ')) {
	  set_param_by_index(i, Value, 0/*offset*/);
	  return; //success
	}else{
	  //keep looking
	}
      }
    }
    itested();
    throw Exception_No_Match(Name);
  }
}
/*--------------------------------------------------------------------------*/
//BUG// This is a kluge for the spice_wrapper, to disable virtual functions.
// It is called during expansion only.

void COMMON_COMPONENT::Set_param_by_name(std::string Name, std::string Value)
{
  assert(!has_parse_params_obsolete_callback());
  
  //BUG// ugly linear search
  for (int i = COMMON_COMPONENT::param_count() - 1;  i >= 0;  --i) {
    for (int j = 0;  COMMON_COMPONENT::param_name(i,j) != "";  ++j) {
      if (Umatch(Name, COMMON_COMPONENT::param_name(i,j) + ' ')) {
	COMMON_COMPONENT::set_param_by_index(i, Value, 0/*offset*/);
	return; //success
      }else{
	//keep looking
      }
    }
  }
  throw Exception_No_Match(Name);
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::parse_numlist(CS&)
{
  return false;
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "tnom",   &_tnom_c)
    || Get(cmd, "dtemp",  &_dtemp)
    || Get(cmd, "temp",   &_temp_c)
    || Get(cmd, "m",	  &_mfactor)
    || Get(cmd, "mfactor",&_mfactor)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT()
  :CARD(),
   _common(0),
   _value(0),
   _mfactor(1),
   _mfactor_fixed(NOT_VALID),
   _converged(false),
   _q_for_eval(-1),
   _time_by()
{
  _sim->uninit();
}
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT(const COMPONENT& p)
  :CARD(p),
   _common(0),
   _value(p._value),
   _mfactor(p._mfactor),
   _mfactor_fixed(p._mfactor_fixed),
   _converged(p._converged),
   _q_for_eval(-1),
   _time_by(p._time_by)
{
  _sim->uninit();
  attach_common(p._common);
  assert(_common == p._common);
}
/*--------------------------------------------------------------------------*/
COMPONENT::~COMPONENT()
{
  detach_common();
  _sim->uninit();
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::node_is_grounded(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].is_grounded();
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::node_is_connected(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].is_connected();
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_by_name(std::string& int_name, std::string& ext_name)
{itested();
  for (int i=0; i<max_nodes(); ++i) {itested();
    if (int_name == port_name(i)) {itested();
      set_port_by_index(i, ext_name);
      return;
    }else{itested();
    }
  }
  untested();
  throw Exception_No_Match(int_name);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_by_index(int num, std::string& ext_name)
{
  if (num <= max_nodes()) {
    _n[num].new_node(ext_name, this);
    if (num+1 > _net_nodes) {
      // make the list bigger
      _net_nodes = num+1;
    }else{itested();
      // it's already big enough, probably assigning out of order
    }
  }else{untested();
    throw Exception_Too_Many(num, max_nodes(), 0/*offset*/);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_to_ground(int num)
{untested();
  if (num <= max_nodes()) {untested();
    _n[num].set_to_ground(this);
    if (num+1 > _net_nodes) {untested();
      _net_nodes = num+1;
    }else{untested();
    }
  }else{untested();
    throw Exception_Too_Many(num, max_nodes(), 0/*offset*/);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_dev_type(const std::string& new_type)
{
  if (common()) {
    if (new_type != dev_type()) {
      COMMON_COMPONENT* c = common()->clone();
      assert(c);
      c->set_modelname(new_type);
      attach_common(c);
    }else{
    }
  }else{
    CARD::set_dev_type(new_type);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::print_args_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  assert(has_common());
  common()->print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::deflate_common()
{untested();
  unreachable();
  if (has_common()) {untested();
    COMMON_COMPONENT* deflated_common = mutable_common()->deflate();
    if (deflated_common != common()) {untested();
      attach_common(deflated_common);
    }else{untested();
    }
  }else{untested();
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::expand()
{
  CARD::expand();
  if (has_common()) {
    COMMON_COMPONENT* new_common = common()->clone();
    new_common->expand(this);
    COMMON_COMPONENT* deflated_common = new_common->deflate();
    if (deflated_common != common()) {
      attach_common(deflated_common);
    }else{untested();
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::precalc_first()
{
  CARD::precalc_first();
  if (has_common()) {
    try {
      mutable_common()->precalc_first(scope());
    }catch (Exception_Precalc& e) {
      error(bWARNING, long_label() + ": " + e.message());
    }
    _mfactor = common()->mfactor();
  }else{
  }
  
  _mfactor.e_val(1, scope());
  _value.e_val(0.,scope());
  trace1(long_label().c_str(), double(_mfactor));
  if (const COMPONENT* o = prechecked_cast<const COMPONENT*>(owner())) {
    _mfactor_fixed = o->mfactor() * _mfactor;
  }else{
    _mfactor_fixed =  _mfactor;
  } 
  trace1(long_label().c_str(), _mfactor_fixed);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::precalc_last()
{
  CARD::precalc_last();
  if (has_common()) {
    try {
      mutable_common()->precalc_last(scope());
    }catch (Exception_Precalc& e) {
      error(bWARNING, long_label() + ": " + e.message());
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::map_nodes()
{
  assert(is_device());
  assert(0 <= min_nodes());
  //assert(min_nodes() <= net_nodes());
  assert(net_nodes() <= max_nodes());
  //assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (int ii = 0; ii < ext_nodes()+int_nodes(); ++ii) {
    _n[ii].map();
  }

  if (subckt()) {
    subckt()->map_nodes();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::tr_iwant_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt()) {
      subckt()->tr_iwant_matrix();
    }else{untested();
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::ac_iwant_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt()) {
      subckt()->ac_iwant_matrix();
    }else{untested();
    }
  }else{
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
  set_label(Label);
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
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_value(double v, COMMON_COMPONENT* c)
{
  if (c != _common) {
    detach_common();
    attach_common(c);
  }else{
  }
  set_value(v);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_param_by_name(std::string Name, std::string Value)
{
  if (has_common()) {
    COMMON_COMPONENT* c = common()->clone();
    assert(c);
    c->set_param_by_name(Name, Value);
    attach_common(c);
  }else{
    CARD::set_param_by_name(Name, Value);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_param_by_index(int i, std::string& Value, int offset)
{
  if (has_common()) {untested();
    COMMON_COMPONENT* c = common()->clone();
    assert(c);
    c->set_param_by_index(i, Value, offset);
    attach_common(c);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0: _value = Value; break;
    case 1: _mfactor = Value; break;
    default: CARD::set_param_by_index(i, Value, offset);
    }
  }
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::param_is_printable(int i)const
{
  if (has_common()) {
    return common()->param_is_printable(i);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0:  return value().has_hard_value();
    case 1:  return _mfactor.has_hard_value();
    default: return CARD::param_is_printable(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_name(int i)const
{
  if (has_common()) {
    return common()->param_name(i);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0:  return value_name();
    case 1:  return "m";
    default: return CARD::param_name(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_name(int i, int j)const
{
  if (has_common()) {untested();
    return common()->param_name(i,j);
  }else{
    if (j == 0) {
      return param_name(i);
    }else if (i >= CARD::param_count()) {untested();
      return "";
    }else{untested();
      return CARD::param_name(i,j);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_value(int i)const
{
  if (has_common()) {
    return common()->param_value(i);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0:  return value().string();
    case 1:  return _mfactor.string();
    default: return CARD::param_value(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
const std::string COMPONENT::port_value(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].short_label();
}
/*--------------------------------------------------------------------------*/
const std::string COMPONENT::current_port_value(int)const 
{untested();
  unreachable();
  static std::string s;
  return s;
}
/*--------------------------------------------------------------------------*/
double COMPONENT::tr_probe_num(const std::string& x)const
{
  CS cmd(CS::_STRING, x);
  if (cmd.umatch("v")) {
    int nn = cmd.ctoi();
    return (nn > 0 && nn <= net_nodes()) ? _n[nn-1].v0() : NOT_VALID;
  }else if (Umatch(x, "error{time} |next{time} ")) {
    return (_time_by._error_estimate < BIGBIG) ? _time_by._error_estimate : 0;
  }else if (Umatch(x, "timef{uture} ")) {
    return (_time_by._error_estimate < _time_by._event) 
      ? _time_by._error_estimate
      : _time_by._event;
  }else if (Umatch(x, "event{time} ")) {
    return (_time_by._event < BIGBIG) ? _time_by._event : 0;
  }else{
    return CARD::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
const MODEL_CARD* COMPONENT::find_model(const std::string& modelname)const
{
  if (modelname == "") {
    throw Exception(long_label() + ": missing args -- need model name");
    unreachable();
    return NULL;
  }else{
    const CARD* c = NULL;
    {
      int bin_count = 0;
      for (const CARD* Scope = this; Scope && !c; Scope = Scope->owner()) {
	// start here, looking out
	try {
	  c = Scope->find_in_my_scope(modelname);
	}catch (Exception_Cant_Find& e1) {
	  // didn't find plain model.  try binned models
	  bin_count = 0;
	  for (;;) {
	    // loop over binned models
	    std::string extended_name = modelname + '.' + to_string(++bin_count);
	    try {
	      c = Scope->find_in_my_scope(extended_name);
	    }catch (Exception_Cant_Find& e2) {
	      // that's all .. looked at all of them
	      c = NULL;
	      break;
	    }
	    const MODEL_CARD* m = dynamic_cast<const MODEL_CARD*>(c);
	    if (m && m->is_valid(this)) {
	      //matching name and correct bin
	      break;
	    }else{
	      // keep looking
	    }
	  }
	}
      }
      if (!c) {
	if (bin_count <= 1) {
	  throw Exception_Cant_Find(long_label(), modelname);
	}else{
	  throw Exception(long_label() + ": no bins match: " + modelname);
	}
	unreachable();
      }else{
      }
    }
    // found something, what is it?
    assert(c);
    const MODEL_CARD* model = dynamic_cast<const MODEL_CARD*>(c);
    if (!model) {untested();
      throw Exception_Type_Mismatch(long_label(), modelname, ".model");
    }else if (!model->is_valid(this)) {itested();
      error(bWARNING, long_label() + ", " + modelname
	   + "\nmodel and device parameters are incompatible, using anyway\n");
    }else{
    }
    assert(model);
    return model;
  }
}
/*--------------------------------------------------------------------------*/
/* q_eval: queue this device for evaluation on the next pass,
 * with a check against doing it twice.
 */
void COMPONENT::q_eval()
{
  if(!is_q_for_eval()) {
    mark_q_for_eval();
    _sim->_evalq_uc->push_back(this);
  }else{itested();
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::tr_queue_eval()
{
  if(tr_needs_eval()) {
    q_eval();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR COMPONENT::tr_review()
{
  _time_by.reset();
  if(has_common()) {
    return _common->tr_review(this);
  }else{
    return _time_by;
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::tr_accept()
{
  if(has_common()) {
    _common->tr_accept(this);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::use_obsolete_callback_parse()const
{
  if (has_common()) {
    return common()->use_obsolete_callback_parse();
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::use_obsolete_callback_print()const
{
  if (has_common()) {
    return common()->use_obsolete_callback_print();
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::obsolete_move_parameters_from_common(const COMMON_COMPONENT* dc)
{
  assert(dc);
  _value   = dc->value();
  _mfactor = dc->mfactor();
}
/*--------------------------------------------------------------------------*/
/* volts_limited: transient voltage, best approximation, with limiting
 */
double COMPONENT::volts_limited(const node_t & n1, const node_t & n2)
{
  bool limiting = false;

  double v1 = n1.v0();
  assert(v1 == v1);
  if (v1 < _sim->_vmin) {
    limiting = true;
    v1 = _sim->_vmin;
  }else if (v1 > _sim->_vmax) {
    limiting = true;
    v1 = _sim->_vmax;
  }

  double v2 = n2.v0();
  assert(v2 == v2);
  if (v2 < _sim->_vmin) {
    limiting = true;
    v2 = _sim->_vmin;
  }else if (v2 > _sim->_vmax) {
    limiting = true;
    v2 = _sim->_vmax;
  }

  if (limiting) {
    _sim->_limiting = true;
    if (OPT::dampstrategy & dsRANGE) {
      _sim->_fulldamp = true;
      error(bTRACE, "range limit damp\n");
    }
    if (OPT::picky <= bTRACE) {untested();
      error(bNOERROR,"node limiting (n1,n2,dif) "
	    "was (%g %g %g) now (%g %g %g)\n",
	    n1.v0(), n2.v0(), n1.v0() - n2.v0(), v1, v2, v1-v2);
    }
  }

  return dn_diff(v1,v2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
