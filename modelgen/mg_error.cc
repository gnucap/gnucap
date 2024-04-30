/*$Id: mg_error.cc 2016/05/15 al $ -*- C++ -*-
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
 * Error handler.
 */
//testing=script,sparse 2006.10.31
#include "ap.h"
extern int errorcount;
/*--------------------------------------------------------------------------*/
static void tab(size_t n)
{ untested();
  for (unsigned i=0; i<n; ++i) { untested();
    std::cout << ' ';
  }
}
/*--------------------------------------------------------------------------*/
/* syntax_msg: print syntax error message
 * echo a portion of the input with an arrow pointing to the current place
 */
//BUG// override CS::warn locally, works with posix, duplicate symbol with windows
// this missing means to use the library function, which is incomplete,
// losing the error messages
#if !defined(__WIN32__)
CS & CS::warn(int badness, size_t spot, const std::string& message)
{ untested();
  if (badness >= 0) { untested();
    ++errorcount;

    size_t linestart = spot;
    for (;;) { untested();
      if (linestart == 0) {untested();
	break;
      }else if (_cmd[linestart] == '\n') { untested();
	++linestart;
	break;
      }else{ untested();
	--linestart;
      }
    }

    int lineno = 1;
    for (size_t i=0; i<linestart; ++i) { untested();
      if (_cmd[i] == '\n') { untested();
	++lineno;
      }else{ untested();
      }
    }

    std::cout << _name << ':' << lineno << ":\n";
    if (spot-linestart < 20) { untested();
      for (size_t i=linestart; _cmd[i] && _cmd[i]!='\n'; ++i) { untested();
	std::cout << _cmd[i];
      }
      std::cout << '\n';
      tab(spot-linestart);
    }else{untested();
      std::cout << "..";
      for (size_t i=spot-15; _cmd[i] && _cmd[i]!='\n'; ++i) {untested();
	std::cout << _cmd[i];
      }
      std::cout << '\n';
      tab(17);
    }
    std::cout << "^ ? " << message << '\n';
  }else{untested();
  }
  return *this;
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
