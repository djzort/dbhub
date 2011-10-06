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

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netdb.h>
#if HAVE_MALLOC_H
# include <malloc.h>
#endif
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif
#include <signal.h>
#include <sys/un.h>
#include <errno.h>
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
#ifdef SWITCH_USER
# include <sys/capability.h>
# include <sys/prctl.h>
# include <pwd.h>
# include <grp.h>
#endif
#include <sys/shm.h>

#include "main.h"
#include "lang.h"
#include "network.h"
#include "commands.h"
#include "utils.h"
#include "fileio.h"
#include "userlist.h"
#include "motd.h"
#include "logger.h"
#include "langvars.h"
#include "semops.h"

#ifdef HAVE_PERL
# include "perl_utils.h"
#endif

#ifndef SIGCHLD
# define SIGCHLD SIGCLD
#endif

#include <getopt.h>

#ifdef HAVE_EXECINFO_H
#include "stacktrace.h"
#endif

/* Set default variables, used if config does not exist or is bad */
int set_default_vars(void)
{
	sprintf(hub_name, "DB Hub");
	listening_port = 4111;
	sprintf(hub_description, "http://www.dbhub.org");
	max_users = 1000;	
	registered_only = 0;
	default_pass[0] = '\0';
	default_prefixes[0] = '\0';
	sprintf(nickchars, "!%%&()*+,-./0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{}");
	sprintf(forbchars, "#!");
	min_share = 0;
	min_psv_share = 0;
	min_reg_share = 0;
	min_vip_share = 0;
	min_kvip_share = 0;
	min_op_share = 0;
	kick_on_zeroshare = 0;
	gag_on_minshare = 0;
	max_share = 500;	
	sprintf(allowed_clients, "++ DCGUI LDC++ Z++ UKC++ PWDC++ PWSDC++ rmDC++ StrgDC++ ApexDC++ microdc2 SP");
	check_class = 0;
	min_command_class = 0;
	min_usercommand_class = 0;
	count_reghubs = 0;
	maxhubs = 100;
	minslots = 1;
	maxslots = 20;
	reverse_ratio = 0;
	ratio = 100;
	sprintf(mindcppver, "0.401");
	min_nick_len = 1;
	deny_passive = 0;
	deny_proxy = 0;
	restrict_mode = 0;
	connlimit = 0;
	sprintf(regmeprefix, "#[REG]");	
	sprintf(protectmeprefix, "[x]");	
	hidetags = 0;
	hide_allshare = 0;
	show_original_desc = 1;
	show_class_in_desc = 0;
	show_mode_in_desc = 1;
	show_original_mail = 1;
	mute_unregistered = 0;
	restrictions = 0;
	punishment = 1;
	kick_bantime = 5;	
	tempban = 10;	
	max_warnings = 5;	
	no_chat_for_chatgagged = 0;
	ban_overrides_allow = 0;
	max_regular_from_one_ip = 0;
	min_search_len = 3;
	searchspam_time = 5;
	deny_main_http = 1;
	deny_pm_http = 1;
	ipgag_type = 0;
	banwords = NULL;
	spam_action = 2;
	flood_action = 1;
	flood_lines = 5;
	slotbeg_kicker = 0;
	kick_for_incomplete = 0;
	seen_class = 0;
	cut_string_to = 512;
	cut_reg_msg = 0;
	offline_msgs = 2;
	self_registration = 0;
	min_regme_share = 0;
	protection = 0;	
	prottime = 14;
	sprintf(botnick, "Security");
	sprintf(opchatnick, "OpChat");
	clockbot = 1;
	count_bots = 1;
	hide_opshare = 0;
	who_can_addrls = 1;
	who_can_seerls = 0;
	max_releases_to_show = 10;
	who_can_addreq = 1;
	who_can_seereq = 0;
	max_requests_to_show = 10;
	expiredtime = 90;
	expiredrls = 90;		
	expiredreq = 90;		
	expiredseen = 30;		
	expiredurl = 90;	
	expiredwarn = 14;
	autofav = 0;
	send_scheduled = 0;
	crypt_enable = 0;
	log_main_chat = 1;
	verbosity = 0;
	config_on_priv = 0;
	help_on_priv = 0;
	rules_on_priv = 0;
	regme_on_priv = 0;
	url_on_priv = 0;
	rls_on_priv = 0;
	req_on_priv = 0;
	show_himsg = 1;
	show_onjoinmsgs = 1;
	show_records = 1;
	show_uptime = 1;
	show_lastmsgs = 1;
	show_releases = 0;
	show_requests = 0;
	show_welcomes = 1;
	show_joinsparts = 0;
	show_kicksbans = 0;
	show_denied = 0;
	show_actions = 3;
	show_notifications = 2;
	show_helpcommand_usage = 0;
	extra_commands = 0;
	extra_files[0] = '\0';
	sprintf(redirect_host_kickban, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_minshare, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_maxshare, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_maxhubs, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_minslots, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_maxslots, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_ratio, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_client, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_connlimit, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_hubfull, "sadistic.no-ip.org:6669");
	sprintf(redirect_host_regonly, "sadistic.no-ip.org:6669");
        sprintf(banmsg, "You are banned on this hub !");
	if((hub_full_msg = realloc(hub_full_msg, sizeof(char) * 50)) == NULL)
	{
		logprintf(LOG_EMERG,"Error - In set_default_vars()/realloc(): %s",strerror(errno));
		quit = 1;
		return 0;
	}
	sprintf(hub_full_msg, "Sorry, this hub is full at the moment");
        sprintf(regonlymsg, "Sorry, only registered user can join this hub");
	sprintf(topic, "Welcome to the DB Hub");
        sprintf(welcomereg, "has joined the hub");        
        sprintf(welcomecheef, "has joined the hub");        
        sprintf(welcomevip, "has joined the hub");        
        sprintf(welcomekvip, "has joined the hub");        
        sprintf(welcomeop, "has joined the hub");        
        sprintf(welcomecheef, "has joined the hub");        
        sprintf(welcomeadmin, "has joined the hub");        
        sprintf(partmsg, "has left the hub");        
	block_main = 0;
	block_pm = 0;
	block_download = 0;
	block_search = 0;
	return 1;
}

/* When all users have left a forked process, that process should be terminated */
void kill_forked_process(void)
{
	int erret;

	set_listening_pid(0);
	remove_all(0xFFFF, 1, 1);

	if(listening_socket != -1) 
	{	
		while(((erret =  close(listening_socket)) != 0) && (errno == EINTR))
			logprintf(LOG_EMERG, "Error - In kill_forked_process()/close(): Interrupted system call. Trying again.\n");	

		if(erret != 0)
		{	
			logprintf(LOG_EMERG, "Error - In kill_forked_process()/close(): %s",strerror(errno));
		}
	}
exit(EXIT_SUCCESS);
}


/* Accept connection from newly created forked process */
void new_forked_process(void)
{
	struct user_t *user;
	struct sockaddr_un remote_addr;
	int flags;
	socklen_t len;

	memset(&remote_addr, 0, sizeof(struct sockaddr_un));
	/* Allocate space for the new user */
	if((user = malloc(sizeof(struct user_t))) == NULL)
	{	
		logprintf(LOG_EMERG,"Error - In new_forked_process()/malloc(): %s", strerror(errno));
		quit = 1;
		return;
	}      

	/* Get a new socket for the connected user */
	len = sizeof(struct sockaddr_un);
	while(((user->sock = accept(listening_unx_socket,
		(struct sockaddr *)&remote_addr, &len)) < 0)
		&& (errno == EINTR))
		logprintf(LOG_EMERG, "Error - In new_forked_process()/accept(): Interrupted system call. Trying again.\n");	

	if(user->sock < 0)
	{	
		logprintf(LOG_EMERG, "Error - In new_forked_process()/accept(): %s", strerror(errno));
		free(user);
		return;
	}

	if((flags = fcntl(user->sock, F_GETFL, 0)) < 0)
	{  
		logprintf(LOG_EMERG, "Error - In new_forked_process()/in fcntl(): %s", strerror(errno));
		close(user->sock);
		free(user);
		return;
	} 

	/* Non blocking mode */
	if(fcntl(user->sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		logprintf(LOG_EMERG,  "Error - In new_forked_process()/in fcntl(): %s",strerror(errno));
		close(user->sock);
		free(user);
		return;
	}


	user->type = FORKED;
	user->rem = 0;
	user->con_type = NULL;
	user->desc = NULL;
	user->myinfos = NULL;
	user->myinfons = NULL;
	user->myinfoons = NULL;
	user->email = NULL;
	user->buf = NULL;
	user->outbuf = NULL;
	snprintf(user->hostname, 50, "forked_process");   
	memset(user->nick, 0, MAX_NICK_LEN+1);

	/* Add the user at the first place in the list.  */
	add_non_human_to_list(user);

	logprintf(LOG_NOTICE, "Got new unix connection on sock %d\n", user->sock);
}


/* Create a new process */
void fork_process(void)
{
	int sock;
	int len;
	int erret;
	struct sockaddr_un remote_addr;
	struct user_t *user;
	int flags;

	memset(&remote_addr, 0, sizeof(struct sockaddr_un));
	if((pid = fork()) == -1)
	{
		logprintf(LOG_EMERG, "Fork failed, exiting process: %s",strerror(errno));
		quit = 1;
		return;
	}

	/* If we are the parent */
	if(pid > 0)
	{
		/* All users are removed from the parent */
		remove_all(NON_LOGGED | REGULAR | PROTECTED | REG | VIP | KVIP | OP 
			| CHEEF | ADMIN | OWNER, 1, 1); 
		logprintf(LOG_EMERG, "Forked new process, childs pid is %d and parents pid is %d\n", pid, getpid());
		/* And set current pid of process */
		pid = getpid();
	}

	/* And if we are the child */
	else
	{
		/* Close the listening sockets */
		while(((erret =  close(listening_unx_socket)) != 0) && (errno == EINTR))
			logprintf(LOG_EMERG, "Error - In fork_process()/close(): Interrupted system call. Trying again.\n");	

		if(erret != 0)
		{	
			logprintf(LOG_EMERG, "Error - In fork_process()/close(): %s", strerror(errno));
		}

		while(((erret =  close(listening_udp_socket)) != 0) && (errno == EINTR))
			logprintf(LOG_EMERG, "Error - In fork_process()/close(): Interrupted system call. Trying again.\n");	

		if(erret != 0)
		{	
			logprintf(LOG_EMERG, "Error - In fork_process()/close(): %s", strerror(errno));
		}

		/* Set the alarm */
		alarm(ALARM_TIME);

		/* And remove all connections to forked process. We only want 
		* connections between parent and child, not between children. Also
		* remove connections to other hubs, we let the parent take care of
		* those.*/
		remove_all(SCRIPT | FORKED, 0, 0);

		/* If some other process already has opened the socket, we'll exit.  */
		if(set_listening_pid((int)getpid()) <= 0)
			exit(EXIT_SUCCESS);

		/* Open the human listening sockets.  */
		if((listening_socket = get_listening_socket(listening_port, 0)) == -1)
		{
			logprintf(LOG_EMERG, "Error - In fork_process(): Couldn't open listening socket\n");
			quit = 1;
		}


		/* And connect to parent process */
		if((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
		{
			logprintf(LOG_EMERG, "Error - In fork_process()/socket(): %s", strerror(errno));
			quit = 1;
			return;
		}		

		remote_addr.sun_family = AF_UNIX;
		strcpy(remote_addr.sun_path, un_sock_path);
		len = strlen(remote_addr.sun_path) + sizeof(remote_addr.sun_family) + 1;
		if(connect(sock, (struct sockaddr *)&remote_addr, len) == -1)
		{
			logprintf(LOG_EMERG, "Error - In fork_process()/connect(): %s", strerror(errno));
			quit = 1;
			return;
		}

		if((user = malloc(sizeof(struct user_t))) == NULL)
		{	     
			logprintf(LOG_EMERG, "Error - In fork_process()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}

		user->sock = sock;
		user->type = FORKED;
		user->rem = 0;
		user->buf = NULL;
		user->outbuf = NULL;
		memset(user->nick, 0, MAX_NICK_LEN+1);
		snprintf(user->hostname, 50, "parent_process");

		if((flags = fcntl(user->sock, F_GETFL, 0)) < 0)
		{     
			logprintf(LOG_EMERG, "Error - In fork_process()/in fcntl(): %s", strerror(errno));
			close(user->sock);
			free(user);
			return;
		}

		/* Non blocking mode */
		if(fcntl(user->sock, F_SETFL, flags | O_NONBLOCK) < 0)
		{
			logprintf(LOG_EMERG, "Error - In fork_process()/in fcntl(): %s", strerror(errno));
			close(user->sock);
			free(user);
			return;
		}


		/* Add the user at the first place in the list */
		add_non_human_to_list(user);
	}
}

/* Removes all users of specified type.  */
void remove_all(int type, int send_quit, int remove_from_list)
{
	struct sock_t *human_user;
	struct user_t *non_human;
	struct sock_t *next_human;
	struct user_t *next_non_human;

	human_user = human_sock_list;
	non_human = non_human_user_list;

	/* First non-humans.  */
	while(non_human != NULL)
	{
		next_non_human = non_human->next;

		if((non_human->type & type) != 0)
			remove_user(non_human, send_quit, remove_from_list);

		non_human = next_non_human;
	}   
	while(human_user != NULL)
	{
		next_human = human_user->next;

		if((human_user->user->type & type) != 0)
			remove_user(human_user->user, send_quit, remove_from_list);

		human_user = next_human;
	}
}

void term_signal(int z)
{
	z=z;
	quit = 1;
}

/* This will execute every ALARM_TIME seconds, it checks timeouts and uploads 
* to public hublist */

void alarm_signal(int z){
	z=z;
	alarmflag=1;
}

void alarm_handler(void)
{
	struct sock_t *human_user;
	char sometxt[250];
	char somerec[21];
	char showrec[251];
	char recpath[MAX_FDP_LEN+1];
	char line[201];
	char fscheck[251];
	long long  numberrec;
	time_t now;

	now=time(NULL);
	if(pid == 0)
	{

	if(get_uptime() > 30)	
	actch++;

	if(debug != 0)
	logprintf(LOG_CRIT, "Got alarm signal\n");

	if(actch == 3)
	{
		    if(isupt > autofav)
		    isupt = 0;
		    if(autofav > 0)
		    isupt++; 	 
		    if(isupt2 > send_scheduled)
		    isupt2 = 0;
		    if(send_scheduled > 0)
		    isupt2++; 	 
	}
        
	human_user = human_sock_list;
	
	while(human_user != NULL)
	{	
		//DB// Check timeouts
		if(human_user->user->type == NON_LOGGED) 
		{
		    if(now - human_user->user->jointime > 30)
		    {
			logprintf(LOG_CRIT, "Timeout for non logged in user at %s, removing user\n", human_user->user->hostname);
			human_user->user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		    }
		}
		else
		{
		    if(actch == 3)
		    {
			if((autofav > 0) && (isupt == autofav) && (human_user->user->type == REGULAR))
                	{ 	 
                	uprintf(human_user->user, "$To: %s From: %s $<%s> ", human_user->user->nick, botnick, botnick); 	 
                	send_to_user(autofav_mem, human_user->user); 	 
                	send_to_user("|", human_user->user); 	 
			}
		    if((send_scheduled > 0) && (isupt2 == send_scheduled))
		    {
        		uprintf(human_user->user, "<%s> \n==================================================\n\n", botnick); 	 
                	send_to_user(schedule_mem, human_user->user); 	 
                	send_to_user("\n\n==================================================|", human_user->user); 	 
		    }
		    }
		}    
		human_user = human_user->next;
	}

	do_purge_user_list = 1;
	if(isupt == autofav)
	isupt = 0;
	if(isupt2 == send_scheduled)
	isupt2 = 0;

	    //DB// If record was broken, write it to file and send note to all users
	    if(show_records == 1)
	    {        
	    snprintf(recpath, MAX_FDP_LEN, "%s/%s", config_dir, RECORDS_FILE);
		if(brokenur == 1)
		{
    		snprintf(line, 200, "users");
    		remove_line_from_file(line, recpath, 0);
    		snprintf(line, 200, "users %d %s", recordusers, usdatetime);
    		add_line_to_file(line, recpath);
		check_if_record("users");
		sscanf(recordusstr, "%20s ", somerec); 
		snprintf(sometxt, 150, "%s", recordusstr + strlen(somerec) + 1);
		snprintf(showrec, 250, "<%s> %s: %s %s @ %s|", botnick, lp_recordusers, somerec, lp_users, sometxt);
		send_to_humans(showrec, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}
		if(brokensr == 1)
		{
	        snprintf(line, 200, "share");
	        remove_line_from_file(line, recpath, 0);
    		snprintf(line, 200, "share %llu %s", recordshare, shdatetime);
	        add_line_to_file(line, recpath);
		check_if_record("share");
		sscanf(recordshstr, "%20s ", somerec);
		sscanf(somerec, "%llu", &numberrec); 
		snprintf(sometxt, 250, "%s", recordshstr + strlen(somerec) + 1);
		    if(strlen(somerec) > 12)
		    snprintf(showrec, 250, "<%s> %s: %2.2f TiB @ %s|", botnick, lp_recordshare, ((double)numberrec / (1024*1024*1024)) / 1024, sometxt);
		    else
		    snprintf(showrec, 250, "<%s> %s: %2.2f GiB @ %s|", botnick, lp_recordshare, (double)numberrec / (1024*1024*1024), sometxt);
		send_to_humans(showrec, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
		}	
		brokenur = 0;
		brokensr = 0;
	    }

	    //DB// Searching for incomplete files
	    if(kick_for_incomplete == 1)
	    {
	    snprintf(fscheck, 250, "$MyINFO $ALL %s [BOT] Hub-Security <DB V:%s,M:P,H:0/0/1,S:0>$ $$hub-security$0$|", botnick, VERSION);
		if(actch == 1)
		sprintfa(fscheck, "$Search Hub:%s F?T?0?1?.dctmp|", botnick);
		if(actch == 2)
		sprintfa(fscheck, "$Search Hub:%s F?T?0?1?INCOMPLETE|", botnick);
		if(actch == 3)
		sprintfa(fscheck, "$Search Hub:%s F?T?0?1?.part.met|$Search Hub:%s F?T?0?1?.bc!|", botnick, botnick);
	    sprintfa(fscheck, "$MyINFO $ALL %s [BOT] Hub-Security <DB V:%s,M:A,H:0/0/1,S:0>$ $$hub-security$0$|", botnick, VERSION);
	    send_to_humans(fscheck, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	    }
	
    
	/* Send the hub_timer sub to the scripts.  */
#ifdef HAVE_PERL
	if((actch == 3) || (get_uptime() < 30))
		command_to_scripts("$Script hub_timer|");
#endif
	if(actch == 3)
	actch = 0;

	update_exp();
	loadfile2buf(&reglist_mem,REG_FILE);
	remove_expired();

	alarm(ALARM_TIME);
    }
}

void init_sig(void)
{  
	struct sigaction sv;  

	memset(&sv, 0, sizeof(struct sigaction));
	sv.sa_flags = 0;
	sigemptyset(&sv.sa_mask);
#ifdef SA_NOCLDWAIT
	sv.sa_flags |= SA_NOCLDWAIT;
#endif
#ifdef SA_NOCLDSTOP
	sv.sa_flags |= SA_NOCLDSTOP;
#endif

	sv.sa_handler = SIG_IGN;
	/* Don't want broken pipes to kill the hub.  */
	sigaction(SIGPIPE, &sv, NULL);

	/* ...or any defunct child processes.  */
	sigaction(SIGCHLD, &sv, NULL);

	sv.sa_handler = term_signal;

	/* Also, shut down properly.  */
	sigaction(SIGTERM, &sv, NULL);
	sigaction(SIGINT, &sv, NULL);

	sv.sa_handler = alarm_signal;

	/* And set handler for the alarm call.  */
	sigaction(SIGALRM, &sv, NULL);   
#ifdef HAVE_EXECINFO_H
	/* Handler for misc crashes: */
	sv.sa_handler = logbacktrace;
	sigaction(SIGSEGV, &sv, NULL);
	sigaction(SIGBUS, &sv, NULL);
#endif
}

/* Send info about one user to another. If all is 1, send to all */
int send_user_info(struct user_t *from_user, char *to_user_nick)
{
	char *olds; char *oldns; char *oldons;
	char *send_buf;
	double userratio;
	char *nosend_buf; 
	char *opnosend_buf;
	struct user_t *to_user;
	char bpath[MAX_FDP_LEN+1];
	char kbstring[512];
	char nbpath[MAX_FDP_LEN+1];
	struct tm * timeinfo;
	char recpath[MAX_FDP_LEN+1];
	int hubs, l, allusers;
	int slots;
	int reghubs = 0;
	char acttime[51];
	char ban_command[MAX_HOST_LEN+4];
	char nban_command[MAX_HOST_LEN+4];
	char clientbuf[MAX_HOST_LEN+1];
	char sclientbuf[MAX_MSG_LEN+1];
	char cversion[51];
	char cmode[2];
	char brstring[401];
	char nbrstring[401];
	char sdayn[6];
	char sday[4];
	char sdate[11];
	char syear[6];
	char smont[6];
	char *temp;
	char *temp3;
	char tempclass[51];
	char client[21];
	int dcversion;
	int mindcver;
	char *true_buf;
	long long allshare;
	char usercheckpsv[53];
	char *llimit;
	int userlimit = 0;
	time_t now;
	int prmode;
	
	now = time(NULL);
        snprintf(bpath, MAX_FDP_LEN, "%s/%s", config_dir, BANRS_FILE);
	snprintf(nbpath, MAX_FDP_LEN, "%s/%s", config_dir, NICKBANRS_FILE);
        snprintf(recpath, MAX_FDP_LEN, "%s/%s", config_dir, RECORDS_FILE);
	
	srand( (unsigned)time( NULL ) );
	        l = rand()%10;
	
    prmode = from_user->mode;

   if((send_buf = malloc(sizeof(char) * 14)) == NULL)
     {
	logprintf(LOG_EMERG, "Error - In get_op_list()/malloc()/send_buf: %s", strerror(errno));
	quit = 1;
	return 0;
     }

   if((opnosend_buf = malloc(sizeof(char) * 14)) == NULL)
     {
	logprintf(LOG_EMERG, "Error - In send_user_info()/malloc()/opnosend_buf: %s", strerror(errno));
	quit = 1;
	return 0;
     }

   if((nosend_buf = malloc(sizeof(char) * 14)) == NULL)
     {
	logprintf(LOG_EMERG, "Error - In send_user_info()/malloc()/nosend_buf: %s", strerror(errno));
	quit = 1;
	return 0;
     }

   if((true_buf = malloc(sizeof(char) * 14)) == NULL)
     {
	logprintf(LOG_EMERG, "Error - In send_user_info()/malloc()/true_buf: %s", strerror(errno));
	quit = 1;
	return 0;
     }

	sprintf(send_buf, "$MyINFO $ALL ");
	sprintf(nosend_buf, "$MyINFO $ALL ");
	sprintf(true_buf, "$MyINFO $ALL ");
	sprintf(opnosend_buf, "$MyINFO $ALL ");

	
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + strlen(from_user->nick) + 2))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + strlen(from_user->nick) + 2))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + strlen(from_user->nick) + 2))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((true_buf = realloc(true_buf, sizeof(char) * (strlen(true_buf) + strlen(from_user->nick) + 2))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/true_buf: %s", strerror(errno));
quit = 1;
return 0;
}

	sprintfa(send_buf, "%s", from_user->nick);
	sprintfa(opnosend_buf, "%s", from_user->nick); 
	sprintfa(nosend_buf, "%s", from_user->nick); 
	sprintfa(true_buf, "%s", from_user->nick); 
	sprintfa(send_buf, " ");
	sprintfa(opnosend_buf, " "); 
	sprintfa(nosend_buf, " "); 
	sprintfa(true_buf, " "); 


        if(((from_user->type == REGULAR)
	|| ((check_class >= 1) && (from_user->type == REG)) // REG
	|| ((check_class >= 2) && (from_user->type == VIP)) // VIP
	|| ((check_class >= 3) && (from_user->type == KVIP)) // KVIP
	|| ((check_class >= 4) && (from_user->type == OP))) // OP	
	&& (from_user->desc == NULL) && ((strncmp(from_user->hostname, "parent_process", 14) != 0)) && ((strncmp(from_user->hostname, "forked_process", 14) != 0)) && ((strncmp(from_user->hostname, "script_process", 14) != 0)))
        {
        if(from_user->type == REGULAR)
	{
	uprintf(from_user, "<%s> %s %s: %s|", botnick, lp_youwerebanned, lp_because, lp_lackoftag);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s|", from_user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_lackoftag);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_ban_, lp_lackoftag, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	else
	{
	uprintf(from_user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_lackoftag);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_lackoftag);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_kick_, lp_lackoftag, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
        sprintf(ban_command, "%s %dm", from_user->hostname, 0);
        ballow(ban_command, BAN, from_user);
        sprintf(nban_command, "%s %dm", from_user->nick, 0);
        ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "0 %s [ %s ]", from_user->hostname, lp_lackoftag);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "0 %s [ %s ]", from_user->nick, lp_lackoftag);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s", from_user->nick, from_user->hostname, lp_lackoftag);
	last_perm(lastbuf);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(send_buf);
	free(opnosend_buf);
	free(nosend_buf);
	free(true_buf);
	return 0;
	}
	if(from_user->desc != NULL)
	{
	if(((from_user->type == REGULAR)
	|| ((check_class >= 1) && (from_user->type == REG)) // REG
	|| ((check_class >= 2) && (from_user->type == VIP)) // VIP
	|| ((check_class >= 3) && (from_user->type == KVIP)) // KVIP
	|| ((check_class >= 4) && (from_user->type == OP)) // OP	
	) && ((strncmp(from_user->hostname, "parent_process", 14) != 0)) && ((strncmp(from_user->hostname, "script_process", 14) != 0)) && ((strncmp(from_user->hostname, "script_process", 14) != 0)))
	{
	if ((temp=strrchr(from_user->desc,'<')) != NULL)
	{ 
	if(sscanf(temp,"<%20s V:%50[^,],M:%2[^,],H:%d%*[^S]S:%d*s>", client, cversion, cmode, &hubs, &slots) != 5)
	{
	if(from_user->type == REGULAR)
	{
	uprintf(from_user, "<%s> %s %s: %s ( %s )|", botnick, lp_youwerebanned, lp_because, lp_faketag, temp);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %s )|", from_user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_faketag, temp);
	sprintf(kbstring, "[ %s ] [ %s : %s ] [ %s (IP: %s) ]", lp_ban_, lp_faketag, temp, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %s )|", botnick, lp_youwerekicked, lp_because, lp_faketag, temp);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %s )|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_faketag, temp);
	sprintf(kbstring, "[ %s ] [ %s : %s ] [ %s (IP: %s) ]", lp_kick_, lp_faketag, temp, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	sprintf(ban_command, "%s %dm", from_user->hostname, 0);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, 0);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "0 %s [ %s : %s ]", from_user->hostname, lp_faketag, temp);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "0 %s [ %s : %s ]", from_user->nick, lp_faketag, temp);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s : %s", from_user->nick, from_user->hostname, lp_faketag, temp);
	last_perm(lastbuf);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	return 0;
	}
	temp3=strstr(temp,"H:");
	if((sscanf(temp3, "H:%d/%d", &hubs, &reghubs) == 2) && (count_reghubs == 1))
	hubs = hubs + reghubs;
    	if((strncmp(cmode, "A", 1) != 0) && (strncmp(cmode, "P", 1) != 0) && (strncmp(cmode, "5", 1) != 0))
	{
        if(from_user->type == REGULAR)
	{
	uprintf(from_user, "<%s> %s %s: %s ( %s )|", botnick, lp_youwerebanned, lp_because, lp_faketag, temp);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %s )|", from_user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_faketag, temp);
	sprintf(kbstring, "[ %s ] [ %s : %s ] [ %s (IP: %s) ]", lp_ban_, lp_faketag, temp, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %s )|", botnick, lp_youwerekicked, lp_because, lp_faketag, temp);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %s )|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_faketag, temp);
	sprintf(kbstring, "[ %s ] [ %s : %s ] [ %s (IP: %s) ]", lp_kick_, lp_faketag, temp, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	sprintf(ban_command, "%s %dm", from_user->hostname, 0);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, 0);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "0 %s [ %s : %s ]", from_user->hostname, lp_faketag, temp);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "0 %s [ %s : %s ]", from_user->nick, lp_faketag, temp);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s : %s", from_user->nick, from_user->hostname, lp_faketag, temp);
	last_perm(lastbuf);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	return 0;
	}
	else
	{
	if(strncmp(cmode, "A", 1) != 0)
	from_user->mode = 2;
	else
	from_user->mode = 1;
	if(prmode != from_user->mode)
	newmyinfo = 1;
	}
	sprintf(clientbuf, " %s ", client);
	sprintf(sclientbuf, " %s  ", allowed_clients);
	if (((strlen(allowed_clients) != 0)) && ((search_in_string(sclientbuf, clientbuf) != 1)))
	{
	if((punishment == 1) && (from_user->type == REGULAR))
	{
	uprintf(from_user, "<%s> %s %s: %s ( %s ). %s: %s|", botnick, lp_youwerekicked, lp_because, lp_forbiddenclient, client, lp_allowedclients, allowed_clients);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s: ( %s ). %s %s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_forbiddenclient, client, lp_allowedclients, allowed_clients);
	if(tempban > 0)
	{
	sprintf(kbstring, "[ %s %d %s ] [ %s : %s ] [ %s (IP: %s) ]", lp_ban_, tempban, lp_minutes, lp_forbiddenclient, client, from_user->nick, from_user->hostname);
	ban_info(kbstring);
        }
	else
	{
	sprintf(kbstring, "[ %s ] [ %s : %s ] [ %s (IP: %s) ]", lp_redir_, lp_forbiddenclient, client, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	}
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %s ). %s %s|", botnick, lp_youwerekicked, lp_because, lp_forbiddenclient, client, lp_allowedclients, allowed_clients);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s: ( %s ). %s %s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_forbiddenclient, client, lp_allowedclients, allowed_clients);
	sprintf(kbstring, "[ %s ] [ %s : %s ] [ %s (IP: %s) ]", lp_kick_, lp_forbiddenclient, client, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	if(tempban > 0)
	{
	sprintf(ban_command, "%s %dm", from_user->hostname, tempban);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, tempban);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "%lu %s [ %s: %s ]", now+(tempban*60), from_user->hostname, lp_forbiddenclient, client);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "%lu %s [ %s: %s ]", now+(tempban*60), from_user->nick, lp_forbiddenclient, client);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s: %s [ %s %d %s ]", from_user->nick, from_user->hostname, lp_forbiddenclient, client, lp_ban_, tempban, lp_minutes);
	last_temp(lastbuf);
	}
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_client);
	return 0; 
	}
        if(restrict_mode == 1) 
	{
	sprintf(usercheckpsv, " %s ", from_user->nick);
	if((search_in_string(usercheckpsv, "psv") == 1) && (strncmp(cmode, "A", 1) == 0)) 
	{
	uprintf(from_user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_psvwithmodea);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_psvwithmodea);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_kick_, lp_psvwithmodea, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_client);
	return 0; 
	}
	}
	if((deny_passive > 0) && (strncmp(cmode, "A", 1) != 0))
	{
	if(deny_passive == 1)
	{
	uprintf(from_user, "<%s> %s|", botnick, lp_psvnotallowed);
	uprintf(from_user, "$To: %s From: %s $<%s> %s|", from_user->nick, botnick, botnick, lp_psvnotallowed);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_kick_, lp_psvnotallowed, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_client);
	return 0; 
	}
	if(deny_passive == 2)
	{
	sprintf(usercheckpsv, " %s ", from_user->nick);
	if(search_in_string(usercheckpsv, "psv") == 0)
	{
	uprintf(from_user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_psvwithoutpsvinnick);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_psvwithoutpsvinnick);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_redir_, lp_psvwithoutpsvinnick, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_client);
	return 0; 
	}
	}
	}
	if((deny_proxy == 1) && (strncmp(cmode, "5", 1) == 0))
	{
	uprintf(from_user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_proxynotallowed);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_proxynotallowed);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_kick_, lp_proxynotallowed, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_client);
	return 0; 
	}	

	
	if((strncmp(client, "++", 2) == 0))
	{
	if(sscanf(cversion, "0.%d", &dcversion) != 1)
	{
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_kickban);
	return 0;
	}
	if(dcversion > 100000)
	{
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_kickban);
	return 0;
	}
	if(dcversion > 10000)
	dcversion = dcversion / 100;
	
	if(dcversion > 1000)
	dcversion = dcversion / 10;
	
	if(dcversion < 100)
	{
	dcversion = dcversion * 10;
	}
	sscanf(mindcppver, "0.%3d", &mindcver);	
	if(mindcver < 10)
	mindcver = mindcver * 100;
	if(mindcver < 100)
	mindcver = mindcver * 10;
	
	if(dcversion < mindcver)
	{
	if((punishment == 1) && (from_user->type == REGULAR))
	{
	uprintf(from_user, "<%s> %s %s: %s ( 0.%d ). %s: 0.%d|", botnick, lp_youwerekicked, lp_because, lp_tooolddcppver, dcversion, lp_requiredatleast, mindcver);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( 0.%d ). %s: 0.%d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_tooolddcppver, dcversion, lp_requiredatleast, mindcver);
	if(tempban > 0)
	{
	sprintf(kbstring, "[ %s %d %s ] [ %s : 0.%d ] [ %s (IP: %s) ]", lp_ban_, tempban, lp_minutes, lp_tooolddcppver, dcversion, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
        }
	else
	{
	sprintf(kbstring, "[ %s ] [ %s : 0.%d ] [ %s (IP: %s) ]", lp_redir_, lp_tooolddcppver, dcversion, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	}
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( 0.%d ). %s: 0.%d|", botnick, lp_youwerekicked, lp_because, lp_tooolddcppver, dcversion, lp_requiredatleast, mindcver);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( 0.%d ). %s: 0.%d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_tooolddcppver, dcversion, lp_requiredatleast, mindcver);
	sprintf(kbstring, "[ %s ] [ %s : 0.%d ] [ %s (IP: %s) ]", lp_kick_, lp_tooolddcppver, dcversion, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	if(tempban > 0)
	{
	sprintf(ban_command, "%s %dm", from_user->hostname, tempban);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, tempban);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "%lu %s [ %s ( 0.%d ). %s 0.%d ]", now + (60*tempban), from_user->hostname, lp_tooolddcppver, dcversion, lp_requiredatleast, mindcver);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "%lu %s [ %s ( 0.%d ). %s 0.%d ]", now + (60*tempban), from_user->nick, lp_tooolddcppver, dcversion, lp_requiredatleast, mindcver);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s ( 0.%d ) [ %s %d %s]", from_user->nick, from_user->hostname, lp_tooolddcppver, dcversion, lp_ban_, tempban, lp_minutes);
	last_temp(lastbuf);
	}
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_client);
	return 0;
	}
	}
	if ((hubs != 0) && (hubs > maxhubs))
	{
	if((punishment == 1) && (from_user->type == REGULAR))
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toomanyhubs, hubs, lp_allowedatmost, maxhubs);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toomanyhubs, hubs, lp_allowedatmost, maxhubs);
	if(tempban > 0)
	{
	sprintf(kbstring, "[ %s %d %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_ban_, tempban, lp_minutes, lp_toomanyhubs, hubs, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
        }
	else
	{
	sprintf(kbstring, "[ %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_redir_, lp_toomanyhubs, hubs, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
        }
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toomanyhubs, hubs, lp_allowedatmost, maxhubs);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toomanyhubs, hubs, lp_allowedatmost, maxhubs);
	sprintf(kbstring, "[ %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_kick_, lp_toomanyhubs, hubs, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
	}
	if(tempban > 0)
	{
	sprintf(ban_command, "%s %dm", from_user->hostname, tempban);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, tempban);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "%lu %s [ %s: %d. %s %d ]", now + (60*tempban), from_user->hostname, lp_toomanyhubs, hubs, lp_allowedatmost, maxhubs);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "%lu %s [ %s: %d. %s %d ]", now + (60*tempban), from_user->nick, lp_toomanyhubs, hubs, lp_allowedatmost, maxhubs);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s: %d [ %s %d %s ]", from_user->nick, from_user->hostname, lp_toomanyhubs, hubs, lp_ban_, tempban, lp_minutes);
	last_temp(lastbuf);
	}
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_maxhubs);
	return 0;
	}
	if (slots < minslots)
	{
	if((punishment == 1) && (from_user->type == REGULAR))
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_notenoughslots, slots, lp_requiredatleast, minslots);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_notenoughslots, slots, lp_requiredatleast, minslots);
	if(tempban > 0)
	{
	sprintf(kbstring, "[ %s %d %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_ban_, tempban, lp_minutes, lp_notenoughslots, slots, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
        }
	else
	{
	sprintf(kbstring, "[ %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_redir_, lp_notenoughslots, slots, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
	}
        }
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_notenoughslots, slots, lp_requiredatleast, minslots);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_notenoughslots, slots, lp_requiredatleast, minslots);
	sprintf(kbstring, "[ %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_kick_, lp_notenoughslots, slots, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
	}
	if(tempban > 0)
	{
	sprintf(ban_command, "%s %dm", from_user->hostname, tempban);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, tempban);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "%lu %s [ %s: %d. %s: %d ]", now + (60*tempban), from_user->hostname, lp_notenoughslots, slots, lp_requiredatleast, minslots);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "%lu %s [ %s: %d. %s: %d ]", now + (60*tempban), from_user->nick, lp_notenoughslots, slots, lp_requiredatleast, minslots);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s: %d [ %s %d %s ]", from_user->nick, from_user->hostname, lp_notenoughslots, slots, lp_ban_, tempban, lp_minutes);
	last_temp(lastbuf);	
	}
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
	free(send_buf);
	free(nosend_buf);
	free(true_buf);
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_minslots);
	return 0;
	}
	if ((slots > maxslots) && (maxslots != 0))
	{
	if(slots > 99)
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);	
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_maxslots);
	return 0;
	}
	else
	{
	if((punishment == 1) && (from_user->type == REGULAR))
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
	if(tempban > 0)
	{
	sprintf(kbstring, "[ %s %d %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_ban_, tempban, lp_minutes, lp_toomanyslots, slots, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
        }
	else
	{
	sprintf(kbstring, "[ %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_redir_, lp_toomanyslots, slots, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
	}
        }
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
	sprintf(kbstring, "[ %s ] [ %s : %d ] [ %s (IP: %s) ]", lp_kick_, lp_toomanyslots, slots, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
	}
	if(tempban > 0)
	{
	sprintf(ban_command, "%s %dm", from_user->hostname, tempban);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, tempban);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "%lu %s [ %s: %d. %s: %d ]", now + (60*tempban), from_user->hostname, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "%lu %s [ %s: %d. %s: %d ]", now + (60*tempban), from_user->nick, lp_toomanyslots, slots, lp_allowedatmost, maxslots);
	if(check_if_banned(from_user, NICKBANRS) == 0)
        add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s: %d [ %s %d %s ]", from_user->nick, from_user->hostname, lp_toomanyslots, slots, lp_ban_, tempban, lp_minutes);
	last_temp(lastbuf);
	}
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);	
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_maxslots);
	return 0;
	}
	}
	if(reverse_ratio == 1)
	{
	userratio = (double)slots / hubs;
	if((ratio != 0) && ((double)userratio < ratio))
	{
	if((punishment == 1) && (from_user->type == REGULAR))
	{
	uprintf(from_user, "<%s> %s %s: %s ( %2.2f ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toolowratio, (double)userratio, lp_requiredatleast, ratio);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %2.2f ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toolowratio, (double)userratio, lp_requiredatleast, ratio);
	if(tempban > 0)
	{
	sprintf(kbstring, "[ %s %d %s ] [ %s : %2.2f ] [ %s (IP: %s) ]", lp_ban_, tempban, lp_minutes, lp_toolowratio, (double)userratio, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
        }
	else
	{
	sprintf(kbstring, "[ %s ] [ %s : %2.2f ] [ %s (IP: %s) ]", lp_redir_, lp_toolowratio, (double)userratio, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
	}
        }
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %2.2f ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toolowratio, (double)userratio, lp_requiredatleast, ratio);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %2.2f ). %s %d|", from_user->nick, botnick, botnick, lp_toolowratio, lp_youwerekicked, lp_because, (double)userratio, lp_requiredatleast, ratio);
	sprintf(kbstring, "[ %s ] [ %s : %2.2f ] [ %s (IP: %s) ]", lp_kick_, lp_toolowratio, (double)userratio, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
	}
	if(tempban > 0)
	{
	sprintf(ban_command, "%s %dm", from_user->hostname, tempban);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, tempban);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "%lu %s [ %s: %2.2f. %s: %d ]", now + (60*tempban), from_user->hostname, lp_toolowratio, (double)userratio, lp_requiredatleast, ratio);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "%lu %s [ %s: %2.2f. %s: %d ]", now + (60*tempban), from_user->nick, lp_toolowratio, (double)userratio, lp_requiredatleast, ratio);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s: %2.2f [ %s %d %s ]", from_user->nick, from_user->hostname, lp_toolowratio, (double)userratio, lp_ban_, tempban, lp_minutes);
	last_temp(lastbuf);
	}
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);	
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_ratio);
	return 0;
	}
	}

	else
	{
	userratio = (double)hubs / slots;
	if((ratio != 0) && ((double)userratio > ratio))
	{
	if((punishment == 1) && (from_user->type == REGULAR))
	{
	uprintf(from_user, "<%s> %s %s: %s ( %2.2f ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toohighratio, (double)userratio, lp_allowedatmost, ratio);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %2.2f ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toohighratio, (double)userratio, lp_allowedatmost, ratio);
	if(tempban > 0)
	{
	sprintf(kbstring, "[ %s %d %s ] [ %s : %2.2f ] [ %s (IP: %s) ]", lp_ban_, tempban, lp_minutes, lp_toohighratio, (double)userratio, from_user->nick, from_user->hostname);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	ban_info(kbstring);
        }
	else
	{
	sprintf(kbstring, "[ %s ] [ %s : %2.2f ] [ %s (IP: %s) ]", lp_redir_, lp_toohighratio, (double)userratio, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
        }
	else
	{
	uprintf(from_user, "<%s> %s %s: %s ( %2.2f ). %s: %d|", botnick, lp_youwerekicked, lp_because, lp_toohighratio, (double)userratio, lp_allowedatmost, ratio);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %2.2f ). %s: %d|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_toohighratio, (double)userratio, lp_allowedatmost, ratio);
	sprintf(kbstring, "[ %s ] [ %s : %2.2f ] [ %s (IP: %s) ]", lp_kick_, lp_toohighratio, (double)userratio, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	if(tempban > 0)
	{
	sprintf(ban_command, "%s %dm", from_user->hostname, tempban);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, tempban);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "%lu %s [ %s: %2.2f. %s: %d ]", now + (60*tempban), from_user->hostname, lp_toohighratio, (double)userratio, lp_allowedatmost, ratio);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "%lu %s [ %s: %2.2f. %s: %d ]", now + (60*tempban), from_user->nick, lp_toohighratio, (double)userratio, lp_allowedatmost, ratio);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s: %2.2f [ %s %d %s ]", from_user->nick, from_user->hostname, lp_toohighratio, (double)userratio, lp_ban_, tempban, lp_minutes);
	last_temp(lastbuf);
	}
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);	
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_ratio);
	return 0;
	}
	}
	if((search_in_string(from_user->desc, ",L:") == 1) && (connlimit > 0))
	{
	llimit = strstr(temp, ",L:");
	sscanf(llimit+3, "%d", &userlimit);
	if(userlimit < connlimit)
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d kB/s ). %s: %d kB/s|", botnick, lp_youwerekicked, lp_because, lp_uploadlimittoolow, userlimit, lp_requiredatleast, connlimit);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d kB/s ). %s: %d kB/s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_uploadlimittoolow, userlimit, lp_requiredatleast, connlimit);
	sprintf(kbstring, "[ %s ] [ %s : %d kB/s ] [ %s (IP: %s) ]", lp_kick_, lp_uploadlimittoolow, userlimit, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);	
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_connlimit);
	return 0;
	}
	}
	if((search_in_string(from_user->desc, ",B:") == 1) && (connlimit > 0))
	{
	llimit = strstr(temp, ",B:");
	sscanf(llimit+3, "%d", &userlimit);
	if(userlimit < connlimit)
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d kB/s ). %s: %d kB/s|", botnick, lp_youwerekicked, lp_because, lp_uploadlimittoolow, userlimit, lp_requiredatleast, connlimit);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d kB/s ). %s: %d kB/s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_uploadlimittoolow, userlimit, lp_requiredatleast, connlimit);
	sprintf(kbstring, "[ %s ] [ %s : %d kB/s ] [ %s (IP: %s) ]", lp_kick_, lp_uploadlimittoolow, userlimit, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);	
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_connlimit);
	return 0;
	}
	}

	}
	else {
        if(from_user->type == REGULAR)
	{
	uprintf(from_user, "<%s> %s %s: %s|", botnick, lp_youwerebanned, lp_because, lp_lackoftag);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s|", from_user->nick, botnick, botnick, lp_youwerebanned, lp_because, lp_lackoftag);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_ban_, lp_lackoftag, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	else
	{
	uprintf(from_user, "<%s> %s %s: %s|", botnick, lp_youwerekicked, lp_because, lp_lackoftag);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_lackoftag);
	sprintf(kbstring, "[ %s ] [ %s ] [ %s (IP: %s) ]", lp_kick_, lp_lackoftag, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	}
	sprintf(ban_command, "%s %dm", from_user->hostname, 0);
	ballow(ban_command, BAN, from_user);
	sprintf(nban_command, "%s %dm", from_user->nick, 0);
	ballow(nban_command, NICKBAN, from_user);
	sprintf(brstring, "0 %s [ %s ]", from_user->hostname, lp_lackoftag);
        if(check_if_banned(from_user, BANRS) == 0)
	add_line_to_file(brstring, bpath);
	sprintf(nbrstring, "0 %s [ %s ]", from_user->nick, lp_lackoftag);
        if(check_if_banned(from_user, NICKBANRS) == 0)
	add_line_to_file(nbrstring, nbpath);
	snprintf(lastbuf, 800, "%s (IP: %s): %s", from_user->nick, from_user->hostname, lp_lackoftag);
	last_perm(lastbuf);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
	free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);
	return 0;
	}
	}
	
	if((show_class_in_desc > 0) && (from_user->type != REGULAR))
	{
tempclass[0] = '\0';
if((from_user->type == PROTECTED) && (show_class_in_desc == 1))
sprintf(tempclass, "[%s] ", lp_class2); 
if((from_user->type == REG) && (show_class_in_desc <= 2))
sprintf(tempclass, "[%s] ", lp_class3); 
if((from_user->type == VIP) && (show_class_in_desc <= 3))
sprintf(tempclass, "[%s] ", lp_class4); 
if((from_user->type == KVIP) && (show_class_in_desc <= 4))
sprintf(tempclass, "[%s] ", lp_class5); 
if((from_user->type == OP) && (show_class_in_desc <= 5))
sprintf(tempclass, "[%s] ", lp_class6); 
if((from_user->type == CHEEF) && (show_class_in_desc <= 6))
sprintf(tempclass, "[%s] ", lp_class7); 
if((from_user->type == ADMIN) && (show_class_in_desc <= 7))
sprintf(tempclass, "[%s] ", lp_class8); 
if(from_user->type == OWNER)
sprintf(tempclass, "[%s] ", lp_class9); 


if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + strlen(tempclass) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + strlen(tempclass) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + strlen(tempclass) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(send_buf, "%s", tempclass);
        sprintfa(opnosend_buf, "%s", tempclass);
        sprintfa(nosend_buf, "%s", tempclass);
	}

        if((temp=strrchr(from_user->desc,'<')) != NULL)
	{
	if(sscanf(temp,"<%20s V:%50[^,],M:%2[^,],H:%d%*[^S]S:%d*s>", client, cversion, cmode, &hubs, &slots) != 5)
	sprintf(cmode, "0");
	if((strncmp(cmode, "A", 1) != 0) && (strncmp(cmode, "P", 1) != 0) && (strncmp(cmode, "5", 1) != 0))
	sprintf(cmode, "0");
	if(strncmp(cmode, "A", 1) == 0) 
        {
        from_user->mode = 2; 
	if(prmode != from_user->mode)
	newmyinfo = 1;

	if(show_mode_in_desc == 1)
	{
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(send_buf, "[A]");
        sprintfa(opnosend_buf, "[A]");
        sprintfa(nosend_buf, "[A]");
	}
	} 
        else if(strncmp(cmode, "P", 1) == 0)
        {
        from_user->mode = 1;
	if(prmode != from_user->mode)
	newmyinfo = 1;

	if(show_mode_in_desc == 1)
        {
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}

        sprintfa(send_buf, "[P]");
        sprintfa(opnosend_buf, "[P]");
        sprintfa(nosend_buf, "[P]");
        }
        }
        else if(strncmp(cmode, "5", 1) == 0)
        {
        from_user->mode = 1;
	if(prmode != from_user->mode)
	newmyinfo = 1;

        if(show_mode_in_desc == 1)
        {
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + 8))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + 8))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 8))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}

        sprintfa(send_buf, "[PROXY]"); 
        sprintfa(opnosend_buf, "[PROXY]");
        sprintfa(nosend_buf, "[PROXY]");
        }
	}
	
	if(from_user->away_flag == 2)
        {
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + 7))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + 7))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 7))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}

	sprintfa(send_buf, "[AWAY]");
	sprintfa(nosend_buf, "[AWAY]");
	sprintfa(opnosend_buf, "[AWAY]");
        }
	if((show_mode_in_desc == 1) || (from_user->away_flag == 2))
	{
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + 2))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + 2))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 2))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}

	sprintfa(send_buf, " ");
        sprintfa(nosend_buf, " ");
	sprintfa(opnosend_buf, " ");
	}
	}
	if(show_original_desc == 0)
	{
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + strlen(temp) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(send_buf, "%s", temp);
	if(hidetags == 0)
	{
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + strlen(temp) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(opnosend_buf, "%s", temp);
	}
	}
	else
	{
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + strlen(from_user->desc) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(send_buf, "%s", from_user->desc);
	
	if(hidetags == 0)
	{
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + strlen(from_user->desc) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(opnosend_buf, "%s", from_user->desc);
	}
	}
if((true_buf = realloc(true_buf, sizeof(char) * (strlen(true_buf) + strlen(from_user->desc) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/true_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(true_buf, "%s", from_user->desc);
	}

if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((true_buf = realloc(true_buf, sizeof(char) * (strlen(true_buf) + 4))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/true_buf: %s", strerror(errno));
quit = 1;
return 0;
}

	sprintfa(send_buf, "$ $");
	sprintfa(opnosend_buf, "$ $");
	sprintfa(nosend_buf, "$ $");
	sprintfa(true_buf, "$ $");

	if(((from_user->type == REGULAR)
	|| ((check_class >= 1) && (from_user->type == REG)) // REG
	|| ((check_class >= 2) && (from_user->type == VIP)) // VIP
	|| ((check_class >= 3) && (from_user->type == KVIP)) // KVIP
	|| ((check_class >= 4) && (from_user->type == OP)) // OP	
	) && ((strncmp(from_user->hostname, "parent_process", 14) != 0)) && ((strncmp(from_user->hostname, "script_process", 14) != 0)) && ((strncmp(from_user->hostname, "script_process", 14) != 0)))
	{
	if(sscanf(from_user->con_type, "NetLimiter [%d kB/s]", &userlimit) == 1)
	{
	if(userlimit < connlimit)
	{
	uprintf(from_user, "<%s> %s %s: %s ( %d kB/s ). %s: %d kB/s|", botnick, lp_youwerekicked, lp_because, lp_uploadlimittoolow, userlimit, lp_requiredatleast, connlimit);
	uprintf(from_user, "$To: %s From: %s $<%s> %s %s: %s ( %d kB/s ). %s: %d kB/s|", from_user->nick, botnick, botnick, lp_youwerekicked, lp_because, lp_uploadlimittoolow, userlimit, lp_requiredatleast, connlimit);
	sprintf(kbstring, "[ %s ] [ %s : %d kB/s ] [ %s (IP: %s) ]", lp_kick_, lp_uploadlimittoolow, userlimit, from_user->nick, from_user->hostname);
	ban_info(kbstring);
	snprintf(nomyinfo, 50, "%s", from_user->nick);
        free(opnosend_buf);
        free(send_buf);
	free(nosend_buf);
	free(true_buf);	
	if(punishment != 0)
	uprintf(from_user, "$ForceMove %s|", redirect_host_connlimit);
	return 0;
	}
	}
	}
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + strlen(from_user->con_type) + 3))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + strlen(from_user->con_type) + 3))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 1 + strlen(from_user->con_type) + 3))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((true_buf = realloc(true_buf, sizeof(char) * (strlen(true_buf) + 1 + strlen(from_user->con_type) + 3))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/true_buf: %s", strerror(errno));
quit = 1;
return 0;
}

	sprintfa(send_buf, "%s", from_user->con_type);
	sprintfa(opnosend_buf, "%s", from_user->con_type);
	sprintfa(nosend_buf, "%s", from_user->con_type);
	sprintfa(true_buf, "%s", from_user->con_type);
	if ((from_user->away_flag == 2) | (check_if_away(from_user->nick) == 1))
	{	
	from_user->flag = 2;
	sprintfa(send_buf, "%c", from_user->flag);
	sprintfa(opnosend_buf, "%c", from_user->flag);
	sprintfa(nosend_buf, "%c", from_user->flag); 
	sprintfa(true_buf, "%c", from_user->flag); 
	}
	else 
	{	
	from_user->flag = 1;
	sprintfa(send_buf, "%c", from_user->flag);
	sprintfa(opnosend_buf, "%c", from_user->flag); 
	sprintfa(nosend_buf, "%c", from_user->flag); 
	sprintfa(true_buf, "%c", from_user->flag); 	
	}
	sprintfa(send_buf, "$");
	sprintfa(opnosend_buf, "$"); 
	sprintfa(nosend_buf, "$"); 
	sprintfa(true_buf, "$"); 

	if((from_user->email != NULL) && (show_original_mail != 0))
	{
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + strlen(from_user->email) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(send_buf, "%s", from_user->email);
	if(hidetags == 0)
	{
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + strlen(from_user->email) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(opnosend_buf, "%s", from_user->email);
	}
if((true_buf = realloc(true_buf, sizeof(char) * (strlen(true_buf) + strlen(from_user->email) + 1))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/true_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(true_buf, "%s", from_user->email);
	}
if((send_buf = realloc(send_buf, sizeof(char) * (strlen(send_buf) + 24))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/send_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((opnosend_buf = realloc(opnosend_buf, sizeof(char) * (strlen(opnosend_buf) + 24))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/opnosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((nosend_buf = realloc(nosend_buf, sizeof(char) * (strlen(nosend_buf) + 24))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/nosend_buf: %s", strerror(errno));
quit = 1;
return 0;
}
if((true_buf = realloc(true_buf, sizeof(char) * (strlen(true_buf) + 24))) == NULL)
{
logprintf(LOG_EMERG, "Error - In send_user_info()/realloc()/true_buf: %s", strerror(errno));
quit = 1;
return 0;
}
	sprintfa(true_buf, "$%lld", from_user->share); 
	if(hide_allshare > 0)
	{
	sprintfa(send_buf, "$0");
	sprintfa(nosend_buf, "$0"); 
	sprintfa(opnosend_buf, "$0");
	}
	else
	{
	if(from_user->hide_share == 1)    
	{
	if(hide_opshare == 2)
	{
	sprintfa(send_buf, "$0");
	sprintfa(opnosend_buf, "$0");
	}
	else
	{
	sprintfa(send_buf, "$0");
	sprintfa(opnosend_buf, "$0");
	}
	sprintfa(nosend_buf, "$0"); 
	}
	else {
	sprintfa(send_buf, "$%lld", from_user->share);
	sprintfa(nosend_buf, "$%lld", from_user->share); 
	sprintfa(opnosend_buf, "$0"); 
	}
	}
	sprintfa(send_buf, "$|");
	sprintfa(nosend_buf, "$|"); 
	sprintfa(opnosend_buf, "$|"); 
	sprintfa(true_buf, "$|"); 	
	

	/* Hiding TAGs from REGULAR users */
	if (hidetags > 0) {
	if(strncmp(to_user_nick, "$Script", 7) == 0)
        send_to_non_humans(true_buf, SCRIPT, NULL);
	else if((to_user = get_human_user(to_user_nick)) != NULL)
	{
	if((from_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) 
	{
	if((to_user->type & (REGULAR)) != 0)
	{
	if(hide_opshare == 2)
	send_to_user(opnosend_buf, to_user);
	else
	send_to_user(nosend_buf, to_user);
	}
	else if((to_user->type & (PROTECTED | REG | VIP | KVIP)) != 0)
	send_to_user(nosend_buf, to_user);
	else
	send_to_user(send_buf, to_user);
	}
	else
	{
	if(hidetags == 1) 
	{
	if ((to_user->type & (REGULAR)) != 0)
	{
	send_to_user(nosend_buf, to_user);
	}
	if ((to_user->type & (PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	send_to_user(send_buf, to_user);
	}
	}
	else if(hidetags == 2)
	{
	if ((to_user->type & (REGULAR | PROTECTED)) != 0)
	{
	send_to_user(nosend_buf, to_user);
	}
	if ((to_user->type & (REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	send_to_user(send_buf, to_user);
	}
	}
	else if(hidetags == 3)
	{
	if ((to_user->type & (REGULAR | PROTECTED | REG)) != 0)
	{
	send_to_user(nosend_buf, to_user);
	}
	if ((to_user->type & (VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	send_to_user(send_buf, to_user);
	}
	}
	else if(hidetags == 4)
	{
	if ((to_user->type & (REGULAR | PROTECTED | REG | VIP)) != 0)
	{
	send_to_user(nosend_buf, to_user);
	}
	if ((to_user->type & (KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	send_to_user(send_buf, to_user);
	}
	}
	else if(hidetags >= 5)
	{
	if ((to_user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0)
	{
	send_to_user(nosend_buf, to_user);
	}
	if ((to_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	send_to_user(send_buf, to_user);
	}
	}
	}
	} 
	else {
	if(from_user->hide_share != 2)
	{
	if((from_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) 
	{
	if(hide_opshare == 2)
	send_to_humans(opnosend_buf, REGULAR, NULL);
	else
	send_to_humans(nosend_buf, REGULAR, NULL);
	
	send_to_humans(nosend_buf, PROTECTED | REG | VIP | KVIP, NULL);
	send_to_humans(send_buf, OP | CHEEF | ADMIN | OWNER, NULL);
	}
	else
	{
	if(hidetags == 1)
	{
	if(newmyinfo == 1)
	send_to_humans(nosend_buf, REGULAR, NULL);
	send_to_humans(send_buf, PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	}
	else if(hidetags == 2)
	{
	if(newmyinfo == 1)
	send_to_humans(nosend_buf, REGULAR | PROTECTED, NULL);
	send_to_humans(send_buf, REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	}
	else if(hidetags == 3)
	{
	if(newmyinfo == 1)
	send_to_humans(nosend_buf, REGULAR | PROTECTED | REG, NULL);
	send_to_humans(send_buf, VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	}
	else if(hidetags == 4)
	{	
	if(newmyinfo == 1)
	send_to_humans(nosend_buf, REGULAR | PROTECTED | REG | VIP, NULL);
	send_to_humans(send_buf, KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	}
	else if(hidetags >= 5)
	{
	if(newmyinfo == 1)
	send_to_humans(nosend_buf, REGULAR | PROTECTED | REG | VIP | KVIP, NULL);
	send_to_humans(send_buf, OP | CHEEF | ADMIN | OWNER, NULL);
	}
	}
	}
	}
	}
	/* TAGs are unhidden from all users */
	if (hidetags == 0) {
	if(strncmp(to_user_nick, "$Script", 7) == 0)
	send_to_non_humans(true_buf, SCRIPT, NULL);
	else if((to_user = get_human_user(to_user_nick)) != NULL)
	{
	if((from_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	if((hide_opshare == 2) && (to_user->type == REGULAR))
	send_to_user(opnosend_buf, to_user);
	else
	send_to_user(send_buf, to_user);
	}
	else
	send_to_user(send_buf, to_user);
	}
	else { 
	if(from_user->hide_share != 2)
	{
	if(hide_opshare != 2)
	send_to_humans(send_buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	else
	{
	if((from_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
	{
	send_to_humans(opnosend_buf, REGULAR, NULL);	
	send_to_humans(send_buf, PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);	
	}
	else
	send_to_humans(send_buf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	}
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

	olds = from_user->myinfos;
	
		if((from_user->myinfos = malloc(sizeof(char) 
			* (strlen(send_buf) + 1))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In send_user_info()/malloc()/from_user->myinfos: %s", strerror(errno));
			quit = 1;
			return 0;
		}
	strcpy(from_user->myinfos, send_buf);

	oldns = from_user->myinfons;

		if((from_user->myinfons = malloc(sizeof(char) 
			* (strlen(nosend_buf) + 1))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In send_user_info()/malloc()/from_user->myinfons: %s", strerror(errno));
			quit = 1;
			return 0;
		}
	strcpy(from_user->myinfons, nosend_buf);

	oldons = from_user->myinfoons;

		if((from_user->myinfoons = malloc(sizeof(char) 
			* (strlen(opnosend_buf) + 1))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In send_user_info()/malloc()/from_user->myinfoons: %s", strerror(errno));
			quit = 1;
			return 0;
		}
	strcpy(from_user->myinfoons, opnosend_buf);
	
	free(olds); free(oldns); free(oldons);
	free(opnosend_buf);
	free(send_buf);
	free(nosend_buf);
	free(true_buf);
return 1;
}

/* This function handles every command in the received packet one by one */
/* Returns 0 if user should be removed */
void uptime2str(struct user_t *user, double seconds)
{
        int days,hours,mins,secs,weeks;
	char uptime_str[301];
	
  weeks  = ((int)seconds/60/60/24/7);
  days  = ((int)seconds/60/60/24)%7;
  hours = ((int)seconds/60/60)%24;
  mins  = ((int)seconds/60)%60;
  secs  = ((int)seconds)%60;

snprintf(uptime_str, 300, "%s ", lp_uptime);

        if (weeks != 0) sprintfa(uptime_str, "%d %s, ",weeks, lp_weeks);
	if (days != 0) sprintfa(uptime_str, "%d %s, ",days, lp_days);
        if (hours != 0) sprintfa(uptime_str, "%d %s, ",hours, lp_hours);
        if (mins != 0) sprintfa(uptime_str,"%d %s, ",mins, lp_minutes);
	if (seconds != 0) sprintfa(uptime_str,"%d %s",secs, lp_seconds);
	else
	uptime_str[strlen(uptime_str) - 2] = '\0'; 

uprintf(user, "%s", uptime_str);
}



int handle_command(char *buf, struct user_t *user)
{
	int ret;
	char *temp;
	char *k;
	char tempip_nick[51];
	time_t now;	
	temp = NULL;
	now = time(NULL);


	while(buf != NULL)
	{
		/* Check if it's a '$' or a '<' first in the command string */
		if((strchr(buf, '$') != NULL) && (strchr(buf, '<') == NULL))
			temp = strchr(buf, '$');
		else if((strchr(buf, '$') == NULL) && (strchr(buf, '<') != NULL))
			temp = strchr(buf, '<');
		else if((strchr(buf, '$') == NULL) && (strchr(buf, '<') == NULL))
		{
			/* This is what happends every time a command doesn't fit in one
			* single package. */
			return 1;
		}

		else
			(strchr(buf, '$') < strchr(buf, '<'))
			? (temp = strchr(buf, '$'))  /* The '$' is first */
			: (temp = strchr(buf, '<')); /* The '<' is first */

		buf = temp;
		temp = NULL;
		/* First check if it's a whole command */
		if(strchr(buf, '|') != NULL)
		{
			/* Copy command to temporary buf so we don't get more sent to the
			* function than neccessary */
			if((temp = malloc(sizeof(char) * (cut_string(buf, '|') + 3))) == NULL)
			{
				logprintf(LOG_EMERG, "Error - In handle_command()/malloc(): %s", strerror(errno));
				quit = 1;
				return -1;
			}
			strncpy(temp, buf, cut_string(buf, '|') + 1);
			if(cut_string(buf, '|') > 0)
				temp[cut_string(buf, '|')+1] = '\0';

			/* The Key command */
			if(strncmp(temp, "$Key ", 5) == 0) 
			{
			}
			else if(strncmp(temp, "$GetINFO ", 9) == 0)
			{
			uprintf(user, "<%s> Your client is too old. $GetINFO is no longer supported|", botnick);
			free(temp);
			return 0;
			}
			else if(strncmp(temp, "$Supports ", 10) == 0)
			{
			if(strstr(temp, "NoGetINFO") != NULL)
			user->nogetinfo = 1;
			}
		        else if(strncmp(temp, "$UserIP ", 8) == 0)
			{
			if((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
			{
			k = temp + 8;
			uprintf(user, "$UserIP ");
			while(strstr(k, "$$") != NULL)
			{
			if(sscanf(k, "%50[^$]$", tempip_nick) == 1)
			{
			if(get_human_user(tempip_nick) != NULL)
			uprintf(user, "%s %s$$", get_human_user(tempip_nick)->nick, get_human_user(tempip_nick)->hostname);
			}			
			k = k + strlen(tempip_nick) + 2;
			}
			if(sscanf(k, "%50[^|]|", tempip_nick) == 1)
			{
			if(get_human_user(tempip_nick) != NULL)
			uprintf(user, "%s %s", get_human_user(tempip_nick)->nick, get_human_user(tempip_nick)->hostname);
			}
			uprintf(user, "|");
			}
			else
			{
			}
			}
			else if(strncmp(temp, "$BotINFO ", 9) == 0)
			{
			if((user->type != NON_LOGGED) || (user->nogetinfo == 0))
			{
			if(strlen(topic) > 0)
			uprintf(user, "$HubINFO %s-%s$%s:%u$[DB Hub V:%s] %s$%d$%llu$%d$%d$DB Hub$http://www.dbhub.org|", hub_name, topic, hub_hostname, listening_port, VERSION, hub_description, max_users, min_share, minslots, maxhubs);
			else
			uprintf(user, "$HubINFO %s$%s:%u$[DB Hub V:%s] %s$%d$%llu$%d$%d$DB Hub$http://www.dbhub.org|", hub_name, hub_hostname, listening_port, VERSION, hub_description, max_users, min_share, minslots, maxhubs);
			}
			else
			snprintf(botinfolater, 50, "%s", user->nick); 
			}
			/* The ValidateNick command */
			else if(strncmp(temp, "$ValidateNick ", 14) == 0)
			{
				/* Only for non logged in users. If client wants to change
				* nick, it first has to disconnect.  */
				/* Also allowed for scripts to register their nick in the
				* nicklist.  */
				if((user->type == NON_LOGGED) || ((user->type == SCRIPT) && (pid > 0)))
				{
					if(validate_nick(temp, user) == 0)
					{
						free(temp);
						return 0;
					}
				}

			}

			/* The Version command */
			else if(strncmp(temp, "$Version ", 9) == 0)
			{
					if(version(temp, user) == 0)
					{
						free(temp);
						return 0;
					}		  
			//DB// hublist.org needs this
			if((user->nogetinfo == 0) && (check_if_registered(user->nick) == 0))
			{
			uprintf(user, "$Hello %s|", user->nick);                                                                             
			send_nick_list(user);
			}
			}
			
			/* The GetNickList command */
			else if(strncasecmp(temp, "$GetNickList", 12) == 0)
			{
			if((user->nogetinfo == 1) && (user->type == NON_LOGGED))
			getinfolater = 1;
			else
			{
			if((user->nogetinfo == 0) && (user->type == NON_LOGGED))
			send_nick_list(user);

			uprintf(user, "$MyINFO $ALL %s [BOT] Hub-Security <DB V:%s,M:A,H:0/0/1,S:0>$ $$hub-security$0$|", botnick, VERSION);
	    		uprintf(user, "$MyINFO $ALL %s [BOT] Op Chat <DB V:%s,M:A,H:0/0/1,S:0>$ $$opchat$0$|", opchatnick, VERSION);
	    		if(clockbot == 1)
			uprintf(user, "$MyINFO $ALL %s [BOT] Server Time <DB V:%s,M:A,H:0/0/1,S:0>$ $$clock$0$|", clocknick, VERSION);
			send_myinfo(user);

			if(check_if_registered(user->nick) >= 5)
			uprintf(user, "$UserIP %s %s$$||", user->nick, user->hostname);
			}                   			
			}

			/* The MyINFO command */
			else if(strncmp(temp, "$MyINFO $", 9) == 0)
			{
					if((my_info(temp, user) == 0) || ((user->type == REGULAR) && (strcasecmp(user->nick, nomyinfo) == 0)))
					{
						free(temp);
						return 0;
					}
			}
			/* The ScriptINFO command */
			else if((strncmp(temp, "$ScriptINFO ", 12) == 0) && ((user->type == SCRIPT) || (user->type == FORKED)))
			script_info(temp, user);

			/* Reloading files to memory if there were some changes in files made by script or edited from shell */
			else if((strncmp(temp, "$ReloadFile ", 12) == 0) && ((user->type & (SCRIPT | FORKED | OWNER)) != 0))
			reload_files_to_memory(temp, user);

			/* Gagging/ungagging user by the script */
			else if((strncmp(temp, "$Gag ", 5) == 0) && ((user->type == SCRIPT) || (user->type == FORKED)))
			gag_user_by_script(temp, user);
					
			/* The To: From: command */
			else if(strncmp(temp, "$To: ", 5) == 0)
			{
    			    if((block_pm == 1) && ((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0))
	    		    {
		    	    blocked_pm++;
			    uprintf(user, "<%s> %s. %s|", botnick, lp_pmoff, lp_canonlypmtoops);
			    }
			    else
			    {
			    if((user->type & (NON_LOGGED | SCRIPT)) == 0) 
			    to_from(temp, user);
			    }
			}

			/* The ConnectToMe command */
			else if(strncmp(temp, "$ConnectToMe ", 13) == 0)
			{
			        if((block_download == 1)  && ((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0))
	    			{
		    		uprintf(user, "<%s> %s|", botnick, lp_downloadoff);
			        blocked_downloads++;
				}
				else
				{
				if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER | FORKED)) != 0)
				{
				  if (user->gagd == 0)
				  {
				accepted_downloads++;	 
    				  connect_to_me(temp, user);
				  }
					else
					blocked_downloads++;
				}
				}
			}

			/* The RevConnectToMe command */
			else if(strncmp(temp, "$RevConnectToMe ", 16) == 0)
			{
        if((block_download == 1) && ((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0))
	        {
		        uprintf(user, "<%s> %s|", botnick, lp_downloadoff);
			blocked_downloads++;
			        }
				        else
					        {
						
				if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER | FORKED)) != 0)
				{
				  if (user->gagd == 0)
				  {
				accepted_downloads++;	 
				   rev_connect_to_me(temp, user);
				   }
					else
					blocked_downloads++;
					}
				}
			}

			/* The Search command */
			else if(strncmp(temp, "$Search ", 8) == 0)
			{
if((block_search == 1) && ((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0))
{
if(strstr(temp, "TTH") == NULL)
uprintf(user, "<%s> %s|", botnick, lp_searchoff);
blocked_searches++;
}
else
{
						

				if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER | FORKED | SCRIPT)) != 0)
				{
				  if (user->gags == 0)
				  {
				    accepted_searches++;	
				    search(temp, user);
				    }
					else
					blocked_searches++;
				}
			}
}
			/* The SR command */
			else if(strncmp(temp, "$SR ", 4) == 0)
			{
				if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER | FORKED)) != 0)
					sr(temp, user);
			
			}

			/* The MyPass command */
			else if(strncmp(temp, "$MyPass ", 8) == 0)
			{
				if(user->type == NON_LOGGED)
				{
					if(my_pass(temp + 8, user) == 0)
					{
						free(temp);
						return 0;
					}
				}
			}

			/* The kick command */
			else if(strncasecmp(temp, "$Kick ", 6) == 0)
			{
				if((user->type & (OP | CHEEF | ADMIN | OWNER | FORKED | SCRIPT)) != 0)
				{
					kick(temp, user, 1);
				}
				else
					logprintf(LOG_CRIT, "%s tried to kick without having priviledges\n", user->nick);
			}

			/* The OpForceMove command */
			else if(strncmp(temp, "$OpForceMove ", 13) == 0)
			{
				if((user->type & (ADMIN | OWNER | FORKED)) != 0)
				{
					op_force_move(temp, user);
				}
				else
					logprintf(LOG_CRIT, "%s tried to redirect without having priviledges\n", user->nick);
			}

			/* The chat command, starts with <nick> */
			else if((*temp == '<') && (strcmp(user->hostname, "parent_process") != 0))
			{
			if(block_main == 1) {	
			if((user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) 
			{
					if (user->gagc == 0)
					{
					 chat(temp, user);
					}
					else
					{
					blocked_mc++;
					uprintf(user, "<%s> %s. %s|", botnick, lp_notallowedtochat, lp_canonlypmtoops);
			}
			}
			else
			blocked_mc++;
			}
			
			else {
			if((user->type & (SCRIPT | NON_LOGGED)) == 0)
				{
				  if ((user->gagc == 0) || ((check_if_registered(user->nick) >= min_command_class) && ((strstr(temp, "> +rules|") != NULL) || (strstr(temp, "> !rules|") != NULL)
				  || (strstr(temp, "> +help") != NULL) || (strstr(temp, "> !help|")  != NULL)
				  || (strstr(temp, "> +regme ") != NULL) || (strstr(temp, "> !regme ") != NULL)
				  || (strstr(temp, "> +regme|") != NULL) || (strstr(temp, "> !regme|") != NULL)
				  || (strstr(temp, "> +protectme ") != NULL) || (strstr(temp, "> !protectme ") != NULL))))
				  {
					  chat(temp, user);
					}
					else
					{
					blocked_mc++;
					if((user->type == REGULAR) && ((mute_unregistered == 1) || (mute_unregistered == 3)))
					{
					uprintf(user, "<%s> %s. %s|", botnick, lp_haveregistertochat, lp_canonlypmtoops);
					}
					else
					{
					uprintf(user, "<%s> %s. %s|", botnick, lp_notallowedtochat, lp_canonlypmtoops);
					}
					}
				}
			
			}
			}

			/* Commands that should be forwarded from forked processes */
							else if((strncmp(temp, "$Quit ", 6) == 0)  ||
							(strncmp(temp, "$OpList ", 8) == 0))
			{
				if(user->type == FORKED)
				{
					if(strncmp(temp, "$OpList ", 8) == 0)
						/* The oplist ends with two '|' */
						strcat(temp, "|");
					send_to_humans(temp, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, user);       
				}
			}

			else if((strncasecmp(temp, "$ForceMove ", 11) == 0)
				&& (user->type == FORKED))
			{		  
				redirect_all(temp + 11, user);
			}	     

			else if((strncasecmp(temp, "$QuitProgram", 12) == 0) 
				&& ((user->type == FORKED) || (user->type == SCRIPT)))
			{
				uprintf(user, "\r\nShutting down hub...\r\n");
				quit = 1;
			}

			else if((strncasecmp(temp, "$RedirectAll ", 13) == 0) && ((user->type & (ADMIN | OWNER)) != 0))
			{
				uprintf(user, "\r\nRedirecting all users...\r\n");
				logprintf(LOG_ALERT, "Admin at %s redirected all users\n", user->hostname);
				redirect_all(temp+13, user);
			}


			else if(strncasecmp(temp, "$Set ", 5) == 0)
			{
				if((user->type & (FORKED | SCRIPT)) != 0)
					set_var(temp, user); 
			}

			else if(strncasecmp(temp, "$Ban ", 5) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
					ret = ballow(temp+5, BAN, user);
				}	 
			}
			else if(strncasecmp(temp, "$Allow ", 7) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
					ret = ballow(temp+7, ALLOW, user);
				}	 
			}
			else if(strncasecmp(temp, "$Unban ", 7) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
					ret = unballow(temp+7, BAN);
				}		  
			}
			else if(strncasecmp(temp, "$Unallow ", 9) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
					ret = unballow(temp+9, ALLOW);
				}		  
			}
			else if(strncasecmp(temp, "$AddRegUser ", 12) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
	    				ret = add_reg_user(temp);
				}
			}	     
			else if(strncasecmp(temp, "$RemoveRegUser ", 15) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
					ret = remove_reg_user(temp+15);
				}
			}		  
			else if(strncasecmp(temp, "$NickBan ", 9) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
					ret = ballow(temp+9, NICKBAN, user);
				}		     
			}
			else if(strncasecmp(temp, "$UnNickBan ", 11) == 0)
			{
				if((user->type & (SCRIPT)) != 0)
				{
					ret = unballow(temp+11, NICKBAN);
				}
			}	     	     
			/* Commands from script processes */
#ifdef HAVE_PERL		  
			else if(strncasecmp(temp, "$NewScript", 10) == 0)
			{
				if(user->type == FORKED)
				{		       
					user->type = SCRIPT;
					snprintf(user->hostname, 50, "script_process");
					snprintf(user->nick, 50, "script process");
				}		  
			}
			else if(strncmp(temp, "$Script ", 8) == 0)
			{		  
				if(pid > 0)
				{
					if(user->type == FORKED)
						non_format_to_scripts(temp);
				}
				else
				{		       
					if(user->type == SCRIPT)
						sub_to_script(temp + 8);		     
				}
			}
			else if(strncasecmp(temp, "$ReloadScripts", 14) == 0)
			{
				if((user->type & (FORKED)) != 0)
				{	
					if(pid > 0)
						script_reload = 1;

				}
			}
			else if(strncasecmp(temp, "$ScriptToUser ", 14) == 0)
			{
				if((user->type & (SCRIPT | FORKED)) != 0)
					script_to_user(temp, user);
			}
			else if(strncasecmp(temp, "$DataToAll ", 11) == 0)
			{
				if(user->type == SCRIPT)
				{
				send_to_non_humans(temp, FORKED, user);	
				send_to_humans(temp + 11, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, user);
				}
		                  else if(user->type == FORKED)                                                                                 
	                          send_to_humans(temp + 11, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, user); 
				  			
			}
#endif
		}

		/* Send to scripts */
#ifdef HAVE_PERL
		if(((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
			&& (temp != NULL) && (strlen(temp) > 2) 
			&& (strncasecmp(temp, "$ReloadScripts", 14) != 0))
		{	     
		if((*temp == '<') && (user->gagc != 0)) {
		}
		else {
		command_to_scripts("$Script data_arrival %c%c%s%c%c", 
				'\005', '\005', user->nick, '\005', '\005');
			non_format_to_scripts(temp);
		}
		}	
#endif

		if((buf = strchr(buf, '|')) != NULL)
			buf++; 
		if(temp != NULL)
			free(temp);

	} 
		if(temp != NULL)
			free(temp);
	
	return 1;

}

void hub_mess(struct user_t *user, int mess_type)
{
	char *send_string;
        char *temp;		        
		        temp = NULL;
			
	send_string = NULL;
	switch(mess_type)
	
																																								
	
	{
		/* If the hub is full, tell user */
	case HUB_FULL_MESS:
		if((send_string = malloc(sizeof(char) 
			* (53 + strlen(hub_full_msg) + 3))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}
		sprintf(send_string, "<%s> %s|", 
			botnick, hub_full_msg);
		break;

	case GET_PASS_MESS:
		if((send_string = malloc(sizeof(char) * 100)) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}

		sprintf(send_string, "$GetPass|");
		break;

	case GET_PASS_MESS2:
		if((send_string = malloc(sizeof(char) * 100)) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}

		sprintf(send_string, "$GetPass|");
		break;

	case LOGGED_IN_MESS:
		/* Construct the reply string */
		if((send_string = malloc(sizeof(char) * (60 + strlen(user->nick)))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}
		sprintf(send_string, "$Hello %s|", user->nick); 
		break;

	case OP_LOGGED_IN_MESS:
		if((send_string = malloc(sizeof(char) * (15 + strlen(user->nick)))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}
		sprintf(send_string, "$LogedIn %s|", user->nick); 
		break;

	case BAD_PASS_MESS:
		/* Construct the reply string */
		if((send_string = malloc(sizeof(char) * 60)) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}
		sprintf(send_string, "$BadPass|"); 
		uprintf(user, "<%s> %s: %s|", botnick, hub_name, lp_passwordincorrect); 
		break;

	case NOPASS_MESS:
		/* Construct the reply string */
		if((send_string = malloc(sizeof(char) * 60)) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}
		sprintf(send_string, "$BadPass|"); 
		uprintf(user, "<%s> %s: %s|", botnick, hub_name, lp_passwordrequired); 
		break;

	case HELLO_MESS:
		/* Construct the reply string */
		if((send_string = malloc(sizeof(char) * (strlen(user->nick) + 12))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}
		sprintf(send_string, "$Hello %s|", user->nick); 
		break;

	case INIT_MESS:
	
	
	if((send_string = malloc(sizeof(char) * (MAX_HUB_NAME+MAX_TOPIC_LEN+256))) == NULL)
		{
			logprintf(LOG_EMERG, "Error - In hub_mess()/malloc(): %s", strerror(errno));
			quit = 1;
			return;
		}

		if (strlen(topic)!=0)
		{ 
		if(block_main == 1)
		sprintf(send_string, "$HubName %s - %s (%s)|", hub_name, topic, lp_mainchatlocked);
		else
		sprintf(send_string, "$HubName %s - %s|", hub_name, topic);
		}
		else 
		{
		if(block_main == 1)
		sprintf(send_string, "$HubName %s (%s)|", hub_name, lp_mainchatlocked);
		else
		sprintf(send_string, "$HubName %s|", hub_name);
		}
		break;

}
	/* Send the constructed string */
	if(send_string != NULL)
	{
	send_to_user(send_string, user);
	free(send_string);
	}
}

/* Add a user who connected */
int new_human_user(int sock)
{
	struct user_t *user;
	struct sockaddr_in client;
	socklen_t namelen;
	int yes = 1;
	int i = 0;
	int banip, banrange, allowip, allowrange;
	int socknum;
	int erret;
        char sometxt[251];                                                                                                                   
	long long  numberrec;                                                                                                                
	char somerec[21];                                                                                                                    
	int flags;
	time_t now;
	
	memset(&client, 0, sizeof(struct sockaddr_in));

	/* Get a socket for the connected user.  */
	namelen = sizeof(client);
	while(((socknum = accept(sock, (struct sockaddr *)&client, 
		&namelen)) == -1) && ((errno == EAGAIN) || (errno == EINTR)))
	{
		i++;
		usleep(500);
		/* Giving up after half a second */
		if(i == 1000)
			return -1;
	}

	/* Allocate space for the new user */
	if((user = malloc(sizeof(struct user_t))) == NULL)
	{	
		logprintf(LOG_EMERG, "Error - In new_human_user()/malloc(): %s", strerror(errno));
		quit = 1;
		return -1;
	}   

	/* Set the sock of the user.  */
	user->sock = socknum;

	/* Reset the last search time */
	user->last_search = 0;

	/* Avoid dead peers */
	if(setsockopt(user->sock, SOL_SOCKET, SO_KEEPALIVE, &yes,
		sizeof(int)) == -1)
	{
		logprintf(LOG_EMERG, "Error - In new_human_user()/set_sock_opt(): %s", strerror(errno));
		close(user->sock);
		free(user);
		return -1;
	}

	if((flags = fcntl(user->sock, F_GETFL, 0)) < 0)
	{	
		logprintf(LOG_EMERG, "Error - In new_human_user()/in fcntl(): %s", strerror(errno));
		close(user->sock);
		free(user);
		return -1;
	}

	/* Non blocking mode */
	if(fcntl(user->sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		logprintf(LOG_EMERG, "Error - In new_human_user()/in fcntl(): %s", strerror(errno));
		close(user->sock);
		free(user);
		return -1;
	}   

	/* Set users ip */
	user->ip = client.sin_addr.s_addr;

		strcpy(user->hostname, inet_ntoa(client.sin_addr));

	/* Send to scripts */
#ifdef HAVE_PERL
	command_to_scripts("$Script attempted_connection %c%c%s|", '\005', '\005', user->hostname);
#endif

	/* Set user vars to 0/NULL */
	user->type = NON_LOGGED;   
	user->tmpop = 0;
	user->wasreg = 0;
	memset(user->nick, 0, MAX_NICK_LEN+1);
	memset(user->version, 0, MAX_VERSION_LEN+1);
	user->email = NULL;
	user->gaginf = 0;
	user->desc = NULL;
	user->myinfos = NULL;
	user->myinfons = NULL;
	user->myinfoons = NULL;
	user->con_type = NULL;
	user->flag = 0;
	user->share = 0;
	user->buf = NULL;
	user->outbuf = NULL;
	user->gaggedonminshare = 0;
	user->hide_share = 0;
	user->password[0] = '\0';
	user->rem = 0;
	user->away_flag = 1;
	user->autoawaytime = 0;
	user->autoawayreason[0] = '\0';
	user->last_search = (time_t)0;
	user->last_pm_write = (time_t)0;
	user->too_fast_write_count = 0; // Clear write speed counter 
	user->haszerobytes = 0;
	user->last_write = (time_t)0;
	user->freeslots = 0;
	user->mode = 0;
	user->allslots = 0;
	user->nogetinfo = 0;
	user->srtime = (time_t)0;
	now = time(NULL);
	user->jointime = now;
	user->gagc = 0;
	user->gagd = 0;
	user->gags = 0;
	user->isbanned = 0;
	snprintf(user->nick, 50, "Non_logged_in_user");

	/* Check if user is banned */
		banip = check_if_banned(user, BAN);
		allowip = check_if_allowed(user, ALLOW);   
		banrange = check_if_banned(user, BANRANGE);
		allowrange = check_if_allowed(user, ALLOWRANGE);

		if(ban_overrides_allow == 0)
		{
			if(banrange == 1)
			{
			if((allowrange == 1) && (banip == 1))
			user->isbanned = 1;
			if((allowrange == 0) && (allowip == 0))
			user->isbanned = 1;
			}
			else
			{
			if((banip == 1) && (allowip == 0) && (allowrange == 0))
			user->isbanned = 1;
			}
		}

		else
		{	
			if((banip == 1) || (banrange == 1))
			user->isbanned = 1;
		}

		if((banip == -1) || (banrange == -1) || (allowip == -1) || (allowrange == -1))
		{	
			while(((erret =  close(user->sock)) != 0) && (errno == EINTR))
				logprintf(LOG_EMERG, "Error - In new_human_user()/close(): Interrupted system call. Trying again.\n");	

			if(erret != 0)
			{	
				logprintf(LOG_EMERG, "Error - In new_human_user()/close(): %s", strerror(errno));
			}  
			free(user);
			return -1;
		}   
			
	/* Add sock struct of the user.  */
	add_socket(user);

	if(sock == listening_socket)
		logprintf(LOG_WARNING, "New connection on socket %d from user at %s\n", user->sock, user->hostname);

	/* If it's a regular user.  */
	if(sock == listening_socket)
	{
		uprintf(user, "$Lock EXTENDEDPROTOCOL_DB_Hub Pk=DB_Hub|");        
uprintf(user, "<%s> This hub is powered by DB Hub %s @ %s ( http://www.dbhub.org/ )\r\n", botnick, VERSION, DBDATE);
if(show_uptime == 1)
{
uptime2str(user, get_uptime());
uprintf(user, "\r\n");
}
if(show_records == 1)
{
if(strcmp(recordusstr, "") != 0)
{
sscanf(recordusstr, "%20s ", somerec);
snprintf(sometxt, 250, "%s", recordusstr + strlen(somerec) + 1);
uprintf(user, "%s: %s %s (%s)", lp_alltime, somerec, lp_users, sometxt);
}
if(strcmp(recordshstr, "") != 0)
{
sscanf(recordshstr, "%20s ", somerec);
sscanf(somerec, "%llu", &numberrec);
snprintf(sometxt, 250, "%s", recordshstr + strlen(somerec) + 1);
if(strlen(somerec) > 12)
uprintf(user, ", %2.2f TiB %s (%s)", ((double)numberrec / (1024*1024*1024)) / 1024, lp_share_, sometxt);
else
uprintf(user, ", %2.2f GiB %s (%s)", (double)numberrec / (1024*1024*1024), lp_share_, sometxt);
}
}
send_to_user("|", user);

		uprintf(user, "$Supports OpPlus NoGetINFO NoHello UserIP2 UserCommand|");        
		hub_mess(user, INIT_MESS);

	}
	return 0;
}

/* Add a non-human user to the linked list.  */
void add_non_human_to_list(struct user_t *user)
{
	/* Add the user at the first place in the list */
	user->next = non_human_user_list;
	non_human_user_list = user;
}

/* Remove a non-human user.  */
void remove_non_human(struct user_t *our_user)
{
	int erret;
	struct user_t *user, *last_user;

	if (our_user==NULL){
		logprintf(LOG_WARNING, "Error: our_user is NULL");
		return;
	}

	user = non_human_user_list;
	last_user = NULL;

	while(user != NULL)
	{
		if(user == our_user)
		{	    
		while(((erret =  close(our_user->sock)) != 0) && (errno == EINTR))
					logprintf(LOG_EMERG, "Error - In remove_non_human()/close(): Interrupted system call. Trying again.\n");	

				if(erret != 0)
				{	
					logprintf(LOG_EMERG, "Error - In remove_non_human()/close(): %s", strerror(errno));
				}  
			if(last_user == NULL)
				non_human_user_list = user->next;

			else
				last_user->next = user->next;
				if(our_user->buf != NULL)
					free(our_user->buf);
				if(our_user->outbuf != NULL)
					free(our_user->outbuf);
				if(our_user->email != NULL)
					free(our_user->email);
				if(our_user->desc != NULL)
					free(our_user->desc);
				if(our_user->con_type != NULL)
					free(our_user->con_type);

			free(our_user);
			our_user=NULL;
			return;
		}
		last_user = user;
		user = user->next;
	}
}

/* Add a human user to the hashtable.  */
void add_human_to_hash(struct user_t *user)
{
	int hashv;

	hashv = get_hash(user->nick);

	/* Adds the user first in the linked list of the specified hash value.  */
	user->next = human_hash_table[hashv];
	human_hash_table[hashv] = user;
}

/* Returns a human user from a certain nick.  */
struct user_t* get_human_user(char *nick)
{
	struct user_t *user;

	user = human_hash_table[get_hash(nick)];

	while((user != NULL) 
		&& !((strncasecmp(user->nick, nick, strlen(nick)) == 0) 
		&& (strlen(nick) == strlen(user->nick))))
		user = user->next;

	return user;
}

/* Removes a human user from hashtable.  */
void remove_human_from_hash(char *nick)
{
	struct user_t *user, *last_user;
	int hashv;

	hashv = get_hash(nick);
	user = human_hash_table[hashv];
	last_user = NULL;

	while(user != NULL)
	{
		if((strncmp(user->nick, nick, strlen(nick)) == 0)
			&& (strlen(nick) == strlen(user->nick)))
		{
			if(last_user == NULL)
				human_hash_table[hashv] = user->next;
			else
				last_user->next = user->next;

			return;
		}
		last_user = user;
		user = user->next;
	}
}

/* Removes a human user.  */
void remove_human_user(struct user_t *user)
{
	int erret;

	/* Remove the user from the hashtable.  */
	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
		remove_human_from_hash(user->nick);


	/* When a logged in user in a non script process leaves, the user should
	* be removed from the list and the users share should be subtracted from 
	* the total share.  */
	if((user->nick != NULL) 
		&& ((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) 
		&& (pid == 0))
	{	
		total_users--;
		if(user->share > 0)
		total_share -= user->share;	
#ifdef HAVE_PERL
                command_to_scripts("$Script stat_info %d %llu|", total_users, total_share);                         
#endif	
	}

	while(((erret =  close(user->sock)) != 0) && (errno == EINTR))
		logprintf(LOG_EMERG, "Error - In remove_human_user()/close(): Interrupted system call. Trying again.\n");	

	if(erret != 0)
	{	
		logprintf(LOG_EMERG, "Error - In remove_human_user()/close(): %s", strerror(errno));
	}  

	if(user->buf != NULL)
	{	     
		free(user->buf);
		user->buf = NULL;
	}   
	if(user->outbuf != NULL)
	{	     
		free(user->outbuf);
		user->outbuf = NULL;
	}   

	if(user->email != NULL)
	{		     
		free(user->email);
		user->email = NULL;
	}   
	if(user->desc != NULL)
	{		     
		free(user->desc);
		user->desc = NULL;
	}      

	if(user->con_type != NULL)
	{		     
		free(user->con_type);
		user->con_type = NULL;
	}      

	if(user->myinfos != NULL)
	{		     
		free(user->myinfos);
		user->myinfos = NULL;
	}      
	if(user->myinfons != NULL)
	{		     
		free(user->myinfons);
		user->myinfons = NULL;
	}      
	if(user->myinfoons != NULL)
	{		     
		free(user->myinfoons);
		user->myinfoons = NULL;
	}      



	/* Remove the socket struct of the user.  */
	remove_socket(user);


#ifdef HAVE_PERL
	if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
	{	
		command_to_scripts("$Script user_disconnected %c%c", '\005', '\005');
		non_format_to_scripts(user->nick);
		command_to_scripts("|");
	}   
#endif 

	/* And free the user.  */
	free(user);

//	if((count_users(NON_LOGGED | REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER) == 0) && (pid == 0) 
//		&& (listening_socket == -1))
//		kill_forked_process();
}

/* Removes a user. Sends the $quit string if send_quit is non-zero and removes
* the user from the userlist if remove_from_list is non-zero.  */
void remove_user(struct user_t *our_user, int send_quit, int remove_from_list)
{
	char seenbuf[101];
	char usertype[51];
	char joinstring[301];
	struct sock_t *human_user;
	struct sock_t *next_human;

	char quit_string[MAX_NICK_LEN+10];

	if(send_quit != 0)
	{
		if((our_user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0)
		{
			snprintf(quit_string, MAX_NICK_LEN+10, "$Quit %s|", our_user->nick);
        		send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, our_user);
		}
		else if((our_user->type == SCRIPT)
			&& (strncmp(our_user->nick, "script process", 14) != 0))
		{
			snprintf(quit_string, MAX_NICK_LEN+10, "$Quit %s|", our_user->nick);
			send_to_humans(quit_string, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, our_user);
		}
	}   


if(((our_user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) && (strcmp(our_user->nick, "removed user") != 0))
{
if((strlen(partmsg) > 0) && (show_welcomes > 0) && (quit == 0))
{               
if(((our_user->type & (ADMIN | OWNER)) != 0) && (strlen(welcomeadmin) > 0) && (our_user->hide_share != 2))
{
snprintf(joinstring, 300, "<%s> %s %s %s|", botnick, lp_class8, our_user->nick, partmsg);
send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((our_user->type == CHEEF) && (strlen(welcomecheef) > 0))
{
snprintf(joinstring, 300, "<%s> %s %s %s|", botnick, lp_class7, our_user->nick, partmsg);
send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((our_user->type == OP) && (strlen(welcomeop) > 0) && (our_user->tmpop == 0))
{
snprintf(joinstring, 300, "<%s> %s %s %s|", botnick, lp_class6, our_user->nick, partmsg);
send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((our_user->type == REG) && (strlen(welcomereg) > 0))
{
snprintf(joinstring, 300, "<%s> %s %s %s|", botnick, lp_class3, our_user->nick, partmsg);
send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((our_user->type == VIP) && (strlen(welcomevip) > 0))
{
snprintf(joinstring, 300, "<%s> %s %s %s|", botnick, lp_class4, our_user->nick, partmsg);
send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
else if((our_user->type == KVIP) && (strlen(welcomekvip) > 0))
{
snprintf(joinstring, 300, "<%s> %s %s %s|", botnick, lp_class5, our_user->nick, partmsg);
send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
}
}
if((show_joinsparts > 0) && (our_user->type != NON_LOGGED) && (quit == 0) 
&& (strcmp(our_user->nick, "removed user") != 0) 
&& (strcmp(our_user->hostname, "parent_process") != 0) && (strcmp(our_user->nick, "Non_logged_in_user") != 0))
{
if(our_user->type == OWNER)
snprintf(usertype, 50, "%s", lp_class9);
else if(our_user->type == ADMIN)
snprintf(usertype, 50, "%s", lp_class8);
else if(our_user->type == CHEEF)
snprintf(usertype, 50, "%s", lp_class7);
else if(our_user->type == OP)
snprintf(usertype, 50, "%s", lp_class6);
else if(our_user->type == KVIP)
snprintf(usertype, 50, "%s", lp_class5);
else if(our_user->type == VIP)
snprintf(usertype, 50, "%s", lp_class4);
else if(our_user->type == REG)
snprintf(usertype, 50, "%s", lp_class3);
else if(our_user->type == PROTECTED)
snprintf(usertype, 50, "%s", lp_class2);
else if(our_user->type == REGULAR)
snprintf(usertype, 50, "%s", lp_class1);
human_user = human_sock_list;
while(human_user != NULL)
{
next_human = human_user->next;
if(((human_user->user->type & (ADMIN | OWNER)) != 0) && (human_user->user != our_user))
{
if(show_joinsparts == 1)
uprintf(human_user->user, "<%s> %s: %s (IP: %s), %s|", botnick, lp_quits, our_user->nick, our_user->hostname, usertype);
if(show_joinsparts == 2)
uprintf(human_user->user, "$To: %s From: %s $<%s> %s: %s (IP: %s), %s|", human_user->user, botnick, botnick, lp_quits, our_user->nick, our_user->hostname, usertype);
}
human_user = next_human;
}
if(show_joinsparts == 3)
{
snprintf(joinstring, 300, "<%s> %s: %s (IP: %s), %s|", botnick, lp_quits, our_user->nick, our_user->hostname, usertype);
send_to_humans(joinstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, our_user);
}
}
if((quit == 0) && (redall == 0) && (strcmp(our_user->nick, "removed user") != 0)) {
if(check_if_registered(our_user->nick) >= seen_class)
{
snprintf(seenbuf, 100, "%s %s", our_user->nick, our_user->hostname);
add_seen_user(seenbuf);
}
}
}

if(our_user->type == REGULAR)
{
if(strcasecmp(our_user->nick, nomyinfo) == 0)
nomyinfo[0] = '\0';
}

	if((remove_from_list != 0)
		&& (our_user->type & (REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER | SCRIPT)) != 0)
		remove_user_from_list(our_user->nick);

	if((our_user->type & (NON_LOGGED | REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER)) != 0) 
		remove_human_user(our_user);
	else
		remove_non_human(our_user);

}

/* Removes all users who have the rem variable set to non-zero */
void clear_user_list(void)
{
	struct user_t *non_human;
	struct user_t *next_non_human;
	struct sock_t *human_user;
	struct sock_t *next_human_user;

	non_human = non_human_user_list;
	human_user = human_sock_list;

	while(non_human != NULL)
	{
		next_non_human = non_human->next;
		if(non_human->rem != 0)
			remove_user(non_human, non_human->rem & SEND_QUIT, non_human->rem & REMOVE_FROM_LIST);

		non_human = next_non_human;
	}
	while(human_user != NULL) 
	{
		next_human_user = human_user->next;
		if(human_user->user->rem != 0)
			remove_user(human_user->user, human_user->user->rem & SEND_QUIT, human_user->user->rem & REMOVE_FROM_LIST);

		human_user = next_human_user;
	}
}

void clockset(void)
{
        char acttime[51];
        char quitstring[101];
	char *op_list;
	char ctime[16];
	char sdayn[5];                                                                                                     
	char sday[3];                                                                                                      
	char sdate[10];                                                                                                    
	char syear[5];                                                                                                     
	char smont[5]; 
	struct tm * timeinfo;
	time_t now;

    if(pid == 0)
    {
	now = time(NULL);
        timeinfo = localtime ( &now );                                                                             
        snprintf(acttime, 50, "%s", asctime (timeinfo));                                                                
        trim_string(acttime);                                                                                      
        sscanf(acttime, "%5s %5s %3s %10s %5s", sdayn, smont, sday, sdate, syear);                                 
	sdate[5] = '\0';
	snprintf(ctime, 15, "-=[%s]=-", sdate);     
	if(strcasecmp(clocknick, ctime) != 0)
	{
	snprintf(quitstring, 100, "$Quit %s|", clocknick);
	send_to_humans(quitstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	snprintf(clocknick, 50, "%s", ctime);
	snprintf(quitstring, 100, "$MyINFO $ALL %s [BOT] Server Time <DB V:%s,M:A,H:0/0/1,S:0>$ $$clock$0$|", clocknick, VERSION);	
	send_to_humans(quitstring, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
        op_list = get_op_list();                                                                                                
        send_to_humans(op_list, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL);
	free(op_list); 	
	}
    }
}

/********************************************************/
/* Get action from a connected socket  */
/* Returns -1 on error,                */
/* 0 on connection closed,             */
/* 1 on received message               */
int socket_action(struct user_t *user)
{
	int buf_len;
	char *command_buf;
	char buf[MAX_MESS_SIZE + 1];
	struct sock_t *human_user;
	struct sock_t *next_human;
	char awaybuf[151];
	time_t now;
	int i = 0;

	command_buf = NULL;

	/* Error or connection closed? */
	while(((buf_len = recv(user->sock, buf, MAX_MESS_SIZE, 0)) == -1) 
		&& ((errno == EAGAIN) || (errno == EINTR)))
	{
		i++;
		usleep(500);
		/* Giving up after half a second */
		if(i == 1000)
			break;	  		
	}
    
    	if(buf_len <= 0)
	{	
		/* Connection closed */
		if(buf_len == 0)
		{
			/* If it was a human user.  */
			if((user->type & (SCRIPT | FORKED)) == 0)
			{		       
				if((int)user->nick[0] > 0x20)
					logprintf(LOG_ALERT, "%s from %s at socket %d hung up\n", user->nick, user->hostname, user->sock);
				else
					logprintf(LOG_ALERT, "User at socket %d from %s hung up\n", user->sock, user->hostname);
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;		  		
			}
			else
			{
				if(pid == 0)
				return 1;
				
				/* If the parent process disconnected, exit this process.  */
				if(pid == -1)
				{
					if(count_users(SCRIPT | FORKED) == 1)
						kill_forked_process();
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
				}
			}
			return 0;
		} 
		else if(errno == ECONNRESET)
		{
			if((user->type & (SCRIPT | FORKED)) == 0)
			{		  
				if((int)user->nick[0] > 0x20)
					logprintf(LOG_ALERT, "%s from %s at socket %d hung up (Connection reset by peer)\n", user->nick, user->hostname, user->sock);
				else
					logprintf(LOG_ALERT, "User at socket %d from %s hung up (Connection reset by peer)\n", user->sock, user->hostname);
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			}	     
			else
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return 0;	       
		}
		else if(errno == ETIMEDOUT)
		{
			if((user->type & (SCRIPT | FORKED)) == 0)
			{		  
				if((int)user->nick[0] > 0x20)
					logprintf(LOG_ALERT, "%s from %s at socket %d hung up (Connection timed out)\n", user->nick, user->hostname, user->sock);
				else
					logprintf(LOG_ALERT, "User at socket %d from %s hung up (Connection timed out)\n", user->sock, user->hostname);
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			}	     
			else
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return 0;
		}
		else if(errno == EHOSTUNREACH)
		{
			if((user->type & (SCRIPT | FORKED)) == 0)
			{		  
				if((int)user->nick[0] > 0x20)
					logprintf(LOG_ALERT, "%s from %s at socket %d hung up (No route to host)\n", user->nick, user->hostname, user->sock);
				else
					logprintf(LOG_ALERT, "User at socket %d from %s hung up (No route to host)\n", user->sock, user->hostname);
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			}
			else
				user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
			return 0;	       
		}
		else
		{
			logprintf(LOG_WARNING, "Error - In get_socket_action()/socket_action()/recv() when receiving from %s: %s", user->hostname, strerror(errno));
			return -1;
		}
	} 
	else 
	{
		/* Set the char after the last received one in buf to null in case the memory
		* position was set to something else than null before */
		buf[buf_len] = '\0';

		/* If the inbuf is empty */
		if(user->buf == NULL)
		{
			if((command_buf = malloc(sizeof(char) * (buf_len + 1))) == NULL)
			{
				logprintf(LOG_EMERG, "Error - In socket_action()/malloc(): %s", strerror(errno));
				quit = 1;
				return -1;
			}
			strcpy(command_buf, buf);
			if(strchr(command_buf, '|') != NULL)
			{
				if(handle_command(command_buf, user) == 0)
				{
					user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
					free(command_buf);
					return 0;
				}
				else
				{
				//DB// Checking for autoaway
				if((user->autoawaytime > 0) && (user->away_flag == 1) && (user->type != NON_LOGGED) && (user->hide_share != 2))
				{
				now = time(NULL);
				if((difftime(now, user->last_write) > (60 * user->autoawaytime)) && (difftime(now, user->last_pm_write) > (60 * user->autoawaytime)) && (difftime(now, user->jointime) > 300))
				{
				user->away_flag = 2;
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		free(command_buf);
		return 0;
		}
				snprintf(awaybuf, 150, "%s %100s|", user->nick, user->autoawayreason); 
				uprintf(user, "<%s> %s|", botnick, lp_autoawaymodeon); 
				add_away_user(awaybuf); 
				snprintf(awaybuf, 150, "<----> %s %s (%s: %s)|", user->nick, lp_isaway, lp_reason, user->autoawayreason); 
				if(no_chat_for_chatgagged == 0)
				send_to_humans(awaybuf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL); 
				else                                                                                                                         
				{                                                                                                                            
				human_user = human_sock_list;                                                                                        
				while(human_user != NULL) 
				{                                                                                                                    
				next_human = human_user->next;                                                                                       
				if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))                                                                                                                                                                                 
				uprintf(human_user->user, "%s", awaybuf);                                                                              
				human_user = next_human;                                                                                             
				}                                                                                                                    
				}   
				}
				}								                
				}
			}

			/* If the string doesn't contain the '|' at all */
			if(strchr(buf, '|') == NULL)
			{
				if((user->buf = malloc(sizeof(char) * (buf_len + 1))) == NULL)
				{
					logprintf(LOG_EMERG, "Error - In socket_action()/malloc(): %s", strerror(errno));
					quit = 1;
					free(command_buf);
					return -1;
				}
				strcpy(user->buf, buf);
			}
			else
				/* If the string continues after the last '|' */
			{
				if((user->buf = malloc(sizeof(char) * strlen(strrchr(buf, '|')))) == NULL)
				{
					logprintf(LOG_EMERG, "Error - In socket_action()/malloc(): %s", strerror(errno));
					quit = 1;
					free(command_buf);
					return -1;
				}
				strcpy(user->buf, strrchr(buf, '|') + 1);
			}
		}
		else
			/* We have something in the inbuf */
		{
			if((command_buf = malloc(sizeof(char) * (buf_len + strlen(user->buf) + 1))) == NULL)
			{
				logprintf(LOG_EMERG, "Error - In socket_action()/malloc(): %s", strerror(errno));
				quit = 1;
				return -1;
			}
			strcpy(command_buf, user->buf);
			strcat(command_buf, buf);
			if(strchr(command_buf, '|') != NULL)
			{
				if(handle_command(command_buf, user) == 0)
				{
					user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
					free(command_buf);
					return 0;
				}
				else
				{
				//DB// Checking for autoaway
				if((user->autoawaytime > 0) && (user->away_flag == 1) && (user->type != NON_LOGGED) && (user->hide_share != 2))
				{
				now = time(NULL);
				if((difftime(now, user->last_write) > (60 * user->autoawaytime)) && (difftime(now, user->last_pm_write) > (60 * user->autoawaytime)) && (difftime(now, user->jointime) > 300))
				{
				user->away_flag = 2;
		if(send_user_info(user, user->version) == 0)
		{
		user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
		free(command_buf);
		return 0;
		}
				snprintf(awaybuf, 150, "%s %100s|", user->nick, user->autoawayreason); 
				uprintf(user, "<%s> %s|", botnick, lp_autoawaymodeon); 
				add_away_user(awaybuf); 
				snprintf(awaybuf, 150, "<----> %s %s (%s: %s)|", user->nick, lp_isaway, lp_reason, user->autoawayreason); 
				if(no_chat_for_chatgagged == 0)
				send_to_humans(awaybuf, REGULAR | PROTECTED | REG | VIP | KVIP | OP | CHEEF | ADMIN | OWNER, NULL); 
				else                                                                                                                         
				{                                                                                                                            
				human_user = human_sock_list;                                                                                        
				while(human_user != NULL)                                                                                            
				{                                                                                                                    
				next_human = human_user->next;                                                                                       
				if((human_user->user->gagc == 0) && (human_user->user->type != NON_LOGGED))                                                                                                                                                                                 
				uprintf(human_user->user, "%s", awaybuf);                                                                              
				human_user = next_human;                                                                                             
				}                                                                                                                    
				}   
				}
				}								                
				}
			}

			/* If the string doesn't contain a '|' */
			if(strchr(buf, '|') == NULL)
			{
				if((user->buf = realloc(user->buf, sizeof(char) 
					* (buf_len + strlen(user->buf) + 1))) == NULL)
				{
					logprintf(LOG_EMERG, "Error - In socket_action()/realloc(): %s", strerror(errno));
					quit = 1;
					free(command_buf);
					return -1;
				}
				strcat(user->buf,  buf);

				/* The buf shouldn't be able to grow too much. If it gets 
				* really big, it's probably due to some kind of attack */
				if(strlen(user->buf) >= MAX_BUF_SIZE)
				{
					if(user->rem == 0)
						logprintf(LOG_ALERT, "User from %s had too big buf, kicking user\n", user->hostname);
					user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
				}
			}	     

			/* If the string continues after the last '|' */
			else if(strlen(strrchr(buf, '|')) > 1)
			{
				if((user->buf = realloc(user->buf, sizeof(char) 
					* strlen(strrchr(buf, '|')))) == NULL)
				{
					logprintf(LOG_EMERG, "Error - In socket_action()/realloc(): %s", strerror(errno));
					quit = 1;
					free(command_buf);
					return -1;
				}
				strcpy(user->buf, strrchr(buf, '|') + 1);

				/* The buf shouldn't be able to grow too much. If it gets 
				* really big, it's probably due to some kind of attack.  */
				if(strlen(user->buf) >= MAX_BUF_SIZE)
				{
					if(user->rem == 0)
						logprintf(LOG_EMERG, "User from %s had to big buf, kicking user\n", user->hostname);
					user->rem = REMOVE_USER | SEND_QUIT | REMOVE_FROM_LIST;
				}
			}


			/* The string ends with the '|' */
			else
			{	
				free(user->buf);
				user->buf = NULL;	
			}
		}

		logprintf(LOG_NOTICE, "PID: %d Received command from %s, type 0x%X: %s\n", 
			(int)getpid(), user->hostname, user->type, command_buf);

		if(command_buf != NULL)
			free(command_buf);

		return 1;
	}
}


/* Takes password and encrypts it. http://www.gnu.org/manual/glibc-2.2.5/html_node/crypt.html */ 
void encrypt_pass(char* password)
{
	unsigned long seed[2];
	char salt[] = "$1$........";

	const char *const seedchars = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	int i;

	seed[0] = time(NULL);								/* Maybe using /dev/urandom. */
	seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

	/* Turn it into printable characters from `seedchars'. */
	for (i = 0; i < 8; i++)
		salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];
	if(crypt_enable != 0)
		strcpy(password, crypt(password, salt));
}


/** @brief daemonizes the current process
 *
 *  Function to daemonize the current process, by forking
 *  and then closing stdio channels
 */

void daemonize (void){
	pid = fork();
	if(pid < 0){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(pid > 0) exit(EXIT_SUCCESS);
	if(setsid() < 0) {
		perror("setsid");
		exit(EXIT_FAILURE);
	}
	if(close(STDIN_FILENO) != 0) {
		logprintf(LOG_EMERG, "Error - When closing STDIN_FILENO, exiting\n");
		exit(EXIT_FAILURE);
	}
	if(close(STDOUT_FILENO) != 0) {
		logprintf(LOG_EMERG, "Error - When closing STDOUT_FILENO, exiting\n");
		exit(EXIT_FAILURE);
	}
	if(close(STDERR_FILENO) != 0) {
		logprintf(LOG_EMERG, "Error - When closing STDERR_FILENO, exiting\n");
		exit(EXIT_FAILURE);
	}
}


/**********************************************************/
/* Main function */
int main(int argc, char *argv[])
{
 	static struct option long_options[]=
 	{
 		{ "help", no_argument, 0, 'h'},
 		{ "version", no_argument, 0, 'v'},
 		{ "workdir", required_argument, 0, 'w'},
 		{ "logfile", required_argument, 0, 'l'},
 		{ "debug", optional_argument, 0, 'd'},
#ifdef SWITCH_USER
		{ "uid", required_argument, 0, 'u'},
 		{ "gid", required_argument, 0, 'g'},
#endif
		{ "syslog", no_argument, 0, 's'},
 		{ 0, 0, 0, 0}
 	};
 
 	int optres, optindex;

	int ret;
	int erret;
#ifdef SWITCH_USER
	struct passwd *userinfo;
	struct group *groupinfo;
	int got_user = 0;
	int got_group = 0;
	cap_t cap;
	int cap_failed = 0;


	cap_value_t caps[1];
	caps[0] = CAP_NET_BIND_SERVICE;
#endif

	max_sockets = getdtablesize();

#ifndef HAVE_POLL
# ifdef FD_SETSIZE
	if(max_sockets > FD_SETSIZE)
		max_sockets = FD_SETSIZE;
# endif
#endif

	/* Init some variables */
	listening_socket = -1;
	debug = 0;
	do_purge_user_list = 0;
	upload = 0;
	quit = 0;
	script_reload = 0;
	banwords = NULL;
	hub_full_msg = NULL;

	if((banreason = malloc(sizeof(char) * 2)) == NULL)
	{
		logprintf(LOG_EMERG, "Error - In main()/malloc(): %s", strerror(errno));
		quit = 1;
		return 0;
	}
	if((ipgagreason = malloc(sizeof(char) * 2)) == NULL)
	{
		logprintf(LOG_EMERG, "Error - In main()/malloc(): %s", strerror(errno));
		quit = 1;
		return 0;
	}
	non_human_user_list = NULL;
	newmyinfo = 0;
	human_sock_list = NULL;
	memset(logfile, 0, MAX_FDP_LEN+1);
	working_dir[0] = '\0';
	brokenur = 0;
	brokensr = 0;
	pmspam = 0;
	prvset = 0;
	isupt = 0;
	isupt2 = 0;
	redall = 0;
	protdays = 0;
	tthsearches = 0;
	ip_banned = 0;
	nick_banned = 0;
	blocked_searches = 0;
	blocked_downloads = 0;
	accepted_searches = 0;
	accepted_downloads = 0;
	blocked_mc = 0;
	commandsnmb = 0;
	accepted_mc = 0;
	blocked_pm = 0;
	accepted_pm = 0;
	totalvisitors = 0;
	clocknick[0] = '\0';
	allreleases = 0;
	h_interval = 0;
	recordusers = 0;
	recordshare = 0;
        shdatetime[0] = '\0';
        usdatetime[0] = '\0';
	actch = 0;
	message1[0] = '\0';
	message2[0] = '\0';
	message3[0] = '\0';
	message4[0] = '\0';
	message5[0] = '\0';
	message6[0] = '\0';
	message7[0] = '\0';
	message8[0] = '\0';
	message9[0] = '\0';
	message10[0] = '\0';
	message11[0] = '\0';
	message12[0] = '\0';
	message13[0] = '\0';
	message14[0] = '\0';
	message15[0] = '\0';
	message16[0] = '\0';
	message17[0] = '\0';
	message18[0] = '\0';
	message19[0] = '\0';
	message20[0] = '\0';
	lastperm1[0] = '\0';
	lastperm2[0] = '\0';
	lastperm3[0] = '\0';
	lastperm4[0] = '\0';
	lastperm5[0] = '\0';
	lastperm6[0] = '\0';
	lastperm7[0] = '\0';
	lastperm8[0] = '\0';
	lastperm9[0] = '\0';
	lastperm10[0] = '\0';
	lastperm11[0] = '\0';
	lastperm12[0] = '\0';
	lastperm13[0] = '\0';
	lastperm14[0] = '\0';
	lastperm15[0] = '\0';
	lastperm16[0] = '\0';
	lastperm17[0] = '\0';
	lastperm18[0] = '\0';
	lastperm19[0] = '\0';
	lastperm20[0] = '\0';
	lasttemp1[0] = '\0';
	lasttemp2[0] = '\0';
	lasttemp3[0] = '\0';
	lasttemp4[0] = '\0';
	lasttemp5[0] = '\0';
	lasttemp6[0] = '\0';
	lasttemp7[0] = '\0';
	lasttemp8[0] = '\0';
	lasttemp9[0] = '\0';
	lasttemp10[0] = '\0';
	lasttemp11[0] = '\0';
	lasttemp12[0] = '\0';
	lasttemp13[0] = '\0';
	lasttemp14[0] = '\0';
	lasttemp15[0] = '\0';
	lasttemp16[0] = '\0';
	lasttemp17[0] = '\0';
	lasttemp18[0] = '\0';
	lasttemp19[0] = '\0';
	lasttemp20[0] = '\0';
	hammertime = (time_t)0;
	hammernick[0] = '\0';
	botinfolater[0] = '\0';
	nomyinfo[0] = '\0';
	topicstring[0] = '\0';
	botcheck = 0;
	snprintf(pmspammer, 50, "noone");

	total_share = 0;

 	/* Switch to option processing routines to handle arguments */
 
#ifdef SWITCH_USER 
 	while ( (optres=getopt_long(argc, argv, "shvu:g:w:l:d::", long_options, &optindex))!=-1){
#else
	while ( (optres=getopt_long(argc, argv, "shvw:l:d::", long_options, &optindex))!=-1){
#endif
 	switch (optres){
 	   case 'h':
 		printf("DB Hub, version %s\n", VERSION);
 		printf("  -d, --debug [level]     : Debug mode. Prevents DB Hub from making itself a\n                            background daemon. Optional level ist verbosity.\n");
 		printf("  -h, --help              : Print this help and exit.\n");+ 		printf("  -v, --version           : Print version.\n");
 		printf("  -l, --logfile <logfile> : Set logfile.\n");
		printf("  -s, --sylog             : Enable Syslog logging.\n");
 		printf("  -w, --workdir <path>    : Set the path to the working directory.\n");
#ifdef SWITCH_USER
 		printf("  -u, --uid <user>        : User to switch to run as.\n");
 		printf("  -g, --gid <group>       : Group to switch to run as.\n");
#endif
 		exit(EXIT_SUCCESS);
 	   case 'v':
 		printf("DB Hub %s\n", VERSION);
 		exit(EXIT_SUCCESS);
 	   case 'w':
 		strncpy(working_dir, optarg, MAX_FDP_LEN);
 		if((ret = access(working_dir, F_OK)) < 0)
  		{
 			printf("Directory does not exist: %s\n", working_dir);
 			perror("access");
 			exit(EXIT_FAILURE);
  		}
 		break;
 	   case 'l':
 	   	strncpy(logfile,optarg,MAX_FDP_LEN);
 		if((ret = open(logfile, O_RDWR | O_CREAT, 0600)) >= 0)
 		{
 			/* Set logfile. */
 			printf("Using logfile: %s\n", logfile);
 			close(ret);
  		}
 		else
  		{
 			printf("Couldn't open logfile: %s\n", logfile);
 			perror("open");
 			exit(EXIT_FAILURE);
  		}
 	   	break;
	   case 's':
	   	logusesyslog=1;
		break;
 	   case 'd':
 	   	debug=1;
 		if (optarg) verbosity=atoi(optarg);
 		break;
#ifdef SWITCH_USER
 	   case 'u':
 	   	userinfo = getpwnam(optarg);
 		if(userinfo == NULL)
  		{
 			printf("Couldn't locate user: %s\n", optarg);
 			perror("getpwnam");
 			exit(EXIT_FAILURE);
  		}
 		dchub_user = userinfo->pw_uid;
 		got_user = 1;
 		if(got_group == 0) dchub_group = userinfo->pw_gid;
 		break;
 	   case 'g':
 	   	groupinfo = getgrnam(optarg);
 		if(groupinfo == NULL)
  		{
 			printf("Couldn't locate group: %s\n", optarg);
 			perror("getgrnam");
 			exit(EXIT_FAILURE);
  		}
 		dchub_group = groupinfo->gr_gid;
 		got_group = 1;
 		break;
#endif
 	   case '?':
 		break;
  	}
 	}
 

	
	
#ifdef SWITCH_USER
	if (got_user)
	{
		if ((geteuid() == 0) && ((cap = cap_init()) != NULL))
		{
			if (prctl(PR_SET_KEEPCAPS, 1))
				cap_failed = 1;
			else if (setgroups(0, NULL) == -1)
				cap_failed = 1;
			else if ((setegid(dchub_group) == -1)
				|| (seteuid(dchub_user) == -1))
				cap_failed = 1;
			else if (cap_set_flag(cap, CAP_EFFECTIVE, 1, caps, CAP_SET) == -1)
				cap_failed = 1;
			else if (cap_set_flag(cap, CAP_PERMITTED, 1, caps, CAP_SET) == -1)
				cap_failed = 1;
			else if (cap_set_flag(cap, CAP_INHERITABLE, 1, caps, CAP_SET) == -1)
				cap_failed = 1;
			else if (cap_set_proc(cap) == -1)
				cap_failed = 1;
			else if ((setresgid(dchub_group, dchub_group, dchub_group) == -1) ||
				(setresuid(dchub_user, dchub_user, dchub_user) == -1))
				cap_failed = 1;
			else if (setuid(0) == 0)
				cap_failed = 1;
			cap_free(cap);
		}
		else
			cap_failed = 1;

		if(cap_failed != 0)
		{
			perror("Error in switching user\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		dchub_user = getuid();
		dchub_group = getgid();
	}
#endif


	/* This is only a list of addresses to users, not users, so it won't be that
	* space consuming although this will use more memory than a linked list.
	* It's simply faster operation on behalf of more memory usage. */
	if((human_hash_table = calloc(max_sockets + 1, sizeof(struct user_t *))) == NULL)
	{
		printf("Couldn't initiate human_hash_table.\n");
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	if(init_dirs() == 0)
		return 1;
	logopen();
	logprintf(LOG_EMERG, "*** Started DB Hub ver %s (released: %s) ***\n", VERSION, DBDATE);
	hub_start_time = time(NULL);

//	set_default_lang();
#ifdef HAVE_LANG
	if(read_lang() == -1)
	set_default_lang();
	write_lang_file();
#endif
	//DB// First setting variables to default...
	set_default_vars();

	//DB// Then read config to override
	if(read_config() == -1)
	{
	//DB// If config wasn't found prompt user for listening_port
	printf("-( Welcome to the DB Hub )-\n\n");
	printf("Enter port number to listen for connections. \nPorts below 1024 is only for root: ");
	scanf("%u", &listening_port);
	if(listening_port == 0)
	{
		printf("Bad port number\n");
		exit(EXIT_FAILURE);
	}
	printf("Listening Port set to %u\n\n", listening_port);
	}
	
	//DB// finally write all to config file
	if(write_config_file() == -1)
	{
			logprintf(LOG_EMERG, "Failed writing config file! Exiting\n");
			exit(EXIT_FAILURE);
	}
	logprintf(LOG_ALERT, "Created config file\n");
	

	create_files();
	CreateAllMOTDs();
	LoadAllMOTDs();
	loadfile2buf(&rules_mem,RULES_FILE);
	loadfile2buf(&regme_mem,REGME_FILE);
	loadfile2buf(&autofav_mem,AUTOFAV_FILE);
	loadfile2buf(&schedule_mem,SCHEDULE_FILE);
	loadfile2buf(&off_mem,MSGS_FILE);
	check_settopic();
	rls_motd();
	req_motd();		
	
	if((int)hub_hostname[0] <= 0x20)
	{
		if(set_hub_hostname() == -1)     
		sprintf(hub_hostname, "127.0.0.1");
	}

	/* Test if we can open the listening socket.  */
	if((listening_socket = get_listening_socket(listening_port, 0)) == -1)
	{
		printf("Bind failed.\nRemember, to use a listening port below 1024, you need to be root.\nAlso, make sure that you don't have another instance of the program\nalready running.\n");
		close(listening_unx_socket);
		close(listening_udp_socket);
		return 1;
	}

	while(((erret =  close(listening_socket)) != 0) && (errno == EINTR))
		logprintf(LOG_EMERG, "Error@close(): %s. Retrying.",strerror(errno));	

	if(erret != 0)
	{	
		logprintf(LOG_EMERG, "Error@close(): %s",strerror(errno));
		return 1;
	}

	listening_socket = -1;
	
	if((listening_unx_socket = get_listening_unx_socket()) == -1)     
		return 1;

	if((listening_udp_socket = get_listening_udp_socket(listening_port)) == -1)
	{
		printf("Bind failed.\nRemember, to use a listening port below 1024, you need to be root.\nAlso, make sure that you don't have another instance of the program\nalready running.\n");
		close(listening_unx_socket);
		return 1;     
	}

	/* Tell user that hub is running */
	printf("---------------------------------------------------------------------------\n");
	printf("DB Hub %s is up and running. Listening for user connections on port %u\n", VERSION, listening_port);
	printf("---------------------------------------------------------------------------\n");
	/* With -d, for debug, we will run in console so skip this part. */
	if(debug == 0) daemonize(); /* Make program a daemon */

	/* Set pid */
	pid = getpid();

	/* Initialize the semaphores.  */
	if(init_sem(&user_list_sem) ==  -1)
	{
		logprintf(LOG_EMERG, "Couldn't initialize the user list semaphore.\n");
		exit(EXIT_FAILURE);
	}

	if(init_user_list() == -1)
	{
		logprintf(LOG_EMERG, "Couldn't initialize the user list.\n");
		semctl(user_list_sem, 0, IPC_RMID, NULL);
	}

	init_sig();

	/* Send initial alarm */
	if((kill(pid, SIGALRM)) < 0)
	{
		return 1;
	}
	if(count_bots > 0)
	{
	if(clockbot == 1)
	total_users = 3;
	else
	total_users = 2;
	}
	
	loadfile2buf(&away_mem,AWAYS);
	loadfile2buf(&autoaway_mem,AUTOAWAYS);
	loadfile2buf(&banlist_mem,BAN_FILE);
	loadfile2buf(&nickbanlist_mem,NICKBAN_FILE);
	loadfile2buf(&allowlist_mem,ALLOW_FILE);
	loadfile2buf(&ipgaglist_mem,IPGAG_FILE);
	loadfile2buf(&warn_mem,WARN_FILE);
	loadfile2buf(&hide_mem,HIDE_FILE);

if(check_if_record("users") == 0)
{
	recordusstr[0] = '\0';
	recordusers = 0;
}
if(check_if_record("share") == 0)
{
	recordshstr[0] = '\0';
	recordshare = 0;
}
	loadfile2buf(&reglist_mem,REG_FILE);

total_scripts = 0;
total_users_scr = 0;
total_share_scr = 0;
	
	/* Init perl scripts */
#ifdef HAVE_PERL	
	if(perl_init() == 0)
		logprintf(LOG_EMERG, "Error - Perl initialization failed.\n");
	else if(pid <= 0)
		sub_to_script("started_serving|");

#endif 
	/* Fork process which holds the listening sockets.  */
	if(pid > 0)
		fork_process();
	while(quit == 0)
	{
		if(pid > 0)
		{
#ifdef HAVE_PERL
			if(script_reload != 0)
			{
				perl_init();
				loadfile2buf(&reglist_mem,REG_FILE);
				script_reload = 0;
			}
#endif
		}
		if(pid == 0)
		{
			if(do_purge_user_list != 0)
			{
				purge_user_list();
				read_config();
				do_purge_user_list = 0;
			}	     
		}
		get_socket_action();
		if(alarmflag){alarm_handler();alarmflag=0;}
		if(clockbot == 1)
		clockset();
		clear_user_list();
	}
	quit_program();
	FreeAllMOTDs();
	free(reglist_mem);reglist_mem=NULL;
	free(rules_mem);rules_mem=NULL;
	free(regme_mem);regme_mem=NULL;
	free(autofav_mem);autofav_mem=NULL;
	free(schedule_mem);schedule_mem=NULL;
	free(off_mem);off_mem=NULL;
	free(away_mem);away_mem=NULL;
	free(autoaway_mem);autoaway_mem=NULL;
	free(banlist_mem);banlist_mem=NULL;
	free(nickbanlist_mem);nickbanlist_mem=NULL;
	free(allowlist_mem);allowlist_mem=NULL;
	free(ipgaglist_mem);ipgaglist_mem=NULL;
	free(warn_mem);warn_mem=NULL;
	free(hide_mem);hide_mem=NULL;
	/*free other dynamic structures for cleanup) */
	free(human_hash_table);human_hash_table=NULL;
	free(hub_full_msg);hub_full_msg=NULL;
	/*close the log file properly */
	closelog();
	exit(EXIT_SUCCESS);
}


