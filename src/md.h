/*$Id: md.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * Machine dependent, configuration, and standard includes
 */
#ifndef MD_H_INCLUDED
#define MD_H_INCLUDED
/*--------------------------------------------------------------------------*/
/* system dependent patches needed before the standard includes */
#if defined (WIN32)
  #pragma warning (disable: 4786) // id truncated to 255 characters
  #pragma warning (disable: 4800) // forcing ___ to bool (performance warning)
  #pragma warning (disable: 4660) // template already instantiated
  // lev 4
  #pragma warning (disable: 4514) // unreferenced inline removed
  #pragma warning (disable: 4710) // function not inlined
  #pragma warning (disable: 4512) // assignment op could not be generated
  #pragma warning (disable: 4244) // conversion from int to char (toupper)
  #pragma warning (disable: 4611) // setjmp/destructor interaction nonportable
  #pragma warning (push, 3)
#endif
/*--------------------------------------------------------------------------*/
/* std collection of includes */
// system
#include <new>
#include <cstdarg>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cstdio>
#include <cerrno>
#include <csetjmp>
#include <iostream>
// types
#include <complex>
#include <string>
// containers
#include <list>
#include <vector>
#include <queue>
#include <map>
// algorithms
#include <typeinfo>
#include <algorithm>
/*--------------------------------------------------------------------------*/
/* constants related to memory size, word size, etc */
enum {
  BUFLEN = 256,
  BIGBUFLEN = 2048
};
/*--------------------------------------------------------------------------*/
#if defined(__unix__)
/* usual but non-standard collection of includes */
#include <unistd.h>	  /* chdir, access, getcwd */
#include <sys/time.h>	  /* struct timeval, may be needed by sys/resource */
#include <sys/resource.h> /* getrusage, used for timer  */
/* file names, etc. */
#define	ENDDIR		"/"
#define PATHSEP		':'
#define SYSTEMSTARTFILE	"gnucap.rc"
#define SYSTEMSTARTPATH	OS::getenv("PATH")
#define USERSTARTFILE	".gnucaprc"
#define	USERSTARTPATH	OS::getenv("HOME")
#define STEPFILE   	"/tmp/SXXXXXX"
#define PLOTFILE    	"gnucap.plot"
#define SHELL		OS::getenv("SHELL")
/* machine and compiler patches */
#if defined(__sun__)
  #define NEEDS_IS_INF
#endif
#if defined(__GNUG__)
  #define MANUAL_TEMPLATES
#elif defined(__SUNPRO_CC)
  #include <setjmp.h>
  #define LINK_TEMPLATES
  #define __FUNCTION__ ""
  #define NEW_IS_BROKEN
  #define NEEDS_COPY_N
  #define DBL_MAX_IS_VARIABLE
#else
  #error what compiler are you using??
#endif
/*--------------------------------------------------------------------------*/
#elif defined(__WIN32__)
/* usual but non-standard collection of includes */
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <time.h>
/* file names, etc. */
#define	ENDDIR		"/\\"
#define PATHSEP		';'
#define SYSTEMSTARTFILE	"gnucap.ini"
#define SYSTEMSTARTPATH	OS::getenv("PATH")
#define USERSTARTFILE	"gnucap.ini"
#define	USERSTARTPATH	OS::getenv("HOME")
#define STEPFILE   	"/tmp/SXXXXXX"
#define PLOTFILE    	"gnucap.plot"
#define SHELL		OS::getenv("COMSPEC")
/* machine and compiler patches */
#define MANUAL_TEMPLATES
#define NEEDS_COPY_N
#define NEEDS_GETRUSAGE
#define NEEDS_RINT
#define NEW_IS_BROKEN
#define ABS_IS_BROKEN
#define SIGSETJMP_IS_BROKEN
enum {F_OK=0, X_OK=1, W_OK=2, R_OK=4};
inline int strcasecmp(const char a[], const char b[]){
  return stricmp(a,b);
}
const char __FUNCTION__[] = "";
#if defined(__BORLANDC__)
  #include <setjmp.h>
  typedef int off_t;
  #define NEEDS_IS_INF
  #define NEEDS_IS_NAN
  #define DBL_MAX_IS_VARIABLE
#elif defined(MICROSOFT)
  #define MIN_MAX_IS_BROKEN
  #define CONST_IS_BROKEN
  inline int isinf(double x){
    if (_fpclass(x) == _FPCLASS_PINF){
      return 1;
    }else if (_fpclass(x) == _FPCLASS_NINF){
      return -1;
    }else{
      return 0;
    }
  }
  inline int isnan(double x){
    return _isnan(x);
  }
#else
  #error what compiler are you using??
#endif
/*--------------------------------------------------------------------------*/
#else
#error what kind of system is this??
#endif
/*--------------------------------------------------------------------------*/
/* The various libraries are inconsistent about what is in std.
 * cover up the inconsistency.
 */
//using namespace std;
/*--------------------------------------------------------------------------*/
/* my standard collection of includes */
#include "io_error.h"
#include "io_trace.h"
/*--------------------------------------------------------------------------*/
typedef std::complex<double> COMPLEX;

#if defined(NDEBUG)
  #define prechecked_cast static_cast
#else
  #define prechecked_cast dynamic_cast
#endif

#if defined(NEEDS_COPY_N)
namespace std {
  template <class InputIter, class Size, class OutputIter>
  pair<InputIter, OutputIter> copy_n(InputIter first, Size count,
				       OutputIter result) {
    for ( ; count > 0; --count) {
      *result++ = *first++;
    }
    return pair<InputIter, OutputIter>(first, result);
  }
}
#endif

#if defined(NEEDS_FILL_N)
namespace std {
  template <class OutputIter, class Size, class Type>
  OutputIter fill_n(OutputIter first, Size count, Type value) {
    for ( ; count > 0; --count) {
      *first++ = value;
    }
    return first;
  }
}
#endif

#if defined(HAS_EXP_BUG)
  inline double Exp(double x){untested(); return (x>-200.) ? exp(x) : 0.;}
#else
  inline double Exp(double x){return exp(x);}
#endif

#if defined(NEEDS_IS_INF)
  inline int isinf(double x) {return (x != x);}
#endif

#if defined(NEEDS_IS_NAN)
  inline int isnan(double x) {untested(); return (x != x);}
#endif

#if defined(NEEDS_RINT)
  inline double rint(double x) {untested(); return (floor(x+.5));}
#endif

#if defined(ABS_IS_BROKEN)
  namespace std {
    inline double abs(double x) {return fabs(x);}
  }
#endif

#if defined(NEEDS_GETRUSAGE)
struct	rusage {
  struct timeval ru_utime;	/* user time used */
  struct timeval ru_stime;	/* system time used */
};
int getrusage(int,struct rusage*);
#define RUSAGE_SELF	0
#endif

#if defined(MIN_MAX_IS_BROKEN)
  #undef min
  #undef max
  namespace std {
    template <class T>
    inline const T& min(const T& a, const T& b) {
      return b < a ? b : a;
    }
    template <class T>
    inline const T& max(const T& a, const T& b) {
      return  a < b ? b : a;
    }
  }
#endif

#if defined(DBL_MAX_IS_VARIABLE)
  #undef DBL_MAX
  #define DBL_MAX (1e300)
#endif

#if defined(SIGSETJMP_IS_BROKEN)
  #define sigjmp_buf jmp_buf
  #define siglongjmp(a,b) longjmp(a,b)
  #define sigsetjmp(a,b) setjmp(a)
#endif

#if defined(CONST_IS_BROKEN)
  #undef  CONST
  #define CONST
#else
  #undef  CONST
  #define CONST const
#endif

#if !defined(SIGNALARGS)
  #define SIGNALARGS int
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
