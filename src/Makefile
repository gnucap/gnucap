#$Id: Makefile,v 22.21 2002/10/06 07:21:50 al Exp $
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
# In most cases you can just type "make" to build it.
# Generally, you will want "make your-system".
# Look in this file for the choices.
# If yours is not here, look for a similar one and copy.
# The special configurations listed here allow you to do multiple builds
# from the same source directory, and patch some problems.
#-----------------------------------------------------------------------------
# The default is to use the makefile built by "configure"
default: nothing
	(cd O; ${MAKE} -k)
#-----------------------------------------------------------------------------
# The most common configuration is g++
# This should work if it is properly installed
# and has the proper libraries and headers.
# It is optimized for speed.  Debugging is off.
g++: nothing
	-mkdir O
	cat Make1 Make2.g++ Make3 Make.depend >O/Makefile
	(cd O; ${MAKE} -k)
#-----------------------------------------------------------------------------
# This one makes a "debug" build ...
# Asserts and some tracing is turned on.
# It prints a trace when "untested" code is exercised.
# "Untested" means the regressions don't test it.
# It doesn't mean TOTALLY untested.
debug: nothing
	-mkdir O-DEBUG
	cat Make1 Make2.Debug Make3 Make.depend >O-DEBUG/Makefile
	(cd O-DEBUG; ${MAKE} -k)
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
# Windows using Borland 5.5
borland: nothing
	-mkdir BORLAND
	copy Make1 + Make2.Borland + Make3 + Make.depend  BORLAND\Makefile.raw
	cd BORLAND
	sed 's/\\.o/.obj/g' <Makefile.raw >Makefile
	${MAKE}
#-----------------------------------------------------------------------------
# If you don't have g++, try this one
# This is based on assumptions that you have a cfront based compiler called CC
# and there is nothing strange about your system
CC: nothing
	-mkdir O-CC
	cat Make1 Make2.CC Make3 Make.depend >O-CC/Makefile
	(cd O-CC; ${MAKE} -k)
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
linux: nothing
	-mkdir LINUX
	cat Make1 Make2.g++ Make3 Make.depend >LINUX/Makefile
	(cd LINUX; ${MAKE} -k)
#-----------------------------------------------------------------------------
netbsd: nothing
	-mkdir NETBSD
	cat Make1 Make2.g++ Make3 Make.depend >NETBSD/Makefile
	(cd NETBSD; ${MAKE} -k)
#-----------------------------------------------------------------------------
next: nothing
	-mkdir NEXT
	cat Make1 Make2.g++ Make3 Make.depend >NEXT/Makefile
	(cd NEXT; ${MAKE} -k)
#-----------------------------------------------------------------------------
sun4-g++: nothing
	-mkdir SUN4-G++
	cat Make1 Make2.g++ Make3 Make.depend >SUN4-G++/Makefile
	(cd SUN4-G++; ${MAKE} -k)
#-----------------------------------------------------------------------------
sun4-CC: nothing
	-mkdir SUN4-CC
	cat Make1 Make2.sun-CC Make3 Make.depend >SUN4-CC/Makefile
	(cd SUN4-CC; ${MAKE} -k)
#-----------------------------------------------------------------------------
freebsd: nothing
	-mkdir FREEBSD
	cat Make1 Make2.g++ Make3 Make.depend >FREEBSD/Makefile
	(cd FREEBSD; ${MAKE} -k)
#-----------------------------------------------------------------------------
tags: nothing
	cat Make1 Make2.g++ Make3 >Make.aux
	(${MAKE} tags -f Make.aux)
#-----------------------------------------------------------------------------
depend: nothing
	cat Make1 Make2.g++ Make3 >Make.aux
	${MAKE} -f Make.aux depend
#-----------------------------------------------------------------------------
checkin: nothing
	cat Make1 Make2.g++ Make3 >Make.aux
	${MAKE} -f Make.aux checkin
#-----------------------------------------------------------------------------
checkout: nothing
	cat Make1 Make2.g++ Make3 >Make.aux
	${MAKE} -f Make.aux checkout
#-----------------------------------------------------------------------------
clean: nothing
	-rm */*.o */*.obj */*.h */*.cc
#-----------------------------------------------------------------------------
install: nothing
	cp O/gnucap /usr/local/bin
#-----------------------------------------------------------------------------
nothing:
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
