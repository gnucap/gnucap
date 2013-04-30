/* $Id: spice-wrapper.cc,v 26.136 2009/12/07 23:20:42 al Exp $ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This file is distributed as is, completely without warranty or
 * service support. The author and its employees are not liable for
 * the condition or performance of the software.
 *
 * The author owns the copyright but shall not be liable for any
 * infringement of copyright or other proprietary rights brought by
 * third parties against the users of the software.
 *
 * The author hereby disclaims all implied warranties.
 *
 * This author grants the users the right to modify, copy, and
 * redistribute this file, for any purpose, both within the user's
 * organization and externally.
 */
//testing=script 2008.11.28
// code style comment:  Use of "reinterpret_cast" is always bad style.
/*--------------------------------------------------------------------------*/
// spice includes
extern "C" {
  #define _complex CompleX
  #define NODE NodE
  #define public PubliC
  #define bool BooL
  #define main MaiN
  #include "capabil.h"
  #include "const.h"
  #include "iferrmsg.h"
  #include "devdefs.h"
  #include "ftedefs.h"
  #include "optdefs.h"
#ifdef JSPICE3
  #include "uflags.h"
  #include "inpdefs.h"
  #include "tskdefs.h"
#endif
  #undef main
  #undef bool
  #undef public
  #undef NODE
  #undef _complex

  #undef eq
  #undef OPT
  #undef LINEAR
  #undef STRING
  #undef BOOLEAN
  
  #undef VT_BOOL
  #undef VT_NUM
  #undef VT_REAL
  #undef VT_STRING
  #undef VT_LIST
}
/*--------------------------------------------------------------------------*/
// gnucap includes
//#include "globals.h"
#include "u_xprobe.h"
#include "d_subckt.h"
#include "e_storag.h"
#include "e_model.h"
/*--------------------------------------------------------------------------*/
// customization -- must be last
#include "wrapper.h"
#if !defined(UNCONNECTED_NODES)
  #define UNCONNECTED_NODES uDISALLOW
  #if (MIN_NET_NODES != MAX_NET_NODES)
    #error "What should I do with the unconnected nodes?"
  #endif
#endif
#if !defined(VALUE_NAME)
  #define VALUE_NAME "#"
#endif
#if !defined(TAIL_SIZE)
  #define TAIL_SIZE 1
#endif
#if !defined(IS_VALID)
#define IS_VALID {return MODEL_CARD::is_valid(d);}
#endif
/*--------------------------------------------------------------------------*/
extern SPICEdev info;
const int SPICE_INVALID_NODE = 0;
const int SPICE_UNCONNECTED_NODE = -1;
const int OFFSET = 1;
enum {uGROUND=1, uFLOAT=2, uDISALLOW=3};
const int MATRIX_NODES = (MAX_NET_NODES + INTERNAL_NODES);
class DEV_SPICE;
class MODEL_SPICE;
static COMMON_SUBCKT Default_Params(CC_STATIC);
/*--------------------------------------------------------------------------*/
/* function mapping: see devdefs.h
 * DEVparam	DEV_SPICE::parse_spice
 * DEVmodParam	MODEL_SPICE::parse_params
 * DEVload	DEV_SPICE::do_tr
 * DEVsetup	MODEL_SPICE::precalc, DEV_SPICE::expand
 * DEVunsetup	not used -- spice baggage -- just zeros some nodes
 * DEVpzSetup	not used -- pole-zero
 * DEVtemperature DEV_SPICE::internal_precalc
 * DEVtrunc	DEV_SPICE::tr_review
 * DEVfindBranch not used -- current probes for current controlled source
 * DEVacLoad	DEV_SPICE::do_ac
 * DEVaccept 	not used -- sets break points //BUG// need for: isrc, ltra, tra, vsrc, cpl, txl
 * DEVdestroy	not used -- spice baggage -- deletes a list
 * DEVmodDelete	not used -- spice baggage -- delete one model
 * DEVdelete	not used -- spice baggage -- delete one instance
 * DEVsetic  	not used -- "getic" -- initial conditions //BUG// need this
 * DEVask	DEV_SPICE::print_args, DEV_SPICE::tr_probe_num
 * DEVmodAsk	MODEL_SPICE::print_params, MODEL_SPICE::print_calculated
 * DEVpzLoad  	not used -- pole zero -- should use for AC
 * DEVconvTest 	DEV_SPICE::do_tr
 * DEVsenSetup 	not used -- sensitivity
 * DEVsenLoad  	not used -- sensitivity
 * DEVsenUpdate	not used -- sensitivity
 * DEVsenAcLoad	not used -- sensitivity
 * DEVsenPrint 	not used -- sensitivity
 * DEVsenTrunc 	not used -- sensitivity
 * DEVdisto    	not used -- distortion
 * DEVnoise   	not used -- noise
*/
/*--------------------------------------------------------------------------*/
union SPICE_MODEL_DATA {
  mutable GENmodel _gen;// generic -- use this one
  MODEL _full;		// determines size
  char  _space;		// char pointer for fill_n
  
  SPICE_MODEL_DATA() {
    std::fill_n(&_space, sizeof(MODEL), '\0');
  }
  SPICE_MODEL_DATA(const SPICE_MODEL_DATA& p) 
    : _full(p._full) {
  }
};
/*--------------------------------------------------------------------------*/
class MODEL_SPICE : public MODEL_CARD{
private:
  static int _count;
  static CKTcircuit _ckt;
public:
  SPICE_MODEL_DATA _spice_model;
  std::string _key;
  std::string _level;
  PARAM_LIST  _params;
protected:
  explicit MODEL_SPICE(const MODEL_SPICE& p);	// for clone
public:
  explicit MODEL_SPICE(const DEV_SPICE* p);	// for dispatcher
  ~MODEL_SPICE();
public: // override virtual
  MODEL_CARD* clone()const {return new MODEL_SPICE(*this);}
  bool is_valid(const COMPONENT* d)const IS_VALID
  //void expand();
  void precalc_first();

public:	// type
  void set_dev_type(const std::string& nt);
  std::string dev_type()const	{ return _key;}

public: // parameters
  bool param_is_printable(int)const;
  std::string param_name(int)const;
  std::string param_name(int i, int j)const;
  std::string param_value(int)const; 
  void set_param_by_name(std::string Name, std::string Value);
  void set_param_by_index(int, std::string&, int);
  int param_count_dont_print()const {return MODEL_CARD::param_count();}
  int param_count()const { return (static_cast<int>(_params.size()) + MODEL_CARD::param_count());}

  void Set_param_by_name(std::string Name, std::string Value);

public: // not virtual
  static int count()		{untested(); return _count;}
  static CKTcircuit* ckt()	{return &_ckt;}
  static void init_ckt();
};
/*--------------------------------------------------------------------------*/
class DEV_SPICE : public STORAGE {
private:
  static int _count;
public:
private:
  union {
    mutable GENinstance _spice_instance;
    INSTANCE _inst;
    char _inst_space;
  };
  std::string _modelname;
  const MODEL_SPICE* _model;
  const SPICE_MODEL_DATA* _spice_model;
  node_t _nodes[MATRIX_NODES];
  COMPLEX* _matrix[MATRIX_NODES+OFFSET];	// For tran, real is now, imag is saved.
  COMPLEX _matrix_core[MATRIX_NODES+OFFSET][MATRIX_NODES+OFFSET];
public:
  double _i0[MATRIX_NODES+OFFSET];	// right side - current offsets or ac real part
  double _i1[MATRIX_NODES+OFFSET];	// right side - saved ......... or ac imag part
  double _v1[MATRIX_NODES+OFFSET];	// input voltages
  double* (_states[8]); // array of 8 pointers
  double* _states_1;
  int _num_states;
  int _maxEqNum;
private:
  explicit DEV_SPICE(const DEV_SPICE& p);
public:
  explicit DEV_SPICE();
  ~DEV_SPICE();
protected: // override virtual
  char	  id_letter()const	{untested();return SPICE_LETTER[0];}
  bool	  print_type_in_spice()const {return true;}
  std::string value_name()const {return VALUE_NAME;}
  int	  max_nodes()const	{return MAX_NET_NODES;}
  int	  min_nodes()const	{return MIN_NET_NODES;}
  int	  matrix_nodes()const	{return MATRIX_NODES;}
  int	  net_nodes()const	{return _net_nodes;}
  int	  int_nodes()const	{return INTERNAL_NODES;}
  CARD*	  clone()const		{return new DEV_SPICE(*this);}
  void	  precalc_first();
  void	  expand();
  void	  precalc_last();
  //void  map_nodes();		//ELEMENT
  void	  internal_precalc();

  void	  tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  void	  tr_begin()		{STORAGE::tr_begin();   internal_precalc();}
  void	  tr_restore()		{STORAGE::tr_restore(); internal_precalc();}
  void	  dc_advance()		{STORAGE::dc_advance(); internal_precalc();}
  void	  tr_advance();
  void	  tr_regress();
  bool	  tr_needs_eval()const;
  //void  tr_queue_eval();	//ELEMENT
  bool	  do_tr();
  void	  tr_load();
  TIME_PAIR tr_review();
  void    tr_accept();
  void	  tr_unload();
  double  tr_involts()const	{unreachable();return NOT_VALID;}
  //double tr_input()const	//ELEMENT
  double  tr_involts_limited()const {unreachable();return NOT_VALID;}
  //double tr_input_limited()const //ELEMENT
  double  tr_amps()const	{itested();return NOT_VALID;}
  double  tr_probe_num(const std::string&)const;

  void	  ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void    ac_begin();
  void	  do_ac();
  void	  ac_load();
  COMPLEX ac_involts()const	{unreachable();return NOT_VALID;}
  COMPLEX ac_amps()const	{unreachable();return NOT_VALID;}
  XPROBE  ac_probe_ext(const std::string&)const {itested(); return XPROBE(NOT_VALID, mtNONE);}
  int	  tail_size()const	{return TAIL_SIZE;}
public:	// type
  void set_dev_type(const std::string& nt);
  std::string dev_type()const	{return _modelname;}
public:	// ports
  // bool port_exists(int i)const //COMPONENT
  std::string port_name(int i)const {itested();
    assert(i >= 0);
    assert(i < MAX_NET_NODES);
    return port_names[i];
  }
  // const std::string& port_value(int i)const; //COMPONENT
  //void set_port_by_name(std::string& name, std::string& value);
  //void set_port_by_index(int index, std::string& value);
private: // parameters
  //bool Param_exists(int i)const; // {return Param_name(i) != "";}
  //bool Param_is_printable(int)const;
  //std::string Param_name(int)const;
  //std::string Param_name(int i, int j)const {return STORAGE::Param_name(i, j);}
  //std::string Param_value(int)const; 
  void set_param_by_name(std::string Name, std::string Value);
  void Set_param_by_name(std::string Name, std::string Value);
  void Set_param_by_index(int, std::string&, int);
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}
private:
  CKTcircuit* ckt()const	{return MODEL_SPICE::ckt();}
  void init_ckt()		{MODEL_SPICE::init_ckt();}
  void update_ckt()const;
  void localize_ckt()const;
  int* spice_nodes()const	{return &(_spice_instance.GENnode1);}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
CKTcircuit MODEL_SPICE::_ckt;
/* 
 * used as intended, copy-out, matters: CKTnoncon, CKTtroubleElt
 *
 * used as intended, func specific: CKTmode, CKTcurrentAnalysis
 *
 * used as intended, localized, matters: CKTstates, CKTdelta, CKTdeltaOld,
 * CKTag(broken), CKTorder(broken), CKTrhs, CKTrhsOld, CKTirhs,
 * CKTtimePoints(broken,ltra?)
 *
 * used as intended, updated, matters: CKTtime, CKTtemp, CKTomega
 *
 * used as intended, constant, matters: CKTnomTemp, CKTabstol,
 * CKTreltol, CKTvoltTol, CKTgmin, CKTsrcFact(broken),
 * CKTdefaultMosL, CKTdefaultMosW, CKTdefaultMosAD, CKTdefaultMosAS,
 * 
 * used almost as intended, matters, probably ok:
 * CKTbypass: false to disable
 * CKTintegrateMethod: used only by Jspice -- set to 0 to disable
 * CKTsenInfo: used by sens, NULL to disable
 * CKTfixLimit(mos1236): 1 for Spice-2 mode
 * CKTbadMos3(mos3): false for Spice-3 mode
 *
 * misused: (not used by spice devs, use to pass gnucap stuff through)
 * CKTstat: the device (type DEV_SPICE)
 * CKTmaxEqNum: use as counter for internal nodes, pass to CKTmkVolt
 *
 * need to handle (ind): CKThead(ind,load)
 * need to handle (cpl,txl): CKTnodes(cpl,txl),
 *
 * need to handle ([iv]src):
 * CKTbreak([iv]src,accept), CKTfinalTime([iv]src,load), CKTstep([iv]src,load),
 *
 * need to handle (ltra):
 * CKTminBreak(ltra,tra,accept), CKTtrtol(ltra,trunc),
 * CKTmaxStep(ltra,temp), CKTtimeListSize(ltra,Jspice,accept),
 * CKTtimeIndex(ltra), CKTsizeIncr(ltra), CKTtryToCompact(ltra)
 *
 * used by "predictor": CKTagp, CKTpred, CKTsols
 * used by "sens": CKTirhsOld, CKTrhsOp
 * used by noise&distortion: CKTcurJob
 *
 * not used: CKTvt, CKTmaxOrder, CKTmatrix, CKTniState, CKTrhsSpare, 
 * CKTirhsSpare, CKTsenRhs, CKTseniRhs, CKTlastNode, CKTnumStates,
 * CKTdcMaxIter, CKTdcTrcvMaxIter, CKTtranMaxIter, CKTbreakSize, CKTsaveDelta,
 * CKTbreaks, CKTpivotAbsTol, CKTpivotRelTol, CKTchgtol, CKTlteReltol, CKTlteAbstol,
 * CKTdelmin, CKTinitTime, CKTdiagGmin, CKTnumSrcSteps, CKTnumGminSteps, CKThadNodeset,
 * CKTnoOpIter, CKTisSetup, CKTdeltaList, CKTkeepOpInfo, CKTtroubleNode
 */
#define assert_ckt_initialized(ckt) {				\
    assert(ckt);						\
    assert((ckt)->CKTnomTemp == OPT::tnom_c + CONSTCtoK);	\
    assert(((ckt)->CKTcurrentAnalysis == DOING_DCOP) == CKT_BASE::_sim->command_is_op());	\
    assert(((ckt)->CKTcurrentAnalysis == DOING_TRCV) == CKT_BASE::_sim->command_is_dc());	\
    assert(((ckt)->CKTcurrentAnalysis == DOING_AC  ) == CKT_BASE::_sim->analysis_is_ac());	\
    assert(((ckt)->CKTcurrentAnalysis == DOING_TRAN) == CKT_BASE::_sim->analysis_is_tran());	\
    assert((ckt)->CKTbypass  == false);				\
    assert((ckt)->CKTabstol == OPT::abstol);			\
    assert((ckt)->CKTreltol == OPT::reltol);			\
    assert((ckt)->CKTvoltTol == OPT::vntol);			\
    assert((ckt)->CKTsrcFact == 1.);				\
    assert((ckt)->CKTdefaultMosL == OPT::defl);			\
    assert((ckt)->CKTdefaultMosW == OPT::defw);			\
    assert((ckt)->CKTdefaultMosAD == OPT::defad);		\
    assert((ckt)->CKTdefaultMosAS == OPT::defas);		\
  }

void MODEL_SPICE::init_ckt()
{
  assert(ckt());
  ckt()->CKTtime = _sim->_time0;
  ckt()->CKTtemp    = _sim->_temp_c + CONSTCtoK; //manage by update
  ckt()->CKTnomTemp = OPT::tnom_c + CONSTCtoK;
  ckt()->CKTintegrateMethod = 0; // disable
  if (_sim->command_is_op()) {
    ckt()->CKTcurrentAnalysis = DOING_DCOP;
  }else if (_sim->command_is_dc()) {
    ckt()->CKTcurrentAnalysis = DOING_TRCV;
  }else if (_sim->command_is_ac()) {
    ckt()->CKTcurrentAnalysis = DOING_AC;
  }else if (_sim->analysis_is_tran()) {
    ckt()->CKTcurrentAnalysis = DOING_TRAN;
  }else{ // probably probe
    ckt()->CKTcurrentAnalysis = 0;
  }    
  ckt()->CKTmode = 0; // wrong but safe
  ckt()->CKTbypass  = false; // manage this elsewhere
  ckt()->CKTabstol = OPT::abstol;
  ckt()->CKTreltol = OPT::reltol;
  ckt()->CKTvoltTol = OPT::vntol;
  ckt()->CKTgmin = OPT::gmin;
  ckt()->CKTsrcFact = 1.; // source stepping kluge
  ckt()->CKTdefaultMosL = OPT::defl;
  ckt()->CKTdefaultMosW = OPT::defw;
  ckt()->CKTdefaultMosAD = OPT::defad;
  ckt()->CKTdefaultMosAS = OPT::defas;
  ckt()->CKTfixLimit = false; // limiting kluge 1 == spice2
#ifndef JSPICE3
  ckt()->CKTbadMos3 = false; // 1 = spice2 compat
  ckt()->CKTsenInfo = NULL; // used as flag to print sens info
#endif
#ifdef NGSPICE_17
  ckt()->CKTdefaultMosM = 1.;
  ckt()->CKTcopyNodesets = false;
#endif
  assert_ckt_initialized(ckt());
}

#define assert_ckt_up_to_date(ckt) {				\
    assert_ckt_initialized(ckt);				\
    assert((ckt)->CKTtime == CKT_BASE::_sim->_time0);		\
    assert((ckt)->CKTtemp == CKT_BASE::_sim->_temp_c + CONSTCtoK);	\
  }

void DEV_SPICE::update_ckt()const
{
  assert_ckt_initialized(ckt());
  ckt()->CKTgmin = OPT::gmin;
  ckt()->CKTstat = NULL; // mark as not localized
  ckt()->CKTtime = _sim->_time0;
  ckt()->CKTdelta = NOT_VALID; // localized
  ckt()->CKTtemp = _sim->_temp_c + CONSTCtoK;
  ckt()->CKTmode = 0;
  ckt()->CKTomega = _sim->_jomega.imag();
  assert_ckt_up_to_date(ckt());
}

#define assert_ckt_localized(ckt) {				\
    assert_ckt_up_to_date(ckt);					\
    assert((ckt)->CKTstat);					\
    DEV_SPICE* d = reinterpret_cast<DEV_SPICE*>((ckt)->CKTstat);\
    assert(d);							\
    assert(dynamic_cast<DEV_SPICE*>(d));			\
    assert((ckt)->CKTdelta == d->_dt);				\
    if (d->_dt == 0) {untested();				\
      assert((ckt)->CKTag[0] == 0);				\
      assert((ckt)->CKTorder == 1);				\
    }else if (d->_time[1] != 0 && d->_method_a == mTRAP) {	\
      assert(conchk((ckt)->CKTag[0], 2 / d->_dt));		\
      assert((ckt)->CKTorder == 2);				\
    }else{							\
      assert(conchk((ckt)->CKTag[0], 1 / d->_dt));		\
      assert((ckt)->CKTorder == 1);				\
    }								\
    assert((ckt)->CKTag[0] == (ckt)->CKTag[0]);			\
    assert((ckt)->CKTrhs == d->_i0);				\
    assert((ckt)->CKTrhsOld == d->_v1);				\
    assert((ckt)->CKTirhs == d->_i1);				\
    assert((ckt)->CKTtimePoints == d->_time);			\
  }

void DEV_SPICE::localize_ckt()const
{
  assert_ckt_up_to_date(ckt());
  ckt()->CKTstat = reinterpret_cast<STATistics*>(const_cast<DEV_SPICE*>(this));
  assert(OPT::_keep_time_steps <= 8);
  for (int ii=0; ii<8; ++ii) {
    ckt()->CKTstates[ii] = _states[ii];
  }
  //assert(ckt()->CKTtime == _time[0]); //BUG// can fail in ac
  ckt()->CKTdelta = _dt;
  for (int ii=0; ii<OPT::_keep_time_steps-1; ++ii) {
    ckt()->CKTdeltaOld[ii] = _time[ii] - _time[ii+1];
  }
  assert(_dt == NOT_VALID || conchk(ckt()->CKTdelta, ckt()->CKTdeltaOld[0]));

  //ckt()->CKTag[0] = tr_c_to_g(1, ckt()->CKTag[0]);
  // defer fixing this -- GEAR not here
  if (_dt == 0) {untested();
    ckt()->CKTag[1] = ckt()->CKTag[0] = 0;
    ckt()->CKTorder = 1;
  }else if (_time[1] != 0 && _method_a == mTRAP) {
    ckt()->CKTag[0] = 2 / _dt;
    ckt()->CKTag[1] = 1;
    ckt()->CKTorder = 2;
  }else{
    ckt()->CKTag[0] =  1 / _dt;
    ckt()->CKTag[1] = -1 / _dt;
    ckt()->CKTorder = 1;
  }
  ckt()->CKTrhs = const_cast<double*>(_i0);
  ckt()->CKTrhsOld = const_cast<double*>(_v1);
  ckt()->CKTirhs = const_cast<double*>(_i1);
  ckt()->CKTmode = 0;
  ckt()->CKTtimePoints = const_cast<double*>(_time);
  assert_ckt_localized(ckt());
}

#define assert_model_raw() {				\
    assert(_spice_model._gen.GENmodType == 0);		\
    assert(_spice_model._gen.GENnextModel == NULL);	\
    assert(_spice_model._gen.GENinstances == NULL);	\
  }
#define assert_model_unlocalized() {		\
    assert(_model->_spice_model._gen.GENinstances == NULL);\
    assert(_spice_model);			\
    assert(_spice_model->_gen.GENmodType == 0);	\
    assert(_spice_model->_gen.GENnextModel == NULL);	\
    assert(_spice_model->_gen.GENinstances == NULL);	\
    assert(_spice_model->_gen.GENmodName);		\
  }
#define assert_model_localized() {			\
    assert(_spice_model);				\
    assert(_spice_model->_gen.GENmodType == 0);		\
    assert(_spice_model->_gen.GENnextModel == NULL);	\
    assert(_spice_model->_gen.GENinstances);		\
    assert(_spice_model->_gen.GENmodName);		\
  }
#define assert_instance() {			\
    assert(_spice_instance.GENnextInstance == NULL);	\
    assert(_spice_instance.GENname == NULL);		\
  }
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
struct IFVA {
  IFvalue* _v;
  int _type;
  IFVA(IFvalue* v, int t) :_v(v), _type(t) {assert(v);}

  void operator=(const std::string& s)
  {
    CS cmd(CS::_STRING, s);
    assert(_v);
    int datatype = _type;
    if (datatype & IF_SET) {
      if (datatype & IF_VECTOR) {untested();
	incomplete();
      }else{
      }
      switch (datatype & 0xff) {
      case IF_FLAG:	_v->iValue = true;	break;
      case IF_INTEGER:	cmd >> _v->iValue;	break;
      case IF_REAL:	cmd >> _v->rValue;	break;
      case IF_COMPLEX:untested();
	//cmd >> _v->cValue;
	incomplete();
	break;
      case IF_NODE:untested();	incomplete();	break;
      case IF_STRING:
	{
	  //assert(!(_v->sValue));
	  //BUG//memory leak -- this is never deleted
	  _v->sValue = new char[s.length()+1];
	  strcpy(_v->sValue, s.c_str());
	  break;
	}
      case IF_INSTANCE: untested(); incomplete();	break;
      case IF_PARSETREE:untested(); incomplete();	break;
      default:		unreachable();			break;
      }
    }else{untested();
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_SPICE::MODEL_SPICE(const DEV_SPICE* p) 
  :MODEL_CARD(p),
   _spice_model(),
   _key(),
   _level(),
   _params()
{
  assert_model_raw();
}
/*--------------------------------------------------------------------------*/
MODEL_SPICE::MODEL_SPICE(const MODEL_SPICE& p)
  :MODEL_CARD(p),
   _spice_model(p._spice_model),
   _key(p._key),
   _level(p._level),
   _params(p._params)
{
  assert_model_raw();
}
/*--------------------------------------------------------------------------*/
MODEL_SPICE::~MODEL_SPICE()
{
  --_count;
}
/*--------------------------------------------------------------------------*/
void MODEL_SPICE::Set_param_by_name(std::string Name, std::string new_value)
{
  assert_model_raw();
  assert(info.DEVpublic.numModelParms);
  assert(info.DEVpublic.modelParms);
  assert(info.DEVmodParam);

  int num_params = *(info.DEVpublic.numModelParms);
  for (int i = 0; i < num_params; ++i) {
    IFparm Parms = info.DEVpublic.modelParms[i];
    if (Name == Parms.keyword) {
      IFvalue Value;
      IFVA v(&Value, Parms.dataType);
      v = new_value;
      int ok = info.DEVmodParam(Parms.id, &Value, &_spice_model._gen);
      assert(ok == OK);
      return;
    }else{
    }
  }
  if (Name != "level") {
    throw Exception_No_Match(Name);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_SPICE::set_param_by_name(std::string Name, std::string Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  _params.set(Name, Value);
  Set_param_by_name(Name, to_string(_params[Name].e_val(1,scope())));
}
/*--------------------------------------------------------------------------*/
void MODEL_SPICE::precalc_first()
{
  MODEL_CARD::precalc_first();

  Set_param_by_name(_key, "1");

  // push down parameters into raw spice data
  for (PARAM_LIST::iterator i = _params.begin(); i != _params.end(); ++i) {
    if (i->second.has_hard_value()) {
      try {
	Set_param_by_name(i->first, to_string(i->second.e_val(1,scope())));
      }catch (Exception_No_Match&) {
	error(bTRACE, long_label() + ": bad parameter: " + i->first + ", ignoring\n");
      }
    }else{
    }
  }

  init_ckt();
  if (info.DEVsetup) {
    assert_model_raw();
    int ok = info.DEVsetup(NULL, &_spice_model._gen, ckt(), NULL);
    assert(ok == OK);
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_SPICE::set_dev_type(const std::string& new_type)
{
  assert_model_raw();

  //_spice_model._gen.set_mod_name(short_label());
  std::string s = short_label();
  char* p = new char[s.length()+1]; //BUG//memory leak
  s.copy(p, std::string::npos);
  p[s.length()] = '\0';
  _spice_model._gen.GENmodName = p;

  _key = new_type;
  if (OPT::case_insensitive) {
    notstd::to_lower(&_key);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_SPICE::param_is_printable(int i)const
{
  assert(i < MODEL_SPICE::param_count());
  if (i >= MODEL_CARD::param_count()) {
    return _params.is_printable(MODEL_SPICE::param_count() - 1 - i);
  }else{
    return MODEL_CARD::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SPICE::param_name(int i)const
{
  assert(i < MODEL_SPICE::param_count());
  if (i >= MODEL_CARD::param_count()) {
    return _params.name(MODEL_SPICE::param_count() - 1 - i);
  }else{
    return MODEL_CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SPICE::param_name(int i, int j)const
{untested();
  assert(i < MODEL_SPICE::param_count());
  if (j == 0) {untested();
    return param_name(i);
  }else if (i >= MODEL_CARD::param_count()) {untested();
    return "";
  }else{untested();
    return MODEL_CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_SPICE::param_value(int i)const
{
  assert(i < MODEL_SPICE::param_count());
  if (i >= MODEL_CARD::param_count()) {
    return _params.value(MODEL_SPICE::param_count() - 1 - i);
  }else{
    return MODEL_CARD::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_SPICE::set_param_by_index(int, std::string&, int)
{untested();
  unreachable();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_SPICE::DEV_SPICE()
  :STORAGE(),
   _inst(),
   _modelname(""),
   _model(NULL),
   _spice_model(NULL),
   _nodes(),
   _matrix(),
   _matrix_core(),
   _i0(),
   _i1(),
   _v1(),
   _states_1(NULL),
   _num_states(0),
   _maxEqNum(0)
{
  attach_common(&Default_Params);
  std::fill_n(&_inst_space, sizeof(INSTANCE), '\0');
  assert_instance();

  {
    int* node = spice_nodes();
    for (int ii = 0; ii < matrix_nodes(); ++ii) {
      node[ii] = SPICE_INVALID_NODE;
    }
  }
  _n = _nodes;
  for (int ii = 0; ii < matrix_nodes(); ++ii) {
    assert(!(_n[ii].n_()));
  }

  for (int ii = 0; ii < matrix_nodes()+OFFSET; ++ii) {
    _matrix[ii] = _matrix_core[ii];
    assert(_matrix[ii]);
  }

  assert(OPT::_keep_time_steps <= 8);
  for (int ii=0; ii<8; ++ii) {
    _states[ii] = NULL;
  }

  ++_count;
  assert_instance();
}
/*--------------------------------------------------------------------------*/
DEV_SPICE::DEV_SPICE(const DEV_SPICE& p)
  :STORAGE(p),
   _inst(p._inst),
   _modelname(p._modelname),
   _model(p._model),
   _spice_model(p._spice_model),
   _nodes(),
   _matrix(),
   _matrix_core(),
   _i0(),
   _i1(),
   _v1(),
   _states_1(NULL),
   _num_states(p._num_states),
   _maxEqNum(p._maxEqNum)
{
  assert_instance();

  {
    int* node = spice_nodes();
    for (int ii = 0; ii < matrix_nodes(); ++ii) {
      assert(node[ii] == SPICE_INVALID_NODE);
    }
  }
  _n = _nodes;
  for (int ii = 0; ii < matrix_nodes(); ++ii) {
    _n[ii] = p._n[ii];
  }

  for (int ii = 0; ii < matrix_nodes()+OFFSET; ++ii) {
    _matrix[ii] = _matrix_core[ii];
    assert(_matrix[ii]);
  }

  assert(OPT::_keep_time_steps <= 8);
  for (int ii=0; ii<8; ++ii) {
    _states[ii] = NULL;
  }

  ++_count;
  assert_instance();
}
/*--------------------------------------------------------------------------*/
DEV_SPICE::~DEV_SPICE()
{
  assert_instance();

  --_count;
  
  if (_states[0]) {
    // regular instances
    for (int ii=0; ii<OPT::_keep_time_steps; ++ii) {
      assert(_states[ii]);
      delete [] _states[ii];
    }
    assert(_states_1);
    delete [] _states_1;
  }else{
    // prototype
    assert(OPT::_keep_time_steps <= 8);
    for (int ii=0; ii<8; ++ii) {
      assert(!_states[ii]);
    }
    assert(!_states_1);
    assert(!_spice_model);
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::set_dev_type(const std::string& new_type)
{
  _modelname = new_type;
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::Set_param_by_name(std::string Name, std::string new_value)
{
  assert_instance();
  assert(info.DEVpublic.numInstanceParms);
  assert(info.DEVpublic.instanceParms);
  assert(info.DEVparam);

  int num_params = *(info.DEVpublic.numInstanceParms);
  for (int i = 0; i < num_params; ++i) {
    IFparm Parms = info.DEVpublic.instanceParms[i];
    if (Name == Parms.keyword) {
      Set_param_by_index(i, new_value, 0);
      return;
    }else{
    }
  }
  mutable_common()->COMMON_COMPONENT::Set_param_by_name(Name, new_value);
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::set_param_by_name(std::string Name, std::string Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  COMPONENT::set_param_by_name(Name, Value);
  COMMON_SUBCKT* c = dynamic_cast<COMMON_SUBCKT*>(mutable_common());
  assert(c);
  Set_param_by_name(Name, to_string(c->_params[Name].e_val(1,scope())));
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::Set_param_by_index(int i, std::string& new_value, int offset)
{
  assert_instance();
  assert(info.DEVpublic.numInstanceParms);
  assert(info.DEVpublic.instanceParms);
  assert(info.DEVparam);

  int num_params = *(info.DEVpublic.numInstanceParms);
  if (i < num_params) {
    IFparm Parms = info.DEVpublic.instanceParms[i];
    IFvalue Value;
    IFVA v(&Value, Parms.dataType);
    v = new_value;
#ifdef JSPICE3
    int ok = info.DEVparam(ckt(), Parms.id, &Value, &_spice_instance, NULL);
#else
    int ok = info.DEVparam(Parms.id, &Value, &_spice_instance, NULL);
#endif
    assert(ok == OK);
  }else{untested();
    STORAGE::set_param_by_index(i-num_params, new_value, offset+num_params);
  }
  assert_instance();
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::expand()
{
  assert_instance();
  assert(info.DEVsetup);

  STORAGE::expand();

  init_ckt();
  
  { //-------- fix up external nodes
    int* node = spice_nodes();
    for (int ii = 0; ii < net_nodes(); ++ii) {
      node[ii] = ii+OFFSET;
    }
    if (UNCONNECTED_NODES == uGROUND) {
      for (int ii = net_nodes(); ii < max_nodes(); ++ii) {itested();
	node[ii] = ii+OFFSET;
      }
    }else if (UNCONNECTED_NODES == uFLOAT) {
      for (int ii = net_nodes(); ii < max_nodes(); ++ii) {untested();
	node[ii] = SPICE_UNCONNECTED_NODE;
      }
    }else{
      assert(UNCONNECTED_NODES == uDISALLOW);
      assert(min_nodes() == max_nodes());
      assert(net_nodes() == max_nodes());
    }
    ckt()->CKTmaxEqNum = max_nodes();

    for (int ii = max_nodes(); ii < matrix_nodes(); ++ii) {
      node[ii] = 0;
    }
  }

  { //------- attach model, set up matrix pointers
    _model = dynamic_cast<const MODEL_SPICE*>(find_model(_modelname));
    if (!_model) {
      throw Exception_Model_Type_Mismatch(long_label(), _modelname, DEVICE_TYPE);
    }else{
      SMPmatrix* matrix = reinterpret_cast<SMPmatrix*>(_matrix);
      _num_states = 0;

      _spice_instance.GENmodPtr = &(_model->_spice_model._gen);
      _spice_model = &(_model->_spice_model);
      SPICE_MODEL_DATA spice_model_copy(*_spice_model);
      spice_model_copy._gen.GENinstances = &_spice_instance;
      //-------------
      int ok = info.DEVsetup(matrix, &(spice_model_copy._gen), ckt(), &_num_states);
      // memory pointer setup, and sets _num_states
      // undesired side effects: sets values, messes up model
      //-------------
      assert(ok == OK);
      _maxEqNum = ckt()->CKTmaxEqNum;
      trace1("expand", ckt()->CKTmaxEqNum);
      assert_model_unlocalized();      
    }
  }

  //-------- allocate state vectors
  if (!_states[0]) {
    for (int ii=0; ii<OPT::_keep_time_steps; ++ii) {
      assert(!_states[ii]);
      _states[ii] = new double[_num_states];
    }
    _states_1 = new double[_num_states];
  }else{
  }
  for (int ii=0; ii<OPT::_keep_time_steps; ++ii) {
    assert(_states[ii]);
    std::fill_n(_states[ii], _num_states, 0);
  }
  assert(_states_1);
  std::fill_n(_states_1, _num_states, 0);
  
  //std::fill_n(_i1, matrix_nodes()+OFFSET, 0);
  //std::fill_n(_v1, matrix_nodes()+OFFSET, 0);
  
  //-------- fix up internal nodes
  if (_sim->is_first_expand()) {
    int start_internal = 0;
    if (UNCONNECTED_NODES == uGROUND) {
      for (int ii = net_nodes(); ii < max_nodes(); ++ii) {itested();
	_n[ii].set_to_ground(this);
      }
      start_internal = max_nodes();
    }else{
      assert(UNCONNECTED_NODES == uDISALLOW || UNCONNECTED_NODES == uFLOAT);
      start_internal = net_nodes();
    }
    assert(start_internal != 0);

    int* node = spice_nodes(); // treat as array
    char fake_name[] = "a";
    for (int ii = start_internal; ii < matrix_nodes(); ++ii) {
      if (node[ii] >= start_internal+OFFSET) {
	// real internal node
	_n[ii].new_model_node('.' + long_label() + '.' + fake_name, this);
	trace1("new int", node[ii]);
	assert(_n[ii].n_());
      }else if (node[ii] >= 0+OFFSET) {
	// collapsed to an external node
	_n[ii] = _n[node[ii]-OFFSET];
	trace1("collapse", node[ii]);
	assert(_n[ii].n_());
      }else{
	// not assigned
	trace1("not used", node[ii]);
	assert(!_n[ii].n_());
      }
      ++(*fake_name);
    }
    
    for (int ii = 0; ii < matrix_nodes(); ++ii) {
      trace2((_n[ii].n_()) ? (_n[ii].n_()->short_label().c_str()) : ("NULL"), ii, node[ii]);
    }
    
    // This could be one loop, but doing it this way gives more info.
    for (int ii = 0; ii < min_nodes(); ++ii) {
      assert(_n[ii].n_());
    }
    for (int ii = min_nodes(); ii < net_nodes(); ++ii) {
      assert(_n[ii].n_());
    }
    for (int ii = net_nodes(); ii < max_nodes(); ++ii) {itested();
      //assert(_n[ii].n_());
    }
    for (int ii = max_nodes(); ii < matrix_nodes(); ++ii) {
      assert(_n[ii].n_() || !node[ii]);
    }
  }else{untested();
  }
  assert_model_unlocalized();
  assert_instance();
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::precalc_first()
{
  STORAGE::precalc_first();

  // push down parameters into spice data
  COMMON_SUBCKT* c = dynamic_cast<COMMON_SUBCKT*>(mutable_common());
  assert(c);
  for (PARAM_LIST::iterator i = c->_params.begin(); i != c->_params.end(); ++i) {
    if (i->second.has_hard_value()) {
      try {
	Set_param_by_name(i->first, to_string(i->second.e_val(1,scope())));
      }catch (Exception_No_Match&) {
	error(bTRACE, long_label() + ": bad parameter: " + i->first + ", ignoring\n");
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::precalc_last()
{
  assert(_model);
  assert_instance();
  assert(info.DEVsetup);

  STORAGE::precalc_last();
  init_ckt();

  int* node = spice_nodes(); // treat as array	//
  int  node_stash[MATRIX_NODES];			//
  notstd::copy_n(node, matrix_nodes(), node_stash);	// save the real nodes
  
  { //-------- fix up external nodes, again ........
    // put the originals back, so DEVsetup can mess them up the same as last time
    int* node = spice_nodes();
    for (int ii = 0; ii < net_nodes(); ++ii) {
      node[ii] = ii+OFFSET;
    }
    if (UNCONNECTED_NODES == uGROUND) {
      for (int ii = net_nodes(); ii < max_nodes(); ++ii) {itested();
	node[ii] = ii+OFFSET;
      }
    }else if (UNCONNECTED_NODES == uFLOAT) {
      for (int ii = net_nodes(); ii < max_nodes(); ++ii) {untested();
	node[ii] = SPICE_UNCONNECTED_NODE;
      }
    }else{
      assert(UNCONNECTED_NODES == uDISALLOW);
      assert(min_nodes() == max_nodes());
      assert(net_nodes() == max_nodes());
    }
    ckt()->CKTmaxEqNum = max_nodes();

    for (int ii = max_nodes(); ii < matrix_nodes(); ++ii) {
      node[ii] = 0;
    }
  }
  
  {
    SMPmatrix* matrix = reinterpret_cast<SMPmatrix*>(_matrix);
    int num_states_garbage = 0;

    assert(_spice_model == &(_model->_spice_model));
    SPICE_MODEL_DATA spice_model_copy(*_spice_model);
    spice_model_copy._gen.GENinstances = &_spice_instance;

    int ok = info.DEVsetup(matrix, &(spice_model_copy._gen), ckt(), &num_states_garbage);

    assert(ok == OK);
    assert(num_states_garbage == _num_states);
    trace3("precalc", maxEqNum_stash, ckt()->CKTmaxEqNum, (maxEqNum_stash == ckt()->CKTmaxEqNum));
    assert(_maxEqNum == ckt()->CKTmaxEqNum);
    notstd::copy_n(node_stash, matrix_nodes(), node); // put back real nodes
    // hopefully, the matrix pointers are the same as last time!
  }
  assert(!is_constant());
  assert_model_unlocalized();
  assert_instance();
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::internal_precalc()
{
  update_ckt();

  if (info.DEVtemperature) {
    assert_instance();

    assert_model_unlocalized();
    _spice_model->_gen.GENinstances = &_spice_instance;
    assert_model_localized();
    
    // ELEMENT::precalc(); .. don't call .. more analysis needed
    //-----
    int ok = info.DEVtemperature(&(_spice_model->_gen), ckt());
    assert(ok == OK);
    //-----
    set_converged();
    _spice_model->_gen.GENinstances = NULL;

    assert(!is_constant());
    assert_instance();
  }else{
  }
  assert_model_unlocalized();
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::tr_advance()
{
  STORAGE::tr_advance();
  update_ckt();
  
  double* t = _states[OPT::_keep_time_steps-1];
  for (int ii = OPT::_keep_time_steps-1;  ii > 0;  --ii) {
    _states[ii] = _states[ii-1];
  }
  _states[0] = t;
  notstd::copy_n(_states[1], _num_states, _states[0]);
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::tr_regress()
{
  ELEMENT::tr_regress();
  update_ckt();
}
/*--------------------------------------------------------------------------*/
bool DEV_SPICE::tr_needs_eval()const
{
  if (is_q_for_eval()) {
    return false;
  }else if (!converged()) {
    return true;
  }else if (_sim->is_advance_iteration()) {
    return true;
  }else if (_time[1] == 0) {
    //BUG// needed for ngspice jfet, but not for spice3f5 jfet
    return true;
  }else{
    int* node = spice_nodes();
    // check the node voltages, reference to ground
    for (int ii=0; ii<matrix_nodes(); ++ii) {
      if ((node[ii] != SPICE_INVALID_NODE) 
	  && !conchk(_v1[node[ii]], _n[ii].v0(), 0, OPT::reltol*OPT::bypasstol)) {
	return true;
      }else{
      }
    }
    // check the node voltages, reference to each other
    for (int ii=0; ii<matrix_nodes(); ++ii) {
      for (int jj=0; jj<ii; ++jj) {
	if ((node[ii] != SPICE_INVALID_NODE) && (node[jj] != SPICE_INVALID_NODE) 
	    && !conchk((_v1[node[ii]] - _v1[node[jj]]),
		       (_n[ii].v0() - _n[jj].v0()),
		       0, OPT::reltol*OPT::bypasstol)) {
	  return true;
	}else{
	}
      }
    }
    return false;
  }
}
/*--------------------------------------------------------------------------*/
// MODEINITFLOAT = normal iteration
// MODEINITPRED  = 1st iter at a new time point
// MODEINITTRAN  = 1st iter at 1st time pt after initial DC
// MODEINITFIX   = like FLOAT, but honor options like "off"
// MODEINITJCT   = initial guess
// MODEINITSMSIG = like FLOAT, but setup for small signal, don't load arrays
/*--------------------------------------------------------------------------*/
bool DEV_SPICE::do_tr()
{
  assert_instance();
  assert(info.DEVload);
  assert(_num_states >= 0);
  
  localize_ckt();

  assert_model_unlocalized();
  _spice_model->_gen.GENinstances = &_spice_instance;
  assert_model_localized();

  if (_sim->analysis_is_tran_dynamic()) {
    if ((_time[1] == 0) && _sim->is_first_iteration()) {
      ckt()->CKTmode = MODETRAN | MODEINITTRAN;
    }else{
      ckt()->CKTmode = MODETRAN | MODEINITFLOAT;
    }
  }else{
    if (_sim->analysis_is_tran_static()) {
      ckt()->CKTmode = MODETRANOP;
    }else if (_sim->analysis_is_tran_restore()) {
      ckt()->CKTmode = MODETRAN;
    }else if (_sim->command_is_dc()) {
      ckt()->CKTmode = MODEDCTRANCURVE;
    }else if (_sim->command_is_op()) {
      ckt()->CKTmode = MODEDCOP;
    }else{unreachable();
      ckt()->CKTmode = 0;
    }
    if (_sim->uic_now()) {
      ckt()->CKTmode |= MODEINITFIX;
      ckt()->CKTmode |= MODEUIC;
    }else if (_sim->is_initial_step()) {
      ckt()->CKTmode |= MODEINITJCT;
    }else{
      ckt()->CKTmode |= MODEINITFLOAT;
    }
  }

  { // copy in
    int* node = spice_nodes();
    assert(ckt()->CKTrhsOld == _v1);
    std::fill_n(_v1, matrix_nodes()+OFFSET, 0);
    for (int ii = 0; ii < matrix_nodes(); ++ii) {
      if (node[ii] != SPICE_INVALID_NODE) {
	_v1[node[ii]] = _n[ii].v0();
      }else{
      }
    }
  }
  { // clear for copy out
    ckt()->CKTtroubleElt = NULL;
    ckt()->CKTnoncon = 0;
    
    assert(ckt()->CKTrhs == _i0);
    std::fill_n(_i0, matrix_nodes()+OFFSET, 0);

    for (int ii = 0; ii < matrix_nodes()+OFFSET; ++ii) {
      for (int jj = 0; jj < matrix_nodes()+OFFSET; ++jj) {
	_matrix[ii][jj].real() = 0;
      }
    }
  }

  // do the work -- it might also do convergence checking, might not
  //-----
  info.DEVload(&(_spice_model->_gen), ckt());
  //-----
  // convergence check -- gnucap method
  set_converged(ckt()->CKTnoncon == 0);
  for (int ii = 0; ii < _num_states; ++ii) {
    set_converged(converged() && conchk(_states[0][ii], _states_1[ii]));
    trace3("", ii, _states_1[ii], _states[0][ii]);
    _states_1[ii] = _states[0][ii];
  }
  for (int ii = 0; converged() && ii < matrix_nodes()+OFFSET; ++ii) {
    set_converged(conchk(_i0[ii], _i1[ii]));
  }
  for (int ii = 0; converged() && ii < matrix_nodes()+OFFSET; ++ii) {
    for (int jj = 0; converged() && jj < matrix_nodes()+OFFSET; ++jj) {
      set_converged(conchk(_matrix[ii][jj].real(), _matrix[ii][jj].imag()));
    }
  }

  // convergence check -- Spice method 
  // not sure if it is worth the effort
  if (converged() && info.DEVconvTest) {
    ckt()->CKTnoncon = 0;
    ckt()->CKTrhs = _v1;    // Spice overlaps _i0 with _v1 as CKTrhs
    info.DEVconvTest(&(_spice_model->_gen), ckt());
    set_converged(ckt()->CKTnoncon == 0);
  }else{
    // either no separate test or already failed
  }

  bool needs_load = !converged();
  for (int ii = 0; !needs_load && ii < matrix_nodes()+OFFSET; ++ii) {
    needs_load = !conchk(_i0[ii], _i1[ii], 0, OPT::reltol*OPT::loadtol);
  }
  for (int ii = 0; !needs_load && ii < matrix_nodes()+OFFSET; ++ii) {
    for (int jj = 0; !needs_load && jj < matrix_nodes()+OFFSET; ++jj) {
      needs_load = !conchk(_matrix[ii][jj].real(), _matrix[ii][jj].imag(),
			   0, OPT::reltol*OPT::loadtol);
    }
  }
  
  if (needs_load) {
    q_load();
  }else{
  }

  assert_model_localized();
  _spice_model->_gen.GENinstances = NULL;
  assert_model_unlocalized();
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::tr_load()
{
#ifndef NDEBUG
  if (_loaditer == _sim->iteration_tag()) {untested();
    error(bDANGER, long_label() + " internal error: double load\n");
  }
  _loaditer = _sim->iteration_tag();
#endif

  int ihit[MATRIX_NODES+OFFSET];
  int jhit[MATRIX_NODES+OFFSET];

  std::fill_n(ihit, matrix_nodes()+OFFSET, 0);
  std::fill_n(jhit, matrix_nodes()+OFFSET, 0);

  int* node = spice_nodes();
  for (int ii = 0; ii < matrix_nodes(); ++ii) {
    int ni = node[ii];
    if (ni && !ihit[ni]) {
      ihit[ni] = 1;
      int nii = ni-OFFSET;
      trace4("", ii, ni, _i0[ni], _i1[ni]);
      tr_load_source_point(_n[ii], &(_i0[ni]), &(_i1[ni]));
      for (int jj = 0; jj < matrix_nodes(); ++jj) {
	int nj = node[jj];
	if (nj && jhit[nj] != ni) {
	  jhit[nj] = ni;
	  int njj = nj-OFFSET;
	  trace2("", jj, nj);
	  trace2("", _matrix[nii][njj].real(), _matrix[nii][njj].imag());
	  tr_load_point(_n[ii], _n[jj], &(_matrix[nii][njj].real()), &(_matrix[nii][njj].imag()));
	}else{
	  trace2("skip", jj, nj);
	}
      }
    }else{
      trace2("=========skip", ii, ni);
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::tr_unload()
{untested();incomplete();

  for (int ii = 0; ii < matrix_nodes(); ++ii) {untested();
    for (int jj = 0; jj < matrix_nodes(); ++jj) {untested();
      _matrix[ii][jj].real() = 0;
    }
  }
  _sim->mark_inc_mode_bad();
  tr_load();
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_SPICE::tr_review()
{
  // not calling STORAGE::tr_review();

  if (info.DEVtrunc) {
    localize_ckt();
    assert_instance();
    //q_accept();
    
    assert_model_unlocalized();
    _spice_model->_gen.GENinstances = &_spice_instance;
    assert_model_localized();
    
    ckt()->CKTtroubleElt = NULL;
    double timestep = NEVER;
    //-----
    info.DEVtrunc(&(_spice_model->_gen), ckt(), &timestep);
    //-----
    
    _time_by._error_estimate = tr_review_check_and_convert(timestep);
    _time_by._event = NEVER;

    _spice_model->_gen.GENinstances = NULL;
    assert_model_unlocalized();
    return _time_by;
  }else{
    return TIME_PAIR(NEVER,NEVER);
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::tr_accept()
{
  assert_model_unlocalized();
  _spice_model->_gen.GENinstances = &_spice_instance;
  assert_model_localized();
  
  //STORAGE::tr_accept(); // doesn't do anything

  if (_sim->analysis_is_dcop() || _sim->analysis_is_ac()) {
    localize_ckt();

    // don't copy in
    assert(ckt()->CKTrhsOld == _v1);
    // _v1 already has correct values
    // _n[ii].v0() is not correct -- may have been cleared
    
    ckt()->CKTmode = MODEINITSMSIG;
    info.DEVload(&(_spice_model->_gen), ckt());
  }else{itested();
  }
  assert_model_localized();
  _spice_model->_gen.GENinstances = NULL;
  assert_model_unlocalized();
}
/*--------------------------------------------------------------------------*/
double DEV_SPICE::tr_probe_num(const std::string& x)const
{
  localize_ckt();
  assert_ckt_up_to_date(ckt());
  assert_instance();

  // all of the "states" in state array
  int num_probe_states = std::min(_num_states, int(sizeof(state_names)/sizeof(std::string)));
  for (int ii=0; ii<num_probe_states && state_names[ii]!=""; ++ii) {
    if (Umatch(x, state_names[ii] + ' ')) {
      return _states[0][ii];
    }else{
    }
  }

  if (info.DEVask) {
    // data that Spice has, through "ask"
    assert(info.DEVpublic.numInstanceParms);
    assert(info.DEVpublic.instanceParms);

    for (int ii=0; ii<(*(info.DEVpublic.numInstanceParms)); ++ii) {
      IFparm Parms = info.DEVpublic.instanceParms[ii];
      int datatype = Parms.dataType;
      if (datatype & IF_ASK && Umatch(x, std::string(Parms.keyword) + ' ')) {
	IFvalue v;
	int ok = info.DEVask(ckt(), &_spice_instance, Parms.id, &v, NULL);
	if (ok == OK) {
	  switch (datatype & 0xff) {
	  case IF_FLAG:
	  case IF_INTEGER:
	    return v.iValue;
	  case IF_REAL:
	    return v.rValue;
	  case IF_COMPLEX:
	  case IF_STRING:
	  default:
	    // make believe it is not a match
	    incomplete();
	    break; // break switch, continue loop
	  }
	}else{untested();
	  // match, but not useful here.
	  assert(errMsg);
	  free(errMsg);
	  errMsg = NULL;
	  assert(errRtn);
	  errRtn = NULL;
	  // maybe there is more than one match, so continue loop
	}
      }else{
	// really not a match, keep looking
      }
    }
  }else{untested();
    // no DEVask ..  can't do anything.
  }
  return STORAGE::tr_probe_num(x);
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::ac_begin()
{
  STORAGE::ac_begin();
  internal_precalc();
  tr_accept();
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::do_ac()
{
  if (info.DEVacLoad || info.DEVpzLoad) {
    assert_instance();
    assert(_num_states >= 0);

    assert_model_unlocalized();
    _spice_model->_gen.GENinstances = &_spice_instance;
    assert_model_localized();

    localize_ckt();
    ckt()->CKTmode = MODEAC;
    ckt()->CKTomega = _sim->_jomega.imag();

    // clear for copy out
    ckt()->CKTtroubleElt = NULL;
    std::fill_n(_i0, matrix_nodes()+OFFSET, 0);
    std::fill_n(_i1, matrix_nodes()+OFFSET, 0);
    for (int ii = 0; ii < matrix_nodes()+OFFSET; ++ii) {
      for (int jj = 0; jj < matrix_nodes()+OFFSET; ++jj) {
	_matrix[ii][jj] = 0;
      }
    }
    
    if (info.DEVpzLoad) {
      info.DEVpzLoad(&(_spice_model->_gen), ckt(), reinterpret_cast<SPcomplex*>(&_sim->_jomega));
    }else if (info.DEVacLoad) {
      info.DEVacLoad(&(_spice_model->_gen), ckt());
    }else{unreachable();
      // nothing
    }

    assert_model_localized();
    _spice_model->_gen.GENinstances = NULL;
    assert_model_unlocalized();
  }else{untested();
    // there is no acLoad function
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SPICE::ac_load()
{
  if (info.DEVacLoad) {
    assert_ckt_up_to_date(ckt());
    
    int ihit[MATRIX_NODES+OFFSET];
    int jhit[MATRIX_NODES+OFFSET];
    
    std::fill_n(ihit, matrix_nodes()+OFFSET, 0);
    std::fill_n(jhit, matrix_nodes()+OFFSET, 0);
    
    int* node = spice_nodes();
    for (int ii = 0; ii < matrix_nodes(); ++ii) {
      int ni = node[ii];
      if (ni && !ihit[ni]) {
	ihit[ni] = 1;
	int nii = ni-OFFSET;
	trace3("", ii, ni, nii);
	ac_load_source_point(_n[ii], COMPLEX(_i0[node[ni]], _i1[node[ni]]));
	for (int jj = 0; jj < matrix_nodes(); ++jj) {
	  int nj = node[jj];
	  if (nj && jhit[nj] != ni) {
	    jhit[nj] = ni;
	    int njj = nj-OFFSET;
	    trace3("", jj, nj, njj);
	    trace2("", _matrix[nii][njj].real(), _matrix[nii][njj].imag());
	    ac_load_point(_n[ii], _n[jj], _matrix[nii][njj]);
	  }else{
	    trace2("skip", jj, nj);
	  }
	}
      }else{
	trace2("=========skip", ii, ni);
      }
    }
  }else{
    // there is no acLoad function
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
extern "C" {
  // needed to satisfy references.  Supposedly unreachable.  Stubs.
  char *errMsg = NULL;
  char *errRtn = NULL;
  char* tmalloc(int size) {itested(); return static_cast<char*>(calloc(size,1));}
  char* trealloc(char*, int) {untested();incomplete(); return NULL;} //DEVnoise
  void txfree(char *ptr) {
    if (ptr) {itested();
      free(ptr);
    }else{untested();
    }
  }
 
  static class FT_CURCKT : public circ {
    TSKtask junk;
  public:
    FT_CURCKT() {
      junk.jobs = NULL;
      ci_curTask = reinterpret_cast<char*>(&junk);
      //::ft_curckt = this;
    }
  } stupid_ft_circ_pointer_to_pointer_hack;

  circ *ft_curckt = &stupid_ft_circ_pointer_to_pointer_hack;

  IFuid CKTnodName(CKTcircuit*,int) {incomplete();return IFuid();} //DEVsenPrint

  double D1i2F1(double, double, double) {incomplete(); return NOT_VALID;} //DEVdisto
  double D1i3F1(double, double, double, double, double, double) {incomplete(); return NOT_VALID;}
  double D1iF12(double, double, double, double, double) {incomplete(); return NOT_VALID;}
  double D1i2F12(double, double, double, double, double, double, double, double, double,
		 double) {incomplete(); return NOT_VALID;}
  double D1n2F1(double, double, double) {incomplete(); return NOT_VALID;}
  double D1n3F1(double, double, double, double, double, double) {incomplete(); return NOT_VALID;}
  double D1nF12(double, double, double, double, double) {incomplete(); return NOT_VALID;}
  double D1n2F12(double, double, double, double, double, double, double, double, double,
		 double) {incomplete(); return NOT_VALID;}
  double DFn2F1(double, double, double, double, double, double, double, double, double,
		double, double, double) {incomplete(); return NOT_VALID;}
  double DFi2F1(double, double, double, double, double, double, double, double, double,
		double, double, double) {incomplete(); return NOT_VALID;}
  double DFi3F1(double, double, double, double, double, double, double, double, double,
		double, double, double, double, double, double, double, double, double,
		double, double, double, double, double, double, double, double, double,
		double) {incomplete(); return NOT_VALID;}
  double DFn3F1(double, double, double, double, double, double, double, double, double,
		double, double, double, double, double, double, double, double, double,
		double, double, double, double, double, double, double, double, double,
		double) {incomplete(); return NOT_VALID;}
  double DFnF12(double, double, double, double, double, double, double, double,
		double, double, double, double, double, double, double, double,
		double, double) {incomplete(); return NOT_VALID;}
  double DFiF12(double, double, double, double, double, double, double, double,
		double, double, double, double, double, double, double, double,
		double, double) {incomplete(); return NOT_VALID;}
  struct DpassStr; //DEVdisto
  double DFn2F12(DpassStr*) {incomplete(); return NOT_VALID;}
  double DFi2F12(DpassStr*) {incomplete(); return NOT_VALID;}
  struct Dderivs;
  void AtanDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void CosDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void CubeDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void DivDeriv(Dderivs*, Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void EqualDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void ExpDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void InvDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void MultDeriv(Dderivs*, Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void PlusDeriv(Dderivs*, Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void PowDeriv(Dderivs*, Dderivs*, double) {incomplete();} //DEVdisto
  void SqrtDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void TanDeriv(Dderivs*, Dderivs*) {incomplete();} //DEVdisto
  void TimesDeriv(Dderivs*, Dderivs*, double) {incomplete();} //DEVdisto
#ifdef JSPICE3
  double Nintegrate(double, double, double, GENERIC*) {incomplete(); return NOT_VALID;} //DEVnoise
#else
  double Nintegrate(double, double, double, Ndata*) {incomplete(); return NOT_VALID;} //DEVnoise
#endif
  void NevalSrc(double*, double*, CKTcircuit*, int, int, int, double) {incomplete();} //DEVnoise
  void NevalSrc2(double*, double*, CKTcircuit*, int, int, int, double, double) {incomplete();}
  //------------------------------------------------
  // should be constants, but spice wants them to be variables.
  double CONSTroot2(sqrt(2.));
  double CONSTvt0(P_CELSIUS0*P_K_Q);
  double CONSTKoverQ(P_K_Q);
  double CONSTe(M_E);
  //------------------------------------------------
  // ngspice baggage
  int ARCHme = 0;
  // jspice baggage
  IFsimulator *ft_sim;
  //------------------------------------------------
  //------------------------------------------------
  int IFerror(int flags, char* format, IFuid* names) /* output an error or warning message */
  {itested();
    static struct mesg {
      const char *string;
      long flag;
    } msgs[] = {
      { "Warning", ERR_WARNING } ,
      { "Fatal error", ERR_FATAL } ,
      { "Panic", ERR_PANIC } ,
      { "Note", ERR_INFO } ,
      { NULL, 0 }
    } ;

    struct mesg *m;
    char buf[10000], *s, *bptr;
    int nindex = 0;

    for (m = msgs; m->flag; m++) {
      if (flags & m->flag) {
	error(bDANGER, "%s: ", m->string);
      }else{
      }
    }

    for (s = format, bptr = buf; *s; s++) {
      if (*s == '%' && (s == format || *(s-1) != '%') && *(s+1) == 's') {
	if (names[nindex]) {
	  strcpy(bptr, reinterpret_cast<char*>(names[nindex]));
	}else{
	  strcpy(bptr, "(null)");
	}
        bptr += strlen(bptr);
        s++;
        nindex++;
      } else {
        *bptr++ = *s;
      }
    }
    *bptr = '\0';
    switch (flags) {
    case ERR_WARNING:error(bWARNING,buf); break;
    case ERR_FATAL:  error(bDANGER, buf); throw Exception("");
    case ERR_PANIC:  error(bDANGER, buf); throw Exception("");
    case ERR_INFO:   error(bTRACE,  buf); break;
    default:         error(bDANGER, buf); break;
    }
    return 0;
  }
  void internalerror(char *message)
  {untested();
    error(bDANGER, "internal error: %s\n", message);
  }
    
#ifdef NGSPICE_17
  int CKTinst2Node(void*, void*, int, CKTnode**, IFuid*)
  {untested();incomplete();
    return 10;
  }
#endif
#ifdef JSPICE3
  static IFfrontEnd fe = {
    NULL, //int ((*IFnewUid)());	/* create a new UID in the circuit */ noise, urcsetup
    NULL, //int ((*IFpauseTest)());	/* should we stop now? */ noisean.c only
    NULL, //double ((*IFseconds)());	/* what time is it? */ bjtdisto unused ifdef only (unused)
    IFerror, //int ((*IFerror)());	/* output an error or warning message */ temp, setup
    NULL,
    NULL,
    NULL,
    NULL,
    NULL 
  };
#else
  static IFfrontEnd fe = {
    NULL, //int ((*IFnewUid)());	/* create a new UID in the circuit */ noise, urcsetup
    NULL, //int ((*IFdelUid)());	/* create a new UID in the circuit */ not used
    NULL, //int ((*IFpauseTest)());	/* should we stop now? */ noisean.c only
    NULL, //double ((*IFseconds)());	/* what time is it? */ bjtdisto unused ifdef only (unused)
    IFerror, //int ((*IFerror)());	/* output an error or warning message */ temp, setup
    NULL, //int ((*OUTpBeginPlot)());	/* start pointwise output plot */ noisean.c only
    NULL, //int ((*OUTpData)());	/* data for pointwise plot */ noisean.c only
    NULL, //int ((*OUTwBeginPlot)());	/* start windowed output plot */ not used
    NULL, //int ((*OUTwReference)());	/* independent vector for windowed plot */ not used
    NULL, //int ((*OUTwData)());	/* data for windowed plot */ not used
    NULL, //int ((*OUTwEnd)());		/* signal end of windows */ not used
    NULL, //int ((*OUTendPlot)());	/* end of plot */ not used
    NULL, //int ((*OUTbeginDomain)());	/* start nested domain */ not used
    NULL, //int ((*OUTendDomain)());	/* end nested domain */ not used
    NULL  //int ((*OUTattributes)());	/* specify output attributes of node */ noisean.c only
  };
#endif
  IFfrontEnd* SPfrontEnd = &fe;
  //------------------------------------------------
  //------------------------------------------------
  int CKTsetBreak(CKTcircuit* ckt, double time)
  {untested();
    if (time < ckt->CKTminBreak) {untested();
      ckt->CKTminBreak = time;
    }else{untested();
    }
    return OK;
  }
  //------------------------------------------------
  void CKTterr(int qcap, CKTcircuit* ckt,double *time_step)
  {
    assert_ckt_localized(ckt);

    std::valarray<FPOLY1> q(OPT::_keep_time_steps);

    for (int ii = 0; ii < OPT::_keep_time_steps; ++ii) {
      assert(ckt->CKTstates[ii]);
      q[ii].x  = NOT_VALID;
      q[ii].f0 = ckt->CKTstates[ii][qcap];
      q[ii].f1 = NOT_VALID;
    }

    DEV_SPICE* d = reinterpret_cast<DEV_SPICE*>(ckt->CKTstat);
    assert(d);
    assert(dynamic_cast<DEV_SPICE*>(d));

    *time_step = std::min(d->tr_review_trunc_error(&q[0]), *time_step);
  }
  //------------------------------------------------
  int NIintegrate(CKTcircuit* ckt,double* geq,double* ceq,double cap,int qcap)
  { //-- used by DEVload (not DC)
    assert_ckt_localized(ckt);

    METHOD method;
    if (ckt->CKTorder == 1) {
      method = mEULER;
    }else{
      assert(ckt->CKTtimePoints[1] != 0.);
      assert(ckt->CKTorder == 2);
      method = mTRAP;
    }

    std::valarray<FPOLY1> q(OPT::_keep_time_steps);
    std::valarray<FPOLY1> i(OPT::_keep_time_steps);

    for (int ii = 0; ii < OPT::_keep_time_steps; ++ii) {
      assert(ckt->CKTstates[ii]);
      q[ii].x  = NOT_VALID;
      q[ii].f0 = ckt->CKTstates[ii][qcap];
      q[ii].f1 = cap;
      trace3("", ii, q[ii].f0, q[ii].f1);
      i[ii].x  = NOT_VALID;
      i[ii].f0 = ckt->CKTstates[ii][qcap+1];
      i[ii].f1 = q[ii].f1 * ckt->CKTag[0];
      trace3("", ii, i[ii].f0, i[ii].f1);
      assert(q[ii].f0 == q[ii].f0);
      assert(q[ii].f1 == q[ii].f1);
      assert(i[ii].f0 == i[ii].f0);
      assert(i[ii].f1 == i[ii].f1);
    }

    i[0] = differentiate(&q[0], &i[0], ckt->CKTtimePoints, method);
    assert(i[0].f0 == i[0].f0);
    assert(i[0].f1 == i[0].f1);
    trace2("", i[0].f0, i[0].f1);

    ckt->CKTstates[0][qcap+1] = i[0].f0;

    assert(ckt->CKTdelta != 0. || (ckt->CKTag[0] == 0. && i[0].f0 == 0.));
    *ceq = i[0].f0 - q[0].f0 * ckt->CKTag[0];
    *geq = i[0].f1;
    assert(*ceq == *ceq);
    assert(*geq == *geq);
    trace2("", *ceq, *geq);
    return OK;
  }
  //------------------------------------------------
  //------------------------------------------------
  int CKTmkVolt(CKTcircuit* ckt, CKTnode** n, IFuid, char*)
  { // get a new node number. -- used by DEVsetup
    assert_ckt_initialized(ckt);
    assert(n);
    static CKTnode n_static;	// always used only on next line
    *n = &n_static;		// so reuse static structure
    (*n)->number = ((ckt->CKTmaxEqNum)++)+OFFSET;
    trace1(__FUNCTION__, (*n)->number);
    // local number (- == internal)   only number is used
    return OK;
  }
  int CKTmkCur(CKTcircuit* ckt, CKTnode** n, IFuid i, char* c)
  {untested();
    return CKTmkVolt(ckt, n, i, c);
  }
  //------------------------------------------------  
  int CKTdltNNum(void*,int)
  {untested(); // complement to CKTmkVolt. -- used by DEVunsetup
    // deletes what was new in CKTmkVolt
    // Nothing, because of no alloc there.
    return OK;
  }
  //------------------------------------------------  
  //------------------------------------------------  
  double* SMPmakeElt(SMPmatrix* mm, int r, int c)
  { // returns a pointer m[r][c] -- used by DEVsetup
    //trace2("", r, c);
    assert(mm);
    if (r == 0 || c == 0) {
      static double trash;
      trash = 0;
      return &trash;
    }else{
      assert(r >= 0+OFFSET);
      assert(r < MATRIX_NODES+OFFSET);
      assert(c >= 0+OFFSET);
      assert(c < MATRIX_NODES+OFFSET);
      COMPLEX** m = reinterpret_cast<COMPLEX**>(mm);
      assert(m);
      assert(m[r-OFFSET]);
      return reinterpret_cast<double*>(&(m[r-OFFSET][c-OFFSET]));
    }
  }
  //------------------------------------------------  
#ifdef JSPICE3
  int IFnewUid(GENERIC*,IFuid*,IFuid,char*,int,GENERIC**) {incomplete(); return 0;}
  int INPpName(char*,IFvalue*,GENERIC*,int,GENERIC*) {incomplete(); return 0;}
  char *INPdevErr(char *) {incomplete(); return NULL;}
  char *INPerror(int) {incomplete(); return NULL;}
  spREAL *spGetElement(char* s, int r, int c) {return SMPmakeElt(s,r,c);}
  char *INPerrCat(char *, char *) {incomplete(); return NULL;}
  int INPgndInsert(GENERIC*,char**,INPtables*,GENERIC**) {incomplete(); return 0;}
  char * INPdevParse(char**,GENERIC*,int,GENERIC*,double*,int*,INPtables*) {incomplete(); return NULL;}
  char *INPgetMod(GENERIC*,char*,INPmodel**,INPtables*) {incomplete(); return NULL;}
  int INPgetTok(char**,char**,int) {incomplete(); return 0;}
  int INPlookMod(char*) {incomplete(); return 0;}
  int INPtermInsert(GENERIC*,char**,INPtables*,GENERIC**) {incomplete(); return 0;}
  int INPinsert(char**,INPtables*) {incomplete(); return 0;}
  char *copy(char*) {incomplete(); return NULL;}
  int NIsum(CKTcircuit*,double*,int) {incomplete(); return 0;}
  double INPevaluate(char**,int*,int) {incomplete(); return NOT_VALID;}
  IFvalue *INPgetValue(GENERIC*,char**,int,INPtables*) {incomplete(); return NULL;}
#endif
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Verify that the layout of complex is as Spice assumes.
// This is not guaranteed by the standard, but is believed to always be true.
static struct COMPLEX_TEST {
  COMPLEX_TEST() {
    COMPLEX x;
    COMPLEX* px = &x;
    double* prx = &x.real();
    double* pix = &x.imag();
    assert(reinterpret_cast<void*>(prx) == reinterpret_cast<void*>(px));
    assert(reinterpret_cast<void*>(pix-1) == reinterpret_cast<void*>(px));
  }
  ~COMPLEX_TEST() {
  }  
} complex_test;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int MODEL_SPICE::_count = -1;
int DEV_SPICE::_count = -1;

static DEV_SPICE p0;
static DISPATCHER<CARD>::INSTALL
  d0(&device_dispatcher, std::string(SPICE_LETTER) + "|" + DEVICE_TYPE, &p0);

static MODEL_SPICE p1(&p0);
static DISPATCHER<MODEL_CARD>::INSTALL
  d1(&model_dispatcher, MODEL_TYPE, &p1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
