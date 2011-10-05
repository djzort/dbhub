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

#ifdef HAVE_PERL

#include <stdlib.h>
#include <stdio.h>
#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#include <sys/shm.h>

#include "xs_functions.h"
#include "main.h"
#include "network.h"
#include "fileio.h"
#include "utils.h"
#include "userlist.h"
#include "logger.h"
#include "semops.h"

#define EXTERN_C extern

EXTERN_C void boot_DynaLoader (CV* cv);
static char *user_list = NULL;

XS(xs_get_ip)
{
	struct user_t *user;
	char ip[21];
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	if((user = get_human_user(SvPVX(ST(0)))) == NULL)
		XSRETURN_UNDEF;

	snprintf(ip, 20, "%s", user->hostname);

	XSRETURN_PV(ip);
}

XS(xs_get_share)
{
	struct user_t *user;
	long long share;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	if((user = get_human_user(SvPVX(ST(0)))) == NULL)
		XSRETURN_UNDEF;

	share = user->share;

	XSRETURN_NV(share);
}

XS(xs_data_to_user)
{
	struct user_t *user;
	char *data;
	dXSARGS;

	if(items != 2)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	if(!SvPOK(ST(1)))
		XSRETURN_UNDEF;

	if((user = get_human_user(SvPVX(ST(0)))) == NULL)
		XSRETURN_UNDEF;

	data = SvPVX(ST(1));

	/* The ScriptToUser command is handled by the parent process. Eventually, 
	* only the data part is sent to the user.  */
	uprintf(non_human_user_list, "$ScriptToUser %s ", user->nick);

	send_to_user(data, non_human_user_list);

	if(data[strlen(data) - 1] != '|')
		send_to_user("|", non_human_user_list);
}

XS(xs_gag_user)
{   
	char *nick;
	int type;
	dXSARGS;
	if(items != 2)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;
	
	if(!SvIOK(ST(1)))
	XSRETURN_UNDEF;

	nick = SvPVX(ST(0));
	type = SvIVX(ST(1));

	send_to_user("$Gag ", non_human_user_list);
	send_to_user(nick, non_human_user_list);
	uprintf(non_human_user_list, " %d|", type);
}

XS(xs_ungag_user)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$Gag ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user(" 0|", non_human_user_list);
}


XS(xs_kick_user)
{
	struct user_t *user;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	if((user = get_human_user(SvPVX(ST(0)))) == NULL)
		XSRETURN_UNDEF;

	uprintf(non_human_user_list, "$ScriptToUser %s $ForceMove %s:%d|", 
		user->nick, hub_hostname, listening_port);
}

XS(xs_force_move_user)
{
	struct user_t *user;
	char *host;
	dXSARGS;

	if(items != 2)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	if(!SvPOK(ST(1)))
		XSRETURN_UNDEF;

	if((user = get_human_user(SvPVX(ST(0)))) == NULL)
		XSRETURN_UNDEF;

	host = SvPVX(ST(1));

	/* Using the ScriptToUser command to send the command to the user.  */
	uprintf(non_human_user_list, "$ScriptToUser %s $ForceMove %s|", 
		user->nick, host);

}

XS(xs_get_variable)
{
	char *var_name;
//	long long share;
//	long long total;
	double uptime;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	var_name = SvPVX(ST(0));

	if(!strncmp(var_name, "allowed_clients", 15))
		XSRETURN_PV(allowed_clients);
	else if(!strncmp(var_name, "autofav", 7))
		XSRETURN_IV(autofav);
	else if(!strncmp(var_name, "ban_overrides_allow", 19))
		XSRETURN_IV(ban_overrides_allow);
	else if(!strncmp(var_name, "banmsg", 6))
		XSRETURN_PV(banmsg);
	else if(!strncmp(var_name, "banwords", 8))
		XSRETURN_PV(banwords);
	else if(!strncmp(var_name, "botnick", 7))
		XSRETURN_PV(botnick);		
	else if(!strncmp(var_name, "check_class", 11))
		XSRETURN_IV(check_class);
	else if(!strncmp(var_name, "clockbot", 8))
		XSRETURN_IV(clockbot);
	else if(!strncmp(var_name, "config_on_priv", 14))
		XSRETURN_IV(config_on_priv);
	else if(!strncmp(var_name, "connlimit", 9))
		XSRETURN_IV(connlimit);
	else if(!strncmp(var_name, "count_bots", 10))
		XSRETURN_IV(count_bots);
	else if(!strncmp(var_name, "count_reghubs", 13))
		XSRETURN_IV(count_reghubs);
	else if(!strncmp(var_name, "crypt_enable", 12))
		XSRETURN_IV(crypt_enable);
	else if(!strncmp(var_name, "cut_reg_msg", 11))
		XSRETURN_IV(cut_reg_msg);
	else if(!strncmp(var_name, "cut_string_to", 12))
		XSRETURN_IV(cut_string_to);
	else if(!strncmp(var_name, "default_pass", 12))
		XSRETURN_PV(default_pass);
	else if(!strncmp(var_name, "default_prefixes", 16))
		XSRETURN_PV(default_prefixes);
	else if(!strncmp(var_name, "deny_main_http", 14))
		XSRETURN_IV(deny_main_http);
	else if(!strncmp(var_name, "deny_passive", 12))
		XSRETURN_IV(deny_passive);
	else if(!strncmp(var_name, "deny_pm_http", 12))
		XSRETURN_IV(deny_pm_http);
	else if(!strncmp(var_name, "deny_proxy", 10))
		XSRETURN_IV(deny_proxy);
	else if(!strncmp(var_name, "expiredreq", 10))
		XSRETURN_IV(expiredreq);
	else if(!strncmp(var_name, "expiredrls", 10))
		XSRETURN_IV(expiredrls);
	else if(!strncmp(var_name, "expiredseen", 11))
		XSRETURN_IV(expiredseen);
	else if(!strncmp(var_name, "expiredtime", 11))
		XSRETURN_IV(expiredtime);
	else if(!strncmp(var_name, "expiredurl", 10))
		XSRETURN_IV(expiredurl);
	else if(!strncmp(var_name, "extra_commands", 14))
		XSRETURN_IV(extra_commands);	
	else if(!strncmp(var_name, "extra_files", 11))
		XSRETURN_PV(extra_files);	
	else if(!strncmp(var_name, "flood_action", 12))
		XSRETURN_IV(flood_action);
	else if(!strncmp(var_name, "flood_lines", 11))
		XSRETURN_IV(flood_lines);
	else if(!strncmp(var_name, "forbchars", 9))
		XSRETURN_PV(forbchars);
	else if(!strncmp(var_name, "gag_on_minshare", 15))
		XSRETURN_IV(gag_on_minshare);
	else if(!strncmp(var_name, "help_on_priv", 12))
		XSRETURN_IV(help_on_priv);
	else if(!strncmp(var_name, "hide_allshare", 13))
		XSRETURN_IV(hide_allshare);
	else if(!strncmp(var_name, "hide_opshare", 12))
		XSRETURN_IV(hide_opshare);
	else if(!strncmp(var_name, "hidetags", 8))
		XSRETURN_IV(hidetags);
	else if(!strncmp(var_name, "hub_description", 15))
		XSRETURN_PV(hub_description);
	else if(!strncmp(var_name, "hub_full_msg", 12))
		XSRETURN_PV(hub_full_msg);
	else if(!strncmp(var_name, "hub_hostname", 12))
		XSRETURN_PV(hub_hostname);
	else if(!strncmp(var_name, "hub_name", 8))
		XSRETURN_PV(hub_name);
	else if(!strncmp(var_name, "ipgag_type", 10))
		XSRETURN_IV(ipgag_type);
	else if(!strncmp(var_name, "kick_bantime", 12))
		XSRETURN_IV(kick_bantime);
	else if(!strncmp(var_name, "kick_for_incomplete", 19))
		XSRETURN_IV(kick_for_incomplete);
	else if(!strncmp(var_name, "listening_port", 14))
		XSRETURN_IV(listening_port);
	else if(!strncmp(var_name, "log_main_chat", 13))
		XSRETURN_IV(log_main_chat);
	else if(!strncmp(var_name, "max_regular_from_one_ip", 23))
		XSRETURN_IV(max_regular_from_one_ip);
	else if(!strncmp(var_name, "max_releases_to_show", 20))
		XSRETURN_IV(max_releases_to_show);
	else if(!strncmp(var_name, "max_requests_to_show", 20))
		XSRETURN_IV(max_requests_to_show);
    	else if(!strncmp(var_name, "max_share", 9))
		XSRETURN_IV(max_share);
	else if(!strncmp(var_name, "max_users", 9))
		XSRETURN_IV(max_users);
	else if(!strncmp(var_name, "max_warnings", 12))
		XSRETURN_IV(max_warnings);
	else if(!strncmp(var_name, "maxhubs", 7))
		XSRETURN_IV(maxhubs);
	else if(!strncmp(var_name, "maxslots", 8))
		XSRETURN_IV(maxslots);
	else if(!strncmp(var_name, "minslots", 8))
		XSRETURN_IV(minslots);
	else if(!strncmp(var_name, "min_command_class", 17))
		XSRETURN_IV(min_command_class);
	else if(!strncmp(var_name, "min_share", 9))
		XSRETURN_NV(min_share);
	else if(!strncmp(var_name, "min_psv_share", 13))
		XSRETURN_NV(min_psv_share);
	else if(!strncmp(var_name, "min_reg_share", 13))
		XSRETURN_NV(min_reg_share);
	else if(!strncmp(var_name, "min_regme_share", 15))
		XSRETURN_NV(min_regme_share);
	else if(!strncmp(var_name, "min_vip_share", 13))
		XSRETURN_NV(min_vip_share);
	else if(!strncmp(var_name, "min_kvip_share", 14))
		XSRETURN_NV(min_kvip_share);
	else if(!strncmp(var_name, "min_op_share", 12))
		XSRETURN_NV(min_op_share);
	else if(!strncmp(var_name, "min_nick_len", 12))
		XSRETURN_IV(min_nick_len);
	else if(!strncmp(var_name, "min_search_len", 14))
		XSRETURN_IV(min_search_len);
	else if(!strncmp(var_name, "min_usercommand_class", 21))
		XSRETURN_IV(min_usercommand_class);
	else if(!strncmp(var_name, "mindcppver", 10))
		XSRETURN_PV(mindcppver);
	else if(!strncmp(var_name, "mute_unregistered", 17))
		XSRETURN_IV(mute_unregistered);
	else if(!strncmp(var_name, "nickchars", 9))
		XSRETURN_PV(nickchars);
	else if(!strncmp(var_name, "no_chat_for_chatgagged", 22))
		XSRETURN_IV(no_chat_for_chatgagged);
	else if(!strncmp(var_name, "opchatnick", 10))
		XSRETURN_PV(opchatnick);
	else if(!strncmp(var_name, "partmsg", 7))
		XSRETURN_PV(partmsg);
	else if(!strncmp(var_name, "protection", 10))
		XSRETURN_IV(protection);
	else if(!strncmp(var_name, "protectmeprefix", 15))
		XSRETURN_PV(protectmeprefix);
	else if(!strncmp(var_name, "prottime", 8))
		XSRETURN_IV(prottime);
	else if(!strncmp(var_name, "punishment", 10))
		XSRETURN_IV(punishment);
	else if(!strncmp(var_name, "ratio", 5))
		XSRETURN_IV(ratio);
	else if(!strncmp(var_name, "reverse_ratio", 13))
		XSRETURN_IV(reverse_ratio);
	else if(!strncmp(var_name, "redirect_host_kickban", 21))
		XSRETURN_PV(redirect_host_kickban);
	else if(!strncmp(var_name, "redirect_host_minshare", 22))
		XSRETURN_PV(redirect_host_minshare);
	else if(!strncmp(var_name, "redirect_host_maxshare", 22))
		XSRETURN_PV(redirect_host_maxshare);
	else if(!strncmp(var_name, "redirect_host_maxhubs", 21))
		XSRETURN_PV(redirect_host_maxhubs);
	else if(!strncmp(var_name, "redirect_host_minslots", 22))
		XSRETURN_PV(redirect_host_minslots);
	else if(!strncmp(var_name, "redirect_host_maxslots", 22))
		XSRETURN_PV(redirect_host_maxslots);
	else if(!strncmp(var_name, "redirect_host_ratio", 19))
		XSRETURN_PV(redirect_host_ratio);
	else if(!strncmp(var_name, "redirect_host_client", 20))
		XSRETURN_PV(redirect_host_client);
	else if(!strncmp(var_name, "redirect_host_connlimit", 23))
		XSRETURN_PV(redirect_host_connlimit);
	else if(!strncmp(var_name, "redirect_host_hubfull", 21))
		XSRETURN_PV(redirect_host_hubfull);
	else if(!strncmp(var_name, "redirect_host_regonly", 21))
		XSRETURN_PV(redirect_host_regonly);
	else if(!strncmp(var_name, "registered_only", 15))
		XSRETURN_IV(registered_only);
	else if(!strncmp(var_name, "regme_on_priv", 13))
		XSRETURN_IV(regme_on_priv);
	else if(!strncmp(var_name, "regmeprefix", 11))
		XSRETURN_PV(regmeprefix);
	else if(!strncmp(var_name, "regonlymsg", 11))
		XSRETURN_PV(regonlymsg);
	else if(!strncmp(var_name, "req_on_priv", 11))
		XSRETURN_IV(req_on_priv);
	else if(!strncmp(var_name, "restrict_mode", 13))
		XSRETURN_IV(restrict_mode);
	else if(!strncmp(var_name, "restrictions", 12))
		XSRETURN_IV(restrictions);
	else if(!strncmp(var_name, "req_on_priv", 11))
		XSRETURN_IV(req_on_priv);
	else if(!strncmp(var_name, "rls_on_priv", 11))
		XSRETURN_IV(rls_on_priv);
	else if(!strncmp(var_name, "rules_on_priv", 13))
		XSRETURN_IV(rules_on_priv);
	else if(!strncmp(var_name, "searchspam_time", 15))
		XSRETURN_IV(searchspam_time);
	else if(!strncmp(var_name, "self_registration", 17))
		XSRETURN_IV(self_registration);
	else if(!strncmp(var_name, "seen_class", 10))
		XSRETURN_IV(seen_class);
	else if(!strncmp(var_name, "send_scheduled", 14))
		XSRETURN_IV(send_scheduled);
	else if(!strncmp(var_name, "show_actions", 12))
		XSRETURN_IV(show_actions);
	else if(!strncmp(var_name, "show_denied", 11))
		XSRETURN_IV(show_denied);
	else if(!strncmp(var_name, "show_helpcommand_usage", 22))
		XSRETURN_IV(show_helpcommand_usage);
	else if(!strncmp(var_name, "show_himsg", 10))
		XSRETURN_IV(show_himsg);
	else if(!strncmp(var_name, "show_joinsparts", 15))
		XSRETURN_IV(show_joinsparts);
	else if(!strncmp(var_name, "show_kicksbans", 14))
		XSRETURN_IV(show_kicksbans);
	else if(!strncmp(var_name, "show_lastmsgs", 13))
		XSRETURN_IV(show_lastmsgs);
	else if(!strncmp(var_name, "show_class_in_desc", 18))
		XSRETURN_IV(show_class_in_desc);
	else if(!strncmp(var_name, "show_mode_in_desc", 17))
		XSRETURN_IV(show_mode_in_desc);
	else if(!strncmp(var_name, "show_notifications", 18))
		XSRETURN_IV(show_notifications);
	else if(!strncmp(var_name, "show_onjoinmsgs", 15))
		XSRETURN_IV(show_onjoinmsgs);
	else if(!strncmp(var_name, "show_original_desc", 18))
		XSRETURN_IV(show_original_desc);
	else if(!strncmp(var_name, "show_original_mail", 18))
		XSRETURN_IV(show_original_mail);
	else if(!strncmp(var_name, "show_records", 12))
		XSRETURN_IV(show_records);
	else if(!strncmp(var_name, "show_releases", 13))
		XSRETURN_IV(show_releases);
	else if(!strncmp(var_name, "show_requests", 13))
		XSRETURN_IV(show_requests);
	else if(!strncmp(var_name, "show_uptime", 11))
		XSRETURN_IV(show_uptime);
	else if(!strncmp(var_name, "show_welcomes", 13))
		XSRETURN_IV(show_welcomes);
	else if(!strncmp(var_name, "slotbeg_kicker", 14))
		XSRETURN_IV(slotbeg_kicker);
	else if(!strncmp(var_name, "spam_action", 11))
		XSRETURN_IV(spam_action);
	else if(!strncmp(var_name, "tempban", 7))
		XSRETURN_IV(tempban);
	else if(!strncmp(var_name, "topic", 5))
		XSRETURN_PV(topic);
	else if(!strncmp(var_name, "url_on_priv", 11))
		XSRETURN_IV(url_on_priv);
	else if(!strncmp(var_name, "verbosity", 9))
		XSRETURN_IV(verbosity);
	else if(!strncmp(var_name, "welcomereg", 10))
		XSRETURN_PV(welcomereg);
	else if(!strncmp(var_name, "welcomevip", 10))
		XSRETURN_PV(welcomevip);
	else if(!strncmp(var_name, "welcomekvip", 11))
		XSRETURN_PV(welcomekvip);
	else if(!strncmp(var_name, "welcomeop", 9))
		XSRETURN_PV(welcomeop);
	else if(!strncmp(var_name, "welcomeadmin", 12))
		XSRETURN_PV(welcomeadmin);
	else if(!strncmp(var_name, "welcomecheef", 12))
		XSRETURN_PV(welcomecheef);
	else if(!strncmp(var_name, "who_can_addreq", 14))
		XSRETURN_IV(who_can_addreq);
	else if(!strncmp(var_name, "who_can_addrls", 14))
		XSRETURN_IV(who_can_addrls);
	else if(!strncmp(var_name, "who_can_seereq", 14))
		XSRETURN_IV(who_can_seereq);
	else if(!strncmp(var_name, "who_can_seerls", 14))
		XSRETURN_IV(who_can_seerls);
	else if(!strncmp(var_name, "working_dir", 11))
		XSRETURN_PV(working_dir);
	else if(!strncmp(var_name, "dbver", 5))
		XSRETURN_PV(VERSION);
	else if(!strncmp(var_name, "hub_uptime", 10))
	{
		uptime = get_uptime();
		XSRETURN_NV(uptime);
	}
	else if(!strncmp(var_name, "total_share", 11))
		XSRETURN_NV(total_share_scr);
}

XS(xs_set_variable)
{   
	char *var_name;
	char *value;
	dXSARGS;

	if(items != 2)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	if(!SvPOK(ST(1)))
		XSRETURN_UNDEF;

	var_name = SvPVX(ST(0));
	value = SvPVX(ST(1));

	send_to_user("$Set ", non_human_user_list);
	send_to_user(var_name, non_human_user_list);
	send_to_user(" ", non_human_user_list);
	send_to_user(value, non_human_user_list);
	send_to_user("|", non_human_user_list);
}

XS(xs_add_ban_entry)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$Ban ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user("|", non_human_user_list);
	uprintf(non_human_user_list, "$ReloadFile banlist|");
}

XS(xs_add_nickban_entry)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$NickBan ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user("|", non_human_user_list);
	uprintf(non_human_user_list, "$ReloadFile banlist|");
}

XS(xs_add_allow_entry)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$Allow ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user("|", non_human_user_list);
	uprintf(non_human_user_list, "$ReloadFile allowlist|");
}

XS(xs_remove_ban_entry)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$Unban ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user("|", non_human_user_list);
	uprintf(non_human_user_list, "$ReloadFile banlist|");
}

XS(xs_remove_allow_entry)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$Unallow ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user("|", non_human_user_list);
	uprintf(non_human_user_list, "$ReloadFile allowlist|");
}

XS(xs_remove_nickban_entry)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$UnNickBan ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user("|", non_human_user_list);
	uprintf(non_human_user_list, "$ReloadFile nickbanlist|");
}

XS(xs_add_reg_user)
{   
	char *nick;
	char *password;
//	char addregstr[151];
	int type;
	dXSARGS;

	if(items != 3)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	if(!SvPOK(ST(1)))
		XSRETURN_UNDEF;

	if(!SvIOK(ST(2)))
		XSRETURN_UNDEF;

	nick = SvPVX(ST(0));
	password = SvPVX(ST(1));
	type = SvIVX(ST(2));

	send_to_user("$AddRegUser ", non_human_user_list);
	send_to_user(nick, non_human_user_list);
	send_to_user(" ", non_human_user_list);
	send_to_user(password, non_human_user_list);
	uprintf(non_human_user_list, " %d|", type);
	uprintf(non_human_user_list, "$ReloadFile reglist|");
}

XS(xs_remove_reg_user)
{   
	char *entry;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	entry = SvPVX(ST(0));

	send_to_user("$RemoveRegUser ", non_human_user_list);
	send_to_user(entry, non_human_user_list);
	send_to_user("|", non_human_user_list);
	uprintf(non_human_user_list, "$ReloadFile reglist|");
}

XS(xs_data_to_all)
{   
	char *data;
	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	data = SvPVX(ST(0));

	send_to_user("$DataToAll ", non_human_user_list);
	
	send_to_user(data, non_human_user_list);
}

XS(xs_register_script_name)
{
	char *nick;
	char c;
	char randpass[11];
	char regstring[151];
	int i;

	dXSARGS;

	if(items != 1)
		XSRETURN_UNDEF;

	if(!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	nick = SvPVX(ST(0));

	/* Check that it's a proper nick.  */
	if((nick[0] == '\0') || (nick[0] == '\r') || (nick[0] == '\n') 
		|| (strlen(nick) > MAX_NICK_LEN))
		XSRETURN_UNDEF;

	/* Register the nick in the nicklist.  */
	if(check_if_registered(nick) == 0)
	{
		/* Generate a random password.  */
		srand(time(NULL));
		for(i = 0; i <= 9; i++)
		{
			c = 'A' + rand()%('z' - 'a');
			randpass[i] = c;
		}
		randpass[10] = '\0';
		snprintf(regstring, 150, "$AddRegUser %s %s 7|", nick, randpass);
		add_reg_user(regstring);
	}
	//DB// if script nickname is already used by botnick or opchatnick we have to skip $ValidateNick
	if((strcasecmp(nick, botnick) != 0) && (strcasecmp(nick, opchatnick) != 0))
	{
	send_to_user("$ValidateNick ", non_human_user_list);
	send_to_user(nick, non_human_user_list);
	send_to_user("|", non_human_user_list);
	}
}

XS(xs_check_if_registered)
{   
	int isregged = 0;
        int z = 0;
	int i, j;
        int fd;
        int erret;
        char *user_nick;
        char nick[51];
	FILE *fp;
        char path[MAX_FDP_LEN+1];
        char line[1024];
	char ciruser[51];
	char cirpass[51];
	dXSARGS;
	if(items != 1)
		XSRETURN_UNDEF;
	if (!SvPOK(ST(0)))
		XSRETURN_UNDEF;

	user_nick = SvPVX(ST(0));
	snprintf(nick, 50, "%s", user_nick);
	    
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, REG_FILE);
   
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(LOG_EMERG, "Error - In xs_check_if_registered()/open(): Interrupted system call. Trying again.\n");   
   
   if(fd < 0)
     {
	logprintf(LOG_EMERG, "Error - In xs_check_if_registered()/open(): %s",strerror(errno));
        XSRETURN_UNDEF;
     }
   
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
	logprintf(LOG_EMERG, "Error - In xs_check_if_registered(): Couldn't set lock\n");
	close(fd);
        XSRETURN_UNDEF;
     }
   
   
   if((fp = fdopen(fd, "r")) == NULL)
     {
	logprintf(LOG_EMERG, "Error - In xs_check_if_registered()/fdopen(): %s",strerror(errno));
	set_lock(fd, F_UNLCK);
	close(fd);
        XSRETURN_UNDEF;
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

	     if(sscanf(line + i, "%50s %50s %i", ciruser, cirpass, &z) == 3)	     
	     {
	    	  if(strcasecmp(nick, ciruser) == 0)
	          {
		  set_lock(fd, F_UNLCK);
		  
		  while(((erret = fclose(fp)) != 0) && (errno == EINTR))
		    logprintf(LOG_EMERG, "Error - In check_if_registered()/fclose(): Interrupted system call. Trying again.\n");
		  
		  if(erret != 0)
		    {
		       logprintf(LOG_EMERG, "Error - In check_if_registered()/fclose(): %s",strerror(errno));
		       XSRETURN_UNDEF;
		      }
		   if(z == 7)
		   isregged = 8;
		   else if(z == 6)
		   isregged = 7;
		   else if(z == 5)
		   isregged = 6;
		   else if(z == 4)
		   isregged = 5;
		   else if(z == 3)
		   isregged = 4;
		   else if(z == 2)
		   isregged = 3;
		   else if(z == 1)
		   isregged = 2;
		   else if(z == 0)
		   isregged = 1;
		   else
		   isregged = 0;
	XSRETURN_IV(isregged);	       
	       }
	     }
	  }
     }
   set_lock(fd, F_UNLCK);
   
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(LOG_NOTICE, "Error - In xs_check_if_registered()/fclose(): Interrupted system call. Trying again.");
   
   if(erret != 0)
     {
	logprintf(LOG_EMERG, "Error - In xs_check_if_registered()/fclose(): %s",strerror(errno));
        XSRETURN_UNDEF;
     }
   
	XSRETURN_IV(isregged);
}

XS(xs_get_user_list)
{
	char *buf, *bufp;
	char temp_nick[MAX_NICK_LEN+1];
	char temp_host[MAX_HOST_LEN+1];
	int spaces=0, entries=0;
	int i;  

	dXSARGS;

	if(items != 0)
		XSRETURN_UNDEF;

	if(user_list != NULL)
		free(user_list);

	if((user_list = malloc(sizeof(char) * 2)) == NULL)
	{	
		logprintf(LOG_EMERG, "Error - In get_user_list()/malloc(): %s",strerror(errno));
		quit = 1;
		XSRETURN_UNDEF;
	}

	*user_list = '\0';

	sem_take(user_list_sem);

	/* Attach to the shared segment */
	if((buf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
		== (char *)-1)
	{	
		logprintf(LOG_EMERG, "Error - In get_user_list()/shmat(): %s",strerror(errno));
		sem_give(user_list_sem);
		quit = 1;
		XSRETURN_UNDEF;
	}

	if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
	{	
		logprintf(LOG_EMERG, "Error - In get_op_list(): Couldn't get number of entries\n");
		shmdt(buf);
		sem_give(user_list_sem);
		quit = 1;
		XSRETURN_UNDEF;
	}

	bufp = buf + 30;

	for(i = 1; i <= spaces; i++)
	{       
		if(*bufp != '\0')
		{	     
			sscanf(bufp, "%50s %120s", temp_nick, temp_host);
			if((user_list = realloc(user_list, sizeof(char)
				* (strlen(user_list) + strlen(temp_nick) + 2))) == NULL)
			{		       
				logprintf(LOG_EMERG, "Error - In get_user_list()/realloc(): %s",strerror(errno));
				shmdt(buf);
				sem_give(user_list_sem);
				quit = 1;
				XSRETURN_UNDEF;
			}		  
			sprintfa(user_list, "%s ", temp_nick);	       	     
		}	
		bufp += USER_LIST_ENT_SIZE;
	}
			if((user_list = realloc(user_list, sizeof(char)
				* (strlen(user_list) + strlen(botnick) + 2))) == NULL)
			{		       
				logprintf(LOG_EMERG, "Error - In get_user_list()/realloc(): %s",strerror(errno));
				shmdt(buf);
				sem_give(user_list_sem);
				quit = 1;
				XSRETURN_UNDEF;
			}		  
		    	sprintfa(user_list, "%s ", botnick);

			if((user_list = realloc(user_list, sizeof(char)
				* (strlen(user_list) + strlen(opchatnick) + 2))) == NULL)
			{		       
				logprintf(LOG_EMERG, "Error - In get_user_list()/realloc(): %s", strerror(errno));
				shmdt(buf);
				sem_give(user_list_sem);
				quit = 1;
				XSRETURN_UNDEF;
			}		  
		    	sprintfa(user_list, "%s ", opchatnick);

	if(clockbot == 1)
	{
			if((user_list = realloc(user_list, sizeof(char)
				* (strlen(user_list) + strlen(clocknick) + 2))) == NULL)
			{		       
				logprintf(LOG_EMERG, "Error - In get_user_list()/realloc(): %s",strerror(errno));
				shmdt(buf);
				sem_give(user_list_sem);
				quit = 1;
				XSRETURN_UNDEF;
			}		  
		    	sprintfa(user_list, "%s ", clocknick);
	}	

	shmdt(buf);
	sem_give(user_list_sem);        

	XSRETURN_PV(user_list);
}

XS(xs_count_users)
{
	dXSARGS;

	if(items != 0)
		XSRETURN_UNDEF;

	XSRETURN_IV(total_users_scr);
}



EXTERN_C void xs_init(void)
{
	char *file = __FILE__;
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
	newXS("odch::get_ip", xs_get_ip, "xs_functions.c");
	newXS("odch::get_share", xs_get_share, "xs_functions.c");
	newXS("odch::data_to_user", xs_data_to_user, "xs_functions.c");
	newXS("odch::kick_user", xs_kick_user, "xs_functions.c");
	newXS("odch::force_move_user", xs_force_move_user, "xs_functions.c");
	newXS("odch::get_variable", xs_get_variable, "xs_functions.c");
	newXS("odch::set_variable", xs_set_variable, "xs_functions.c");
	newXS("odch::add_ban_entry", xs_add_ban_entry, "xs_functions.c");
	newXS("odch::add_nickban_entry", xs_add_nickban_entry, "xs_functions.c");
	newXS("odch::add_allow_entry", xs_add_allow_entry, "xs_functions.c");
	newXS("odch::remove_ban_entry", xs_remove_ban_entry, "xs_functions.c");
	newXS("odch::remove_nickban_entry", xs_remove_nickban_entry, "xs_functions.c");
	newXS("odch::remove_allow_entry", xs_remove_allow_entry, "xs_functions.c");
	newXS("odch::add_reg_user", xs_add_reg_user, "xs_functions.c");
	newXS("odch::remove_reg_user", xs_remove_reg_user, "xs_functions.c");
	newXS("odch::data_to_all", xs_data_to_all, "xs_functions.c");
	newXS("odch::count_users", xs_count_users, "xs_functions.c");
	newXS("odch::register_script_name", xs_register_script_name, "xs_functions.c");
	newXS("odch::check_if_registered", xs_check_if_registered, "xs_functions.c");
	newXS("odch::get_user_list", xs_get_user_list, "xs_functions.c");
	newXS("odch::gag_user", xs_gag_user, "xs_functions.c");
	newXS("odch::ungag_user", xs_ungag_user, "xs_functions.c");
}

#endif /* #ifdef HAVE_PERL */
