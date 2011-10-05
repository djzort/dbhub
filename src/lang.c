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
#include "main.h"
#include "utils.h"
#include "fileio.h"
#include "userlist.h"
#include "network.h"
#include "commands.h"
#include "utils.h"
#include "logger.h"
#include "lang.h"
#include "langvars.h"

//DB// Default language variables in english
void set_default_lang(void)
{
sprintf(lp_accepted_, "accepted");
sprintf(lp_accountupdated, "Account updated successfully");
sprintf(lp_active_, "Active");
sprintf(lp_added_, "added");
sprintf(lp_addedentryto, "Added entry to");
sprintf(lp_admins_, "admins");
sprintf(lp_ago_, "ago");
sprintf(lp_after, "after");
sprintf(lp_allowed_, "allowed");
sprintf(lp_allowedatmost, "Allowed at most");
sprintf(lp_allowedclients, "Allowed clients are");
sprintf(lp_alltime, "Alltime hub records");
sprintf(lp_autoawaymodeoff, "Auto-Away mode OFF");
sprintf(lp_autoawaymodeon, "Auto-Away mode ON");
sprintf(lp_awayfor, "Away for");
sprintf(lp_awaymodeoff, "Away mode OFF");
sprintf(lp_awaymodeon, "Away mode ON");
sprintf(lp_availablecommands, "Available commands for your class");
sprintf(lp_avgdataperuser, "Average data per user");
sprintf(lp_badpass, "has provided bad password");
sprintf(lp_autoawaynotset, "Autoaway was not set. Minimum autoaway time is 15 minutes");
sprintf(lp_autoawayremoved, "Your autoaway has been removed");
sprintf(lp_floodwarn, "FLOOD... Do not write to the main chat in next 20 seconds...");
sprintf(lp_searchforip, "Searching for users using IP");
sprintf(lp_categnotfound, "was not found in categories. Choose one of these");
sprintf(lp_slotbegging, "slotbegging");
sprintf(lp_toreqlist, "to the list of requests. Type !getreq for the complete list of latest requests");
sprintf(lp_toreleaselist, "to the list of releases. Type !getrls for the complete list of latest releases");
sprintf(lp_ban_, "BAN");
sprintf(lp_autoawayset, "Your autoaway has been set. Time of inactivity to enable");
sprintf(lp_whohubswarn, "You can search for users connected to at least 3 hubs");
sprintf(lp_musthavedot, "Partial IP address should contain at least one dot");
sprintf(lp_categlist, "List of allowed categories");
sprintf(lp_forother, "for other releases from category");
sprintf(lp_charnumbers, "Required number of characters for that command is");
sprintf(lp_searchresults, "Search results for");
sprintf(lp_bannedby, "Banned by");
sprintf(lp_banning_, "Banning");
sprintf(lp_banreason, "BAN REASON");
sprintf(lp_banworddetected, "Banword detected from user");
sprintf(lp_badsyntax, "Bad syntax. Correct one is");
sprintf(lp_because, "because");
sprintf(lp_blocked_, "blocked");
sprintf(lp_bots_, "bots");
sprintf(lp_bytes, "bytes");
sprintf(lp_cannotbeblank, "cannot be blank or have space(s)");
sprintf(lp_canonlypmtoops, "You can only send private messages to ops!");
sprintf(lp_category_, "category");
sprintf(lp_categletter_, "category 1st letter");
sprintf(lp_youcanwarnonly, "You can warn only users with lower class than your class");
sprintf(lp_chatting_, "chatting");
sprintf(lp_cheefs_, "cheefs");
sprintf(lp_client_, "Client");
sprintf(lp_class1, "REGULAR");
sprintf(lp_class2, "PROTECTED");
sprintf(lp_class3, "REG");
sprintf(lp_class4, "VIP");
sprintf(lp_class5, "KVIP");
sprintf(lp_class6, "OP");
sprintf(lp_class7, "CHEEF");
sprintf(lp_class8, "ADMIN");
sprintf(lp_class9, "OWNER");
sprintf(lp_commandscalled, "Hub commands called");
sprintf(lp_connectedsince, "Connected since");
sprintf(lp_connectedusers, "Connected users");
sprintf(lp_connection, "Connection");
sprintf(lp_couldntaddentry, "Couldn't add entry to");
sprintf(lp_couldntremoveentry, "Couldn't remove entry from");
sprintf(lp_currentdate, "Current date is");
sprintf(lp_cut_, "cut");
sprintf(lp_datashared, "Data shared");
sprintf(lp_days, "days");
sprintf(lp_delcommand, "Removed file-command");
sprintf(lp_deletedby, "deleted by");
sprintf(lp_deletedmsg, "All stored messages left for you were deleted");
sprintf(lp_deleteduser, "Deleted account");
sprintf(lp_denied_, "denied");
sprintf(lp_deniedconnections, "Denied connections");
sprintf(lp_desctoolong, "description too long");
sprintf(lp_description, "Description");
sprintf(lp_detectedfrom, "detected from");
sprintf(lp_downloading_, "downloading");
sprintf(lp_downloadoff, "Downloading was disabled on this hub");
sprintf(lp_downloadon, "Downloading was unblocked");
sprintf(lp_downloads, "Downloads");
sprintf(lp_email, "E-mail");
sprintf(lp_entryalreadyonlist, "Entry is already on the");
sprintf(lp_entrywasntfound, "Entry wasn't found in");
sprintf(lp_erasemsgs, "Type !delmsg to erase all messages");
sprintf(lp_expires_, "expires");
sprintf(lp_expiresin, "expires in");
sprintf(lp_fakeshare, "Fake share");
sprintf(lp_faketag, "Fake TAG description");
sprintf(lp_favsend, "Fav message was send to all unregistered users");
sprintf(lp_file_, "file");
sprintf(lp_flooding, "flooding");
sprintf(lp_forbchar, "nickname started with forbidden character");
sprintf(lp_fornext, "for next");
sprintf(lp_forbiddenclient, "forbidden client");
sprintf(lp_freeslots, "Active users with free slots");
sprintf(lp_from_, "from");
sprintf(lp_fulfilment, "Fulfilment");
sprintf(lp_gaggedfrom, "Gagged from");
sprintf(lp_gaggingall, "Gagging all users except admins!");
sprintf(lp_gagminshare, "You will be gagged from searching and downloading until you share enough data");
sprintf(lp_givingtempop, "Giving temporary OP to user");
sprintf(lp_hasparted, "has parted");
sprintf(lp_haveregistertochat, "You have to register to be able to speak to the main chat and send private messages");
sprintf(lp_hi_, "Hi");
sprintf(lp_hidingpresence, "Hiding your presence...");
sprintf(lp_hidingshare, "Hiding your share...");
sprintf(lp_hours, "hours");
sprintf(lp_hubaddr, "Hub address");
sprintf(lp_hubdesc, "Hub description");
sprintf(lp_hubforregistered, "This hub is intended for registered users only. To get full access contact with ops/admins");
sprintf(lp_hubinfo, "Information about hub");
sprintf(lp_hubkilled, "Hub was killed by");
sprintf(lp_hubmassmsg, "HUB MASS MESSAGE");
sprintf(lp_hubname, "Hub name");
sprintf(lp_hubs_, "Hubs");
sprintf(lp_hubsperslot, "hubs per one slot maximally");
sprintf(lp_isalreadyopped, "is already opped");
sprintf(lp_ipban_, "IP ban");
sprintf(lp_ipbans, "IP bans");
sprintf(lp_is_, "is");
sprintf(lp_isaway, "is away");
sprintf(lp_isback, "is back from away");
sprintf(lp_isbanning, "is banning");
sprintf(lp_iskicking, "is kicking");
sprintf(lp_isonlinenow, "is currently online");
sprintf(lp_isoffline, "is currently offline");
sprintf(lp_itemstotal, "items total");
sprintf(lp_joins, "JOINS");
sprintf(lp_kick_, "KICK");
sprintf(lp_kickdeny, "You cannot kick that user");
sprintf(lp_kickedby, "Kicked by");
sprintf(lp_kvips_, "kvips");
sprintf(lp_langpackfound, "Language pack found and loaded...");
sprintf(lp_lackoftag, "Lack of TAG description");
sprintf(lp_lackofprefix, "lack of required nickname prefix");
sprintf(lp_lastmainmsgs, "last main chat messages");
sprintf(lp_lastpermbans, "last permanent bans");
sprintf(lp_lastsearch, "Last sending a search string");
sprintf(lp_lastsendpm, "Last sending of private message");
sprintf(lp_lasttempbans, "last temporary bans");
sprintf(lp_lastwritetomain, "Last write to main chat");
sprintf(lp_listofgagged, "List of gagged users");
sprintf(lp_loggedin, "logged in");
sprintf(lp_longestconnected, "longest connected users");
sprintf(lp_masssend, "Massmessage was send to all");
sprintf(lp_maxusers, "Max users");
sprintf(lp_mailtoolong, "e-mail address too long");
sprintf(lp_mainchatlocked, "Main chat locked");
sprintf(lp_mainchatoff, "Main chat was disabled on this hub");
sprintf(lp_mainchaton, "Main chat was unblocked");
sprintf(lp_mainchatmsgs, "Main chat messages");
sprintf(lp_maxhubs, "Maxhubs");
sprintf(lp_maxshare, "Maximum share");
sprintf(lp_maxslots, "Maxslots");
sprintf(lp_message_, "message");
sprintf(lp_mindcppver, "Minimum DC++ version");
sprintf(lp_minactshare, "Minimum share for active REGULAR users");
sprintf(lp_minpsvshare, "Minimum share for passive REGULAR users");
sprintf(lp_minkvipshare, "Minimum share for KVIP users");
sprintf(lp_minopshare, "Minimum share for OP users");
sprintf(lp_minregshare, "Minimum share for REG users");
sprintf(lp_minshare, "Minimum share");
sprintf(lp_minvipshare, "Minimum share for VIP users");
sprintf(lp_minslots, "Minslots");
sprintf(lp_minutes, "minutes");
sprintf(lp_mode_, "Mode");
sprintf(lp_msgforyou, "Messages left for you");
sprintf(lp_name_, "name");
sprintf(lp_never_, "never");
sprintf(lp_newcommand, "New file-command added");
sprintf(lp_newregistered, "New registered user");
sprintf(lp_nextwarningban, "next warning will make you banned!");
sprintf(lp_nick_, "nickname");
sprintf(lp_nickban_, "nickban");
sprintf(lp_nickbans, "nickbans");
sprintf(lp_nickchar, "nickname contains unallowed character(s)");
sprintf(lp_nickinuse, "nickname already in use");
sprintf(lp_nickname, "Nickname");
sprintf(lp_nicktooshort, "nickname too short");
sprintf(lp_nomsgtodelete, "No messages to delete");
sprintf(lp_nonewlines, "Using newlines is not allowed");
sprintf(lp_nonewmsg, "There are no new messages left for you");
sprintf(lp_nopass, "tried to login to registered account without password");
sprintf(lp_nopermission, "Operation not permitted");
sprintf(lp_normal_, "normal");
sprintf(lp_notallowedtoadd, "You are not allowed to add new");
sprintf(lp_notallowedtochat, "You are not allowed to chat at this time");
sprintf(lp_notallowedtosee, "You are not allowed to see");
sprintf(lp_notenoughslots, "not enough slots");
sprintf(lp_nosuchfile, "No such file");
sprintf(lp_nosuchvariable, "No such variable");
sprintf(lp_notallowedtodelreg, "You are not allowed to remove this user");
sprintf(lp_nowarnings, "No warnings for user");
sprintf(lp_number_, "number");
sprintf(lp_opped_, "opped");
sprintf(lp_ops_, "ops");
sprintf(lp_owners_, "owners");
sprintf(lp_pass_, "password");
sprintf(lp_passchanged, "Your password was changed");
sprintf(lp_passive_, "Passive");
sprintf(lp_passwordincorrect, "That password was incorrect!");
sprintf(lp_passwordrequired, "Password required for that account!");
sprintf(lp_permanent_, "permanent");
sprintf(lp_permanently, "permanently");
sprintf(lp_permbans, "perm bans");
sprintf(lp_pmoff, "Private messaging was disabled on this hub");
sprintf(lp_pmon, "Private messaging was unblocked");
sprintf(lp_pmmsgs, "Private messages");
sprintf(lp_presencealreadyhidden, "Your presence is already hidden...");
sprintf(lp_presencehidden, "Your presence is hidden, you have to use !unhideme command first...");
sprintf(lp_protectmepass, "Type +protectme 'password' to get temporary protection to this hub.");
sprintf(lp_prots_, "protected");
sprintf(lp_prottime, "Protection time");
sprintf(lp_prottype, "Protection type");
sprintf(lp_proxy_, "Passive proxy");
sprintf(lp_proxynotallowed, "proxy connections not allowed");
sprintf(lp_psvnotallowed, "passive mode not allowed");
sprintf(lp_psvwithmodea, "nickname with psv, but mode M:A");
sprintf(lp_psvwithoutpsvinnick, "passive mode requires 'psv' in nick");
sprintf(lp_quits, "QUITS");
sprintf(lp_rawadm, "Administration");
sprintf(lp_rawop, "Security");
sprintf(lp_rawuser, "User");
sprintf(lp_reason, "reason");
sprintf(lp_recentlyadded, "The recently added");
sprintf(lp_redir_, "REDIRECT");
sprintf(lp_registeredby, "registered by");
sprintf(lp_registrationfailed, "Registration failed! Password has to be ONE word!");
sprintf(lp_registrationrules, "Registration rules");
sprintf(lp_regmepass, "Type +regme 'password' to register to this hub.");
sprintf(lp_regonly, "reg only");
sprintf(lp_regs_, "regs");
sprintf(lp_registered_, "registered");
sprintf(lp_regular_, "regular");
sprintf(lp_releases, "releases");
sprintf(lp_removedallwarns, "Removed all warnings for user");
sprintf(lp_removedentryfrom, "Removed entry from");
sprintf(lp_requiredatleast, "Required at least");
sprintf(lp_requests, "requests");
sprintf(lp_reportedby, "Reported by");
sprintf(lp_reportsend, "Your report was send to the ops");
sprintf(lp_restrictions1, "Due to restrictions you are able to download files and filelist ONLY from users from your class or lower! Unable to connect to");
sprintf(lp_restrictions2, "Due to restrictions you are able to download files and filelist ONLY from other UNREGISTERED users! Register to get the full access to this hub! Unable to connect to");
sprintf(lp_savedmsg, "Successfully saved your message to");
sprintf(lp_searches, "Searches");
sprintf(lp_searching_, "searching");
sprintf(lp_searchignored, "Search ignored due to search spam. Wait at least");
sprintf(lp_searchoff, "Searching was disabled on this hub");
sprintf(lp_searchon, "Searching was unblocked");
sprintf(lp_searchspam, "Searchspam time");
sprintf(lp_searchtooshort, "Your search string is too short. Minimum search characters is");
sprintf(lp_seconds, "seconds");
sprintf(lp_setby, "set by");
sprintf(lp_setto, "set to");
sprintf(lp_share_, "of shared data");
sprintf(lp_sharesize, "Sharesize");
sprintf(lp_shouldbechecked, "should be checked because of");
sprintf(lp_slotsperhub, "slots per hub minimally");
sprintf(lp_spacesinnick, "spaces in nickname");
sprintf(lp_recordresetted, "Share and users records has been resetted");
sprintf(lp_recordshare, "Alltime hub share record was broken");
sprintf(lp_recordusers, "Alltime hub users record was broken");
sprintf(lp_redirectingall, "Redirecting all users to");
sprintf(lp_reloadingscripts, "Reloading scripts...");
sprintf(lp_selfregistrationdisabled, "Self-registration is disabled. Type +regme for registration rules");
sprintf(lp_since, "since");
sprintf(lp_slotratio, "Slotratio");
sprintf(lp_slots_, "Slots");
sprintf(lp_spamming, "spamming");
sprintf(lp_statistics, "statistics");
sprintf(lp_tag_, "TAG");
sprintf(lp_tempbans, "temp bans");
sprintf(lp_tempkickban, "Temporary ban after kick");
sprintf(lp_temporary_, "temporary");
sprintf(lp_text_, "text");
sprintf(lp_thesameip, "Too many connection from one IP");
sprintf(lp_time_, "time");
sprintf(lp_timetounban, "Time to unban");
sprintf(lp_toohighratio, "hubs/slot ratio too high");
sprintf(lp_toolowratio, "slots/hub ratio too low");
sprintf(lp_toomanyhubs, "too many opened hubs");
sprintf(lp_toomanyslots, "too many open slots");
sprintf(lp_toomanywarnings, "too many warnings");
sprintf(lp_tooolddcppver, "Too old DC++ version");
sprintf(lp_total_, "TOTAL");
sprintf(lp_topicchangedby, "Topic was changed by");
sprintf(lp_topicis, "Topic is");
sprintf(lp_topicnotset, "Topic is not set");
sprintf(lp_topicunsetby, "Topic was unset by");
sprintf(lp_totalvisitors, "Total visitors");
sprintf(lp_type_, "type");
sprintf(lp_typehelp, "Type +help to see all available commands for you.");
sprintf(lp_typeregme, "Type +regme to see registration rules.");
sprintf(lp_typerules, "Type +rules to see hub rules.");
sprintf(lp_unfinishedfiles, "unfinished files in share");
sprintf(lp_ungaggingall, "Ungagging all users");
sprintf(lp_ungagminshare, "Required amount of data shared. Now you can search and download");
sprintf(lp_unhidingpresence, "Unhiding your share and presence...");
sprintf(lp_unhidingshare, "Unhiding your share...");
sprintf(lp_uploadlimittoolow, "upload limit too low");
sprintf(lp_uptime, "Current hub uptime:");
sprintf(lp_userinfo, "Information about user");
sprintf(lp_user_, "user");
sprintf(lp_users, "users");
sprintf(lp_usertype, "User type");
sprintf(lp_value, "value");
sprintf(lp_variable, "variable");
sprintf(lp_version, "Version");
sprintf(lp_vips_, "vips");
sprintf(lp_warning_, "warning");
sprintf(lp_wasaddedto, "was added to");
sprintf(lp_wasbannedby, "was banned by");
sprintf(lp_wascleared, "was cleared");
sprintf(lp_wasdropped, "was dropped from");
sprintf(lp_wasgaggedby, "was gagged by");
sprintf(lp_wasgaggedfrom, "was gagged from");
sprintf(lp_waskickedby, "was kicked by");
sprintf(lp_wasregistered, "was successfully registered as the");
sprintf(lp_wasreloaded, "was reloaded to memory");
sprintf(lp_wasungagged, "was ungagged");
sprintf(lp_waswarnedby, "was warned by");
sprintf(lp_weeks, "weeks");
sprintf(lp_welcometo, "welcome to the");
sprintf(lp_withpass, "with password");
sprintf(lp_youraccountwillexpire, "Your account will expire in");
sprintf(lp_youripis, "Your IP is");
sprintf(lp_yourpasswordis, "Your current password is");
sprintf(lp_yourwarnings, "Your warnings");
sprintf(lp_youshare, "You share");
sprintf(lp_youwerebanned, "You were banned from the hub");
sprintf(lp_youweregagged, "You were gagged from");
sprintf(lp_youwerekicked, "You were kicked from the hub");
sprintf(lp_youwereregistered, "You were registered as the");
sprintf(lp_youweretempoppedby, "You were temporary opped by");
sprintf(lp_youwerewarned, "You were warned");
/* UserCommands */
sprintf(lp_us001, "Help");
sprintf(lp_us002, "Hub rules");
sprintf(lp_us003, "Registration rules");
sprintf(lp_us004, "Protect me");
sprintf(lp_us005, "Reg me");
sprintf(lp_us006, "MOTD");
sprintf(lp_us007, "IRC /me");
sprintf(lp_us008, "Your IP");
sprintf(lp_us009, "Your info");
sprintf(lp_us010, "Hub info");
sprintf(lp_us011, "Report user");
sprintf(lp_us012, "Search user");
sprintf(lp_us013, "Last 20 messages");
sprintf(lp_us014, "Free slots");
sprintf(lp_us015, "Away ON");
sprintf(lp_us016, "Away OFF");
sprintf(lp_us017, "Set Auto-Away");
sprintf(lp_us018, "Remove Auto-Away");
sprintf(lp_us019, "Date");
sprintf(lp_us020, "Longest 10 connected");
sprintf(lp_us021, "Records");
sprintf(lp_us022, "Uptime");
sprintf(lp_us023, "Get URL list");
sprintf(lp_us024, "Add release");
sprintf(lp_us025, "Get releases list");
sprintf(lp_us026, "Get releases list with type");
sprintf(lp_us027, "Find release");
sprintf(lp_us028, "Request file");
sprintf(lp_us029, "Get req list");
sprintf(lp_us030, "Leave message for someone");
sprintf(lp_us031, "Read messages to you");
sprintf(lp_us032, "Erase messages to you");
sprintf(lp_us033, "Userinfo");
sprintf(lp_us034, "Kick user");
sprintf(lp_us035, "Your password");
sprintf(lp_us036, "Change your password");
sprintf(lp_us037, "Add temporary PROTECTED user");
sprintf(lp_us038, "Add permanent PROTECTED user");
sprintf(lp_us039, "Add REG user");
sprintf(lp_us040, "Add VIP user");
sprintf(lp_us041, "Add KVIP user");
sprintf(lp_us042, "Warn user");
sprintf(lp_us043, "Perm ban user");
sprintf(lp_us044, "Perm ban IP");
sprintf(lp_us045, "Perm ban nick");
sprintf(lp_us046, "Get banlist");
sprintf(lp_us048, "Remove IP ban");
sprintf(lp_us049, "Remove nickban");
sprintf(lp_us050, "Last 20 perm bans");
sprintf(lp_us051, "Last 20 temp bans");
sprintf(lp_us052, "Allow");
sprintf(lp_us053, "Get allowlist");
sprintf(lp_us054, "Unallow");
sprintf(lp_us055, "Gag user");
sprintf(lp_us056, "Gag IP");
sprintf(lp_us057, "Get gaglist");
sprintf(lp_us058, "Gagged IPs list");
sprintf(lp_us059, "Ungag user");
sprintf(lp_us060, "Ungag IP");
sprintf(lp_us063, "IP history");
sprintf(lp_us064, "Who IP");
sprintf(lp_us065, "Who hubs");
sprintf(lp_us066, "Who slots");
sprintf(lp_us067, "Who ratio");
sprintf(lp_us068, "Send favmessage");
sprintf(lp_us069, "Send massmessage to REGULAR users");
sprintf(lp_us070, "Send massmessage to PROTECTED users");
sprintf(lp_us071, "Send massmessage to REG users");
sprintf(lp_us072, "Send massmessage to VIP users");
sprintf(lp_us073, "Send massmessage to KVIP users");
sprintf(lp_us074, "Send massmessage to OP users");
sprintf(lp_us075, "Send massmessage to CHEEF users");
sprintf(lp_us076, "Send massmessage to ADMIN users");
sprintf(lp_us077, "Send massmessage to all");
sprintf(lp_us078, "Set topic");
sprintf(lp_us079, "Unset topic");
sprintf(lp_us080, "Hide your share");
sprintf(lp_us081, "Unhide your share");
sprintf(lp_us082, "Hide your presence");
sprintf(lp_us083, "Unhide your presence");
sprintf(lp_us084, "Get config");
sprintf(lp_us085, "Set variable");
sprintf(lp_us086, "Get langfile");
sprintf(lp_us087, "Set lang variable");
sprintf(lp_us088, "Get reglist");
sprintf(lp_us089, "Reload scripts");
sprintf(lp_us090, "Reload lang");
sprintf(lp_us091, "Reset lang to English");
sprintf(lp_us092, "Add OP user");
sprintf(lp_us093, "Add CHEEF user");
sprintf(lp_us094, "Add ADMIN user");
sprintf(lp_us095, "Add OWNER user");
sprintf(lp_us096, "Delete registered user");
sprintf(lp_us097, "Give OP");
sprintf(lp_us098, "Say as bot");
sprintf(lp_us099, "Say as someone");
sprintf(lp_us100, "Block download");
sprintf(lp_us101, "Block main");
sprintf(lp_us102, "Block pm");
sprintf(lp_us103, "Block search");
sprintf(lp_us104, "Unblock download");
sprintf(lp_us105, "Unblock main");
sprintf(lp_us106, "Unblock pm");
sprintf(lp_us107, "Unblock search");
sprintf(lp_us108, "Redirect all");
sprintf(lp_us109, "Gag all");
sprintf(lp_us110, "Ungag all");
sprintf(lp_us111, "Add release category");
sprintf(lp_us112, "Get categories list");
sprintf(lp_us113, "Remove release category");
sprintf(lp_us114, "Remove release");
sprintf(lp_us115, "Remove request");
sprintf(lp_us116, "Remove URL");
sprintf(lp_us117, "Remove warn");
sprintf(lp_us118, "Add welcome");
sprintf(lp_us119, "Get randomwelcomes");
sprintf(lp_us120, "Remove welcome");
sprintf(lp_us122, "Kill hub");
sprintf(lp_us123, "Users with warnings");
sprintf(lp_us124, "Create new command-file");
sprintf(lp_us125, "Remove command-file");
sprintf(lp_us126, "Remove all IP bans");                                                                                           
sprintf(lp_us127, "Remove all nick bans");
sprintf(lp_us128, "Temp ban user");
sprintf(lp_us129, "Temp ban IP");
sprintf(lp_us130, "Temp ban nick");
sprintf(lp_us131, "Disconnect user");
sprintf(lp_us132, "Reload file to memory");
sprintf(lp_us133, "Change reg class");
sprintf(lp_us134, "Change reg nick");
sprintf(lp_us135, "Reset records");
/* !help commands' descriptions */
sprintf(lp_he001, "this command");
sprintf(lp_he002, "displays hub rules");
sprintf(lp_he003, "displays hub registration rules");
sprintf(lp_he004, "registers yourself as the PROTECTED user");
sprintf(lp_he005, "registers yourself as the REG user");
sprintf(lp_he006, "displays the motd file");
sprintf(lp_he007, "IRC-like /me command (sometimes called 'action')");
sprintf(lp_he008, "shows your IP");
sprintf(lp_he009, "shows some information about you");
sprintf(lp_he010, "shows some hub settings");
sprintf(lp_he011, "reports online users who is breaking the rules to the ops and admins");
sprintf(lp_he012, "looks for selected user in 'last seen' database");
sprintf(lp_he013, "shows last 20 main chat messages");
sprintf(lp_he014, "shows users in active mode with free slots");
sprintf(lp_he015, "toggles away mode on with 'reason'");
sprintf(lp_he016, "toggles away mode off");
sprintf(lp_he017, "toggles auto-away mode on with 'reason' every 'time_in_minutes' of inactivity");
sprintf(lp_he018, "removes your auto-away");
sprintf(lp_he019, "shows current (server) date");
sprintf(lp_he020, "shows 10 longest connected users");
sprintf(lp_he021, "displays the alltime share/users hub records");
sprintf(lp_he022, "shows hub uptime");
sprintf(lp_he023, "displays the list of grabbed URLs");
sprintf(lp_he024, "adds release to releases list");
sprintf(lp_he025, "displays the list of recently added releases");
sprintf(lp_he026, "displays the list of recently added 'category' releases");
sprintf(lp_he027, "looks for release");
sprintf(lp_he028, "adds req to reqests list");
sprintf(lp_he029, "displays the list of recently added requests");
sprintf(lp_he030, "leaves message to offline user");
sprintf(lp_he031, "shows messages written to you");
sprintf(lp_he032, "erases messages written to you");
sprintf(lp_he033, "shows information about the selected user");
sprintf(lp_he034, "kicks user from hub");
sprintf(lp_he035, "shows your password");
sprintf(lp_he036, "changes your password");
sprintf(lp_he037, "adds permanent PROTECTED user to reglist");
sprintf(lp_he038, "adds temporary PROTECTED user to reglist");
sprintf(lp_he039, "adds REG user to reglist");
sprintf(lp_he040, "adds VIP user to reglist");
sprintf(lp_he041, "adds KVIP user to reglist");
sprintf(lp_he042, "sends warning to specified user");
sprintf(lp_he043, "bans permanently user's IP and nickname");
sprintf(lp_he044, "bans permanently IP");
sprintf(lp_he045, "bans permanently nickname");
sprintf(lp_he046, "displays the banlist file");
sprintf(lp_he048, "removes an entry from the banlist file");
sprintf(lp_he049, "removes an entry from the nickbanlist file");
sprintf(lp_he050, "shows 20 last permanent bans");
sprintf(lp_he051, "shows 20 last temporary bans");
sprintf(lp_he052, "adds an entry to the allowlist");
sprintf(lp_he053, "displays the allowlist file");
sprintf(lp_he054, "removes an entry from the allowlist file");
sprintf(lp_he055, "gags user, available gag types: chat, search, download (if left blank: all of these)");
sprintf(lp_he056, "gags permanently selected 'ip' from chatting");
sprintf(lp_he057, "shows all gagged users");
sprintf(lp_he058, "displays the list of gagged IPs");
sprintf(lp_he059, "removes the gag from a user");
sprintf(lp_he060, "removes an entry from the ipgag file");
sprintf(lp_he063, "history of connections from specified IP");
sprintf(lp_he064, "displays users connected from specified IP");
sprintf(lp_he065, "displays users connected to at least a specified number of hubs");
sprintf(lp_he066, "displays users who opened specified number of slots");
sprintf(lp_he067, "displays users with equal or higher hubs per slot ratio");
sprintf(lp_he068, "sends fav-message to all unregistered users");
sprintf(lp_he069, "sends a private message to all logged in REGULAR users");
sprintf(lp_he070, "sends a private message to all logged in PROTECTED users");
sprintf(lp_he071, "sends a private message to all logged in REG users");
sprintf(lp_he072, "sends a private message to all logged in VIP users");
sprintf(lp_he073, "sends a private message to all logged in KVIP users");
sprintf(lp_he074, "sends a private message to all logged in OP users");
sprintf(lp_he075, "sends a private message to all logged in CHEEF users");
sprintf(lp_he076, "sends a private message to all logged in ADMIN users");
sprintf(lp_he077, "sends a private message to all logged in users");
sprintf(lp_he078, "changes hub topic");
sprintf(lp_he079, "unsets hub topic");
sprintf(lp_he080, "hides your share");
sprintf(lp_he081, "unhides your share");
sprintf(lp_he082, "hides your presence");
sprintf(lp_he083, "unhides your presence");
sprintf(lp_he084, "displays the config file");
sprintf(lp_he085, "sets a value in the config file");
sprintf(lp_he086, "displays the language file");
sprintf(lp_he087, "sets language variable to new value");
sprintf(lp_he088, "displays the reglist");
sprintf(lp_he089, "reloads the scripts in the script directory");
sprintf(lp_he090, "reloads the language file");
sprintf(lp_he091, "resets the language to English");
sprintf(lp_he092, "adds OP user to reglist");
sprintf(lp_he093, "adds CHEEF user to reglist");
sprintf(lp_he094, "adds ADMIN user to reglist");
sprintf(lp_he095, "adds OWNER user to reglist");
sprintf(lp_he096, "removes a user from the reglist");
sprintf(lp_he097, "gives online user the temporary OP status (until he/she leaved the hub)");
sprintf(lp_he098, "says something as the botnick");
sprintf(lp_he099, "says some 'text' from the 'nickname'");
sprintf(lp_he100, "locks main chat, priv messaging, searching or downloading");
sprintf(lp_he101, "unlocks main chat, priv messaging, searching or downloading");
sprintf(lp_he102, "redirects all users to 'ip or hostname'");
sprintf(lp_he103, "gags all user except admins from chatting, searching and downloading");
sprintf(lp_he104, "ungags all users");
sprintf(lp_he105, "adds release category");
sprintf(lp_he106, "displays the release categories");
sprintf(lp_he107, "removes release category");
sprintf(lp_he108, "removes an entry from releases list");
sprintf(lp_he109, "removes an entry from req list");
sprintf(lp_he110, "removes an entry from urls list");
sprintf(lp_he111, "removes all warnings from specified user");
sprintf(lp_he112, "adds 'welcome' to randomwelomes");
sprintf(lp_he113, "displays the randomwelomes list");
sprintf(lp_he114, "removes 'welcome' from randomwelomes");
sprintf(lp_he116, "terminates the program");
sprintf(lp_he117, "shows the users with warnings");
sprintf(lp_he118, "creates new command-file");
sprintf(lp_he119, "removes command-file");
sprintf(lp_he120, "removes all IP bans");                                                                                          
sprintf(lp_he121, "removes all nickname bans");  
sprintf(lp_he122, "bans temporary user's IP and nickname");
sprintf(lp_he123, "bans temporary IP");
sprintf(lp_he124, "bans temporary nickname");
sprintf(lp_he125, "disconnects user from the hub");
sprintf(lp_he126, "reloads file edited from shell to memory");
sprintf(lp_he127, "changes the class of the registered user");
sprintf(lp_he128, "changes the nickname of the registered user");
sprintf(lp_he129, "resets share and users records");
}

#ifdef HAVE_LANG

//DB// Reads language file if exists
int read_lang(void)
{
   int i, j;
   int fd;
   int erret;
   FILE *fp;
   char path[MAX_FDP_LEN+1];
   char line[1024];
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, LANG_FILE);
   while(((fd = open(path, O_RDONLY)) < 0) && (errno == EINTR))
     logprintf(0, "Error - In read_lang()/open(): Interrupted system call. Trying again.\n");
   if(fd < 0)
     {
        logprintf(0, "Error - In read_lang()/open(): %s", strerror(errno));
        return -1;
     }
   /* Set the lock */
   if(set_lock(fd, F_RDLCK) == 0)
     {
        logprintf(0, "Error - In read_lang(): Couldn't set file lock\n");
        close(fd);
        return -1;
     }
   if((fp = fdopen(fd, "r")) == NULL)
     {
        logprintf(0, "Error - In read_lang()/fdopen(): %s", strerror(errno));
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
             if(strncmp(line + i, "lp_accepted_", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_accepted_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_accepted_ + strlen(lp_accepted_) - 1) == '"')
                    *(lp_accepted_ + strlen(lp_accepted_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_accountupdated", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_accountupdated, strchr(line + i, '"') + 1, 100);
                  if(*(lp_accountupdated + strlen(lp_accountupdated) - 1) == '"')
                    *(lp_accountupdated + strlen(lp_accountupdated) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_active_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_active_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_active_ + strlen(lp_active_) - 1) == '"')
                    *(lp_active_ + strlen(lp_active_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_added_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_added_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_added_ + strlen(lp_added_) - 1) == '"')
                    *(lp_added_ + strlen(lp_added_) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_addedentryto", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_addedentryto, strchr(line + i, '"') + 1, 50);
                  if(*(lp_addedentryto + strlen(lp_addedentryto) - 1) == '"')
                    *(lp_addedentryto + strlen(lp_addedentryto) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_admins_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_admins_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_admins_ + strlen(lp_admins_) - 1) == '"')
                    *(lp_admins_ + strlen(lp_admins_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_after", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_after, strchr(line + i, '"') + 1, 50);
                  if(*(lp_after + strlen(lp_after) - 1) == '"')
                    *(lp_after + strlen(lp_after) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_ago_", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_ago_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_ago_ + strlen(lp_ago_) - 1) == '"')
                    *(lp_ago_ + strlen(lp_ago_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_allowed_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_allowed_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_allowed_ + strlen(lp_allowed_) - 1) == '"')
                    *(lp_allowed_ + strlen(lp_allowed_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_allowedatmost", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_allowedatmost, strchr(line + i, '"') + 1, 50);
                  if(*(lp_allowedatmost + strlen(lp_allowedatmost) - 1) == '"')
                    *(lp_allowedatmost + strlen(lp_allowedatmost) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_allowedclients", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_allowedclients, strchr(line + i, '"') + 1, 50);
                  if(*(lp_allowedclients + strlen(lp_allowedclients) - 1) == '"')
                    *(lp_allowedclients + strlen(lp_allowedclients) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_alltime", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_alltime, strchr(line + i, '"') + 1, 50);
                  if(*(lp_alltime + strlen(lp_alltime) - 1) == '"')
                    *(lp_alltime + strlen(lp_alltime) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_autoawaymodeoff", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_autoawaymodeoff, strchr(line + i, '"') + 1, 50);
                  if(*(lp_autoawaymodeoff + strlen(lp_autoawaymodeoff) - 1) == '"')
                    *(lp_autoawaymodeoff + strlen(lp_autoawaymodeoff) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_autoawaymodeon", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_autoawaymodeon, strchr(line + i, '"') + 1, 50);
                  if(*(lp_autoawaymodeon + strlen(lp_autoawaymodeon) - 1) == '"')
                    *(lp_autoawaymodeon + strlen(lp_autoawaymodeon) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_autoawaynotset", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_autoawaynotset, strchr(line + i, '"') + 1, 100);
                  if(*(lp_autoawaynotset + strlen(lp_autoawaynotset) - 1) == '"')
                    *(lp_autoawaynotset + strlen(lp_autoawaynotset) - 1) = '\0';
               }
	     else if(strncmp(line + i, "lp_autoawayremoved", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_autoawayremoved, strchr(line + i, '"') + 1, 100);
                  if(*(lp_autoawayremoved + strlen(lp_autoawayremoved) - 1) == '"')
                    *(lp_autoawayremoved + strlen(lp_autoawayremoved) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_autoawayset", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_autoawayset, strchr(line + i, '"') + 1, 100);
                  if(*(lp_autoawayset + strlen(lp_autoawayset) - 1) == '"')
                    *(lp_autoawayset + strlen(lp_autoawayset) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_availablecommands", 20) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_availablecommands, strchr(line + i, '"') + 1, 100);
                  if(*(lp_availablecommands + strlen(lp_availablecommands) - 1) == '"')
                    *(lp_availablecommands + strlen(lp_availablecommands) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_avgdataperuser", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_avgdataperuser, strchr(line + i, '"') + 1, 50);
                  if(*(lp_avgdataperuser + strlen(lp_avgdataperuser) - 1) == '"')
                    *(lp_avgdataperuser + strlen(lp_avgdataperuser) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_awayfor", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_awayfor, strchr(line + i, '"') + 1, 50);
                  if(*(lp_awayfor + strlen(lp_awayfor) - 1) == '"')
                    *(lp_awayfor + strlen(lp_awayfor) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_awaymodeoff", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_awaymodeoff, strchr(line + i, '"') + 1, 50);
                  if(*(lp_awaymodeoff + strlen(lp_awaymodeoff) - 1) == '"')
                    *(lp_awaymodeoff + strlen(lp_awaymodeoff) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_awaymodeon", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_awaymodeon, strchr(line + i, '"') + 1, 50);
                  if(*(lp_awaymodeon + strlen(lp_awaymodeon) - 1) == '"')
                    *(lp_awaymodeon + strlen(lp_awaymodeon) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_badpass", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_badpass, strchr(line + i, '"') + 1, 100);
                  if(*(lp_badpass + strlen(lp_badpass) - 1) == '"')
                    *(lp_badpass + strlen(lp_badpass) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_badsyntax", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_badsyntax, strchr(line + i, '"') + 1, 100);
                  if(*(lp_badsyntax + strlen(lp_badsyntax) - 1) == '"')
                    *(lp_badsyntax + strlen(lp_badsyntax) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_ban_", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_ban_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_ban_ + strlen(lp_ban_) - 1) == '"')
                    *(lp_ban_ + strlen(lp_ban_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_bannedby", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_bannedby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_bannedby + strlen(lp_bannedby) - 1) == '"')
                    *(lp_bannedby + strlen(lp_bannedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_banning_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_banning_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_banning_ + strlen(lp_banning_) - 1) == '"')
                    *(lp_banning_ + strlen(lp_banning_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_banreason", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_banreason, strchr(line + i, '"') + 1, 50);
                  if(*(lp_banreason + strlen(lp_banreason) - 1) == '"')
                    *(lp_banreason + strlen(lp_banreason) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_banworddetected", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_banworddetected, strchr(line + i, '"') + 1, 50);
                  if(*(lp_banworddetected + strlen(lp_banworddetected) - 1) == '"')
                    *(lp_banworddetected + strlen(lp_banworddetected) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_because", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_because, strchr(line + i, '"') + 1, 50);
                  if(*(lp_because + strlen(lp_because) - 1) == '"')
                    *(lp_because + strlen(lp_because) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_blocked_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_blocked_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_blocked_ + strlen(lp_blocked_) - 1) == '"')
                    *(lp_blocked_ + strlen(lp_blocked_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_bots_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_bots_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_bots_ + strlen(lp_bots_) - 1) == '"')
                    *(lp_bots_ + strlen(lp_bots_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_bytes", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_bytes, strchr(line + i, '"') + 1, 50);
                  if(*(lp_bytes + strlen(lp_bytes) - 1) == '"')
                    *(lp_bytes + strlen(lp_bytes) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_cannotbeblank", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_cannotbeblank, strchr(line + i, '"') + 1, 100);
                  if(*(lp_cannotbeblank + strlen(lp_cannotbeblank) - 1) == '"')
                    *(lp_cannotbeblank + strlen(lp_cannotbeblank) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_canonlypmtoops", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_canonlypmtoops, strchr(line + i, '"') + 1, 100);
                  if(*(lp_canonlypmtoops + strlen(lp_canonlypmtoops) - 1) == '"')
                    *(lp_canonlypmtoops + strlen(lp_canonlypmtoops) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_categletter_", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_categletter_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_categletter_ + strlen(lp_categletter_) - 1) == '"')
                    *(lp_categletter_ + strlen(lp_categletter_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_categlist", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_categlist, strchr(line + i, '"') + 1, 100);
                  if(*(lp_categlist + strlen(lp_categlist) - 1) == '"')
                    *(lp_categlist + strlen(lp_categlist) - 1) = '\0';
               }             
	     else if(strncmp(line + i, "lp_categnotfound", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_categnotfound, strchr(line + i, '"') + 1, 100);
                  if(*(lp_categnotfound + strlen(lp_categnotfound) - 1) == '"')
                    *(lp_categnotfound + strlen(lp_categnotfound) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_category_", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_category_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_category_ + strlen(lp_category_) - 1) == '"')
                    *(lp_category_ + strlen(lp_category_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_charnumbers", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_charnumbers, strchr(line + i, '"') + 1, 100);
                  if(*(lp_charnumbers + strlen(lp_charnumbers) - 1) == '"')
                    *(lp_charnumbers + strlen(lp_charnumbers) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_chatting_", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_chatting_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_chatting_ + strlen(lp_chatting_) - 1) == '"')
                    *(lp_chatting_ + strlen(lp_chatting_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_cheefs_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_cheefs_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_cheefs_ + strlen(lp_cheefs_) - 1) == '"')
                    *(lp_cheefs_ + strlen(lp_cheefs_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class1", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class1, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class1 + strlen(lp_class1) - 1) == '"')
                    *(lp_class1 + strlen(lp_class1) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class2", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class2, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class2 + strlen(lp_class2) - 1) == '"')
                    *(lp_class2 + strlen(lp_class2) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class3", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class3, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class3 + strlen(lp_class3) - 1) == '"')
                    *(lp_class3 + strlen(lp_class3) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class4", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class4, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class4 + strlen(lp_class4) - 1) == '"')
                    *(lp_class4 + strlen(lp_class4) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class5", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class5, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class5 + strlen(lp_class5) - 1) == '"')
                    *(lp_class5 + strlen(lp_class5) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class6", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class6, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class6 + strlen(lp_class6) - 1) == '"')
                    *(lp_class6 + strlen(lp_class6) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class7", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class7, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class7 + strlen(lp_class7) - 1) == '"')
                    *(lp_class7 + strlen(lp_class7) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class8", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class8, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class8 + strlen(lp_class8) - 1) == '"')
                    *(lp_class8 + strlen(lp_class8) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_class9", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_class9, strchr(line + i, '"') + 1, 50);
                  if(*(lp_class9 + strlen(lp_class9) - 1) == '"')
                    *(lp_class9 + strlen(lp_class9) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_client_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_client_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_client_ + strlen(lp_client_) - 1) == '"')
                    *(lp_client_ + strlen(lp_client_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_commandscalled", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_commandscalled, strchr(line + i, '"') + 1, 50);
                  if(*(lp_commandscalled + strlen(lp_commandscalled) - 1) == '"')
                    *(lp_commandscalled + strlen(lp_commandscalled) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_connectedsince", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_connectedsince, strchr(line + i, '"') + 1, 50);
                  if(*(lp_connectedsince + strlen(lp_connectedsince) - 1) == '"')
                    *(lp_connectedsince + strlen(lp_connectedsince) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_connectedusers", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_connectedusers, strchr(line + i, '"') + 1, 50);
                  if(*(lp_connectedusers + strlen(lp_connectedusers) - 1) == '"')
                    *(lp_connectedusers + strlen(lp_connectedusers) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_connection", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_connection, strchr(line + i, '"') + 1, 50);
                  if(*(lp_connection + strlen(lp_connection) - 1) == '"')
                    *(lp_connection + strlen(lp_connection) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_couldntaddentry", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_couldntaddentry, strchr(line + i, '"') + 1, 50);
                  if(*(lp_couldntaddentry + strlen(lp_couldntaddentry) - 1) == '"')
                    *(lp_couldntaddentry + strlen(lp_couldntaddentry) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_couldntremoveentry", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_couldntremoveentry, strchr(line + i, '"') + 1, 50);
                  if(*(lp_couldntremoveentry + strlen(lp_couldntremoveentry) - 1) == '"')
                    *(lp_couldntremoveentry + strlen(lp_couldntremoveentry) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_currentdate", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_currentdate, strchr(line + i, '"') + 1, 50);
                  if(*(lp_currentdate + strlen(lp_currentdate) - 1) == '"')
                    *(lp_currentdate + strlen(lp_currentdate) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_cut_", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_cut_, strchr(line + i, '"') + 1, 30);
                  if(*(lp_cut_ + strlen(lp_cut_) - 1) == '"')
                    *(lp_cut_ + strlen(lp_cut_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_datashared", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_datashared, strchr(line + i, '"') + 1, 50);
                  if(*(lp_datashared + strlen(lp_datashared) - 1) == '"')
                    *(lp_datashared + strlen(lp_datashared) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_days", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_days, strchr(line + i, '"') + 1, 50);
                  if(*(lp_days + strlen(lp_days) - 1) == '"')
                    *(lp_days + strlen(lp_days) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_delcommand", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_delcommand, strchr(line + i, '"') + 1, 100);
                  if(*(lp_delcommand + strlen(lp_delcommand) - 1) == '"')
                    *(lp_delcommand + strlen(lp_delcommand) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_deletedby", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_deletedby, strchr(line + i, '"') + 1, 100);
                  if(*(lp_deletedby + strlen(lp_deletedby) - 1) == '"')
                    *(lp_deletedby + strlen(lp_deletedby) - 1) = '\0';
               }
	    else if(strncmp(line + i, "lp_deletedmsg", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_deletedmsg, strchr(line + i, '"') + 1, 100);
                  if(*(lp_deletedmsg + strlen(lp_deletedmsg) - 1) == '"')
                    *(lp_deletedmsg + strlen(lp_deletedmsg) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_deleteduser", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_deleteduser, strchr(line + i, '"') + 1, 100);
                  if(*(lp_deleteduser + strlen(lp_deleteduser) - 1) == '"')
                    *(lp_deleteduser + strlen(lp_deleteduser) - 1) = '\0';
               }
	     else if(strncmp(line + i, "lp_denied_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_denied_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_denied_ + strlen(lp_denied_) - 1) == '"')
                    *(lp_denied_ + strlen(lp_denied_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_deniedconnections", 20) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_deniedconnections, strchr(line + i, '"') + 1, 50);
                  if(*(lp_deniedconnections + strlen(lp_deniedconnections) - 1) == '"')
                    *(lp_deniedconnections + strlen(lp_deniedconnections) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_description", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_description, strchr(line + i, '"') + 1, 50);
                  if(*(lp_description + strlen(lp_description) - 1) == '"')
                    *(lp_description + strlen(lp_description) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_desctoolong", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_desctoolong, strchr(line + i, '"') + 1, 100);
                  if(*(lp_desctoolong + strlen(lp_desctoolong) - 1) == '"')
                    *(lp_desctoolong + strlen(lp_desctoolong) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_detectedfrom", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_detectedfrom, strchr(line + i, '"') + 1, 50);
                  if(*(lp_detectedfrom + strlen(lp_detectedfrom) - 1) == '"')
                    *(lp_detectedfrom + strlen(lp_detectedfrom) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_downloading_", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_downloading_, strchr(line + i, '"') + 1, 30);
                  if(*(lp_downloading_ + strlen(lp_downloading_) - 1) == '"')
                    *(lp_downloading_ + strlen(lp_downloading_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_downloadoff", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_downloadoff, strchr(line + i, '"') + 1, 100);
                  if(*(lp_downloadoff + strlen(lp_downloadoff) - 1) == '"')
                    *(lp_downloadoff + strlen(lp_downloadoff) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_downloadon", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_downloadon, strchr(line + i, '"') + 1, 100);
                  if(*(lp_downloadon + strlen(lp_downloadon) - 1) == '"')
                    *(lp_downloadon + strlen(lp_downloadon) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_downloads", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_downloads, strchr(line + i, '"') + 1, 50);
                  if(*(lp_downloads + strlen(lp_downloads) - 1) == '"')
                    *(lp_downloads + strlen(lp_downloads) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_email", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_email, strchr(line + i, '"') + 1, 50);
                  if(*(lp_email + strlen(lp_email) - 1) == '"')
                    *(lp_email + strlen(lp_email) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_entryalreadyonlist", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_entryalreadyonlist, strchr(line + i, '"') + 1, 50);
                  if(*(lp_entryalreadyonlist + strlen(lp_entryalreadyonlist) - 1) == '"')
                    *(lp_entryalreadyonlist + strlen(lp_entryalreadyonlist) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_entrywasntfound", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_entrywasntfound, strchr(line + i, '"') + 1, 50);
                  if(*(lp_entrywasntfound + strlen(lp_entrywasntfound) - 1) == '"')
                    *(lp_entrywasntfound + strlen(lp_entrywasntfound) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_erasemsgs", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_erasemsgs, strchr(line + i, '"') + 1, 100);
                  if(*(lp_erasemsgs + strlen(lp_erasemsgs) - 1) == '"')
                    *(lp_erasemsgs + strlen(lp_erasemsgs) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_expires_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_expires_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_expires_ + strlen(lp_expires_) - 1) == '"')
                    *(lp_expires_ + strlen(lp_expires_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_expiresin", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_expiresin, strchr(line + i, '"') + 1, 50);
                  if(*(lp_expiresin + strlen(lp_expiresin) - 1) == '"')
                    *(lp_expiresin + strlen(lp_expiresin) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_fakeshare", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_fakeshare, strchr(line + i, '"') + 1, 50);
                  if(*(lp_fakeshare + strlen(lp_fakeshare) - 1) == '"')
                    *(lp_fakeshare + strlen(lp_fakeshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_faketag", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_faketag, strchr(line + i, '"') + 1, 50);
                  if(*(lp_faketag + strlen(lp_faketag) - 1) == '"')
                    *(lp_faketag + strlen(lp_faketag) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_favsend", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_favsend, strchr(line + i, '"') + 1, 100);
                  if(*(lp_favsend + strlen(lp_favsend) - 1) == '"')
                    *(lp_favsend + strlen(lp_favsend) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_file_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_file_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_file_ + strlen(lp_file_) - 1) == '"')
                    *(lp_file_ + strlen(lp_file_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_flooding", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_flooding, strchr(line + i, '"') + 1, 50);
                  if(*(lp_flooding + strlen(lp_flooding) - 1) == '"')
                    *(lp_flooding + strlen(lp_flooding) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_floodwarn", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_floodwarn, strchr(line + i, '"') + 1, 100);
                  if(*(lp_floodwarn + strlen(lp_floodwarn) - 1) == '"')
                    *(lp_floodwarn + strlen(lp_floodwarn) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_forbchar", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_forbchar, strchr(line + i, '"') + 1, 100);
                  if(*(lp_forbchar + strlen(lp_forbchar) - 1) == '"')
                    *(lp_forbchar + strlen(lp_forbchar) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_forbiddenclient", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_forbiddenclient, strchr(line + i, '"') + 1, 50);
                  if(*(lp_forbiddenclient + strlen(lp_forbiddenclient) - 1) == '"')
                    *(lp_forbiddenclient + strlen(lp_forbiddenclient) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_fornext", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_fornext, strchr(line + i, '"') + 1, 50);
                  if(*(lp_fornext + strlen(lp_fornext) - 1) == '"')
                    *(lp_fornext + strlen(lp_fornext) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_forother", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_forother, strchr(line + i, '"') + 1, 100);
                  if(*(lp_forother + strlen(lp_forother) - 1) == '"')
                    *(lp_forother + strlen(lp_forother) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_freeslots", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_freeslots, strchr(line + i, '"') + 1, 100);
                  if(*(lp_freeslots + strlen(lp_freeslots) - 1) == '"')
                    *(lp_freeslots + strlen(lp_freeslots) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_from_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_from_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_from_ + strlen(lp_from_) - 1) == '"')
                    *(lp_from_ + strlen(lp_from_) - 1) = '\0';
               }             
	     else if(strncmp(line + i, "lp_fulfilment", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_fulfilment, strchr(line + i, '"') + 1, 50);
                  if(*(lp_fulfilment + strlen(lp_fulfilment) - 1) == '"')
                    *(lp_fulfilment + strlen(lp_fulfilment) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_gaggedfrom", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_gaggedfrom, strchr(line + i, '"') + 1, 50);
                  if(*(lp_gaggedfrom + strlen(lp_gaggedfrom) - 1) == '"')
                    *(lp_gaggedfrom + strlen(lp_gaggedfrom) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_gaggingall", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_gaggingall, strchr(line + i, '"') + 1, 100);
                  if(*(lp_gaggingall + strlen(lp_gaggingall) - 1) == '"')
                    *(lp_gaggingall + strlen(lp_gaggingall) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_gagminshare", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_gagminshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_gagminshare + strlen(lp_gagminshare) - 1) == '"')
                    *(lp_gagminshare + strlen(lp_gagminshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_givingtempop", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_givingtempop, strchr(line + i, '"') + 1, 50);
                  if(*(lp_givingtempop + strlen(lp_givingtempop) - 1) == '"')
                    *(lp_givingtempop + strlen(lp_givingtempop) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hasparted", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hasparted, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hasparted + strlen(lp_hasparted) - 1) == '"')
                    *(lp_hasparted + strlen(lp_hasparted) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_haveregistertochat", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_haveregistertochat, strchr(line + i, '"') + 1, 100);
                  if(*(lp_haveregistertochat + strlen(lp_haveregistertochat) - 1) == '"')
                    *(lp_haveregistertochat + strlen(lp_haveregistertochat) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hi_", 6) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hi_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hi_ + strlen(lp_hi_) - 1) == '"')
                    *(lp_hi_ + strlen(lp_hi_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hidingpresence", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hidingpresence, strchr(line + i, '"') + 1, 100);
                  if(*(lp_hidingpresence + strlen(lp_hidingpresence) - 1) == '"')
                    *(lp_hidingpresence + strlen(lp_hidingpresence) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hidingshare", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hidingshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_hidingshare + strlen(lp_hidingshare) - 1) == '"')
                    *(lp_hidingshare + strlen(lp_hidingshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hours", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hours, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hours + strlen(lp_hours) - 1) == '"')
                    *(lp_hours + strlen(lp_hours) - 1) = '\0';
               }

             else if(strncmp(line + i, "lp_hubaddr", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubaddr, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hubaddr + strlen(lp_hubaddr) - 1) == '"')
                    *(lp_hubaddr + strlen(lp_hubaddr) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubdesc", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubdesc, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hubdesc + strlen(lp_hubdesc) - 1) == '"')
                    *(lp_hubdesc + strlen(lp_hubdesc) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubforregistered", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubforregistered, strchr(line + i, '"') + 1, 200);
                  if(*(lp_hubforregistered + strlen(lp_hubforregistered) - 1) == '"')
                    *(lp_hubforregistered + strlen(lp_hubforregistered) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubinfo", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubinfo, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hubinfo + strlen(lp_hubinfo) - 1) == '"')
                    *(lp_hubinfo + strlen(lp_hubinfo) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubkilled", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubkilled, strchr(line + i, '"') + 1, 100);
                  if(*(lp_hubkilled + strlen(lp_hubkilled) - 1) == '"')
                    *(lp_hubkilled + strlen(lp_hubkilled) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubmassmsg", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubmassmsg, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hubmassmsg + strlen(lp_hubmassmsg) - 1) == '"')
                    *(lp_hubmassmsg + strlen(lp_hubmassmsg) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubname", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubname, strchr(line + i, '"') + 1, 20);
                  if(*(lp_hubname + strlen(lp_hubname) - 1) == '"')
                    *(lp_hubname + strlen(lp_hubname) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubs_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubs_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hubs_ + strlen(lp_hubs_) - 1) == '"')
                    *(lp_hubs_ + strlen(lp_hubs_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_hubsperslot", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_hubsperslot, strchr(line + i, '"') + 1, 50);
                  if(*(lp_hubsperslot + strlen(lp_hubsperslot) - 1) == '"')
                    *(lp_hubsperslot + strlen(lp_hubsperslot) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_ipban_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_ipban_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_ipban_ + strlen(lp_ipban_) - 1) == '"')
                    *(lp_ipban_ + strlen(lp_ipban_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_ipbans", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_ipbans, strchr(line + i, '"') + 1, 50);
                  if(*(lp_ipbans + strlen(lp_ipbans) - 1) == '"')
                    *(lp_ipbans + strlen(lp_ipbans) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_is_", 6) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_is_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_is_ + strlen(lp_is_) - 1) == '"')
                    *(lp_is_ + strlen(lp_is_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_isalreadyopped", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_isalreadyopped, strchr(line + i, '"') + 1, 50);
                  if(*(lp_isalreadyopped + strlen(lp_isalreadyopped) - 1) == '"')
                    *(lp_isalreadyopped + strlen(lp_isalreadyopped) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_isaway", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_isaway, strchr(line + i, '"') + 1, 50);
                  if(*(lp_isaway + strlen(lp_isaway) - 1) == '"')
                    *(lp_isaway + strlen(lp_isaway) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_isback", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_isback, strchr(line + i, '"') + 1, 50);
                  if(*(lp_isback + strlen(lp_isback) - 1) == '"')
                    *(lp_isback + strlen(lp_isback) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_isbanning", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_isbanning, strchr(line + i, '"') + 1, 50);
                  if(*(lp_isbanning + strlen(lp_isbanning) - 1) == '"')
                    *(lp_isbanning + strlen(lp_isbanning) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_iskicking", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_iskicking, strchr(line + i, '"') + 1, 50);
                  if(*(lp_iskicking + strlen(lp_iskicking) - 1) == '"')
                    *(lp_iskicking + strlen(lp_iskicking) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_isoffline", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_isoffline, strchr(line + i, '"') + 1, 50);
                  if(*(lp_isoffline + strlen(lp_isoffline) - 1) == '"')
                    *(lp_isoffline + strlen(lp_isoffline) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_isonlinenow", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_isonlinenow, strchr(line + i, '"') + 1, 100);
                  if(*(lp_isonlinenow + strlen(lp_isonlinenow) - 1) == '"')
                    *(lp_isonlinenow + strlen(lp_isonlinenow) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_itemstotal", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_itemstotal, strchr(line + i, '"') + 1, 50);
                  if(*(lp_itemstotal + strlen(lp_itemstotal) - 1) == '"')
                    *(lp_itemstotal + strlen(lp_itemstotal) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_joins", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_joins, strchr(line + i, '"') + 1, 50);
                  if(*(lp_joins + strlen(lp_joins) - 1) == '"')
                    *(lp_joins + strlen(lp_joins) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_kick_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_kick_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_kick_ + strlen(lp_kick_) - 1) == '"')
                    *(lp_kick_ + strlen(lp_kick_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_kickdeny", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_kickdeny, strchr(line + i, '"') + 1, 100);
                  if(*(lp_kickdeny + strlen(lp_kickdeny) - 1) == '"')
                    *(lp_kickdeny + strlen(lp_kickdeny) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_kickedby", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_kickedby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_kickedby + strlen(lp_kickedby) - 1) == '"')
                    *(lp_kickedby + strlen(lp_kickedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_kvips_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_kvips_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_kvips_ + strlen(lp_kvips_) - 1) == '"')
                    *(lp_kvips_ + strlen(lp_kvips_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lackofprefix", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lackofprefix, strchr(line + i, '"') + 1, 100);
                  if(*(lp_lackofprefix + strlen(lp_lackofprefix) - 1) == '"')
                    *(lp_lackofprefix + strlen(lp_lackofprefix) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lackoftag", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lackoftag, strchr(line + i, '"') + 1, 50);
                  if(*(lp_lackoftag + strlen(lp_lackoftag) - 1) == '"')
                    *(lp_lackoftag + strlen(lp_lackoftag) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_langpackfound", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_langpackfound, strchr(line + i, '"') + 1, 100);
                  if(*(lp_langpackfound + strlen(lp_langpackfound) - 1) == '"')
                    *(lp_langpackfound + strlen(lp_langpackfound) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lastmainmsgs", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lastmainmsgs, strchr(line + i, '"') + 1, 50);
                  if(*(lp_lastmainmsgs + strlen(lp_lastmainmsgs) - 1) == '"')
                    *(lp_lastmainmsgs + strlen(lp_lastmainmsgs) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lastpermbans", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lastpermbans, strchr(line + i, '"') + 1, 50);
                  if(*(lp_lastpermbans + strlen(lp_lastpermbans) - 1) == '"')
                    *(lp_lastpermbans + strlen(lp_lastpermbans) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lastsearch", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lastsearch, strchr(line + i, '"') + 1, 50);
                  if(*(lp_lastsearch + strlen(lp_lastsearch) - 1) == '"')
                    *(lp_lastsearch + strlen(lp_lastsearch) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lastsendpm", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lastsendpm, strchr(line + i, '"') + 1, 50);
                  if(*(lp_lastsendpm + strlen(lp_lastsendpm) - 1) == '"')
                    *(lp_lastsendpm + strlen(lp_lastsendpm) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lasttempbans", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lasttempbans, strchr(line + i, '"') + 1, 50);
                  if(*(lp_lasttempbans + strlen(lp_lasttempbans) - 1) == '"')
                    *(lp_lasttempbans + strlen(lp_lasttempbans) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_lastwritetomain", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_lastwritetomain, strchr(line + i, '"') + 1, 50);
                  if(*(lp_lastwritetomain + strlen(lp_lastwritetomain) - 1) == '"')
                    *(lp_lastwritetomain + strlen(lp_lastwritetomain) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_listofgagged", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_listofgagged, strchr(line + i, '"') + 1, 100);
                  if(*(lp_listofgagged + strlen(lp_listofgagged) - 1) == '"')
                    *(lp_listofgagged + strlen(lp_listofgagged) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_loggedin", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_loggedin, strchr(line + i, '"') + 1, 50);
                  if(*(lp_loggedin + strlen(lp_loggedin) - 1) == '"')
                    *(lp_loggedin + strlen(lp_loggedin) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_longestconnected", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_longestconnected, strchr(line + i, '"') + 1, 100);
                  if(*(lp_longestconnected + strlen(lp_longestconnected) - 1) == '"')
                    *(lp_longestconnected + strlen(lp_longestconnected) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_mailtoolong", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_mailtoolong, strchr(line + i, '"') + 1, 100);
                  if(*(lp_mailtoolong + strlen(lp_mailtoolong) - 1) == '"')
                    *(lp_mailtoolong + strlen(lp_mailtoolong) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_mainchatlocked", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_mainchatlocked, strchr(line + i, '"') + 1, 50);
                  if(*(lp_mainchatlocked + strlen(lp_mainchatlocked) - 1) == '"')
                    *(lp_mainchatlocked + strlen(lp_mainchatlocked) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_mainchatmsgs", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_mainchatmsgs, strchr(line + i, '"') + 1, 50);
                  if(*(lp_mainchatmsgs + strlen(lp_mainchatmsgs) - 1) == '"')
                    *(lp_mainchatmsgs + strlen(lp_mainchatmsgs) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_mainchatoff", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_mainchatoff, strchr(line + i, '"') + 1, 100);
                  if(*(lp_mainchatoff + strlen(lp_mainchatoff) - 1) == '"')
                    *(lp_mainchatoff + strlen(lp_mainchatoff) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_mainchaton", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_mainchaton, strchr(line + i, '"') + 1, 100);
                  if(*(lp_mainchaton + strlen(lp_mainchaton) - 1) == '"')
                    *(lp_mainchaton + strlen(lp_mainchaton) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_masssend", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_masssend, strchr(line + i, '"') + 1, 100);
                  if(*(lp_masssend + strlen(lp_masssend) - 1) == '"')
                    *(lp_masssend + strlen(lp_masssend) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_maxhubs", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_maxhubs, strchr(line + i, '"') + 1, 50);
                  if(*(lp_maxhubs + strlen(lp_maxhubs) - 1) == '"')
                    *(lp_maxhubs + strlen(lp_maxhubs) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_maxshare", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_maxshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_maxshare + strlen(lp_maxshare) - 1) == '"')
                    *(lp_maxshare + strlen(lp_maxshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_maxslots", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_maxslots, strchr(line + i, '"') + 1, 50);
                  if(*(lp_maxslots + strlen(lp_maxslots) - 1) == '"')
                    *(lp_maxslots + strlen(lp_maxslots) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_maxusers", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_maxusers, strchr(line + i, '"') + 1, 50);
                  if(*(lp_maxusers + strlen(lp_maxusers) - 1) == '"')
                    *(lp_maxusers + strlen(lp_maxusers) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_message_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_message_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_message_ + strlen(lp_message_) - 1) == '"')
                    *(lp_message_ + strlen(lp_message_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minactshare", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minactshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_minactshare + strlen(lp_minactshare) - 1) == '"')
                    *(lp_minactshare + strlen(lp_minactshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_mindcppver", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_mindcppver, strchr(line + i, '"') + 1, 50);
                  if(*(lp_mindcppver + strlen(lp_mindcppver) - 1) == '"')
                    *(lp_mindcppver + strlen(lp_mindcppver) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minkvipshare", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minkvipshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_minkvipshare + strlen(lp_minkvipshare) - 1) == '"')
                    *(lp_minkvipshare + strlen(lp_minkvipshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minopshare", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minopshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_minopshare + strlen(lp_minopshare) - 1) == '"')
                    *(lp_minopshare + strlen(lp_minopshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minpsvshare", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minpsvshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_minpsvshare + strlen(lp_minpsvshare) - 1) == '"')
                    *(lp_minpsvshare + strlen(lp_minpsvshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minregshare", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minregshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_minregshare + strlen(lp_minregshare) - 1) == '"')
                    *(lp_minregshare + strlen(lp_minregshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minshare", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_minshare + strlen(lp_minshare) - 1) == '"')
                    *(lp_minshare + strlen(lp_minshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minslots", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minslots, strchr(line + i, '"') + 1, 50);
                  if(*(lp_minslots + strlen(lp_minslots) - 1) == '"')
                    *(lp_minslots + strlen(lp_minslots) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minvipshare", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minvipshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_minvipshare + strlen(lp_minvipshare) - 1) == '"')
                    *(lp_minvipshare + strlen(lp_minvipshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minslots", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minslots, strchr(line + i, '"') + 1, 30);
                  if(*(lp_minslots + strlen(lp_minslots) - 1) == '"')
                    *(lp_minslots + strlen(lp_minslots) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_minutes", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_minutes, strchr(line + i, '"') + 1, 50);
                  if(*(lp_minutes + strlen(lp_minutes) - 1) == '"')
                    *(lp_minutes + strlen(lp_minutes) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_mode_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_mode_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_mode_ + strlen(lp_mode_) - 1) == '"')
                    *(lp_mode_ + strlen(lp_mode_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_msgforyou", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_msgforyou, strchr(line + i, '"') + 1, 100);
                  if(*(lp_msgforyou + strlen(lp_msgforyou) - 1) == '"')
                    *(lp_msgforyou + strlen(lp_msgforyou) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_musthavedot", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_musthavedot, strchr(line + i, '"') + 1, 100);
                  if(*(lp_musthavedot + strlen(lp_musthavedot) - 1) == '"')
                    *(lp_musthavedot + strlen(lp_musthavedot) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_name_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_name_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_name_ + strlen(lp_name_) - 1) == '"')
                    *(lp_name_ + strlen(lp_name_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_never_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_never_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_never_ + strlen(lp_never_) - 1) == '"')
                    *(lp_never_ + strlen(lp_never_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_newcommand", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_newcommand, strchr(line + i, '"') + 1, 100);
                  if(*(lp_newcommand + strlen(lp_newcommand) - 1) == '"')
                    *(lp_newcommand + strlen(lp_newcommand) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_newregistered", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_newregistered, strchr(line + i, '"') + 1, 100);
                  if(*(lp_newregistered + strlen(lp_newregistered) - 1) == '"')
                    *(lp_newregistered + strlen(lp_newregistered) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nextwarningban", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nextwarningban, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nextwarningban + strlen(lp_nextwarningban) - 1) == '"')
                    *(lp_nextwarningban + strlen(lp_nextwarningban) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nick_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nick_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_nick_ + strlen(lp_nick_) - 1) == '"')
                    *(lp_nick_ + strlen(lp_nick_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nickban_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nickban_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_nickban_ + strlen(lp_nickban_) - 1) == '"')
                    *(lp_nickban_ + strlen(lp_nickban_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nickbans", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nickbans, strchr(line + i, '"') + 1, 50);
                  if(*(lp_nickbans + strlen(lp_nickbans) - 1) == '"')
                    *(lp_nickbans + strlen(lp_nickbans) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nickchar", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nickchar, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nickchar + strlen(lp_nickchar) - 1) == '"')
                    *(lp_nickchar + strlen(lp_nickchar) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nickinuse", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nickinuse, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nickinuse + strlen(lp_nickinuse) - 1) == '"')
                    *(lp_nickinuse + strlen(lp_nickinuse) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nickname", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nickname, strchr(line + i, '"') + 1, 50);
                  if(*(lp_nickname + strlen(lp_nickname) - 1) == '"')
                    *(lp_nickname + strlen(lp_nickname) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nicktooshort", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nicktooshort, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nicktooshort + strlen(lp_nicktooshort) - 1) == '"')
                    *(lp_nicktooshort + strlen(lp_nicktooshort) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nomsgtodelete", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nomsgtodelete, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nomsgtodelete + strlen(lp_nomsgtodelete) - 1) == '"')
                    *(lp_nomsgtodelete + strlen(lp_nomsgtodelete) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nonewlines", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nonewlines, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nonewlines + strlen(lp_nonewlines) - 1) == '"')
                    *(lp_nonewlines + strlen(lp_nonewlines) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nonewmsg", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nonewmsg, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nonewmsg + strlen(lp_nonewmsg) - 1) == '"')
                    *(lp_nonewmsg + strlen(lp_nonewmsg) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nopass", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nopass, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nopass + strlen(lp_nopass) - 1) == '"')
                    *(lp_nopass + strlen(lp_nopass) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nopermission", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nopermission, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nopermission + strlen(lp_nopermission) - 1) == '"')
                    *(lp_nopermission + strlen(lp_nopermission) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_normal_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_normal_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_normal_ + strlen(lp_normal_) - 1) == '"')
                    *(lp_normal_ + strlen(lp_normal_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nosuchfile", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nosuchfile, strchr(line + i, '"') + 1, 50);
                  if(*(lp_nosuchfile + strlen(lp_nosuchfile) - 1) == '"')
                    *(lp_nosuchfile + strlen(lp_nosuchfile) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nosuchvariable", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nosuchvariable, strchr(line + i, '"') + 1, 50);
                  if(*(lp_nosuchvariable + strlen(lp_nosuchvariable) - 1) == '"')
                    *(lp_nosuchvariable + strlen(lp_nosuchvariable) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_notallowedtoadd", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_notallowedtoadd, strchr(line + i, '"') + 1, 100);
                  if(*(lp_notallowedtoadd + strlen(lp_notallowedtoadd) - 1) == '"')
                    *(lp_notallowedtoadd + strlen(lp_notallowedtoadd) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_notallowedtochat", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_notallowedtochat, strchr(line + i, '"') + 1, 100);
                  if(*(lp_notallowedtochat + strlen(lp_notallowedtochat) - 1) == '"')
                    *(lp_notallowedtochat + strlen(lp_notallowedtochat) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_notallowedtodelreg", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_notallowedtodelreg, strchr(line + i, '"') + 1, 100);
                  if(*(lp_notallowedtodelreg + strlen(lp_notallowedtodelreg) - 1) == '"')
                    *(lp_notallowedtodelreg + strlen(lp_notallowedtodelreg) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_notallowedtosee", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_notallowedtosee, strchr(line + i, '"') + 1, 100);
                  if(*(lp_notallowedtosee + strlen(lp_notallowedtosee) - 1) == '"')
                    *(lp_notallowedtosee + strlen(lp_notallowedtosee) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_notenoughslots", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_notenoughslots, strchr(line + i, '"') + 1, 100);
                  if(*(lp_notenoughslots + strlen(lp_notenoughslots) - 1) == '"')
                    *(lp_notenoughslots + strlen(lp_notenoughslots) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_nowarnings", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_nowarnings, strchr(line + i, '"') + 1, 100);
                  if(*(lp_nowarnings + strlen(lp_nowarnings) - 1) == '"')
                    *(lp_nowarnings + strlen(lp_nowarnings) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_number_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_number_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_number_ + strlen(lp_number_) - 1) == '"')
                    *(lp_number_ + strlen(lp_number_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_opped_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_opped_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_opped_ + strlen(lp_opped_) - 1) == '"')
                    *(lp_opped_ + strlen(lp_opped_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_ops_", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_ops_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_ops_ + strlen(lp_ops_) - 1) == '"')
                    *(lp_ops_ + strlen(lp_ops_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_owners_", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_owners_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_owners_ + strlen(lp_owners_) - 1) == '"')
                    *(lp_owners_ + strlen(lp_owners_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_pass_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_pass_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_pass_ + strlen(lp_pass_) - 1) == '"')
                    *(lp_pass_ + strlen(lp_pass_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_passchanged", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_passchanged, strchr(line + i, '"') + 1, 50);
                  if(*(lp_passchanged + strlen(lp_passchanged) - 1) == '"')
                    *(lp_passchanged + strlen(lp_passchanged) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_passive_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_passive_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_passive_ + strlen(lp_passive_) - 1) == '"')
                    *(lp_passive_ + strlen(lp_passive_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_passwordincorrect", 20) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_passwordincorrect, strchr(line + i, '"') + 1, 100);
                  if(*(lp_passwordincorrect + strlen(lp_passwordincorrect) - 1) == '"')
                    *(lp_passwordincorrect + strlen(lp_passwordincorrect) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_passwordrequired", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_passwordrequired, strchr(line + i, '"') + 1, 100);
                  if(*(lp_passwordrequired + strlen(lp_passwordrequired) - 1) == '"')
                    *(lp_passwordrequired + strlen(lp_passwordrequired) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_permanent_", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_permanent_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_permanent_ + strlen(lp_permanent_) - 1) == '"')
                    *(lp_permanent_ + strlen(lp_permanent_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_permanently", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_permanently, strchr(line + i, '"') + 1, 50);
                  if(*(lp_permanently + strlen(lp_permanently) - 1) == '"')
                    *(lp_permanently + strlen(lp_permanently) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_permbans", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_permbans, strchr(line + i, '"') + 1, 50);
                  if(*(lp_permbans + strlen(lp_permbans) - 1) == '"')
                    *(lp_permbans + strlen(lp_permbans) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_pmmsgs", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_pmmsgs, strchr(line + i, '"') + 1, 50);
                  if(*(lp_pmmsgs + strlen(lp_pmmsgs) - 1) == '"')
                    *(lp_pmmsgs + strlen(lp_pmmsgs) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_pmon", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_pmon, strchr(line + i, '"') + 1, 100);
                  if(*(lp_pmon + strlen(lp_pmon) - 1) == '"')
                    *(lp_pmon + strlen(lp_pmon) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_pmoff", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_pmoff, strchr(line + i, '"') + 1, 100);
                  if(*(lp_pmoff + strlen(lp_pmoff) - 1) == '"')
                    *(lp_pmoff + strlen(lp_pmoff) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_presencealreadyhidden", 24) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_presencealreadyhidden, strchr(line + i, '"') + 1, 100);
                  if(*(lp_presencealreadyhidden + strlen(lp_presencealreadyhidden) - 1) == '"')
                    *(lp_presencealreadyhidden + strlen(lp_presencealreadyhidden) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_presencehidden", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_presencehidden, strchr(line + i, '"') + 1, 100);
                  if(*(lp_presencehidden + strlen(lp_presencehidden) - 1) == '"')
                    *(lp_presencehidden + strlen(lp_presencehidden) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_protectmepass", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_protectmepass, strchr(line + i, '"') + 1, 100);
                  if(*(lp_protectmepass + strlen(lp_protectmepass) - 1) == '"')
                    *(lp_protectmepass + strlen(lp_protectmepass) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_prots_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_prots_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_prots_ + strlen(lp_prots_) - 1) == '"')
                    *(lp_prots_ + strlen(lp_prots_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_prottime", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_prottime, strchr(line + i, '"') + 1, 50);
                  if(*(lp_prottime + strlen(lp_prottime) - 1) == '"')
                    *(lp_prottime + strlen(lp_prottime) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_prottype", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_prottype, strchr(line + i, '"') + 1, 50);
                  if(*(lp_prottype + strlen(lp_prottype) - 1) == '"')
                    *(lp_prottype + strlen(lp_prottype) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_proxy_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_proxy_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_proxy_ + strlen(lp_proxy_) - 1) == '"')
                    *(lp_proxy_ + strlen(lp_proxy_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_proxynotallowed", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_proxynotallowed, strchr(line + i, '"') + 1, 50);
                  if(*(lp_proxynotallowed + strlen(lp_proxynotallowed) - 1) == '"')
                    *(lp_proxynotallowed + strlen(lp_proxynotallowed) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_psvnotallowed", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_psvnotallowed, strchr(line + i, '"') + 1, 50);
                  if(*(lp_psvnotallowed + strlen(lp_psvnotallowed) - 1) == '"')
                    *(lp_psvnotallowed + strlen(lp_psvnotallowed) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_psvwithmodea", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_psvwithmodea, strchr(line + i, '"') + 1, 100);
                  if(*(lp_psvwithmodea + strlen(lp_psvwithmodea) - 1) == '"')
                    *(lp_psvwithmodea + strlen(lp_psvwithmodea) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_psvwithoutpsvinnick", 22) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_psvwithoutpsvinnick, strchr(line + i, '"') + 1, 100);
                  if(*(lp_psvwithoutpsvinnick + strlen(lp_psvwithoutpsvinnick) - 1) == '"')
                    *(lp_psvwithoutpsvinnick + strlen(lp_psvwithoutpsvinnick) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_quits", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_quits, strchr(line + i, '"') + 1, 50);
                  if(*(lp_quits + strlen(lp_quits) - 1) == '"')
                    *(lp_quits + strlen(lp_quits) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_rawadm", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_rawadm, strchr(line + i, '"') + 1, 50);
                  if(*(lp_rawadm + strlen(lp_rawadm) - 1) == '"')
                    *(lp_rawadm + strlen(lp_rawadm) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_rawop", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_rawop, strchr(line + i, '"') + 1, 50);
                  if(*(lp_rawop + strlen(lp_rawop) - 1) == '"')
                    *(lp_rawop + strlen(lp_rawop) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_rawuser", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_rawuser, strchr(line + i, '"') + 1, 50);
                  if(*(lp_rawuser + strlen(lp_rawuser) - 1) == '"')
                    *(lp_rawuser + strlen(lp_rawuser) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_reason", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_reason, strchr(line + i, '"') + 1, 50);
                  if(*(lp_reason + strlen(lp_reason) - 1) == '"')
                    *(lp_reason + strlen(lp_reason) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_recentlyadded", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_recentlyadded, strchr(line + i, '"') + 1, 50);
                  if(*(lp_recentlyadded + strlen(lp_recentlyadded) - 1) == '"')
                    *(lp_recentlyadded + strlen(lp_recentlyadded) - 1) = '\0';
               }

             else if(strncmp(line + i, "lp_recordresetted", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_recordresetted, strchr(line + i, '"') + 1, 50);
                  if(*(lp_recordresetted + strlen(lp_recordresetted) - 1) == '"')
                    *(lp_recordresetted + strlen(lp_recordresetted) - 1) = '\0';
               }

             else if(strncmp(line + i, "lp_recordshare", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_recordshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_recordshare + strlen(lp_recordshare) - 1) == '"')
                    *(lp_recordshare + strlen(lp_recordshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_recordusers", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_recordusers, strchr(line + i, '"') + 1, 100);
                  if(*(lp_recordusers + strlen(lp_recordusers) - 1) == '"')
                    *(lp_recordusers + strlen(lp_recordusers) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_redir_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_redir_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_redir_ + strlen(lp_redir_) - 1) == '"')
                    *(lp_redir_ + strlen(lp_redir_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_redirectingall", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_redirectingall, strchr(line + i, '"') + 1, 100);
                  if(*(lp_redirectingall + strlen(lp_redirectingall) - 1) == '"')
                    *(lp_redirectingall + strlen(lp_redirectingall) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_registered_", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_registered_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_registered_ + strlen(lp_registered_) - 1) == '"')
                    *(lp_registered_ + strlen(lp_registered_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_registeredby", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_registeredby, strchr(line + i, '"') + 1, 100);
                  if(*(lp_registeredby + strlen(lp_registeredby) - 1) == '"')
                    *(lp_registeredby + strlen(lp_registeredby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_registrationfailed", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_registrationfailed, strchr(line + i, '"') + 1, 100);
                  if(*(lp_registrationfailed + strlen(lp_registrationfailed) - 1) == '"')
                    *(lp_registrationfailed + strlen(lp_registrationfailed) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_registrationrules", 20) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_registrationrules, strchr(line + i, '"') + 1, 50);
                  if(*(lp_registrationrules + strlen(lp_registrationrules) - 1) == '"')
                    *(lp_registrationrules + strlen(lp_registrationrules) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_regmepass", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_regmepass, strchr(line + i, '"') + 1, 100);
                  if(*(lp_regmepass + strlen(lp_regmepass) - 1) == '"')
                    *(lp_regmepass + strlen(lp_regmepass) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_regonly", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_regonly, strchr(line + i, '"') + 1, 50);
                  if(*(lp_regonly + strlen(lp_regonly) - 1) == '"')
                    *(lp_regonly + strlen(lp_regonly) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_regs_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_regs_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_regs_ + strlen(lp_regs_) - 1) == '"')
                    *(lp_regs_ + strlen(lp_regs_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_regular_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_regular_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_regular_ + strlen(lp_regular_) - 1) == '"')
                    *(lp_regular_ + strlen(lp_regular_) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_releases", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_releases, strchr(line + i, '"') + 1, 50);
                  if(*(lp_releases + strlen(lp_releases) - 1) == '"')
                    *(lp_releases + strlen(lp_releases) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_reloadingscripts", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_reloadingscripts, strchr(line + i, '"') + 1, 50);
                  if(*(lp_reloadingscripts + strlen(lp_reloadingscripts) - 1) == '"')
                    *(lp_reloadingscripts + strlen(lp_reloadingscripts) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_removedallwarns", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_removedallwarns, strchr(line + i, '"') + 1, 100);
                  if(*(lp_removedallwarns + strlen(lp_removedallwarns) - 1) == '"')
                    *(lp_removedallwarns + strlen(lp_removedallwarns) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_removedentryfrom", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_removedentryfrom, strchr(line + i, '"') + 1, 50);
                  if(*(lp_removedentryfrom + strlen(lp_removedentryfrom) - 1) == '"')
                    *(lp_removedentryfrom + strlen(lp_removedentryfrom) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_reportedby", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_reportedby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_reportedby + strlen(lp_reportedby) - 1) == '"')
                    *(lp_reportedby + strlen(lp_reportedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_reportsend", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_reportsend, strchr(line + i, '"') + 1, 50);
                  if(*(lp_reportsend + strlen(lp_reportsend) - 1) == '"')
                    *(lp_reportsend + strlen(lp_reportsend) - 1) = '\0';
               }

            else if(strncmp(line + i, "lp_requests", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_requests, strchr(line + i, '"') + 1, 50);
                  if(*(lp_requests + strlen(lp_requests) - 1) == '"')
                    *(lp_requests + strlen(lp_requests) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_requiredatleast", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_requiredatleast, strchr(line + i, '"') + 1, 100);
                  if(*(lp_requiredatleast + strlen(lp_requiredatleast) - 1) == '"')
                    *(lp_requiredatleast + strlen(lp_requiredatleast) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_restrictions1", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_restrictions1, strchr(line + i, '"') + 1, 200);
                  if(*(lp_restrictions1 + strlen(lp_restrictions1) - 1) == '"')
                    *(lp_restrictions1 + strlen(lp_restrictions1) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_restrictions2", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_restrictions2, strchr(line + i, '"') + 1, 200);
                  if(*(lp_restrictions2 + strlen(lp_restrictions2) - 1) == '"')
                    *(lp_restrictions2 + strlen(lp_restrictions2) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_savedmsg", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_savedmsg, strchr(line + i, '"') + 1, 100);
                  if(*(lp_savedmsg + strlen(lp_savedmsg) - 1) == '"')
                    *(lp_savedmsg + strlen(lp_savedmsg) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searches", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searches, strchr(line + i, '"') + 1, 50);
                  if(*(lp_searches + strlen(lp_searches) - 1) == '"')
                    *(lp_searches + strlen(lp_searches) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searchforip", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searchforip, strchr(line + i, '"') + 1, 100);
                  if(*(lp_searchforip + strlen(lp_searchforip) - 1) == '"')
                    *(lp_searchforip + strlen(lp_searchforip) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searchignored", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searchignored, strchr(line + i, '"') + 1, 100);
                  if(*(lp_searchignored + strlen(lp_searchignored) - 1) == '"')
                    *(lp_searchignored + strlen(lp_searchignored) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searching_", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searching_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_searching_ + strlen(lp_searching_) - 1) == '"')
                    *(lp_searching_ + strlen(lp_searching_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searchoff", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searchoff, strchr(line + i, '"') + 1, 100);
                  if(*(lp_searchoff + strlen(lp_searchoff) - 1) == '"')
                    *(lp_searchoff + strlen(lp_searchoff) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searchon", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searchon, strchr(line + i, '"') + 1, 100);
                  if(*(lp_searchon + strlen(lp_searchon) - 1) == '"')
                    *(lp_searchon + strlen(lp_searchon) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searchresults", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searchresults, strchr(line + i, '"') + 1, 100);
                  if(*(lp_searchresults + strlen(lp_searchresults) - 1) == '"')
                    *(lp_searchresults + strlen(lp_searchresults) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searchspam", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searchspam, strchr(line + i, '"') + 1, 50);
                  if(*(lp_searchspam + strlen(lp_searchspam) - 1) == '"')
                    *(lp_searchspam + strlen(lp_searchspam) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_searchtooshort", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_searchtooshort, strchr(line + i, '"') + 1, 100);
                  if(*(lp_searchtooshort + strlen(lp_searchtooshort) - 1) == '"')
                    *(lp_searchtooshort + strlen(lp_searchtooshort) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_seconds", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_seconds, strchr(line + i, '"') + 1, 51);
                  if(*(lp_seconds + strlen(lp_seconds) - 1) == '"')
                    *(lp_seconds + strlen(lp_seconds) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_selfregistrationdisabled", 27) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_selfregistrationdisabled, strchr(line + i, '"') + 1, 100);
                  if(*(lp_selfregistrationdisabled + strlen(lp_selfregistrationdisabled) - 1) == '"')
                    *(lp_selfregistrationdisabled + strlen(lp_selfregistrationdisabled) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_setby", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_setby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_setby + strlen(lp_setby) - 1) == '"')
                    *(lp_setby + strlen(lp_setby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_setto", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_setto, strchr(line + i, '"') + 1, 50);
                  if(*(lp_setto + strlen(lp_setto) - 1) == '"')
                    *(lp_setto + strlen(lp_setto) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_share_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_share_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_share_ + strlen(lp_share_) - 1) == '"')
                    *(lp_share_ + strlen(lp_share_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_sharesize", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_sharesize, strchr(line + i, '"') + 1, 50);
                  if(*(lp_sharesize + strlen(lp_sharesize) - 1) == '"')
                    *(lp_sharesize + strlen(lp_sharesize) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_shouldbechecked", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_shouldbechecked, strchr(line + i, '"') + 1, 100);
                  if(*(lp_shouldbechecked + strlen(lp_shouldbechecked) - 1) == '"')
                    *(lp_shouldbechecked + strlen(lp_shouldbechecked) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_since", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_since, strchr(line + i, '"') + 1, 51);
                  if(*(lp_since + strlen(lp_since) - 1) == '"')
                    *(lp_since + strlen(lp_since) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_slotbegging", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_slotbegging, strchr(line + i, '"') + 1, 100);
                  if(*(lp_slotbegging + strlen(lp_slotbegging) - 1) == '"')
                    *(lp_slotbegging + strlen(lp_slotbegging) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_slotratio", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_slotratio, strchr(line + i, '"') + 1, 50);
                  if(*(lp_slotratio + strlen(lp_slotratio) - 1) == '"')
                    *(lp_slotratio + strlen(lp_slotratio) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_slots_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_slots_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_slots_ + strlen(lp_slots_) - 1) == '"')
                    *(lp_slots_ + strlen(lp_slots_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_slotsperhub", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_slotsperhub, strchr(line + i, '"') + 1, 100);
                  if(*(lp_slotsperhub + strlen(lp_slotsperhub) - 1) == '"')
                    *(lp_slotsperhub + strlen(lp_slotsperhub) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_spacesinnick", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_spacesinnick, strchr(line + i, '"') + 1, 100);
                  if(*(lp_spacesinnick + strlen(lp_spacesinnick) - 1) == '"')
                    *(lp_spacesinnick + strlen(lp_spacesinnick) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_spamming", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_spamming, strchr(line + i, '"') + 1, 50);
                  if(*(lp_spamming + strlen(lp_spamming) - 1) == '"')
                    *(lp_spamming + strlen(lp_spamming) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_statistics", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_statistics, strchr(line + i, '"') + 1, 50);
                  if(*(lp_statistics + strlen(lp_statistics) - 1) == '"')
                    *(lp_statistics + strlen(lp_statistics) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_tag_", 7) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_tag_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_tag_ + strlen(lp_tag_) - 1) == '"')
                    *(lp_tag_ + strlen(lp_tag_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_tempbans", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_tempbans, strchr(line + i, '"') + 1, 50);
                  if(*(lp_tempbans + strlen(lp_tempbans) - 1) == '"')
                    *(lp_tempbans + strlen(lp_tempbans) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_tempkickban", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_tempkickban, strchr(line + i, '"') + 1, 50);
                  if(*(lp_tempkickban + strlen(lp_tempkickban) - 1) == '"')
                    *(lp_tempkickban + strlen(lp_tempkickban) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_temporary_", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_temporary_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_temporary_ + strlen(lp_temporary_) - 1) == '"')
                    *(lp_temporary_ + strlen(lp_temporary_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_text_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_text_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_text_ + strlen(lp_text_) - 1) == '"')
                    *(lp_text_ + strlen(lp_text_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_thesameip", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_thesameip, strchr(line + i, '"') + 1, 100);
                  if(*(lp_thesameip + strlen(lp_thesameip) - 1) == '"')
                    *(lp_thesameip + strlen(lp_thesameip) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_time_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_time_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_time_ + strlen(lp_time_) - 1) == '"')
                    *(lp_time_ + strlen(lp_time_) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_timetounban", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_timetounban, strchr(line + i, '"') + 1, 50);
                  if(*(lp_timetounban + strlen(lp_timetounban) - 1) == '"')
                    *(lp_timetounban + strlen(lp_timetounban) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_toohighratio", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_toohighratio, strchr(line + i, '"') + 1, 100);
                  if(*(lp_toohighratio + strlen(lp_toohighratio) - 1) == '"')
                    *(lp_toohighratio + strlen(lp_toohighratio) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_toolowratio", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_toolowratio, strchr(line + i, '"') + 1, 100);
                  if(*(lp_toolowratio + strlen(lp_toolowratio) - 1) == '"')
                    *(lp_toolowratio + strlen(lp_toolowratio) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_toomanyhubs", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_toomanyhubs, strchr(line + i, '"') + 1, 100);
                  if(*(lp_toomanyhubs + strlen(lp_toomanyhubs) - 1) == '"')
                    *(lp_toomanyhubs + strlen(lp_toomanyhubs) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_toomanyslots", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_toomanyslots, strchr(line + i, '"') + 1, 100);
                  if(*(lp_toomanyslots + strlen(lp_toomanyslots) - 1) == '"')
                    *(lp_toomanyslots + strlen(lp_toomanyslots) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_toomanywarnings", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_toomanywarnings, strchr(line + i, '"') + 1, 100);
                  if(*(lp_toomanywarnings + strlen(lp_toomanywarnings) - 1) == '"')
                    *(lp_toomanywarnings + strlen(lp_toomanywarnings) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_tooolddcppver", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_tooolddcppver, strchr(line + i, '"') + 1, 100);
                  if(*(lp_tooolddcppver + strlen(lp_tooolddcppver) - 1) == '"')
                    *(lp_tooolddcppver + strlen(lp_tooolddcppver) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_topicchangedby", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_topicchangedby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_topicchangedby + strlen(lp_topicchangedby) - 1) == '"')
                    *(lp_topicchangedby + strlen(lp_topicchangedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_topicis", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_topicis, strchr(line + i, '"') + 1, 50);
                  if(*(lp_topicis + strlen(lp_topicis) - 1) == '"')
                    *(lp_topicis + strlen(lp_topicis) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_topicnotset", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_topicnotset, strchr(line + i, '"') + 1, 50);
                  if(*(lp_topicnotset + strlen(lp_topicnotset) - 1) == '"')
                    *(lp_topicnotset + strlen(lp_topicnotset) - 1) = '\0';
               }
            else if(strncmp(line + i, "lp_topicunsetby", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_topicunsetby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_topicunsetby + strlen(lp_topicunsetby) - 1) == '"')
                    *(lp_topicunsetby + strlen(lp_topicunsetby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_toreleaselist", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_toreleaselist, strchr(line + i, '"') + 1, 200);
                  if(*(lp_toreleaselist + strlen(lp_toreleaselist) - 1) == '"')
                    *(lp_toreleaselist + strlen(lp_toreleaselist) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_toreqlist", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_toreqlist, strchr(line + i, '"') + 1, 200);
                  if(*(lp_toreqlist + strlen(lp_toreqlist) - 1) == '"')
                    *(lp_toreqlist + strlen(lp_toreqlist) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_total_", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_total_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_total_ + strlen(lp_total_) - 1) == '"')
                    *(lp_total_ + strlen(lp_total_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_totalvisitors", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_totalvisitors, strchr(line + i, '"') + 1, 51);
                  if(*(lp_totalvisitors + strlen(lp_totalvisitors) - 1) == '"')
                    *(lp_totalvisitors + strlen(lp_totalvisitors) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_type_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_type_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_type_ + strlen(lp_type_) - 1) == '"')
                    *(lp_type_ + strlen(lp_type_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_typehelp", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_typehelp, strchr(line + i, '"') + 1, 100);
                  if(*(lp_typehelp + strlen(lp_typehelp) - 1) == '"')
                    *(lp_typehelp + strlen(lp_typehelp) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_typeregme", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_typeregme, strchr(line + i, '"') + 1, 100);
                  if(*(lp_typeregme + strlen(lp_typeregme) - 1) == '"')
                    *(lp_typeregme + strlen(lp_typeregme) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_typerules", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_typerules, strchr(line + i, '"') + 1, 100);
                  if(*(lp_typerules + strlen(lp_typerules) - 1) == '"')
                    *(lp_typerules + strlen(lp_typerules) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_unfinishedfiles", 18) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_unfinishedfiles, strchr(line + i, '"') + 1, 100);
                  if(*(lp_unfinishedfiles + strlen(lp_unfinishedfiles) - 1) == '"')
                    *(lp_unfinishedfiles + strlen(lp_unfinishedfiles) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_ungaggingall", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_ungaggingall, strchr(line + i, '"') + 1, 100);
                  if(*(lp_ungaggingall + strlen(lp_ungaggingall) - 1) == '"')
                    *(lp_ungaggingall + strlen(lp_ungaggingall) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_ungagminshare", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_ungagminshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_ungagminshare + strlen(lp_ungagminshare) - 1) == '"')
                    *(lp_ungagminshare + strlen(lp_ungagminshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_unhidingpresence", 19) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_unhidingpresence, strchr(line + i, '"') + 1, 100);
                  if(*(lp_unhidingpresence + strlen(lp_unhidingpresence) - 1) == '"')
                    *(lp_unhidingpresence + strlen(lp_unhidingpresence) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_unhidingshare", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_unhidingshare, strchr(line + i, '"') + 1, 100);
                  if(*(lp_unhidingshare + strlen(lp_unhidingshare) - 1) == '"')
                    *(lp_unhidingshare + strlen(lp_unhidingshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_uploadlimittoolow", 20) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_uploadlimittoolow, strchr(line + i, '"') + 1, 50);
                  if(*(lp_uploadlimittoolow + strlen(lp_uploadlimittoolow) - 1) == '"')
                    *(lp_uploadlimittoolow + strlen(lp_uploadlimittoolow) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_uptime", 9) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_uptime, strchr(line + i, '"') + 1, 50);
                  if(*(lp_uptime + strlen(lp_uptime) - 1) == '"')
                    *(lp_uptime + strlen(lp_uptime) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_user_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_user_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_user_ + strlen(lp_user_) - 1) == '"')
                    *(lp_user_ + strlen(lp_user_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_userinfo", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_userinfo, strchr(line + i, '"') + 1, 50);
                  if(*(lp_userinfo + strlen(lp_userinfo) - 1) == '"')
                    *(lp_userinfo + strlen(lp_userinfo) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_users", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_users, strchr(line + i, '"') + 1, 50);
                  if(*(lp_users + strlen(lp_users) - 1) == '"')
                    *(lp_users + strlen(lp_users) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_usertype", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_usertype, strchr(line + i, '"') + 1, 50);
                  if(*(lp_usertype + strlen(lp_usertype) - 1) == '"')
                    *(lp_usertype + strlen(lp_usertype) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_value", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_value, strchr(line + i, '"') + 1, 50);
                  if(*(lp_value + strlen(lp_value) - 1) == '"')
                    *(lp_value + strlen(lp_value) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_variable", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_variable, strchr(line + i, '"') + 1, 50);
                  if(*(lp_variable + strlen(lp_variable) - 1) == '"')
                    *(lp_variable + strlen(lp_variable) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_version", 10) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_version, strchr(line + i, '"') + 1, 50);
                  if(*(lp_version + strlen(lp_version) - 1) == '"')
                    *(lp_version + strlen(lp_version) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_vips_", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_vips_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_vips_ + strlen(lp_vips_) - 1) == '"')
                    *(lp_vips_ + strlen(lp_vips_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_warning_", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_warning_, strchr(line + i, '"') + 1, 50);
                  if(*(lp_warning_ + strlen(lp_warning_) - 1) == '"')
                    *(lp_warning_ + strlen(lp_warning_) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wasaddedto", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasaddedto, strchr(line + i, '"') + 1, 50);
                  if(*(lp_wasaddedto + strlen(lp_wasaddedto) - 1) == '"')
                    *(lp_wasaddedto + strlen(lp_wasaddedto) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wasbannedby", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasbannedby, strchr(line + i, '"') + 1, 30);
                  if(*(lp_wasbannedby + strlen(lp_wasbannedby) - 1) == '"')
                    *(lp_wasbannedby + strlen(lp_wasbannedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wascleared", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wascleared, strchr(line + i, '"') + 1, 50);
                  if(*(lp_wascleared + strlen(lp_wascleared) - 1) == '"')
                    *(lp_wascleared + strlen(lp_wascleared) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wasdropped", 13) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasdropped, strchr(line + i, '"') + 1, 50);
                  if(*(lp_wasdropped + strlen(lp_wasdropped) - 1) == '"')
                    *(lp_wasdropped + strlen(lp_wasdropped) - 1) = '\0';
               }             
	     else if(strncmp(line + i, "lp_wasgaggedby", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasgaggedby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_wasgaggedby + strlen(lp_wasgaggedby) - 1) == '"')
                    *(lp_wasgaggedby + strlen(lp_wasgaggedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wasgaggedfrom", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasgaggedfrom, strchr(line + i, '"') + 1, 100);
                  if(*(lp_wasgaggedfrom + strlen(lp_wasgaggedfrom) - 1) == '"')
                    *(lp_wasgaggedfrom + strlen(lp_wasgaggedfrom) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_waskickedby", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_waskickedby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_waskickedby + strlen(lp_waskickedby) - 1) == '"')
                    *(lp_waskickedby + strlen(lp_waskickedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wasregistered", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasregistered, strchr(line + i, '"') + 1, 100);
                  if(*(lp_wasregistered + strlen(lp_wasregistered) - 1) == '"')
                    *(lp_wasregistered + strlen(lp_wasregistered) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wasreloaded", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasreloaded, strchr(line + i, '"') + 1, 100);
                  if(*(lp_wasreloaded + strlen(lp_wasreloaded) - 1) == '"')
                    *(lp_wasreloaded + strlen(lp_wasreloaded) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_wasungagged", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_wasungagged, strchr(line + i, '"') + 1, 100);
                  if(*(lp_wasungagged + strlen(lp_wasungagged) - 1) == '"')
                    *(lp_wasungagged + strlen(lp_wasungagged) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_waswarnedby", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_waswarnedby, strchr(line + i, '"') + 1, 50);
                  if(*(lp_waswarnedby + strlen(lp_waswarnedby) - 1) == '"')
                    *(lp_waswarnedby + strlen(lp_waswarnedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_weeks", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_weeks, strchr(line + i, '"') + 1, 50);
                  if(*(lp_weeks + strlen(lp_weeks) - 1) == '"')
                    *(lp_weeks + strlen(lp_weeks) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_welcometo", 12) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_welcometo, strchr(line + i, '"') + 1, 50);
                  if(*(lp_welcometo + strlen(lp_welcometo) - 1) == '"')
                    *(lp_welcometo + strlen(lp_welcometo) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_whohubswarn", 14) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_whohubswarn, strchr(line + i, '"') + 1, 100);
                  if(*(lp_whohubswarn + strlen(lp_whohubswarn) - 1) == '"')
                    *(lp_whohubswarn + strlen(lp_whohubswarn) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_withpass", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_withpass, strchr(line + i, '"') + 1, 100);
                  if(*(lp_withpass + strlen(lp_withpass) - 1) == '"')
                    *(lp_withpass + strlen(lp_withpass) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youcanwarnonly", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youcanwarnonly, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youcanwarnonly + strlen(lp_youcanwarnonly) - 1) == '"')
                    *(lp_youcanwarnonly + strlen(lp_youcanwarnonly) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youraccountwillexpire", 24) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youraccountwillexpire, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youraccountwillexpire + strlen(lp_youraccountwillexpire) - 1) == '"')
                    *(lp_youraccountwillexpire + strlen(lp_youraccountwillexpire) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youripis", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youripis, strchr(line + i, '"') + 1, 50);
                  if(*(lp_youripis + strlen(lp_youripis) - 1) == '"')
                    *(lp_youripis + strlen(lp_youripis) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_yourpasswordis", 17) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_yourpasswordis, strchr(line + i, '"') + 1, 50);
                  if(*(lp_yourpasswordis + strlen(lp_yourpasswordis) - 1) == '"')
                    *(lp_yourpasswordis + strlen(lp_yourpasswordis) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_yourwarnings", 15) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_yourwarnings, strchr(line + i, '"') + 1, 50);
                  if(*(lp_yourwarnings + strlen(lp_yourwarnings) - 1) == '"')
                    *(lp_yourwarnings + strlen(lp_yourwarnings) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youshare", 11) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youshare, strchr(line + i, '"') + 1, 50);
                  if(*(lp_youshare + strlen(lp_youshare) - 1) == '"')
                    *(lp_youshare + strlen(lp_youshare) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youwerebanned", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youwerebanned, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youwerebanned + strlen(lp_youwerebanned) - 1) == '"')
                    *(lp_youwerebanned + strlen(lp_youwerebanned) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youweregagged", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youweregagged, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youweregagged + strlen(lp_youweregagged) - 1) == '"')
                    *(lp_youweregagged + strlen(lp_youweregagged) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youwerekicked", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youwerekicked, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youwerekicked + strlen(lp_youwerekicked) - 1) == '"')
                    *(lp_youwerekicked + strlen(lp_youwerekicked) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youwereregistered", 20) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youwereregistered, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youwereregistered + strlen(lp_youwereregistered) - 1) == '"')
                    *(lp_youwereregistered + strlen(lp_youwereregistered) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youweretempoppedby", 21) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youweretempoppedby, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youweretempoppedby + strlen(lp_youweretempoppedby) - 1) == '"')
                    *(lp_youweretempoppedby + strlen(lp_youweretempoppedby) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_youwerewarned", 16) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_youwerewarned, strchr(line + i, '"') + 1, 100);
                  if(*(lp_youwerewarned + strlen(lp_youwerewarned) - 1) == '"')
                    *(lp_youwerewarned + strlen(lp_youwerewarned) - 1) = '\0';
               }

             else if(strncmp(line + i, "lp_us001", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us001, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us001 + strlen(lp_us001) - 1) == '"')
                    *(lp_us001 + strlen(lp_us001) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us002", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us002, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us002 + strlen(lp_us002) - 1) == '"')
                    *(lp_us002 + strlen(lp_us002) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us003", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us003, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us003 + strlen(lp_us003) - 1) == '"')
                    *(lp_us003 + strlen(lp_us003) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us004", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us004, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us004 + strlen(lp_us004) - 1) == '"')
                    *(lp_us004 + strlen(lp_us004) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us005", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us005, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us005 + strlen(lp_us005) - 1) == '"')
                    *(lp_us005 + strlen(lp_us005) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us006", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us006, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us006 + strlen(lp_us006) - 1) == '"')
                    *(lp_us006 + strlen(lp_us006) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us007", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us007, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us007 + strlen(lp_us007) - 1) == '"')
                    *(lp_us007 + strlen(lp_us007) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us008", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us008, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us008 + strlen(lp_us008) - 1) == '"')
                    *(lp_us008 + strlen(lp_us008) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us009", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us009, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us009 + strlen(lp_us009) - 1) == '"')
                    *(lp_us009 + strlen(lp_us009) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us010", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us010, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us010 + strlen(lp_us010) - 1) == '"')
                    *(lp_us010 + strlen(lp_us010) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us011", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us011, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us011 + strlen(lp_us011) - 1) == '"')
                    *(lp_us011 + strlen(lp_us011) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us012", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us012, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us012 + strlen(lp_us012) - 1) == '"')
                    *(lp_us012 + strlen(lp_us012) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us013", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us013, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us013 + strlen(lp_us013) - 1) == '"')
                    *(lp_us013 + strlen(lp_us013) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us014", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us014, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us014 + strlen(lp_us014) - 1) == '"')
                    *(lp_us014 + strlen(lp_us014) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us015", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us015, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us015 + strlen(lp_us015) - 1) == '"')
                    *(lp_us015 + strlen(lp_us015) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us016", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us016, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us016 + strlen(lp_us016) - 1) == '"')
                    *(lp_us016 + strlen(lp_us016) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us017", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us017, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us017 + strlen(lp_us017) - 1) == '"')
                    *(lp_us017 + strlen(lp_us017) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us018", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us018, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us018 + strlen(lp_us018) - 1) == '"')
                    *(lp_us018 + strlen(lp_us018) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us019", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us019, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us019 + strlen(lp_us019) - 1) == '"')
                    *(lp_us019 + strlen(lp_us019) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us020", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us020, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us020 + strlen(lp_us020) - 1) == '"')
                    *(lp_us020 + strlen(lp_us020) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us021", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us021, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us021 + strlen(lp_us021) - 1) == '"')
                    *(lp_us021 + strlen(lp_us021) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us022", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us022, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us022 + strlen(lp_us022) - 1) == '"')
                    *(lp_us022 + strlen(lp_us022) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us023", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us023, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us023 + strlen(lp_us023) - 1) == '"')
                    *(lp_us023 + strlen(lp_us023) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us024", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us024, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us024 + strlen(lp_us024) - 1) == '"')
                    *(lp_us024 + strlen(lp_us024) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us025", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us025, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us025 + strlen(lp_us025) - 1) == '"')
                    *(lp_us025 + strlen(lp_us025) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us026", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us026, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us026 + strlen(lp_us026) - 1) == '"')
                    *(lp_us026 + strlen(lp_us026) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us027", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us027, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us027 + strlen(lp_us027) - 1) == '"')
                    *(lp_us027 + strlen(lp_us027) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us028", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us028, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us028 + strlen(lp_us028) - 1) == '"')
                    *(lp_us028 + strlen(lp_us028) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us029", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us029, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us029 + strlen(lp_us029) - 1) == '"')
                    *(lp_us029 + strlen(lp_us029) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us030", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us030, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us030 + strlen(lp_us030) - 1) == '"')
                    *(lp_us030 + strlen(lp_us030) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us031", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us031, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us031 + strlen(lp_us031) - 1) == '"')
                    *(lp_us031 + strlen(lp_us031) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us032", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us032, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us032 + strlen(lp_us032) - 1) == '"')
                    *(lp_us032 + strlen(lp_us032) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us033", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us033, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us033 + strlen(lp_us033) - 1) == '"')
                    *(lp_us033 + strlen(lp_us033) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us034", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us034, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us034 + strlen(lp_us034) - 1) == '"')
                    *(lp_us034 + strlen(lp_us034) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us035", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us035, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us035 + strlen(lp_us035) - 1) == '"')
                    *(lp_us035 + strlen(lp_us035) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us036", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us036, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us036 + strlen(lp_us036) - 1) == '"')
                    *(lp_us036 + strlen(lp_us036) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us037", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us037, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us037 + strlen(lp_us037) - 1) == '"')
                    *(lp_us037 + strlen(lp_us037) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us038", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us038, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us038 + strlen(lp_us038) - 1) == '"')
                    *(lp_us038 + strlen(lp_us038) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us039", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us039, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us039 + strlen(lp_us039) - 1) == '"')
                    *(lp_us039 + strlen(lp_us039) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us040", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us040, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us040 + strlen(lp_us040) - 1) == '"')
                    *(lp_us040 + strlen(lp_us040) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us041", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us041, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us041 + strlen(lp_us041) - 1) == '"')
                    *(lp_us041 + strlen(lp_us041) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us042", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us042, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us042 + strlen(lp_us042) - 1) == '"')
                    *(lp_us042 + strlen(lp_us042) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us043", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us043, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us043 + strlen(lp_us043) - 1) == '"')
                    *(lp_us043 + strlen(lp_us043) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us044", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us044, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us044 + strlen(lp_us044) - 1) == '"')
                    *(lp_us044 + strlen(lp_us044) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us045", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us045, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us045 + strlen(lp_us045) - 1) == '"')
                    *(lp_us045 + strlen(lp_us045) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us046", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us046, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us046 + strlen(lp_us046) - 1) == '"')
                    *(lp_us046 + strlen(lp_us046) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us048", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us048, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us048 + strlen(lp_us048) - 1) == '"')
                    *(lp_us048 + strlen(lp_us048) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us049", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us049, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us049 + strlen(lp_us049) - 1) == '"')
                    *(lp_us049 + strlen(lp_us049) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us050", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us050, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us050 + strlen(lp_us050) - 1) == '"')
                    *(lp_us050 + strlen(lp_us050) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us051", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us051, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us051 + strlen(lp_us051) - 1) == '"')
                    *(lp_us051 + strlen(lp_us051) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us052", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us052, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us052 + strlen(lp_us052) - 1) == '"')
                    *(lp_us052 + strlen(lp_us052) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us053", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us053, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us053 + strlen(lp_us053) - 1) == '"')
                    *(lp_us053 + strlen(lp_us053) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us054", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us054, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us054 + strlen(lp_us054) - 1) == '"')
                    *(lp_us054 + strlen(lp_us054) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us055", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us055, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us055 + strlen(lp_us055) - 1) == '"')
                    *(lp_us055 + strlen(lp_us055) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us056", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us056, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us056 + strlen(lp_us056) - 1) == '"')
                    *(lp_us056 + strlen(lp_us056) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us057", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us057, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us057 + strlen(lp_us057) - 1) == '"')
                    *(lp_us057 + strlen(lp_us057) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us058", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us058, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us058 + strlen(lp_us058) - 1) == '"')
                    *(lp_us058 + strlen(lp_us058) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us059", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us059, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us059 + strlen(lp_us059) - 1) == '"')
                    *(lp_us059 + strlen(lp_us059) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us060", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us060, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us060 + strlen(lp_us060) - 1) == '"')
                    *(lp_us060 + strlen(lp_us060) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us063", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us063, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us063 + strlen(lp_us063) - 1) == '"')
                    *(lp_us063 + strlen(lp_us063) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us064", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us064, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us064 + strlen(lp_us064) - 1) == '"')
                    *(lp_us064 + strlen(lp_us064) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us065", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us065, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us065 + strlen(lp_us065) - 1) == '"')
                    *(lp_us065 + strlen(lp_us065) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us066", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us066, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us066 + strlen(lp_us066) - 1) == '"')
                    *(lp_us066 + strlen(lp_us066) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us067", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us067, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us067 + strlen(lp_us067) - 1) == '"')
                    *(lp_us067 + strlen(lp_us067) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us068", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us068, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us068 + strlen(lp_us068) - 1) == '"')
                    *(lp_us068 + strlen(lp_us068) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us069", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us069, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us069 + strlen(lp_us069) - 1) == '"')
                    *(lp_us069 + strlen(lp_us069) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us070", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us070, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us070 + strlen(lp_us070) - 1) == '"')
                    *(lp_us070 + strlen(lp_us070) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us071", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us071, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us071 + strlen(lp_us071) - 1) == '"')
                    *(lp_us071 + strlen(lp_us071) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us072", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us072, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us072 + strlen(lp_us072) - 1) == '"')
                    *(lp_us072 + strlen(lp_us072) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us073", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us073, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us073 + strlen(lp_us073) - 1) == '"')
                    *(lp_us073 + strlen(lp_us073) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us074", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us074, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us074 + strlen(lp_us074) - 1) == '"')
                    *(lp_us074 + strlen(lp_us074) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us075", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us075, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us075 + strlen(lp_us075) - 1) == '"')
                    *(lp_us075 + strlen(lp_us075) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us076", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us076, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us076 + strlen(lp_us076) - 1) == '"')
                    *(lp_us076 + strlen(lp_us076) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us077", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us077, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us077 + strlen(lp_us077) - 1) == '"')
                    *(lp_us077 + strlen(lp_us077) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us078", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us078, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us078 + strlen(lp_us078) - 1) == '"')
                    *(lp_us078 + strlen(lp_us078) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us079", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us079, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us079 + strlen(lp_us079) - 1) == '"')
                    *(lp_us079 + strlen(lp_us079) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us080", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us080, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us080 + strlen(lp_us080) - 1) == '"')
                    *(lp_us080 + strlen(lp_us080) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us081", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us081, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us081 + strlen(lp_us081) - 1) == '"')
                    *(lp_us081 + strlen(lp_us081) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us082", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us082, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us082 + strlen(lp_us082) - 1) == '"')
                    *(lp_us082 + strlen(lp_us082) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us083", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us083, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us083 + strlen(lp_us083) - 1) == '"')
                    *(lp_us083 + strlen(lp_us083) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us084", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us084, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us084 + strlen(lp_us084) - 1) == '"')
                    *(lp_us084 + strlen(lp_us084) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us085", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us085, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us085 + strlen(lp_us085) - 1) == '"')
                    *(lp_us085 + strlen(lp_us085) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us086", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us086, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us086 + strlen(lp_us086) - 1) == '"')
                    *(lp_us086 + strlen(lp_us086) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us087", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us087, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us087 + strlen(lp_us087) - 1) == '"')
                    *(lp_us087 + strlen(lp_us087) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us088", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us088, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us088 + strlen(lp_us088) - 1) == '"')
                    *(lp_us088 + strlen(lp_us088) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us089", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us089, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us089 + strlen(lp_us089) - 1) == '"')
                    *(lp_us089 + strlen(lp_us089) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us090", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us090, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us090 + strlen(lp_us090) - 1) == '"')
                    *(lp_us090 + strlen(lp_us090) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us091", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us091, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us091 + strlen(lp_us091) - 1) == '"')
                    *(lp_us091 + strlen(lp_us091) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us092", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us092, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us092 + strlen(lp_us092) - 1) == '"')
                    *(lp_us092 + strlen(lp_us092) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us093", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us093, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us093 + strlen(lp_us093) - 1) == '"')
                    *(lp_us093 + strlen(lp_us093) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us094", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us094, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us094 + strlen(lp_us094) - 1) == '"')
                    *(lp_us094 + strlen(lp_us094) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us095", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us095, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us095 + strlen(lp_us095) - 1) == '"')
                    *(lp_us095 + strlen(lp_us095) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us096", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us096, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us096 + strlen(lp_us096) - 1) == '"')
                    *(lp_us096 + strlen(lp_us096) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us097", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us097, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us097 + strlen(lp_us097) - 1) == '"')
                    *(lp_us097 + strlen(lp_us097) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us098", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us098, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us098 + strlen(lp_us098) - 1) == '"')
                    *(lp_us098 + strlen(lp_us098) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us099", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us099, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us099 + strlen(lp_us099) - 1) == '"')
                    *(lp_us099 + strlen(lp_us099) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us100", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us100, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us100 + strlen(lp_us100) - 1) == '"')
                    *(lp_us100 + strlen(lp_us100) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us101", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us101, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us101 + strlen(lp_us101) - 1) == '"')
                    *(lp_us101 + strlen(lp_us101) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us102", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us102, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us102 + strlen(lp_us102) - 1) == '"')
                    *(lp_us102 + strlen(lp_us102) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us103", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us103, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us103 + strlen(lp_us103) - 1) == '"')
                    *(lp_us103 + strlen(lp_us103) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us104", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us104, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us104 + strlen(lp_us104) - 1) == '"')
                    *(lp_us104 + strlen(lp_us104) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us105", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us105, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us105 + strlen(lp_us105) - 1) == '"')
                    *(lp_us105 + strlen(lp_us105) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us106", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us106, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us106 + strlen(lp_us106) - 1) == '"')
                    *(lp_us106 + strlen(lp_us106) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us107", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us107, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us107 + strlen(lp_us107) - 1) == '"')
                    *(lp_us107 + strlen(lp_us107) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us108", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us108, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us108 + strlen(lp_us108) - 1) == '"')
                    *(lp_us108 + strlen(lp_us108) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us109", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us109, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us109 + strlen(lp_us109) - 1) == '"')
                    *(lp_us109 + strlen(lp_us109) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us110", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us110, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us110 + strlen(lp_us110) - 1) == '"')
                    *(lp_us110 + strlen(lp_us110) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us111", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us111, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us111 + strlen(lp_us111) - 1) == '"')
                    *(lp_us111 + strlen(lp_us111) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us112", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us112, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us112 + strlen(lp_us112) - 1) == '"')
                    *(lp_us112 + strlen(lp_us112) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us113", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us113, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us113 + strlen(lp_us113) - 1) == '"')
                    *(lp_us113 + strlen(lp_us113) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us114", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us114, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us114 + strlen(lp_us114) - 1) == '"')
                    *(lp_us114 + strlen(lp_us114) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us115", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us115, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us115 + strlen(lp_us115) - 1) == '"')
                    *(lp_us115 + strlen(lp_us115) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us116", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us116, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us116 + strlen(lp_us116) - 1) == '"')
                    *(lp_us116 + strlen(lp_us116) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us117", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us117, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us117 + strlen(lp_us117) - 1) == '"')
                    *(lp_us117 + strlen(lp_us117) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us118", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us118, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us118 + strlen(lp_us118) - 1) == '"')
                    *(lp_us118 + strlen(lp_us118) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us119", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us119, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us119 + strlen(lp_us119) - 1) == '"')
                    *(lp_us119 + strlen(lp_us119) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us120", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us120, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us120 + strlen(lp_us120) - 1) == '"')
                    *(lp_us120 + strlen(lp_us120) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us122", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us122, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us122 + strlen(lp_us122) - 1) == '"')
                    *(lp_us122 + strlen(lp_us122) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us123", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us123, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us123 + strlen(lp_us123) - 1) == '"')
                    *(lp_us123 + strlen(lp_us123) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us124", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us124, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us124 + strlen(lp_us124) - 1) == '"')
                    *(lp_us124 + strlen(lp_us124) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us125", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us125, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us125 + strlen(lp_us125) - 1) == '"')
                    *(lp_us125 + strlen(lp_us125) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us126", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us126, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us126 + strlen(lp_us126) - 1) == '"')
                    *(lp_us126 + strlen(lp_us126) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us127", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us127, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us127 + strlen(lp_us127) - 1) == '"')
                    *(lp_us127 + strlen(lp_us127) - 1) = '\0';
               }

             else if(strncmp(line + i, "lp_us128", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us128, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us128 + strlen(lp_us128) - 1) == '"')
                    *(lp_us128 + strlen(lp_us128) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us129", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us129, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us129 + strlen(lp_us129) - 1) == '"')
                    *(lp_us129 + strlen(lp_us129) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us130", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us130, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us130 + strlen(lp_us130) - 1) == '"')
                    *(lp_us130 + strlen(lp_us130) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_us131", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us131, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us131 + strlen(lp_us131) - 1) == '"')
                    *(lp_us131 + strlen(lp_us131) - 1) = '\0';
	    }
             else if(strncmp(line + i, "lp_us132", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us132, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us132 + strlen(lp_us132) - 1) == '"')
                    *(lp_us132 + strlen(lp_us132) - 1) = '\0';
	    }
             else if(strncmp(line + i, "lp_us133", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us133, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us133 + strlen(lp_us133) - 1) == '"')
                    *(lp_us133 + strlen(lp_us133) - 1) = '\0';
	    }
             else if(strncmp(line + i, "lp_us134", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us134, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us134 + strlen(lp_us134) - 1) == '"')
                    *(lp_us134 + strlen(lp_us134) - 1) = '\0';
	    }
             else if(strncmp(line + i, "lp_us135", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_us135, strchr(line + i, '"') + 1, 50);
                  if(*(lp_us135 + strlen(lp_us135) - 1) == '"')
                    *(lp_us135 + strlen(lp_us135) - 1) = '\0';
	    }
             else if(strncmp(line + i, "lp_he001", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he001, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he001 + strlen(lp_he001) - 1) == '"')
                    *(lp_he001 + strlen(lp_he001) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he002", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he002, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he002 + strlen(lp_he002) - 1) == '"')
                    *(lp_he002 + strlen(lp_he002) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he003", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he003, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he003 + strlen(lp_he003) - 1) == '"')
                    *(lp_he003 + strlen(lp_he003) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he004", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he004, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he004 + strlen(lp_he004) - 1) == '"')
                    *(lp_he004 + strlen(lp_he004) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he005", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he005, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he005 + strlen(lp_he005) - 1) == '"')
                    *(lp_he005 + strlen(lp_he005) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he006", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he006, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he006 + strlen(lp_he006) - 1) == '"')
                    *(lp_he006 + strlen(lp_he006) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he007", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he007, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he007 + strlen(lp_he007) - 1) == '"')
                    *(lp_he007 + strlen(lp_he007) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he008", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he008, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he008 + strlen(lp_he008) - 1) == '"')
                    *(lp_he008 + strlen(lp_he008) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he009", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he009, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he009 + strlen(lp_he009) - 1) == '"')
                    *(lp_he009 + strlen(lp_he009) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he010", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he010, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he010 + strlen(lp_he010) - 1) == '"')
                    *(lp_he010 + strlen(lp_he010) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he011", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he011, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he011 + strlen(lp_he011) - 1) == '"')
                    *(lp_he011 + strlen(lp_he011) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he012", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he012, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he012 + strlen(lp_he012) - 1) == '"')
                    *(lp_he012 + strlen(lp_he012) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he013", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he013, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he013 + strlen(lp_he013) - 1) == '"')
                    *(lp_he013 + strlen(lp_he013) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he014", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he014, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he014 + strlen(lp_he014) - 1) == '"')
                    *(lp_he014 + strlen(lp_he014) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he015", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he015, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he015 + strlen(lp_he015) - 1) == '"')
                    *(lp_he015 + strlen(lp_he015) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he016", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he016, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he016 + strlen(lp_he016) - 1) == '"')
                    *(lp_he016 + strlen(lp_he016) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he017", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he017, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he017 + strlen(lp_he017) - 1) == '"')
                    *(lp_he017 + strlen(lp_he017) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he018", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he018, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he018 + strlen(lp_he018) - 1) == '"')
                    *(lp_he018 + strlen(lp_he018) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he019", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he019, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he019 + strlen(lp_he019) - 1) == '"')
                    *(lp_he019 + strlen(lp_he019) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he020", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he020, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he020 + strlen(lp_he020) - 1) == '"')
                    *(lp_he020 + strlen(lp_he020) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he021", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he021, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he021 + strlen(lp_he021) - 1) == '"')
                    *(lp_he021 + strlen(lp_he021) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he022", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he022, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he022 + strlen(lp_he022) - 1) == '"')
                    *(lp_he022 + strlen(lp_he022) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he023", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he023, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he023 + strlen(lp_he023) - 1) == '"')
                    *(lp_he023 + strlen(lp_he023) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he024", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he024, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he024 + strlen(lp_he024) - 1) == '"')
                    *(lp_he024 + strlen(lp_he024) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he025", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he025, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he025 + strlen(lp_he025) - 1) == '"')
                    *(lp_he025 + strlen(lp_he025) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he026", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he026, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he026 + strlen(lp_he026) - 1) == '"')
                    *(lp_he026 + strlen(lp_he026) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he027", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he027, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he027 + strlen(lp_he027) - 1) == '"')
                    *(lp_he027 + strlen(lp_he027) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he028", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he028, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he028 + strlen(lp_he028) - 1) == '"')
                    *(lp_he028 + strlen(lp_he028) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he029", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he029, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he029 + strlen(lp_he029) - 1) == '"')
                    *(lp_he029 + strlen(lp_he029) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he030", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he030, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he030 + strlen(lp_he030) - 1) == '"')
                    *(lp_he030 + strlen(lp_he030) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he031", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he031, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he031 + strlen(lp_he031) - 1) == '"')
                    *(lp_he031 + strlen(lp_he031) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he032", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he032, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he032 + strlen(lp_he032) - 1) == '"')
                    *(lp_he032 + strlen(lp_he032) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he033", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he033, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he033 + strlen(lp_he033) - 1) == '"')
                    *(lp_he033 + strlen(lp_he033) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he034", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he034, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he034 + strlen(lp_he034) - 1) == '"')
                    *(lp_he034 + strlen(lp_he034) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he035", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he035, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he035 + strlen(lp_he035) - 1) == '"')
                    *(lp_he035 + strlen(lp_he035) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he036", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he036, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he036 + strlen(lp_he036) - 1) == '"')
                    *(lp_he036 + strlen(lp_he036) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he037", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he037, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he037 + strlen(lp_he037) - 1) == '"')
                    *(lp_he037 + strlen(lp_he037) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he038", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he038, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he038 + strlen(lp_he038) - 1) == '"')
                    *(lp_he038 + strlen(lp_he038) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he039", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he039, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he039 + strlen(lp_he039) - 1) == '"')
                    *(lp_he039 + strlen(lp_he039) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he040", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he040, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he040 + strlen(lp_he040) - 1) == '"')
                    *(lp_he040 + strlen(lp_he040) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he041", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he041, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he041 + strlen(lp_he041) - 1) == '"')
                    *(lp_he041 + strlen(lp_he041) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he042", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he042, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he042 + strlen(lp_he042) - 1) == '"')
                    *(lp_he042 + strlen(lp_he042) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he043", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he043, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he043 + strlen(lp_he043) - 1) == '"')
                    *(lp_he043 + strlen(lp_he043) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he044", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he044, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he044 + strlen(lp_he044) - 1) == '"')
                    *(lp_he044 + strlen(lp_he044) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he045", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he045, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he045 + strlen(lp_he045) - 1) == '"')
                    *(lp_he045 + strlen(lp_he045) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he046", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he046, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he046 + strlen(lp_he046) - 1) == '"')
                    *(lp_he046 + strlen(lp_he046) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he048", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he048, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he048 + strlen(lp_he048) - 1) == '"')
                    *(lp_he048 + strlen(lp_he048) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he049", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he049, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he049 + strlen(lp_he049) - 1) == '"')
                    *(lp_he049 + strlen(lp_he049) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he050", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he050, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he050 + strlen(lp_he050) - 1) == '"')
                    *(lp_he050 + strlen(lp_he050) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he051", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he051, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he051 + strlen(lp_he051) - 1) == '"')
                    *(lp_he051 + strlen(lp_he051) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he052", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he052, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he052 + strlen(lp_he052) - 1) == '"')
                    *(lp_he052 + strlen(lp_he052) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he053", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he053, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he053 + strlen(lp_he053) - 1) == '"')
                    *(lp_he053 + strlen(lp_he053) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he054", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he054, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he054 + strlen(lp_he054) - 1) == '"')
                    *(lp_he054 + strlen(lp_he054) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he055", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he055, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he055 + strlen(lp_he055) - 1) == '"')
                    *(lp_he055 + strlen(lp_he055) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he056", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he056, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he056 + strlen(lp_he056) - 1) == '"')
                    *(lp_he056 + strlen(lp_he056) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he057", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he057, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he057 + strlen(lp_he057) - 1) == '"')
                    *(lp_he057 + strlen(lp_he057) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he058", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he058, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he058 + strlen(lp_he058) - 1) == '"')
                    *(lp_he058 + strlen(lp_he058) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he059", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he059, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he059 + strlen(lp_he059) - 1) == '"')
                    *(lp_he059 + strlen(lp_he059) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he060", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he060, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he060 + strlen(lp_he060) - 1) == '"')
                    *(lp_he060 + strlen(lp_he060) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he063", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he063, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he063 + strlen(lp_he063) - 1) == '"')
                    *(lp_he063 + strlen(lp_he063) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he064", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he064, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he064 + strlen(lp_he064) - 1) == '"')
                    *(lp_he064 + strlen(lp_he064) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he065", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he065, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he065 + strlen(lp_he065) - 1) == '"')
                    *(lp_he065 + strlen(lp_he065) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he066", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he066, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he066 + strlen(lp_he066) - 1) == '"')
                    *(lp_he066 + strlen(lp_he066) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he067", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he067, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he067 + strlen(lp_he067) - 1) == '"')
                    *(lp_he067 + strlen(lp_he067) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he068", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he068, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he068 + strlen(lp_he068) - 1) == '"')
                    *(lp_he068 + strlen(lp_he068) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he069", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he069, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he069 + strlen(lp_he069) - 1) == '"')
                    *(lp_he069 + strlen(lp_he069) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he070", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he070, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he070 + strlen(lp_he070) - 1) == '"')
                    *(lp_he070 + strlen(lp_he070) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he071", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he071, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he071 + strlen(lp_he071) - 1) == '"')
                    *(lp_he071 + strlen(lp_he071) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he072", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he072, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he072 + strlen(lp_he072) - 1) == '"')
                    *(lp_he072 + strlen(lp_he072) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he073", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he073, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he073 + strlen(lp_he073) - 1) == '"')
                    *(lp_he073 + strlen(lp_he073) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he074", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he074, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he074 + strlen(lp_he074) - 1) == '"')
                    *(lp_he074 + strlen(lp_he074) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he075", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he075, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he075 + strlen(lp_he075) - 1) == '"')
                    *(lp_he075 + strlen(lp_he075) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he076", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he076, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he076 + strlen(lp_he076) - 1) == '"')
                    *(lp_he076 + strlen(lp_he076) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he077", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he077, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he077 + strlen(lp_he077) - 1) == '"')
                    *(lp_he077 + strlen(lp_he077) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he078", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he078, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he078 + strlen(lp_he078) - 1) == '"')
                    *(lp_he078 + strlen(lp_he078) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he079", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he079, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he079 + strlen(lp_he079) - 1) == '"')
                    *(lp_he079 + strlen(lp_he079) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he080", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he080, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he080 + strlen(lp_he080) - 1) == '"')
                    *(lp_he080 + strlen(lp_he080) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he081", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he081, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he081 + strlen(lp_he081) - 1) == '"')
                    *(lp_he081 + strlen(lp_he081) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he082", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he082, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he082 + strlen(lp_he082) - 1) == '"')
                    *(lp_he082 + strlen(lp_he082) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he083", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he083, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he083 + strlen(lp_he083) - 1) == '"')
                    *(lp_he083 + strlen(lp_he083) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he084", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he084, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he084 + strlen(lp_he084) - 1) == '"')
                    *(lp_he084 + strlen(lp_he084) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he085", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he085, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he085 + strlen(lp_he085) - 1) == '"')
                    *(lp_he085 + strlen(lp_he085) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he086", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he086, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he086 + strlen(lp_he086) - 1) == '"')
                    *(lp_he086 + strlen(lp_he086) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he087", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he087, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he087 + strlen(lp_he087) - 1) == '"')
                    *(lp_he087 + strlen(lp_he087) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he088", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he088, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he088 + strlen(lp_he088) - 1) == '"')
                    *(lp_he088 + strlen(lp_he088) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he089", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he089, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he089 + strlen(lp_he089) - 1) == '"')
                    *(lp_he089 + strlen(lp_he089) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he090", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he090, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he090 + strlen(lp_he090) - 1) == '"')
                    *(lp_he090 + strlen(lp_he090) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he091", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he091, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he091 + strlen(lp_he091) - 1) == '"')
                    *(lp_he091 + strlen(lp_he091) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he092", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he092, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he092 + strlen(lp_he092) - 1) == '"')
                    *(lp_he092 + strlen(lp_he092) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he093", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he093, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he093 + strlen(lp_he093) - 1) == '"')
                    *(lp_he093 + strlen(lp_he093) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he094", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he094, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he094 + strlen(lp_he094) - 1) == '"')
                    *(lp_he094 + strlen(lp_he094) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he095", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he095, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he095 + strlen(lp_he095) - 1) == '"')
                    *(lp_he095 + strlen(lp_he095) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he096", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he096, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he096 + strlen(lp_he096) - 1) == '"')
                    *(lp_he096 + strlen(lp_he096) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he097", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he097, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he097 + strlen(lp_he097) - 1) == '"')
                    *(lp_he097 + strlen(lp_he097) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he098", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he098, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he098 + strlen(lp_he098) - 1) == '"')
                    *(lp_he098 + strlen(lp_he098) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he099", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he099, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he099 + strlen(lp_he099) - 1) == '"')
                    *(lp_he099 + strlen(lp_he099) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he100", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he100, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he100 + strlen(lp_he100) - 1) == '"')
                    *(lp_he100 + strlen(lp_he100) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he101", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he101, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he101 + strlen(lp_he101) - 1) == '"')
                    *(lp_he101 + strlen(lp_he101) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he102", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he102, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he102 + strlen(lp_he102) - 1) == '"')
                    *(lp_he102 + strlen(lp_he102) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he103", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he103, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he103 + strlen(lp_he103) - 1) == '"')
                    *(lp_he103 + strlen(lp_he103) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he104", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he104, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he104 + strlen(lp_he104) - 1) == '"')
                    *(lp_he104 + strlen(lp_he104) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he105", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he105, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he105 + strlen(lp_he105) - 1) == '"')
                    *(lp_he105 + strlen(lp_he105) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he106", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he106, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he106 + strlen(lp_he106) - 1) == '"')
                    *(lp_he106 + strlen(lp_he106) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he107", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he107, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he107 + strlen(lp_he107) - 1) == '"')
                    *(lp_he107 + strlen(lp_he107) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he108", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he108, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he108 + strlen(lp_he108) - 1) == '"')
                    *(lp_he108 + strlen(lp_he108) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he109", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he109, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he109 + strlen(lp_he109) - 1) == '"')
                    *(lp_he109 + strlen(lp_he109) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he110", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he110, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he110 + strlen(lp_he110) - 1) == '"')
                    *(lp_he110 + strlen(lp_he110) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he111", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he111, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he111 + strlen(lp_he111) - 1) == '"')
                    *(lp_he111 + strlen(lp_he111) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he112", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he112, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he112 + strlen(lp_he112) - 1) == '"')
                    *(lp_he112 + strlen(lp_he112) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he113", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he113, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he113 + strlen(lp_he113) - 1) == '"')
                    *(lp_he113 + strlen(lp_he113) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he114", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he114, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he114 + strlen(lp_he114) - 1) == '"')
                    *(lp_he114 + strlen(lp_he114) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he116", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he116, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he116 + strlen(lp_he116) - 1) == '"')
                    *(lp_he116 + strlen(lp_he116) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he117", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he117, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he117 + strlen(lp_he117) - 1) == '"')
                    *(lp_he117 + strlen(lp_he117) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he118", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he118, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he118 + strlen(lp_he118) - 1) == '"')
                    *(lp_he118 + strlen(lp_he118) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he119", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he119, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he119 + strlen(lp_he119) - 1) == '"')
                    *(lp_he119 + strlen(lp_he119) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he120", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he120, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he120 + strlen(lp_he120) - 1) == '"')
                    *(lp_he120 + strlen(lp_he120) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he121", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he121, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he121 + strlen(lp_he121) - 1) == '"')
                    *(lp_he121 + strlen(lp_he121) - 1) = '\0';
               }
             else if(strncmp(line + i, "lp_he122", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he122, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he122 + strlen(lp_he122) - 1) == '"')
                    *(lp_he122 + strlen(lp_he122) - 1) = '\0';
               }	       
             else if(strncmp(line + i, "lp_he123", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he123, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he123 + strlen(lp_he123) - 1) == '"')
                    *(lp_he123 + strlen(lp_he123) - 1) = '\0';
               }	       
             else if(strncmp(line + i, "lp_he124", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he124, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he124 + strlen(lp_he124) - 1) == '"')
                    *(lp_he124 + strlen(lp_he124) - 1) = '\0';
               }	       
             else if(strncmp(line + i, "lp_he125", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he125, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he125 + strlen(lp_he125) - 1) == '"')
                    *(lp_he125 + strlen(lp_he125) - 1) = '\0';
               }	       
             else if(strncmp(line + i, "lp_he126", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he126, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he126 + strlen(lp_he126) - 1) == '"')
                    *(lp_he126 + strlen(lp_he126) - 1) = '\0';
               }	       
             else if(strncmp(line + i, "lp_he127", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he127, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he127 + strlen(lp_he127) - 1) == '"')
                    *(lp_he127 + strlen(lp_he127) - 1) = '\0';
               }	       
             else if(strncmp(line + i, "lp_he128", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he128, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he128 + strlen(lp_he128) - 1) == '"')
                    *(lp_he128 + strlen(lp_he128) - 1) = '\0';
               }	       
             else if(strncmp(line + i, "lp_he129", 8) == 0)
               {
                  if(strchr(line + i, '"') == NULL)
                    {
                       set_lock(fd, F_UNLCK);
                       while(((erret = fclose(fp)) != 0) && (errno == EINTR))
                         logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
                       if(erret != 0)
                         {
                            logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
                            return -1;
                         }
                       return -1;
                    }
                  strncpy(lp_he129, strchr(line + i, '"') + 1, 100);
                  if(*(lp_he129 + strlen(lp_he129) - 1) == '"')
                    *(lp_he129 + strlen(lp_he129) - 1) = '\0';
               }	       
	       
     }
   }
   set_lock(fd, F_UNLCK);
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In read_lang()/fclose(): Interrupted system call. Trying again.\n");
   if(erret != 0)
     {
        logprintf(0, "Error - In read_lang()/fclose(): %s", strerror(errno));
        return -1;
     }
   return 1;
}
void set_lang(char *buf, struct user_t *user)
{
        char *c;
if(prvset == 1)
uprintf(user, "$To: %s From: %s $", user->nick, botnick);

if(strncmp(buf, "lp_accepted_ ", 13) == 0)
{
buf += 13;
strncpy(lp_accepted_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_accepted_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_accepted_ ] %s \"%s\"|", botnick, lp_setto, lp_accepted_);
}
else if(strncmp(buf, "lp_accountupdated ", 18) == 0)
{
buf += 18;
strncpy(lp_accountupdated, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_accountupdated[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_accountupdated ] %s \"%s\"|", botnick, lp_setto, lp_accountupdated);
}
else if(strncmp(buf, "lp_active_ ", 11) == 0)
{
buf += 11;
strncpy(lp_active_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_active_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_active_ ] %s \"%s\"|", botnick, lp_setto, lp_active_);
}
else if(strncmp(buf, "lp_added_ ", 10) == 0)
{
buf += 10;
strncpy(lp_added_, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_added_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_added_ ] %s \"%s\"|", botnick, lp_setto, lp_added_);
}
else if(strncmp(buf, "lp_addedentryto ", 16) == 0)
{
buf += 16;
strncpy(lp_addedentryto, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_addedentryto[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_addedentryto ] %s \"%s\"|", botnick, lp_setto, lp_addedentryto);
}
else if(strncmp(buf, "lp_admins_ ", 11) == 0)
{
buf += 11;
strncpy(lp_admins_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_admins_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_admins_ ] %s \"%s\"|", botnick, lp_setto, lp_admins_);
}
else if(strncmp(buf, "lp_after ", 9) == 0)
{
buf += 9;
strncpy(lp_after, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_after[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_after ] %s \"%s\"|", botnick, lp_setto, lp_after);
}
else if(strncmp(buf, "lp_ago_ ", 8) == 0)
{
buf += 8;
strncpy(lp_ago_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_ago_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_ago_ ] %s \"%s\"|", botnick, lp_setto, lp_ago_);
}
else if(strncmp(buf, "lp_allowed_ ", 12) == 0)
{
buf += 12;
strncpy(lp_allowed_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_allowed_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_allowed_ ] %s \"%s\"|", botnick, lp_setto, lp_allowed_);
}
else if(strncmp(buf, "lp_allowedatmost ", 17) == 0)
{
buf += 17;
strncpy(lp_allowedatmost, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_allowedatmost[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_allowedatmost ] %s \"%s\"|", botnick, lp_setto, lp_allowedatmost);
}
else if(strncmp(buf, "lp_allowedclients ", 18) == 0)
{
buf += 18;
strncpy(lp_allowedclients, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_allowedclients[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_allowedclients ] %s \"%s\"|", botnick, lp_setto, lp_allowedclients);
}
else if(strncmp(buf, "lp_alltime ", 11) == 0)
{
buf += 11;
strncpy(lp_alltime, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_alltime[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_alltime ] %s \"%s\"|", botnick, lp_setto, lp_alltime);
}
else if(strncmp(buf, "lp_autoawaymodeoff ", 19) == 0)
{
buf += 19;
strncpy(lp_autoawaymodeoff, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_autoawaymodeoff[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_autoawaymodeoff ] %s \"%s\"|", botnick, lp_setto, lp_autoawaymodeoff);
}
else if(strncmp(buf, "lp_autoawaymodeon ", 18) == 0)
{
buf += 18;
strncpy(lp_autoawaymodeon, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_autoawaymodeon[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_autoawaymodeon ] %s \"%s\"|", botnick, lp_setto, lp_autoawaymodeon);
}
else if(strncmp(buf, "lp_autoawaynotset ", 18) == 0)
{
buf += 18;
strncpy(lp_autoawaynotset, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_autoawaynotset[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_autoawaynotset ] %s \"%s\"|", botnick, lp_setto, lp_autoawaynotset);
}
else if(strncmp(buf, "lp_autoawayremoved ", 19) == 0)
{
buf += 19;
strncpy(lp_autoawayremoved, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_autoawayremoved[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_autoawayremoved ] %s \"%s\"|", botnick, lp_setto, lp_autoawayremoved);
}
else if(strncmp(buf, "lp_autoawayset ", 15) == 0)
{
buf += 15;
strncpy(lp_autoawayset, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_autoawayset[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_autoawayset ] %s \"%s\"|", botnick, lp_setto, lp_autoawayset);
}
else if(strncmp(buf, "lp_availablecommands ", 21) == 0)
{
buf += 21;
strncpy(lp_availablecommands, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_availablecommands[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_availablecommands ] %s \"%s\"|", botnick, lp_setto, lp_availablecommands);
}
else if(strncmp(buf, "lp_avgdataperuser ", 18) == 0)
{
buf += 18;
strncpy(lp_avgdataperuser, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_avgdataperuser[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_avgdataperuser ] %s \"%s\"|", botnick, lp_setto, lp_avgdataperuser);
}
else if(strncmp(buf, "lp_awayfor ", 11) == 0)
{
buf += 11;
strncpy(lp_awayfor, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_awayfor[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_awayfor ] %s \"%s\"|", botnick, lp_setto, lp_awayfor);
}
else if(strncmp(buf, "lp_awaymodeoff ", 15) == 0)
{
buf += 15;
strncpy(lp_awaymodeoff, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_awaymodeoff[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_awaymodeoff ] %s \"%s\"|", botnick, lp_setto, lp_awaymodeoff);
}
else if(strncmp(buf, "lp_awaymodeon ", 14) == 0)
{
buf += 14;
strncpy(lp_awaymodeon, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_awaymodeon[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_awaymodeon ] %s \"%s\"|", botnick, lp_setto, lp_awaymodeon);
}
else if(strncmp(buf, "lp_badpass ", 11) == 0)
{
buf += 11;
strncpy(lp_badpass, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_badpass[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_badpass ] %s \"%s\"|", botnick, lp_setto, lp_badpass);
}
else if(strncmp(buf, "lp_badsyntax ", 13) == 0)
{
buf += 13;
strncpy(lp_badsyntax, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_badsyntax[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_badsyntax ] %s \"%s\"|", botnick, lp_setto, lp_badsyntax);
}
else if(strncmp(buf, "lp_ban_ ", 8) == 0)
{
buf += 8;
strncpy(lp_ban_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_ban_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_ban_ ] %s \"%s\"|", botnick, lp_setto, lp_ban_);
}
else if(strncmp(buf, "lp_bannedby ", 12) == 0)
{
buf += 12;
strncpy(lp_bannedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_bannedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_bannedby ] %s \"%s\"|", botnick, lp_setto, lp_bannedby);
}
else if(strncmp(buf, "lp_banning_ ", 12) == 0)
{
buf += 12;
strncpy(lp_banning_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_banning_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_banning_ ] %s \"%s\"|", botnick, lp_setto, lp_banning_);
}
else if(strncmp(buf, "lp_banreason ", 13) == 0)
{
buf += 13;
strncpy(lp_banreason, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_banreason[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_banreason ] %s \"%s\"|", botnick, lp_setto, lp_banreason);
}
else if(strncmp(buf, "lp_banworddetected ", 19) == 0)
{
buf += 19;
strncpy(lp_banworddetected, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_banworddetected[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_banworddetected ] %s \"%s\"|", botnick, lp_setto, lp_banworddetected);
}
else if(strncmp(buf, "lp_because ", 11) == 0)
{
buf += 11;
strncpy(lp_because, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_because[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_because ] %s \"%s\"|", botnick, lp_setto, lp_because);
}
else if(strncmp(buf, "lp_blocked_ ", 12) == 0)
{
buf += 12;
strncpy(lp_blocked_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_blocked_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_blocked_ ] %s \"%s\"|", botnick, lp_setto, lp_blocked_);
}
else if(strncmp(buf, "lp_bots_ ", 9) == 0)
{
buf += 9;
strncpy(lp_bots_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_bots_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_bots_ ] %s \"%s\"|", botnick, lp_setto, lp_bots_);
}
else if(strncmp(buf, "lp_bytes ", 9) == 0)
{
buf += 9;
strncpy(lp_bytes, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_bytes[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_bytes ] %s \"%s\"|", botnick, lp_setto, lp_bytes);
}
else if(strncmp(buf, "lp_cannotbeblank ", 17) == 0)
{
buf += 17;
strncpy(lp_cannotbeblank, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_cannotbeblank[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_cannotbeblank ] %s \"%s\"|", botnick, lp_setto, lp_cannotbeblank);
}
else if(strncmp(buf, "lp_canonlypmtoops ", 18) == 0)
{
buf += 18;
strncpy(lp_canonlypmtoops, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_canonlypmtoops[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_canonlypmtoops ] %s \"%s\"|", botnick, lp_setto, lp_canonlypmtoops);
}
else if(strncmp(buf, "lp_categletter_ ", 16) == 0)
{
buf += 16;
strncpy(lp_categletter_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_categletter_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_categletter_ ] %s \"%s\"|", botnick, lp_setto, lp_categletter_);
}
else if(strncmp(buf, "lp_categlist ", 13) == 0)
{
buf += 13;
strncpy(lp_categlist, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_categlist[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_categlist ] %s \"%s\"|", botnick, lp_setto, lp_categlist);
}
else if(strncmp(buf, "lp_categnotfound ", 17) == 0)
{
buf += 17;
strncpy(lp_categnotfound, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_categnotfound[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_categnotfound ] %s \"%s\"|", botnick, lp_setto, lp_categnotfound);
}
else if(strncmp(buf, "lp_category_ ", 13) == 0)
{
buf += 13;
strncpy(lp_category_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_category_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_category_ ] %s \"%s\"|", botnick, lp_setto, lp_category_);
}
else if(strncmp(buf, "lp_charnumbers ", 15) == 0)
{
buf += 15;
strncpy(lp_charnumbers, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_charnumbers[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_charnumbers ] %s \"%s\"|", botnick, lp_setto, lp_charnumbers);
}
else if(strncmp(buf, "lp_chatting_ ", 13) == 0)
{
buf += 13;
strncpy(lp_chatting_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_chatting_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_chatting_ ] %s \"%s\"|", botnick, lp_setto, lp_chatting_);
}
else if(strncmp(buf, "lp_cheefs_ ", 11) == 0)
{
buf += 11;
strncpy(lp_cheefs_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_cheefs_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_cheefs_ ] %s \"%s\"|", botnick, lp_setto, lp_cheefs_);
}
else if(strncmp(buf, "lp_class1 ", 10) == 0)
{
buf += 10;
strncpy(lp_class1, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class1[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class1 ] %s \"%s\"|", botnick, lp_setto, lp_class1);
}
else if(strncmp(buf, "lp_class2 ", 10) == 0)
{
buf += 10;
strncpy(lp_class2, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class2[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class2 ] %s \"%s\"|", botnick, lp_setto, lp_class2);
}
else if(strncmp(buf, "lp_class3 ", 10) == 0)
{
buf += 10;
strncpy(lp_class3, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class3[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class3 ] %s \"%s\"|", botnick, lp_setto, lp_class3);
}
else if(strncmp(buf, "lp_class4 ", 10) == 0)
{
buf += 10;
strncpy(lp_class4, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class4[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class4 ] %s \"%s\"|", botnick, lp_setto, lp_class4);
}
else if(strncmp(buf, "lp_class5 ", 10) == 0)
{
buf += 10;
strncpy(lp_class5, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class5[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class5 ] %s \"%s\"|", botnick, lp_setto, lp_class5);
}
else if(strncmp(buf, "lp_class6 ", 10) == 0)
{
buf += 10;
strncpy(lp_class6, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class6[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class6 ] %s \"%s\"|", botnick, lp_setto, lp_class6);
}
else if(strncmp(buf, "lp_class7 ", 10) == 0)
{
buf += 10;
strncpy(lp_class7, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class7[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class7 ] %s \"%s\"|", botnick, lp_setto, lp_class7);
}
else if(strncmp(buf, "lp_class8 ", 10) == 0)
{
buf += 10;
strncpy(lp_class8, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class8[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class8 ] %s \"%s\"|", botnick, lp_setto, lp_class8);
}
else if(strncmp(buf, "lp_class9 ", 10) == 0)
{
buf += 10;
strncpy(lp_class9, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_class9[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_class9 ] %s \"%s\"|", botnick, lp_setto, lp_class9);
}
else if(strncmp(buf, "lp_client_ ", 11) == 0)
{
buf += 11;
strncpy(lp_client_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_client_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_client_ ] %s \"%s\"|", botnick, lp_setto, lp_client_);
}
else if(strncmp(buf, "lp_commandscalled ", 18) == 0)
{
buf += 18;
strncpy(lp_commandscalled, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_commandscalled[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_commandscalled ] %s \"%s\"|", botnick, lp_setto, lp_commandscalled);
}
else if(strncmp(buf, "lp_connectedsince ", 18) == 0)
{
buf += 18;
strncpy(lp_connectedsince, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_connectedsince[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_connectedsince ] %s \"%s\"|", botnick, lp_setto, lp_connectedsince);
}
else if(strncmp(buf, "lp_connectedusers ", 18) == 0)
{
buf += 18;
strncpy(lp_connectedusers, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_connectedusers[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_connectedusers ] %s \"%s\"|", botnick, lp_setto, lp_connectedusers);
}
else if(strncmp(buf, "lp_connection ", 14) == 0)
{
buf += 14;
strncpy(lp_connection, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_connection[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_connection ] %s \"%s\"|", botnick, lp_setto, lp_connection);
}
else if(strncmp(buf, "lp_couldntaddentry ", 19) == 0)
{
buf += 19;
strncpy(lp_couldntaddentry, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_couldntaddentry[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_couldntaddentry ] %s \"%s\"|", botnick, lp_setto, lp_couldntaddentry);
}
else if(strncmp(buf, "lp_couldntremoveentry ", 22) == 0)
{
buf += 22;
strncpy(lp_couldntremoveentry, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_couldntremoveentry[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_couldntremoveentry ] %s \"%s\"|", botnick, lp_setto, lp_couldntremoveentry);
}
else if(strncmp(buf, "lp_currentdate ", 15) == 0)
{
buf += 15;
strncpy(lp_currentdate, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_currentdate[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_currentdate ] %s \"%s\"|", botnick, lp_setto, lp_currentdate);
}
else if(strncmp(buf, "lp_cut_ ", 8) == 0)
{
buf += 8;
strncpy(lp_cut_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_cut_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_cut_ ] %s \"%s\"|", botnick, lp_setto, lp_cut_);
}
else if(strncmp(buf, "lp_datashared ", 14) == 0)
{
buf += 14;
strncpy(lp_datashared, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_datashared[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_datashared ] %s \"%s\"|", botnick, lp_setto, lp_datashared);
}
else if(strncmp(buf, "lp_days ", 8) == 0)
{
buf += 8;
strncpy(lp_days, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_days[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_days ] %s \"%s\"|", botnick, lp_setto, lp_days);
}
else if(strncmp(buf, "lp_delcommand ", 14) == 0)
{
buf += 14;
strncpy(lp_delcommand, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_delcommand[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_delcommand ] %s \"%s\"|", botnick, lp_setto, lp_delcommand);
}
else if(strncmp(buf, "lp_deletedby ", 13) == 0)
{
buf += 13;
strncpy(lp_deletedby, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_deletedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_deletedby ] %s \"%s\"|", botnick, lp_setto, lp_deletedby);
}
else if(strncmp(buf, "lp_deletedmsg ", 14) == 0)
{
buf += 14;
strncpy(lp_deletedmsg, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_deletedmsg[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_deletedmsg ] %s \"%s\"|", botnick, lp_setto, lp_deletedmsg);
}
else if(strncmp(buf, "lp_deleteduser ", 15) == 0)
{
buf += 15;
strncpy(lp_deleteduser, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_deleteduser[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_deleteduser ] %s \"%s\"|", botnick, lp_setto, lp_deleteduser);
}
else if(strncmp(buf, "lp_denied_ ", 11) == 0)
{
buf += 11;
strncpy(lp_denied_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_denied_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_denied_ ] %s \"%s\"|", botnick, lp_setto, lp_denied_);
}
else if(strncmp(buf, "lp_deniedconnections ", 21) == 0)
{
buf += 21;
strncpy(lp_deniedconnections, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_deniedconnections[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_deniedconnections ] %s \"%s\"|", botnick, lp_setto, lp_deniedconnections);
}
else if(strncmp(buf, "lp_description ", 15) == 0)
{
buf += 15;
strncpy(lp_description, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_description[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_description ] %s \"%s\"|", botnick, lp_setto, lp_description);
}
else if(strncmp(buf, "lp_desctoolong ", 15) == 0)
{
buf += 15;
strncpy(lp_desctoolong, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_desctoolong[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_desctoolong ] %s \"%s\"|", botnick, lp_setto, lp_desctoolong);
}
else if(strncmp(buf, "lp_detectedfrom ", 16) == 0)
{
buf += 16;
strncpy(lp_detectedfrom, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_detectedfrom[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_detectedfrom ] %s \"%s\"|", botnick, lp_setto, lp_detectedfrom);
}
else if(strncmp(buf, "lp_downloading_ ", 16) == 0)
{
buf += 16;
strncpy(lp_downloading_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_downloading_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_downloading_ ] %s \"%s\"|", botnick, lp_setto, lp_downloading_);
}
else if(strncmp(buf, "lp_downloadoff ", 15) == 0)
{
buf += 15;
strncpy(lp_downloadoff, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_downloadoff[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_downloadoff ] %s \"%s\"|", botnick, lp_setto, lp_downloadoff);
}
else if(strncmp(buf, "lp_downloadon ", 14) == 0)
{
buf += 14;
strncpy(lp_downloadon, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_downloadon[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_downloadon ] %s \"%s\"|", botnick, lp_setto, lp_downloadon);
}
else if(strncmp(buf, "lp_downloads ", 13) == 0)
{
buf += 13;
strncpy(lp_downloads, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_downloads[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_downloads ] %s \"%s\"|", botnick, lp_setto, lp_downloads);
}
else if(strncmp(buf, "lp_email ", 9) == 0)
{
buf += 9;
strncpy(lp_email, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_email[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_email ] %s \"%s\"|", botnick, lp_setto, lp_email);
}
else if(strncmp(buf, "lp_entryalreadyonlist ", 22) == 0)
{
buf += 22;
strncpy(lp_entryalreadyonlist, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_entryalreadyonlist[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_entryalreadyonlist ] %s \"%s\"|", botnick, lp_setto, lp_entryalreadyonlist);
}
else if(strncmp(buf, "lp_entrywasntfound ", 19) == 0)
{
buf += 19;
strncpy(lp_entrywasntfound, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_entrywasntfound[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_entrywasntfound ] %s \"%s\"|", botnick, lp_setto, lp_entrywasntfound);
}
else if(strncmp(buf, "lp_erasemsgs ", 13) == 0)
{
buf += 13;
strncpy(lp_erasemsgs, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_erasemsgs[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_erasemsgs ] %s \"%s\"|", botnick, lp_setto, lp_erasemsgs);
}
else if(strncmp(buf, "lp_expires_ ", 12) == 0)
{
buf += 12;
strncpy(lp_expires_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_expires_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_expires_ ] %s \"%s\"|", botnick, lp_setto, lp_expires_);
}
else if(strncmp(buf, "lp_expiresin ", 13) == 0)
{
buf += 13;
strncpy(lp_expiresin, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_expiresin[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_expiresin ] %s \"%s\"|", botnick, lp_setto, lp_expiresin);
}
else if(strncmp(buf, "lp_fakeshare ", 13) == 0)
{
buf += 13;
strncpy(lp_fakeshare, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_fakeshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_fakeshare ] %s \"%s\"|", botnick, lp_setto, lp_fakeshare);
}
else if(strncmp(buf, "lp_faketag ", 11) == 0)
{
buf += 11;
strncpy(lp_faketag, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_faketag[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_faketag ] %s \"%s\"|", botnick, lp_setto, lp_faketag);
}
else if(strncmp(buf, "lp_favsend ", 11) == 0)
{
buf += 11;
strncpy(lp_favsend, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_favsend[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_favsend ] %s \"%s\"|", botnick, lp_setto, lp_favsend);
}
else if(strncmp(buf, "lp_file_ ", 9) == 0)
{
buf += 9;
strncpy(lp_file_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_file_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_file_ ] %s \"%s\"|", botnick, lp_setto, lp_file_);
}
else if(strncmp(buf, "lp_flooding ", 12) == 0)
{
buf += 12;
strncpy(lp_flooding, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_flooding[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_flooding ] %s \"%s\"|", botnick, lp_setto, lp_flooding);
}
else if(strncmp(buf, "lp_floodwarn ", 13) == 0)
{
buf += 13;
strncpy(lp_floodwarn, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_floodwarn[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_floodwarn ] %s \"%s\"|", botnick, lp_setto, lp_floodwarn);
}
else if(strncmp(buf, "lp_forbchar ", 12) == 0)
{
buf += 12;
strncpy(lp_forbchar, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_forbchar[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_forbchar ] %s \"%s\"|", botnick, lp_setto, lp_forbchar);
}
else if(strncmp(buf, "lp_forbiddenclient ", 19) == 0)
{
buf += 19;
strncpy(lp_forbiddenclient, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_forbiddenclient[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_forbiddenclient ] %s \"%s\"|", botnick, lp_setto, lp_forbiddenclient);
}
else if(strncmp(buf, "lp_fornext ", 11) == 0)
{
buf += 11;
strncpy(lp_fornext, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_fornext[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_fornext ] %s \"%s\"|", botnick, lp_setto, lp_fornext);
}
else if(strncmp(buf, "lp_forother ", 12) == 0)
{
buf += 12;
strncpy(lp_forother, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_forother[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_forother ] %s \"%s\"|", botnick, lp_setto, lp_forother);
}
else if(strncmp(buf, "lp_freeslots ", 13) == 0)
{
buf += 13;
strncpy(lp_freeslots, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_freeslots[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_freeslots ] %s \"%s\"|", botnick, lp_setto, lp_freeslots);
}
else if(strncmp(buf, "lp_from_ ", 9) == 0)
{
buf += 9;
strncpy(lp_from_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_from_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_from_ ] %s \"%s\"|", botnick, lp_setto, lp_from_);
}
else if(strncmp(buf, "lp_fulfilment ", 14) == 0)
{
buf += 14;
strncpy(lp_fulfilment, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_fulfilment[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_fulfilment ] %s \"%s\"|", botnick, lp_setto, lp_fulfilment);
}
else if(strncmp(buf, "lp_gaggedfrom ", 14) == 0)
{
buf += 14;
strncpy(lp_gaggedfrom, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_gaggedfrom[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_gaggedfrom ] %s \"%s\"|", botnick, lp_setto, lp_gaggedfrom);
}
else if(strncmp(buf, "lp_gaggingall ", 14) == 0)
{
buf += 14;
strncpy(lp_gaggingall, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_gaggingall[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_gaggingall ] %s \"%s\"|", botnick, lp_setto, lp_gaggingall);
}
else if(strncmp(buf, "lp_gagminshare ", 15) == 0)
{
buf += 15;
strncpy(lp_gagminshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_gagminshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_gagminshare ] %s \"%s\"|", botnick, lp_setto, lp_gagminshare);
}
else if(strncmp(buf, "lp_givingtempop ", 16) == 0)
{
buf += 16;
strncpy(lp_givingtempop, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_givingtempop[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_givingtempop ] %s \"%s\"|", botnick, lp_setto, lp_givingtempop);
}
else if(strncmp(buf, "lp_hasparted ", 13) == 0)
{
buf += 13;
strncpy(lp_hasparted, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hasparted[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hasparted ] %s \"%s\"|", botnick, lp_setto, lp_hasparted);
}
else if(strncmp(buf, "lp_haveregistertochat ", 22) == 0)
{
buf += 22;
strncpy(lp_haveregistertochat, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_haveregistertochat[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_haveregistertochat ] %s \"%s\"|", botnick, lp_setto, lp_haveregistertochat);
}
else if(strncmp(buf, "lp_hi_ ", 7) == 0)
{
buf += 7;
strncpy(lp_hi_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hi_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hi_ ] %s \"%s\"|", botnick, lp_setto, lp_hi_);
}
else if(strncmp(buf, "lp_hidingpresence ", 18) == 0)
{
buf += 18;
strncpy(lp_hidingpresence, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_hidingpresence[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hidingpresence ] %s \"%s\"|", botnick, lp_setto, lp_hidingpresence);
}
else if(strncmp(buf, "lp_hidingshare ", 15) == 0)
{
buf += 15;
strncpy(lp_hidingshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_hidingshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hidingshare ] %s \"%s\"|", botnick, lp_setto, lp_hidingshare);
}
else if(strncmp(buf, "lp_hours ", 9) == 0)
{
buf += 9;
strncpy(lp_hours, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hours[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hours ] %s \"%s\"|", botnick, lp_setto, lp_hours);
}
else if(strncmp(buf, "lp_hubaddr ", 11) == 0)
{
buf += 11;
strncpy(lp_hubaddr, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hubaddr[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubaddr ] %s \"%s\"|", botnick, lp_setto, lp_hubaddr);
}
else if(strncmp(buf, "lp_hubdesc ", 11) == 0)
{
buf += 11;
strncpy(lp_hubdesc, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hubdesc[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubdesc ] %s \"%s\"|", botnick, lp_setto, lp_hubdesc);
}
else if(strncmp(buf, "lp_hubforregistered ", 20) == 0)
{
buf += 20;
strncpy(lp_hubforregistered, buf, (cut_string(buf, '|') > 200) ? 200 : cut_string(buf, '|'));
lp_hubforregistered[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubforregistered ] %s \"%s\"|", botnick, lp_setto, lp_hubforregistered);
}
else if(strncmp(buf, "lp_hubinfo ", 11) == 0)
{
buf += 11;
strncpy(lp_hubinfo, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hubinfo[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubinfo ] %s \"%s\"|", botnick, lp_setto, lp_hubinfo);
}
else if(strncmp(buf, "lp_hubkilled ", 13) == 0)
{
buf += 13;
strncpy(lp_hubkilled, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_hubkilled[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubkilled ] %s \"%s\"|", botnick, lp_setto, lp_hubkilled);
}
else if(strncmp(buf, "lp_hubmassmsg ", 14) == 0)
{
buf += 14;
strncpy(lp_hubmassmsg, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hubmassmsg[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubmassmsg ] %s \"%s\"|", botnick, lp_setto, lp_hubmassmsg);
}
else if(strncmp(buf, "lp_hubname ", 11) == 0)
{
buf += 11;
strncpy(lp_hubname, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hubname[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubname ] %s \"%s\"|", botnick, lp_setto, lp_hubname);
}
else if(strncmp(buf, "lp_hubs_ ", 9) == 0)
{
buf += 9;
strncpy(lp_hubs_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hubs_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubs_ ] %s \"%s\"|", botnick, lp_setto, lp_hubs_);
}
else if(strncmp(buf, "lp_hubsperslot ", 15) == 0)
{
buf += 15;
strncpy(lp_hubsperslot, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_hubsperslot[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_hubsperslot ] %s \"%s\"|", botnick, lp_setto, lp_hubsperslot);
}
else if(strncmp(buf, "lp_ipban_ ", 10) == 0)
{
buf += 10;
strncpy(lp_ipban_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_ipban_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_ipban_ ] %s \"%s\"|", botnick, lp_setto, lp_ipban_);
}
else if(strncmp(buf, "lp_ipbans ", 10) == 0)
{
buf += 10;
strncpy(lp_ipbans, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_ipbans[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_ipbans ] %s \"%s\"|", botnick, lp_setto, lp_ipbans);
}
else if(strncmp(buf, "lp_is_ ", 7) == 0)
{
buf += 7;
strncpy(lp_is_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_is_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_is_ ] %s \"%s\"|", botnick, lp_setto, lp_is_);
}
else if(strncmp(buf, "lp_isalreadyopped ", 18) == 0)
{
buf += 18;
strncpy(lp_isalreadyopped, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_isalreadyopped[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_isalreadyopped ] %s \"%s\"|", botnick, lp_setto, lp_isalreadyopped);
}
else if(strncmp(buf, "lp_isaway ", 10) == 0)
{
buf += 10;
strncpy(lp_isaway, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_isaway[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_isaway ] %s \"%s\"|", botnick, lp_setto, lp_isaway);
}
else if(strncmp(buf, "lp_isback ", 10) == 0)
{
buf += 10;
strncpy(lp_isback, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_isback[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_isback ] %s \"%s\"|", botnick, lp_setto, lp_isback);
}
else if(strncmp(buf, "lp_isbanning ", 13) == 0)
{
buf += 13;
strncpy(lp_isbanning, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_isbanning[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_isbanning ] %s \"%s\"|", botnick, lp_setto, lp_isbanning);
}
else if(strncmp(buf, "lp_iskicking ", 13) == 0)
{
buf += 13;
strncpy(lp_iskicking, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_iskicking[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_iskicking ] %s \"%s\"|", botnick, lp_setto, lp_iskicking);
}
else if(strncmp(buf, "lp_isoffline ", 13) == 0)
{
buf += 13;
strncpy(lp_isoffline, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_isoffline[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_isoffline ] %s \"%s\"|", botnick, lp_setto, lp_isoffline);
}
else if(strncmp(buf, "lp_isonlinenow ", 15) == 0)
{
buf += 15;
strncpy(lp_isonlinenow, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_isonlinenow[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_isonlinenow ] %s \"%s\"|", botnick, lp_setto, lp_isonlinenow);
}
else if(strncmp(buf, "lp_itemstotal ", 14) == 0)
{
buf += 14;
strncpy(lp_itemstotal, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_itemstotal[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_itemstotal ] %s \"%s\"|", botnick, lp_setto, lp_itemstotal);
}
else if(strncmp(buf, "lp_joins ", 9) == 0)
{
buf += 9;
strncpy(lp_joins, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_joins[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_joins ] %s \"%s\"|", botnick, lp_setto, lp_joins);
}
else if(strncmp(buf, "lp_kick_ ", 9) == 0)
{
buf += 9;
strncpy(lp_kick_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_kick_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_kick_ ] %s \"%s\"|", botnick, lp_setto, lp_kick_);
}
else if(strncmp(buf, "lp_kickdeny ", 12) == 0)
{
buf += 12;
strncpy(lp_kickdeny, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_kickdeny[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_kickdeny ] %s \"%s\"|", botnick, lp_setto, lp_kickdeny);
}
else if(strncmp(buf, "lp_kickedby ", 12) == 0)
{
buf += 12;
strncpy(lp_kickedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_kickedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_kickedby ] %s \"%s\"|", botnick, lp_setto, lp_kickedby);
}
else if(strncmp(buf, "lp_kvips_ ", 10) == 0)
{
buf += 10;
strncpy(lp_kvips_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_kvips_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_kvips_ ] %s \"%s\"|", botnick, lp_setto, lp_kvips_);
}
else if(strncmp(buf, "lp_lackofprefix ", 16) == 0)
{
buf += 16;
strncpy(lp_lackofprefix, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_lackofprefix[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lackofprefix ] %s \"%s\"|", botnick, lp_setto, lp_lackofprefix);
}
else if(strncmp(buf, "lp_lackoftag ", 13) == 0)
{
buf += 13;
strncpy(lp_lackoftag, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_lackoftag[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lackoftag ] %s \"%s\"|", botnick, lp_setto, lp_lackoftag);
}
else if(strncmp(buf, "lp_langpackfound ", 17) == 0)
{
buf += 17;
strncpy(lp_langpackfound, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_langpackfound[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_langpackfound ] %s \"%s\"|", botnick, lp_setto, lp_langpackfound);
}
else if(strncmp(buf, "lp_lastmainmsgs ", 16) == 0)
{
buf += 16;
strncpy(lp_lastmainmsgs, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_lastmainmsgs[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lastmainmsgs ] %s \"%s\"|", botnick, lp_setto, lp_lastmainmsgs);
}
else if(strncmp(buf, "lp_lastpermbans ", 16) == 0)
{
buf += 16;
strncpy(lp_lastpermbans, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_lastpermbans[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lastpermbans ] %s \"%s\"|", botnick, lp_setto, lp_lastpermbans);
}
else if(strncmp(buf, "lp_lastsearch ", 14) == 0)
{
buf += 14;
strncpy(lp_lastsearch, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_lastsearch[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lastsearch ] %s \"%s\"|", botnick, lp_setto, lp_lastsearch);
}
else if(strncmp(buf, "lp_lastsendpm ", 14) == 0)
{
buf += 14;
strncpy(lp_lastsendpm, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_lastsendpm[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lastsendpm ] %s \"%s\"|", botnick, lp_setto, lp_lastsendpm);
}
else if(strncmp(buf, "lp_lasttempbans ", 16) == 0)
{
buf += 16;
strncpy(lp_lasttempbans, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_lasttempbans[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lasttempbans ] %s \"%s\"|", botnick, lp_setto, lp_lasttempbans);
}
else if(strncmp(buf, "lp_lastwritetomain ", 19) == 0)
{
buf += 19;
strncpy(lp_lastwritetomain, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_lastwritetomain[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_lastwritetomain ] %s \"%s\"|", botnick, lp_setto, lp_lastwritetomain);
}
else if(strncmp(buf, "lp_listofgagged ", 16) == 0)
{
buf += 16;
strncpy(lp_listofgagged, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_listofgagged[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_listofgagged ] %s \"%s\"|", botnick, lp_setto, lp_listofgagged);
}
else if(strncmp(buf, "lp_loggedin ", 12) == 0)
{
buf += 12;
strncpy(lp_loggedin, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_loggedin[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_loggedin ] %s \"%s\"|", botnick, lp_setto, lp_loggedin);
}
else if(strncmp(buf, "lp_longestconnected ", 20) == 0)
{
buf += 20;
strncpy(lp_longestconnected, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_longestconnected[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_longestconnected ] %s \"%s\"|", botnick, lp_setto, lp_longestconnected);
}
else if(strncmp(buf, "lp_mailtoolong ", 15) == 0)
{
buf += 15;
strncpy(lp_mailtoolong, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_mailtoolong[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_mailtoolong ] %s \"%s\"|", botnick, lp_setto, lp_mailtoolong);
}
else if(strncmp(buf, "lp_mainchatlocked ", 18) == 0)
{
buf += 18;
strncpy(lp_mainchatlocked, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_mainchatlocked[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_mainchatlocked ] %s \"%s\"|", botnick, lp_setto, lp_mainchatlocked);
}
else if(strncmp(buf, "lp_mainchatmsgs ", 16) == 0)
{
buf += 16;
strncpy(lp_mainchatmsgs, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_mainchatmsgs[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_mainchatmsgs ] %s \"%s\"|", botnick, lp_setto, lp_mainchatmsgs);
}
else if(strncmp(buf, "lp_mainchatoff ", 15) == 0)
{
buf += 15;
strncpy(lp_mainchatoff, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_mainchatoff[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_mainchatoff ] %s \"%s\"|", botnick, lp_setto, lp_mainchatoff);
}
else if(strncmp(buf, "lp_mainchaton ", 14) == 0)
{
buf += 14;
strncpy(lp_mainchaton, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_mainchaton[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_mainchaton ] %s \"%s\"|", botnick, lp_setto, lp_mainchaton);
}
else if(strncmp(buf, "lp_masssend ", 12) == 0)
{
buf += 12;
strncpy(lp_masssend, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_masssend[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_masssend ] %s \"%s\"|", botnick, lp_setto, lp_masssend);
}
else if(strncmp(buf, "lp_maxhubs ", 11) == 0)
{
buf += 11;
strncpy(lp_maxhubs, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_maxhubs[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_maxhubs ] %s \"%s\"|", botnick, lp_setto, lp_maxhubs);
}
else if(strncmp(buf, "lp_maxshare ", 12) == 0)
{
buf += 12;
strncpy(lp_maxshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_maxshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_maxshare ] %s \"%s\"|", botnick, lp_setto, lp_maxshare);
}
else if(strncmp(buf, "lp_maxslots ", 12) == 0)
{
buf += 12;
strncpy(lp_maxslots, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_maxslots[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_maxslots ] %s \"%s\"|", botnick, lp_setto, lp_maxslots);
}
else if(strncmp(buf, "lp_maxusers ", 12) == 0)
{
buf += 12;
strncpy(lp_maxusers, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_maxusers[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_maxusers ] %s \"%s\"|", botnick, lp_setto, lp_maxusers);
}
else if(strncmp(buf, "lp_message_ ", 12) == 0)
{
buf += 12;
strncpy(lp_message_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_message_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_message_ ] %s \"%s\"|", botnick, lp_setto, lp_message_);
}
else if(strncmp(buf, "lp_minactshare ", 15) == 0)
{
buf += 15;
strncpy(lp_minactshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_minactshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minactshare ] %s \"%s\"|", botnick, lp_setto, lp_minactshare);
}
else if(strncmp(buf, "lp_mindcppver ", 14) == 0)
{
buf += 14;
strncpy(lp_mindcppver, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_mindcppver[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_mindcppver ] %s \"%s\"|", botnick, lp_setto, lp_mindcppver);
}
else if(strncmp(buf, "lp_minkvipshare ", 16) == 0)
{
buf += 16;
strncpy(lp_minkvipshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_minkvipshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minkvipshare ] %s \"%s\"|", botnick, lp_setto, lp_minkvipshare);
}
else if(strncmp(buf, "lp_minopshare ", 14) == 0)
{
buf += 14;
strncpy(lp_minopshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_minopshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minopshare ] %s \"%s\"|", botnick, lp_setto, lp_minopshare);
}
else if(strncmp(buf, "lp_minpsvshare ", 15) == 0)
{
buf += 15;
strncpy(lp_minpsvshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_minpsvshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minpsvshare ] %s \"%s\"|", botnick, lp_setto, lp_minpsvshare);
}
else if(strncmp(buf, "lp_minregshare ", 15) == 0)
{
buf += 15;
strncpy(lp_minregshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_minregshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minregshare ] %s \"%s\"|", botnick, lp_setto, lp_minregshare);
}
else if(strncmp(buf, "lp_minshare ", 12) == 0)
{
buf += 12;
strncpy(lp_minshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_minshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minshare ] %s \"%s\"|", botnick, lp_setto, lp_minshare);
}
else if(strncmp(buf, "lp_minslots ", 12) == 0)
{
buf += 12;
strncpy(lp_minslots, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_minslots[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minslots ] %s \"%s\"|", botnick, lp_setto, lp_minslots);
}
else if(strncmp(buf, "lp_minvipshare ", 15) == 0)
{
buf += 15;
strncpy(lp_minvipshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_minvipshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minvipshare ] %s \"%s\"|", botnick, lp_setto, lp_minvipshare);
}
else if(strncmp(buf, "lp_minutes ", 11) == 0)
{
buf += 11;
strncpy(lp_minutes, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_minutes[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_minutes ] %s \"%s\"|", botnick, lp_setto, lp_minutes);
}
else if(strncmp(buf, "lp_mode_ ", 9) == 0)
{
buf += 9;
strncpy(lp_mode_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_mode_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_mode_ ] %s \"%s\"|", botnick, lp_setto, lp_mode_);
}
else if(strncmp(buf, "lp_msgforyou ", 13) == 0)
{
buf += 13;
strncpy(lp_msgforyou, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_msgforyou[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_msgforyou ] %s \"%s\"|", botnick, lp_setto, lp_msgforyou);
}
else if(strncmp(buf, "lp_musthavedot ", 15) == 0)
{
buf += 15;
strncpy(lp_musthavedot, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_musthavedot[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_musthavedot ] %s \"%s\"|", botnick, lp_setto, lp_musthavedot);
}
else if(strncmp(buf, "lp_name_ ", 9) == 0)
{
buf += 9;
strncpy(lp_name_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_name_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_name_ ] %s \"%s\"|", botnick, lp_setto, lp_name_);
}
else if(strncmp(buf, "lp_never_ ", 10) == 0)
{
buf += 10;
strncpy(lp_never_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_never_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_never_ ] %s \"%s\"|", botnick, lp_setto, lp_never_);
}
else if(strncmp(buf, "lp_newcommand ", 14) == 0)
{
buf += 14;
strncpy(lp_newcommand, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_newcommand[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_newcommand ] %s \"%s\"|", botnick, lp_setto, lp_newcommand);
}
else if(strncmp(buf, "lp_newregistered ", 17) == 0)
{
buf += 17;
strncpy(lp_newregistered, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_newregistered[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_newregistered ] %s \"%s\"|", botnick, lp_setto, lp_newregistered);
}
else if(strncmp(buf, "lp_nextwarningban ", 18) == 0)
{
buf += 18;
strncpy(lp_nextwarningban, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nextwarningban[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nextwarningban ] %s \"%s\"|", botnick, lp_setto, lp_nextwarningban);
}
else if(strncmp(buf, "lp_nick_ ", 9) == 0)
{
buf += 9;
strncpy(lp_nick_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_nick_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nick_ ] %s \"%s\"|", botnick, lp_setto, lp_nick_);
}
else if(strncmp(buf, "lp_nickban_ ", 12) == 0)
{
buf += 12;
strncpy(lp_nickban_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_nickban_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nickban_ ] %s \"%s\"|", botnick, lp_setto, lp_nickban_);
}
else if(strncmp(buf, "lp_nickbans ", 12) == 0)
{
buf += 12;
strncpy(lp_nickbans, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_nickbans[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nickbans ] %s \"%s\"|", botnick, lp_setto, lp_nickbans);
}
else if(strncmp(buf, "lp_nickchar ", 12) == 0)
{
buf += 12;
strncpy(lp_nickchar, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nickchar[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nickchar ] %s \"%s\"|", botnick, lp_setto, lp_nickchar);
}
else if(strncmp(buf, "lp_nickinuse ", 13) == 0)
{
buf += 13;
strncpy(lp_nickinuse, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nickinuse[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nickinuse ] %s \"%s\"|", botnick, lp_setto, lp_nickinuse);
}
else if(strncmp(buf, "lp_nickname ", 12) == 0)
{
buf += 12;
strncpy(lp_nickname, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_nickname[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nickname ] %s \"%s\"|", botnick, lp_setto, lp_nickname);
}
else if(strncmp(buf, "lp_nicktooshort ", 16) == 0)
{
buf += 16;
strncpy(lp_nicktooshort, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nicktooshort[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nicktooshort ] %s \"%s\"|", botnick, lp_setto, lp_nicktooshort);
}
else if(strncmp(buf, "lp_nomsgtodelete ", 17) == 0)
{
buf += 17;
strncpy(lp_nomsgtodelete, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nomsgtodelete[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nomsgtodelete ] %s \"%s\"|", botnick, lp_setto, lp_nomsgtodelete);
}
else if(strncmp(buf, "lp_nonewlines ", 14) == 0)
{
buf += 14;
strncpy(lp_nonewlines, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nonewlines[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nonewlines ] %s \"%s\"|", botnick, lp_setto, lp_nonewlines);
}
else if(strncmp(buf, "lp_nonewmsg ", 12) == 0)
{
buf += 12;
strncpy(lp_nonewmsg, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nonewmsg[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nonewmsg ] %s \"%s\"|", botnick, lp_setto, lp_nonewmsg);
}
else if(strncmp(buf, "lp_nopass ", 10) == 0)
{
buf += 10;
strncpy(lp_nopass, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nopass[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nopass ] %s \"%s\"|", botnick, lp_setto, lp_nopass);
}
else if(strncmp(buf, "lp_nopermission ", 16) == 0)
{
buf += 16;
strncpy(lp_nopermission, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nopermission[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nopermission ] %s \"%s\"|", botnick, lp_setto, lp_nopermission);
}
else if(strncmp(buf, "lp_normal_ ", 11) == 0)
{
buf += 11;
strncpy(lp_normal_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_normal_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_normal_ ] %s \"%s\"|", botnick, lp_setto, lp_normal_);
}
else if(strncmp(buf, "lp_nosuchfile ", 14) == 0)
{
buf += 14;
strncpy(lp_nosuchfile, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_nosuchfile[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nosuchfile ] %s \"%s\"|", botnick, lp_setto, lp_nosuchfile);
}
else if(strncmp(buf, "lp_nosuchvariable ", 18) == 0)
{
buf += 18;
strncpy(lp_nosuchvariable, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_nosuchvariable[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nosuchvariable ] %s \"%s\"|", botnick, lp_setto, lp_nosuchvariable);
}
else if(strncmp(buf, "lp_notallowedtoadd ", 19) == 0)
{
buf += 19;
strncpy(lp_notallowedtoadd, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_notallowedtoadd[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_notallowedtoadd ] %s \"%s\"|", botnick, lp_setto, lp_notallowedtoadd);
}
else if(strncmp(buf, "lp_notallowedtochat ", 20) == 0)
{
buf += 20;
strncpy(lp_notallowedtochat, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_notallowedtochat[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_notallowedtochat ] %s \"%s\"|", botnick, lp_setto, lp_notallowedtochat);
}
else if(strncmp(buf, "lp_notallowedtodelreg ", 22) == 0)
{
buf += 22;
strncpy(lp_notallowedtodelreg, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_notallowedtodelreg[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_notallowedtodelreg ] %s \"%s\"|", botnick, lp_setto, lp_notallowedtodelreg);
}
else if(strncmp(buf, "lp_notallowedtosee ", 19) == 0)
{
buf += 19;
strncpy(lp_notallowedtosee, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_notallowedtosee[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_notallowedtosee ] %s \"%s\"|", botnick, lp_setto, lp_notallowedtosee);
}
else if(strncmp(buf, "lp_notenoughslots ", 18) == 0)
{
buf += 18;
strncpy(lp_notenoughslots, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_notenoughslots[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_notenoughslots ] %s \"%s\"|", botnick, lp_setto, lp_notenoughslots);
}
else if(strncmp(buf, "lp_nowarnings ", 14) == 0)
{
buf += 14;
strncpy(lp_nowarnings, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_nowarnings[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_nowarnings ] %s \"%s\"|", botnick, lp_setto, lp_nowarnings);
}
else if(strncmp(buf, "lp_number_ ", 11) == 0)
{
buf += 11;
strncpy(lp_number_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_number_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_number_ ] %s \"%s\"|", botnick, lp_setto, lp_number_);
}
else if(strncmp(buf, "lp_opped_ ", 10) == 0)
{
buf += 10;
strncpy(lp_opped_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_opped_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_opped_ ] %s \"%s\"|", botnick, lp_setto, lp_opped_);
}
else if(strncmp(buf, "lp_ops_ ", 8) == 0)
{
buf += 8;
strncpy(lp_ops_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_ops_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_ops_ ] %s \"%s\"|", botnick, lp_setto, lp_ops_);
}
else if(strncmp(buf, "lp_owners_ ", 11) == 0)
{
buf += 11;
strncpy(lp_owners_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_owners_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_owners_ ] %s \"%s\"|", botnick, lp_setto, lp_owners_);
}
else if(strncmp(buf, "lp_pass_ ", 9) == 0)
{
buf += 9;
strncpy(lp_pass_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_pass_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_pass_ ] %s \"%s\"|", botnick, lp_setto, lp_pass_);
}
else if(strncmp(buf, "lp_passchanged ", 15) == 0)
{
buf += 15;
strncpy(lp_passchanged, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_passchanged[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_passchanged ] %s \"%s\"|", botnick, lp_setto, lp_passchanged);
}
else if(strncmp(buf, "lp_passive_ ", 12) == 0)
{
buf += 12;
strncpy(lp_passive_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_passive_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_passive_ ] %s \"%s\"|", botnick, lp_setto, lp_passive_);
}
else if(strncmp(buf, "lp_passwordincorrect ", 21) == 0)
{
buf += 21;
strncpy(lp_passwordincorrect, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_passwordincorrect[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_passwordincorrect ] %s \"%s\"|", botnick, lp_setto, lp_passwordincorrect);
}
else if(strncmp(buf, "lp_passwordrequired ", 20) == 0)
{
buf += 20;
strncpy(lp_passwordrequired, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_passwordrequired[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_passwordrequired ] %s \"%s\"|", botnick, lp_setto, lp_passwordrequired);
}
else if(strncmp(buf, "lp_permanent_ ", 14) == 0)
{
buf += 14;
strncpy(lp_permanent_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_permanent_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_permanent_ ] %s \"%s\"|", botnick, lp_setto, lp_permanent_);
}
else if(strncmp(buf, "lp_permanently ", 15) == 0)
{
buf += 15;
strncpy(lp_permanently, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_permanently[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_permanently ] %s \"%s\"|", botnick, lp_setto, lp_permanently);
}
else if(strncmp(buf, "lp_permbans ", 12) == 0)
{
buf += 12;
strncpy(lp_permbans, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_permbans[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_permbans ] %s \"%s\"|", botnick, lp_setto, lp_permbans);
}
else if(strncmp(buf, "lp_pmmsgs ", 10) == 0)
{
buf += 10;
strncpy(lp_pmmsgs, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_pmmsgs[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_pmmsgs ] %s \"%s\"|", botnick, lp_setto, lp_pmmsgs);
}
else if(strncmp(buf, "lp_pmoff ", 9) == 0)
{
buf += 9;
strncpy(lp_pmoff, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_pmoff[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_pmoff ] %s \"%s\"|", botnick, lp_setto, lp_pmoff);
}
else if(strncmp(buf, "lp_pmon ", 8) == 0)
{
buf += 8;
strncpy(lp_pmon, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_pmon[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_pmon ] %s \"%s\"|", botnick, lp_setto, lp_pmon);
}
else if(strncmp(buf, "lp_presencealreadyhidden ", 25) == 0)
{
buf += 25;
strncpy(lp_presencealreadyhidden, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_presencealreadyhidden[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_presencealreadyhidden ] %s \"%s\"|", botnick, lp_setto, lp_presencealreadyhidden);
}
else if(strncmp(buf, "lp_presencehidden ", 18) == 0)
{
buf += 18;
strncpy(lp_presencehidden, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_presencehidden[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_presencehidden ] %s \"%s\"|", botnick, lp_setto, lp_presencehidden);
}
else if(strncmp(buf, "lp_protectmepass ", 17) == 0)
{
buf += 17;
strncpy(lp_protectmepass, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_protectmepass[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_protectmepass ] %s \"%s\"|", botnick, lp_setto, lp_protectmepass);
}
else if(strncmp(buf, "lp_prots_ ", 10) == 0)
{
buf += 10;
strncpy(lp_prots_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_prots_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_prots_ ] %s \"%s\"|", botnick, lp_setto, lp_prots_);
}
else if(strncmp(buf, "lp_prottime ", 12) == 0)
{
buf += 12;
strncpy(lp_prottime, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_prottime[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_prottime ] %s \"%s\"|", botnick, lp_setto, lp_prottime);
}
else if(strncmp(buf, "lp_prottype ", 12) == 0)
{
buf += 12;
strncpy(lp_prottype, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_prottype[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_prottype ] %s \"%s\"|", botnick, lp_setto, lp_prottype);
}
else if(strncmp(buf, "lp_proxy_ ", 10) == 0)
{
buf += 10;
strncpy(lp_proxy_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_proxy_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_proxy_ ] %s \"%s\"|", botnick, lp_setto, lp_proxy_);
}
else if(strncmp(buf, "lp_proxynotallowed ", 19) == 0)
{
buf += 19;
strncpy(lp_proxynotallowed, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_proxynotallowed[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_proxynotallowed ] %s \"%s\"|", botnick, lp_setto, lp_proxynotallowed);
}
else if(strncmp(buf, "lp_psvnotallowed ", 17) == 0)
{
buf += 17;
strncpy(lp_psvnotallowed, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_psvnotallowed[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_psvnotallowed ] %s \"%s\"|", botnick, lp_setto, lp_psvnotallowed);
}
else if(strncmp(buf, "lp_psvwithmodea ", 16) == 0)
{
buf += 16;
strncpy(lp_psvwithmodea, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_psvwithmodea[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_psvwithmodea ] %s \"%s\"|", botnick, lp_setto, lp_psvwithmodea);
}
else if(strncmp(buf, "lp_psvwithoutpsvinnick ", 23) == 0)
{
buf += 23;
strncpy(lp_psvwithoutpsvinnick, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_psvwithoutpsvinnick[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_psvwithoutpsvinnick ] %s \"%s\"|", botnick, lp_setto, lp_psvwithoutpsvinnick);
}
else if(strncmp(buf, "lp_quits ", 9) == 0)
{
buf += 9;
strncpy(lp_quits, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_quits[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_quits ] %s \"%s\"|", botnick, lp_setto, lp_quits);
}
else if(strncmp(buf, "lp_rawadm ", 10) == 0)
{
buf += 10;
strncpy(lp_rawadm, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_rawadm[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_rawadm ] %s \"%s\"|", botnick, lp_setto, lp_rawadm);
}
else if(strncmp(buf, "lp_rawop ", 9) == 0)
{
buf += 9;
strncpy(lp_rawop, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_rawop[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_rawop ] %s \"%s\"|", botnick, lp_setto, lp_rawop);
}
else if(strncmp(buf, "lp_rawuser ", 11) == 0)
{
buf += 11;
strncpy(lp_rawuser, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_rawuser[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_rawuser ] %s \"%s\"|", botnick, lp_setto, lp_rawuser);
}
else if(strncmp(buf, "lp_reason ", 10) == 0)
{
buf += 10;
strncpy(lp_reason, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_reason[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_reason ] %s \"%s\"|", botnick, lp_setto, lp_reason);
}
else if(strncmp(buf, "lp_recentlyadded ", 17) == 0)
{
buf += 17;
strncpy(lp_recentlyadded, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_recentlyadded[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_recentlyadded ] %s \"%s\"|", botnick, lp_setto, lp_recentlyadded);
}
else if(strncmp(buf, "lp_recordresetted ", 18) == 0)
{
buf += 18;
strncpy(lp_recordresetted, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_recordresetted[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_recordresetted ] %s \"%s\"|", botnick, lp_setto, lp_recordresetted);
}
else if(strncmp(buf, "lp_recordshare ", 15) == 0)
{
buf += 15;
strncpy(lp_recordshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_recordshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_recordshare ] %s \"%s\"|", botnick, lp_setto, lp_recordshare);
}
else if(strncmp(buf, "lp_recordusers ", 15) == 0)
{
buf += 15;
strncpy(lp_recordusers, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_recordusers[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_recordusers ] %s \"%s\"|", botnick, lp_setto, lp_recordusers);
}
else if(strncmp(buf, "lp_redir_ ", 10) == 0)
{
buf += 10;
strncpy(lp_redir_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_redir_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_redir_ ] %s \"%s\"|", botnick, lp_setto, lp_redir_);
}
else if(strncmp(buf, "lp_redirectingall ", 18) == 0)
{
buf += 18;
strncpy(lp_redirectingall, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_redirectingall[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_redirectingall ] %s \"%s\"|", botnick, lp_setto, lp_redirectingall);
}
else if(strncmp(buf, "lp_registered_ ", 15) == 0)
{
buf += 15;
strncpy(lp_registered_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_registered_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_registered_ ] %s \"%s\"|", botnick, lp_setto, lp_registered_);
}
else if(strncmp(buf, "lp_registeredby ", 16) == 0)
{
buf += 16;
strncpy(lp_registeredby, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_registeredby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_registeredby ] %s \"%s\"|", botnick, lp_setto, lp_registeredby);
}
else if(strncmp(buf, "lp_registrationfailed ", 22) == 0)
{
buf += 22;
strncpy(lp_registrationfailed, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_registrationfailed[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_registrationfailed ] %s \"%s\"|", botnick, lp_setto, lp_registrationfailed);
}
else if(strncmp(buf, "lp_registrationrules ", 21) == 0)
{
buf += 21;
strncpy(lp_registrationrules, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_registrationrules[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_registrationrules ] %s \"%s\"|", botnick, lp_setto, lp_registrationrules);
}
else if(strncmp(buf, "lp_regmepass ", 13) == 0)
{
buf += 13;
strncpy(lp_regmepass, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_regmepass[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_regmepass ] %s \"%s\"|", botnick, lp_setto, lp_regmepass);
}
else if(strncmp(buf, "lp_regonly ", 11) == 0)
{
buf += 11;
strncpy(lp_regonly, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_regonly[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_regonly ] %s \"%s\"|", botnick, lp_setto, lp_regonly);
}
else if(strncmp(buf, "lp_regs_ ", 9) == 0)
{
buf += 9;
strncpy(lp_regs_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_regs_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_regs_ ] %s \"%s\"|", botnick, lp_setto, lp_regs_);
}
else if(strncmp(buf, "lp_regular_ ", 12) == 0)
{
buf += 12;
strncpy(lp_regular_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_regular_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_regular_ ] %s \"%s\"|", botnick, lp_setto, lp_regular_);
}
else if(strncmp(buf, "lp_releases ", 12) == 0)
{
buf += 12;
strncpy(lp_releases, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_releases[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_releases ] %s \"%s\"|", botnick, lp_setto, lp_releases);
}
else if(strncmp(buf, "lp_reloadingscripts ", 20) == 0)
{
buf += 20;
strncpy(lp_reloadingscripts, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_reloadingscripts[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_reloadingscripts ] %s \"%s\"|", botnick, lp_setto, lp_reloadingscripts);
}
else if(strncmp(buf, "lp_removedallwarns ", 19) == 0)
{
buf += 19;
strncpy(lp_removedallwarns, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_removedallwarns[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_removedallwarns ] %s \"%s\"|", botnick, lp_setto, lp_removedallwarns);
}
else if(strncmp(buf, "lp_removedentryfrom ", 20) == 0)
{
buf += 20;
strncpy(lp_removedentryfrom, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_removedentryfrom[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_removedentryfrom ] %s \"%s\"|", botnick, lp_setto, lp_removedentryfrom);
}
else if(strncmp(buf, "lp_reportedby ", 14) == 0)
{
buf += 14;
strncpy(lp_reportedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_reportedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_reportedby ] %s \"%s\"|", botnick, lp_setto, lp_reportedby);
}
else if(strncmp(buf, "lp_reportsend ", 14) == 0)
{
buf += 14;
strncpy(lp_reportsend, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_reportsend[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_reportsend ] %s \"%s\"|", botnick, lp_setto, lp_reportsend);
}
else if(strncmp(buf, "lp_requests ", 12) == 0)
{
buf += 12;
strncpy(lp_requests, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_requests[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_requests ] %s \"%s\"|", botnick, lp_setto, lp_requests);
}
else if(strncmp(buf, "lp_requiredatleast ", 19) == 0)
{
buf += 19;
strncpy(lp_requiredatleast, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_requiredatleast[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_requiredatleast ] %s \"%s\"|", botnick, lp_setto, lp_requiredatleast);
}
else if(strncmp(buf, "lp_restrictions1 ", 17) == 0)
{
buf += 17;
strncpy(lp_restrictions1, buf, (cut_string(buf, '|') > 200) ? 200 : cut_string(buf, '|'));
lp_restrictions1[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_restrictions1 ] %s \"%s\"|", botnick, lp_setto, lp_restrictions1);
}
else if(strncmp(buf, "lp_restrictions2 ", 17) == 0)
{
buf += 17;
strncpy(lp_restrictions2, buf, (cut_string(buf, '|') > 200) ? 200 : cut_string(buf, '|'));
lp_restrictions2[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_restrictions2 ] %s \"%s\"|", botnick, lp_setto, lp_restrictions2);
}
else if(strncmp(buf, "lp_savedmsg ", 12) == 0)
{
buf += 12;
strncpy(lp_savedmsg, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_savedmsg[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_savedmsg ] %s \"%s\"|", botnick, lp_setto, lp_savedmsg);
}
else if(strncmp(buf, "lp_searches ", 12) == 0)
{
buf += 12;
strncpy(lp_searches, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_searches[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searches ] %s \"%s\"|", botnick, lp_setto, lp_searches);
}
else if(strncmp(buf, "lp_searchforip ", 15) == 0)
{
buf += 15;
strncpy(lp_searchforip, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_searchforip[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searchforip ] %s \"%s\"|", botnick, lp_setto, lp_searchforip);
}
else if(strncmp(buf, "lp_searchignored ", 17) == 0)
{
buf += 17;
strncpy(lp_searchignored, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_searchignored[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searchignored ] %s \"%s\"|", botnick, lp_setto, lp_searchignored);
}
else if(strncmp(buf, "lp_searching_ ", 14) == 0)
{
buf += 14;
strncpy(lp_searching_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_searching_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searching_ ] %s \"%s\"|", botnick, lp_setto, lp_searching_);
}
else if(strncmp(buf, "lp_searchoff ", 13) == 0)
{
buf += 13;
strncpy(lp_searchoff, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_searchoff[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searchoff ] %s \"%s\"|", botnick, lp_setto, lp_searchoff);
}
else if(strncmp(buf, "lp_searchon ", 12) == 0)
{
buf += 12;
strncpy(lp_searchon, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_searchon[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searchon ] %s \"%s\"|", botnick, lp_setto, lp_searchon);
}
else if(strncmp(buf, "lp_searchresults ", 17) == 0)
{
buf += 17;
strncpy(lp_searchresults, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_searchresults[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searchresults ] %s \"%s\"|", botnick, lp_setto, lp_searchresults);
}
else if(strncmp(buf, "lp_searchspam ", 14) == 0)
{
buf += 14;
strncpy(lp_searchspam, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_searchspam[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searchspam ] %s \"%s\"|", botnick, lp_setto, lp_searchspam);
}
else if(strncmp(buf, "lp_searchtooshort ", 18) == 0)
{
buf += 18;
strncpy(lp_searchtooshort, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_searchtooshort[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_searchtooshort ] %s \"%s\"|", botnick, lp_setto, lp_searchtooshort);
}
else if(strncmp(buf, "lp_seconds ", 11) == 0)
{
buf += 11;
strncpy(lp_seconds, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_seconds[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_seconds ] %s \"%s\"|", botnick, lp_setto, lp_seconds);
}
else if(strncmp(buf, "lp_selfregistrationdisabled ", 28) == 0)
{
buf += 28;
strncpy(lp_selfregistrationdisabled, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_selfregistrationdisabled[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_selfregistrationdisabled ] %s \"%s\"|", botnick, lp_setto, lp_selfregistrationdisabled);
}
else if(strncmp(buf, "lp_setby ", 9) == 0)
{
buf += 9;
strncpy(lp_setby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_setby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_setby ] %s \"%s\"|", botnick, lp_setto, lp_setby);
}
else if(strncmp(buf, "lp_setto ", 9) == 0)
{
buf += 9;
strncpy(lp_setto, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_setto[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_setto ] %s \"%s\"|", botnick, lp_setto, lp_setto);
}
else if(strncmp(buf, "lp_share_ ", 10) == 0)
{
buf += 10;
strncpy(lp_share_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_share_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_share_ ] %s \"%s\"|", botnick, lp_setto, lp_share_);
}
else if(strncmp(buf, "lp_sharesize ", 13) == 0)
{
buf += 13;
strncpy(lp_sharesize, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_sharesize[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_sharesize ] %s \"%s\"|", botnick, lp_setto, lp_sharesize);
}
else if(strncmp(buf, "lp_shouldbechecked ", 19) == 0)
{
buf += 19;
strncpy(lp_shouldbechecked, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_shouldbechecked[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_shouldbechecked ] %s \"%s\"|", botnick, lp_setto, lp_shouldbechecked);
}
else if(strncmp(buf, "lp_since ", 9) == 0)
{
buf += 9;
strncpy(lp_since, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_since[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_since ] %s \"%s\"|", botnick, lp_setto, lp_since);
}
else if(strncmp(buf, "lp_slotbegging ", 15) == 0)
{
buf += 15;
strncpy(lp_slotbegging, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_slotbegging[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_slotbegging ] %s \"%s\"|", botnick, lp_setto, lp_slotbegging);
}
else if(strncmp(buf, "lp_slotratio ", 13) == 0)
{
buf += 13;
strncpy(lp_slotratio, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_slotratio[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_slotratio ] %s \"%s\"|", botnick, lp_setto, lp_slotratio);
}
else if(strncmp(buf, "lp_slots_ ", 10) == 0)
{
buf += 10;
strncpy(lp_slots_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_slots_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_slots_ ] %s \"%s\"|", botnick, lp_setto, lp_slots_);
}
else if(strncmp(buf, "lp_slotsperhub ", 15) == 0)
{
buf += 15;
strncpy(lp_slotsperhub, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_slotsperhub[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_slotsperhub ] %s \"%s\"|", botnick, lp_setto, lp_slotsperhub);
}
else if(strncmp(buf, "lp_spacesinnick ", 16) == 0)
{
buf += 16;
strncpy(lp_spacesinnick, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_spacesinnick[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_spacesinnick ] %s \"%s\"|", botnick, lp_setto, lp_spacesinnick);
}
else if(strncmp(buf, "lp_spamming ", 12) == 0)
{
buf += 12;
strncpy(lp_spamming, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_spamming[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_spamming ] %s \"%s\"|", botnick, lp_setto, lp_spamming);
}
else if(strncmp(buf, "lp_statistics ", 14) == 0)
{
buf += 14;
strncpy(lp_statistics, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_statistics[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_statistics ] %s \"%s\"|", botnick, lp_setto, lp_statistics);
}
else if(strncmp(buf, "lp_tag_ ", 8) == 0)
{
buf += 8;
strncpy(lp_tag_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_tag_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_tag_ ] %s \"%s\"|", botnick, lp_setto, lp_tag_);
}
else if(strncmp(buf, "lp_tempbans ", 12) == 0)
{
buf += 12;
strncpy(lp_tempbans, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_tempbans[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_tempbans ] %s \"%s\"|", botnick, lp_setto, lp_tempbans);
}
else if(strncmp(buf, "lp_tempkickban ", 15) == 0)
{
buf += 15;
strncpy(lp_tempkickban, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_tempkickban[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_tempkickban ] %s \"%s\"|", botnick, lp_setto, lp_tempkickban);
}
else if(strncmp(buf, "lp_temporary_ ", 14) == 0)
{
buf += 14;
strncpy(lp_temporary_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_temporary_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_temporary_ ] %s \"%s\"|", botnick, lp_setto, lp_temporary_);
}
else if(strncmp(buf, "lp_text_ ", 9) == 0)
{
buf += 9;
strncpy(lp_text_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_text_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_text_ ] %s \"%s\"|", botnick, lp_setto, lp_text_);
}
else if(strncmp(buf, "lp_thesameip ", 13) == 0)
{
buf += 13;
strncpy(lp_thesameip, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_thesameip[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_thesameip ] %s \"%s\"|", botnick, lp_setto, lp_thesameip);
}
else if(strncmp(buf, "lp_time_ ", 9) == 0)
{
buf += 9;
strncpy(lp_time_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_time_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_time_ ] %s \"%s\"|", botnick, lp_setto, lp_time_);
}
else if(strncmp(buf, "lp_timetounban ", 15) == 0)
{
buf += 15;
strncpy(lp_timetounban, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_timetounban[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_timetounban ] %s \"%s\"|", botnick, lp_setto, lp_timetounban);
}
else if(strncmp(buf, "lp_toohighratio ", 16) == 0)
{
buf += 16;
strncpy(lp_toohighratio, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_toohighratio[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_toohighratio ] %s \"%s\"|", botnick, lp_setto, lp_toohighratio);
}
else if(strncmp(buf, "lp_toolowratio ", 15) == 0)
{
buf += 15;
strncpy(lp_toolowratio, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_toolowratio[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_toolowratio ] %s \"%s\"|", botnick, lp_setto, lp_toolowratio);
}
else if(strncmp(buf, "lp_toomanyhubs ", 15) == 0)
{
buf += 15;
strncpy(lp_toomanyhubs, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_toomanyhubs[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_toomanyhubs ] %s \"%s\"|", botnick, lp_setto, lp_toomanyhubs);
}
else if(strncmp(buf, "lp_toomanyslots ", 16) == 0)
{
buf += 16;
strncpy(lp_toomanyslots, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_toomanyslots[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_toomanyslots ] %s \"%s\"|", botnick, lp_setto, lp_toomanyslots);
}
else if(strncmp(buf, "lp_toomanywarnings ", 19) == 0)
{
buf += 19;
strncpy(lp_toomanywarnings, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_toomanywarnings[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_toomanywarnings ] %s \"%s\"|", botnick, lp_setto, lp_toomanywarnings);
}
else if(strncmp(buf, "lp_tooolddcppver ", 17) == 0)
{
buf += 17;
strncpy(lp_tooolddcppver, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_tooolddcppver[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_tooolddcppver ] %s \"%s\"|", botnick, lp_setto, lp_tooolddcppver);
}
else if(strncmp(buf, "lp_topicchangedby ", 18) == 0)
{
buf += 18;
strncpy(lp_topicchangedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_topicchangedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_topicchangedby ] %s \"%s\"|", botnick, lp_setto, lp_topicchangedby);
}
else if(strncmp(buf, "lp_topicis ", 11) == 0)
{
buf += 11;
strncpy(lp_topicis, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_topicis[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_topicis ] %s \"%s\"|", botnick, lp_setto, lp_topicis);
}
else if(strncmp(buf, "lp_topicnotset ", 15) == 0)
{
buf += 15;
strncpy(lp_topicnotset, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_topicnotset[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_topicnotset ] %s \"%s\"|", botnick, lp_setto, lp_topicnotset);
}
else if(strncmp(buf, "lp_topicunsetby ", 16) == 0)
{
buf += 16;
strncpy(lp_topicunsetby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_topicunsetby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_topicunsetby ] %s \"%s\"|", botnick, lp_setto, lp_topicunsetby);
}
else if(strncmp(buf, "lp_toreleaselist ", 17) == 0)
{
buf += 17;
strncpy(lp_toreleaselist, buf, (cut_string(buf, '|') > 200) ? 200 : cut_string(buf, '|'));
lp_toreleaselist[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_toreleaselist ] %s \"%s\"|", botnick, lp_setto, lp_toreleaselist);
}
else if(strncmp(buf, "lp_toreqlist ", 13) == 0)
{
buf += 13;
strncpy(lp_toreqlist, buf, (cut_string(buf, '|') > 200) ? 200 : cut_string(buf, '|'));
lp_toreqlist[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_toreqlist ] %s \"%s\"|", botnick, lp_setto, lp_toreqlist);
}
else if(strncmp(buf, "lp_total_ ", 10) == 0)
{
buf += 10;
strncpy(lp_total_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_total_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_total_ ] %s \"%s\"|", botnick, lp_setto, lp_total_);
}
else if(strncmp(buf, "lp_totalvisitors ", 17) == 0)
{
buf += 17;
strncpy(lp_totalvisitors, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_totalvisitors[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_totalvisitors ] %s \"%s\"|", botnick, lp_setto, lp_totalvisitors);
}
else if(strncmp(buf, "lp_type_ ", 9) == 0)
{
buf += 9;
strncpy(lp_type_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_type_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_type_ ] %s \"%s\"|", botnick, lp_setto, lp_type_);
}
else if(strncmp(buf, "lp_typehelp ", 12) == 0)
{
buf += 12;
strncpy(lp_typehelp, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_typehelp[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_typehelp ] %s \"%s\"|", botnick, lp_setto, lp_typehelp);
}
else if(strncmp(buf, "lp_typeregme ", 13) == 0)
{
buf += 13;
strncpy(lp_typeregme, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_typeregme[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_typeregme ] %s \"%s\"|", botnick, lp_setto, lp_typeregme);
}
else if(strncmp(buf, "lp_typerules ", 13) == 0)
{
buf += 13;
strncpy(lp_typerules, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_typerules[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_typerules ] %s \"%s\"|", botnick, lp_setto, lp_typerules);
}
else if(strncmp(buf, "lp_unfinishedfiles ", 19) == 0)
{
buf += 19;
strncpy(lp_unfinishedfiles, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_unfinishedfiles[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_unfinishedfiles ] %s \"%s\"|", botnick, lp_setto, lp_unfinishedfiles);
}
else if(strncmp(buf, "lp_ungaggingall ", 16) == 0)
{
buf += 16;
strncpy(lp_ungaggingall, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_ungaggingall[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_ungaggingall ] %s \"%s\"|", botnick, lp_setto, lp_ungaggingall);
}
else if(strncmp(buf, "lp_ungagminshare ", 17) == 0)
{
buf += 17;
strncpy(lp_ungagminshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_ungagminshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_ungagminshare ] %s \"%s\"|", botnick, lp_setto, lp_ungagminshare);
}
else if(strncmp(buf, "lp_unhidingpresence ", 20) == 0)
{
buf += 20;
strncpy(lp_unhidingpresence, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_unhidingpresence[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_unhidingpresence ] %s \"%s\"|", botnick, lp_setto, lp_unhidingpresence);
}
else if(strncmp(buf, "lp_unhidingshare ", 17) == 0)
{
buf += 17;
strncpy(lp_unhidingshare, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_unhidingshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_unhidingshare ] %s \"%s\"|", botnick, lp_setto, lp_unhidingshare);
}
else if(strncmp(buf, "lp_uploadlimittoolow ", 21) == 0)
{
buf += 21;
strncpy(lp_uploadlimittoolow, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_uploadlimittoolow[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_uploadlimittoolow ] %s \"%s\"|", botnick, lp_setto, lp_uploadlimittoolow);
}
else if(strncmp(buf, "lp_uptime ", 10) == 0)
{
buf += 10;
strncpy(lp_uptime, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_uptime[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_uptime ] %s \"%s\"|", botnick, lp_setto, lp_uptime);
}
else if(strncmp(buf, "lp_user_ ", 9) == 0)
{
buf += 9;
strncpy(lp_user_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_user_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_user_ ] %s \"%s\"|", botnick, lp_setto, lp_user_);
}
else if(strncmp(buf, "lp_userinfo ", 12) == 0)
{
buf += 12;
strncpy(lp_userinfo, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_userinfo[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_userinfo ] %s \"%s\"|", botnick, lp_setto, lp_userinfo);
}
else if(strncmp(buf, "lp_users ", 9) == 0)
{
buf += 9;
strncpy(lp_users, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_users[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_users ] %s \"%s\"|", botnick, lp_setto, lp_users);
}
else if(strncmp(buf, "lp_usertype ", 12) == 0)
{
buf += 12;
strncpy(lp_usertype, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_usertype[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_usertype ] %s \"%s\"|", botnick, lp_setto, lp_usertype);
}
else if(strncmp(buf, "lp_value ", 9) == 0)
{
buf += 9;
strncpy(lp_value, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_value[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_value ] %s \"%s\"|", botnick, lp_setto, lp_value);
}
else if(strncmp(buf, "lp_variable ", 12) == 0)
{
buf += 12;
strncpy(lp_variable, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_variable[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_variable ] %s \"%s\"|", botnick, lp_setto, lp_variable);
}
else if(strncmp(buf, "lp_version ", 11) == 0)
{
buf += 11;
strncpy(lp_version, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_version[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_version ] %s \"%s\"|", botnick, lp_setto, lp_version);
}
else if(strncmp(buf, "lp_vips_ ", 9) == 0)
{
buf += 9;
strncpy(lp_vips_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_vips_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_vips_ ] %s \"%s\"|", botnick, lp_setto, lp_vips_);
}
else if(strncmp(buf, "lp_warning_ ", 12) == 0)
{
buf += 12;
strncpy(lp_warning_, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_warning_[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_warning_ ] %s \"%s\"|", botnick, lp_setto, lp_warning_);
}
else if(strncmp(buf, "lp_wasaddedto ", 14) == 0)
{
buf += 14;
strncpy(lp_wasaddedto, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_wasaddedto[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasaddedto ] %s \"%s\"|", botnick, lp_setto, lp_wasaddedto);
}
else if(strncmp(buf, "lp_wasbannedby ", 15) == 0)
{
buf += 15;
strncpy(lp_wasbannedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_wasbannedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasbannedby ] %s \"%s\"|", botnick, lp_setto, lp_wasbannedby);
}
else if(strncmp(buf, "lp_wascleared ", 14) == 0)
{
buf += 14;
strncpy(lp_wascleared, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_wascleared[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wascleared ] %s \"%s\"|", botnick, lp_setto, lp_wascleared);
}
else if(strncmp(buf, "lp_wasdropped ", 14) == 0)
{
buf += 14;
strncpy(lp_wasdropped, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_wasdropped[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasdropped ] %s \"%s\"|", botnick, lp_setto, lp_wasdropped);
}
else if(strncmp(buf, "lp_wasgaggedby ", 15) == 0)
{
buf += 15;
strncpy(lp_wasgaggedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_wasgaggedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasgaggedby ] %s \"%s\"|", botnick, lp_setto, lp_wasgaggedby);
}
else if(strncmp(buf, "lp_wasgaggedfrom ", 17) == 0)
{
buf += 17;
strncpy(lp_wasgaggedfrom, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_wasgaggedfrom[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasgaggedfrom ] %s \"%s\"|", botnick, lp_setto, lp_wasgaggedfrom);
}
else if(strncmp(buf, "lp_waskickedby ", 13) == 0)
{
buf += 15;
strncpy(lp_waskickedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_waskickedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_waskickedby ] %s \"%s\"|", botnick, lp_setto, lp_waskickedby);
}
else if(strncmp(buf, "lp_wasregistered ", 17) == 0)
{
buf += 17;
strncpy(lp_wasregistered, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_wasregistered[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasregistered ] %s \"%s\"|", botnick, lp_setto, lp_wasregistered);
}
else if(strncmp(buf, "lp_wasreloaded ", 15) == 0)
{
buf += 15;
strncpy(lp_wasreloaded, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_wasreloaded[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasreloaded ] %s \"%s\"|", botnick, lp_setto, lp_wasreloaded);
}
else if(strncmp(buf, "lp_wasungagged ", 15) == 0)
{
buf += 15;
strncpy(lp_wasungagged, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_wasungagged[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_wasungagged ] %s \"%s\"|", botnick, lp_setto, lp_wasungagged);
}
else if(strncmp(buf, "lp_waswarnedby ", 15) == 0)
{
buf += 15;
strncpy(lp_waswarnedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_waswarnedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_waswarnedby ] %s \"%s\"|", botnick, lp_setto, lp_waswarnedby);
}
else if(strncmp(buf, "lp_weeks ", 9) == 0)
{
buf += 9;
strncpy(lp_weeks, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_weeks[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_weeks ] %s \"%s\"|", botnick, lp_setto, lp_weeks);
}
else if(strncmp(buf, "lp_welcometo ", 13) == 0)
{
buf += 13;
strncpy(lp_welcometo, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_welcometo[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_welcometo ] %s \"%s\"|", botnick, lp_setto, lp_welcometo);
}
else if(strncmp(buf, "lp_whohubswarn ", 15) == 0)
{
buf += 15;
strncpy(lp_whohubswarn, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_whohubswarn[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_whohubswarn ] %s \"%s\"|", botnick, lp_setto, lp_whohubswarn);
}
else if(strncmp(buf, "lp_withpass ", 12) == 0)
{
buf += 12;
strncpy(lp_withpass, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_withpass[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_withpass ] %s \"%s\"|", botnick, lp_setto, lp_withpass);
}
else if(strncmp(buf, "lp_youcanwarnonly ", 18) == 0)
{
buf += 18;
strncpy(lp_youcanwarnonly, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_youcanwarnonly[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youcanwarnonly ] %s \"%s\"|", botnick, lp_setto, lp_youcanwarnonly);
}
else if(strncmp(buf, "lp_youraccountwillexpire ", 25) == 0)
{
buf += 25;
strncpy(lp_youraccountwillexpire, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_youraccountwillexpire[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youraccountwillexpire ] %s \"%s\"|", botnick, lp_setto, lp_youraccountwillexpire);
}
else if(strncmp(buf, "lp_youripis ", 12) == 0)
{
buf += 12;
strncpy(lp_youripis, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_youripis[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youripis ] %s \"%s\"|", botnick, lp_setto, lp_youripis);
}
else if(strncmp(buf, "lp_yourpasswordis ", 18) == 0)
{
buf += 18;
strncpy(lp_yourpasswordis, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_yourpasswordis[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_yourpasswordis ] %s \"%s\"|", botnick, lp_setto, lp_yourpasswordis);
}
else if(strncmp(buf, "lp_yourwarnings ", 16) == 0)
{
buf += 16;
strncpy(lp_yourwarnings, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_yourwarnings[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_yourwarnings ] %s \"%s\"|", botnick, lp_setto, lp_yourwarnings);
}
else if(strncmp(buf, "lp_youshare ", 12) == 0)
{
buf += 12;
strncpy(lp_youshare, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_youshare[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youshare ] %s \"%s\"|", botnick, lp_setto, lp_youshare);
}
else if(strncmp(buf, "lp_youwerebanned ", 17) == 0)
{
buf += 17;
strncpy(lp_youwerebanned, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_youwerebanned[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youwerebanned ] %s \"%s\"|", botnick, lp_setto, lp_youwerebanned);
}
else if(strncmp(buf, "lp_youweregagged ", 17) == 0)
{
buf += 17;
strncpy(lp_youweregagged, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_youweregagged[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youweregagged ] %s \"%s\"|", botnick, lp_setto, lp_youweregagged);
}
else if(strncmp(buf, "lp_youwerekicked ", 17) == 0)
{
buf += 17;
strncpy(lp_youwerekicked, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_youwerekicked[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youwerekicked ] %s \"%s\"|", botnick, lp_setto, lp_youwerekicked);
}
else if(strncmp(buf, "lp_youweretempoppedby ", 22) == 0)
{
buf += 22;
strncpy(lp_youweretempoppedby, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_youweretempoppedby[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youweretempoppedby ] %s \"%s\"|", botnick, lp_setto, lp_youweretempoppedby);
}
else if(strncmp(buf, "lp_youwereregistered ", 21) == 0)
{
buf += 21;
strncpy(lp_youwereregistered, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_youwereregistered[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youwereregistered ] %s \"%s\"|", botnick, lp_setto, lp_youwereregistered);
}
else if(strncmp(buf, "lp_youwerewarned ", 17) == 0)
{
buf += 17;
strncpy(lp_youwerewarned, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_youwerewarned[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_youwerewarned ] %s \"%s\"|", botnick, lp_setto, lp_youwerewarned);
}
else if(strncmp(buf, "lp_us001 ", 9) == 0)
{
buf += 9;
strncpy(lp_us001, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us001[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us001 ] %s \"%s\"|", botnick, lp_setto, lp_us001);
}
else if(strncmp(buf, "lp_us002 ", 9) == 0)
{
buf += 9;
strncpy(lp_us002, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us002[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us002 ] %s \"%s\"|", botnick, lp_setto, lp_us002);
}
else if(strncmp(buf, "lp_us003 ", 9) == 0)
{
buf += 9;
strncpy(lp_us003, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us003[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us003 ] %s \"%s\"|", botnick, lp_setto, lp_us003);
}
else if(strncmp(buf, "lp_us004 ", 9) == 0)
{
buf += 9;
strncpy(lp_us004, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us004[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us004 ] %s \"%s\"|", botnick, lp_setto, lp_us004);
}
else if(strncmp(buf, "lp_us005 ", 9) == 0)
{
buf += 9;
strncpy(lp_us005, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us005[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us005 ] %s \"%s\"|", botnick, lp_setto, lp_us005);
}
else if(strncmp(buf, "lp_us006 ", 9) == 0)
{
buf += 9;
strncpy(lp_us006, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us006[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us006 ] %s \"%s\"|", botnick, lp_setto, lp_us006);
}
else if(strncmp(buf, "lp_us007 ", 9) == 0)
{
buf += 9;
strncpy(lp_us007, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us007[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us007 ] %s \"%s\"|", botnick, lp_setto, lp_us007);
}
else if(strncmp(buf, "lp_us008 ", 9) == 0)
{
buf += 9;
strncpy(lp_us008, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us008[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us008 ] %s \"%s\"|", botnick, lp_setto, lp_us008);
}
else if(strncmp(buf, "lp_us009 ", 9) == 0)
{
buf += 9;
strncpy(lp_us009, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us009[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us009 ] %s \"%s\"|", botnick, lp_setto, lp_us009);
}
else if(strncmp(buf, "lp_us010 ", 9) == 0)
{
buf += 9;
strncpy(lp_us010, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us010[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us010 ] %s \"%s\"|", botnick, lp_setto, lp_us010);
}
else if(strncmp(buf, "lp_us011 ", 9) == 0)
{
buf += 9;
strncpy(lp_us011, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us011[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us011 ] %s \"%s\"|", botnick, lp_setto, lp_us011);
}
else if(strncmp(buf, "lp_us012 ", 9) == 0)
{
buf += 9;
strncpy(lp_us012, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us012[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us012 ] %s \"%s\"|", botnick, lp_setto, lp_us012);
}
else if(strncmp(buf, "lp_us013 ", 9) == 0)
{
buf += 9;
strncpy(lp_us013, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us013[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us013 ] %s \"%s\"|", botnick, lp_setto, lp_us013);
}
else if(strncmp(buf, "lp_us014 ", 9) == 0)
{
buf += 9;
strncpy(lp_us014, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us014[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us014 ] %s \"%s\"|", botnick, lp_setto, lp_us014);
}
else if(strncmp(buf, "lp_us015 ", 9) == 0)
{
buf += 9;
strncpy(lp_us015, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us015[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us015 ] %s \"%s\"|", botnick, lp_setto, lp_us015);
}
else if(strncmp(buf, "lp_us016 ", 9) == 0)
{
buf += 9;
strncpy(lp_us016, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us016[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us016 ] %s \"%s\"|", botnick, lp_setto, lp_us016);
}
else if(strncmp(buf, "lp_us017 ", 9) == 0)
{
buf += 9;
strncpy(lp_us017, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us017[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us017 ] %s \"%s\"|", botnick, lp_setto, lp_us017);
}
else if(strncmp(buf, "lp_us018 ", 9) == 0)
{
buf += 9;
strncpy(lp_us018, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us018[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us018 ] %s \"%s\"|", botnick, lp_setto, lp_us018);
}
else if(strncmp(buf, "lp_us019 ", 9) == 0)
{
buf += 9;
strncpy(lp_us019, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us019[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us019 ] %s \"%s\"|", botnick, lp_setto, lp_us019);
}
else if(strncmp(buf, "lp_us020 ", 9) == 0)
{
buf += 9;
strncpy(lp_us020, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us020[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us020 ] %s \"%s\"|", botnick, lp_setto, lp_us020);
}
else if(strncmp(buf, "lp_us021 ", 9) == 0)
{
buf += 9;
strncpy(lp_us021, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us021[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us021 ] %s \"%s\"|", botnick, lp_setto, lp_us021);
}
else if(strncmp(buf, "lp_us022 ", 9) == 0)
{
buf += 9;
strncpy(lp_us022, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us022[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us022 ] %s \"%s\"|", botnick, lp_setto, lp_us022);
}
else if(strncmp(buf, "lp_us023 ", 9) == 0)
{
buf += 9;
strncpy(lp_us023, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us023[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us023 ] %s \"%s\"|", botnick, lp_setto, lp_us023);
}
else if(strncmp(buf, "lp_us024 ", 9) == 0)
{
buf += 9;
strncpy(lp_us024, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us024[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us024 ] %s \"%s\"|", botnick, lp_setto, lp_us024);
}
else if(strncmp(buf, "lp_us025 ", 9) == 0)
{
buf += 9;
strncpy(lp_us025, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us025[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us025 ] %s \"%s\"|", botnick, lp_setto, lp_us025);
}
else if(strncmp(buf, "lp_us026 ", 9) == 0)
{
buf += 9;
strncpy(lp_us026, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us026[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us026 ] %s \"%s\"|", botnick, lp_setto, lp_us026);
}
else if(strncmp(buf, "lp_us027 ", 9) == 0)
{
buf += 9;
strncpy(lp_us027, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us027[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us027 ] %s \"%s\"|", botnick, lp_setto, lp_us027);
}
else if(strncmp(buf, "lp_us028 ", 9) == 0)
{
buf += 9;
strncpy(lp_us028, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us028[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us028 ] %s \"%s\"|", botnick, lp_setto, lp_us028);
}
else if(strncmp(buf, "lp_us029 ", 9) == 0)
{
buf += 9;
strncpy(lp_us029, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us029[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us029 ] %s \"%s\"|", botnick, lp_setto, lp_us029);
}
else if(strncmp(buf, "lp_us030 ", 9) == 0)
{
buf += 9;
strncpy(lp_us030, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us030[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us030 ] %s \"%s\"|", botnick, lp_setto, lp_us030);
}
else if(strncmp(buf, "lp_us031 ", 9) == 0)
{
buf += 9;
strncpy(lp_us031, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us031[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us031 ] %s \"%s\"|", botnick, lp_setto, lp_us031);
}
else if(strncmp(buf, "lp_us032 ", 9) == 0)
{
buf += 9;
strncpy(lp_us032, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us032[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us032 ] %s \"%s\"|", botnick, lp_setto, lp_us032);
}
else if(strncmp(buf, "lp_us033 ", 9) == 0)
{
buf += 9;
strncpy(lp_us033, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us033[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us033 ] %s \"%s\"|", botnick, lp_setto, lp_us033);
}
else if(strncmp(buf, "lp_us034 ", 9) == 0)
{
buf += 9;
strncpy(lp_us034, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us034[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us034 ] %s \"%s\"|", botnick, lp_setto, lp_us034);
}
else if(strncmp(buf, "lp_us035 ", 9) == 0)
{
buf += 9;
strncpy(lp_us035, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us035[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us035 ] %s \"%s\"|", botnick, lp_setto, lp_us035);
}
else if(strncmp(buf, "lp_us036 ", 9) == 0)
{
buf += 9;
strncpy(lp_us036, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us036[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us036 ] %s \"%s\"|", botnick, lp_setto, lp_us036);
}
else if(strncmp(buf, "lp_us037 ", 9) == 0)
{
buf += 9;
strncpy(lp_us037, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us037[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us037 ] %s \"%s\"|", botnick, lp_setto, lp_us037);
}
else if(strncmp(buf, "lp_us038 ", 9) == 0)
{
buf += 9;
strncpy(lp_us038, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us038[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us038 ] %s \"%s\"|", botnick, lp_setto, lp_us038);
}
else if(strncmp(buf, "lp_us039 ", 9) == 0)
{
buf += 9;
strncpy(lp_us039, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us039[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us039 ] %s \"%s\"|", botnick, lp_setto, lp_us039);
}
else if(strncmp(buf, "lp_us040 ", 9) == 0)
{
buf += 9;
strncpy(lp_us040, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us040[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us040 ] %s \"%s\"|", botnick, lp_setto, lp_us040);
}
else if(strncmp(buf, "lp_us041 ", 9) == 0)
{
buf += 9;
strncpy(lp_us041, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us041[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us041 ] %s \"%s\"|", botnick, lp_setto, lp_us041);
}
else if(strncmp(buf, "lp_us042 ", 9) == 0)
{
buf += 9;
strncpy(lp_us042, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us042[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us042 ] %s \"%s\"|", botnick, lp_setto, lp_us042);
}
else if(strncmp(buf, "lp_us043 ", 9) == 0)
{
buf += 9;
strncpy(lp_us043, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us043[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us043 ] %s \"%s\"|", botnick, lp_setto, lp_us043);
}
else if(strncmp(buf, "lp_us044 ", 9) == 0)
{
buf += 9;
strncpy(lp_us044, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us044[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us044 ] %s \"%s\"|", botnick, lp_setto, lp_us044);
}
else if(strncmp(buf, "lp_us045 ", 9) == 0)
{
buf += 9;
strncpy(lp_us045, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us045[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us045 ] %s \"%s\"|", botnick, lp_setto, lp_us045);
}
else if(strncmp(buf, "lp_us046 ", 9) == 0)
{
buf += 9;
strncpy(lp_us046, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us046[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us046 ] %s \"%s\"|", botnick, lp_setto, lp_us046);
}
else if(strncmp(buf, "lp_us048 ", 9) == 0)
{
buf += 9;
strncpy(lp_us048, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us048[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us048 ] %s \"%s\"|", botnick, lp_setto, lp_us048);
}
else if(strncmp(buf, "lp_us049 ", 9) == 0)
{
buf += 9;
strncpy(lp_us049, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us049[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us049 ] %s \"%s\"|", botnick, lp_setto, lp_us049);
}
else if(strncmp(buf, "lp_us050 ", 9) == 0)
{
buf += 9;
strncpy(lp_us050, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us050[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us050 ] %s \"%s\"|", botnick, lp_setto, lp_us050);
}
else if(strncmp(buf, "lp_us051 ", 9) == 0)
{
buf += 9;
strncpy(lp_us051, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us051[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us051 ] %s \"%s\"|", botnick, lp_setto, lp_us051);
}
else if(strncmp(buf, "lp_us052 ", 9) == 0)
{
buf += 9;
strncpy(lp_us052, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us052[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us052 ] %s \"%s\"|", botnick, lp_setto, lp_us052);
}
else if(strncmp(buf, "lp_us053 ", 9) == 0)
{
buf += 9;
strncpy(lp_us053, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us053[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us053 ] %s \"%s\"|", botnick, lp_setto, lp_us053);
}
else if(strncmp(buf, "lp_us054 ", 9) == 0)
{
buf += 9;
strncpy(lp_us054, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us054[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us054 ] %s \"%s\"|", botnick, lp_setto, lp_us054);
}
else if(strncmp(buf, "lp_us055 ", 9) == 0)
{
buf += 9;
strncpy(lp_us055, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us055[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us055 ] %s \"%s\"|", botnick, lp_setto, lp_us055);
}
else if(strncmp(buf, "lp_us056 ", 9) == 0)
{
buf += 9;
strncpy(lp_us056, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us056[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us056 ] %s \"%s\"|", botnick, lp_setto, lp_us056);
}
else if(strncmp(buf, "lp_us057 ", 9) == 0)
{
buf += 9;
strncpy(lp_us057, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us057[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us057 ] %s \"%s\"|", botnick, lp_setto, lp_us057);
}
else if(strncmp(buf, "lp_us058 ", 9) == 0)
{
buf += 9;
strncpy(lp_us058, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us058[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us058 ] %s \"%s\"|", botnick, lp_setto, lp_us058);
}
else if(strncmp(buf, "lp_us059 ", 9) == 0)
{
buf += 9;
strncpy(lp_us059, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us059[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us059 ] %s \"%s\"|", botnick, lp_setto, lp_us059);
}
else if(strncmp(buf, "lp_us060 ", 9) == 0)
{
buf += 9;
strncpy(lp_us060, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us060[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us060 ] %s \"%s\"|", botnick, lp_setto, lp_us060);
}
else if(strncmp(buf, "lp_us063 ", 9) == 0)
{
buf += 9;
strncpy(lp_us063, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us063[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us063 ] %s \"%s\"|", botnick, lp_setto, lp_us063);
}
else if(strncmp(buf, "lp_us064 ", 9) == 0)
{
buf += 9;
strncpy(lp_us064, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us064[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us064 ] %s \"%s\"|", botnick, lp_setto, lp_us064);
}
else if(strncmp(buf, "lp_us065 ", 9) == 0)
{
buf += 9;
strncpy(lp_us065, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us065[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us065 ] %s \"%s\"|", botnick, lp_setto, lp_us065);
}
else if(strncmp(buf, "lp_us066 ", 9) == 0)
{
buf += 9;
strncpy(lp_us066, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us066[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us066 ] %s \"%s\"|", botnick, lp_setto, lp_us066);
}
else if(strncmp(buf, "lp_us067 ", 9) == 0)
{
buf += 9;
strncpy(lp_us067, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us067[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us067 ] %s \"%s\"|", botnick, lp_setto, lp_us067);
}
else if(strncmp(buf, "lp_us068 ", 9) == 0)
{
buf += 9;
strncpy(lp_us068, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us068[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us068 ] %s \"%s\"|", botnick, lp_setto, lp_us068);
}
else if(strncmp(buf, "lp_us069 ", 9) == 0)
{
buf += 9;
strncpy(lp_us069, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us069[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us069 ] %s \"%s\"|", botnick, lp_setto, lp_us069);
}
else if(strncmp(buf, "lp_us070 ", 9) == 0)
{
buf += 9;
strncpy(lp_us070, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us070[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us070 ] %s \"%s\"|", botnick, lp_setto, lp_us070);
}
else if(strncmp(buf, "lp_us071 ", 9) == 0)
{
buf += 9;
strncpy(lp_us071, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us071[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us071 ] %s \"%s\"|", botnick, lp_setto, lp_us071);
}
else if(strncmp(buf, "lp_us072 ", 9) == 0)
{
buf += 9;
strncpy(lp_us072, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us072[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us072 ] %s \"%s\"|", botnick, lp_setto, lp_us072);
}
else if(strncmp(buf, "lp_us073 ", 9) == 0)
{
buf += 9;
strncpy(lp_us073, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us073[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us073 ] %s \"%s\"|", botnick, lp_setto, lp_us073);
}
else if(strncmp(buf, "lp_us074 ", 9) == 0)
{
buf += 9;
strncpy(lp_us074, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us074[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us074 ] %s \"%s\"|", botnick, lp_setto, lp_us074);
}
else if(strncmp(buf, "lp_us075 ", 9) == 0)
{
buf += 9;
strncpy(lp_us075, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us075[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us075 ] %s \"%s\"|", botnick, lp_setto, lp_us075);
}
else if(strncmp(buf, "lp_us076 ", 9) == 0)
{
buf += 9;
strncpy(lp_us076, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us076[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us076 ] %s \"%s\"|", botnick, lp_setto, lp_us076);
}
else if(strncmp(buf, "lp_us077 ", 9) == 0)
{
buf += 9;
strncpy(lp_us077, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us077[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us077 ] %s \"%s\"|", botnick, lp_setto, lp_us077);
}
else if(strncmp(buf, "lp_us078 ", 9) == 0)
{
buf += 9;
strncpy(lp_us078, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us078[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us078 ] %s \"%s\"|", botnick, lp_setto, lp_us078);
}
else if(strncmp(buf, "lp_us079 ", 9) == 0)
{
buf += 9;
strncpy(lp_us079, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us079[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us079 ] %s \"%s\"|", botnick, lp_setto, lp_us079);
}
else if(strncmp(buf, "lp_us080 ", 9) == 0)
{
buf += 9;
strncpy(lp_us080, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us080[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us080 ] %s \"%s\"|", botnick, lp_setto, lp_us080);
}
else if(strncmp(buf, "lp_us081 ", 9) == 0)
{
buf += 9;
strncpy(lp_us081, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us081[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us081 ] %s \"%s\"|", botnick, lp_setto, lp_us081);
}
else if(strncmp(buf, "lp_us082 ", 9) == 0)
{
buf += 9;
strncpy(lp_us082, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us082[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us082 ] %s \"%s\"|", botnick, lp_setto, lp_us082);
}
else if(strncmp(buf, "lp_us083 ", 9) == 0)
{
buf += 9;
strncpy(lp_us083, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us083[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us083 ] %s \"%s\"|", botnick, lp_setto, lp_us083);
}
else if(strncmp(buf, "lp_us084 ", 9) == 0)
{
buf += 9;
strncpy(lp_us084, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us084[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us084 ] %s \"%s\"|", botnick, lp_setto, lp_us084);
}
else if(strncmp(buf, "lp_us085 ", 9) == 0)
{
buf += 9;
strncpy(lp_us085, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us085[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us085 ] %s \"%s\"|", botnick, lp_setto, lp_us085);
}
else if(strncmp(buf, "lp_us086 ", 9) == 0)
{
buf += 9;
strncpy(lp_us086, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us086[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us086 ] %s \"%s\"|", botnick, lp_setto, lp_us086);
}
else if(strncmp(buf, "lp_us087 ", 9) == 0)
{
buf += 9;
strncpy(lp_us087, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us087[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us087 ] %s \"%s\"|", botnick, lp_setto, lp_us087);
}
else if(strncmp(buf, "lp_us088 ", 9) == 0)
{
buf += 9;
strncpy(lp_us088, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us088[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us088 ] %s \"%s\"|", botnick, lp_setto, lp_us088);
}
else if(strncmp(buf, "lp_us089 ", 9) == 0)
{
buf += 9;
strncpy(lp_us089, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us089[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us089 ] %s \"%s\"|", botnick, lp_setto, lp_us089);
}
else if(strncmp(buf, "lp_us090 ", 9) == 0)
{
buf += 9;
strncpy(lp_us090, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us090[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us090 ] %s \"%s\"|", botnick, lp_setto, lp_us090);
}
else if(strncmp(buf, "lp_us091 ", 9) == 0)
{
buf += 9;
strncpy(lp_us091, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us091[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us091 ] %s \"%s\"|", botnick, lp_setto, lp_us091);
}
else if(strncmp(buf, "lp_us092 ", 9) == 0)
{
buf += 9;
strncpy(lp_us092, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us092[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us092 ] %s \"%s\"|", botnick, lp_setto, lp_us092);
}
else if(strncmp(buf, "lp_us093 ", 9) == 0)
{
buf += 9;
strncpy(lp_us093, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us093[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us093 ] %s \"%s\"|", botnick, lp_setto, lp_us093);
}
else if(strncmp(buf, "lp_us094 ", 9) == 0)
{
buf += 9;
strncpy(lp_us094, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us094[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us094 ] %s \"%s\"|", botnick, lp_setto, lp_us094);
}
else if(strncmp(buf, "lp_us095 ", 9) == 0)
{
buf += 9;
strncpy(lp_us095, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us095[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us095 ] %s \"%s\"|", botnick, lp_setto, lp_us095);
}
else if(strncmp(buf, "lp_us096 ", 9) == 0)
{
buf += 9;
strncpy(lp_us096, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us096[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us096 ] %s \"%s\"|", botnick, lp_setto, lp_us096);
}
else if(strncmp(buf, "lp_us097 ", 9) == 0)
{
buf += 9;
strncpy(lp_us097, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us097[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us097 ] %s \"%s\"|", botnick, lp_setto, lp_us097);
}
else if(strncmp(buf, "lp_us098 ", 9) == 0)
{
buf += 9;
strncpy(lp_us098, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us098[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us098 ] %s \"%s\"|", botnick, lp_setto, lp_us098);
}
else if(strncmp(buf, "lp_us099 ", 9) == 0)
{
buf += 9;
strncpy(lp_us099, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us099[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us099 ] %s \"%s\"|", botnick, lp_setto, lp_us099);
}
else if(strncmp(buf, "lp_us100 ", 9) == 0)
{
buf += 9;
strncpy(lp_us100, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us100[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us100 ] %s \"%s\"|", botnick, lp_setto, lp_us100);
}
else if(strncmp(buf, "lp_us101 ", 9) == 0)
{
buf += 9;
strncpy(lp_us101, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us101[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us101 ] %s \"%s\"|", botnick, lp_setto, lp_us101);
}
else if(strncmp(buf, "lp_us102 ", 9) == 0)
{
buf += 9;
strncpy(lp_us102, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us102[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us102 ] %s \"%s\"|", botnick, lp_setto, lp_us102);
}
else if(strncmp(buf, "lp_us103 ", 9) == 0)
{
buf += 9;
strncpy(lp_us103, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us103[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us103 ] %s \"%s\"|", botnick, lp_setto, lp_us103);
}
else if(strncmp(buf, "lp_us104 ", 9) == 0)
{
buf += 9;
strncpy(lp_us104, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us104[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us104 ] %s \"%s\"|", botnick, lp_setto, lp_us104);
}
else if(strncmp(buf, "lp_us105 ", 9) == 0)
{
buf += 9;
strncpy(lp_us105, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us105[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us105 ] %s \"%s\"|", botnick, lp_setto, lp_us105);
}
else if(strncmp(buf, "lp_us106 ", 9) == 0)
{
buf += 9;
strncpy(lp_us106, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us106[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us106 ] %s \"%s\"|", botnick, lp_setto, lp_us106);
}
else if(strncmp(buf, "lp_us107 ", 9) == 0)
{
buf += 9;
strncpy(lp_us107, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us107[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us107 ] %s \"%s\"|", botnick, lp_setto, lp_us107);
}
else if(strncmp(buf, "lp_us108 ", 9) == 0)
{
buf += 9;
strncpy(lp_us108, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us108[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us108 ] %s \"%s\"|", botnick, lp_setto, lp_us108);
}
else if(strncmp(buf, "lp_us109 ", 9) == 0)
{
buf += 9;
strncpy(lp_us109, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us109[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us109 ] %s \"%s\"|", botnick, lp_setto, lp_us109);
}
else if(strncmp(buf, "lp_us110 ", 9) == 0)
{
buf += 9;
strncpy(lp_us110, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us110[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us110 ] %s \"%s\"|", botnick, lp_setto, lp_us110);
}
else if(strncmp(buf, "lp_us111 ", 9) == 0)
{
buf += 9;
strncpy(lp_us111, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us111[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us111 ] %s \"%s\"|", botnick, lp_setto, lp_us111);
}
else if(strncmp(buf, "lp_us112 ", 9) == 0)
{
buf += 9;
strncpy(lp_us112, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us112[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us112 ] %s \"%s\"|", botnick, lp_setto, lp_us112);
}
else if(strncmp(buf, "lp_us113 ", 9) == 0)
{
buf += 9;
strncpy(lp_us113, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us113[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us113 ] %s \"%s\"|", botnick, lp_setto, lp_us113);
}
else if(strncmp(buf, "lp_us114 ", 9) == 0)
{
buf += 9;
strncpy(lp_us114, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us114[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us114 ] %s \"%s\"|", botnick, lp_setto, lp_us114);
}
else if(strncmp(buf, "lp_us115 ", 9) == 0)
{
buf += 9;
strncpy(lp_us115, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us115[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us115 ] %s \"%s\"|", botnick, lp_setto, lp_us115);
}
else if(strncmp(buf, "lp_us116 ", 9) == 0)
{
buf += 9;
strncpy(lp_us116, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us116[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us116 ] %s \"%s\"|", botnick, lp_setto, lp_us116);
}
else if(strncmp(buf, "lp_us117 ", 9) == 0)
{
buf += 9;
strncpy(lp_us117, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us117[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us117 ] %s \"%s\"|", botnick, lp_setto, lp_us117);
}
else if(strncmp(buf, "lp_us118 ", 9) == 0)
{
buf += 9;
strncpy(lp_us118, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us118[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us118 ] %s \"%s\"|", botnick, lp_setto, lp_us118);
}
else if(strncmp(buf, "lp_us119 ", 9) == 0)
{
buf += 9;
strncpy(lp_us119, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us119[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us119 ] %s \"%s\"|", botnick, lp_setto, lp_us119);
}
else if(strncmp(buf, "lp_us120 ", 9) == 0)
{
buf += 9;
strncpy(lp_us120, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us120[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us120 ] %s \"%s\"|", botnick, lp_setto, lp_us120);
}
else if(strncmp(buf, "lp_us122 ", 9) == 0)
{
buf += 9;
strncpy(lp_us122, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us122[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us122 ] %s \"%s\"|", botnick, lp_setto, lp_us122);
}
else if(strncmp(buf, "lp_us123 ", 9) == 0)
{
buf += 9;
strncpy(lp_us123, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us123[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us123 ] %s \"%s\"|", botnick, lp_setto, lp_us123);
}
else if(strncmp(buf, "lp_us124 ", 9) == 0)
{
buf += 9;
strncpy(lp_us124, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us124[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us124 ] %s \"%s\"|", botnick, lp_setto, lp_us124);
}
else if(strncmp(buf, "lp_us125 ", 9) == 0)
{
buf += 9;
strncpy(lp_us125, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us125[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us125 ] %s \"%s\"|", botnick, lp_setto, lp_us125);
}
else if(strncmp(buf, "lp_us126 ", 9) == 0)
{
buf += 9;
strncpy(lp_us126, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us126[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us126 ] %s \"%s\"|", botnick, lp_setto, lp_us126);
}
else if(strncmp(buf, "lp_us127 ", 9) == 0)
{
buf += 9;
strncpy(lp_us127, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us127[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us127 ] %s \"%s\"|", botnick, lp_setto, lp_us127);
}
else if(strncmp(buf, "lp_us128 ", 9) == 0)
{
buf += 9;
strncpy(lp_us128, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us128[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us128 ] %s \"%s\"|", botnick, lp_setto, lp_us128);
}
else if(strncmp(buf, "lp_us129 ", 9) == 0)
{
buf += 9;
strncpy(lp_us129, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us129[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us129 ] %s \"%s\"|", botnick, lp_setto, lp_us129);
}
else if(strncmp(buf, "lp_us130 ", 9) == 0)
{
buf += 9;
strncpy(lp_us130, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us130[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us130 ] %s \"%s\"|", botnick, lp_setto, lp_us130);
}
else if(strncmp(buf, "lp_us131 ", 9) == 0)
{
buf += 9;
strncpy(lp_us131, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us131[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us131 ] %s \"%s\"|", botnick, lp_setto, lp_us131);
}
else if(strncmp(buf, "lp_us132 ", 9) == 0)
{
buf += 9;
strncpy(lp_us132, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us132[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us132 ] %s \"%s\"|", botnick, lp_setto, lp_us132);
}
else if(strncmp(buf, "lp_us133 ", 9) == 0)
{
buf += 9;
strncpy(lp_us133, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us133[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us133 ] %s \"%s\"|", botnick, lp_setto, lp_us133);
}
else if(strncmp(buf, "lp_us134 ", 9) == 0)
{
buf += 9;
strncpy(lp_us134, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us134[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us134 ] %s \"%s\"|", botnick, lp_setto, lp_us134);
}
else if(strncmp(buf, "lp_us135 ", 9) == 0)
{
buf += 9;
strncpy(lp_us135, buf, (cut_string(buf, '|') > 50) ? 50 : cut_string(buf, '|'));
lp_us135[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_us135 ] %s \"%s\"|", botnick, lp_setto, lp_us135);
}
else if(strncmp(buf, "lp_he001 ", 9) == 0)
{
buf += 9;
strncpy(lp_he001, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he001[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he001 ] %s \"%s\"|", botnick, lp_setto, lp_he001);
}
else if(strncmp(buf, "lp_he002 ", 9) == 0)
{
buf += 9;
strncpy(lp_he002, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he002[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he002 ] %s \"%s\"|", botnick, lp_setto, lp_he002);
}
else if(strncmp(buf, "lp_he003 ", 9) == 0)
{
buf += 9;
strncpy(lp_he003, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he003[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he003 ] %s \"%s\"|", botnick, lp_setto, lp_he003);
}
else if(strncmp(buf, "lp_he004 ", 9) == 0)
{
buf += 9;
strncpy(lp_he004, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he004[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he004 ] %s \"%s\"|", botnick, lp_setto, lp_he004);
}
else if(strncmp(buf, "lp_he005 ", 9) == 0)
{
buf += 9;
strncpy(lp_he005, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he005[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he005 ] %s \"%s\"|", botnick, lp_setto, lp_he005);
}
else if(strncmp(buf, "lp_he006 ", 9) == 0)
{
buf += 9;
strncpy(lp_he006, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he006[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he006 ] %s \"%s\"|", botnick, lp_setto, lp_he006);
}
else if(strncmp(buf, "lp_he007 ", 9) == 0)
{
buf += 9;
strncpy(lp_he007, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he007[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he007 ] %s \"%s\"|", botnick, lp_setto, lp_he007);
}
else if(strncmp(buf, "lp_he008 ", 9) == 0)
{
buf += 9;
strncpy(lp_he008, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he008[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he008 ] %s \"%s\"|", botnick, lp_setto, lp_he008);
}
else if(strncmp(buf, "lp_he009 ", 9) == 0)
{
buf += 9;
strncpy(lp_he009, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he009[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he009 ] %s \"%s\"|", botnick, lp_setto, lp_he009);
}
else if(strncmp(buf, "lp_he010 ", 9) == 0)
{
buf += 9;
strncpy(lp_he010, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he010[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he010 ] %s \"%s\"|", botnick, lp_setto, lp_he010);
}
else if(strncmp(buf, "lp_he011 ", 9) == 0)
{
buf += 9;
strncpy(lp_he011, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he011[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he011 ] %s \"%s\"|", botnick, lp_setto, lp_he011);
}
else if(strncmp(buf, "lp_he012 ", 9) == 0)
{
buf += 9;
strncpy(lp_he012, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he012[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he012 ] %s \"%s\"|", botnick, lp_setto, lp_he012);
}
else if(strncmp(buf, "lp_he013 ", 9) == 0)
{
buf += 9;
strncpy(lp_he013, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he013[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he013 ] %s \"%s\"|", botnick, lp_setto, lp_he013);
}
else if(strncmp(buf, "lp_he014 ", 9) == 0)
{
buf += 9;
strncpy(lp_he014, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he014[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he014 ] %s \"%s\"|", botnick, lp_setto, lp_he014);
}
else if(strncmp(buf, "lp_he015 ", 9) == 0)
{
buf += 9;
strncpy(lp_he015, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he015[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he015 ] %s \"%s\"|", botnick, lp_setto, lp_he015);
}
else if(strncmp(buf, "lp_he016 ", 9) == 0)
{
buf += 9;
strncpy(lp_he016, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he016[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he016 ] %s \"%s\"|", botnick, lp_setto, lp_he016);
}
else if(strncmp(buf, "lp_he017 ", 9) == 0)
{
buf += 9;
strncpy(lp_he017, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he017[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he017 ] %s \"%s\"|", botnick, lp_setto, lp_he017);
}
else if(strncmp(buf, "lp_he018 ", 9) == 0)
{
buf += 9;
strncpy(lp_he018, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he018[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he018 ] %s \"%s\"|", botnick, lp_setto, lp_he018);
}
else if(strncmp(buf, "lp_he019 ", 9) == 0)
{
buf += 9;
strncpy(lp_he019, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he019[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he019 ] %s \"%s\"|", botnick, lp_setto, lp_he019);
}
else if(strncmp(buf, "lp_he020 ", 9) == 0)
{
buf += 9;
strncpy(lp_he020, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he020[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he020 ] %s \"%s\"|", botnick, lp_setto, lp_he020);
}
else if(strncmp(buf, "lp_he021 ", 9) == 0)
{
buf += 9;
strncpy(lp_he021, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he021[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he021 ] %s \"%s\"|", botnick, lp_setto, lp_he021);
}
else if(strncmp(buf, "lp_he022 ", 9) == 0)
{
buf += 9;
strncpy(lp_he022, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he022[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he022 ] %s \"%s\"|", botnick, lp_setto, lp_he022);
}
else if(strncmp(buf, "lp_he023 ", 9) == 0)
{
buf += 9;
strncpy(lp_he023, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he023[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he023 ] %s \"%s\"|", botnick, lp_setto, lp_he023);
}
else if(strncmp(buf, "lp_he024 ", 9) == 0)
{
buf += 9;
strncpy(lp_he024, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he024[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he024 ] %s \"%s\"|", botnick, lp_setto, lp_he024);
}
else if(strncmp(buf, "lp_he025 ", 9) == 0)
{
buf += 9;
strncpy(lp_he025, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he025[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he025 ] %s \"%s\"|", botnick, lp_setto, lp_he025);
}
else if(strncmp(buf, "lp_he026 ", 9) == 0)
{
buf += 9;
strncpy(lp_he026, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he026[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he026 ] %s \"%s\"|", botnick, lp_setto, lp_he026);
}
else if(strncmp(buf, "lp_he027 ", 9) == 0)
{
buf += 9;
strncpy(lp_he027, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he027[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he027 ] %s \"%s\"|", botnick, lp_setto, lp_he027);
}
else if(strncmp(buf, "lp_he028 ", 9) == 0)
{
buf += 9;
strncpy(lp_he028, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he028[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he028 ] %s \"%s\"|", botnick, lp_setto, lp_he028);
}
else if(strncmp(buf, "lp_he029 ", 9) == 0)
{
buf += 9;
strncpy(lp_he029, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he029[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he029 ] %s \"%s\"|", botnick, lp_setto, lp_he029);
}
else if(strncmp(buf, "lp_he030 ", 9) == 0)
{
buf += 9;
strncpy(lp_he030, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he030[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he030 ] %s \"%s\"|", botnick, lp_setto, lp_he030);
}
else if(strncmp(buf, "lp_he031 ", 9) == 0)
{
buf += 9;
strncpy(lp_he031, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he031[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he031 ] %s \"%s\"|", botnick, lp_setto, lp_he031);
}
else if(strncmp(buf, "lp_he032 ", 9) == 0)
{
buf += 9;
strncpy(lp_he032, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he032[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he032 ] %s \"%s\"|", botnick, lp_setto, lp_he032);
}
else if(strncmp(buf, "lp_he033 ", 9) == 0)
{
buf += 9;
strncpy(lp_he033, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he033[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he033 ] %s \"%s\"|", botnick, lp_setto, lp_he033);
}
else if(strncmp(buf, "lp_he034 ", 9) == 0)
{
buf += 9;
strncpy(lp_he034, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he034[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he034 ] %s \"%s\"|", botnick, lp_setto, lp_he034);
}
else if(strncmp(buf, "lp_he035 ", 9) == 0)
{
buf += 9;
strncpy(lp_he035, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he035[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he035 ] %s \"%s\"|", botnick, lp_setto, lp_he035);
}
else if(strncmp(buf, "lp_he036 ", 9) == 0)
{
buf += 9;
strncpy(lp_he036, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he036[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he036 ] %s \"%s\"|", botnick, lp_setto, lp_he036);
}
else if(strncmp(buf, "lp_he037 ", 9) == 0)
{
buf += 9;
strncpy(lp_he037, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he037[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he037 ] %s \"%s\"|", botnick, lp_setto, lp_he037);
}
else if(strncmp(buf, "lp_he038 ", 9) == 0)
{
buf += 9;
strncpy(lp_he038, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he038[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he038 ] %s \"%s\"|", botnick, lp_setto, lp_he038);
}
else if(strncmp(buf, "lp_he039 ", 9) == 0)
{
buf += 9;
strncpy(lp_he039, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he039[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he039 ] %s \"%s\"|", botnick, lp_setto, lp_he039);
}
else if(strncmp(buf, "lp_he040 ", 9) == 0)
{
buf += 9;
strncpy(lp_he040, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he040[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he040 ] %s \"%s\"|", botnick, lp_setto, lp_he040);
}
else if(strncmp(buf, "lp_he041 ", 9) == 0)
{
buf += 9;
strncpy(lp_he041, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he041[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he041 ] %s \"%s\"|", botnick, lp_setto, lp_he041);
}
else if(strncmp(buf, "lp_he042 ", 9) == 0)
{
buf += 9;
strncpy(lp_he042, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he042[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he042 ] %s \"%s\"|", botnick, lp_setto, lp_he042);
}
else if(strncmp(buf, "lp_he043 ", 9) == 0)
{
buf += 9;
strncpy(lp_he043, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he043[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he043 ] %s \"%s\"|", botnick, lp_setto, lp_he043);
}
else if(strncmp(buf, "lp_he044 ", 9) == 0)
{
buf += 9;
strncpy(lp_he044, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he044[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he044 ] %s \"%s\"|", botnick, lp_setto, lp_he044);
}
else if(strncmp(buf, "lp_he045 ", 9) == 0)
{
buf += 9;
strncpy(lp_he045, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he045[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he045 ] %s \"%s\"|", botnick, lp_setto, lp_he045);
}
else if(strncmp(buf, "lp_he046 ", 9) == 0)
{
buf += 9;
strncpy(lp_he046, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he046[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he046 ] %s \"%s\"|", botnick, lp_setto, lp_he046);
}
else if(strncmp(buf, "lp_he048 ", 9) == 0)
{
buf += 9;
strncpy(lp_he048, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he048[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he048 ] %s \"%s\"|", botnick, lp_setto, lp_he048);
}
else if(strncmp(buf, "lp_he049 ", 9) == 0)
{
buf += 9;
strncpy(lp_he049, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he049[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he049 ] %s \"%s\"|", botnick, lp_setto, lp_he049);
}
else if(strncmp(buf, "lp_he050 ", 9) == 0)
{
buf += 9;
strncpy(lp_he050, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he050[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he050 ] %s \"%s\"|", botnick, lp_setto, lp_he050);
}
else if(strncmp(buf, "lp_he051 ", 9) == 0)
{
buf += 9;
strncpy(lp_he051, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he051[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he051 ] %s \"%s\"|", botnick, lp_setto, lp_he051);
}
else if(strncmp(buf, "lp_he052 ", 9) == 0)
{
buf += 9;
strncpy(lp_he052, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he052[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he052 ] %s \"%s\"|", botnick, lp_setto, lp_he052);
}
else if(strncmp(buf, "lp_he053 ", 9) == 0)
{
buf += 9;
strncpy(lp_he053, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he053[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he053 ] %s \"%s\"|", botnick, lp_setto, lp_he053);
}
else if(strncmp(buf, "lp_he054 ", 9) == 0)
{
buf += 9;
strncpy(lp_he054, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he054[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he054 ] %s \"%s\"|", botnick, lp_setto, lp_he054);
}
else if(strncmp(buf, "lp_he055 ", 9) == 0)
{
buf += 9;
strncpy(lp_he055, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he055[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he055 ] %s \"%s\"|", botnick, lp_setto, lp_he055);
}
else if(strncmp(buf, "lp_he056 ", 9) == 0)
{
buf += 9;
strncpy(lp_he056, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he056[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he056 ] %s \"%s\"|", botnick, lp_setto, lp_he056);
}
else if(strncmp(buf, "lp_he057 ", 9) == 0)
{
buf += 9;
strncpy(lp_he057, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he057[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he057 ] %s \"%s\"|", botnick, lp_setto, lp_he057);
}
else if(strncmp(buf, "lp_he058 ", 9) == 0)
{
buf += 9;
strncpy(lp_he058, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he058[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he058 ] %s \"%s\"|", botnick, lp_setto, lp_he058);
}
else if(strncmp(buf, "lp_he059 ", 9) == 0)
{
buf += 9;
strncpy(lp_he059, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he059[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he059 ] %s \"%s\"|", botnick, lp_setto, lp_he059);
}
else if(strncmp(buf, "lp_he060 ", 9) == 0)
{
buf += 9;
strncpy(lp_he060, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he060[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he060 ] %s \"%s\"|", botnick, lp_setto, lp_he060);
}
else if(strncmp(buf, "lp_he063 ", 9) == 0)
{
buf += 9;
strncpy(lp_he063, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he063[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he063 ] %s \"%s\"|", botnick, lp_setto, lp_he063);
}
else if(strncmp(buf, "lp_he064 ", 9) == 0)
{
buf += 9;
strncpy(lp_he064, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he064[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he064 ] %s \"%s\"|", botnick, lp_setto, lp_he064);
}
else if(strncmp(buf, "lp_he065 ", 9) == 0)
{
buf += 9;
strncpy(lp_he065, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he065[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he065 ] %s \"%s\"|", botnick, lp_setto, lp_he065);
}
else if(strncmp(buf, "lp_he066 ", 9) == 0)
{
buf += 9;
strncpy(lp_he066, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he066[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he066 ] %s \"%s\"|", botnick, lp_setto, lp_he066);
}
else if(strncmp(buf, "lp_he067 ", 9) == 0)
{
buf += 9;
strncpy(lp_he067, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he067[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he067 ] %s \"%s\"|", botnick, lp_setto, lp_he067);
}
else if(strncmp(buf, "lp_he068 ", 9) == 0)
{
buf += 9;
strncpy(lp_he068, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he068[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he068 ] %s \"%s\"|", botnick, lp_setto, lp_he068);
}
else if(strncmp(buf, "lp_he069 ", 9) == 0)
{
buf += 9;
strncpy(lp_he069, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he069[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he069 ] %s \"%s\"|", botnick, lp_setto, lp_he069);
}
else if(strncmp(buf, "lp_he070 ", 9) == 0)
{
buf += 9;
strncpy(lp_he070, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he070[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he070 ] %s \"%s\"|", botnick, lp_setto, lp_he070);
}
else if(strncmp(buf, "lp_he071 ", 9) == 0)
{
buf += 9;
strncpy(lp_he071, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he071[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he071 ] %s \"%s\"|", botnick, lp_setto, lp_he071);
}
else if(strncmp(buf, "lp_he072 ", 9) == 0)
{
buf += 9;
strncpy(lp_he072, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he072[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he072 ] %s \"%s\"|", botnick, lp_setto, lp_he072);
}
else if(strncmp(buf, "lp_he073 ", 9) == 0)
{
buf += 9;
strncpy(lp_he073, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he073[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he073 ] %s \"%s\"|", botnick, lp_setto, lp_he073);
}
else if(strncmp(buf, "lp_he074 ", 9) == 0)
{
buf += 9;
strncpy(lp_he074, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he074[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he074 ] %s \"%s\"|", botnick, lp_setto, lp_he074);
}
else if(strncmp(buf, "lp_he075 ", 9) == 0)
{
buf += 9;
strncpy(lp_he075, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he075[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he075 ] %s \"%s\"|", botnick, lp_setto, lp_he075);
}
else if(strncmp(buf, "lp_he076 ", 9) == 0)
{
buf += 9;
strncpy(lp_he076, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he076[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he076 ] %s \"%s\"|", botnick, lp_setto, lp_he076);
}
else if(strncmp(buf, "lp_he077 ", 9) == 0)
{
buf += 9;
strncpy(lp_he077, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he077[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he077 ] %s \"%s\"|", botnick, lp_setto, lp_he077);
}
else if(strncmp(buf, "lp_he078 ", 9) == 0)
{
buf += 9;
strncpy(lp_he078, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he078[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he078 ] %s \"%s\"|", botnick, lp_setto, lp_he078);
}
else if(strncmp(buf, "lp_he079 ", 9) == 0)
{
buf += 9;
strncpy(lp_he079, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he079[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he079 ] %s \"%s\"|", botnick, lp_setto, lp_he079);
}
else if(strncmp(buf, "lp_he080 ", 9) == 0)
{
buf += 9;
strncpy(lp_he080, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he080[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he080 ] %s \"%s\"|", botnick, lp_setto, lp_he080);
}
else if(strncmp(buf, "lp_he081 ", 9) == 0)
{
buf += 9;
strncpy(lp_he081, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he081[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he081 ] %s \"%s\"|", botnick, lp_setto, lp_he081);
}
else if(strncmp(buf, "lp_he082 ", 9) == 0)
{
buf += 9;
strncpy(lp_he082, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he082[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he082 ] %s \"%s\"|", botnick, lp_setto, lp_he082);
}
else if(strncmp(buf, "lp_he083 ", 9) == 0)
{
buf += 9;
strncpy(lp_he083, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he083[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he083 ] %s \"%s\"|", botnick, lp_setto, lp_he083);
}
else if(strncmp(buf, "lp_he084 ", 9) == 0)
{
buf += 9;
strncpy(lp_he084, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he084[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he084 ] %s \"%s\"|", botnick, lp_setto, lp_he084);
}
else if(strncmp(buf, "lp_he085 ", 9) == 0)
{
buf += 9;
strncpy(lp_he085, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he085[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he085 ] %s \"%s\"|", botnick, lp_setto, lp_he085);
}
else if(strncmp(buf, "lp_he086 ", 9) == 0)
{
buf += 9;
strncpy(lp_he086, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he086[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he086 ] %s \"%s\"|", botnick, lp_setto, lp_he086);
}
else if(strncmp(buf, "lp_he087 ", 9) == 0)
{
buf += 9;
strncpy(lp_he087, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he087[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he087 ] %s \"%s\"|", botnick, lp_setto, lp_he087);
}
else if(strncmp(buf, "lp_he088 ", 9) == 0)
{
buf += 9;
strncpy(lp_he088, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he088[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he088 ] %s \"%s\"|", botnick, lp_setto, lp_he088);
}
else if(strncmp(buf, "lp_he089 ", 9) == 0)
{
buf += 9;
strncpy(lp_he089, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he089[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he089 ] %s \"%s\"|", botnick, lp_setto, lp_he089);
}
else if(strncmp(buf, "lp_he090 ", 9) == 0)
{
buf += 9;
strncpy(lp_he090, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he090[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he090 ] %s \"%s\"|", botnick, lp_setto, lp_he090);
}
else if(strncmp(buf, "lp_he091 ", 9) == 0)
{
buf += 9;
strncpy(lp_he091, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he091[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he091 ] %s \"%s\"|", botnick, lp_setto, lp_he091);
}
else if(strncmp(buf, "lp_he092 ", 9) == 0)
{
buf += 9;
strncpy(lp_he092, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he092[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he092 ] %s \"%s\"|", botnick, lp_setto, lp_he092);
}
else if(strncmp(buf, "lp_he093 ", 9) == 0)
{
buf += 9;
strncpy(lp_he093, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he093[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he093 ] %s \"%s\"|", botnick, lp_setto, lp_he093);
}
else if(strncmp(buf, "lp_he094 ", 9) == 0)
{
buf += 9;
strncpy(lp_he094, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he094[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he094 ] %s \"%s\"|", botnick, lp_setto, lp_he094);
}
else if(strncmp(buf, "lp_he095 ", 9) == 0)
{
buf += 9;
strncpy(lp_he095, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he095[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he095 ] %s \"%s\"|", botnick, lp_setto, lp_he095);
}
else if(strncmp(buf, "lp_he096 ", 9) == 0)
{
buf += 9;
strncpy(lp_he096, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he096[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he096 ] %s \"%s\"|", botnick, lp_setto, lp_he096);
}
else if(strncmp(buf, "lp_he097 ", 9) == 0)
{
buf += 9;
strncpy(lp_he097, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he097[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he097 ] %s \"%s\"|", botnick, lp_setto, lp_he097);
}
else if(strncmp(buf, "lp_he098 ", 9) == 0)
{
buf += 9;
strncpy(lp_he098, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he098[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he098 ] %s \"%s\"|", botnick, lp_setto, lp_he098);
}
else if(strncmp(buf, "lp_he099 ", 9) == 0)
{
buf += 9;
strncpy(lp_he099, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he099[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he099 ] %s \"%s\"|", botnick, lp_setto, lp_he099);
}

else if(strncmp(buf, "lp_he100 ", 9) == 0)
{
buf += 9;
strncpy(lp_he100, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he100[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he100 ] %s \"%s\"|", botnick, lp_setto, lp_he100);
}

else if(strncmp(buf, "lp_he101 ", 9) == 0)
{
buf += 9;
strncpy(lp_he101, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he101[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he101 ] %s \"%s\"|", botnick, lp_setto, lp_he101);
}
else if(strncmp(buf, "lp_he102 ", 9) == 0)
{
buf += 9;
strncpy(lp_he102, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he102[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he102 ] %s \"%s\"|", botnick, lp_setto, lp_he102);
}
else if(strncmp(buf, "lp_he103 ", 9) == 0)
{
buf += 9;
strncpy(lp_he103, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he103[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he103 ] %s \"%s\"|", botnick, lp_setto, lp_he103);
}

else if(strncmp(buf, "lp_he104 ", 9) == 0)
{
buf += 9;
strncpy(lp_he104, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he104[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he104 ] %s \"%s\"|", botnick, lp_setto, lp_he104);
}
else if(strncmp(buf, "lp_he105 ", 9) == 0)
{
buf += 9;
strncpy(lp_he105, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he105[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he105 ] %s \"%s\"|", botnick, lp_setto, lp_he105);
}
else if(strncmp(buf, "lp_he106 ", 9) == 0)
{
buf += 9;
strncpy(lp_he106, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he106[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he106 ] %s \"%s\"|", botnick, lp_setto, lp_he106);
}
else if(strncmp(buf, "lp_he107 ", 9) == 0)
{
buf += 9;
strncpy(lp_he107, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he107[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he107 ] %s \"%s\"|", botnick, lp_setto, lp_he107);
}
else if(strncmp(buf, "lp_he108 ", 9) == 0)
{
buf += 9;
strncpy(lp_he108, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he108[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he108 ] %s \"%s\"|", botnick, lp_setto, lp_he108);
}
else if(strncmp(buf, "lp_he109 ", 9) == 0)
{
buf += 9;
strncpy(lp_he109, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he109[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he109 ] %s \"%s\"|", botnick, lp_setto, lp_he109);
}
else if(strncmp(buf, "lp_he110 ", 9) == 0)
{
buf += 9;
strncpy(lp_he110, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he110[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he110 ] %s \"%s\"|", botnick, lp_setto, lp_he110);
}
else if(strncmp(buf, "lp_he111 ", 9) == 0)
{
buf += 9;
strncpy(lp_he111, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he111[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he111 ] %s \"%s\"|", botnick, lp_setto, lp_he111);
}
else if(strncmp(buf, "lp_he112 ", 9) == 0)
{
buf += 9;
strncpy(lp_he112, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he112[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he112 ] %s \"%s\"|", botnick, lp_setto, lp_he112);
}
else if(strncmp(buf, "lp_he113 ", 9) == 0)
{
buf += 9;
strncpy(lp_he113, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he113[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he113 ] %s \"%s\"|", botnick, lp_setto, lp_he113);
}
else if(strncmp(buf, "lp_he114 ", 9) == 0)
{
buf += 9;
strncpy(lp_he114, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he114[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he114 ] %s \"%s\"|", botnick, lp_setto, lp_he114);
}
else if(strncmp(buf, "lp_he116 ", 9) == 0)
{
buf += 9;
strncpy(lp_he116, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he116[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he116 ] %s \"%s\"|", botnick, lp_setto, lp_he116);
}
else if(strncmp(buf, "lp_he117 ", 9) == 0)
{
buf += 9;
strncpy(lp_he117, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he117[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he117 ] %s \"%s\"|", botnick, lp_setto, lp_he117);
}
else if(strncmp(buf, "lp_he118 ", 9) == 0)
{
buf += 9;
strncpy(lp_he118, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he118[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he118 ] %s \"%s\"|", botnick, lp_setto, lp_he118);
}
else if(strncmp(buf, "lp_he119 ", 9) == 0)
{
buf += 9;
strncpy(lp_he119, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he119[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he119 ] %s \"%s\"|", botnick, lp_setto, lp_he119);
}
else if(strncmp(buf, "lp_he120 ", 9) == 0)
{
buf += 9;
strncpy(lp_he120, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he120[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he120 ] %s \"%s\"|", botnick, lp_setto, lp_he120);
}
else if(strncmp(buf, "lp_he121 ", 9) == 0)
{
buf += 9;
strncpy(lp_he121, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he121[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he121 ] %s \"%s\"|", botnick, lp_setto, lp_he121);
}
else if(strncmp(buf, "lp_he122 ", 9) == 0)
{
buf += 9;
strncpy(lp_he122, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he122[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he122 ] %s \"%s\"|", botnick, lp_setto, lp_he122);
}
else if(strncmp(buf, "lp_he123 ", 9) == 0)
{
buf += 9;
strncpy(lp_he123, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he123[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he123 ] %s \"%s\"|", botnick, lp_setto, lp_he123);
}
else if(strncmp(buf, "lp_he124 ", 9) == 0)
{
buf += 9;
strncpy(lp_he124, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he124[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he124 ] %s \"%s\"|", botnick, lp_setto, lp_he124);
}
else if(strncmp(buf, "lp_he125 ", 9) == 0)
{
buf += 9;
strncpy(lp_he125, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he125[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he125 ] %s \"%s\"|", botnick, lp_setto, lp_he125);
}
else if(strncmp(buf, "lp_he126 ", 9) == 0)
{
buf += 9;
strncpy(lp_he126, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he126[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he126 ] %s \"%s\"|", botnick, lp_setto, lp_he126);
}
else if(strncmp(buf, "lp_he127 ", 9) == 0)
{
buf += 9;
strncpy(lp_he127, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he127[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he127 ] %s \"%s\"|", botnick, lp_setto, lp_he127);
}
else if(strncmp(buf, "lp_he128 ", 9) == 0)
{
buf += 9;
strncpy(lp_he128, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he128[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he128 ] %s \"%s\"|", botnick, lp_setto, lp_he128);
}
else if(strncmp(buf, "lp_he129 ", 9) == 0)
{
buf += 9;
strncpy(lp_he129, buf, (cut_string(buf, '|') > 100) ? 100 : cut_string(buf, '|'));
lp_he129[cut_string(buf, '|')] = '\0';
uprintf(user, "<%s> [ lp_he129 ] %s \"%s\"|", botnick, lp_setto, lp_he129);
}

else                                                                                                                                                                
{                                                                                                                                                                   
if((c = strrchr(buf, '|')) == NULL)                                                                                                                                 
return;                                                                                                                                                             
*c = '\0';                                                                                                                                                          
uprintf(user, "<%s> %s: '%s' -> !lang %s|", botnick, lp_nosuchvariable, buf, buf);                                                                                         
} 
write_lang_file();
}
//DB// Write language file
int write_lang_file(void)
{
   FILE *fp;
   int fd;
   int erret;
   char path[MAX_FDP_LEN+1];
   snprintf(path, MAX_FDP_LEN, "%s/%s", config_dir, LANG_FILE);
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
        logprintf(0, "Error - In write_lang_file(): Couldn't set file lock\n");
        close(fd);
        return -1;
     }
   if((fp = fdopen(fd, "w")) == NULL)
     {
        logprintf(0, "Error - In write_lang_file()/fdopen(): %s", strerror(errno));
        set_lock(fd, F_UNLCK);
        close(fd);
        return -1;
     }
    fprintf(fp, "---- !lang %s %s ----\n", lp_variable, lp_value);
    fprintf(fp, "lp_accepted_ = \"%s\"\n", lp_accepted_);
    fprintf(fp, "lp_accountupdated = \"%s\"\n", lp_accountupdated);
    fprintf(fp, "lp_active_ = \"%s\"\n", lp_active_);
    fprintf(fp, "lp_added_ = \"%s\"\n", lp_added_);
    fprintf(fp, "lp_addedentryto = \"%s\"\n", lp_addedentryto);
    fprintf(fp, "lp_admins_ = \"%s\"\n", lp_admins_);
    fprintf(fp, "lp_after = \"%s\"\n", lp_after);
    fprintf(fp, "lp_ago_ = \"%s\"\n", lp_ago_);
    fprintf(fp, "lp_alltime = \"%s\"\n", lp_alltime);
    fprintf(fp, "lp_allowed_ = \"%s\"\n", lp_allowed_);
    fprintf(fp, "lp_allowedatmost = \"%s\"\n", lp_allowedatmost);
    fprintf(fp, "lp_allowedclients = \"%s\"\n", lp_allowedclients);
    fprintf(fp, "lp_autoawaymodeoff = \"%s\"\n", lp_autoawaymodeoff);
    fprintf(fp, "lp_autoawaymodeon = \"%s\"\n", lp_autoawaymodeon);
    fprintf(fp, "lp_autoawaynotset = \"%s\"\n", lp_autoawaynotset);
    fprintf(fp, "lp_autoawayremoved = \"%s\"\n", lp_autoawayremoved);
    fprintf(fp, "lp_autoawayset = \"%s\"\n", lp_autoawayset);
    fprintf(fp, "lp_availablecommands = \"%s\"\n", lp_availablecommands);
    fprintf(fp, "lp_avgdataperuser = \"%s\"\n", lp_avgdataperuser);
    fprintf(fp, "lp_awayfor = \"%s\"\n", lp_awayfor);
    fprintf(fp, "lp_awaymodeoff = \"%s\"\n", lp_awaymodeoff);
    fprintf(fp, "lp_awaymodeon = \"%s\"\n", lp_awaymodeon);
    fprintf(fp, "lp_badpass = \"%s\"\n", lp_badpass);
    fprintf(fp, "lp_badsyntax = \"%s\"\n", lp_badsyntax);
    fprintf(fp, "lp_ban_ = \"%s\"\n", lp_ban_);
    fprintf(fp, "lp_bannedby = \"%s\"\n", lp_bannedby);
    fprintf(fp, "lp_banning_ = \"%s\"\n", lp_banning_);
    fprintf(fp, "lp_banreason = \"%s\"\n", lp_banreason);
    fprintf(fp, "lp_banworddetected = \"%s\"\n", lp_banworddetected);
    fprintf(fp, "lp_because = \"%s\"\n", lp_because);
    fprintf(fp, "lp_blocked_ = \"%s\"\n", lp_blocked_);
    fprintf(fp, "lp_bots_ = \"%s\"\n", lp_bots_);
    fprintf(fp, "lp_bytes = \"%s\"\n", lp_bytes);
    fprintf(fp, "lp_cannotbeblank = \"%s\"\n", lp_cannotbeblank);
    fprintf(fp, "lp_canonlypmtoops = \"%s\"\n", lp_canonlypmtoops);
    fprintf(fp, "lp_categletter_ = \"%s\"\n", lp_categletter_);
    fprintf(fp, "lp_categlist = \"%s\"\n", lp_categlist);
    fprintf(fp, "lp_categnotfound = \"%s\"\n", lp_categnotfound);
    fprintf(fp, "lp_category_ = \"%s\"\n", lp_category_);
    fprintf(fp, "lp_charnumbers = \"%s\"\n", lp_charnumbers);
    fprintf(fp, "lp_chatting_ = \"%s\"\n", lp_chatting_);
    fprintf(fp, "lp_cheefs_ = \"%s\"\n", lp_cheefs_);
    fprintf(fp, "lp_class1 = \"%s\"\n", lp_class1);
    fprintf(fp, "lp_class2 = \"%s\"\n", lp_class2);
    fprintf(fp, "lp_class3 = \"%s\"\n", lp_class3);
    fprintf(fp, "lp_class4 = \"%s\"\n", lp_class4);
    fprintf(fp, "lp_class5 = \"%s\"\n", lp_class5);
    fprintf(fp, "lp_class6 = \"%s\"\n", lp_class6);
    fprintf(fp, "lp_class7 = \"%s\"\n", lp_class7);
    fprintf(fp, "lp_class8 = \"%s\"\n", lp_class8);
    fprintf(fp, "lp_class9 = \"%s\"\n", lp_class9);
    fprintf(fp, "lp_client_ = \"%s\"\n", lp_client_);
    fprintf(fp, "lp_commandscalled = \"%s\"\n", lp_commandscalled);
    fprintf(fp, "lp_connectedsince = \"%s\"\n", lp_connectedsince);
    fprintf(fp, "lp_connectedusers = \"%s\"\n", lp_connectedusers);
    fprintf(fp, "lp_connection = \"%s\"\n", lp_connection);
    fprintf(fp, "lp_couldntaddentry = \"%s\"\n", lp_couldntaddentry);
    fprintf(fp, "lp_couldntremoveentry = \"%s\"\n", lp_couldntremoveentry);
    fprintf(fp, "lp_currentdate = \"%s\"\n", lp_currentdate);
    fprintf(fp, "lp_cut_ = \"%s\"\n", lp_cut_);
    fprintf(fp, "lp_datashared = \"%s\"\n", lp_datashared);
    fprintf(fp, "lp_days = \"%s\"\n", lp_days);
    fprintf(fp, "lp_delcommand = \"%s\"\n", lp_delcommand);
    fprintf(fp, "lp_deletedby = \"%s\"\n", lp_deletedby);
    fprintf(fp, "lp_deletedmsg = \"%s\"\n", lp_deletedmsg);
    fprintf(fp, "lp_deleteduser = \"%s\"\n", lp_deleteduser);
    fprintf(fp, "lp_denied_ = \"%s\"\n", lp_denied_);
    fprintf(fp, "lp_deniedconnections = \"%s\"\n", lp_deniedconnections);
    fprintf(fp, "lp_description = \"%s\"\n", lp_description);
    fprintf(fp, "lp_desctoolong = \"%s\"\n", lp_desctoolong);
    fprintf(fp, "lp_detectedfrom = \"%s\"\n", lp_detectedfrom);
    fprintf(fp, "lp_downloading_ = \"%s\"\n", lp_downloading_);
    fprintf(fp, "lp_downloadoff = \"%s\"\n", lp_downloadoff);
    fprintf(fp, "lp_downloadon = \"%s\"\n", lp_downloadon);
    fprintf(fp, "lp_downloads = \"%s\"\n", lp_downloads);
    fprintf(fp, "lp_email = \"%s\"\n", lp_email);
    fprintf(fp, "lp_entryalreadyonlist = \"%s\"\n", lp_entryalreadyonlist);
    fprintf(fp, "lp_entrywasntfound = \"%s\"\n", lp_entrywasntfound);    
    fprintf(fp, "lp_erasemsgs = \"%s\"\n", lp_erasemsgs);
    fprintf(fp, "lp_expires_ = \"%s\"\n", lp_expires_);
    fprintf(fp, "lp_expiresin = \"%s\"\n", lp_expiresin);
    fprintf(fp, "lp_fakeshare = \"%s\"\n", lp_fakeshare);
    fprintf(fp, "lp_faketag = \"%s\"\n", lp_faketag);
    fprintf(fp, "lp_favsend = \"%s\"\n", lp_favsend);
    fprintf(fp, "lp_file_ = \"%s\"\n", lp_file_);
    fprintf(fp, "lp_flooding = \"%s\"\n", lp_flooding);
    fprintf(fp, "lp_floodwarn = \"%s\"\n", lp_floodwarn);
    fprintf(fp, "lp_forbchar = \"%s\"\n", lp_forbchar);
    fprintf(fp, "lp_forbiddenclient = \"%s\"\n", lp_forbiddenclient);
    fprintf(fp, "lp_fornext = \"%s\"\n", lp_fornext);
    fprintf(fp, "lp_forother = \"%s\"\n", lp_forother);
    fprintf(fp, "lp_freeslots = \"%s\"\n", lp_freeslots);
    fprintf(fp, "lp_from_ = \"%s\"\n", lp_from_);
    fprintf(fp, "lp_fulfilment = \"%s\"\n", lp_fulfilment);
    fprintf(fp, "lp_gaggedfrom = \"%s\"\n", lp_gaggedfrom);
    fprintf(fp, "lp_gaggingall = \"%s\"\n", lp_gaggingall);
    fprintf(fp, "lp_gagminshare = \"%s\"\n", lp_gagminshare);
    fprintf(fp, "lp_givingtempop = \"%s\"\n", lp_givingtempop);
    fprintf(fp, "lp_hasparted = \"%s\"\n", lp_hasparted);
    fprintf(fp, "lp_haveregistertochat = \"%s\"\n", lp_haveregistertochat);
    fprintf(fp, "lp_hi_ = \"%s\"\n", lp_hi_);
    fprintf(fp, "lp_hidingpresence = \"%s\"\n", lp_hidingpresence);
    fprintf(fp, "lp_hidingshare = \"%s\"\n", lp_hidingshare);
    fprintf(fp, "lp_hours = \"%s\"\n", lp_hours);
    fprintf(fp, "lp_hubaddr = \"%s\"\n", lp_hubaddr);
    fprintf(fp, "lp_hubdesc = \"%s\"\n", lp_hubdesc);
    fprintf(fp, "lp_hubforregistered = \"%s\"\n", lp_hubforregistered);
    fprintf(fp, "lp_hubinfo = \"%s\"\n", lp_hubinfo);
    fprintf(fp, "lp_hubkilled = \"%s\"\n", lp_hubkilled);
    fprintf(fp, "lp_hubmassmsg = \"%s\"\n", lp_hubmassmsg);
    fprintf(fp, "lp_hubname = \"%s\"\n", lp_hubname);
    fprintf(fp, "lp_hubs_ = \"%s\"\n", lp_hubs_);    
    fprintf(fp, "lp_hubsperslot = \"%s\"\n", lp_hubsperslot);
    fprintf(fp, "lp_ipban_ = \"%s\"\n", lp_ipban_);
    fprintf(fp, "lp_ipbans = \"%s\"\n", lp_ipbans);
    fprintf(fp, "lp_is_ = \"%s\"\n", lp_is_);
    fprintf(fp, "lp_isalreadyopped = \"%s\"\n", lp_isalreadyopped);
    fprintf(fp, "lp_isaway = \"%s\"\n", lp_isaway);
    fprintf(fp, "lp_isback = \"%s\"\n", lp_isback);
    fprintf(fp, "lp_isbanning = \"%s\"\n", lp_isbanning);
    fprintf(fp, "lp_iskicking = \"%s\"\n", lp_iskicking);
    fprintf(fp, "lp_isoffline = \"%s\"\n", lp_isoffline);
    fprintf(fp, "lp_isonlinenow = \"%s\"\n", lp_isonlinenow);
    fprintf(fp, "lp_itemstotal = \"%s\"\n", lp_itemstotal);
    fprintf(fp, "lp_joins = \"%s\"\n", lp_joins);
    fprintf(fp, "lp_kick_ = \"%s\"\n", lp_kick_);
    fprintf(fp, "lp_kickdeny = \"%s\"\n", lp_kickdeny);
    fprintf(fp, "lp_kickedby = \"%s\"\n", lp_kickedby);
    fprintf(fp, "lp_kvips_ = \"%s\"\n", lp_kvips_);
    fprintf(fp, "lp_lackofprefix = \"%s\"\n", lp_lackofprefix);
    fprintf(fp, "lp_lackoftag = \"%s\"\n", lp_lackoftag);
    fprintf(fp, "lp_langpackfound = \"%s\"\n", lp_langpackfound);
    fprintf(fp, "lp_lastmainmsgs = \"%s\"\n", lp_lastmainmsgs);
    fprintf(fp, "lp_lastpermbans = \"%s\"\n", lp_lastpermbans);
    fprintf(fp, "lp_lastsearch = \"%s\"\n", lp_lastsearch);
    fprintf(fp, "lp_lastsendpm = \"%s\"\n", lp_lastsendpm);
    fprintf(fp, "lp_lasttempbans = \"%s\"\n", lp_lasttempbans);
    fprintf(fp, "lp_lastwritetomain = \"%s\"\n", lp_lastwritetomain);
    fprintf(fp, "lp_listofgagged = \"%s\"\n", lp_listofgagged);
    fprintf(fp, "lp_loggedin = \"%s\"\n", lp_loggedin);
    fprintf(fp, "lp_longestconnected = \"%s\"\n", lp_longestconnected);
    fprintf(fp, "lp_mailtoolong = \"%s\"\n", lp_mailtoolong);
    fprintf(fp, "lp_mainchatlocked = \"%s\"\n", lp_mainchatlocked);
    fprintf(fp, "lp_mainchatmsgs = \"%s\"\n", lp_mainchatmsgs);
    fprintf(fp, "lp_mainchatoff = \"%s\"\n", lp_mainchatoff);
    fprintf(fp, "lp_mainchaton = \"%s\"\n", lp_mainchaton);
    fprintf(fp, "lp_masssend = \"%s\"\n", lp_masssend);
    fprintf(fp, "lp_maxhubs = \"%s\"\n", lp_maxhubs);
    fprintf(fp, "lp_maxshare = \"%s\"\n", lp_maxshare);
    fprintf(fp, "lp_maxslots = \"%s\"\n", lp_maxslots);
    fprintf(fp, "lp_maxusers = \"%s\"\n", lp_maxusers);
    fprintf(fp, "lp_message_ = \"%s\"\n", lp_message_);
    fprintf(fp, "lp_mindcppver = \"%s\"\n", lp_mindcppver);
    fprintf(fp, "lp_minactshare = \"%s\"\n", lp_minactshare);
    fprintf(fp, "lp_minkvipshare = \"%s\"\n", lp_minkvipshare);
    fprintf(fp, "lp_minopshare = \"%s\"\n", lp_minopshare);
    fprintf(fp, "lp_minpsvshare = \"%s\"\n", lp_minpsvshare);
    fprintf(fp, "lp_minregshare = \"%s\"\n", lp_minregshare);
    fprintf(fp, "lp_minshare = \"%s\"\n", lp_minshare);
    fprintf(fp, "lp_minslots = \"%s\"\n", lp_minslots);
    fprintf(fp, "lp_minvipshare = \"%s\"\n", lp_minvipshare);
    fprintf(fp, "lp_minutes = \"%s\"\n", lp_minutes);
    fprintf(fp, "lp_mode_ = \"%s\"\n", lp_mode_);
    fprintf(fp, "lp_msgforyou = \"%s\"\n", lp_msgforyou);
    fprintf(fp, "lp_musthavedot = \"%s\"\n", lp_musthavedot);
    fprintf(fp, "lp_name_ = \"%s\"\n", lp_name_);
    fprintf(fp, "lp_nextwarningban = \"%s\"\n", lp_nextwarningban);
    fprintf(fp, "lp_never_ = \"%s\"\n", lp_never_);
    fprintf(fp, "lp_newcommand = \"%s\"\n", lp_newcommand);
    fprintf(fp, "lp_newregistered = \"%s\"\n", lp_newregistered);
    fprintf(fp, "lp_nick_ = \"%s\"\n", lp_nick_);
    fprintf(fp, "lp_nickban_ = \"%s\"\n", lp_nickban_);
    fprintf(fp, "lp_nickbans = \"%s\"\n", lp_nickbans);
    fprintf(fp, "lp_nickchar = \"%s\"\n", lp_nickchar);
    fprintf(fp, "lp_nickinuse = \"%s\"\n", lp_nickinuse);
    fprintf(fp, "lp_nickname = \"%s\"\n", lp_nickname);
    fprintf(fp, "lp_nicktooshort = \"%s\"\n", lp_nicktooshort);
    fprintf(fp, "lp_nomsgtodelete = \"%s\"\n", lp_nomsgtodelete);
    fprintf(fp, "lp_nonewlines = \"%s\"\n", lp_nonewlines);
    fprintf(fp, "lp_nonewmsg = \"%s\"\n", lp_nonewmsg);
    fprintf(fp, "lp_nopass = \"%s\"\n", lp_nopass);
    fprintf(fp, "lp_nopermission = \"%s\"\n", lp_nopermission);
    fprintf(fp, "lp_normal_ = \"%s\"\n", lp_normal_);
    fprintf(fp, "lp_nosuchfile = \"%s\"\n", lp_nosuchfile);
    fprintf(fp, "lp_nosuchvariable = \"%s\"\n", lp_nosuchvariable);
    fprintf(fp, "lp_notallowedtoadd = \"%s\"\n", lp_notallowedtoadd);
    fprintf(fp, "lp_notallowedtochat = \"%s\"\n", lp_notallowedtochat);
    fprintf(fp, "lp_notallowedtodelreg = \"%s\"\n", lp_notallowedtodelreg);
    fprintf(fp, "lp_notallowedtosee = \"%s\"\n", lp_notallowedtosee);
    fprintf(fp, "lp_notenoughslots = \"%s\"\n", lp_notenoughslots);
    fprintf(fp, "lp_nowarnings = \"%s\"\n", lp_nowarnings);
    fprintf(fp, "lp_number_ = \"%s\"\n", lp_number_);
    fprintf(fp, "lp_opped_ = \"%s\"\n", lp_opped_);
    fprintf(fp, "lp_ops_ = \"%s\"\n", lp_ops_);
    fprintf(fp, "lp_owners_ = \"%s\"\n", lp_owners_);
    fprintf(fp, "lp_pass_ = \"%s\"\n", lp_pass_);
    fprintf(fp, "lp_passchanged = \"%s\"\n", lp_passchanged);
    fprintf(fp, "lp_passwordincorrect = \"%s\"\n", lp_passwordincorrect);
    fprintf(fp, "lp_passwordrequired = \"%s\"\n", lp_passwordrequired);
    fprintf(fp, "lp_passive_ = \"%s\"\n", lp_passive_);
    fprintf(fp, "lp_permanent_ = \"%s\"\n", lp_permanent_);
    fprintf(fp, "lp_permanently = \"%s\"\n", lp_permanently);
    fprintf(fp, "lp_permbans = \"%s\"\n", lp_permbans);
    fprintf(fp, "lp_pmmsgs = \"%s\"\n", lp_pmmsgs);
    fprintf(fp, "lp_pmoff = \"%s\"\n", lp_pmoff);
    fprintf(fp, "lp_pmon = \"%s\"\n", lp_pmon);
    fprintf(fp, "lp_presencealreadyhidden = \"%s\"\n", lp_presencealreadyhidden);
    fprintf(fp, "lp_presencehidden = \"%s\"\n", lp_presencehidden);
    fprintf(fp, "lp_protectmepass = \"%s\"\n", lp_protectmepass);
    fprintf(fp, "lp_prots_ = \"%s\"\n", lp_prots_);
    fprintf(fp, "lp_prottime = \"%s\"\n", lp_prottime);
    fprintf(fp, "lp_prottype = \"%s\"\n", lp_prottype);
    fprintf(fp, "lp_proxy_ = \"%s\"\n", lp_proxy_);
    fprintf(fp, "lp_proxynotallowed = \"%s\"\n", lp_proxynotallowed);
    fprintf(fp, "lp_psvnotallowed = \"%s\"\n", lp_psvnotallowed);
    fprintf(fp, "lp_psvwithmodea = \"%s\"\n", lp_psvwithmodea);
    fprintf(fp, "lp_psvwithoutpsvinnick = \"%s\"\n", lp_psvwithoutpsvinnick);
    fprintf(fp, "lp_quits = \"%s\"\n", lp_quits);
    fprintf(fp, "lp_rawadm = \"%s\"\n", lp_rawadm);
    fprintf(fp, "lp_rawop = \"%s\"\n", lp_rawop);
    fprintf(fp, "lp_rawuser = \"%s\"\n", lp_rawuser);
    fprintf(fp, "lp_reason = \"%s\"\n", lp_reason);
    fprintf(fp, "lp_recentlyadded = \"%s\"\n", lp_recentlyadded);
    fprintf(fp, "lp_recordresetted = \"%s\"\n", lp_recordresetted);
    fprintf(fp, "lp_recordshare = \"%s\"\n", lp_recordshare);
    fprintf(fp, "lp_recordusers = \"%s\"\n", lp_recordusers);
    fprintf(fp, "lp_redir_ = \"%s\"\n", lp_redir_);
    fprintf(fp, "lp_redirectingall = \"%s\"\n", lp_redirectingall);
    fprintf(fp, "lp_registered_ = \"%s\"\n", lp_registered_);
    fprintf(fp, "lp_registeredby = \"%s\"\n", lp_registeredby);
    fprintf(fp, "lp_registrationfailed = \"%s\"\n", lp_registrationfailed);
    fprintf(fp, "lp_registrationrules = \"%s\"\n", lp_registrationrules);
    fprintf(fp, "lp_regmepass = \"%s\"\n", lp_regmepass);
    fprintf(fp, "lp_regonly = \"%s\"\n", lp_regonly);
    fprintf(fp, "lp_regs_ = \"%s\"\n", lp_regs_);
    fprintf(fp, "lp_regular_ = \"%s\"\n", lp_regular_);
    fprintf(fp, "lp_releases = \"%s\"\n", lp_releases);
    fprintf(fp, "lp_reloadingscripts = \"%s\"\n", lp_reloadingscripts);
    fprintf(fp, "lp_removedallwarns = \"%s\"\n", lp_removedallwarns);
    fprintf(fp, "lp_removedentryfrom = \"%s\"\n", lp_removedentryfrom);
    fprintf(fp, "lp_reportedby = \"%s\"\n", lp_reportedby);
    fprintf(fp, "lp_reportsend = \"%s\"\n", lp_reportsend);
    fprintf(fp, "lp_requests = \"%s\"\n", lp_requests);
    fprintf(fp, "lp_requiredatleast = \"%s\"\n", lp_requiredatleast);
    fprintf(fp, "lp_restrictions1 = \"%s\"\n", lp_restrictions1);
    fprintf(fp, "lp_restrictions2 = \"%s\"\n", lp_restrictions2);
    fprintf(fp, "lp_savedmsg = \"%s\"\n", lp_savedmsg);
    fprintf(fp, "lp_seconds = \"%s\"\n", lp_seconds);
    fprintf(fp, "lp_searches = \"%s\"\n", lp_searches);
    fprintf(fp, "lp_searchforip = \"%s\"\n", lp_searchforip);
    fprintf(fp, "lp_searchignored = \"%s\"\n", lp_searchignored);
    fprintf(fp, "lp_searching_ = \"%s\"\n", lp_searching_);
    fprintf(fp, "lp_searchoff = \"%s\"\n", lp_searchoff);
    fprintf(fp, "lp_searchon = \"%s\"\n", lp_searchon);
    fprintf(fp, "lp_searchresults = \"%s\"\n", lp_searchresults);
    fprintf(fp, "lp_searchspam = \"%s\"\n", lp_searchspam);
    fprintf(fp, "lp_searchtooshort = \"%s\"\n", lp_searchtooshort);
    fprintf(fp, "lp_selfregistrationdisabled = \"%s\"\n", lp_selfregistrationdisabled);
    fprintf(fp, "lp_setby = \"%s\"\n", lp_setby);
    fprintf(fp, "lp_setto = \"%s\"\n", lp_setto);
    fprintf(fp, "lp_share_ = \"%s\"\n", lp_share_);
    fprintf(fp, "lp_sharesize = \"%s\"\n", lp_sharesize);
    fprintf(fp, "lp_shouldbechecked = \"%s\"\n", lp_shouldbechecked);
    fprintf(fp, "lp_since = \"%s\"\n", lp_since);
    fprintf(fp, "lp_slotbegging = \"%s\"\n", lp_slotbegging);
    fprintf(fp, "lp_slotratio = \"%s\"\n", lp_slotratio);
    fprintf(fp, "lp_slots_ = \"%s\"\n", lp_slots_);
    fprintf(fp, "lp_slotsperhub = \"%s\"\n", lp_slotsperhub);
    fprintf(fp, "lp_spacesinnick = \"%s\"\n", lp_spacesinnick);
    fprintf(fp, "lp_spamming = \"%s\"\n", lp_spamming);
    fprintf(fp, "lp_statistics = \"%s\"\n", lp_statistics);
    fprintf(fp, "lp_tag_ = \"%s\"\n", lp_tag_);
    fprintf(fp, "lp_tempbans = \"%s\"\n", lp_tempbans);
    fprintf(fp, "lp_tempkickban = \"%s\"\n", lp_tempkickban);
    fprintf(fp, "lp_temporary_ = \"%s\"\n", lp_temporary_);
    fprintf(fp, "lp_text_ = \"%s\"\n", lp_text_);
    fprintf(fp, "lp_thesameip = \"%s\"\n", lp_thesameip);
    fprintf(fp, "lp_time_ = \"%s\"\n", lp_time_);
    fprintf(fp, "lp_timetounban = \"%s\"\n", lp_timetounban);
    fprintf(fp, "lp_toohighratio = \"%s\"\n", lp_toohighratio);
    fprintf(fp, "lp_toolowratio = \"%s\"\n", lp_toolowratio);
    fprintf(fp, "lp_toomanyhubs = \"%s\"\n", lp_toomanyhubs);
    fprintf(fp, "lp_toomanyslots = \"%s\"\n", lp_toomanyslots);
    fprintf(fp, "lp_toomanywarnings = \"%s\"\n", lp_toomanywarnings);
    fprintf(fp, "lp_tooolddcppver = \"%s\"\n", lp_tooolddcppver);
    fprintf(fp, "lp_topicchangedby = \"%s\"\n", lp_topicchangedby);
    fprintf(fp, "lp_topicis = \"%s\"\n", lp_topicis);
    fprintf(fp, "lp_topicnotset = \"%s\"\n", lp_topicnotset);
    fprintf(fp, "lp_topicunsetby = \"%s\"\n", lp_topicunsetby);
    fprintf(fp, "lp_toreleaselist = \"%s\"\n", lp_toreleaselist);
    fprintf(fp, "lp_toreqlist = \"%s\"\n", lp_toreqlist);
    fprintf(fp, "lp_total_ = \"%s\"\n", lp_total_);
    fprintf(fp, "lp_totalvisitors = \"%s\"\n", lp_totalvisitors);
    fprintf(fp, "lp_type_ = \"%s\"\n", lp_type_);
    fprintf(fp, "lp_typehelp = \"%s\"\n", lp_typehelp);
    fprintf(fp, "lp_typeregme = \"%s\"\n", lp_typeregme);
    fprintf(fp, "lp_typerules = \"%s\"\n", lp_typerules);
    fprintf(fp, "lp_unfinishedfiles = \"%s\"\n", lp_unfinishedfiles);
    fprintf(fp, "lp_ungaggingall = \"%s\"\n", lp_ungaggingall);
    fprintf(fp, "lp_ungagminshare = \"%s\"\n", lp_ungagminshare);
    fprintf(fp, "lp_unhidingpresence = \"%s\"\n", lp_unhidingpresence);
    fprintf(fp, "lp_unhidingshare = \"%s\"\n", lp_unhidingshare);
    fprintf(fp, "lp_uploadlimittoolow = \"%s\"\n", lp_uploadlimittoolow);
    fprintf(fp, "lp_uptime = \"%s\"\n", lp_uptime);
    fprintf(fp, "lp_userinfo = \"%s\"\n", lp_userinfo);
    fprintf(fp, "lp_user_ = \"%s\"\n", lp_user_);
    fprintf(fp, "lp_users = \"%s\"\n", lp_users);
    fprintf(fp, "lp_usertype = \"%s\"\n", lp_usertype);
    fprintf(fp, "lp_value = \"%s\"\n", lp_value);
    fprintf(fp, "lp_variable = \"%s\"\n", lp_variable);
    fprintf(fp, "lp_version = \"%s\"\n", lp_version);
    fprintf(fp, "lp_vips_ = \"%s\"\n", lp_vips_);
    fprintf(fp, "lp_warning_ = \"%s\"\n", lp_warning_);
    fprintf(fp, "lp_wasaddedto = \"%s\"\n", lp_wasaddedto);
    fprintf(fp, "lp_wasbannedby = \"%s\"\n", lp_wasbannedby);
    fprintf(fp, "lp_wascleared = \"%s\"\n", lp_wascleared);
    fprintf(fp, "lp_wasdropped = \"%s\"\n", lp_wasdropped);
    fprintf(fp, "lp_wasgaggedby = \"%s\"\n", lp_wasgaggedby);
    fprintf(fp, "lp_wasgaggedfrom = \"%s\"\n", lp_wasgaggedfrom);
    fprintf(fp, "lp_waskickedby = \"%s\"\n", lp_waskickedby);
    fprintf(fp, "lp_wasregistered = \"%s\"\n", lp_wasregistered);
    fprintf(fp, "lp_wasreloaded = \"%s\"\n", lp_wasreloaded);
    fprintf(fp, "lp_wasungagged = \"%s\"\n", lp_wasungagged);
    fprintf(fp, "lp_waswarnedby = \"%s\"\n", lp_waswarnedby);
    fprintf(fp, "lp_whohubswarn = \"%s\"\n", lp_whohubswarn);
    fprintf(fp, "lp_weeks = \"%s\"\n", lp_weeks);
    fprintf(fp, "lp_welcometo = \"%s\"\n", lp_welcometo);
    fprintf(fp, "lp_withpass = \"%s\"\n", lp_withpass);
    fprintf(fp, "lp_youcanwarnonly = \"%s\"\n", lp_youcanwarnonly);
    fprintf(fp, "lp_youraccountwillexpire = \"%s\"\n", lp_youraccountwillexpire);
    fprintf(fp, "lp_youripis = \"%s\"\n", lp_youripis);
    fprintf(fp, "lp_yourpasswordis = \"%s\"\n", lp_yourpasswordis);
    fprintf(fp, "lp_yourwarnings = \"%s\"\n", lp_yourwarnings);
    fprintf(fp, "lp_youshare = \"%s\"\n", lp_youshare);
    fprintf(fp, "lp_youwerebanned = \"%s\"\n", lp_youwerebanned);    
    fprintf(fp, "lp_youweregagged = \"%s\"\n", lp_youweregagged);
    fprintf(fp, "lp_youwerekicked = \"%s\"\n", lp_youwerekicked);    
    fprintf(fp, "lp_youwereregistered = \"%s\"\n", lp_youwereregistered);
    fprintf(fp, "lp_youweretempoppedby = \"%s\"\n", lp_youweretempoppedby);
    fprintf(fp, "lp_youwerewarned = \"%s\"\n", lp_youwerewarned);    
    fprintf(fp, "------------------------- UserCommands:\n");
    fprintf(fp, "lp_us001 = \"%s\"\n", lp_us001);    
    fprintf(fp, "lp_us002 = \"%s\"\n", lp_us002);    
    fprintf(fp, "lp_us003 = \"%s\"\n", lp_us003);    
    fprintf(fp, "lp_us004 = \"%s\"\n", lp_us004);    
    fprintf(fp, "lp_us005 = \"%s\"\n", lp_us005);    
    fprintf(fp, "lp_us006 = \"%s\"\n", lp_us006);    
    fprintf(fp, "lp_us007 = \"%s\"\n", lp_us007);    
    fprintf(fp, "lp_us008 = \"%s\"\n", lp_us008);    
    fprintf(fp, "lp_us009 = \"%s\"\n", lp_us009);    
    fprintf(fp, "lp_us010 = \"%s\"\n", lp_us010);    
    fprintf(fp, "lp_us011 = \"%s\"\n", lp_us011);    
    fprintf(fp, "lp_us012 = \"%s\"\n", lp_us012);    
    fprintf(fp, "lp_us013 = \"%s\"\n", lp_us013);    
    fprintf(fp, "lp_us014 = \"%s\"\n", lp_us014);    
    fprintf(fp, "lp_us015 = \"%s\"\n", lp_us015);    
    fprintf(fp, "lp_us016 = \"%s\"\n", lp_us016);    
    fprintf(fp, "lp_us017 = \"%s\"\n", lp_us017);    
    fprintf(fp, "lp_us018 = \"%s\"\n", lp_us018);    
    fprintf(fp, "lp_us019 = \"%s\"\n", lp_us019);    
    fprintf(fp, "lp_us020 = \"%s\"\n", lp_us020);    
    fprintf(fp, "lp_us021 = \"%s\"\n", lp_us021);    
    fprintf(fp, "lp_us022 = \"%s\"\n", lp_us022);    
    fprintf(fp, "lp_us023 = \"%s\"\n", lp_us023);    
    fprintf(fp, "lp_us024 = \"%s\"\n", lp_us024);    
    fprintf(fp, "lp_us025 = \"%s\"\n", lp_us025);    
    fprintf(fp, "lp_us026 = \"%s\"\n", lp_us026);    
    fprintf(fp, "lp_us027 = \"%s\"\n", lp_us027);    
    fprintf(fp, "lp_us028 = \"%s\"\n", lp_us028);    
    fprintf(fp, "lp_us029 = \"%s\"\n", lp_us029);    
    fprintf(fp, "lp_us030 = \"%s\"\n", lp_us030);    
    fprintf(fp, "lp_us031 = \"%s\"\n", lp_us031);    
    fprintf(fp, "lp_us032 = \"%s\"\n", lp_us032);    
    fprintf(fp, "lp_us033 = \"%s\"\n", lp_us033);    
    fprintf(fp, "lp_us034 = \"%s\"\n", lp_us034);    
    fprintf(fp, "lp_us035 = \"%s\"\n", lp_us035);    
    fprintf(fp, "lp_us036 = \"%s\"\n", lp_us036);    
    fprintf(fp, "lp_us037 = \"%s\"\n", lp_us037);    
    fprintf(fp, "lp_us038 = \"%s\"\n", lp_us038);    
    fprintf(fp, "lp_us039 = \"%s\"\n", lp_us039);    
    fprintf(fp, "lp_us040 = \"%s\"\n", lp_us040);    
    fprintf(fp, "lp_us041 = \"%s\"\n", lp_us041);    
    fprintf(fp, "lp_us042 = \"%s\"\n", lp_us042);    
    fprintf(fp, "lp_us043 = \"%s\"\n", lp_us043);    
    fprintf(fp, "lp_us044 = \"%s\"\n", lp_us044);    
    fprintf(fp, "lp_us045 = \"%s\"\n", lp_us045);    
    fprintf(fp, "lp_us046 = \"%s\"\n", lp_us046);    
    fprintf(fp, "lp_us048 = \"%s\"\n", lp_us048);    
    fprintf(fp, "lp_us049 = \"%s\"\n", lp_us049);    
    fprintf(fp, "lp_us050 = \"%s\"\n", lp_us050);    
    fprintf(fp, "lp_us051 = \"%s\"\n", lp_us051);    
    fprintf(fp, "lp_us052 = \"%s\"\n", lp_us052);    
    fprintf(fp, "lp_us053 = \"%s\"\n", lp_us053);    
    fprintf(fp, "lp_us054 = \"%s\"\n", lp_us054);    
    fprintf(fp, "lp_us055 = \"%s\"\n", lp_us055);    
    fprintf(fp, "lp_us056 = \"%s\"\n", lp_us056);    
    fprintf(fp, "lp_us057 = \"%s\"\n", lp_us057);    
    fprintf(fp, "lp_us058 = \"%s\"\n", lp_us058);    
    fprintf(fp, "lp_us059 = \"%s\"\n", lp_us059);    
    fprintf(fp, "lp_us060 = \"%s\"\n", lp_us060);    
    fprintf(fp, "lp_us063 = \"%s\"\n", lp_us063);    
    fprintf(fp, "lp_us064 = \"%s\"\n", lp_us064);    
    fprintf(fp, "lp_us065 = \"%s\"\n", lp_us065);    
    fprintf(fp, "lp_us066 = \"%s\"\n", lp_us066);    
    fprintf(fp, "lp_us067 = \"%s\"\n", lp_us067);    
    fprintf(fp, "lp_us068 = \"%s\"\n", lp_us068);    
    fprintf(fp, "lp_us069 = \"%s\"\n", lp_us069);    
    fprintf(fp, "lp_us070 = \"%s\"\n", lp_us070);    
    fprintf(fp, "lp_us071 = \"%s\"\n", lp_us071);    
    fprintf(fp, "lp_us072 = \"%s\"\n", lp_us072);    
    fprintf(fp, "lp_us073 = \"%s\"\n", lp_us073);    
    fprintf(fp, "lp_us074 = \"%s\"\n", lp_us074);    
    fprintf(fp, "lp_us075 = \"%s\"\n", lp_us075);    
    fprintf(fp, "lp_us076 = \"%s\"\n", lp_us076);    
    fprintf(fp, "lp_us077 = \"%s\"\n", lp_us077);    
    fprintf(fp, "lp_us078 = \"%s\"\n", lp_us078);    
    fprintf(fp, "lp_us079 = \"%s\"\n", lp_us079);    
    fprintf(fp, "lp_us080 = \"%s\"\n", lp_us080);    
    fprintf(fp, "lp_us081 = \"%s\"\n", lp_us081);    
    fprintf(fp, "lp_us082 = \"%s\"\n", lp_us082);    
    fprintf(fp, "lp_us083 = \"%s\"\n", lp_us083);    
    fprintf(fp, "lp_us084 = \"%s\"\n", lp_us084);    
    fprintf(fp, "lp_us085 = \"%s\"\n", lp_us085);    
    fprintf(fp, "lp_us086 = \"%s\"\n", lp_us086);    
    fprintf(fp, "lp_us087 = \"%s\"\n", lp_us087);    
    fprintf(fp, "lp_us088 = \"%s\"\n", lp_us088);    
    fprintf(fp, "lp_us089 = \"%s\"\n", lp_us089);    
    fprintf(fp, "lp_us090 = \"%s\"\n", lp_us090);    
    fprintf(fp, "lp_us091 = \"%s\"\n", lp_us091);    
    fprintf(fp, "lp_us092 = \"%s\"\n", lp_us092);    
    fprintf(fp, "lp_us093 = \"%s\"\n", lp_us093);    
    fprintf(fp, "lp_us094 = \"%s\"\n", lp_us094);    
    fprintf(fp, "lp_us095 = \"%s\"\n", lp_us095);    
    fprintf(fp, "lp_us096 = \"%s\"\n", lp_us096);    
    fprintf(fp, "lp_us097 = \"%s\"\n", lp_us097);    
    fprintf(fp, "lp_us098 = \"%s\"\n", lp_us098);    
    fprintf(fp, "lp_us099 = \"%s\"\n", lp_us099);    
    fprintf(fp, "lp_us100 = \"%s\"\n", lp_us100);    
    fprintf(fp, "lp_us101 = \"%s\"\n", lp_us101);    
    fprintf(fp, "lp_us102 = \"%s\"\n", lp_us102);    
    fprintf(fp, "lp_us103 = \"%s\"\n", lp_us103);    
    fprintf(fp, "lp_us104 = \"%s\"\n", lp_us104);    
    fprintf(fp, "lp_us105 = \"%s\"\n", lp_us105);    
    fprintf(fp, "lp_us106 = \"%s\"\n", lp_us106);    
    fprintf(fp, "lp_us107 = \"%s\"\n", lp_us107);    
    fprintf(fp, "lp_us108 = \"%s\"\n", lp_us108);    
    fprintf(fp, "lp_us109 = \"%s\"\n", lp_us109);    
    fprintf(fp, "lp_us110 = \"%s\"\n", lp_us110);    
    fprintf(fp, "lp_us111 = \"%s\"\n", lp_us111);    
    fprintf(fp, "lp_us112 = \"%s\"\n", lp_us112);    
    fprintf(fp, "lp_us113 = \"%s\"\n", lp_us113);    
    fprintf(fp, "lp_us114 = \"%s\"\n", lp_us114);    
    fprintf(fp, "lp_us115 = \"%s\"\n", lp_us115);    
    fprintf(fp, "lp_us116 = \"%s\"\n", lp_us116);    
    fprintf(fp, "lp_us117 = \"%s\"\n", lp_us117);    
    fprintf(fp, "lp_us118 = \"%s\"\n", lp_us118);    
    fprintf(fp, "lp_us119 = \"%s\"\n", lp_us119);    
    fprintf(fp, "lp_us120 = \"%s\"\n", lp_us120);    
    fprintf(fp, "lp_us122 = \"%s\"\n", lp_us122);    
    fprintf(fp, "lp_us123 = \"%s\"\n", lp_us123);    
    fprintf(fp, "lp_us124 = \"%s\"\n", lp_us124);    
    fprintf(fp, "lp_us125 = \"%s\"\n", lp_us125);    
    fprintf(fp, "lp_us126 = \"%s\"\n", lp_us126);    
    fprintf(fp, "lp_us127 = \"%s\"\n", lp_us127);    
    fprintf(fp, "lp_us128 = \"%s\"\n", lp_us128);    
    fprintf(fp, "lp_us129 = \"%s\"\n", lp_us129);    
    fprintf(fp, "lp_us130 = \"%s\"\n", lp_us130);    
    fprintf(fp, "lp_us131 = \"%s\"\n", lp_us131);    
    fprintf(fp, "lp_us132 = \"%s\"\n", lp_us132);    
    fprintf(fp, "lp_us133 = \"%s\"\n", lp_us133);    
    fprintf(fp, "lp_us134 = \"%s\"\n", lp_us134);    
    fprintf(fp, "lp_us135 = \"%s\"\n", lp_us135);    
    fprintf(fp, "------------------------- !help:\n");
    fprintf(fp, "lp_he001 = \"%s\"\n", lp_he001);    
    fprintf(fp, "lp_he002 = \"%s\"\n", lp_he002);    
    fprintf(fp, "lp_he003 = \"%s\"\n", lp_he003);    
    fprintf(fp, "lp_he004 = \"%s\"\n", lp_he004);    
    fprintf(fp, "lp_he005 = \"%s\"\n", lp_he005);    
    fprintf(fp, "lp_he006 = \"%s\"\n", lp_he006);    
    fprintf(fp, "lp_he007 = \"%s\"\n", lp_he007);    
    fprintf(fp, "lp_he008 = \"%s\"\n", lp_he008);    
    fprintf(fp, "lp_he009 = \"%s\"\n", lp_he009);    
    fprintf(fp, "lp_he010 = \"%s\"\n", lp_he010);    
    fprintf(fp, "lp_he011 = \"%s\"\n", lp_he011);    
    fprintf(fp, "lp_he012 = \"%s\"\n", lp_he012);    
    fprintf(fp, "lp_he013 = \"%s\"\n", lp_he013);    
    fprintf(fp, "lp_he014 = \"%s\"\n", lp_he014);    
    fprintf(fp, "lp_he015 = \"%s\"\n", lp_he015);    
    fprintf(fp, "lp_he016 = \"%s\"\n", lp_he016);    
    fprintf(fp, "lp_he017 = \"%s\"\n", lp_he017);    
    fprintf(fp, "lp_he018 = \"%s\"\n", lp_he018);    
    fprintf(fp, "lp_he019 = \"%s\"\n", lp_he019);    
    fprintf(fp, "lp_he020 = \"%s\"\n", lp_he020);    
    fprintf(fp, "lp_he021 = \"%s\"\n", lp_he021);    
    fprintf(fp, "lp_he022 = \"%s\"\n", lp_he022);    
    fprintf(fp, "lp_he023 = \"%s\"\n", lp_he023);    
    fprintf(fp, "lp_he024 = \"%s\"\n", lp_he024);    
    fprintf(fp, "lp_he025 = \"%s\"\n", lp_he025);    
    fprintf(fp, "lp_he026 = \"%s\"\n", lp_he026);    
    fprintf(fp, "lp_he027 = \"%s\"\n", lp_he027);    
    fprintf(fp, "lp_he028 = \"%s\"\n", lp_he028);    
    fprintf(fp, "lp_he029 = \"%s\"\n", lp_he029);    
    fprintf(fp, "lp_he030 = \"%s\"\n", lp_he030);    
    fprintf(fp, "lp_he031 = \"%s\"\n", lp_he031);    
    fprintf(fp, "lp_he032 = \"%s\"\n", lp_he032);    
    fprintf(fp, "lp_he033 = \"%s\"\n", lp_he033);    
    fprintf(fp, "lp_he034 = \"%s\"\n", lp_he034);    
    fprintf(fp, "lp_he035 = \"%s\"\n", lp_he035);    
    fprintf(fp, "lp_he036 = \"%s\"\n", lp_he036);    
    fprintf(fp, "lp_he037 = \"%s\"\n", lp_he037);    
    fprintf(fp, "lp_he038 = \"%s\"\n", lp_he038);    
    fprintf(fp, "lp_he039 = \"%s\"\n", lp_he039);    
    fprintf(fp, "lp_he040 = \"%s\"\n", lp_he040);    
    fprintf(fp, "lp_he041 = \"%s\"\n", lp_he041);    
    fprintf(fp, "lp_he042 = \"%s\"\n", lp_he042);    
    fprintf(fp, "lp_he043 = \"%s\"\n", lp_he043);    
    fprintf(fp, "lp_he044 = \"%s\"\n", lp_he044);    
    fprintf(fp, "lp_he045 = \"%s\"\n", lp_he045);    
    fprintf(fp, "lp_he046 = \"%s\"\n", lp_he046);    
    fprintf(fp, "lp_he048 = \"%s\"\n", lp_he048);    
    fprintf(fp, "lp_he049 = \"%s\"\n", lp_he049);    
    fprintf(fp, "lp_he050 = \"%s\"\n", lp_he050);    
    fprintf(fp, "lp_he051 = \"%s\"\n", lp_he051);    
    fprintf(fp, "lp_he052 = \"%s\"\n", lp_he052);    
    fprintf(fp, "lp_he053 = \"%s\"\n", lp_he053);    
    fprintf(fp, "lp_he054 = \"%s\"\n", lp_he054);    
    fprintf(fp, "lp_he055 = \"%s\"\n", lp_he055);    
    fprintf(fp, "lp_he056 = \"%s\"\n", lp_he056);    
    fprintf(fp, "lp_he057 = \"%s\"\n", lp_he057);    
    fprintf(fp, "lp_he058 = \"%s\"\n", lp_he058);    
    fprintf(fp, "lp_he059 = \"%s\"\n", lp_he059);    
    fprintf(fp, "lp_he060 = \"%s\"\n", lp_he060);    
    fprintf(fp, "lp_he063 = \"%s\"\n", lp_he063);    
    fprintf(fp, "lp_he064 = \"%s\"\n", lp_he064);    
    fprintf(fp, "lp_he065 = \"%s\"\n", lp_he065);    
    fprintf(fp, "lp_he066 = \"%s\"\n", lp_he066);    
    fprintf(fp, "lp_he067 = \"%s\"\n", lp_he067);    
    fprintf(fp, "lp_he068 = \"%s\"\n", lp_he068);    
    fprintf(fp, "lp_he069 = \"%s\"\n", lp_he069);    
    fprintf(fp, "lp_he070 = \"%s\"\n", lp_he070);    
    fprintf(fp, "lp_he071 = \"%s\"\n", lp_he071);    
    fprintf(fp, "lp_he072 = \"%s\"\n", lp_he072);    
    fprintf(fp, "lp_he073 = \"%s\"\n", lp_he073);    
    fprintf(fp, "lp_he074 = \"%s\"\n", lp_he074);    
    fprintf(fp, "lp_he075 = \"%s\"\n", lp_he075);    
    fprintf(fp, "lp_he076 = \"%s\"\n", lp_he076);    
    fprintf(fp, "lp_he077 = \"%s\"\n", lp_he077);    
    fprintf(fp, "lp_he078 = \"%s\"\n", lp_he078);    
    fprintf(fp, "lp_he079 = \"%s\"\n", lp_he079);    
    fprintf(fp, "lp_he080 = \"%s\"\n", lp_he080);    
    fprintf(fp, "lp_he081 = \"%s\"\n", lp_he081);    
    fprintf(fp, "lp_he082 = \"%s\"\n", lp_he082);    
    fprintf(fp, "lp_he083 = \"%s\"\n", lp_he083);    
    fprintf(fp, "lp_he084 = \"%s\"\n", lp_he084);    
    fprintf(fp, "lp_he085 = \"%s\"\n", lp_he085);    
    fprintf(fp, "lp_he086 = \"%s\"\n", lp_he086);    
    fprintf(fp, "lp_he087 = \"%s\"\n", lp_he087);    
    fprintf(fp, "lp_he088 = \"%s\"\n", lp_he088);    
    fprintf(fp, "lp_he089 = \"%s\"\n", lp_he089);    
    fprintf(fp, "lp_he090 = \"%s\"\n", lp_he090);    
    fprintf(fp, "lp_he091 = \"%s\"\n", lp_he091);    
    fprintf(fp, "lp_he092 = \"%s\"\n", lp_he092);    
    fprintf(fp, "lp_he093 = \"%s\"\n", lp_he093);    
    fprintf(fp, "lp_he094 = \"%s\"\n", lp_he094);    
    fprintf(fp, "lp_he095 = \"%s\"\n", lp_he095);    
    fprintf(fp, "lp_he096 = \"%s\"\n", lp_he096);    
    fprintf(fp, "lp_he097 = \"%s\"\n", lp_he097);    
    fprintf(fp, "lp_he098 = \"%s\"\n", lp_he098);    
    fprintf(fp, "lp_he099 = \"%s\"\n", lp_he099);    
    fprintf(fp, "lp_he100 = \"%s\"\n", lp_he100);    
    fprintf(fp, "lp_he101 = \"%s\"\n", lp_he101);    
    fprintf(fp, "lp_he102 = \"%s\"\n", lp_he102);    
    fprintf(fp, "lp_he103 = \"%s\"\n", lp_he103);    
    fprintf(fp, "lp_he104 = \"%s\"\n", lp_he104);    
    fprintf(fp, "lp_he105 = \"%s\"\n", lp_he105);    
    fprintf(fp, "lp_he106 = \"%s\"\n", lp_he106);    
    fprintf(fp, "lp_he107 = \"%s\"\n", lp_he107);    
    fprintf(fp, "lp_he108 = \"%s\"\n", lp_he108);    
    fprintf(fp, "lp_he109 = \"%s\"\n", lp_he109);    
    fprintf(fp, "lp_he110 = \"%s\"\n", lp_he110);    
    fprintf(fp, "lp_he111 = \"%s\"\n", lp_he111);    
    fprintf(fp, "lp_he112 = \"%s\"\n", lp_he112);    
    fprintf(fp, "lp_he113 = \"%s\"\n", lp_he113);    
    fprintf(fp, "lp_he114 = \"%s\"\n", lp_he114);    
    fprintf(fp, "lp_he116 = \"%s\"\n", lp_he116);    
    fprintf(fp, "lp_he117 = \"%s\"\n", lp_he117);        
    fprintf(fp, "lp_he118 = \"%s\"\n", lp_he118);    
    fprintf(fp, "lp_he119 = \"%s\"\n", lp_he119);        
    fprintf(fp, "lp_he120 = \"%s\"\n", lp_he120);        
    fprintf(fp, "lp_he121 = \"%s\"\n", lp_he121);            
    fprintf(fp, "lp_he122 = \"%s\"\n", lp_he122);            
    fprintf(fp, "lp_he123 = \"%s\"\n", lp_he123);            
    fprintf(fp, "lp_he124 = \"%s\"\n", lp_he124);            
    fprintf(fp, "lp_he125 = \"%s\"\n", lp_he125);            
    fprintf(fp, "lp_he126 = \"%s\"\n", lp_he126);            
    fprintf(fp, "lp_he127 = \"%s\"\n", lp_he127);            
    fprintf(fp, "lp_he128 = \"%s\"\n", lp_he128);                
    fprintf(fp, "lp_he129 = \"%s\"\n", lp_he129);                

   set_lock(fd, F_UNLCK);
   while(((erret = fclose(fp)) != 0) && (errno == EINTR))
     logprintf(0, "Error - In write_lang_file()/fclose(): Interrupted system call. Trying again.\n");
   if(erret != 0)
     {
        logprintf(0, "Error - In write_lang_file()/fclose(): %s", strerror(errno));
        return -1;
     }
   return 1;
}

#endif
