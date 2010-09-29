/*$Id: u_probe.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * single probe item
 */
#ifndef U_PROBE_H
#define U_PROBE_H
#include "l_compar.h" // inorder
/*--------------------------------------------------------------------------*/
class CKT_BASE;
/*--------------------------------------------------------------------------*/
class PROBE {
private:
  std::string	_what;    
  int		_node;
  CKT_BASE*	_brh;
  double	_lo,_hi;
public:
  explicit  PROBE(const std::string& what, int node, double lo=0, double hi=0);
  explicit  PROBE(const std::string& what, CKT_BASE *brh,
		  double lo=0, double hi=0);
	    PROBE(const PROBE& p);
	    ~PROBE()				{detach();}

  void	    set_limit(double lo,double hi)	{_lo = lo; _hi = hi;}
  void	    detach();
  PROBE&    operator=(const PROBE& p);

  bool      operator<(const PROBE&)const;
  bool	    operator==(const PROBE&)const;
  bool	    operator!=(const PROBE& p)const	{return !(*this == p);}
  
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
