/**@file stacktrace.h
  @brief Header for backtrace functionality

  This defines the needed function prototype for logging a 'backtrace'.
*/

#ifdef DEBUG
#ifndef _STACKTRACE_H_
#define _STACKTRACE_H_

extern void logbacktrace(int z);

#endif /*_STACKTRACE_H_ */
#endif /* DEBUG */
