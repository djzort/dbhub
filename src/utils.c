/**@file utils.c
 * @brief Implementation of utility functions

* DB Hub - A Linux/Unix version of the Direct Connect hub                                                                 
*  Copyright (C) 2005-2007  Centurion (DB Hub)                                                                             
*  Copyright (C) 2002,2003  Jonatan Nilsson (Open DC Hub)                                                                  
*     
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include <errno.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "main.h"
#include "utils.h"
#include "fileio.h"
#include "userlist.h"
#include "network.h"
#include "commands.h"
#include "logger.h"
#include "langvars.h"
#include "semops.h"

/* the number of the next char which is the char c */
/* Maybe it's better with just strchr(buf) - buf */
int cut_string(char *buf, char c)
{
	int i;
	i = 0;
	while((buf[i] != c) && (buf[i] != '\0'))
		i++;
	if(buf[i] == c)
		return i;
	return -1;
}

/* Appends to the end of a string */
/* NO checking of buf size, so it MUST be big enough */
/* The string usually has to be zeroed before this can be used */
void sprintfa(char *buf, const char *format, ...)
{  
	if(format)
	{
		va_list args;
		va_start(args, format);
		vsprintf(buf + strlen(buf), format, args);
		va_end(args);
	}
}

/* Send formated string to user. Maximum length is 4096 chars */
void uprintf(struct user_t *user, char *format, ...)
{
	char buf[4096];
	if(format)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(buf, 4095, format, args);
		va_end(args);
		send_to_user(buf, user);
	}
}

/* Removes \r:s and \n:s and bs:s from end of a string */
int trim_string(char *buf)
{
	int len;
	if(!buf)
		return 0;
	if(buf[0] == '\0')
		return 1;
	for(len = strlen(buf)-1; len >=0; len--)
	{
		if((buf[len] == '\n') || (buf[len] == '\r') || (buf[len] == ' '))
			buf[len] = '\0';
		else
			break;
	}
	return 1;
}

/* Counts number of users which are included in type.  */
int count_users(int type)
{
	int count;
	struct user_t *non_human;
	struct sock_t *human_user;

	count = 0;
	non_human = non_human_user_list;
	human_user = human_sock_list;

	/* Start with non-human users.  */
	while(non_human != NULL)
	{
		if((type & non_human->type) != 0)
			count++;

		non_human = non_human->next;
	}

	/* And the human users.  */
	while(human_user != NULL)
	{
		if((type & human_user->user->type) != 0)
			count++;

		human_user = human_user->next;
	}

	return count;
}

/* Puts users hostname in buffy.  */
void get_users_hostname(char *nick, char *buffy)
{
	char *buf, *bufp;
	char temp_nick[MAX_NICK_LEN+1];
	char temp_host[MAX_HOST_LEN+1];
	int spaces=0, entries=0;
	int i;

	sem_take(user_list_sem);

	/* Attach to the shared segment.  */
	if((buf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
		== (char *)-1)
	{	
		logprintf(LOG_EMERG, "Error@shmat(): %s",strerror(errno));
		sem_give(user_list_sem);
		quit = 1;
		return;
	}

	if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
	{	
		logprintf(LOG_EMERG, "Error: Couldn't get number of entries");
		shmdt(buf);
		sem_give(user_list_sem);
		quit = 1;
		return;
	}   

	bufp = buf + 30;

	for(i = 1; i <= spaces; i++)
	{	
		if(*bufp != '\0')
		{	     
			sscanf(bufp, "%50s %120s", temp_nick, temp_host);
			if((strncasecmp(temp_nick, nick, strlen(nick)) == 0)
				&& (strlen(nick) == strlen(temp_nick)))
			{		  
				/* The user is here, so detach and put the hostname in the
				* buf.  */
				sprintf(buffy, "%s", temp_host);
				shmdt(buf);
				sem_give(user_list_sem);
				return;
			}	     
		}	
		bufp += USER_LIST_ENT_SIZE;
	}

	/* If user wasn't found, put null in returning string */
	*buffy = '\0';

	shmdt(buf);
	sem_give(user_list_sem);
}

/* Returns a hash value from a users nickname. It's important that this 
* function generates values as random as possible, but also stays fast.  */
int get_hash(char *nick)
{
	register char *s1, *s2;
	register int i = 0;
	register int hash = 0;

	/* First char in nick.  */
	s1 = nick;

	/* Last char in nick.  */
	s2 = nick + strlen(nick) - 1;

	do 
	{      	
		hash |= ((*s1 & 0x1) << i);
		i++;
		hash |= ((*s2 & 0x1) << i);
		i++;
		s1++;
		s2--;
	} while((s1 <= s2) && (hash < max_sockets));

	while(hash > max_sockets)
		hash >>= 1;

	return hash;
}


/* Get the uptime of the hub in seconds.  */
long long get_uptime(void)
{
	return difftime(time(NULL), hub_start_time);
}

/* Returns 1 if buf1 is a match in buf2, wich can contain wildcards.  */
int match_with_wildcards(char *buf1, char *buf2)
{
	int k = 0;
	char token[MAX_HOST_LEN+1];
	char *fbuf, *ubuf;

	/* The '*' is allowed as wildcard. To ban a nick with a '*'in it, it has to be
	* escaped with a '\'. '\':s also have to be escaped with '\'.  */

	fbuf = buf2;
	ubuf = buf1;
	while((*fbuf != '\0') && (*ubuf != '\0'))
	{
		/* If we are escaping a '\' or a '*':  */
		if(*fbuf == '\\')
		{
			/* After a '\', only '*' and '\' is allowed.  */
			fbuf++;
			if(*fbuf == '\0')
				return 0;
			if(*fbuf == '\\')
			{
				if(*ubuf != '\\')
					return 0;
			}
			else if(*fbuf == '*')
			{
				if(*ubuf != '*')
					return 0;
			}
			else
				return 0;
		}			    

		/* If we have a wildcard.  */
		if(*fbuf == '*')
		{	
			fbuf++;
			if(*fbuf == '\0')
				return 1;

			if(*fbuf == '*')
				return 0;

			if((k = cut_string(fbuf, '*')+1) == 0)
				k = strlen(fbuf)+1;

			if(k == 1)
				k = strlen(fbuf);

			if((strncmp(fbuf, ubuf, k) == 0) && (*(ubuf+k) == '\0'))
				return 1;

			strncpy(token, fbuf, k-1);
			*(token + k - 1) = '\0';
			if(strstr(ubuf, token) == NULL)
				return 0;

			fbuf += k-2;
			ubuf = strstr(ubuf, token) + k - 2;
		}   

		/* No wildcard, just compare the strings character by 
		* character.  */
		else if(*fbuf != *ubuf)
			return 0;

		fbuf++;
		ubuf++;
	}

	if((*ubuf == '\0') && ((*fbuf == '*') && (*(fbuf+1) == '\0')))
		return 1;

	if(*fbuf != *ubuf)
	{	
		fbuf = buf2+strlen(buf2)-1;
		ubuf = buf1+strlen(buf1)-1;
		while((fbuf >= buf2) && (*fbuf != '*'))
		{
			if(*fbuf != *ubuf)
				return 0;
			fbuf--;
			ubuf--;
		}
	}   
	return 1;
}

/** @brief Converts a string to all lowercase
 *
 * This Functions converts the string passed as string to all lowercase 
 * letters.
 *  @param [in] string: char *, string to convert to lowercase.
 */
void strtolower(char *string) {
	while ( (*string=tolower(*string)) ) string++;
}            
 
void time2str(double seconds, char *strtime)
{
        int days,hours,mins,secs,weeks;
  
  weeks  = ((int)seconds/60/60/24/7);
  days  = ((int)seconds/60/60/24)%7;
  hours = ((int)seconds/60/60)%24;
  mins  = ((int)seconds/60)%60;
  secs  = ((int)seconds)%60;

  // Create uptime string
        strtime[0]='\0';
        if (weeks != 0) sprintfa(strtime,"%d %s, ",weeks, lp_weeks);
	if (days != 0) sprintfa(strtime,"%d %s, ",days, lp_days);
        if (hours != 0) sprintfa(strtime,"%d %s, ",hours, lp_hours);
        if (mins != 0) sprintfa(strtime,"%d %s, ",mins, lp_minutes);
	if (seconds != 0) sprintfa(strtime,"%d %s",secs, lp_seconds);
	else
	strtime[strlen(strtime) - 2] = '\0';
}

int search_in_string (char *search_here, char *search_this)
{
    int i=0;
 
    for (i=0;strlen(search_here)-strlen(search_this);i++)
    {
    if (strncasecmp(search_here,search_this,strlen(search_this))==0) return 1;
    search_here++;
    }
    return 0;
}

//DB// replacement for case insenstive for strstr
//DB// taken from http://www.daniweb.com/code/snippet313.html

const char *mystristr(const char *haystack, const char *needle)
{
if ( !*needle )
{
return haystack;
}
for ( ; *haystack; ++haystack )
{
if ( toupper(*haystack) == toupper(*needle) )
{
/*
* Matched starting char -- loop through remaining chars.
*/
const char *h, *n;
for ( h = haystack, n = needle; *h && *n; ++h, ++n )
{
if ( toupper(*h) != toupper(*n) )
{
break;
}
}
if ( !*n ) /* matched all of 'needle' to null termination */
{
return haystack; /* return the start of the match */
}
}
}
return 0;
}

/** @brief Loads a File into buffer
 *
 * Loads a file given by name into a continious chunk of memory (a buffer)
 * @param [in] buffer:		char ** 	- Adress of Pointer to memory chunk
 * @param [in] filename: 	const char * 	- Character Pointer to filename
 */
void loadfile2buf(char **buffer, const char *filename){
        int size = 0;
        int fd;
        char path[MAX_FDP_LEN+1];
        int erret;
	struct stat statret;

#ifdef DEBUG
	logprintf(LOG_INFO,"entering..");
#endif
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, filename );
	logprintf(LOG_INFO,"File[%s]", path);
	if (stat(path, &statret)<0) {
		logprintf(LOG_EMERG, "Error@stat(%s): %s", filename, strerror(errno));
		return;
	}else size=statret.st_size;
#ifdef DEBUG
	logprintf(LOG_INFO,"Filesize of %s: %d",filename, size);
#endif
	while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
	logprintf(LOG_NOTICE, "@open(%s): %s. Retrying.",filename,strerror(errno));
	
	if(fd < 0){
		logprintf(LOG_EMERG, "Error@open(%s): %s",filename,strerror(errno));
		return;
	}
	if(set_lock(fd, F_RDLCK) == 0){
		logprintf(LOG_EMERG, "Error@set_lock(%s): Locking failed",filename);
		close(fd);
		return;
	}

        if(*buffer != NULL){
#ifdef DEBUG
	logprintf(LOG_INFO,"free(%p)",*buffer);
#endif
		free(*buffer);
	}
        
	if((*buffer = malloc(sizeof(char) * (size+1))) == NULL){
		logprintf(LOG_EMERG, "Error@malloc(): %s",strerror(errno));
		quit = 1;
		return;
        }

        if (size != read(fd, *buffer, sizeof(char)*size)){
                free(*buffer);
		*buffer=NULL;
                logprintf(LOG_EMERG, "Error@read(%s): %s",filename,strerror(errno));
                quit = 1;
                return;
        }
        (*buffer)[size] = '\0';
#ifdef DEBUG
	logprintf(LOG_DEBUG,"File Contents[%s]",*buffer);
#endif

	set_lock(fd, F_UNLCK);

	while(((erret = close(fd)) != 0) && (errno == EINTR))
	logprintf(LOG_NOTICE, "Error@close(%s): %s. Retrying.",filename,strerror(errno));

	if(erret != 0){
		logprintf(LOG_WARNING, "Error@close(%s): %s",filename, strerror(errno));
	}

#ifdef DEBUG
	logprintf(LOG_INFO,"leaving...");
#endif

}


