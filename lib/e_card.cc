/*$Id: e_card.cc $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
//testing=script 2014.07.04
#include "e_logicnode.h"
#include "e_card.h"
#include "u_xprobe.h"
#include "u_prblst.h"
#include "u_time_pair.h"
#include "e_cardlist.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
double CARD::tr_probe_num(const std::string&)const {return NOT_VALID;}
XPROBE CARD::ac_probe_ext(const std::string&)const {return XPROBE(NOT_VALID, mtNONE);}
/*--------------------------------------------------------------------------*/
CARD::CARD()
  :CKT_BASE(),
   _subckt(nullptr),
   _owner_tag(0),
   _probes(0),
   _constant(false)
{
}
/*--------------------------------------------------------------------------*/
CARD::CARD(const std::string& S)
  :CKT_BASE(S),
   _subckt(nullptr),
   _owner_tag(0),
   _probes(0),
   _constant(false)
{
}
/*--------------------------------------------------------------------------*/
CARD::CARD(const CARD& p)
  :CKT_BASE(p),
   _subckt(nullptr), //BUG// isn't this supposed to copy????
   _owner_tag(0),
   _probes(0),
   _constant(p._constant)
{
}
/*--------------------------------------------------------------------------*/
CARD::~CARD()
{
  if (_probes > 0) {
    assert(_probe_lists);
    _probe_lists->purge(this);
  }else{
  }
  assert(_probes==0);

  // purge();
  delete _subckt;
}
/*--------------------------------------------------------------------------*/
void CARD::purge()
{
  erase_attributes(id_tag()-net_nodes(), id_tag()+1+param_count());
  CKT_BASE::purge();
}
/*--------------------------------------------------------------------------*/
const std::string CARD::long_label()const
{
  std::string buffer(short_label());
  for (const CARD* brh = owner();  brh;  brh = brh->owner()) {
    buffer = brh->short_label() + '.' + buffer;
  }
  return buffer;
}
/*--------------------------------------------------------------------------*/
//#include "trace_on.h"
void CARD:: make_fanout()
{
  if (is_device()) {
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      if (!n_(ii).is_grounded()) {
	if (!n_(ii)->subckt()) {
	  n_(ii)->new_subckt();
	  assert(n_(ii)->subckt());
	}else{
	}
	assert(n_(ii)->subckt());
	n_(ii)->subckt()->push_back(this);
	trace3(long_label(),ii, n_(ii).m_(), n_(ii)->subckt()->size());
      }else{
	trace2(long_label(),ii, n_(ii).m_());
      }
    }
  }else{
  }
}
//#include "trace_off.h"
/*--------------------------------------------------------------------------*/
/* connects_to: does this part connect to this node?
 * input: a node
 * returns: how many times this part connects to it.
 * does not traverse subcircuits
 */
int CARD::connects_to(const node_t& node)const
{untested();
  int count = 0;
  if (is_device()) {untested();
    for (int ii = 0;  ii < net_nodes();  ++ii) {untested();
      if (node.n_() == n_(ii).n_()) {untested();
        ++count;
      }else{untested();
      }
    }
  }else{untested();
  }
  return count;
}
/*--------------------------------------------------------------------------*/
void CARD::set_owner(CARD* o)
{
  assert(!o                                  // reset owner.
      || !_owner_tag                         // uninitialised.
      || !_owners[_owner_tag]._owner         // owner was reset.
      || _owner_index.at(o) == _owner_tag    // new owner==old_owner. no-op
      );

  trace2("", __LINE__, _owner_tag);
  if (_owner_index.count(o) == 0) {
    static owner_tag_t _owner_count = 0; // o==0 is the root.
    _owner_tag =  ++_owner_count;	// still a nonzero tag.
    assert(_owner_count > 0);
    _owner_index[o] = _owner_tag;
    owner_scope_t& os = _owners[_owner_tag];
    os._owner = o;
    os._scope = ((o) ? o->subckt() : &(CARD_LIST::card_list));
  }else{
    _owner_tag = _owner_index.at(o);
    // BUG: scope may have changed. update just in case.
    _owners[_owner_tag]._scope = ((o) ? o->subckt() : &(CARD_LIST::card_list));
  }

  assert( _owners.at(_owner_tag)._owner == o);
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::scope()
{
  if (owner()) {
    assert(_owners.at(_owner_tag)._scope == owner()->subckt());
  }else{
    assert(_owners.at(_owner_tag)._scope == &(CARD_LIST::card_list));
  }
  return _owners.at(_owner_tag)._scope;
}
/*--------------------------------------------------------------------------*/
const CARD_LIST* CARD::scope()const
{
  if (owner()) {
    assert(_owners.at(_owner_tag)._scope == owner()->subckt());
  }else{
    assert(_owners.at(_owner_tag)._scope == &(CARD_LIST::card_list));
  }
  return _owners.at(_owner_tag)._scope;
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope: find in same scope as myself
 * whatever is found will have the same owner as me.
 * capable of finding me.
 * throws exception if can't find.
 */
CARD* CARD::find_in_my_scope(const std::string& name)
{
  assert(name != "");
  assert(scope());

  CARD_LIST::iterator i = scope()->find_(name);
  CARD_LIST::iterator j = i;
  if (i == scope()->end()) {
    throw Exception_Cant_Find(long_label(), name,
			      ((owner()) ? owner()->long_label() : "(root)"));
  }else if(scope()->find_again(name, ++j) != scope()->end()){
    error(bWARNING, "duplicate match " + name + " in " + long_label() + "\n");
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope const. same as above, but const
 */
const CARD* CARD::find_in_my_scope(const std::string& name) const
{
  return const_cast<CARD*>(this)->find_in_my_scope(name);
}
/*--------------------------------------------------------------------------*/
/* find_in_parent_scope: find in parent's scope
 * parent is what my scope is a copy of.
 * capable of finding my parent, who should be just like me.
 * If there is no parent (I'm an original), use my scope.
 * throws exception if can't find.
 */
const CARD* CARD::find_in_parent_scope(const std::string& name)const
{
  assert(name != "");
  const CARD_LIST* p_scope = (scope()->parent()) ? scope()->parent() : scope();

  CARD_LIST::const_iterator i = p_scope->find_(name);
  CARD_LIST::const_iterator j = i;
  if (i == p_scope->end()) {
    throw Exception_Cant_Find(long_label(), name);
  }else if(p_scope->find_again(name, ++j) != p_scope->end()){
    error(bWARNING, "duplicate match " + name + " in " + long_label() + "\n");
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_looking_out: find in my or enclosing scope
 * capable of finding me, or anything back to root.
 * throws exception if can't find.
 */
const CARD* CARD::find_looking_out(const std::string& name)const
{
  try {
    return find_in_parent_scope(name);
  }catch (Exception_Cant_Find&) {
    if (owner()) {
      return owner()->find_looking_out(name);
    }else if (makes_own_scope()) {
      // probably a subckt or "module"
      // BUG? why not "find_again?"
      auto i = CARD_LIST::card_list.find_(name);
      if (i != CARD_LIST::card_list.end()) {
	return *i;
      }else{
	throw;
      }
    }else{
      throw;
    }
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR CARD::tr_review()
{
  return TIME_PAIR(NEVER,NEVER);
}
/*--------------------------------------------------------------------------*/
void CARD::new_subckt()
{
  assert(!_subckt);
  _subckt = new CARD_LIST;
}
/*--------------------------------------------------------------------------*/
void CARD::new_subckt(const CARD* Model, PARAM_LIST const* Params)
{
  delete _subckt;
  _subckt = nullptr;
  _subckt = new CARD_LIST(Model, this, scope(), Params);
  _subckt->set_owner(this);
}
/*--------------------------------------------------------------------------*/
void CARD::renew_subckt(const CARD* Model, PARAM_LIST const* Params)
{
  if (_sim->is_first_expand()) {
    new_subckt(Model, Params);
  }else{untested();
    assert(subckt());
    subckt()->attach_params(Params, scope());
  }
}
/*--------------------------------------------------------------------------*/
node_t& CARD::n_(int)const
{untested();
  unreachable();
  static node_t n;
  return n;
}
/*--------------------------------------------------------------------------*/
int CARD::set_param_by_name(std::string Name, std::string Value)
{
  //BUG// ugly linear search
  for (int i = 0;  i < param_count();  ++i) {
    for (int j = 0;  param_name(i,j) != "";  ++j) { // multiple names
      if (Umatch(Name, param_name(i,j) + ' ')) {
	set_param_by_index(i, Value, 0/*offset*/);
	return i; //success
      }else{
	//keep looking
      }
    }
  }
  throw Exception_No_Match(Name);
}
/*--------------------------------------------------------------------------*/
/* set_dev_type: Attempt to change the type of an existing device.
 * Usually, it just throws an exception, unless there is no change.
 * Practical use is to override, so you can set things like NPN vs. PNP.
 */
void CARD::set_dev_type(const std::string& New_Type)
{
  if (!Umatch(New_Type, dev_type() + ' ')) {
    //throw Exception_Cant_Set_Type(dev_type(), New_Type);
  }else{
    // it matches -- ok.
  }
}
/*--------------------------------------------------------------------------*/
double CARD::probe_num(const std::string& what)const
{
  double x;
  if (_sim->analysis_is_ac()) {
    x = ac_probe_num(what);
  }else{
    x = tr_probe_num(what);
  }
  return (std::abs(x)>=1) ? x : floor(x/OPT::floor + .5) * OPT::floor;
}
/*--------------------------------------------------------------------------*/
static char fix_case(char c)
{
  return ((OPT::case_insensitive) ? (static_cast<char>(tolower(c))) : (c));
}
/*--------------------------------------------------------------------------*/
double CARD::ac_probe_num(const std::string& what)const
{
  size_t length = what.length();
  mod_t modifier = mtNONE;
  bool want_db = false;
  char parameter[BUFLEN+1];
  strcpy(parameter, what.c_str());

  if (length > 2  &&  Umatch(&parameter[length-2], "db ")) {
    want_db = true;
    length -= 2;
  }
  if (length > 1) { // selects modifier based on last letter of parameter
    switch (fix_case(parameter[length-1])) {
      case 'm': modifier = mtMAG;   length--;	break;
      case 'p': modifier = mtPHASE; length--;	break;
      case 'r': modifier = mtREAL;  length--;	break;
      case 'i': modifier = mtIMAG;  length--;	break;
      default:  modifier = mtNONE;		break;
    }
  }
  parameter[length] = '\0'; // chop

  // "p" is "what" with the modifier chopped off.
  // Try that first.
  XPROBE xp = ac_probe_ext(parameter);

  // If we don't find it, try again with the full string.
  if (!xp.exists()) {
    xp = ac_probe_ext(what);
    if (!xp.exists()) {
      // Still didn't find anything.  Print "??".
    }else{itested();
      // The second attempt worked.
    }
  }
  return xp(modifier, want_db);
}
/*--------------------------------------------------------------------------*/
/*static*/ double CARD::probe(const CARD *This, const std::string& what)
{
  if (This) {
    return This->probe_num(what);
  }else{				/* return 0 if doesn't exist */
    return 0.0;				/* happens when optimized models */
  }					/* don't have all parts */
}
/*--------------------------------------------------------------------------*/
void CARD::q_expand_last()
{
  _sim->_explast_q.push_back(this);
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
