/*$Id: e_base.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * real base for anything to do with a circuit
 */
//testing=script,complete 2006.07.12
#ifndef E_BASE_H
#define E_BASE_H
#include "l_compar.h"
#include "u_opt.h"
#include "u_status.h"
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
// this file
class CKT_BASE;
/*--------------------------------------------------------------------------*/
// external
class XPROBE;
class CS;
/*--------------------------------------------------------------------------*/
class CKT_BASE {
private:
  mutable int	_probes;		/* number of probes set */
  std::string	_label;
public:
  static double _vmax;
  static double _vmin;
protected:
  explicit CKT_BASE()			  :_probes(0), _label() {}
  explicit CKT_BASE(const std::string& s) :_probes(0), _label(s) {}
  explicit CKT_BASE(const CKT_BASE& p)	  :_probes(0), _label(p._label) {}
  virtual  ~CKT_BASE();
public:
  static  BSMATRIX<double>  aa;	/* raw matrix for DC & tran */
  static  BSMATRIX<double>  lu;	/* decomposed matrix for DC & tran */
  static  BSMATRIX<COMPLEX> acx;/* raw & decomposed matrix for AC */
public:
  bool operator!=(const std::string& n)const
    {return strcasecmp(_label.c_str(),n.c_str())!=0;}
  virtual const std::string long_label()const;
  const std::string&  short_label()const {return _label;}
	  double      probe_num(const std::string&)const;
	  double      tr_probe_num_str(const std::string&)const;
	  double      ac_probe_num(const std::string&)const;
  virtual double      tr_probe_num(CS&)const;
  virtual XPROBE      ac_probe_ext(CS&)const;
	  void	      inc_probes()const	{++_probes;}
	  void	      dec_probes()const	{assert(_probes>0); --_probes;}
	  bool	      has_probes()const	{return _probes > 0;}
  static  void	      set_limit(double v);
  static  void	      clear_limit();
protected:
  void	set_label(const std::string& s) {_label = s;}
  void	parse_label(CS&);	// as it says
  void	set_Label(const std::string& s);
  void	parse_Label(CS&);	// as above, but capitalize the first letter
  //--------------------------------------------------------------------
  // tamed access to the environment
  void	reset_iteration_counter(int i)
    {assert(up_order(0,i,iCOUNT-1)); ::status.iter[i] = 0;}
  void	count_iterations(int i)
    {assert(up_order(0,i,iCOUNT-1)); ++::status.iter[i];}
public:
  int	iteration_tag()const
    {return ::status.iter[iTOTAL];}
  int	iteration_number()const
    {return ::status.iter[iSTEP];}
  //int	print_iteration_number()const
  //  {untested();return ::status.iter[iPRINTSTEP];}

  bool	initial_step(SIM_MODE m)const
    {return (::status.iter[m] <= 1);}
  //bool is_advance_iteration()const
  //  {untested();return (::status.iter[iSTEP] == 0);}
  bool	is_advance_or_first_iteration()const
    {assert(::status.iter[iSTEP] >= 0); return (::status.iter[iSTEP] <= 1);}
  bool	is_first_iteration()const
    {assert(::status.iter[iSTEP] > 0); return (::status.iter[iSTEP] == 1);}
  bool	is_second_iteration()const
    {assert(::status.iter[iSTEP] > 0); return (::status.iter[iSTEP] == 2);}
  bool	is_iteration_number(int n)const
    {return (::status.iter[iSTEP] == n);}
  bool	exceeds_iteration_limit(OPT::ITL itlnum)const
    {return(::status.iter[iSTEP] > OPT::itl[itlnum]);}
};
/*--------------------------------------------------------------------------*/
inline bool exists(const CKT_BASE* c) {return c!=0;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
