/*$Id: s_tr_rev.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * review the solution after solution at a time point
 * Set up events, evaluate logic inputs, truncation error.
 * Recommend adjusted step size. (time_suggested_by_review)
 * and say why (control)
 */
//testing=script,complete 2006.06.14
#include "e_cardlist.h"
#include "u_status.h"
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::review(void);
/*--------------------------------------------------------------------------*/
bool TRANSIENT::review(void)
{
  ::status.review.start();
  count_iterations(iTOTAL);

  DPAIR times = CARD_LIST::card_list.tr_review();
  time_by_error_estimate = times.first;
  time_by_ambiguous_event = times.second;

  time_suggested_by_review = std::min(times.first, times.second);
  control = scTE;

  ::status.review.stop();

  return (time_suggested_by_review > time0);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
