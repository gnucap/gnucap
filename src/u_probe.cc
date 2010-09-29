/*$Id: u_probe.cc,v 22.10 2002/07/26 03:15:27 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * general probe object
 */
#include "ap.h"
#include "e_node.h"
#include "u_probe.h"
/*--------------------------------------------------------------------------*/
//		PROBE::PROBE(const string&,int,double,double);
//		PROBE::PROBE(const string&,CKT_BASE*,double,double);
//		PROBE::PROBE(const PROBE& p);
//	PROBE&  PROBE::operator=(const PROBE& p);
//	bool	PROBE::operator<(const PROBE&)const;
//	bool	PROBE::operator==(const PROBE&)const;
//	void	PROBE::detach();
//	char*	PROBE::label(void)const;
//	double	PROBE::value(void)const;
//	double	PROBE::probe_node(void)const;
/*--------------------------------------------------------------------------*/
extern NODE* nstat;
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const std::string& what, int node, double lo, double hi)
  :_what(what),
   _node(node),
   _brh(0),
   _lo(lo),
   _hi(hi)
{
}
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const std::string& what, CKT_BASE *brh, double lo, double hi)
  :_what(what),
   _node(0),
   _brh(brh),
   _lo(lo),
   _hi(hi)
{
  assert(brh);
  _brh->inc_probes();
}
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const PROBE& p)
  :_what(p._what),
   _node(p._node),
   _brh(p._brh),
   _lo(p._lo),
   _hi(p._hi)
{
  if (_brh){
    _brh->inc_probes();
  }
}
/*--------------------------------------------------------------------------*/
/* operator=  ...  assignment
 * copy a probe
 */
PROBE& PROBE::operator=(const PROBE& p)
{
  detach();
  _what = p._what;
  _node = p._node;
  _brh  = p._brh;
  _lo   = p._lo;
  _hi   = p._hi;
  if (_brh){
    _brh->inc_probes();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* operator <
 * something to create an order relationship
 * to satisfy Microsoft's STL, which insists on generating code for
 * the whole SORTED vector class, even though it is not used.
 */
bool PROBE::operator<(const PROBE& p)const
{
  unreachable();
  return ((_brh<p._brh) || ((_brh==p._brh) && (_what<p._what)));
}
/*--------------------------------------------------------------------------*/
bool PROBE::operator==(const PROBE& p)const
{
  unreachable();
  return ((_brh==p._brh) && (_what==p._what));
}
/*--------------------------------------------------------------------------*/
/* "detach" a probe from a device, node, etc.
 * which means ...  1. tell the device that the probe has been removed
 *		    2. blank out the probe, so it doesn't reference anything
 * does not remove the probe from the list
 */
void PROBE::detach()
{
  if (_brh){
    _brh->dec_probes();
  }
  _what = "";
  _node = 0;
  _brh = 0;
}
/*--------------------------------------------------------------------------*/
/* label: returns a string corresponding to a possible probe point
 * (suitable for printing)
 * It has nothing to do with whether it was selected or not
 */
const std::string PROBE::label(void)const
{
  assert(!(_brh && _node));
  {if (!_brh){
    char s[BUFLEN+8];
    sprintf(s, "%s(%u)", _what.c_str(), _node);
    return std::string(s);
  }else{
    return _what + '(' + _brh->long_label() + ')';
  }}
}
/*--------------------------------------------------------------------------*/
double PROBE::value(void)const
{
  assert(!(_brh && _node));
  {if (!_brh){
    {if (_node <= STATUS::total_nodes
	 && _node > 0
	 &&  nstat[NODE::to_internal(_node)].needs_analog()){
      return nstat[NODE::to_internal(_node)].probe_num(_what);
    }else{
      return probe_node();
    }}
  }else{
    return _brh->probe_num(_what);
  }}
}
/*--------------------------------------------------------------------------*/
double PROBE::probe_node(void)const
{
  CS cmd(_what);
  {if (cmd.pmatch("ITer")  &&  _node < iCOUNT){
    assert(iPRINTSTEP - sCOUNT == 0);
    assert(iSTEP      - sCOUNT == 1);
    assert(iTOTAL     - sCOUNT == 2);
    assert(iCOUNT     - sCOUNT == 3);
    return static_cast<double>(STATUS::iter[_node+sCOUNT]);
  }else if (cmd.pmatch("Control")  &&  _node < cCOUNT){
    return static_cast<double>(STATUS::control[_node]);
  }else if (cmd.pmatch("DAMP")){
    untested();
    return SIM::damp;
  }else if (cmd.pmatch("GEnerator")){
    untested();
    return SIM::genout;
  }else if (cmd.pmatch("TEmperature")){
    untested();
    return SIM::temp + ABS_ZERO;
  }else if (cmd.pmatch("TIme")){
    untested();
    return SIM::time0;
  }else{
    return NOT_VALID;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
