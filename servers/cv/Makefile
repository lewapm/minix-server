# Makefile for IPC server
PROG=	cv
SRCS=	main.c queue.c expectant.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys -lminlib -ltimers -lcompat_minix

MAN=

BINDIR?= /usr/sbin
FILES=cv.conf
FILESNAME=cv
FILESDIR= /etc/system.conf.d

.include <minix.service.mk>
