/*                -*- C++ -*-
 * Copyright (C) 2026 Felix Salfelder
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
 * discipline and nature
 */
#include "e_node_type.h"
#include "globals.h"
#include "e_logicnode.h"
/*--------------------------------------------------------------------------*/
NODE_TYPE::NODE_TYPE(std::string const& name)
  : NODE(name)
{
  set_label(name);
  static int k;
  _type_number = k++;
}
/*--------------------------------------------------------------------------*/
class ELECTRICAL : public NODE_TYPE {
  ELECTRICAL(ELECTRICAL const& p) : NODE_TYPE(p) {untested();}
public:
  explicit ELECTRICAL() : NODE_TYPE("electrical") {
    // set_continuous();
    // set_potential("Voltage");
    // set_flow("Current");
  }
  CARD* clone()const override {untested(); return new ELECTRICAL(*this);}
  NODE* deflate()override { untested();
    return new LOGIC_NODE(); // TODO
  }
}electrical;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
