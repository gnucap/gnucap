/*$Id: s_tr_rev.cc,v 24.20 2004/01/18 07:42:51 al Exp $ -*- C++ -*-
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
 * review the solution after solution at a time point
 * Set up events, evaluate logic inputs, truncation error.
 * Recommend adjusted step size. (approxtime)
 * and say why (control)
 */
#include "u_opt.h"
#include "e_cardlist.h"
#include "u_status.h"
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::review(void);
/*--------------------------------------------------------------------------*/
void TRANSIENT::review(void)
{
  static double rtime;	/* next time by iteration count and smoothing */
 
  STATUS::review.start();
  ++STATUS::iter[iTOTAL];
  {if (phase == pINIT_DC){
    rtime = dtmax/ 100.;	/* set (guess) initial internal step */
    rtime = time0 + std::max(rtime, dtmin);
    control = scITER_A;
  }else{
    double rdt = rtime - time1;/* review dt recommended by PREVIOUS review */
    double adt = time0 - time1;/* actual dt most recently used */
    if (adt > rdt + dtmin){
      error(bDANGER,"internal error: step control (adt=%g,rdt=%g)\n",adt,rdt);
      error(bERROR, "time0=%g  time1=%g  rtime=%g\n", time0, time1, rtime);
    }

    {if (STATUS::iter[iSTEP] > OPT::itl[OPT::TRHIGH]){/* too many iterations */
      rtime = time1 + adt / OPT::trstepshrink;  /* try again, smaller step */
      control = scITER_R;
    }else if (STATUS::iter[iSTEP] > OPT::itl[OPT::TRLOW]){
      rtime = time0 + adt;			/* no growth */
      control = scITER_A;
    }else{					/* too few iterations */
      rtime = time0 + dtmax;			/* ok to use bigger steps */
      control = scSKIP;
      if (rtime > time0 + adt*OPT::trstepgrow  &&  rtime > time0 + rdt){
	{if (rdt > adt * OPT::trstepgrow){
	  rtime = time0 + rdt;
	}else{					/* limit to max(rdt,adt*2) */
	  rtime = time0 + adt * OPT::trstepgrow;
	}}
	control = scADT;
      }
      if (rtime > time0 + rdt * OPT::trstepgrow){
	rtime = time0 + rdt * OPT::trstepgrow;	/* limit to rdt*2 */
	control = scRDT;
      }
    }}
  }}
  double tetime = CARD_LIST::card_list.tr_review();	/* trunc error, etc. */
  {if (tetime < rtime){
    control = scTE;
    approxtime = tetime;
  }else{
    approxtime = rtime;
  }}
  STATUS::review.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
