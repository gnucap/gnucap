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
#include "e_logicnode.h"
#include "u_node.h"
#include "u_xprobe.h"
/*--------------------------------------------------------------------------*/
double USER_NODE::tr_probe_num(const std::string& s) const
{
  if(_n) { untested();
    return _n->tr_probe_num(s);
  }else{
    return NODE::tr_probe_num(s);
  }
}
/*--------------------------------------------------------------------------*/
inline XPROBE USER_NODE::ac_probe_ext(const std::string& s) const
{
  if(_n) { untested();
    return _n->ac_probe_ext(s);
  }else{
    return NODE::ac_probe_ext(s);
  }
}
/*--------------------------------------------------------------------------*/
