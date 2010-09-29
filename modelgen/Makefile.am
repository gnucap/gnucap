# -*- Automake -*-
#
# $Id: Makefile.am,v 25.96 2006/08/28 05:45:36 al Exp $
#
# Process this file with autoconf to produce a configure script.
#
#
#                            COPYRIGHT
#
# This file is part of "Gnucap", the Gnu Circuit Analysis Package
#
#  Copyright (C) 2005 Dan McMahill
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

bin_PROGRAMS= gnucap-modelgen

gnucap_modelgen_SOURCES= \
	ap_construct.cc ap_convert.cc  ap_get.cc ap.h \
	ap_match.cc ap_skip.cc io_error.h io_.h io_trace.h l_lib.h md.h \
	mg_error.cc mg_.h mg_in.cc mg_main.cc mg_out_common.cc \
	mg_out_dev.cc mg_out.h mg_out_h.cc mg_out_lib.cc mg_out_model.cc \
	mg_out_root.cc patchlev.h 

EXTRA_DIST= \
 Make1 Make3 Make.depend Make2.g++ Make2.Debug Make2.mingw32 \
 configure.old Makefile.template


INCLUDES=	-I$(top_srcdir)/src

