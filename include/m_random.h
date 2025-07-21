/*                        -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
 * Author: Felix Salfelder
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
 * Random number sampling
 */
#ifndef GNUCAP_RDIST_VAMS_H
#define GNUCAP_RDIST_VAMS_H
/*--------------------------------------------------------------------------*/
#include "md.h"
#include "l_indirect.h"
/*--------------------------------------------------------------------------*/
extern INDIRECT<int32_t, int32_t> random_seeds;
int32_t& random_seed(int32_t slot);
/*--------------------------------------------------------------------------*/
// Verilog AMS $rdist functions
namespace rdist {
/*--------------------------------------------------------------------------*/
double uniform( int32_t& seed, double start, double end );
double normal(int32_t& seed, double mean, double deviation);
double exponential(int32_t& seed, double mean);
double poisson(int32_t& seed, double mean);
double chi_square(int32_t& seed, int deg_of_free);
double t(int32_t& seed, int deg_of_free);
double erlangian(int32_t& seed, double k, double mean);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
