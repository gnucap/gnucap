/*                -*- C++ -*-
 * Copyright (C) 2025-26 Felix Salfelder
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
 * node type, aka "discipline"
 */
#ifndef E_NODE_TYPE
#define E_NODE_TYPE
#include "globals.h"
#include "e_base.h"
#include "e_node.h"
#include "u_nodemap.h"
#include "e_usernode.h"
#include "d_dot.h"
/*--------------------------------------------------------------------------*/
class NATURE : public CKT_BASE {
  std::list<std::pair<std::string, std::string> > _attribs;
  double abstol{0.};
};
/*--------------------------------------------------------------------------*/
class NODE_TYPE : public NODE {
  typedef DISPATCHER<NODE>::INSTALL inst;
  int _type_number{INVALID_NODE};
  inst* _installer{nullptr};
  smode_t _domain;
public:
  explicit NODE_TYPE(std::string const&);
  explicit NODE_TYPE() : NODE() { }
protected:
  explicit NODE_TYPE(const NODE_TYPE& p)
    : NODE(p), _type_number(p._type_number), _domain(p._domain) { }
public:
  ~NODE_TYPE();
  CARD* clone_instance()const override { untested();
    return nullptr;
    // return new NODE_DECL(short_label());
  }
protected:
  void install();
public:
  int param_count()const override { return 3; }
  using NODE::param_name;
  std::string param_name(int i)const override {
    switch(i){
    case 0: return "domain";
    case 1: return "potential";
    case 2: return "flow";
    default: unreachable(); return "???";
    }
  }
  smode_t domain()const {return _domain;}
protected:
  int user_number()const override   {return INVALID_NODE;}
  int flat_number()const override   {return INVALID_NODE;}
  int matrix_number()const override {return INVALID_NODE;}
public:
  int type_number()const override   { return _type_number;}
  void set_type_number(int t)       {_type_number = t;}
  // void set_flat_number()const override {unreachable();}
protected:
  void unset_domain(){_domain = moUNKNOWN;}
  void set_analog()  {_domain = moANALOG;}
  void set_digital() {_domain = moDIGITAL;}
  void set_mixed()   {_domain = moMIXED;}
private:
  double tr_probe_num(const std::string&)const override {return NOT_VALID;}
  XPROBE ac_probe_ext(const std::string&)const override;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
