/*$Id: u_sim_data.h,v 26.133 2009/11/26 05:02:28 al Exp $ -*- C++ -*-
 * Copyright (C) 2009 Albert Davis
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
//testing=obsolete
#ifndef U_SIM_DATA_H
#define U_SIM_DATA_H
#include "constant.h"
#include "l_compar.h"
#include "u_opt.h"
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
// external
class WAVE;
class CARD;
class LOGIC_NODE;
/*--------------------------------------------------------------------------*/
enum TRI_STATE {tsNO=0, tsYES=1, tsBAD=-1};
/*--------------------------------------------------------------------------*/
struct INTERFACE SIM_DATA {
  double _time0;	/* time now */
  double _freq;		/* AC frequency to analyze at (Hertz) */
  double _temp_c;	/* ambient temperature, actual */
  double _damp;		/* Newton-Raphson damping coefficient actual */
  double _dtmin;	/* min internal step size */
  double _genout;	/* tr dc input to circuit (generator) */
  bool   _bypass_ok;	/* flag: ok to bypass model evaluation */
  bool	_fulldamp; 	/* flag: big iter. jump. use full (min) damp */
  double _last_time;	/* time at which "volts" is valid */
  bool _freezetime;	/* flag: don't advance stored time */
  int _iter[iCOUNT];
  int _user_nodes;
  int _subckt_nodes;
  int _model_nodes;
  int _total_nodes;
  COMPLEX _jomega;	/* AC frequency to analyze at (radians) */
  bool _limiting;	/* flag: node limiting */
  double _vmax;
  double _vmin;
  bool _uic;		/* flag: use initial conditions (spice-like) */
  TRI_STATE _inc_mode;	/* flag: make incremental changes (3 state) */
  SIM_MODE _mode;	/* simulation type (AC, DC, ...) */
  SIM_PHASE _phase;	/* phase of simulation (iter, init-dc,) */
  int	*_nm;		/* node map (external to internal)	*/
  double *_i;		/* dc-tran current (i) vector		*/
  double *_v0;		/* dc-tran voltage, new			*/
  double *_vt1;		/* dc-tran voltage, 1 time ago		*/
			/*  used to restore after rejected step	*/
  COMPLEX *_ac;		/* ac right side			*/
  LOGIC_NODE* _nstat;	/* digital data				*/
  double *_vdc;		/* saved dc voltages			*/
  BSMATRIX<double> _aa;	/* raw matrix for DC & tran */
  BSMATRIX<double> _lu;	/* decomposed matrix for DC & tran */
  BSMATRIX<COMPLEX> _acx;/* raw & decomposed matrix for AC */
  std::priority_queue<double, std::vector<double> > _eq; /*event queue*/
  std::vector<CARD*> _loadq;
  std::vector<CARD*> _acceptq;
  std::deque<CARD*>  _evalq1; /* evaluate queues -- alternate between */
  std::deque<CARD*>  _evalq2; /* build one while other is processed */
  std::deque<CARD*>  _late_evalq; /* eval after everything else */
  std::deque<CARD*>* _evalq;   /* pointer to evalq to process */
  std::deque<CARD*>* _evalq_uc;/* pointer to evalq under construction */
  WAVE *_waves;		/* storage for waveforms "store" command*/
  SIM_DATA() {
    _evalq = &_evalq1;
    _evalq_uc = &_evalq2;
  }
  bool is_first_expand() {return !_nstat;}
  void alloc_hold_vectors(); /* s__init.cc */
  void alloc_vectors();
  void unalloc_vectors();
  void init();
  void uninit();
  void set_limit();  /* s__aux.cc */
  void set_limit(double v);
  void clear_limit();
  void keep_voltages();
  void restore_voltages();
  void zero_voltages();
  void map__nodes();		/* s__map.cc */
  void order_reverse();
  void order_forward();
  void order_auto();
  int init_node_count(int user, int sub, int mod) {
    _user_nodes=user; _subckt_nodes=sub; _model_nodes=mod; return (_total_nodes=user+sub+mod);
  }
  int newnode_subckt() {++_subckt_nodes; return ++_total_nodes;}
  int newnode_model()  {++_model_nodes;  return ++_total_nodes;}
  bool is_inc_mode()	 {return _inc_mode;}
  bool inc_mode_is_no()	 {return _inc_mode == tsNO;}
  bool inc_mode_is_bad() {return _inc_mode == tsBAD;}
  void set_inc_mode_bad() {_inc_mode = tsBAD;}
  void set_inc_mode_yes() {_inc_mode = tsYES;}
  void set_inc_mode_no()  {_inc_mode = tsNO;}
  void mark_inc_mode_bad() {
    switch (_inc_mode) {
    case tsYES: _inc_mode = tsBAD; break;
    case tsBAD: break;
    case tsNO:  break;
    }
  }
  void new_event(double etime) {
    if (etime <= BIGBIG) {
      _eq.push(etime);
    }else{
    }
  }
  void set_command_none() {_mode = s_NONE;}
  void set_command_ac()	  {_mode = s_AC;}
  void set_command_dc()	  {_mode = s_DC;}
  void set_command_op()	  {_mode = s_OP;}
  void set_command_tran() {_mode = s_TRAN;}
  void set_command_fourier() {_mode = s_FOURIER;}
  SIM_MODE sim_mode()	   {return _mode;}
  bool command_is_ac()	   {return _mode == s_AC;}
  bool command_is_dc()	   {return _mode == s_DC;}
  bool command_is_op()	   {return _mode == s_OP;}
  //bool command_is_tran()    {return _mode == s_TRAN;}
  //bool command_is_fourier() {return _mode == s_FOURIER;}
  bool analysis_is_ac()      {return _mode == s_AC;}
  bool analysis_is_dcop()    {return _mode == s_DC || _mode == s_OP;}
  bool analysis_is_static()  {return _phase == p_INIT_DC || _phase == p_DC_SWEEP;}
  bool analysis_is_restore() {return _phase == p_RESTORE;}
  bool analysis_is_tran()    {return _mode == s_TRAN || _mode == s_FOURIER;}
  bool analysis_is_tran_static()  {return analysis_is_tran() && _phase == p_INIT_DC;}
  bool analysis_is_tran_restore() {return analysis_is_tran() && _phase == p_RESTORE;}
  bool analysis_is_tran_dynamic() {return analysis_is_tran() && _phase == p_TRAN;}

  void reset_iteration_counter(int i) {assert(up_order(0,i,iCOUNT-1)); _iter[i] = 0;}
  void count_iterations(int i)	{assert(up_order(0,i,iCOUNT-1)); ++_iter[i];}
  int iteration_tag()const      {return _iter[iTOTAL];}
  int iteration_number()const   {return _iter[iSTEP];}
  bool is_initial_step()	{return (_iter[_mode] <= 1  && analysis_is_static());}
  bool is_advance_iteration()const   {return (_iter[iSTEP] == 0);}
  bool is_advance_or_first_iteration()const {assert(_iter[iSTEP]>=0); return (_iter[iSTEP]<=1);}
  bool is_first_iteration()const     {assert(_iter[iSTEP] > 0); return (_iter[iSTEP] == 1);}
  bool is_second_iteration()const    {assert(_iter[iSTEP] > 0); return (_iter[iSTEP] == 2);}
  bool is_iteration_number(int n)const    {return (_iter[iSTEP] == n);}
  bool exceeds_iteration_limit(OPT::ITL itlnum)const {return(_iter[iSTEP] > OPT::itl[itlnum]);}
  bool uic_now() {return _uic && analysis_is_static() && _time0==0.;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
