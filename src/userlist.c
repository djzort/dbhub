/* DB Hub - A Linux/Unix version of the Direct Connect hub                                                                 
*  Copyright (C) 2005-2007  Centurion (DB Hub)                                                                             
*  Copyright (C) 2002,2003  Jonatan Nilsson (Open DC Hub)                                                                  
*     
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License 
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* The user list is a shared memory segment that contains a list of all users 
 * and their hostnames. The first 20 bytes in the segment are reserved for two
 * integers; one which holds the number of spaces for entries in the list, and 
 * the other contains the actual number of entries, i.e, the amount of 
 * connected users.  
 * After the first 20 bytes, an integer is held which tells the current 
 * process that is listening for connections.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#include "main.h"
#include "userlist.h"
#include "utils.h"
#include "fileio.h"
#include "network.h"
#include "logger.h"
#include "semops.h"

int init_user_list(void)
{
   int i;
   int user_list_shm_id;
   char *buf, *bufp;
   
   if(init_user_list_shm_shm() == -1)
     return -1;
      
   /* Get identifier for the shared data segment, starting with space for 50
    * users, plus some space for the number of entries and spaces.  */
   if((user_list_shm_id = shmget(IPC_PRIVATE, 50*USER_LIST_ENT_SIZE+35, 0600)) < 0)
     {	
	logprintf(LOG_EMERG, "Error - In init_user_list()/shmget(): %s",strerror(errno));
	quit = 1;
	return -1;
     }
   
   /* Set the user_list_shm_id.  */
   set_user_list_shm_id(user_list_shm_id);
   
   /* Attach to the shared segment */
   if((buf = (char *)shmat(user_list_shm_id, NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In init_user_list()/shmat(): %s",strerror(errno));
	shmctl(user_list_shm_id, IPC_RMID, NULL);
	shmctl(user_list_shm_shm, IPC_RMID, NULL);
	quit = 1;
	return -1;
     }
   
   shmdt((char *)user_list_shm_id);
   
   /* Print the current number of entries.  */
   sprintf(buf, "%d %d", 50, 0);

   bufp = buf + 20;
   sprintf(bufp, "%d", 0);
   
   /* Initialize the entries to 0.  */
   bufp = buf + 30;
   for(i = 1; i <= 50; i++)
     {	
	*bufp = '\0';
	bufp += USER_LIST_ENT_SIZE;
     }   
   
   shmdt(buf);
   
   return 1;
}

/* Initializes the shared memory segment that contains the id of the shared
 * memory segment for the user list.  */
int init_user_list_shm_shm(void)
{
   /* Get identifier for the shared segment that contains the identifier for
    * the user list. Since the id for the user list may change when it's 
    * resized, it has to be done this way.  */
   
   if((user_list_shm_shm = shmget(IPC_PRIVATE, sizeof(int), 0600)) < 0)
     {	 
	logprintf(LOG_EMERG, "Error - In init_user_list_shm_shm()/shmget(): %s",strerror(errno));
	quit = 1;
	return -1;
     }
   return 1;
}

/* Gets the current id of the shared memory segment that contains the user list.  */
int get_user_list_shm_id(void)
{
   int *shmid;
   int id;
      
   /* Attach to the shared memory segment.  */
   if((shmid = (int *)shmat(user_list_shm_shm, NULL, 0))
      == (int *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In get_user_list_shm_id()/shmat(): %s",strerror(errno));
	return -1;
     }
   
   id = *shmid;
   shmdt((char *)shmid);
   return id;
}

/* Sets the current id of the shared memory segment that contains the user list.  */
void set_user_list_shm_id(int id)
{
   int *shmid;
      
   /* Attach to the shared memory segment.  */
   if((shmid = (int *)shmat(user_list_shm_shm, NULL, 0))
      == (int *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In set_user_list_shm_id()/shmat(): %s",strerror(errno));
	return;
     }
   
   *shmid = id;
   shmdt((char *)shmid);
}

/* Adds a user to the list. Returns 0 if user list needs to be increased.  */
int add_user_to_list(struct user_t *user)
{
   char *buf, *bufp;
   int spaces=0, entries=0;
   int i;
   
   if(check_if_on_user_list(user->nick) != NULL)
     return 1;
   
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((buf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In add_user_to_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return -1;
     }
   
   /* Check how many entries are in the list.  */
   if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
     {
	logprintf(LOG_EMERG, "Error - In add_user_to_list(): Couldn't get number of entries\n");
	shmdt(buf);
	sem_give(user_list_sem);
	quit = 1;
	return -1;
     }

   bufp = buf + 30;

   for(i = 1; i <= spaces; i++) 
     {
	if(*bufp == '\0')
	  {
	     /* And add users nick and hostname.  */
	     snprintf(bufp, USER_LIST_ENT_SIZE, "%s %s", user->nick, user->hostname);
	     sprintf(buf, "%d %d", spaces, entries+1);
	     /* Detach from the segment.  */
	     shmdt(buf);
	     sem_give(user_list_sem);
	     return 1;
	  }	
	bufp += USER_LIST_ENT_SIZE;
     }
   
   shmdt(buf);
   sem_give(user_list_sem);   
   return 0;
}

/* Removes a user from the list. Returns 1 if user is remove, 0 if user 
 * wasn't found and -1 on error.  */
int remove_user_from_list(char *nick)
{
   char *buf, *bufp;
   char temp_nick[MAX_NICK_LEN+1];
   int spaces=0, entries=0;
   int i;
      
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((buf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In remove_user_from_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return -1;
     }
   
   if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
     {
	logprintf(LOG_EMERG, "Error - In remove_user_from_list(): Couldn't get number of entries\n");
	shmdt(buf);
	sem_give(user_list_sem);
	quit = 1;
	return -1;
     }

   bufp = buf + 30;     
   
   for(i = 1; i <= spaces; i++) 
     {
	if(*bufp != '\0')
	  {	     
	     sscanf(bufp, "%50s", temp_nick);
	     if((strncasecmp(temp_nick, nick, strlen(nick)) == 0)
		&& (strlen(nick) == strlen(temp_nick)))
	       {
		  sprintf(buf, "%d %d", spaces, entries-1);
		  /* Set the first character in the nick to null.  */
		  *bufp = '\0';
		  shmdt(buf);
		  sem_give(user_list_sem);
		  return 1;
	       }
	  }	
	bufp += USER_LIST_ENT_SIZE;
     }
   
   shmdt(buf);
   sem_give(user_list_sem);
   
   return 0;
}

/* Check if user is on the list. Returns the nick with the case in the 
 * userlist if found, otherwise NULL is returned.  */
char *check_if_on_user_list(char *nick)
{
   char *buf, *bufp;
   static char temp_nick[MAX_NICK_LEN+1];
   int spaces=0, entries=0;
   int i;
      
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((buf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In check_if_on_user_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return NULL;
     }
   
   if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
     {
	logprintf(LOG_EMERG, "Error - In check_if_on_user_list(): Couldn't get number of entries\n");
	shmdt(buf);
	sem_give(user_list_sem);
	quit = 1;
	return NULL;
     }

   bufp = buf + 30;
   
   for(i = 1; i <= spaces; i++) 
     {
	if(*bufp != '\0')
	  {	     
	     sscanf(bufp, "%50s", temp_nick);
	     if((strncasecmp(temp_nick, nick, strlen(nick)) == 0)
		&& (strlen(nick) == strlen(temp_nick)))
	       {
		  /* The user is here, so detach and return 1.  */
		  shmdt(buf);
		  sem_give(user_list_sem);
		  return temp_nick;
	       }
	  }	
	
	bufp += USER_LIST_ENT_SIZE;
     }
   
   shmdt(buf);
   sem_give(user_list_sem);
   
   return NULL;
}
   

/* If there aren't space in our user list, increase it.  */
void increase_user_list(void)
{
   char *oldbuf, *newbuf, *oldbufp, *newbufp;
   char temp_nick[MAX_NICK_LEN+1];
   char temp_host[MAX_HOST_LEN+1];
   int spaces=0, entries=0, oldpid=0;
   int new_user_list_shm;
   int i;
      
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((oldbuf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In increase_user_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return;
     }
   
   /* Check how many entries are in the list.  */
   if(sscanf(oldbuf, "%d %d", &spaces, &entries) != 2)
     {
	logprintf(LOG_EMERG, "Error - In increase_user_list(): Couldn't get number of entries\n");
	shmdt(oldbuf);
	sem_give(user_list_sem);
	quit = 1;
	return;
     }   
    
   /* Get identifier for the shared data segment, adding space for 50
    * users, plus some space for the number of entries.  */
   if((new_user_list_shm = shmget(IPC_PRIVATE, (spaces+50)*USER_LIST_ENT_SIZE+35, 0600)) < 0)
     {	
	logprintf(LOG_EMERG, "Error - In increase_user_list()/shmget(): %s",strerror(errno));
	shmdt(oldbuf);
	sem_give(user_list_sem);
	quit = 1;
	return;
     }
   
   /* Attach to the shared segment */
   if((newbuf = (char *)shmat(new_user_list_shm, NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In increase_user_list()/shmat(): %s",strerror(errno));
	shmctl(get_user_list_shm_id(), IPC_RMID, NULL);
	quit = 1;
	return;
     }
   
   /* Print the current number of entries.  */
   sprintf(newbuf, "%d %d", spaces+50, entries);
   
   oldbufp = oldbuf + 20;
   newbufp = newbuf + 20;

   sscanf(oldbufp, "%d", &oldpid);
   sprintf(newbufp, "%d", oldpid);
   
   oldbufp = oldbuf + 30;
   newbufp = newbuf + 30;
   
   for(i = 1; i <= spaces; i++) 
     {
	if(*oldbufp != '\0')
	  {
	     /* Get the users nick and hostname.  */
	     sscanf(oldbufp, "%s %s", temp_nick, temp_host);
	   
	     /* Print it in the new shared segment.  */
	     sprintf(newbufp, "%s %s", temp_nick, temp_host);
	     newbufp += USER_LIST_ENT_SIZE;
	  }	
	oldbufp += USER_LIST_ENT_SIZE;
     }

   /* Detach from the old segment and remove it.  */
   shmdt(oldbuf);
   shmctl(get_user_list_shm_id(), IPC_RMID, NULL);
   
   /* And set the global user_list_shm to what our new segment id is.  */
   set_user_list_shm_id(new_user_list_shm);
   
   /* Detach from the new one as well.  */
   shmdt(newbuf);
   
   /* Finally, give back the semaphore.  */
   sem_give(user_list_sem);
}

/* This function is run every ALARM_TIME seconds and checks if the user_list
 * is larger than it needs to be. If it is, it makes it smaller.  */
void purge_user_list(void)
{
   char *oldbuf, *newbuf, *oldbufp, *newbufp;
   char temp_nick[MAX_NICK_LEN+1];
   char temp_host[MAX_HOST_LEN+1];
   int oldspaces=0, entries=0, oldpid=0;
   int newspaces;
   int new_user_list_shm;
   int i;
   int diff;

   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((oldbuf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In purge_user_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return;
     }
   
   /* Check how many entries are in the list.  */
   if(sscanf(oldbuf, "%d %d", &oldspaces, &entries) != 2)
     {	
	logprintf(LOG_EMERG, "Error - In purge_user_list(): Couldn't get number of entries\n");
	shmdt(oldbuf);
	sem_give(user_list_sem);
	quit = 1;
	return;
     }
   
   /* Check if we need to make the userlist smaller.  */
   
   /* Difference between spaces and entries in blocks of 50s.  */
   diff = (oldspaces-entries)/50;
   
   /* If the difference is less than 50, we're satisfied.  */
   if(diff < 1)
     {
	shmdt(oldbuf);
	sem_give(user_list_sem);
	return;
     }   
   
   newspaces = oldspaces - diff*50;
   
   if(newspaces < 50)
     newspaces = 50;
   
   /* Get identifier for the shared data segment, adding space for 50
    *  users, plus some space for the number of entries.  */
   if((new_user_list_shm = shmget(IPC_PRIVATE, newspaces*USER_LIST_ENT_SIZE+35, 0600)) < 0)
     {	
	logprintf(LOG_EMERG, "Error - In purge_user_list()/shmget(): %s",strerror(errno));
	shmdt(oldbuf);
	sem_give(user_list_sem);
	quit = 1;
	return;
     }
   
   /* Attach to the shared segment */
   if((newbuf = (char *)shmat(new_user_list_shm, NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In purge_user_list()/shmat(): %s",strerror(errno));
	shmctl(get_user_list_shm_id(), IPC_RMID, NULL);
	quit = 1;
	return;
     }
   
   /* Print the current number of entries.  */
   sprintf(newbuf, "%d %d", newspaces, entries);
   
   oldbufp = oldbuf + 20;
   newbufp = newbuf + 20;

   sscanf(oldbufp, "%d", &oldpid);
   sprintf(newbufp, "%d", oldpid);
   
   oldbufp = oldbuf + 30;
   newbufp = newbuf + 30;
   
   for(i = 1; i <= oldspaces; i++)
     {
	if(*oldbufp != '\0')
	  {	     
	     /* Get the users nick and hostname.  */
	     sscanf(oldbufp, "%s %s", temp_nick, temp_host);
	     
	     /* Print it in the new shared segment.  */
	     sprintf(newbufp, "%s %s", temp_nick, temp_host);
	     newbufp += USER_LIST_ENT_SIZE;
	  }	
	oldbufp += USER_LIST_ENT_SIZE;
     }
   
   /* Detach from the old segment and remove it.  */
   shmdt(oldbuf);
   shmctl(get_user_list_shm_id(), IPC_RMID, NULL);
   
   /* And set the global user_list_shm to what our new segment id is.  */
   set_user_list_shm_id(new_user_list_shm);
   
   /* Detach from the new one as well.  */
   shmdt(newbuf);
   
   /* Finally, give back the semaphore.  */
   sem_give(user_list_sem);
}


/* Send all nicknames, but only those of properly logged in users */
void send_nick_list(struct user_t *user)
{
   char *buf, *bufp;
   char temp_nick[MAX_NICK_LEN+1];
   char temp_host[MAX_HOST_LEN+1];
   int spaces, entries;
   int i;
    struct user_t *tmp_user;
   char *op_list;
      
   send_to_user("$NickList ", user);
   
   /* If the user isn't on the list, send it back anyway */
   if((user->type & (NON_LOGGED)) != 0) 
     {
	if(user->nick[0] != '\0')
	  {
	     sprintf(temp_nick, "%s$$", user->nick);
	     send_to_user(temp_nick, user);
	  }
     }
	     sprintf(temp_nick, "%s$$%s$$", botnick, opchatnick);
   	     send_to_user(temp_nick, user);
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((buf = shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {
	logprintf(LOG_EMERG, "Error - In send_nick_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return;
     }
   
   if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
     {
	logprintf(LOG_EMERG, "Error - In send_nick_list(): Couldn't get number of entries\n");
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
	  if((tmp_user = get_human_user(temp_nick)) != NULL)
	    {    
	    if(get_human_user(temp_nick)->hide_share != 2) 
	    {
	    send_to_user(temp_nick, user);
	     send_to_user("$$", user);
	    }
	    }
	    else if(strcmp(temp_host, "script_process") == 0)
	    { 
	     send_to_user(temp_nick, user);
	     send_to_user("$$", user);
	    }
	    }
	bufp += USER_LIST_ENT_SIZE;
     }
   
   shmdt(buf);
   sem_give(user_list_sem);
   
   /* Add the two '|' at the end */
   send_to_user("||", user);
   
   /* And send the oplist */
   op_list = get_op_list();
   send_to_user(op_list, user);
   free(op_list);
}

/* Returns the op list as a string. The used string must me freed after 
 * use.  */
char *get_op_list(void)
{
   char *buf, *bufp;
   char temp_nick[MAX_NICK_LEN+1];
   char temp_host[MAX_HOST_LEN+1];
   char *op_list;
   int spaces=0, entries=0;
   int i;
    struct user_t *tmp_user;
int ishum;
      
ishum = 0;
   if((op_list = malloc(sizeof(char) * 9)) == NULL)
     {
	logprintf(LOG_EMERG, "Error - In get_op_list()/malloc(): %s",strerror(errno));
	quit = 1;
	return NULL;
     }
   
   sprintf(op_list, "%s", "$OpList ");

if((op_list = realloc(op_list, sizeof(char) * (strlen(op_list) + strlen(botnick) + 3))) == NULL)
{
logprintf(LOG_EMERG, "Error - In get_op_list()/realloc(): %s",strerror(errno));
quit = 1;
return NULL;
}

sprintfa(op_list, "%s$$", botnick);   

		  if((op_list = realloc(op_list, sizeof(char)
					* (strlen(op_list) + strlen(opchatnick) + 3))) == NULL)
		    {
		       logprintf(LOG_EMERG, "Error - In get_op_list()/realloc(): %s",strerror(errno));
		       quit = 1;
		       return NULL;
		    }

   sprintfa(op_list, "%s$$", opchatnick);   

if(clockbot == 1)
{
		  if((op_list = realloc(op_list, sizeof(char)
					* (strlen(op_list) + strlen(clocknick) + 3))) == NULL)
		    {
		       logprintf(LOG_EMERG, "Error - In get_op_list()/realloc(): %s",strerror(errno));
		       quit = 1;
		       return NULL;
		    }

   sprintfa(op_list, "%s$$", clocknick);   
}
  
   
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((buf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {
	logprintf(LOG_EMERG, "Error - In get_op_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return NULL;
     }
   
   if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
     {
	logprintf(LOG_EMERG, "Error - In get_op_list(): Couldn't get number of entries\n");
	shmdt(buf);
	sem_give(user_list_sem);
	quit = 1;
	return NULL;
     }
   
   bufp = buf + 30;

   for(i = 1; i <= spaces; i++)
     {
	if(*bufp != '\0')
	  {
	     sscanf(bufp, "%50s %120s", temp_nick, temp_host);
if((tmp_user = get_human_user(temp_nick)) != NULL)
{
if((((tmp_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (tmp_user->hide_share != 2)) || (tmp_user->tmpop == 1)) 
	       {
		  if((op_list = realloc(op_list, sizeof(char)
					* (strlen(op_list) + strlen(temp_nick) + 3))) == NULL)
		    {
		       logprintf(LOG_EMERG, "Error - In get_op_list()/realloc(): %s",strerror(errno));
		       shmdt(buf);
		       sem_give(user_list_sem);
		       quit = 1;
		       return NULL;
		    }
		  sprintfa(op_list, "%s$$", temp_nick);
	}       
	}
	else
	{
	if(strcmp(temp_host, "script_process") == 0)
	{	
		  if((op_list = realloc(op_list, sizeof(char)
					* (strlen(op_list) + strlen(temp_nick) + 3))) == NULL)
		    {
		       logprintf(LOG_EMERG, "Error - In get_op_list()/realloc(): %s",strerror(errno));
		       shmdt(buf);
		       sem_give(user_list_sem);
		       quit = 1;
		       return NULL;
		    }
		  sprintfa(op_list, "%s$$", temp_nick);
	}
	}
	  }
	bufp += USER_LIST_ENT_SIZE;
     }
   
   shmdt(buf);
   sem_give(user_list_sem);
   
   /* Add two '|' at the end */
   if((op_list = realloc(op_list, sizeof(char)
			 * (strlen(op_list) + 3))) == NULL)
     {
	logprintf(LOG_EMERG, "Error - In get_op_list()/realloc(): %s",strerror(errno));
	quit = 1;
	return NULL;
     }
   
   strcat(op_list, "||");
   return op_list;
}

/* Sets the pid of the current listening process. Returns 0 if the listening
 * sockets are already taken.  */
int set_listening_pid(int newpid)
{
   char *buf, *bufp;
   int oldpid;
      
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((buf = (char *)shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {	
	logprintf(LOG_EMERG, "Error - In set_listening_pid()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return -1;
     }
   
   bufp = buf + 20;
   
   sscanf(bufp, "%d", &oldpid);
   
   if((oldpid != 0) && (oldpid != (int)getpid()))
     {
	shmdt(buf);
	sem_give(user_list_sem);
	return 0;
     }
   
   sprintf(bufp, "%d", newpid);

   shmdt(buf);
   
   sem_give(user_list_sem);
   
   return 1;
}

void send_myinfo(struct user_t *user)
{
   char *buf, *bufp;
   char temp_nick[MAX_NICK_LEN+1];
   char temp_host[MAX_HOST_LEN+1];
   int spaces, entries;
   int i;
    struct user_t *tmp_user;
   char *op_list;

   total_users = total_users - total_scripts;
   total_scripts = 0;
         
   sem_take(user_list_sem);
   
   /* Attach to the shared segment */
   if((buf = shmat(get_user_list_shm_id(), NULL, 0))
      == (char *)-1)
     {
	logprintf(LOG_EMERG, "Error - In send_nick_list()/shmat(): %s",strerror(errno));
	sem_give(user_list_sem);
	quit = 1;
	return;
     }
   
   if(sscanf(buf, "%d %d", &spaces, &entries) != 2)
     {
	logprintf(LOG_EMERG, "Error - In send_nick_list(): Couldn't get number of entries\n");
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
	  if((tmp_user = get_human_user(temp_nick)) != NULL)
	    {    
	    if((tmp_user->hide_share != 2) && (tmp_user->myinfos != NULL)
	    && (tmp_user->myinfons != NULL) && (tmp_user->myinfos != NULL) && 
	    (tmp_user != user) && (tmp_user->type != NON_LOGGED))
	    {
if((hidetags == 0) && (tmp_user->myinfos != NULL) && (tmp_user->myinfoons != NULL))
{
if((user->type == REGULAR) && ((tmp_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0) && (hide_opshare == 2))
send_to_user(tmp_user->myinfoons, user);
else
send_to_user(tmp_user->myinfos, user);
if(check_if_registered(user->nick) >= 5)
uprintf(user, "$UserIP %s %s||", tmp_user->nick, tmp_user->hostname);
}
else
{
if((tmp_user->myinfos != NULL) && (tmp_user->myinfoons != NULL) && (tmp_user->myinfons != NULL))
{
if((tmp_user->type & (OP | CHEEF | ADMIN | OWNER)) != 0)
{
if(user->type == REGULAR)
{
if(hide_opshare == 2)
send_to_user(tmp_user->myinfoons, user);
else
send_to_user(tmp_user->myinfons, user);
}
else if((user->type & (PROTECTED | REG | VIP | KVIP)) != 0)
send_to_user(tmp_user->myinfons, user);
else
send_to_user(tmp_user->myinfos, user);
}
else
{
if(hidetags == 1)
{
if((user->type & (REGULAR)) != 0)
send_to_user(tmp_user->myinfons, user);
else
send_to_user(tmp_user->myinfos, user);
}
else if(hidetags == 2)
{
if((user->type & (REGULAR | PROTECTED)) != 0)
send_to_user(tmp_user->myinfons, user);
else
send_to_user(tmp_user->myinfos, user);
}
else if(hidetags == 3)
{
if((user->type & (REGULAR | PROTECTED | REG)) != 0)
send_to_user(tmp_user->myinfons, user);
else
send_to_user(tmp_user->myinfos, user);
}
else if(hidetags == 4)
{
if((user->type & (REGULAR | PROTECTED | REG | VIP)) != 0)
send_to_user(tmp_user->myinfons, user);
else
send_to_user(tmp_user->myinfos, user);
}
else if(hidetags >= 5)
{
if((user->type & (REGULAR | PROTECTED | REG | VIP | KVIP)) != 0)
send_to_user(tmp_user->myinfons, user);
else
send_to_user(tmp_user->myinfos, user);
}
}
if(check_if_registered(user->nick) >= 5)
uprintf(user, "$UserIP %s %s||", tmp_user->nick, tmp_user->hostname);
}
}
}	    

	    }
	    else if(strcmp(temp_host, "script_process") == 0)
	    { 
	    total_scripts++;
	    uprintf(user, "$MyINFO $ALL %s [BOT] <DB V:%s,M:A,H:0/0/1,S:0>$ $$$0$|", temp_nick, VERSION); 
	    }
	    }
	bufp += USER_LIST_ENT_SIZE;
     }
   
   shmdt(buf);
   sem_give(user_list_sem);
   
   total_users = total_users + total_scripts;
   
   /* And send the oplist */
   op_list = get_op_list();
   send_to_user(op_list, user);
   free(op_list);
}

