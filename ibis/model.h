/*$Id: model.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 */
#include "base.h"
/*--------------------------------------------------------------------------*/
class Ramp;
class Wave;
class Wave_List;
class Model_Base;
class IO_Base;
class Series_Mosfet;
class Series_Core;
class Series;
class Series_Switch;
class Model_Spec;
class Input_Base;
class Driver_Schedule;
class Driver_Schedule_List;
class Output_Base;
class Terminator;
class Input;
class I_O;
class Output;
class Model_List;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Ramp
  :public Base
{
  Range<Fraction> _dv_dt_r;
  Range<Fraction> _dv_dt_f;
  Float		  _r_load; // 2

  void parse(CS& file);
  void dump(std::ostream& o)const;
public:
  const Range<Fraction>& dv_dt_r()const	{return _dv_dt_r;}
  const Range<Fraction>& dv_dt_f()const	{return _dv_dt_f;}
  const Float&		 r_load()const	{return _r_load;}
};
/*--------------------------------------------------------------------------*/
class Wave
  :public Base
{
  const Wave_List* _owner;
  Float	_c_fixture;
  Float	_l_fixture;
  Float	_r_fixture;
  Float	_v_fixture_min;
  Float	_v_fixture_max;
  Float	_v_fixture_typ;
  Range<Float> _v_fixture;
  Float	_c_dut;
  Float	_l_dut;
  Float	_r_dut;
  Vector _wave;

  void parse(CS& file);
  void dump(std::ostream& o)const;
public:
  Wave(CS& file, const Wave_List* owner)
    : _owner(owner), _c_fixture(0), _l_fixture(0) {parse(file);}
  const Float&	c_fixture()const	{return _c_fixture;}
  const Float&	l_fixture()const	{return _l_fixture;}
  const Float&	r_fixture()const	{return _r_fixture;}
  const Float&	v_fixture_min()const	{return _v_fixture_min;}
  const Float&	v_fixture_max()const	{return _v_fixture_max;}
  const Float&	v_fixture_typ()const	{return _v_fixture_typ;}
  const Float&	v_fixture_val()const	{return _v_fixture.value_num();}
  const Float&	c_dut()const		{return _c_dut;}
  const Float&	l_dut()const		{return _l_dut;}
  const Float&	r_dut()const		{return _r_dut;}
  const Vector&	wave()const		{return _wave;}

  typedef Vector::const_iterator const_iterator;
  const_iterator     begin()const	{return wave().begin();}
  const_iterator     end()const		{return wave().end();}
  const Vector_Item& front()const	{return *(wave().front());}
  const Vector_Item& back()const	{return *(wave().back());}
  bool		     is_empty()const	{return wave().is_empty();}
  const Range<Float>& c_comp()const;
};
/*--------------------------------------------------------------------------*/
class Wave_List
  :public Base
{
  const Output_Base* _owner;
  std::string _head;
  std::list<const Wave*> _list;

  void parse(CS& file);
  void dump(std::ostream& o)const;
public:
  Wave_List(const std::string& head, const Output_Base* owner)
    : _owner(owner), _head(head) {}
  typedef std::list<const Wave*>::const_iterator const_iterator;
  const_iterator begin()const	 {return _list.begin();}
  const_iterator end()const	 {return _list.end();}
  bool		 is_empty()const {return _list.empty();}
  const Wave&	 pullup()const  {assert(!is_empty()); return *(_list.front());}
  const Wave&	 pulldown()const {assert(!is_empty()); return *(_list.back());}
  const Range<Float>& c_comp()const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Model_Base
  :public Base
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Type
    :public Base
  {
    enum {/*1*/mtNA, mtINPUT, mtOUTPUT, mtIO, mt3STATE, mtOPEN_DRAIN,
	  /*2*/mtIO_OPEN_DRAIN, mtOPEN_SINK, mtIO_OPEN_SINK,
	  mtOPEN_SOURCE, mtIO_OPEN_SOURCE, mtINPUT_ECL, 
	  mtOUTPUT_ECL, mtIO_ECL, mtTERMINATOR, mt3STATE_ECL,
	  /*3*/mtSERIES, mtSERIES_SWITCH} _type;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    Type() : _type(mtNA) {}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  std::string	_name;
  Type		_model_type;	// 1
protected:
  void parse(CS&);
  void dump(std::ostream& o)const;
  //Model_Base() {}
  Model_Base(CS&, const std::string& name): _name(name) {}
public:
  const std::string& name()const	{return _name;}
  const Type&	     model_type()const	{return _model_type;}
};
/*--------------------------------------------------------------------------*/
class IO_Base
  :public Model_Base
{
public:
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Polarity
    :public Base
  {
    enum {pNON_INVERTING, pINVERTING} _p;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    Polarity() : _p(pNON_INVERTING) {}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Enable
    :public Base
  {
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    enum E {eACTIVE_HIGH, eACTIVE_LOW} _e;
    Enable() : _e(eACTIVE_HIGH) {}
    operator E()const {return _e;}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Submodel
    :public Base
  {
  public:
    class Mode
      :public Base
    {
      void parse(CS&);
      void dump(std::ostream& o)const;
    public:
      enum M {smALL, smDRIVING, smNON_DRIVING} _m;
      Mode() : _m(smALL) {}
      M mode()const {return _m;}
    };
      
  private:
    String	_name;
    Mode	_mode;
    
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    Submodel(CS& file) {parse(file);}
    const std::string&	name()const {return _name;}
    const Mode::M	mode()const {return _mode.mode();}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Submodel_List
    :public Base
  {
    std::list<const Submodel*> _list;
    
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    typedef std::list<const Submodel*>::const_iterator const_iterator;
    const_iterator begin()const	{return _list.begin();}
    const_iterator end()const	{return _list.end();}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
private:
  Polarity	_polarity;		// 1
  Enable	_enable;		// 1
  Float 	_vinl;			// 1
  Float 	_vinh;			// 1
  Float 	_cref;	// test load
  Float 	_rref;	// test load
  Float 	_vref;	// test load
  Float 	_vmeas;			// 2
  Range<Float>	_c_comp;		// 1
  Range<Float>	_tt_gnd;		// 3
  Range<Float>	_tt_pwr;		// 3
  Range<Float>	_temperature_range;	// 2
  Range<Float>	_voltage_range;		// 1
  Vector	_power_clamp;		// 1
  Vector	_gnd_clamp;		// 1
  Range<Float>	_power_clamp_ref;	// 2
  Range<Float>	_gnd_clamp_ref;		// 2
  Submodel_List _submodels;		// 3
protected:
  IO_Base(CS& file, const std::string& name)
    : Model_Base(file, name), _c_comp(0.) {}
  void parse(CS& file);
  void dump(std::ostream& o)const;
public:
  const Polarity&	polarity()const		{return _polarity;}
  const Enable& 	enable()const		{return _enable;}
  const Float&  	vinl()const		{return _vinl;}
  const Float&  	vinh()const		{return _vinh;}
  const Float&		cref()const		{return _cref;}
  const Float&  	rref()const		{return _rref;}
  const Float&  	vref()const		{return _vref;}
  const Float&  	vmeas()const		{return _vmeas;}
  const Range<Float>&	c_comp()const		{return _c_comp;}
  const Range<Float>&	tt_gnd()const		{return _tt_gnd;}
  const Range<Float>&	tt_pwr()const		{return _tt_pwr;}
  const Range<Float>&	temperature_range()const{return _temperature_range;}
  const Range<Float>&	voltage_range()const	{return _voltage_range;}
  const Vector&		power_clamp()const	{return _power_clamp;}
  const Vector&		gnd_clamp()const	{return _gnd_clamp;}
  const Range<Float>&	power_clamp_ref()const	{return _power_clamp_ref;}
  const Range<Float>&	gnd_clamp_ref()const	{return _gnd_clamp_ref;}
  const Submodel_List&	submodels()const 	{return _submodels;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Series_Mosfet
  :public Base
{
  class Entry
    :public Base
  {
    Float  _vds;
    Vector _table;
    
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    Entry(CS& file) {parse(file);}
  };
  
  std::list<const Entry*> _list;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  typedef std::list<const Entry*>::const_iterator const_iterator;
  const_iterator begin()const	{return _list.begin();}
  const_iterator end()const	{return _list.end();}
  bool		 is_empty()const{return _list.empty();}
};
/*--------------------------------------------------------------------------*/
class Series_Core
  :public Base
{
  Range<Float>	_r_series;
  Range<Float>	_l_series;
  Range<Float>	_rl_series;
  Range<Float>	_c_series;
  Range<Float>	_lc_series;
  Range<Float>	_rc_series;
  Vector	_series_current;
  Series_Mosfet _series_mosfet;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  const Range<Float>& 	r_series()const		{return _r_series;}
  const Range<Float>& 	l_series()const		{return _l_series;}
  const Range<Float>& 	rl_series()const	{return _rl_series;}
  const Range<Float>& 	c_series()const		{return _c_series;}
  const Range<Float>& 	lc_series()const	{return _lc_series;}
  const Range<Float>& 	rc_series()const	{return _rc_series;}
  const Vector&		series_current()const	{return _series_current;}
  const Series_Mosfet&	series_mosfet()const	{return _series_mosfet;}
};
/*--------------------------------------------------------------------------*/
class Series
  :public IO_Base
{
  Series_Core	_core;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  Series(CS& file, const std::string& name)
    : IO_Base(file, name) {parse(file);}
  const Series_Core& core()const {return _core;}
};
/*--------------------------------------------------------------------------*/
class Series_Switch
  :public IO_Base
{
  Series_Core	_on;
  Series_Core	_off;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  Series_Switch(CS& file, const std::string& name)
    : IO_Base(file, name) {parse(file);}
  const Series_Core& on()const  {return _on;}
  const Series_Core& off()const {return _off;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Model_Spec
  :public Base
{
  Range<Float>	_vinh;
  Range<Float>	_vinl;
  Range<Float>	_vinh_p;
  Range<Float>	_vinh_m;
  Range<Float>	_vinl_p;
  Range<Float>	_vinl_m;
  Range<Float>	_s_overshoot_high;
  Range<Float>	_s_overshoot_low;
  Range<Float>	_d_overshoot_high;
  Range<Float>	_d_overshoot_low;
  Range<Float>	_d_overshoot_time;
  Range<Float>	_pulse_high;
  Range<Float>	_pulse_low;
  Range<Float>	_pulse_time;
  Range<Float>	_vmeas;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  const Range<Float>&	vinh()const		{return _vinh;}
  const Range<Float>&	vinl()const		{return _vinl;}
  const Range<Float>&	vinh_p()const		{return _vinh_p;}
  const Range<Float>&	vinh_m()const		{return _vinh_m;}
  const Range<Float>&	vinl_p()const		{return _vinl_p;}
  const Range<Float>&	vinl_m()const		{return _vinl_m;}
  const Range<Float>&	s_overshoot_high()const	{return _s_overshoot_high;}
  const Range<Float>&	s_overshoot_low()const	{return _s_overshoot_low;}
  const Range<Float>&	d_overshoot_high()const	{return _d_overshoot_high;}
  const Range<Float>&	d_overshoot_low()const	{return _d_overshoot_low;}
  const Range<Float>&	d_overshoot_time()const	{return _d_overshoot_time;}
  const Range<Float>&	pulse_high()const	{return _pulse_high;}
  const Range<Float>&	pulse_low()const	{return _pulse_low;}
  const Range<Float>&	pulse_time()const	{return _pulse_time;}
  const Range<Float>&	vmeas()const		{return _vmeas;}
};
/*--------------------------------------------------------------------------*/
class Input_Base
  :public Base
{
  Model_Spec	_model_spec;		// 3
protected:
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  const Model_Spec&	model_spec()const	{return _model_spec;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Driver_Schedule
  :public Base
{
  String	_name;
  Float 	_rise_on_delay;
  Float 	_rise_off_delay;
  Float		_fall_on_delay;
  Float		_fall_off_delay;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  Driver_Schedule(CS& file) {parse(file);}
  const std::string&	name()const	{return _name;}
  const Float&	rise_on_delay()const	{return _rise_on_delay;}
  const Float&	rise_off_delay()const	{return _rise_off_delay;}
  const Float&	fall_on_delay()const	{return _fall_on_delay;}
  const Float&	fall_off_delay()const	{return _fall_off_delay;}
};
/*--------------------------------------------------------------------------*/
class Driver_Schedule_List
  :public Base
{
  std::list<const Driver_Schedule*> _list;
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  typedef std::list<const Driver_Schedule*>::const_iterator const_iterator;
  const_iterator begin()const	{return _list.begin();}
  const_iterator end()const	{return _list.end();}
  bool is_empty()const		{return _list.empty();}
};
/*--------------------------------------------------------------------------*/
class Output_Base
  :public Base
{
  const IO_Base* _owner;
  Vector	_pulldown;		// 1
  Vector	_pullup;		// 1
  Ramp  	_ramp;			// 1
  Range<Float>	_pulldown_ref;		// 2
  Range<Float>	_pullup_ref;		// 2
  Wave_List	_rising_waveforms;	// 2
  Wave_List	_falling_waveforms;	// 2
  Driver_Schedule_List _driver_sched;	// 3
protected:
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  Output_Base(const IO_Base* owner)
    : _owner(owner), _rising_waveforms("[Rising Waveform]", this),
      _falling_waveforms("[Falling Waveform]", this) {}
  const Vector&		pulldown()const		 {return _pulldown;}
  const Vector&		pullup()const		 {return _pullup;}
  const Ramp&		ramp()const		 {return _ramp;}
  const Range<Float>&	pulldown_ref()const	 {return _pulldown_ref;}
  const Range<Float>&	pullup_ref()const	 {return _pullup_ref;}
  const Wave_List&	rising_waveforms()const	 {return _rising_waveforms;}
  const Wave_List&	falling_waveforms()const {return _falling_waveforms;}
  const Driver_Schedule_List& driver_sched()const{return _driver_sched;}
  const Range<Float>&	c_comp()const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Terminator	// 2
  :public IO_Base
{
  Range<Float>	_r_gnd;
  Range<Float>	_r_power;
  Range<Float>	_r_ac;
  Range<Float>	_c_ac;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  Terminator(CS& file,const std::string& name):IO_Base(file,name){parse(file);}
  const Range<Float>&	r_gnd()const	{return _r_gnd;}
  const Range<Float>&	r_power()const	{return _r_power;}
  const Range<Float>&	r_ac()const	{return _r_ac;}
  const Range<Float>&	c_ac()const	{return _c_ac;}
};
/*--------------------------------------------------------------------------*/
class Input // includes *ECL
  :public IO_Base,
   public Input_Base
{
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  Input(CS& file, const std::string& name)
    : IO_Base(file, name) {parse(file);}
};
/*--------------------------------------------------------------------------*/
class I_O // includes 3-state, open*, *ECL
  :public IO_Base,
   public Input_Base,
   public Output_Base
{
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  I_O(CS& file, const std::string& name)
    : IO_Base(file, name), Output_Base(this) {parse(file);}
};
/*--------------------------------------------------------------------------*/
class Output // includes 3-state, open*, *ECL
  :public IO_Base,
   public Output_Base
{
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  Output(CS& file, const std::string& name)
    : IO_Base(file, name), Output_Base(this) {parse(file);}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Model_List
  :public Base
{
  std::list<const Model_Base*> _list;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  typedef std::list<const Model_Base*>::const_iterator const_iterator;
  const_iterator begin()const	{return _list.begin();}
  const_iterator end()const	{return _list.end();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline const Range<Float>& Wave::c_comp()const	     {return _owner->c_comp();}
inline const Range<Float>& Wave_List::c_comp()const  {return _owner->c_comp();}
inline const Range<Float>& Output_Base::c_comp()const{return _owner->c_comp();}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
