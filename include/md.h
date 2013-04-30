/*$Id: md.h,v 26.112 2009/07/24 00:10:32 al Exp $ -*- C++ -*-
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
 * Machine dependent, configuration, and standard includes
 */
//testing=trivial 2006.07.17
#ifndef MD_H_INCLUDED
#define MD_H_INCLUDED
/*--------------------------------------------------------------------------*/
/* autoconf stuff */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
/*--------------------------------------------------------------------------*/
/* std collection of includes */
// system
#include <new>
#include <cstdarg>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <climits>
#include <limits>
#include <cstdio>
#include <cerrno>
#include <csetjmp>
#include <csignal>
#include <cstring>
#include <iostream>
// types
#include <complex>
#include <string>
// containers
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <valarray>
// algorithms
#include <typeinfo>
#include <algorithm>
/* usual but non-standard (POSIX??) collection of includes */
#include <unistd.h>	/* chdir, access, getcwd */
#include <fcntl.h>	/* old style unix files */
/*--------------------------------------------------------------------------*/
/* constants related to memory size, word size, etc */
enum {
  BUFLEN = 256,
  BIGBUFLEN = 4096
};
/*--------------------------------------------------------------------------*/
/* user interface preferences */
#define I_PROMPT "gnucap> "
#define CKT_PROMPT ">"
#define ANTI_COMMENT "*>"
#define DEFAULT_LANGUAGE "acs"
/*--------------------------------------------------------------------------*/
#if defined(__WIN32__)
#define	ENDDIR		"/\\"
#define PATHSEP		';'
#define SYSTEMSTARTFILE	"gnucap.rc"
#define SYSTEMSTARTPATH	OS::getenv("PATH")
#define USERSTARTFILE	"gnucap.rc"
#define	USERSTARTPATH	OS::getenv("HOME")
#define STEPFILE   	"/tmp/SXXXXXX"
#define SHELL		OS::getenv("COMSPEC")
/*--------------------------------------------------------------------------*/
#else
#define	ENDDIR		"/"
#define PATHSEP		':'
#define SYSTEMSTARTFILE	"gnucap.rc"
#define SYSTEMSTARTPATH	OS::getenv("PATH")
#define USERSTARTFILE	".gnucaprc"
#define	USERSTARTPATH	OS::getenv("HOME")
#define STEPFILE   	"/tmp/SXXXXXX"
#define SHELL		OS::getenv("SHELL")
#endif
/*--------------------------------------------------------------------------*/
/* machine and compiler patches */
#if defined(__MINGW32__)
  #define SIGSETJMP_IS_BROKEN
  #define MS_DLL
#endif
/*--------------------------------------------------------------------------*/
/* some convenient names */
typedef std::complex<double> COMPLEX;
typedef std::pair<double,double> DPAIR;
/*--------------------------------------------------------------------------*/
// dynamic cast kluge.
// Strictly, this should always be dynamic_cast, but if it has already
// been checked, don't bother checking again, hence static_cast.
// It works and is faster.
#if defined(NDEBUG)
  #define prechecked_cast static_cast
#else
  #define prechecked_cast dynamic_cast
#endif

/*--------------------------------------------------------------------------*/
/* portability hacks */

#if !defined(MS_DLL)
  // The usual way for POSIX compliant systems
  #include <dlfcn.h>
  #define INTERFACE
#else
// Microsoft DLL hacks -- thanks to Holger Vogt and Cesar Strauss for the info
// Make the MS DLL functions look like the posix ones.
#include <windows.h>
#undef min
#undef max
#undef INTERFACE
  #ifdef MAKE_DLL
    #define INTERFACE __declspec(dllimport)
  #else
    #define INTERFACE __declspec(dllexport)
  #endif

inline void* dlopen(const char* f, int)
{
  return LoadLibrary(const_cast<char*>(f));
}

inline void dlclose(void* h)
{
  FreeLibrary((HINSTANCE)h);
}

inline char* dlerror()
{
  static LPVOID lpMsgBuf = NULL;
  // free the error message buffer
  if (lpMsgBuf) {
    LocalFree(lpMsgBuf);
  }
  // get the error code
  DWORD dw = GetLastError();
  // get the corresponding error message
  FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL);
  return (char*)lpMsgBuf;
}
#define RTLD_LAZY       0x00001 /* Lazy function call binding.  */
#define RTLD_NOW        0x00002 /* Immediate function call binding.  */
#define RTLD_BINDING_MASK   0x3 /* Mask of binding time value.  */
#define RTLD_NOLOAD     0x00004 /* Do not load the object.  */
#define RTLD_DEEPBIND   0x00008 /* Use deep binding.  */
#define RTLD_GLOBAL     0x00100
#define RTLD_LOCAL      0
#define RTLD_NODELETE   0x01000
#endif

#if defined(SIGSETJMP_IS_BROKEN)
  #undef sigjmp_buf
  #undef siglongjmp
  #undef sigsetjmp
  #define sigjmp_buf jmp_buf
  #define siglongjmp(a,b) longjmp(a,b)
  #define sigsetjmp(a,b) setjmp(a)
#endif

#if !defined(SIGNALARGS)
  #define SIGNALARGS int
#endif
/*--------------------------------------------------------------------------*/
/* temporary hacks */
enum RUN_MODE {
  rPRE_MAIN,	/* it hasn't got to main yet			*/
  rPRESET,	/* do set up commands now, but not simulation	*/
		/* store parameters, so bare invocation of a	*/
		/* simulation command will do it this way.	*/
  rINTERACTIVE,	/* run the commands, interactively		*/
  rSCRIPT,	/* execute now, as a command, then restore mode	*/
  rBATCH	/* execute now, as a command, then exit		*/
};
class INTERFACE ENV {
public:
  static RUN_MODE run_mode; // variations on handling of dot commands
};
/*--------------------------------------------------------------------------*/
/* my standard collection of includes */
#include "io_trace.h"
#include "io_error.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
