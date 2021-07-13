#
# Makefile
#

MYLIBDIR = ../mylib
MYLIB = $(MYLIBDIR)/mylib.a
OBJS1 = server.o sessionman.o live_accept.o
OBJS2 = client.o session.o
CFLAGS = -I$(MYLIBDIR) -Wall -lpthread
TARGET1 = was
TARGET2 = wa


all: $(TARGET1) $(TARGET2)
$(TARGET1): $(OBJS1)
	$(CC) -o $@ $^ $(MYLIB) -lncurses -lpthread -Wall

$(TARGET2): $(OBJS2)
	$(CC) -o $@ $^ $(MYLIB) -lncurses -lpthread -Wall

$(OBJS1): sessionman.h commands.h Makefile
$(OBJS2): session.h commands.h Makefile

clean:
	$(RM) $(TARGET1) $(TARGET2) $(OBJS1) $(OBJS2) *~
