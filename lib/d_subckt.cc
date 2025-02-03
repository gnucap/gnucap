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
      new_nodes[i] = n[i];
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
  node_t* _n{nullptr};
  int _node_capacity{0};
private:
  friend class DEV_SUBCKT_PROTO;
  const BASE_SUBCKT* _parent;
  std::vector<std::string> _port_name; // common?
protected:
  explicit	DEV_SUBCKT(const DEV_SUBCKT&);
public:
  explicit	DEV_SUBCKT(COMMON_COMPONENT* c=nullptr);
		~DEV_SUBCKT()		{--_count; delete[] _n; _node_capacity = 0; }
  CARD*		clone()const override;
  CARD*		clone_instance()const override;
private:
  void		set_port_by_index(int Index, std::string& Value) override;
  int		set_port_by_name(std::string&, std::string&) override;
  int set_param_by_name(std::string Name, std::string Value)override;
private: // override virtual
  bool		is_device()const override;
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
  bool		is_valid()const override;
  CARD_LIST*	   scope()override;
  const CARD_LIST* scope()const override	{return const_cast<DEV_SUBCKT*>(this)->scope();}

  void		expand() override;

private: // no ops for prototype
	 // temporary. pending lang_verilog adjustments
  void map_nodes()override	{if(is_device()){ BASE_SUBCKT::map_nodes();}else{} }
  void tr_begin()override	{if(is_device()){ BASE_SUBCKT::tr_begin();}else{} }
  void tr_restore()override	{if(is_device()){ BASE_SUBCKT::tr_restore();}else{} }
  void ac_begin()override	{if(is_device()){ BASE_SUBCKT::ac_begin();}else{} }
  void tr_load()override	{if(is_device()){ BASE_SUBCKT::tr_load();}else{} }
  void tr_accept()override	{if(is_device()){ BASE_SUBCKT::tr_accept();}else{} }
  void tr_regress()override	{if(is_device()){ BASE_SUBCKT::tr_regress();}else{} }
  void dc_final()override	{if(is_device()){ BASE_SUBCKT::dc_final();}else{}}
  void tr_final()override	{if(is_device()){ BASE_SUBCKT::tr_final();}else{}}
  void tr_advance()override	{if(is_device()){ BASE_SUBCKT::tr_advance();}else{} }
  TIME_PAIR tr_review()override	{if(is_device()){ return BASE_SUBCKT::tr_review();}else{
                                                  return TIME_PAIR(NEVER, NEVER);}}
  void dc_advance()override	{if(is_device()){ BASE_SUBCKT::dc_advance();}else{} }
  void do_ac()override		{if(is_device()){ BASE_SUBCKT::do_ac();}else{} }
  void ac_load()override	{if(is_device()){ BASE_SUBCKT::ac_load();}else{} }
  void ac_final()override	{if(is_device()){ BASE_SUBCKT::ac_final();}else{}}

  void tr_queue_eval()override{
    if(is_device()){
      BASE_SUBCKT::tr_queue_eval();
    }else{
    }
  }
  bool do_tr() override		{if(is_device()){ return BASE_SUBCKT::do_tr();}else{ return true;} }

  bool tr_needs_eval()const override{ untested();
    if(is_device()){untested();
      return BASE_SUBCKT::tr_needs_eval();
    }else{untested();
      return false;
    }
  }
private:
  void		precalc_last()override;
  double	tr_probe_num(const std::string&)const override;
  int param_count_dont_print()const override {return common()->COMMON_COMPONENT::param_count();}

  node_t& n_(int i)const override {
    assert(_n); assert(i>=0);
    if(i<_node_capacity) {
      return _n[i];
    }else{
      // getting here in d_subckt.error3.ckt
      static node_t dummy;
      return dummy;
    }
  }
  std::string port_name(int i)const override;
public:
  static int	count()			{untested();return _count;}

  static int	_count;
} p1(&Default_SUBCKT);
int DEV_SUBCKT::_count = -1;
DISPATCHER<CARD>::INSTALL d0(&device_dispatcher, "module", &p1);
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
  bool		is_device()const override	{return false;}
  bool		makes_own_scope()const override	{return true;}
  CARD_LIST*	   scope()override		{untested();return subckt();}
  const CARD_LIST* scope()const override	{return subckt();}
private: // no-ops for prototype
  void precalc_first()override {}
  void expand()override {}
  void precalc_last()override {}
  void map_nodes()override {}
  void tr_begin()override {}
  void tr_load()override {}
  TIME_PAIR tr_review() override { return TIME_PAIR(NEVER, NEVER);}
  void tr_accept()override {}
  void tr_advance()override {}
  void tr_restore()override {}
  void tr_regress()override {}
  void dc_final()override {}
  void tr_final()override {}
  void dc_advance()override {}
  void ac_begin()override {}
  void do_ac()override {}
  void ac_load()override {}
  void ac_final()override {}
  bool do_tr()override { return true;}
  bool tr_needs_eval()const override {untested(); return false;}
  void tr_queue_eval()override {}
  int  set_port_by_name(std::string& name, std::string& value) override;
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
int DEV_SUBCKT_PROTO::set_port_by_name(std::string& name, std::string& value)
{ untested();
  int index = net_nodes();
  assert(index == int(_port_name.size()));
  _port_name.push_back(name);
  set_port_by_index(index, value); // bumps _net_nodes
  return index;
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
  // assert(!new_instance->subckt());

  if (this == &pp){
    // cloning from static, empty model
    // look out for _parent in expand
    assert(!new_instance->_parent);
    assert(!new_instance->is_device());
    new_instance->_parent = this;
  }else{
    new_instance->_parent = this;
    //assert(common() == new_instance->common());

    auto c = prechecked_cast<COMMON_PARAMLIST const*>(common());
    for(auto p : c->_params){ untested();
      trace2("clone instance from spice", p.first, p.second.string());
    }

  }

 // assert(new_instance->is_device());
  return new_instance;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool DEV_SUBCKT::is_device() const
{
  if(!_parent) {
    return false;
  }else if(_parent==&pp){
    // spice proto
    return true;
  }else{
    return true;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::set_port_by_index(int Index, std::string& Value)
{
  grow_nodes(Index, _n, _node_capacity, node_capacity_floor);
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
    // grow_nodes(index, _n, _node_capacity, node_capacity_floor);
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
  if(_parent == &pp && 0) { untested();
    return subckt();
  }else if(is_device()){
    return COMPONENT::scope();
  }else{
    return subckt();
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_SUBCKT::is_valid() const
{ untested();
  PARAM_LIST const* params;
  if(_parent){ untested();
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
  Base const* x = v.e_val(nullptr, subckt());
  Integer c;
  Integer* res = c.assign(x);
  if(!res) { untested();
    return true;
  }else{ untested();
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
  }else{
    // verilog. build proto
    assert(!new_instance->is_device());
    if(new_instance->subckt()){ untested();
    }else{
     new_instance->new_subckt();
    }
  }

  return new_instance;
}
/*--------------------------------------------------------------------------*/
CARD* DEV_SUBCKT::clone_instance() const
{
  auto m = clone();
  auto s = prechecked_cast<DEV_SUBCKT*>(m);
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
  assert(_n == nullptr);
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p),
   _parent(p._parent)
{
  trace3("DEV_SUBCKT::DEV_SUBCKT", short_label(), net_nodes(), p.max_nodes());
  _node_capacity = p.max_nodes();
  if(_node_capacity){
    _n = new node_t[_node_capacity];
  }else{ untested();
    assert(_n == nullptr);
  }
  if(p.is_device()){
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      _n[ii] = p._n[ii];
    }
  }else{
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      assert(!_n[ii].n_());
    }
  }
  assert(!subckt());
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
    trace2("spice spbn", Name, Value);
    int i = BASE_SUBCKT::set_param_by_name(Name,Value);
    for(auto p : c->_params){
      trace2("spbn param spice", p.first, p.second.string());
    }
    return i;
  }else{
    trace2("normal spbn", Name, Value);
    for(auto p : *_parent->subckt()->params()){
      trace2("normal param", p.first, p.second.string());
    }
    PARAM_LIST::const_iterator p = _parent->subckt()->params()->find(Name);
    if(p != _parent->subckt()->params()->end()){
      return BASE_SUBCKT::set_param_by_name(Name,Value);
    }else{ untested();
      throw Exception_No_Match(Name);
    }
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
  }else if(_parent) { untested(); untested();
    unreachable();
    // reachable?
    return "";
  }else if(i<int(_port_name.size())) {
    if(_port_name[i]!=""){
      return _port_name[i];
    }else{ untested();
      return  port_value(i);
    }
  }else if(i<net_nodes()) {
    return port_value(i);
  }else{ untested();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::expand()
{
  BASE_SUBCKT::expand();
  trace3("DEV_SUBCKT::expand", long_label(), max_nodes(), is_device());

  if(!is_device()){
  }else if(_parent == &pp){
    COMMON_PARAMLIST const* c = prechecked_cast<COMMON_PARAMLIST const*>(common());
    assert(c);
    // first time spice
    assert(c->modelname()!="");
    const CARD* model = find_looking_out(c->modelname());
    if ((_parent = dynamic_cast<const DEV_SUBCKT_PROTO*>(model))) {
      // good
    }else if (dynamic_cast<const BASE_SUBCKT*>(model)) {
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt proto");
    }else{
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt");
    }
    assert(!_parent->is_device()); // really?
  }else if(_sim->is_first_expand()){
    trace1("DEV_SUBCKT::expand0", max_nodes());
    for(int i=0; i<max_nodes(); ++i) {
      if(n_(i).is_connected()){
      }else{
	n_(i).new_model_node("." + long_label() + "." + port_name(i), this);
	n_(i).n_()->set_label("");
      }
    }
  }else{ untested();
  }

  trace1("DEV_SUBCKT::expand1", max_nodes());


  if(!_parent || _parent==&pp){
    // not a device. probably a prototype
  }else{
    COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
    assert(c);
    assert(_parent);
    assert(_parent->subckt());
    assert(_parent->subckt()->params());
    PARAM_LIST* pl = const_cast<PARAM_LIST*>(_parent->subckt()->params());
    assert(pl);
    c->_params.set_try_again(pl);
    for(auto p : c->_params){
      trace2("expand param", p.first, p.second.string());
    }

    trace3("expand", short_label(), net_nodes(), max_nodes());
    renew_subckt(_parent, &(c->_params));
    subckt()->expand();

#if 1 // move to CARD_LIST::expand?
    for(CARD_LIST::iterator i=subckt()->begin(); i!=subckt()->end(); ++i){
      CARD* d = (*i)->deflate();

      if(d == (*i)){
      }else{ untested();
	assert(d->owner() == this);
	delete *i;
	*i = d;
      }
    }
#endif
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_first()
{
  BASE_SUBCKT::precalc_first();

  if (subckt()) {
  }else{
    new_subckt();
  }
  trace3("DEV_SUBCKT::precalc_first", long_label(), my_mfactor(), subckt()->size());

  COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
  assert(c);

  if(_parent == &pp && 0){ untested();
  }else if(_parent){
    PARAM_LIST* pl = const_cast<PARAM_LIST*>(_parent->subckt()->params());
    assert(pl);
    c->_params.set_try_again(pl);

    subckt()->attach_params(&(c->_params), scope());
    subckt()->precalc_first();
    assert(!is_constant()); /* because I have more work to do */
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
  if(_parent == &pp){ untested();
    CARD::precalc_last();
    // its a proto, bypass common clash hotfix
  }else if(is_device()){
    COMPONENT::precalc_last();
    COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
    assert(c);
    subckt()->attach_params(&(c->_params), scope());

    for(auto p : c->_params){
      trace3("pl", p.first, p.second, p.second.string());
    }

    subckt()->precalc_last();

    assert(!is_constant()); /* because I have more work to do */
  }else{
    CARD::precalc_last();
    // its a proto, bypass common clash hotfix
  }
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
// vim:ts=8:sw=2:noet:
