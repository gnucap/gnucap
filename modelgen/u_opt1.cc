/*$Id: u_opt1.cc,v 26.80 2008/05/27 02:18:47 al Exp $ -*- C++ -*-
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
 * all the options set by the .options card.
 * initialization and declaration of statics
 */
//testing=trivial 2006.07.17
#include "u_opt.h"

bool	OPT::acct = false;
bool	OPT::listing = false;
bool	OPT::mod = true;
bool	OPT::page = false;
bool	OPT::node = false;
bool	OPT::opts = false;
double	OPT::gmin = 1e-12;
double	OPT::reltol = .001;
double	OPT::abstol = 1e-12;
double	OPT::vntol = 1e-6;
double	OPT::trtol = 7.0;
double	OPT::chgtol = 1e-14;
double	OPT::pivtol = 1e-13;
double	OPT::pivrel = 1e-3;
int	OPT::numdgt = 5;
double	OPT::tnom_c = 27.0;
int	OPT::cptime = 30000;
int	OPT::limtim = 2;
int	OPT::limpts = 201;
int	OPT::lvlcod = 2;
int	OPT::lvltim = 2;
method_t OPT::method = meTRAP;
int	OPT::maxord = 2;
double	OPT::defl = 100e-6;
double	OPT::defw = 100e-6;
double	OPT::defad = 0.;
double	OPT::defas = 0.;

bool	OPT::clobber = true;
bool	OPT::keys_between_nodes = true;
double	OPT::dampmax = 1.0;
double	OPT::dampmin = 0.5;
int	OPT::dampstrategy = 0; //dsINIT|dsDEVREGION|dsREVERSE;
double	OPT::floor = 1e-21;
double	OPT::vfloor = 1e-15;
double	OPT::roundofftol = 1e-13;
double	OPT::temp_c = 27.0;
double	OPT::shortckt = 10e-6;
int	OPT::picky = bPICKY;
unsigned OPT::outwidth = 9999;
double	OPT::ydivisions = 4.;
phase_t	OPT::phase = pDEGREES;
order_t	OPT::order = oAUTO;
smode_t	OPT::mode = moMIXED;
int	OPT::transits = 2;
bool	OPT::dupcheck = false;
bool	OPT::bypass = true;
bool	OPT::incmode = true;
bool	OPT::lcbypass = true;
bool	OPT::lubypass = true;
bool	OPT::fbbypass = true;
bool	OPT::traceload = true;
int	OPT::itermin = 1;
double	OPT::vmax =  5;
double	OPT::vmin = -5;
double	OPT::dtmin = 1e-12;
double	OPT::dtratio = 1e9;
bool	OPT::rstray = true;
bool	OPT::cstray = true;
int	OPT::harmonics = 9;
double	OPT::trstepgrow = 1e99;
double	OPT::trstephold = 1e99;
double	OPT::trstepshrink = 2.;		/* spice is fixed at 8 */
double	OPT::trreject = .5;
int	OPT::trsteporder = 3;
double	OPT::trstepcoef[_keep_time_steps] = {1., 1./4., 1./24., 1./192.};

bool	OPT::showall = false;
int	OPT::foooo = 0;
int	OPT::diodeflags = 0;
int	OPT::mosflags = 0;
bool	OPT::quitconvfail = false;
bool	OPT::edit = true;
int	OPT::recursion = 20;
LANGUAGE* OPT::language = NULL;
bool	OPT::case_insensitive = false;
UNITS	OPT::units = uSI;
double	OPT::lowlim = 1. - OPT::reltol;
double	OPT::uplim = 1. + OPT::reltol;

int	OPT::itl[OPT::ITL_COUNT] = { 
		100,	/* 0=dummy */
		100,	/* 1=dc (bias) iteration limit */
		50, 	/* 2=dc transfer iteration limit */
		6,	/* 3=lower transient iteration limit (spice is 4) */
		20,	/* 4=upper transient iteration limit (spice is 10) */
		5000,	/* 5=transient total iterations allowed */
		0,	/* 6=source stepping iteration limit */
		1,	/* 7=worst case iteration limit */
		99	/* 8=trace nonconvergence start iteration */
};

#ifdef KNEECHORD
strategy_t OPT::strategy = stNEWTON; /* Default is Newton's Method, fastest */
#endif
