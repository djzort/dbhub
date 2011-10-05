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

void   	block_proc(char *buf);
void   	sr(char *buf, struct user_t *user);
void   	to_from(char *buf, struct user_t *user);
void   	connect_to_me(char *buf, struct user_t *user);
void   	rev_connect_to_me(char *buf, struct user_t *user);
void   	chat(char *buf, struct user_t *user);
void   	search(char *buf, struct user_t *user);
int    	my_info(char *buf, struct user_t *user);
int    	validate_nick(char *buf, struct user_t *user);
int    	version(char *buf, struct user_t *user);
int    	my_pass(char *buf, struct user_t *user);
void   	kick(char *buf, struct user_t *user, int tempban);
void   	quit_program(void);
void   	set_var(char *buf, struct user_t *user);
int    	ballow(char *buf, int type, struct user_t *user);
int    	unballow(char *buf, int type);
void   	op_force_move(char *buf, struct user_t *user);
void   	redirect_all(char *buf, struct user_t *user);
void   	send_commands(struct user_t *user);
void   	send_mass_message(char *buf, int type);
void   	remove_expired(void);
void	gag_user(char *buf, struct user_t *user);
void	ungag_user(char *buf, struct user_t *user);
void   	send_gagged_list(struct user_t *user);
void   	gagall(struct user_t *user);
void   	ungagall(struct user_t *user);
void   	give_op(char *buf, struct user_t *user);
int   	check_share(struct user_t *user);
void   	send_raws(struct user_t *user);
void   	last_perm(char *buf);
void   	last_temp(char *buf);
void 	ban_info(char *buf);
void 	send_offmsg(struct user_t *user, int type);
void 	denied_info(char *buf);
void 	send_extrafiles(struct user_t *user, char *command);
int 	check_extrafiles(char *command);
int 	is_command(char *temp, struct user_t *user, int type);
void 	remove_offmsg(struct user_t *user);
void 	script_info(char *buf, struct user_t *user);
void 	reload_files_to_memory(char *buf, struct user_t *user);
void 	gag_user_by_script(char *buf, struct user_t *user);
