/*$Id: d_meter.cc,v 26.138 2013/04/24 02:44:30 al Exp $ -*- C++ -*-
 * Copyright (C) 2010 Albert Davis
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
 * 2-port "meter" device
 * does nothing to the circuit, but has probes
 */
#include "globals.h"
#include "e_elemnt.h"
#include "u_xprobe.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV : public ELEMENT {
private:
  explicit DEV(const DEV& p) :ELEMENT(p) {}
public:
  explicit DEV()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const	{return '\0';}
  std::string value_name()const {return "";}
  std::string dev_type()const	{return "meter";}
  int	   max_nodes()const	{return 4;}
  int	   min_nodes()const	{return 4;}
  int	   matrix_nodes()const	{return 4;}
  int	   net_nodes()const	{return 4;}
  CARD*	   clone()const		{return new DEV(*this);}
  void	   tr_iwant_matrix()	{}
  void	   ac_iwant_matrix()	{}
  void     precalc_last();
  double   tr_involts()const	{return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_involts_limited()const {return tr_involts();}
  COMPLEX  ac_involts()const	{return _n[IN1]->vac() - _n[IN2]->vac();}
  double   tr_probe_num(const std::string&)const;
  XPROBE   ac_probe_ext(const std::string&)const;

  std::string port_name(int i)const {
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"outp", "outn", "inp", "inn"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(true);
  set_converged();
}
/*--------------------------------------------------------------------------*/
double DEV::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "gain ")) {
    return tr_outvolts() / tr_involts();
  }else{
    return ELEMENT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
XPROBE DEV::ac_probe_ext(const std::string& x)const
{
  if (Umatch(x, "gain ")) {
    return XPROBE(ac_outvolts() / ac_involts());
  }else{
    return ELEMENT::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "meter", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
