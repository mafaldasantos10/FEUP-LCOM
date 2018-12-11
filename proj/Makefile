PROG=proj

SRCS = proj.c cromoparty.c assembly.S keyboard.c timer.c interface.c menu.c

CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
