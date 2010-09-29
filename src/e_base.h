/*$Id: e_base.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * real base for anything to do with a circuit
 */
#ifndef E_BASE_H
#define E_BASE_H
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
// this file
class CKT_BASE;
/*--------------------------------------------------------------------------*/
// external
class XPROBE;
/*--------------------------------------------------------------------------*/
class CKT_BASE {
private:
  std::string _label;
  int	      _probes;		/* number of probes set */
  static int  _devcount;
public:
  static double _vmax;
  static double _vmin;
protected:
  explicit CKT_BASE()		     :_label(),	       _probes(0){++_devcount;}
  explicit CKT_BASE(const CKT_BASE&p):_label(p._label),_probes(0){++_devcount;}
  virtual  ~CKT_BASE();
public:
  static  BSMATRIX<double>  aa;	/* raw matrix for DC & tran */
  static  BSMATRIX<double>  lu;	/* decomposed matrix for DC & tran */
  static  BSMATRIX<COMPLEX> acx;/* raw & decomposed matrix for AC */
public:
  bool operator!=(const std::string& n)const
    {return strcasecmp(_label.c_str(),n.c_str())!=0;}
  const std::string&  short_label()const{return _label;}
  virtual const std::string long_label()const = 0;
  void	  set_label(const std::string& s) {_label = s;}
  void	  set_Label(const std::string& s);
	  double      probe_num(const std::string&)const;
	  double      tr_probe_num_str(const std::string&)const;
	  double      ac_probe_num(const std::string&)const;
  virtual double      tr_probe_num(CS&)const;
  virtual XPROBE      ac_probe_ext(CS&)const;
	  void	      inc_probes()	{++_probes;}
	  void	      dec_probes()	{assert(_probes>0); --_probes;}
	  bool	      has_probes()const	{return _probes > 0;}
  static  int	      device_count()	{return _devcount;}
  static  void	      set_limit(double v);
  static  void	      clear_limit();
};
/*--------------------------------------------------------------------------*/
inline bool exists(const CKT_BASE* c) {return c!=0;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
