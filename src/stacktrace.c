/* 
   DB Hub - A Linux/Unix version of the Direct Connect hub

   Backtrace Facility Implementation

   Copyright (C) DarKRaveR 2007
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file stacktrace.c
 *
 * @brief Backtrace Facilities Implementation
 *
 * This file implemtens a crash (sig)handler writing a 'backtrace' into our logfile
 */


#include "config.h"
#ifdef DEBUG
#include <execinfo.h>
#include <stdlib.h>
#include "logger.h"

/** @brief Grabs stackframe(s) and writes trace to logfile */
void logbacktrace(int z){
	void *bt[64];
	int ssize;
	char **syms;
	int i;

	ssize=backtrace (bt, 64);
	syms=backtrace_symbols(bt,ssize);

	logprintbt("#### Recieved signal %d",z);
	logprintbt("#### Backtrace showing %zd items ####",ssize);
	for (i=0; i<ssize; i++) logprintbt("%d: %s",i,syms[i]);
	logprintbt("#### Backtrace ends here ####");
	if (syms) free(syms);
	abort();
}

#endif
