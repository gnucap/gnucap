/*$Id: u_probe.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * single probe item
 */
//testing=script,complete 2006.07.14
#ifndef U_PROBE_H
#define U_PROBE_H
#include "l_compar.h" // inorder
/*--------------------------------------------------------------------------*/
class CKT_BASE;
/*--------------------------------------------------------------------------*/
class INTERFACE PROBE {
private:
  std::string	_what;    
  const CKT_BASE* _brh;
  double	_lo,_hi;
public:
  explicit  PROBE(const std::string& what, const CKT_BASE *brh);
	    PROBE(const PROBE& p);
	    ~PROBE()				{detach();}

  void	    set_limit(double Lo,double Hi)	{_lo = Lo; _hi = Hi;}
  void	    detach();
  PROBE&    operator=(const PROBE& p);

  const std::string label()const;
  double	  value()const;
  const CKT_BASE* object()const	 {return _brh;}
  double	  lo()const	 {return _lo;}
  double	  hi()const	 {return _hi;}
  double	  range()const	 {return hi()-lo();}
  bool		  in_range()const{return in_order(lo(),value(),hi());}
private:
  double	  probe_node()const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
