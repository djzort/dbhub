/* 
   DB Hub - A Linux/Unix version of the Direct Connect hub

   Logging Facility Implementation

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
 * @file logger.c
 *
 * @brief Logging Facilities Implementation
 *
 * This file implemtens the logging facility. Logging is split in logopen (which sets everything
 * up to log to the selected destination, logprintf, which creates a log entry and logclose, which
 * does the final cleanup.
 */

#include "logger.h"
#include "conf.h"
#include "main.h"
#include "fileio.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>

/* Will be used as soon as new config module is ready
static config_item_t loggerconf[]={
	{ "logger.filename", CONF_TYPE_STRING, CONF_PRESET_STRING("log"),
		"Default logfile name, if not overriden by command line"},
	{ "logger.verbosity", CONF_TYPE_INT, CONF_PRESET_INT(0),
		"Default Verbosity Level of logging"},
	CONF_ENDTABLE};
*/

static int loggerhandle=-1; /*!< Holds the logs handle (or 0 for syslog) */
int logusesyslog=0; /*!< 1 if we want to use syslog */

/** @brief Opens the log.
 */
void logopen(){
	char path[MAX_FDP_LEN+1];
	int i=10;
	int fd;
	if (debug) loggerhandle=STDOUT_FILENO;
	else{
		if (logusesyslog){
			openlog("dbhub", LOG_PID, LOG_DAEMON);
			loggerhandle=0;
		}else{
			if (strlen(logfile) > 1) strncpy(path, logfile, MAX_FDP_LEN);
			else snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, LOG_FILE); /* No preset logfile */
			while(((fd = open(path, O_WRONLY | O_CREAT|O_APPEND|O_SYNC, 0600)) < 0) && (errno == EINTR) && i) i--;
			loggerhandle=fd;
			if (fd<0) fprintf(stderr,"WARNING: Failed to open Logfile %s\n",path);
		}
	}
}

/** @brief Works like printf and puts message into log  */
void logprint(int verb, const char *format, ...)
{
   static char filebuf[8192];
   int erret;
   int towrite;
   char *localtime,*temp,*bufs;
   time_t current_time;
   
   if(verb > verbosity) return;   
   if(format)
     {
	va_list args;
	va_start(args, format);
	
	if (logusesyslog){
		vsyslog(verb,format, args);
		va_end(args);
		return;
	}

	current_time = time(NULL);
	if (current_time != -1){
		localtime = (char *)ctime(&current_time);
		temp = strchr(localtime,' ')+1;
		bufs = strrchr(localtime,' ');
		if (temp && bufs) strncpy(filebuf,temp, bufs-temp+1);
		vsnprintf(filebuf+(bufs-temp+1),8190-(bufs-temp), format, args);
		va_end(args);
		towrite=strlen(filebuf);
		if (filebuf[towrite-1]!='\n') {
			filebuf[towrite]='\n';
			filebuf[towrite+1]='\0';
		}
		if (!debug) if(set_lock(loggerhandle, F_WRLCK) == 0) return;
		towrite=strlen(filebuf);
		bufs=filebuf;
		while ( (erret=write(loggerhandle,bufs,towrite))!=towrite ){
			if (erret>=0) {
				towrite-=erret;
				bufs+=erret;
			}
		}

		if (!debug) set_lock(loggerhandle, F_UNLCK);
	}
     }
}


void logclose(){
	int i=10;
	int erret;
	if (loggerhandle!=STDOUT_FILENO && loggerhandle!=0) while ( (( erret = close(loggerhandle)) != 0) && (errno ==EINTR) && i<0) i--;
	if (loggerhandle==0) closelog();
}
