PROG=proj

SRCS = proj.c cromoparty.c keyboard.c timer.c interface.c menu.c PS2mouse.c score.c kbc_asm_ih.S

CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
