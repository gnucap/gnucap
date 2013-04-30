/*$Id: lang_spice.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
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
 */
//testing=script 2007.07.13
#include "u_status.h"
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "u_lang.h"

// header hack
#include "d_logic.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class LANG_SPICE_BASE : public LANGUAGE {
public:
  enum EOB {NO_EXIT_ON_BLANK, EXIT_ON_BLANK};

public: // override virtual, used by callback
  std::string arg_front()const {return " ";}
  std::string arg_mid()const {return "=";}
  std::string arg_back()const {return "";}

public: // override virtual, called by commands
  DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*);
  DEV_DOT*	parse_command(CS&, DEV_DOT*);
  MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*);
  MODEL_SUBCKT* parse_module(CS&, MODEL_SUBCKT*);
  COMPONENT*	parse_instance(CS&, COMPONENT*);
  std::string	find_type_in_string(CS&);
public: // "local?", called by own commands
  void parse_module_body(CS&, MODEL_SUBCKT*, CARD_LIST*, const std::string&,
			 EOB, const std::string&);
private: // local
  void parse_type(CS&, CARD*);
  void parse_args(CS&, CARD*);
  void parse_label(CS&, CARD*);
  void parse_ports(CS&, COMPONENT*, int minnodes, int start, int num_nodes, bool all_new);
private: // compatibility hacks
  void parse_element_using_obsolete_callback(CS&, COMPONENT*);
  void parse_logic_using_obsolete_callback(CS&, COMPONENT*);

private: // override virtual, called by print_item
  void print_paramset(OMSTREAM&, const MODEL_CARD*);
  void print_module(OMSTREAM&, const MODEL_SUBCKT*);
  void print_instance(OMSTREAM&, const COMPONENT*);
  void print_comment(OMSTREAM&, const DEV_COMMENT*);
  void print_command(OMSTREAM&, const DEV_DOT*);
private: // local
  void print_args(OMSTREAM&, const MODEL_CARD*);
  void print_type(OMSTREAM&, const COMPONENT*);
  void print_args(OMSTREAM&, const COMPONENT*);
  void print_label(OMSTREAM&, const COMPONENT*);
  void print_ports(OMSTREAM&, const COMPONENT*);
};
/*--------------------------------------------------------------------------*/
class LANG_SPICE : public LANG_SPICE_BASE {
public:
  std::string name()const {return "spice";}
  bool case_insensitive()const {return true;}
  UNITS units()const {return uSPICE;}
  void parse_top_item(CS&, CARD_LIST*);
} lang_spice;
DISPATCHER<LANGUAGE>::INSTALL
	ds(&language_dispatcher, lang_spice.name(), &lang_spice);
/*--------------------------------------------------------------------------*/
class LANG_ACS : public LANG_SPICE_BASE {
public:
  std::string name()const {return "acs";}
  bool case_insensitive()const {return true;}
  UNITS units()const {return uSPICE;}
} lang_acs;
DISPATCHER<LANGUAGE>::INSTALL
	da(&language_dispatcher, lang_acs.name(), &lang_acs);
/*--------------------------------------------------------------------------*/
DEV_COMMENT p0;
DISPATCHER<CARD>::INSTALL
	d0(&device_dispatcher, ";|#|*|'|\"|dev_comment", &p0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void skip_pre_stuff(CS& cmd)
{
  cmd.skipbl();
  while (cmd.umatch(CKT_PROMPT)) {untested();
    /* skip any number of copies of the prompt */
  }
  cmd.umatch(ANTI_COMMENT); /* skip mark so spice ignores but gnucap reads */
}
/*--------------------------------------------------------------------------*/
/* count_ports: figure out how many ports
 * returns the number of ports
 * side effect:  "CS" is advanced to past the ports, ready for what follows
 */
static int count_ports(CS& cmd, int maxnodes, int minnodes, int leave_tail, int start)
{
  assert(start < maxnodes);
  assert(minnodes <= maxnodes);

  int num_nodes = 0;
  std::vector<unsigned> spots;
  int paren = cmd.skip1b('(');
  int i = start;
  // loop over the tokens to try to guess where the nodes end
  // and other stuff begins
  spots.push_back(cmd.cursor());
  for (;;) {
    ++i;
    //cmd.skiparg();
    std::string node_name;
    cmd >> node_name;
    spots.push_back(cmd.cursor());

    if (paren && cmd.skip1b(')')) {
      num_nodes = i;
      break;
    }else if (cmd.is_end()) {
      // found the end, no '='
      if (i <= minnodes) {
	num_nodes = i;
      }else if (i <= minnodes + leave_tail) {
	num_nodes = minnodes;
      }else if (i <= maxnodes + leave_tail) {
	num_nodes = i - leave_tail;
      }else{
	num_nodes = maxnodes;
      }
      break;
    }else if (cmd.skip1b("({})")) {
      // found '(', it's past the end of nodes
      if (i > maxnodes + leave_tail) {
	num_nodes = maxnodes;
      }else{
	num_nodes = i - leave_tail;
      }
      break;
    }else if (cmd.skip1b('=')) {
      // found '=', it's past the end of nodes
      if (i > maxnodes + leave_tail + 1) {
	num_nodes = maxnodes;
      }else{
	num_nodes = i - leave_tail - 1;
      }
      break;
    }else{
    }
  }
  if (num_nodes < start) {
    cmd.reset(spots.back());
    throw Exception("what's this?");
  }else{
  }

  cmd.reset(spots[static_cast<unsigned>(num_nodes-start)]);

  //cmd.warn(bDANGER, "past-nodes?");
  //BUG// assert fails on current controlled sources with (node node dev) syntax
  // it's ok with (node node) dev syntax or node node dev syntax
  assert(num_nodes <= maxnodes);
  return num_nodes;
}
/*--------------------------------------------------------------------------*/
/* parse_ports: parse circuit connections from input string
 * fills in the rest of the array with 0
 * returns the number of nodes actually read
 * The purpose of this complexity is to handle the variants of Spice formats,
 * which might have keys between nodes,
 * an unknown number of nodes with unknown number of args and no delimeters.
 * Consider:  X1 a b c d e f g h i j k
 * Clearly (?) a,b,c,d,e are nodes, f is a subckt name, the rest are args.
 * But how do you know?
 *
 * Args:
 * maxnodes: the maximum number of port nodes to parse.
 *	Stop here, even if it seems there should be more.
 *
 * minnodes: the minimum number of port nodes to parse.
 *	It is an error if there are fewer than this.
 *
 * leave_tail: The number of arguments that are not nodes,
 *	and don't have equal signs, following.
 *	It is an aid to distinguishing the nodes from things that follow.
 *	minnodes wins over leave_tail, but leave_tail wins over maxnodes.
 *	The above example would need leave_tail=6 to parse correctly.
 *	This one: X1 a b c d e f g=h i=j k
 *	where a,b,c,d,e are nodes, f is a subckt or model identifier
 *	would parse correctly with leave_tail=1.
 *	Usual values for leave_tail are 0 or 1.
 *
 * start: start at this number, used for continuing after HSPICE kluge.
 *	Other than that, the only useful value for start is 0.
 *
 * all_new: All node names must be new (not already defined) and unique.
 *	This is used for the header line in .subckt, which starts clean.
 *	The main benefit is to reject duplicates.
 */
void LANG_SPICE_BASE::parse_ports(CS& cmd, COMPONENT* x, int minnodes,
			     int start, int num_nodes, bool all_new)
{
  assert(x);

  int paren = cmd.skip1b('(');
  int ii = start;
  unsigned here1 = cmd.cursor();
  try{
    for (;;) {
      here1 = cmd.cursor();
      if (paren && cmd.skip1b(')')) {
	--paren;
	break; // done.  have closing paren.
      }else if (ii >= num_nodes) {
	break; // done.  have maxnodes.
      }else if (!cmd.more()) {untested();
	break; // done.  premature end of line.
      }else if (OPT::keys_between_nodes &&
		(cmd.umatch("poly ")
		 || cmd.umatch("pwl ")
		 || cmd.umatch("vccap ")
		 || cmd.umatch("vcg ")
		 || cmd.umatch("vcr "))
		) {
	cmd.reset(here1);
	break; // done, found reserved word between nodes
      }else{
	//----------------------
	unsigned here = cmd.cursor();
	std::string node_name;
	cmd >> node_name;
	if (cmd.stuck(&here)) {itested();
	  // didn't move, probably a terminator.
	  throw Exception("bad node name");
	}else{
	  // legal node name, store it.
	  x->set_port_by_index(ii, node_name);
	}
	//----------------------
	if (!(x->node_is_connected(ii))) {untested();
	  break; // illegal node name, might be proper exit.
	}else{
	  if (all_new) {
	    if (x->node_is_grounded(ii)) {untested();
	      cmd.warn(bDANGER, here1, "node 0 not allowed here");
	    }else{
	    }
	  }else{
	  }
	  ++ii;
	}
      }
    }
  }catch (Exception& e) {itested();
    cmd.warn(bDANGER, here1, e.message());
  }
  if (ii < minnodes) {itested();
    cmd.warn(bDANGER, "need " + to_string(minnodes-ii) +" more nodes");
  }else{
  }
  if (paren != 0) {untested();
    cmd.warn(bWARNING, "need )");
  }else{
  }
  //assert(x->_net_nodes == ii);
  
  // ground unused input nodes
  for (int iii = ii;  iii < minnodes;  ++iii) {itested();
    x->set_port_to_ground(iii);
  }
  //assert(x->_net_nodes >= ii);
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_element_using_obsolete_callback(CS& cmd, COMPONENT* x)
{
  assert(x);
  ELEMENT* xx = dynamic_cast<ELEMENT*>(x);
  assert(xx);

  {
    unsigned here = cmd.cursor();
    int stop_nodes = x->max_nodes() - x->num_current_ports();
    int num_nodes = count_ports(cmd, stop_nodes, 0,  0,   0);
    //				     max	 min tail already_got
    cmd.reset(here);
    parse_ports(cmd, x, 0,  0,		num_nodes, false);
    //			min already_got
  }
  int gotnodes = x->_net_nodes;
  COMMON_COMPONENT* c = NULL;

  if (gotnodes < x->min_nodes()) {
    // HSPICE compatibility kluge.
    // The device type or function type could be stuck between the nodes.
    xx->skip_dev_type(cmd); // (redundant)
    c = EVAL_BM_ACTION_BASE::parse_func_type(cmd);
    {
      unsigned here = cmd.cursor();
      int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), 0, gotnodes);
      cmd.reset(here);
      parse_ports(cmd, x, x->min_nodes(), gotnodes, num_nodes, false);
    }
  }else{
    // Normal mode.  nodes first, then data.
  }

  if (!c) {
    xx->skip_dev_type(cmd); // (redundant)
    c = bm_dispatcher.clone("eval_bm_cond");
  }else{
  }
  if (!c) {
    c = bm_dispatcher.clone("eval_bm_value");
  }else{
  }
  assert(c);
  
  // we have a blank common of the most general type
  // (or HSPICE kluge)
  // let it continue parsing

  unsigned here = cmd.cursor();
  c->parse_common_obsolete_callback(cmd); //BUG//callback
  if (cmd.stuck(&here)) {untested();
    cmd.warn(bDANGER, "needs a value");
  }else{
  }

  // At this point, there is ALWAYS a common "c", which may have more
  // commons attached to it.  Try to reduce its complexity.
  // "c->deflate()" may return "c" or some simplification of "c".

  COMMON_COMPONENT* dc = c->deflate();
  
  // dc == deflated_common
  // It might be just "c".
  // It might be something else that is simpler but equivalent.
  if (dc->is_trivial()) {
    assert(dynamic_cast<EVAL_BM_VALUE*>(dc));
    // If it is a simple value, don't use a common.
    // Just store the value directly.
    x->obsolete_move_parameters_from_common(dc);
    delete c;
  }else{
    x->attach_common(dc);
    if (dc != c) {
      delete c;
    }else{
    }
  }
  cmd.check(bDANGER, "what's this?");
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_logic_using_obsolete_callback(CS& cmd, COMPONENT* x)
{
  assert(x);
  {
    unsigned here = cmd.cursor();
    int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), x->tail_size(), 0/*start*/);
    cmd.reset(here);
    parse_ports(cmd, x, x->min_nodes(), 0/*start*/, num_nodes, false);
  }
  int incount = x->net_nodes() - x->min_nodes() + 1;
  assert(incount > 0);

  std::string modelname = cmd.ctos(TOKENTERM);

  COMMON_LOGIC* common = 0;
  if      (cmd.umatch("and " )) {untested();common = new LOGIC_AND;}
  else if (cmd.umatch("nand ")) {common = new LOGIC_NAND;}
  else if (cmd.umatch("or "  )) {untested();common = new LOGIC_OR;}
  else if (cmd.umatch("nor " )) {common = new LOGIC_NOR;}
  else if (cmd.umatch("xor " )) {untested();common = new LOGIC_XOR;}
  else if (cmd.umatch("xnor ")) {untested();common = new LOGIC_XNOR;}
  else if (cmd.umatch("inv " )) {common = new LOGIC_INV;}
  else {itested();
    cmd.warn(bWARNING,"need and,nand,or,nor,xor,xnor,inv");
    common=new LOGIC_NONE;
  }
  
  assert(common);
  common->incount = incount;
  common->set_modelname(modelname);
  x->attach_common(common);
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_type(CS& cmd, CARD* x)
{
  assert(x);
  std::string new_type;
  cmd >> new_type;
  x->set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_args(CS& cmd, CARD* x)
{
  assert(x);
  COMPONENT* xx = dynamic_cast<COMPONENT*>(x);

  cmd >> "params:";	// optional, skip it.

  if (!x->use_obsolete_callback_parse()) {
    int paren = cmd.skip1b('(');
    if (xx && cmd.is_float()) {		// simple unnamed value
      std::string value;
      cmd >> value;
      x->set_param_by_name(xx->value_name(), value);
    }else if (cmd.match1("'{")) {	// quoted unnamed value
      std::string value;
      cmd >> value; // strips off the quotes
      value = '{' + value + '}'; // put them back
      x->set_param_by_name(xx->value_name(), value);
    }else{				// only name=value pairs
    }
    unsigned here = cmd.cursor();
    for (int i=0; ; ++i) {
      if (paren && cmd.skip1b(')')) {
	break;
      }else if (!cmd.more()) {
	break;
      }else{
	std::string Name  = cmd.ctos("=", "", "");
	cmd >> '=';
	std::string value = cmd.ctos(",=;)", "\"'{(", "\"'})");
	unsigned there = here;
	if (cmd.stuck(&here)) {untested();
	  break;
	}else{
	  try{
	    if (value == "") {
	      cmd.warn(bDANGER, there, x->long_label() + ": " + Name + " has no value?");
	    }else{
	    }
	    x->set_param_by_name(Name, value);
	  }catch (Exception_No_Match&) {itested();
	    cmd.warn(bDANGER, there, x->long_label() + ": bad parameter " + Name + " ignored");
	  }
	}
      }
    }
  }else if (MODEL_CARD* pp = dynamic_cast<MODEL_CARD*>(x)) {
    // used only for "table"
    int paren = cmd.skip1b('(');
    bool in_error = false;
    for (;;) {
      unsigned here = cmd.cursor();
      pp->parse_params_obsolete_callback(cmd);  //BUG//callback//
      if (!cmd.more()) {
	break;
      }else if (paren && cmd.skip1b(')')) {untested();
	break;
      }else if (cmd.stuck(&here)) {untested();
	if (in_error) {untested();
	  // so you don't get two errors on name = value.
	  cmd.skiparg();
	  in_error = false;
	}else{untested();
	  cmd.warn(bDANGER, "bad paramerter -- ignored");
	  cmd.skiparg().skip1b("=");
	  in_error = true;
	}
      }else{
	in_error = false;
      }
    }
  }else{untested();
    // using obsolete_callback
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_label(CS& cmd, CARD* x)
{
  assert(x);
  std::string my_name;
  cmd >> my_name;
  x->set_label(my_name);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_SPICE_BASE::parse_comment(CS& cmd, DEV_COMMENT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_SPICE_BASE::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset();
  skip_pre_stuff(cmd);

  unsigned here = cmd.cursor();

  std::string s;
  cmd >> s;
  cmd.reset(here);
  if (!command_dispatcher[s]) {
    cmd.skip();
    ++here;
  }else{
  }
  CMD::cmdproc(cmd, scope);

  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
MODEL_CARD* LANG_SPICE_BASE::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset();
  cmd >> ".model ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  parse_args(cmd, x);
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT* LANG_SPICE_BASE::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  assert(x);

  // header
  cmd.reset();
  (cmd >> ".subckt |.macro ");
  parse_label(cmd, x);
  {
    unsigned here = cmd.cursor();
    int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), 
				0/*no unnamed par*/, 0/*start*/);
    cmd.reset(here);
    parse_ports(cmd, x, x->min_nodes(), 0/*start*/, num_nodes, true/*all new*/);
  }
  x->subckt()->params()->parse(cmd);

  // body
  parse_module_body(cmd, x, x->subckt(), name() + "-subckt>", NO_EXIT_ON_BLANK, ".ends |.eom ");
  return x;
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_module_body(CS& cmd, MODEL_SUBCKT* x, CARD_LIST* Scope,
		const std::string& prompt, EOB exit_on_blank, const std::string& exit_key)
{
  try {
    for (;;) {
      cmd.get_line(prompt);
      
      if ((exit_on_blank==EXIT_ON_BLANK && cmd.is_end()) 
	  || cmd.umatch(exit_key)) {
	break;
      }else{
	skip_pre_stuff(cmd);
	new__instance(cmd, x, Scope);
      }
    }
  }catch (Exception_End_Of_Input& e) {
  }
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_SPICE_BASE::parse_instance(CS& cmd, COMPONENT* x)
{
  try {
    assert(x);
    cmd.reset().umatch(ANTI_COMMENT);
    
    // ACS dot type specifier
    if (cmd.skip1b('.')) {
      parse_type(cmd, x);
    }else{
    }
    
    parse_label(cmd, x);
    
    if (x->use_obsolete_callback_parse()) {
      parse_element_using_obsolete_callback(cmd, x);
    }else if (DEV_LOGIC* xx = dynamic_cast<DEV_LOGIC*>(x)) {
      parse_logic_using_obsolete_callback(cmd, xx);
    }else{
      {
	unsigned here = cmd.cursor();
	int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), x->tail_size(), 0);
	cmd.reset(here);
	parse_ports(cmd, x, x->min_nodes(), 0/*start*/, num_nodes, false);
      }
      if (x->print_type_in_spice()) {
	parse_type(cmd, x);
      }else{
      }
      parse_args(cmd, x);
    }
  }catch (Exception& e) {
    cmd.warn(bDANGER, e.message());
  }
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_SPICE_BASE::find_type_in_string(CS& cmd)
{
  cmd.umatch(ANTI_COMMENT); /* skip mark so spice ignores but gnucap reads */

  unsigned here = cmd.cursor();
  std::string s;
  char id_letter = cmd.peek();
  if (OPT::case_insensitive) {
    id_letter = static_cast<char>(toupper(id_letter));
  }else{
  }
  switch (id_letter) {
  case '\0':untested();
    s = "";
    break;
  case '.':
    cmd >> s;
    cmd.reset(here);
    if (!command_dispatcher[s]) {
      cmd.skip();
      ++here;
      s = s.substr(1);
    }else{
    }
    break;
  case 'G':
    here = cmd.cursor();

    if (cmd.scan("vccap |vcg |vcr |vccs ")) {
      s = cmd.trimmed_last_match();
    }else{
      s = "G";
    }
    break;
  default:
    s = id_letter;
    break;
  }
  cmd.reset(here);
  return s;
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  if (0 && cmd.is_file()
      && cmd.is_first_read()
      && (Scope == &CARD_LIST::card_list)
      && (Scope->is_empty())
      && (head == "'")) {untested();	//BUG// ugly hack
    cmd.get_line("gnucap-spice-title>");
    head = cmd.fullstring();
    IO::mstdout << head << '\n';
  }else{itested();
    cmd.get_line("gnucap-spice>");
    new__instance(cmd, NULL, Scope);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static char fix_case(char c)
{
  return ((OPT::case_insensitive) ? (static_cast<char>(tolower(c))) : (c));
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  o << ".model " << x->short_label() << ' ' << x->dev_type() << " (";
  print_args(o, x);
  o << ")\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_module(OMSTREAM& o, const MODEL_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  o << ".subckt " <<  x->short_label();
  print_ports(o, x);
  o << '\n';
  
  for (CARD_LIST::const_iterator 
	 ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }
  
  o << ".ends " << x->short_label() << "\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  print_label(o, x);
  print_ports(o, x);
  print_type(o, x);
  print_args(o, x);
  o << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{
  assert(x);
  if (x->comment()[1] != '+') {
    o << x->comment() << '\n';
  }else{
  }
  // Suppress printing of comment lines starting with "*+".
  // These are generated as a way to display calculated values.
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_command(OMSTREAM& o, const DEV_DOT* x)
{
  assert(x);
  o << x->s() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_args(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= x->param_count_dont_print();  --ii) {
      if (x->param_is_printable(ii)) {
	std::string arg = " " + x->param_name(ii) + "=" + x->param_value(ii);
	o << arg;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_type(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  if (x->print_type_in_spice()) {
    o << "  " << x->dev_type();
  }else if (fix_case(x->short_label()[0]) != fix_case(x->id_letter())) {itested();
    o << "  " << x->dev_type();
  }else{
    // don't print type
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_args(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << ' ';
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= x->param_count_dont_print();  --ii) {
      if (x->param_is_printable(ii)) {
	if ((ii != x->param_count() - 1) || (x->param_name(ii) != x->value_name())) {
	  // skip name if plain value
	  o << " " << x->param_name(ii) << "=";
	}else{
	}
	o << x->param_value(ii);
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_label(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  std::string label = x->short_label();
  o << label;
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_ports(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);

  o <<  " ( ";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_value(ii);
    sep = " ";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_value(ii);
    sep = " ";
  }
  o << " )";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CMD_MODEL : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    // already got "model"
    std::string my_name, base_name;
    cmd >> my_name;
    unsigned here1 = cmd.cursor();    
    cmd >> base_name;
    
    // "level" kluge ....
    // if there is a "level" keyword, with integer argument,
    // tack that onto the given modelname and look for that
    cmd.skip1b('(');
    int level = 0;
    {
      unsigned here = cmd.cursor();
      scan_get(cmd, "level ", &level);
      if (!cmd.stuck(&here)) {
	char buf[20];
	sprintf(buf, "%u", level);
	base_name += buf;
      }else{
      }
    }

    const MODEL_CARD* p = model_dispatcher[base_name];

    if (p) {
      MODEL_CARD* new_card = dynamic_cast<MODEL_CARD*>(p->clone());
      if (exists(new_card)) {
	assert(!new_card->owner());
	lang_spice.parse_paramset(cmd, new_card);
	Scope->push_back(new_card);
      }else{untested();
	cmd.warn(bDANGER, here1, "model: base has incorrect type");
      }
    }else{
      cmd.warn(bDANGER, here1, "model: \"" + base_name + "\" no match");
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, ".model", &p1);
/*--------------------------------------------------------------------------*/
class CMD_SUBCKT : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    MODEL_SUBCKT* new_module = new MODEL_SUBCKT;
    assert(new_module);
    assert(!new_module->owner());
    assert(new_module->subckt());
    assert(new_module->subckt()->is_empty());
    lang_spice.parse_module(cmd, new_module);
    Scope->push_back(new_module);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, ".subckt|.macro", &p2);
/*--------------------------------------------------------------------------*/
enum Skip_Header {NO_HEADER, SKIP_HEADER};
/*--------------------------------------------------------------------------*/
/* getmerge: actually do the work for "get", "merge", etc.
 */
static void getmerge(CS& cmd, Skip_Header skip_header, CARD_LIST* Scope)
{
  ::status.get.reset().start();
  assert(Scope);

  std::string file_name, section_name;
  cmd >> file_name;
  
  bool  echoon = false;		/* echo on/off flag (echo as read from file) */
  bool  liston = false;		/* list on/off flag (list actual values) */
  bool  quiet = false;		/* don't echo title */
  unsigned here = cmd.cursor();
  do{
    ONE_OF
      || Get(cmd, "echo",  &echoon)
      || Get(cmd, "list",  &liston)
      || Get(cmd, "quiet", &quiet)
      || Get(cmd, "section", &section_name)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  if (cmd.more()) {
    cmd >> section_name;
  }else{
  }
  cmd.check(bWARNING, "need section, echo, list, or quiet");

  CS file(CS::_INC_FILE, file_name);

  if (skip_header) { // get and store the header line
    file.get_line(">>>>");
    head = file.fullstring();

    if (!quiet) {
      IO::mstdout << head << '\n';
    }else{untested();
    }
  }else{
  }
  if (section_name == "") {
    lang_spice.parse_module_body(file, NULL, Scope, ">>>>", lang_spice.NO_EXIT_ON_BLANK, ".end ");
  }else{
    try {
      for (;;) {
	file.get_line("lib " + section_name + '>');
	if (file.umatch(".lib " + section_name + ' ')) {
	  lang_spice.parse_module_body(file, NULL, Scope, section_name,
			lang_spice.NO_EXIT_ON_BLANK, ".endl {" + section_name + "}");
	}else{
	  // skip it
	}
      }
    }catch (Exception_End_Of_Input& e) {
    }
  }
  ::status.get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_lib: lib command
 */
class CMD_LIB : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    unsigned here = cmd.cursor();
    std::string section_name, more_stuff;
    cmd >> section_name >> more_stuff;
    if (more_stuff != "") {
      cmd.reset(here);
      getmerge(cmd, NO_HEADER, Scope);
    }else{
      for (;;) {
	cmd.get_line(section_name + '>');
	if (cmd.umatch(".endl {" + section_name + "}")) {
	  break;
	}else{
	  // skip it
	}
      }
    }
  }
} p33;
DISPATCHER<CMD>::INSTALL d33(&command_dispatcher, ".lib|lib", &p33);
/*--------------------------------------------------------------------------*/
/* cmd_include: include command
 * as get or run, but do not clear first, inherit the run-mode.
 */
class CMD_INCLUDE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    getmerge(cmd, NO_HEADER, Scope);
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, ".include", &p3);
/*--------------------------------------------------------------------------*/
/* cmd_merge: merge command
 * as get, but do not clear first
 */
class CMD_MERGE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {untested();
    SET_RUN_MODE xx(rPRESET);
    getmerge(cmd, NO_HEADER, Scope);
  }
} p4;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, ".merge|merge", &p4);
/*--------------------------------------------------------------------------*/
/* cmd_run: "<" and "<<" commands
 * run in batch mode.  Spice format.
 * "<<" clears old circuit first, "<" does not
 * get circuit from file, execute dot cards in sequence
 */
class CMD_RUN : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    while (cmd.match1('<')) {untested();
      command("clear", Scope);
      cmd.skip();
      cmd.skipbl();
    }
    SET_RUN_MODE xx(rSCRIPT);
    getmerge(cmd, SKIP_HEADER, Scope);
  }
} p5;
DISPATCHER<CMD>::INSTALL d5(&command_dispatcher, "<", &p5);
/*--------------------------------------------------------------------------*/
/* cmd_get: get command
 * get circuit from a file, after clearing the old one
 * preset, but do not execute "dot cards"
 */
class CMD_GET : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    SET_RUN_MODE xx(rPRESET);
    command("clear", Scope);
    getmerge(cmd, SKIP_HEADER, Scope);
  }
} p6;
DISPATCHER<CMD>::INSTALL d6(&command_dispatcher, ".get|get", &p6);
/*--------------------------------------------------------------------------*/
/* cmd_build: build command
 * get circuit description direct from keyboard (or stdin if redirected)
 * Command syntax: build <before>
 * Bare command: add to end of list
 * If there is an arg: add before that element
 * null line exits this mode
 * preset, but do not execute "dot cards"
 */
class CMD_BUILD : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    SET_RUN_MODE xx(rPRESET);
    ::status.get.reset().start();
    lang_spice.parse_module_body(cmd, NULL, Scope, ">", lang_spice.EXIT_ON_BLANK, ". ");
    ::status.get.stop();
  }
} p7;
DISPATCHER<CMD>::INSTALL d7(&command_dispatcher, ".build|build", &p7);
/*--------------------------------------------------------------------------*/
class CMD_SPICE : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=spice", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "spice", &p8);
/*--------------------------------------------------------------------------*/
class CMD_ACS : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=acs", Scope);
  }
} p9;
DISPATCHER<CMD>::INSTALL d9(&command_dispatcher, "acs", &p9);
/*--------------------------------------------------------------------------*/
class CMD_ENDC : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    if (OPT::language == &lang_acs) {
      command("options lang=spice", Scope);
    }else{
    }
  }
} p88;
DISPATCHER<CMD>::INSTALL d88(&command_dispatcher, ".endc", &p88);
/*--------------------------------------------------------------------------*/
class CMD_CONTROL : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    if (OPT::language == &lang_spice) {
      command("options lang=acs", Scope);
    }else{
    }
  }
} p99;
DISPATCHER<CMD>::INSTALL d99(&command_dispatcher, ".control", &p99);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
