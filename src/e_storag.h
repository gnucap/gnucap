/*$Id: e_storag.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * "base" class for energy storage elements (L & C)
 */
//testing=script,complete 2006.07.11
#ifndef E_STORAGE_H
#define E_STORAGE_H
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
enum METHOD {mTRAPGEAR, mEULER, mTRAP, mGEAR, mTRAPEULER};
/*--------------------------------------------------------------------------*/
class STORAGE : public ELEMENT {
private:
protected:
  explicit STORAGE()			
    :ELEMENT(), _method_u(meUNKNOWN), _method_a(mTRAPGEAR)  {}
  explicit STORAGE(const STORAGE& p)
    :ELEMENT(p), _method_u(p._method_u), _method_a(p._method_a) {}
  ~STORAGE() {}
protected: // override virtual
  void	   precalc();
  void     dc_begin();
  void     tr_begin()		{STORAGE::dc_begin();}
  void     tr_restore();
  void     dc_advance();
  void     tr_advance();
  bool	   tr_needs_eval()const;
  //void   tr_queue_eval()	//ELEMENT
  DPAIR    tr_review();
  double   tr_probe_num(CS&)const;
protected:
  double   differentiate();
  double   tr_c_to_g(double c, double g)const;

  double   c_mult()const {
    return _c_mult;
  }
  double   c_mult_num()const {
    const double f[] = {1., 1., 2., 1., 1.};
    return f[_method_a];
  }
  int	   order()const {
    const int o[] = {1, 1, 2, 1, 1};
    int ord = o[_method_a];
    assert(ord <= _max_order);
    return ord;
  }
  double   error_factor()const {
    const double f[]={1./2., 1./2., 1./12., 1./2., 1./2.};
    return f[_method_a];
  }
protected:
  enum {_max_order = 2, _keep_time_steps=_max_order+1};
  method_t _method_u;	/* method to use for this part per user */
  METHOD   _method_a;	/* actual integration method (auto)	*/
  double   _time_future;
  double   _time[_keep_time_steps];
  double   _dt;
  double   _c_mult;
public: // used by commons
  FPOLY1   _q[_keep_time_steps]; /* charge or flux, and deriv.	*/
protected:
  FPOLY1   _i0;		/* after integration, now		*/
  double   _it1_f0;	/* after integration, 1 time ago	*/
protected:
  static METHOD method_select[meNUM_METHODS][meNUM_METHODS];
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
