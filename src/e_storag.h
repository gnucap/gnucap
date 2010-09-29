/*$Id: e_storag.h,v 24.12 2003/12/14 01:58:35 al Exp $ -*- C++ -*-
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
 * "base" class for energy storage elements (L & C)
 */
#ifndef E_STORAGE_H
#define E_STORAGE_H
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
enum METHOD {mTRAPGEAR, mEULER, mTRAP, mGEAR, mTRAPEULER};
/*--------------------------------------------------------------------------*/
class STORAGE : public ELEMENT {
protected:
  explicit STORAGE()			
    :ELEMENT(), _method_u(meUNKNOWN), _method_a(mTRAPGEAR)  {}
  explicit STORAGE(const STORAGE& p)
    :ELEMENT(p), _method_u(p._method_u), _method_a(p._method_a) {}
protected: // override virtual
  void	   precalc();
  void     dc_begin();
  void     tr_begin()		{STORAGE::dc_begin();}
  void     tr_restore();
  void     dc_advance();
  void     tr_advance();
  bool	   tr_needs_eval()	{return true;}
  //void   tr_queue_eval()	//ELEMENT
  double   tr_review()		{return const_tr_review();}
  double   tr_probe_num(CS&)const;
protected:
  double   differentiate();
  double   tr_c_to_g(double c, double g)const;
  double   const_tr_review()const;

  double   c_mult()const {return _c_mult;}
  double   c_mult_num()const
		{const double f[] = {1., 1., 2., 1., 1.}; return f[_method_a];}
  int	   order()const {const int o[] = {1, 1, 2, 1, 1}; return o[_method_a];}
  double   error_factor()const
  {const double f[]={1./2., 1./2., 1./12., 1./2., 1./2.}; return f[_method_a];}
protected:
  enum {_max_order = 2, _keep_time_steps=_max_order+2};
  method_t _method_u;	/* method to use for this part per user */
  METHOD   _method_a;	/* actual integration method (auto)	*/
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
#if 0
// this works, and saves significant time
// but possible errors.
// Errors do not seem significant, but I can't tell without more data.
// Taking it out to be sure.
  bool	   tr_needs_eval() {
    {if (!converged()) {
      return true;
    }else{
      return (STATUS::iter[iSTEP]<2 || !conchk(_y0.x,tr_involts(),OPT::vntol));
    }}
  }
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
