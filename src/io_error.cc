/*$Id: io_error.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * Error handler.
 * Collection of functions to handle all types of errors
 * including user interrupts, system errors, overflow, etc.
 */
//testing=script,sparse 2006.07.17
#include "u_opt.h"
#include "l_jmpbuf.h"
#include "io_error.h"
/*--------------------------------------------------------------------------*/
	void	error(int,const char*,...);
	void	error(int,const std::string&);
/*--------------------------------------------------------------------------*/
extern JMP_BUF env;
/*--------------------------------------------------------------------------*/
/* error: error message printer
 * print it, if severe enough
 * terminate command, if really bad
 */
void error(int badness, const char* fmt, ...)
{
  if (badness >= OPT::picky) {
    char buffer[BIGBUFLEN] = "";
    va_list arg_ptr;
    va_start(arg_ptr,fmt);
    vsprintf(buffer,fmt,arg_ptr);
    va_end(arg_ptr);
    IO::error << buffer;
  }
  if (badness >= bDISASTER) {untested();
    abort();
  }else if (badness >= bEXIT) {itested();
    exit(badness);
  }else if (badness >= bERROR) {itested();
    if (ENV::run_mode == rBATCH) {untested();
      exit(badness);
    }else{itested();
      siglongjmp(env.p,1);
      //throw badness;
    }
  }
}
/*--------------------------------------------------------------------------*/
void error(int badness, const std::string& message)
{
  if (badness >= OPT::picky) {
    IO::error << message;
  }
  if (badness >= bDISASTER) {untested();
    abort();
  }else if (badness >= bEXIT) {untested();
    exit(badness);
  }else if (badness >= bERROR) {
    if (ENV::run_mode == rBATCH) {untested();
      exit(badness);
    }else{
      siglongjmp(env.p,1);
      //throw badness;
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
