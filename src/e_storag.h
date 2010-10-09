/*$Id: e_storag.h,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
 * "base" class for energy storage elements (L & C)
 */
//testing=script,complete 2007.07.13
#ifndef E_STORAGE_H
#define E_STORAGE_H
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
enum METHOD {mTRAPGEAR, mEULER, mTRAP, mGEAR, mTRAPEULER};
/*--------------------------------------------------------------------------*/
INTERFACE FPOLY1 differentiate(const FPOLY1* q, const FPOLY1* i, double* time,
		     METHOD method);
/*--------------------------------------------------------------------------*/
class INTERFACE STORAGE : public ELEMENT {
private:
protected:
  explicit STORAGE()			
    :ELEMENT(), _method_u(meUNKNOWN), _method_a(mTRAPGEAR)  {}
  explicit STORAGE(const STORAGE& p)
    :ELEMENT(p), _method_u(p._method_u), _method_a(p._method_a) {}
  ~STORAGE() {}
public: // override virtual
  //void   precalc_first();	//ELEMENT
  //void   expand();		//COMPONENT
  void	   precalc_last();
  void     tr_begin();
  void     tr_restore();
  void     dc_advance();
  void     tr_advance();
  //void   tr_regress();	//ELEMENT
  bool	   tr_needs_eval()const;
  //void   tr_queue_eval()	//ELEMENT
  TIME_PAIR tr_review();
  double   tr_probe_num(const std::string&)const;
public:
  double   tr_c_to_g(double c, double g)const;
private:
  int	   order()const {
    const int o[] = {1, 1, 2, 1, 1};
    int ord = o[_method_a];
    assert(ord < OPT::_keep_time_steps);
    return ord;
  }
  double   error_factor()const {
    const double f[]={1./2., 1./2., 1./12., 1./6., 1./2.};
    return f[_method_a];
  }
public: // used by commons
  method_t _method_u;	/* method to use for this part per user */
  METHOD   _method_a;	/* actual integration method (auto)	*/
protected:
  FPOLY1   _i[OPT::_keep_time_steps]; /* deriv of _q */
protected:
  static METHOD method_select[meNUM_METHODS][meNUM_METHODS];
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
