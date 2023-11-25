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
/* default constructor : unconnected, don't use
 */
NODE::NODE()
  :CKT_BASE(),
   _user_number(INVALID_NODE)
   //_flat_number(INVALID_NODE)
   //_matrix_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
/* copy constructor : user data only
 */
NODE::NODE(const NODE& p)
  :CKT_BASE(p),
   _user_number(p._user_number)
   //_flat_number(p._flat_number)
   //_matrix_number(INVALID_NODE)
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
/* constructor taking a pointer : it must be valid
 * supposedly not used, but used by a required function that is also not used
 */
NODE::NODE(const NODE* p)
  :CKT_BASE(*p),
   _user_number(p->_user_number)
   //_flat_number(p->_flat_number)
   //_matrix_number(INVALID_NODE)
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
/* usual initializing constructor : name and index
 */
NODE::NODE(const std::string& s, int n)
  :CKT_BASE(s),
   _user_number(n)
   //_flat_number(n)
   //_matrix_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
node_t::node_t()
  :_nnn(0),
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
  _ttt = p._ttt;
  _m   = p._m;
  return *this;
}
/*--------------------------------------------------------------------------*/
LOGIC_NODE& node_t::data()const
{
  assert(CKT_BASE::_sim->_nstat);
  return CKT_BASE::_sim->_nstat[m_()];
}
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    // return v0(); denoised
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (Umatch(x, "z ")) {
    return port_impedance(node_t(const_cast<NODE*>(this)), node_t(&ground_node), _sim->_lu, 0.);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) {
    assert(_sim->_nstat);
    return _sim->_nstat[matrix_number()].tr_probe_num(x);
  }else if (Umatch(x, "mdy ")) {
    // matrix diagonal admittance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return aaa.d(m_(),m_());
  }else if (Umatch(x, "mdz ")) {
    // matrix diagonal impedance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return 1/aaa.d(m_(),m_());
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
  }else{untested();
    return CKT_BASE::tr_probe_num(x);
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
    return CKT_BASE::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
void node_t::set_to_ground(CARD* d)
{
  //assert(!_nnn); //BUG// fails on MUTUAL_L::expand after clone
  assert(d);

  NODE_MAP* Map = d->scope()->nodes();
  assert(Map);
  _nnn = (*Map)["0"];
  _ttt = 0;
  assert(_nnn);
}
/*--------------------------------------------------------------------------*/
/* new_node: a raw new node, as when a netlist is parsed
 */
void node_t::new_node(const std::string& node_name, const CARD* d)
{
  //assert(!_nnn); //BUG// fails on MUTUAL_L::expand after clone
  assert(d);

  NODE_MAP* Map = d->scope()->nodes();
  assert(Map);
  _nnn = Map->new_node(node_name);
  _ttt = _nnn->user_number();
  assert(_nnn);
}
/*--------------------------------------------------------------------------*/
/* new_model_node: a mapped new node, produced through model expansion.
 * Not really a model_node, but a node in the subckt that is made
 * in model expansion.
 * Supposedly equivalent to new_node() then map_subckt_node()
 * but it does it without building a map
 */
void node_t::new_model_node(const std::string& node_name, CARD* d)
{
  new_node(node_name, d);
  _ttt = CKT_BASE::_sim->newnode_model();
  //assert(_ttt == _nnn->flat_number());
}
/*--------------------------------------------------------------------------*/
void node_t::map_subckt_node(int* m, const CARD* d)
{
  assert(m);
  if (e_() != INVALID_NODE) {
    if (node_is_valid(m[e_()])) {
      _ttt = m[e_()];
    }else{
      throw Exception(d->long_label() + ": need more nodes");
    }
  }else{untested();
    throw Exception(d->long_label() + ": invalid nodes");
  }
  //_nnn->set_flat_number(_ttt);
  assert(node_is_valid(_ttt));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
