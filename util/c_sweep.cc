/*$Id: c_sweep.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
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
 * Step a parameter and repeat a group of commands
 */
//testing=none 2006.07.17
#include "c_comand.h"
#include "globals.h"
extern int swp_count[], swp_steps[];
extern int swp_type[];
extern int swp_nest;
/*--------------------------------------------------------------------------*/
namespace {
  static std::string tempfile = STEPFILE;
/*--------------------------------------------------------------------------*/
static void setup(CS& cmd)
{ untested();
  for (;;) { untested();
    if (cmd.is_digit()) { untested();
      swp_steps[swp_nest] = cmd.ctoi() ;
      swp_steps[swp_nest] = (swp_steps[swp_nest]) 
	? swp_steps[swp_nest]-1
	: 0;
    }else if (cmd.umatch("li{near} ")) { untested();
      swp_type[swp_nest] = 0;
    }else if (cmd.umatch("lo{g} ")) { untested();
      swp_type[swp_nest] = 'L';
    }else{ untested();
      break;
    }
  }
}
/*--------------------------------------------------------------------------*/
static void buildfile(CS& cmd)
{ untested();
  static FILE *fptr;
  
  setup(cmd);
  if (fptr) { untested();
    fclose(fptr);
  }else{ untested();
  }
  fptr = fopen(tempfile.c_str(), "w");
  if (!fptr) { untested();
    throw Exception_File_Open("can't open temporary file:" + tempfile);
  }else{ untested();
  }
  fprintf(fptr, "%s\n", cmd.fullstring().c_str());
  
  for (;;) { untested();
    char buffer[BUFLEN];
    getcmd(">>>", buffer, BUFLEN);
    if (Umatch(buffer,"go ")) { untested();
      break;
    }else{ untested();
    }
    fprintf(fptr, "%s\n", buffer);
  }
  fclose(fptr);
  fptr = nullptr;
}
/*--------------------------------------------------------------------------*/
static void doit(CARD_LIST* scope)
{ untested();
  static FILE *fptr;
  
  for (swp_count[swp_nest]=0; swp_count[swp_nest]<=swp_steps[swp_nest];
       ++swp_count[swp_nest]) { untested();
    if (fptr) { untested();
      fclose(fptr);
    }else{ untested();
    }
    fptr = fopen(tempfile.c_str(), "r");
    if (!fptr) { untested();
      throw Exception_File_Open("can't open " + tempfile);
    }else{ untested();
    }
    char buffer[BUFLEN];
    fgets(buffer,BUFLEN,fptr);
    { untested();
      CS cmd(CS::_STRING, buffer); //fgets from local file, obsolete
      if (cmd.umatch("sw{eep} ")) { untested();
	setup(cmd);
      }else{ untested();
	throw Exception("bad file format: " + tempfile);
      }
      size_t ind = cmd.cursor();
      strncpy(buffer, "fault                              ", ind);
      buffer[ind-1] = ' ';		/* make sure there is a delimiter   */
    }					/* in case the words run together   */
    for (;;) {				/* may wipe out one letter of fault */
      { untested();
	CS cmd(CS::_STRING, buffer); //fgets from local file, obsolete
	CMD::cmdproc(cmd, scope);
      }
      if (!fgets(buffer,BUFLEN,fptr)) { untested();
	break;
      }else{ untested();
      }
      { untested();
	CS cmd(CS::_STRING, buffer); //fgets from local file, obsolete
	if (cmd.umatch("sw{eep} ")) { untested();
	  cmd.warn(bDANGER, "command not allowed in sweep");
	  buffer[0] = '\'';
	}else{ untested();
	}
      }
      IO::mstdout << swp_count[swp_nest]+1 << "> " << buffer;
    }
  }
  fclose(fptr);
  fptr = nullptr;
  swp_count[swp_nest] = 0;
}
/*--------------------------------------------------------------------------*/
class CMD_SWEEP : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override { untested();
    if (cmd.more()) { untested();
      buildfile(cmd);
    }else{ untested();
    }
    doit(Scope);
    command("unfault", Scope);
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "sweep", &p);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
