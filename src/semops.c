/**@file semops.c
 * @brief Implementation of semaphore operations

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

#include <errno.h>
#include <string.h>
#include "semops.h"
#include "logger.h"
#include "main.h"

/* Initializes the semaphore sem to state "taken".  */
int init_sem(int *sem)
{
	union my_semun arg;

	*sem = semget(IPC_PRIVATE, 1, 0600);   
	if(*sem < 0)
	{
		logprintf(LOG_EMERG, "Error: %s",strerror(errno));
		return -1;
	}

	arg.val = 1;
	if(semctl(*sem, 0, SETVAL, arg) == -1)
	{	
		logprintf(LOG_EMERG, "Error: %s",strerror(errno));
		return -1;
	}
	return 1;
}

/* Takes a semaphore.  */
void sem_take(int sem)
{
	int ret;
	struct sembuf buf;

	memset(&buf, 0, sizeof(struct sembuf));

	buf.sem_num = 0;
	buf.sem_op = -1;
	buf.sem_flg = 0;

	/* Take the semaphore.  */
	while(((ret = semop(sem, &buf, 1)) < 0) && (errno == EINTR))
		logprintf(LOG_WARNING, "Error@semop(): %s. Retrying.",strerror(errno));

	if(ret < 0)
	{	
		logprintf(LOG_EMERG, "Error@semop(): %s",strerror(errno));
		quit = 1;
	}   
}

/* Gives a semaphore.  */
void sem_give(int sem)
{
	int ret;
	struct sembuf buf;

	memset(&buf, 0, sizeof(struct sembuf));

	buf.sem_num = 0;
	buf.sem_op = 1;
	buf.sem_flg = 0;

	/* Give the semaphore.  */
	while(((ret = semop(sem, &buf, 1)) < 0) && (errno == EINTR)) 
		logprintf(LOG_WARNING, "Error@semop(): %s. Retrying.",strerror(errno));

	if(ret < 0)
	{	
		logprintf(LOG_EMERG, "Error - In sem_give()/semop(): %s",strerror(errno));
		quit = 1;
	}   
}

