/* DB Hub - A Linux/Unix version of the Direct Connect hub
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

#include <sys/types.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <errno.h>

#include "main.h"
#include "utils.h"
#include "fileio.h"
#include "commands.h"
#include "network.h"
#include "userlist.h"
#ifdef HAVE_PERL
# include "perl_utils.h"
#endif

#ifndef HAVE_STRTOLL
# ifdef HAVE_STRTOQ
#  define strtoll(X, Y, Z) (long long)strtoq(X, Y, Z)
# endif
#endif

#include "motd.h"
#include "logger.h"
#include "langvars.h"
#include "semops.h"

#ifdef HAVE_LANG
#include "lang.h"
#endif

/* This command has the following format:
*$SR fromnick filename\5filesize openslots/totalslots\5hubname (hubip:hubport)\5tonick| */
void sr(char *buf, struct user_t *user)
{
	char command[6];
	char fromnick[MAX_NICK_LEN+1];
	char filename[501]; /* Should do */
	long long unsigned filesize;
	time_t now;
	int totalslots = 0;
	int openslots = 0;
	char hubname[301];
	char longmessage[1001];
	char tonick[MAX_NICK_LEN+1];
	char *send_buf;
	struct user_t *to_user;

	now = time(NULL);

        user->freeslots = 0;                                                                         
	user->allslots = 0;     

	if(sscanf(buf, "%5s %50s %500[^\5]\5%llu %d/%d\5%300[^\5]\5%50[^|]|", 
		command, fromnick, filename, &filesize, &openslots, 
		&totalslots, hubname, tonick) != 8)
	{
		/* Sometimes, the filesize seems to be skipped. */
		if(sscanf(buf, "%5s %50s %500[^\5]\5%300[^\5]\5%50[^|]|", 
			command, fromnick, filename, hubname, tonick) != 5)
		{	     
			logprintf(4, "Received bad $SR command from %s at %s:\n", 
				user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}	
	}

	/* First a validation check */
	if(tonick[0] == '\0')
	{
		logprintf(4, "Received bad $SR command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}
	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
		if((strncmp(fromnick, user->nick, strlen(fromnick)) != 0)
			|| (strlen(fromnick) != strlen(user->nick)))
		{
			logprintf(3, "User %s at %s claims to be someone else in $SR:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(3, "%s\n", buf);
			else
				logprintf(3, "too large buf\n");
			user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return;
		}
	}
	
	//DB// Kicking user (except ADMIN/OWNER) for having incomplete files in share like:
	//DB// .part.met, INCOMPLETE~, .dctmp, .bc!
	//DB// It's called by alarm_signal() function and kick user on $SR reply
	//DB// note: it will kick only users in active mode
	//DB// $SR is send as a reply from active user to passive user through the hub
	
	if((kick_for_incomplete == 1) && ((user->type & (ADMIN | OWNER)) == 0) && (filesize > 0)) 
	{
		if((strstr(filename, ".part.met") != NULL) || (strstr(filename, ".dctmp") != NULL) || 
		(strstr(filename, "INCOMPLETE~") != NULL) || (strstr(filename, ".bc!") != NULL))
		{
			uprintf(user, "$To: %s From: %s $<%s> You are being kicked because: %s -> %s|", user->nick, botnick, botnick, lp_unfinishedfiles, filename);	
			snprintf(longmessage, 1000, "<%s> is kicking %s (IP: %s) because: %s -> %s|", botnick, user->nick, user->hostname, lp_unfinishedfiles, filename);
			send_to_humans(longmessage, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);	
			user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return;
		}
	}
	

	if((send_buf = malloc(sizeof(char) * (strlen(buf) + 1))) == NULL)
	{
		logprintf(0, "Error - In sr()/malloc(): %s", strerror(errno));
		quit = 1;
		return;
	}
	strcpy(send_buf, buf);

	/* Remove the nick at the end */
	*(strrchr(send_buf, '\005') + 1) = '\0';
	*(strrchr(send_buf, '\005')) = '|';

	if(totalslots > 0)
	{
		user->freeslots = openslots;
		user->allslots = totalslots;
		user->srtime = now;
	}
	/* And then forward it */
	if((to_user = get_human_user(tonick)) != NULL)
		send_to_user(send_buf, to_user); 
	free(send_buf); 
}
/* The search command, has the following format:
* $Search ip:port byte1?byte2?size?byte3?searchpattern|
* If the search was made by a client in passive mode, the ip:port is replaced
* by Hub:nickname */
void search(char *buf, struct user_t *user)
{
	char command[15]; 
	char ip[MAX_HOST_LEN+1];
	char port[MAX_NICK_LEN+1];
	char byte1, byte2, byte3;
	char pattern[51]; /* It's the last argument, so it doesn't matter if it fits in the string */
	long long unsigned size;
	time_t now;
	/* Don't bother to check the command if it was sent from a forked process */
	if(user->type != FORKED)
	{	
		if(sscanf(buf, "%14s %122[^:]:%50s %c?%c?%llu?%c?%50[^|]|", 
			command, ip, port, &byte1, &byte2, &size, &byte3, pattern) != 8)
		{
			logprintf(4, "Received bad $Search command from %s at %s:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}
		if(pattern[0] == '\0')
		{
			logprintf(4, "Received bad $Search command from %s at %s:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}
	
		if(strlen(pattern) < min_search_len)
		{
			uprintf(user, "<%s> %s: %d|", botnick, lp_searchtooshort, min_search_len);	
			return;
		}

	
	
	}
	
	now = time(NULL);
	
	if((user->type != FORKED) && (strstr(pattern, "TTH:") == NULL))
	{
		if((searchspam_time > 0) && 
			(difftime(now, user->last_search) <= (double)searchspam_time))
		{
			user->last_search = now;
			uprintf(user, "<%s> %s %d %s|", botnick, lp_searchignored, searchspam_time, lp_seconds);
			return;
		}
		
	}

	user->last_search = now;
	
	/* Now, forward to all users */
        if(hide_allshare < 2)
	{
		if(restrictions == 0)
		{
			if(hide_opshare == 1)
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP, NULL);
			else if((hide_opshare == 2) && (user->type == REGULAR))
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP, NULL);
			else
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}
		else if(restrictions == 1)
		{
			if(user->type == REGULAR)
				send_to_humans(buf, REGULAR, NULL);
			else if(user->type == PROTECTED)
				send_to_humans(buf, REGULAR | PROTECTED, NULL);
			else if(user->type == REG)
				send_to_humans(buf, REGULAR | PROTECTED | REG, NULL);
			else if(user->type == VIP)
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP, NULL);
			else if(user->type == KVIP)
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP, NULL);
			else if(user->type == OP)
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP, NULL);
			else if(user->type == CHEEF)
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF, NULL);
			else if((user->type == ADMIN) || (user->type == OWNER))
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}
		else if(restrictions == 2)
		{
			if(user->type == REGULAR)
				send_to_humans(buf, REGULAR, NULL);
			else
				send_to_humans(buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}
	
		if(strstr(pattern, "TTH:") != NULL)
		tthsearches++;
	}
}


/* Forwards to all logged in users */
void chat(char *buf, struct user_t *user)
{
	char *antybuf;
	const char *kstr;
	char *bwtmp;
	char *bwstr;
	char *temp;
	char *urltemp;
	char sdayn[6];
	char sday[4];
	char sdate[11];
	char syear[6];
	char smont[6];
	char chatstring[301];
	char period[2];
	char kg1user[51];
	char kg2user[51];
	char largestring[501];
	char msgl[801];
	char bigstring[1001]; 
	char tempstring[151];
	char nick[51];
	char phrase[51];
	char acttime[51];
	int messagebuf, a, b, c;
	struct user_t *buser;
        struct sock_t *human_user;
	struct sock_t *next_human;
	struct tm * timeinfo;
	char path[MAX_FDP_LEN+1];
	time_t nicklistban;
	time_t now;

	now = time(NULL);

	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
		if(sscanf(buf, "<%50[^>]> %300[^|]|", nick, chatstring) < 1)
		{
			logprintf(0, "Received bad chat command from %s at %s\n", user->nick, user->hostname);
			user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return;
		}
		if(chatstring[0] == '\0')
		{                                                                             		return;
		}

		if((strncmp(buf + 1, user->nick, strlen(nick)) != 0) || (strlen(nick) != strlen(user->nick)))
		{
			logprintf(3, "User %s at %s claims to be someone else in chat\n", user->nick, user->hostname);
			user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return;
		}
	}
	
	//DB// Filtering banwords, non-opped users only, case insensitive
	if(((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0) && (banwords != NULL))
	{
		if(strlen(banwords) > 0)
		{
			if(strstr(banwords, ";") != NULL)
			{
				if((bwtmp = malloc(sizeof(char) * (strlen(banwords) + 1))) == NULL)
				{
					logprintf(0, "Error - In chat()/malloc(): %s", strerror(errno));
					quit = 1;
					return;
				}
				sprintf(bwtmp, "%s", banwords);
				bwstr = bwtmp;
				while(strstr(bwstr, ";") != NULL)
				{
					if(sscanf(bwstr, "%50[^;];", phrase) == 1)
					{
						if(mystristr(buf, phrase) != NULL)
						{
							snprintf(largestring, 500, "<%s> %s %s (IP: %s): ", botnick, lp_banworddetected, user->nick, user->hostname);
							send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, NULL);		
							send_to_humans(buf, OP | CHEEF | ADMIN | OWNER, NULL);
							free(bwtmp);
							blocked_mc++;
							return;
						}
					}
					bwstr = bwstr + strlen(phrase) + 1;
		    		}
		    if(strlen(bwstr) > 0)
		    {	    
			    if(mystristr(buf, bwstr) != NULL)
			    {
			    snprintf(largestring, 500, "<%s> %s %s (IP: %s): ", botnick, lp_banworddetected, user->nick, user->hostname);
			    send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, NULL);		
			    send_to_humans(buf, OP | CHEEF | ADMIN | OWNER, NULL);
			    free(bwtmp);
			    blocked_mc++;
			    return;
			    }
		    }
		free(bwtmp);		
		}		
		else
		{
		    if(mystristr(buf, banwords) != NULL)
		    {
		    snprintf(largestring, 500, "<%s> %s %s (IP: %s): ", botnick, lp_banworddetected, user->nick, user->hostname);
		    send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, NULL);		
		    send_to_humans(buf, OP | CHEEF | ADMIN | OWNER, NULL);
		    blocked_mc++;
		    return;
		    }
		}
		}
		}

	if(((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && ((chatstring[0] == '!') || (chatstring[0] == '+')) && (strlen(chatstring) > 1))
	{
		temp = strchr(buf, ' ') + 1;
		commandsnmb++;
		
		if(is_command(temp, user, 0) == 1)
		return;
		
		if(strlen(extra_files) > 0)
		{
		snprintf(bigstring, 1000, " %s  ", extra_files);	
		snprintf(largestring, 500, " %s ", chatstring+1);
		if((search_in_string(chatstring, " ") == 0) && (chatstring[1] != '\0') && (search_in_string(bigstring, largestring) == 1))
		{
		if(check_extrafiles(chatstring+1) == 1)
		{
		uprintf(user, "<%s> %s:\r\n", botnick, chatstring+1);
		send_extrafiles(user, chatstring+1);
		uprintf(user, "|");
		}
		else
		uprintf(user, "<%s> %s: %s/%s/%s|", botnick, lp_nosuchfile, config_dir, EXTRAFILES_DIR, chatstring+1);
		}
		}
		else if((strstr(buf, "is kicking") != NULL) && (strstr(buf, "because:") != NULL))
		{
		if((sscanf(buf, "<%50[^>]> %50s is kicking %50s because*", kg1user, kg2user, acttime) == 3)
		|| (sscanf(buf, "<%50[^>]> is kicking %50s because*", kg1user, acttime) == 2))
		{
		kstr = strstr(buf, "because");
		if((buser = get_human_user(acttime)) != NULL)
		{
		if((get_human_user(kg1user)->type & (KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
		{
		sscanf(kstr+7, "%500[^|]|", largestring);
		snprintf(bigstring, 1000, "<%s> is kicking %s (IP: %s) because%s|", kg1user, buser->nick, buser->hostname, largestring);
		send_to_humans(bigstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		if((kstr = mystristr(buf, "_ban_")) != NULL)
		{
		if(sscanf(kstr+5, "%lu%1s", &nicklistban, period) == 2)
		{
		if(nicklistban != 0)
		{
		if(strcasecmp(period, "s") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_seconds);
		snprintf(tempstring, 150, "%s %lu|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %lu|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + nicklistban, buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + nicklistban, buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_seconds);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "m") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_minutes);
		snprintf(tempstring, 150, "%s %lum|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %lum|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 60), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 60), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_minutes);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "h") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_hours);
		snprintf(tempstring, 150, "%s %luh|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %luh|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 3600), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 3600), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s %s %s [ %s %lu %s ]", buser->nick, buser->hostname, largestring,lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_hours);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "d") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_days);
		snprintf(tempstring, 150, "%s %lud|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %lud|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 86400), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 86400), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_days);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "w") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_weeks);
		snprintf(tempstring, 150, "%s %luw|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %luw|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 604800), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 604800), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_weeks);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}

		}
		}
		else if(((strncmp(kstr+5, "|", 1) == 0) || (strncmp(kstr+5, " ", 1) == 0)) && ((get_human_user(kg1user)->type & (OP | CHEEF | ADMIN | OWNER)) != 0))
		{
		unballow(buser->hostname, BAN);
		unballow(buser->nick, NICKBAN);
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s|", botnick, lp_banning_, buser->nick, lp_permanently);
		snprintf(tempstring, 150, "%s 0|", buser->hostname);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s 0|", buser->nick);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "0 %s [ %s ]", buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "0 %s [ %s ]", buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ]", buser->nick, buser->hostname, largestring, lp_bannedby, get_human_user(kg1user)->nick);        
		last_perm(lastbuf);
		}
		}
		else
		{
		if((kick_bantime > 0) && (tempban != 0))
		{
		if((strchr(largestring, '\n') == NULL) && (strlen(largestring) > 2))
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (kick_bantime * 60), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if((strchr(largestring, '\n') == NULL) && (strlen(largestring) > 2))
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (kick_bantime * 60), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		}
		}
		
		}
		return;
		}
		}
		}
		
	else
	{
	if(extra_commands == 0)
	goto nocommand;
	}
	}
	else if (strncasecmp(chatstring, "/me ", 4) == 0)
		{	
		commandsnmb++;
		user->last_write = now;
                if((user->autoawaytime > 0) && (user->away_flag == 2))
		{
		uprintf(user, "<%s> %s|", botnick, lp_autoawaymodeoff);
		if(check_if_away(user->nick) == 1)
		{
		time2str(difftime(now, time_to_unban), tempstring);
		snprintf(largestring, 500, "<----> %s %s %s %s|", user->nick, lp_isback, lp_after, tempstring);
		}
		else
                snprintf(largestring, 500, "<----> %s %s|", user->nick, lp_isback);
		remove_away_user(user);
		user->away_flag=1;
		if(user->hide_share != 2)
		{
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return;
		}
		if(no_chat_for_chatgagged == 0)
		send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			send_to_user(largestring, human_user->user);
			human_user = next_human;
			}
		}
		}
		}
		snprintf(largestring, 150, "** %s %s|", user->nick, chatstring+4); 
		if(no_chat_for_chatgagged == 0)
		send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			send_to_user(largestring, human_user->user);
			human_user = next_human;
			}
		}
		if(((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (log_main_chat == 1) && (strchr(largestring, '\n') == NULL))
		{
		timeinfo = localtime ( &now );
		snprintf(acttime, 50, "%s", asctime (timeinfo));
		trim_string(acttime);
		sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		sdate[5] = '\0';
		if(strcmp(smont, "Jan") == 0)
		snprintf(smont, 4, "01");
		if(strcmp(smont, "Feb") == 0)
		snprintf(smont, 4, "02");
		if(strcmp(smont, "Mar") == 0)
		snprintf(smont, 4, "03");
		if(strcmp(smont, "Apr") == 0)
		snprintf(smont, 4, "04");
		if(strcmp(smont, "May") == 0)
		snprintf(smont, 4, "05");
		if(strcmp(smont, "Jun") == 0)
		snprintf(smont, 4, "06");
		if(strcmp(smont, "Jul") == 0)
		snprintf(smont, 4, "07");
		if(strcmp(smont, "Aug") == 0)
		snprintf(smont, 4, "08");
		if(strcmp(smont, "Sep") == 0)
		snprintf(smont, 4, "09");
		if(strcmp(smont, "Oct") == 0)
		snprintf(smont, 4, "10");
		if(strcmp(smont, "Nov") == 0)
		snprintf(smont, 4, "11");
		if(strcmp(smont, "Dec") == 0)
		snprintf(smont, 4, "12");
		if(strlen(sday) == 1)
		snprintf(acttime, 50, "%s-%s-0%s", syear, smont, sday); 
		else
		snprintf(acttime, 50, "%s-%s-%s", syear, smont, sday); 
		create_dailylog(acttime);
		snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, LOG_DIR, acttime);		
		snprintf(bigstring, 1000, "[%s] * %s %s\n", sdate, user->nick, chatstring+4); 
		add_line_to_file(bigstring, path);
		}
		largestring[strlen(largestring) - 1] = '\0';
		timeinfo = localtime ( &now );
		snprintf(acttime, 50, "%s", asctime (timeinfo));
		trim_string(acttime);
		sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		
		snprintf(message20, 400, "%s", message19);
		snprintf(message19, 400, "%s", message18);
		snprintf(message18, 400, "%s", message17);
		snprintf(message17, 400, "%s", message16);
		snprintf(message16, 400, "%s", message15);
		snprintf(message15, 400, "%s", message14);
		snprintf(message14, 400, "%s", message13);
		snprintf(message13, 400, "%s", message12);
		snprintf(message12, 400, "%s", message11);
		snprintf(message11, 400, "%s", message10);
		snprintf(message10, 400, "%s", message9);
		snprintf(message9, 400, "%s", message8);
		snprintf(message8, 400, "%s", message7);
		snprintf(message7, 400, "%s", message6);
		snprintf(message6, 400, "%s", message5);
		snprintf(message5, 400, "%s", message4);
		snprintf(message4, 400, "%s", message3);
		snprintf(message3, 400, "%s", message2);
		snprintf(message2, 400, "%s", message1);
		if(strlen(largestring) <= 256)
		{
		snprintf(message1, 400, "--- [%s] %s", sdate, largestring);
		}
		else
		{
		largestring[255] = '\0';
		snprintf(message1, 400, "--- [%s] %s [%s...]", sdate, largestring, lp_cut_);
		}

		}
	
	else
	{
	nocommand:
	accepted_mc++;
	

		if((user->autoawaytime > 0) && (user->hide_share != 2) && (user->away_flag == 2) && (strstr(buf, "is kicking") == NULL))
		{
                uprintf(user, "<%s> %s|", botnick, lp_autoawaymodeoff);			    	     
		if(check_if_away(user->nick) == 1)
		{
		time2str(difftime(now, time_to_unban), tempstring);
		snprintf(largestring, 500, "<----> %s %s %s %s|", user->nick, lp_isback, lp_after, tempstring);
		}
		else
		snprintf(largestring, 500, "<----> %s %s|", user->nick, lp_isback);
		remove_away_user(user);
		user->away_flag=1;
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return;
		}
		if(no_chat_for_chatgagged == 0)
		send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			send_to_user(largestring, human_user->user);
			human_user = next_human;
			}
		}
		}
	
	/* New antiflooder by Centurion */
	antybuf = strstr(buf, ">");
	messagebuf = strlen(antybuf);
	if(flood_lines < 5)
	flood_lines = 5;
	
if(flood_action > 0)
{	
	if ((messagebuf > flood_lines+1) && (messagebuf == oldbuf) && (user->type == REGULAR))
	{
	if (thesamebuf <= flood_lines) {
	thesamebuf++;
	}
	else if (thesamebuf > flood_lines) {
	if(flood_action == 1)
	{
	uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_flooding);                
	uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_flooding);
	}
	else
	{
	uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerebanned, lp_because, lp_flooding);                
	uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_flooding);
        snprintf(tempstring, 150, "%s 0|", user->hostname);
        ballow(tempstring, BAN, user);
        snprintf(tempstring, 150, "%s 0|", user->nick);
	ballow(tempstring, NICKBAN, user);
	snprintf(tempstring, 150, "0 %s [ %s ]", user->hostname, lp_flooding);
	if(check_if_banned(user, BANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
	add_line_to_file(tempstring, path);
	}
	snprintf(tempstring, 150, "0 %s [ %s ]", user->nick, lp_flooding);
	if(check_if_banned(user, NICKBANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
	add_line_to_file(tempstring, path);
        }
	snprintf(lastbuf, 800, "%s (IP: %s): %s", user->nick, user->hostname, lp_flooding);
	last_perm(lastbuf);	
	}
	snprintf(largestring, 500, "<%s> %s %s: %s (IP: %s)|", botnick, lp_flooding, lp_detectedfrom, user->nick, user->hostname);
	send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, user);
	user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
	return;
	}
	}              
	else
	{
	thesamebuf = 0;
	}
	oldbuf = strlen(antybuf);

        //DB// Old !MeM's antiflooder, strongly modified by Centurion
	if((difftime(now, user->last_write) <= 1) && (user->type == REGULAR))
        {
        user->too_fast_write_count++;
	user->last_write = now;
	if(user->too_fast_write_count == flood_lines)
        {
	uprintf(user, "<%s> %s|", botnick, lp_floodwarn);
        return;
        }
        if(user->too_fast_write_count == flood_lines + 3)
        {
	if(flood_action == 1)
	{
	uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_flooding);                
	uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_flooding);
	}
	else
	{
	uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerebanned, lp_because, lp_flooding);                
	uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_flooding);
	snprintf(tempstring, 150, "%s 0|", user->hostname);
        ballow(tempstring, BAN, user);
        snprintf(tempstring, 150, "%s 0|", user->nick);
	ballow(tempstring, NICKBAN, user);
	snprintf(tempstring, 150, "0 %s [ %s ]", user->hostname, lp_flooding);
	if(check_if_banned(user, BANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
	add_line_to_file(tempstring, path);
	}
	snprintf(tempstring, 150, "0 %s [ %s ]", user->nick, lp_flooding);
	if(check_if_banned(user, NICKBANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
	add_line_to_file(tempstring, path);
        }
	snprintf(lastbuf, 800, "%s (IP: %s): %s", user->nick, user->hostname, lp_flooding);
	last_perm(lastbuf);	
	}
	snprintf(largestring, 500, "<%s> %s %s: %s (IP: %s)|", botnick, lp_flooding, lp_detectedfrom, user->nick, user->hostname);
	send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, user);
    	user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
	return;
	}
        }

	if (((difftime(now, user->last_write) >= 20)) && (user->too_fast_write_count!=0)) user->too_fast_write_count=0;
        user->last_write = now;
}																																																														
        user->last_write = now;

if(spam_action > 0)
{
	/* Spam detector by Centurion */	
	if ((user->type == REGULAR) && ((strstr(buf, "dchub://") != NULL) || (strstr(buf, ".zapto.") != NULL)
	|| (strstr(buf, ".no-ip.") != NULL) || (strstr(buf, ".hopto.") != NULL)) && (strstr(buf, hub_hostname) == NULL))
	{
	if(spam_action == 1)
	uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_spamming);
	else
	{
	uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerebanned, lp_because, lp_spamming);
	snprintf(tempstring, 150, "%s 0|", user->hostname);
	if(check_if_banned(user, BAN) == 0)
	ballow(tempstring, BAN, user);
	snprintf(tempstring, 150, "%s 0|", user->nick);
	if(check_if_banned(user, NICKBAN) == 0)
	ballow(tempstring, NICKBAN, user);
	snprintf(largestring, 500, "0 %s [ %s ]", user->hostname, lp_spamming);
	if(check_if_banned(user, BANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
	add_line_to_file(largestring, path);
	}
	snprintf(largestring, 500, "0 %s [ %s ]", user->nick, lp_spamming);
	if(check_if_banned(user, NICKBANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
	add_line_to_file(largestring, path);
        }
	snprintf(lastbuf, 800, "%s (IP: %s): %s", user->nick, user->hostname, lp_spamming);
	last_perm(lastbuf);	
	}
	snprintf(largestring, 500, "<%s> %s %s: %s (IP: %s): %s|", botnick, lp_spamming, lp_detectedfrom, user->nick, user->hostname, buf);
	send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, user);
	user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
	return;
	}	
}
		if((strstr(buf, "is kicking") != NULL) && (strstr(buf, "because:") != NULL))
		{
		if((sscanf(buf, "<%50[^>]> %50s is kicking %50s because*", kg1user, kg2user, acttime) == 3)
		|| (sscanf(buf, "<%50[^>]> is kicking %50s because*", kg1user, acttime) == 2))
		{
		kstr = strstr(buf, "because");
		if((buser = get_human_user(acttime)) != NULL)
		{
		if((get_human_user(kg1user)->type & (KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
		{
		sscanf(kstr+7, "%500[^|]|", largestring);		
		snprintf(bigstring, 1000, "<%s> is kicking %s (IP: %s) because%s|", kg1user, buser->nick, buser->hostname, largestring);
		send_to_humans(bigstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		if((kstr = mystristr(buf, "_ban_")) != NULL)
		{
		if(sscanf(kstr+5, "%lu%1s", &nicklistban, period) == 2)
		{
		if(nicklistban != 0)
		{
		if(strcasecmp(period, "s") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_seconds);
		snprintf(tempstring, 150, "%s %lu|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %lu|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + nicklistban, buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + nicklistban, buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_seconds);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "m") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_minutes);
		snprintf(tempstring, 150, "%s %lum|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %lum|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 60), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 60), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_minutes);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "h") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_hours);
		snprintf(tempstring, 150, "%s %luh|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %luh|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 3600), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 3600), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_hours);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "d") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_days);
		snprintf(tempstring, 150, "%s %lud|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %lud|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 86400), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 86400), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_days);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		if(strcasecmp(period, "w") == 0)
		{
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s [ %lu ] %s|", botnick, lp_banning_, buser->nick, lp_fornext, nicklistban, lp_weeks);
		snprintf(tempstring, 150, "%s %luw|", buser->hostname, nicklistban);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s %luw|", buser->nick, nicklistban);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 604800), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (nicklistban * 604800), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ] [ %s %lu %s ]", buser->nick, buser->hostname, largestring, lp_kickedby, get_human_user(kg1user)->nick, lp_ban_, nicklistban, lp_weeks);        
		last_temp(lastbuf);
		if(get_human_user(kg1user)->type == KVIP)
		{
		uprintf(buser, "$To: %s From: %s $<%s> You are being kicked because: %s|", buser->nick, kg1user, kg1user, largestring);
		buser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		}
		}
		
		}
		}
		else if(((strncmp(kstr+5, "|", 1) == 0) || (strncmp(kstr+5, " ", 1) == 0)) && ((get_human_user(kg1user)->type & (OP | CHEEF | ADMIN | OWNER)) != 0))
		{
		unballow(buser->hostname, BAN);
		unballow(buser->nick, NICKBAN);
		uprintf(get_human_user(kg1user), "<%s> %s [ %s ] %s|", botnick, lp_banning_, buser->nick, lp_permanently);
		snprintf(tempstring, 150, "%s 0|", buser->hostname);
		ballow(tempstring, BAN, get_human_user(kg1user));
		snprintf(tempstring, 150, "%s 0|", buser->nick);
		ballow(tempstring, NICKBAN, get_human_user(kg1user));
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "0 %s [ %s ]", buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if(strchr(largestring, '\n') == NULL)
		{
		snprintf(bigstring, 1000, "0 %s [ %s ]", buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		snprintf(lastbuf, 800, "%s (IP: %s)%s [ %s %s ]", buser->nick, buser->hostname, largestring, lp_bannedby, get_human_user(kg1user)->nick);        
		last_perm(lastbuf);
		}
		}
		else
		{
		if((kick_bantime > 0) && (tempban != 0))
		{
		if((strchr(largestring, '\n') == NULL) && (strlen(largestring) > 2))
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (kick_bantime * 60), buser->hostname, largestring+2);
		if(check_if_banned(buser, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
		if((strchr(largestring, '\n') == NULL) && (strlen(largestring) > 2))
		{
		snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (kick_bantime * 60), buser->nick, largestring+2);
		if(check_if_banned(buser, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(bigstring, path);
		}
		}
    		}
		}		
		}
		return;
		}

		}
		}
		
		else {	
		/* And forward the message to all.  */
                if(user->too_fast_write_count<flood_lines || user->too_fast_write_count == flood_lines+3)
		{
		if(sscanf(buf, "<%50[^>]> %800[^|]", nick, msgl) <= 1)
		return;
		if((user->type == REGULAR) && (deny_main_http != 0))
		{				
		if(((strstr(buf, "http://") != NULL) || (strstr(buf, "www.") != NULL)) && (strstr(buf, hub_hostname) == NULL))
		{
		snprintf(largestring, 500, "<%s> %s %s (IP: %s): ", botnick, lp_banworddetected, user->nick, user->hostname);
		send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, NULL);
		send_to_humans(buf, OP | CHEEF | ADMIN | OWNER, NULL);
		return;
		}
		//DB// Checking if user has typed IP address to main chat
		if(strstr(buf, ".") != NULL)	
		antybuf = strstr(buf, ".");
		if(sscanf(antybuf, ".%d.%d.%d", &a, &b, &c) == 3)
		{
		snprintf(largestring, 500, "<%s> %s %s (IP: %s): ", botnick, lp_banworddetected, user->nick, user->hostname);
		send_to_humans(largestring, OP | CHEEF | ADMIN | OWNER, NULL);
		send_to_humans(buf, OP | CHEEF | ADMIN | OWNER, NULL);
		return;
		}
		}
		if((strstr(buf, "http://") != NULL) && (strlen(user->nick) > 0))
		{
		urltemp=strstr(buf, "http://");
                if(sscanf(urltemp, "%150s", tempstring) == 1)
		{
		if(tempstring[strlen(tempstring) - 1] == '|')
		tempstring[strlen(tempstring) - 1] = '\0';
		}			
        	snprintf(largestring, 500, "[ %s ]", tempstring);
		loadfile2buf(&url_mem,URL_FILE);
		if(mystristr(url_mem, largestring) == NULL)
		{
		timeinfo = localtime ( &now );
		snprintf(acttime, 50, "%s", asctime (timeinfo));
		trim_string(acttime);
		sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);		
		snprintf(largestring, 500, "%lu <-> [ %s ], %s: [ %s @ %s %s %s, %s ]", now + (expiredurl * 86400), tempstring, lp_user_, user->nick, sday, smont, syear, sdate);
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, URL_FILE);
		add_line_to_file(largestring, path);
		}
//		if(url_mem != NULL)
//		free(url_mem);
		}
                
		// Logging main chat - Centurion - 03.06.2006
		trim_string(msgl);
		if(((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (log_main_chat == 1) && (strstr(msgl, "[") == NULL) && (strchr(msgl, '\n') == NULL))
		{
		timeinfo = localtime ( &now );
		snprintf(acttime, 50, "%s", asctime (timeinfo));
		trim_string(acttime);
		sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		sdate[5] = '\0';
		if(strcmp(smont, "Jan") == 0)
		snprintf(smont, 4, "01");
		if(strcmp(smont, "Feb") == 0)
		snprintf(smont, 4, "02");
		if(strcmp(smont, "Mar") == 0)
		snprintf(smont, 4, "03");
		if(strcmp(smont, "Apr") == 0)
		snprintf(smont, 4, "04");
		if(strcmp(smont, "May") == 0)
		snprintf(smont, 4, "05");
		if(strcmp(smont, "Jun") == 0)
		snprintf(smont, 4, "06");
		if(strcmp(smont, "Jul") == 0)
		snprintf(smont, 4, "07");
		if(strcmp(smont, "Aug") == 0)
		snprintf(smont, 4, "08");
		if(strcmp(smont, "Sep") == 0)
		snprintf(smont, 4, "09");
		if(strcmp(smont, "Oct") == 0)
		snprintf(smont, 4, "10");
		if(strcmp(smont, "Nov") == 0)
		snprintf(smont, 4, "11");
		if(strcmp(smont, "Dec") == 0)
		snprintf(smont, 4, "12");
		if(strlen(sday) == 1)
		snprintf(acttime, 50, "%s-%s-0%s", syear, smont, sday); 
		else
		snprintf(acttime, 50, "%s-%s-%s", syear, smont, sday); 
		create_dailylog(acttime);
		snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, LOG_DIR, acttime);
		snprintf(bigstring, 4096, "[%s] <%s> %s\n", sdate, nick, msgl); 
		add_line_to_file(bigstring, path);
		}
		
		// last 10 main chat msgs
		if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
		{
		timeinfo = localtime ( &now );
		snprintf(acttime, 50, "%s", asctime (timeinfo));
		trim_string(acttime);
		sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		
		snprintf(message20, 400, "%s", message19);
		snprintf(message19, 400, "%s", message18);
		snprintf(message18, 400, "%s", message17);
		snprintf(message17, 400, "%s", message16);
		snprintf(message16, 400, "%s", message15);
		snprintf(message15, 400, "%s", message14);
		snprintf(message14, 400, "%s", message13);
		snprintf(message13, 400, "%s", message12);
		snprintf(message12, 400, "%s", message11);
		snprintf(message11, 400, "%s", message10);
		snprintf(message10, 400, "%s", message9);
		snprintf(message9, 400, "%s", message8);
		snprintf(message8, 400, "%s", message7);
		snprintf(message7, 400, "%s", message6);
		snprintf(message6, 400, "%s", message5);
		snprintf(message5, 400, "%s", message4);
		snprintf(message4, 400, "%s", message3);
		snprintf(message3, 400, "%s", message2);
		snprintf(message2, 400, "%s", message1);
		if(strstr(msgl, "--- [") == NULL)
		{
		if(strlen(msgl) <= 256)
		{
		snprintf(message1, 400, "--- [%s] <%s> %s", sdate, user->nick, msgl);
		}
		else
		{
		msgl[255] = '\0';
		snprintf(message1, 400, "--- [%s] <%s> %s [%s...]", sdate, user->nick, msgl, lp_cut_);
		}
		}
		}
		//DB// Cutting too long main chat messages
		//DB// First we have to not allow to cut nicknames, 100 is minimum
		if((cut_string_to < 100) && (cut_string_to != 0))
		cut_string_to = 100;
		
		if(cut_string_to > 0)
		{
		if(((user->type == REGULAR) || (((user->type & (PROTECTED | REG | VIP | KVIP)) != 0) && (cut_reg_msg != 0)))		
		&& ((strlen(buf) - strlen(user->nick) - 3) > cut_string_to))
		{
		buf[cut_string_to] = '\0';
		snprintf(tempstring, 150, "\r\n--- [ %s... ] ---|", lp_cut_);
		if(no_chat_for_chatgagged == 0)
		{
		send_to_humans(buf, REGULAR | PROTECTED | REG| VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		send_to_humans(tempstring, REGULAR | PROTECTED | REG| VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			{
			send_to_user(buf, human_user->user);
			send_to_user(tempstring, human_user->user);
			}
			human_user = next_human;
			}
		}
		}
		else
		{
		if(no_chat_for_chatgagged == 0)
		send_to_humans(buf, REGULAR | PROTECTED | REG| VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			send_to_user(buf, human_user->user);
			human_user = next_human;
			}
		}		
		}
		}
		else
		{
		if(no_chat_for_chatgagged == 0)
		send_to_humans(buf, REGULAR | PROTECTED | REG| VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			send_to_user(buf, human_user->user);
			human_user = next_human;
			}
		}		
		}
		}
		}
	}
}

void give_op(char *buf, struct user_t *user)
{
struct user_t *opus;
char snick[51];
struct sock_t *human_user;
struct sock_t *next_human;
char *op_list;

sscanf(buf, "%50[^|]|", snick);
if((opus = get_human_user(snick)) != NULL)
{
remove_user_from_list(opus->nick);
if(check_if_registered(opus->nick) == 0)
{
opus->type = OP;
opus->tmpop = 1;
}
else if((check_if_registered(opus->nick) > 0) && (check_if_registered(opus->nick) < 5))
{
opus->wasreg = 1;
opus->type = OP;
opus->tmpop = 1;
}
else {
uprintf(user, "<%s> '%s' %s|", botnick, snick, lp_isalreadyopped); 
return;
}

add_user_to_list(opus);

human_user = human_sock_list;
op_list = get_op_list();
while(human_user != NULL)
{
next_human = human_user->next;
send_to_user(op_list, human_user->user);
human_user = next_human;
}
free(op_list);
uprintf(user, "<%s> %s '%s'|", botnick, lp_givingtempop, opus->nick); 
uprintf(opus, "<%s> %s '%s'|", botnick, lp_youweretempoppedby, user->nick); 
}
else 
{
uprintf(user, "<%s> '%s' %s|", botnick, snick, lp_isoffline); 
}
}

//This function is use for block main chat, pm, search or downloading
void block_proc (char *buf)
{
char send_buf[351];
int type;
send_buf[0] = '\0';		
type=0;


if (strncasecmp (buf-7,"!",1) == 0) type=1;
		
if (strncasecmp (buf,"main",4) == 0)
 {
 block_main=1;
if (type == 1) { 
                      send_to_user("$Set block_main 1|", non_human_user_list);
snprintf(send_buf, 350, "<%s> %s|", botnick, lp_mainchatoff);					    
if (strlen(topic)!=0) 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s - %s (%s)|", hub_name, topic, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s - %s|", hub_name, topic);
}
else 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s (%s)|", hub_name, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s|", hub_name);
}
send_to_humans(set_topic, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);

  }
     else { block_main=0;
		      send_to_user("$Set block_main 0|", non_human_user_list);
snprintf(send_buf, 350, "<%s> %s|", botnick, lp_mainchaton);
if (strlen(topic)!=0) 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s - %s (%s)|", hub_name, topic, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s - %s|", hub_name, topic);
}
else 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s (%s)|", hub_name, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s|", hub_name);
}

send_to_humans(set_topic, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);

}
	      }
	      else if (strncasecmp (buf,"search",6) == 0)
	        {
		   block_search=1;
		      if (type == 1) {
		      send_to_user("$Set block_search 1|", non_human_user_list);
snprintf(send_buf, 350, "<%s> %s|", botnick, lp_searchoff);					    
	         }
	    else { block_search=0;
	    send_to_user("$Set block_search 0|", non_human_user_list);
snprintf(send_buf, 350, "<%s> %s|", botnick, lp_searchon);
	       }
	         }
		 else if (strncasecmp (buf,"pm",2) == 0)
		   {
		      block_pm=1;
	         if (type == 1) {
		 send_to_user("$Set block_pm 1|", non_human_user_list);
snprintf(send_buf, 350, "<%s> %s|", botnick, lp_pmoff);					    
		    }
	       else { block_pm=0;
	       send_to_user("$Set block_pm 0|", non_human_user_list);
snprintf(send_buf, 350, "<%s> %s|", botnick, lp_pmon);
          }
		    }
	    else if (strncasecmp (buf,"download",8) == 0)
	     {
		       block_download=1;
         if (type == 1) {
	 send_to_user("$Set block_download 1|", non_human_user_list);
	 snprintf(send_buf, 350, "<%s> %s|", botnick, lp_downloadoff);
	   }
     else { block_download=0;
     send_to_user("$Set block_download 0|", non_human_user_list);
snprintf(send_buf, 350, "<%s> %s|", botnick, lp_downloadon);
   }
    }
			
send_to_humans(send_buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
return;
}
												

/* Forwards request from one user to another, 
$RevConnectToMe requesting_user requested_user| i.e, the other way around if you compare it
with $ConnectToMe */
void rev_connect_to_me(char *buf, struct user_t *user)
{
	char command[21];
	char requesting[MAX_NICK_LEN+1];
	char requested[MAX_NICK_LEN+1];
	struct user_t *to_user;


	if(sscanf(buf, "%20s %50s %50[^|]|", command, requesting, requested) != 3)
	{                                                                           
		logprintf(4, "Received bad $RevConnectToMe command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}

	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
		if(requested[0] == '\0')
		{	                                                                               
			logprintf(4, "Received bad $RevConnectToMe command from %s at %s:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}
		if((strncmp(requesting, user->nick, strlen(requesting)) != 0) 
			|| (strlen(requesting) != strlen(user->nick)))
		{	                                                                                   
			logprintf(3, "User %s at %s claims to be someone else in $RevConnectToMe:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(3, "%s\n", buf);
			else
				logprintf(3, "too large buf\n");
			user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return;
		}
	}

	/* And forward to requested user.  */
	if((to_user = get_human_user(requested)) != NULL)
	{
	if(restrictions > 0)
	{
	if(restrictions == 1)
	{
	if(user->type < to_user->type)
	uprintf(user, "<%s> %s: %s|", botnick, lp_restrictions1, to_user->nick); 
	}
	else if(restrictions == 2)
	{
	if((user->type == REGULAR) && (to_user->type != REGULAR))
	uprintf(user, "<%s> %s: %s|", botnick, lp_restrictions2, to_user->nick); 
	else
	send_to_user(buf, to_user);
	}
	}
	else
	send_to_user(buf, to_user);
	}
}

/* Forwards request from one user to another. The command has the following fomat:
$ConnectToMe requested_user requesting_ip:requesting_port */
void connect_to_me(char *buf, struct user_t *user)
{
	char command[21];
	char requested[MAX_NICK_LEN+1];
	char ip[MAX_HOST_LEN+1];
	unsigned int port;
	struct user_t *to_user;

	if(sscanf(buf, "%20s %50s %121[^:]:%u|", command, requested, ip, &port) != 4)
	{                                                                        
		logprintf(4, "Received bad $ConnectToMe command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}

	/* Validation check */
	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
		if(port == 0)
		{	                                                                            
			logprintf(4, "Received bad $ConnectToMe command from %s at %s:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}
	}

	/* And forward to requested user */
	if((to_user = get_human_user(requested)) != NULL)
	{
	if(restrictions > 0)
	{
	if(restrictions == 1)
	{
	if(user->type < to_user->type)
	uprintf(user, "<%s> %s: %s|", botnick, lp_restrictions1, to_user->nick); 
	}
	else if(restrictions == 2)
	{
	if((user->type == REGULAR) && (to_user->type != REGULAR))
	uprintf(user, "<%s> %s: %s|", botnick, lp_restrictions2, to_user->nick); 
	else
	send_to_user(buf, to_user);
	}
	}
	else
	send_to_user(buf, to_user);
	}
}

/* Send message from user to specified user, has the following format:
* $To: tonick From: fromnick $message string| */
void to_from(char *buf, struct user_t *user)
{
	char command[6];
        char fake_buf[1001];
	char temp_str[401];
	time_t now;
	char str_time_to_unban[256];
	struct user_t *to_user;
	char fromnick[MAX_NICK_LEN+1];
	char path[MAX_FDP_LEN+1];
	char tonick[MAX_NICK_LEN+1];
	char chatnick[MAX_NICK_LEN+1];
	char message[601];
        char setmessage[602];
	struct sock_t *human_user;
	struct sock_t *next_human;
	now = time(NULL);

	if(sscanf(buf, "%5s %50s From: %50s $<%50[^>]> %600[^|]|", command, tonick, fromnick, chatnick, message) != 5)
	{                                                                
		logprintf(4, "Received bad $To command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}
    	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
		if(message[0] == '\0')
		{	                                                                    
			logprintf(4, "Received bad $To command from %s at %s:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}
		if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0)
		{	     
			if(((strncmp(fromnick, user->nick, strlen(fromnick)) != 0) 
				|| (strlen(fromnick) != strlen(user->nick))) 
				|| ((strncmp(chatnick, user->nick, strlen(fromnick)) != 0) 
				|| (strlen(chatnick) != strlen(user->nick))))
			{	                                                                   	                        
				logprintf(3, "User %s at %s claims to be someone else in $To:\n", user->nick, user->hostname);
				if(strlen(buf) < 3500)
					logprintf(3, "%s\n", buf);
				else
					logprintf(3, "too large buf\n");
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
				return;
			}
		}	
	}

        if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && ((strcasecmp(tonick, opchatnick) == 0)))
	{
	human_user = human_sock_list;
        while(human_user != NULL)
	{
	next_human = human_user->next;
	if(((human_user->user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && ((strncasecmp(human_user->user->nick, user->nick, strlen(user->nick)) != 0)))
	uprintf(human_user->user, "$To: %s From: %s $<%s> %s|", human_user->user->nick, opchatnick, user->nick, message);		

	human_user = next_human;
	}
	}
        if(strcmp(tonick, botnick) == 0)
	{             
	snprintf(setmessage, 601, "%s|", message);
	if(is_command(setmessage, user, 1) == 1)
	return;
	}
	
	/* And forward the message to specified user.  */
	if((to_user = get_human_user(tonick)) != NULL)
	{
	  /* gagged users, only can send pvt msgs to op and/or admin users */

	  	
		if((user->autoawaytime > 0) && (user->away_flag == 2) && (strstr(buf, "are being kicked") == NULL))
		{
                uprintf(user, "<%s> %s|", botnick, lp_autoawaymodeoff);			    	     
		if(check_if_away(user->nick) == 1)
		{
		time2str(difftime(now, time_to_unban), str_time_to_unban);
		snprintf(temp_str, 400, "<----> %s %s %s %s|", user->nick, lp_isback, lp_after, str_time_to_unban);
		}
		else
		snprintf(temp_str, 400, "<----> %s %s|", user->nick, lp_isback);
		remove_away_user(user);
		user->away_flag=1;
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return;
		}
		if(no_chat_for_chatgagged == 0)
		send_to_humans(temp_str, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			send_to_user(temp_str, human_user->user);
			human_user = next_human;
			}
		}
		}

		if((to_user->type != SCRIPT) && (check_if_away(to_user->nick) == 1))
		{
		time2str(difftime(now, time_to_unban), str_time_to_unban);
		uprintf(user, "$To: %s From: %s $<%s> [ %s ]  %s %s, %s: %s|", user->nick, to_user->nick, to_user->nick, lp_awaymodeon, lp_awayfor, str_time_to_unban, lp_reason, away_reason);		
		}
		if ((deny_pm_http > 0) && (user->type == REGULAR) && ((strstr(buf,"http://") != NULL) || (strstr(buf,"www.") != NULL)))
		{
		if(deny_pm_http == 1)
		{
		uprintf(user, "<%s> %s %s: http:// %s|", botnick, lp_youwerekicked, lp_because, lp_spamming);
		uprintf(user, "$To: %s From: %s $<%s> %s %s: http:// %s.|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_spamming);
snprintf(fake_buf, 1000, "<%s> http:// %s %s: %s (IP: %s): <%s> %s|", botnick, lp_spamming, lp_detectedfrom, user->nick, user->hostname, chatnick, message);
send_to_humans(fake_buf, OP | CHEEF | ADMIN | OWNER, user);
user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
return;
}		
else if(deny_pm_http == 2)
{
if(strcmp(pmspammer, user->nick) == 0)
pmspam = pmspam + 1;
else
pmspam = 1;
snprintf(pmspammer, 50, "%s", user->nick);
if(pmspam < 5)
{
snprintf(fake_buf, 1000, "<%s> http:// %s %s: %s (IP: %s): <%s> %s (%s: %d)|", botnick, lp_spamming, lp_detectedfrom, user->nick, user->hostname, chatnick, message, lp_message_, pmspam);
send_to_humans(fake_buf, OP | CHEEF | ADMIN | OWNER, NULL);
}
else if(pmspam >= 5)
{
		snprintf(temp_str, 400, "%s %dm", user->hostname, 0);
		if(check_if_banned(user, BAN) == 0)
		ballow(temp_str, BAN, user);
		snprintf(temp_str, 400, "%s %dm", user->nick, 0);
		if(check_if_banned(user, NICKBAN) == 0)
		ballow(temp_str, NICKBAN, user);
		snprintf(temp_str, 400, "0 %s [ http:// %s ]", user->hostname, lp_spamming);
		if(check_if_banned(user, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(temp_str, path);
		}
		snprintf(temp_str, 400, "0 %s [ http:// %s ]", user->nick, lp_spamming);
		if(check_if_banned(user, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(temp_str, path);
		}
		snprintf(lastbuf, 800, "%s (IP: %s): http:// %s", user->nick, user->hostname, lp_spamming);
		last_perm(lastbuf);	

		uprintf(user, "<%s> %s %s: http:// %s|", botnick, lp_youwerebanned, lp_because, lp_spamming);
		uprintf(user, "$To: %s From: %s $<%s> %s %s: http:// %s|", user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_spamming);

snprintf(fake_buf, 1000, "<%s> http:// %s %s: %s (IP: %s): <%s> %s (%s: %d) [ %s ]|", botnick, lp_spamming, lp_detectedfrom, user->nick, user->hostname, chatnick, message, lp_message_, pmspam, lp_ban_);
send_to_humans(fake_buf, OP | CHEEF | ADMIN | OWNER, NULL);
user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
pmspam = 0;
return;
}
}
}
		if(spam_action > 0)
		{
	  	if ((user->type == REGULAR) && ((strstr(buf,"dchub://") != NULL) || (strstr(buf,".no-ip.") != NULL)
		|| (strstr(buf,".zapto.") != NULL) || (strstr(buf,".hopto.") != NULL)) && (strstr(buf, hub_hostname) == NULL))
		{
		if(spam_action == 1)
		{
		uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_spamming);
		uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_spamming);
		}
		else
		{
		uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerebanned, lp_because, lp_spamming);
		uprintf(user, "$To: %s From: %s $<%s> %s %s %s|", user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_spamming);
		snprintf(temp_str, 400, "%s %dm", user->hostname, 0);
		if(check_if_banned(user, BAN) == 0)
		ballow(temp_str, BAN, user);
		snprintf(temp_str, 400, "%s %dm", user->nick, 0);
		if(check_if_banned(user, NICKBAN) == 0)
		ballow(temp_str, NICKBAN, user);
		snprintf(temp_str, 400, "0 %s [ %s ]", user->hostname, lp_spamming);
		if(check_if_banned(user, BANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
		add_line_to_file(temp_str, path);
		}
		snprintf(temp_str, 400, "0 %s [ %s ]", user->nick, lp_spamming);
		if(check_if_banned(user, NICKBANRS) == 0)
		{
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
		add_line_to_file(temp_str, path);
		}
		snprintf(lastbuf, 800, "%s (IP: %s): %s", user->nick, user->hostname, lp_spamming);
		last_perm(lastbuf);	
		}
		snprintf(fake_buf, 1000, "<%s> %s %s: %s (IP: %s): <%s> %s|", botnick, lp_spamming, lp_detectedfrom, user->nick, user->hostname, chatnick, message);
		send_to_humans(fake_buf, OP | CHEEF | ADMIN | OWNER, user);
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return;
		}
		}	

		if ((slotbeg_kicker == 1) && (user->type == REGULAR) && ((strstr(buf, "slot") != NULL)))
		{
		uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_slotbegging);
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return;
		}
	  
	  
	  else
	  {
	    /* default behavior... */
	    if((user->type != SCRIPT) && (user->gagc != 0))
	    {
	    if((to_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
	    {
	    	accepted_pm++;
		send_to_user(buf, to_user);
	    }
			else
			{
			blocked_pm++;
				buf = NULL;
    			if((user->type == REGULAR) && ((mute_unregistered == 1) || (mute_unregistered == 3)))
            		{
			uprintf(user, "$To: %s From: %s $<%s> %s. %s|", user->nick, to_user->nick, botnick, lp_haveregistertochat, lp_canonlypmtoops);
			uprintf(user, "<%s> %s. %s|", botnick, lp_haveregistertochat, lp_canonlypmtoops);
			}
			else
		        {
			uprintf(user, "$To: %s From: %s $<%s> %s. %s|", user->nick, to_user->nick, botnick, lp_notallowedtochat, lp_canonlypmtoops);
			uprintf(user, "<%s> %s. %s|", botnick, lp_notallowedtochat, lp_canonlypmtoops);
			}    

				
			}
	  }
	else 
	{
		now = time(NULL);
		user->last_pm_write = now;  
		accepted_pm++;
		send_to_user(buf, to_user);
	}
		}
	}
}

/* Handles the MyINFO command. Returns 0 if user should be removed. 
* Has the following format:
* $MyINFO $ALL nickname filedescription$ $connection type$email$sharesize$| 
* Since some of these variables can be empty, I havent used sscanf which 
* makes this function a little bit hard to follow.  */
int my_info(char *org_buf, struct user_t *user)
{
	int i, k, ret, m, allusers;
	int desc_too_long = 0;
	int email_too_long = 0;	
	char acttime[51];
	char *buf;
	struct tm * timeinfo;
	char *op_list;
	char *send_buf;
	char kbstring[501];
	struct sock_t *human_user;
	struct sock_t *next_human;
	char usertype[51];
	char ban_command[151];
	char temp_size[50];
	char to_nick[51];
	char temp_nick[51];
	char path[MAX_FDP_LEN+1];
	struct user_t *to_user;
	char share_buf[21];
	int thesameip;
        char sdayn[6];
	char sday[4];
	char sdate[11];
	char syear[6];
	char smont[6];					
	long long allshare;
	long long prshare;
	char *prcontype;
	struct user_t *save_user = NULL;
	int new_user = 0;  
        time_t now;

	prshare = 0;
	prcontype = NULL;

	buf = org_buf + 9;

	/* Check if message is for all or for a specific user */
	if(strncmp(buf, "ALL ", 4) == 0)
	{
		buf += 4;
		if(*user->nick == '\0')
			return 0;
		if((check_if_registered(user->nick) != 0)
			&& ((user->type & (NON_LOGGED | REGULAR)) != 0)) 
		{
			logprintf(1, "User at %s tried to log in with registered nick without providing password, kicking user\n", user->hostname);
			return 0;
		} else if((strlen(default_pass) > 0)
			&& ((user -> type & (NON_LOGGED)) != 0))
		{
			logprintf(1, "User at %s tried to log in with %s without providing default password, kicking user\n", user->hostname, user->nick);
			return 0;
		}
	}
	else
	{	
		/* It's not $MyINFO $ALL, but $MyINFO to_nick, so send $MyINFO $ALL to
		* the specified user in to_nick.  */
		i = cut_string(buf, ' ');
		if((i == -1) || (i>50) || (user->type != FORKED))
			return -1;

		strncpy(to_nick, buf, i);
		to_nick[i] = '\0';
		buf += (i + 1);

		/* Check if the destination user is in this process */
		if(((to_user = get_human_user(to_nick)) != NULL) 
			|| (strncmp(to_nick, "$Script", 7) == 0))
		{
			if((send_buf = malloc(sizeof(char) * (strlen(buf) + 14))) == NULL)
			{
				logprintf(0, "Error - In my_info()/malloc(): %s", strerror(errno));
				quit = 1;
				return -1;
			}
			strcpy(send_buf, "$MyINFO $ALL ");
			strcat(send_buf, buf);
			/* If it's the $Script user, send to all scripts.  */
			if((strncmp(to_nick, "$Script", 7) == 0) && (pid > 0))
				send_to_non_humans(send_buf, SCRIPT, user);
			/* Otherwise, send to the specified user.  */
			else
				send_to_user(send_buf, to_user);
			free(send_buf);
		}

		return 1;
	}  

	/* If the user was NON_LOGGED before, or if the flag was 0, it's the first 
	* time the user sends $MyINFO $ALL.  */

	if(((user->type & (NON_LOGGED | REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (user->con_type == NULL))
		new_user = 1;
	/* First set users variables */
	if(((i = cut_string(buf, ' ')) == -1)
		|| cut_string(buf, ' ') > cut_string(buf, '$'))
		return 0;

	sscanf(buf, "%50s", temp_nick);

	/* If we are a script process, temporary save the parent process user.  */
	if(pid == -1)
	{
		save_user = user;
		if((user = get_human_user(temp_nick)) == NULL)
			return -1;
	}   

	/* Make sure that user isn't on the user list already. This could only
	* happen if a user first sends ValidateNick, then the process forks, and
	* after that the user sends MyINFO $ALL.  */
	if(user->type == NON_LOGGED)
	{		
		if((check_if_on_user_list(temp_nick)) != NULL)
			return 0;
	}

	/* If the command was sent from a human, make sure that the provided nick 
	* matches the one provided with $ValidateNick.  */
	if((user->type & (NON_LOGGED | REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
		if((strncmp(temp_nick, user->nick, strlen(user->nick)) != 0)
			|| (strlen(temp_nick) != strlen(user->nick)))
		{
			logprintf(3, "User from %s provided a nick in $MyINFO that does not match the one from $ValidateNick, removing user.\n", user->hostname);
			return 0;
		}
	}
	
	buf = buf + i + 1;

	if(user->desc != NULL)
	{
	free(user->desc);
	user->desc = NULL;
	}

	if(*buf != '$')
	{
		k = cut_string(buf, '$');
		if(k <= 120)
		{
			if((user->desc = (char *) malloc(sizeof(char) * (k + 1))) == NULL)
			{
				logprintf(0, "Error - In my_info()/malloc(): %s", strerror(errno));
				quit = 1;
				return -1;
			}
			strncpy(user->desc, buf, k);
			user->desc[k] = '\0';
		}
		else
			desc_too_long = 1;
		buf = buf + k + 1;
	}
	buf++;

	/* Not sure if the next argument is ever set to anything else than a 
	* blankspace. Skipping it for now.  */
	if((i = cut_string(buf, '$')) == -1)
		return 0;

	buf = buf + i + 1;

	/* Get connection of user */
	if((i = cut_string(buf, '$')) == -1)
		return 0;

	if(user->con_type != NULL)
	prcontype = user->con_type;

	if(user->con_type != NULL)
	{
		free(user->con_type);
		user->con_type = NULL;
	}

	if(buf[0] != '$')
	{
		m = cut_string(buf, '$');
			if((user->con_type = (char *) malloc(sizeof(char) * (m + 1))) == NULL)
			{
				logprintf(0, "Error - In my_info()/malloc(): %s", strerror(errno));
				quit = 1;
				return -1;
			}
			user->flag = (int)buf[m-1];
			strncpy(user->con_type, buf, m-1);
			user->con_type[m-1] = '\0';
	}
	buf = buf + i + 1;
if(user->con_type != NULL)
{
if(prcontype != user->con_type)
newmyinfo = 1;	
}
	if((i = cut_string(buf, '$')) == -1)
		return 0;

	if(user->email != NULL)
	{
		free(user->email);
		user->email = NULL;
	}

	/* Set email.  */
	if(buf[0] != '$')
	{
		k = cut_string(buf, '$');
		if(k <= 50)
		{
			if((user->email = (char *) malloc(sizeof(char) * (k + 1))) == NULL)
			{
				logprintf(0, "Error - In my_info()/malloc(): %s", strerror(errno));
				quit = 1;
				return -1;
			}
			strncpy(user->email, buf, k);
			user->email[k] = '\0';
		}
		else
			email_too_long = 1;
	}
	buf = buf + i + 1;

	/* Parse share size.  */
	if((i = cut_string(buf, '$')) == -1)
		return 0;

	/* If a user has uploaded share size before, we'll have to subtract the 
	* old share from the total share first.  */



	if(((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) 
		&& (user->share > 0) && (save_user == NULL))
	{
	    total_share -= user->share;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif
	}

	prshare = user->share;

	/* If the size of users share is a number with more than 20 digits, 
	* something must be wrong */
	if(i>20)
		return 0;

	memset(temp_size, 0, sizeof(temp_size));
	if(*buf != '$')
	{

		strncpy(temp_size, buf, i);
	user->share = strtoll(temp_size,(char **)NULL, 10);
	}
	
	else
		user->share = 0;
	//DB// Detecting basic fake share, reporting to opped users and ip/nick-banning
	snprintf(share_buf, 20, "%llu", user->share);
	
	if(((user->type & (NON_LOGGED | REGULAR)) != 0) && ((strncmp(share_buf, "1111111", 7) == 0)
	|| (strncmp(share_buf, "2222222", 7) == 0) || (strncmp(share_buf, "3333333", 7) == 0) ||
	(strncmp(share_buf, "4444444", 7) == 0) || (strncmp(share_buf, "5555555", 7) == 0) ||
	(strncmp(share_buf, "6666666", 7) == 0) || (strncmp(share_buf, "7777777", 7) == 0) ||
	(strncmp(share_buf, "8888888", 7) == 0) || (strncmp(share_buf, "9999999", 7) == 0) ||
	(strncmp(share_buf, "1234567", 7) == 0) || (strncmp(share_buf, "9876543", 7) == 0)
	|| (strncmp(share_buf, "0000000", 7) == 0))) 
	{
    	uprintf(user, "<%s> %s %s: %s ( %llu %s )|", botnick, lp_youwerebanned, lp_because, lp_fakeshare, user->share, lp_bytes);

	snprintf(ban_command, 150, "%s 0|", user->hostname);
	ballow(ban_command, BAN, user);
	snprintf(ban_command, 150, "%s 0|", user->nick);
	ballow(ban_command, NICKBAN, user);
	snprintf(ban_command, 150, "0 %s [ %s ]", user->hostname, lp_fakeshare);
	if(check_if_banned(user, BANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
	add_line_to_file(ban_command, path);
	}
	snprintf(ban_command, 150, "0 %s [ %s ]", user->nick, lp_fakeshare);
	if(check_if_banned(user, NICKBANRS) == 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
	add_line_to_file(ban_command, path);
    	}
	snprintf(lastbuf, 800, "%s (IP: %s): %s: %llu %s", user->nick, user->hostname, lp_fakeshare, user->share, lp_bytes);        
	last_perm(lastbuf);

	snprintf(kbstring, 500, "<%s> %s %s: %s (IP: %s): %llu %s|", botnick, lp_fakeshare, lp_detectedfrom, user->nick, user->hostname, user->share, lp_bytes);
	send_to_humans(kbstring, OP | CHEEF | ADMIN | OWNER, user);
	return 0;

	}
	
	if(user->share != prshare)
	newmyinfo = 1;	
	
	/* Switch back to the parent process user.  */
	if(save_user != NULL)
		user = save_user;

	/* Check if user is sharing enough.  */
	/* Op:s don't have to meet the requirement for now. May be optional in 
	* the future.  */

	if((user->type & (NON_LOGGED | REGULAR | PROTECTED | REG | VIP | KVIP | OP)) != 0)
	{
	if(check_share(user) == 1)
	{
	//DB// If this is an old user, who has unshared some data we need to zero his share, to avoid total_share mismatch
	if(new_user == 0)
	user->share = 0;
	return 0;
	}
	}
	/* Disconnect user if email or descriptions are too long */
	if(desc_too_long != 0)
	{
		uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_desctoolong);
		uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_desctoolong);
    		snprintf(kbstring, 500, "[ %s ] [ %s  ] [ %s (IP: %s) ]", lp_kick_, lp_desctoolong, user->nick, user->hostname);
	        ban_info(kbstring);
		
		return 0;
	}
	if(email_too_long != 0)
	{
		uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_mailtoolong);
		uprintf(user, "$To: %s From: %s $<%s> %s %s: %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_mailtoolong);
    		snprintf(kbstring, 500, "[ %s ] [ %s  ] [ %s (IP: %s) ]", lp_kick_, lp_mailtoolong, user->nick, user->hostname);
	        ban_info(kbstring);
		return 0;
	}

	if((max_regular_from_one_ip > 0) && (user->type == NON_LOGGED))
	{
	thesameip = 0;
	human_user = human_sock_list; 
	while(human_user != NULL) 
	{ 
	next_human = human_user->next; 
	if(human_user->user->type == REGULAR)
	{
	if(strcmp(user->hostname, human_user->user->hostname) == 0)
	thesameip++;
	}
	human_user = next_human; 
	}
	if(thesameip > max_regular_from_one_ip)
	{
	uprintf(user, "<%s> %s %s: %s (%d)|", botnick, lp_youwerekicked, lp_because, lp_thesameip, thesameip);
	uprintf(user, "$To: %s From: %s $<%s> %s %s: %s (%d)|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_thesameip, thesameip);
	snprintf(ban_command, 150, "%s 300|", user->hostname); 
	ballow(ban_command, BAN, user); 
	snprintf(ban_command, 150, "$Kick %s|", user->nick);
	kick(ban_command, NULL, 0);
	snprintf(kbstring, 500, "<%s> %s (%d): %s (IP: %s)|", botnick, lp_thesameip, thesameip, user->nick, user->hostname);
	send_to_humans(kbstring, OP | CHEEF | ADMIN | OWNER, user);
	return 0;
	}	
	}

	/* By now, the user should have passed all tests and therefore be counted
	* as logged in.  */
	if(user->type == NON_LOGGED)
	{	
	user->type = REGULAR;
	totalvisitors++;
	total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif
	
	
	logprintf(1, "%s logged in from %s\n", user->nick, user->hostname);
	}
	
	
	/* Add share to total_share.  */
	if((user->type & (FORKED | SCRIPT)) == 0)
	{
	total_share += user->share;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

	}
	/* To scripts, also send the info not covered by MyINFO.  */     
#ifdef HAVE_PERL
	if((new_user != 0) 
		&& ((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0))
                command_to_scripts("$Script user_info %s %s %llu|", user->nick,                         
	        user->hostname, user->share);
#endif

        if((new_user != 0) && ((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0))
	{
	add_human_to_hash(user);

	    if((check_if_ipgagged(user) == 1) && (check_if_registered(temp_nick) < 7))
	    {
    		if((ipgag_type == 0) || (ipgag_type == 2))
		{
		user->gagc = 1;
		}
		if((ipgag_type == 1) || (ipgag_type == 2))
		{
		user->gags = 1;
		user->gagd = 1;
		}
		uprintf(user, "$To: %s From: %s $<%s> %s ", user->nick, botnick, botnick, lp_youweregagged);
		if(ipgag_type == 0)
		uprintf(user, "%s", lp_chatting_);
		else if(ipgag_type == 1)
		uprintf(user, "%s, %s", lp_downloading_, lp_searching_);
		else
		uprintf(user, "%s, %s, %s", lp_chatting_, lp_downloading_, lp_searching_);
		if(check_if_banned(user, IPGAGRS) == 1)
		uprintf(user, " %s: %s", lp_because, ipgagreason);
		uprintf(user, "|");
	    }
	    if(user->type == REGULAR)
	    {
		if(mute_unregistered == 1)
		{	
		user->gagc = 1;
		user->gagd = 0;
		user->gags = 0;
		}
		else if(mute_unregistered == 2)
		{	
		user->gagc = 0;
		user->gagd = 1;
		user->gags = 1;
		}
		else if(mute_unregistered == 3)
		{	
		user->gagc = 1;
		user->gagd = 1;
		user->gags = 1;
		}
	    }
	}
	
	allshare = total_share;
	allusers = total_users;

	if(allshare > recordshare)
	{
	recordshare = allshare;
        now = time(NULL);
        timeinfo = localtime ( &now );
        snprintf(acttime, 50, "%s", asctime (timeinfo));
        trim_string(acttime);
        sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
        snprintf(shdatetime, 100, "%s", acttime);
        snprintf(recordshstr, 119, "%llu %s %s %s %s, %s", recordshare, sdayn, sday, smont, syear, sdate);
	brokensr = 1;
	}
	if(allusers > recordusers)
	{
	recordusers = allusers;
        now = time(NULL);
        timeinfo = localtime ( &now );
        snprintf(acttime, 50, "%s", asctime (timeinfo));
        trim_string(acttime);
        sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
        snprintf(usdatetime, 100, "%s", acttime);
	snprintf(recordusstr, 119, "%d %s %s %s %s, %s", recordusers, sdayn, sday, smont, syear, sdate);
	brokenur = 1;
	}
	
	/* And then send the MyINFO string. */
	if(new_user == 1)
	{
	newmyinfo = 1;
	if(check_if_away(user->nick) == 1)
	{
	user->away_flag = 2;
	check_if_autoaway(user->nick);
	}
	else
	check_if_autoaway(user->nick);

	if(getinfolater == 1)
	{
	getinfolater = 0;
	
	uprintf(user, "$MyINFO $ALL %s [BOT] Hub-Security <DB V:%s,M:A,H:0/0/1,S:0>$ $$hub-security$0$|", botnick, VERSION);
	uprintf(user, "$MyINFO $ALL %s [BOT] Op Chat <DB V:%s,M:A,H:0/0/1,S:0>$ $$opchat$0$|", opchatnick, VERSION);
	if(clockbot == 1)
	uprintf(user, "$MyINFO $ALL %s [BOT] Server Time <DB V:%s,M:A,H:0/0/1,S:0>$ $$clock$0$|", clocknick, VERSION);
	send_myinfo(user);
	} 			
	
if(strcasecmp(user->nick, botinfolater) == 0)
{
	if(strlen(topic) > 0)                                                                              
	uprintf(user, "$HubINFO %s-%s$%s:%d$[DB Hub V:%s] %s$%d$%llu$%d$%d$DB Hub$http://dbhub.ir.pl|", hub_name, topic, hub_hostname, listening_port, VERSION, hub_description, max_users, min_share, minslots, maxhubs);                                                                                
        else                                                                                               
        uprintf(user, "$HubINFO %s$%s:%d$[DB Hub V:%s] %s$%d$%llu$%d$%d$DB Hub$http://dbhub.ir.pl|", hub_name, hub_hostname, listening_port, VERSION, hub_description, max_users, min_share, minslots, maxhubs);                                                                                          
}             
if((check_if_registered(user->nick) >= 5) && (user->hide_share != 2))
{
	op_list = get_op_list();                                                                                                   
	send_to_humans(op_list, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	free(op_list);                                                                                                             
}
}

if(user->type != SCRIPT)
{
if(send_user_info(user, user->version) == 0)
{
newmyinfo = 0;
return 0;
}
}
newmyinfo = 0;

if(new_user == 1)
{
if(user->hide_share != 2)
{	
snprintf(ban_command, 150, "$UserIP %s %s$$||", user->nick, user->hostname);
send_to_humans(ban_command, OP | CHEEF | ADMIN | OWNER, NULL);
}
if(show_joinsparts > 0)
{
if(user->type == OWNER)
snprintf(usertype, 50, "%s", lp_class9);
else if(user->type == ADMIN)
snprintf(usertype, 50, "%s", lp_class8);
else if(user->type == CHEEF)
snprintf(usertype, 50, "%s", lp_class7);
else if(user->type == OP)
snprintf(usertype, 50, "%s", lp_class6);
else if(user->type == KVIP)
snprintf(usertype, 50, "%s", lp_class5);
else if(user->type == VIP)
snprintf(usertype, 50, "%s", lp_class4);
else if(user->type == REG)
snprintf(usertype, 50, "%s", lp_class3);
else if(user->type == PROTECTED)
snprintf(usertype, 50, "%s", lp_class2);
else if(user->type == REGULAR)
snprintf(usertype, 50, "%s", lp_class1);


human_user = human_sock_list; 
while(human_user != NULL) 
{ 
next_human = human_user->next; 
if(((human_user->user->type & (ADMIN | OWNER)) != 0) && (user->hide_share != 2))
{
if(show_joinsparts == 1)
uprintf(human_user->user, "<%s> %s: %s (IP: %s), %s|", botnick, lp_joins, user->nick, user->hostname, usertype);
if(show_joinsparts == 2)
uprintf(human_user->user, "$To: %s From: %s $<%s> %s: %s (IP: %s), %s|", human_user->user, botnick, botnick, lp_joins, user->nick, user->hostname, usertype);
}
human_user = next_human; 
}
if(show_joinsparts == 3)
{
snprintf(kbstring, 500, "<%s> %s: %s (IP: %s). %s|", botnick, lp_joins, user->nick, user->hostname, usertype);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, user);
}
}		
uprintf(user, "<MOTD> \r\n");
send_to_user(motd_mem, user);
send_to_user("\r\n", user);
if(user->type == REGULAR)
send_to_user(motdn_mem, user);
else if(user->type == PROTECTED)
send_to_user(motdp_mem, user);
else if(user->type == REG)
send_to_user(motdr_mem, user);
else if(user->type == VIP)
send_to_user(motdv_mem, user);
else if(user->type == KVIP)
send_to_user(motdk_mem, user);
else if(user->type == OP)
send_to_user(motdo_mem, user);
else if(user->type == CHEEF)
send_to_user(motdc_mem, user);
else if((user->type == ADMIN) || (user->type == OWNER))
send_to_user(motda_mem, user);
send_to_user("\r\n", user);
send_to_user("\r\n", user);
if(show_onjoinmsgs == 1)
{
uprintf(user, "%s\r\n", lp_typehelp);
uprintf(user, "%s\r\n", lp_typerules);
if((self_registration == 1) && (user->share >= min_regme_share))
uprintf(user, "%s\r\n", lp_regmepass);
else
{
uprintf(user, "%s\r\n", lp_typeregme);
if(protection == 1)
uprintf(user, "%s\r\n", lp_protectmepass);
}
}
send_to_user("|", user);
if(strlen(topic) > 0)
uprintf(user, "<%s> %s: '%s'%s|", botnick, lp_topicis, topic, topicstring);      	
if(show_himsg == 1)
uprintf(user, "<%s> %s %s, %s %s. %s %s|", botnick, lp_hi_, user->nick, lp_welcometo, hub_name, lp_youripis, user->hostname);
if(((check_if_registered(user->nick) >= min_command_class) && (check_if_registered(user->nick) >= min_usercommand_class)) || (check_if_registered(user->nick) >= 7))
send_raws(user);
if((show_releases > 0) && (check_if_registered(user->nick) >= who_can_seerls)) 
{
		if(show_releases == 2)
		uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s %s:\r\n", botnick, lp_recentlyadded, lp_releases);
		send_to_user(rlsmotd, user);
		if(allreleases > max_releases_to_show)
		uprintf(user, "[ %d/%d %s ]|", max_releases_to_show, allreleases, lp_itemstotal);
		else
		uprintf(user, "[ %d %s ]|", allreleases, lp_itemstotal);
}
if((show_requests > 0) && (check_if_registered(user->nick) >= who_can_seereq)) 
{
		if(show_requests == 2)
		uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s %s:\r\n", botnick, lp_recentlyadded, lp_requests);
		send_to_user(reqmotd, user);
		if(allrequests > max_requests_to_show)
		uprintf(user, "[ %d/%d %s ]|", max_requests_to_show, allrequests, lp_itemstotal);
		else
		uprintf(user, "[ %d %s ]|", allrequests, lp_itemstotal);
}

if((show_lastmsgs == 1) && (strlen(message1) > 0))
{
if((no_chat_for_chatgagged == 0) || ((user->gagc == 0) && (no_chat_for_chatgagged == 1)))
uprintf(user, "<%s> 10 %s:\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n|", botnick, lp_lastmainmsgs, message10, message9, message8, message7, message6, message5, message4, message3, message2, message1);
}


	if((user->type == REGULAR) && (mute_unregistered != 0))
	{
	uprintf(user, "$To: %s From: %s $<%s> %s:\r\n", user->nick, botnick, botnick, lp_registrationrules);	
	
	send_to_user(regme_mem, user);
	send_to_user("|", user);
	
	if(mute_unregistered == 1) 
	uprintf(user, "<%s> %s. [ %s: %s ]|", botnick, lp_hubforregistered, lp_youweregagged, lp_chatting_);
	
	else if(mute_unregistered == 2) 
	uprintf(user, "<%s> %s. [ %s: %s, %s ]|", botnick, lp_hubforregistered, lp_youweregagged, lp_downloading_, lp_searching_);

	else if(mute_unregistered == 3) 
	uprintf(user, "<%s> %s. [ %s: %s, %s, %s ]|", botnick, lp_hubforregistered, lp_youweregagged, lp_chatting_, lp_downloading_, lp_searching_);
	}	
user->gaginf = 1;
check_share(user);	       
if((user->type & (OP | CHEEF | ADMIN | OWNER)) == 0)
{
ret = check_warning(user, 0);
if(ret > 0)
{
if(ret - max_warnings > 1)
{
user->gags = 1;
user->gagd = 1;
user->gagc = 1;
uprintf(user, "<%s> %s %s, %s, %s %s: %s. %s|", botnick, lp_youweregagged, lp_chatting_, lp_downloading_, lp_searching_, lp_because, lp_toomanywarnings, lp_canonlypmtoops);
}
else
uprintf(user, "<%s> %s: [ %d/%d %s ]|", botnick, lp_yourwarnings, ret, max_warnings, lp_allowed_);
}
}
if(mystristr(off_mem, user->nick) != NULL)		
send_offmsg(user, 0);
}


if((new_user == 1) && (((user->type & (OWNER | ADMIN | CHEEF | OP | KVIP | VIP | REG)) != 0) && (show_welcomes > 0)))
{
if(show_welcomes == 1)
{
if(((user->type == ADMIN) || (user->type == OWNER)) && (strlen(welcomeadmin) > 0) && (user->hide_share != 2))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class8, user->nick, welcomeadmin);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == CHEEF) && (strlen(welcomecheef) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class7, user->nick, welcomecheef);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == OP) && (strlen(welcomeop) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class6, user->nick, welcomeop);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == KVIP) && (strlen(welcomekvip) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class5, user->nick, welcomekvip);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == VIP) && (strlen(welcomevip) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class4, user->nick, welcomevip);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == REG) && (strlen(welcomereg) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class3, user->nick, welcomereg);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
}
else if((show_welcomes == 2) && (choose_welcome() == 1))
{
if(((user->type == ADMIN) || (user->type == OWNER)) && (strlen(welcomeadmin) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class8, user->nick, randomwelcome);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == CHEEF) && (strlen(welcomecheef) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class7, user->nick, randomwelcome);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == OP) && (strlen(welcomeop) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class6, user->nick, randomwelcome);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == KVIP) && (strlen(welcomekvip) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class5, user->nick, randomwelcome);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == VIP) && (strlen(welcomevip) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class4, user->nick, randomwelcome);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((user->type == REG) && (strlen(welcomereg) > 0))
{
snprintf(kbstring, 500, "<%s> %s %s %s|", botnick, lp_class3, user->nick, randomwelcome);
send_to_humans(kbstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
}
}


	/* Send to scripts */
#ifdef HAVE_PERL
	if(new_user == 1)
	{ 
		if(user->type == REGULAR)
			command_to_scripts("$Script new_user_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == PROTECTED)
			command_to_scripts("$Script prot_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == REG)
			command_to_scripts("$Script reg_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == VIP)
			command_to_scripts("$Script vip_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == KVIP)
			command_to_scripts("$Script kvip_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == OP)
			command_to_scripts("$Script op_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == CHEEF)
			command_to_scripts("$Script cheef_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == ADMIN)
			command_to_scripts("$Script admin_connected %c%c%s|", 
			'\005', '\005', user->nick);
		else if(user->type == OWNER)
			command_to_scripts("$Script owner_connected %c%c%s|", 
			'\005', '\005', user->nick);

	}   
#endif

	/* Add user to user list */
	if((user->type & (NON_LOGGED | REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{	
		if((ret = add_user_to_list(user)) == 0)
		{
			increase_user_list();
			if(add_user_to_list(user) == -1)
				return 0;
		}	
		else if(ret == -1)
			return 0;
	}   
	return 1;
}

int check_share(struct user_t *user)
{
	time_t now;
		
	now = time(NULL);
			
	if((user->type & (NON_LOGGED | REGULAR)) != 0)
	{
	if((user->share == 0) && (kick_on_zeroshare == 1))
	{
	uprintf(user, "<%s> %s %s: %s 0 %s|", botnick, lp_youwerekicked, lp_because, lp_youshare, lp_bytes);
	uprintf(user, "$To: %s From: %s $<%s> %s %s: %s 0 %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_youshare, lp_bytes);
	if(punishment != 0)
	uprintf(user, "$ForceMove %s|", redirect_host_minshare);
	return 1;
	}
	if(min_psv_share > 0)
	{
	if((user->mode == 1) && (user->share < min_psv_share))
	{
		if(min_psv_share < (1 << 30))
		{
			if((user->share == 0) && (kick_on_zeroshare == 1))
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s 0 %s|", botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (long long)min_psv_share / (1024*1024), lp_youshare, lp_bytes);
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s 0 %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (long long)min_psv_share / (1024*1024), lp_youshare, lp_bytes);
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (long long)min_psv_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (long long)min_psv_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "$To: %s From: %s $<%s> %s %s  %lld MiB. %s %lld MiB. %s|", user->nick, botnick, botnick, lp_minpsvshare, lp_is_, (long long)min_psv_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);			
			uprintf(user, "<%s> %s %s %lld MiB. %s %lld MiB. %s|", botnick, lp_minpsvshare, lp_is_, (long long)min_psv_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			}
			
		}
		else
		{
			if((user->share == 0) && (kick_on_zeroshare == 1))
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s 0 %s|", botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (double)min_psv_share / (1024*1024*1024), lp_youshare, lp_bytes);
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s 0 %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (double)min_psv_share / (1024*1024*1024), lp_youshare, lp_bytes);
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (double)min_psv_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minpsvshare, lp_is_, (double)min_psv_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			uprintf(user, "<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", botnick, lp_minpsvshare, lp_is_, (double)min_psv_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			uprintf(user, "$To: %s From: %s $<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", user->nick, botnick, botnick, lp_minpsvshare, lp_is_, (double)min_psv_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			}
		}
	}
	else if((user->mode != 1) && (user->share < min_share))
	{
		if(min_share < (1 << 30))
		{
			if((user->share == 0) && (kick_on_zeroshare == 1))
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s 0 %s|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, lp_bytes);
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s 0 %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, lp_bytes);
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %lld MiB. %s %lld MiB|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %lld MiB. %s %lld MiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "$To: %s From: %s $<%s> %s %s %lld MiB. %s %lld MiB. %s|", user->nick, botnick, botnick, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);			
			uprintf(user, "<%s> %s %s %lld MiB. %s %lld MiB. %s|", botnick, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			}
			
		}
		else
		{
			if((user->share == 0) && (kick_on_zeroshare == 1))
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s 0 %s|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, lp_bytes);
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s 0 %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, lp_bytes);
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			uprintf(user, "<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", botnick, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			uprintf(user, "$To: %s From: %s $<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", user->nick, botnick, botnick, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			}
		}
}	
}	
else	
{
	if(user->share < min_share)
	{
		if(min_share < (1 << 30))
		{
			if((user->share == 0) && (kick_on_zeroshare == 1))
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s 0 %s|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, lp_bytes);
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s 0 %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, lp_bytes);
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "$To: %s From: %s $<%s> %s %s %lld MiB. %s %lld MiB. %s|", user->nick, botnick, botnick, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);			
			uprintf(user, "<%s> %s %s %lld MiB. %s %lld MiB. %s|", botnick, lp_minshare, lp_is_, (long long)min_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			}
			
		}
		else
		{
			if((user->share == 0) && (kick_on_zeroshare == 1))
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s 0 %s|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, lp_bytes);
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s 0 %s|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, lp_bytes);
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			uprintf(user, "<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", botnick, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			uprintf(user, "$To: %s From: %s $<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", user->nick, botnick, botnick, lp_minshare, lp_is_, (double)min_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			}
		}
}	
}	
	
	}
	else if((user->type == REG) && (user->share < min_reg_share))
        {
                if(min_reg_share < (1 << 30))
                {
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", botnick, lp_youwerekicked, lp_because, lp_minregshare, lp_is_, (long long)min_reg_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minregshare, lp_is_, lp_youshare, (long long)min_reg_share / (1024*1024), (long long)user->share / (1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
            		return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "<%s> %s %s %lld MiB. %s %lld MiB. %s|", botnick, lp_minregshare, lp_is_, (long long)min_reg_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			uprintf(user, "$To: %s From: %s $<%s> %s %s %lld MiB. %s %lld MiB. %s|", user->nick, botnick, botnick, lp_minregshare, lp_is_, (long long)min_reg_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			}
		}
                else
                {
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_minregshare, lp_is_, (double)min_reg_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minregshare, lp_is_, (double)min_reg_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
            		return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", botnick, lp_minregshare, lp_is_, (double)min_reg_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			uprintf(user, "$To: %s From: %s $<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", user->nick, botnick, botnick, lp_minregshare, lp_is_, (double)min_reg_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			}
			
		}
                
	}
	else if((user->type == VIP) && (user->share < min_vip_share))
        {
                if(min_vip_share < (1 << 30))
                {
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", botnick, lp_youwerekicked, lp_because, lp_minvipshare, lp_is_, (long long)min_vip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minvipshare, lp_is_, (long long)min_vip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
            		return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "<%s> %s %s %lld MiB. %s %lld MiB. %s|", botnick, lp_minvipshare, lp_is_, (long long)min_vip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			uprintf(user, "$To: %s From: %s $<%s> %s %s %lld MiB. %s %lld MiB. %s|", user->nick, botnick, botnick, lp_minvipshare, lp_is_, (long long)min_vip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			}

		}
		else
                {
			if(gag_on_minshare == 0)
			{
                        uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_minvipshare, lp_is_, (double)min_vip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minvipshare, lp_is_, (double)min_vip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
            		return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", botnick, lp_minvipshare, lp_is_, (double)min_vip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			uprintf(user, "$To: %s From: %s $<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", user->nick, botnick, botnick, lp_minvipshare, lp_is_, (double)min_vip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			}

		}
	}
	else if((user->type == KVIP) && (user->share < min_kvip_share))
        {
                if(min_kvip_share < (1 << 30))
                {
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", botnick, lp_youwerekicked, lp_because, lp_minkvipshare, lp_is_, (long long)min_kvip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minkvipshare, lp_is_, (long long)min_kvip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
            		return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "<%s> %s %s %lld MiB. %s %lld MiB. %s|", botnick, lp_minkvipshare, lp_is_, (long long)min_kvip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			uprintf(user, "$To: %s From: %s $<%s> %s %s %lld MiB. %s %lld MiB. %s|", user->nick, botnick, botnick, lp_minkvipshare, lp_is_, (long long)min_kvip_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			}
		}
                else
                {
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_minkvipshare, lp_is_, (double)min_kvip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minkvipshare, lp_is_, (double)min_kvip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
                        return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", botnick, lp_minkvipshare, lp_is_, (double)min_kvip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			uprintf(user, "$To: %s From: %s $<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", user->nick, botnick, botnick, lp_minkvipshare, lp_is_, (double)min_kvip_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			}
		}
	}
	
	else if(((user->type & (OP)) != 0)
                && (user->share < min_op_share))
        {
                if(min_op_share < (1 << 30))
                {
			if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", botnick, lp_youwerekicked, lp_because, lp_minopshare, lp_is_, (long long)min_op_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %lld MiB. %s %lld MiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minopshare, lp_is_, (long long)min_op_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
            		return 1;
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "$To: %s From: %s $<%s> %s %s %lld MiB. %s %lld MiB. %s|", user->nick, botnick, botnick, lp_minopshare, lp_is_, (long long)min_op_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			uprintf(user, "<%s> %s %s %lld MiB. %s %lld MiB. %s|", botnick, lp_minopshare, lp_is_, (long long)min_op_share / (1024*1024), lp_youshare, (long long)user->share / (1024*1024), lp_gagminshare);
			}

                }
		else
                {
                	if(gag_on_minshare == 0)
			{
			uprintf(user, "<%s> %s %s: %s %s %2.2f GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_minopshare, lp_is_, (double)min_op_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %2.2f GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_minopshare, lp_is_, (double)min_op_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_minshare);
			}
			else if((gag_on_minshare == 1) && (user->gaginf != 0) && ((user->gags == 0) || (user->gagd == 0)) && (user->gaggedonminshare == 0))
			{
			user->gagd = 1;
			user->gags = 1;
			user->gaggedonminshare = 1;
			uprintf(user, "$To: %s From: %s $<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", user->nick, botnick, botnick, lp_minopshare, lp_is_, (double)min_op_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			uprintf(user, "<%s> %s %s %2.2f GiB. %s %2.2f GiB. %s|", botnick, lp_minopshare, lp_is_, (double)min_op_share / (1024*1024*1024), lp_youshare, (double)user->share / (1024*1024*1024), lp_gagminshare);			
			}
                }
}
	if(((user->type & (NON_LOGGED | REGULAR)) != 0)
		&& (((double)user->share / (1024 * 1024 * 1024)) > max_share))
	{
			uprintf(user, "<%s> %s %s: %s %s %d GiB. %s %2.2f GiB|", botnick, lp_youwerekicked, lp_because, lp_maxshare, lp_is_, max_share, lp_youshare, (double)user->share / (1024*1024*1024));
			uprintf(user, "$To: %s From: %s $<%s> %s %s: %s %s %d GiB. %s %2.2f GiB|", user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_maxshare, lp_is_, max_share, lp_youshare, (double)user->share / (1024*1024*1024));
			if(punishment != 0)
			uprintf(user, "$ForceMove %s|", redirect_host_maxshare);
		return 1;
	}
if((gag_on_minshare == 1) && (user->gaggedonminshare == 1) && 
((((user->type & (NON_LOGGED | REGULAR)) != 0) && (((min_psv_share == 0) && (user->share >= min_share))
|| ((user->mode == 1) && (min_psv_share > 0) && (user->share >= min_psv_share))
|| ((user->mode != 1) && (min_psv_share > 0) && (user->share >= min_share))))
|| (((user->type & (REG)) != 0) && (user->share >= min_reg_share))
|| (((user->type & (VIP)) != 0) && (user->share >= min_vip_share))
|| (((user->type & (KVIP)) != 0) && (user->share >= min_kvip_share))
|| (((user->type & (OP)) != 0) && (user->share >= min_op_share))))
{
user->gagd = 0;
user->gags = 0;
uprintf(user, "<%s> %s|", botnick, lp_ungagminshare);
uprintf(user, "$To: %s From: %s $<%s> %s|", user->nick, botnick, botnick, lp_ungagminshare);
user->gaggedonminshare = 0;
}
return 0;

}

/* Handles the ValidateNick command */
/* This one has to check if the name is taken or if it is reserved */
/* Returns 0 if user should be kicked */
int validate_nick(char *buf, struct user_t *user)
{	
	char command[21];
	char first_letter[2];
	char temp_nick[51];

	int i, matchprx;
	int nmbofusers = 0;
	char nmdcstring[57];
        char str_time_to_unban[256];
	struct sock_t *sock;
	struct user_t *non_human;
	char *user_list_nick;
	char *dfprx;
	char prx1[21];
	char prx2[21];
	char *op_list;
        struct sock_t *human_user;
	struct sock_t *next_human;
	char longbanmsg[601];
	struct user_t *d_user;
	time_t now;
	now = time(NULL);

	if(sscanf(buf, "%20s %50s|", command, temp_nick) < 2)
	{                                                                         
		logprintf(4, "Received bad $ValidateNick command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		uprintf(user, "$ValidateDenide %s|", temp_nick);
		return 0;
	}

	/* Remove trailing '|'  */
	if(temp_nick[strlen(temp_nick)-1] == '|')
		temp_nick[strlen(temp_nick)-1] = '\0';


	/* Make sure that it doesn't contain ascii char 5.  */
	if((strchr(temp_nick, '\005') != NULL) || (strlen(temp_nick) == 0))
	{
		uprintf(user, "$ValidateDenide %s|", temp_nick);
		return 0;
	}



	/* Hammering protection by Centurion@09/02/2007 */
	if((strcasecmp(hammernick, temp_nick) == 0) && (hammertime > now))
	return 0;

	/* Check that it isn't "Hub-Security" */
	if((strncasecmp(temp_nick, "hub-security", 12) == 0) && (user->type != SCRIPT))
	{	
		/* I know that it should be spelled "ValidateDenied", but since the
		* protocol is designed this way, we can't expect the clients to 
		* understand the command if it's spelled in any other way.  */
		uprintf(user, "$ValidateDenide %s|", temp_nick);
		return 0;
	}

	if((strcasecmp(temp_nick, botnick) == 0) || (strcasecmp(temp_nick, opchatnick) == 0))
	{
		uprintf(user, "$ValidateDenide %s|", temp_nick);
		return 0;
	}

	if(strchr(temp_nick, '\240') != NULL)
	{
    		uprintf(user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_spacesinnick);	
    		return 0;
	}
	if((strlen(nickchars) > 0) && (check_if_registered(temp_nick) == 0))
	{
	for(i = 0; (unsigned long)i <= (strlen(temp_nick)-1); i++)
	{
	sscanf(temp_nick+i, "%1s", first_letter);
	if(strstr(nickchars, first_letter) == NULL)
	{
	uprintf(user, "<%s> %s %s: unallowed character in nickname [ %s ]|", botnick, lp_youwerekicked, lp_because, first_letter);
	return 0;
	}
	}
	}
	
	sscanf(temp_nick, "%1s", first_letter);
	if((strlen(forbchars) > 0) && (strstr(forbchars, first_letter) != NULL) && (check_if_registered(temp_nick) == 0))
	{
	uprintf(user, "<%s> %s %s: %s [ %s ]|", botnick, lp_youwerekicked, lp_because, lp_forbchar, first_letter);
		return 0;
	}
	
	if((strlen(temp_nick) < min_nick_len) && (check_if_registered(temp_nick) == 0))
	{
		uprintf(user, "<%s> %s %s: %s (min: %d)|", botnick, lp_youwerekicked, lp_because, lp_nicktooshort, min_nick_len);
		return 0;
	}
	
	if((strlen(default_prefixes) > 0) && (check_if_registered(temp_nick) == 0)) 
	{
	if((strstr(default_prefixes, " ") == NULL) && (strncasecmp(temp_nick, default_prefixes, strlen(default_prefixes)) != 0))
	{
        uprintf(user, "<%s> %s %s: %s ( %s )|", botnick, lp_youwerekicked, lp_because, lp_lackofprefix, default_prefixes);
	return 0;
	}
	else if(strstr(default_prefixes, " ") != NULL)
	{
	matchprx = 0;
	dfprx=default_prefixes;
	while((strstr(dfprx, " ") != NULL) && (matchprx != 1))
	{
	sscanf(dfprx, "%s %s", prx1, prx2);
	if((strncasecmp(temp_nick, prx1, strlen(prx1)) == 0) || (strncasecmp(temp_nick, prx2, strlen(prx2)) == 0)) 
	matchprx = 1;
	dfprx= dfprx + strlen(prx1) + 1;
	}
	if(matchprx == 0)
	{
        uprintf(user, "<%s> %s %s: %s ( %s )|", botnick, lp_youwerekicked, lp_because, lp_lackofprefix, default_prefixes);
	return 0;
	}
	}
	}
	/* Or "Administrator"  */
	if((strncasecmp(temp_nick, "Administrator", 13) == 0) 
		&& (user->type != SCRIPT))
	{
		uprintf(user, "$ValidateDenide %s|", temp_nick);
		return 0;
	}   

	/* Check if it's already taken.  */
	if((((user_list_nick = check_if_on_user_list(temp_nick)) != NULL)
		|| (get_human_user(temp_nick) != NULL)) 
		&& (check_if_registered(temp_nick) == 0))
	{
	
		if(user->type != SCRIPT)
		{	     
			if(strcmp(get_human_user(temp_nick)->hostname, user->hostname) != 0)
			{
			uprintf(user, "$ValidateDenide %s|", temp_nick);
			memset(temp_nick, 0, sizeof(temp_nick));
			return -1;
			}
			else
			{
			if((user_list_nick = check_if_on_user_list(get_human_user(temp_nick)->nick)) != NULL)
			{
			remove_user_from_list(get_human_user(temp_nick)->nick);
			snprintf(nmdcstring, 56, "$Quit %s|", user_list_nick);
			send_to_humans(nmdcstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(get_human_user(temp_nick)->nick)) != NULL)
			{		 
				remove_human_from_hash(get_human_user(temp_nick)->nick);
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}    
			}
			}
		}
		else
		{
			/* If user is a script, kick the user who has taken the nick.  */
			logprintf(4, "validate_nick() - Warning: Script already in user_list.\n");	
			snprintf(nmdcstring, 56, "$Kick %s|", temp_nick);
			kick(nmdcstring, NULL, 0);
		}
	}

	if(user->type != SCRIPT)
	{		
		strcpy(user->nick, temp_nick);
		now = time(NULL);

		human_user = human_sock_list;
		while(human_user != NULL)
		{
		next_human = human_user->next;
		if(human_user->user->type == REGULAR) 
		nmbofusers++;
		human_user = next_human;
		} 

		if(nmbofusers > max_users)
        	{
		if(check_if_registered(temp_nick)==0)
                {
		        hub_mess(user, HUB_FULL_MESS);
                        snprintf(longbanmsg, 600, "[ denied / %s ] [ %s , IP: %s ]", lp_maxusers, user->nick, user->hostname);
	        
		if(punishment != 0)
		uprintf(user, "$ForceMove %s|", redirect_host_hubfull);                                             
		denied_info(longbanmsg);
                        return 0;
                }
		}
		
		if(user->isbanned != 0)
		{
		if(check_if_registered(temp_nick)==0)
			{
                        if((btime_to_unban != 0) && (difftime(btime_to_unban, now) < 0))
			{
			unballow(user->hostname, BAN);
			if(check_if_banned(user, BANRS) != 0)
			unballow(user->hostname, BANRS);
			}
			else
			{
				 ip_banned++;
      				 uprintf(user, "<%s> %s: %s|", botnick, hub_name, banmsg);
			         uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, hub_name, banmsg);
				snprintf(longbanmsg, 600, "[ %s / %s ] [ %s , IP: %s ]", lp_denied_, lp_ipban_, user->nick, user->hostname);
			if(check_if_banned(user, BANRS) != 0)
			{
			uprintf(user, "<%s> %s: %s|", botnick, lp_banreason, banreason);
			uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, lp_banreason, banreason);
				sprintfa(longbanmsg, " [ %s: %s", lp_reason, banreason+2);

			}
				 if(btime_to_unban > 0) 
                                 {
                                 now = time(NULL);
                                 time2str(difftime(btime_to_unban, now), str_time_to_unban);
                                 if(difftime(btime_to_unban, now) > 0)
				 {
				 uprintf(user, "<%s> %s: %s|", botnick, lp_timetounban, str_time_to_unban);
				 uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, lp_timetounban, str_time_to_unban);
                    		 sprintfa(longbanmsg, " [ %s: %s ]", lp_expiresin, str_time_to_unban);				    
				 }
				 }
				    else
                    		 sprintfa(longbanmsg, " [ %s: %s ]", lp_expires_, lp_never_);				    
		    if(btime_to_unban == 0)
		    hammertime = now + 600;
		    else
		    hammertime = btime_to_unban;
		    snprintf(hammernick, 50, "%s", user->nick);
                denied_info(longbanmsg);
		if(punishment != 0)
		uprintf(user, "$ForceMove %s|", redirect_host_kickban);                                             
				return 0;
			}
		}
		}
		if((check_if_banned(user, NICKBAN) != 0) && (check_if_registered(temp_nick) < 5))
		{
                        
			if((btime_to_unban != 0) && (difftime(btime_to_unban, now) < 0))
			{
			unballow(user->nick, NICKBAN);
			if(check_if_banned(user, NICKBANRS) != 0)
			unballow(user->nick, NICKBANRS);
			}
			else
			{
			nick_banned++;
			uprintf(user, "<%s> %s: %s|", botnick, hub_name, banmsg);
			uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, hub_name, banmsg);
			        snprintf(longbanmsg, 600, "[ %s / %s ] [ %s , IP: %s ]", lp_denied_, lp_nickban_, user->nick, user->hostname);
			if(check_if_banned(user, NICKBANRS) != 0)
			{
			uprintf(user, "<%s> %s: %s|", botnick, lp_banreason, banreason);
			uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, lp_banreason, banreason);
				sprintfa(longbanmsg, " [ %s: %s", lp_reason, banreason+2);

			}
			        if (btime_to_unban > 0)
                                 {
                                 now = time(NULL);
                                 time2str(difftime(btime_to_unban, now), str_time_to_unban);
                                 if(difftime(btime_to_unban, now) > 0)
				 {
				 uprintf(user, "<%s> %s: %s|", botnick, lp_timetounban, str_time_to_unban);
				 uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, lp_timetounban, str_time_to_unban);
                    		 sprintfa(longbanmsg, " [ %s: %s ]", lp_expiresin, str_time_to_unban);				    
                                 }
				 }
				    else
                    		 sprintfa(longbanmsg, " [ %s: %s ]", lp_expires_, lp_never_);				    
		    if(btime_to_unban == 0)
		    hammertime = now + 600;
		    else
		    hammertime = btime_to_unban;
		    snprintf(hammernick, 50, "%s", user->nick);
		if(punishment != 0)
		uprintf(user, "$ForceMove %s|", redirect_host_kickban);                                             
                denied_info(longbanmsg);
			return 0;
			}
		}


		if(check_if_registered(temp_nick) != 0)
		{

			hub_mess(user, GET_PASS_MESS);	     
			return 1;
		}

		else if(strlen(default_pass) > 0)
		{
			hub_mess(user, GET_PASS_MESS2);

			if(check_if_banned(user, NICKBAN) != 0)
			{
                        if((btime_to_unban != 0) && (difftime(btime_to_unban, now) < 0))
			{
			unballow(user->nick, NICKBAN);
			if(check_if_banned(user, NICKBANRS) != 0)
			unballow(user->nick, NICKBANRS);
			}
			else
			{
                                nick_banned++;
				uprintf(user, "<%s> %s: %s|", botnick, hub_name, banmsg);
			uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, hub_name, banmsg);
			        snprintf(longbanmsg, 600, "[ %s / %s ] [ %s , IP: %s ]", lp_denied_, lp_nickban_, user->nick, user->hostname);
				if(check_if_banned(user, NICKBANRS) != 0)
				{
				uprintf(user, "<%s> %s: %s|", botnick, lp_banreason, banreason);
			uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, lp_banreason, banreason);
				sprintfa(longbanmsg, " [ %s: %s", lp_reason, banreason+2);
				}
				if (btime_to_unban > 0)
                                {
                                now = time(NULL);
                                time2str(difftime(btime_to_unban, now), str_time_to_unban);
                                 if(difftime(btime_to_unban, now) > 0)
				 {
                                uprintf(user, "<%s> %s: %s|", botnick, lp_timetounban, str_time_to_unban);
				 uprintf(user, "$To: %s From: %s $<%s> %s: %s|", user->nick, botnick, botnick, lp_timetounban, str_time_to_unban);
				        sprintfa(longbanmsg, " [ %s: %s ]", lp_expiresin, str_time_to_unban);				     
				}
				}

				else
                    		 sprintfa(longbanmsg, " [ %s: %s ]", lp_expires_, lp_never_);				    
		    if(btime_to_unban == 0)
		    hammertime = now + 600;
		    else
		    hammertime = btime_to_unban;
		    snprintf(hammernick, 50, "%s", user->nick);

		if(punishment != 0)
		uprintf(user, "$ForceMove %s|", redirect_host_kickban);                                             
				denied_info(longbanmsg);
				return 0;
			}
			}
			
			return 1;

		}

		else if(registered_only != 0)
		{
                        uprintf(user, "<%s> %s: %s|", botnick, hub_name, regonlymsg);
			snprintf(longbanmsg, 600, "[ %s / %s ] [ %s , IP: %s ]", lp_denied_, lp_regonly, user->nick, user->hostname);

		if(punishment != 0)
		uprintf(user, "$ForceMove %s|", redirect_host_regonly);                                             
				denied_info(longbanmsg);
			return 0;
		}

		if(strlen(default_pass) == 0)
		{
			hub_mess(user, HELLO_MESS);
		}



	}
	/* And if user is a script, set the nick and add to nicklist.  */
	else
	{	
		strcpy(user->nick, temp_nick);
		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}	
		sock = human_sock_list;
		op_list = get_op_list();
		while(sock != NULL)
		{
			if(((sock->user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER | FORKED)) != 0)
				&& (user != sock->user))
			{
				send_to_user(op_list, sock->user);
			}
			sock = sock->next;
		}
		non_human = non_human_user_list;
		while(non_human != NULL)
		{
			if(((non_human->type & FORKED) != 0)
				&& (user != non_human))
			{
				send_to_user(op_list, non_human);
			}
			non_human = non_human->next;
		}
		free(op_list);
	}
	return 1;
}

/* Sets the version of the client the user is using */
int version(char *buf, struct user_t *user)
{  
	if(sscanf(buf, "$Version %30[^ |]|", user->version) != 1)
	{                                                                    
		logprintf(4, "Received bad $Version command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return 0;
	}
	return 1;
}

/* Checks if users password is valid and if user is op.  */
int my_pass(char *buf, struct user_t *user)
{
	int ret, het;
	char quit_string[101];
	char bigstring[1001];
	char cutpass[51];
	struct user_t *d_user;
	char *user_list_nick;
	time_t now;	
	char str_time_to_unban[256];	

	if(strcmp(buf, "|") == 0)
	{		
		hub_mess(user, NOPASS_MESS);
		if(show_notifications > 0)
		{
		snprintf(bigstring, 1000, "%s [ IP: %s ] %s|", user->nick, user->hostname, lp_nopass);		
		send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
		}
		return 0;
	}

	ret = check_pass(buf, user);
	user->hide_share = 0;

	user->tmpop = 0;
	user->flag = 0;

	switch(ret)
	{

	case 9:
		/* User is OWNER */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				/* Change the nick so that it won't be removed from the
				* hashtable after it has been added again.  */
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}    
		}

		/* Add to user list */
		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}
		user->type = OWNER;
		user->gagc = 0;
		user->gagd = 0;
	    	user->gags = 0;

		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif
		
		hub_mess(user, LOGGED_IN_MESS);
		hub_mess(user, OP_LOGGED_IN_MESS);

		het = check_if_hidden(user->nick);
		if((hide_opshare == 1) || (het == 1))
		user->hide_share = 1;
		if(het == 2)
		{
		user->hide_share = 2;
		remove_user_from_list(user->nick);
		snprintf(quit_string, 100, "$Quit %s|", user->nick);
		send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}

		logprintf(1, "OWNER %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;
		break;
	case 8:
		/* User is ADMIN */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				/* Change the nick so that it won't be removed from the
				* hashtable after it has been added again.  */
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}    
		}

		/* Add to user list */
		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}
		user->type = ADMIN;
		user->gagc = 0;
		user->gagd = 0;
	    	user->gags = 0;

		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		hub_mess(user, OP_LOGGED_IN_MESS);

		het = check_if_hidden(user->nick);
		if((hide_opshare == 1) || (het == 1))
		user->hide_share = 1;
		if(het == 2)
		{
		user->hide_share = 2;
		remove_user_from_list(user->nick);
		snprintf(quit_string, 100, "$Quit %s|", user->nick);
		send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}


		logprintf(1, "ADMIN %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;

		break;
	case 7:
		/* User is CHEEF */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				/* Change the nick so that it won't be removed from the
				* hashtable after it has been added again.  */
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}    
		}

		/* Add to user list */
		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}
		user->type = CHEEF;
		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		hub_mess(user, OP_LOGGED_IN_MESS);

		het = check_if_hidden(user->nick);
		if((hide_opshare == 1) || (het == 1))
		user->hide_share = 1;
		
		logprintf(1, "CHEEF %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;

		break;

	case 6:
		/* User is OP */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				/* Change the nick so that it won't be removed from the
				* hashtable after it has been added again.  */
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}    
		}

		/* Add to user list */
		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}
		user->type = OP;
		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		hub_mess(user, OP_LOGGED_IN_MESS);

		het = check_if_hidden(user->nick);
		if((hide_opshare == 1) || (het == 1))
		user->hide_share = 1;
		
		logprintf(1, "OP %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;

		break;


	case 5:
		/* User is KVIP */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{	
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}   
		}

		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}	
		user->type = KVIP;
		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		logprintf(1, "KVIP %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;

		break;
	case 4:
		/* User is VIP */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{	
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}   
		}

		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}	
		user->type = VIP;
		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		logprintf(1, "VIP %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;

		break;
	case 3:
		/* User is REG */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{	
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}   
		}

		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}	
		user->type = REG;
		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		logprintf(1, "REG %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;

		break;
case 2:
		/* User is PROTECTED */

		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{	
			remove_user_from_list(user->nick);
			snprintf(quit_string, 100, "$Quit %s|", user_list_nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER,
				user);
			if((d_user = get_human_user(user->nick)) != NULL)
			{		 
				remove_human_from_hash(user->nick);
				strcpy(d_user->nick, "removed user");
				d_user->rem = REMOVE_USER;
			}   
		}

		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}	
		user->type = PROTECTED;
		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		logprintf(1, "PROTECTED %s logged in from %s\n", user->nick, user->hostname);

if(check_if_permprotected(user->nick) == 0)
{
check_if_registered(user->nick);
now = time(NULL);
time2str(difftime(time_to_unban, now), str_time_to_unban);
uprintf(user, "<%s> %s: %s|", botnick, lp_youraccountwillexpire, str_time_to_unban);
}

	totalvisitors++;

		break;

	case 1:
		if((user_list_nick = check_if_on_user_list(user->nick)) != NULL)
		{
			hub_mess(user, BAD_PASS_MESS);
			logprintf(1, "Host %s attempted to re-use %s nick\n", user->hostname, user_list_nick);
			return 0;
		}

		if(add_user_to_list(user) == 0)
		{
			increase_user_list();
			add_user_to_list(user);
		}
		user->type = REGULAR;
		total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif

		hub_mess(user, LOGGED_IN_MESS);
		logprintf(1, "Regular user %s logged in from %s\n", user->nick, user->hostname);

	totalvisitors++;		
	break;

	case 0:
		/* Validation failed */
		hub_mess(user, BAD_PASS_MESS);
                sscanf(buf, "%50[^|]|", cutpass);

		if(show_notifications > 0)
		{
		snprintf(bigstring, 1000, "%s [ IP: %s ] %s: [ %s ]|", user->nick, user->hostname, lp_badpass, cutpass);	
		send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
		}
		return 0;
	}
	return 1;
}

/* Kick a user. tempban is 1 if the command is sent from a human, but 0 if
* used internally.  */
void kick(char *buf, struct user_t *user, int tempban) 
{ 
char command[11]; 
char nick[MAX_NICK_LEN+1]; 
char host[MAX_HOST_LEN+1]; 
char ban_command[61]; 
struct user_t *to_user; 
 
if(sscanf(buf, "%10s %50[^|]|", command, nick) != 2) 
{  
logprintf(4, "Received bad $Kick command from %s at %s:\n", user->nick, user->hostname); 
if(strlen(buf) < 3500) 
logprintf(4, "%s\n", buf); 
else 
logprintf(4, "too large buf\n"); 
return; 
} 
 
if((user != NULL) && (strncmp(nick, user->nick, strlen(nick)) == 0) 
&& (strlen(nick) == strlen(user->nick))) 
return; 
 
/* If it was triggered internally. */ 
if(user == NULL) 
{ 
if(check_if_on_user_list(nick) == NULL) 
return; 
remove_user_from_list(nick); 
} 
 
else if((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) 
{  
if(check_if_on_user_list(nick) == NULL) 
return; 
 
get_users_hostname(nick, host); 
logprintf(1, "User %s at %s was kicked by %s\n", nick, host, user->nick); 
 
if((strncmp(botnick, nick, strlen(nick)) == 0) | (strncmp(opchatnick, nick, strlen(nick)) == 0))
return;

if((to_user = get_human_user(nick)) == NULL)
return;

if(user->type <= to_user->type)
{ 
uprintf(user, "<%s> %s|", botnick, lp_kickdeny); 
return; 
} 

if(get_human_user(nick) != NULL)
remove_user_from_list(nick); 
else
return; 

if((kick_bantime > 0) && (tempban != 0)) 
{ 
snprintf(ban_command, 60, "%s %dm", host, kick_bantime); 
ballow(ban_command, BAN, user); 
snprintf(ban_command, 60, "%s %dm", to_user->nick , kick_bantime); 
ballow(ban_command, NICKBAN, user); 
} 
 
#ifdef HAVE_PERL 
command_to_scripts("$Script kicked_user %c%c%s%c%c%s|", 
'\005', '\005', nick, '\005', '\005', user->nick); 
#endif  
} 

else if(user->type == SCRIPT)  
{ 
if(check_if_on_user_list(nick) == NULL) 
return; 
 
remove_user_from_list(nick); 
}  
 
if((to_user = get_human_user(nick)) != NULL) 
{ 
to_user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST; 
return; 
} 
}
/* Quits the program */
void quit_program(void)
{  
	/* If we are a child process and the command wasn't sent from a forked
	* process, don't remove users.  */

	if(pid <= 0){ 
		send_to_non_humans("$QuitProgram|", SCRIPT | FORKED, NULL);
		remove_all(0xFFFF, 0, 0);
	}
	else
	{	   
		logprintf(LOG_NOTICE, "Got term signal, exiting...");

		/* If we are the parent.  */
		sleep(1);
		remove_all(0xFFFF, 0, 0);
		write_config_file();	
		/* Remove semaphores and shared memory segments.  */
		semctl(user_list_sem, 0, IPC_RMID, NULL);
		shmctl(get_user_list_shm_id(), IPC_RMID, NULL);
		shmctl(user_list_shm_shm, IPC_RMID, NULL);	

		/* If we are the parent, close the listening sockets and close the temp file */
		close(listening_socket);
		close(listening_unx_socket);
		unlink(un_sock_path);
		//exit(EXIT_SUCCESS);
	}
}
/* Set various variables through the admin connection */
void set_var(char *org_buf, struct user_t *user)
{
	char *buf;
	char *c;
	char temp1[31];
	char temp2[31];
	char mindc_buf[7];
	char new_string[201];
	char botinuse[51];
	struct sock_t *human_user;
	struct sock_t *next_human;
	char *op_list;
	buf = org_buf+5;
		
if(prvset == 1)
uprintf(user, "$To: %s From: %s $", user->nick, botnick);


	if(strncmp(buf, "motd ", 5) == 0)
	{
		buf += 5;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		
		if(write_file(buf, "motd", 1) == -1)
			return;
		loadfile2buf(&motd_mem,MOTD_FILE);
		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd ] %s: \r\n", botnick, lp_setto);
			send_to_user(motd_mem, user);
			uprintf(user, "|");
		}
	}
	if(strncmp(buf, "motd_normal ", 12) == 0)
	{
		buf += 12;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motdn", 1) == -1)
			return;
		loadfile2buf(&motdn_mem, MOTD_N);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd_normal ] %s: \r\n", botnick, lp_setto);
			send_to_user(motdn_mem, user);
			uprintf(user, "%c", '|');
		}
	}

	if(strncmp(buf, "motd_prot ", 10) == 0)
	{
		buf += 10;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motdp", 1) == -1)
			return;
		loadfile2buf(&motdp_mem, MOTD_P);

		if((user->type & (ADMIN | OWNER)) != 0)
{
			uprintf(user, "<%s> [ motd_prot ] %s: \r\n", botnick, lp_setto);
			send_to_user(motdp_mem, user);
			uprintf(user, "%c", '|');
		}
	}

	if(strncmp(buf, "motd_reg ", 9) == 0)
	{
		buf += 9;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motdr", 1) == -1)
			return;
		loadfile2buf(&motdr_mem, MOTD_R);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd_reg ] %s: \r\n", botnick, lp_setto);
			send_to_user(motdp_mem, user);
			uprintf(user, "%c", '|');
		}
	}

	if(strncmp(buf, "motd_vip ", 9) == 0)
	{
		buf += 9;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motdv", 1) == -1)
			return;
		loadfile2buf(&motdv_mem, MOTD_V);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd_vip ] %s: \r\n", botnick, lp_setto);
			send_to_user(motdv_mem, user);
			uprintf(user, "%c", '|');
		}
	}

	if(strncmp(buf, "motd_kvip ", 10) == 0)
	{
		buf += 10;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motdk", 1) == -1)
			return;
		loadfile2buf(&motdk_mem, MOTD_K);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd_kvip ] %s: \r\n", botnick, lp_setto);
			send_to_user(motdv_mem, user);
			uprintf(user, "%c", '|');
		}
	}

	if(strncmp(buf, "motd_op ", 8) == 0)
	{
		buf += 8;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motdo", 1) == -1)
			return;
		loadfile2buf(&motdo_mem, MOTD_O);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd_op ] %s: \r\n", botnick, lp_setto);
			send_to_user(motdo_mem, user);
			uprintf(user, "%c", '|');
		}
	}
	if(strncmp(buf, "motd_cheef ", 11) == 0)
	{
		buf += 10;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motdc", 1) == -1)
			return;
		loadfile2buf(&motdc_mem,MOTD_C);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd_cheef ] %s: \r\n", botnick, lp_setto);
			send_to_user(motdc_mem, user);
			uprintf(user, "%c", '|');
		}
	}

	if(strncmp(buf, "motd_admin ", 11) == 0)
	{
		buf += 11;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "motda", 1) == -1)
			return;
		loadfile2buf(&motda_mem, MOTD_A);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
			uprintf(user, "<%s> [ motd_admin ] %s: \r\n", botnick, lp_setto);
			send_to_user(motda_mem, user);
			uprintf(user, "%c", '|');
		}
	}


	else if(strncmp(buf, "rules ", 6) == 0)
	{
		buf += 6;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "rules", 1) == -1)
			return;
		loadfile2buf(&rules_mem,RULES_FILE);

		if((user->type & (ADMIN | OWNER)) != 0)
		{
		uprintf(user, "<%s> [ rules ] %s: \r\n", botnick, lp_setto);
		send_to_user(rules_mem, user);
			uprintf(user, "%c", '|');
		}
	}
	else if(strncmp(buf, "regme ", 6) == 0)
	{
		buf += 6;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "regme", 1) == -1)
			return;
		loadfile2buf(&regme_mem,REGME_FILE);

		if((user->type & (ADMIN | OWNER)) != 0)
{
		uprintf(user, "<%s> [ regme ] %s: \r\n", botnick, lp_setto);
		send_to_user(regme_mem, user);
			uprintf(user, "%c", '|');
		}
	}
	else if(strncmp(buf, "autofavmsg ", 11) == 0)
	{
		buf += 11;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "autofav", 1) == -1)
			return;
		

		if((user->type & (ADMIN | OWNER)) != 0)
		{
		loadfile2buf(&autofav_mem,AUTOFAV_FILE);
		uprintf(user, "<%s> [ autofavmsg ] %s: \r\n", botnick, lp_setto);
		send_to_user(autofav_mem, user);
			uprintf(user, "%c", '|');
		}
	}

	else if(strncmp(buf, "scheduledmsg ", 13) == 0)
	{
		buf += 13;
		if((c = strrchr(buf, '|')) == NULL)
			return;
		*c = '\0';
		if(write_file(buf, "scheduledmsg", 1) == -1)
			return;
		

		if((user->type & (ADMIN | OWNER)) != 0)
		{
		loadfile2buf(&schedule_mem,SCHEDULE_FILE);
		uprintf(user, "<%s> [ scheduledmsg ] %s: \r\n", botnick, lp_setto);
		send_to_user(schedule_mem, user);
			uprintf(user, "%c", '|');
		}
	}

else if(strncmp(buf, "allowed_clients ", 16) == 0)
{
buf += 16;
strncpy(allowed_clients, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
allowed_clients[cut_string(buf, '|')] = '\0';
if(strlen(allowed_clients) == 0)
{
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ allowed_clients ] %s (ALL)|", botnick, lp_setto);
}
else
{
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ allowed_clients ] %s \"%s\"|", botnick, lp_setto, allowed_clients);
}
}
else if(strncmp(buf, "autofav ", 8) == 0)
{
buf += 8;
autofav = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ autofav ] %s %d|", botnick, lp_setto, autofav);
}
else if(strncmp(buf, "ban_overrides_allow ", 20) == 0)
{
buf += 20;
ban_overrides_allow = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ ban_overrides_allow ] %s %d|", botnick, lp_setto, ban_overrides_allow);
}
else if(strncmp(buf, "banmsg ", 7) == 0)
{
buf += 7;
strncpy(banmsg, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
banmsg[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ banmsg ] %s \"%s\"|", botnick, lp_setto, banmsg);
}
else if(strncmp(buf, "banwords ", 9) == 0)
{
buf += 9;
if((banwords = realloc(banwords, sizeof(char) * (cut_string(buf, '|') + 1))) == NULL)
{
logprintf(0, "Error - In set_var()/realloc(): %s", strerror(errno));
quit = 1;
return;
}
strncpy(banwords, buf, cut_string(buf, '|'));
banwords[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ banwords ] %s \"%s\"|", botnick, lp_setto, banwords);
}
else if(strncmp(buf, "botnick ", 8) == 0)
{
buf += 8;
sscanf(buf, "%50[^|]|", botinuse);
if((strncasecmp(buf, "|", 1) == 0) || (strncasecmp(buf, " ", 1) == 0) || (strstr(buf, " ") != NULL)) 
{
if((user->type & (ADMIN | OWNER)) != 0)
{
uprintf(user, "<%s> [ botnick ] %s|", botnick, lp_cannotbeblank);
return;
}
}
else if((get_human_user(botinuse) != NULL) || (strncmp(botinuse, opchatnick, strlen(opchatnick)) == 0))	
{
if((user->type & (ADMIN | OWNER)) != 0)
{
uprintf(user, "<%s> [ botnick ] - %s: [ %s ]|", botnick, lp_nickinuse, botinuse);
return;
}
}
else {		
snprintf(new_string, 200, "$Quit %s|", botnick);
send_to_humans(new_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
strncpy(botnick, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
botnick[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ botnick ] %s \"%s\"|", botnick, lp_setto, botnick);
snprintf(new_string, 200, "$MyINFO $ALL %s [BOT] Hub-Security <DB V:%s,M:A,H:0/0/1,S:0>$ $$hub-security$0$|", botnick, VERSION);
send_to_humans(new_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
human_user = human_sock_list;
op_list = get_op_list();
while(human_user != NULL)
{
next_human = human_user->next;
send_to_user(op_list, human_user->user);
human_user = next_human;
}
free(op_list);
}								
}
else if(strncmp(buf, "check_class ", 12) == 0)
{
buf += 12;
check_class = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ check_class ] %s %d|", botnick, lp_setto, check_class);
}
else if(strncmp(buf, "clockbot ", 9) == 0)
{
if((clockbot > 0) && (count_bots > 0))
{
total_users--;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif
}
buf += 9;
clockbot = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ clockbot ] %s %d|", botnick, lp_setto, clockbot);
if(clockbot == 0)
{
snprintf(new_string, 200, "$Quit %s|", clocknick);
send_to_humans(new_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else
{
if(count_bots > 0)
total_users++;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif
}
}
else if(strncmp(buf, "count_bots ", 11) == 0)
{
buf += 11;
count_bots = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ count_bots ] %s  %d|", botnick, lp_setto, count_bots);
}
else if(strncmp(buf, "config_on_priv ", 15) == 0)
{
buf += 15;
config_on_priv = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ config_on_priv ] %s  %d|", botnick, lp_setto, config_on_priv);
}
else if(strncmp(buf, "connlimit ", 10) == 0)
{
buf += 10;
connlimit = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ connlimit ] %s %d kB/s|", botnick, lp_setto, connlimit);
}
else if(strncmp(buf, "count_reghubs ", 14) == 0)
{
buf += 14;
count_reghubs = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ count_reghubs ] %s %d|", botnick, lp_setto, count_reghubs);
}
else if(strncmp(buf, "crypt_enable ", 13) == 0)
{
buf += 13;
crypt_enable = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ crypt_enable ] %s %d|", botnick, lp_setto, crypt_enable);
}
else if(strncmp(buf, "cut_reg_msg ", 12) == 0)
{
buf += 12;
cut_reg_msg = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ cut_reg_msg] %s %d|", botnick, lp_setto, cut_reg_msg);
}
else if(strncmp(buf, "cut_string_to ", 14) == 0)
{
buf += 14;
cut_string_to = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ cut_string_to ] %s %d|", botnick, lp_setto, cut_string_to);
}
else if(strncmp(buf, "default_pass ", 13) == 0)
{
buf += 13;
strncpy(default_pass, buf, (cut_string(buf, '|') > MAX_ADMIN_PASS_LEN) ? MAX_ADMIN_PASS_LEN : cut_string(buf, '|'));
default_pass[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ default_pass ] %s \"%s\"|", botnick, lp_setto, default_pass);
}
else if(strncmp(buf, "default_prefixes ", 17) == 0)
{
buf += 17;
strncpy(default_prefixes, buf, (cut_string(buf, '|') > 255) ? 255 : cut_string(buf, '|'));
default_prefixes[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ default_prefixes ] %s \"%s\"|", botnick, lp_setto, default_prefixes);
}
else if(strncmp(buf, "deny_main_http ", 15) == 0)
{
buf += 15;
deny_main_http = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ deny_main_http ] %s %d|", botnick, lp_setto, deny_main_http);
}
else if(strncmp(buf, "deny_passive ", 13) == 0)
{
buf += 13;
deny_passive = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ deny_passive ] %s %d|", botnick, lp_setto, deny_passive);
}
else if(strncmp(buf, "deny_pm_http ", 13) == 0)
{
buf += 13;
deny_pm_http = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ deny_pm_http ] %s %d|", botnick, lp_setto, deny_pm_http);
}
else if(strncmp(buf, "deny_proxy ", 11) == 0)
{
buf += 11;
deny_proxy = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ deny_proxy ] %s %d|", botnick, lp_setto, deny_proxy);
}
else if(strncmp(buf, "expiredreq ", 11) == 0)
{
buf += 11;
expiredreq = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ expiredreq ] %s %u|", botnick, lp_setto, expiredreq);
}
else if(strncmp(buf, "expiredrls ", 11) == 0)
{
buf += 11;
expiredrls = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)		
uprintf(user, "<%s> [ expiredrls ] %s %u|", botnick, lp_setto, expiredrls);
}
else if(strncmp(buf, "expiredseen ", 12) == 0)
{
buf += 12;
expiredseen = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)		
uprintf(user, "<%s> [ expiredseen ] %s %u|", botnick, lp_setto, expiredseen);
}
else if(strncmp(buf, "expiredtime ", 12) == 0)
{
buf += 12;
expiredtime = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)		
uprintf(user, "<%s> [ expiredtime ] %s %u|", botnick, lp_setto, expiredtime);
}
else if(strncmp(buf, "expiredurl ", 11) == 0)
{
buf += 11;
expiredurl = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)		
uprintf(user, "<%s> [ expiredurl ] %s %u|", botnick, lp_setto, expiredurl);
}
else if(strncmp(buf, "expiredwarn ", 12) == 0)
{
buf += 12;
expiredwarn = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)		
uprintf(user, "<%s> [ expiredwarn ] %s %u|", botnick, lp_setto, expiredwarn);
}
else if(strncmp(buf, "extra_commands ", 15) == 0)
{
buf += 15;
extra_commands = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)		
uprintf(user, "<%s> [ extra_commands ] %s %d|", botnick, lp_setto, extra_commands);
}
else if(strncmp(buf, "extra_files ", 12) == 0)
{
buf += 12;
strncpy(extra_files, buf, (cut_string(buf, '|') > 500) ? 500 : cut_string(buf, '|'));
extra_files[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ extra_files ] %s \"%s\"|", botnick, lp_setto, extra_files);
}
else if(strncmp(buf, "flood_action ", 13) == 0)
{
buf += 13;
flood_action = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ flood_action ] %s %d|", botnick, lp_setto, flood_action);
}
else if(strncmp(buf, "flood_lines ", 12) == 0)
{
buf += 12;
flood_lines = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ flood_lines ] %s %d|", botnick, lp_setto, flood_lines);
}
else if(strncmp(buf, "forbchars ", 10) == 0)
{
buf += 10;
strncpy(forbchars, buf, (cut_string(buf, '|') > 30) ? 30 : cut_string(buf, '|'));
forbchars[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ forbchars ] %s \"%s\"|", botnick, lp_setto, forbchars);
}
else if(strncmp(buf, "gag_on_minshare ", 16) == 0)
{
buf += 16;
gag_on_minshare = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ gag_on_minshare ] %s %d|", botnick, lp_setto, gag_on_minshare);
}
else if(strncmp(buf, "help_on_priv ", 13) == 0)
{
buf += 13;
help_on_priv = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ help_on_priv ] %s %d|", botnick, lp_setto, help_on_priv);
}
else if(strncmp(buf, "hide_allshare ", 14) == 0)
{
buf += 14;
hide_allshare = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ hide_allshare ] %s %d|", botnick, lp_setto, hide_allshare);
}
else if(strncmp(buf, "hide_opshare ", 13) == 0)
{
buf += 13;
hide_opshare = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ hide_opshare ] %s %d|", botnick, lp_setto, hide_opshare);
}
else if(strncmp(buf, "hidetags ", 9) == 0)
{
buf += 9;
hidetags = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ hidetags ] %s %d|", botnick, lp_setto, hidetags);
}
else if(strncmp(buf, "hub_description ", 16) == 0)
{
buf += 16;
strncpy(hub_description, buf, (cut_string(buf, '|') > MAX_HUB_DESC) ? MAX_HUB_DESC : cut_string(buf, '|'));
hub_description[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ hub_description ] %s \"%s\"|", botnick, lp_setto, hub_description);	
}
else if(strncmp(buf, "hub_full_msg ", 13) == 0)
{
buf += 13;
if((hub_full_msg = realloc(hub_full_msg, sizeof(char) * (cut_string(buf, '|') + 1))) == NULL)
{
logprintf(0, "Error - In set_var()/realloc(): %s", strerror(errno));
quit = 1;
return;
}
strncpy(hub_full_msg, buf, cut_string(buf, '|'));
hub_full_msg[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ hub_full_msg ] %s \"%s\"|", botnick, lp_setto, hub_full_msg);
}
else if(strncmp(buf, "hub_hostname ", 13) == 0)
{
buf += 13;
strncpy(hub_hostname, buf, (cut_string(buf, '|') > 120) ? 120 : cut_string(buf, '|'));
hub_hostname[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ hub_hostname ] %s \"%s\"|", botnick, lp_setto, hub_hostname);
}
else if(strncmp(buf, "hub_name ", 9) == 0)
{
buf += 9;
strncpy(hub_name, buf, (cut_string(buf, '|') > MAX_HUB_NAME) ? MAX_HUB_NAME : cut_string(buf, '|'));
hub_name[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)
{
uprintf(user, "<%s> [ hub_name ] %s \"%s\"|", botnick, lp_setto, hub_name);
}
if (strlen(topic)!=0) 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s - %s (%s)|", hub_name, topic, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s - %s|", hub_name, topic);
}
else 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s (%s)|", hub_name, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s|", hub_name);
}
send_to_humans(set_topic, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if(strncmp(buf, "ipgag_type ", 11) == 0)
{
buf += 11;
ipgag_type = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ ipgag_type ] %s %d|", botnick, lp_setto, ipgag_type);
}
else if(strncmp(buf, "kick_bantime ", 13) == 0)
{
buf += 13;
kick_bantime = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ kick_bantime ] %s %d|", botnick, lp_setto, kick_bantime);
}
else if(strncmp(buf, "kick_for_incomplete ", 20) == 0)
{
buf += 20;
kick_for_incomplete = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ kick_for_incomplete ] %s %d|", botnick, lp_setto, kick_for_incomplete);
}
else if(strncmp(buf, "kick_on_zeroshare ", 18) == 0)
{
buf += 18;
kick_on_zeroshare = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ kick_on_zeroshare ] %s %d|", botnick, lp_setto, kick_on_zeroshare);
}
else if(strncmp(buf, "listening_port ", 15) == 0)
{	
buf += 15;
listening_port = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ listening_port ] %s %u|", botnick, lp_setto, listening_port);
}
else if(strncmp(buf, "log_main_chat ", 14) == 0)
{
buf += 14;
log_main_chat = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ log_main_chat ] %s %d|", botnick, lp_setto, log_main_chat);
}
else if(strncmp(buf, "max_regular_from_one_ip ", 24) == 0)
{
buf += 24;
max_regular_from_one_ip = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ max_regular_from_one_ip ] %s %d|", botnick, lp_setto, max_regular_from_one_ip);
}
else if(strncmp(buf, "max_releases_to_show ", 21) == 0)
{
buf += 21;
max_releases_to_show = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
{
uprintf(user, "<%s> [ max_releases_to_show ] %s %d|", botnick, lp_setto, max_releases_to_show);
}
rls_motd();
}
else if(strncmp(buf, "max_requests_to_show ", 21) == 0)
{
buf += 21;
max_requests_to_show = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ max_requests_to_show ] %s %d|", botnick, lp_setto, max_requests_to_show);
}
else if(strncmp(buf, "max_share ", 10) == 0)
{
buf += 10;
max_share = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ max_share ] %s %d GiB|", botnick, lp_setto, max_share);
}
else if(strncmp(buf, "max_users ", 10) == 0)
{
buf += 10;
max_users = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ max_users ] %s %d|", botnick, lp_setto, max_users);
}
else if(strncmp(buf, "max_warnings ", 13) == 0)
{
buf += 13;
max_warnings = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ max_warnings ] %s %d|", botnick, lp_setto, max_warnings);
}
else if(strncmp(buf, "maxhubs ", 8) == 0)
{
buf += 8;
maxhubs = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ maxhubs ] %s %d|", botnick, lp_setto, maxhubs);
}
else if(strncmp(buf, "maxslots ", 9) == 0)
{
buf += 9;
maxslots = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ maxslots ] %s %d|", botnick, lp_setto, maxslots);
}
else if(strncmp(buf, "min_command_class ", 18) == 0)
{
buf += 18;
min_command_class = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ min_command_class ] %s %d|", botnick, lp_setto, min_command_class);
}
else if(strncmp(buf, "min_kvip_share ", 15) == 0)
{
buf += 15;
memset(temp2, 0, sizeof(temp2));
if(sscanf(buf, "%30s %30[^|]|", temp1, temp2) == 2)
{
min_kvip_share = strtoll(temp1, (char **)NULL, 10);
if(!strcasecmp(temp2, "mb"))
min_kvip_share = min_kvip_share << 20;
else if(!strcasecmp(temp2, "gb"))
min_kvip_share = min_kvip_share << 30;
}
else
min_kvip_share = strtoll(temp1, (char **)NULL, 10);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(!strcasecmp(temp2, "mb"))
uprintf(user, "<%s> [ min_kvip_share ] %s %lld MiB|", botnick, lp_setto, min_kvip_share >> 20);
else if(!strcasecmp(temp2, "gb"))
uprintf(user, "<%s> [ min_kvip_share ] %s %lld GiB|", botnick, lp_setto, min_kvip_share >> 30);
else
uprintf(user, "<%s> [ min_kvip_share ] %s %lld B|", botnick, lp_setto, min_kvip_share);
}
}
else if(strncmp(buf, "min_nick_len ", 13) == 0)
{
buf += 13;
min_nick_len = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ min_nick_len ] %s %d|", botnick, lp_setto, min_nick_len);
}
else if(strncmp(buf, "min_op_share ", 13) == 0)
{
buf += 13;
memset(temp2, 0, sizeof(temp2));
if(sscanf(buf, "%30s %30[^|]|", temp1, temp2) == 2)
{
min_op_share = strtoll(temp1, (char **)NULL, 10);
if(!strcasecmp(temp2, "mb"))
min_op_share = min_op_share << 20;
else if(!strcasecmp(temp2, "gb"))
min_op_share = min_op_share << 30;
}
else
min_op_share = strtoll(temp1, (char **)NULL, 10);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(!strcasecmp(temp2, "mb"))
uprintf(user, "<%s> [ min_op_share ] %s %lld MiB|", botnick, lp_setto, min_op_share >> 20);
else if(!strcasecmp(temp2, "gb"))
uprintf(user, "<%s> [ min_op_share ] %s %lld GiB|", botnick, lp_setto, min_op_share >> 30);
else
uprintf(user, "<%s> [ min_op_share ] %s %lld B|", botnick, lp_setto, min_op_share);
}
}
else if(strncmp(buf, "min_reg_share ", 14) == 0)
{
buf += 14;
memset(temp2, 0, sizeof(temp2));
if(sscanf(buf, "%30s %30[^|]|", temp1, temp2) == 2)
{
min_reg_share = strtoll(temp1, (char **)NULL, 10);
if(!strcasecmp(temp2, "mb"))
min_reg_share = min_reg_share << 20;
else if(!strcasecmp(temp2, "gb"))
min_reg_share = min_reg_share << 30;
}
else
min_reg_share = strtoll(temp1, (char **)NULL, 10);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(!strcasecmp(temp2, "mb"))
uprintf(user, "<%s> [ min_reg_share ] %s %lld MiB|", botnick, lp_setto, min_reg_share >> 20);
else if(!strcasecmp(temp2, "gb"))
uprintf(user, "<%s> [ min_reg_share ] %s %lld GiB|", botnick, lp_setto, min_reg_share >> 30);
else
uprintf(user, "<%s> [ min_reg_share ] %s %lld B|", botnick, lp_setto, min_reg_share);
}
}
else if(strncmp(buf, "min_regme_share ", 16) == 0)
{
buf += 16;
memset(temp2, 0, sizeof(temp2));
if(sscanf(buf, "%30s %30[^|]|", temp1, temp2) == 2)
{
min_regme_share = strtoll(temp1, (char **)NULL, 10);
if(!strcasecmp(temp2, "mb"))
min_regme_share = min_regme_share << 20;
else if(!strcasecmp(temp2, "gb"))
min_regme_share = min_regme_share << 30;
}
else
min_regme_share = strtoll(temp1, (char **)NULL, 10);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(!strcasecmp(temp2, "mb"))
uprintf(user, "<%s> [ min_regme_share ] %s %lld MiB|", botnick, lp_setto, min_regme_share >> 20);
else if(!strcasecmp(temp2, "gb"))
uprintf(user, "<%s> [ min_regme_share ] %s %lld GiB|", botnick, lp_setto, min_regme_share >> 30);
else
uprintf(user, "<%s> [ min_regme_share ] %s %lld B|", botnick, lp_setto, min_regme_share);
}
}

else if(strncmp(buf, "min_search_len ", 15) == 0)
{
buf += 15;
min_search_len = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
{
uprintf(user, "<%s> [ min_search_len ] %s %d|", botnick, lp_setto, min_search_len);
}
}
else if(strncmp(buf, "min_share ", 10) == 0)
{
buf += 10;
memset(temp2, 0, sizeof(temp2));
if(sscanf(buf, "%30s %30[^|]|", temp1, temp2) == 2)
{
min_share = strtoll(temp1, (char **)NULL, 10);
if(!strcasecmp(temp2, "mb"))
min_share = min_share << 20;
else if(!strcasecmp(temp2, "gb"))
min_share = min_share << 30;
}
else
min_share = strtoll(temp1, (char **)NULL, 10);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(!strcasecmp(temp2, "mb"))
uprintf(user, "<%s> [ min_share ] %s %lld MiB|", botnick, lp_setto, min_share >> 20);
else if(!strcasecmp(temp2, "gb"))
uprintf(user, "<%s> [ min_share ] %s %lld GiB|", botnick, lp_setto, min_share >> 30);
else
uprintf(user, "<%s> [ min_share ] %s %lld B|", botnick, lp_setto, min_share);
}
}
else if(strncmp(buf, "min_psv_share ", 14) == 0)
{
buf += 14;
memset(temp2, 0, sizeof(temp2));
if(sscanf(buf, "%30s %30[^|]|", temp1, temp2) == 2)
{
min_psv_share = strtoll(temp1, (char **)NULL, 10);
if(!strcasecmp(temp2, "mb"))
min_psv_share = min_psv_share << 20;
else if(!strcasecmp(temp2, "gb"))
min_psv_share = min_psv_share << 30;
}
else
min_psv_share = strtoll(temp1, (char **)NULL, 10);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(!strcasecmp(temp2, "mb"))
uprintf(user, "<%s> [ min_psv_share ] %s %lld MiB|", botnick, lp_setto, min_psv_share >> 20);
else if(!strcasecmp(temp2, "gb"))
uprintf(user, "<%s> [ min_psv_share ] %s %lld GiB|", botnick, lp_setto, min_psv_share >> 30);
else
uprintf(user, "<%s> [ min_psv_share ] %s %lld B|", botnick, lp_setto, min_psv_share);
}
}
else if(strncmp(buf, "min_vip_share ", 14) == 0)
{
buf += 14;
memset(temp2, 0, sizeof(temp2));
if(sscanf(buf, "%30s %30[^|]|", temp1, temp2) == 2)
{
min_vip_share = strtoll(temp1, (char **)NULL, 10);
if(!strcasecmp(temp2, "mb"))
min_vip_share = min_vip_share << 20;
else if(!strcasecmp(temp2, "gb"))
min_vip_share = min_vip_share << 30;
}
else
min_vip_share = strtoll(temp1, (char **)NULL, 10);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(!strcasecmp(temp2, "mb"))
uprintf(user, "<%s> [ min_vip_share ] %s %lld MegaBytes|", botnick, lp_setto, min_vip_share >> 20);
else if(!strcasecmp(temp2, "gb"))
uprintf(user, "<%s> [ min_vip_share ] %s %lld GigaBytes|", botnick, lp_setto, min_vip_share >> 30);
else
uprintf(user, "<%s> [ min_vip_share ] %s %lld B|", botnick, lp_setto, min_vip_share);
}        
}
else if(strncmp(buf, "mindcppver ", 11) == 0)
{
buf += 11;
if(strncmp(buf, "0.", 2) != 0)
strncpy(buf, "0.401|", 6);
sscanf(buf, "%5[^|]", mindc_buf);
if(strlen(mindc_buf) == 2)
strncpy(mindc_buf, "0.401", 6);
strncpy(mindcppver, mindc_buf, 5);
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ mindcppver ] %s \"%s\"|", botnick, lp_setto, mindcppver);
}
else if(strncmp(buf, "minslots ", 9) == 0)
{
buf += 9;
minslots = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ minslots ] %s %d|", botnick, lp_setto, minslots);
}
else if(strncmp(buf, "min_usercommand_class ", 22) == 0)
{
buf += 22;
min_usercommand_class = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ min_user_commandclass ] %s %d|", botnick, lp_setto, min_usercommand_class);
}

else if(strncmp(buf, "mute_unregistered ", 18) == 0)
{
buf += 18;
mute_unregistered = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ mute_unregistered] %s %d|", botnick, lp_setto, mute_unregistered);
}
else if(strncmp(buf, "nickchars ", 10) == 0)
{
buf += 10;
strncpy(nickchars, buf, (cut_string(buf, '|') > 300) ? 300 : cut_string(buf, '|'));
nickchars[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ nickchars ] %s \"%s\"|", botnick, lp_setto, nickchars);
}
else if(strncmp(buf, "no_chat_for_chatgagged ", 23) == 0)
{
buf += 23;
no_chat_for_chatgagged = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ no_chat_for_chatgagged ] %s %d|", botnick, lp_setto, no_chat_for_chatgagged);
}
else if(strncmp(buf, "offline_msgs ", 13) == 0)
{
buf += 13;
offline_msgs = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ offline_msgs ] %s %d|", botnick, lp_setto, offline_msgs);
}
else if(strncmp(buf, "opchatnick ", 11) == 0)
{
buf += 11;
sscanf(buf, "%50[^|]|", botinuse);
if((strncasecmp(buf, "|", 1) == 0) || (strncasecmp(buf, " ", 1) == 0) || (strstr(buf, " ") != NULL)) 
{
if((user->type & (ADMIN | OWNER)) != 0)
{
uprintf(user, "<%s> [ opchatnick ] %s|", botnick, lp_cannotbeblank);
return;
}
}
else if((get_human_user(botinuse) != NULL) || (strncmp(botinuse, botnick, strlen(botnick)) == 0))	
{
if((user->type & (ADMIN | OWNER)) != 0)
{
uprintf(user, "<%s> [ opchatnick ] - %s: [ %s ]|", botnick, lp_nickinuse, botinuse);
return;
}
}
else {
snprintf(new_string, 200, "$Quit %s|", opchatnick);
send_to_humans(new_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
strncpy(opchatnick, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
opchatnick[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ opchatnick ] %s \"%s\"|", botnick, lp_setto, opchatnick);
snprintf(new_string, 200, "$MyINFO $ALL %s [BOT] OpChat <DB V:%s,M:A,H:0/0/1,S:0>$ $$opchat$0$|", opchatnick, VERSION);
send_to_humans(new_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
human_user = human_sock_list;
op_list = get_op_list();
while(human_user != NULL)
{
next_human = human_user->next;
send_to_user(op_list, human_user->user);
human_user = next_human;
}
free(op_list);
}
}
else if(strncmp(buf, "partmsg ", 8) == 0)
{
buf += 8;
strncpy(partmsg, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
partmsg[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ partmsg ] %s  \"%s\"|", botnick, lp_setto, partmsg);
}
else if(strncmp(buf, "protection ", 11) == 0)
{
buf += 11;
protection = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ protection ] %s %d|", botnick, lp_setto, protection);
}
else if(strncmp(buf, "protectmeprefix ", 16) == 0)
{
buf += 16;
strncpy(protectmeprefix, buf, (cut_string(buf, '|') > 20) ? 20 : cut_string(buf, '|'));
protectmeprefix[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ protectmeprefix ] %s \"%s\"|", botnick, lp_setto, protectmeprefix);
}
else if(strncmp(buf, "prottime ", 9) == 0)
{
buf += 9;
prottime = (unsigned int)(atoi(buf));
if((user->type & (ADMIN | OWNER)) != 0)		
uprintf(user, "<%s> [ prottime ] %s %u|", botnick, lp_setto, prottime);
}
else if(strncmp(buf, "punishment ", 11) == 0)
{
buf += 11;
punishment = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ punishment ] %s %d|", botnick, lp_setto, punishment);
}
else if(strncmp(buf, "ratio ", 6) == 0)
{
buf += 6;
ratio = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
{
if(reverse_ratio == 1)
uprintf(user, "<%s> [ ratio ] %s %d slots/hub|", botnick, lp_setto, ratio);
else
uprintf(user, "<%s> [ ratio ] %s %d hubs/slot|", botnick, lp_setto, ratio);
}
}
else if(strncmp(buf, "reverse_ratio ", 14) == 0)
{
buf += 14;
reverse_ratio = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)
uprintf(user, "<%s> [ reverse_ratio ] %s %d|", botnick, lp_setto, reverse_ratio);
}
else if(strncmp(buf, "redirect_host_all ", 18) == 0)
{
buf += 18;
strncpy(redirect_host_kickban, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_kickban[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_minshare, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_minshare[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_maxshare, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_maxshare[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_maxhubs, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_maxhubs[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_minslots, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_minslots[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_maxslots, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_maxslots[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_ratio, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_ratio[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_client, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_client[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_connlimit, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_connlimit[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_hubfull, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_hubfull[cut_string(buf, '|')] = '\0';
strncpy(redirect_host_regonly, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_regonly[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_kickban, redirect_host_minshare, redirect_host_maxshare, redirect_host_maxhubs, redirect_host_minslots, redirect_host_maxslots, redirect_host_ratio, redirect_host_client, redirect_host_connlimit, redirect_host_hubfull, redirect_host_regonly ] %s \"%s\"|", botnick, lp_setto, redirect_host_kickban);
}
else if(strncmp(buf, "redirect_host_kickban ", 22) == 0)
{
buf += 22;
strncpy(redirect_host_kickban, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_kickban[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_kickban ] %s \"%s\"|", botnick, lp_setto, redirect_host_kickban);
}
else if(strncmp(buf, "redirect_host_minshare ", 23) == 0)
{
buf += 23;
strncpy(redirect_host_minshare, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_minshare[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_minshare ] %s \"%s\"|", botnick, lp_setto, redirect_host_minshare);
}
else if(strncmp(buf, "redirect_host_maxshare ", 23) == 0)
{
buf += 23;
strncpy(redirect_host_maxshare, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_maxshare[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_maxshare ] %s \"%s\"|", botnick, lp_setto, redirect_host_maxshare);
}
else if(strncmp(buf, "redirect_host_maxhubs ", 22) == 0)
{
buf += 22;
strncpy(redirect_host_maxhubs, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_maxhubs[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_maxhubs ] %s \"%s\"|", botnick, lp_setto, redirect_host_maxhubs);
}
else if(strncmp(buf, "redirect_host_minslots ", 23) == 0)
{
buf += 23;
strncpy(redirect_host_minslots, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_minslots[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_minslots ] %s \"%s\"|", botnick, lp_setto, redirect_host_minslots);
}
else if(strncmp(buf, "redirect_host_maxslots ", 23) == 0)
{
buf += 23;
strncpy(redirect_host_maxslots, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_maxslots[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_maxslots ] %s \"%s\"|", botnick, lp_setto, redirect_host_maxslots);
}
else if(strncmp(buf, "redirect_host_ratio ", 20) == 0)
{
buf += 20;
strncpy(redirect_host_ratio, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_ratio[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_ratio ] %s \"%s\"|", botnick, lp_setto, redirect_host_ratio);
}
else if(strncmp(buf, "redirect_host_client ", 21) == 0)
{
buf += 21;
strncpy(redirect_host_client, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_client[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_client ] %s \"%s\"|", botnick, lp_setto, redirect_host_client);
}
else if(strncmp(buf, "redirect_host_connlimit ", 24) == 0)
{
buf += 24;
strncpy(redirect_host_connlimit, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_connlimit[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_connlimit ] %s \"%s\"|", botnick, lp_setto, redirect_host_connlimit);
}
else if(strncmp(buf, "redirect_host_hubfull ", 22) == 0)
{
buf += 22;
strncpy(redirect_host_hubfull, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_hubfull[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_hubfull ] %s \"%s\"|", botnick, lp_setto, redirect_host_hubfull);
}
else if(strncmp(buf, "redirect_host_regonly ", 22) == 0)
{
buf += 22;
strncpy(redirect_host_regonly, buf, (cut_string(buf, '|') > MAX_HOST_LEN) ? MAX_HOST_LEN : cut_string(buf, '|'));
redirect_host_regonly[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ redirect_host_regonly ] %s \"%s\"|", botnick, lp_setto, redirect_host_regonly);
}
else if(strncmp(buf, "registered_only ", 16) == 0)
{
buf += 16;
registered_only = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ registered_only ] %s %d|", botnick, lp_setto, registered_only);
}
else if(strncmp(buf, "regme_on_priv ", 14) == 0)
{
buf += 14;
regme_on_priv = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ regme_on_priv ] %s %d|", botnick, lp_setto, regme_on_priv);
}
else if(strncmp(buf, "regmeprefix ", 12) == 0)
{
buf += 12;
strncpy(regmeprefix, buf, (cut_string(buf, '|') > 20) ? 20 : cut_string(buf, '|'));
regmeprefix[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ regmeprefix ] %s \"%s\"|", botnick, lp_setto, regmeprefix);
}
else if(strncmp(buf, "regonlymsg ", 10) == 0)
{
buf += 10;
strncpy(regonlymsg, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
regonlymsg[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ regonlymsg ] %s \"%s\"|", botnick, lp_setto, regonlymsg);
}
else if(strncmp(buf, "restrict_mode ", 14) == 0)
{
buf += 14;
restrict_mode = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ restrict_mode ] %s %d|", botnick, lp_setto, restrict_mode);
}
else if(strncmp(buf, "restrictions ", 13) == 0)
{
buf += 13;
restrictions = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ restrictions ] %s %d|", botnick, lp_setto, restrictions);
}
else if(strncmp(buf, "req_on_priv ", 11) == 0)
{
buf += 11;
req_on_priv = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ req_on_priv ] %s %d|", botnick, lp_setto, req_on_priv);
}
else if(strncmp(buf, "rls_on_priv ", 11) == 0)
{
buf += 11;
rls_on_priv = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ rls_on_priv ] %s %d|", botnick, lp_setto, rls_on_priv);
}
else if(strncmp(buf, "rules_on_priv ", 14) == 0)
{
buf += 14;
rules_on_priv = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ rules_on_priv ] %s %d|", botnick, lp_setto, rules_on_priv);
}
else if(strncmp(buf, "searchspam_time ", 16) == 0)
{
buf += 16;
searchspam_time = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ searchspam_time ] %s %d|", botnick, lp_setto, searchspam_time);
}
else if(strncmp(buf, "seen_class ", 11) == 0)
{
buf += 11;
seen_class = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ seen_class ] %s %d|", botnick, lp_setto, seen_class);
}
else if(strncmp(buf, "self_registration ", 18) == 0)
{
buf += 18;
self_registration = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ self_registration ] %s %d|", botnick, lp_setto, self_registration);
}
else if(strncmp(buf, "send_scheduled ", 15) == 0)
{
buf += 15;
send_scheduled = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ send_scheduled ] %s %d|", botnick, lp_setto, send_scheduled);
}
else if(strncmp(buf, "show_actions ", 13) == 0)
{
buf += 13;
show_actions = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_actions ] %s %d|", botnick, lp_setto, show_actions);
}
else if(strncmp(buf, "show_denied ", 12) == 0)
{
buf += 12;
show_denied = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_denied ] %s %d|", botnick, lp_setto, show_denied);
}
else if(strncmp(buf, "show_helpcommand_usage ", 23) == 0)
{
buf += 23;
show_helpcommand_usage = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_helpcommand_usage ] %s %d|", botnick, lp_setto, show_helpcommand_usage);
}
else if(strncmp(buf, "show_himsg ", 11) == 0)
{
buf += 11;
show_himsg = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_himsg ] %s %d|", botnick, lp_setto, show_himsg);
}
else if(strncmp(buf, "show_joinsparts ", 16) == 0)
{
buf += 16;
show_joinsparts = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_joinsparts ] %s %d|", botnick, lp_setto, show_joinsparts);
}
else if(strncmp(buf, "show_kicksbans ", 15) == 0)
{
buf += 15;
show_kicksbans = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_kicksbans ] %s %d|", botnick, lp_setto, show_kicksbans);
}
else if(strncmp(buf, "show_lastmsgs ", 14) == 0)
{
buf += 14;
show_lastmsgs = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_lastmsgs ] %s %d|", botnick, lp_setto, show_lastmsgs);
}
else if(strncmp(buf, "show_class_in_desc ", 19) == 0)
{
buf += 19;
show_class_in_desc = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_class_in_desc ] %s %d|", botnick, lp_setto, show_class_in_desc);
}
else if(strncmp(buf, "show_mode_in_desc ", 18) == 0)
{
buf += 18;
show_mode_in_desc = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_mode_in_desc ] %s %d|", botnick, lp_setto, show_mode_in_desc);
}
else if(strncmp(buf, "show_notifications ", 19) == 0)
{
buf += 19;
show_notifications = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_notifications ] %s %d|", botnick, lp_setto, show_notifications);
}
else if(strncmp(buf, "show_onjoinmsgs ", 16) == 0)
{
buf += 16;
show_onjoinmsgs = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_onjoinmsgs ] %s %d|", botnick, lp_setto, show_onjoinmsgs);
}
else if(strncmp(buf, "show_original_desc ", 19) == 0)
{
buf += 19;
show_original_desc = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_original_desc ] %s %d|", botnick, lp_setto, show_original_desc);
}
else if(strncmp(buf, "show_original_mail ", 19) == 0)
{
buf += 19;
show_original_mail = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_original_mail ] %s %d|", botnick, lp_setto, show_original_mail);
}
else if(strncmp(buf, "show_records ", 13) == 0)
{
buf += 13;
show_records = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_records ] %s %d|", botnick, lp_setto, show_records);
}
else if(strncmp(buf, "show_releases ", 14) == 0)
{
buf += 14;
show_releases = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_releases ] %s %d|", botnick, lp_setto, show_releases);
}
else if(strncmp(buf, "show_requests ", 14) == 0)
{
buf += 14;
show_requests = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_requests ] %s %d|", botnick, lp_setto, show_requests);
}
else if(strncmp(buf, "show_uptime ", 12) == 0)
{
buf += 12;
show_uptime = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_uptime ] %s %d|", botnick, lp_setto, show_uptime);
}
else if(strncmp(buf, "show_welcomes ", 14) == 0)
{
buf += 14;
show_welcomes = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ show_welcomes ] %s %d|", botnick, lp_setto, show_welcomes);
}
else if(strncmp(buf, "slotbeg_kicker ", 15) == 0)
{
buf += 15;
slotbeg_kicker = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ slotbeg_kicker ] %s %d|", botnick, lp_setto, slotbeg_kicker);
}
else if(strncmp(buf, "spam_action ", 12) == 0)
{
buf += 12;
spam_action = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ spam_action ] %s %d|", botnick, lp_setto, spam_action);
}
else if(strncmp(buf, "tempban ", 8) == 0)
{
buf += 8;
tempban = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ tempban ] %s %d|", botnick, lp_setto, tempban);
}
else if(strncmp(buf, "topic ", 6) == 0)
{
buf += 6;
strncpy(topic, buf, (cut_string(buf, '|') > MAX_TOPIC_LEN) ? MAX_TOPIC_LEN : cut_string(buf, '|'));
topic[cut_string(buf, '|')] = '\0';
if (strlen(topic)!=0) 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s - %s (%s)|", hub_name, topic, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s - %s|", hub_name, topic);
}
else 
{
if(block_main == 1)
snprintf(set_topic, 300, "$HubName %s (%s)|", hub_name, lp_mainchatlocked);
else
snprintf(set_topic, 300, "$HubName %s|", hub_name);
}
send_to_humans(set_topic, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if(strncmp(buf, "url_on_priv ", 11) == 0)
{
buf += 11;
url_on_priv = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ url_on_priv ] %s %d|", botnick, lp_setto, url_on_priv);
}
else if(strncmp(buf, "verbosity ", 10) == 0)
{
buf += 10;
verbosity = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ verbosity ] %s %d|", botnick, lp_setto, verbosity);
}
else if(strncmp(buf, "welcomeadmin ", 13) == 0)
{
buf += 13;
strncpy(welcomeadmin, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
welcomeadmin[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ welcomeadmin ] %s \"%s\"|", botnick, lp_setto, welcomeadmin);
}
else if(strncmp(buf, "welcomecheef ", 13) == 0)
{
buf += 12;
strncpy(welcomecheef, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
welcomecheef[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ welcomecheef ] %s \"%s\"|", botnick, lp_setto, welcomecheef);
}
else if(strncmp(buf, "welcomekvip ", 12) == 0)
{
buf += 12;
strncpy(welcomekvip, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
welcomekvip[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ welcomekvip ] %s \"%s\"|", botnick, lp_setto, welcomekvip);
}
else if(strncmp(buf, "welcomeop ", 10) == 0)
{
buf += 10;
strncpy(welcomeop, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
welcomeop[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ welcomeop ] %s \"%s\"|", botnick, lp_setto, welcomeop);
}
else if(strncmp(buf, "welcomereg ", 11) == 0)
{
buf += 11;
strncpy(welcomereg, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
welcomereg[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ welcomereg ] %s \"%s\"|", botnick, lp_setto, welcomereg);
}
else if(strncmp(buf, "welcomevip ", 11) == 0)
{
buf += 11;
strncpy(welcomevip, buf, (cut_string(buf, '|') > MAX_MSG_LEN) ? MAX_MSG_LEN : cut_string(buf, '|'));
welcomevip[cut_string(buf, '|')] = '\0';
if((user->type & (ADMIN | OWNER)) != 0)        
uprintf(user, "<%s> [ welcomevip ] %s \"%s\"|", botnick, lp_setto, welcomevip);
}
else if(strncmp(buf, "who_can_addreq ", 15) == 0)
{
buf += 15;
who_can_addreq = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ who_can_addreq ] %s %d|", botnick, lp_setto, who_can_addreq);
}
else if(strncmp(buf, "who_can_addrls ", 15) == 0)
{
buf += 15;
who_can_addrls = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ who_can_addrls ] %s  %d|", botnick, lp_setto, who_can_addrls);
}
else if(strncmp(buf, "who_can_seereq ", 15) == 0)
{
buf += 15;
who_can_seereq = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ who_can_seereq ] %s %d|", botnick, lp_setto, who_can_seereq);
}
else if(strncmp(buf, "who_can_seerls ", 15) == 0)
{
buf += 15;
who_can_seerls = atoi(buf);
if((user->type & (ADMIN | OWNER)) != 0)	
uprintf(user, "<%s> [ who_can_seerls ] %s %d|", botnick, lp_setto, who_can_seerls);
}
else if(strncmp(buf, "block_main ", 11) == 0)
{
buf += 11;
block_main = atoi(buf);
}
else if(strncmp(buf, "block_pm ", 9) == 0)
{
buf += 9;
block_pm = atoi(buf);
}
else if(strncmp(buf, "block_search ", 13) == 0)
{
buf += 13;
block_search = atoi(buf);
}
else if(strncmp(buf, "block_download ", 15) == 0)
{
buf += 15;
block_download = atoi(buf);
}
else
{
if((user->type & (ADMIN | OWNER)) != 0)	
{
if((c = strrchr(buf, '|')) == NULL)
return;
*c = '\0';
uprintf(user, "<%s> %s: '%s' -> !set %s|",  botnick, lp_nosuchvariable, buf, buf);
}
}

		write_config_file();


	/* Forward command to other processes */

	/* If it was a "!set", the '!' has to be changed to a '$' */
	*org_buf = '$';

	if((strncmp(org_buf + 5, "motd", 4) != 0) && (strncmp(org_buf + 5, "rules ", 6) != 0)
	&& (strncmp(org_buf + 5, "regme ", 6) != 0) && (strncmp(org_buf + 5, "autofavmsg ", 11) != 0)
	&& (strncmp(org_buf + 5, "scheduledmsg ", 13) != 0))
	{	
		send_to_non_humans(org_buf, FORKED | SCRIPT, user);

		/* If it was sent from a script, we need to send it back.  */
		if((user->type == SCRIPT) && (pid > 0))
			send_to_user(org_buf, user);
	}   
}
/* Adds an entry to banlist or nickbanlist if type is BAN. 
Adds entry to allowlist if type is ALLOW */
int ballow(char *buf, int type, struct user_t *user)
{
	FILE *fp;
	int fd;
	int i;
	size_t j;
	int ret;
	int erret;
	char path[MAX_FDP_LEN+1];
	char line[1024];
	char period = '\0';
	char ban_host[51];
	char ban_line[121];
	time_t ban_time = 0;
	time_t old_time;
	time_t now_time;

	
	if(type == BAN)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BAN_FILE);
	else if(type == ALLOW)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, ALLOW_FILE);
	else if(type == NICKBAN)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBAN_FILE);
	else if(type == IPGAG)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAG_FILE);		
	
	else
		return -1;

	now_time = time(NULL);
	if(type == NICKBAN)
	{
		if(sscanf(buf, "%50s %lu%c", ban_host, &ban_time, &period) == 1)
			ban_host[strlen(ban_host) - 1] = '\0';

		if(check_if_registered(ban_host) > check_if_registered(user->nick))
			return -1;
	}
	else
	{
		if (sscanf(buf, "%120s %lu%c", ban_host, &ban_time, &period) == 1)
			ban_host[strlen(ban_host) - 1] = '\0';
	}

	switch(period)
	{
	case 'w':
		ban_time = ban_time * 7;
	case 'd':
		ban_time = ban_time * 24;
	case 'h':
		ban_time = ban_time * 60;
	case 'm':
		ban_time = ban_time * 60;
	case 's':
	default:
		break;
	}

	/* First, check if user is already on list */
	while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
		logprintf(0, "Error - In ballow()/open(): Interrupted system call. Trying again.\n");

	if(fd < 0)
	{
		logprintf(0, "Error - In ballow()/open(): %s", strerror(errno));
		return -1;
	}

	/* Set the lock */
	if(set_lock(fd, F_RDLCK) == 0)
	{
		logprintf(0, "Error - In ballow(): Couldn't set file lock\n");
		close(fd);
		return -1;
	}

	if((fp = fdopen(fd, "r")) == NULL)
	{
		set_lock(fd, F_UNLCK);
		logprintf(0, "Error - In ballow()/fdopen(): %s", strerror(errno));
		close(fd);
		return -1;
	}

	while(fgets(line, 1023, fp) != NULL)
	{
		trim_string(line);
		old_time = 0;

		j = strlen(line);
		if(j != 0)
		{
			/* Jump to next char which isn't a space */
			i = 0;
			while(line[i] == ' ')
				i++;

			sscanf(line+i, "%120s %lu", ban_line, &old_time);
			if(strcasecmp(ban_host, ban_line) == 0)
			{
				set_lock(fd, F_UNLCK);

				while(((erret = fclose(fp)) != 0) && (errno == EINTR))
					logprintf(0, "Error - In ballow()/fclose(): Interrupted system call. Trying again.\n");

				if(erret != 0)
				{
					logprintf(0, "Error - In ballow()/fclose(): %s", strerror(errno));
					return -1;
				}		  

				return 0;
			}
		}
	}
	set_lock(fd, F_UNLCK);

	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
		logprintf(0, "Error - In ballow()/fclose(): Interrupted system call. Trying again.\n");

	if(erret != 0)
	{
		logprintf(0, "Error - In ballow()/fclose(): %s", strerror(errno));
		return -1;
	}	

	if(type == NICKBAN)
	{
		if (ban_time > 0)
			snprintf(ban_line, 120, "%s %lu", ban_host, now_time + ban_time);
		else
			snprintf(ban_line, 120, "%s 0", ban_host);
	}
	if(type == IPGAG)
		snprintf(ban_line, 120, "%s", ban_host);
		
	
	else
	{
		if (ban_time > 0)
			snprintf(ban_line, 120, "%s %lu", ban_host, now_time + ban_time);
		else	  
			snprintf(ban_line, 120, "%s 0", ban_host);
	}
	ret = add_line_to_file(ban_line, path); 

	/* Send to scripts */
#ifdef HAVE_PERL
	if(ret == 1)
	{
		if(type == BAN)
		{	     
			if (ban_time > 0)
			{
				command_to_scripts("$Script added_temp_ban %c%c", '\005', '\005');
				non_format_to_scripts(ban_host);
				command_to_scripts("%c%c%lu", '\005', '\005', ban_time);
			}
			else
			{
				command_to_scripts("$Script added_perm_ban %c%c", '\005', '\005');
				non_format_to_scripts(ban_host);
			}
			command_to_scripts("|");
		}	
		else if(type == ALLOW)
		{	     
			if (ban_time > 0)
		 {
			 command_to_scripts("$Script added_temp_allow %c%c", '\005', '\005');
			 non_format_to_scripts(ban_host);
			 command_to_scripts("%c%c%lu", '\005', '\005', ban_time);
		 }
			else
		 {
			 command_to_scripts("$Script added_perm_allow %c%c", '\005', '\005');
			 non_format_to_scripts(ban_host);
		 }
		 command_to_scripts("|");
		}
		else if(type == NICKBAN)
		{
			if (ban_time > 0)
		 {
			 command_to_scripts("$Script added_temp_nickban %c%c", '\005', '\005');
			 non_format_to_scripts(ban_host);
			 command_to_scripts("%c%c%lu", '\005', '\005', ban_time);
		 }
			else
		 {
			 command_to_scripts("$Script added_perm_nickban %c%c", '\005', '\005');
			 non_format_to_scripts(ban_host);
		 }
		 command_to_scripts("|");
		}	
	}   
#endif
if(type == BAN)
{
loadfile2buf(&banlist_mem,BAN_FILE);
hammernick[0] = '\0';
hammertime = (time_t)0;
}

if(type == NICKBAN)
{
loadfile2buf(&nickbanlist_mem,NICKBAN_FILE);
hammernick[0] = '\0';
hammertime = (time_t)0;
}
if(type == ALLOW)
{
loadfile2buf(&allowlist_mem,ALLOW_FILE);
}
if(type == IPGAG)
{
loadfile2buf(&ipgaglist_mem,IPGAG_FILE);
}
	return ret;
}
/* Removes an entry from banlist or allowlist */
int unballow(char *buf, int type)
{
	int ret;
	char line[501];
	char path[MAX_FDP_LEN+1];

	if(type == BAN)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BAN_FILE);
	else if(type == ALLOW)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, ALLOW_FILE);
	else if(type == NICKBAN)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBAN_FILE);
	else if(type == IPGAG)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAG_FILE);
	else if(type == URL)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, URL_FILE);
	else if(type == RELEASE)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);
	else if(type == REQ)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);
	else if(type == BANRS)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
	else if(type == NICKBANRS)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
	else if(type == RANDWEL)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WELCOME_FILE);		

	else
		return -1;

	sscanf(buf, "%500[^|]", line);
	ret = remove_line_from_file(line, path, 0);

	if(type == BAN)
	{
	loadfile2buf(&banlist_mem,BAN_FILE);
	hammernick[0] = '\0';
	hammertime = (time_t)0; 
	}
	
	if(type == NICKBAN)
	{
	loadfile2buf(&nickbanlist_mem,NICKBAN_FILE);
	hammernick[0] = '\0';
	hammertime = (time_t)0; 
	}

	if(type == ALLOW)
	loadfile2buf(&allowlist_mem,ALLOW_FILE);
	if(type == IPGAG)
	loadfile2buf(&ipgaglist_mem,IPGAG_FILE);

	return ret;
}

/* Send banlist, allowlist or reg list to user */
void send_user_list(int type, struct user_t *user)
{
	FILE *fp;
	int fd;
	int erret;
	int rlown = 0;
	int rladm = 0;
	int rlcheef = 0;
	int rlop = 0;
	int rlreg = 0;
	int rlkvip = 0;
	int rlvip = 0;
	int rlprot = 0;
	int entries = 0;
	char rangtype[31];
	int bltb = 0;
	int blpb = 0;
	char line[4095];
	char pass[51];
	char nick[MAX_NICK_LEN+1];
	char sdayn[6];
	char sday[4];
	char sdate[11];
	char timestr[51];
	char newtimestr[51];
	char syear[6];
	char smont[6];
	struct tm * timeinfo;
	int regtype;
	char path[MAX_FDP_LEN+1];
	time_t ban_time;

	if(type == BAN)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BAN_FILE);
	else if(type == ALLOW)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, ALLOW_FILE);
	else if(type == REGLIST)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);
	else if(type == CONFIG)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CONFIG_FILE);
	else if(type == NICKBAN)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBAN_FILE);
	else if(type == IPGAG)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAG_FILE);
	else if(type == LANG)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, LANG_FILE);		
	else if(type == URL)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, URL_FILE);		
	else if(type == RELEASE)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);		
	else if(type == BANRS)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);		
	else if(type == NICKBANRS)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);		
	else if(type == REGME)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REGME_FILE);		
	else if(type == RULES)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RULES_FILE);		
	else if(type == RANDWEL)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WELCOME_FILE);		
	else if(type == AUTOFAV)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AUTOFAV_FILE);		
	else if(type == CATEG)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CATEG_FILE);		
	else if(type == WARN)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WARN_FILE);		

	else
		return;

	while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/open(): Interrupted system call. Trying again.\n");

	if(fd < 0)
	{
		logprintf(0, "Error - In send_user_list()/open(): %s", strerror(errno));
		return;
	}

	/* Set the lock */
	if(set_lock(fd, F_RDLCK) == 0)
	{
		logprintf(0, "Error - In send_user_list(): Couldn't set file lock\n");
		close(fd);
		return;
	}              

	if((fp = fdopen(fd, "r")) == NULL)
	{
		logprintf(0, "Error - In send_user_list()/fdopen(): %s", strerror(errno));
		set_lock(fd, F_UNLCK);
		close(fd);
		return;
	}

	if(fgets(line, 4094, fp) != NULL)
	{	
		
		trim_string(line);
		entries++;
		if(type == REGLIST)
		{
			sscanf(line, "%s %s %d %lu", nick, pass, &regtype, &ban_time);
		timeinfo = localtime ( &ban_time );
		snprintf(timestr, 50, "%s", asctime (timeinfo));
		trim_string(timestr);
		sscanf(timestr, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(newtimestr, 50, "%s %s %s, %s", sday, smont, syear, sdate);

		if(regtype == 7)
		{
		snprintf(rangtype, 30, "%s", lp_class9);
		rlown++;
		}
		else if(regtype == 6)
		{
		snprintf(rangtype, 30, "%s", lp_class8);		
		rladm++;
		}
		else if(regtype == 5)
		{
		snprintf(rangtype, 30, "%s", lp_class7);				
		rlcheef++;
		}
		else if(regtype == 4)
		{
		snprintf(rangtype, 30, "%s", lp_class6);				
		rlop++;
		}
		else if(regtype == 3)
		{
		snprintf(rangtype, 30, "%s", lp_class5);				
		rlkvip++;
		}
		else if(regtype == 2)
		{
		snprintf(rangtype, 30, "%s", lp_class4);						
		rlvip++;
		}
		else if(regtype == 1)
		{
		snprintf(rangtype, 30, "%s", lp_class3);						
		rlreg++;
		}
		else if(regtype == 0)
		{
		snprintf(rangtype, 30, "%s", lp_class2);						
		rlprot++;
		}
				if(expiredtime > 0)
				uprintf(user, "   %s   --- [ %s ] ---   %s: [ %s ]", nick, rangtype, lp_expires_, newtimestr);
				else
				uprintf(user, "   %s   --- [ %s ]", nick, rangtype);

		}
		
		else if((type == BAN) || (type == NICKBAN))
		{
		if(sscanf(line, "%s %lu", nick, &ban_time) < 2)
		{
		ban_time = 0;
		}
		if(ban_time > 0)
		{
		bltb++;
		timeinfo = localtime ( &ban_time );
		snprintf(timestr, 50, "%s", asctime (timeinfo));
		trim_string(timestr);
		sscanf(timestr, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(newtimestr, 50, "%s %s %s, %s", sday, smont, syear, sdate);
		}
		else
		{
		blpb++;
		snprintf(newtimestr, 50, "%s", lp_never_);
		}
		if(type != BAN)
		uprintf(user, "  %s:  ", lp_nickname);		
		else
		uprintf(user, "  IP:  ");
		if(add_reason(nick, type) == 1)
		{
		uprintf(user, "[   %s   ]     %s: [ %s ]     %s: %s", nick, lp_expires_, newtimestr, lp_reason, banreason);		
		}
		else
		uprintf(user, "[   %s   ]     %s: [ %s ]", nick, lp_expires_, newtimestr);
		}
		else if(type == IPGAG)
		{
		if(sscanf(line, "%s", nick) == 1)
		{
		if(add_reason(nick, type) == 1)
		{
		uprintf(user, "[  IP: %s   ]     %s: %s", nick, lp_reason, banreason);		
		}
		else
		uprintf(user, "[  IP: %s   ]", nick);
		}
		}
		else if(type == WARN)
		{
		if(sscanf(line, "%s %lu", nick, &ban_time) == 2)
		{
		timeinfo = localtime ( &ban_time );
		snprintf(timestr, 50, "%s", asctime (timeinfo));
		trim_string(timestr);
		sscanf(timestr, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(newtimestr, 50, "%s %s %s, %s", sday, smont, syear, sdate);
		uprintf(user, "[   %s   ]      %s       %s: [ %s ]", nick, line + strlen(nick) + 12, lp_expires_, newtimestr);		
		}
		}
		else
		uprintf(user, "%s", line); 
		while(fgets(line, 4094, fp) != NULL)
		{	     
			
			trim_string(line);
			entries++;
			if(type == REGLIST)
			{
				sscanf(line, "%s %s %d %lu", nick, pass, &regtype, &ban_time);
		timeinfo = localtime ( &ban_time );
		snprintf(timestr, 50, "%s", asctime (timeinfo));
		trim_string(timestr);
		sscanf(timestr, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(newtimestr, 50, "%s %s %s, %s", sday, smont, syear, sdate);


		if(regtype == 7)
		{
		snprintf(rangtype, 30, "%s", lp_class9);
		rlown++;
		}
		else if(regtype == 6)
		{
		snprintf(rangtype, 30, "%s", lp_class8);		
		rladm++;
		}
		else if(regtype == 5)
		{
		snprintf(rangtype, 30, "%s", lp_class7);				
		rlcheef++;
		}
		else if(regtype == 4)
		{
		snprintf(rangtype, 30, "%s", lp_class6);				
		rlop++;
		}
		else if(regtype == 3)
		{
		snprintf(rangtype, 30, "%s", lp_class5);				
		rlkvip++;
		}
		else if(regtype == 2)
		{
		snprintf(rangtype, 30, "%s", lp_class4);						
		rlvip++;
		}
		else if(regtype == 1)
		{
		snprintf(rangtype, 30, "%s", lp_class3);						
		rlreg++;
		}
		else if(regtype == 0)
		{
		snprintf(rangtype, 30, "%s", lp_class2);						
		rlprot++;
		}
				if(expiredtime > 0)
				uprintf(user, "\r\n   %s   --- [ %s ] ---   %s: [ %s ]", nick, rangtype, lp_expires_, newtimestr);
				else
				uprintf(user, "\r\n   %s   --- [ %s ]", nick, rangtype);


		}
		else if((type == BAN) || (type == NICKBAN))
		{
		if(sscanf(line, "%s %lu", nick, &ban_time) < 2)
		{
		ban_time = 0;
		}
		if(ban_time > 0)
		{
		bltb++;
		timeinfo = localtime ( &ban_time );
		snprintf(timestr, 50, "%s", asctime (timeinfo));
		trim_string(timestr);
		sscanf(timestr, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(newtimestr, 50, "%s %s %s, %s", sday, smont, syear, sdate);
		}
		else
		{
		blpb++;
		snprintf(newtimestr, 50, "%s", lp_never_);
		}
		if(type != BAN)
		uprintf(user, "\r\n  %s:  ", lp_nickname);		
		else
		uprintf(user, "\r\n  IP:  ");

		if(add_reason(nick, type) == 1)
		{
		uprintf(user, "[   %s   ]     %s: [ %s ]     %s: %s", nick, lp_expires_, newtimestr, lp_reason, banreason);
		}
		else
		uprintf(user, "[   %s   ]     %s: [ %s ]", nick, lp_expires_, newtimestr);

		}
		else if(type == IPGAG)
		{
		if(sscanf(line, "%s", nick) == 1)
		{
		if(add_reason(nick, type) == 1)
		{
		uprintf(user, "\r\n[  IP: %s   ]     %s: %s", nick, lp_reason, banreason);		
		}
		else
		uprintf(user, "\r\n[  IP: %s   ]", nick);
		}
		}
		
		else if(type == WARN)
		{
		if(sscanf(line, "%s %lu", nick, &ban_time) == 2)
		{
		timeinfo = localtime ( &ban_time );
		snprintf(timestr, 50, "%s", asctime (timeinfo));
		trim_string(timestr);
		sscanf(timestr, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(newtimestr, 50, "%s %s %s, %s", sday, smont, syear, sdate);
		uprintf(user, "\r\n[   %s   ]      %s       %s: [ %s ]", nick, line + strlen(nick) + 12, lp_expires_, newtimestr);		
		}
		}
		
			else
				uprintf(user, "\r\n%s", line);
		}	
	}
	if(type == REGLIST)
	{
	uprintf(user, "\r\n===== reglist %s =====\r\n", lp_statistics);
	uprintf(user, "[ %d ] %s\r\n", rlown, lp_owners_);	
	uprintf(user, "[ %d ] %s\r\n", rladm, lp_admins_);	
	uprintf(user, "[ %d ] %s\r\n", rlcheef, lp_cheefs_);	
	uprintf(user, "[ %d ] %s\r\n", rlop, lp_ops_);	
	uprintf(user, "[ %d ] %s\r\n", rlkvip, lp_kvips_);	
	uprintf(user, "[ %d ] %s\r\n", rlvip, lp_vips_);	
	uprintf(user, "[ %d ] %s\r\n", rlreg, lp_regs_);	
	uprintf(user, "[ %d ] %s\r\n", rlprot, lp_prots_);	
	uprintf(user, "%s: [ %d ]\r\n", lp_total_, entries);
	}
	if(type == BAN)
	{
	uprintf(user, "\r\n===== banlist %s =====\r\n", lp_statistics);
	uprintf(user, "[ %d ] %s\r\n", bltb, lp_tempbans);	
	uprintf(user, "[ %d ] %s\r\n", blpb, lp_permbans);	
	uprintf(user, "%s: [ %d ]\r\n", lp_total_, entries);
	}
	if(type == NICKBAN)
	{
	uprintf(user, "\r\n===== nickbanlist %s =====\r\n", lp_statistics);
	uprintf(user, "[ %d ] %s\r\n", bltb, lp_tempbans);	
	uprintf(user, "[ %d ] %s\r\n", blpb, lp_permbans);	
	uprintf(user, "%s: [ %d ]\r\n", lp_total_, entries);
	}
	if(type == WARN)
	{
	uprintf(user, "\r\n--- %d %s ---|", entries, lp_users);
	}
	
	
	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/fclose(): Interrupted system call. Trying again.\n");

	if(erret != 0)
	{
		logprintf(0, "Error - In send_user_list()/fclose(): %s", strerror(errno));
	}	
}

/* Redirect a user by request from op or admin, the format is:
* $OpForceMove $Who:nick$Where:redirectip$Msg:message| */
void op_force_move(char *buf, struct user_t *user)
{
	char command[21];
	char *temp;
	char nick[MAX_NICK_LEN+1];
	char ip[MAX_HOST_LEN+1];
	char message[11];
	struct user_t *to_user;
	char quit_string[101];
	int num;

	num = sscanf(buf, "%20s $Who:%50[^$]$Where:%121[^$]$Msg:%10[^|]|", command, nick, ip, message);
	if(user->type != FORKED)
	{
		if(num != 4)
		{		
			logprintf(4, "Received bad $OpForceMove command from %s at %s:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}

		if(message[0] == '\0')
		{                                                         
			logprintf(4, "Received bad $OpForceMove command from %s at %s:\n", user->nick, user->hostname);
			if(strlen(buf) < 3500)
				logprintf(4, "%s\n", buf);
			else
				logprintf(4, "too large buf\n");
			return;
		}
	}

	if((temp = strstr(buf, "$Msg:")) == NULL)
	{                                                         
		logprintf(4, "Received bad $OpForceMove command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}

	/* If we received the command directly from the user.  */
	if(user->type != FORKED)
	{
		if(check_if_on_user_list(nick) != NULL)
			logprintf(4, "%s was redirected to %s by %s\n", nick, ip, user->nick);	   
		else
		{
			return;
		}

		temp += 5;

		/* First check if user is connected to this process */
		if((to_user = get_human_user(nick)) != NULL)
		{	
			uprintf(to_user, "$To: %s From: %s $<%s> ", nick, user->nick, user->nick);
			send_to_user(temp, to_user);
		}
		else
		{
			uprintf(non_human_user_list, "$To: %s From: %s $<%s> ", nick, user->nick, user->nick);
			send_to_user(temp, non_human_user_list);
		}
	}

	/* If the user to be redirected is connected to this process.  */
	if((to_user = get_human_user(nick)) != NULL)
	{
		/* And then the ForceMove command */
		uprintf(to_user, "$ForceMove %s|", ip);
		remove_user_from_list(to_user->nick);
		remove_human_from_hash(to_user->nick);
		to_user->type = NON_LOGGED;
	    
		/* Remove the users share from the total share.  */
		if(to_user->share > 0)
		{
		total_share -= to_user->share;
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif
		}
		snprintf(quit_string, 100, "$Quit %s|", to_user->nick);
		send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, to_user);
		total_users--;
		
#ifdef HAVE_PERL		       
		command_to_scripts("$Script user_disconnected %c%c", '\005', '\005');
		non_format_to_scripts(to_user->nick);
		command_to_scripts("|");		       
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif		       	
	}
}

/* Redirect all users to address specified in buf */
void redirect_all(char *buf, struct user_t *user)
{
	char move_string[MAX_HOST_LEN+20];

	redall = 1;
	snprintf(move_string, MAX_HOST_LEN, "$ForceMove %s", buf);
	
	send_to_humans(move_string, REGULAR, user);
	remove_all(NON_LOGGED | REGULAR, 1, 1);
    
	redall = 0;
	/* To scripts. */
#ifdef HAVE_PERL
	command_to_scripts("$Script started_redirecting %c%c", '\005', '\005');
	non_format_to_scripts(buf);
#endif
}

void send_commands(struct user_t *user)
{
	char buf[51];

	switch (user->type)
	{
	case REGULAR: snprintf(buf, 50, "%s", lp_class1); break;
	case PROTECTED: snprintf(buf, 50, "%s", lp_class2); break;
	case REG: snprintf(buf, 50, "%s", lp_class3); break;
	case VIP: snprintf(buf, 50, "%s", lp_class4); break;
	case KVIP: snprintf(buf, 50, "%s", lp_class5); break;
	case OP: snprintf(buf, 50, "%s", lp_class6); break;
	case CHEEF: snprintf(buf, 50, "%s", lp_class7); break;
	case ADMIN:	snprintf(buf, 50, "%s", lp_class8); break;
	case OWNER:	snprintf(buf, 50, "%s", lp_class9); break;
	}

	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	if(help_on_priv == 0)
	uprintf(user, "<%s> %s (%s):\r\n", botnick, lp_availablecommands, buf);
	else
	uprintf(user, "$To: %s From: %s $<%s> %s (%s):\r\n", user->nick, botnick, botnick, lp_availablecommands, buf);
	}
        if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
        {
                uprintf(user, "\r\n");

                uprintf(user, "\t!help\t\t\t\t\t- %s\r\n", lp_he001);
                uprintf(user, "\t!rules\t\t\t\t\t- %s\r\n", lp_he002);
                uprintf(user, "\t!regme\t\t\t\t\t- %s\r\n", lp_he003);
                if((protection == 1) && (user->type == REGULAR))
                uprintf(user, "\t!protectme '%s'\t\t\t- %s\r\n", lp_pass_, lp_he004);
                if((self_registration == 1) && (user->type == REGULAR) && (user->share >= min_regme_share))
                uprintf(user, "\t!regme '%s'\t\t\t- %s\r\n", lp_pass_, lp_he005);
	    if(user->gagc == 0)
	    {
                uprintf(user, "\t!motd\t\t\t\t\t- %s\r\n", lp_he006);
                uprintf(user, "\t!me\t\t\t\t\t- %s\r\n", lp_he007);
                uprintf(user, "\t!myip\t\t\t\t\t- %s\r\n", lp_he008);
                uprintf(user, "\t!myinfo\t\t\t\t\t- %s\r\n", lp_he009);
                uprintf(user, "\t!hubinfo\t\t\t\t\t- %s\r\n", lp_he010);
                uprintf(user, "\t!report '%s' '%s'\t\t\t- %s\r\n", lp_user_, lp_reason, lp_he011);
                uprintf(user, "\t!seen '%s'\t\t\t\t- %s\r\n", lp_nick_, lp_he012);
                uprintf(user, "\t!chat\t\t\t\t\t- %s\r\n", lp_he013);
                uprintf(user, "\t!fs\t\t\t\t\t- %s\r\n", lp_he014);
                uprintf(user, "\t!away '%s'\t\t\t\t- %s\r\n", lp_reason, lp_he015);
                uprintf(user, "\t!back\t\t\t\t\t- %s\r\n", lp_he016);
                uprintf(user, "\t!autoaway '%s', '%s'\t\t\t- %s\r\n", lp_time_, lp_reason, lp_he017);
                uprintf(user, "\t!noautoaway\t\t\t\t- %s\r\n", lp_he018);
                uprintf(user, "\t!date\t\t\t\t\t- %s\r\n", lp_he019);
                uprintf(user, "\t!longest\t\t\t\t\t- %s.\r\n", lp_he020);
                uprintf(user, "\t!record\t\t\t\t\t- %s\r\n", lp_he021);
                uprintf(user, "\t!uptime\t\t\t\t\t- %s\r\n", lp_he022);
                uprintf(user, "\t!geturl\t\t\t\t\t- %s\r\n", lp_he023);
		if(check_if_registered(user->nick) >= who_can_addrls)
		{
		uprintf(user, "\t!addrls '%s' '%s'\t\t\t- %s\r\n", lp_categletter_, lp_file_, lp_he024);
                uprintf(user, "\t!remrls '%s'\t\t\t\t- %s\r\n", lp_name_, lp_he108);                
		}
		if(check_if_registered(user->nick) >= who_can_seerls)
                {
		uprintf(user, "\t!getrls\t\t\t\t\t- %s\r\n", lp_he025);
                uprintf(user, "\t!getrls '%s'\t\t\t\t- %s\r\n", lp_category_, lp_he026);
		}
		if(check_if_registered(user->nick) >= who_can_seerls)
                uprintf(user, "\t!findrls '%s'\t\t\t\t- %s\r\n", lp_name_, lp_he027);
		if(check_if_registered(user->nick) >= who_can_addreq)
		{
		uprintf(user, "\t!addreq '%s'\t\t\t\t- %s\r\n", lp_file_, lp_he028);
		uprintf(user, "\t!remreq '%s'\t\t\t\t- %s\r\n", lp_name_, lp_he109);                
		}
		if(check_if_registered(user->nick) >= who_can_seereq)
                uprintf(user, "\t!getreq\t\t\t\t\t- %s\r\n", lp_he029);
                if((offline_msgs == 2) || ((offline_msgs == 1) && ((user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)))
                uprintf(user, "\t!msg '%s' '%s'\t\t\t- %s\r\n", lp_nick_, lp_message_, lp_he030);
                if(offline_msgs > 0)
                uprintf(user, "\t!readmsg\t\t\t\t\t- %s\r\n", lp_he031);
                if(offline_msgs > 0)
                uprintf(user, "\t!delsmg\t\t\t\t\t- %s\r\n", lp_he032);
	    }
        }
        
        if((user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
        {
                uprintf(user, "\r\n");
                if((user->type & (KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
                {
		uprintf(user, "\t!ui '%s'\t\t\t\t\t- %s\r\n", lp_user_, lp_he033);
                if((user->type & (ADMIN | OWNER)) != 0)
		uprintf(user, "\t!drop '%s'\t\t\t\t- %s\r\n", lp_user_, lp_he125);
		uprintf(user, "\t!kick '%s' '%s'\t\t\t\t- %s\r\n", lp_user_, lp_reason, lp_he034);
                }
		uprintf(user, "\t!mypass\t\t\t\t\t- %s\r\n", lp_he035);
                uprintf(user, "\t!setpass '%s'\t\t\t\t- %s\r\n", lp_pass_, lp_he036);
        }
        if((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
        {
                uprintf(user, "\r\n");
                uprintf(user, "\t!addprot '%s' '%s' \t\t- %s\r\n", lp_nick_, lp_pass_, lp_he037);
                uprintf(user, "\t!addprot '%s' '%s' '%s'\t\t- %s\r\n", lp_nick_, lp_pass_, lp_days, lp_he038);
                if(user->type != OP)
                {
		uprintf(user, "\t!addreg '%s' '%s'\t\t\t- %s\r\n", lp_nick_, lp_pass_, lp_he039);
		uprintf(user, "\t!addvip '%s' '%s'\t\t\t- %s\r\n", lp_nick_, lp_pass_, lp_he040);
		uprintf(user, "\t!addkvip '%s' '%s'\t\t- %s\r\n", lp_nick_, lp_pass_, lp_he041);
		uprintf(user, "\t!changeregclass '%s' '%s'\t\t\t- %s\r\n", lp_nick_, lp_type_, lp_he127);
		uprintf(user, "\t!changeregnick '%s' '%s'\t\t\t- %s\r\n", lp_nick_, lp_nick_, lp_he128);
		}
		uprintf(user, "\t!warn '%s' '%s'\t\t\t\t- %s\r\n", lp_user_, lp_reason, lp_he042);
                uprintf(user, "\t!getwarn \t\t\t\t\t- %s\r\n", lp_he117);
		uprintf(user, "\t!remwarn '%s'\t\t\t\t- %s\r\n", lp_user_, lp_he111);
                uprintf(user, "\t!ban '%s'\t\t\t\t- %s\r\n", lp_user_, lp_he043);
                uprintf(user, "\t!banip 'IP' '%s'\t\t\t\t- %s\r\n",  lp_reason, lp_he044);
                uprintf(user, "\t!bannick '%s' '%s'\t\t\t- %s\r\n", lp_nick_, lp_reason, lp_he045);
                uprintf(user, "\t!tban '%s' '%s' '%s'\t\t\t\t- %s\r\n", lp_time_, lp_user_, lp_reason, lp_he122);
                uprintf(user, "\t!tbanip '%s' 'IP' '%s'\t\t\t\t- %s\r\n",  lp_time_, lp_reason, lp_he123);
                uprintf(user, "\t!tbannick '%s' '%s' '%s'\t\t\t- %s\r\n", lp_time_, lp_nick_, lp_reason, lp_he124);
                uprintf(user, "\t!banlist\t\t\t\t- %s\r\n", lp_he046);
                uprintf(user, "\t!unban 'IP'\t\t\t\t- %s\r\n", lp_he048);
                uprintf(user, "\t!unnickban '%s'\t\t\t- %s\r\n", lp_nick_, lp_he049);
                uprintf(user, "\t!lastperm\t\t\t\t\t- %s\r\n", lp_he050);
                uprintf(user, "\t!lasttemp\t\t\t\t\t- %s\r\n", lp_he051);
		uprintf(user, "\t!allow 'IP'\t\t\t\t\t- %s\r\n", lp_he052);
                uprintf(user, "\t!allowlist\t\t\t\t- %s\r\n", lp_he053);
                uprintf(user, "\t!unallow 'IP'\t\t\t\t- %s\r\n", lp_he054);
                uprintf(user, "\t!gag '%s' '%s'\t\t\t\t- %s\r\n", lp_user_, lp_type_, lp_he055);
                uprintf(user, "\t!gagip 'IP' '%s'\t\t\t\t\t- %s\r\n", lp_reason, lp_he056);
                uprintf(user, "\t!getgaglist\t\t\t\t- %s\r\n", lp_he057);
                uprintf(user, "\t!ipgaglist\t\t\t\t\t- %s\r\n", lp_he058);
		uprintf(user, "\t!ungag '%s'\t\t\t\t- %s\r\n", lp_user_, lp_he059);
		uprintf(user, "\t!ungagip 'IP'\t\t\t\t- %s\r\n", lp_he060);
                uprintf(user, "\t!ipinfo 'IP'\t\t\t\t\t- %s\r\n", lp_he063);
                uprintf(user, "\t!whoip 'IP'\t\t\t\t- %s\r\n", lp_he064);
                uprintf(user, "\t!whohubs '%s'\t\t\t\t- %s\r\n", lp_number_, lp_he065);
		uprintf(user, "\t!whoslots '%s'\t\t\t\t- %s\r\n", lp_number_, lp_he066);
                uprintf(user, "\t!whoratio '%s'\t\t\t\t- %s\r\n", lp_number_, lp_he067);
                uprintf(user, "\t!fav\t\t\t\t\t- %s\r\n", lp_he068);
                uprintf(user, "\t!mass_n '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he069);
                uprintf(user, "\t!mass_p '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he070);
                uprintf(user, "\t!mass_r '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he071);
                uprintf(user, "\t!mass_v '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he072);
                uprintf(user, "\t!mass_k '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he073);
                uprintf(user, "\t!mass_o '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he074);
                if((user->type & (CHEEF | ADMIN | OWNER)) != 0)
		uprintf(user, "\t!mass_c '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he075);
                if((user->type & (ADMIN | OWNER)) != 0)
		uprintf(user, "\t!mass_a '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he076);
                uprintf(user, "\t!mass '%s'\t\t\t\t- %s\r\n", lp_message_, lp_he077);
                if((user->type & (CHEEF | ADMIN | OWNER)) != 0)
                {
		uprintf(user, "\t!topic '%s'\t\t\t\t- %s\r\n", lp_text_, lp_he078);
                uprintf(user, "\t!untopic\t\t\t\t\t- %s\r\n", lp_he079);
                }
		uprintf(user, "\t!hide\t\t\t\t\t- %s\r\n", lp_he080);
                uprintf(user, "\t!unhide\t\t\t\t\t- %s\r\n", lp_he081);
                if((user->type & (ADMIN | OWNER)) != 0)
                {
		uprintf(user, "\t!hideme\t\t\t\t\t- %s\r\n", lp_he082);
                uprintf(user, "\t!unhideme\t\t\t\t- %s\r\n", lp_he083);
		}
                
        }
        if((user->type & (ADMIN | OWNER)) != 0)
        {
                uprintf(user, "\r\n");
                uprintf(user, "\t!config\t\t\t\t\t- %s\r\n", lp_he084);
		uprintf(user, "\t!set '%s' '%s'\t\t\t\t- %s\r\n", lp_variable, lp_value, lp_he085);
#ifdef HAVE_LANG
                uprintf(user, "\t!lang\t\t\t\t\t- %s\r\n", lp_he086);
                uprintf(user, "\t!lang '%s' '%s'\t\t\t- %s\r\n", lp_variable, lp_value, lp_he087);
#endif
                uprintf(user, "\t!reglist\t\t\t\t\t- %s\r\n", lp_he088);
#ifdef HAVE_PERL
                uprintf(user, "\t!reloadscripts\t\t\t\t- %s\r\n", lp_he089);
#endif
#ifdef HAVE_LANG
                uprintf(user, "\t!reloadlang\t\t\t\t- %s\r\n", lp_he090);
                uprintf(user, "\t!resetlang\t\t\t\t\t- %s\r\n", lp_he091);
#endif
                uprintf(user, "\t!resetrecord\t\t\t\t- %s\r\n", lp_he129);
		uprintf(user, "\t!addop '%s' '%s'\t\t\t- %s\r\n", lp_nick_, lp_pass_, lp_he092);
                uprintf(user, "\t!addcheef '%s' '%s'\t\t- %s\r\n", lp_nick_, lp_pass_, lp_he093);
		if(user->type == OWNER)
		{
		uprintf(user, "\t!addadmin '%s' '%s'\t\t- %s\r\n", lp_nick_, lp_pass_, lp_he094);
		uprintf(user, "\t!addowner '%s' '%s'\t\t- %s\r\n", lp_nick_, lp_pass_, lp_he095);
		}
		uprintf(user, "\t!deluser '%s'\t\t\t\t- %s\r\n", lp_nick_, lp_he096);
                uprintf(user, "\t!op '%s'\t\t\t\t\t- %s\r\n", lp_user_, lp_he097);		
                uprintf(user, "\t!bot '%s'\t\t\t\t\t- %s\r\n", lp_text_, lp_he098);
		uprintf(user, "\t!say '%s' '%s'\t\t\t- %s\r\n",  lp_nick_, lp_text_, lp_he099);
		uprintf(user, "\t!clearipbans\t\t\t\t- %s\r\n", lp_he120);
		uprintf(user, "\t!clearnickbans\t\t\t\t- %s\r\n", lp_he121);                
		uprintf(user, "\t!block main/pm/search/download\t\t- %s\r\n", lp_he100);
		uprintf(user, "\t!unblock main/pm/search/download\t\t- %s\r\n", lp_he101);
                uprintf(user, "\t!redirectall 'IP'\t\t\t\t- %s\r\n", lp_he102);
                uprintf(user, "\t!gagall\t\t\t\t\t- %s\r\n", lp_he103);
		uprintf(user, "\t!ungagall\t\t\t\t\t- %s\r\n", lp_he104);
                uprintf(user, "\t!addfile '%s' '%s'\t\t- %s\r\n", lp_file_, lp_text_, lp_he118);                
                uprintf(user, "\t!remfile '%s'\t\t\t- %s\r\n", lp_file_, lp_he119);
                uprintf(user, "\t!addcateg '%s' '%s'\t\t- %s\r\n", lp_categletter_, lp_category_, lp_he105);                
                uprintf(user, "\t!getcateg\t\t\t\t\t- %s\r\n", lp_he106);
                uprintf(user, "\t!remcateg '%s'\t\t\t- %s\r\n", lp_categletter_, lp_he107);
                uprintf(user, "\t!remrls '%s'\t\t\t\t- %s\r\n", lp_name_, lp_he108);                
                uprintf(user, "\t!remreq '%s'\t\t\t\t- %s\r\n", lp_name_, lp_he109);                
                uprintf(user, "\t!remurl '%s'\t\t\t\t- %s\r\n", lp_name_, lp_he110);
                uprintf(user, "\t!addwelcome '%s'\t\t\t\t- %s\r\n", lp_text_, lp_he112);
                uprintf(user, "\t!getwelcomes\t\t\t\t- %s\r\n", lp_he113);
                uprintf(user, "\t!remwelcome '%s(unix)' \t\t\t- %s\r\n", lp_time_, lp_he114);
		if(user->type == OWNER)
                {
		uprintf(user, "\t!reloadfile '%s'\t\t\t\t- %s\r\n", lp_file_, lp_he126);
		uprintf(user, "\t!quitprogram\t\t\t\t- %s\r\n", lp_he116);
    		}
	}
	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	send_to_user("|", user);
}

void send_raws(struct user_t *user)
{

		uprintf(user, "$UserCommand 255 7||$UserCommand 0 7||");	
        if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{		
	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !help&#124;|", hub_name, lp_rawuser, lp_us001, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !rules&#124;|", hub_name, lp_rawuser, lp_us002, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !regme&#124;|", hub_name, lp_rawuser, lp_us003, user->nick);	
		if((protection == 1) && (user->type == REGULAR))
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !protectme %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us004, user->nick, lp_pass_);	
		if((self_registration == 1) && (user->type == REGULAR) && (user->share >= min_regme_share))
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !regme %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us005, user->nick, lp_pass_);	
	    if(user->gagc == 0)
	    {
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !motd&#124;|", hub_name, lp_rawuser, lp_us006, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !me %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us007, user->nick, lp_text_);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !myip&#124;|", hub_name, lp_rawuser, lp_us008, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !myinfo&#124;|", hub_name, lp_rawuser, lp_us009, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !hubinfo&#124;|", hub_name, lp_rawuser, lp_us010, user->nick);	
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !report %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us011, user->nick, lp_reason);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !seen %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us012, user->nick, lp_nick_);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !chat&#124;|", hub_name, lp_rawuser, lp_us013, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !fs&#124;|", hub_name, lp_rawuser, lp_us014, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !away %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us015, user->nick, lp_reason);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !back&#124;|", hub_name, lp_rawuser, lp_us016, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !autoaway %%[line:%s], %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us017, user->nick, lp_time_, lp_reason);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !noautoaway&#124;|", hub_name, lp_rawuser, lp_us018, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !date&#124;|", hub_name, lp_rawuser, lp_us019, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !longest&#124;|", hub_name, lp_rawuser, lp_us020, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !record&#124;|", hub_name, lp_rawuser, lp_us021, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !uptime&#124;|", hub_name, lp_rawuser, lp_us022, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !geturl&#124;|", hub_name, lp_rawuser, lp_us023, user->nick);	
		if(check_if_registered(user->nick) >= who_can_addrls)
		{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addrls %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us024, user->nick, lp_categletter_, lp_file_);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !remrls %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us114, user->nick, lp_name_);
		}
		if(check_if_registered(user->nick) >= who_can_seerls)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !getrls&#124;|", hub_name, lp_rawuser, lp_us025, user->nick);	
		if(check_if_registered(user->nick) >= who_can_seerls)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !getrls %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us026, user->nick, lp_categletter_);			
		if(check_if_registered(user->nick) >= who_can_seerls)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !findrls %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us027, user->nick, lp_name_);	
		if(check_if_registered(user->nick) >= who_can_addreq)
		{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addreq %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us028, user->nick, lp_file_);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !remreq %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us115, user->nick, lp_name_);
		}
		if(check_if_registered(user->nick) >= who_can_seereq)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !getreq&#124;|", hub_name, lp_rawuser, lp_us029, user->nick);	
		if((offline_msgs == 2) || ((offline_msgs == 1) && ((user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)))
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !msg %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us030, user->nick, lp_nick_, lp_message_);	
		if(offline_msgs > 0)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !readmsg&#124;|", hub_name, lp_rawuser, lp_us031, user->nick);	
		if(offline_msgs > 0)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !delmsg&#124;|", hub_name, lp_rawuser, lp_us032, user->nick);	
	    }
    	}

        if((user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{		
		if((user->type & (CHEEF | ADMIN | OWNER | OP | KVIP)) != 0)
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !ui %%[nick]&#124;|", hub_name, lp_rawop, lp_us033, user->nick);
		if((user->type & (CHEEF | ADMIN | OWNER | OP | KVIP)) != 0)
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !kick %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us034, user->nick, lp_reason);
		if((user->type & (ADMIN | OWNER)) != 0)
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !drop %%[nick]&#124;|", hub_name, lp_rawop, lp_us131, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mypass&#124;|", hub_name, lp_rawuser, lp_us035, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !setpass %%[line:%s]&#124;|", hub_name, lp_rawuser, lp_us036, user->nick, lp_pass_);
	}
        if((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
	{		
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addprot %%[nick] %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us037, user->nick, lp_pass_, lp_days);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addprot %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us038, user->nick, lp_pass_);
		if(user->type != OP)
		{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addreg %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us039, user->nick, lp_pass_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addvip %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us040, user->nick, lp_pass_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addkvip %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us041, user->nick, lp_pass_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !changeregclass %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us133, user->nick, lp_type_);		
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !changeregnick %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us134, user->nick, lp_nick_);
		}
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !warn %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us042, user->nick, lp_reason);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !getwarn&#124;|", hub_name, lp_rawadm, lp_us123, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !remwarn %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us117, user->nick, lp_user_);
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !ban %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us043, user->nick, lp_reason);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !banip %%[line:IP] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us044, user->nick, lp_reason);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !bannick %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us045, user->nick, lp_reason);
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !tban %%[line:%s] %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us128, user->nick, lp_time_, lp_reason);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !tbanip %%[line:%s] %%[line:IP] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us129, user->nick, lp_time_, lp_reason);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !tbannick %%[line:%s] %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us130, user->nick, lp_time_, lp_reason);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !banlist&#124;|", hub_name, lp_rawop, lp_us046, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unban %%[line:IP]&#124;|", hub_name, lp_rawop, lp_us048, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unnickban %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us049, user->nick, lp_nick_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !lastperm&#124;|", hub_name, lp_rawop, lp_us050, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !lasttemp&#124;|", hub_name, lp_rawop, lp_us051, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !allow %%[line:IP]&#124;|", hub_name, lp_rawop, lp_us052, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !allowlist&#124;|", hub_name, lp_rawop, lp_us053, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unallow %%[line:IP]&#124;|", hub_name, lp_rawop, lp_us054, user->nick);
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !gag %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us055, user->nick, lp_type_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !gagip %%[line:IP] %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us056, user->nick, lp_reason);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !getgaglist&#124;|", hub_name, lp_rawop, lp_us057, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !ipgaglist&#124;|", hub_name, lp_rawop, lp_us058, user->nick);	
		uprintf(user, "$UserCommand 2 6 %s\\%s\\%s $<%s> !ungag %%[nick]&#124;|", hub_name, lp_rawop, lp_us059, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !ungagip %%[line:IP]&#124;|", hub_name, lp_rawop, lp_us060, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !ipinfo %%[line:IP]&#124;|", hub_name, lp_rawop, lp_us063, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !whoip %%[line:IP]&#124;|", hub_name, lp_rawop, lp_us064, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !whohubs %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us065, user->nick, lp_number_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !whoslots %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us066, user->nick, lp_number_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !whoratio %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us067, user->nick, lp_number_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !fav&#124;|", hub_name, lp_rawop, lp_us068, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_n %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us069, user->nick, lp_message_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_p %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us070, user->nick, lp_message_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_r %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us071, user->nick, lp_message_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_v %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us072, user->nick, lp_message_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_k %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us073, user->nick, lp_message_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_o %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us074, user->nick, lp_message_);
		if((user->type & (CHEEF | ADMIN | OWNER)) != 0)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_c %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us075, user->nick, lp_message_);
		if((user->type & (ADMIN | OWNER)) != 0)
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass_a %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us076, user->nick, lp_message_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !mass %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us077, user->nick, lp_message_);
		if((user->type & (CHEEF | ADMIN | OWNER)) != 0)
		{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !topic %%[line:%s]&#124;|", hub_name, lp_rawop, lp_us078, user->nick, lp_text_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !untopic&#124;|", hub_name, lp_rawop, lp_us079, user->nick);
		}
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !hide&#124;|", hub_name, lp_rawop, lp_us080, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unhide&#124;|", hub_name, lp_rawop, lp_us081, user->nick);	
		if((user->type & (ADMIN | OWNER)) != 0)
		{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !hideme&#124;|", hub_name, lp_rawop, lp_us082, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unhideme&#124;|", hub_name, lp_rawop, lp_us083, user->nick);	
		}
	}
	if((user->type & (ADMIN | OWNER)) != 0)
	{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !config&#124;|", hub_name, lp_rawadm, lp_us084, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !set %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us085, user->nick, lp_variable, lp_value);	
#ifdef HAVE_LANG
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !lang&#124;|", hub_name, lp_rawadm, lp_us086, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !lang %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us087, user->nick, lp_variable, lp_value);	
#endif
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !reglist&#124;|", hub_name, lp_rawadm, lp_us088, user->nick);	
#ifdef HAVE_PERL
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !reloadscripts&#124;|", hub_name, lp_rawadm, lp_us089, user->nick);	
#endif
#ifdef HAVE_LANG
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !reloadlang&#124;|", hub_name, lp_rawadm, lp_us090, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !resetlang&#124;|", hub_name, lp_rawadm, lp_us091, user->nick);	
#endif
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !resetrecord&#124;|", hub_name, lp_rawadm, lp_us135, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addop %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us092, user->nick, lp_pass_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addcheef %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us093, user->nick, lp_pass_);
		if(user->type == OWNER)
		{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addadmin %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us094, user->nick, lp_pass_);		
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addowner %%[nick] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us095, user->nick, lp_pass_);		
		}
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !deluser %%[nick]&#124;|", hub_name, lp_rawadm, lp_us096, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !op %%[nick]&#124;|", hub_name, lp_rawadm, lp_us097, user->nick);		
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !bot %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us098, user->nick, lp_text_);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !say %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us099, user->nick, lp_nick_, lp_text_);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !clearipbans&#124;|", hub_name, lp_rawadm, lp_us126, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !clearnickbans&#124;|", hub_name, lp_rawadm, lp_us127, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !block download&#124;|", hub_name, lp_rawadm, lp_us100, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !block main&#124;|", hub_name, lp_rawadm, lp_us101, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !block pm&#124;|", hub_name, lp_rawadm, lp_us102, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !block search&#124;|", hub_name, lp_rawadm, lp_us103, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unblock download&#124;|", hub_name, lp_rawadm, lp_us104, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unblock main&#124;|", hub_name, lp_rawadm, lp_us105, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unblock pm&#124;|", hub_name, lp_rawadm, lp_us106, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !unblock search&#124;|", hub_name, lp_rawadm, lp_us107, user->nick);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !redirectall %%[line:IP]&#124;|", hub_name, lp_rawadm, lp_us108, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !gagall&#124;|", hub_name, lp_rawadm, lp_us109, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !ungagall&#124;|", hub_name, lp_rawadm, lp_us110, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addfile %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us124, user->nick, lp_file_, lp_text_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !delfile %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us125, user->nick, lp_file_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addcateg %%[line:%s] %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us111, user->nick, lp_categletter_, lp_category_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !getcateg&#124;|", hub_name, lp_rawadm, lp_us112, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !remcateg %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us113, user->nick, lp_categletter_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !remurl %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us116, user->nick, lp_name_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !addwelcome %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us118, user->nick, lp_text_);
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !getwelcomes&#124;|", hub_name, lp_rawadm, lp_us119, user->nick);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !remwelcome %%[line:%s(unix)]&#124;|", hub_name, lp_rawadm, lp_us120, user->nick, lp_time_);
		if(user->type == OWNER)
		{
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !reloadfile %%[line:%s]&#124;|", hub_name, lp_rawadm, lp_us132, user->nick, lp_file_);	
		uprintf(user, "$UserCommand 2 3 %s\\%s\\%s $<%s> !quitprogram&#124;|", hub_name, lp_rawadm, lp_us122, user->nick);	
		}
	}
}

void send_mass_message(char *buffy, int type)
{
        struct sock_t *human_user;
	struct sock_t *next_human;

			human_user = human_sock_list;
			while(human_user != NULL)
			{
			next_human = human_user->next;
			if((type == 0) && ((human_user->user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, buffy);
			if((type == 1) && (human_user->user->type == REGULAR))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class1, buffy);
			if((type == 2) && (human_user->user->type == PROTECTED))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class2, buffy);
			if((type == 3) && (human_user->user->type == REG))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class3, buffy);
			if((type == 4) && (human_user->user->type == VIP))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class4, buffy);
			if((type == 5) && (human_user->user->type == KVIP))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class5, buffy);
			if((type == 6) && (human_user->user->type == OP))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class6, buffy);
			if((type == 7) && (human_user->user->type == CHEEF))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class7, buffy);
			if((type == 8) && ((human_user->user->type == ADMIN) || (human_user->user->type == OWNER)))
			uprintf(human_user->user, "$To: %s From: %s $<%s> %s [ %s ]:\r\n%s|", human_user->user->nick, botnick, botnick, lp_hubmassmsg, lp_class8, buffy);
			human_user = next_human;
			}			


	/* Send to scripts */
#ifdef HAVE_PERL
	command_to_scripts("$Script mass_message %c%c", '\005', '\005');
	non_format_to_scripts(buffy);
#endif
}

/* Remove all expired temporary bans.  */
void remove_expired(void)
{
	char path[MAX_FDP_LEN+1];
	time_t now_time;
	now_time = time(NULL);

	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BAN_FILE);
	if(remove_exp_from_file(now_time, path) == 0)
	loadfile2buf(&banlist_mem,BAN_FILE);

	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, ALLOW_FILE);
	if(remove_exp_from_file(now_time, path) == 0)
	loadfile2buf(&allowlist_mem,ALLOW_FILE);

	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBAN_FILE);
	if(remove_exp_from_file(now_time, path) == 0)
	{
	loadfile2buf(&nickbanlist_mem,NICKBAN_FILE);
	hammernick[0] = '\0';
	hammertime = (time_t)0; 
	}

	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
	remove_exp_from_file(now_time, path);

	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
	remove_exp_from_file(now_time, path);

	if(expiredwarn != 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WARN_FILE);
	if(remove_exp_from_file(now_time, path) == 0)
	loadfile2buf(&warn_mem,WARN_FILE);
	}
	
	if(expiredrls != 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);
	if(remove_exp_from_file(now_time, path) == 0)
	rls_motd();
	}
	
	if(expiredurl != 0)
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, URL_FILE);
	remove_exp_from_file(now_time, path);
	}
	
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SEEN_FILE);
	remove_exp_from_file(now_time, path);
	
	if(expiredreq != 0) 
	{
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);
	if(remove_exp_from_file(now_time, path) == 0)
	req_motd();
	}
}
/* GAG human users */
void gag_user(char *buf, struct user_t *user)
{
	char bigstring[1001];
	char nick[51];
	char gtype[21];
	struct user_t *targ_user;

	if((strstr(buf,"chat") != NULL) | (strstr(buf,"download") != NULL) | (strstr(buf,"search") != NULL))
	{
	sscanf(buf, "%50s %20[^|]|", nick, gtype);
	    
	    if((strncmp(gtype, "chat", 4) == 0))
	    {
		if((targ_user = get_human_user(nick)) != NULL)
		{
		targ_user->gagc=1;
		uprintf(user, "<%s> [ %s ] %s %s|", botnick, targ_user->nick, lp_wasgaggedfrom, lp_chatting_);
			if(show_actions > 0)
			{
				snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s [ %s ]|", targ_user->nick, targ_user->hostname, lp_wasgaggedby, user->nick, lp_from_, lp_chatting_);
				send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			}
		}
		else
		uprintf(user, "<%s> '%s' %s|", botnick, nick, lp_isoffline);
	    }
	    if((strncmp(gtype, "download", 8) == 0))
	    {
		if((targ_user = get_human_user(nick)) != NULL)
		{
		targ_user->gagd=1;
		uprintf(user, "<%s> [ %s ] %s %s|", botnick, targ_user->nick, lp_wasgaggedfrom, lp_downloading_);
			if(show_actions > 0)
			{
				snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s [ %s ]|", targ_user->nick, targ_user->hostname, lp_wasgaggedby, user->nick, lp_from_, lp_downloading_);
				send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			}
		}
		else
		uprintf(user, "<%s> '%s' %s|", botnick, nick, lp_isoffline);
	    }
	    if((strncmp(gtype, "search", 6) == 0))
	    {
		if((targ_user = get_human_user(nick)) != NULL)
		{
		targ_user->gags=1;
		uprintf(user, "<%s> [ %s ] %s %s|", botnick, targ_user->nick, lp_wasgaggedfrom, lp_searching_);
			if(show_actions > 0)
			{
				snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s [ %s ]|", targ_user->nick, targ_user->hostname, lp_wasgaggedby, user->nick, lp_from_, lp_searching_);
				send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			}
		}
		else
		uprintf(user, "<%s> '%s' %s|", botnick, nick, lp_isoffline);
	    }
	}
	    else
	    {
	    sscanf(buf, "%50s", nick);
	    if(nick[strlen(nick)-1] == '|')
	    nick[strlen(nick) - 1] = '\0';

		if((targ_user = get_human_user(nick)) != NULL)
		{
		targ_user->gagc=1;
		targ_user->gagd=1;
		targ_user->gags=1;
		uprintf(user, "<%s> [ %s ] %s %s, %s, %s|", botnick, targ_user->nick, lp_wasgaggedfrom, lp_chatting_, lp_downloading_, lp_searching_);
			if(show_actions > 0)
			{
				snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s [ %s, %s, %s ]|", targ_user->nick, targ_user->hostname, lp_wasgaggedby, user->nick, lp_from_, lp_chatting_, lp_downloading_, lp_searching_);
				send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			}
		}
		else
		uprintf(user, "<%s> '%s' %s|", botnick, nick, lp_isoffline);
	    }
}
void ungag_user(char *buf, struct user_t *user)
{
	struct user_t *targ_user;
	char nick[51];
	
	if((sscanf(buf, "%50[^|]|", nick) < 1))
	return;
	else 
	{
	    if((targ_user = get_human_user(nick)) != NULL)
	    {
	    targ_user->gagc=0;
	    targ_user->gagd=0;
	    targ_user->gags=0;
	    uprintf(user, "<%s> [ %s ] %s|", botnick, targ_user->nick, lp_wasungagged);
	    }
	}
}

/* Sends a list of all gagged human users */
void send_gagged_list(struct user_t *user)
{
	int totalgags = 0;
	char buf[501];
	struct sock_t *human_user;
	struct sock_t *next_human_user;

	buf[0] = '\0';
	human_user = human_sock_list;

	uprintf(user, "<%s> %s:\r\n", botnick, lp_listofgagged);
	while(human_user != NULL) 
	{
		next_human_user = human_user->next;
		if((human_user->user->gagc != 0) | (human_user->user->gagd != 0) | (human_user->user->gags != 0))
		{
			totalgags++;
			snprintf(buf, 500, "[ %s: %s ]  [ IP: %s ]  [ ", lp_user_, human_user->user->nick, human_user->user->hostname);
      
			/* Type of user */ 
			switch (human_user->user->type)
			{
			case REGULAR:		sprintfa(buf, "%s", lp_class1); break;
			case PROTECTED:		sprintfa(buf, "%s", lp_class2); break;
			case REG: sprintfa(buf, "%s", lp_class3); break;
			case VIP: sprintfa(buf, "%s", lp_class4); break;
			case KVIP: sprintfa(buf, "%s", lp_class5); break;
			case OP:				sprintfa(buf, "%s", lp_class6); break;
			case CHEEF:				sprintfa(buf, "%s", lp_class7); break;
			case ADMIN:	sprintfa(buf, "%s", lp_class8); break;
			case OWNER:	sprintfa(buf, "%s", lp_class9); break;
			}

			sprintfa(buf, " ]  [ %s: ", lp_gaggedfrom);

			/* Type of gag */ 
			if (human_user->user->gagc != 0)
			{
				sprintfa(buf, "%s ", lp_chatting_);
			}
			if (human_user->user->gagd != 0)
			{
				sprintfa(buf, "%s ", lp_downloading_);
			}
			if (human_user->user->gags != 0)
			{
				sprintfa(buf, "%s ", lp_searching_);
			}

			sprintfa(buf, "]\r\n");
			/* Send data to user... */ 
			send_to_user(buf, user);
		}

		human_user = next_human_user;
	}

	/* Finally */ 
	uprintf(user, "--- %d %s ---|", totalgags, lp_users);
} 

void gagall(struct user_t *user)
{
	struct sock_t *human_user;
	struct sock_t *next_human_user;

	human_user = human_sock_list;

	uprintf(user, "<%s> %s|", botnick, lp_gaggingall);
	while(human_user != NULL) 
	{
	next_human_user = human_user->next;
	    if((human_user->user->type & (ADMIN | OWNER)) == 0) 
	    {
	    human_user->user->gagc = 1;
	    human_user->user->gagd = 1;
	    human_user->user->gags = 1;
	    }
	human_user = next_human_user;
	}
} 

void ungagall(struct user_t *user)
{
	struct sock_t *human_user;
	struct sock_t *next_human_user;

	human_user = human_sock_list;
	uprintf(user, "<%s> %s|", botnick, lp_ungaggingall);

	while(human_user != NULL) 
	{
	next_human_user = human_user->next;
	human_user->user->gagc = 0;
	human_user->user->gagd = 0;
	human_user->user->gags = 0;
	human_user = next_human_user;
	}
} 

void last_temp(char *buf)
{
        struct tm * timeinfo;		
	char acttime[51];
	time_t now;
	char sdayn[6];
	char sday[4];
	char sdate[11];
	char syear[6];
	char smont[6];
		now = time(NULL);
		timeinfo = localtime ( &now );
		snprintf(acttime, 50, "%s", asctime (timeinfo));
		trim_string(acttime);
		sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		
		snprintf(lasttemp20, 295, "%s", lasttemp19);
		snprintf(lasttemp19, 295, "%s", lasttemp18);
		snprintf(lasttemp18, 295, "%s", lasttemp17);
		snprintf(lasttemp17, 295, "%s", lasttemp16);
		snprintf(lasttemp16, 295, "%s", lasttemp15);
		snprintf(lasttemp15, 295, "%s", lasttemp14);
		snprintf(lasttemp14, 295, "%s", lasttemp13);
		snprintf(lasttemp13, 295, "%s", lasttemp12);
		snprintf(lasttemp12, 295, "%s", lasttemp11);
		snprintf(lasttemp11, 295, "%s", lasttemp10);
		snprintf(lasttemp10, 295, "%s", lasttemp9);
		snprintf(lasttemp9, 295, "%s", lasttemp8);
		snprintf(lasttemp8, 295, "%s", lasttemp7);
		snprintf(lasttemp7, 295, "%s", lasttemp6);
		snprintf(lasttemp6, 295, "%s", lasttemp5);
		snprintf(lasttemp5, 295, "%s", lasttemp4);
		snprintf(lasttemp4, 295, "%s", lasttemp3);
		snprintf(lasttemp3, 295, "%s", lasttemp2);
		snprintf(lasttemp2, 295, "%s", lasttemp1);
		snprintf(lasttemp1, 295, "[%s-%s-%s %s] %s", sday, smont, syear, sdate, buf);
}

void last_perm(char *buf)
{
        struct tm * timeinfo;		
	char acttime[51];
	time_t now;
	char sdayn[6];
	char sday[4];
	char sdate[11];
	char syear[6];
	char smont[6];
									
		now = time(NULL);
		timeinfo = localtime ( &now );
		snprintf(acttime, 50, "%s", asctime (timeinfo));
		trim_string(acttime);
		sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		
		snprintf(lastperm20, 295, "%s", lastperm19);
		snprintf(lastperm19, 295, "%s", lastperm18);
		snprintf(lastperm18, 295, "%s", lastperm17);
		snprintf(lastperm17, 295, "%s", lastperm16);
		snprintf(lastperm16, 295, "%s", lastperm15);
		snprintf(lastperm15, 295, "%s", lastperm14);
		snprintf(lastperm14, 295, "%s", lastperm13);
		snprintf(lastperm13, 295, "%s", lastperm12);
		snprintf(lastperm12, 295, "%s", lastperm11);
		snprintf(lastperm11, 295, "%s", lastperm10);
		snprintf(lastperm10, 295, "%s", lastperm9);
		snprintf(lastperm9, 295, "%s", lastperm8);
		snprintf(lastperm8, 295, "%s", lastperm7);
		snprintf(lastperm7, 295, "%s", lastperm6);
		snprintf(lastperm6, 295, "%s", lastperm5);
		snprintf(lastperm5, 295, "%s", lastperm4);
		snprintf(lastperm4, 295, "%s", lastperm3);
		snprintf(lastperm3, 295, "%s", lastperm2);
		snprintf(lastperm2, 295, "%s", lastperm1);
		snprintf(lastperm1, 295, "[%s-%s-%s %s] %s", sday, smont, syear, sdate, buf);
}

void ban_info(char *buf)
{
        char joinstring[301];
	struct sock_t *human_user;
	struct sock_t *next_human;

	if(show_kicksbans > 0)
	{
	    if(show_kicksbans < 3)
	    {
	    human_user = human_sock_list; 
	    	while(human_user != NULL) 
		{ 
		next_human = human_user->next; 
		    if((human_user->user->type & (ADMIN | OWNER)) != 0)
		    {
		    if(show_kicksbans == 1)
		    uprintf(human_user->user, "<%s> %s|", botnick, buf);
		    if(show_kicksbans == 2)
		    uprintf(human_user->user, "$To: %s From: %s $<%s> %s|", human_user->user, botnick, botnick, buf);
		    }
		    human_user = next_human; 
		}
	    }
	    else
	    {
	    snprintf(joinstring, 300, "<%s> %s|", botnick, buf);
	    send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	    }
	}
}
void denied_info(char *buf)
{
        char joinstring[701];
	struct sock_t *human_user;
	struct sock_t *next_human;

	if(show_denied > 0)
	{
	    if(show_denied < 3)
	    {
	    human_user = human_sock_list; 
		while(human_user != NULL) 
		{ 
		next_human = human_user->next; 
		    if((human_user->user->type & (ADMIN | OWNER)) != 0)
		    {
		    if(show_denied == 1)
		    uprintf(human_user->user, "<%s> %s|", botnick, buf);
		    if(show_denied == 2)
		    uprintf(human_user->user, "$To: %s From: %s $<%s> %s|", human_user->user, botnick, botnick, buf);
		    }
		human_user = next_human; 
		}
	    }
	    else
	    {
	    snprintf(joinstring, 700, "<%s> %s|", botnick, buf);
	    send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	    }
	}
}
void send_offmsg(struct user_t *user, int type)
{
	int i,j;
	FILE *fp;
	int fd;
	int erret;
	char line[4095];
	char offnick[51];
	char path[MAX_FDP_LEN+1];
	int matched = 0;
        snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MSGS_FILE);
	while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/open(): Interrupted system call. Trying again.\n");

	if(fd < 0)
	{
		logprintf(0, "Error - In send_user_list()/open(): %s", strerror(errno));
		return;
	}

	/* Set the lock */
	if(set_lock(fd, F_RDLCK) == 0)
	{
		logprintf(0, "Error - In send_user_list(): Couldn't set file lock\n");
		close(fd);
		return;
	}              

	if((fp = fdopen(fd, "r")) == NULL)
	{
		logprintf(0, "Error - In send_user_list()/fdopen(): %s", strerror(errno));
		set_lock(fd, F_UNLCK);
		close(fd);
		return;
	}

   while(fgets(line, 1023, fp) != NULL)
     {
	trim_string(line);
	j = strlen(line);
	if(j != 0)
	  {
	     /* Jump to next char which isn't a space */
	     i = 0;
	     while(line[i] == ' ')
	       i++;
	     
	     
		    sscanf(line+i, "%s", offnick);
		  if((strcasecmp(user->nick, offnick) == 0))
		    {
	matched++;
	if(matched == 1)
	uprintf(user, "$To: %s From: %s $<%s> %s:\r\n", user, botnick, botnick, lp_msgforyou);  
	uprintf(user, "%s\r\n", line+ i + strlen(offnick) + 1);  
	       }
	  }
     }

if(matched > 0)
uprintf(user, "|$To: %s From: %s $<%s> %s|", user, botnick, botnick, lp_erasemsgs);  

if((matched == 0) && (type == 1))
uprintf(user, "$To: %s From: %s $<%s> %s|",  user, botnick, botnick, lp_nonewmsg);		

	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/fclose(): Interrupted system call. Trying again.\n");

	if(erret != 0)
	{
		logprintf(0, "Error - In send_user_list()/fclose(): %s", strerror(errno));
	}	
}

void remove_offmsg(struct user_t *user)
{
	int i,j;
	FILE *fp;
	int fd;
	int erret;
	char line[4095];
	char offnick[51];
	char path[MAX_FDP_LEN+1];
	int matched = 0;
       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MSGS_FILE);
	while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/open(): Interrupted system call. Trying again.\n");

	if(fd < 0)
	{
		logprintf(0, "Error - In send_user_list()/open(): %s", strerror(errno));
		return;
	}

	/* Set the lock */
	if(set_lock(fd, F_RDLCK) == 0)
	{
		logprintf(0, "Error - In send_user_list(): Couldn't set file lock\n");
		close(fd);
		return;
	}              

	if((fp = fdopen(fd, "r")) == NULL)
	{
		logprintf(0, "Error - In send_user_list()/fdopen(): %s", strerror(errno));
		set_lock(fd, F_UNLCK);
		close(fd);
		return;
	}

   while(fgets(line, 1023, fp) != NULL)
     {
	trim_string(line);
	j = strlen(line);
	if(j != 0)
	  {
	     /* Jump to next char which isn't a space */
	     i = 0;
	     while(line[i] == ' ')
	       i++;
	     
	     
		    sscanf(line+i, "%s", offnick);
		  if((strcasecmp(user->nick, offnick) == 0))
		    {
	matched++;
		    remove_line_from_file(offnick, path, 0);
	       }
	  }
     }

if(matched > 0)
{
uprintf(user, "$To: %s From: %s $<%s> %s|", user, botnick, botnick, lp_deletedmsg);  
loadfile2buf(&off_mem,MSGS_FILE);
}

	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/fclose(): Interrupted system call. Trying again.\n");

	if(erret != 0)
	{
		logprintf(0, "Error - In send_user_list()/fclose(): %s", strerror(errno));
	}	
if(matched == 0)
uprintf(user, "$To: %s From: %s $<%s> %s|",  user, botnick, botnick, lp_nomsgtodelete);		
}

void send_extrafiles(struct user_t *user, char *command)
{
	FILE *fp;
	int fd;
	int erret;
	char line[4095];
	char path[MAX_FDP_LEN+1];

       snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, EXTRAFILES_DIR, command);
	while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/open(): Interrupted system call. Trying again.\n");

	if(fd < 0)
	{
		logprintf(0, "Error - In send_user_list()/open(): %s", strerror(errno));
		return;
	}

	/* Set the lock */
	if(set_lock(fd, F_RDLCK) == 0)
	{
		logprintf(0, "Error - In send_user_list(): Couldn't set file lock\n");
		close(fd);
		return;
	}              

	if((fp = fdopen(fd, "r")) == NULL)
	{
		logprintf(0, "Error - In send_user_list()/fdopen(): %s", strerror(errno));
		set_lock(fd, F_UNLCK);
		close(fd);
		return;
	}

	if(fgets(line, 4094, fp) != NULL)
	{	
		trim_string(line);
			uprintf(user, "%s", line);

		while(fgets(line, 4094, fp) != NULL)
		{	     
			trim_string(line);
				uprintf(user, "\r\n%s", line);
		}	
	}

	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
		logprintf(0, "Error - In send_user_list()/fclose(): Interrupted system call. Trying again.\n");

	if(erret != 0)
	{
		logprintf(0, "Error - In send_user_list()/fclose(): %s", strerror(errno));
	}	
}

//DB// Cheking if we have a file that has the same name as command
int check_extrafiles(char *command)
{
	FILE *fp;
	int fd;
	int erret;
	char path[MAX_FDP_LEN+1];

       snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, EXTRAFILES_DIR, command);

	if((fd = open(path, O_RDONLY)) < 0)
	{
		return 0;
	}

	/* Set the lock */
	if(set_lock(fd, F_RDLCK) == 0)
	{
		logprintf(0, "Error - In check_extrafiles(): Couldn't set file lock\n");
		close(fd);
		return 0;
	}              

	if((fp = fdopen(fd, "r")) == NULL)
	{
		logprintf(0, "Error - In check_extrafiles()/fdopen(): %s", strerror(errno));
		set_lock(fd, F_UNLCK);
		close(fd);
		return 0;
	}

	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
		logprintf(0, "Error - In check_extrafiles()/fclose(): Interrupted system call. Trying again.\n");

	if(erret != 0)
	{
		logprintf(0, "Error - In check_extrafiles()/fclose(): %s", strerror(errno));
	}	
return 1;
}

int is_command(char *temp, struct user_t *user, int type)
{
	char onestring = '\0';
	char *hubstemp;
	char *op_list;
	char *stemp;
	char bigstring[1001];
	char client[21];		
	char longstring[4097];
	char largestring[501];
	char ministring[2];
	char path[MAX_FDP_LEN+1];
	char sdate[11];
	char sday[4];
	char sdayn[6];
	char smont[6];
	char syear[6];
	char tempnick[51];
	char temppass[51];
	char tempstring[151];
	char ujt1[51];
	char ujt2[51];	
	char ujt3[51];
	char ujt4[51];
	char ujt5[51];	
	char ujt6[51];
	char ujt7[51];	
	char ujt8[51];
	char ujt9[51];
	char ujt10[51];	
	int matches = 0;
	int hubs, ret, slots;
	int reghubs = 0; int ophubs = 0;
	int usadmins = 0; int usops = 0; int usowners = 0; int ushidden = 0;
	int uskvips = 0; int usvips = 0; int uscheefs = 0; int uscomms = 0;
	int usregs = 0; int usprots = 0;
	long long numberrec;
        struct sock_t *human_user;
	struct sock_t *next_human;
	struct tm * timeinfo;
	struct user_t *tempuser;
	time_t jt1; time_t jt2; time_t jt3; time_t jt4; time_t jt5; 
	time_t jt6; time_t jt7; time_t jt8; time_t jt9; time_t jt10;
        time_t exp;	
	time_t nicklistban;
	time_t now;
	
	now = time(NULL);
	
	if((check_if_registered(user->nick) < min_command_class) && (check_if_registered(user->nick) < 7))
	return 0;
	
		if(((user->type & (OWNER)) != 0) && (strncasecmp(temp+1, "addadmin ", 9) == 0))
		{
 	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+10, "%50s %50s", tempnick, temppass) != 2)
			{
			uprintf(user, "<%s> %s: !addadmin <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_pass_);
			return 1;
			}
		temppass[strlen(temppass)-1] = '\0';
		snprintf(tempstring, 150, "!addreguser %s %s 6|", tempnick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class8, lp_withpass, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
			snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class8, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
			send_to_ops(bigstring, OWNER, show_notifications);
			}
		    }	     
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addcateg ", 9) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((sscanf(temp+10, "%1s %50[^|]|", ministring, tempnick) != 2) || (strncasecmp(temp+11, " ", 1) != 0))
			{
			uprintf(user, "<%s> %s: !addcateg <%s> <%s> (eg: !addcateg A AUDIO)|", botnick, lp_badsyntax, lp_categletter_, lp_category_);
			return 1; 
			}		
		    if(check_if_categ(ministring) == -1)	
				uprintf(user, "<%s> %s categories -> %s", botnick, lp_couldntaddentry, temp);
		    else if(check_if_categ(ministring) == 1)		  
				uprintf(user, "<%s> %s categories -> %s", botnick, lp_entryalreadyonlist, temp);
		    else
		    {
			    snprintf(tempstring, 150, "%s %s%s", ministring, ministring, tempnick+1);
			    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CATEG_FILE);
			    add_line_to_file(tempstring, path);
			    uprintf(user, "<%s> %s categories -> '%s'|", botnick, lp_addedentryto, ministring);
		    }	     	    	     
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addcheef ", 9) == 0))
		{
 	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+10, "%50s %50s", tempnick, temppass) != 2)
			{
			uprintf(user, "<%s> %s: !addcheef <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_pass_);
			return 1;
			}
		temppass[strlen(temppass)-1] = '\0';
		snprintf(tempstring, 150, "!addreguser %s %s 5|", tempnick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class7, lp_withpass, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
				    snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class7, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
				    send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
			}	
		    }	     
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addfile ", 8) == 0))
		{
		temp += 9;
		    if(sscanf(temp, "%50s %50s", tempnick, temppass) != 2)
		    {
			uprintf(user, "<%s> %s: !addfile <%s> <%s>|", botnick, lp_badsyntax, lp_file_, lp_text_);
			return 1;
		    }
		    if(temppass[0] == '|')
		    {
			uprintf(user, "<%s> %s: !addfile <%s> <%s>|", botnick, lp_badsyntax, lp_file_, lp_text_);
			return 1;
		    }
		    temp = temp + strlen(tempnick) + 1;
			if(write_file(temp, tempnick, 1) == -1)
			return 1;
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s: [ %s ]|", botnick, lp_newcommand, tempnick);
		return 1;
		}
		else if(((user->type & (CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addkvip ", 8) == 0))
		{
 	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+9, "%50s %50s", tempnick, temppass) != 2)
			{
			uprintf(user, "<%s> %s: !addkvip <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_pass_);
			return 1;
			}
		temppass[strlen(temppass)-1] = '\0';
		snprintf(tempstring, 150, "!addreguser %s %s 3|", tempnick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class5, lp_withpass, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
			    snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class5, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
			    send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
			}
		    }	     
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addop ", 6) == 0))
		{
 	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+7, "%50s %50s", tempnick, temppass) != 2)
			{
			uprintf(user, "<%s> %s: !addop <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_pass_);
			return 1;
			}
		temppass[strlen(temppass)-1] = '\0';
		snprintf(tempstring, 150, "!addreguser %s %s 4|", tempnick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class6, lp_withpass, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
				    snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class6, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
				    send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
			}
		    }	     
		return 1;
		}
                else if(((user->type & (OWNER)) != 0) && (strncasecmp(temp+1, "addowner ", 9) == 0))
		{
 		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
    			if(sscanf(temp+10, "%50s %50s", tempnick, temppass) != 2)
			{
			uprintf(user, "<%s> %s: !addowner <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_pass_);
			return 1;
			}
		temppass[strlen(temppass)-1] = '\0';
		    snprintf(tempstring, 150, "!addreguser %s %s 7|", tempnick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class9, lp_withpass, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
				    snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class9, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
				    send_to_ops(bigstring, OWNER, show_notifications);
			}
		    }	     
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addprot ", 8) == 0))
		{
 		protdays = 0;
	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+9, "%50s %50s %i|", tempnick, temppass, &protdays) < 3)
			{
			temppass[strlen(temppass) - 1] = '\0';
			    if(strlen(temppass) == 0)
			    {
			    uprintf(user, "<%s> %s: !addprot <%s> <%s> (<%s>)|", botnick, lp_badsyntax, lp_nick_, lp_pass_, lp_days);
			    return 1;
			    }
			}
    		    if(protdays == 0) 
		    {
		    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, PROT_FILE);
		    add_line_to_file(tempnick, path);
		    }
			snprintf(tempstring, 150, "!addreguser %s %s 0|", tempnick, temppass);
			if((ret = add_reg_user(tempstring)) == -1)
				uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
			else if(ret == 3)
				uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
			else
			{		  
			if(protdays == 0)	
			uprintf(user, "<%s> %s  %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class2, lp_withpass, temppass, hub_hostname, listening_port);
			else
			uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d (%s: %d %s)|", botnick, tempnick, lp_wasregistered, lp_class2, lp_withpass, temppass, hub_hostname, listening_port, lp_prottime, protdays, lp_days);
		if(show_notifications > 0)
		{	
		if(protdays == 0)
		{
		snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class2, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
		send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
		}
		else
		{
		snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ], %s: [ %d %s ]|", lp_newregistered, lp_class2, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick, lp_prottime, protdays, lp_days);
		send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
		}
		}
		}
		protdays = 0;
		return 1;
		}
		else if((strncasecmp(temp+1, "addreg ", 7) == 0) && ((user->type & (CHEEF | ADMIN | OWNER)) != 0))
		{
 	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+8, "%50s %50s", tempnick, temppass) != 2)
			{
			uprintf(user, "<%s> %s: !addreg <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_pass_);
    			return 1;
			}
		temppass[strlen(temppass)-1] = '\0';
		snprintf(tempstring, 150, "!addreguser %s %s 1|", tempnick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class3, lp_withpass, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
				    snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class3, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
				    send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
			}
		    }	     
		return 1;
		}
		else if(strncasecmp(temp+1, "addreq ", 7) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(check_if_registered(user->nick) < who_can_addreq)
			{
			uprintf(user, "<%s> %s request|", botnick, lp_notallowedtoadd);
			return 1;
			}
		if(sscanf(temp+8, "%500[^|]|", largestring) != 1)
		{
		uprintf(user, "<%s> %s: !addreq 'file'|", botnick, lp_badsyntax);
		return 1;
		}
		    if(check_if_exist_req(largestring) == 1)
		    {
		    uprintf(user, "<%s> %s reqlist -> '%s'|", botnick, lp_entryalreadyonlist, largestring);
		    return 1;
		    }
		exp = now + (expiredreq * 86400);
		timeinfo = localtime ( &now );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(bigstring, 1000, "<%s> [ %s ] %s [ %s ] %s|", botnick, user->nick, lp_added_, largestring, lp_toreqlist);
		uprintf(user, "<%s> [ %s ] %s reqlist|", botnick, largestring, lp_wasaddedto);
		send_to_humans(bigstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);
		snprintf(bigstring, 1000, "%lu <-> [ %s ], %s : [ %s @ %s %s %s, %s ]", exp, largestring, lp_user_, user->nick, sday, smont, syear, sdate);
		add_line_to_file(bigstring, path);
		req_motd();
		return 1;
		}	
		else if(strncasecmp(temp+1, "addrls ", 7) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(check_if_registered(user->nick) < who_can_addrls)
			{
			uprintf(user, "<%s> %s release|", botnick, lp_notallowedtoadd);
	    		return 1;
			}
		    if((sscanf(temp+8, "%1s %500[^|]|", ministring, largestring) != 2) || (strncasecmp(temp+9, " ", 1) != 0))
		    {
		    uprintf(user, "<%s> %s: !addrls <%s> <%s>\r\n'x' is one of the release types:\r\n", botnick, lp_badsyntax, lp_categletter_, lp_file_);
		    send_user_list(CATEG, user);
		    send_to_user("|", user);
		    return 1;
		    }
			if(check_if_exist_rls(largestring) == 1)
			{
	    		uprintf(user, "<%s> %s releaselist -> '%s'|", botnick, lp_entryalreadyonlist, largestring);
			return 1;
			}
		exp = now + (expiredrls * 86400);

		if(check_if_categ(ministring) == 1)
		{
		timeinfo = localtime ( &now );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(bigstring, 1000, "%lu <-> [ %s : %s ], %s : [ %s @ %s %s %s, %s ]", exp, category, largestring, lp_user_, user->nick, sday, smont, syear, sdate);
		uprintf(user, "<%s> [ %s : %s ] %s releaselist|", botnick, category, largestring, lp_wasaddedto);
		snprintf(longstring, 4096, "<%s> [ %s ] %s [ %s : %s ] %s (!getrls %s %s [ %s ])|", botnick, user->nick, lp_added_, category, largestring, lp_toreleaselist, ministring, lp_forother, category);
		}
		else
		{
		uprintf(user, "<%s> [ %s ] %s:\r\n", botnick, ministring, lp_categnotfound);
		send_user_list(CATEG, user);
		send_to_user("|", user);
		return 1;
		}
		human_user = human_sock_list;
		while(human_user != NULL)
		{
		next_human = human_user->next; 
		if(check_if_registered(human_user->user->nick) >= who_can_seerls)
		uprintf(human_user->user, "%s", longstring);
		human_user = next_human; 
		}
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);
		add_line_to_file(bigstring, path);
		rls_motd();
		return 1;
		}	
		else if(((user->type & (CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addvip ", 7) == 0))		
		{
 	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+8, "%50s %50s", tempnick, temppass) != 2)
			{
			uprintf(user, "<%s> %s: !addvip <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_pass_);
			return 1;
			}
		temppass[strlen(temppass)-1] = '\0';
    		snprintf(tempstring, 150, "!addreguser %s %s 2|", tempnick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempnick);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    uprintf(user, "<%s> %s %s %s %s: %s @ %s:%d|", botnick, tempnick, lp_wasregistered, lp_class4, lp_withpass, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
			    snprintf(bigstring, 1000, "%s [ %s ] -> %s: [ %s ], %s: [ %s ], %s: [ %s ]|", lp_newregistered, lp_class4, lp_nick_, tempnick, lp_pass_, temppass, lp_registeredby, user->nick);
			    send_to_ops(bigstring, ADMIN | OWNER, show_notifications);				    
			}
		    }	     
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "addwelcome ", 11) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		sscanf(temp+12, "%500[^|]|", largestring);
		snprintf(bigstring, 1000, "%lu %s", now, largestring);
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WELCOME_FILE);
                uprintf(user, "<%s> [ %s ] %s randomwelcomes|", botnick, largestring, lp_wasaddedto);
		add_line_to_file(bigstring, path);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "allow ", 6) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(temp+7, ALLOW, user)) == -1)
				uprintf(user, "<%s> %s allowlist -> %s", botnick, lp_couldntaddentry, temp);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s allowlist -> %s", botnick, lp_entryalreadyonlist, temp);
			else
				uprintf(user, "<%s> %s allowlist -> %s", botnick, lp_addedentryto, temp);
		return 1;
		}	
		else if (strncasecmp(temp+1, "autoaway ", 9) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+10, "%d, %150[^|]|", &ret, tempstring) != 2)
			{
			uprintf(user, "<%s> %s: !autoaway <%s>, <%s>|", botnick, lp_badsyntax, lp_time_, lp_reason);
			return 1;
			}
		    if(ret < 15)
		    {
		    uprintf(user, "<%s> %s|", botnick, lp_autoawaynotset);
		    return 1;
		    }
		user->autoawaytime = ret;
		snprintf(user->autoawayreason, 150, "%s", tempstring);
		uprintf(user, "<%s> %s: [ %d ] %s, %s: [ %s ]|", botnick, lp_autoawayset, user->autoawaytime, lp_minutes, lp_reason, user->autoawayreason);		
		snprintf(longstring, 500, "%s %d %s", user->nick, ret, tempstring);
		add_autoaway_user(longstring);
		return 1;
		}
		else if(strncasecmp(temp+1, "away ",5) == 0)
		{
	 	    if(sscanf(temp+6, "%150[^|]|", tempstring) != 1)
		    {
			if(type == 1)
		        uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !away <%s>|", botnick, lp_badsyntax, lp_reason);
		    return 1;
		    }
		user->away_flag=2;
		snprintf(largestring, 500, "%s %100s", user->nick, temp+6);
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s|", botnick, lp_awaymodeon);
		add_away_user(largestring);
		snprintf(largestring, 500, "<----> %s %s (%s: %s)|", user->nick, lp_isaway, lp_reason, tempstring);
		    if(user->hide_share != 2)
		    {
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return 1;
		}
			if(no_chat_for_chatgagged == 0)
			send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			else
			{
			human_user = human_sock_list;
			    while(human_user != NULL) 
			    {
			    next_human = human_user->next;
				if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
				uprintf(human_user->user, "%s", largestring);
			    human_user = next_human;
			    }
			}
		    }
		return 1;		
		}
		else if((strncasecmp(temp+1, "back|", 5) == 0) || (strncasecmp(temp+1, "away|", 5) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
                uprintf(user, "<%s> %s|", botnick, lp_awaymodeoff);			    	     
			if(check_if_away(user->nick) == 1)
			{
			time2str(difftime(now, time_to_unban), tempstring);
			snprintf(largestring, 500, "<----> %s %s %s %s|", user->nick, lp_isback, lp_after, tempstring);
			}
			else
			snprintf(largestring, 500, "<----> %s %s|", user->nick, lp_isback);
		    remove_away_user(user);
		    user->away_flag=1;
			if(user->hide_share != 2)
			{
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return 1;
		}
			    if(no_chat_for_chatgagged == 0)
			    send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			    else
			    {
			    human_user = human_sock_list;
				while(human_user != NULL) 
				{
				next_human = human_user->next;
				    if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
				    uprintf(human_user->user, "%s", largestring);
				human_user = next_human;
				}
			    }
			}
		return 1;				
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "ban ", 4) == 0))
		{	    	     
		    if((sscanf(temp+5, "%s %1000[^|]|", tempnick, largestring) != 2))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !ban <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_reason);
		    return 1;
		    }
			if(((tempuser = get_human_user(tempnick)) != NULL))
			{
			    if((tempuser->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0)	
			    {
			    snprintf(tempstring, 150, "%s|", tempuser->hostname);
				if(type == 1)
				uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, BAN, user)) == -1)
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_couldntaddentry, tempuser->hostname);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_entryalreadyonlist, tempuser->hostname);
			else
			{
			uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_addedentryto, tempuser->hostname);
			    if(strchr(largestring, '\n') == NULL)
			    {
			    snprintf(bigstring, 1000, "0 %s [ %s ]", tempuser->hostname, largestring);
			    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
			    if(check_if_banned(tempuser, BANRS) == 0)
			    add_line_to_file(bigstring, path);
			    }
			}
			snprintf(tempstring, 150, "%s|", tempuser->nick);
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, NICKBAN, user)) == -1)
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_couldntaddentry, tempnick);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
			else
			{
			uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_addedentryto, tempnick);
			    if(strchr(largestring, '\n') == NULL)
			    {
			    snprintf(bigstring, 1000, "0 %s [ %s ]", tempuser->nick, largestring);			
			    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
			    if(check_if_banned(tempuser, NICKBANRS) == 0)			
    			    add_line_to_file(bigstring, path);
			    }
			}
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s: %s|", tempuser->nick, user->nick, user->nick, lp_youwerebanned, lp_because, largestring);
			snprintf(bigstring, 1000, "<%s> %s %s (IP: %s) %s: %s|", user->nick, lp_isbanning, tempuser->nick, tempuser->hostname, lp_because , largestring);
                    	send_to_humans(bigstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
    			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ]", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick);             
	    		last_perm(lastbuf);			
			    if(punishment != 0)
			    uprintf(tempuser, "$ForceMove %s|", redirect_host_kickban);                                             
	    		tempuser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;		
			    if(show_actions > 0)
			    {
					snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s: [ %s ]|", tempuser->nick, tempuser->hostname, lp_wasbannedby, user->nick, lp_because, largestring);
					send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			    }
			}
			else
			{
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: %s", botnick, lp_nopermission, temp);			
			}
			}
			else
			{
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> '%s' %s|", botnick, tempnick, lp_isoffline);			
			}
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "banip ", 6) == 0))
		{	    	     
		    if((sscanf(temp+7, "%s %1000[^|]|", tempnick, largestring) != 2))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !banip <IP> <%s>|", botnick, lp_badsyntax, lp_reason);
		    return 1;
		    }
		    snprintf(tempstring, 150, "%s|", tempnick);
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, BAN, user)) == -1)
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_couldntaddentry, tempnick);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
			else
			{
			uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_addedentryto, tempnick);
			    if(strchr(largestring, '\n') == NULL)
			    {
			    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
			    snprintf(bigstring, 1000, "0 %s [ %s ]", tempnick, largestring);
			    add_line_to_file(bigstring, path);
		    	    }
    			snprintf(lastbuf, 800, "[ IP: %s ]: %s [ %s %s ]", tempnick, largestring, lp_bannedby, user->nick);             
	    		last_perm(lastbuf);			
			    if(show_actions > 0)
			    {
					snprintf(bigstring, 1000, "[ IP: %s ] %s [ %s ] %s: [ %s ]|", tempnick, lp_wasbannedby, user->nick, lp_because, largestring);
					send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);							    
			    }
			}
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "banlist", 7) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> \r\n\r\n===== banlist [!unban]: ===== \r\n", botnick);
		send_user_list(BAN, user);
		uprintf(user, "\r\n===== nickbanlist [!unnickban]: ===== \r\n");
		send_user_list(NICKBAN, user);
		send_to_user("|", user);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "bannick ", 8) == 0))
		{	    	     
		    if((sscanf(temp+9, "%s %500[^|]|", tempnick, largestring) != 2))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s: !bannick <%s> <%s>|", botnick, lp_badsyntax, lp_nickname, lp_reason);
		return 1;
		    }
			snprintf(tempstring, 150, "%s|", tempnick);
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, NICKBAN, user)) == -1)
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_couldntaddentry, tempnick);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
			else
			{				
			uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_addedentryto, tempnick);
			    if(strchr(largestring, '\n') == NULL)
			    {
			    snprintf(bigstring, 1000, "0 %s [ %s ]", tempnick, largestring);
			    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
			    add_line_to_file(bigstring, path);
			    }
    			snprintf(lastbuf, 800, "[ %s: %s ]: %s [ %s %s ]", lp_nick_, tempnick, largestring, lp_bannedby, user->nick);             
	    		last_perm(lastbuf);			
			    if(show_actions > 0)
			    {
					snprintf(bigstring, 1000, "[ %s : %s ] %s [ %s ] %s: [ %s ]|", lp_nickname, tempnick, lp_wasbannedby, user->nick, lp_because, largestring);
					send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			    }
			}
		return 1;	     	    	     
		}	
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "block ", 6) == 0))
                {        
		block_proc(temp+7);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "bot ", 4) == 0))
		{
		snprintf(longstring, 4096, "<%s> %s", botnick, temp+5);
		send_to_humans(longstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		sscanf(temp+5, "%4096[^|]|", longstring);
		timeinfo = localtime ( &now );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(message20, 400, "%s", message19);
		snprintf(message19, 400, "%s", message18);
		snprintf(message18, 400, "%s", message17);
		snprintf(message17, 400, "%s", message16);
		snprintf(message16, 400, "%s", message15);
		snprintf(message15, 400, "%s", message14);
		snprintf(message14, 400, "%s", message13);
		snprintf(message13, 400, "%s", message12);
		snprintf(message12, 400, "%s", message11);
		snprintf(message11, 400, "%s", message10);
		snprintf(message10, 400, "%s", message9);
		snprintf(message9, 400, "%s", message8);
		snprintf(message8, 400, "%s", message7);
		snprintf(message7, 400, "%s", message6);
		snprintf(message6, 400, "%s", message5);
		snprintf(message5, 400, "%s", message4);
		snprintf(message4, 400, "%s", message3);
		snprintf(message3, 400, "%s", message2);
		snprintf(message2, 400, "%s", message1);
		    if(strlen(longstring) <= 256)
		    snprintf(message1, 400, "--- [%s] <%s> %s", sdate, botnick, longstring);
		    else
		    {
		    longstring[255] = '\0';
		    snprintf(message1, 400, "--- [%s] <%s> %s [%s...]", sdate, botnick, longstring, lp_cut_);
		    }
		return 1;
		}
		else if (strncasecmp(temp+1, "chat", 4) == 0)
		{
		    if(type == 1)
	    	    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> 20 %s:\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n|", botnick, lp_lastmainmsgs, message20, message19, message18, message17, message16, message15, message14, message13, message12, message11, message10, message9, message8, message7, message6, message5, message4, message3, message2, message1);
		return 1;
		}
		else if ((strncasecmp(temp+1, "changeregclass ", 15) == 0) && ((user->type & (CHEEF | ADMIN | OWNER)) != 0))
		{
		    if(sscanf(temp+16, "%50s %d", tempnick, &ret) != 2)
		    {
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !changeregclass <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_type_);
		    return 1;
		    }
			if((check_if_registered(tempnick) > 0) && (ret <= 7) && 
			((user->type == OWNER) || ((user->type == ADMIN) && (check_if_registered(tempnick) <= 6) && (ret <= 5))
			|| ((user->type == CHEEF) && (check_if_registered(tempnick) <= 4) && (ret <= 3))))
			{
			change_class(tempnick, ret);
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s -> %s", botnick, lp_accountupdated, temp);
			loadfile2buf(&reglist_mem,REG_FILE);
			}
			else
			{
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s -> %s", botnick, lp_nopermission, temp);
			}
		return 1;
		}
		else if((strncasecmp(temp+1, "changeregnick ", 14) == 0) && ((user->type & (CHEEF | ADMIN | OWNER)) != 0))
		{
		    if(sscanf(temp+15, "%50s %50s", tempnick, temppass) != 2)
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !changeregnick <%s> <%s>|", botnick, lp_badsyntax, lp_nick_, lp_nick_);
		    return 1;
		    }
		temppass[strlen(temppass)-1] = '\0';
		    if((check_if_registered(tempnick) > 0) && (check_if_registered(temppass) == 0) && 
		    ((user->type == OWNER) || ((user->type == ADMIN) && (check_if_registered(tempnick) <= 6))
		    || ((user->type == CHEEF) && (check_if_registered(tempnick) <= 4))))
		    {
		    change_nick(tempnick, temppass);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s -> %s", botnick, lp_accountupdated, temp);
		    loadfile2buf(&reglist_mem,REG_FILE);
		    }
		    else
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s -> %s", botnick, lp_nopermission, temp);
		    }
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "clearipbans|", 12) == 0))
		{
		    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BAN_FILE); 
		    unlink(path);
		    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE); 
		    unlink(path);
		    create_files();
		    loadfile2buf(&banlist_mem,BAN_FILE);
		    hammernick[0] = '\0';
		    hammertime=(time_t)0;                                                                          
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s 'banlist' %s|", botnick, lp_file_, lp_wascleared);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s 'banreasons' %s|", botnick, lp_file_, lp_wascleared);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "clearnickbans|", 14) == 0))
		{
		    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBAN_FILE); 
		    unlink(path);
		    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE); 
		    unlink(path);
		    create_files();
		    loadfile2buf(&nickbanlist_mem,NICKBAN_FILE);
		    hammernick[0] = '\0';
		    hammertime=(time_t)0;                                                                          
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s 'nickbanlist' %s|", botnick, lp_file_, lp_wascleared);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s 'nickbanreasons' %s|", botnick, lp_file_, lp_wascleared);
		return 1;
		}
		else if(strncasecmp(temp+1, "date", 4) == 0)
		{
	    	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		timeinfo = localtime ( &now );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		uprintf(user, "<%s> %s: %s|", botnick, lp_currentdate, tempnick);
		return 1;
		}						
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "delfile ", 8) == 0))
		{
		    if(strstr(temp+9, " ") == NULL)
		    {
			if(sscanf(temp+9, "%50[^|]|", tempnick) == 1)
			{
			    if(check_extrafiles(tempnick) == 1)
			    {
			    snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, EXTRAFILES_DIR, tempnick);		
			    unlink(path);
				if(type == 1)
				uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			    uprintf(user, "<%s> %s: [ %s ]|", botnick, lp_delcommand, tempnick);
			    }
			}
		    }
		return 1;
		}
		else if((strncasecmp(temp+1, "delmsg", 6) == 0) && (offline_msgs > 0))
		{
		remove_offmsg(user);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "deluser ", 8) == 0))
		{		
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		sscanf(temp+9, "%50[^|]|", tempnick);
		    if(user->type == ADMIN)
		    {
			if(check_if_registered(tempnick) > 6)
			{
			uprintf(user, "<%s> %s -> %s|", botnick, lp_notallowedtodelreg, tempnick);
			return 1;
			}
		    }	
			if((ret = remove_reg_user(temp+9)) == 0)
			uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entrywasntfound, tempnick);
			else if(ret == 2)
			uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntremoveentry, tempnick);
			else
			{		  
			uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_removedentryfrom, tempnick);
			    if(show_notifications > 0)
			    {
					snprintf(bigstring, 1000, "%s -> %s: [ %s ], %s: [ %s ]|", lp_deleteduser, lp_nick_, tempnick, lp_deletedby, user->nick);
					send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
			    }
			}	     
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "drop ", 5) == 0))
		{	    	     
		    if((sscanf(temp+6, "%50[^|]|", tempnick) < 1))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !drop <%s>|", botnick, lp_badsyntax, lp_user_);
		    return 1;
		    }
			if(strstr(tempnick, " ") != NULL)
			{
    			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: !drop <%s>|", botnick, lp_badsyntax, lp_user_);
			return 1;
			}
		    if(((tempuser = get_human_user(tempnick)) != NULL))
		    {
			if(tempuser->type < user->type)
			{
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s %s %s|", botnick, tempnick, lp_wasdropped, hub_name);			
			tempuser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;			
			}		    
			else
			{
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: %s", botnick, lp_nopermission, temp);			
			}
		    }
		    else
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> '%s' %s|", botnick, tempnick, lp_isoffline);			
		    }
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "fav", 3) == 0))
		{
		human_user = human_sock_list;
		    while(human_user != NULL)
		    {
		    next_human = human_user->next;
			if(human_user->user->type == REGULAR) 
			{
			uprintf(human_user->user, "<%s> \r\n", botnick);
			send_to_user(autofav_mem, human_user->user);
			send_to_user("|", human_user->user);
			uprintf(human_user->user, "$To: %s From: %s $<%s> \r\n", human_user->user->nick, botnick, botnick);
			send_to_user(autofav_mem, human_user->user);
			send_to_user("|", human_user->user);
			}
		    human_user = next_human;
		    }
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s|", botnick, lp_favsend);
		return 1;
		}	
		else if(strncasecmp(temp+1, "findrls ", 8) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(check_if_registered(user->nick) < who_can_seerls)
			{
			uprintf(user, "<%s> %s releaselist|", botnick, lp_notallowedtosee);
			return 1;
			}
		    sscanf(temp+9, "%150[^|]|", tempstring);
		    find_rls(user, tempstring);
		return 1;
		}
		else if(strncasecmp(temp+1, "fs", 2) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s: \r\n", botnick, lp_freeslots);
		human_user = human_sock_list;
		    while(human_user != NULL)
		    {
		    next_human = human_user->next;
			if(human_user->user->freeslots > 0)
			{
			matches++;
			time2str(difftime(now, human_user->user->srtime), largestring);
			uprintf(user, "%s: %d / %d (%s %s) [%s]\r\n", human_user->user->nick, human_user->user->freeslots, human_user->user->allslots, largestring, lp_ago_, human_user->user->con_type);
			}
		    human_user = next_human;
		    }
		uprintf(user, "[ %d %s ]|", matches, lp_users);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "gag ", 4) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		gag_user(temp+5, user);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "gagall", 6) == 0))
		{
    		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		gagall(user);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "gagip ", 6) == 0))
		{	    	     
			if((sscanf(temp+7, "%s %500[^|]|", tempnick, largestring) != 2))
			{
			if(tempnick[strlen(tempnick)-1] == '|')
			tempnick[strlen(tempnick) - 1] = '\0';
			largestring[0] = '\0';
			}
		snprintf(tempstring, 150, "%s|", tempnick);
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, IPGAG, user)) == -1)
			{
				uprintf(user, "<%s> %s ipgaglist -> %s", botnick, lp_couldntaddentry, temp);
				logprintf(4, "Error - Failed adding entry to ipgag list\n");
			}
			else if(ret == 0)		  
				uprintf(user, "<%s> %s ipgaglist -> %s", botnick, lp_entryalreadyonlist, temp);
			else
			{
			    uprintf(user, "<%s> %s ipgaglist -> %s", botnick, lp_addedentryto, temp);
		if(strlen(largestring) > 0)
		{
		    if(strchr(largestring, '\n') == NULL)
		    {
		    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAGRS_FILE);
		    snprintf(bigstring, 1000, "%s [ %s ]", tempnick, largestring);
		    add_line_to_file(bigstring, path);
		    }
		}
		    if(show_actions > 0)
		    {
				snprintf(bigstring, 1000, "[ IP: %s ] %s [ %s ]|", tempnick, lp_wasgaggedby, user->nick);
		    		send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
		    }
		}	     	    	     
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && ((strncasecmp(temp+1, "getallowlist", 12) == 0) || (strncasecmp(temp+1, "allowlist", 9) == 0)))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> allowlist:\r\n", botnick);
		send_user_list(ALLOW, user);
		send_to_user("|", user);
		return 1;
		}
		else if (strncasecmp(temp+1, "getcateg", 8) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> categories:\r\n", botnick);
		send_user_list(CATEG, user);
		send_to_user("|", user);
		return 1;
		}

		else if(((user->type & (ADMIN | OWNER)) != 0) && ((strncasecmp(temp+1, "getconfig", 9) == 0) || (strncasecmp(temp+1, "config|", 7) == 0)))
		{
		    if((config_on_priv == 1) || (type == 1))
		    uprintf(user, "$To: %s From: %s $<%s> config: \r\n", user->nick, botnick, botnick);
		    else
		    uprintf(user, "<%s> config:\r\n", botnick);
		send_user_list(CONFIG, user);
		send_to_user("|", user);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && ((strncasecmp(temp+1, "getgaglist", 10) == 0) || (strncasecmp(temp+1, "gaglist", 7) == 0)))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		send_gagged_list(user);
		return 1;
		}
#ifdef HAVE_LANG
		else if(((user->type & (ADMIN | OWNER)) != 0) && ((strncasecmp(temp+1, "getlang", 7) == 0) || (strncasecmp(temp+1, "lang|", 6) == 0)))
		{
		uprintf(user, "$To: %s From: %s $<%s> Lang Pack: \r\n", user->nick, botnick, botnick);
		send_user_list(LANG, user);
		send_to_user("|", user);
		return 1;
		}
#endif
		else if(((user->type & (ADMIN | OWNER)) != 0) && ((strncasecmp(temp+1, "getreglist", 10) == 0) || (strncasecmp(temp+1, "reglist", 7) == 0)))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> reglist:\r\n", botnick);
		send_user_list(REGLIST, user);
		send_to_user("|", user);
		return 1;
		}
		else if(strncasecmp(temp+1, "getreq|", 7) == 0)
		{	
    		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(check_if_registered(user->nick) < who_can_seereq)
			{
			uprintf(user, "<%s> %s reqlist|", botnick, lp_notallowedtosee);
			return 1;
			}
		    if(req_on_priv == 0)
		    uprintf(user, "<%s> %s %s:\r\n", botnick, lp_recentlyadded, lp_requests);
            	    else
		    uprintf(user, "$To: %s From: %s $<%s> %s %s:\r\n", user->nick, botnick, botnick, lp_recentlyadded, lp_requests);
		send_req(user);
		return 1;
		}
		else if(strncasecmp(temp+1, "getrls|", 7) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(check_if_registered(user->nick) < who_can_seerls)
			{
			uprintf(user, "<%s> %s releaselist|", botnick, lp_notallowedtosee);
			return 1;
			}
		    if(rls_on_priv == 0)
		    uprintf(user, "<%s> %s %s:\r\n", botnick, lp_recentlyadded, lp_releases);
		    else
		    uprintf(user, "$To: %s From: %s $<%s> %s %s:\r\n", user->nick, botnick, botnick, lp_recentlyadded, lp_releases);
		rls_type(user, "ALL");
		return 1;
		}
		else if (strncasecmp(temp+1, "getrls ", 7) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(check_if_registered(user->nick) < who_can_seerls)
			{
			uprintf(user, "<%s> %s releaselist|", botnick, lp_notallowedtosee);
			return 1;
			}
		sscanf(temp+8, "%1[^|]|", ministring);
		rls_type(user, ministring);
		return 1;
		}
		else if((strncasecmp(temp+1, "geturl", 6) == 0) || (strncasecmp(temp+1, "url", 3) == 0))
		{	
		    if((url_on_priv == 0) && (type == 0))
		    uprintf(user, "<%s> urllist:\r\n", botnick);
		    else
		    uprintf(user, "$To: %s From: %s $<%s> urllist:\r\n", user->nick, botnick, botnick);
		sort_urls(user);
		send_to_user("|", user);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "getwarn", 7) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> warnings:\r\n", botnick);
		send_user_list(WARN, user);
		send_to_user("|", user);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "getwelcomes", 11) == 0))
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> randomwelcomes:\r\n", botnick);
		send_user_list(RANDWEL, user);
		send_to_user("|", user);
		return 1;
		}
		else if(strncasecmp(temp+1, "help", 4) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		send_commands(user);
		    if((user->type == REGULAR) && (show_helpcommand_usage > 0))
		    {
		    human_user = human_sock_list;
			while(human_user != NULL)
			{
			next_human = human_user->next; 
			    if((human_user->user->type & (ADMIN | OWNER)) != 0)
			    uprintf(human_user->user, "<%s> %s (IP: %s) [ !help ]|", botnick, user->nick, user->hostname);
			human_user = next_human; 
			}
		    }
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "hide|", 5) == 0))
		{
		    if((check_if_on_user_list(user->nick)) != NULL)
		    {
			if(user->hide_share != 2)
			{
			user->hide_share=1;
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return 1;
		}
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s|", botnick, lp_hidingshare);
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, HIDE_FILE);
			ret = check_if_hidden(user->nick);
			    if(ret > 0)
			    {
			    remove_line_from_file(user->nick, path, 0);
			    snprintf(tempstring, 150, "%s 1", user->nick);
	    		    add_line_to_file(tempstring, path);
			    loadfile2buf(&hide_mem,HIDE_FILE);
			    } 
			    else
			    {
			    snprintf(tempstring, 150, "%s 1", user->nick);
	    		    add_line_to_file(tempstring, path);
			    loadfile2buf(&hide_mem,HIDE_FILE);
			    }
			}
		    }
		    else
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s|", botnick, lp_presencehidden);
		    }
		return 1;
		}
                else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "hideme|", 7) == 0))
		{
		user->hide_share=2;
		    if((check_if_on_user_list(user->nick)) != NULL)
		    {
		    remove_user_from_list(user->nick);
		    snprintf(tempstring, 150, "$Quit %s|", user->nick);
		    send_to_humans(tempstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s|", botnick, lp_hidingpresence);
		    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, HIDE_FILE);
		    ret = check_if_hidden(user->nick);
			if(ret > 0)
			{
			remove_line_from_file(user->nick, path, 0);
			snprintf(tempstring, 150, "%s 2", user->nick);
	    		add_line_to_file(tempstring, path);
			loadfile2buf(&hide_mem,HIDE_FILE);
			} 
			else
			{
			snprintf(tempstring, 150, "%s 2", user->nick);
	    		add_line_to_file(tempstring, path);
			loadfile2buf(&hide_mem,HIDE_FILE);
			}
		    }
		    else
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s|", botnick, lp_presencealreadyhidden);
		    }
		return 1;
		}
		else if(strncasecmp(temp+1, "hubinfo|", 7) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s:\r\n", botnick, lp_hubinfo);
		uprintf(user, "%s: %s\r\n", lp_hubname, hub_name);
		uprintf(user, "%s: %s:%d\r\n", lp_hubaddr, hub_hostname, listening_port);
		uprintf(user, "%s: %s\r\n", lp_hubdesc, hub_description);		
		uprintf(user, "%s: %d %s\r\n", lp_maxusers, max_users, lp_users);		
		    if(((((double)total_users) * 100) / max_users) < 10) 
		    uprintf(user, "%s: [I----------] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users + 2) * 100) / max_users) >= 10) && (((((double)total_users)* 100) / max_users) < 20))
		    uprintf(user, "%s: [-I---------] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users) * 100) / max_users) >= 20) && (((((double)total_users)* 100) / max_users) < 30))
		    uprintf(user, "%s: [--I--------] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users)* 100) / max_users) >= 30) && (((((double)total_users)* 100) / max_users) < 40))
		    uprintf(user, "%s: [---I-------] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users)* 100) / max_users) >= 40) && (((((double)total_users)* 100) / max_users) < 50))
		    uprintf(user, "%s: [----I------] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users)* 100) / max_users) >= 50) && (((((double)total_users)* 100) / max_users) < 60))
		    uprintf(user, "%s: [-----I-----] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users)* 100) / max_users) >= 60) && (((((double)total_users)* 100) / max_users) < 70))
		    uprintf(user, "%s: [------I----] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users)* 100) / max_users) >= 70) && (((((double)total_users)* 100) / max_users) < 80))
		    uprintf(user, "%s: [-------I---] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users)* 100) / max_users) >= 80) && (((((double)total_users)* 100) / max_users) < 90))
		    uprintf(user, "%s: [--------I--] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if((((((double)total_users)* 100) / max_users) >= 90) && (((((double)total_users)* 100) / max_users) < 100))
		    uprintf(user, "%s: [---------I-] %2.2f%\r\n", lp_fulfilment, (((double)total_users)* 100)/ max_users);
		    if(((((double)total_users)* 100) / max_users) >= 100)
		    uprintf(user, "%s: [----------I] 100%\r\n", lp_fulfilment);
			if(min_psv_share > 0)
			{
			uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_minactshare, (double)min_share / (1024 * 1024 * 1024), min_share, lp_bytes);		
			uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_minpsvshare, (double)min_psv_share / (1024 * 1024 * 1024), min_psv_share, lp_bytes);		
			}
			else
			uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_minshare, (double)min_share / (1024 * 1024 * 1024), min_share, lp_bytes);		
		    uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_minregshare, (double)min_reg_share / (1024 * 1024 * 1024), min_reg_share, lp_bytes);		
		    uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_minvipshare, (double)min_vip_share / (1024 * 1024 * 1024), min_vip_share, lp_bytes);				
		    uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_minkvipshare, (double)min_kvip_share / (1024 * 1024 * 1024), min_kvip_share, lp_bytes);		
		    uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_minopshare, (double)min_op_share / (1024 * 1024 * 1024), min_op_share, lp_bytes);				
		    uprintf(user, "%s: %d GiB\r\n", lp_maxshare, max_share);		
		    uprintf(user, "%s: %d\r\n", lp_maxhubs, maxhubs);		
		    uprintf(user, "%s: %d\r\n", lp_minslots, minslots);		
		    uprintf(user, "%s: %d\r\n", lp_maxslots, maxslots);		
			if(reverse_ratio == 1)
			uprintf(user, "%s: %d %s\r\n", lp_slotratio, ratio, lp_slotsperhub);		
			else
			uprintf(user, "%s: %d %s\r\n", lp_slotratio, ratio, lp_hubsperslot);		
		    uprintf(user, "%s: %s\r\n", lp_mindcppver, mindcppver);		
		    uprintf(user, "%s: %d %s\r\n", lp_tempkickban, kick_bantime, lp_minutes);		
		    uprintf(user, "%s: %d %s\r\n", lp_searchspam, searchspam_time, lp_seconds);		
		    human_user = human_sock_list;
			while(human_user != NULL)
			{
			next_human = human_user->next;
			    if(human_user->user->type == OWNER)
			    {
				if(human_user->user->hide_share != 2)
				usowners++;
				else
				ushidden++;
			    }
			    else if(human_user->user->type == ADMIN)
			    {
				if(human_user->user->hide_share != 2)
				usadmins++;
				else
				ushidden++;
			    }
			    else if(human_user->user->type == CHEEF)
			    uscheefs++;
			    else if(human_user->user->type == OP)
			    usops++;
			    else if(human_user->user->type == KVIP)
			    uskvips++;
	    		    else if(human_user->user->type == VIP)
			    usvips++;
			    else if(human_user->user->type == REG)
			    usregs++;
			    else if(human_user->user->type == PROTECTED)
			    usprots++;
			    else if(human_user->user->type == REGULAR)
			    uscomms++;
			human_user = next_human;
			}
		uprintf(user, "%s: %d (", lp_connectedusers, total_users - ushidden);
		if(usowners > 0)
		uprintf(user, "%d %s, ", usowners, lp_owners_);
		if(usadmins > 0)
		uprintf(user, "%d %s, ", usadmins, lp_admins_);
		if(uscheefs > 0)
		uprintf(user, "%d %s, ", uscheefs, lp_cheefs_);
		if(usops > 0)
		uprintf(user, "%d %s, ", usops, lp_ops_);
		if(uskvips > 0)
		uprintf(user, "%d %s, ", uskvips, lp_kvips_);
		if(usvips > 0)
		uprintf(user, "%d %s, ", usvips, lp_vips_);
		if(usregs > 0)
		uprintf(user, "%d %s, ", usregs, lp_regs_);
		if(usprots > 0)
		uprintf(user, "%d %s, ", usprots, lp_prots_);
		if(uscomms > 0)
		uprintf(user, "%d %s, ", uscomms, lp_regular_);
		uprintf(user, "%d %s)\r\n", total_users - ushidden - usowners - usadmins - uscheefs - usops - uskvips - usvips - usregs - usprots - uscomms, lp_bots_);
		snprintf(tempnick, 50, "%llu", total_share);
		if(strlen(tempnick) > 12)
		uprintf(user, "%s: %2.2f TiB\r\n", lp_datashared, ((double)total_share / (1024*1024*1024)) / 1024, longstring);
		else
		uprintf(user, "%s: %2.2f GiB\r\n", lp_datashared, (double)total_share / (1024*1024*1024), longstring);
		uprintf(user, "%s: %2.2f GiB\r\n", lp_avgdataperuser, ((double)total_share / total_users) / (1024*1024*1024), longstring);
		if(strlen(topic) > 0)
		{
		uprintf(user, "%s: '%s'%s\r\n", lp_topicis, topic, topicstring);		
		}
		if(strlen(recordusstr) > 0)
		{
		sscanf(recordusstr, "%20s", tempnick);
		snprintf(largestring, 500, "%s", recordusstr + strlen(tempnick) + 1);
		uprintf(user, "%s: %s %s (%s)", lp_alltime, tempnick, lp_users, largestring);
		}
		if(strlen(recordshstr) > 0)
		{
		sscanf(recordshstr, "%20s", tempnick);
		sscanf(tempnick, "%lli", &numberrec);
		snprintf(largestring, 500, "%s", recordshstr + strlen(tempnick) + 1);
		if(strlen(tempnick) > 12)
		uprintf(user, ", %2.2f TiB %s (%s)\r\n", ((double)numberrec / (1024*1024*1024)) / 1024, lp_share_, largestring);
		else
		uprintf(user, ", %2.2f GiB %s (%s)\r\n", (double)numberrec / (1024*1024*1024), lp_share_, largestring);
		}
                uptime2str(user, get_uptime());
		if((user->type & (ADMIN | OWNER)) != 0)
		{
		timeinfo = localtime ( &hub_start_time );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		uprintf(user, "\r\n%s (%s %s %s %s, %s): %llu\r\n", lp_totalvisitors, lp_since, sday, smont, syear, sdate, totalvisitors);
		uprintf(user, "%s [ %llu ]: %s [ %llu ], %s [ %llu ]\r\n", lp_deniedconnections, ip_banned + nick_banned, lp_ipbans, ip_banned, lp_nickbans, nick_banned);
		    if(blocked_mc == 0)
		    uprintf(user, "%s: %llu\r\n", lp_mainchatmsgs, accepted_mc);
		    else
		    uprintf(user, "%s [ %llu ]: %s [ %llu ], %s [ %llu ]\r\n", lp_mainchatmsgs, accepted_mc + blocked_mc, lp_accepted_, accepted_mc, lp_blocked_, blocked_mc);
			if(blocked_pm == 0)
			uprintf(user, "%s: %llu\r\n", lp_pmmsgs, accepted_pm);
			else
			uprintf(user, "%s [ %llu ]: %s [ %llu ], %s [ %llu ]\r\n", lp_pmmsgs, accepted_pm + blocked_pm, lp_accepted_, accepted_pm, lp_blocked_, blocked_pm);
		    if(blocked_searches == 0)
		    uprintf(user, "%s: %llu (TTH: %llu)\r\n", lp_searches, accepted_searches, tthsearches);
		    else
		    uprintf(user, "%s [ %llu ]: %s [ %llu (TTH: %llu) ], %s [ %llu ]\r\n", lp_searches, accepted_searches + blocked_searches, lp_accepted_, accepted_searches, tthsearches, lp_blocked_, blocked_searches);
			if(blocked_downloads == 0)
			uprintf(user, "%s: %llu\r\n", lp_downloads, accepted_downloads);
			else
			uprintf(user, "%s [ %llu ]: %s [ %llu ], %s [ %llu ]\r\n", lp_downloads, accepted_downloads + blocked_downloads, lp_accepted_, accepted_downloads, lp_blocked_, blocked_downloads);
		uprintf(user, "%s: %llu", lp_commandscalled, commandsnmb);
		}
		send_to_user("\r\n|", user);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "ipgaglist", 9) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> ipgaglist:\r\n", botnick);
		send_user_list(IPGAG, user);
		send_to_user("|", user);
		return 1;
		}		
		else if(strncasecmp(temp+1, "ipinfo ", 7) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		check_ipinfo(temp+8, user);
		return 1;
		}
		else if(((user->type & (KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "kick ", 5) == 0))
		{	    	     
		    if((sscanf(temp+6, "%50s %500[^|]|", tempnick, largestring) < 2))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !kick <%s> <%s>|", botnick, lp_badsyntax, lp_user_, lp_reason);
		    return 1;
		    }
			if(((tempuser = get_human_user(tempnick)) != NULL))
			{
			    if(tempuser->type < user->type)
			    {
				if(kick_bantime > 0)
				{
				snprintf(tempstring, 150, "%s %dm|", tempuser->hostname, kick_bantime);
				ballow(tempstring, BAN, user);
				snprintf(tempstring, 150, "%s %dm|", tempuser->nick, kick_bantime);
				ballow(tempstring, NICKBAN, user);
				    if(strchr(largestring, '\n') == NULL)
				    {
				    snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (kick_bantime * 60), tempuser->hostname, largestring);
				    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
					if(check_if_banned(tempuser, BANRS) == 0)
					add_line_to_file(bigstring, path);
				    }
					if(strchr(largestring, '\n') == NULL)
					{
					snprintf(bigstring, 1000, "%lu %s [ %s ]", now + (kick_bantime * 60), tempuser->nick, largestring);
					snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
					    if(check_if_banned(tempuser, NICKBANRS) == 0)
					    add_line_to_file(bigstring, path);
					}
				snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %d %s ]", tempuser->nick, tempuser->hostname, largestring, lp_kickedby, user->nick, lp_ban_, kick_bantime, lp_minutes);
				last_temp(lastbuf);
				}
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s: %s|", tempuser->nick, tempuser->nick, user->nick, lp_youwerekicked, lp_because, largestring);
			snprintf(bigstring, 1000, "<%s> is kicking %s (IP: %s) because: %s|", user->nick, tempuser->nick, tempuser->hostname, largestring);
                    	send_to_humans(bigstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			    if(punishment != 0)
			    uprintf(tempuser, "$ForceMove %s|", redirect_host_kickban);                                             
	    		    tempuser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;		
				if(show_actions == 1)
				{
					    snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s: [ %s ]|", tempuser->nick, tempuser->hostname, lp_waskickedby, user->nick, lp_because, largestring);
					    send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
				}
			    }
			    else
			    {
				if(type == 1)
				uprintf(user, "$To: %s From: %s $", user->nick, botnick);
				uprintf(user, "<%s> %s: %s", botnick, lp_nopermission, temp);			
			    }
			}
			else
			{
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> '%s' %s|", botnick, tempnick, lp_isoffline);			
			}
		return 1;
		}	
#ifdef HAVE_LANG
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "lang ", 5) == 0)) 
		{
		    if(type == 1)
		    prvset = 1;
		set_lang(temp+6, user);
		prvset = 0;
		return 1;
		}
#endif
		else if(strncasecmp(temp+1, "lastperm", 8) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> 20 %s:\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n|", botnick, lp_lastpermbans, lastperm20, lastperm19, lastperm18, lastperm17, lastperm16, lastperm15, lastperm14, lastperm13, lastperm12, lastperm11, lastperm10, lastperm9, lastperm8, lastperm7, lastperm6, lastperm5, lastperm4, lastperm3, lastperm2, lastperm1);
		return 1;
		}
		else if(strncasecmp(temp+1, "lasttemp", 8) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> 20 %s:\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n|", botnick, lp_lasttempbans, lasttemp20, lasttemp19, lasttemp18, lasttemp17, lasttemp16, lasttemp15, lasttemp14, lasttemp13, lasttemp12, lasttemp11, lasttemp10, lasttemp9, lasttemp8, lasttemp7, lasttemp6, lasttemp5, lasttemp4, lasttemp3, lasttemp2, lasttemp1);
		return 1;
		}
		else if(strncasecmp(temp+1, "longest", 7) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> 10 %s:\r\n", botnick, lp_longestconnected);
		jt10 = (time_t)0; jt9 = (time_t)0; jt8 = (time_t)0; jt7 = (time_t)0; jt6 = (time_t)0;
		jt5 = (time_t)0; jt4 = (time_t)0; jt3 = (time_t)0; jt2 = (time_t)0; jt1 = (time_t)0;
		human_user = human_sock_list;
		    while(human_user != NULL)
		    {
			next_human = human_user->next;
			    if((human_user->user->jointime <= jt1) || (jt1 == 0))
			    {
			    jt10 = jt9; jt9 = jt8; jt8 =jt7; jt7 = jt6;jt6 = jt5;
			    jt5 = jt4;jt4 = jt3;jt3 = jt2;jt2 = jt1;jt1 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", ujt7);
			    snprintf(ujt7, 50, "%s", ujt6);
			    snprintf(ujt6, 50, "%s", ujt5);
			    snprintf(ujt5, 50, "%s", ujt4);
			    snprintf(ujt4, 50, "%s", ujt3);
			    snprintf(ujt3, 50, "%s", ujt2);
			    snprintf(ujt2, 50, "%s", ujt1);
			    snprintf(ujt1, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt2) || (jt2 == 0))
			    {
			    jt10 = jt9; jt9 = jt8; jt8 = jt7; jt7 = jt6; jt6 = jt5; 
			    jt5 = jt4; jt4 = jt3; jt3 = jt2; jt2 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", ujt7);
			    snprintf(ujt7, 50, "%s", ujt6);
			    snprintf(ujt6, 50, "%s", ujt5);
			    snprintf(ujt5, 50, "%s", ujt4);
			    snprintf(ujt4, 50, "%s", ujt3);
			    snprintf(ujt3, 50, "%s", ujt2);
			    snprintf(ujt2, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt3) || (jt3 == 0))
			    {
			    jt10 = jt9; jt9 = jt8 ;jt8 = jt7;jt7 = jt6;jt6 = jt5;
			    jt5 = jt4;jt4 = jt3;jt3 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", ujt7);
			    snprintf(ujt7, 50, "%s", ujt6);
			    snprintf(ujt6, 50, "%s", ujt5);
			    snprintf(ujt5, 50, "%s", ujt4);
			    snprintf(ujt4, 50, "%s", ujt3);
			    snprintf(ujt3, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt4) || (jt4 == 0))
			    {
			    jt10 = jt9; jt9 = jt8; jt8 = jt7; jt7 = jt6; jt6 = jt5; 
			    jt5 = jt4; jt4 = human_user->user->jointime;
	    		    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", ujt7);
			    snprintf(ujt7, 50, "%s", ujt6);
			    snprintf(ujt6, 50, "%s", ujt5);
			    snprintf(ujt5, 50, "%s", ujt4);
			    snprintf(ujt4, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt5) || (jt5 == 0))
			    {
			    jt10 = jt9; jt9 = jt8; jt8 = jt7; jt7 = jt6; jt6 = jt5; 
			    jt5 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", ujt7);
			    snprintf(ujt7, 50, "%s", ujt6);
			    snprintf(ujt6, 50, "%s", ujt5);
			    snprintf(ujt5, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt6) || (jt6 == 0))
			    {
			    jt10 = jt9; jt9 = jt8; jt8 = jt7; jt7 = jt6; jt6 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", ujt7);
			    snprintf(ujt7, 50, "%s", ujt6);
			    snprintf(ujt6, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt7) || (jt7 == 0))
			    {
			    jt10 = jt9; jt9 = jt8; jt8 = jt7; jt7 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", ujt7);
			    snprintf(ujt7, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt8) || (jt8 == 0))
			    {
			    jt10 = jt9;jt9 = jt8;jt8 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", ujt8);
			    snprintf(ujt8, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt9) || (jt9 == 0))
			    {
			    jt10 = jt9;jt9 = human_user->user->jointime;;
			    snprintf(ujt10, 50, "%s", ujt9);
			    snprintf(ujt9, 50, "%s", human_user->user->nick);
			    }		
			    else if((human_user->user->jointime <= jt10) || (jt10 == 0))
			    {
			    jt10 = human_user->user->jointime;
			    snprintf(ujt10, 50, "%s", human_user->user->nick);		
			    }		
		    human_user = next_human;
		    }
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    if(jt1 > 0)
		    {
			timeinfo = localtime ( &jt1 );
			snprintf(tempnick, 50, "%s", asctime (timeinfo));
			trim_string(tempnick);
			sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
			uprintf(user, "1. %s (%s %s %s, %s)\r\n", ujt1, sday, smont, syear, sdate);
		    }
		    if(jt2 > 0)
		    {
			timeinfo = localtime ( &jt2 );
			snprintf(tempnick, 50, "%s", asctime (timeinfo));
			trim_string(tempnick);
			sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
			uprintf(user, "2. %s (%s %s %s, %s)\r\n", ujt2, sday, smont, syear, sdate);		
		    }
		    if(jt3 > 0)
		    {
		    timeinfo = localtime ( &jt3 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "3. %s (%s %s %s, %s)\r\n", ujt3, sday, smont, syear, sdate);		
		    }
		    if(jt4 > 0)
		    {
		    timeinfo = localtime ( &jt4 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "4. %s (%s %s %s, %s)\r\n", ujt4, sday, smont, syear, sdate);		
		    }
		    if(jt5 > 0)
		    {
		    timeinfo = localtime ( &jt5 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "5. %s (%s %s %s, %s)\r\n", ujt5, sday, smont, syear, sdate);		
		    }
		    if(jt6 > 0)
		    {
		    timeinfo = localtime ( &jt6 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "6. %s (%s %s %s, %s)\r\n", ujt6, sday, smont, syear, sdate);		
		    }
		    if(jt7 > 0)
		    {
		    timeinfo = localtime ( &jt7 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "7. %s (%s %s %s, %s)\r\n", ujt7, sday, smont, syear, sdate);		
		    }
		    if(jt8 > 0)
		    {
		    timeinfo = localtime ( &jt8 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "8. %s (%s %s %s, %s)\r\n", ujt8, sday, smont, syear, sdate);		
		    }
		    if(jt9 > 0)
		    {
		    timeinfo = localtime ( &jt9 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "9. %s (%s %s %s, %s)\r\n", ujt9, sday, smont, syear, sdate);		
		    }
		    if(jt10 > 0)
	    	    {
		    timeinfo = localtime ( &jt10 );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		    uprintf(user, "10. %s (%s %s %s, %s)\r\n", ujt10, sday, smont, syear, sdate);		
		    }
		uprintf(user, "|", botnick);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass ", 5) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s %s|", botnick, lp_masssend, lp_users);
		send_mass_message(temp+6, 0);
		return 1;
		}	
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_a ", 7) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class8, lp_users);
		send_mass_message(temp+8, 8);
		return 1;
		}	
		else if(((user->type & (CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_c ", 7) == 0))		
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class7, lp_users);
		send_mass_message(temp+8, 7);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_k ", 7) == 0))		
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class5, lp_users);
		send_mass_message(temp+8, 5);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_n ", 7) == 0))		
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class1, lp_users);
		send_mass_message(temp + 8, 1);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_o ", 7) == 0))		
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class6, lp_users);
		send_mass_message(temp+8, 6);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_p ", 7) == 0))		
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class2, lp_users);
		send_mass_message(temp + 8, 2);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_r ", 7) == 0))		
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class3, lp_users);
		send_mass_message(temp + 8, 3);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mass_v ", 7) == 0))		
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s [ %s ] %s|", botnick, lp_masssend, lp_class4, lp_users);
		send_mass_message(temp + 8, 4);
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "massmessage ", 12) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s %s|", botnick, lp_masssend, lp_users);
		send_mass_message(temp+13, 0);
		return 1;
		}	
		else if(strncasecmp(temp+1, "me ", 3) == 0)
		{	
    		user->last_write = now;
            	    if((user->autoawaytime > 0) && (user->away_flag == 2))
		    {
		    uprintf(user, "<%s> %s|", botnick, lp_autoawaymodeoff);
			if(check_if_away(user->nick) == 1)
			{
			time2str(difftime(now, time_to_unban), tempstring);
			snprintf(largestring, 500, "<----> %s %s %s %s|", user->nick, lp_isback, lp_after, tempstring);
			}
			else
            		snprintf(largestring, 500, "<----> %s %s|", user->nick, lp_isback);
		    remove_away_user(user);
		    user->away_flag=1;
			if(user->hide_share != 2)
			{
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return 1;
		}
			if(no_chat_for_chatgagged == 0)
			send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			    else
			    {
			    human_user = human_sock_list;
				while(human_user != NULL) 
				{
				next_human = human_user->next;
				    if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
				    uprintf(human_user->user, "%s", largestring);
				human_user = next_human;
				}
			    }
			}
		    }
		snprintf(bigstring, 1000, "** %s %s", user->nick, temp+4); 
		if(no_chat_for_chatgagged == 0)
		send_to_humans(bigstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		else
		{
			human_user = human_sock_list;
			while(human_user != NULL) 
			{
			next_human = human_user->next;
			if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))
			uprintf(human_user->user, "%s", bigstring);
			human_user = next_human;
			}
		}
		if(((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (log_main_chat == 1) && (strchr(bigstring, '\n') == NULL))
		{
		timeinfo = localtime ( &now );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		sdate[5] = '\0';
		if(strcmp(smont, "Jan") == 0)
		snprintf(smont, 3, "01");
		if(strcmp(smont, "Feb") == 0)
		snprintf(smont, 3, "02");
		if(strcmp(smont, "Mar") == 0)
		snprintf(smont, 3, "03");
		if(strcmp(smont, "Apr") == 0)
		snprintf(smont, 3, "04");
		if(strcmp(smont, "May") == 0)
		snprintf(smont, 3, "05");
		if(strcmp(smont, "Jun") == 0)
		snprintf(smont, 3, "06");
		if(strcmp(smont, "Jul") == 0)
		snprintf(smont, 3, "07");
		if(strcmp(smont, "Aug") == 0)
		snprintf(smont, 3, "08");
		if(strcmp(smont, "Sep") == 0)
		snprintf(smont, 3, "09");
		if(strcmp(smont, "Oct") == 0)
		snprintf(smont, 3, "10");
		if(strcmp(smont, "Nov") == 0)
		snprintf(smont, 3, "11");
		if(strcmp(smont, "Dec") == 0)
		snprintf(smont, 3, "12");
		if(strlen(sday) == 1)
		snprintf(tempnick, 50, "%s-%s-0%s", syear, smont, sday); 
		else
		snprintf(tempnick, 50, "%s-%s-%s", syear, smont, sday); 
		create_dailylog(tempnick);
		snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, LOG_DIR, tempnick);		
		snprintf(bigstring, 1000, "%s", temp+4); 
		bigstring[strlen(bigstring) - 1] = '\0';
		snprintf(longstring, 4096, "[%s] * %s %s\n", sdate, user->nick, bigstring); 
		trim_string(longstring);
		add_line_to_file(longstring, path);
		}
		else
		bigstring[strlen(bigstring) - 1] = '\0';
		timeinfo = localtime ( &now );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(message20, 400, "%s", message19);
		snprintf(message19, 400, "%s", message18);
		snprintf(message18, 400, "%s", message17);
		snprintf(message17, 400, "%s", message16);
		snprintf(message16, 400, "%s", message15);
		snprintf(message15, 400, "%s", message14);
		snprintf(message14, 400, "%s", message13);
		snprintf(message13, 400, "%s", message12);
		snprintf(message12, 400, "%s", message11);
		snprintf(message11, 400, "%s", message10);
		snprintf(message10, 400, "%s", message9);
		snprintf(message9, 400, "%s", message8);
		snprintf(message8, 400, "%s", message7);
		snprintf(message7, 400, "%s", message6);
		snprintf(message6, 400, "%s", message5);
		snprintf(message5, 400, "%s", message4);
		snprintf(message4, 400, "%s", message3);
		snprintf(message3, 400, "%s", message2);
		snprintf(message2, 400, "%s", message1);
		if(strlen(bigstring) <= 256)
		{
		snprintf(message1, 400, "--- [%s] ** %s %s", sdate, user->nick, bigstring);
		}
		else
		{
		bigstring[255] = '\0';
		snprintf(message1, 400, "--- [%s] ** %s %s [%s...]", sdate, user->nick, bigstring, lp_cut_);
		}
		return 1;
		}
		else if(strncasecmp(temp+1, "motd", 4) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> Motd:\r\n", botnick);
		send_to_user(motd_mem, user);
		send_to_user("\r\n", user);
		send_to_user("\r\n", user);
		    if(user->type == REGULAR)
		    send_to_user(motdn_mem, user);
		    else if(user->type == PROTECTED)
		    send_to_user(motdp_mem, user);
		    else if(user->type == REG)
		    send_to_user(motdr_mem, user);
		    else if(user->type == VIP)
		    send_to_user(motdv_mem, user);
		    else if(user->type == KVIP)
		    send_to_user(motdk_mem, user);
		    else if(user->type == OP)
		    send_to_user(motdo_mem, user);
		    else if(user->type == CHEEF)
		    send_to_user(motdc_mem, user);
			else if((user->type == ADMIN) || (user->type == OWNER))
			{
			uprintf(user, "\r\n--> %s:\r\n", lp_class1);
			send_to_user(motdn_mem, user);
			uprintf(user, "\r\n--> %s:\r\n", lp_class2);
                        send_to_user(motdp_mem, user);
			uprintf(user, "\r\n--> %s:\r\n", lp_class3);
                        send_to_user(motdr_mem, user);
			uprintf(user, "\r\n--> %s:\r\n", lp_class4);
                        send_to_user(motdv_mem, user);
			uprintf(user, "\r\n--> %s:\r\n", lp_class5);
                        send_to_user(motdk_mem, user);
			uprintf(user, "\r\n--> %s:\r\n", lp_class6);
                        send_to_user(motdo_mem, user);
			uprintf(user, "\r\n--> %s:\r\n", lp_class7);
                        send_to_user(motdc_mem, user);
			uprintf(user, "\r\n--> %s:\r\n", lp_class8);
                        send_to_user(motda_mem, user);
			}
		send_to_user("|", user);
		return 1;
		}
		else if (strncasecmp(temp+1, "msg ", 4) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
    			if((offline_msgs == 2) || ((offline_msgs == 1) && ((user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)))
			{
			if(sscanf(temp+5, "%50s %500[^|]|", tempnick, largestring) != 2)
		    {
		    uprintf(user, "<%s> %s: !msg <%s>|", botnick, lp_badsyntax, lp_nick_);
		    return 1;
		    }
			if(strchr(temp, '\n') != NULL)
			{
			uprintf(user, "<%s> %s|", botnick, lp_nonewlines);
			return 1;
			}
		    if(get_human_user(tempnick) != NULL)
		    {	
		    uprintf(user, "<%s> %s %s|", botnick, tempnick, lp_isonlinenow);
		    return 1;
		    }
		timeinfo = localtime ( &now );
		snprintf(temppass, 50, "%s", asctime (timeinfo));
		trim_string(temppass);
		sscanf(temppass, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MSGS_FILE);		
		snprintf(bigstring, 1000, "%s [ %s @ %s %s %s, %s ]: %s", tempnick, user->nick, sday, smont, syear, sdate, largestring); 
		add_line_to_file(bigstring, path);
		uprintf(user, "<%s> %s [ %s ]|", botnick, lp_savedmsg, tempnick);		
		loadfile2buf(&off_mem,MSGS_FILE);
		    }
		return 1;
		}
		else if(strncasecmp(temp+1, "myinfo", 6) == 0)
		{	
		hubs = 0;
		slots = 0;
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s '%s':\r\n", botnick, lp_userinfo, user->nick);
		uprintf(user, "%s: %s\r\n", lp_nickname, user->nick);	                                                      
		    switch (user->type)
		    {
	    	    case REGULAR: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class1); break;
		    case PROTECTED: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class2); 
			if(check_if_permprotected(user->nick) == 0)
			{
			check_if_registered(user->nick);
			time2str(difftime(time_to_unban, now), largestring);
			uprintf(user, "%s: %s, %s: %s\r\n", lp_prottype, lp_temporary_, lp_expiresin, largestring);
			}
			else
			uprintf(user, "%s: %s\r\n", lp_prottype, lp_permanent_);
			break;
		    case REG: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class3); break;
		    case VIP: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class4); break;
		    case KVIP: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class5); break;
		    case OP: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class6); break;
		    case CHEEF: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class7); break;
    		    case ADMIN:  uprintf(user, "%s: %s\r\n", lp_usertype, lp_class8); break;
		    case OWNER:  uprintf(user, "%s: %s\r\n", lp_usertype, lp_class9); break;
		    }
		uprintf(user, "IP: %s\r\n", user->hostname);
		uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_sharesize, (double)user->share / (1024*1024*1024), user->share, lp_bytes);
		    if(user->desc != NULL)
    		    {
	    		if((stemp=strrchr(user->desc,'<')) != NULL)
			{
			    if(sscanf(user->desc, "%150[^<]<*s", tempstring) > 0)
			    uprintf(user, "%s: %s\r\n", lp_description, tempstring);
				if(sscanf(stemp,"<%20s V:%50[^,],M:%1[^,],H:%d%*[^S]S:%i*s>", client, tempnick, ministring, &hubs, &slots) != 5)
				{ 
				    if(user->email != NULL) 
				    uprintf(user, "%s: %s\r\n", lp_email, user->email);
				    uprintf(user, "%s: %s\r\n", lp_connection, user->con_type);
				    }
				    else
				    {
		    		    uprintf(user, "%s: %s\r\n", lp_tag_, stemp);
					if(user->email != NULL) 
					uprintf(user, "%s: %s\r\n", lp_email, user->email);
				    uprintf(user, "%s: %s\r\n", lp_connection, user->con_type);
					if(strcmp(client, "++") == 0)
					uprintf(user, "%s: DC++\r\n", lp_client_);
					else
					uprintf(user, "%s: %s\r\n", lp_client_, client);
				    uprintf(user, "%s: %s\r\n", lp_version, tempnick);
					if(strcmp(ministring, "A") == 0)
					uprintf(user, "%s: %s\r\n", lp_mode_, lp_active_);
					    if(strcmp(ministring, "P") == 0)
					    uprintf(user, "%s: %s\r\n", lp_mode_, lp_passive_);
					    if(strcmp(ministring, "5") == 0)
					    uprintf(user, "%s: %s\r\n", lp_proxy_);
				    hubstemp = strstr(stemp, ",H:");
					if(sscanf(hubstemp, ",H:%d/%d/%d", &hubs, &reghubs, &ophubs) == 3)
					uprintf(user, "%s: %d (%d %s/%d %s/%d %s)\r\n", lp_hubs_, hubs+reghubs+ophubs, hubs, lp_normal_, reghubs, lp_registered_, ophubs, lp_opped_);
					else
					uprintf(user, "%s: %d\r\n", lp_hubs_, hubs);
					uprintf(user, "%s: %d\r\n", lp_slots_, slots);
					}
				}
				else
				uprintf(user, "%s: %s\r\n", lp_description, user->desc);
			}
		if((user->gagc == 1) || (user->gagd == 1) || (user->gags == 1))
		{
		uprintf(user, "%s: ", lp_gaggedfrom);
		    if(user->gagc == 1)
		    uprintf(user, "%s ", lp_chatting_);		
		    if(user->gagd == 1)
		    uprintf(user, "%s ", lp_downloading_);				
		    if(user->gags == 1)
		    uprintf(user, "%s ", lp_searching_);				
		uprintf(user, "\r\n");
		}
		timeinfo = localtime ( &user->jointime );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		uprintf(user, "%s: %s\r\n", lp_connectedsince, tempnick);
		if(user->last_write > 0)
		{
		time2str(difftime(now, user->last_write), largestring);
		uprintf(user, "%s: %s %s\r\n", lp_lastwritetomain, largestring, lp_ago_);
		}
		    if(user->last_pm_write > 0)
		    {
		    time2str(difftime(now, user->last_pm_write), largestring);
		    uprintf(user, "%s: %s %s\r\n", lp_lastsendpm, largestring, lp_ago_);
		    }
		if(user->last_search > 0)
		{
		time2str(difftime(now, user->last_search), largestring);
		uprintf(user, "%s: %s %s\r\n", lp_lastsearch, largestring, lp_ago_);
		}
		    if(check_if_away(user->nick) == 1)
		    {
		    time2str(difftime(now, time_to_unban), largestring);
		    uprintf(user, "%s %s, %s: %s\r\n", lp_awayfor, largestring, lp_reason, away_reason);		
		    }
		send_to_user("|", user);
		return 1;
		}

		else if(strncasecmp(temp+1, "myip", 4) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s %s|", botnick, lp_youripis, ip_to_string(user->ip));
		return 1;
		}		
		else if(((user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "mypass", 6) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s: %s|", botnick, lp_yourpasswordis, user->password);
		return 1;
		}
		else if (strncasecmp(temp+1, "noautoaway", 10) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		user->autoawaytime = 0;
		user->autoawayreason[0] = '\0';
		remove_autoaway_user(user);
		uprintf(user, "<%s> %s|", botnick, lp_autoawayremoved);		
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "op ", 3) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		give_op(temp+4, user);
		return 1;
		}
		else if(strncasecmp(temp+1, "protectme ", 10) == 0)
		{
		    if(user->type != REGULAR)
		    return 1;
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		if(protection == 1)
		{
    		    if((strstr(temp+11, " ") != NULL) || (strstr(temp+11, "\r") != NULL) || (strstr(temp+11, "\n") != NULL)) 
		    {
		    uprintf(user, "<%s> %s|", botnick, lp_registrationfailed);
		    return 1;
		    }
		sscanf(temp+11, "%50[^|]|", temppass);
		    if(strlen(protectmeprefix) > 0)
		    snprintf(tempstring, 150, "!addreguser %s%s %s 0|", protectmeprefix, user->nick, temppass);
		    else
		    snprintf(tempstring, 150, "!addreguser %s %s 0|", user->nick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempstring);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    if(strlen(protectmeprefix) > 0)
		    uprintf(user, "<%s> %s %s\r\n%s: %s\r\n%s: %s\r\n@ %s:%d|", botnick, lp_youwereregistered, lp_class2, lp_nick_, protectmeprefix, user->nick, lp_pass_, temppass, hub_hostname, listening_port);
		    else
		    uprintf(user, "<%s> %s %s\r\n%s: %s\r\n%s: %s\r\n@ %s:%d|", botnick, lp_youwereregistered, lp_class2, lp_nick_, user->nick, lp_pass_, temppass, hub_hostname, listening_port);
		    	if(show_notifications > 0)
			{	
				    if(strlen(protectmeprefix) > 0)	    
				    snprintf(bigstring, 1000, "%s [ %s ]: [ %s%s (IP: %s) ] %s: [ %s ]|", lp_newregistered, lp_class2, protectmeprefix, user->nick, user->hostname, lp_pass_, temppass);
				    else
				    snprintf(bigstring, 1000, "%s [ %s ]: [ %s (IP: %s) ] %s: [ %s ]|", lp_newregistered, lp_class2, user->nick, user->hostname, lp_pass_, temppass);
				    send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
			}
		    }	     
		}
		return 1;
		}
		else if(((user->type & (OWNER)) != 0) && (strncasecmp(temp+1, "quitprogram", 11) == 0))
		{
			snprintf(largestring, 500, "<%s> %s %s|", botnick, lp_hubkilled, user->nick);
			send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			quit = 1;
		return 1;
		}
		else if ((strncasecmp(temp+1, "readmsg", 7) == 0) && (offline_msgs > 0))
		{
		send_offmsg(user, 1);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "redirectall ", 12) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> %s: %s", botnick, lp_redirectingall, temp+13);
		redirect_all(temp+13, user);
		return 1;
		}
		else if(strncasecmp(temp+1, "record", 6) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(strlen(recordusstr) > 0)
			{
			sscanf(recordusstr, "%20s ", tempnick);
			snprintf(tempstring, 150, "%s", recordusstr + strlen(tempnick) + 1);
			uprintf(user, "<%s> %s: %s %s (%s)", botnick, lp_alltime, tempnick, lp_users, tempstring);
			}
		    if(strlen(recordshstr) > 0)
		    {
		    sscanf(recordshstr, "%20s ", tempnick);
		    sscanf(tempnick, "%lli", &numberrec);
		    snprintf(tempstring, 150, "%s", recordshstr + strlen(tempnick) + 1);
		    if(strlen(tempnick) > 12)
		    uprintf(user, ", %2.2f TiB %s (%s)|", ((double)numberrec / (1024*1024*1024)) / 1024, lp_share_, tempstring);
		    else
		    uprintf(user, ", %2.2f GiB %s (%s)|", (double)numberrec / (1024*1024*1024), lp_share_, tempstring);
		    }
		return 1;
		}
		else if(strncasecmp(temp+1, "regme|", 6) == 0)
		{
		    if((regme_on_priv == 0) && (type == 0))
		    uprintf(user, "<%s> %s:\r\n", botnick, lp_registrationrules);
            	    else
		    uprintf(user, "$To: %s From: %s $<%s> %s:\r\n", user->nick, botnick, botnick, lp_registrationrules);
		send_to_user(regme_mem, user);
                send_to_user("|", user);
		    if((user->type == REGULAR) && (show_helpcommand_usage > 0))
		    {
		    human_user = human_sock_list;
			while(human_user != NULL)
			{
			next_human = human_user->next; 
			    if((human_user->user->type & (CHEEF | ADMIN | OWNER)) != 0)
    			    uprintf(human_user->user, "$To: %s From: %s $<%s> %s (IP: %s) [ !regme ]|", human_user->user, botnick, botnick, user->nick, user->hostname);
			human_user = next_human; 
			}
		    }
		return 1;
		}
		else if(strncasecmp(temp+1, "regme ", 6) == 0) 
		{
	    	    if((user->share < min_regme_share) || (user->type != REGULAR))
		    return 1;
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		if(self_registration == 1)
		{
		    if((strstr(temp+7, " ") != NULL) || (strstr(temp+7, "\r") != NULL) || (strstr(temp+7, "\n") != NULL)) 
		    {
		    uprintf(user, "<%s> %s|", botnick, lp_registrationfailed);
		    return 1;
		    }
		sscanf(temp+7, "%50[^|]|", temppass);
		    if(strlen(regmeprefix) > 0)
		    snprintf(tempstring, 150, "!addreguser %s%s %s 1|", regmeprefix, user->nick, temppass);
		    else
		    snprintf(tempstring, 150, "!addreguser %s %s 1|", user->nick, temppass);
		    if((ret = add_reg_user(tempstring)) == -1)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_couldntaddentry, tempstring);
		    else if(ret == 3)
		    uprintf(user, "<%s> %s reglist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
		    else
		    {		  
		    if(strlen(regmeprefix) > 0)
		    uprintf(user, "<%s> %s %s\r\n%s: %s%s\r\n%s: %s\r\n@ %s:%d|", botnick, lp_youwereregistered, lp_class3, lp_nick_, regmeprefix, user->nick, lp_pass_, temppass, hub_hostname, listening_port);
		    else
		    uprintf(user, "<%s> %s %s\r\n%s: %s\r\n%s: %s\r\n@ %s:%d|", botnick, lp_youwereregistered, lp_class3, lp_nick_, user->nick, lp_pass_, temppass, hub_hostname, listening_port);
			if(show_notifications > 0)
			{
				    if(strlen(regmeprefix) > 0) 
				    snprintf(bigstring, 1000, "%s [ %s ]: [ %s%s (IP: %s) ] %s: [ %s ]|", lp_newregistered, lp_class3, regmeprefix, user->nick, user->hostname, lp_pass_, temppass);
				    else
				    snprintf(bigstring, 1000, "%s [ %s ]: [ %s (IP: %s) ] %s: [ %s ]|", lp_newregistered, lp_class3, user->nick, user->hostname, lp_pass_, temppass);
				    send_to_ops(bigstring, ADMIN | OWNER, show_notifications);
			}	
		    }	     
		}
		else
		uprintf(user, "<%s> %s|", botnick, lp_selfregistrationdisabled);
		return 1;
		}
                else if((user->type == OWNER) && (strncasecmp(temp+1, "reloadfile", 10) == 0))
		{
		    if(type == 1)
		    prvset = 1;
		reload_files_to_memory(temp, user);
		prvset = 0;
		return 1;
		}
#ifdef HAVE_LANG
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "reloadlang", 10) == 0))
                {
            	    if(read_lang() == -1)
            	    {
            	    set_default_lang();
            		if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> File: %s/%s not found!|", botnick, config_dir, LANG_FILE);
            		if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> Language pack not found, resetting to defaults...|", botnick);
            	    write_lang_file();
		    }
            	    else
            	    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s|", botnick, lp_langpackfound);
            	    }
		return 1;
		}
#endif
#ifdef HAVE_PERL
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "reloadscripts", 13) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s|", botnick, lp_reloadingscripts);
			    if(pid > 0)
				script_reload = 1;
                    	    else                                                                                               
			        send_to_non_humans("$ReloadScripts|", FORKED, NULL); 
		return 1;
		}
#endif
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "remcateg ", 9) == 0))	
	        {
		
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			    if(sscanf(temp+10, "%1[^|]|", ministring) == 1)
		    	    {
				if(check_if_categ(ministring) == -1)	
				    uprintf(user, "<%s> %s categories -> %s", botnick, lp_couldntremoveentry, temp);
				else if(check_if_categ(ministring) == 0)		  
				    uprintf(user, "<%s> %s categories -> %s", botnick, lp_entrywasntfound, temp);
				else
				{
				snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CATEG_FILE);
				remove_line_from_file(ministring, path, 0);	
				uprintf(user, "<%s> %s categories -> %s", botnick, lp_removedentryfrom, temp);
				}
			    }
			else 
			uprintf(user, "<%s> %s: !remcateg <%s> (eg: !remcateg A)|", botnick, lp_badsyntax, lp_categletter_);
		return 1;
		}
		else if((check_if_registered(user->nick) >= who_can_addreq) && (strncasecmp(temp+1, "remreq ", 7) == 0))
		{
		remove_entry(user, temp+8, 1, type);
		req_motd();
		return 1;
		}
		else if((check_if_registered(user->nick) >= who_can_addrls) && (strncasecmp(temp+1, "remrls ", 7) == 0))
		{
		remove_entry(user, temp+8, 0, type);
		rls_motd();	
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "remurl ", 7) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = unballow(temp+8, URL)) == -1)
			    uprintf(user, "<%s> %s urllist -> %s", botnick, lp_couldntremoveentry, temp);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s urllist -> %s", botnick, lp_entrywasntfound, temp);
			else
				uprintf(user, "<%s> %s urllist -> %s", botnick, lp_removedentryfrom, temp);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "remwarn ", 8) == 0))
		{
		    if((sscanf(temp+9, "%50[^|]|", tempnick) != 1))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !remwarn <%s>|", botnick, lp_badsyntax, lp_nick_);
		    return 1;
		    }
		ret = remove_warnings(tempnick);
		    if(ret > 0)
		    {
		    loadfile2buf(&warn_mem,WARN_FILE);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: [ %s ]|", botnick, lp_removedallwarns, tempnick);			
		    }
		    else
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: [ %s ]|", botnick, lp_nowarnings, tempnick);
		    }
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "remwelcome ", 11) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = unballow(temp+12, RANDWEL)) == -1)
				uprintf(user, "<%s> %s randomwelcomes -> %s", botnick, lp_couldntremoveentry, temp);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s randomwelcomes -> %s", botnick, lp_entrywasntfound, temp);
			else
				uprintf(user, "<%s> %s randomwelcomes -> %s", botnick, lp_removedentryfrom, temp);
		return 1;
		}
		else if(strncasecmp(temp+1, "report ", 7) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+8, "%50s %500[^|]|", tempnick, largestring) != 2) 
			{
			uprintf(user, "<%s> %s: !report <%s> <%s>|", botnick, lp_badsyntax, lp_user_, lp_reason);
			return 1;
			}
		    if(((tempuser = get_human_user(tempnick)) == NULL))
		    {
		    uprintf(user, "<%s> [ %s ] %s|", botnick, tempnick, lp_isoffline);
		    return 1;
		    }
		human_user = human_sock_list;
		    while(human_user != NULL)
		    {
		    next_human = human_user->next;
		    if((human_user->user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
		    uprintf(human_user->user, "$To: %s From: %s $<%s> [ %s (IP: %s) ] %s [ %s ]. %s [ %s (IP: %s) ].|", human_user->user->nick, opchatnick, opchatnick, tempuser->nick, tempuser->hostname, lp_shouldbechecked, largestring, lp_reportedby, user->nick, user->hostname);
		human_user = next_human;
		}
		uprintf(user, "<%s> %s|", botnick, lp_reportsend);
		return 1;
		}
#ifdef HAVE_LANG
                else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "resetlang", 9) == 0))
                {
            	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			set_default_lang();
            		uprintf(user, "<%s> Language settings have been resetted to default|", botnick);
            		write_lang_file();
		return 1;
		}
#endif
		else if(strncasecmp(temp+1, "rules|", 6) == 0)
		{
		    if((rules_on_priv == 0) && (type == 0))
		    uprintf(user, "<%s> rules:\r\n", botnick);
            	    else
		    uprintf(user, "$To: %s From: %s $<%s> rules:\r\n", user->nick, botnick, botnick);
		send_to_user(rules_mem, user);
		send_to_user("|", user);
		    if((user->type == REGULAR) && (show_helpcommand_usage > 0))
		    {
		    human_user = human_sock_list;
			while(human_user != NULL)
			{
			next_human = human_user->next; 
			    if((human_user->user->type & (ADMIN | OWNER)) != 0)
			    uprintf(human_user->user, "<%s> %s (IP: %s) [ !rules ]|", botnick, user->nick, user->hostname);
			human_user = next_human; 
			}
		    }
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "resetrecord", 11) == 0))
		{
		recordshare = total_share;
    		now = time(NULL);
    		timeinfo = localtime ( &now );
    		snprintf(tempnick, 50, "%s", asctime (timeinfo));
    		trim_string(tempnick);
    		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
    		snprintf(shdatetime, 100, "%s", tempnick);
    		snprintf(recordshstr, 119, "%llu %s %s %s %s, %s", recordshare, sdayn, sday, smont, syear, sdate);
		brokensr = 1;

		recordusers = total_users;
    		now = time(NULL);
    		timeinfo = localtime ( &now );
    		snprintf(tempnick, 50, "%s", asctime (timeinfo));
    		trim_string(tempnick);
    		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
    		snprintf(usdatetime, 100, "%s", tempnick);
		snprintf(recordusstr, 119, "%d %s %s %s %s, %s", recordusers, sdayn, sday, smont, syear, sdate);
		brokenur = 1;
            	    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s|", botnick, lp_recordresetted);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "say ", 4) == 0))
		{
		    if(sscanf(temp+5, "%50s %4096[^|]|", tempnick, longstring) != 2)
		    return 0;
		snprintf(tempstring, 150, "<%s> ", tempnick);
		send_to_humans(tempstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		send_to_humans(temp+5+strlen(tempnick)+1, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		timeinfo = localtime ( &now );
		snprintf(temppass, 50, "%s", asctime (timeinfo));
		trim_string(temppass);
		sscanf(temppass, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		snprintf(message20, 400, "%s", message19);
		snprintf(message19, 400, "%s", message18);
		snprintf(message18, 400, "%s", message17);
		snprintf(message17, 400, "%s", message16);
		snprintf(message16, 400, "%s", message15);
		snprintf(message15, 400, "%s", message14);
		snprintf(message14, 400, "%s", message13);
		snprintf(message13, 400, "%s", message12);
		snprintf(message12, 400, "%s", message11);
		snprintf(message11, 400, "%s", message10);
		snprintf(message10, 400, "%s", message9);
		snprintf(message9, 400, "%s", message8);
		snprintf(message8, 400, "%s", message7);
		snprintf(message7, 400, "%s", message6);
		snprintf(message6, 400, "%s", message5);
		snprintf(message5, 400, "%s", message4);
		snprintf(message4, 400, "%s", message3);
		snprintf(message3, 400, "%s", message2);
		snprintf(message2, 400, "%s", message1);
		    if(strlen(longstring) <= 256)
		    snprintf(message1, 400, "--- [%s] <%s> %s", sdate, tempnick, longstring);
		    else
		    {
		    longstring[255] = '\0';
		    snprintf(message1, 400, "--- [%s] <%s> %s [%s...]", sdate, tempnick, longstring, lp_cut_);
		    }
		return 1;
		}
		else if(strncasecmp(temp+1, "seen ", 5) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		check_if_saw(temp+6, user);
		return 1;
		}
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "set ", 4) == 0)) 
		{	
		    if(type == 1)
		    prvset = 1;
		set_var(temp, user);
		prvset = 0;
		return 1;
		}	
		else if((strncasecmp(temp+1, "setpass ", 8) == 0) && ((user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0))
		{
			temp[strlen(temp)-1]= '\0';
			strncpy(temppass, temp+9, 50); 
			
			if(strstr(temppass, " ") != NULL)
			{
    			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: !setpass <%s>|", botnick, lp_badsyntax, lp_pass_);
			return 1;
			}

			if(remove_reg_user(user->nick) > 0)
			{		
				encrypt_pass(temppass);
				if ((user->type & (OWNER)) != 0)
					snprintf(tempstring, 150, "%s %s 7", user->nick, temppass);
				else if (user->type == ADMIN)
					snprintf(tempstring, 150, "%s %s 6", user->nick, temppass);
				else if (user->type == CHEEF)
					snprintf(tempstring, 150, "%s %s 5", user->nick, temppass);
				else if (user->type == OP)
					snprintf(tempstring, 150, "%s %s 4", user->nick, temppass);
				else if (user->type == KVIP)
					snprintf(tempstring, 150, "%s %s 3", user->nick, temppass);
				else if (user->type == VIP)
					snprintf(tempstring, 150, "%s %s 2", user->nick, temppass);
				else if (user->type == REG)
					snprintf(tempstring, 150, "%s %s 1", user->nick, temppass);
				else if (user->type == PROTECTED)
					snprintf(tempstring, 150, "%s %s 0", user->nick, temppass);
				snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);
				if(add_line_to_file(tempstring, path) > 0)
				{
				    if(type == 1)
				    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
				uprintf(user, "<%s> %s|", botnick, lp_passchanged);
				loadfile2buf(&reglist_mem,REG_FILE);
				}
			}
			else
			logprintf(0, "Error - Failed to change password for user %s\n", user->nick);
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "tban ", 5) == 0))
		{	    	     
		    if((sscanf(temp+6, "%lu%c %s %500[^|]|", &nicklistban, &onestring, tempnick, largestring) != 4))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !tban <%s> <%s> <%s> (eg: !tban 2h nick reason)|", botnick, lp_badsyntax, lp_time_, lp_nick_, lp_reason);
		    return 1;
		    }
			if(onestring == 's')
			exp = nicklistban;
			else if(onestring == 'm')
			exp = nicklistban * 60;
			else if(onestring == 'h')
			exp = nicklistban * 3600;
			else if(onestring == 'd')
			exp = nicklistban * 3600 * 24;
			else if(onestring == 'w')
			exp = nicklistban * 3600 * 24 * 7;
			else
			{
			exp = nicklistban * 60;
			onestring = 'm';
			}
			    if(((tempuser = get_human_user(tempnick)) != NULL))
			    {
				if((tempuser->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0)	
				{
				snprintf(tempstring, 150, "%s %lu%c|", tempuser->hostname, nicklistban, onestring);
				if(type == 1)
				uprintf(user, "$To: %s From: %s $", user->nick, botnick);
				if((ret = ballow(tempstring, BAN, user)) == -1)
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_couldntaddentry, tempuser->hostname);
				else if(ret == 0)		  
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_entryalreadyonlist, tempuser->hostname);
				else
				{
				uprintf(user, "<%s> %s banlist -> %s [%lu%c]|", botnick, lp_addedentryto, tempuser->hostname, nicklistban, onestring);
			if(strchr(largestring, '\n') == NULL)
			{
			snprintf(bigstring, 1000, "%lu %s [ %s ]", now + exp, tempuser->hostname, largestring);
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
			if(check_if_banned(tempuser, BANRS) == 0)
			add_line_to_file(bigstring, path);
			}
			snprintf(tempstring, 150, "%s %lu%c|", tempuser->nick, nicklistban, onestring);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, NICKBAN, user)) == -1)
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_couldntaddentry, tempnick);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
			else
			{
			uprintf(user, "<%s> %s banlist -> %s [%lu%c]|", botnick, lp_addedentryto, tempnick, nicklistban, onestring);
			if(strchr(largestring, '\n') == NULL)
			{
			snprintf(bigstring, 1000, "%lu %s [ %s ]", now + exp, tempuser->nick, largestring);
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
			if(check_if_banned(tempuser, NICKBANRS) == 0)			
			add_line_to_file(bigstring, path);
			}
			}
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s %lu%c %s: %s|", tempuser->nick, user->nick, user->nick, lp_youwerebanned, lp_fornext, nicklistban, onestring, lp_because, largestring);
			snprintf(bigstring, 1000, "<%s> %s %s (IP: %s) %s %lu%c %s: %s|", user->nick, lp_isbanning, tempuser->nick, tempuser->hostname, lp_fornext, nicklistban, onestring, lp_because, largestring);
                    	send_to_humans(bigstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %lu ", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban);
			if(onestring == 's')
			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %lu %s ]", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_seconds);
			else if(onestring == 'm')
			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %lu %s ]", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_minutes);
			else if(onestring == 'h')
			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %lu %s ]", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_hours);
			else if(onestring == 'd')
			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %lu %s ]", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_days);
			else if(onestring == 'w')
			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %lu %s ]", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_weeks);
			else
			snprintf(lastbuf, 800, "%s (IP: %s): %s [ %s %s ] [ %s %lu %s ]", tempuser->nick, tempuser->hostname, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_minutes);
			last_temp(lastbuf);
			if(punishment != 0)
			uprintf(tempuser, "$ForceMove %s|", redirect_host_kickban);                                             
	    		tempuser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;		
			if(show_actions > 0)
			{
			snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s [ %lu%c ] %s: [ %s ]|", tempuser->nick, tempuser->hostname, lp_wasbannedby, user->nick, lp_fornext, nicklistban, onestring, lp_because, largestring);
			send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			}
			}
			}
			else
			{
    		        if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: %s", botnick, lp_nopermission, temp);			
			}
			}
			else
			{
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> '%s' %s|", botnick, tempnick, lp_isoffline);			
			}
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "tbanip ", 7) == 0))
		{	    	     
			if((sscanf(temp+8, "%lu%c %s %500[^|]|", &nicklistban, &onestring, tempnick, largestring) != 4))
			{
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: !tbanip <%s> <IP> <%s> (eg: !tbanip 2h IP reason)|", botnick, lp_badsyntax, lp_time_, lp_reason);
			return 1;
			}
			if(onestring == 's')
			exp = nicklistban;
			else if(onestring == 'm')
			exp = nicklistban * 60;
			else if(onestring == 'h')
			exp = nicklistban * 3600;
			else if(onestring == 'd')
			exp = nicklistban * 3600 * 24;
			else if(onestring == 'w')
			exp = nicklistban * 3600 * 24 * 7;
			else
			{
			exp = nicklistban * 60;
			onestring = 'm';
			}
			snprintf(tempstring, 150, "%s %lu%c|", tempnick, nicklistban, onestring);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, BAN, user)) == -1)
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_couldntaddentry, tempnick);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s banlist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
			else
			{
			uprintf(user, "<%s> %s banlist -> %s [%lu%c]|", botnick, lp_addedentryto, tempnick, nicklistban, onestring);
			if(strchr(largestring, '\n') == NULL)
			{
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
			snprintf(bigstring, 1000, "%lu %s [ %s ]", now + exp, tempnick, largestring);
			add_line_to_file(bigstring, path);
			}
			if(onestring == 's')
			snprintf(lastbuf, 800, "[ IP: %s ]: %s [ %s %s ] [ %s %lu %s ]", tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_seconds);
			else if(onestring == 'm')
			snprintf(lastbuf, 800, "[ IP: %s ]: %s [ %s %s ] [ %s %lu %s ]", tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_minutes);
			else if(onestring == 'h')
			snprintf(lastbuf, 800, "[ IP: %s ]: %s [ %s %s ] [ %s %lu %s ]", tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_hours);
			else if(onestring == 'd')
			snprintf(lastbuf, 800, "[ IP: %s ]: %s [ %s %s ] [ %s %lu %s ]", tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_days);
			else if(onestring == 'w')
			snprintf(lastbuf, 800, "[ IP: %s ]: %s [ %s %s ] [ %s %lu %s ]", tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_weeks);
			else
			snprintf(lastbuf, 800, "[ IP: %s ]: %s [ %s %s ] [ %s %lu %s ]", tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_minutes);
			last_temp(lastbuf);
			if(show_actions > 0)
			{
			snprintf(bigstring, 1000, "[ IP: %s ] %s [ %s ] %s [ %lu%c ] %s: [ %s ]|", tempnick, lp_wasbannedby, user->nick, lp_fornext, nicklistban, onestring, lp_because, largestring);
			send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);				
			}
			}
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "tbannick ", 9) == 0))
		{	    	     
			if((sscanf(temp+10, "%lu%c %s %500[^|]|", &nicklistban, &onestring, tempnick, largestring) != 4))
			{
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: !tbannick <%s> <%s> <%s> (eg: !tbannick 2h nick reason)|", botnick, lp_badsyntax, lp_time_, lp_nickname, lp_reason);
			return 1;
			}
			if(onestring == 's')
			exp = nicklistban;
			else if(onestring == 'm')
			exp = nicklistban * 60;
			else if(onestring == 'h')
			exp = nicklistban * 3600;
			else if(onestring == 'd')
			exp = nicklistban * 3600 * 24;
			else if(onestring == 'w')
			exp = nicklistban * 3600 * 24 * 7;
			else
			{
			exp = nicklistban * 60;
			onestring = 'm';
			}
			snprintf(tempstring, 150, "%s %lu%c|", tempnick, nicklistban, onestring);
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = ballow(tempstring, NICKBAN, user)) == -1)
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_couldntaddentry, tempnick);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s nickbanlist -> %s|", botnick, lp_entryalreadyonlist, tempnick);
			else
			{
			uprintf(user, "<%s> %s banlist -> %s [%lu%c]|", botnick, lp_addedentryto, tempnick, nicklistban, onestring);
			if(strchr(largestring, '\n') == NULL)
			{
			snprintf(bigstring, 1000, "%lu %s [ %s ]", now + exp, tempnick, largestring);
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
			add_line_to_file(bigstring, path);
			}
			if(onestring == 's')
			snprintf(lastbuf, 800, "[ %s: %s ]: %s [ %s %s ] [ %s %lu %s ]", lp_nick_, tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_seconds);
			else if(onestring == 'm')
			snprintf(lastbuf, 800, "[ %s: %s ]: %s [ %s %s ] [ %s %lu %s ]", lp_nick_, tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_minutes);
			else if(onestring == 'h')
			snprintf(lastbuf, 800, "[ %s: %s ]: %s [ %s %s ] [ %s %lu %s ]", lp_nick_, tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_hours);
			else if(onestring == 'd')
			snprintf(lastbuf, 800, "[ %s: %s ]: %s [ %s %s ] [ %s %lu %s ]", lp_nick_, tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_days);
			else if(onestring == 'w')
			snprintf(lastbuf, 800, "[ %s: %s ]: %s [ %s %s ] [ %s %lu %s ]", lp_nick_, tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_weeks);
			else
			snprintf(lastbuf, 800, "[ %s: %s ]: %s [ %s %s ] [ %s %lu %s ]", lp_nick_, tempnick, largestring, lp_bannedby, user->nick, lp_ban_, nicklistban, lp_minutes);
			last_temp(lastbuf);
			if(show_actions > 0)
			{
			snprintf(bigstring, 1000, "[ %s: %s ] %s [ %s ] %s [ %lu%c ] %s: [ %s ]|", lp_nickname, tempnick, lp_wasbannedby, user->nick, lp_fornext, nicklistban, onestring, lp_because, largestring);
			send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);							
			}
			}
		return 1;
		}	
		else if((strncasecmp(temp+1, "topic ", 6) == 0) && ((user->type & (CHEEF | ADMIN | OWNER)) != 0))
		{
		snprintf(largestring, 500, "!set topic %s", temp+7);
		set_var(largestring, user);
		check_settopic();
		    if(strlen(temp+7) == 1)
		    {
		    snprintf(largestring, 500, "<%s> %s %s|", botnick, lp_topicunsetby, user->nick);
		    send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		    remove_settopic();
		    topicstring[0] = '\0';
		    }
		    else
		    {
		    snprintf(largestring, 500, "<%s> %s %s: %s", botnick, lp_topicchangedby, user->nick, temp+7);
		    send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		    remove_settopic();
		    timeinfo = localtime ( &now );
		    snprintf(tempnick, 50, "%s", asctime (timeinfo));
		    trim_string(tempnick);
		    snprintf(tempstring, 150, ", %s %s (%s)", lp_setby, user->nick, tempnick);
		    add_settopic(tempstring);
		    check_settopic();
		if((log_main_chat == 1) && (strstr(temp, "\n") == NULL))
		{
		timeinfo = localtime ( &now );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		sscanf(tempnick, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
		sdate[5] = '\0';
		if(strcmp(smont, "Jan") == 0)
		snprintf(smont, 3, "01");
		if(strcmp(smont, "Feb") == 0)
		snprintf(smont, 3, "02");
		if(strcmp(smont, "Mar") == 0)
		snprintf(smont, 3, "03");
		if(strcmp(smont, "Apr") == 0)
		snprintf(smont, 3, "04");
		if(strcmp(smont, "May") == 0)
		snprintf(smont, 3, "05");
		if(strcmp(smont, "Jun") == 0)
		snprintf(smont, 3, "06");
		if(strcmp(smont, "Jul") == 0)
		snprintf(smont, 3, "07");
		if(strcmp(smont, "Aug") == 0)
		snprintf(smont, 3, "08");
		if(strcmp(smont, "Sep") == 0)
		snprintf(smont, 3, "09");
		if(strcmp(smont, "Oct") == 0)
		snprintf(smont, 3, "10");
		if(strcmp(smont, "Nov") == 0)
		snprintf(smont, 3, "11");
		if(strcmp(smont, "Dec") == 0)
		snprintf(smont, 3, "12");
		if(strlen(sday) == 1)
		snprintf(tempnick, 50, "%s-%s-0%s", syear, smont, sday); 
		else
		snprintf(tempnick, 50, "%s-%s-%s", syear, smont, sday); 
		create_dailylog(tempnick);
		snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, LOG_DIR, tempnick);		
		temp[strlen(temp) - 1] = '\0';
		snprintf(largestring, 500, "[%s] *** %s changes topic to: %s\n", sdate, user->nick, temp+7); 
		trim_string(largestring);
		add_line_to_file(largestring, path);
		}	
		}
		return 1;
		}
		else if(strncasecmp(temp+1, "topic|", 6) == 0)
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(strlen(topic) > 0)
			uprintf(user, "<%s> %s: '%s'%s|", botnick, lp_topicis, topic, topicstring);		
			else
			uprintf(user, "<%s> %s|", botnick, lp_topicnotset);
		return 1;
		}
		else if(((user->type & (KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "ui ", 3) == 0))
		{	    	     
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((sscanf(temp+4, "%50[^|]|", tempnick) < 1))
			{
			uprintf(user, "<%s> %s: !ui <%s>|", botnick, lp_badsyntax, lp_user_);
			return 1;
			}
		    if(((tempuser = get_human_user(tempnick)) != NULL))
		    {
		    hubs = 0;
		    slots = 0;
		    uprintf(user, "<%s> %s '%s':\r\n", botnick, lp_userinfo, tempuser->nick);
		    uprintf(user, "%s: %s\r\n", lp_nickname, tempuser->nick);
			switch (tempuser->type)
			{
			case REGULAR: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class1); break;
			case PROTECTED: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class2); 
			    if(check_if_permprotected(tempuser->nick) == 0)
			    {
			    check_if_registered(tempuser->nick);
			    time2str(difftime(time_to_unban, now), largestring);
			    uprintf(user, "%s: %s, %s: %s\r\n", lp_prottype, lp_temporary_, lp_expiresin, largestring);
			    }
			    else
			    uprintf(user, "%s: %s\r\n", lp_prottype, lp_permanent_);	
			    break;
			case REG: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class3); break;
			case VIP: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class4); break;
			case KVIP: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class5); break;
			case OP: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class6); break;
			case CHEEF: uprintf(user, "%s: %s\r\n", lp_usertype, lp_class7); break;
    			case ADMIN:  uprintf(user, "%s: %s\r\n", lp_usertype, lp_class8); break;
			case OWNER:  uprintf(user, "%s: %s\r\n", lp_usertype, lp_class9); break;
			}
		uprintf(user, "IP: %s\r\n", tempuser->hostname);
		uprintf(user, "%s: %2.2f GiB (%llu %s)\r\n", lp_sharesize, (double)tempuser->share / (1024*1024*1024), tempuser->share, lp_bytes);
		    if(tempuser->desc != NULL)
    		    {
            		if((stemp=strrchr(tempuser->desc,'<')) != NULL)
	    		{
			    if(sscanf(tempuser->desc, "%150[^<]<*s", tempstring) > 0)
			    uprintf(user, "%s: %s\r\n", lp_description, tempstring);
				if(sscanf(stemp,"<%20s V:%50[^,],M:%1[^,],H:%d%*[^S]S:%i*s>", client, tempnick, ministring, &hubs, &slots) != 5)
				{
				    if(tempuser->email != NULL) 
				    uprintf(user, "%s: %s\r\n", lp_email, tempuser->email);
				uprintf(user, "%s: %s\r\n", lp_connection, tempuser->con_type);
				}
				else
				{
				uprintf(user, "%s: %s\r\n", lp_tag_, stemp);
				    if(tempuser->email != NULL) 
				    uprintf(user, "%s: %s\r\n", lp_email, tempuser->email);
				uprintf(user, "%s: %s\r\n", lp_connection, tempuser->con_type);
				    if(strcmp(client, "++") == 0)
				    uprintf(user, "%s: DC++\r\n", lp_client_);
				    else
				    uprintf(user, "%s: %s\r\n", lp_client_, client);
				uprintf(user, "%s: %s\r\n", lp_version, tempnick);
				    if(strcmp(ministring, "A") == 0)
				    uprintf(user, "%s: %s\r\n", lp_mode_, lp_active_);
				    if(strcmp(ministring, "P") == 0)
				    uprintf(user, "%s: %s\r\n", lp_mode_, lp_passive_);
				    if(strcmp(ministring, "5") == 0)
				    uprintf(user, "%s: %s\r\n", lp_mode_, lp_proxy_);
				hubstemp = strstr(stemp, ",H:");
				    if(sscanf(hubstemp, ",H:%d/%d/%d", &hubs, &reghubs, &ophubs) == 3)
				    uprintf(user, "%s: %d (%d %s/%d %s/%d %s)\r\n", lp_hubs_, hubs+reghubs+ophubs, hubs, lp_normal_, reghubs, lp_registered_, ophubs, lp_opped_);
				    else
				    uprintf(user, "%s: %d\r\n", lp_hubs_, hubs);
				uprintf(user, "%s: %d\r\n", lp_slots_, slots);
				}
			}
			else
			uprintf(user, "%s: %s\r\n", lp_description, tempuser->desc);
		    }
		if((tempuser->gagc == 1) || (tempuser->gagd == 1) || (tempuser->gags == 1))
		{
		uprintf(user, "%s: ", lp_gaggedfrom);
		    if(tempuser->gagc == 1)
		    uprintf(user, "%s ", lp_chatting_);		
		    if(tempuser->gagd == 1)
		    uprintf(user, "%s ", lp_downloading_);				
		    if(tempuser->gags == 1)
		    uprintf(user, "%s ", lp_searching_);				
		uprintf(user, "\r\n");
		}
		timeinfo = localtime ( &user->jointime );
		snprintf(tempnick, 50, "%s", asctime (timeinfo));
		trim_string(tempnick);
		uprintf(user, "%s: %s\r\n", lp_connectedsince, tempnick);
		    if(tempuser->last_write > 0)
		    {
		    time2str(difftime(now, tempuser->last_write), largestring);
		    uprintf(user, "%s: %s %s\r\n", lp_lastwritetomain, largestring, lp_ago_);
		    }
			if(tempuser->last_pm_write > 0)
			{
			time2str(difftime(now, tempuser->last_pm_write), largestring);
			uprintf(user, "%s: %s %s\r\n", lp_lastsendpm, largestring, lp_ago_);
			}
		    if(tempuser->last_search > 0)
		    {
		    time2str(difftime(now, tempuser->last_search), largestring);
		    uprintf(user, "%s: %s %s\r\n", lp_lastsearch, largestring, lp_ago_);
		    }
			if(check_if_away(tempuser->nick) == 1)
			{
			time2str(difftime(now, time_to_unban), largestring);
			uprintf(user, "%s %s, %s: %s\r\n", lp_awayfor, largestring, lp_reason, away_reason);		
			}
		    send_to_user("|", user);
		    }
		    else
		    uprintf(user, "<%s> '%s' %s|", botnick, tempnick, lp_isoffline);			
		return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "unallow ", 8) == 0))
		{	     	     
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = unballow(temp+9, ALLOW)) == -1)
			{
				uprintf(user, "<%s> %s allowlist -> %s", botnick, lp_couldntremoveentry, temp);
				logprintf(4, "Error - Failed removing entry from allow list\n");
			}
			else if(ret == 0)		  
				uprintf(user, "<%s> %s in allowlist -> %s", botnick, lp_entrywasntfound, temp);
			else
				uprintf(user, "<%s> %s allowlist -> %s", botnick, lp_removedentryfrom, temp);
		return 1;    	    	     
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "unban ", 6) == 0))
		{	     	     
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = unballow(temp+7, BAN)) == -1)
				uprintf(user, "<%s> %s banlist -> %s", botnick, lp_couldntremoveentry, temp);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s banlist -> %s", botnick, lp_entrywasntfound, temp);
			else
				{
				uprintf(user, "<%s> %s banlist -> %s", botnick, lp_removedentryfrom, temp);
				unballow(temp+7, BANRS);
				}
		return 1;
		}		     	    	     
                else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "unblock ", 8) == 0))
                {        
		block_proc(temp+9);
                return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "ungag ", 6) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		ungag_user(temp+7, user);
		return 1;
		} 
		else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "ungagall", 8) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		ungagall(user);
		return 1;
		} 
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "ungagip ", 8) == 0))
		{	     	     
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = unballow(temp+9, IPGAG)) == -1)
				uprintf(user, "<%s> %s ipgaglist -> %s", botnick, lp_couldntremoveentry, temp);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s ipgaglist -> %s", botnick, lp_entrywasntfound, temp);
			else
			{
				uprintf(user, "<%s> %s ipgaglist -> %s", botnick, lp_removedentryfrom, temp);
				sscanf(temp+9, "%150[^|]", tempstring);
				snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAGRS_FILE);
				remove_line_from_file(tempstring, path, 0);
			}	 
		return 1;    	    	     
		}
                else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1,"unhide|",7) == 0))
		{
		    if((check_if_on_user_list(user->nick)) != NULL)
		    {
			if(user->hide_share != 2)
			{
			user->hide_share=0;
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return 1;
		}
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s|", botnick, lp_unhidingshare);
			    if(check_if_hidden(user->nick) == 1)
			    {
			    snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, HIDE_FILE);
	    		    remove_line_from_file(user->nick, path, 0);
			    loadfile2buf(&hide_mem,HIDE_FILE);
			    }
			}
			else
			{
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s|", botnick, lp_presencehidden);
			}
		    }
		    else
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s|", botnick, lp_presencehidden);
		    }
		return 1;
		}		
                else if(((user->type & (ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "unhideme|", 9) == 0))
		{
		    if(user->hide_share == 2)
		    {	
		    increase_user_list();
		    add_user_to_list(user);
		    user->hide_share = 0;
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return 1;
		}
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s|", botnick, lp_unhidingpresence);
		    human_user = human_sock_list;
		    op_list = get_op_list();
			while(human_user != NULL)
			{
			next_human = human_user->next;
			send_to_user(op_list, human_user->user);
			human_user = next_human;
			}
		    free(op_list);
			if(check_if_hidden(user->nick) == 2)
			{
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, HIDE_FILE);
	    	        remove_line_from_file(user->nick, path, 0);
			loadfile2buf(&hide_mem,HIDE_FILE);
			}
		    }
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && ((strncasecmp(temp+1, "unnickban ", 10) == 0) || (strncasecmp(temp+1, "unbannick ", 10) == 0)))
		{	     	     
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if((ret = unballow(temp+11, NICKBAN)) == -1)
				uprintf(user, "<%s> %s banlist -> %s", botnick, lp_couldntremoveentry, temp);
			else if(ret == 0)		  
				uprintf(user, "<%s> %s banlist -> %s", botnick, lp_entrywasntfound, temp);
			else
				{	
				uprintf(user, "<%s> %s banlist -> %s", botnick, lp_removedentryfrom, temp);
				unballow(temp+11, NICKBANRS);
				}		
		return 1;    	    	     
		}
		else if((strncasecmp(temp+1, "untopic", 7) == 0) && ((user->type & (CHEEF | ADMIN | OWNER)) != 0))
		{
		    if(strlen(topic) > 0)
            	    {
			snprintf(tempnick, 50, "!set topic |");
			set_var(tempnick, user);
			snprintf(largestring, 500, "<%s> %s %s|", botnick, lp_topicunsetby, user->nick);
			send_to_humans(largestring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
			remove_settopic();
			topicstring[0] = '\0';
			topic[0] = '\0';
		    }
		    else
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s|", botnick, lp_topicnotset);
		    }
		return 1;
		}
		else if (strncasecmp(temp+1, "uptime", 6) == 0)
		{	
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		uprintf(user, "<%s> ", botnick);
		uptime2str(user, get_uptime());
		uprintf(user, "|");
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "warn ", 5) == 0))
		{	    	     
		    if((sscanf(temp+6, "%s %500[^|]|", tempnick, largestring) < 2))
		    {
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		    uprintf(user, "<%s> %s: !warn <%s> <%s>|", botnick, lp_badsyntax, lp_user_, lp_reason);
		    return 1;
		    }
		    if(((tempuser = get_human_user(tempnick)) != NULL))
		    {
		    if(tempuser->type < user->type)
		    {
		    ret = check_warning(tempuser, 1);
		    if(ret >= 1)
		    {
		    ret = ret + 1;
		    	timeinfo = localtime ( &now );			
			snprintf(temppass, 50, "%s", asctime (timeinfo));
			trim_string(temppass);
			sscanf(temppass, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
			sdate[5] = '\0';
			snprintf(bigstring, 1000, "%s %lu [ %s @ %s %s %s, %s ]: %s", tempuser->nick, now + (expiredwarn * 86400), user->nick, sday, smont, syear, sdate , largestring);			
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WARN_FILE);
			add_line_to_file(bigstring, path);
			if(ret > max_warnings)
			{
			if(ret - max_warnings == 1)
			{
			if(show_actions > 0)
			{
			snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s: [ %s ] [ %s %d/%d ] [ %s ]|", tempuser->nick, tempuser->hostname, lp_waswarnedby, user->nick, lp_because, largestring, lp_warning_, ret, max_warnings, lp_kick_);
			send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);							
			}
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s: [ %s ] [ %s %d/%d ]|", tempuser->nick, user->nick, user->nick, lp_youwerewarned, lp_because, largestring, lp_warning_, ret, max_warnings);			
			uprintf(tempuser, "<%s> %s %s: %s [ %d/%d ], %s|", botnick, lp_youwerekicked, lp_because, lp_toomanywarnings, ret, max_warnings, lp_nextwarningban);			
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s: %s [ %d/%d ], %s|", tempuser->nick, user->nick, user->nick, lp_youwerekicked, lp_because, lp_toomanywarnings, ret, max_warnings, lp_nextwarningban);			
			if(punishment != 0)
			uprintf(tempuser, "$ForceMove %s|", redirect_host_kickban);                                             
	    		tempuser->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;		
			}
			else if(ret - max_warnings > 1)
			{
			if(show_actions > 0)
			{
			snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s: [ %s ] [ %s %d/%d ] [ %s: %s %s %s ]|", tempuser->nick, tempuser->hostname, lp_waswarnedby, user->nick, lp_because, largestring, lp_warning_, ret, max_warnings, lp_gaggedfrom, lp_chatting_, lp_searching_, lp_downloading_);
			send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);							
			}
			tempuser->gagc = 1;
			tempuser->gags = 1;
			tempuser->gagd = 1;
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s: [ %s ] [ %s %d/%d ]|", tempuser->nick, user->nick, user->nick, lp_youwerewarned, lp_because, largestring, lp_warning_, ret, max_warnings);			
			uprintf(tempuser, "<%s> %s %s/%s/%s %s: %s [ %d/%d ]|", botnick, lp_youweregagged, lp_chatting_, lp_downloading_, lp_searching_, lp_because, lp_toomanywarnings, ret, max_warnings);			
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s/%s/%s %s: %s [ %d/%d ]|", tempuser->nick, user->nick, user->nick, lp_youweregagged, lp_chatting_, lp_downloading_, lp_searching_, lp_because, lp_toomanywarnings, ret, max_warnings);			
			}
			}
			else
			{
			if(show_actions > 0)
			{
			snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s: [ %s ] [ %s %d/%d ]|", tempuser->nick, tempuser->hostname, lp_waswarnedby, user->nick, lp_because, largestring, lp_warning_, ret, max_warnings);
			send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);							
			}
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s: [ %s ] [ %s %d/%d ]|", tempuser->nick, user->nick, user->nick, lp_youwerewarned, lp_because, largestring, lp_warning_, ret, max_warnings);			
			}
		}
			else
			{
			timeinfo = localtime ( &now );			
			snprintf(temppass, 50, "%s", asctime (timeinfo));
			trim_string(temppass);
			sscanf(temppass, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);
			sdate[5] = '\0';
			snprintf(bigstring, 1000, "%s %lu [ %s @ %s %s %s, %s ]: %s", tempuser->nick, now + (expiredwarn * 86400), user->nick, sday, smont, syear, sdate , largestring);			
			snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WARN_FILE);
			add_line_to_file(bigstring, path);
			if(show_actions > 0)
			{
			snprintf(bigstring, 1000, "[ %s (IP: %s) ] %s [ %s ] %s: [ %s ] [ %s 1/%d ]|", tempuser->nick, tempuser->hostname, lp_waswarnedby, user->nick, lp_because, largestring, lp_warning_, max_warnings);
			send_to_ops(bigstring, OP | CHEEF | ADMIN | OWNER, show_actions);							
			}
			uprintf(tempuser, "$To: %s From: %s $<%s> %s %s: [ %s ] [ %s 1/%d ]|", tempuser->nick, user->nick, user->nick, lp_youwerewarned, lp_because, largestring, lp_warning_, max_warnings);			
			}
			loadfile2buf(&warn_mem,WARN_FILE);
			}
			else
			{
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s|", botnick, lp_youcanwarnonly);			
			}
			}
			else
			{
			if(type == 1)
			uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> '%s' %s|", botnick, tempnick, lp_isoffline);			
			}
			return 1;
		}	
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "whohubs ", 8) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
		if(sscanf(temp+9, "%d|", &ret) != 1) 
		{
		uprintf(user, "<%s> %s: !whohubs <%s>|", botnick, lp_badsyntax, lp_number_);
		return 1;
		}
		    if(ret < 3)
		    {
		    uprintf(user, "<%s> %s|", botnick, lp_whohubswarn);
		    return 1;
		    }
		uprintf(user, "<%s> %s >= %d:\r\n", botnick, lp_hubs_, ret); 
		human_user = human_sock_list;
		    while(human_user != NULL)
		    {
		    next_human = human_user->next;
			if((human_user->user->desc != NULL) && (human_user->user->type != NON_LOGGED) && (strcmp(human_user->user->nick, "Non_logged_in_user") != 0))
    			{
			    if((stemp=strrchr(human_user->user->desc,'<')) != NULL)
			    {
    				if(sscanf(stemp,"<%20s V:%50[^,],M:%1[^,],H:%d%*[^S]S:%d*s>", client, tempnick, ministring, &hubs, &slots) == 5)
				{
				hubstemp = strstr(stemp, ",H:");
				if((sscanf(hubstemp, ",H:%d/%d", &hubs, &reghubs) == 2) && (count_reghubs == 1))
				hubs = hubs + reghubs;
				    if(hubs >= ret)
				    {
				    matches++;
				    uprintf(user, "%s (IP: %s). %s: %d\r\n", human_user->user->nick, human_user->user->hostname, lp_hubs_, hubs);
				    }
				}
			    }
			}
		    human_user = next_human;
		    }
		uprintf(user, "[ %d %s ]|", matches, lp_users); 
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "whoip ", 6) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+7, "%16[^|]|", tempnick) != 1) 
			{
			uprintf(user, "<%s> %s: !whoip <IP>|", botnick, lp_badsyntax);
			return 1;
			}
		if(strstr(tempnick, ".") == NULL)
		{
		uprintf(user, "<%s> %s|", botnick, lp_musthavedot);
		return 1;
		}
		    if(strlen(tempnick) < 5)
		    {
		    uprintf(user, "<%s> %s: 5|", botnick, lp_charnumbers);
		    return 1;
		    }
		uprintf(user, "<%s> %s [ %s ]:\r\n", botnick, lp_searchforip, tempnick); 
		human_user = human_sock_list;
		    while(human_user != NULL)
		    {
		    next_human = human_user->next;
			if(strstr(human_user->user->hostname, tempnick) != NULL)
			{
			matches++;
			uprintf(user, "%s: IP %s\r\n", human_user->user->nick, ip_to_string(human_user->user->ip)); 
			}
		    human_user = next_human;
		    }
		uprintf(user, "[ %d %s ]|", matches, lp_users); 
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "whoratio ", 9) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+10, "%d|", &ret) != 1) 
			{
			uprintf(user, "<%s> %s: !whoratio <%s>|", botnick, lp_badsyntax, lp_number_);
			return 1;
			}
		    if(reverse_ratio == 1)
		    uprintf(user, "<%s> %s <= %d:\r\n", botnick, lp_slotratio, ret); 
		    else
		    uprintf(user, "<%s> %s >= %d:\r\n", botnick, lp_slotratio, ret); 
		human_user = human_sock_list;
		    while(human_user != NULL)
		    {
			next_human = human_user->next;
			if((human_user->user->desc != NULL) && (human_user->user->type != NON_LOGGED) && (strcmp(human_user->user->nick, "Non_logged_in_user") != 0))
			{
			    if((stemp=strrchr(human_user->user->desc,'<')) != NULL)
			    {
    				if(sscanf(stemp,"<%20s V:%50[^,],M:%1[^,],H:%d%*[^S]S:%d*s>", client, tempnick, ministring, &hubs, &slots) == 5)
				{
				hubstemp = strstr(stemp, ",H:");
				if((sscanf(hubstemp, ",H:%d/%d", &hubs, &reghubs) == 2) && (count_reghubs == 1))
				hubs = hubs + reghubs;
				    if(reverse_ratio != 1)
				    {
					if((((double)hubs/slots) >= ret) && (hubs > 0) && (slots > 0))
					{
					matches++;
					uprintf(user, "%s (IP: %s). %s: %2.2f (%s: %d, %s: %d)\r\n", human_user->user->nick, human_user->user->hostname, lp_slotratio, (double)hubs/slots, lp_hubs_, hubs, lp_slots_, slots);
					}
				    }
				    else
				    {
					if((((double)slots/hubs) <= ret) && (hubs > 0) && (slots > 0))
					{
					matches++;
					uprintf(user, "%s (IP: %s): %s: %2.2f (%s: %d, %s: %d)\r\n", human_user->user->nick, human_user->user->hostname, lp_slotratio, (double)slots/hubs, lp_slots_, slots, lp_hubs_, hubs);
					}
				    }
				}
			    }
			}
			human_user = next_human;
		    }
		uprintf(user, "[ %d %s ]|", matches, lp_users); 
		return 1;
		}
		else if(((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (strncasecmp(temp+1, "whoslots ", 9) == 0))
		{
		    if(type == 1)
		    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			if(sscanf(temp+10, "%d|", &ret) != 1) 
			{
			    if(type == 1)
			    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
			uprintf(user, "<%s> %s: !whoslots <%s>|", botnick, lp_badsyntax, lp_number_);
			return 1;
			}
		    uprintf(user, "<%s> %s == %d:\r\n", botnick, lp_slots_, ret); 
		    human_user = human_sock_list;
			while(human_user != NULL)
			{
			next_human = human_user->next;
			    if((human_user->user->desc != NULL) && (human_user->user->type != NON_LOGGED) && (strcmp(human_user->user->nick, "Non_logged_in_user") != 0))
    			    {
			    if((stemp=strrchr(human_user->user->desc,'<')) != NULL)
			    {
    				if(sscanf(stemp,"<%20s V:%50[^,],M:%1[^,],H:%d%*[^S]S:%d*s>", client, tempnick, ministring, &hubs, &slots) == 5)
				{
				    if(slots == ret)
				    {
				    matches++;
				    uprintf(user, "%s (IP: %s). %s: %d\r\n", human_user->user->nick, human_user->user->hostname, lp_slots_, slots);
				    }
				}
			    }
			    }
			human_user = next_human;
			}
		uprintf(user, "[ %d %s ]|", matches, lp_users); 
		return 1;
		}
return 0;
}

//DB// If a user wants info about one other, it looks like this:
//DB// $ScriptINFO requested_user $Script|
void script_info(char *buf, struct user_t *user)
{
	char command[14];
	char requesting[51];
	char requested[51];
	struct user_t *from_user;

	if(sscanf(buf, "%13s %50s %50[^|]|", command, requested, requesting) != 3)
	{                                                                    
		logprintf(4, "Received bad $ScriptINFO command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}

	if((from_user = get_human_user(requested)) != NULL)
	{	
		if(strncmp(requesting, "$Script", 7) == 0)
		{	     	 
#ifdef HAVE_PERL
            	        command_to_scripts("$Script user_info %s %s %llu|", from_user->nick,                         
	                from_user->hostname, from_user->share);
			
			if(pid > 0)
		{
		if(send_user_info(from_user, requesting) == 0)
		{
		from_user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return;
		}
		}
			else
		{
		if(send_user_info(from_user, requesting ) == 0)
		{
		from_user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		return;
		}
		}
#endif
		}
	}
	
	else
	send_to_non_humans(buf, FORKED, user);
}

void reload_files_to_memory(char *buf, struct user_t *user)
{
	char command[12];
	char file[21];

	if(sscanf(buf, "%11s %20[^|]|", command, file) != 2)
	{                                                                    
		logprintf(4, "Received bad $GetINFO command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}

	if((user->type == FORKED) || (user->type == OWNER))
	{	
	if(strcmp(file, "all") == 0)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	read_config();
	uprintf(user, "<%s> \r\n'config' %s\r\n", botnick, lp_wasreloaded);
	loadfile2buf(&rules_mem,RULES_FILE);
	uprintf(user, "'rules' %s\r\n", lp_wasreloaded);
	loadfile2buf(&regme_mem,REGME_FILE);
	uprintf(user, "'regme' %s\r\n", lp_wasreloaded);
	loadfile2buf(&reglist_mem,REG_FILE);
	uprintf(user, "'reglist' %s\r\n", lp_wasreloaded);
	loadfile2buf(&banlist_mem,BAN_FILE);
	uprintf(user, "'banlist' %s\r\n", lp_wasreloaded);
	loadfile2buf(&nickbanlist_mem,NICKBAN_FILE);
	uprintf(user, "'nickbanlist' %s\r\n", lp_wasreloaded);
	loadfile2buf(&allowlist_mem,ALLOW_FILE);
	uprintf(user, "'allowlist' %s\r\n", lp_wasreloaded);
	loadfile2buf(&autofav_mem,AUTOFAV_FILE);
	uprintf(user, "'autofavmsg' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motd_mem, MOTD_FILE);
	uprintf(user, "'scheduledmsg' %s\r\n", lp_wasreloaded);
	loadfile2buf(&schedule_mem,SCHEDULE_FILE);
	uprintf(user, "'motd' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motdn_mem, MOTD_N);
	uprintf(user, "'motd_normal' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motdp_mem, MOTD_P);
	uprintf(user, "'motd_prot' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motdr_mem, MOTD_R);
	uprintf(user, "'motd_reg' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motdv_mem, MOTD_V);
	uprintf(user, "'motd_vip' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motdk_mem, MOTD_K);
	uprintf(user, "'motd_kvip' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motdo_mem, MOTD_O);
	uprintf(user, "'motd_op' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motdc_mem, MOTD_C);
	uprintf(user, "'motd_cheef' %s\r\n", lp_wasreloaded);
	loadfile2buf(&motda_mem, MOTD_A);
	uprintf(user, "'motd_admin' %s|", lp_wasreloaded);
	}
	if(strcmp(file, "config") == 0)
	{
	read_config();
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'config' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "rules") == 0)
	{
	loadfile2buf(&rules_mem,RULES_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'rules' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "regme") == 0)
	{
	loadfile2buf(&regme_mem,REGME_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'regme' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "reglist") == 0)
	{
	loadfile2buf(&reglist_mem,REG_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'reglist' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd") == 0)
	{
	loadfile2buf(&motd_mem, MOTD_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_normal") == 0)
	{
	loadfile2buf(&motdn_mem,MOTD_N);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_normal' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_prot") == 0)
	{
	loadfile2buf(&motdp_mem,MOTD_P);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_prot' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_reg") == 0)
	{
	loadfile2buf(&motdr_mem,MOTD_R);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_reg' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_vip") == 0)
	{
	loadfile2buf(&motdv_mem,MOTD_V);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_vip' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_kvip") == 0)
	{
	loadfile2buf(&motdk_mem,MOTD_K);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_kvip' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_op") == 0)
	{
	loadfile2buf(&motdo_mem,MOTD_O);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_op' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_cheef") == 0)
	{
	loadfile2buf(&motdc_mem,MOTD_C);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_cheef' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "motd_admin") == 0)
	{
	loadfile2buf(&motda_mem,MOTD_A);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'motd_admin' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "banlist") == 0)
	{
	loadfile2buf(&banlist_mem,BAN_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'banlist' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "autofavmsg") == 0)
	{
	loadfile2buf(&autofav_mem,AUTOFAV_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'autofavmsg' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "scheduledmsg") == 0)
	{
	loadfile2buf(&schedule_mem,SCHEDULE_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'scheduledmsg' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "nickbanlist") == 0)
	{
	loadfile2buf(&nickbanlist_mem,NICKBAN_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'nickbanlist' %s|", botnick, lp_wasreloaded);
	}
	}
	if(strcmp(file, "allowlist") == 0)
	{
	loadfile2buf(&allowlist_mem,ALLOW_FILE);
	if(user->type == OWNER)
	{
	if(prvset == 1)
	uprintf(user, "$To: %s From: %s $", user->nick, botnick);
	uprintf(user, "<%s> 'allowlist' %s|", botnick, lp_wasreloaded);
	}
	}
	prvset = 0;
	return;
	}
	else
	send_to_non_humans(buf, FORKED, user);
}

void gag_user_by_script(char *buf, struct user_t *user)
{
	char command[6];
	char requested[MAX_NICK_LEN+1];
	int type;
	struct user_t *from_user;

	if(sscanf(buf, "%5s %50s %d|", command, requested, &type) != 3)
	{                                                                    
		logprintf(4, "Received bad $Gag command from %s at %s:\n", user->nick, user->hostname);
		if(strlen(buf) < 3500)
			logprintf(4, "%s\n", buf);
		else
			logprintf(4, "too large buf\n");
		return;
	}
	if((from_user = get_human_user(requested)) != NULL)
	{	
	//DB// ungag
	if(type == 0)
	{
	from_user->gagc = 0;
	from_user->gagd = 0;
	from_user->gags = 0;
	}
	//DB// gag chat
	else if(type == 1)
	from_user->gagc = 1;
	//DB// gag search
	else if(type == 2)
	from_user->gagd = 1;
	//DB// gag download
	else if(type == 3)
	from_user->gags = 1;
	}
	
	else
	send_to_non_humans(buf, FORKED, user);
}
