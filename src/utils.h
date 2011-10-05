/**@file utils.h
 * @brief Header for some utility functions

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

#include "main.h" /*needed for struct user_t */

int    cut_string(char *buf, char c);
void   sprintfa(char *buf, const char *format, ...);
int    trim_string(char *buf);
int    count_users(int type);
void   uprintf(struct user_t *user, char *format, ...);
void   get_users_hostname(char *nick, char *buf);
int    get_hash(char *nick);
long long get_uptime(void);
int    match_with_wildcards(char *buf1, char *buf2);
void   strtolower(char *string); /*!< Converts string to all lower case*/
void   time2str(double seconds, char *strtime);
void   uptime2str(struct user_t *user, double seconds);
int    search_in_string(char *search_here, char *search_this);
const char *mystristr(const char *haystack, const char *needle);
void loadfile2buf(char **buffer, const char *filename); /*<! Loads a file into a buffer */

