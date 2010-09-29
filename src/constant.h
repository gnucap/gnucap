/*$Id: constant.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * defined constants for just about everything
 */
#ifndef CONSTANT_H
#define CONSTANT_H
#include "md.h"
/*--------------------------------------------------------------------------*/
const double kE    = exp(1.0);
const double kRT2  = 1.414213562;
const double kPIx2 = 6.2831853071795864769252867665590057683944;
const double kPI   = 3.1415926535897932384626433832795028841972;
const double kPId2 = 1.5707963267948966192313216916397514420986;
const double DTOR  = 0.0174532925199432957692369076848861271344;
const double RTOD  = 57.2957795130823208768;
const double ONE_OVER_PI= 0.3183098861837906715377675267450287240689;
const double ABS_ZERO = -273.15;
const double E_0   = 8.854214871e-12;	/* permittivity of air 		  */
const double E_SI  = 11.7*E_0;	   /* permittivity of silicon (1.0359e-10)*/
const double E_OX  = 3.9*E_0;	   /* permittivity of oxide (3.45e-11)	  */
const double K	   = 1.3806226e-23;	/* Boltzmann's constant		  */
const double Q	   = 1.6021918e-19;	/* electronic charge		  */
const double KoQ   = K/Q;

/* dimension conversions.
 * Hopefully, all internal distances are in meters.
 * (except for some Berkeley models)
 * In some cases the user data is in other units
 */
const double CM2M  = 1e-2;		/*	centimeters to meters 	 */
const double CM2M2 = 1e-4;		/*          ...........  squared */
const double ICM2M = 1e2;		/* inverse  ...........    	 */
const double ICM2M2 = 1e4;		/* inverse  ...........  squared */
const double ICM2M3 = 1e6;		/* inverse  ...........  cubed   */
const double MICRON2METER = 1e-6;	/*	microns to meters	 */

const double BIGBIG    =  (DBL_MAX)*(.9247958);	/* unlikely number	  */
const double OVERDUE   = -(DBL_MAX)*(.9347958);	/* unlikely number	  */
const double NEVER     =  (DBL_MAX)*(.9447958);	/* unlikely number	  */
const double NOT_INPUT = -(DBL_MAX)*(.9547658);	/* unlikely number	  */
const double NOT_VALID = -(DBL_MAX)*(.9647958);	/* unlikely number	  */
const double LINEAR    = -(DBL_MAX)*(.9747958);	/* unlikely number	  */
const double VOLTMIN   =  1.0e-50;
const double PWRMIN    =  1.0e-100;

const char TOKENTERM[] = ",=()[]";

enum {FILE_OK=0, FILE_BAD=-1};
enum TRI_STATE {tsNO=0, tsYES=1, tsBAD=-1};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
