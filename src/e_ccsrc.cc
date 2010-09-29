/*$Id: e_ccsrc.cc,v 22.9 2002/07/23 20:09:02 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * current controlled source base
 */
#include "ap.h"
#include "e_ccsrc.h"
/*--------------------------------------------------------------------------*/
void CCSRC_BASE::parse_more_nodes(CS& cmd, int)
{
  _input_label = cmd.ctos(TOKENTERM);
  _input_label[0] = toupper(_input_label[0]);
}
/*--------------------------------------------------------------------------*/
void CCSRC_BASE::print(OMSTREAM& where, int /*detail*/)const
{
  where << short_label();
  printnodes(where);
  where << "  ";

  {if (_input){ // has been expanded
    where << _input->short_label();
  }else{ // not expanded
    where << _input_label;	/* could always print as not expanded, */
  }}				/* but this way provides error check   */
  
  if (!has_common() || value() != 0){
    where << ' ' << value();
  }
  if (has_common()){
    common()->print(where);
  }
  where << '\n';
}
/*--------------------------------------------------------------------------*/
void CCSRC_BASE::expand()
{
  {if (!_input_label.empty()){
    _input = dynamic_cast<ELEMENT*>(find_in_scope(_input_label));
  }else{
    // _input already set, an internal element.  example: mutual L.
  }}
  if (!_input || _input->is_2port() || _input->subckt().exists()){
    error(bERROR, long_label() + ": " + _input_label
	  + " cannot be used as current probe\n");
  }
  assert(_input->is_1port()  ||  _input->is_source());
  _n[IN1] = _input->_n[OUT1];
  _n[IN2] = _input->_n[OUT2];
  //_n[IN1].e = _n[IN2].e = INVALID_NODE;
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
