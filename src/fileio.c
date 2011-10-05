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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if HAVE_MALLOC_H
# include <malloc.h>
#endif
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
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
#include <dirent.h>
#ifdef HAVE_SYSLOG_H
# include <syslog.h>
#endif
#ifdef SWITCH_USER
# include <pwd.h>
#endif

#include "main.h"
#include "utils.h"
#include "fileio.h"
#include "network.h"
#include "motd.h"
#include "logger.h"
#include "userlist.h"
#include "langvars.h"

#ifdef HAVE_PERL
# include "perl_utils.h"
#endif

#ifndef HAVE_STRTOLL
# ifdef HAVE_STRTOQ
#  define strtoll(X, Y, Z) (long long)strtoq(X, Y, Z)
# endif
#endif

/* Reads config file */
int read_config(void)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CONFIG_FILE);
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In read_config()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In read_config()/open(): %s", strerror(errno));
	return -1;	
     }
   	
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In read_config(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }
   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In read_config()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     
	     /* Name of the hub */
	     if(strncmp(line + i, "hub_name", 8) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  strncpy(hub_name, strchr(line + i, '"') + 1, MAX_HUB_NAME);
		  if(*(hub_name + strlen(hub_name) - 1) == '"')
		    *(hub_name + strlen(hub_name) - 1) = '\0';
	       }
	     /* Hostname to upload to public hublist and for hublist pingers */
	     else if(strncmp(line + i, "hub_hostname", 12) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  strncpy(hub_hostname, strchr(line + i, '"') + 1, 120);
		  if(*(hub_hostname + strlen(hub_hostname) - 1) == '"')
		    *(hub_hostname + strlen(hub_hostname) - 1) = '\0';
	       }
	     /* The port the hub is listening on */
	     else if(strncmp(line + i, "listening_port", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  listening_port = (unsigned int)(atoi(line + i));
	       }
	     /* Description of hub. Sent to public hub list */
	     else if(strncmp(line + i, "hub_description", 15) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  strncpy(hub_description, strchr(line + i, '"') + 1, MAX_HUB_DESC);
		  if(*(hub_description + strlen(hub_description) - 1) == '"')
		    *(hub_description + strlen(hub_description) - 1) = '\0';
	       }
	     /* Maximum hub users */
	     else if(strncmp(line + i, "max_users", 9) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  max_users = atoi(line + i);
	       }
	     /* 1 for registered only mode */
	     else if(strncmp(line + i, "registered_only", 15) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  registered_only = atoi(line + i);
	       }
             /* If set, all unregistered users will be prompted for it while logging in */
	     else if(strncmp(line + i, "default_pass", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                         while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return -1;
                    }
                  strncpy(default_pass, strchr(line + i, '"') + 1, MAX_ADMIN_PASS_LEN);
                  if(*(default_pass + strlen(default_pass) - 1) == '"')
                    *(default_pass + strlen(default_pass) - 1) = '\0';
               }
	     /* If set, unregistered user will have to start their nicknames with one of these prefixes */
	     else if(strncmp(line + i, "default_prefixes", 16) == 0)
	       {
                  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                  strncpy(default_prefixes, strchr(line + i, '"') + 1, 255);
                  if(*(default_prefixes + strlen(default_prefixes) - 1) == '"')
                     *(default_prefixes + strlen(default_prefixes) - 1) = '\0'; 
	       }
	     /* If set, all unregistered users with characters in nicknames not listed in this variable will get kicked */
	     else if(strncmp(line + i, "nickchars", 9) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(nickchars, strchr(line + i, '"') + 1, 300);
                   if(*(nickchars + strlen(nickchars) - 1) == '"')
                     *(nickchars + strlen(nickchars) - 1) = '\0';
	       }
	     /* If set, all unregistered users with nickname started with one of these characters will get kicked */
	     else if(strncmp(line + i, "forbchars", 9) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(forbchars, strchr(line + i, '"') + 1, 30);
                   if(*(forbchars + strlen(forbchars) - 1) == '"')
                     *(forbchars + strlen(forbchars) - 1) = '\0';
	       }
	     /* Minimum share for REGULAR class */
	     else if(strncmp(line + i, "min_share", 9) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  min_share = strtoll(line + i, (char **)NULL, 10);
	       }
	     /* Minimum share for passvive REGULAR class users, if different than 0
	     it overrides min_share for passive users */
	     else if(strncmp(line + i, "min_psv_share", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  min_psv_share = strtoll(line + i, (char **)NULL, 10);
	       }
             /* Minimum share for REG class */
	     else if(strncmp(line + i, "min_reg_share", 13) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  min_reg_share = strtoll(line + i, (char **)NULL, 10);
               }
             /* Minimum share for VIP class */
	     else if(strncmp(line + i, "min_vip_share", 13) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  min_vip_share = strtoll(line + i, (char **)NULL, 10);
               }
             /* Minimum share for KVIP class */
	     else if(strncmp(line + i, "min_kvip_share", 14) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  min_kvip_share = strtoll(line + i, (char **)NULL, 10);
               }
             /* Minimum share for OP class */
	     else if(strncmp(line + i, "min_op_share", 12) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  min_op_share = strtoll(line + i, (char **)NULL, 10);
	       }
	     /* If set to 1 kicks REGULAR users for not having any shared data */
	     else if(strncmp(line + i, "kick_on_zeroshare", 17) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  kick_on_zeroshare = atoi(line + i);
	       }
	      /* If set to 1 gags user from searching/downloading instead of kicking for not enough shared data */
	      else if(strncmp(line + i, "gag_on_minshare", 15) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  gag_on_minshare = atoi(line + i);
	       }
	     /* Maximum data that can be shared by REGULAR user */
	     else if(strncmp(line + i, "max_share", 9) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  max_share = strtoll(line + i, (char **)NULL, 10);
	       }
	     /* List of allowed clients (DC++ is ++) */
	     else if(strncmp(line + i, "allowed_clients", 15) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(allowed_clients, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(allowed_clients + strlen(allowed_clients) - 1) == '"')
                     *(allowed_clients + strlen(allowed_clients) - 1) = '\0';
                 }
	     /* Maximum class that will have TAG description checked */	
	     else if(strncmp(line + i, "check_class", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  check_class = strtoll(line + i, (char **)NULL, 10);
	       }
	     /* Minimum class that can use commands */
	     else if(strncmp(line + i, "min_command_class", 17) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  min_command_class = strtoll(line + i, (char **)NULL, 10);
	       }
	     /* Minimum class that will receive UserCommands after logging in */
	     else if(strncmp(line + i, "min_usercommand_class", 21) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  min_usercommand_class = strtoll(line + i, (char **)NULL, 10);
	       }
	     /* If set to 1, reghubs will be added to normalhubs (H:n/r/o) while checking for maxhubs */	
    	     else if(strncmp(line + i, "count_reghubs", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  count_reghubs = atoi(line + i);
	       }
	     /* Maximum allowed number of opened hubs */
	     else if(strncmp(line + i, "maxhubs", 7) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  maxhubs = atoi(line + i);
	       }
	     /* Minimum required number of opened upload slots */
	     else if(strncmp(line + i, "minslots", 8) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  minslots = atoi(line + i);
	       }
	     /* Maximum allowed number of opened upload slots */
	     else if(strncmp(line + i, "maxslots", 8) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  maxslots = atoi(line + i);
	       }
	     /* If set to 0 ratio is hubs/slot, if 1 slots/hub */
	     else if(strncmp(line + i, "reverse_ratio", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  reverse_ratio = atoi(line + i);
	       }
	     /* Maximum hubs/slot or slots/hub ratio allowed due to reverse_ratio */
	     else if(strncmp(line + i, "ratio", 5) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  ratio = atoi(line + i);
	       }
	     /* Miniumum version of DC++ required */
	     else if(strncmp(line + i, "mindcppver", 10) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(mindcppver, strchr(line + i, '"') + 1, 10);
                   if(*(mindcppver + strlen(mindcppver) - 1) == '"')
                     *(mindcppver + strlen(mindcppver) - 1) = '\0';
		}
	     /* Miniumum nickname lenght required for unregistered users */
	     else if(strncmp(line + i, "min_nick_len", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  min_nick_len = atoi(line + i);
	       }
	     /* If set to 1 kicks REGULAR user in passive mode */
	     else if(strncmp(line + i, "deny_passive", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  deny_passive = atoi(line + i);
	       }
	     /* If set to 1 kicks REGULAR user using proxy to connect to the hub */
	     else if(strncmp(line + i, "deny_proxy", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  deny_proxy = atoi(line + i);
	       }
	     /* If set to 1 kicks REGULAR user for tag M:A and psv string in nickname */
	     else if(strncmp(line + i, "restrict_mode", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  restrict_mode = atoi(line + i);
	       }
	     /* Minimum upload limit allowed, works with tags B: and L: and with [Netlimiter Xkb/s] connection type */
	     else if(strncmp(line + i, "connlimit", 9) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  connlimit = atoi(line + i);
	       }
	     /* Default prefix taht is added to nicknames of users using !regme command */
	     else if(strncmp(line + i, "regmeprefix", 11) == 0)
	       {
                  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                  strncpy(regmeprefix, strchr(line + i, '"') + 1, 20);
                  if(*(regmeprefix + strlen(regmeprefix) - 1) == '"')
                     *(regmeprefix + strlen(regmeprefix) - 1) = '\0'; 
		}
	     /* Default prefix that is added to nicknames of users using !protectme command */
	     else if(strncmp(line + i, "protectmeprefix", 15) == 0)
	       {
                  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                  strncpy(protectmeprefix, strchr(line + i, '"') + 1, 20);
                  if(*(protectmeprefix + strlen(protectmeprefix) - 1) == '"')
                     *(protectmeprefix + strlen(protectmeprefix) - 1) = '\0'; 
		  }
             /* Minimum class allowed to see descriptions/TAGs/emails of all users */
	     else if(strncmp(line + i, "hidetags", 8) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                 hidetags = atoi(line + i);
               }
	     /* If set to 1, prentends that all users on the hub have no sharesize */
	     else if(strncmp(line + i, "hide_allshare", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  hide_allshare = atoi(line + i);
	       }
             /* If set to 0, doesn't send original user's description in $MyINFO */
	     else if(strncmp(line + i, "show_original_desc", 18) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  show_original_desc = atoi(line + i);
               }
	     /* If set to 0, adds [user's class name] to description */	     
	     else if(strncmp(line + i, "show_class_in_desc", 18) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  show_class_in_desc = atoi(line + i);
               }
	     /* If set to 1, adds [A], [P] or [PROXY] to user's description due to mode M:A(P,5) */	     
	     else if(strncmp(line + i, "show_mode_in_desc", 17) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  show_mode_in_desc = atoi(line + i);
               }
             /* If set to 0, doesn't send original user's email in $MyINFO */
	     else if(strncmp(line + i, "show_original_mail", 18) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  show_original_mail = atoi(line + i);
               }
	     /* Muting all unregistered users: 1-from chatting,2-from downloading/searching,3-from all */
	     else if(strncmp(line + i, "mute_unregistered", 17) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  mute_unregistered = atoi(line + i);
	       }
	     /* Restrictions for searching/downloading: 1-user can s/d from user with equal/lower class,2-unregistered cannot s/d from registered */
	     else if(strncmp(line + i, "restrictions", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  restrictions = atoi(line + i);
	       }
	     /* 1 if user should be redirected, 0 if only kicked */
	     else if(strncmp(line + i, "punishment", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  punishment = atoi(line + i);
	       }
	     /* Number of minutes user should be banned for when kicked */
	     else if(strncmp(line + i, "kick_bantime", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  kick_bantime = atoi(line + i);
	       }
	     /* Number of minutes user should be banned for when kicked while TAG checking */
	     else if(strncmp(line + i, "tempban", 7) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  tempban = atoi(line + i);
	       }
	     /* Maximum number of allowed warnings for user */
	     else if(strncmp(line + i, "max_warnings", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  max_warnings = atoi(line + i);
	       }
    	     /* If set to 1, chat-gagged users will not get main chat messages at all */ 
	     else if(strncmp(line + i, "no_chat_for_chatgagged", 22) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  no_chat_for_chatgagged = atoi(line + i);
	       }
	      /* 1 for ban to override allow */
	     else if(strncmp(line + i, "ban_overrides_allow", 19) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  ban_overrides_allow = atoi(line + i);
	       }
	     /* Maximum allowed number of REGULAR users connected from the same IP */
	     else if(strncmp(line + i, "max_regular_from_one_ip", 23) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  max_regular_from_one_ip = atoi(line + i);
	       }
	     /* Minimum lenght of search string */
	     else if(strncmp(line + i, "min_search_len", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  min_search_len = atoi(line + i);
	       }
	     /* Minimum number of seconds between searches */
	     else if(strncmp(line + i, "searchspam_time", 15) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  searchspam_time = atoi(line + i);
	       }
	     /* If 1, filters messages from REGULAR for occurance of http://, www or IP string and reporting to ops if found */
	     else if(strncmp(line + i, "deny_main_http", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  deny_main_http = atoi(line + i);
	       }
	     /* If 1, kicks REGULAR for http:// pm string, if 2 bans them for the 5th pm message with http:// */
	     else if(strncmp(line + i, "deny_pm_http", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  deny_pm_http = atoi(line + i);
	       }
	      /* Type of IP gag: 0-chat(default),1-download and search,2-all */
	      else if(strncmp(line + i, "ipgag_type", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  ipgag_type = atoi(line + i);
	       }
	     /* List of forbidden words of phrases delimited with ';' */
	     else if(strncmp(line + i, "banwords", 8) == 0)
	       {
		  /* The string has to begin with a '"' at the same line */
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  if((banwords = malloc(sizeof(char) 
		       * (strlen(line+i+1) + 1))) == NULL)
		    {
		       logprintf(0, "Error - In read_config()/malloc(): %s", strerror(errno));
		       quit = 1;
		       set_lock(fd, F_UNLCK);
		       fclose(fp);
		       return -1;
		    }
		  strcpy(banwords, strchr(line + i, '"') + 1);
		  while((line[strlen(line) - 1] != '"') && (fgets(line, 1023, fp) != NULL))
		    {		
		       trim_string(line);
		       if((banwords = realloc(banwords, sizeof(char) 
			* (strlen(banwords) + strlen(line) + 3))) == NULL)
			 {
			    logprintf(0, "Error - In read_config()/realloc(): %s", strerror(errno));
			    quit = 1;
			    set_lock(fd, F_UNLCK);
			    fclose(fp);
			    return -1;
			 }
		       sprintfa(banwords, "\r\n%s", line);
		    }
		  if(*(banwords + strlen(banwords) - 1) == '"')
		     *(banwords + strlen(banwords) - 1) = '\0';
	       }
	     /* On spam: 0-do nothing, 1-kick user, 2-ban user */ 
	     else if(strncmp(line + i, "spam_action", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  spam_action = atoi(line + i);
	       }
	     /* On flood: 0-do nothing, 1-kick user, 2-ban user */
	     else if(strncmp(line + i, "flood_action", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  flood_action = atoi(line + i);
	       }
	     /* Number of lines when user is marked as a flooder */
	     else if(strncmp(line + i, "flood_lines", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  flood_lines = atoi(line + i);
	       }
	     /* If set to 1, REGULAR users will get kick for 'slot' word written to someone's priv */
	     else if(strncmp(line + i, "slotbeg_kicker", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  slotbeg_kicker = atoi(line + i);
	       }	     
	     /* If set to 1, will seek for incomplete files in active users' shared datas */
	     else if(strncmp(line + i, "kick_for_incomplete", 19) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  kick_for_incomplete = atoi(line + i);
	       }
	     /* Minimum class that will be stored on the seenlist */
	     else if(strncmp(line + i, "seen_class", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  seen_class = atoi(line + i);
	       }
	    /* Maximum number or characters that can be send to main chat by user */
	     else if(strncmp(line + i, "cut_string_to", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		 cut_string_to = atoi(line + i);
	       }
	     /* If set to 1, too long main chat messages from registered users will also be cutted */
	     else if(strncmp(line + i, "cut_reg_msg", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		 cut_reg_msg = atoi(line + i);
	       }
	     /* Minimum class allowed to use !msg command */
	     else if(strncmp(line + i, "offline_msgs", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  offline_msgs = atoi(line + i);
	       }
	     /* If set to 1, REGULAR user can register himself with !regme <password> command */
	     else if(strncmp(line + i, "self_registration", 17) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  self_registration = atoi(line + i);
	       }
	     /* Minimum share for users who wanna register themselves with !regme <password> command */
             else if(strncmp(line + i, "min_regme_share", 15) == 0)
               {
                  while(!isdigit((int)line[i]))
                    i++;
                  min_regme_share = strtoll(line + i, (char **)NULL, 10);
               }
	     /* If set to 1, self-temporary-protection is enabled (!protectme) */
	     else if(strncmp(line + i, "protection", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  protection = atoi(line + i);
	       }
	     /* Nubmer of protection days for users who used !protectme command */
	     else if(strncmp(line + i, "prottime", 8) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  prottime = (unsigned int)(atoi(line + i));
	       }
	     /* Bot's name */
             else if(strncmp(line + i, "botnick", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                         while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return -1;
                    }
                  strncpy(botnick, strchr(line + i, '"') + 1, 50);
                  if(*(botnick + strlen(botnick) - 1) == '"')
                    *(botnick + strlen(botnick) - 1) = '\0';
               }
             /* Op chat's name */
	     else if(strncmp(line + i, "opchatnick", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                         while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return -1;
                    }
                  strncpy(opchatnick, strchr(line + i, '"') + 1, 50);
                  if(*(opchatnick + strlen(opchatnick) - 1) == '"')
                    *(opchatnick + strlen(opchatnick) - 1) = '\0';
               }
	     /* Clock-bot with current server time: 0-disabled, 1-enabled */
	     else if(strncmp(line + i, "clockbot", 8) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  clockbot = (unsigned int)(atoi(line + i));
	       }
	     /* Counting bots as users: 0-disabled, 1-enabled */
	     else if(strncmp(line + i, "count_bots", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  count_bots = (unsigned int)(atoi(line + i));
	       }
	     /* Hiding share of opped users 0-off,1-from all users,2-from REGULAR users */
	     else if(strncmp(line + i, "hide_opshare", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  hide_opshare = atoi(line + i);
	       }
	      /* Minimum class allowed to add new releases */
	      else if(strncmp(line + i, "who_can_addrls", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  who_can_addrls  = atoi(line + i);
	       }
	      /* Minimum class allowed to see new releases */
	      else if(strncmp(line + i, "who_can_seerls", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  who_can_seerls  = atoi(line + i);
	       }
	     /* Number of last releases displayed in MOTD */
	     else if(strncmp(line + i, "max_releases_to_show", 20) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  max_releases_to_show = atoi(line + i);
	       }
              /* Minimum class allowed to add new requests */
	      else if(strncmp(line + i, "who_can_addreq", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  who_can_addreq  = atoi(line + i);
	       }
	      /* Minimum class allowed to see new releases */
	      else if(strncmp(line + i, "who_can_seereq", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  who_can_seereq  = atoi(line + i);
	       }
	     /* Number of last requests displayed in MOTD */
	     else if(strncmp(line + i, "max_requests_to_show", 20) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  max_requests_to_show = atoi(line + i);
	       }
	     /* Number of days after old entries form reglist file are deleted (0-never) */	     
	     else if(strncmp(line + i, "expiredtime", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  expiredtime = (unsigned int)(atoi(line + i));
	       }
	     /* Number of days after old entries from releaselist file are deleted (0-never) */	     
	     else if(strncmp(line + i, "expiredrls", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  expiredrls = (unsigned int)(atoi(line + i));
	       }
	     /* Number of days after old entries from reqlist file are deleted (0-never) */	     
	     else if(strncmp(line + i, "expiredreq", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  expiredreq = (unsigned int)(atoi(line + i));
		}
	     /* Number of days after old entries from seen file are deleted */	     
	     else if(strncmp(line + i, "expiredseen", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  expiredseen = (unsigned int)(atoi(line + i));
	       }
	     /* Number of days after old entries from urlist file are deleted (0-never) */	     
	     else if(strncmp(line + i, "expiredurl", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  expiredurl = (unsigned int)(atoi(line + i));
	       }
	     /* Number of days after old entries form warnings file are deleted (0-never) */	     
	     else if(strncmp(line + i, "expiredwarn", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  expiredwarn = (unsigned int)(atoi(line + i));
	       }
	     /* Period of sending autofavmsg to unregistered users: 0-off, else every value*15 minutes */
	     else if(strncmp(line + i, "autofav", 7) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		 autofav = atoi(line + i);
	       }
	     /* Period of sending scheduledmsg to all users: 0-off, else every value*15 minutes */
	     else if(strncmp(line + i, "send_scheduled", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		 send_scheduled = atoi(line + i);
	       }
	     /* Enable encrypted passwords? 1 is on, 0 is off */
	     else if(strncmp(line + i, "crypt_enable", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		 crypt_enable = atoi(line + i);
	       }
	     /* Main chat logging, 1 is on, 0 is off */
	     else if(strncmp(line + i, "log_main_chat", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  log_main_chat = atoi(line + i);
	       }
	     /* This sets the verbosity of the log file, may vary from 0 to 5 */
	     else if(strncmp(line + i, "verbosity", 9) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  verbosity = atoi(line + i);
	       }
	     /* If set to 1, !config is displayed as a private message from botnick */
	     else if(strncmp(line + i, "config_on_priv", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  config_on_priv = atoi(line + i);
	       }
	     /* If set to 1, !help is displayed as a private message from botnick */
	     else if(strncmp(line + i, "help_on_priv", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  help_on_priv = atoi(line + i);
	       }
	     /* If set to 1, !rules is displayed as a private message from botnick */ 
	     else if(strncmp(line + i, "rules_on_priv", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  rules_on_priv = atoi(line + i);
	       }
	     /* If set to 1, !regme is displayed as a private message from botnick */
	     else if(strncmp(line + i, "regme_on_priv", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  regme_on_priv = atoi(line + i);
	       }
	     /* If set to 1, !geturl is displayed as a private message from botnick */
	     else if(strncmp(line + i, "url_on_priv", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  url_on_priv = atoi(line + i);
	       }
	     /* If set to 1, !getrls is displayed as a private message from botnick */
	     else if(strncmp(line + i, "rls_on_priv", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  rls_on_priv = atoi(line + i);
	       }
	     /* If set to 1, !getreq is displayed as a private message from botnick */
	     else if(strncmp(line + i, "req_on_priv", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  req_on_priv = atoi(line + i);
	       }
	      /* Show Hi! welcome message with user's IP in MOTD: 0-off,1-on */
	      else if(strncmp(line + i, "show_himsg", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_himsg = atoi(line + i);
	       }
	      /* Show info about basic hub commands in MOTD: 0-off,1-on */
	      else if(strncmp(line + i, "show_onjoinmsgs", 15) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_onjoinmsgs = atoi(line + i);
	       }
	      /* Show hub records in MOTD and info if they're broken: 0-off,1-on */
	      else if(strncmp(line + i, "show_records", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_records = atoi(line + i);
    	       }
	      /* Show hub uptime in MOTD: 0-off,1-on */
	      else if(strncmp(line + i, "show_uptime", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_uptime = atoi(line + i);
	       }
	      /* Show 10 last main chat messages in MOTD: 0-off,1-on */
	      else if(strncmp(line + i, "show_lastmsgs", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_lastmsgs = atoi(line + i);
	       }
              /* Show last added releases in MOTD: 0-off,1-on */
	      else if(strncmp(line + i, "show_releases", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_releases = atoi(line + i);
	       }
	      /* Show last added request in MOTD: 0-off,1-on */
	      else if(strncmp(line + i, "show_requests", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_requests = atoi(line + i);
	       }
              /* Show welcomes of REG and higher classes to main chat: 0-off,1-on,2-on with random welcomes */
	      else if(strncmp(line + i, "show_welcomes", 13) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_welcomes = atoi(line + i);
	       }
	      /* Showing joins/parts of users: 0-off,1-main chat for admins/owners,2-pm to admins/owners,2-pm to admins/owners,3-main chat for all */
	      else if(strncmp(line + i, "show_joinsparts", 15) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_joinsparts = atoi(line + i);
	       }
	      /* Showing hub kicks/bans of users: 0-off,1-main chat for admins/owners,2-pm to admins/owners,3-main chat for all */
	      else if(strncmp(line + i, "show_kicksbans", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_kicksbans = atoi(line + i);
	       }
	      /* Showing info about connection attempt of banned user: 0-off,1-main chat for admins/owners,2-pm to admins/owners,3-main chat for all */
	      else if(strncmp(line + i, "show_denied", 11) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_denied = atoi(line + i);
	       }
    	      /* Notify ops/cheefs/admins/owners on some actions: 0-off, 1-main chat,2-pm */
	      else if(strncmp(line + i, "show_actions", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_actions = atoi(line + i);
	       }
	      /* Notify admins/owners on some events: 0-off, 1-main chat,2-pm */
	      else if(strncmp(line + i, "show_notifications", 18) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_notifications = atoi(line + i);
	       }
	      /* Notify admins/owners on !help/!rules!regme usage: 0-off, 1-on */
	      else if(strncmp(line + i, "show_helpcommand_usage", 22) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  show_helpcommand_usage = atoi(line + i);
	       }
	     /* If set to 1, no strings started with ! and + will be displayed in the main chat */
	     else if(strncmp(line + i, "extra_commands", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  extra_commands = atoi(line + i);
	       }
	     /* Pointer containing names of command-files created by user */	     
             else if(strncmp(line + i, "extra_files", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                         while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return -1;
                    }
                  strncpy(extra_files, strchr(line + i, '"') + 1, 500);
                  if(*(extra_files + strlen(extra_files) - 1) == '"')
                    *(extra_files + strlen(extra_files) - 1) = '\0';
               }
	     /* Redirect host on kick/ban */
	     else if(strncmp(line + i, "redirect_host_kickban", 21) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       redirect_host_kickban[0] = '\0';
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return 1;
		    }
		  strncpy(redirect_host_kickban, strchr(line + i, '"') + 1, 121);
		  if(*(redirect_host_kickban + strlen(redirect_host_kickban) - 1) == '"')
		    *(redirect_host_kickban + strlen(redirect_host_kickban) - 1) = '\0';
	       }
             /* Redirect host on not enough share */
	     else if(strncmp(line + i, "redirect_host_minshare", 22) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_minshare[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_minshare, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_minshare + strlen(redirect_host_minshare) - 1) == '"')
                    *(redirect_host_minshare + strlen(redirect_host_minshare) - 1) = '\0';
               }
	     /* Redirect host on too large share */
	     else if(strncmp(line + i, "redirect_host_maxshare", 22) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_maxshare[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_maxshare, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_maxshare + strlen(redirect_host_maxshare) - 1) == '"')
                    *(redirect_host_maxshare + strlen(redirect_host_maxshare) - 1) = '\0';
               }
	     /* Redirect host on too many opened hubs */
	     else if(strncmp(line + i, "redirect_host_maxhubs", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_maxhubs[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_maxhubs, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_maxhubs + strlen(redirect_host_maxhubs) - 1) == '"')
                    *(redirect_host_maxhubs + strlen(redirect_host_maxhubs) - 1) = '\0';
               }
	     /* Redirect host on not enough slots */
	     else if(strncmp(line + i, "redirect_host_minslots", 22) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_minslots[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_minslots, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_minslots + strlen(redirect_host_minslots) - 1) == '"')
                    *(redirect_host_minslots + strlen(redirect_host_minslots) - 1) = '\0';
               }
	     /* Redirect host on too many slots */
	     else if(strncmp(line + i, "redirect_host_maxslots", 22) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_maxslots[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_maxslots, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_maxslots + strlen(redirect_host_maxslots) - 1) == '"')
                    *(redirect_host_maxslots + strlen(redirect_host_maxslots) - 1) = '\0';
               }
	     /* Redirect host on too low/high ratio */
	     else if(strncmp(line + i, "redirect_host_ratio", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_ratio[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_ratio, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_ratio + strlen(redirect_host_ratio) - 1) == '"')
                    *(redirect_host_ratio + strlen(redirect_host_ratio) - 1) = '\0';
               }
	     /* Redirect host on forbidden client */
	     else if(strncmp(line + i, "redirect_host_client", 20) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_client[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_client, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_client + strlen(redirect_host_client) - 1) == '"')
                    *(redirect_host_client + strlen(redirect_host_client) - 1) = '\0';
               }
	     /* Redirect host on upload limit */
	     else if(strncmp(line + i, "redirect_host_connlimit", 22) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_connlimit[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_connlimit, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_connlimit + strlen(redirect_host_connlimit) - 1) == '"')
                    *(redirect_host_connlimit + strlen(redirect_host_connlimit) - 1) = '\0';
               }
	     /* Redirect host when hub is full */
	     else if(strncmp(line + i, "redirect_host_hubfull", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_hubfull[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_hubfull, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_hubfull + strlen(redirect_host_hubfull) - 1) == '"')
                    *(redirect_host_hubfull + strlen(redirect_host_hubfull) - 1) = '\0';
               }
	     /* Redirect host when hub is registered only */
	     else if(strncmp(line + i, "redirect_host_regonly", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       redirect_host_regonly[0] = '\0';
                       set_lock(fd, F_UNLCK);

                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return 1;
                    }
                  strncpy(redirect_host_regonly, strchr(line + i, '"') + 1, 121);
                  if(*(redirect_host_regonly + strlen(redirect_host_regonly) - 1) == '"')
                    *(redirect_host_regonly + strlen(redirect_host_regonly) - 1) = '\0';
               }
	     /* Ban message */
	     else if(strncmp(line + i, "banmsg", 6) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(banmsg, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(banmsg + strlen(banmsg) - 1) == '"')
                     *(banmsg + strlen(banmsg) - 1) = '\0';
		}
	     /* The message displayed if hub is full */
	     else if(strncmp(line + i, "hub_full_msg", 12) == 0)
	       {
		  /* The string has to begin with a '"' at the same line */
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  if (hub_full_msg) free(hub_full_msg);
		  if((hub_full_msg = malloc(sizeof(char) 
		       * (strlen(line+i+1) + 1))) == NULL)
		    {
		       logprintf(0, "Error - In read_config()/malloc(): %s", strerror(errno));
		       quit = 1;
		       set_lock(fd, F_UNLCK);
		       fclose(fp);
		       return -1;
		    }
		  strcpy(hub_full_msg, strchr(line + i, '"') + 1);
		  while((line[strlen(line) - 1] != '"') && (fgets(line, 1023, fp) != NULL))
		    {		
		       trim_string(line);
		       if((hub_full_msg = realloc(hub_full_msg, sizeof(char) 
			* (strlen(hub_full_msg) + strlen(line) + 3))) == NULL)
			 {
			    logprintf(0, "Error - In read_config()/realloc(): %s", strerror(errno));
			    quit = 1;
			    set_lock(fd, F_UNLCK);
			    fclose(fp);
			    return -1;
			 }
		       sprintfa(hub_full_msg, "\r\n%s", line);
		    }
		  if(*(hub_full_msg + strlen(hub_full_msg) - 1) == '"')
		     *(hub_full_msg + strlen(hub_full_msg) - 1) = '\0';
	       }
             /* RegOnly message */
	     else if(strncmp(line + i, "regonlymsg", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");

                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
                            return -1;
                         }

                       return -1;
                    }
                   strncpy(regonlymsg, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(regonlymsg + strlen(regonlymsg) - 1) == '"')
                     *(regonlymsg + strlen(regonlymsg) - 1) = '\0';
               }
	     /* Actual hub topic */
	     else if(strncmp(line + i, "topic", 5) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  strncpy(topic, strchr(line + i, '"') + 1, MAX_TOPIC_LEN);
		  if(*(topic + strlen(topic) - 1) == '"')
		    *(topic + strlen(topic) - 1) = '\0';
	       }
	     /* Welcome message for REG class users */
	     else if(strncmp(line + i, "welcomereg", 10) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(welcomereg, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(welcomereg + strlen(welcomereg) - 1) == '"')
                     *(welcomereg + strlen(welcomereg) - 1) = '\0';
		}
	     /* Welcome message for VIP class users */	     
	     else if(strncmp(line + i, "welcomevip", 10) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(welcomevip, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(welcomevip + strlen(welcomevip) - 1) == '"')
                     *(welcomevip + strlen(welcomevip) - 1) = '\0';
		}
	     /* Welcome message for KVIP class users */
	     else if(strncmp(line + i, "welcomekvip", 11) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(welcomekvip, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(welcomekvip + strlen(welcomekvip) - 1) == '"')
                     *(welcomekvip + strlen(welcomekvip) - 1) = '\0';
		}
	     /* Welcome message for OP class users */
	     else if(strncmp(line + i, "welcomeop", 9) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(welcomeop, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(welcomeop + strlen(welcomeop) - 1) == '"')
                     *(welcomeop + strlen(welcomeop) - 1) = '\0';
		}
	     /* Welcome message for CHEEF class users */
	     else if(strncmp(line + i, "welcomecheef", 12) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(welcomecheef, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(welcomecheef + strlen(welcomecheef) - 1) == '"')
                     *(welcomecheef + strlen(welcomecheef) - 1) = '\0';
		}
	     /* Welcome message for ADMIN/OWNER class users */
	     else if(strncmp(line + i, "welcomeadmin", 12) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(welcomeadmin, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(welcomeadmin + strlen(welcomeadmin) - 1) == '"')
                     *(welcomeadmin + strlen(welcomeadmin) - 1) = '\0';
		}
	     /* Information about registered user's quit send to main chat */
	     else if(strncmp(line + i, "partmsg", 7) == 0)
	       {
		  if(strchr(line + i, '"') == NULL)
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
                   strncpy(partmsg, strchr(line + i, '"') + 1, MAX_MSG_LEN);
                   if(*(partmsg + strlen(partmsg) - 1) == '"')
                     *(partmsg + strlen(partmsg) - 1) = '\0';
		}
	     /*  !block/!unblock main */
	      else if(strncmp(line + i, "block_main", 10) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  block_main = atoi(line + i);
	       }
	     /*  !block/!unblock private messaging */
	      else if(strncmp(line + i, "block_pm", 8) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  block_pm = atoi(line + i);
	       }
	     /*  !block/!unblock downloading */
	      else if(strncmp(line + i, "block_download", 14) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  block_download = atoi(line + i);
	       }
	     /*  !block/!unblock searching */
	      else if(strncmp(line + i, "block_search", 12) == 0)
	       {
		  while(!isdigit((int)line[i]))
		    i++;
		  block_search = atoi(line + i);
	       }
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In read_config()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In read_config()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 1;
}

void create_files(void)
{
   FILE *fp;
   int fd;
   int erret;
   char path[MAX_FDP_LEN+1];
   int i = 0;
   char owner[51];
   char ow_pass[51];
   char bstring[201];
					
while (i < 27)
{
skipped: 
i++;
if(i > 26)
return;
if(i == 1)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BAN_FILE);
else if(i == 2)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WARN_FILE);
else if(i == 3)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CATEG_FILE);
else if(i == 4)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SETTOPIC);
else if(i == 5)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WELCOME_FILE);
else if(i == 6)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REGME_FILE);
else if(i == 7)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RECORDS_FILE);
else if(i == 8)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RULES_FILE);
else if(i == 9)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AUTOFAV_FILE);
else if(i == 10)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAG_FILE);
else if(i == 11)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
else if(i == 12)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
else if(i == 14)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SEEN_FILE);
else if(i == 16)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AWAYS);
else if(i == 17)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AUTOAWAYS);
else if(i == 18)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBAN_FILE);
else if(i == 19)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, ALLOW_FILE);
else if(i == 20)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, URL_FILE);
else if(i == 21)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);
else if(i == 13)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, PROT_FILE);
else if(i == 23)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);
else if(i == 22)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, HIDE_FILE);
else if(i == 15)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);
else if(i == 24)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MSGS_FILE);
else if(i == 25)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAGRS_FILE);
else if(i == 26)
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SCHEDULE_FILE);
  
    while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In create_files()/open(): Interrupted system call. Trying again.\n"); 
   
   if(fd >= 0)
     {
	close(fd);
     goto skipped;
     }
   
   while(((fd = open(path, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In create_files()/open(): Interrupted system call. Trying again.\n");
   
   if(fd < 0)
     {
	logprintf(0, "Error - In create_files()/open(): %s", strerror(errno));
	return;
     }
   
   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {
	logprintf(0, "Error - In create_files(): Couldn't set lock\n");
	close(fd);
	return;
     }
   
   if((fp = fdopen(fd, "a")) == NULL)
     {
	logprintf(0, "Error - In create_files()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return;
     }


   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In create_files()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In create_files()/fclose(): %s", strerror(errno));
     }
if (i == 3) 
{
    snprintf(bstring, 200, "A AUDIO");
    add_line_to_file(bstring, path);
    snprintf(bstring, 200, "D DVDR");
    add_line_to_file(bstring, path);
    snprintf(bstring, 200, "G GAMES");
    add_line_to_file(bstring, path);
    snprintf(bstring, 200, "O OTHER");
    add_line_to_file(bstring, path);
    snprintf(bstring, 200, "V VIDEO");
    add_line_to_file(bstring, path);
    snprintf(bstring, 200, "X XXX");
    add_line_to_file(bstring, path);
}
if(i == 6)
{
    snprintf(bstring, 200, "(Here should be written registration rules for this hub, but if you see this message it means that hub owner didn't edit this file yet so ask any operator about registration :)");
    add_line_to_file(bstring, path);
}
if(i == 8)
{
    snprintf(bstring, 200, "(Here should be written rules for this hub, but if you see this message it means that hub owner didn't edit this file yet :)");
    add_line_to_file(bstring, path);
}
if(i == 9)
{
    snprintf(bstring, 200, "TYPE /fav on main chat to add this hub to FAVOURITES!!!");
    add_line_to_file(bstring, path);
}
if(i == 23)
{
loadfile2buf(&reglist_mem,REG_FILE);
printf("Enter owner nickname: ");
scanf("%50s", owner);
printf("Enter owner password: ");
scanf("%50s", ow_pass);
printf("Created owner account for nickname: %s. Password is: %s \n\n", owner, ow_pass);

sprintf(bstring, "$AddRegUser %s ", owner);
sprintfa(bstring, "%s 7|", ow_pass);

   add_reg_user(bstring); 
}
}
}

void create_dailylog(char *numb)
{
   FILE *fp;
   int fd;
   int erret;
   char path[MAX_FDP_LEN+10];
   snprintf(path, MAX_FDP_LEN+10, "%s/%s/%s", config_dir, LOG_DIR, numb);
   
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In create_dailylog()/open(): Interrupted system call. Trying again.\n"); 
   
   if(fd >= 0)
     {
	close(fd);
	return;
     }
   
   while(((fd = open(path, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In create_dailylog()/open(): Interrupted system call. Trying again.\n");
   
   if(fd < 0)
	return;
   
   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {
	close(fd);
	return;
     }
   
   if((fp = fdopen(fd, "a")) == NULL)
     {
	set_lock(fd, F_UNLCK);
	close(fd);
	return;
     }
   
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In create_dailylog()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In create_dailylog()/fclose(): %s", strerror(errno));
     }
}

/* Returns 1 if user is on the banlist.  */
void check_if_saw(char *buf, struct user_t *user)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   time_t seenexp;
   char line[1024];
   char ban_host[51];
   time_t now_time;
int matches = 0;
struct user_t *seenus; 
char sstr[53];
char snick[51]; 
char sdayn[5];
char sday[3];
char sdate[10];
char syear[5];
char smont[5];

	snprintf(snick, 50, "%s", "");
	sscanf(buf, "%50[^|]|", snick); 
	if(strlen(snick) < 3) 
	{ 
	uprintf(user, "<%s> %s: 3|", botnick, lp_charnumbers);  
	return; 
	}        
	if((seenus = get_human_user(snick)) != NULL) 
	{ 
	uprintf(user, "<%s> %s %s|", botnick, snick, lp_isonlinenow);  
	return;
	} 
       
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SEEN_FILE);	

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_saw()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	set_lock(fd, F_UNLCK);
	close(fd);
	return;
     }
   now_time = time(NULL);
	uprintf(user, "<%s> %s [ %s ]:\r\n", botnick, lp_searchresults, snick);  
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
	     
	     
sscanf(line+i, "%lu %120s %50s %5s %5s %3s %10s %5s", &seenexp, ban_host, seenhost, sdayn, smont, sday, sdate, syear);
snprintf(s_time, 119, "%s, %s %s %s, %s", sdayn, sday, smont, syear, sdate);
snprintf(sstr, 51, " %s ", ban_host);
		  if((search_in_string(sstr, snick) != 0))
		    {
	matches++;
	uprintf(user, "%s (IP: %s) %s %s @ %s\r\n", ban_host, seenhost, lp_hasparted, hub_name, s_time);  
	       }
	  }
     }
   set_lock(fd, F_UNLCK);
	uprintf(user, "[ %d %s ]|", matches, lp_users);     
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_saw()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;

}

int check_if_record(char *user_nick)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char ban_host[51];
   time_t now_time;
char sdayn[5];
char sday[3];
char sdate[10];
char syear[5];
char smont[5];

       
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RECORDS_FILE);	

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_record()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return -1;	

   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }
   now_time = time(NULL);
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
	     
	     
sscanf(line+i, "%120s %50s %5s %5s %3s %10s %5s", ban_host, seenhost, sdayn, smont, sday, sdate, syear);
snprintf(ss_time, 119, "%s, %s %s %s, %s", sdayn, sday, smont, syear, sdate);
		  /* Check if a nickname is banned.  */
		  if((match_with_wildcards(user_nick, ban_host) != 0))
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_record()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			    return -1;

if(strcmp(user_nick, "share") == 0)
{
snprintf(recordshstr, 119, "%s %s %s %s %s, %s", seenhost, sdayn, sday, smont, syear, sdate);
sscanf(seenhost, "%llu", &recordshare);
}
else if(strcmp(user_nick, "users") == 0)
{
snprintf(recordusstr, 119, "%s %s %s %s %s, %s", seenhost, sdayn, sday, smont, syear, sdate);
sscanf(seenhost, "%d", &recordusers);
}
		       return 1;
	       }
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_record()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_record()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}


void check_ipinfo(char *buf, struct user_t *user)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   time_t seenexp;
   char line[1024];
   char ban_host[51];
   time_t now_time;
int matches = 0;
char sdayn[5];
char sday[3];
char sdate[10];
char syear[5];
char ip_time[120];
char smont[5];
char sip[101];
char shost[51];
struct sock_t *human_user;
struct sock_t *next_human;
		

sscanf(buf, "%50[^|]|", sip);

snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SEEN_FILE);	

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_ipinfo()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_ipinfo()/open(): %s", strerror(errno));
	return;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_ipinfo(): Couldn't set file lock\n");
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_ipinfo()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return;
     }
   now_time = time(NULL);
uprintf(user, "<%s> %s [ %s ]:\r\n", botnick, lp_searchforip, sip);
human_user = human_sock_list;
while(human_user != NULL)
{
next_human = human_user->next;
if(match_with_wildcards(human_user->user->hostname, sip) != 0)
{
matches++;
uprintf(user, "%s (IP: %s), %s\r\n", human_user->user->nick, ip_to_string(human_user->user->ip), lp_loggedin);                
}
human_user = next_human;
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
	     
	     
sscanf(line+i, "%lu %120s %50s %5s %5s %3s %10s %5s", &seenexp, ban_host, shost, sdayn, smont, sday, sdate, syear);
snprintf(ip_time, 119, "%s, %s %s %s, %s", sdayn, sday, smont, syear, sdate);
		  if((match_with_wildcards(shost, sip) != 0))
		    {
	       matches++;
		uprintf(user, "%s (IP: %s) %s %s @ %s\r\n", ban_host, shost, lp_hasparted, hub_name, ip_time);  
	       }
	  
	  }
     }
   set_lock(fd, F_UNLCK);
uprintf(user, "[ %d %s ]|", matches, lp_users);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_ipinfo()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_ipinfo()/fclose(): %s", strerror(errno));
	return;
     }
return;   
}

int check_if_away(char *user_nick)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char awayuser[51];
   time_t now_time;

       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AWAYS);	
   if(mystristr(away_mem, user_nick) == NULL)
   return 0;

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_away()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_if_away()/open(): %s", strerror(errno));
	return -1;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_if_away(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_if_away()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }
   now_time = time(NULL);
   while(fgets(line, 1023, fp) != NULL)
     {
	trim_string(line);
	now_time = 0;
	j = strlen(line);
	if(j != 0)
	  {
	     /* Jump to next char which isn't a space */
	     i = 0;
	     while(line[i] == ' ')
	       i++;
	     
	     sscanf(line+i, "%50s %lu %200[^|]", awayuser, &now_time, away_reason);
		  /* Check if a nickname is banned.  */
		  if((match_with_wildcards(user_nick, awayuser) != 0))
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_away()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_if_away()/fclose(): %s", strerror(errno));
			    return -1;
			 }
			time_to_unban = now_time;
		       return 1;
	       }
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_away()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_away()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

int check_if_autoaway(char *user_nick)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char awayuser[51];
   char awayrs[151];
   struct user_t *aauser;
   int awaynmb;

   if(mystristr(autoaway_mem, user_nick) == NULL)
   return 0;

       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AUTOAWAYS);	
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_autoaway()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_if_autoaway()/open(): %s", strerror(errno));
	return -1;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_if_autoaway(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_if_autoaway()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     
	     sscanf(line+i, "%50s %d %150[^|]", awayuser, &awaynmb, awayrs);
		  /* Check if a nickname is banned.  */
		  if((match_with_wildcards(user_nick, awayuser) != 0))
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_autoaway()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_if_autoaway()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		        
		       if((aauser = get_human_user(user_nick)) != NULL)
		       {
		       aauser->autoawaytime = awaynmb;
		       snprintf(aauser->autoawayreason, 150, "%s", awayrs);
		       }
		       return 1;
	       }
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_autoaway()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_autoaway()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

int check_settopic(void)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
    char someintro[11];
    char sometopic[201];

       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SETTOPIC);	
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_settopic()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_settopic()/open(): %s", strerror(errno));
	return -1;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_settopic(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_settopic()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     
	     sscanf(line+i, "%10s %200[^|]", someintro, sometopic);
		  /* Check if a nickname is banned.  */
		  if((match_with_wildcards(someintro, "topic") != 0))
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_settopic()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_settopic()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		        
		snprintf(topicstring, 250, "%s", sometopic);
		       return 1;
	       }
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_settopic()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_settopic()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}




int check_if_permprotected(char *user_nick)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char awayuser[51];

       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, PROT_FILE);	
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_permprotected()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_if_permprotected()/open(): %s", strerror(errno));
	return -1;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_if_permprotected(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_if_permprotected()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     
	     snprintf(awayuser, 50, "%s", line+i);
		  /* Check if a nickname is banned.  */
		  if((match_with_wildcards(user_nick, awayuser) != 0))
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_permprotected()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_if_permprotected()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       return 1;
	       }
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_permprotected()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_permprotected()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

int check_if_categ(char *categ)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char rlslet[2];
   char line[1024];
       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CATEG_FILE);	
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_categ()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_if_categ()/open(): %s", strerror(errno));
	return -1;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_if_categ(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_if_categ()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     sscanf(line+i, "%1s ", rlslet);
	    snprintf(category, 50, "%s", line+i+strlen(rlslet)+1);
	    
	       if(strcmp(categ, rlslet) == 0)
	       return 1;
       
       


       }


     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_categ()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_categ()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

int add_reason(char *buf, int type)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char ban_host[MAX_HOST_LEN+1];
   char banxp[17];
   time_t ban_time;
   time_t now_time;
	

   if(type == BAN)
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
   else if(type == NICKBAN)
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
   else if(type == IPGAG)
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAGRS_FILE);
   
   else
	return -1;

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In add_reason()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In add_reason()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In add_reason(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In add_reason()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }
   
   now_time = time(NULL);
   
   while(fgets(line, 1023, fp) != NULL)
     {
	trim_string(line);
	ban_time = 0;
	
	j = strlen(line);
	if(j != 0)
	  {
	     /* Jump to next char which isn't a space */
	     i = 0;
	     while(line[i] == ' ')
	       i++;

	     if(type == IPGAG)
	     {
	     sscanf(line+i, "%120s", ban_host);
			    if((banreason = realloc(banreason, sizeof(char) * (strlen(line) - strlen(ban_host) + 1))) == NULL)
    			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
	     sprintf(banreason, "%s", line+i + strlen(ban_host) + 1);
	     }
	     else
	     {
	     sscanf(line+i, "%17s %120s", banxp, ban_host);
			    if((banreason = realloc(banreason, sizeof(char) * (strlen(line) - strlen(ban_host) - strlen(banxp) + 1))) == NULL)
    			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
	     sprintf(banreason, "%s", line+i + strlen(banxp) + 1 + strlen(ban_host) + 1);
	     }
	     if(strcasecmp(buf, ban_host) == 0)
		    {
	
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In add_reason()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In add_reason()/fclose(): %s", strerror(errno));
			    return -1;
			 }
				    return 1;
		    }
	       
     }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In add_reason()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In add_reason()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

/* Returns 1 if user is on the banlist.  */
int check_if_banned(struct user_t *user, int type)
{
   int i;
   int rr = 0;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char *k;
   char *temp=NULL;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char ban_host[MAX_HOST_LEN+1];
   char *string_ip = NULL;
   unsigned long userip = 0;
   unsigned long fileip = 0;
   char banxp[17];
   int byte1, byte2, byte3, byte4, mask;
   time_t ban_time;
   time_t now_time;

   now_time = time(NULL);
   ban_time = 0;    

if((type == BAN) || (type == BANRANGE)) 
{
if((string_ip = ip_to_string(user->ip)) == NULL)
return 0;
userip = ntohl(user->ip);
}

if(type == BAN)
{
   if((temp = malloc(sizeof(char) * (strlen(banlist_mem) + 1))) == NULL)
     {
	logprintf(0, "Error - In check_if_banned()/malloc(): %s", strerror(errno));
	quit = 1;
	return 0;
     }

sprintf(temp, "%s", banlist_mem);
k = temp;

while(strstr(k, "\n") != NULL)
{
if(sscanf(k, "%120s %lu", ban_host, &ban_time) == 2)
{
if((strncmp(string_ip, ban_host, strlen(ban_host)) == 0) && (strlen(ban_host) == strlen(string_ip))
&& ((ban_time == 0) || (ban_time > now_time)))
{
btime_to_unban = ban_time;
if(temp != NULL)
free(temp);
return 1;
}
}
if((k = strstr(k, "\n")) == NULL)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
{
if(strlen(k) <= 1)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
k = k + 1;
}
}
if(sscanf(k, "%120s %lu", ban_host, &ban_time) == 2)
{
if((strncmp(string_ip, ban_host, strlen(ban_host)) == 0) && (strlen(ban_host) == strlen(string_ip))
&& ((ban_time == 0) || (ban_time > now_time)))
{
btime_to_unban = ban_time;
if(temp != NULL)
free(temp);
return 1;
}
}
}

if(type == BANRANGE)
{
   if((temp = malloc(sizeof(char) * (strlen(banlist_mem) + 1))) == NULL)
     {
	logprintf(0, "Error - In check_if_banned()/malloc(): %s", strerror(errno));
	quit = 1;
	return 0;
     }

sprintf(temp, "%s", banlist_mem);
k = temp;

while(strstr(k, "\n") != NULL)
{
if(sscanf(k, "%120s %lu", ban_host, &ban_time) == 2)
{
if((sscanf(ban_host, "%d.%d.%d.%d/%d", &byte1, &byte2, &byte3, &byte4, &mask) == 5) && (mask > 0) && (mask <= 32))
{
fileip = (byte1<<24) | (byte2<<16) | (byte3<<8) | byte4;
if((((0xFFFF << (32-mask)) & userip) == ((0xFFFF << (32-mask)) & fileip)) && ((ban_time == 0) || (ban_time > now_time)))
{
btime_to_unban = ban_time;
if(temp != NULL)                                                                                             
free(temp);
return 1;
}
}
/* Else if there is a wildcard in address - Centurion */
if((strstr(ban_host, "*") != NULL) && (match_with_wildcards(string_ip, ban_host) != 0))
{
btime_to_unban = ban_time;
if(temp != NULL)
free(temp);
return 1;
}
}
if((k = strstr(k, "\n")) == NULL)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
{
if(strlen(k) <= 1)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
k = k + 1;
}
}
if(sscanf(k, "%120s %lu", ban_host, &ban_time) == 2)
{
if((sscanf(ban_host, "%d.%d.%d.%d/%d", &byte1, &byte2, &byte3, &byte4, &mask) == 5) && (mask > 0) && (mask <= 32))
{
fileip = (byte1<<24) | (byte2<<16) | (byte3<<8) | byte4;
if((((0xFFFF << (32-mask)) & userip) == ((0xFFFF << (32-mask)) & fileip)) && ((ban_time == 0) || (ban_time > now_time)))
{
btime_to_unban = ban_time;
if(temp != NULL)
free(temp);
return 1;
}
}

/* Else if there is a wildcard in address - Centurion */
if((strstr(ban_host, "*") != NULL) && (match_with_wildcards(string_ip, ban_host) != 0))
{
btime_to_unban = ban_time;
if(temp != NULL)
free(temp);
return 1;
}
}
}

if(type == NICKBAN)
{
   if((temp = malloc(sizeof(char) * (strlen(nickbanlist_mem) + 1))) == NULL)
     {
	logprintf(0, "Error - In check_if_banned()/malloc(): %s", strerror(errno));
	quit = 1;
	return 0;
     }

sprintf(temp, "%s", nickbanlist_mem);
k = temp;

while(strstr(k, "\n") != NULL)
{
if(sscanf(k, "%120s %lu", ban_host, &ban_time) == 2)
{
if(((ban_time == 0) || (ban_time > now_time)) && (match_with_wildcards(user->nick, ban_host) != 0))
{
if(temp != NULL)
free(temp);
btime_to_unban = ban_time;
return 1;
}
}
if((k = strstr(k, "\n")) == NULL)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
{
if(strlen(k) <= 1)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
k = k + 1;
}
}
if(sscanf(k, "%120s %lu", ban_host, &ban_time) == 2)
{
if(((ban_time == 0) || (ban_time > now_time)) && (match_with_wildcards(user->nick, ban_host) != 0))
{
if(temp != NULL)
free(temp);
btime_to_unban = ban_time;
return 1;
}
}
}

if((type == BAN) || (type == NICKBAN) || (type == BANRANGE))
{
if(temp != NULL)
free(temp);
return 0;
}

	if(type == BANRS)
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
   else if(type == NICKBANRS)
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
   else if(type == IPGAGRS)
	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, IPGAGRS_FILE);
   
   else
	return -1;

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_banned()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_if_banned()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_if_banned(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_if_banned()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }
   now_time = time(NULL);
   
   if((type == BANRS) || (type == IPGAGRS))
     {	
	if((string_ip = ip_to_string(user->ip)) == NULL)
	  {
	     set_lock(fd, F_UNLCK);
	     
	     while(((erret = fclose(fp)) != 0) && (errno == EINTR))
	       logprintf(0, "Error - In check_if_banned()/fclose(): Interrupted system call. Trying again.\n");
	     
	     if(erret != 0)
	       {
		  logprintf(0, "Error - In check_if_banned()/fclose(): %s", strerror(errno));
		  return -1;
	       }
   
	     return 0;
	  }
	userip = ntohl(user->ip);
     }
   
   while(fgets(line, 1023, fp) != NULL)
     {
	trim_string(line);
	ban_time = 0;
	
	j = strlen(line);
	if(j != 0)
	  {
	     /* Jump to next char which isn't a space */
	     i = 0;
	     while(line[i] == ' ')
	       i++;

	     if(type == IPGAGRS)
	     sscanf(line+i, "%120s", ban_host);
	     else
	     sscanf(line+i, "%17s %120s", banxp, ban_host);
	    
	     if((type == BANRS) || (type == IPGAGRS))
	       {		  
		  /* If not, it has to be an exact match.  */
		  if((strncmp(string_ip, ban_host, strlen(ban_host)) == 0)
			  && (strlen(ban_host) == strlen(string_ip))
			  && ((ban_time == 0) || (ban_time > now_time)))
		    {
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_banned()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_if_banned()/fclose(): %s", strerror(errno));
			    return -1;
			 }
			if(type == IPGAGRS)
			{
			    if((ipgagreason = realloc(ipgagreason, sizeof(char) * (strlen(line) - strlen(ban_host) + 1))) == NULL)
			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
			sprintf(ipgagreason, "%s", line+i + strlen(ban_host) + 1);
			}			
			else
			{
			    if((banreason = realloc(banreason, sizeof(char) * (strlen(line) - strlen(banxp) - strlen(ban_host) + 1))) == NULL)
    			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
			sprintf(banreason, "%s", line+i + strlen(banxp) + 1 + strlen(ban_host) + 1);
			}
		       return 1;
	            }
		    	    		    
		  /* Check if it's part of user's ip */
		  /* First, check if it's a subnet.  */
		  if((sscanf(ban_host, "%d.%d.%d.%d/%d", 
			     &byte1, &byte2, &byte3, &byte4, &mask) == 5)
		     && (mask > 0) && (mask <= 32))
		    {
		       fileip = (byte1<<24) | (byte2<<16) | (byte3<<8) | byte4;
		       if((((0xFFFF << (32-mask)) & userip) 
			   == ((0xFFFF << (32-mask)) & fileip))
			  && ((ban_time == 0) || (ban_time > now_time)))
			{	
			if(type == IPGAGRS)
			{
			    if((ipgagreason = realloc(ipgagreason, sizeof(char) * (strlen(line) - strlen(ban_host) + 1))) == NULL)
			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
			sprintf(ipgagreason, "%s", line+i + strlen(ban_host) + 1);
			}			
			else
			{
			    if((banreason = realloc(banreason, sizeof(char) * (strlen(line) - strlen(banxp) - strlen(ban_host) + 1))) == NULL)
    			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
			sprintf(banreason, "%s", line+i + strlen(banxp) + 1 + strlen(ban_host) + 1);
			}
		       rr = 1;
	            }
		    }
		/* Else if there is a wildcard in address - Centurion */
			if((strstr(ban_host, "*") != NULL) && (match_with_wildcards(string_ip, ban_host) != 0))
			{
			if(type == IPGAGRS)
			{
			    if((ipgagreason = realloc(ipgagreason, sizeof(char) * (strlen(line) - strlen(ban_host) + 1))) == NULL)
			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
			sprintf(ipgagreason, "%s", line+i + strlen(ban_host) + 1);
			}			
			else
			{
			    if((banreason = realloc(banreason, sizeof(char) * (strlen(line) - strlen(banxp) - strlen(ban_host) + 1))) == NULL)
    			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
			sprintf(banreason, "%s", line+i + strlen(banxp) + 1 + strlen(ban_host) + 1);
			}
			rr = 1;
			}
	       }	     
	     else if(type == NICKBANRS)
	       {
		  /* Check if a nickname is banned.  */
		  if(((ban_time == 0) || (ban_time > now_time))
		     && (match_with_wildcards(user->nick, ban_host) != 0))
		    {
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_banned()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_if_banned()/fclose(): %s", strerror(errno));
			    return -1;
			 }
			    if((banreason = realloc(banreason, sizeof(char) * (strlen(line) - strlen(banxp) - strlen(ban_host) + 1))) == NULL)
    			    {
			    logprintf(0, "Error - In main()/realloc(): %s", strerror(errno));
			    quit = 1;
			    return 0;
			    }
			sprintf(banreason, "%s", line+i + strlen(banxp) + 1 + strlen(ban_host) + 1);
		       return 1;
		    }
	       }	
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_banned()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_banned()/fclose(): %s", strerror(errno));
	return -1;
     }
    //DB// Range reasons at the end
    if(rr == 1)
    return 1;

   return 0;
}


/* Returns 1 if user is on the banlist.  */
int check_if_ipgagged(struct user_t *user)
{
   char *k;
   char *temp;
   char ban_host[MAX_HOST_LEN+1];
   char *string_ip = NULL;
   unsigned long userip = 0;
   unsigned long fileip = 0;
   int byte1, byte2, byte3, byte4, mask;

if((string_ip = ip_to_string(user->ip)) == NULL)
return 0;
userip = ntohl(user->ip);


   if((temp = malloc(sizeof(char) * (strlen(ipgaglist_mem) + 1))) == NULL)
     {
	logprintf(0, "Error - In check_if_ipgagged()/malloc(): %s", strerror(errno));
	quit = 1;
	return 0;
     }

sprintf(temp, "%s", ipgaglist_mem);
k = temp;

while(strstr(k, "\n") != NULL)
{
if(sscanf(k, "%120s", ban_host) == 1)
{
if((sscanf(ban_host, "%d.%d.%d.%d/%d", &byte1, &byte2, &byte3, &byte4, &mask) == 5) && (mask > 0) && (mask <= 32))
{
fileip = (byte1<<24) | (byte2<<16) | (byte3<<8) | byte4;
if((((0xFFFF << (32-mask)) & userip) == ((0xFFFF << (32-mask)) & fileip)))
{
if(temp != NULL)
free(temp);
return 1;
}
}
/* If not, it has to be an exact match.  */
if((strncmp(string_ip, ban_host, strlen(ban_host)) == 0) && (strlen(ban_host) == strlen(string_ip)))
{
if(temp != NULL)
free(temp);
return 1;
}
/* Else if there is a wildcard in address - Centurion */
if(match_with_wildcards(string_ip, ban_host) != 0)
{
if(temp != NULL)
free(temp);
return 1;
}
/* Check if users hostname is gagged */
if((user->hostname != NULL) && (strncmp(user->hostname, string_ip, strlen(string_ip)) != 0))
{
if(match_with_wildcards(user->hostname, ban_host) != 0)
{
if(temp != NULL)
free(temp);
return 1;
}
}		       
}
if((k = strstr(k, "\n")) == NULL)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
{
if(strlen(k) <= 1)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
k = k + 1;
}
}
if(sscanf(k, "%120s", ban_host) == 1)
{
if((sscanf(ban_host, "%d.%d.%d.%d/%d", &byte1, &byte2, &byte3, &byte4, &mask) == 5) && (mask > 0) && (mask <= 32))
{
fileip = (byte1<<24) | (byte2<<16) | (byte3<<8) | byte4;
if((((0xFFFF << (32-mask)) & userip) == ((0xFFFF << (32-mask)) & fileip)))
{
if(temp != NULL)
free(temp);
return 1;
}
}
/* If not, it has to be an exact match.  */
if((strncmp(string_ip, ban_host, strlen(ban_host)) == 0) && (strlen(ban_host) == strlen(string_ip)))
{
if(temp != NULL)
free(temp);
return 1;
}
/* Else if there is a wildcard in address - Centurion */
if(match_with_wildcards(string_ip, ban_host) != 0)
{
if(temp != NULL)
free(temp);
return 1;
}
/* Check if users hostname is gagged.  */
if((user->hostname != NULL) && (strncmp(user->hostname, string_ip, strlen(string_ip)) != 0))
{
if(match_with_wildcards(user->hostname, ban_host) != 0)
{
if(temp != NULL)
free(temp);
return 1;
}
}
}
   if(temp != NULL)
   free(temp);
   return 0;
}

int choose_welcome(void)
{
   int i;
size_t j;
int lines = 0;
int nextlines = 0;
int l = 0;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char line_welcome[251];  
   time_t allow_time;
   srand( (unsigned)time( NULL ) );   
   
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WELCOME_FILE);
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In choose_welcome()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In choose_welcome()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In choose_welcome(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In choose_welcome()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }


   
   while(fgets(line, 1023, fp) != NULL)
     {
	trim_string(line);
	lines++;
     }
    if(lines == 0)
{
   set_lock(fd, F_UNLCK);
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In choose_welcome()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In choose_welcome()/fclose(): %s", strerror(errno));
	return -1;
     }

    return 0;
}
    else
    l = 1 + rand()%lines;

   set_lock(fd, F_UNLCK);
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In choose_welcome()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In choose_welcome()/fclose(): %s", strerror(errno));
	return -1;
     }


   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In choose_welcome()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In choose_welcome()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In choose_welcome(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In choose_welcome()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }


   
   
   while(fgets(line, 1023, fp) != NULL)
     {
	trim_string(line);
	nextlines++;	
	
	j = strlen(line);
	if(j != 0)
	  {
	     /* Jump to next char which isn't a space */
	     i = 0;
	     while(line[i] == ' ')
	       i++;
	     sscanf(line+i, "%lu ", &allow_time);
	     snprintf(line_welcome, 250, "%lu", allow_time);
	     snprintf(randomwelcome, 250, "%s", line+i+strlen(line_welcome)+1);
	     /* Check if it's part of user's ip */
	     /* First, check if it's a subnet.  */
	    if(nextlines == l)
	       {
		    
		    {
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In choose_welcome()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In choose_welcome()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return 1;
		    }
	       }
	     
	  }	
     }

   set_lock(fd, F_UNLCK);
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In choose_welcome()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In choose_welcome()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

/* Returns 1 if user is on the allowlist.  */
int check_if_allowed(struct user_t *user, int type)
{
   char *k;
   char *temp;
   char allow_host[MAX_HOST_LEN+1];
   char *string_ip = NULL;
   unsigned long userip = 0;
   unsigned long fileip = 0;
   int byte1, byte2, byte3, byte4, mask;
   time_t allow_time;
   time_t now_time;

   now_time = time(NULL);
   allow_time = 0;

if((string_ip = ip_to_string(user->ip)) == NULL)
return 0;
userip = ntohl(user->ip);

   if((temp = malloc(sizeof(char) * (strlen(allowlist_mem) + 1))) == NULL)
     {
	logprintf(0, "Error - In check_if_allowed()/malloc(): %s", strerror(errno));
	quit = 1;
	return 0;
     }

sprintf(temp, "%s", allowlist_mem);
k = temp;

while(strstr(k, "\n") != NULL)
{
if(sscanf(k, "%120s %lu", allow_host, &allow_time) == 2)
{
if(type == ALLOW)
{
/* If not, it has to be an exact match.  */
if((strncmp(string_ip, allow_host, strlen(allow_host)) == 0) && (strlen(allow_host) == strlen(string_ip))
&& ((allow_time == 0) || (allow_time > now_time)))
{
if(temp != NULL)
free(temp);
return 1;
}
}
if(type == ALLOWRANGE)
{
if((sscanf(allow_host, "%d.%d.%d.%d/%d", &byte1, &byte2, &byte3, &byte4, &mask) == 5) && (mask > 0) && (mask <= 32))
{
fileip = (byte1<<24) | (byte2<<16) | (byte3<<8) | byte4;
if((((0xFFFF << (32-mask)) & userip) == ((0xFFFF << (32-mask)) & fileip)) && ((allow_time == 0) || (allow_time > now_time)))
{
if(temp != NULL)
free(temp);
return 1;
}
}
if((strstr(allow_host, "*") != NULL) && (match_with_wildcards(string_ip, allow_host) != 0))
{
if(temp != NULL)
free(temp);
return 1;
}
}
}
if((k = strstr(k, "\n")) == NULL)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
{
if(strlen(k) <= 1)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
k = k + 1;
}
}
if(type == ALLOW)
{
if((strncmp(string_ip, allow_host, strlen(allow_host)) == 0) && (strlen(allow_host) == strlen(string_ip))
&& ((allow_time == 0) || (allow_time > now_time)))
{
if(temp != NULL)
free(temp);
return 1;
}
}
if(type == ALLOWRANGE)
{
if(sscanf(k, "%120s %lu", allow_host, &allow_time) == 2)
{
if((sscanf(allow_host, "%d.%d.%d.%d/%d", &byte1, &byte2, &byte3, &byte4, &mask) == 5) && (mask > 0) && (mask <= 32))
{
fileip = (byte1<<24) | (byte2<<16) | (byte3<<8) | byte4;
if((((0xFFFF << (32-mask)) & userip) == ((0xFFFF << (32-mask)) & fileip)) && ((allow_time == 0) || (allow_time > now_time)))
{
if(temp != NULL)
free(temp);
return 1;
}
}
if(match_with_wildcards(string_ip, allow_host) != 0)
{
if(temp != NULL)
free(temp);
return 1;
}
}
}
if(temp != NULL)
free(temp);
return 0;
}

int check_if_registered(char *user_nick)
{
   int z;
   char user_nicktmp[55];
   const char *temp;
   char path[MAX_FDP_LEN+1];

   char ciruser[51];
   time_t bt;
   char cirpass[51];
   bt = 0;
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);
   //DB// Need to add extra space for exact match
   snprintf(user_nicktmp, 51, "%s ", user_nick);    

   if((temp = mystristr(reglist_mem, user_nicktmp)) != NULL)
   {
   if(strncasecmp(temp, reglist_mem, strlen(user_nicktmp)) == 0)
   {
   if(sscanf(temp, "%50s %50s %i %lu", ciruser, cirpass, &z, &bt) >= 3)
   {
   if(bt > 0)
   time_to_unban = bt; 
   
   if(z == 7)
   return 8;
   if(z == 6)
   return 7;
   if(z == 5)
   return 6;
   if(z == 4)
   return 5;
   if(z == 3)
   return 4;
   if(z == 2)
   return 3;
   if(z == 1)
   return 2;
   if(z == 0)
   return 1;
   return 0;
   }
   else
   return 0;
   }
   else
   {
   snprintf(user_nicktmp, 54, "\n%s ", user_nick);    
   if((temp = mystristr(reglist_mem, user_nicktmp)) != NULL)
   {
   if(sscanf(temp+1, "%50s %50s %i %lu", ciruser, cirpass, &z, &bt) >= 3)
   {
   if(bt > 0)
   time_to_unban = bt; 
		 
   if(z == 7)
   return 8;
   if(z == 6)
   return 7;
   if(z == 5)
   return 6;
   if(z == 4)
   return 5;
   if(z == 3)
   return 4;
   if(z == 2)
   return 3;
   if(z == 1)
   return 2;
   if(z == 0)
   return 1;
   return 0;
   }
   }
   else
   return 0;
   }
   }
   return 0;
}

/* Returns 0 if user is not on the list, 2 if user is registered, 3 if user
 * is OP, 4 if user is Op Admin and -1 if error */
int check_pass(char *buf, struct user_t *user)
{
   int i, u;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char reg_passwd[51];
   char this_passwd[51];
   char* tmp;
   time_t bt;

   u = 0;
   bt = 0;
   
   strncpy(this_passwd,buf,50);
   this_passwd[strlen(this_passwd)-1] = '\0';	
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);
   
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_pass()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_pass()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {	
	logprintf(0, "Error - In check_pass(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }      
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_pass()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     
	     if((strncasecmp(line + i, user->nick, cut_string(line + i, ' ')) == 0)
		&& ((size_t)cut_string(line + i, ' ') == strlen(user->nick)))
	       {
		  /* User is on the list */
		  if((i = cut_string(line + i, ' ')) == -1)
		    {
		       logprintf(0, "Error - In check_pass(): Erroneous line in file\n");
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_pass()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_pass()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
	    		  
		  while(line[i] == ' ')
		    i++;
		  if(line[i] == '\0')
		    {
		       logprintf(0, "Error - In check_pass(): Erroneous line in file\n");
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_pass()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_pass()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  if((j = i + cut_string(line + i, ' ')) == -1)
		    {
		       logprintf(0, "Error - In check_pass(): Erroneous line in file\n");
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_pass()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_pass()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return -1;
		    }
		  
		  /* The password check. */
		sscanf(line+i, "%50s %i %lu", reg_passwd, &u, &bt);
		
		  if(crypt_enable != 0)
		    tmp = crypt(this_passwd,reg_passwd);
		  else
		    tmp = this_passwd;

		  if(strcmp(tmp,reg_passwd) == 0) 
		    {
		       /* Users password is correct */
		    strcpy(user->password, this_passwd);
		       set_lock(fd, F_UNLCK);
		       
		       time_to_unban = bt;
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_pass()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_pass()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       	 
		       if(u == 7)
			    return 9;
		       else if(u == 6)
			    return 8;
		       else if(u == 5)
			    return 7;
		       else if(u == 4)
			    return 6;
		       else if(u == 3)
			    return 5;
		       else if(u == 2)
			    return 4;
		       else if(u == 1)
			    return 3;
		       else if(u == 0)
			 {
			    /* User is registered */
			    return 2;
			    time_to_unban = bt;
			 }
		       else
			 {
			    logprintf(0, "Error - In check_pass(): Erroneous line in file\n");
			    return -1;
			 }
		    }
		  else
		    {
		       logprintf(0, "User at %s provided bad password for %s\n", user->hostname, user->nick);
		       set_lock(fd, F_UNLCK);
		       
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_pass()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_pass()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       
		       return 0;
		    }
	       }
	  }
     }
  
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_pass()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_pass()/fclose(): %s", strerror(errno));
	return -1;
     }
 
   if(strlen(default_pass) > 0)
     {
        if(strcmp(this_passwd,default_pass) == 0)
          {
            /* User is regular but default pass required */
            return 1;
          }
        else
	  {
            return 0;
          }
     }
   return -1;
}


/* Write config file */
int write_config_file(void)
{
   FILE *fp;
   int fd;
   int erret;
   char path[MAX_FDP_LEN+1];
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, CONFIG_FILE);
   /* Remove existing config file */
   unlink(path);
   
   while(((fd = open(path, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In write_config_file()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In write_config_file()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {
	logprintf(0, "Error - In write_config_file(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }
   
   if((fp = fdopen(fd, "w")) == NULL)
     {
	logprintf(0, "Error - In write_config_file()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }
    fprintf(fp, "---- !set %s %s ----\n", lp_variable, lp_value);   
    fprintf(fp, "hub_name = \"%s\"\n", hub_name);	       	       
    fprintf(fp, "hub_hostname = \"%s\"\n", hub_hostname);
    fprintf(fp, "listening_port = %u\n", listening_port);
    fprintf(fp, "hub_description = \"%s\"\n", hub_description);
    fprintf(fp, "max_users = %d\n", max_users);	       
    fprintf(fp, "registered_only = %d\n", registered_only);
    fprintf(fp, "default_pass = \"%s\"\n", default_pass);
    fprintf(fp, "default_prefixes = \"%s\"\n", default_prefixes);
    fprintf(fp, "nickchars = \"%s\"\n", nickchars);
    fprintf(fp, "forbchars = \"%s\"\n", forbchars);
    fprintf(fp, "min_share = %llu\n", min_share);
    fprintf(fp, "min_psv_share = %llu\n", min_psv_share);
    fprintf(fp, "min_reg_share = %llu\n", min_reg_share);
    fprintf(fp, "min_vip_share = %llu\n", min_vip_share);
    fprintf(fp, "min_kvip_share = %llu\n", min_kvip_share);
    fprintf(fp, "min_op_share = %llu\n", min_op_share);
    fprintf(fp, "kick_on_zeroshare = %d\n", kick_on_zeroshare);
    fprintf(fp, "gag_on_minshare = %d\n", gag_on_minshare);
    fprintf(fp, "max_share = %d\n", max_share);
    fprintf(fp, "allowed_clients = \"%s\"\n", allowed_clients);
    fprintf(fp, "check_class = %d\n", check_class);
    fprintf(fp, "min_command_class = %d\n", min_command_class);
    fprintf(fp, "min_usercommand_class = %d\n", min_usercommand_class);
    fprintf(fp, "count_reghubs = %d\n", count_reghubs);
    fprintf(fp, "maxhubs = %d\n", maxhubs);
    fprintf(fp, "minslots = %d\n", minslots);
    fprintf(fp, "maxslots = %d\n", maxslots);
    fprintf(fp, "reverse_ratio = %d\n", reverse_ratio);	       
    fprintf(fp, "ratio = %d\n", ratio);	       
    fprintf(fp, "mindcppver = \"%s\"\n", mindcppver);
    fprintf(fp, "min_nick_len = %d\n", min_nick_len);
    fprintf(fp, "deny_passive = %d\n", deny_passive);   
    fprintf(fp, "deny_proxy = %d\n", deny_proxy);       
    fprintf(fp, "restrict_mode = %d\n", restrict_mode);
    fprintf(fp, "connlimit = %d\n", connlimit);       

    fprintf(fp, "regmeprefix = \"%s\"\n", regmeprefix);
    fprintf(fp, "protectmeprefix = \"%s\"\n", protectmeprefix);

    fprintf(fp, "hidetags = %d\n", hidetags);
    fprintf(fp, "hide_allshare = %d\n", hide_allshare);
    fprintf(fp, "show_original_desc = %d\n", show_original_desc);
    fprintf(fp, "show_class_in_desc = %d\n", show_class_in_desc);
    fprintf(fp, "show_mode_in_desc = %d\n", show_mode_in_desc);
    fprintf(fp, "show_original_mail = %d\n", show_original_mail);

    fprintf(fp, "mute_unregistered = %d\n", mute_unregistered);   
    fprintf(fp, "restrictions = %d\n", restrictions);
    fprintf(fp, "punishment = %d\n", punishment);
    fprintf(fp, "kick_bantime = %d\n", kick_bantime);
    fprintf(fp, "tempban = %d\n", tempban);
    fprintf(fp, "max_warnings = %d\n", max_warnings);    
    fprintf(fp, "no_chat_for_chatgagged = %d\n", no_chat_for_chatgagged);    
    fprintf(fp, "ban_overrides_allow = %d\n", ban_overrides_allow);
    fprintf(fp, "max_regular_from_one_ip = %d\n", max_regular_from_one_ip);
    fprintf(fp, "min_search_len = %d\n", min_search_len);
    fprintf(fp, "searchspam_time = %d\n", searchspam_time);    
    fprintf(fp, "deny_main_http = %d\n", deny_main_http);   
    fprintf(fp, "deny_pm_http = %d\n", deny_pm_http);   
    fprintf(fp, "ipgag_type = %d\n", ipgag_type);
    if(banwords != NULL)
    fprintf(fp, "banwords = \"%s\"\n", banwords);
    fprintf(fp, "spam_action = %d\n", spam_action); 
    fprintf(fp, "flood_action = %d\n", flood_action); 
    fprintf(fp, "flood_lines = %d\n", flood_lines);     
    fprintf(fp, "slotbeg_kicker = %d\n", slotbeg_kicker);
    fprintf(fp, "kick_for_incomplete = %d\n", kick_for_incomplete);
    fprintf(fp, "seen_class = %d\n", seen_class); 
    fprintf(fp, "cut_string_to = %d\n", cut_string_to);
    fprintf(fp, "cut_reg_msg = %d\n", cut_reg_msg);
    fprintf(fp, "offline_msgs = %d\n", offline_msgs);

    fprintf(fp, "self_registration = %d\n", self_registration);
    fprintf(fp, "min_regme_share = %llu\n", min_regme_share);
    fprintf(fp, "protection = %d\n", protection);
    fprintf(fp, "prottime = %u\n", prottime);

    fprintf(fp, "botnick = \"%s\"\n", botnick);
    fprintf(fp, "opchatnick = \"%s\"\n", opchatnick);    
    fprintf(fp, "clockbot = %d\n", clockbot);    
    fprintf(fp, "count_bots = %d\n", count_bots);    

    fprintf(fp, "hide_opshare = %d\n", hide_opshare);

    fprintf(fp, "who_can_addrls = %d\n", who_can_addrls);
    fprintf(fp, "who_can_seerls = %d\n", who_can_seerls);
    fprintf(fp, "max_releases_to_show = %d\n", max_releases_to_show);
    fprintf(fp, "who_can_addreq = %d\n", who_can_addreq);
    fprintf(fp, "who_can_seereq = %d\n", who_can_seereq);
    fprintf(fp, "max_requests_to_show = %d\n", max_requests_to_show);

    fprintf(fp, "expiredtime = %u\n", expiredtime);
    fprintf(fp, "expiredrls = %u\n", expiredrls);        
    fprintf(fp, "expiredreq = %u\n", expiredreq);        
    fprintf(fp, "expiredseen = %u\n", expiredseen);        
    fprintf(fp, "expiredurl = %u\n", expiredurl);    
    fprintf(fp, "expiredwarn = %u\n", expiredwarn);

    fprintf(fp, "autofav = %d\n", autofav);
    fprintf(fp, "send_scheduled = %d\n", send_scheduled);
    fprintf(fp, "crypt_enable = %d\n", crypt_enable);
    fprintf(fp, "log_main_chat = %d\n", log_main_chat);   
    fprintf(fp, "verbosity = %d\n", verbosity);

    fprintf(fp, "config_on_priv = %d\n", config_on_priv);
    fprintf(fp, "help_on_priv = %d\n", help_on_priv);
    fprintf(fp, "rules_on_priv = %d\n", rules_on_priv);
    fprintf(fp, "regme_on_priv = %d\n", regme_on_priv);
    fprintf(fp, "url_on_priv = %d\n", url_on_priv);
    fprintf(fp, "rls_on_priv = %d\n", rls_on_priv);
    fprintf(fp, "req_on_priv = %d\n", req_on_priv);

    fprintf(fp, "show_himsg = %d\n", show_himsg);    
    fprintf(fp, "show_onjoinmsgs = %d\n", show_onjoinmsgs);
    fprintf(fp, "show_records = %d\n", show_records);
    fprintf(fp, "show_uptime = %d\n", show_uptime);    
    fprintf(fp, "show_lastmsgs = %d\n", show_lastmsgs);
    fprintf(fp, "show_releases = %d\n", show_releases);
    fprintf(fp, "show_requests = %d\n", show_requests);
    fprintf(fp, "show_welcomes = %d\n", show_welcomes);
    fprintf(fp, "show_joinsparts = %d\n", show_joinsparts);
    fprintf(fp, "show_kicksbans = %d\n", show_kicksbans);
    fprintf(fp, "show_denied = %d\n", show_denied);
    fprintf(fp, "show_actions = %d\n", show_actions);
    fprintf(fp, "show_notifications = %d\n", show_notifications);
    fprintf(fp, "show_helpcommand_usage = %d\n", show_helpcommand_usage);

    fprintf(fp, "extra_commands = %d\n", extra_commands);
    fprintf(fp, "extra_files = \"%s\"\n", extra_files);

    fprintf(fp, "redirect_host_kickban = \"%s\"\n", redirect_host_kickban);
    fprintf(fp, "redirect_host_minshare = \"%s\"\n", redirect_host_minshare);
    fprintf(fp, "redirect_host_maxshare = \"%s\"\n", redirect_host_maxshare);
    fprintf(fp, "redirect_host_maxhubs = \"%s\"\n", redirect_host_maxhubs);
    fprintf(fp, "redirect_host_minslots = \"%s\"\n", redirect_host_minslots);
    fprintf(fp, "redirect_host_maxslots = \"%s\"\n", redirect_host_maxslots);
    fprintf(fp, "redirect_host_ratio = \"%s\"\n", redirect_host_ratio);
    fprintf(fp, "redirect_host_client = \"%s\"\n", redirect_host_client);
    fprintf(fp, "redirect_host_connlimit = \"%s\"\n", redirect_host_connlimit);
    fprintf(fp, "redirect_host_hubfull = \"%s\"\n", redirect_host_hubfull);
    fprintf(fp, "redirect_host_regonly = \"%s\"\n", redirect_host_regonly);

    fprintf(fp, "banmsg = \"%s\"\n", banmsg);
    if(hub_full_msg != NULL)
    fprintf(fp, "hub_full_msg = \"%s\"\n", hub_full_msg);
    fprintf(fp, "regonlymsg = \"%s\"\n", regonlymsg);
    fprintf(fp, "topic = \"%s\"\n", topic);

    fprintf(fp, "welcomereg = \"%s\"\n", welcomereg);
    fprintf(fp, "welcomevip = \"%s\"\n", welcomevip);    
    fprintf(fp, "welcomekvip = \"%s\"\n", welcomekvip);    
    fprintf(fp, "welcomeop = \"%s\"\n", welcomeop);
    fprintf(fp, "welcomecheef = \"%s\"\n", welcomecheef);
    fprintf(fp, "welcomeadmin = \"%s\"\n", welcomeadmin);
    fprintf(fp, "partmsg = \"%s\"\n", partmsg);
    fprintf(fp, "---- !block/!unblock main/pm/download/search ----\n");
    fprintf(fp, "block_main = %d\n", block_main);   
    fprintf(fp, "block_pm = %d\n", block_pm);	    
    fprintf(fp, "block_download = %d\n", block_download);
    fprintf(fp, "block_search = %d\n", block_search);

   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In write_config_file()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In write_config_file()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 1;
}
     
/* Set lock on file */
int set_lock(int fd, int type)
{
   int ret;
   struct flock lock;
   memset(&lock, 0, sizeof(struct flock));
   lock.l_whence = SEEK_SET;
   lock.l_start = 0;
   lock.l_len = 0;
   
   lock.l_type = type;
   
   while(((ret = fcntl(fd, F_SETLKW, &lock)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In set_lock()/fcntl(): Interrupted system call. Trying again.\n");
   
   if(ret < 0)
     {
	logprintf(0, "Error - In set_lock()/fcntl(): %s", strerror(errno));
	quit = 1;
	return 0;
     }   
   return 1;
}

/* Removes a user from the reglist */
int remove_reg_user(char *buf)
{
   int nick_len, i;
   char *temp;
   char nick[MAX_NICK_LEN+1];
   char path[MAX_FDP_LEN+1];
   int line_nbr;

   
   line_nbr = 0;
   temp = NULL;
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);
   
   if(buf[strlen(buf)-1] == '|')
     nick_len = strlen(buf)-1;
   else
     nick_len = strlen(buf);
   
   snprintf(nick, (nick_len>MAX_NICK_LEN)?MAX_NICK_LEN+1:nick_len+1, buf);
   i = remove_line_from_file(nick, path, 0);
   loadfile2buf(&reglist_mem,REG_FILE);
   return i;
}
   

/* Adds a user to the reglist. Returns 2 if the command had bad format and 3
 * if it's already registered Format is: $AddRegUser <nick> <pass> <opstatus> */
int add_reg_user(char *buf)
{
   int ret;
   char command[21];
   char nick[MAX_NICK_LEN+1];
   char pass[51];
   char path[MAX_FDP_LEN+1];
   char line[151];
   int  type;
   time_t timeofdelete;
   time_t now;
   now = time(NULL);

   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);

   
if(sscanf(buf, "%20s %50s %50s %d", command, nick, pass, &type) != 4)
     return 2;
   
   if((pass[0] == '\0') || (type < 0) || (type > 7))
     return 2;

   /* If the user already is there, then remove the user first */
   if(check_if_registered(nick) != 0)
     return 3;
   
   encrypt_pass(pass);

if(protdays > 0)
timeofdelete = now + (protdays * 86400);
else
{
if((type == 0) && (check_if_permprotected(nick) == 0))
timeofdelete = now + (prottime * 86400);
else
timeofdelete = now + (expiredtime * 86400);
}


if(expiredtime != 0)
   snprintf(line, 151, "%s %s %d %lu", nick, pass, type, timeofdelete);
else
   snprintf(line, 151, "%s %s %d", nick, pass, type);   

   ret = add_line_to_file(line, path);
   
   /* Send the event to script */
#ifdef HAVE_PERL
   if(ret == 1)
     {	
	command_to_scripts("$Script added_registered_user %c%c", '\005', '\005');
	non_format_to_scripts(nick);
	command_to_scripts("|");
     }   
#endif
loadfile2buf(&reglist_mem,REG_FILE);
return ret;
}

int add_seen_user(char *buf)
{
   int ret;
   char nick[MAX_NICK_LEN+1];
   char path[MAX_FDP_LEN+1];
   char shost[51];
   char thost[101];
   time_t seenxp;
   char line[151];
struct tm * timeinfo;
   time_t now;
   now = time(NULL);

   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SEEN_FILE);
   
  if(sscanf(buf, "%50s %50s", nick, shost) != 2)
     return 2;

  snprintf(thost, 100, "%s %s", nick, shost);
  remove_line_from_file(thost, path, 0);   

    timeinfo = localtime ( &now );
    seenxp = now + (expiredseen * 86400);
   snprintf(line, 150, "%lu %s %s %s", seenxp, nick, shost, asctime (timeinfo));
   
   ret = add_line_to_file(line, path);
   return ret;
}

int add_away_user(char *buf)
{
   int ret;
   char nick[MAX_NICK_LEN+1];
   char path[MAX_FDP_LEN+1];
   char shost[501];
   char line[601];
   time_t now;
   now = time(NULL);
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AWAYS);
   
   if(sscanf(buf, "%50s %500[^|]|", nick, shost) != 2)
     return 2;

   remove_line_from_file(nick, path, 0);   
   snprintf(line, 600, "%s %lu %s", nick, now, shost);
   ret = add_line_to_file(line, path);
   loadfile2buf(&away_mem,AWAYS);
   return ret;
}

int remove_away_user(struct user_t *user)
{
int ret;
   char path[MAX_FDP_LEN+1];
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AWAYS);
ret = remove_line_from_file(user->nick, path, 0);
   loadfile2buf(&away_mem,AWAYS);
return ret;
}

//DB// Adding autoaway on !autoaway
int add_autoaway_user(char *buf)
{
   int ret, time;
   char nick[MAX_NICK_LEN+1];
   char path[MAX_FDP_LEN+1];
   char reason[501];
   char line[601];
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AUTOAWAYS);
   
   if(sscanf(buf, "%50s %d %500[^|]|", nick, &time, reason) != 3)
   return 2;

   remove_line_from_file(nick, path, 0);   

   snprintf(line, 600, "%s", buf);
   
   ret = add_line_to_file(line, path);
   loadfile2buf(&autoaway_mem,AUTOAWAYS);
   
   return ret;
}

//DB// Removing autoaway on !noautoaway
int remove_autoaway_user(struct user_t *user)
{
   int ret;
   char path[MAX_FDP_LEN+1];
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AUTOAWAYS);
   
   ret = remove_line_from_file(user->nick, path, 0);
   loadfile2buf(&autoaway_mem,AUTOAWAYS);
   
   return ret;
}

//DB// Creates file with current topic details (an author and time of setting)
int add_settopic(char *buf)
{
   int ret;
   char path[MAX_FDP_LEN+1];
   char line[151];

   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SETTOPIC);

   remove_line_from_file("topic", path, 0);   
   snprintf(line, 150, "topic %s", buf);
   
   ret = add_line_to_file(line, path);
   return ret;
}

//DB// Removes file with topic details
int remove_settopic(void)
{
    
   int ret;
   char path[MAX_FDP_LEN+1];
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SETTOPIC);
   
   ret = remove_line_from_file("topic", path, 0);
   topicstring[0] = '\0';
   
   return ret;
}

/* Set the directories used */
int init_dirs(void)
{
   char path[MAX_FDP_LEN+1];
   char script_dir[MAX_FDP_LEN+1];
   char extrafiles_dir[MAX_FDP_LEN+1];
   char log_dir[MAX_FDP_LEN+1];
   if(strlen(working_dir) == 0)
     {
#ifdef __CYGWIN__
	getcwd(working_dir, MAX_FDP_LEN);
#else
#ifdef SWITCH_USER
	struct passwd *user = getpwuid(dchub_user);
	snprintf( working_dir, MAX_FDP_LEN, user->pw_dir );
#else
	if( getenv( "HOME" ) == NULL )
	   return 0;
   
	snprintf( working_dir, MAX_FDP_LEN, getenv( "HOME" ) );
#endif
#endif
     }
   strncpy(path, working_dir, MAX_FDP_LEN);
   snprintf( config_dir, MAX_FDP_LEN, "%s/.dbhub", path );

   sprintfa(path, "/tmp");
   sprintf(un_sock_path, "%s/%s", path, UN_SOCK_NAME);
   sprintf(script_dir, "%s/%s", config_dir, SCRIPT_DIR);
   sprintf(log_dir, "%s/%s", config_dir, LOG_DIR);
   sprintf(extrafiles_dir, "%s/%s", config_dir, EXTRAFILES_DIR);
   mkdir(path, 0700);
   mkdir(config_dir, 0700);
   mkdir(script_dir, 0700);
   mkdir(extrafiles_dir, 0700);
   mkdir(log_dir, 0700);
   return 1;
}


//DB// Write files created by !set
int write_file(char *buf, char *type, int overwrite)
{
   FILE *fp;
   int fd;
   int erret;
   char path[MAX_FDP_LEN+1];
   if(strcmp(type, "motd") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_FILE);
   else if(strcmp(type, "motdn") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_N);
   else if(strcmp(type, "motdp") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_P);
   else if(strcmp(type, "motdr") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_R);
   else if(strcmp(type, "motdv") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_V);
   else if(strcmp(type, "motdk") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_K);
   else if(strcmp(type, "motdo") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_O);
   else if(strcmp(type, "motdc") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_C);
   else if(strcmp(type, "motda") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_A);
   else if(strcmp(type, "rules") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RULES_FILE);
   else if(strcmp(type, "regme") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REGME_FILE);
   else if(strcmp(type, "autofav") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, AUTOFAV_FILE);
   else if(strcmp(type, "scheduledmsg") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, SCHEDULE_FILE);
   else    
    snprintf(path, MAX_FDP_LEN, "%s/%s/%s", config_dir, EXTRAFILES_DIR, type);
   if(overwrite == 0)
     {
	while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
	  logprintf(0, "Error - In write_file()/open(): Interrupted system call. Trying again.\n"); 
	
	if(fd >= 0)
	  {
	     //DB// file already exists
	     close(fd);
	     return 0;
	  }
     }
   
   if(overwrite != 0)
     unlink(path);
   
   while(((fd = open(path, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In write_file()/open(): Interrupted system call. Trying again.\n");
   
   if(fd < 0)
     {
	logprintf(0, "Error - In write_file()/open(): %s", strerror(errno));
	return -1;
     }
   
   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {	
	close(fd);
	return -1;
     }
   
   if((fp = fdopen(fd, "w")) == NULL)
     {
	logprintf(0, "Error - In write_file()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }
   fprintf(fp, "%s", buf);
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In write_file()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In write_file()/fclose(): %s", strerror(errno));
	return -1;
     }
   if(overwrite != 0)
     return 0;
   else
     return 1;
}

/* Sends the motd to the particular user. */
int send_motd(struct user_t *user)
{
   FILE *fp;
   int fd;
   int erret;
   char line[4095];
   char path[MAX_FDP_LEN+1];
   
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_FILE);
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In send_motd()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In send_motd()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In send_motd(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In send_motd()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
     logprintf(0, "Error - In send_motd()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In send_motd()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 1;
}

/* Sends the motd to the particular user. */
int send_new_motd(struct user_t *user, char *type)
{
   FILE *fp;
   int fd;
   int erret;
   char line[4095];
   char path[MAX_FDP_LEN+1];
   if(strcmp(type, "motdn") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_N);
   else if(strcmp(type, "motdp") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_P);
   else if(strcmp(type, "motdr") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_R);
   else if(strcmp(type, "motdv") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_V);
   else if(strcmp(type, "motdk") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_K);
   else if(strcmp(type, "motdo") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_O);
   else if(strcmp(type, "motdc") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_C);
   else if(strcmp(type, "motda") == 0)
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, MOTD_A);
   
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In send_motd()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In send_motd()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In send_motd(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In send_motd()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
     logprintf(0, "Error - In send_motd()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In send_motd()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 1;
}



/* Adds line to end of a file */
int add_line_to_file(char *line, char *file)
{
   FILE *fp;
   int fd;
   int erret;

    trim_string(line);
   /* Open the file */
   while(((fd = open(file, O_RDWR)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In add_line_to_file()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In add_line_to_file()/open(), file = %s: %s", file, strerror(errno));
	return -1;	
     }   
   
   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {	
	logprintf(0, "Error - In add_line_to_file(): Couldn't set file lock, file = %s\n", file);
	close(fd);
	return -1;
     }
   
   if((fp = fdopen(fd, "a")) == NULL)
     {	
	logprintf(0, "Error - In add_line_to_file()/fdopen(), file = %s: %s", file, strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
     }
   fprintf(fp, "%s\n", line);
   
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In add_line_to_file()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In add_line_to_file()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 1;
}

/* Removes line from file. Word has to match first word in the line in
 * the file. If port is set to anything else than zero, it assumes it's the
 * linklist file and then the port must match as well. Returns 1 on success, 
 * 0 if pattern wasn't found and -1 on error.  */
int remove_line_from_file(char *line, char *file, int port)
{
   FILE *fp;
   int fd;
   int erret;
   char *temp;
   char *urls;
   char word[501];
   char fileline[1024];
   char fileword[501];
   char filetime[17];
   char tempword1[51];
   char tempword2[51];
   int i, len;
   int fileport;
   int line_nbr = 0;
   if((temp = malloc(sizeof(char) * 2)) == NULL)
     {
	logprintf(0, "Error - In remove_line_from_file()/malloc(): %s", strerror(errno));
	quit = 1;
	return -1;
     }   

   snprintf(word, 500, "%s", line);
   sprintf(temp, "%c", '\0');

   while(((fd = open(file, O_RDWR)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_line_from_file()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In remove_line_from_file()/open(), file = %s: %s", file, strerror(errno));
	free(temp);
	return -1;	
     }   
   
   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {	
	logprintf(0, "Error - In remove_line_from_file(): Couldn't set file lock, file = %s\n", file);
	close(fd);
	free(temp);
	return -1;
     }
   
   if((fp = fdopen(fd, "r")) == NULL)
     {	
	logprintf(0, "Error - In remove_line_from_file()/fdopen(), file = %s: %s", file, strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	free(temp);
	return -1;
     }
   
   while(fgets(fileline, 1023, fp) != NULL)
     {	
	line_nbr++;
	if(port != 0)	     
	  sscanf(fileline, "%200s %d", fileword, &fileport);
	else 
	  {	     
	    if(search_in_string(file, "seen") == 1)    
{	   
sscanf(fileline, "%16s %50s %50s", filetime, tempword1, tempword2);
snprintf(fileword, 500, "%s %s", tempword1, tempword2);
}	   
	   else if(search_in_string(file, "relea") == 1)
{
	   sscanf(fileline, "%16s <-> [ %*50[^:]: %500[^,],", filetime, fileword);
fileword[strlen(fileword) -2] = '\0';
}	   
	   else if(search_in_string(file, "hidden") == 1)
{
	  sscanf(fileline, "%50s", fileword);
}	   


	   else if(search_in_string(file, "req") == 1)
{
	   sscanf(fileline, "%16s <-> [ %200[^,],", filetime, fileword);
fileword[strlen(fileword) -2] = '\0';
}	   
	   else if(search_in_string(file, "url") == 1)
{
urls = strstr(fileline, "[");
sscanf(urls, "[ %100[^,],", fileword);
fileword[strlen(fileword) -2] = '\0';
}	   
	   else if(search_in_string(file, "banreaso") == 1)
	   sscanf(fileline, "%16s %200s", filetime, fileword);

	    else
	    sscanf(fileline, "%200s", fileword);

	    fileport = 0;
	  }	
	if(((strncasecmp(word, fileword, strlen(word)) == 0)
	   && (strlen(word) == strlen(fileword))
	   && (port == fileport)))
	  {	     
	     /* Put the rest of the file in the temporary string */
	     while(fgets(fileline, 1023, fp) != NULL)
	       {		  
		  if((temp = realloc(temp, sizeof(char)
				     * (strlen(temp) + strlen(fileline) + 1))) == NULL)
		    {	
		       logprintf(0, "Error - In remove_line_from_file()/realloc(): %s", strerror(errno));
		       quit = 1;
		       set_lock(fd, F_UNLCK);
		       fclose(fp);		       
		       return -1;
		    }		  
		  strcat(temp, fileline);
	       }	     
	     rewind(fp);
	     
	     /* Go to the position where the user name is */
	     for(i = 1; i<= (line_nbr-1); i++)
	       fgets(fileline, 1023, fp);
	     
	     /* Truncate the file */
	     len = ftell(fp);
	     
	     while(((erret = fclose(fp)) != 0) && (errno == EINTR))
	       logprintf(0, "Error - In remove_line_from_file()/fclose(): Interrupted system call. Trying again.\n");
	     
	     if(erret != 0)
	       {
		  logprintf(0, "Error - In remove_line_from_file()/fclose(): %s", strerror(errno));
		  return -1;
	       }
	     
	     truncate(file, len);
	     
	     while(((fd = open(file, O_RDWR)) < 0) && (errno == EINTR))
	       logprintf(0, "Error - In remove_line_from_file()/open(): Interrupted system call. Trying again.\n");   
	     
	     if(fd < 0)
	       {		  
		  logprintf(0, "Error - In remove_line_from_file()/open(), file = %s: %s", file, strerror(errno));
		  free(temp);
		  return -1;
	       }
	     
	     if((fp = fdopen(fd, "a")) == NULL)
	       {		  
		  logprintf(0, "Error - In remove_line_from_file()/fdopen(), file = %s: %s", file, strerror(errno));
		  set_lock(fd, F_UNLCK);
		  close(fd);
		  free(temp);
		  return -1;
	       }	     
	     fwrite(temp, strlen(temp), 1, fp);
	     
	     set_lock(fd, F_UNLCK);
	     
	     while(((erret = fclose(fp)) != 0) && (errno == EINTR))
	       logprintf(0, "Error - In remove_line_from_file()/fclose(): Interrupted system call. Trying again.\n");
	     
	     if(erret != 0)
	       {
		  logprintf(0, "Error - In remove_line_from_file()/fclose(): %s", strerror(errno));
		  free(temp);
		  return -1;
	       }
	     
	     free(temp);
	     return 1;
	  }	
     }   
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_line_from_file()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In remove_line_from_file()/fclose(): %s", strerror(errno));
	free(temp);
	return -1;
     }
   free(temp);
   return 0;
}

/* Remove an expired ban/allow line from a file.  */
int remove_exp_from_file(time_t now_time, char *file)
{
   FILE *fp;
   FILE *newfp;
   int fd;
   int erret;
   int newfd;
   char *newfile;
   char somenick[51];
   char somepass[51];
int sometype;
   char fileline[1024];
   char fileword[201];
   time_t exp_time;
   exp_time = 0;
    sometype = 0;

   if((newfile = malloc(strlen(file) + 2)) == NULL)
     {
	logprintf(0, "Error - In remove_exp_from_file()/malloc(): %s", strerror(errno));
	quit = 1;
	return -1;
     }   

   strcpy(newfile, file);
   strcat(newfile, "1");
   while(((fd = open(file, O_RDWR)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_exp_from_file()/open(): Interrupted system call. Trying again.\n");
   
   if(fd < 0)
     {	
	logprintf(0, "Error - In remove_exp_from_file()/open(), file = %s: %s", file, strerror(errno));
	free(newfile);
	return -1;
     }

   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {	
	logprintf(0, "Error - In remove_exp_from_file(): Couldn't set file lock, file = %s\n", file);
	close(fd);
	free(newfile);
	return -1;
     }

   if((fp = fdopen(fd, "r")) == NULL)
     {	
	logprintf(0, "Error - In remove_exp_from_file()/fdopen(), file = %s: %s", file, strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	free(newfile);
	return -1;
     }


   unlink(newfile);
   
   while(((newfd = open(newfile, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_exp_from_file()/open(): Interrupted system call. Trying again.\n");   
   
   if(newfd < 0)
     {
	logprintf(0, "Error - In remove_exp_from_file()/open(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
	  logprintf(0, "Error - In remove_exp_from_file()/fclose(): Interrupted system call. Trying again.\n");
	
	if(erret != 0)
	  {
	     logprintf(0, "Error - In remove_exp_from_file()/fclose(): %s", strerror(errno));
	     free(newfile);
	     return -1;
	  }
       
	free(newfile);
	return -1;
     }
   
   if(set_lock(newfd, F_WRLCK) == 0)
     {
	logprintf(0, "Error - In remove_exp_from_file(): Couldn't set file lock, file = %s\n", newfile);
	set_lock(fd, F_UNLCK);
	fclose(fp);
	close(newfd);
	free(newfile);
	return -1;
     }

   if((newfp = fdopen(newfd, "w")) == NULL)
     {
	logprintf(0, "Error - In remove_exp_from_file()/fdopen(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	fclose(fp);
	set_lock(newfd, F_UNLCK);
	close(newfd);
	free(newfile);
	return -1;
     }

   while(fgets(fileline, 1023, fp) != NULL)
     {	
	exp_time = 0;
if(strstr(file, "reglist") != NULL)
{
if(sscanf(fileline, "%50s %50s %i %lu", somenick, somepass, &sometype, &exp_time) >= 3)
{
	if((exp_time == 0) || (exp_time > now_time) || (sometype == 2))
	fprintf(newfp, "%s", fileline);
}
}
else 
{
if((strstr(file, "url") != NULL) || (strstr(file, "release") != NULL) || (strstr(file, "seen") != NULL) || (strstr(file, "req") != NULL))
{
if(sscanf(fileline, "%lu", &exp_time) == 1)
{
	if(exp_time > now_time)
	  fprintf(newfp, "%s", fileline);
}
}
else if(strstr(file, "reason") != NULL)
{
if(sscanf(fileline, "%lu", &exp_time) == 1)
{
if((exp_time == 0) || (exp_time > now_time))
	  fprintf(newfp, "%s", fileline);
}
}
else 
{
if(sscanf(fileline, "%200s %lu", fileword, &exp_time) == 2)
{
	if((exp_time == 0) || (exp_time > now_time))
	  fprintf(newfp, "%s", fileline);
}
}
     
     }   
 }
   set_lock(newfd, F_UNLCK);
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(newfp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_exp_from_file()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In remove_exp_from_file()/fclose(): %s", strerror(errno));
	free(newfile);
	return -1;
     }
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_exp_from_file()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In remove_exp_from_file()/fclose(): %s", strerror(errno));
	free(newfile);
	return -1;
     }
   rename(newfile, file);
   free(newfile);

   return 0;
}

/* This puts a list of all files in directory dirname that ends with '.pl'
 * in namelist. It returns the number of matching entries.  */
int my_scandir(char *dirname, char *namelist[])
{
   DIR *dp;
   struct dirent *dent;
   int i = 0;
   if((dp = opendir(dirname)) == NULL)
     return -1;
   
   while((dent = readdir(dp)) != NULL)
     i++;
   
   if(i == 0)
     return 0;
   
   rewinddir(dp);
   
   i = 0;
   
   while((dent = readdir(dp)) != NULL)
     {
	
	/* Only parse files with filenames ending with .pl  */
	if(!((strlen( (strrchr(dent->d_name, 'l') == NULL)
		      ? "" : strrchr(dent->d_name, 'l')) == 1)
	     && (strlen( (strrchr(dent->d_name, 'p') == NULL)
			 ? "" : strrchr(dent->d_name, 'p')) == 2)
	     && (strlen( (strrchr(dent->d_name, '.') == NULL)
			 ? "" : strrchr(dent->d_name, '.')) == 3)))
	  continue;
	if((namelist[i] = (char *)malloc(sizeof(char)
				    * (strlen(dirname) + strlen(dent->d_name) + 2))) == NULL)
	  {	     
	     logprintf(0, "Error - In my_scandir()/malloc(): %s", strerror(errno));
	     quit = 1;
	     return 0;
	  }
	sprintf(namelist[i], "%s/%s", dirname, dent->d_name);
	i++;
     }
   closedir(dp);
   return i;
}

void rls_type(struct user_t *user, char *rlstype)
{
   int i;
   size_t j;
   int fd;
   int erret;
   int matches = 0;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
char current[1025];


snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);	

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In rls_type()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	set_lock(fd, F_UNLCK);
	close(fd);
	return;
     }
if(strcmp(rlstype, "ALL") != 0)
{
if(rls_on_priv == 1)
uprintf(user, "$To: %s From: %s $", user->nick, botnick);

if(check_if_categ(rlstype) == 1)
uprintf(user, "<%s> %s [ %s ] %s:\r\n", botnick, lp_recentlyadded, category, lp_releases);
else
{
uprintf(user, "<%s> [ %s ] %s:\r\n", botnick, rlstype, lp_categnotfound);
send_user_list(CATEG, user);
send_to_user("|", user);
return;
}
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
if(strcmp(rlstype, "ALL") == 0)
{
matches++;
uprintf(user, "%s\r\n", line+i+15);
}
else
{
snprintf(current, 1024, "%s", line+i+17);
current[1] = '\0';
if((strcmp(rlstype, current) == 0))
		    {
		    
		    matches++;
		    uprintf(user, "%s\r\n", line+i+15);
}
	  }
	  }
     }

   set_lock(fd, F_UNLCK);
   uprintf(user, "[ %d %s ]|", matches, lp_itemstotal);
   allreleases = matches;
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In rls_type()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;

return;
}

void send_req(struct user_t *user)
{
   int i;
   size_t j;
   int fd;
   int erret;
   int matches = 0;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];


snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);	

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In send_req()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
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
matches++;
uprintf(user, "%s\r\n", line+i+15);
}
     }

   set_lock(fd, F_UNLCK);
   uprintf(user, "[ %d %s ]|", matches, lp_itemstotal);
   allreleases = matches;
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In send_req()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;

return;
}



int check_if_exist_rls(char *rlsname)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char fileword[201];

       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);
    while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_exist_rls()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_if_exist_rls()/open(): %s", strerror(errno));
	return -1;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_if_exist_rls(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_if_exist_rls()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     

	   sscanf(line+i, "%*10[^ ] <-> [ %*50[^:]: %200[^,],", fileword);
	    fileword[strlen(fileword) -2] = '\0';

	if((strncasecmp(rlsname, fileword, strlen(rlsname)) == 0)
	   && (strlen(rlsname) == strlen(fileword)))
{
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_exist_rls()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_if_exist_rls()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       return 1;
}
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_exist_rls()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_exist_rls()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

int check_if_exist_req(char *rlsname)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char fileword[201];

       snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);
    while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_exist_req()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_if_exist_req()/open(): %s", strerror(errno));
	return -1;	
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_if_exist_req(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_if_exist_req()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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
	     

	   sscanf(line+i, "%*10[^ ] <-> [ %200[^,],", fileword);
	    fileword[strlen(fileword) -2] = '\0';

	if((strncasecmp(rlsname, fileword, strlen(rlsname)) == 0)
	   && (strlen(rlsname) == strlen(fileword)))
{
		       set_lock(fd, F_UNLCK);
		       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
			 logprintf(0, "Error - In check_if_exist_req()/fclose(): Interrupted system call. Trying again.\n");
		       
		       if(erret != 0)
			 {
			    logprintf(0, "Error - In check_if_exist_req()/fclose(): %s", strerror(errno));
			    return -1;
			 }
		       return 1;
}
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_if_exist_req()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_if_exist_req()/fclose(): %s", strerror(errno));
	return -1;
     }
   return 0;
}

int check_warning(struct user_t *user, int type)
{
   char *warnreason;
   char someone[51];
   int warns = 0;
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char ban_host[MAX_HOST_LEN+1];

   if(mystristr(warn_mem, user->nick) == NULL)
   return 0;

	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WARN_FILE);
   
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In check_warning()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In check_warning()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In check_warning(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In check_warning()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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

	     sscanf(line+i, "%120s %17s", ban_host, someone);
	     warnreason = line+i + strlen(someone) + 1 + strlen(ban_host) + 1;

	     if(strcasecmp(user->nick, ban_host) == 0)
		    {
		    warns++;
		    if(type == 0)
		    {
		    if(warns == 1)
		    uprintf(user, "$To: %s From: %s $<%s> %s:", user->nick, botnick, botnick, lp_yourwarnings);
		    uprintf(user, "\r\n%s", warnreason);
		    }
		    }
	       
    	}
     }
   set_lock(fd, F_UNLCK);
   
   if((type == 0) && (warns != 0))
   uprintf(user, "|");
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In check_warning()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In check_warning()/fclose(): %s", strerror(errno));
	return -1;
     }
   return warns;
}

int remove_warnings(char *nick)
{
   char someone[51];
   int warns = 0;
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   char ban_host[MAX_HOST_LEN+1];

	snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, WARN_FILE);
   
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_warnings()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(0, "Error - In remove_warnings()/open(): %s", strerror(errno));
	return -1;	
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(0, "Error - In remove_warnings(): Couldn't set file lock\n");
	close(fd);
	return -1;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(0, "Error - In remove_warnings()/fdopen(): %s", strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	return -1;
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

	     sscanf(line+i, "%120s %17s", ban_host, someone);

	     if(strcasecmp(nick, ban_host) == 0)
		    {
		    warns++;
		    remove_line_from_file(nick, path, 0);
		    }
	       
    	}
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_warning()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In remove_warning()/fclose(): %s", strerror(errno));
	return -1;
     }
   return warns;
}


void find_rls(struct user_t *user, char *rlstype)
{
   int i;
   size_t j;
   int fd;
   int erret;
   int matches = 0;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);	


   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In find_rls()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	set_lock(fd, F_UNLCK);
	close(fd);
	return;
     }
if(rls_on_priv == 1)
uprintf(user, "$To: %s From: %s $", user->nick, botnick);
uprintf(user, "<%s> %s [ %s ]:\r\n", botnick, lp_searchresults, rlstype);

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
if(search_in_string(line+i, rlstype) == 1)						
{
matches++;
uprintf(user, "%s\r\n", line+i+15);
}
	  
	  }
     }

   set_lock(fd, F_UNLCK);
   uprintf(user, "[ %d %s ]|", matches, lp_itemstotal);
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In find_rls()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;
return;
}

void sort_urls(struct user_t *user)
{
   int i;
   size_t j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];

snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, URL_FILE);	

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In sort_urls()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
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

		    
		    uprintf(user, "%s\r\n", line+i+15);
	  }
     }

   set_lock(fd, F_UNLCK);
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In sort_urls()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;
return;
}

void rls_motd()
{
   int i;
   size_t j;
   int fd;
   int erret;
   int matches = 0;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];


snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);	
if(rlsmotd != NULL)
free(rlsmotd);

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In rls_motd()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
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
		    
		    matches++;
	  }
	  
     }

   set_lock(fd, F_UNLCK);
   allreleases = matches;
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In rls_motd()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;

   matches = 0;
   if((rlsmotd = malloc(sizeof(char) * 2)) == NULL)
        {
	logprintf(0, "Error - In get_op_list()/malloc(): %s", strerror(errno));
	quit = 1;
	return;
	}
    rlsmotd[0] = '\0';

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In rls_motd()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
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
	matches++;
if(matches >= allreleases - max_releases_to_show + 1)
{					     
   if((rlsmotd = realloc(rlsmotd, sizeof(char)
                         * (strlen(rlsmotd) + strlen(line+i+15) + 3))) == NULL)
     {
        logprintf(0, "Error - In get_op_list()/realloc(): %s", strerror(errno));
        quit = 1;
        return;
     }
sprintfa(rlsmotd, "%s\r\n", line+i+15);
}	  
     }
}

   set_lock(fd, F_UNLCK);

   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In rls_motd()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;
return;
}


void req_motd()
{
   int i;
   size_t j;
   int fd;
   int erret;
   int matches = 0;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];

if(reqmotd != NULL)
free(reqmotd);

snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);	


   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In req_motd()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
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
matches++;

}
     }

   set_lock(fd, F_UNLCK);
   allrequests = matches;
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In req_motd()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;

   matches = 0;
   if((reqmotd = malloc(sizeof(char) * 2)) == NULL)
        {
	logprintf(0, "Error - In get_op_list()/malloc(): %s", strerror(errno));
	quit = 1;
	return;
	}
    
    reqmotd[0] = '\0';

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In req_motd()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
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

matches++;
if(matches >= allrequests - max_requests_to_show + 1)
{					     

   if((reqmotd = realloc(reqmotd, sizeof(char)
                         * (strlen(reqmotd) + strlen(line+i+15) + 3))) == NULL)
     {
        logprintf(0, "Error - In get_op_list()/realloc(): %s", strerror(errno));
        quit = 1;
        return;
     }
sprintfa(reqmotd, "%s\r\n", line+i+15);
}


}
	  }

   set_lock(fd, F_UNLCK);


   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In req_motd()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;
return;
}

//DB// Checking if user's share (return 1) or presence (return 2) is hidden
int check_if_hidden(char *user_nick)
{
    char *k;
    char *temp;
    int type = 0;
    char awayuser[51];

   if((temp = malloc(sizeof(char) * (strlen(hide_mem) + 1))) == NULL)
     {
	logprintf(0, "Error - In check_if_hidden()/malloc(): %s", strerror(errno));
	quit = 1;
	return 0;
     }

sprintf(temp, "%s", hide_mem);
k = temp;

    while(strstr(k, "\n") != NULL)
    {
	if(sscanf(k, "%s %d", awayuser, &type) == 2)
	{
	    if(strcasecmp(user_nick, awayuser) == 0)
	    {
	    if(type == 2)
	    {
	    if(temp != NULL)
	    free(temp);
	    return 2;
	    }
	    if(type == 1)
	    {
	    if(temp != NULL)
	    free(temp);
	    return 1;
	    }
	    }
    }
if((k = strstr(k, "\n")) == NULL)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
{
if(strlen(k) <= 1)
{
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}
else
k = k + 1;
}
    }
    
    if(sscanf(k, "%s %d", awayuser, &type) == 2)
    {
        if(strcasecmp(user_nick, awayuser) == 0)
    	{
	if(type == 2)
        {
	if(temp != NULL)                                                                                             
	free(temp);                                                                                                  
	return 2;
        }
	if(type == 1)
        {
	if(temp != NULL)                                                                                             
	free(temp);                                                                                                  
	return 1;
	}
	}
    }
if(temp != NULL)                                                                                             
free(temp);                                                                                                  
return 0;
}

//DB// This function updates reglist every ALARM_TIME
//DB// Removing expired entries and adding new expiration times if user is online while checking
void update_exp(void)
{
   FILE *fp;
   FILE *newfp;
   int fd;
   int erret;
   int newfd;
   char *newfile;
   char somenick[51];
   char somepass[51];
   int sometype;
   char fileline[1024];
   char file[MAX_FDP_LEN+1];
   time_t exp_time;
   time_t now_time;
    
   now_time = time(NULL);    
   sometype = 0;

   snprintf(file, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);

   if((newfile = malloc(strlen(file) + 2)) == NULL)
     {
	logprintf(0, "Error - In update_exp()/malloc(): %s", strerror(errno));
	quit = 1;
	return;
     }   

   strcpy(newfile, file);
   strcat(newfile, "1");
   while(((fd = open(file, O_RDWR)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/open(): Interrupted system call. Trying again.\n");
   
   if(fd < 0)
     {	
	logprintf(0, "Error - In update_exp()/open(), file = %s: %s", file, strerror(errno));
	free(newfile);
	return;
     }

   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {	
	logprintf(0, "Error - In update_exp(): Couldn't set file lock, file = %s\n", file);
	close(fd);
	free(newfile);
	return;
     }

   if((fp = fdopen(fd, "r")) == NULL)
     {	
	logprintf(0, "Error - In update_exp()/fdopen(), file = %s: %s", file, strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	free(newfile);
	return;
     }


   unlink(newfile);
   
   while(((newfd = open(newfile, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/open(): Interrupted system call. Trying again.\n");   
   
   if(newfd < 0)
     {
	logprintf(0, "Error - In update_exp()/open(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
	  logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
	
	if(erret != 0)
	  {
	     logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	     free(newfile);
	     return;
	  }
       
	free(newfile);
	return;
     }
   
   if(set_lock(newfd, F_WRLCK) == 0)
     {
	logprintf(0, "Error - In update_exp(): Couldn't set file lock, file = %s\n", newfile);
	set_lock(fd, F_UNLCK);
	fclose(fp);
	close(newfd);
	free(newfile);
	return;
     }
   if((newfp = fdopen(newfd, "w")) == NULL)
     {
	logprintf(0, "Error - In updated_exp()/fdopen(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	fclose(fp);
	set_lock(newfd, F_UNLCK);
	close(newfd);
	free(newfile);
	return;
     }

   while(fgets(fileline, 1023, fp) != NULL)
     {	
	exp_time = 0;
        if(sscanf(fileline, "%50s %50s %i %lu", somenick, somepass, &sometype, &exp_time) >= 3)
	{
	if(expiredtime == 0)
	{
	if((sometype != 0) || ((sometype == 0) && (check_if_permprotected(somenick) == 0) && (exp_time >= now_time)))
	fprintf(newfp, "%s", fileline);
	}
	else
	{
	if(get_human_user(somenick) != NULL) 
	{
	if((sometype == 0) && (check_if_permprotected(somenick) == 0) && (exp_time >= now_time))
	fprintf(newfp, "%s", fileline);
	else
	fprintf(newfp, "%s %s %i %lu\n", somenick, somepass, sometype, now_time + (expiredtime * 86400));
	}
	else
	{
	if((exp_time == 0) || (exp_time >= now_time))
	{
	fprintf(newfp, "%s", fileline);
	}
	}
	}	
	}
    }
   set_lock(newfd, F_UNLCK);
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(newfp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	free(newfile);
	return;
     }
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	free(newfile);
	return;
     }
   rename(newfile, file);
   free(newfile);
}

//DB// This function removes entry form releaselist/reqlist
//DB// The author can only remove his own release/request, admins/owners can remove any
//DB// Wildcard * is allowed
void remove_entry(struct user_t *user, char *buf, int file, int place)
{
	int i;
	size_t j;
	char rline[501];
	char path[MAX_FDP_LEN+1];
        int fd;
        int erret;
        FILE *fp;
	char fileline[1024];
	char filetime[17];
	char fileword[501];
	char usernick[53];
	int matches = 0;
	
	if(file == 0)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, RELEASE_FILE);
	else if(file == 1)
		snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REQ_FILE);
	else
		return;

    sscanf(buf, "%500[^|]", rline);
    snprintf(usernick, 52, " %s ", user->nick);

   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - remove_entry()/open(): Interrupted system call. Trying again.\n");   
   if(fd < 0)
	return;	

   if(set_lock(fd, F_RDLCK) == 0)
     {
	close(fd);
	return;
     }   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	set_lock(fd, F_UNLCK);
	close(fd);
	return;
     }

   while(fgets(fileline, 1023, fp) != NULL)
     {
	trim_string(fileline);
	j = strlen(fileline);
	if(j != 0)
	  {
	     /* Jump to next char which isn't a space */
	     i = 0;
	     while(fileline[i] == ' ')
	       i++;
	     
if(file == 0)
{
sscanf(fileline, "%16s <-> [ %*50[^:]: %500[^,],", filetime, fileword);
fileword[strlen(fileword) -2] = '\0';
}	   
else if(file == 1)
{
sscanf(fileline, "%16s <-> [ %200[^,],", filetime, fileword);
fileword[strlen(fileword) -2] = '\0';
}	   

if((match_with_wildcards(fileword, rline) != 0))
{
matches++;
if((user->type == ADMIN) || (user->type == OWNER) || (mystristr(fileline, usernick) != NULL))
{
remove_line_from_file(fileword, path, 0);
if(place == 1)
uprintf(user, "$To: %s From: %s $", user->nick, botnick);
uprintf(user, "<%s> %s ", botnick, lp_removedentryfrom);  
if(file == 0)
uprintf(user, "releaselist -> %s|", fileword);  
else
uprintf(user, "reqlist -> %s|", fileword);  
}
else
{
if(place == 1)
uprintf(user, "$To: %s From: %s $", user->nick, botnick);
uprintf(user, "<%s> %s ", botnick, lp_couldntremoveentry);  
if(file == 0)
uprintf(user, "releaselist -> %s|", fileword);  
else
uprintf(user, "reqlist -> %s|", fileword);  
}
}
	  }
     }
   set_lock(fd, F_UNLCK);
    
    if(matches == 0)
    {
    if(place == 1)
    uprintf(user, "$To: %s From: %s $", user->nick, botnick);
    if(file == 0)
    uprintf(user, "<%s> %s releaselist -> %s|", botnick, lp_entrywasntfound, rline);
    else
    uprintf(user, "<%s> %s reqlist -> %s|", botnick, lp_entrywasntfound, rline);
    }

   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In remove_entry()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
	return;
}

int change_nick(char *nick1, char *nick2)
{
   FILE *fp;
   FILE *newfp;
   int fd;
   int erret;
   int newfd;
   char *newfile;
   char somenick[51];
   int sometype;
   char fileline[1024];
   char file[MAX_FDP_LEN+1];
   time_t now_time;
    
   now_time = time(NULL);    
   sometype = 0;

   snprintf(file, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);

   if((newfile = malloc(strlen(file) + 2)) == NULL)
     {
	logprintf(0, "Error - In update_exp()/malloc(): %s", strerror(errno));
	quit = 1;
	return -1;
     }   

   strcpy(newfile, file);
   strcat(newfile, "1");
   while(((fd = open(file, O_RDWR)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/open(): Interrupted system call. Trying again.\n");
   
   if(fd < 0)
     {	
	logprintf(0, "Error - In update_exp()/open(), file = %s: %s", file, strerror(errno));
	free(newfile);
	return -1;
     }

   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {	
	logprintf(0, "Error - In update_exp(): Couldn't set file lock, file = %s\n", file);
	close(fd);
	free(newfile);
	return -1;
     }

   if((fp = fdopen(fd, "r")) == NULL)
     {	
	logprintf(0, "Error - In update_exp()/fdopen(), file = %s: %s", file, strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	free(newfile);
	return -1;
     }


   unlink(newfile);
   
   while(((newfd = open(newfile, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/open(): Interrupted system call. Trying again.\n");   
   
   if(newfd < 0)
     {
	logprintf(0, "Error - In update_exp()/open(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
	  logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
	
	if(erret != 0)
	  {
	     logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	     free(newfile);
	     return -1;
	  }
       
	free(newfile);
	return -1;
     }
   
   if(set_lock(newfd, F_WRLCK) == 0)
     {
	logprintf(0, "Error - In update_exp(): Couldn't set file lock, file = %s\n", newfile);
	set_lock(fd, F_UNLCK);
	fclose(fp);
	close(newfd);
	free(newfile);
	return -1;
     }
   if((newfp = fdopen(newfd, "w")) == NULL)
     {
	logprintf(0, "Error - In updated_exp()/fdopen(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	fclose(fp);
	set_lock(newfd, F_UNLCK);
	close(newfd);
	free(newfile);
	return -1;
     }

   while(fgets(fileline, 1023, fp) != NULL)
     {	
	sscanf(fileline, "%50s", somenick);
	if(strcasecmp(somenick, nick1) == 0)
	fprintf(newfp, "%s %s", nick2, fileline + strlen(somenick) + 1);
	else	
	fprintf(newfp, "%s", fileline);
    }
   set_lock(newfd, F_UNLCK);
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(newfp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	free(newfile);
	return -1;
     }
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	free(newfile);
	return -1;
     }
   rename(newfile, file);
   free(newfile);
    
   return 0;
}

int change_class(char *nick1, int newtype)
{
   FILE *fp;
   FILE *newfp;
   int fd;
   int erret;
   int newfd;
   char *newfile;
   char somenick[51];
   char somepass[51];
   int sometype;
   char fileline[1024];
   char file[MAX_FDP_LEN+1];
   time_t now_time;
    
   now_time = time(NULL);    
   sometype = 0;

   snprintf(file, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);

   if((newfile = malloc(strlen(file) + 2)) == NULL)
     {
	logprintf(0, "Error - In update_exp()/malloc(): %s", strerror(errno));
	quit = 1;
	return -1;
     }   

   strcpy(newfile, file);
   strcat(newfile, "1");
   while(((fd = open(file, O_RDWR)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/open(): Interrupted system call. Trying again.\n");
   
   if(fd < 0)
     {	
	logprintf(0, "Error - In update_exp()/open(), file = %s: %s", file, strerror(errno));
	free(newfile);
	return -1;
     }

   /* Set the lock */
   if(set_lock(fd, F_WRLCK) == 0)
     {	
	logprintf(0, "Error - In update_exp(): Couldn't set file lock, file = %s\n", file);
	close(fd);
	free(newfile);
	return -1;
     }

   if((fp = fdopen(fd, "r")) == NULL)
     {	
	logprintf(0, "Error - In update_exp()/fdopen(), file = %s: %s", file, strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
	free(newfile);
	return -1;
     }


   unlink(newfile);
   
   while(((newfd = open(newfile, O_RDWR | O_CREAT, 0600)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/open(): Interrupted system call. Trying again.\n");   
   
   if(newfd < 0)
     {
	logprintf(0, "Error - In update_exp()/open(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	while(((erret = fclose(fp)) != 0) && (errno == EINTR))
	  logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
	
	if(erret != 0)
	  {
	     logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	     free(newfile);
	     return -1;
	  }
       
	free(newfile);
	return -1;
     }
   
   if(set_lock(newfd, F_WRLCK) == 0)
     {
	logprintf(0, "Error - In update_exp(): Couldn't set file lock, file = %s\n", newfile);
	set_lock(fd, F_UNLCK);
	fclose(fp);
	close(newfd);
	free(newfile);
	return -1;
     }
   if((newfp = fdopen(newfd, "w")) == NULL)
     {
	logprintf(0, "Error - In updated_exp()/fdopen(), file = %s: %s", newfile, strerror(errno));
	set_lock(fd, F_UNLCK);
	fclose(fp);
	set_lock(newfd, F_UNLCK);
	close(newfd);
	free(newfile);
	return -1;
     }

   while(fgets(fileline, 1023, fp) != NULL)
     {	
	sscanf(fileline, "%50s %50s %d", somenick, somepass, &sometype);
	if(strcasecmp(somenick, nick1) == 0)
	fprintf(newfp, "%s %s %d %s", somenick, somepass, newtype, fileline + strlen(somenick) + 1 + strlen(somepass) + 3);
	else	
	fprintf(newfp, "%s", fileline);
    }
   set_lock(newfd, F_UNLCK);
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(newfp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	free(newfile);
	return -1;
     }
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In update_exp()/fclose(): Interrupted system call. Trying again.\n");
   
   if(erret != 0)
     {
	logprintf(0, "Error - In update_exp()/fclose(): %s", strerror(errno));
	free(newfile);
	return -1;
     }
   rename(newfile, file);
   free(newfile);
    
   return 0;
}
