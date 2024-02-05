/*$Id: e_base.h $ -*- C++ -*-
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
 * real base for anything to do with a circuit
 */
//testing=script 2014.07.04
#ifndef E_BASE_H
#define E_BASE_H
#include "u_attrib.h"
#include "l_indirect.h"
/*--------------------------------------------------------------------------*/
// external
class XPROBE;
class WAVE;
class OMSTREAM;
struct SIM_DATA;
class PROBE_LISTS;
/*--------------------------------------------------------------------------*/
class INTERFACE CKT_BASE {
private:
  mutable int	_probes;		/* number of probes set */
  std::string	_label;
public:
  static SIM_DATA* _sim;
  static PROBE_LISTS* _probe_lists;
//protected: // TODO
  static INDIRECT<ATTRIB_LIST_p>* _attribs;
public:
  ATTRIB_LIST_p& attributes(const void* x) { assert(_attribs); return (*_attribs)[x];}
  const ATTRIB_LIST_p& attributes(const void* x)const { assert(_attribs); return _attribs->at(x);}
  //--------------------------------------------------------------------
protected: // create and destroy
  explicit CKT_BASE()			  :_probes(0), _label() {}
  explicit CKT_BASE(const std::string& s) :_probes(0), _label(s) {}
  explicit CKT_BASE(const CKT_BASE& p)	  :_probes(0), _label(p._label) {}
  virtual  ~CKT_BASE();
  virtual void	      purge() {}
  //--------------------------------------------------------------------
public: // user stuff
  virtual std::string help_text()const {return "";}
  virtual bool	      help(CS&, OMSTREAM&)const;
  virtual std::string status()const {untested();return "";}
  //--------------------------------------------------------------------
public: // probes
	  double      probe_num(const std::string&)const;
	  double      ac_probe_num(const std::string&)const;
  virtual double      tr_probe_num(const std::string&)const;
  virtual XPROBE      ac_probe_ext(const std::string&)const;
	  void	      inc_probes()const	{++_probes;}
	  void	      dec_probes()const	{assert(_probes>0); --_probes;}
	  bool	      has_probes()const	{return _probes > 0;}
  static  double      probe(const CKT_BASE*,const std::string&);
  static  WAVE*	      find_wave(const std::string& probe_name);
  //--------------------------------------------------------------------
public: // label
  bool operator!=(const std::string& n)const;
  virtual const std::string long_label()const;
  const std::string&  short_label()const {return _label;}
  void	set_label(const std::string& s) {_label = s;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
