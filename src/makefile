.PHNOY : clean

CC = gcc

CFLAGS = -g -lm -Wall -I cli  -I include -I parser -I vm -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers -fgnu89-inline

TARGET = newlan

DIRS = cli include parser vm

CFILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))

OBJS = $(patsubst %.c, %.o, $(CFILES))

TARGET : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

clean :
	-$(RM) $(TARGET) $(OBJS)
