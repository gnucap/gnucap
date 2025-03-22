/*$Id: e_node.h $ -*- C++ -*-
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
 * circuit node class
 */
//testing=script,sparse 2006.07.11
#ifndef E_NODE_H
#define E_NODE_H
#include "u_sim_data.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
enum {
  OUT1 = 0,
  OUT2 = 1,
  IN1 = 2,
  IN2 = 3,
  NODES_PER_BRANCH = 4,
  INVALID_NODE = -1
};
/*--------------------------------------------------------------------------*/
// base class for various NODEs
// avoiding stuff that is not needed everywere.
// shield from accidental use
class NODE : public CARD {
protected:
  explicit NODE() : CARD() {}
private: // inhibited
  explicit NODE(const NODE& p) : CARD(p) {unreachable();}
public:
  explicit NODE(const NODE* p); // u_nodemap.cc:49 (deep copy)
  explicit NODE(const std::string& s, int idx=0) : CARD(s) {assert(!idx);}
  ~NODE() {}

  CARD* clone()const override	{untested(); return new NODE(*this);}

public: // raw data access (rvalues)
  virtual int user_number()const	{/*ground only*/return 0;}
  //int	flat_number()const	{itested();return _flat_number;}
public: // simple calculated data access (rvalues)
  int	matrix_number()const	{return _sim->_nm[user_number()];} // TODO: -> MATRIX_NODE
  int	m_()const		{return matrix_number();}
public: // virtuals
  double	tr_probe_num(const std::string&)const override;
  XPROBE	ac_probe_ext(const std::string&)const override;

  double      v0()const	{
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_v0[m_()];
  }
  double      vt1()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_vt1[m_()];
  }
  COMPLEX     vac()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_ac[m_()];
  }
  //double      vdc()const		{untested();return _vdc[m_()];}

  //double&     i()	{untested();return _i[m_()];}  /* lvalues */
  COMPLEX&    iac() {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_ac[m_()];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class INTERFACE node_t {
private: // this should eventually fit into 64 bits.
  NODE* _nnn{nullptr};
  mutable node_t* _link{nullptr};
  bool _own{false}; // indicate that _nnn is ours.
private: // treee stuff.
  bool is_node()const;
  bool is_link()const;
  bool is_root()const;
  node_t&       root();
  node_t const& root()const;
  node_t*       link() {return _link;}
  node_t const* link()const {return _link;}
private:
  int _index{INVALID_NODE}; // index in node map
  int _ttt;		// m == nm[t] if properly set up // obsolete.
  int _m;		// mapped, after reordering

private:
  static bool node_is_valid(int i) {
    if (i == INVALID_NODE) {
    }else if (i < 0) { untested();
      unreachable();
    }else if (i > NODE::_sim->_total_nodes) { untested();
      unreachable();
    }else{
    }
    return i>=0 && i<=NODE::_sim->_total_nodes;
  }
  static int  to_internal(int n) {
    assert(node_is_valid(n));
    assert(NODE::_sim->_nm);
    return NODE::_sim->_nm[n];
  }

public:
  int	      m_()const	{return _m;}

  int	      t_()const {
    if(!_nnn){
    }else if(_ttt == 0){
      // has been set to ground? ddid not update _nnn?
      incomplete();
    }else if(_ttt == _nnn->user_number()) {
    }else{
      incomplete();
    }
    return _ttt;
  }	// e_cardlist.cc:CARD_LIST::map_subckt_nodes:436 and
	// e_node.h:node_t::map:263,265 only
  bool is_valid() const {
    return node_is_valid(_ttt); // BUG: _ttt is obsolete.
  }

  int	      e_()const {
    if(_nnn) {
      // assert(_index == _nnn->user_number());
      return _nnn->user_number();
    }else{
      assert(_index==INVALID_NODE);
    }
    return _index;
  }
  const NODE* n_()const {return _nnn;}
  NODE*	      n_()	{return _nnn;}

  
  const std::string  short_label()const {return ((n_()) ? (n_()->short_label()) : "?????");}
  void	set_to_ground(CARD* Owner)	{new_node("0", Owner);}
  void	new_node(const std::string&, const CARD*);
  void	new_model_node(const std::string& n, CARD* d);
  void	map_subckt_node(node_t* map_array, const CARD* d);
  bool	is_grounded()const {return (e_() == 0);}
  bool	is_connected()const {return (e_() != INVALID_NODE);}
  bool	is_short_to(node_t const& n)const {return n_() == n.n_();} // BUG. doesn't work

  node_t&     map() {
    if (t_() != INVALID_NODE) {
      assert(_nnn);

      _m = to_internal(t_()); // BUG. ask _nnn
    }else{
      assert(_m == INVALID_NODE);
    }
    return *this;
  } // e_compon.cc:COMPONENT::map_nodes:522

  explicit    node_t();
	      node_t(const node_t&);
  explicit    node_t(int i) : _ttt(i) {}
  explicit    node_t(NODE*);
	      ~node_t();

private: // raw data access (lvalues)
  LOGIC_NODE&	data()const;

public:
  //LOGIC_NODE&	    operator*()const	{untested();return data();}
  const LOGIC_NODE* operator->()const	{return &data();}
  LOGIC_NODE*	    operator->()	{return &data();}
  operator bool()const {return _nnn;}

  node_t& operator=(const node_t& p);
  node_t& operator=(NODE* p);
  node_t& set_own(NODE* p);

  bool operator==(const node_t& p) {return _nnn==p._nnn && _ttt==p._ttt && _m==p._m;}

public:
  double      v0()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    assert(n_());
    //assert(n_()->m_() == m_());
    //assert(n_()->v0() == NODE::_sim->_v0[m_()]);
    return NODE::_sim->_v0[m_()];
  }
  
  COMPLEX     vac()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    assert(n_());
    //assert(n_()->m_() == m_());
    //assert(n_()->vac() == NODE::_ac[m_()]);
    return NODE::_sim->_ac[m_()];
  }
  
  double&     i() {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    return NODE::_sim->_i[m_()];
  }
#if 0
  COMPLEX&    iac() {untested();
    assert(n_());
    assert(n_()->m_() == m_());
    assert(n_()->iac() == NODE::_ac[m_()]);
    //return n_()->iac();
    return NODE::_sim->_ac[m_()];
  }
#endif
};
/*--------------------------------------------------------------------------*/
inline bool node_t::is_node() const
{ untested();
  assert(!_nnn || _link == this);
  return _nnn;
}
/*--------------------------------------------------------------------------*/
inline bool node_t::is_link() const
{ untested();
  assert(!_nnn || _link == this);
  return !_nnn && _link;
}
/*--------------------------------------------------------------------------*/
inline bool node_t::is_root() const
{ untested();
  if(is_node()){ untested();
    return true;
  }else if(is_link()) { untested();
    return link() == this;
  }else{ untested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
inline node_t& node_t::root()
{ untested();
  node_t* r = this;
  while (r->is_link() && r->link() != r) { untested();
    r = r->link();
  }
  return *r;
}
/*--------------------------------------------------------------------------*/
inline node_t const& node_t::root() const
{
  node_t const* r = this;
  while (r->is_link() && r->link() != r) {
    r = r->link();
  }
  return *r;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
