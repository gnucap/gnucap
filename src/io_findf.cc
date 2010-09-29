/*$Id: io_findf.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * Modified by AD.  Sent to me by C-WARE
 * This file contains the routine to locate a file,
 *	using a path string for the directories to search.
 * Interface:
 *	findfile(filename, paths, mode)
 *	    filename is the name of the file to be searched for,
 *	    paths is the path to follow to find it.
 *	    mode is how you want to open the file
 *	returns full path name, if successful, else "".
 *
 * PATHSEP, ENDDIR are system dependent, defined in md.h
 */
#include "l_lib.h"
/*--------------------------------------------------------------------------*/
std::string findfile(const std::string& filename, const std::string& path,
		     int mode)
{
#ifdef CHECK_LOCAL_FIRST
  if (OS::access_ok(filename, mode)){
    untested(); 
    return filename;
  }else{
    untested(); 
  }
#endif
					// for each item in the path
  for (std::string::const_iterator
	 p_ptr=path.begin(); p_ptr!=path.end(); ++p_ptr){
    // p_ptr changed internally                  ^^^^^ skip sep
    std::string target = "";
    while (*p_ptr != PATHSEP  &&  p_ptr != path.end()){ // copy 1 path item
      target += *p_ptr++;
    }
    if (!target.empty() &&  !strchr(ENDDIR,p_ptr[-1])){
      target += *ENDDIR;		// append '/' if needed
    }
    
    target += filename;
    {if (OS::access_ok(target, mode)){	// found it
      return target;
    }else if (p_ptr==path.end()){	// ran out of path, didn't find it
      return "";
    }}					// else try again
  }
  return ""; // path doesn't exist - didn't go thru loop at all
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
