/*$Id: e_node.h $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2025 Felix Salfelder
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
  explicit NODE(const NODE& p) : CARD(p) { untested();unreachable();}
public:
  explicit NODE(const NODE* p); // u_nodemap.cc:49 (deep copy)
  explicit NODE(const std::string& s, int idx=0)
    : CARD(s) {(void)idx; assert(!idx);}
  ~NODE();

  CARD* clone()const override	{untested(); return new NODE(*this);}

public: // raw data access (rvalues)
  virtual int user_number()const;
  virtual int flat_number()const {return INVALID_NODE;}
public: // simple calculated data access (rvalues)
  virtual int matrix_number()const;
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
#ifndef NDEBUG
extern NODE ground_node;
#endif
/*--------------------------------------------------------------------------*/
inline int NODE::user_number() const
{
  // assert(this==&ground_node);
  return 0;
}
/*--------------------------------------------------------------------------*/
inline int NODE::matrix_number() const
{
  // assert(this==&ground_node);
  return 0;
}
/*--------------------------------------------------------------------------*/
class INTERFACE node_t {
private: // this should eventually fit into 128 bits.
  NODE* _nnn{nullptr};
  mutable node_t* _link{nullptr};
  int _index{INVALID_NODE}; // index in node map
  int _m{INVALID_NODE};	// mapped, after reordering
  bool _own{false}; // indicate that _nnn is ours.
  enum dir_t {
    dir_none = 0,
    dir_in   = 1,
    dir_out  = 2,
    dir_io   = 3
  } _dir{dir_none};
private: // treee stuff.
  bool is_node()const;
  bool is_link()const;
  bool is_root()const;
private: // debugging. not yet
  node_t&       root();
  node_t const& root()const {
    return const_cast<node_t*>(this)->root();
  }
private:
  node_t*       link() { assert(!_nnn || !_link); return _link; }
  node_t const* link()const { untested();return _link;}
private: // union find
  int rank()const {return !!_nnn;} // TODO: hierarchy.
  int inc_rank()const {return 0;} // TODO
  friend node_t* root(node_t const*);
  friend int     rank(node_t const*);
  friend int     inc_rank(node_t*);
  friend bool    has_parent(node_t*n) {return parent(n);}
  friend node_t* parent(node_t*);
  friend node_t* set_parent(node_t* n, node_t* p);
public:
  void set_input(){ _dir = dir_t(_dir | dir_in); }
  void set_output(){ _dir = dir_t(_dir | dir_out); }
  bool is_port()const {untested(); return _dir == dir_none; }
  bool is_input()const {return _dir & dir_in; }
  bool is_output()const {return _dir & dir_out; }
  bool is_inout()const {return _dir == dir_none || _dir == dir_io; }
  void allocate(int u=0);
public: // BUG
  void clear();
private:
  static bool node_is_valid(int i) {
    if (i == INVALID_NODE) { untested();
    }else if (i < 0) { untested();
      unreachable();
    }else if (i > NODE::_sim->_total_nodes) { untested();
      unreachable();
    }else{
    }
    return i>=0 && i<=NODE::_sim->_total_nodes;
  }
  static int  to_internal(int n) {
    if(n == 0){ untested();
      return 0;
    }else if(NODE::_sim->_nm){
      assert(node_is_valid(n));
      return NODE::_sim->_nm[n];
    }else{ untested();
      // possibly building map. no need for this
      // (remove later)
      return INVALID_NODE;
    }
  }

public:
  int	      m_()const	{return _m;}

  bool is_valid() const { untested();
    return _link || _nnn;
  }

  int	      e_()const {return _index;}
  NODE const* n_()const {return _nnn;}
  NODE*       n_()      {return _nnn;}

  const std::string  short_label()const { itested();
    if (n_()){ itested();
      return n_()->short_label();
    }else if(link()) { untested();
      return link()->short_label();
    }else{ untested();
      return "?????";
    }
  }
  void	set_to_ground(CARD* Owner);
  void	new_node(const std::string&, const CARD*);
  void	new_model_node(const std::string& n, CARD* d);
  void	map_subckt_node(node_t* map_array, const CARD* d);
  bool	is_grounded()const;
  bool	is_connected()const { return _nnn || _link || e_()!=INVALID_NODE; }
  bool	is_short_to(node_t const& n)const {return &root() == &n.root();}

  node_t&     map() {
    if (_nnn) {
    }else if (_link) {
      _own = false;
      _nnn = root()._nnn;
    }else if(_m==0){itested();
    }else{
      assert(_m == INVALID_NODE);
    }
    if (_nnn) {
      _m = _nnn->matrix_number();
      _link = nullptr;
    }else{
    }
    return *this;
  } // e_compon.cc:COMPONENT::map_nodes:522

  explicit    node_t(int i=INVALID_NODE) : _index(i) {};
	      node_t(const node_t&);
	      node_t(node_t&);
	      node_t(node_t&&);
  explicit    node_t(NODE*);
	      ~node_t();

private: // raw data access (lvalues)
  NODE&	data()const;

public:
  //LOGIC_NODE&	    operator*()const	{untested();return data();}
  const NODE* operator->()const	{return &data();}
  NODE*       operator->()	{return &data();}
  operator bool()const {return _nnn;}

  node_t& operator=(node_t& p);
  node_t& operator=(const node_t& p);
  node_t& operator=(node_t&& p);
  node_t& operator=(NODE* p);

  bool operator==(const node_t& p)const { return _link==p._link && _nnn==p._nnn && _m==p._m;}

private:
public: // top level kludge. u_sim_data.cc line 457
        // & used in set_parent.
  node_t& link_to(node_t* nn){
    if(nn != this){
      assert(nn);
      assert(nn==&nn->root());
      if(_own){ untested();
	delete _nnn;
      }else if(_nnn){ untested();
      }else{
      }
      _nnn = nullptr;
      if(!_link){
      }else{
      }
      _link = nn;
    }else if(!_nnn){
      _link = this;
    }else{
    }
    return *this;
  }
private:
  node_t& set_own(NODE* p);
  void connect(node_t&);

public:
  double      v0()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    assert(!m_() || n_() || root().n_());
    assert(!m_() || n_()->m_() == m_());
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
{
  assert(!_nnn || !_link);
  return _nnn; //  || _link == this;
}
/*--------------------------------------------------------------------------*/
inline bool node_t::is_link() const
{
  assert(!_nnn || !_link);
  return !_nnn && _link && _link != this;
}
/*--------------------------------------------------------------------------*/
inline bool node_t::is_root() const
{ untested();
  assert(!_nnn || !_link);
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
{
  assert(!_nnn || !_link);
 // call find_subset?
  node_t* r = this;
  while (r->is_link() && r->link() != r) {
    r = r->link();
  }
  return *r;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* set parent of n to p, return new parent
 */
inline node_t* set_parent(node_t* n, node_t* p)
{
  assert(n);
  assert(p);
  n->link_to(p);
  return p;
}
/*--------------------------------------------------------------------------*/
inline node_t* parent(node_t* n)
{
  assert(n);
  return n->link();
}
/*--------------------------------------------------------------------------*/
inline int rank(node_t const* n)
{
  assert(n);
  return n->rank();
}
/*--------------------------------------------------------------------------*/
inline int inc_rank(node_t* n)
{
  assert(n);
  return n->inc_rank();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
