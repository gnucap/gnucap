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
node_t::node_t()
  :_nnn(nullptr),
   _ttt(INVALID_NODE),
   _m(INVALID_NODE)
{
}
node_t::node_t(const node_t& p)
  :_nnn(p._nnn),
   _ttt(p._ttt),
   _m(p._m)
{
  //assert(_ttt == _nnn->flat_number());
}
node_t::node_t(NODE* n)
  :_nnn(n),
   _ttt(n->user_number()),
   _m(to_internal(n->user_number()))
{
  //assert(_ttt == _nnn->flat_number());
}
node_t& node_t::operator=(const node_t& p)
{
  if (p._nnn) {
    //assert(p._ttt == p._nnn->flat_number());
  }else{
    assert(p._ttt == INVALID_NODE);
    assert(p._m   == INVALID_NODE);
  }
  _nnn   = p._nnn;
  _ttt = p._ttt; // BUG.
  _index = p._index;
  _m   = p._m;
  _own = false;
  return *this;
}
/*--------------------------------------------------------------------------*/
// ordinary pointer assignment
node_t& node_t::operator=(NODE* n)
{
  if(_own){ untested();
    delete _nnn;
    _own = false;
  }else{
  }
  _nnn = n;

  if(n==&ground_node){
    // BUG. transition.
    _ttt = 0;
    _index = 0;
  }else{
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
// take ownership
node_t& node_t::set_own(NODE* n)
{
  operator=(n);
  _own = true; // take ownership.
  _ttt = n->user_number(); // BUG: transition.
  _index = n->user_number();
  return *this;
}
/*--------------------------------------------------------------------------*/
LOGIC_NODE& node_t::data()const
{
  assert(CARD::_sim->_nstat);
  return CARD::_sim->_nstat[m_()];
}
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    // return v0(); denoised
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (Umatch(x, "z ")) {
    return port_impedance(node_t(const_cast<NODE*>(this)), node_t(&ground_node), _sim->_aa, 0.);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) {
    assert(_sim->_nstat);
    return _sim->_nstat[matrix_number()].tr_probe_num(x);
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
  assert(Owner); // the device that owns this port.
  assert(Owner->scope()); // the CARD_LIST that owns this device.
  NODE_MAP* Map = Owner->scope()->nodes();
  assert(Map);

  _nnn = Map->new_node(node_name); // not neessarily "new"
				   // temporary. will _link instead.
  // assert(is_connected()); // for now.
  assert(_nnn);
  _index = _nnn->user_number();
  _ttt = _index; // _ttt is obsolete.

  if(_ttt==0){
    assert(_nnn==&ground_node);
  }else{
  }
  _nnn->set_owner(nullptr); // Owner?
}
/*--------------------------------------------------------------------------*/
// free memory. NODES are owned by roots in the union forest, but path
// contraction will make the others point directly to root()->_nnn
// only delete once.
node_t::~node_t()
{
  if(_own){
    _nnn->purge();
    delete _nnn;
    _nnn = nullptr;
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
void node_t::new_model_node(const std::string& node_name, CARD* Owner)
{
  new_node(node_name, Owner);
  int index = CARD::_sim->newnode_model();
  auto ln = new LOGIC_NODE(); // TODO: use requested type
  ln->set_user_number(index);

  _nnn = ln;
  _own = true; // garbage collect.
  _index = _ttt = index; // BUG. why?
}
/*--------------------------------------------------------------------------*/
void node_t::map_subckt_node(node_t* m, const CARD* d)
{
  assert(m);
  if (e_() != INVALID_NODE) {
    if (m[e_()].is_valid()) {
      _ttt = m[e_()]._ttt; // BUG. don't use _ttt
      _link = &m[e_()];
      if(_ttt==0){
	if(_own){ untested();
	  delete _nnn;
	}else{
	}
	_nnn = &ground_node;
      }else{
      }
    }else{
      throw Exception(d->long_label() + ": need more nodes");
    }
  }else{
    throw Exception(d->long_label() + ": invalid nodes");
  }
  //_nnn->set_flat_number(_ttt);
  assert(node_is_valid(_ttt));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
