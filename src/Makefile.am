INCLUDES = $(perl_flags)

bin_PROGRAMS = dbhub

dbhub_SOURCES =	\
	commands.c	\
	commands.h	\
	fileio.c 	\
	fileio.h	\
	main.c		\
	main.h		\
	network.c	\
	network.h	\
	userlist.c	\
	userlist.h	\
	utils.c		\
	utils.h		\
	langvars.h	\
	langvars.c	\
	logger.c \
	logger.h \
	motd.c \
	motd.h \
	semops.c \
	semops.h \
	conf.h
if BDEBUG
dbhub_SOURCES+= stacktrace.c stacktrace.h
endif
if BPERL
dbhub_SOURCES+= perl_utils.c perl_utils.h xs_functions.c xs_functions.h
dbhub_LDADD =$(perl_ldd)
endif
if BNLS
dbhub_SOURCES+= lang.c lang.h
endif
