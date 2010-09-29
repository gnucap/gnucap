/*$Id: u_nodemap.h,v 22.19 2002/09/26 04:54:38 al Exp $ -*- C++ -*-
 * Copyright (C) 2002 Albert Davis
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
 * node name to number mapping -- for named nodes
 */
#ifndef U_NODEMAP_H
#define U_NODEMAP_H
#include "md.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class NODE_MAP {
private:
  int count;
  std::map<const std::string, int> string_to_int;
  std::vector<std::string> int_to_string;
public:
  NODE_MAP() : count(0), string_to_int(), int_to_string() {}
  const std::string& operator[](int)const;
  int operator[](const std::string&);
  int new_node(const std::string&);
};
/*--------------------------------------------------------------------------*/
inline const std::string& NODE_MAP::operator[](int n)const
{
  {if (n == 0) {
    static std::string zero("0");
    return zero;
  }else{
    assert(n > 0);
    assert(n <= count);
    return int_to_string[n-1];
  }}
}
/*--------------------------------------------------------------------------*/
inline int NODE_MAP::operator[](const std::string& s)
{
  {if (s == "0") {
    untested();
    return 0;
  }else{
    int n = string_to_int[s];
    if (!n) {
      untested();
      error(bDANGER, "no such node: " + s + "\n");
    }else{
      untested();
    }
    return n;
  }}
}
/*--------------------------------------------------------------------------*/
inline int NODE_MAP::new_node(const std::string& s)
{
  {if (s == "0") {
    return 0;
  }else{
    int n = string_to_int[s];
    if (!n) {
      n = ++count;
      string_to_int[s] = n;
      int_to_string.push_back(s);
    }
    return n;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
