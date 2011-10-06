/** @file main.h
 * @brief Main Header File
 *
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include <sys/types.h>
#include <sys/signal.h>

/* Using the 32 bit int (sometimes even 64 bits) for boolean variables and
* other variables that always will be between -128 and 127 would be a waste
* of space, especially those in the user_t struct since it's used to 
* frequently.  */

#define DBDATE "02/02/2008"

#define ALARM_TIME        300             /*!< Seconds between alarm calls */ 
#define MAX_NICK_LEN       50              /*!< Maximum length of nickname */
#define MAX_HOST_LEN       121             /*!< Maximum length of hostname */
#define MAX_VERSION_LEN    30              /*!< Maximum length of version name */
#define MAX_MESS_SIZE      0xFFFF          /*!< Maximum size of a received message */
#define MAX_HUB_NAME       60              /*!< Maximum length of hub name */
#define MAX_ADMIN_PASS_LEN  50 
#define MAX_TOPIC_LEN      256             /*!< Maximum length of actual topic */
#define MAX_MSG_LEN         256             /*!< Maximum length of ban, regonly message */ 
#define MAX_HUB_DESC       100             /*!< Maximum length of hub description */
#define MAX_BUF_SIZE       100000         /*!< Maximum length of users buf */
#define MAX_FDP_LEN	   200		   /*!< Maximum length of file/dir/path variables */
#define USER_LIST_ENT_SIZE 173             /*!< Size of an entry in the user list nick length + host length.  */

#define CONFIG_FILE        "config"        /*!< Name of config file */
#define BAN_FILE           "banlist"       /* Name of file with banlist */
#define HIDE_FILE           "hiddenlist"       /* Name of file with banlist */
#define SEEN_FILE          "seenlist"      /* name of file with seenlist */
#define NICKBAN_FILE       "nickbanlist"   /* Name of file with nick banlist */
#define ALLOW_FILE         "allowlist"     /* Name of file with allowlist */
#define REG_FILE           "reglist"       /* Name of file with list of registered users */
#define LOG_FILE           "log"           /* Name of log file */

#define AWAYS	           "aways"         /* Name of aways file */
#define AUTOAWAYS	   "autoaways"         /* Name of aways file */
#define IPGAG_FILE         "ipgaglist"
#define IPGAGRS_FILE         "ipgagreasons"
#define URL_FILE           "urllist"
#define RELEASE_FILE       "releaselist"
#define REQ_FILE  	   "reqlist"
#define BANRS_FILE	   "banreasons"
#define RECORDS_FILE	   "hubrecords"
#define PROT_FILE	   "permprot"
#define REGME_FILE	   "regme"
#define RULES_FILE	   "rules"
#define SETTOPIC           "settopic"
#define MSGS_FILE       "savedmsgs"		
#define NICKBANRS_FILE     "nickbanreasons"
#define AUTOFAV_FILE     "autofavmsg"
#define SCHEDULE_FILE     "scheduledmsg"
#define WELCOME_FILE     "randomwelcomes"
#define CATEG_FILE	   	"categories"
#define WARN_FILE "warnfile"
#define UN_SOCK_NAME       "dbhub"          /* Name of unix socket file */
#define USER_LIST          "dbhublist"      /* Name of temporary user list file */
#define SCRIPT_DIR         "scripts"       /* Name of script directory.  */
#define EXTRAFILES_DIR         "extra_files"       /* Name of extra files directory.  */
#define LOG_DIR            "logs"       /* Name of logs directory.  */
#define SYSLOG_IDENT       "dbhub"          /* Identity for system log */
#define LANG_FILE           "lang"

#define INIT_MESS          1
#define HELLO_MESS         2 
#define HUB_FULL_MESS      3
#define GET_PASS_MESS      4
#define LOGGED_IN_MESS     5
#define OP_LOGGED_IN_MESS  6
#define BAD_PASS_MESS      7
#define	GET_PASS_MESS2	   8
#define NOPASS_MESS        9

/* The different user types */
#define NON_LOGGED         0x1
#define REGULAR            0x2
#define PROTECTED	   0x4
#define REG	           0x8
#define VIP		   0x10
#define KVIP		   0x20
#define OP                 0x40
#define CHEEF		   0x80
#define ADMIN		   0x100
#define OWNER		   0x200
#define FORKED             0x400
#define SCRIPT		   0x800

#define PRIV               0
#define TO_ALL             1

#define ALLOW              0
#define BAN                1
#define REGLIST            2
#define CONFIG             3
#define NICKBAN            5
#define SEEN		   6
#define IPGAG		   7	
#define BW		   8
#define URL		   9
#define RELEASE           10
#define BANRS 		  11
#define NICKBANRS	  12
#define REGME		  13
#define RULES		  14
#define RANDWEL 	  15
#define AUTOFAV		  16
#define CATEG		  17
#define REQ		  18
#define LANG		  19
#define WARN		  20
#define IPGAGRS		  21
#define BANRANGE          22
#define ALLOWRANGE        23

#define HOST               0
#define IP                 1

#ifndef HAVE_STRTOLL
# ifdef HAVE_STRTOQ
#  define strtoll(X, Y, Z) (long long)strtoq(X, Y, Z)
# endif
#endif

/* Possible values for user->rem  */
#define REMOVE_USER        0x1 
#define SEND_QUIT          0x2
#define REMOVE_FROM_LIST   0x4

int    user_list_shm_shm;  /* Identifier for shared memory segment containing the shared memory segment for the user list :)  */
int    user_list_sem;      /* And a semaphore to control access to it.  */ 

int    listening_socket;            /* Socket for incoming connections from clients */
int    listening_unx_socket;        /* Socket for forked processes to connect to */
int    listening_udp_socket;        /* Socket for incoming multi-hub messages */

struct user_t 
{ 
	int sock;                          /* What socket the user is on */ 
	long unsigned ip;                  /* IP address of user */ 
	char hostname[MAX_HOST_LEN+1];     /* IP address of user as char pointer */
	int  type;                         /* Type of user, types defined above. */
	char nick[MAX_NICK_LEN+1];         /* Nickname of user */ 
	char version[MAX_VERSION_LEN+1];   /* Version of client */ 
	char *email;                       /* Email of user, optional */ 
	char *desc;                        /* Description of users with TAG */ 
	char *con_type;                    /* Connection types of users */ 
	char password[51];		   /* User's password, if registered */
	int flag;                          /* Users flag, represented by one byte (1 means 'away') */ 
	int away_flag;			   /* If user used !away or !autoaway and was marked to be in away mode */
	int haszerobytes;		   /* User was marked as the one who have no sharesize */
	int gaggedonminshare;		   /* User didn't share enough data and was gagged from searching/downloading */
	int autoawaytime;	           /* Time in minutes when autoaway is fired */
	char autoawayreason[151];	   /* Reason of being in autoaway mode */
	long long share;                   /* Size of users share in bytes */
	char *buf;                         /* If a command doesnt't fit in one packet, it's saved here for later */
	char *outbuf;                      /* If a command doesnt't fit in one packet, it's saved here for later */
	char fromnick[MAX_NICK_LEN+1];
	struct user_t *next;               /* Next user in list*/
	int mode;			   /* User's mode: 1-active, 2-passive */
	int nogetinfo;			   /* 1 if user supports NoGetINFO */
	int gaginf;
	int rem;                           /* 1 if user is to be removed */
    	int wasreg;
	int tmpop;			   /* 1 if users was temporary opped with !op command */
	int gagc;			   /* 1 means user is gagged from chatting */
	int gagd;			   /* 1 means user is gagged from downloading */
	int gags;			   /* 1 means user is gagged from searching */
	time_t last_search;                /* Time of the last search attempt */
	time_t last_pm_write;              /* Last sending of private message */
	time_t last_write;		   /* Last write to hub */
	time_t jointime;		   /* Time when user has joined to the hub */
	time_t srtime;
	int freeslots;			   /* Number of free slots of active user */
	int allslots;			   /* Number of all slots of active user */
	int too_fast_write_count;	   /* How much user write too fast */
        int hide_share;			   /* 1 if user's sharesize is displyed as 0, 2 if user is hidden */
	char *myinfos;
	char *myinfons;
	char *myinfoons;
	int isbanned; 			   /* 1 if user's IP is banned */
};

/* This is used for a linked list of the humans. This is to get faster 
* send_to_all:s. I'm pretending it contains the user's sockets, but it really
* only contains pointers to the users.  */
struct sock_t 
{
	struct user_t *user;
	struct sock_t *next;
};

//DB// config variables
char	hub_name[MAX_HUB_NAME+1];    	/* Name of the hub */
char	hub_hostname[121];		/* This is the hostname that is uploaded to the public hublist, so don't try setting this to "127.0.0.1" or "localhost" */
unsigned int listening_port;        	/* Port on which we listen for connections */
char	hub_description[MAX_HUB_DESC+1];/* The description of hub that is uploaded to public hublist */
int	max_users;			/* Maximum number of users allowed the be on the hub in the same time */
int 	registered_only;             	/* 1 for registered only mode, else 0 */
char	default_pass[MAX_ADMIN_PASS_LEN+1]; /* If set, all unregistered users will be prompted for it while logging in */
char	default_prefixes[256]; 		/* If set, unregistered user will have to start their nicknames with one of these prefixes */
char   	nickchars[301]; 		/* If set, all unregistered users with characters in nicknames not listed in this variable will get kicked */
char   	forbchars[31]; 			/* If set, all unregistered users with nickname started with one of these characters will get kicked */
long long min_share;      		/* Minimum share for REGULAR class */
long long min_psv_share;      		/* Minimum share for REGULAR class users in passive mode  */
long long min_reg_share;		/* Minimum share for REG class */
long long min_vip_share;		/* Minimum share for VIP class */
long long min_kvip_share;		/* Minimum share for KVIP class */
long long min_op_share;			/* Minimum share for OP class */
int   	kick_on_zeroshare;		/* If set to 1 kicks REGULAR users for not having any shared data */
int  	gag_on_minshare;		/* If set to 1 gags user from searching/downloading instead of kicking for not enough shared data */
int 	max_share;  			/* Maximum data that can be shared by REGULAR user */
char   	allowed_clients[MAX_MSG_LEN+1]; /* List of allowed clients (DC++ is ++) */
int 	check_class; 			/* Maximum class that will have TAG description checked */
int 	min_command_class;		/* Minimum class that can use commands */
int 	min_usercommand_class;		/* Minimum class that will receive UserCommands after logging in */
int 	count_reghubs;			/* If set to 1, reghubs will be added to normalhubs (H:n/r/o) while checking for maxhubs */
int 	maxhubs;			/* Maximum allowed number of opened hubs */
int 	minslots;			/* Minimum required number of opened upload slots */
int 	maxslots;			/* Maximum allowed number of opened upload slots */
int 	reverse_ratio;			/* If set to 0 ratio is hubs/slot, if 1 slots/hub */
int 	ratio;				/* Maximum hubs/slot or slots/hub ratio allowed due to reverse_ratio */
char   	mindcppver[11];			/* Miniumum version of DC++ required */
size_t 	min_nick_len;			/* Miniumum nickname lenght required for unregistered users */
int 	deny_passive;			/* If set to 1 kicks REGULAR user in passive mode */
int 	deny_proxy;			/* If set to 1 kicks REGULAR user using proxy to connect to the hub */
int 	restrict_mode;			/* If set to 1 kicks REGULAR user for tag M:A and psv string in nickname */
int 	connlimit;			/* Minimum upload limit allowed, works with tags B: and L: and with [Netlimiter Xkb/s connection] type */
char   	regmeprefix[21]; 		/* Default prefix taht is added to nicknames of users using !regme command */
char 	protectmeprefix[21];		/* Default prefix that is added to nicknames of users using !protectme command */
int 	hidetags;			/* Minimum class allowed to see descriptions/TAGs/emails of all users */
int   	hide_allshare;			/* If set to 1, prentends that all users on the hub have no sharesize */
int 	show_original_desc;		/* If set to 0, doesn't send original user's description in $MyINFO */
int 	show_class_in_desc;		/* If set to 0, adds [user's class name] to description */
int 	show_mode_in_desc;		/* If set to 1, adds [A], [P] or [PROXY] to user's description due to mode M:A(P,5) */
int 	show_original_mail;		/* If set to 0, doesn't send original user's email in $MyINFO */
int 	mute_unregistered;		/* Muting all unregistered users: 1-from chatting,2-from downloading/searching,3-from all */
int 	restrictions;			/* Restrictions for searching/downloading: 1-user can s/d from user with equal/lower class,2-unregistered cannot s/d from registered */
int   	punishment;          		/* 1 if user should be redirected, 0 if only kicked */
int    	kick_bantime;    		/* Number of minutes user should be banned for when kicked */
int 	tempban;			/* Number of minutes user should be banned for when kicked while TAG checking */
int 	max_warnings;			/* Maximum number of allowed warnings for user */
int 	no_chat_for_chatgagged;		/* If set to 1, chat-gagged users will not get main chat messages at all */
int   	ban_overrides_allow;         	/* 1 for ban to override allow */
int 	max_regular_from_one_ip;	/* Maximum allowed number of REGULAR users connected from the same IP */
size_t 	min_search_len;			/* Minimum lenght of search string */
int    	searchspam_time;		/* Minimum number of seconds between searches */
int 	deny_main_http;			/* If 1, filters messages from REGULAR for occurance of http://, www or IP string and reporting to ops if found */
int 	deny_pm_http;			/* If 1, kicks REGULAR for http:// pm string, if 2 bans them for the 5th pm message with http:// */
int  	ipgag_type;			/* Type of IP gag: 0-chat(default),1-download and search,2-all */
char	*banwords;			/* List of forbidden words of phrases delimited with ';' */ 
int 	spam_action;			/* On spam: 0-do nothing, 1-kick user, 2-ban user */
int 	flood_action;			/* On flood: 0-do nothing, 1-kick user, 2-ban user */
int 	flood_lines;			/* Number of lines when user is marked as a flooder */
int 	slotbeg_kicker;			/* If set to 1, REGULAR users will get kick for 'slot' word written to someone's priv */
int   	kick_for_incomplete;		/* If set to 1, will seek for incomplete files in active users' shared datas */
int 	seen_class;			/* Minimum class that will be stored on the seenlist */
size_t 	cut_string_to;		        /* Maximum number or characters that can be send to main chat by user */
int 	cut_reg_msg;			/* If set to 1, too long main chat messages from registered users will also be cutted */
int 	offline_msgs;			/* Minimum class allowed to use !msg command */
int     self_registration;		/* If set to 1, REGULAR user can register himself with !regme <password> command */ 
long long min_regme_share;		/* Minimum share for users who wanna register themselves with !regme <password> command */
int	protection;			/* If set to 1, self-temporary-protection is enabled (!protectme) */
unsigned int prottime;			/* Number of protection days for users who used !protectme command */
char 	botnick[51]; 			/* Bot's name */
char 	opchatnick[51];			/* Op chat's name */
int 	clockbot;			/* Clock-bot with current server time: 0-disabled, 1-enabled */
int 	count_bots;			/* Counting bots as users: 0-disabled, 1-enabled */
int	hide_opshare;			/* Hiding share of opped users 0-off,1-from all users,2-from REGULAR users */
int 	who_can_addrls;			/* Minimum class allowed to add new releases */
int 	who_can_seerls;			/* Minimum class allowed to see new releases */
int 	max_releases_to_show;		/* Number of last releases displayed in MOTD */
int 	who_can_addreq;			/* Minimum class allowed to add new requests */
int 	who_can_seereq;			/* Minimum class allowed to see new releases */
int 	max_requests_to_show;		/* Number of last requests displayed in MOTD */
unsigned int expiredtime;		/* Number of days after old entries form reglist file are deleted (0-never) */
unsigned int expiredrls;		/* Number of days after old entries from releaselist file are deleted (0-never) */
unsigned int expiredreq;		/* Number of days after old entries from reqlist file are deleted (0-never) */
unsigned int expiredseen;		/* Number of days after old entries from seen file are deleted */
unsigned int expiredurl;		/* Number of days after old entries from urlist file are deleted (0-never) */
unsigned int expiredwarn;		/* Number of days after old entries form warnings file are deleted (0-never) */
int 	autofav;			/* Period of sending autofavmsg to unregistered users: 0-off, else every value*15 minutes */
int 	send_scheduled;			/* Period of sending scheduledmsg to all users: 0-off, else every value*15 minutes */
int 	crypt_enable;			/* Enable encrypted passwords? 1 is on, 0 is off */
int 	log_main_chat;			/* Main chat logging, 1 is on, 0 is off */
int     verbosity;                   	/* This sets the verbosity of the log file, may vary from 0 to 5 */
int 	config_on_priv;			/* If set to 1, !config is displayed as a private message from botnick */
int 	help_on_priv;			/* If set to 1, !help is displayed as a private message from botnick */
int 	rules_on_priv;			/* If set to 1, !rules is displayed as a private message from botnick */
int 	regme_on_priv;			/* If set to 1, !regme is displayed as a private message from botnick */
int 	url_on_priv;			/* If set to 1, !geturl is displayed as a private message from botnick */
int 	rls_on_priv;			/* If set to 1, !getrls is displayed as a private message from botnick */
int 	req_on_priv;			/* If set to 1, !getreq is displayed as a private message from botnick */
int 	show_himsg;			/* Show Hi! welcome message with user's IP in MOTD: 0-off,1-on */
int 	show_onjoinmsgs;		/* Show info about basic hub commands in MOTD: 0-off,1-on */
int 	show_records;			/* Show hub records in MOTD and info if they're broken: 0-off,1-on */
int 	show_uptime;			/* Show hub uptime in MOTD: 0-off,1-on */
int 	show_lastmsgs;			/* Show 10 last main chat messages in MOTD: 0-off,1-on */
int 	show_releases;			/* Show last added releases in MOTD: 0-off,1-on */
int 	show_requests;			/* Show last added request in MOTD: 0-off,1-on */
int 	show_welcomes;			/* Show welcomes of REG and higher classes to main chat: 0-off,1-on,2-on with random welcomes */
int 	show_joinsparts;		/* Showing joins/parts of users: 0-off,1-main chat for admins/owners,2-pm to admins/owners,3-main chat for all */
int 	show_kicksbans;			/* Showing hub kicks/bans of users: 0-off,1-main chat for admins/owners,2-pm to admins/owners,3-main chat for all */
int 	show_denied;			/* Showing info about connection attempt of banned user: 0-off,1-main chat for admins/owners,2-pm to admins/owners,3-main chat for all */
int 	show_actions;			/* Notify ops/cheefs/admins/owners on some actions: 0-off, 1-main chat,2-pm */
int 	show_notifications;		/* Notify admins/owners on some events: 0-off, 1-main chat,2-pm */
int 	show_helpcommand_usage;		/* Notify admins/owners on !help/!rules!regme usage: 0-off, 1-on */
int 	extra_commands;			/* If set to 1, no strings started with ! and + will be displayed in the main chat */
char 	extra_files[501];		/* Pointer containing names of command-files created by user */
char	redirect_host_kickban[MAX_HOST_LEN+1]; /* Redirect host on kick/ban */ 
char    redirect_host_minshare[MAX_HOST_LEN+1]; /* Redirect host on not enough share */
char    redirect_host_maxshare[MAX_HOST_LEN+1]; /* Redirect host on too large share */
char    redirect_host_maxhubs[MAX_HOST_LEN+1]; /* Redirect host on too many opened hubs */
char    redirect_host_minslots[MAX_HOST_LEN+1]; /* Redirect host on not enough slots */
char    redirect_host_maxslots[MAX_HOST_LEN+1]; /* Redirect host on too many slots */
char    redirect_host_ratio[MAX_HOST_LEN+1]; /* Redirect host on too low/high ratio */
char    redirect_host_client[MAX_HOST_LEN+1]; /* Redirect host on forbidden client */
char    redirect_host_connlimit[MAX_HOST_LEN+1]; /* Redirect host on upload limit */
char    redirect_host_hubfull[MAX_HOST_LEN+1]; /* Redirect host when hub is full */
char    redirect_host_regonly[MAX_HOST_LEN+1]; /* Redirect host when hub is registered only */
char    banmsg[MAX_MSG_LEN+1];		/* Ban message */
char    *hub_full_msg;			/* The message displayed if hub is full */
char    regonlymsg[MAX_MSG_LEN+1];   	/* RegOnly message */
char   	topic[MAX_TOPIC_LEN+1];      	/* Actual hub topic */
char 	welcomereg[MAX_MSG_LEN+1];	/* Welcome message for REG class users */
char 	welcomevip[MAX_MSG_LEN+1];	/* Welcome message for VIP class users */
char 	welcomekvip[MAX_MSG_LEN+1];	/* Welcome message for KVIP class users */
char 	welcomeop[MAX_MSG_LEN+1];	/* Welcome message for OP class users */
char 	welcomecheef[MAX_MSG_LEN+1];	/* Welcome message for CHEEF class users */
char 	welcomeadmin[MAX_MSG_LEN+1];	/* Welcome message for ADMIN/OWNER class users */
char 	partmsg[MAX_MSG_LEN+1];		/* Information about registered user's quit send to main chat */
//DB// !block/!unblock variables
int 	block_main;        	       	/*  !block/!unblock main */
int 	block_pm;         		/*  !block/!unblock private messaging */
int 	block_download;   		/*  !block/!unblock downloading */
int 	block_search;      		/*  !block/!unblock searching */

time_t time_to_unban;
time_t btime_to_unban;
char seenhost[MAX_HOST_LEN+1];
pid_t  pid;                         /* Pid of process if parent, if it's a child, pid is 0, for scripts, it's -1 and for hublist upload processes it's -2  */
unsigned int scripts[10];
struct user_t *non_human_user_list; /* List of non-human users */
struct user_t **human_hash_table;  /* Hashtable of human users */
struct sock_t *human_sock_list;
char   set_topic[301];/* Message send to user when the topic was changed */
char  randomwelcome[251];
int actch;
char topicstring[251];
int botcheck;
int allreleases;
int allrequests;
int thesamebuf;
int warnings;
int oldbuf;
int   debug;                       /* 1 for debug mode, else 0 */
int h_interval;
int newmyinfo;
int 	pmspam;
int 	redall;
int 	prvset;			 
char pmspammer[51];
char hammernick[101];
time_t hammertime;
char botinfolater[51];
int getinfolater;
char  *rlsmotd;
char  *reqmotd;
char  *banreason;
char  *ipgagreason;
char  longdate[201];
char  s_time[120];
char  ss_time[120];
char away_reason[501];
char shdatetime[101];
char usdatetime[101];
char category[51];
int    max_sockets;
unsigned int total_users;
int total_users_scr;
int total_scripts;
unsigned long long total_share;
unsigned long long total_share_scr;
int isupt;
int isupt2;
int   upload;                      /* keeps track on when it's time to upload to public hub list */
sig_atomic_t quit;
sig_atomic_t alarmflag;
int   do_purge_user_list;
int   script_reload;
char   banword[151];
char   config_dir[MAX_FDP_LEN+1];
char   un_sock_path[MAX_FDP_LEN+1];
char   logfile[MAX_FDP_LEN+1];	/* Logfile if specifically set */
uid_t  dchub_user;
gid_t  dchub_group;
char   working_dir[MAX_FDP_LEN+1];
time_t hub_start_time;
int    current_forked;   
char nomyinfo[51];
int protdays;

//DB// Records counting stuff
int 	recordusers; 		/* Users record */
long long recordshare; 		/* Share record */
int 	brokenur; 		/* 1 means users record was broken */
int 	brokensr;		/* 1 means share record was broken */
char  	recordshstr[120];
char  	recordusstr[120];

char 	clocknick[50];

//DB// Pointers that count various things and display them in !ui command to admins/owners
long long blocked_pm;
long long accepted_pm;
long long blocked_mc;
long long accepted_mc;
long long blocked_searches;
long long accepted_downloads;
long long blocked_downloads;
long long accepted_searches;
long long totalvisitors;
long long commandsnmb;
long long tthsearches;
long long ip_banned;
long long nick_banned;

//DB// Last 20 main chat messages
char   message1[401];
char   message2[401];
char   message3[401];
char   message4[401];
char   message5[401];
char   message6[401];
char   message7[401];
char   message8[401];
char   message9[401];
char   message10[401];
char   message11[401];
char   message12[401];
char   message13[401];
char   message14[401];
char   message15[401];
char   message16[401];
char   message17[401];
char   message18[401];
char   message19[401];
char   message20[401];

//DB// Pointers that contains the files loaded to memory
char *reglist_mem;
char *hide_mem;
char *regme_mem;
char *rules_mem;
char *autofav_mem;
char *schedule_mem;
char *away_mem;
char *autoaway_mem;
char *banlist_mem;
char *allowlist_mem;
char *ipgaglist_mem;
char *warn_mem;
char *nickbanlist_mem;
char *banrs_mem;
char *nickbanrs_mem;
char *off_mem;
char *url_mem;

//DB// Info about last temporary bans
char   lasttemp1[296];
char   lasttemp2[296];
char   lasttemp3[296];
char   lasttemp4[296];
char   lasttemp5[296];
char   lasttemp6[296];
char   lasttemp7[296];
char   lasttemp8[296];
char   lasttemp9[296];
char   lasttemp10[296];
char   lasttemp11[296];
char   lasttemp12[296];
char   lasttemp13[296];
char   lasttemp14[296];
char   lasttemp15[296];
char   lasttemp16[296];
char   lasttemp17[296];
char   lasttemp18[296];
char   lasttemp19[296];
char   lasttemp20[296];

//DB// Info about last permanent bans
char   lastperm1[296];
char   lastperm2[296];
char   lastperm3[296];
char   lastperm4[296];
char   lastperm5[296];
char   lastperm6[296];
char   lastperm7[296];
char   lastperm8[296];
char   lastperm9[296];
char   lastperm10[296];
char   lastperm11[296];
char   lastperm12[296];
char   lastperm13[296];
char   lastperm14[296];
char   lastperm15[296];
char   lastperm16[296];
char   lastperm17[296];
char   lastperm18[296];
char   lastperm19[296];
char   lastperm20[296];
char   lastbuf[801];

//DB// lang.c variables for DB Hub 0.4



/* Functions */
int    new_human_user(int sock);
int    socket_action(struct user_t *user);
void   remove_user(struct user_t *our_user, int send_quit, int remove_from_list);
void   send_init(int sock);
void   do_upload_to_hublist();
int    handle_command(char *buf, struct user_t *user);
void   hub_mess(struct user_t *user, int mess_type);
int    send_user_info(struct user_t *from_user, char *to_user_nick);
void   init_sig(void);
void   remove_all(int type, int send_quit, int remove_from_list);
void   new_forked_process(void);
void   kill_forked_process(void);
void   term_signal(int z);
void   alarm_signal(int z);
void   alarm_handler(void);
int    set_default_vars(void);
void   new_admin_connection();
void   add_non_human_to_list(struct user_t *user);
void   remove_non_human(struct user_t *our_user);
void   add_human_to_hash(struct user_t *user);
void   remove_human_from_hash(char *nick);
struct user_t* get_human_user(char *nick);
void   remove_human_user(struct user_t *user);
void   encrypt_pass(char* password);
void   clockset(void);
void	daemonize(void);

#endif /* _MAIN_H_ */
