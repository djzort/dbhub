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

int read_config(void);
int check_banlist(void);
int check_if_banned(struct user_t *user, int type);
int check_if_allowed(struct user_t *user, int type);
int check_if_registered(char *user_nick);
void check_if_saw(char *buf, struct user_t *user);
int check_if_record(char *user_nick);
int check_if_away(char *user_nick);
int check_if_autoaway(char *user_nick);
int check_if_gagged(char *user_nick);
int check_if_ipgagged(struct user_t *user);
int check_if_permprotected(char *user_nick);
int check_if_hidden(char *user_nick);
int check_if_categ(char *categ);
int check_pass(char *buf, struct user_t *user);
int get_permissions(char *user_nick);
int write_config_file(void);
int set_lock(int fd, int type);
void create_files(void);
int add_reg_user(char *buf);
int add_seen_user(char *buf);
int add_away_user(char *buf);
int add_autoaway_user(char *buf);
int remove_reg_user(char *buf);
int remove_away_user(struct user_t *user);
int remove_autoaway_user(struct user_t *user);
int init_dirs(void);
int send_motd(struct user_t *user);
int write_file(char *buf, char *type, int overwrite);
int add_line_to_file(char *line, char *file);
int remove_line_from_file(char *line, char *file, int port);
int my_scandir(char *dirname, char *namelist[]);
int remove_exp_from_file(time_t now_time, char *file);
int add_perm(char *buf, struct user_t *user);
int remove_perm(char *buf, struct user_t *user);
void create_dailylog(char *numb);
void create_offmsg(char *nickname);
int choose_welcome(void);
int add_settopic(char *buf);
int remove_settopic(void);
int check_settopic(void);
void check_ipinfo(char *buf, struct user_t *user);
void rls_type(struct user_t *user, char *rlstype);
int check_if_exist_rls(char *rlsname);
int check_if_exist_req(char *rlsname);
int check_warning(struct user_t *user, int type);
void find_rls(struct user_t *user, char *rlstype);
void sort_urls(struct user_t *user);
int add_reason(char *buf, int type);
int send_new_motd(struct user_t *user, char *type);
void send_req(struct user_t *user);
void rls_motd();
void req_motd();
void update_exp(void);
void remove_entry(struct user_t *user, char *buf, int file, int place);
int remove_warnings(char *nick);
int change_nick(char *nick1, char *nick2);
int change_class(char *nick1, int newtype);
