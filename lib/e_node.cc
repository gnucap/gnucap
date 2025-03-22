/*$Id: e_node.cc $ -*- C++ -*-
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
 * node probes
 */
//testing=script,sparse 2006.07.11
#include "e_cardlist.h"
#include "e_card.h"
#include "u_nodemap.h"
#include "e_aux.h"
#include "u_xprobe.h"
#include "e_logicnode.h"
#include "m_union.h"
/*--------------------------------------------------------------------------*/
/* constructor taking a pointer : it must be valid
 * supposedly not used, but used by a required function that is also not used
 */
NODE::NODE(const NODE* p)
  :CARD(*p)
{ untested();
  unreachable();
}
/*--------------------------------------------------------------------------*/
node_t::node_t(const node_t& p)
  :_nnn(p._nnn),
   _link(p._link),
   _index(p._index),
   _m(p._m)
{
}
/*--------------------------------------------------------------------------*/
node_t::node_t(node_t&& p)
  :_nnn(p._nnn),
   _link(p._link),
   _index(p._index),
   _m(p._m)
{
  if(p._link == &p) { untested();
    _link = this;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
#ifndef NDEBUG
extern NODE ground_node;
#endif
node_t::node_t(NODE* n)
  :_nnn(n),
   _index(n->user_number()),
   _m(n->matrix_number())
{
  assert(n!=&ground_node || _m==0);
}
/*--------------------------------------------------------------------------*/
node_t& node_t::operator=(const node_t& p)
{
  if(_own){
    delete _nnn;
  }else{
  }
  _nnn = nullptr;
  _link = p._link;
  _index = p._index;// wrong scope ??
  _m   = p._m;
  _own = false;
  return *this;
}
/*--------------------------------------------------------------------------*/
node_t& node_t::operator=(node_t&& p)
{
  _nnn   = nullptr; // p._nnn;
  _link   = p._link;
  _index = p._index;
  _m   = p._m;
  _own = false;

  if(p._link == &p) { untested();
    _link = this;
  }else{
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
// ordinary pointer assignment
node_t& node_t::operator=(NODE* n)
{
  assert(!_link || _link == this);
  // clear();
  if(!_nnn){
    _own = false;
  }else if(_own){ untested();
    _nnn->purge();
    delete _nnn;
    _own = false;
  }else{
  }
  _link = this;
  _nnn = n;

  _index = n->user_number();
  return *this;
}
/*--------------------------------------------------------------------------*/
// take ownership
node_t& node_t::set_own(NODE* n)
{
  assert(n != &ground_node);
  operator=(n);
  _own = true; // take ownership.
  return *this;
}
/*--------------------------------------------------------------------------*/
extern NODE ground_node;
LOGIC_NODE& node_t::data()const
{
  if(auto d = dynamic_cast<LOGIC_NODE*>(_nnn)){
    return *d;
  }else if(auto e = dynamic_cast<LOGIC_NODE*>(root()._nnn)){ untested();
    return *e;
  }else if(_index==0 || _nnn == &ground_node){
    // BUG. ground is not a logic node, but asking for one.
    //  d_cccs.2.ckt
    static LOGIC_NODE logic_ground(0);
    return logic_ground;
  }else{ untested();
    unreachable();
    static LOGIC_NODE logic_ground(0);
    return logic_ground;
  }
}
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    // return v0(); denoised
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (Umatch(x, "z ")) {
    return port_impedance(node_t(const_cast<NODE*>(this)), node_t(&ground_node), _sim->_aa, 0.);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) { untested();
    unreachable();
    return NOT_VALID;
  }else if (Umatch(x, "mdy ")) {
    // matrix diagonal admittance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return aaa.d(m_());
  }else if (Umatch(x, "mdz ")) {
    // matrix diagonal impedance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return 1/aaa.d(m_());
  }else if (Umatch(x, "zero ")) {
    // fake probe: 0.0
    return 0.0;
  }else if (Umatch(x, "pdz ")) {
    // fake probe 1/0 .. positive divide by zero = Infinity
    double z1 = tr_probe_num("zero ");
    return 1.0/z1;
  }else if (Umatch(x, "ndz ")) {
    // fake probe -1/0 .. negative divide by zero = -Infinity
    double z1 = tr_probe_num("zero ");
    return -1.0/z1;
  }else if (Umatch(x, "nan ")) {
    // fake probe 0/0 = NaN
    double z1 = tr_probe_num("zero ");
    double z2 = tr_probe_num("zero ");
    return z1/z2;
  }else{itested();
    return CARD::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
XPROBE NODE::ac_probe_ext(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    return XPROBE(vac());
  }else if (Umatch(x, "z ")) {
    return XPROBE(port_impedance(node_t(const_cast<NODE*>(this)),
				 node_t(&ground_node), _sim->_acx, COMPLEX(0.)));
  }else{untested();
    return CARD::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
/* new_node: a raw new node, as when a netlist is parsed
 * It's only "new" if this is the first use in this scope.
 * If it is not the first use of this node, it makes a connection.
 */
void node_t::new_node(const std::string& node_name, const CARD* Owner)
{
  if (_nnn) {//206
    // Repeat assign to this port, must be by name.  Probably an error.
    // Just clobber it.  Might be a leak but probably isn't.
  }else{//33312
    // proper first assign to this port.  The usual case.
  }
  CARD_LIST const* scope; // the CARD_LIST that owns this device.
  if(Owner) {
    scope = Owner->scope();
  }else{
    scope = &CARD_LIST::card_list;
  }
  assert(scope);
  NODE_MAP* Map = scope->nodes();
  assert(Map);

  NODE* n = Map->new_node(node_name); // not neessarily "new"
  _index = n->user_number();
  assert(_index!=INVALID_NODE);

  assert(is_connected()); // for now.
  n->set_owner(nullptr); // Owner?
}
/*--------------------------------------------------------------------------*/
// free memory. NODES are owned by roots in the union forest, but path
// contraction will make the others point directly to root()->_nnn
// only delete once.
node_t::~node_t()
{
  // clear();
  if(!_nnn){
    _own = false;
  }else if(_own){
    _nnn->purge();
    delete _nnn;
    _nnn = nullptr;
    _own = false;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* new_model_node: a mapped new node, produced through model expansion.
 * Not really a model_node, but a node in the subckt that is made
 * in model expansion.
 * Supposedly equivalent to new_node() then map_subckt_node()
 * but it does it without building a map
 */
void node_t::new_model_node(const std::string& /*node_name*/, CARD* Owner)
{
  if(_nnn){ untested();
    //it's already there.
  }else{
    // BUG: only request node, and allocate post-expand in appropriate order.
    int idx = CARD::_sim->newnode_model();
    auto ln = new LOGIC_NODE(node_name); // TODO: use requested type
    ln->set_owner(Owner);
    ln->set_flat_number(idx);
    ln->set_owner(Owner);
    set_own(ln);
  }
}
/*--------------------------------------------------------------------------*/
/* (re)connect a port to an external node making use of index.
 * m: external nodes are in m. usually m == d->scope->nodes.
 * TODO: pass map, and range check..?
 */
void node_t::map_subckt_node(node_t* m, const CARD* d)
{
  assert(m);
  if (e_() != INVALID_NODE) {
    clear(); // keep index.
    build_union(this, &m[e_()]);
    assert(_link);
    assert(!_nnn);
    if(!_nnn){
      _own = false;
    }else if(_own){ untested();
      delete _nnn;
      _own = false;
    }else{ untested();
    }
    _nnn = nullptr;
  }else{
    throw Exception(d->long_label() + ": invalid nodes");
  }
}
/*--------------------------------------------------------------------------*/
void node_t::allocate(int u)
{
  if(is_node()) { untested();
    // done.
    trace3("node_t::allocate is_node", this, &root(), _nnn->short_label());
    assert(_link);
  }else if(_link==this) {
    int flat_number;
    if(u){
      flat_number = CKT_BASE::_sim->newnode_user();
    }else{
      flat_number = CKT_BASE::_sim->newnode_subckt();
    }
    trace3("node_t::allocate new", this, &root(), flat_number);
    NODE* nn = new LOGIC_NODE(flat_number);
    nn->set_owner(nullptr);
    set_own(nn);
  }else{
    trace2("node_t::allocate no allocate", _index, u);
  }
}
/*--------------------------------------------------------------------------*/
void node_t::set_to_ground(CARD* Owner)
{
  assert(!_link || _link == this);
  int idx = _index;
  clear();
  assert(!_nnn);
  new_node("0", Owner);
  if(Owner){
    assert(Owner->scope());
    assert(Owner->scope()->nodes());
    NODE_MAP& nodes = *Owner->scope()->nodes();
    _link = &nodes["0"]->n_(0);
    _index = nodes["0"]->user_number();
  }else{
    NODE_MAP& nodes = *CARD_LIST::card_list.nodes();
    _link = &nodes["0"]->n_(0);
    // must retain index. connection is in _link...
    _index = idx;
  }
}
/*--------------------------------------------------------------------------*/
bool node_t::is_grounded() const
{
  if(_nnn==&ground_node){ untested();
    return true;
  }else if(&root()!=this){ untested();
    return root().is_grounded();
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
void node_t::clear()
{
  if(!_nnn){
  }else if(_own){ untested();
    _nnn->purge();
    delete _nnn;
  }else{
  }
  _own = false;
  _nnn = nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
