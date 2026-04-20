/*                              -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2022, 2023, 2025 Felix Salfelder
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
 * subcircuit stuff
 * base class for other elements using internal subckts
 * netlist syntax:
 * device: Xxxxx <nodelist> <subckt-name> <args>
 * model:  .subckt <subckt-name> <nodelist>
 *	   (device cards)
 *	   .ends <subckt-name>
 * storage note ...
 * the .subckt always has a comment at the hook point, so a for loop works
 * the expansion (attact to the X) has all comments removed
 *	- need to process the entire ring - for doesn't work
 */
//testing=script 2016.09.16
#include "u_nodemap.h"
#include "e_node.h"
#include "globals.h"
#include "e_paramlist.h"
#include "e_subckt.h"
#include "e_hsparam.h"
#include "e_model.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
// components with one node are unlikely.
const int node_capacity_floor = 2;
/*--------------------------------------------------------------------------*/
static void grow_nodes(int Index, node_t*& n, int& capacity, int capacity_floor)
{
  if(Index < capacity){
  }else{
    int new_capacity = std::max(capacity, capacity_floor);
    while(new_capacity <= Index) {
      assert(new_capacity < new_capacity * 2);
      new_capacity *= 2;
    }
    node_t* new_nodes = new node_t[new_capacity];
    for(int i=0; i<capacity; ++i){
      new_nodes[i] = std::move(n[i]);
    }
    delete[] n;
    n = new_nodes;
    capacity = new_capacity;
  }
}
/*--------------------------------------------------------------------------*/
static COMMON_PARAMLIST Default_SUBCKT(CC_STATIC);
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT : public BASE_SUBCKT {
  friend class DEV_SUBCKT_PROTO;
  friend class DEV_MODULE_PROTO;
protected:
  const BASE_SUBCKT* _parent;
private:
  node_t* _nodes{nullptr};
  int _node_capacity{0};
  std::vector<std::string> _port_name; // common?
  static int	_count;
private:
  explicit	DEV_SUBCKT(const DEV_SUBCKT&);
public:
  explicit	DEV_SUBCKT(COMMON_COMPONENT* c=nullptr);
		~DEV_SUBCKT()		{--_count; delete[] _nodes; _node_capacity = 0; }
  CARD*		clone()const override;
  CARD*		clone_instance()const override;
private:
  void		set_port_by_index(int Index, std::string& Value) override;
  int		set_port_by_name(std::string&, std::string&) override;
  int set_param_by_name(std::string Name, std::string Value)override;
  void set_param_by_index(int i, std::string& Value, int j)override;
private: // override virtual
  char		id_letter()const override	{return 'X';}
  bool		print_type_in_spice()const override {return true;}
  std::string   value_name()const override	{return "#";}
  int		max_nodes()const override;
  int		ext_nodes()const override	{return _node_capacity;}
  int		min_nodes()const override	{return 0;}
  int		matrix_nodes()const override	{return 0;}
  int		net_nodes()const override	{return _net_nodes;}
  void		precalc_first()override;
  bool		makes_own_scope()const override;
  int		is_valid()const override;
  CARD_LIST*	   scope()override;
  const CARD_LIST* scope()const override	{return const_cast<DEV_SUBCKT*>(this)->scope();}

  void		expand() override;
private:
  void		precalc_last()override;
  double	tr_probe_num(const std::string&)const override;
  int param_count_dont_print()const override {return 0; } // {common()->COMMON_COMPONENT::param_count();}

  node_t& n_(int i)const override {
    assert(i>=0);
    if(i<_node_capacity) {
      assert(_nodes);
      return _nodes[i];
    }else{
      // getting here in d_subckt.error3.ckt
      static node_t dummy;
      return dummy;
    }
  }
  std::string port_name(int i)const override;
public:
  static int	count()			{untested();return _count;}
} p1(&Default_SUBCKT);
int DEV_SUBCKT::_count = -1;
/*--------------------------------------------------------------------------*/
class DEV_MODULE_PROTO : public DEV_SUBCKT {
private:
  explicit	DEV_MODULE_PROTO(const DEV_MODULE_PROTO&p) : DEV_SUBCKT(p)
  {
    new_subckt();
  }
public:
  explicit	DEV_MODULE_PROTO(COMMON_COMPONENT* c=nullptr) : DEV_SUBCKT(c) {}
		~DEV_MODULE_PROTO(){}
  bool		makes_own_scope()const override	{ return true;}
  CARD_LIST*	   scope()override		{ untested(); return subckt();}
  const CARD_LIST* scope()const override	{ return subckt();}
  CARD*		clone()const override		{ return new DEV_MODULE_PROTO(*this);}
} p0(&Default_SUBCKT);
DISPATCHER<CARD>::INSTALL d0(&device_dispatcher, "module", &p0);
/*--------------------------------------------------------------------------*/
//BUG// fixed limit on number of ports, spice only
#define PORTS_PER_SUBCKT 100
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT_PROTO : public DEV_SUBCKT {
private:
  explicit	DEV_SUBCKT_PROTO(const DEV_SUBCKT_PROTO&p);
public:
  explicit	DEV_SUBCKT_PROTO(COMMON_COMPONENT* c=nullptr);
		~DEV_SUBCKT_PROTO(){}
public: // override virtual
  char		id_letter()const override	{untested();return '\0';}
  CARD*		clone_instance()const override;
  bool		print_type_in_spice()const override { untested();unreachable(); return false;}
  std::string   value_name()const override	{untested();incomplete(); return "";}
  std::string   dev_type()const override	{itested(); return "";}
  int		max_nodes()const override	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const override	{return 0;}
  int		matrix_nodes()const override	{untested();return 0;}
  int		net_nodes()const override	{return _net_nodes;}
  CARD*		clone()const override		{return new DEV_SUBCKT_PROTO(*this);}
  bool		makes_own_scope()const override	{return true;}
  CARD_LIST*	   scope()override		{return subckt();}
  const CARD_LIST* scope()const override	{return subckt();}
  std::string port_name(int i)const override;
} pp(&Default_SUBCKT);
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "X|subckt", &pp);
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO(const DEV_SUBCKT_PROTO& p)
  :DEV_SUBCKT(p)
{
  new_subckt();
  attach_common(nullptr);
  attach_common(p.common()->clone());
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO(COMMON_COMPONENT* c)
  :DEV_SUBCKT(c)
{
  new_subckt();
}
/*--------------------------------------------------------------------------*/
std::string DEV_SUBCKT_PROTO::port_name(int i) const
{
  if(i>=int(_port_name.size())) {
    return port_value(i);
  }else if(_port_name[i]!="") { untested();
    return _port_name[i];
  }else{ untested();
    return port_value(i);
  }
}
/*--------------------------------------------------------------------------*/
CARD* DEV_SUBCKT_PROTO::clone_instance()const
{
#if 1
  DEV_SUBCKT* new_instance = dynamic_cast<DEV_SUBCKT*>(p1.clone());
#else
  DEV_SUBCKT* new_instance = dynamic_cast<DEV_SUBCKT*>(DEV_SUBCKT::clone());
  assert(new_instance);
  for (int ii = 0;  ii < net_nodes();  ++ii) { untested();
    new_instance->n_(ii) = node_t();
  }
  new_instance->_net_nodes = 0;
#endif

  new_instance->_parent = this;
  assert(new_instance->is_device());
  return new_instance;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::set_port_by_index(int Index, std::string& Value)
{
  grow_nodes(Index, _nodes, _node_capacity, node_capacity_floor);
  BASE_SUBCKT::set_port_by_index(Index, Value);
}
/*--------------------------------------------------------------------------*/
int DEV_SUBCKT::set_port_by_name(std::string& name, std::string& value)
{
  if(_parent){
    trace2("DEV_SUBCKT::spbn", name, value);
    return BASE_SUBCKT::set_port_by_name(name, value);
  }else{
    int index = net_nodes();
    // grow_nodes(index, _nodes, _node_capacity, node_capacity_floor);
    _port_name.push_back(name);
    set_port_by_index(index, value); // bumps _net_nodes
    return index;
  }
}
/*--------------------------------------------------------------------------*/
int DEV_SUBCKT::max_nodes() const
{
  if(_parent == &pp){
    // spice..
    return _parent->max_nodes();
  }else if(_parent){
    return static_cast<CARD const*>(_parent)->net_nodes();
  }else if(this == &pp){ untested();
    incomplete();
    unreachable();
    return 100000;
  }else{
    // building proto. allow one more.
    // they come in order anyway.
    return std::min(INT_MAX, net_nodes() + 1);
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST* DEV_SUBCKT::scope()
{
  if(is_device()){
    return COMPONENT::scope();
  }else{ untested();
    return subckt();
  }
}
/*--------------------------------------------------------------------------*/
// TODO: local parameters with ranges
int DEV_SUBCKT::is_valid() const
{itested();
  PARAM_LIST const* params;
  if(_parent){itested();
    assert(_parent->subckt());
    params = _parent->subckt()->params();
  }else{ untested();
    assert(subckt());
    params = subckt()->params();
  }
  assert(subckt());
  trace1("DEV_SUBCKT::is_valid I", long_label());
  PARAM_INSTANCE v = params->deep_lookup("_..is_valid");
  trace2("DEV_SUBCKT::is_valid II", long_label(), v.string());
  Base const* x = v.e_val(nullptr, subckt()->params());
  Integer c;
  Integer* res = c.assign(x);
  if(!res) {itested();
    return true;
  }else{itested();
    assert(x);
    trace1("DEV_SUBCKT::is_valid III", typeid(*x).name());
    int a = res->value();
    delete res;
    return a;
  }
}
/*--------------------------------------------------------------------------*/
CARD* DEV_SUBCKT::clone()const
{
  DEV_SUBCKT* new_instance = new DEV_SUBCKT(*this);
  assert(!new_instance->subckt());

  if (this == &p1){
    // cloning from static, empty model
    // has no parent.
    new_instance->new_subckt(); // from DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO
  }else if(_parent){
    new_instance->_parent = _parent;
    assert(new_instance->is_device());
  }else{ untested();
    // verilog. build proto
    // assert(!new_instance->is_device());
    if(new_instance->subckt()){ untested();
    }else{ untested();
     new_instance->new_subckt();
    }
  }

  return new_instance;
}
/*--------------------------------------------------------------------------*/
CARD* DEV_SUBCKT::clone_instance() const
{
  auto m = new DEV_SUBCKT(*this);
  auto s = prechecked_cast<DEV_SUBCKT*>(m);
    for (int ii = 0;  ii < s->net_nodes();  ++ii) {
      s->n_(ii) = nullptr;
      assert(!s->n_(ii).is_connected());
    }
  s->_net_nodes = 0; // needed in v_instance: 274?? v_paramset.module.1.gc
  s->_parent = this;
  assert(s->is_device());
  return m;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(COMMON_COMPONENT* c)
  :BASE_SUBCKT(c),
   _parent(nullptr)
{
  ++_count;
  assert(_nodes == nullptr);
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p),
   _parent(p._parent)
{
  trace3("DEV_SUBCKT::DEV_SUBCKT", short_label(), net_nodes(), p.max_nodes());
  _node_capacity = p.net_nodes();
  if(_node_capacity){
    _nodes = new node_t[_node_capacity];
  }else{
    assert(_nodes == nullptr);
  }
  if(p.is_device()){
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      _nodes[ii] = p._nodes[ii];
    }
  }else{ untested();
    for (int ii = 0;  ii < net_nodes();  ++ii) { untested();
      assert(!_nodes[ii].n_());
    }
  }
  assert(!subckt());
  ++_count;
}
/*--------------------------------------------------------------------------*/
int DEV_SUBCKT::set_param_by_name(std::string Name, std::string Value)
{
  assert(_parent);
  assert(_parent->subckt());
  COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
  assert(c);

  if (Name[0] == '$'){
    return BASE_SUBCKT::set_param_by_name(Name, Value);
  }else if(_parent==&pp || !_parent) {
    // spice.
    trace3("spice spbn", Name, Value, c);
    int i = BASE_SUBCKT::set_param_by_name(Name,Value);
    trace1("spbn done", common());
    return i;
  }else{
    PARAM_LIST::const_iterator p = _parent->subckt()->params()->find(Name);
    if(p != _parent->subckt()->params()->end()){
      return BASE_SUBCKT::set_param_by_name(Name,Value);
    }else{itested();
      throw Exception_No_Match(Name);
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::set_param_by_index(int i, std::string& Value, int Offset)
{
  COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
  assert(c);
  auto p=dynamic_cast<const DEV_SUBCKT*>(_parent);
  assert(p);
  int param_count = p->subckt()->params()->size();
  // assert(param_count == p->param_count()); BUG, incomplete

  if(p && i<param_count){
    std::string param_name = p->subckt()->params()->name(i);
    auto cc = c->mutable_clone();
    trace2("DS::spbi found name", i, param_name);
    cc->set_param_by_name(param_name, "");
    cc->set_param_by_name(param_name, Value);
    attach_common(cc);
  }else{ untested();
    trace2("DS::spbi out of range", long_label(), i);
    throw Exception_Too_Many(i+1, 0, Offset);
  }
}
/*--------------------------------------------------------------------------*/
std::string DEV_SUBCKT::port_name(int i)const
{
  if (const DEV_SUBCKT* p=dynamic_cast<const DEV_SUBCKT*>(_parent)) {
    if (i<p->net_nodes()){
      return p->port_name(i);
    }else{
      return "";
    }
  }else if(_parent) { untested();
    unreachable();
    return "";
  }else if(i<int(_port_name.size())) {
    if(_port_name[i]!=""){
      return _port_name[i];
    }else{ untested();
      return  port_value(i);
    }
  }else if(i<net_nodes()) {
    return port_value(i);
  }else{itested();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::expand()
{
  BASE_SUBCKT::expand();
  trace4("DEV_SUBCKT::expand", long_label(), max_nodes(), is_device(), common());

  assert(is_device());
  if(_parent == &pp){ untested();
    COMMON_PARAMLIST const* c = prechecked_cast<COMMON_PARAMLIST const*>(common());
    assert(c);
    // first time spice
    assert(c->modelname()!="");
    const CARD* model = find_looking_out(c->modelname());
    if (_parent == dynamic_cast<const DEV_SUBCKT_PROTO*>(model)) { untested();
      // good
    }else if (auto ms = dynamic_cast<const MODEL_SUBCKT*>(model)) { untested();
      // good
      _parent = prechecked_cast<BASE_SUBCKT const*>(ms->component_proto());
      assert(_parent);
    }else if (_parent == dynamic_cast<const DEV_MODULE_PROTO*>(model)) { untested();
      // good
    }else if (dynamic_cast<const BASE_SUBCKT*>(model)) { untested();
      // bad
    }else{ untested();
      // bad
    }
    assert(!_parent->is_device()); // really?
  }else{itested();
  }

  trace1("DEV_SUBCKT::expand1", max_nodes());


  if(!_parent || _parent==&pp){ untested();
    // not a device. probably a prototype
  }else{
    COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
    assert(c);
    assert(_parent);
    assert(_parent->subckt());
    assert(_parent->subckt()->params());
    PARAM_LIST* pl = const_cast<PARAM_LIST*>(_parent->subckt()->params());
    assert(pl);
    assert(c->_params.is_try_again(pl)); // precalc_first?

    trace3("expand", short_label(), net_nodes(), max_nodes());
    renew_subckt(_parent, &(c->_params));

    subckt()->expand();

    // do connect port stuff here.
    // (part of allocate??)

    trace3("expand", long_label(), this, subckt()->size());
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_first()
{
  COMPONENT::precalc_first();

  if(subckt()) {
  }else{
    new_subckt();
  }

  assert(is_device());
  if(_parent == &pp){
    COMMON_PARAMLIST const* c = prechecked_cast<COMMON_PARAMLIST const*>(common());
    assert(c);
    // first time spice
    assert(c->modelname()!="");
    const CARD* model = find_looking_out(c->modelname());
    if ((_parent = dynamic_cast<const DEV_SUBCKT_PROTO*>(model))) { untested();
      // good
    }else if ((_parent = dynamic_cast<const DEV_MODULE_PROTO*>(model))) { untested();
      // good
    }else if (auto ms = dynamic_cast<const MODEL_SUBCKT*>(model)) {
      // good
      _parent = prechecked_cast<BASE_SUBCKT const*>(ms->component_proto());
      assert(_parent);
    }else if (dynamic_cast<const BASE_SUBCKT*>(model)) {
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt proto");
    }else{
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt");
    }
  }else{itested();
  }

  if(_parent) {
    auto c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
    assert(c);

    PARAM_LIST* pl = const_cast<PARAM_LIST*>(_parent->subckt()->params());
    assert(pl);
#ifndef NDEBUG
    if(c->_params.is_try_again(pl)){
    }else{
    }
#endif
    c->_params.set_try_again(pl);

#if 0
    // deletes parameters
    subckt()->attach_params(&(c->_params), scope());
#else
    subckt()->params()->set_try_again(nullptr);
    subckt()->params()->eval_copy(c->_params, scope()->params());
    subckt()->params()->set_try_again(&c->_params);
#endif
    subckt()->precalc_first();
  }else{
  }

}
/*--------------------------------------------------------------------------*/
bool DEV_SUBCKT::makes_own_scope() const
{
  return !_parent; //  || _parent == &pp;
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_last()
{
  // skip BASE_SUBCKT, need params before sckt->precalc
  COMPONENT::precalc_last();

  COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
  assert(c);

  // move to COMPONENT or BASE_SUBCKT?
  if(HS_PARAM const* h = hsparam()){
    PARAM_LIST const* s = scope()->params();
    h->precalc_hierarchy(s, subckt()->params());
  }else{ untested();
    unreachable();
  }

  subckt()->params()->set_try_again(nullptr);
  trace1("DEV_SUBCKT::precalc_last5", &c->_params);
  subckt()->params()->eval_copy(c->_params, scope()->params());
  subckt()->params()->set_try_again(&c->_params);
  trace1("DEV_SUBCKT::precalc_last2", mfactor());

  subckt()->precalc_last();
  assert(!is_constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
double DEV_SUBCKT::tr_probe_num(const std::string& x)const
{untested();
  if (Umatch(x, "p ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"P");
    }      
    return power;
  }else if (Umatch(x, "pd ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PD");
    }      
    return power;
  }else if (Umatch(x, "ps ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PS");
    }      
    return power;
  }else{untested();
    return COMPONENT::tr_probe_num(x);
  }
  /*NOTREACHED*/
}
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
