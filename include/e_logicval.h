/*$Id: e_logicval.h $ -*- C++ -*-
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
 * circuit node class
 */
//testing=none
#ifndef E_LOGICVAL_H
#define E_LOGICVAL_H
#include "md.h"
/*--------------------------------------------------------------------------*/
// 0=00, 1=01, Z=10, X=11
enum _LOGICVAL {
  lv00, lv01, lv10, lv11, // 0000, 0001, 0010, 0011; 00, 01, 10, 11;
  lv0Z, lv0X, lv1Z, lv1X, // 0100, 0101, 0110, 0111; 0Z, 0X, 1Z, 1X;
  lvZ0, lvX0, lvZ1, lvX1, // 1000, 1001, 1010, 1011; Z0, X0, Z1, X1;
  lvZZ, lvZX, lvXZ, lvXX, // 1100, 1101, 1110, 1111; ZZ, ZX, XZ, XX;
  lvSTABLE0=lv00, lvRISING=lv01, lvFALLING=lv10, lvSTABLE1=lv11,
  lvUNKNOWN=lvXX
};
enum {lvNUM_STATES = lvUNKNOWN+1};
/*--------------------------------------------------------------------------*/
class INTERFACE LOGICVAL {
private:
  _LOGICVAL _lv;
  static const _LOGICVAL _or_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL _xor_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL _and_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL _not_truth[lvNUM_STATES];
  static const _LOGICVAL _prop_truth[lvNUM_STATES][lvNUM_STATES];
public:
  LOGICVAL() :_lv(lvUNKNOWN)			{}
  LOGICVAL(const LOGICVAL& p)	:_lv(p._lv)	{}
  LOGICVAL(_LOGICVAL p)		:_lv(p)		{}
  ~LOGICVAL() {}

  operator _LOGICVAL()const {return static_cast<_LOGICVAL>(_lv);}
  
  LOGICVAL& operator=(_LOGICVAL p)	 {_lv=p; return *this;}
  LOGICVAL& operator=(const LOGICVAL& p) {_lv=p._lv; return *this;}

  LOGICVAL& operator&=(LOGICVAL p)
	{_lv = and_truth(_lv, p._lv); return *this;}
  LOGICVAL& operator|=(LOGICVAL p)
	{_lv = or_truth(_lv, p._lv); return *this;}
  LOGICVAL  operator^=(LOGICVAL p)
	{untested(); _lv = xor_truth(_lv, p._lv); return *this;}
  LOGICVAL  operator~()const	{return not_truth(_lv);}
  LOGICVAL  operator!()const	{return not_truth(_lv);}
  
  bool is_unknown()const	{return _lv > lv11;}
  bool lv_future()const		{assert(_lv!=lvUNKNOWN); return (_lv & 1) && !(_lv & 4);}
  bool lv_old()const		{assert(_lv!=lvUNKNOWN); return (_lv & 2) && !(_lv & 8);}

  bool is_rising() const	{return _lv == lv01 || _lv == lvX1 || _lv == lvZ1;}
  bool is_falling()const	{return _lv == lv10 || _lv == lvX0 || _lv == lvZ0;}

  LOGICVAL& set_in_transition(LOGICVAL newval);
  _LOGICVAL or_truth(_LOGICVAL, _LOGICVAL)const;
  _LOGICVAL xor_truth(_LOGICVAL, _LOGICVAL)const;
  _LOGICVAL and_truth(_LOGICVAL, _LOGICVAL)const;
  _LOGICVAL not_truth(_LOGICVAL)const;
  _LOGICVAL prop_truth(_LOGICVAL, _LOGICVAL)const;
};
/*--------------------------------------------------------------------------*/
inline LOGICVAL& LOGICVAL::set_in_transition(LOGICVAL newval)
{
  _lv = prop_truth(_lv, newval);
  assert(_lv != lvUNKNOWN);
  return *this;
}
/*--------------------------------------------------------------------------*/
inline _LOGICVAL LOGICVAL::or_truth(_LOGICVAL a, _LOGICVAL b) const
{
  assert(_or_truth[a][b] == _or_truth[b][a]);
  return _or_truth[a][b];
}
/*--------------------------------------------------------------------------*/
inline _LOGICVAL LOGICVAL::xor_truth(_LOGICVAL a, _LOGICVAL b) const
{
  assert(_xor_truth[a][b] == _xor_truth[b][a]);
  return _xor_truth[a][b];
}
/*--------------------------------------------------------------------------*/
inline _LOGICVAL LOGICVAL::and_truth(_LOGICVAL a, _LOGICVAL b) const
{
  assert(_xor_truth[a][b] == _xor_truth[b][a]);
  return _and_truth[a][b];
}
/*--------------------------------------------------------------------------*/
inline _LOGICVAL LOGICVAL::not_truth(_LOGICVAL a) const
{
  return _not_truth[a];
}
/*--------------------------------------------------------------------------*/
inline _LOGICVAL LOGICVAL::prop_truth(_LOGICVAL a, _LOGICVAL b) const
{
  return _prop_truth[a][b];
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
