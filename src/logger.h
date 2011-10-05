/**@file logger.h
  @brief Header for logging functions

  This defines the needed functions for logging to stderr/file/syslog.
*/

#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <syslog.h>

#ifdef DEBUG
#define logprintf(verb, format, params...) logprint(verb, "%s[%d]@%s: " format, __FILE__, __LINE__,__FUNCTION__,##params)
#define logprintbt(format, params...) logprint(LOG_EMERG,format,##params)
#else
#define logprintf(verb, format, params...) logprint(verb, "%s(): " format, __FUNCTION__,## params)
#endif

extern int logusesyslog;
extern void logopen();
extern void logclose();
extern void logprint(int verb, const char *format, ...);

#endif /*logger.h*/
