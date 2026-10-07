/*$Id: e_logicnode.h $ -*- C++ -*-
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
#ifndef E_MATRIXNODE_H
#define E_MATRIXNODE_H
#include "e_logicval.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
class INTERFACE MATRIX_NODE : public NODE {
private:
  int _flat_number{INVALID_NODE};
private: // inhibited
  explicit MATRIX_NODE(const MATRIX_NODE&):NODE(){incomplete();unreachable();}
public: // general use
  explicit MATRIX_NODE(std::string const& s) : NODE(s) {untested();}
  explicit MATRIX_NODE() {}
	   ~MATRIX_NODE() {}

private:
  // int 	      _a_iter;		/* iteration of last update - analog */

public:
  int user_number()const override { return INVALID_NODE;}
  int flat_number()const override { return _flat_number;}
  int matrix_number()const override;
  NODE& set_flat_number(int n)override {_flat_number = n; return *this;}
  // NODE& set_user_number(int n)override {_user_number = n; return *this;}

  // so it is not pure virtual
  //const	      std::string long_label()const;
public: // virtuals
  //double	tr_probe_num(const std::string&)const override;
  //XPROBE	ac_probe_ext(const std::string&)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
