/*                 -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
 * user nodes
 */
#ifndef U_NODE_H
#define U_NODE_H
#include "e_node.h"
#include "e_logicnode.h"
/*--------------------------------------------------------------------------*/
// USER_NODE is permanent, and acts as a proxy for probes.
// .. n_(0) refers to the NODE used in simulation,
// has an index aka. user_number formerly node_t::_t, before mapping
// mapping to a node as a 1-net_node device does.
class USER_NODE : public NODE {
  mutable node_t _n;
  bool _global{false};
public:
  explicit USER_NODE(std::string const& s, int i=INVALID_NODE)
    : NODE(s), _n(i) {
    assert(_n.e_() == i);
  }
public:
  int user_number()const override {return _n.e_();}
  void set_to_ground() { _global=true; _n.set_to_ground(nullptr); }
  bool is_global()const {return _global;}
private: // probe proxy
  double	tr_probe_num(const std::string&)const override;
  XPROBE	ac_probe_ext(const std::string&)const override;
public: // connection
  int net_nodes()const override {return 1;}
  node_t& n_(int i)const override {
    (void)i;
    assert(i==0);
    return _n;
  }
  void map_nodes()override {
    _n.map();
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif // guard
// vim:ts=8:sw=2:noet:
