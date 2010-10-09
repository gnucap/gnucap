/*$Id: e_ccsrc.cc,v 26.124 2009/09/28 22:59:33 al Exp $ -*- C++ -*-
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
 * current controlled source base
 */
//testing=script,complete 2006.07.12
#include "e_ccsrc.h"
/*--------------------------------------------------------------------------*/
void CCSRC_BASE::expand_last()
{
  ELEMENT::expand_last();

  if (_input_label != "") {
    _input = dynamic_cast<const ELEMENT*>(find_in_my_scope(_input_label));
  }else{untested();
    // _input already set, an internal element.  example: mutual L.
  }

  if (!_input) {untested();
    throw Exception(long_label() + ": " + _input_label + " cannot be used as current probe");
  }else if (_input->subckt()) {untested();
    throw Exception(long_label() + ": " + _input_label
		    + " has a subckt, cannot be used as current probe");
  }else if (_input->has_inode()) {untested();
    _n[IN1] = _input->n_(IN1);
    _n[IN2].set_to_ground(this);
  }else if (_input->has_iv_probe()) {
    _n[IN1] = _input->n_(OUT1);
    _n[IN2] = _input->n_(OUT2);
  }else{
    throw Exception(long_label() + ": " + _input_label + " cannot be used as current probe");
  }
}
/*--------------------------------------------------------------------------*/
void CCSRC_BASE::set_port_by_index(int num, std::string& Value)
{
  if (num == 2) {
    _input_label = Value;
  }else{
    ELEMENT::set_port_by_index(num, Value);
  }
}
/*--------------------------------------------------------------------------*/
bool CCSRC_BASE::node_is_connected(int i)const
{
  if (i == 2) {
    return _input_label != "";
  }else{
    return ELEMENT::node_is_connected(i);
  }
}
/*--------------------------------------------------------------------------*/
void CCSRC_BASE::set_parameters_cc(const std::string& Label, CARD *Owner,
			       COMMON_COMPONENT *Common, double Value,
			       const node_t& N0, const node_t& N1,
			       ELEMENT* Input)
{
  node_t nodes[] = {N0, N1};
  COMPONENT::set_parameters(Label, Owner, Common, Value, 0, 0, 2, nodes);
  _input = Input;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
