/*$Id: comp.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
class Component
  :public Base
{
private:
  class Pin_Die
    :public Base
  {
  private:
    enum {pdPIN, pdDIE} _pd;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Pin_Die() : _pd(pdPIN) {}
  };
  /*------------------------------------------------------------------------*/
public:
  class Package
    :public Base
  {
  private:
    Range<Float> _r_pkg;
    Range<Float> _l_pkg;
    Range<Float> _c_pkg;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Package() {}
    const Range<Float>&	r_pkg()const {return _r_pkg;}
    const Range<Float>&	l_pkg()const {return _l_pkg;}
    const Range<Float>&	c_pkg()const {return _c_pkg;}
  };
  /*------------------------------------------------------------------------*/
  class Pin
    :public Base
  {
  private:
    std::string	_name;
    std::string	_signal;
    std::string	_model;
    Float 	_r_pin;
    Float 	_l_pin;
    Float 	_c_pin;
    void parse(CS&) {unreachable();}
    void dump(std::ostream& o)const;
  public:
    explicit Pin(CS&, const int*);
    const std::string& name()const	{return _name;}
    const std::string& signal()const	{return _signal;}
    const std::string& model()const	{return _model;}
    const Float&	 r_pin()const	{return _r_pin;}
    const Float& 	 l_pin()const	{return _l_pin;}
    const Float& 	 c_pin()const	{return _c_pin;}
  };
  /*------------------------------------------------------------------------*/
  class Pin_List
    :public List_Base<Pin>
  {
  private:
    std::map<std::string, const Pin*> _map;
    void parse(CS&);
  public: 
    explicit Pin_List() {}
  };
  /*------------------------------------------------------------------------*/
  class Pin_Map
    :public Base
  {
  private:
    std::string	_name;
    std::string	_pulldown_ref;
    std::string	_pullup_ref;
    std::string	_gnd_clamp_ref;
    std::string	_pwr_clamp_ref;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Pin_Map(CS& file)			{parse(file);}
    const std::string& name()const		{return _name;}
    const std::string& pulldown_ref()const	{return _pulldown_ref;}
    const std::string& pullup_ref()const	{return _pullup_ref;}
    const std::string& gnd_clamp_ref()const	{return _gnd_clamp_ref;}
    const std::string& pwr_clamp_ref()const	{return _pwr_clamp_ref;}
  };
  /*------------------------------------------------------------------------*/
private:
  class Pin_Map_List
    :public List_Base<Pin_Map>
  {
  private:
    mutable std::map<std::string, const Pin_Map*> _map;
    void parse(CS&);
  public: 
    explicit Pin_Map_List() {}
    const Pin_Map* operator[](const std::string& key)const {return _map[key];}
  };
  /*------------------------------------------------------------------------*/
  class Diff_Pin
    :public Base
  {
  private:
    std::string	 _pin;
    std::string	 _diff_pin;
    Float	 _vdiff;
    Range<Float> _tdelay;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Diff_Pin(CS& file)		{parse(file);}
    const std::string&  pin()const	{return _pin;}
    const std::string&  diff_pin()const	{return _diff_pin;}
    const Float& 	vdiff()const	{return _vdiff;}
    const Range<Float>& tdelay()const	{return _tdelay;}
  };
  /*------------------------------------------------------------------------*/
  class Diff_Pin_List
    :public List<Diff_Pin>
  {
  private:
    void parse(CS& f) {skip_tail(f); List<Diff_Pin>::parse(f);}
  public: 
    explicit Diff_Pin_List() {}
  };
  /*------------------------------------------------------------------------*/
  class Series_Pin
    :public Base
  {
  private:
    std::string	_pin1;
    std::string	_pin2;
    std::string	_model;
    std::string	_group;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Series_Pin(CS& file)	{parse(file);}
    const std::string& pin1()const	{return _pin1;}
    const std::string& pin2()const	{return _pin2;}
    const std::string& model()const	{return _model;}
    const std::string& group()const	{return _group;}
  };
  /*------------------------------------------------------------------------*/
  class Series_Pin_List
    :public List<Series_Pin>
  {
  private:
    void parse(CS& f) {skip_tail(f); List<Series_Pin>::parse(f);}
  public: 
    explicit Series_Pin_List() {}
  };
  /*------------------------------------------------------------------------*/
  class Series_Switch_List
    :public Base
  {
  private:
    std::map<std::string, bool> _list;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public: 
    explicit Series_Switch_List() {}
    typedef std::map<std::string, bool>::const_iterator const_iterator;
    const_iterator begin()const	  {return _list.begin();}
    const_iterator end()const	  {return _list.end();}
    bool	  is_empty()const {return _list.empty();}
  };
  /*------------------------------------------------------------------------*/
private:
  Pin_Die		_si_location;
  Pin_Die		_timing_location;
  Tail_String		_name;		// 1
  Tail_String		_manufacturer;	// 1
  std::string		_package_model;	// 2
  Package		_package;	// 1
  Pin_List		_pins;		// 1
  Pin_Map_List		_pin_mapping;	// 2
  Diff_Pin_List		_diff_pins;	// 2
  Series_Pin_List	_series_pins;	// 3
  Series_Switch_List	_series_switches; // 3
  
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit		    Component(CS& file)	   {parse(file);}
  const Pin_Die&	    si_location()const	   {return _si_location;}
  const Pin_Die&	    timing_location()const {return _timing_location;}
  const std::string&	    name()const		   {return _name;}
  const std::string&	    manufacturer()const	   {return _manufacturer;}
  const Package&	    package()const	   {return _package;}
  const Pin_List&	    pins()const		   {return _pins;}
  const Pin_Map_List&	    pin_mapping()const	   {return _pin_mapping;}
  const Diff_Pin_List&	    diff_pins()const	   {return _diff_pins;}
  const Series_Pin_List&    series_pins()const	   {return _series_pins;}
  const Series_Switch_List& series_switches()const {return _series_switches;}
};  
/*--------------------------------------------------------------------------*/
class Component_List
  :public Collection<Component>
{
public:
  explicit Component_List() {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
