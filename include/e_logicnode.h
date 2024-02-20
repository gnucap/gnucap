/*$Id: e_logicnode.h $ -*- C++ -*-
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
//testing=script,sparse 2006.07.11
#ifndef E_LOGICNODE_H
#define E_LOGICNODE_H
#include "e_logicval.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC;
#define	qBAD	 (0)
#define qGOOD	 (OPT::transits)
/*--------------------------------------------------------------------------*/
class INTERFACE LOGIC_NODE : public NODE {
private:
  const MODEL_LOGIC *_family;	/* logic family */
  int 	      _d_iter;		/* iteration of last update - digital */
  int 	      _a_iter;		/* iteration of last update - analog */
  double      _final_time;	/* time logic transition attains final state */
  double      _lastchange;	/* time of last change */
  double      _old_lastchange;	/* in case it rejects a step */
  smode_t     _mode;		/* simulation mode (analog or digital)*/
  LOGICVAL    _lv;		/* "logic value" (real type is LOGICVAL) */
  LOGICVAL    _old_lv;		/* in case it rejects a step */
  int	      _quality;		/* quality of digital mode */
  std::string _failure_mode;

  // so it is not pure virtual
  //const	      std::string long_label()const;
public: // virtuals
  double	tr_probe_num(const std::string&)const override;
  //XPROBE	ac_probe_ext(const std::string&)const;

public: // raw data access (rvalues)
  LOGICVAL lv()const			{return _lv;}
  int	   quality()const		{return _quality;}
  const std::string& failure_mode()const {return _failure_mode;}
  int	   d_iter()const		{return _d_iter;}
  int	   a_iter()const		{return _a_iter;}
  double   final_time()const		{return _final_time;}
  double   last_change_time()const	{return _lastchange;}
private:
  const MODEL_LOGIC* process()const	{return _family;}
  double   old_last_change_time()const	{return _old_lastchange;}
  const LOGICVAL old_lv()const		{return _old_lv;}

public: // simple calculated data access (rvalues)
  bool	 lv_future()const	{return lv().lv_future();}
  bool	 is_unknown()const	{return lv().is_unknown();}
  bool	 in_transit()const	{return final_time() < NEVER;}
private:
  bool	 is_digital()const	{return _mode == moDIGITAL;}
  bool	 is_analog()const	{return _mode == moANALOG;}
  double annotated_logic_value()const;

private: // calculated data access (rvalues)
  bool	just_reached_stable()const;

private: // raw data access (lvalues)
  void	set_quality(int q)		{_quality = q;}
  void	set_failure_mode(const std::string& f) {_failure_mode = f;}
  void	set_final_time(double t)	{_final_time = t;}
  
  void	set_d_iter()			{_d_iter = _sim->iteration_tag();}
  void	set_last_change_time()		{_lastchange = _sim->_time0;}
  void	set_last_change_time(double t)	{_lastchange = t;}
  void	set_lv(LOGICVAL v)		{_lv = v;}
  void	set_process(const MODEL_LOGIC* f) {_family = f;}
public:
  void  store_old_last_change_time()	{_old_lastchange = last_change_time();}
  void	store_old_lv()			{_old_lv = lv();}
  void	set_mode(smode_t m)		{_mode = m;}

private: // other internal
  void  set_bad_quality(const std::string& f) {
    set_quality(qBAD);
    set_failure_mode(f);
  }
  void  set_good_quality(const std::string& f = "ok") {
    set_quality(qGOOD);
    set_failure_mode(f);
  }
  void	dont_set_quality(const std::string& f = "don't know") {
    set_failure_mode(f);
  }
  void	improve_quality() {
    if (quality() < qGOOD) {
      ++_quality;
    }
  }

public: // action, used by logic
  void	      set_event(double delay, LOGICVAL v);
  void	      force_initial_value(LOGICVAL v);
  void	      propagate();
  void	      unpropagate();
  double      to_analog(const MODEL_LOGIC*f);
  void	      to_logic(const MODEL_LOGIC*f);

private: // inhibited
  explicit LOGIC_NODE(const LOGIC_NODE&):NODE(){incomplete();unreachable();}
public: // general use
  explicit LOGIC_NODE();
	   ~LOGIC_NODE() {}

public: // used by matrix
  LOGIC_NODE&	set_a_iter()	{_a_iter = _sim->iteration_tag(); return *this;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
