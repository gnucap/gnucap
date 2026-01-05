/*$Id: e_compon.cc  $ -*- C++ -*-
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
//testing=script 2014.07.04
#include "u_lang.h"
#include "u_nodemap.h"
#include "u_parameter.h"
#include "u_nodemap.h"
#include "e_hsparam.h"
#include "e_elemnt.h"
#include "e_model.h"
#include "e_cardlist.h"
#include <cfenv>
#include <typeindex>
/*--------------------------------------------------------------------------*/
static void check_pool_consistency()
{
#ifdef DEBUG_POOL
  COMMON_COMPONENT::check_pool_consistency();
#endif
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(const COMMON_COMPONENT& p)
  :CKT_BASE(p),
   _modelname(p._modelname),
   _model(p._model),
   _attach_count(0)
{
  attach_next(p._next);
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(int c)
  :CKT_BASE(),
   _modelname(),
   _model(nullptr),
   _attach_count(c)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common,destruct", _attach_count);
  if(_attach_count == 0){
    // not attached to anything.
  }else if(_attach_count == CC_STATIC) {
    // static, not attached to anything.
    unlink_common(this);
  }else if(_attach_count > CC_STATIC) { untested();
    // static, still attached to another common
    // the other is static (presumably), but
    // there seems no way to influence destruction order
  }else{ untested();
    assert(0 && "common still in use");
  }
  detach_next();
  assert(!_next);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::attach_common(COMMON_COMPONENT*c, COMMON_COMPONENT**to)
{
  trace1("attach", c);

  assert(to);
  if (c == *to) {
    // The new and old are the same object.  Do nothing.
  }else if (!c) {
    // There is no new common.  probably a simple element
    detach_common(to);
  }else if (!*to) {
    // No old one, but have a new one.
    unique_common(&c);
    ++(c->_attach_count);
    trace1("++1", c->_attach_count);
    *to = c;
  }else if (*c != **to) {
    // They are different, usually by edit.
    unique_common(&c);
    assert(c != *to);
    assert(*c != **to);
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
  }else if (c->_attach_count == CC_STATIC) { untested();
    // need to cleanup anyway.
    c->detach_next();
  }else{untested();
    trace2("identical", c->_attach_count, *to);
    assert(!c->has_less());
    // The new and old are identical.
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::detach_common(COMMON_COMPONENT** from)
{
  trace1("detach", *from);
  assert(from);
  if (*from) {
    assert((**from)._attach_count > 0);
    --((**from)._attach_count);
    trace1("--", (**from)._attach_count);
    if ((**from)._attach_count == 0) {
      trace1("delete", (**from)._attach_count);
      unlink_common(*from);
      delete *from;
    }else if ((**from)._attach_count == CC_STATIC) {
      trace1("cleanup", (**from)._attach_count);
      (*from)->detach_next();
    }else{
      trace1("nodelete", (**from)._attach_count);
    }
    *from = nullptr;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::unique_common(COMMON_COMPONENT**c)
{
  assert(c);
  assert(*c);

  COMMON_COMPONENT* d = *c;

  int ac = (*c)->_attach_count;
  trace1("unique_common", (*c)->_attach_count);

  if(!(*c)->has_less()){
  }else if(ac == 0){
    d = COMMON_COMPONENT::_commons[*c];
    if(d == *c){
      // assert(d->_attach_count);
    }else{
      // using d which is attached to sth else.
      // c is no longer needed.
      assert(d->_attach_count);
      delete *c;
      *c = d;
    }
  }else if(ac == CC_STATIC){
    d = COMMON_COMPONENT::_commons[*c];
    assert(d == *c);
  }else if(ac > CC_STATIC){
  }else{
    assert(*c == COMMON_COMPONENT::_commons[*c]);
    assert(d->_attach_count);
  }

#if 0
  if(d != *c){
    assert(!(*d < **c) || !(*d == **c));
    assert(!(**c < *d) || !(*d == **c));
    assert(!(**c < *d && *d < **c) || (*d == **c));
    assert(*d == **c);
  }else{
    assert(*d == **c);
  }
#endif

}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::unlink_common(COMMON_COMPONENT*c)
{
  trace1("unlink", c->_attach_count);
  assert(c->_attach_count == 0
       ||c->_attach_count == CC_STATIC);
  if(c->has_less()){
    assert(COMMON_COMPONENT::_commons.size());
    size_t howmany = COMMON_COMPONENT::_commons.unlink(c);
    assert(howmany);
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
  size_t start = cmd.cursor();
  size_t here = cmd.cursor();
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
    size_t start = cmd.cursor();
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
    size_t start = cmd.cursor();
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
void COMMON_COMPONENT::set_param_by_index(int i, std::string& V, int Offset)
{
  if(has_next()) { untested();
    COMMON_COMPONENT* m = next_common()->clone(); // shared?
    m->set_param_by_index(i, V, Offset);
    attach_next(m);
  }else{
    throw Exception_Too_Many(i+1, 0, Offset);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::param_is_printable(int i)const
{
  if(next_common()) {
    return next_common()->param_is_printable(i);
  }else{ untested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_name(int i)const
{
  if(next_common()) {
    return next_common()->param_name(i);
  }else{ untested();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_name(int i, int j) const
{
  if(j==0){ untested();
    return param_name(i);
  }else if(next_common()) { untested();
    return next_common()->param_name(i, j);
  }else{ untested();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_value(int i) const
{
  if(next_common()) {
    return next_common()->param_value(i);
  }else{ untested();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::expand(const COMPONENT* comp)
{
  if(has_next()){
    COMMON_COMPONENT* c = next_common()->clone();
    assert(c);
    try {
      c->expand(comp);
      attach_next(c);
    }catch (Exception const& e) { untested();
      attach_next(c);
      throw e;
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::precalc_first_chain(PARAM_LIST const* p)
{
  assert(p);

  if(!_next){
  }else if(_next->params()){
    // depth first.
    COMMON_COMPONENT* c = _next->mutable_clone();
    assert(c);
    try {
      c->precalc_first_chain(p);
      attach_next(c);
      _next->params()->set_verilog(p->is_verilog());
      _next->params()->set_try_again(p);
      p = _next->params();
      assert(p);
    }catch (Exception const& e) { untested();
      attach_next(c);
      throw e;
    }
  }else{
  }

  precalc_first(p);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::precalc_last_chain(PARAM_LIST const* p)
{
  assert(p);

  if(!_next){
  }else if(_next->params()){
    // depth first.
    COMMON_COMPONENT* c = _next->mutable_clone();
    assert(c);
    try {
      c->precalc_last_chain(p);
      attach_next(c);
      _next->params()->set_verilog(p->is_verilog());
      _next->params()->set_try_again(p);
      p = _next->params();
      assert(p);
    }catch (Exception const& e) { untested();
      attach_next(c);
      throw e;
    }
  }else{
  }

  precalc_last(p);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::tr_eval(ELEMENT*x)const
{untested();
  assert(_model);
  _model->tr_eval(x);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::ac_eval(ELEMENT*x)const
{
  if(_model){ untested();
    _model->ac_eval(x);
  }else{
    // should not get here.
    // but need to get rid of _model anyway.
    // incomplete();
  }
}
/*--------------------------------------------------------------------------*/
int COMMON_COMPONENT::compare(const COMMON_COMPONENT& x) const
{
#ifndef NDEBUG
  if(this == &x){ itested();
#ifndef DEBUG_POOL
    unreachable();
    assert(0);
#endif
    return 0;
  }else{
  }
#endif
  intptr_t c0;
#if 0
  c0 = intptr_t(&typeid(*this)) - intptr_t(&typeid(x));
  if(c0 < 0) {
    return -1;
  }else if(c0 > 0) {
    return 1;
  }else{
    assert(b == a);
  }
#else
  std::type_index a(typeid(*this));
  std::type_index b(typeid(x));
  if(a < b){
    return -1;
  }else if(a > b){
    return 1;
  }else{
  }
#endif

  c0 =  intptr_t(next_common()) - intptr_t(x.next_common());
  if(c0 < 0) {
    return -1;
  }else if(c0 > 0) {
    return 1;
  }else{
  }

  c0 = intptr_t(_model) - intptr_t(x._model);
  if(c0 < 0) {
    return -1;
  }else if(c0 > 0) {
    return 1;
  }else{
  }

  if(int c1 = _modelname.compare(x._modelname)) {
    return c1;
  }else{
  }

  return 0;
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::operator==(const COMMON_COMPONENT& x)const
{
#ifdef NDEBUG
  if(this == &x){
    // redundant call, should not get here.
    unreachable();
    // return true;
  }else if(&typeid(*this) != &typeid(x)){ untested();
    // impossible call, should not get here.
    assert(0);
    unreachable();
    // return false;
  }else{
  }
#endif
  // return false; // test re-attach logic. BUG: breaks mos1.
  return (_modelname == x._modelname
	  && _next == x._next
	  && _model == x._model);
}
/*--------------------------------------------------------------------------*/
int COMMON_COMPONENT::set_param_by_name(std::string Name, std::string Value)
{
  trace2("spbn", Name, Value);
  if(Name[0] == '$'){
    if(!has_next()) {
      auto n = &HS_PARAM::hs_param;
      attach_next(n);
      trace1("CC::spbn new hsparam", _next);
      assert(_next == n);
    }else{
    }

    // todo: figure out index.
    COMMON_COMPONENT* c = next_common()->clone();
    int idx = c->set_param_by_name(Name, Value); //  + param_count();
    attach_next(c);
    return idx;
  }else if (has_parse_params_obsolete_callback()) {itested();
    std::string args(Name + "=" + Value);
    CS cmd(CS::_STRING, args); //obsolete_callback
    bool ok = parse_params_obsolete_callback(cmd); //BUG//callback
    if (!ok) {
      throw Exception_No_Match(Name);
    }else{untested();
    }
    return 0;
  }else{
    //BUG// ugly linear search
    for (int i = 0;  i < param_count();  ++i) {
      for (int j = 0;  param_name(i,j) != "";  ++j) {
	if (Umatch(Name, param_name(i,j) + ' ')) {
	  set_param_by_index(i, Value, 0/*offset*/);
	  return i; //success
	}else{
	  //keep looking
	}
      }
    }
    {
    }
    throw Exception_No_Match(Name);
  }
}
/*--------------------------------------------------------------------------*/
//BUG// This is a kluge for the spice_wrapper, to disable virtual functions.
// It is called during expansion only.

int COMMON_COMPONENT::Set_param_by_name(std::string Name, std::string Value)
{untested();
  assert(!has_parse_params_obsolete_callback());
  
  //BUG// ugly linear search
  for (int i = 0;  i < COMMON_COMPONENT::param_count();  ++i) {itested();
    for (int j = 0;  COMMON_COMPONENT::param_name(i,j) != "";  ++j) {itested();
      if (Umatch(Name, COMMON_COMPONENT::param_name(i,j) + ' ')) {itested();
	COMMON_COMPONENT::set_param_by_index(i, Value, 0/*offset*/);
	return i; //success
      }else{untested();
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
  trace1("COMMON_COMPONENT::parse_params_obsolete_callback", cmd.tail());
  std::string val;
  if(cmd.match1('$')){
    size_t here = cmd.cursor();

    std::string name;
    cmd >> name;
    if(cmd >> '='){
      val = cmd.ctos(",=;)", "\"'{(", "\"'})");
      set_param_by_name(name, val);
      assert(cmd);
    }else{ untested();
      cmd.reset_fail(here);
    }
    return cmd;
  }else if (cmd.umatch( "tnom {=}")){ untested();
    cmd >> val;
    set_param_by_name("$tnom_c", val);
    return true;
  }else if (cmd.umatch("dtemp {=}")){ untested();
    cmd >> val;
    set_param_by_name("$dtemp", val);
    return true;
  }else if (cmd.umatch("temp {=}")){
    cmd >> val;
    set_param_by_name("$temp_c", val);
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT(COMMON_COMPONENT* c)
  :CARD(),
   _time_by()
{
  if (_sim) {
    _sim->uninit();
  }else{
  }
  attach_common(c);
  assert(_common == c);
}
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT(const COMPONENT& p)
  :CARD(p),
   _converged(p._converged),
   _mfactor_fixed(p._mfactor_fixed),
   _time_by(p._time_by),
   _net_nodes(p._net_nodes)
{
  if (_sim) {
    _sim->uninit();
  }else{untested();
  }
  attach_common(p._common);
  assert(_common == p._common);
  if(has_attributes(p.id_tag())) {
    set_attributes(id_tag()) = attributes(p.id_tag());
  }else{
  }
}
/*--------------------------------------------------------------------------*/
COMPONENT::~COMPONENT()
{
  if(has_common()){
  }else{
  }
  detach_common();
  if (_sim) {
    _sim->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
HS_PARAM const* COMPONENT::hsparam() const
{
  HS_PARAM const* h = nullptr;
  if(has_common()){
    h = common()->hsparam();
  }else{
  }
  if(h){
  }else{
    h = &HS_PARAM::hs_param;
  }
  return h;
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::node_is_grounded(int i)const 
{
  assert(i >= 0);
  assert(i < net_nodes());
  return n_(i).is_grounded();
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::node_is_connected(int i)const 
{
  assert(i >= 0);
  assert(i < net_nodes());
  return n_(i).is_connected();
}
/*--------------------------------------------------------------------------*/
int COMPONENT::set_port_by_name(std::string& int_name, std::string& ext_name)
{
  trace3("spbn", int_name, ext_name, net_nodes());
  for (int i=0; i<max_nodes(); ++i) {
    if (int_name == port_name(i)) {
      set_port_by_index(i, ext_name);
      return i;
    }else{
    }
  }
  {
    throw Exception_No_Match(int_name);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_by_index(int num, std::string& ext_name)
{
  if (num < max_nodes()) {
    n_(num).new_node(ext_name, this);  // Really look-up node, make new if needed.

    if (num+1 > _net_nodes) {
      // Update _net_nodes for net_nodes().  Not really a count.
      _net_nodes = short(num+1);
    }else{
      // probably assigning out of order.
    }
  }else{
    throw Exception_Too_Many(num+1, max_nodes(), 0/*offset*/);
  }

  // why store user number in NODE?
  // assert(n_(num).n_()->user_number() == n_(num).e_());
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_to_ground(int num)
{
  if (num < max_nodes()) {
    n_(num).set_to_ground(this);
    if (num+1 > _net_nodes) {
      _net_nodes = short(num+1);
    }else{untested();
    }
  }else{untested();
    throw Exception_Too_Many(num+1, max_nodes(), 0/*offset*/);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_dev_type(const std::string& new_type)
{
  if (common()) {
    if (new_type != dev_type()) {
      COMMON_COMPONENT* c = mutable_common()->mutable_clone();
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
    }else{
    }
    attach_common(deflated_common);
  }else{untested();
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::expand()
{
  trace2("COMPONENT::expand", long_label(), common());
  CARD::expand();
  if (_sim->is_first_expand()) {
    for(int i=net_nodes(); i<ext_nodes()+int_nodes(); ++i){
      n_(i).clear();
    }
  }else{ untested();
  }
  if (has_common()) {
    COMMON_COMPONENT* new_common = common()->clone();
    assert(*new_common == *common());
    assert(*common() == *new_common);
    new_common->expand(this);
    COMMON_COMPONENT* deflated_common = new_common->deflate();
    if(new_common == deflated_common) {
      new_common = nullptr;
    }else{
    }
    if (deflated_common != common()) {
      attach_common(deflated_common);
    }else{untested();
    }
    delete new_common;
  }else{
  }
  check_pool_consistency();
}
/*--------------------------------------------------------------------------*/
void COMPONENT::precalc_first()
{
  trace2("COMPONENT::precalc_first1", long_label(), common());
  for(int i = 0; i < min_nodes(); ++i){
    if(!node_is_connected(i)) {
      throw Exception(long_label() + ": invalid nodes");
    }else{
    }
  }
  CARD::precalc_first();

  if (has_common()) {
    COMMON_COMPONENT* c = mutable_common()->mutable_clone();
    assert(c);
    std::feclearexcept(FE_ALL_EXCEPT);
    try {
      c->precalc_first_chain(scope()->params());
    }catch (Exception_Precalc& e) { untested();
      error(bWARNING, long_label() + ": " + e.message());
    }catch (Exception& e) { untested();
      if(c != common()){ untested();
	delete c;
      }else{ untested();
      }
      throw e;
    }

    int n = std::fetestexcept(FE_ALL_EXCEPT);
    if (n & FE_INVALID) {
      if(c != common()){ untested();
	assert(0);
	delete c;
      }else{ untested();
      }
      throw(Exception("floating point error in " + long_label()));
    }else{
    }

    attach_common(c);
  }else{
  }

  trace2("COMPONENT::precalc_first2", long_label(), common());
}
/*--------------------------------------------------------------------------*/
// .. bypassed in mg_out_dev.
void COMPONENT::precalc_last()
{
  trace2("COMPONENT::precalc_last1", long_label(), common());
  PARAM_LIST const* params = scope()->params();
  CARD::precalc_last();
  if (has_common()) {
    COMMON_COMPONENT* c = mutable_common()->mutable_clone();
    assert(c);
    std::feclearexcept(FE_ALL_EXCEPT);
    try {
      c->precalc_last_chain(params);
    }catch (Exception_Precalc& e) {
      error(bWARNING, long_label() + ": " + e.message());
    }catch (Exception& e) {
      if(c != common()){
	delete c;
      }else{
      }
      throw e;
    }

    int n = std::fetestexcept(FE_ALL_EXCEPT);
    if (n & FE_INVALID) {
      if(c != common()){
	delete c;
      }else{
      }
      throw(Exception("floating point error in " + long_label()));
    }else{
    }

    attach_common(c);
  }else{
  }

  float mfactor_new;
  if(HS_PARAM const* hsp = hsparam()){
    mfactor_new = float(hsp->mfactor(params));
  }else{
    mfactor_new = float(HS_PARAM::hs_param.mfactor(params));
  }

  if(_mfactor_fixed != mfactor_new){
    _sim->mark_inc_mode_bad();
  }else{
  }
  _mfactor_fixed = mfactor_new;
  assert(mfactor() == _mfactor_fixed);
  trace2("COMPONENT::pl", long_label(), mfactor());
  trace2("COMPONENT::precalc_last2", long_label(), common());
}
/*--------------------------------------------------------------------------*/
void COMPONENT::map_nodes()
{
  assert(is_device());
  assert(0 <= min_nodes());
  //assert(min_nodes() <= net_nodes());
  assert(net_nodes() <= max_nodes());
  //assert(ext_nodes() + int_nodes() == matrix_nodes());
  trace3("COMPONENT::map_nodes", long_label(), ext_nodes(), int_nodes());

  for (int ii = 0; ii < ext_nodes()+int_nodes(); ++ii) {
    n_(ii).map();
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
    }else{itested();
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
    }else{itested();
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
  attach_common(Common);
  obsolete_set_value(Value);

  assert(node_count <= net_nodes());
  if(node_count){
    std::copy_n(Nodes, node_count, &n_(0));
  }else{itested();
  }
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
/*--------------------------------------------------------------------------*/
int COMPONENT::set_param_by_name(std::string Name, std::string Value)
{
  if(Name[0] == '$'){
    if(!has_common()) { untested();
      attach_common(&HS_PARAM::hs_param);
    }else{
    }
  }else{
  }

  if (has_common()) {
    COMMON_COMPONENT* c = mutable_common()->mutable_clone();
    assert(c);
    int index = c->set_param_by_name(Name, Value);
    attach_common(c);
    trace3("COMPONENT::spbn prepare HSP1c", Name, Value, index);
    return index;
  }else{ untested();
    throw Exception_No_Match(Name);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_param_by_index(int I, std::string& Value, int offset)
{
  if (has_common()) {
    COMMON_COMPONENT* c = mutable_common()->clone();
    assert(c);
    try{
      c->set_param_by_index(I, Value, offset);
    }catch(Exception_Too_Many const&){
      delete c;
      throw;
    }
    attach_common(c);
  }else{ untested();
    throw Exception_Too_Many(I, param_count(), 0);
  }
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::param_is_printable(int I)const
{
  trace1("COMPONENT::pip", I);
  if(has_common()) {
    return common()->param_is_printable(I);
  }else{
    return CARD::param_is_printable(I);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_name(int I)const
{
  if(has_common()) {
    return common()->param_name(I);
  }else{ untested();
    return CARD::param_name(I);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_name(int I, int j)const
{untested();
  if (j == 0) { untested();
    return param_name(I);
  }else if(has_common()){ untested();
    return common()->param_name(I, j);
  }else if (I >= CARD::param_count()) { untested();
    return "";
  }else{untested();
    return CARD::param_name(I,j);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_value(int I)const
{
  if(has_common()){
    return common()->param_value(I);
  }else{ untested();
    return CARD::param_value(I);
  }
}
/*--------------------------------------------------------------------------*/
const std::string COMPONENT::port_value(int i)const 
{
  assert(i >= 0);
  assert(i < net_nodes());

  int idx = n_(i).e_();
  assert(scope());
  assert(scope()->nodes());
  if(idx>=0){
    return scope()->nodes()->name(idx);
  }else{
    // d_subckt.error.2.gc
    return "?????";
  }
}
/*--------------------------------------------------------------------------*/
double COMPONENT::tr_probe_num(const std::string& x)const
{
  CS cmd(CS::_STRING, x);
  if (cmd.umatch("v")) {
    int nn = cmd.ctoi();
    return (nn > 0 && nn <= net_nodes()) ? n_(nn-1).v0() : NOT_VALID;
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
    return nullptr;
  }else{
    const CARD* c = nullptr;
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
	      c = nullptr;
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
    if (!model) {
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
// obsolete. use COMMON_COMPONENT::expand..
void COMPONENT::attach_model()
{
  COMMON_COMPONENT* c = common()->clone();
  c->attach_model(this);
  attach_common(c);
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
  }else{untested();
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
  }else{itested();
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
  }else{ untested();
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
    }else{
    }
    if (OPT::picky <= bTRACE) {itested();
      error(bNOERROR,"node limiting (n1,n2,dif) "
	    "was (%g %g %g) now (%g %g %g)\n",
	    n1.v0(), n2.v0(), n1.v0() - n2.v0(), v1, v2, v1-v2);
    }else{
    }
  }else{
  }

  return dn_diff(v1,v2);
}
/*--------------------------------------------------------------------------*/
HS_PARAM* COMMON_COMPONENT::hsparam()
{
  if(next_common()) {
    return next_common()->hsparam();
  }else{
    return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::set_mfactor(double m)
{
  COMMON_COMPONENT* nn;
  if(!has_next()) {untested();
    nn = HS_PARAM::hs_param.clone();
  }else{itested();
    nn = next_common()->mutable_clone();
  }
  nn->set_mfactor(m);
  attach_next(nn);
}
/*--------------------------------------------------------------------------*/
double COMMON_COMPONENT::temp_k(PARAM_LIST const* Scope) const
{
  if(has_hsparam()){
    return hsparam()->temp_k(Scope);
  }else{
    double t = CARD_LIST::card_list.params()->temperature();
    if(t==NOT_INPUT){ itested();
      return OPT::temp_k;
    }else{
      return t;
    }
  }
}
/*--------------------------------------------------------------------------*/
double COMMON_COMPONENT::temp_diff(PARAM_LIST const* Scope) const
{
  if(auto h = hsparam()){
    return h->temp_diff(Scope);
  }else{
    return 0;
  }
}
/*--------------------------------------------------------------------------*/
double COMMON_COMPONENT::temp_c(PARAM_LIST const* Scope) const
{
  return temp_k(Scope) - P_CELSIUS0;
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang) const
{
  if(has_next()){
    next_common()->print_common_obsolete_callback(o, lang);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
