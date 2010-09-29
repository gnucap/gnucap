#$Id: Make2.CC,v 11.22 1996/02/18 11:41:21 al Exp $ -*- Makefile -*-
# Copyright (C) 2001 Albert Davis
# Author: Albert Davis <aldavis@ieee.org>
#
# This file is part of "GnuCap", the Gnu Circuit Analysis Package
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#------------------------------------------------------------------------
VPATH = .:..

# starting point for cfront based "CC" compilers

CCC = CC
CCFLAGS = -DNDEBUG -DUNIX -O
LIBS = -lm
LDFLAGS =

.SUFFIXES : .o .cc
.cc.o:; $(CCC) $(CCFLAGS) -c $<
#------------------------------------------------------------------------
$(TARGET): $(OBJS)
	-rm $@ >/dev/null 2>&1
	$(CCC) $(CCFLAGS) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)
#------------------------------------------------------------
# If you are porting and using a cfront based compiler,
# you should use this file as a start.
# If you get errors or warnings, make a copy of it and modify that.
# Warnings are turned off here because many of the supplied headers
# make their own warnings.
# If the port is non-trivial, check with me first because someone else
# may have already done it.
# If it works without changes, send me a note so I can put it in the docs.
# Please send the changes to aldavis@ieee.org
#------------------------------------------------------------
#------------------------------------------------------------
